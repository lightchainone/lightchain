#include "stdafx.h"
#include "Kylin.h"
#include "EPoller.h"
#include "MemoryOp.h"
#include "CycleTimer.h"
#include <sys/ioctl.h>

_KYLIN_PACKAGE_BEGIN_

extern uint32 g_nTickPrecision;

char* 
State2Str(int nState)
{
    switch (nState) {
    case SS_VOID:
        return "VOID";
    case SS_LISTENING_0:
        return "LISTENING_0";
    case SS_LISTENING:
        return "LISTENING";
    case SS_CONNECTING_0:
        return "CONNECTING_0";
    case SS_CONNECTING:
        return "CONNECTING";
    case SS_CONNECTED_0:
        return "CONNECTED_0";
    case SS_CONNECTED:
        return "CONNECTED";
    case SS_ERROR:
        return "ERROR";
    case SS_SHUTDOWN:
        return "SHUTDOWN";
    };
    return "Unknown";
};

static void 
NotifyClient(int nErrCode, NetContext* pCtx, NetRequest* pReq)
{
    CAsyncClient* pClient = pCtx->pClient;

    pReq->async.nErrCode = nErrCode;
    pReq->async.pClient = pClient;
    

    
    if (0 != (pCtx->flag & SF_DIRECT_CALLBACK)) {
        pClient->OnCompletion((AsyncContext*)pReq);
        pClient->Release();
    }
    else {
        g_pExecMan->QueueExec((AsyncContext*)pReq, true);
    }
    
}

static void 
NotifyClient(int nErrCode, int nAction, NetContext* pCtx)
{
    CAsyncClient* pClient = pCtx->pClient;
    NetRequest* pReq = new NetRequest;

    InitAsyncContext(&pReq->async);
    pReq->async.nErrCode = nErrCode;
    pReq->async.nAction = nAction;
    pReq->async.pClient = pClient;

    
    if (0 != (pCtx->flag & SF_DIRECT_CALLBACK)) {
        pClient->OnCompletion((AsyncContext*)pReq);
        pClient->Release();
    }
    else {
        g_pExecMan->QueueExec((AsyncContext*)pReq, true);
    }
    
}

static void
DoWithError(NetContext* pCtx, int nErr=APFE_NET_ERROR)
{
    bool bConnecting;
    NetRequest* pReq;
    int state;

    if (0 != AtomicSetValueIf(pCtx->nDelayRead, -1, 0)) {
        return;
    }

    state = AtomicSetValue(pCtx->state, SS_ERROR);
    if (SS_ERROR == state)
        return;

    while (0 != pCtx->tWrite.SetIf(0, 1)) {
        thread_yield();
    }
    while (0 != pCtx->tRead.SetIf(0, 1)) {
        thread_yield();
    }

    switch (state) {
    case SS_CONNECTING:
        bConnecting = true;
        break;
    
    default:
        pReq = pCtx->pWriteReq ? pCtx->pWriteReq : pCtx->qWrite.get_front();
        bConnecting = (pReq && (AA_CONNECT == pReq->async.nAction));
    }
    
    pCtx->nEnabled = 0;
    
    if (!bConnecting) {
        g_pNetworkMan->Deassociate(pCtx);
    }
    if (NULL != pCtx->pReadReq) {
        pReq = pCtx->pReadReq;
        pCtx->pReadReq = NULL;
        NotifyClient(nErr, pCtx, pReq);
    }
    if (NULL != pCtx->pWriteReq) {
        pReq = pCtx->pWriteReq;
        pCtx->pWriteReq = NULL;
        NotifyClient(nErr, pCtx, pReq);
    }
    if (!bConnecting) {
        NotifyClient(nErr, AA_BROKEN, pCtx);
    }
}

static BufHandle*
NotifyAndNextWrite(NetContext* pCtx, int nWritten)
{
    BufHandle* pHdl = &pCtx->wHdl;
    NetRequest* pReq;

    if (0 == pHdl->nDataLen) {
        if (NULL != (pHdl=pHdl->_next)) {
            pCtx->wHdl._next = pHdl->_next;
            pCtx->wHdl.pBuf = pHdl->pBuf;
            pCtx->wHdl.nDataLen = pHdl->nDataLen;
            ASSERT(pHdl->nDataLen != 0);
            return &pCtx->wHdl;
        }

        if (NULL != pCtx->pWriteReq) {
            ASSERT(0 != nWritten);
            pReq = pCtx->pWriteReq;
            pCtx->pWriteReq = NULL;
            NotifyClient(0, pCtx, pReq);
        }
        while (NULL != (pCtx->pWriteReq=pCtx->qWrite.pop_front())) {
            pHdl = &pCtx->pWriteReq->hdl;
            while (pHdl && 0==pHdl->nDataLen) {
                pHdl = pHdl->_next;
            }
            if (pHdl) {
                pCtx->wHdl._next = pHdl->_next;
                pCtx->wHdl.pBuf = pHdl->pBuf;
                pCtx->wHdl.nDataLen = pHdl->nDataLen;
                return &pCtx->wHdl;
            }
            pReq = pCtx->pWriteReq;
            pCtx->pWriteReq = NULL;
            NotifyClient(0, pCtx, pReq);
        }
        return NULL;
    }
    pHdl->pBuf += nWritten;
    return pHdl;
}

bool CEPoller::StartWrite(NetContext* pCtx, bool bUser) 
{
    BufHandle* pHdl;
    int t, n;

    while (0 == (t=pCtx->tWrite.Add())) {
        if (SS_CONNECTED != AtomicGetValue(pCtx->state)) {
            
            
            if (1 == pCtx->tWrite.Set(0)) {
                return false;
            }
            continue;
        }
        n = 0;
        while (NULL != (pHdl=NotifyAndNextWrite(pCtx, n))) {
            ASSERT(0 != pHdl->nDataLen);
            if (0 == (pCtx->flag & SF_UDP)) {
                n = send(pCtx->s, pHdl->pBuf, pHdl->nDataLen, 0);
            }
            else {
                SOCKADDR_IN sa;
                ZeroMemory(&sa, sizeof(SOCKADDR_IN));
                sa.sin_port = htons(pCtx->pWriteReq->port);
                sa.sin_family = AF_INET;
                sa.sin_addr.s_addr = pCtx->pWriteReq->ip;
                n = sendto(pCtx->s, pHdl->pBuf, pHdl->nDataLen, 0, (LPSOCKADDR)&sa, sizeof(sa));
            }
            
            if (0 < n) {
                pHdl->nDataLen -= n;
                ASSERT(pHdl->nDataLen >= 0);
                ASSERT(NULL != pCtx->pWriteReq);
                pCtx->pWriteReq->xfered += n;
                m_nWrite += n;
                g_pExecMan->RunTimer();
            }
            else if (EAGAIN == errno) {
                pCtx->tWrite.Set(0);
                return true;
            }
            else {
                TRACE2("Error in send(%x, %p, %d): %d, %d(%s), %s\n", pCtx->s, pHdl->pBuf, 
                    pHdl->nDataLen, n, errno, strerror(errno), State2Str(pCtx->state));
                AtomicSetValueIf(pCtx->state, SS_SHUTDOWN, SS_CONNECTED);
                shutdown(pCtx->s, SHUT_RDWR);
                pCtx->tWrite.Set(0);
                return false;
            }
        }
        if (1 == (t=pCtx->tWrite.Set(0))) {
            return true;
        }
    }
    if (0 < t)
        return true;
    ASSERT(bUser);
    
    NetRequest* pReq = pCtx->qWrite.pop_back();
    if (pReq) {
        NotifyClient(APFE_AFTER_BROKEN, pCtx, pReq);
    }
    return false;
}

bool CEPoller::StartRead(NetContext* pCtx, bool bUser) 
{
    NetRequest* pReq;
    int t, n;

    while (0 == (t=pCtx->tRead.Add())) {
        if (SS_CONNECTED != AtomicGetValue(pCtx->state)) {
            
            if (1 == pCtx->tRead.Set(0)) {
                return false;
            }
            continue;
        }
        if (NULL == pCtx->pReadReq) {
            pCtx->pReadReq = pCtx->qRead.pop_front();
        }
        while (pCtx->pReadReq) {
            pReq = pCtx->pReadReq;
            ASSERT(pReq->len > pReq->xfered);
            if (0 == (pCtx->flag & SF_UDP)) {
                n = recv(pCtx->s, (char*)pReq->buf+pReq->xfered, pReq->len-pReq->xfered, 0);
            }
            else {
                SOCKADDR_IN sa;
                socklen_t len = sizeof(sa);
                n = recvfrom(pCtx->s, (char*)pReq->buf+pReq->xfered, pReq->len-pReq->xfered, 0, (LPSOCKADDR)&sa, &len);
                pReq->port = ntohs(sa.sin_port);
                pReq->ip = sa.sin_addr.s_addr;
            }
            
            
            if (0>n && EAGAIN==errno) {
                pCtx->tRead.Set(0);
                return true;
            }
            else if (0 < n) {
                m_nRead += n;
                pReq->xfered += n;
                if (pReq->xfered >= pReq->request) {
                    pCtx->pReadReq = NULL;
                    NotifyClient(0, pCtx, pReq);
                    pCtx->pReadReq = pCtx->qRead.pop_front();
                }
                g_pExecMan->RunTimer();
            }
            else {
                TRACE3("Error in recv(%x, %d): %d, %d(%s)\n", pCtx->s, pReq->len-pReq->xfered, 
                    n, errno, strerror(errno));
                AtomicSetValueIf(pCtx->state, SS_SHUTDOWN, SS_CONNECTED);
                shutdown(pCtx->s, SHUT_RDWR);
                pCtx->tRead.Set(0);
                return false;
            }
        }
        if (1 == (t=pCtx->tRead.Set(0))) {
            return SS_CONNECTED == AtomicGetValue(pCtx->state); 
        }
    }
    if (0 < t)
        return true;
    ASSERT(bUser);
    
    pReq = pCtx->qRead.pop_back();
    if (pReq) {
        NotifyClient(APFE_AFTER_BROKEN, pCtx, pReq);
    }
    return false;
}

APF_ERROR CEPoller::Create(int size)
{
    m_fd = sys_epoll_create(size);
    if (0 < m_fd)
        return APFE_OK;
    PERROR("epoll_create");
    return GetLastError();
}

void CEPoller::Close() 
{
    
    if (m_fd) {
        close(m_fd);
        m_fd = 0;
    }
}

void CEPoller::GetXfered(uint64* pnRead, uint64* pnWrite, bool bReset)
{
    if (pnRead)
        *pnRead = m_nRead;
    if (pnWrite)
        *pnWrite = m_nWrite;
    if (bReset) {
        m_nRead = 0;
        m_nWrite = 0;
    }
}

APF_ERROR CEPoller::Associate(NetContext* pCtx) 
{
    struct epoll_event ev;
    ASSERT(pCtx->state > SS_VOID);

    SetNonBlock(pCtx->s);
    ev.events = EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLET;
    ev.data.ptr = pCtx;
    
    
    pCtx->pClient->AddRef();
    
    if (0 == sys_epoll_ctl(m_fd, EPOLL_CTL_ADD, pCtx->s, &ev)) {
        return APFE_OK;
    }
    PERROR("epoll_ctl in Associate");
    pCtx->pClient->Release();
    return GetLastError();
}

APF_ERROR CEPoller::Deassociate(NetContext* pCtx) 
{
    if (pCtx->s != INVALID_SOCKET) {
        if (0 == sys_epoll_ctl(m_fd, EPOLL_CTL_DEL, pCtx->s, NULL)) {
            
            pCtx->s = INVALID_SOCKET;
            return APFE_OK;
        }
        
        
        
        return GetLastError();
    }
    return APFE_ALREADY_FREED;
}

void CEPoller::Read(NetContext* pCtx, NetRequest* pReq) 
{
    pReq->async.nAction = AA_READ;
    pCtx->pClient->AddRef();
    pCtx->qRead.push_back(pReq);
    StartRead(pCtx, true);
}

void CEPoller::Write(NetContext* pCtx, NetRequest* pReq) 
{
    pReq->async.nAction = AA_WRITE;
    pCtx->pClient->AddRef();
    pCtx->qWrite.push_back(pReq);
    StartWrite(pCtx, true);
}

bool CEPoller::DoConnect(NetContext* pCtx) 
{
    SOCKADDR_IN sa;
    ZeroMemory(&sa, sizeof(SOCKADDR_IN));
    sa.sin_port = htons(pCtx->port);
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = pCtx->ip;

    AtomicSetValue(pCtx->state, SS_CONNECTING);
    pCtx->tTimeoutCycle = 0;

    int err = connect(pCtx->s, (LPSOCKADDR)&sa, sizeof(sa));
    
    if (EINPROGRESS == errno) {
        if (INFINITE != pCtx->nTimeout) {
            Delay(pCtx, pCtx->nTimeout);
        }
        return true;
    }
    return 0 == err;
}

static void 
DoListen(NetContext* pCtx) 
{
    AtomicSetValue(pCtx->state, SS_LISTENING);
    listen(pCtx->s, pCtx->backlog);
}

void CEPoller::Delay(NetContext* pCtx, uint32 nMilliseconds)
{
    uint64 nCycle = rdtsc();
    nCycle += (GetCpuFreq() * nMilliseconds) / 1000;

    m_DListLock.Lock();
    m_DelayedList.insert(nCycle, pCtx);
    m_DListLock.Unlock();

    pCtx->tTimeoutCycle = nCycle;
}

bool CEPoller::CancelDelay(NetContext* pCtx)
{
    bool bRet = false;
    if (0 != pCtx->tTimeoutCycle) {
        m_DListLock.Lock();
        bRet = m_DelayedList.remove(pCtx->tTimeoutCycle, pCtx);
        m_DListLock.Unlock();
    }
    return bRet;
}

void CEPoller::DoWithDelayed()
{
    DelayedList::Iterator it;
    NetContext *pCtx, *p;
    int state;
    CAsyncClient* pClient;
    uint64 nCycles = rdtsc();

    while (1) {
        m_DListLock.Lock();
        it = m_DelayedList.begin();
        if (m_DelayedList.end()!=it && nCycles>=it->first) {
            p = it->second;
            m_DelayedList.erase(it);
            m_DListLock.Unlock();
        }
        else {
            m_DListLock.Unlock();
            break;
        }
        do {
            pCtx = p;
            p = dlink_pop_self(p);

            state = AtomicGetValue(pCtx->state);
            pClient = pCtx->pClient;
            AtomicSetValue(pCtx->nDelayRead, 0);
            
            switch (state) {
            case SS_CONNECTED:
                
                NotifyClient(APFE_NOTIFY, AA_READ, pCtx);
                break;

            case SS_CONNECTING_0:
                
                pCtx->nEnabled = 1;
                {
                    int i;
                    const int RETRY_T = 3;
                    for (i=0; i<RETRY_T; i++) {
                        if (DoConnect(pCtx)) {
                            break;
                        }
                    }
                    if (RETRY_T == i) {
                        DoWithError(pCtx, APFE_SYS_ERROR);
                    }
                }
                break;

            case SS_CONNECTING:
                DoWithError(pCtx, APFE_TIMEDOUT);
                break;

            default:
                pClient->Release();
                DoWithError(pCtx);
                break;
            }
        } while (p);
    }
}

APF_ERROR CEPoller::Run() 
{
    NetContext* pCtx;
    NetRequest *pReq;
#define MAX_EPOLL_EVENT     64  
    struct epoll_event events[MAX_EPOLL_EVENT];
    int nfds;
    bool bRead, bWrite, bErr;

    while (1) {
        
        nfds = sys_epoll_wait(m_fd, events, MAX_EPOLL_EVENT, g_nTickPrecision);
        
        if (nfds >= 0) {
            for (int i=0; i<nfds; i++) {
                pCtx = (NetContext*)events[i].data.ptr;
                ASSERT(pCtx && pCtx->pClient);
                if (0 == pCtx->nEnabled) {
                    continue;
                }
                bRead = (events[i].events & EPOLLIN) ? true : false;
                bWrite = (events[i].events & EPOLLOUT) ? true : false;
                bErr = (events[i].events & EPOLLERR) ? true : false;

#ifdef  _DEBUG
                if (0 != bErr) {
                    TRACE4("EPOLLERR: %x(%p), state=%s\n", pCtx->s, pCtx, State2Str(pCtx->state));
                }
                if (bRead) {
                    TRACE7("EPOLLIN: %x(%p), state=%s\n", pCtx->s, pCtx, State2Str(pCtx->state));
                }
                if (bWrite) {
                    TRACE7("EPOLLOUT: %x(%p), state=%s\n", pCtx->s, pCtx, State2Str(pCtx->state));
                }
#endif
                switch (AtomicGetValue(pCtx->state)) {
                case SS_CONNECTED_0:            
                    AtomicSetValue(pCtx->state, SS_CONNECTED);
                    NotifyClient(APFE_NOTIFY, AA_READ, pCtx);
                    
                case SS_CONNECTED:
                    if (bRead) {
                        while (pCtx->tRead.Get() > 0) {
                            thread_yield();
                        }
                        if (!StartRead(pCtx, false)) {
                            DoWithError(pCtx);
                            break;
                        }
                    }

                    if (!bErr) {
                        if (bWrite) {
                            while (pCtx->tWrite.Get() > 0) {
                                thread_yield();
                            }
                            if (!StartWrite(pCtx, false)) {
                                DoWithError(pCtx);
                                break;
                            }
                        }
                    }
                    else {
                        int bytes = 0;
                        if (0 == (pCtx->flag & SF_DONT_EXHAUST)) {
                            ioctl(pCtx->s, FIONREAD, &bytes);
                        }
                        if (0 == bytes) {
                            AtomicSetValueIf(pCtx->state, SS_SHUTDOWN, SS_CONNECTED);
                            DoWithError(pCtx);
                        }
                    }
                    break;

                case SS_CONNECTING_0:
                    if (DoConnect(pCtx))
                        break;
                    bErr = true;
    
                case SS_CONNECTING:
                    CancelDelay(pCtx);
                    if (!bErr && bWrite) {
                        pReq = pCtx->pWriteReq;
                        pCtx->pWriteReq = NULL;
                        ASSERT(pReq != pCtx->qWrite.get_front());
                       
                        if (0==pReq->request && NULL==pReq->pHdl) {
                            AtomicSetValue(pCtx->state, SS_CONNECTED);
                            NotifyClient(0, pCtx, pReq);
                        }
                        else {
                            pCtx->qWrite.push_front(pReq);
                            AtomicSetValue(pCtx->state, SS_CONNECTED);
                        }
                        if (!StartWrite(pCtx, false)) {
                            DoWithError(pCtx);
                        }
                    }
                    else if (bErr) {
                        DoWithError(pCtx);
                    }
                    break;

                case SS_LISTENING_0:
                    DoListen(pCtx);
                    break;

                case SS_LISTENING:
                    
                    if (!bErr && bRead) {
                        pCtx->pClient->AddRef();
                        NotifyClient(APFE_OK, AA_LISTEN, pCtx);
                    }
                    break;

                case SS_SHUTDOWN:
                    DoWithError(pCtx);
                    break;

                default:
                    break;
                }
            }

            DoWithDelayed();
            g_pExecMan->RunTimer();
        }
        else if (EINTR != GetLastError()) {
            if (IsKylinRunning())
                PERROR("Error in epoll_wait");
            return GetLastError();
        }
    }
    return APFE_OK;
}

_KYLIN_PACKAGE_END_
