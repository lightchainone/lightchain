#define _XOPEN_SOURCE 500
#include "stdafx.h"
#include "Kylin.h"
#include "SyncObj.h"
#include "thread.h"
#include "CycleTimer.h"

uint64 g_nDiskStats[MAX_NR_DISKS * 2];
uint64 g_nDiskCosts[MAX_NR_DISKS * 2];

#define NotifyClient(err, req)    {                                     \
        req->async.nErrCode = err;                                      \
        g_pExecMan->QueueExec((AsyncContext*)req, true);                \
    }

#define DISK_OP(op, j, rw)                                              \
    DiskRequest* pReq = CONTAINING_RECORD(j, DiskRequest, job);         \
    DiskContext* pCtx = pReq->pCtx;                                     \
    UNLIKELY_IF (-1 == pCtx->fd) {                                      \
        pCtx->fd = open(pCtx->pPath, pCtx->nFlag, 0644);                \
        UNLIKELY_IF (-1 == pCtx->fd) {                                  \
            NotifyClient(errno, pReq);                                  \
            return;                                                     \
        }                                                               \
    }                                                                   \
    uint64 cost = rdtsc();                                              \
    int len;                                                            \
    if (pReq->off != pCtx->nCurOff) {                                   \
        len = p ## op(pCtx->fd, pReq->buf, pReq->request, pReq->off);   \
        pCtx->nCurOff = pReq->off;                                      \
    }                                                                   \
    else {                                                              \
        len = op(pCtx->fd, pReq->buf, pReq->request);                   \
    }                                                                   \
    if (len > 0) {                                                      \
        cost = rdtsc() - cost;                                          \
        int which = (pCtx->diskno<<1) + rw;                             \
        g_nDiskStats[which] += len;                                     \
        g_nDiskCosts[which] += cost;                                    \
        pCtx->nCurOff += len;                                           \
        pReq->off += len;                                               \
        pReq->xfered = len;                                             \
        NotifyClient(0, pReq);                                          \
    }                                                                   \
    else {                                                              \
        NotifyClient(errno, pReq);                                      \
    }

static void
ReadOp(Job* pJob)
{
    DISK_OP(read, pJob, 0);
}

static void
WriteOp(Job* pJob)
{
    DISK_OP(write, pJob, 1);
}

IMPLEMENT_SINGLETON_WITHOUT_CTOR(CDiskMan)
CDiskMan::CDiskMan() : m_ThreadPool(TT_DISK, MAX_NR_DISKS, false)        
{
    m_bStarted = false;
    memset(g_nDiskStats, 0, MAX_NR_DISKS * 2 * sizeof(uint64));
    memset(g_nDiskCosts, 0, MAX_NR_DISKS * 2 * sizeof(uint64));
}

CDiskMan::~CDiskMan() 
{
    Stop();
}

APF_ERROR CDiskMan::Start(int nDisks, THREAD_INIT_PROC fInit)
{
    ASSERT(nDisks <= MAX_NR_DISKS);
    ASSERT(!IsStarted());
    
    if (m_ThreadPool.Start(nDisks, fInit) > 0) {
        m_bStarted = true;
        return APFE_OK;
    }
    return APFE_SYS_ERROR;
}

void CDiskMan::Stop()
{
    if (m_bStarted) {
        m_bStarted = false;
        m_ThreadPool.Stop();
    }
}

APF_ERROR CDiskMan::Associate(int diskno, int fd, 
                              CAsyncClient* pClient, DiskContext* pContext)
{
    pContext->fd = fd;
    pContext->diskno = diskno;
    pContext->pClient = pClient;
    pContext->nCurOff = pContext->nRead = pContext->nWrite = 0;
    pContext->pPath = NULL;
    pContext->nFlag = 0;
    pClient->AddRef();
    return APFE_OK;
}

APF_ERROR CDiskMan::Associate(int diskno, char* pPath, int nFlag, 
                              CAsyncClient* pClient, DiskContext* pContext)
{
    pContext->fd = -1;
    pContext->diskno = diskno;
    pContext->pClient = pClient;
    pContext->nCurOff = pContext->nRead = pContext->nWrite = 0;
    pContext->pPath = pPath;
    pContext->nFlag = nFlag;
    pClient->AddRef();
    return APFE_OK;
}

APF_ERROR CDiskMan::Deassociate(DiskContext* pContext)
{
    if (pContext->pPath && pContext->fd!=-1) {
        close(pContext->fd);
    }
    pContext->pClient->Release();
    return APFE_OK;
}

#define QUEUE_TASK(pContext, pReq, f, pBuf, count)                  \
    pContext->pClient->AddRef();                                    \
    pReq->async.pClient = pContext->pClient;                        \
    pReq->job.fProc = f;                                            \
    pReq->buf = pBuf;                                               \
    pReq->request = count;                                          \
    pReq->xfered = 0;                                               \
    pReq->pCtx = pContext;                                          \
    m_ThreadPool.QueueJob(&pReq->job, pContext->diskno)


void CDiskMan::Read(DiskContext* pContext, void* pBuf, int count, DiskRequest* pReq)
{
    ASSERT(IsStarted()); 
    ASSERT(NULL != pReq);
    ASSERT(NULL != pContext);
    ASSERT(-1!=pContext->fd || pContext->pPath);

    pReq->async.nAction = AA_READ;
    QUEUE_TASK(pContext, pReq, ReadOp, pBuf, count);
}

void CDiskMan::Write(DiskContext* pContext, void* pBuf, int count, DiskRequest* pReq)
{
    ASSERT(IsStarted()); 
    ASSERT(NULL != pReq);
    ASSERT(NULL != pContext);
    ASSERT(-1!=pContext->fd || pContext->pPath);
    
    pReq->async.nAction = AA_WRITE;
    QUEUE_TASK(pContext, pReq, WriteOp, pBuf, count);
}

void CDiskMan::GetXfered(uint64* pnRead, uint64* pnWrite)
{
    uint64 nRead=0, nWrite=0;

    if (IsStarted()) {
        int n = m_ThreadPool.GetWorkerCount();
        for (int i=0; i<n; i++) {
            nRead += g_nDiskStats[i*2];
            nWrite += g_nDiskStats[i*2 + 1];
        }
    }
    if (pnRead)
        *pnRead = nRead;
    if (pnWrite)
        *pnWrite = nWrite;
}

void CDiskMan::PrintState() 
{
    static uint64 nDiskStats[MAX_NR_DISKS * 2];
    static uint64 nDiskCosts[MAX_NR_DISKS * 2];
    uint64 nStat, tCost, n;
    dolcle f[2], fTotal[2];
    uint64 freq = GetCpuFreq();

    if (IsStarted()) {
        int m = m_ThreadPool.GetWorkerCount();
        fTotal[0] = fTotal[1] = 0;
        
        TRACE0("DISK_BW    IN        OUT\n");
        for (int i=0; i<m*2; i++) {
            n = g_nDiskStats[i];
            nStat = n - nDiskStats[i]; nDiskStats[i] = n;

            n = g_nDiskCosts[i];
            tCost = n - nDiskCosts[i]; nDiskCosts[i] = n;
            int x = (i % 2);
            if (tCost) {
                f[x] = (((dolcle)nStat/1048576) * freq) / tCost;
                fTotal[x] += f[x];
            }
            else {
                f[x] = 0;
            }
            
            if (1 == x) {
                TRACE0("%4d: %9.2f, %9.2f MB/s\n", i/2, f[0], f[1]);
            }
        }
        TRACE0("TOTL: %9.2f, %9.2f MB/s\n", fTotal[0], fTotal[1]);
    }
    
}
