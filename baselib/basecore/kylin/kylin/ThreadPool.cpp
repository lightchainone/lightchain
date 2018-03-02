#include "stdafx.h"
#include "Trace.h"
#include "MemoryOp.h"
#include "Kylin.h"

_KYLIN_PACKAGE_BEGIN_

static void* 
WorkerProc(void* pData) 
{
    WorkerContext* pCtx = (WorkerContext*)pData;    
    JobQ* pJobQ = pCtx->pJobQ;
    Job* pJob;
    
    TRACE4("%s worker#%d started...\n", ThreadType2Str(pCtx->type), pCtx->id);
    pCtx->thread_id = thread_getid();
    if (pCtx->fInit) {
        pCtx->fInit(pCtx->type, pCtx->id);
    }
    while (1) {
        pJob = pJobQ->pop_front();
        ASSERT_NOT_EQUAL((Job*)NULL, pJob);
        if (pJob->fProc != 0) {
            pCtx->bDoing = true;
            pJob->fProc(pJob);
            pCtx->bDoing = false;
        }
        else {
            ControlJob* pCtl = (ControlJob*)pJob;
            if (!pCtl->fProc(pCtl, pCtx)) {
                break;
            }
        }
    }
    TRACE4("%s worker#%d stopped.\n", ThreadType2Str(pCtx->type), pCtx->id);
    return NULL;
}

CThreadPool::CThreadPool(int type, int nMaxWorkers, bool bShareQ) 
{
    m_nWorkers = 0;
    m_nMaxWorkers = nMaxWorkers;
    m_nJobs = 0;
    m_nType = type;
    
    m_hWorkerThreads = (thread_t*)ZeroAlloc(m_nMaxWorkers*sizeof(thread_t));
    m_pContexts =(WorkerContext*)ZeroAlloc(m_nMaxWorkers*sizeof(WorkerContext));
    for (int i=0; i<nMaxWorkers; i++) {
        m_pContexts[i].id = i;
        m_pContexts[i].type = type;
        m_pContexts[i].nJobs = 0;
    }
    if (bShareQ) {
        m_pContexts[0].pJobQ = new JobQ;
        for (int i=1; i<nMaxWorkers; i++) {
            m_pContexts[i].pJobQ = m_pContexts[0].pJobQ;
        }
    }
    m_bShareQ = bShareQ;
}

CThreadPool::~CThreadPool() 
{ 
    Stop(); 
    
    free(m_hWorkerThreads);
    if (m_bShareQ) {
        delete m_pContexts[0].pJobQ;
    }
    else {
        for (int i=0; i<m_nMaxWorkers; i++) {
            if (m_pContexts[i].pJobQ)
                delete m_pContexts[i].pJobQ;
        }
    }
    free(m_pContexts);
}

int CThreadPool::_AddWorker(int nAdd) 
{
    int i;
    
    for (i=0; i<nAdd && m_nWorkers<m_nMaxWorkers; i++) {
        m_pContexts[m_nWorkers].fInit = m_fInit;
        if (m_pContexts[m_nWorkers].pJobQ == NULL) {
            m_pContexts[m_nWorkers].pJobQ = new JobQ;
        }
        if (0 != thread_create(&m_hWorkerThreads[m_nWorkers], NULL, WorkerProc, &m_pContexts[m_nWorkers])) {
            PERROR("thread_create");
            break;
        }
        m_nWorkers ++;
    }
    return i;
}

struct TerminationJob : ControlJob {
    int id;
};

static bool
TerminateWorker(ControlJob* pCtl, WorkerContext* pCtx)
{
    TerminationJob* pT = (TerminationJob*)pCtl;
    if (pT->id!=-1 && pT->id!=pCtx->id) {
        pCtx->pJobQ->push_back((Job*)pCtl); 
        thread_yield();                     
        return true;
    }
    return false;
}

int CThreadPool::_DelWorker(int nDel, bool bFinal) 
{
    TerminationJob *pTerminations = new TerminationJob[nDel];
    int i;
    
    TRACE4("%s start terminating %d workers...\n", ThreadType2Str(m_nType), nDel);
    for (i=0; i<nDel && m_nWorkers>0; i++) {
        m_nWorkers --;
        DLINK_INITIALIZE(&pTerminations[m_nWorkers].link);
        pTerminations[m_nWorkers].fZero = 0;
        pTerminations[m_nWorkers].fProc = TerminateWorker;
        pTerminations[m_nWorkers].id = bFinal ? -1 : m_nWorkers;
        m_pContexts[m_nWorkers].pJobQ->push_back((Job*)&pTerminations[m_nWorkers]);
    }
    for (int j=m_nWorkers; j<i+m_nWorkers; j++) {
        TRACE4("%s wait for worker #%d.\n", ThreadType2Str(m_nType), j);
        thread_join(m_hWorkerThreads[j], NULL);
    }
    TRACE4("%s end terminating workers.\n", ThreadType2Str(m_nType));
    delete[] pTerminations;
    return i;
}

void CThreadPool::PrintState()
{
    TRACE0("%s total %d jobs\n", ThreadType2Str(m_nType), AtomicGetValue(m_nJobs));
    for (int i=0; i<m_nWorkers; i++) {
        TRACE0("%s #%d queue: %d:%d jobs%s\n", ThreadType2Str(m_nType), i, 
            m_pContexts[i].pJobQ->size(), AtomicGetValue(m_pContexts[i].nJobs), m_pContexts[i].bDoing?", working":"");
        m_pContexts[i].pJobQ->PrintState();
    }
}

static void
DoNothing(Job* pJob)
{
    free(pJob);
}

bool CThreadPool::CancelJob(Job* pJob, int nWhich) 
{
    VERIFY_OR_RETURN(nWhich >= -1, false);
    ASSERT_EQUAL(thread_getid(), GetWorkThreadId(nWhich))

    Job* p = ZeroAlloc<Job>();
    p->fProc = DoNothing;
    if (m_pContexts[nWhich].pJobQ->replace(pJob, p)) {
        return true;
    }
    free(p);
    return false;
}

_KYLIN_PACKAGE_END_
