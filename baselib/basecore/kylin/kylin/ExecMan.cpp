#include "stdafx.h"
#include "ExecMan.h"
#include "CompQ.h"
#include "CycleTimer.h"

_KYLIN_PACKAGE_BEGIN_

typedef TMultiMap<uint32, AsyncContext> TimerMap;
static TimerMap s_TimerMap;
static CMutex s_Lock;

uint32 g_nTickPrecision = 0;
uint32 g_nLastTick = 0;
uint64 g_nCycleStart = 0;
uint64 g_nCyclesInTick = 0;

static void
Proc(Job* pJob)
{
	AsyncContext* pCtx = (AsyncContext*)pJob;
	CAsyncClient* pClient = pCtx->pClient;
	
    pCtx->fProc = NULL;
	pClient->OnCompletion(pCtx);
	pClient->Release();
}

IMPLEMENT_SINGLETON_WITHOUT_CTOR(CExecMan)
CExecMan::CExecMan() : m_ThreadPool(TT_EXEC, MAX_NR_EXEC_WORKERS, false)	
{
	m_nCurJobs = LARGE_ENOUGH_NEGATIVE;
}

CExecMan::~CExecMan() 
{
	Stop(true);
}

APF_ERROR CExecMan::QueueExec(AsyncContext* pCtx, bool bClientReferred)
{
    VERIFY_OR_RETURN(NULL != pCtx, APFE_INVALID_ARGS);
    VERIFY_OR_RETURN(NULL != pCtx->pClient, APFE_INVALID_ARGS);
	
	if (atomic_add(&m_nCurJobs, 1) >= 0) {
		
        if (!bClientReferred) {
		    pCtx->pClient->AddRef();
        }
        pCtx->fProc = Proc;
		m_ThreadPool.QueueJob((Job*)pCtx, pCtx->pClient->GetAsyncId() % m_ThreadPool.GetWorkerCount());
		atomic_add(&m_nCurJobs, -1);
		return APFE_OK;
	}
    if (bClientReferred) {
        pCtx->pClient->Release();
    }
    AtomicSetValue(m_nCurJobs, LARGE_ENOUGH_NEGATIVE);
    if (IsKylinRunning()) {
		TRACE0("Fatal error: Exec workers are not started\n");
    }
	return APFE_NO_WORKER;
}

APF_ERROR CExecMan::QueueExecEmergent(AsyncContext* pCtx, bool bClientReferred)
{
    VERIFY_OR_RETURN(NULL != pCtx, APFE_INVALID_ARGS);
    VERIFY_OR_RETURN(NULL != pCtx->pClient, APFE_INVALID_ARGS);

    if (atomic_add(&m_nCurJobs, 1) >= 0) {
        
        if (!bClientReferred) {
            pCtx->pClient->AddRef();
        }
        pCtx->fProc = Proc;
        m_ThreadPool.QueueEmergentJob((Job*)pCtx, pCtx->pClient->GetAsyncId() % m_ThreadPool.GetWorkerCount());
        atomic_add(&m_nCurJobs, -1);
        return APFE_OK;
    }
    if (bClientReferred) {
        pCtx->pClient->Release();
    }
    AtomicSetValue(m_nCurJobs, LARGE_ENOUGH_NEGATIVE);
    if (IsKylinRunning()) {
        TRACE0("Fatal error: Exec workers are not started\n");
    }
    return APFE_NO_WORKER;
}

static void
ProxyProc(Job* pJob)
{
    ProxyExecCtx* pCtx = (ProxyExecCtx*)pJob;
    CAsyncClient* pClient = pCtx->pClient;
    int nErrCode = pCtx->proc(pCtx);

    if (APFE_NO_RETURN != nErrCode) {
        VERIFY_OR_RETURN(NULL != pClient, BLANK);
        pCtx->nErrCode = nErrCode;
        g_pExecMan->QueueExec(pCtx, true);
    }
    else if (NULL != pClient) {
        pClient->Release();
    }
}

APF_ERROR CExecMan::ProxyExec(int nAckCode, CAsyncClient* pClient, PROXY_EXEC_PROC fProc, ProxyExecCtx* pCtx)
{
    static volatile int s_nThisWorker = 0;
    VERIFY_OR_RETURN(NULL != pCtx, APFE_INVALID_ARGS);

    pCtx->nAction = nAckCode;
    pCtx->pClient = pClient;
    pCtx->proc = fProc;
    
    if (atomic_add(&m_nCurJobs, 1) >= 0) {
        pCtx->fProc = ProxyProc;
        if (pClient) {
            pClient->AddRef();
        }

        uint32 n = (uint32)atomic_add(&s_nThisWorker, 1);
        m_ThreadPool.QueueJob((Job*)pCtx, n % m_ThreadPool.GetWorkerCount());
        atomic_add(&m_nCurJobs, -1);
        return APFE_OK;
    }
    AtomicSetValue(m_nCurJobs, LARGE_ENOUGH_NEGATIVE);
    if (IsKylinRunning()) {
        TRACE0("Fatal error: Exec workers are not started\n");
    }
    return APFE_NO_WORKER; 
}

APF_ERROR CExecMan::Start(int nWorkers, THREAD_INIT_PROC fInit, uint32 nTimerPrecision)
{
    g_nCycleStart = rdtsc();
    g_nLastTick = 0;
    g_nTickPrecision = (nTimerPrecision >= 1000) ? 1000 : nTimerPrecision;
    g_nCyclesInTick = GetCpuFreq() / (1000 / g_nTickPrecision);
	
	if (0 < m_ThreadPool.Start(nWorkers, fInit)) {
		AtomicSetValue(m_nCurJobs, 0);
		return APFE_OK;
	}
	return APFE_SYS_ERROR;
}

void CExecMan::Stop(bool bWait)
{
    if (bWait) {
        
        for (int i=0; i<m_ThreadPool.GetWorkerCount(); i++) {
            VERIFY_OR_RETURN(thread_getid()!=m_ThreadPool.GetWorkThreadId(i), BLANK);
        }

	    int n;
	    while (0 != (n=atomic_comp_swap(&m_nCurJobs, LARGE_ENOUGH_NEGATIVE, 0))) {
            if (LARGE_ENOUGH_NEGATIVE == n) {
			    return;
            }
		    Sleep(1);
	    }
	    m_ThreadPool.Stop();
    }
    else {
	    AtomicSetValue(m_nCurJobs, LARGE_ENOUGH_NEGATIVE);
    }
}

void CExecMan::RunTimer()
{
    static CToken token;
    TimerMap::Iterator it;
    AsyncContext *p, *pp;

    UNLIKELY_IF (!token.TryAcquire(1)) {
        return;
    }
    uint32 nTick = (rdtsc() - g_nCycleStart) / g_nCyclesInTick;
    if (nTick == g_nLastTick) {
        token.Release(1);
        return;
    }

    g_nLastTick = nTick;
    while (1) {
        s_Lock.Lock();
        it = s_TimerMap.begin();
        if (s_TimerMap.end()!=it && nTick>=it->first) {
            p = it->second;
            s_TimerMap.erase(it);
        }
        else {
            s_Lock.Unlock();
            break;
        }
        
        do {
            pp = p;
            p = dlink_pop_self(p);
            g_pExecMan->QueueExecEmergent((AsyncContext*)pp, true);
        } while (p);
        s_Lock.Unlock();
    }

    token.Release(1);
}

APF_ERROR CExecMan::DelayExec(int nAction, CAsyncClient* pClient, uint32 nMilliseconds, AsyncContext* pCtx)
{
    VERIFY_OR_RETURN(NULL != pClient, APFE_INVALID_ARGS);
    VERIFY_OR_RETURN(NULL != pCtx, APFE_INVALID_ARGS);

    pCtx->nAction = nAction;
    pCtx->pClient = pClient;
    pCtx->fProc = (JOB_PROC)2;
    
    if (g_nTickPrecision <= nMilliseconds) {
        pClient->AddRef();

        s_Lock.Lock();
        
        nMilliseconds = g_nLastTick + nMilliseconds / g_nTickPrecision;
        pCtx->nErrCode = nMilliseconds;
        s_TimerMap.insert(nMilliseconds, pCtx);
        s_Lock.Unlock();
        return APFE_OK;
    }

    APF_ERROR err;
    s_Lock.Lock();
    err = QueueExec(pCtx, false);
    s_Lock.Unlock();
    return err;
}

APF_ERROR CExecMan::CancelExec(AsyncContext* pCtx)
{
    TLocalLock<CMutex> ll(s_Lock);
    bool bCanceled = false;

    if ((JOB_PROC)2 == pCtx->fProc) {
        bCanceled = s_TimerMap.remove(pCtx->nErrCode, pCtx);

        if (bCanceled) {
       	    pCtx->fProc = NULL;
            pCtx->pClient->Release();
            return APFE_OK;
        }
    }
    if (Proc == pCtx->fProc) {
        int n = pCtx->pClient->GetAsyncId() % m_ThreadPool.GetWorkerCount();
        if (thread_getid() == m_ThreadPool.GetWorkThreadId(n)) {
            VERIFY_OR_ABORT(m_ThreadPool.CancelJob(pCtx, n));
            pCtx->fProc = NULL;
            pCtx->pClient->Release();
            return APFE_OK;
#if 0
            
            for (int i=0; i<1000; i++) {
                if (!DLINK_IS_STANDALONE(&pCtx->link)) {   
                    VERIFY_OR_ABORT(m_ThreadPool.CancelJob(pCtx, n));
                    ASSERT(DLINK_IS_STANDALONE((DLINK*)pCtx));

                    pCtx->fProc = NULL;
                    pCtx->pClient->Release();
                    return APFE_OK;
                }
                thread_yield();
            }
            return APFE_INVALID_ARGS;
#endif
        }
        return APFE_ALREADY_EXECED; 
    }
    return APFE_INVALID_ARGS;
}

APF_ERROR QueueExec(APF_ERROR nErrCode, int nAction, CAsyncClient* pClient, AsyncContext* pCtx, bool bClientReferred)
{
    VERIFY_OR_RETURN(NULL != pClient, APFE_INVALID_ARGS);
    VERIFY_OR_RETURN(NULL != pCtx, APFE_INVALID_ARGS);

    pCtx->nErrCode = nErrCode;
    pCtx->nAction = nAction;
    pCtx->pClient = pClient;

    return g_pExecMan->QueueExec(pCtx, bClientReferred);
}

_KYLIN_PACKAGE_END_
