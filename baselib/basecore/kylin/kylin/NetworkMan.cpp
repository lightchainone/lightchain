#include "stdafx.h"
#include "Trace.h"
#include "Kylin.h"

_KYLIN_PACKAGE_BEGIN_

IMPLEMENT_SINGLETON_WITHOUT_CTOR(CNetworkMan)
CNetworkMan::CNetworkMan() : m_ThreadPool(TT_NETWORK, MAX_NR_NETWORK_WORKERS, true)      
{
    m_bStarted = 0;
}

CNetworkMan::~CNetworkMan() 
{
    Stop();
}

struct MyJob {
    Job job;
    volatile int count;
    CEPoller poller;
};
static MyJob s_Jobs[MAX_NR_NETWORK_WORKERS];

#define GetPollerClientNum(i)   (AtomicGetValue(s_Jobs[i].count))

static void
Polling(Job* pJob)
{
    MyJob* pMyJob = CONTAINING_RECORD(pJob, MyJob, job);
    pMyJob->poller.Run();
}

static bool 
PrintContext(NetContext* pCtx, ptr)
{
    char* State2Str(int nState);
    if (SS_CONNECTED == pCtx->state)
    TRACE0("%s:%d (%s,%d): Pending out=#%d%s, in=%d\n", 
        in_ntoa(pCtx->ip), pCtx->port,
        State2Str(pCtx->state), pCtx->pClient->GetRef(),
        pCtx->qWrite.size(),
        pCtx->pWriteReq ? "*" : "", 
        
        0
    );
    return true;
}

void CNetworkMan::PrintState()
{
    
    
    
    m_NetContexts.Enumerate(PrintContext, NULL);
}

void CNetworkMan::GetXfered(uint64* pnRead, uint64* pnWrite)
{
    uint64 nRead=0, nWrite=0, nRead1, nWrite1;

    if (IsStarted()) {
        int n = m_ThreadPool.GetWorkerCount();
        for (int i=0; i<n; i++) {
            s_Jobs[i].poller.GetXfered(&nRead1, &nWrite1, false);
            nRead += nRead1;
            nWrite += nWrite1;
        }
    }
    if (pnRead)
        *pnRead = nRead;
    if (pnWrite)
        *pnWrite = nWrite;
}

APF_ERROR CNetworkMan::Start(int nWorkers, THREAD_INIT_PROC fInit)
{
    ASSERT(!IsStarted());

    int n = m_ThreadPool.Start(nWorkers, fInit);
    if (n > 0) {
        for (int i=0; i<n; i++) {
            s_Jobs[i].poller.Create(256);
            s_Jobs[i].job.fProc = Polling;
            s_Jobs[i].count = 0;
            DLINK_INITIALIZE(&s_Jobs[i].job.link);
            m_ThreadPool.QueueJob(&s_Jobs[i].job, i);
        }
        AtomicSetValue(m_bStarted, 1);
        return APFE_OK;
    }
    return APFE_SYS_ERROR;
}

void CNetworkMan::Stop()
{
    if (1 == atomic_comp_swap(&m_bStarted, -1, 1)) {
        
        int n = m_ThreadPool.GetWorkerCount();
        for (int i=0; i<n; i++) {
            s_Jobs[i].poller.Close();
        }
        
    }
}

void CNetworkMan::Enumerate(NET_CTX_ENUMERATOR f, ptr p)
{
    m_NetContexts.Enumerate(f, p);
}

void InitNetContext(NetContext* pCtx, SOCKET s, CAsyncClient* pClient, uint32 flag)
{
    pCtx->s = s;
    pCtx->pClient = pClient;
    pCtx->tRead.Init();
    pCtx->tWrite.Init();
    pCtx->qRead.Init();
    pCtx->qWrite.Init();
    pCtx->pReadReq = pCtx->pWriteReq = NULL;
    pCtx->wHdl._next = NULL;
    pCtx->wHdl.nDataLen = 0;
    pCtx->flag = flag;
    pCtx->state = SS_VOID;
    pCtx->pPoller = NULL;
    pCtx->nDelayRead = 0;
    pCtx->nEnabled = 1;
    pCtx->nTimeout = INFINITE;
    pCtx->tTimeoutCycle = 0;
    DLINK_INITIALIZE(&pCtx->link);
}

#ifdef _DEBUG

#define SANITY_CHECK_BEGIN                  \
    ASSERT(0 != AtomicGetValue(m_bStarted));\
    ASSERT(NULL != pCtx);                   \
    ASSERT(0 != pCtx->s);                   \
    ASSERT(NULL != pCtx->pClient);
#define SANITY_CHECK_END

#else

#define SANITY_CHECK_BEGIN                  \
    if (IsStarted()) {
#define SANITY_CHECK_END                    }

#endif


APF_ERROR CNetworkMan::Associate(NetContext* pCtx)
{
    APF_ERROR err = APFE_NOT_CREATED;
    SANITY_CHECK_BEGIN
    ASSERT(pCtx->state > SS_VOID);
    
    
    int x1=AtomicGetValue(s_Jobs[0].count), m=0, n;
    for (int i=1; i<m_ThreadPool.GetWorkerCount(); i++) {
        n = AtomicGetValue(s_Jobs[i].count);
        if (n < x1) {
            x1 = n;
            m = i;
        }
    }
    
    pCtx->pPoller = &s_Jobs[m].poller;
    atomic_add(&s_Jobs[m].count, 1);

    m_NetContexts.push_back(pCtx);
    if (APFE_OK != (err=pCtx->pPoller->Associate(pCtx))) {
        m_NetContexts.remove(pCtx);
    }
    SANITY_CHECK_END
    return err;
}

APF_ERROR CNetworkMan::Deassociate(NetContext* pCtx)
{
    APF_ERROR err;
    MyJob* pJob;

    if (NULL != pCtx->pPoller) {
        if (APFE_OK == (err=pCtx->pPoller->Deassociate(pCtx))) {
            m_NetContexts.remove(pCtx);
            pJob = CONTAINING_RECORD(pCtx->pPoller, MyJob, poller);
            atomic_add(&pJob->count, -1);
        }
        return err;
    }
    m_NetContexts.remove(pCtx);
    return APFE_OK;
}

void CNetworkMan::Listen(NetContext* pCtx, int backlog)
{
    SANITY_CHECK_BEGIN

    pCtx->backlog = backlog;
    
    pCtx->pClient->AddRef();
    pCtx->state = SS_LISTENING_0;
    Associate(pCtx);
    
    SANITY_CHECK_END
}

void CNetworkMan::Connect(NetContext* pCtx, uint32 ip, uint16 port, 
                            void *pBuf, int count, uint32 timeout, NetRequest* pReq)
{
    SANITY_CHECK_BEGIN
    ASSERT(NULL != pReq);

    pCtx->ip = ip;
    pCtx->port = port;

    pReq->async.nAction = AA_CONNECT;
    pReq->pHdl = NULL;
    pReq->buf = pBuf;
    pReq->request = count;
    pReq->xfered = 0;
    pCtx->pWriteReq = pReq;
    pCtx->nTimeout = timeout;

    pCtx->pClient->AddRef();
    pCtx->state = SS_CONNECTING_0;
    Associate(pCtx);
    
    SANITY_CHECK_END
}

void CNetworkMan::Monitor(NetContext* pCtx)
{
    SANITY_CHECK_BEGIN

    pCtx->pClient->AddRef();
    pCtx->state = SS_CONNECTED_0;
    Associate(pCtx);
    
    SANITY_CHECK_END
}

bool CNetworkMan::Delay(NetContext* pCtx, uint32 nMilliseconds)
{
    bool bRet = false;
    SANITY_CHECK_BEGIN
    int bDelay = AtomicSetValue(pCtx->nDelayRead, 1);
    if (0 == bDelay) {
        pCtx->pClient->AddRef();
        pCtx->pPoller->Delay(pCtx, nMilliseconds);
        bRet = true;
    }
    SANITY_CHECK_END
    return bRet;
}

void CNetworkMan::Read(NetContext* pCtx, void* pBuf, int len, int request, NetRequest* pReq)
{
    SANITY_CHECK_BEGIN
    ASSERT(NULL != pReq);
    ASSERT(len != 0);
    ASSERT(request <= len);
    ASSERT(pCtx->state > SS_VOID);

    pReq->pHdl = NULL;
    pReq->buf = pBuf;
    pReq->request = request;
    pReq->len = len;
    pReq->xfered = 0;
    pCtx->pPoller->Read(pCtx, pReq);
    SANITY_CHECK_END
}

void CNetworkMan::Read(NetContext* pCtx, NetRequest* pReq)
{
    SANITY_CHECK_BEGIN
    ASSERT(NULL != pReq);
    ASSERT(pReq->len != 0);
    ASSERT(pReq->request <= pReq->len);
    ASSERT(pCtx->state > SS_VOID);

    pReq->xfered = 0;
    pCtx->pPoller->Read(pCtx, pReq);
    SANITY_CHECK_END
}

void CNetworkMan::Write(NetContext* pCtx, void* pBuf, int count, NetRequest* pReq)
{
    SANITY_CHECK_BEGIN
    ASSERT(NULL != pReq);
    ASSERT(pCtx->state > SS_VOID);

    pReq->pHdl = NULL;
    pReq->buf = pBuf;
    pReq->request = count;
    pReq->xfered = 0;
    pCtx->pPoller->Write(pCtx, pReq);
    SANITY_CHECK_END
}

void CNetworkMan::Write(NetContext* pCtx, NetRequest* pReq)
{
    SANITY_CHECK_BEGIN
    ASSERT(NULL != pReq);
    ASSERT(pCtx->state > SS_VOID);
    
    pReq->xfered = 0;
    pCtx->pPoller->Write(pCtx, pReq);
    SANITY_CHECK_END
}

_KYLIN_PACKAGE_END_
