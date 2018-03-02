#include "stdafx.h"
#include "thread.h"
#include "Trace.h"
#include "Kylin.h"
#include "Socket.h"
#include "MemoryOp.h"

_KYLIN_PACKAGE_BEGIN_

static volatile int s_nSockets = 0;

bool CSocketV::Close() 
{
    if (INVALID_SOCKET != m_s) {
        g_pNetworkMan->Deassociate(&m_Ctx);
        closesocket(m_s);
        m_s = INVALID_SOCKET;
        atomic_add(&s_nSockets, -1);
        
        if (ST_LISTEN == AtomicSetValue(m_type, ST_NONE)) {
            Release();
        }
        return true;
    }
    return false;
}

bool CSocketV::Shutdown(bool bLingerOff)
{
    int state = AtomicSetValue(m_Ctx.state, SS_SHUTDOWN);
    if (INVALID_SOCKET != m_s) {
        TRACE2("Shutdown socket(%x), LingerOff=%d\n", m_s, bLingerOff);
        if (bLingerOff) {
            SetLingerOff(m_s);
        }
        shutdown(m_s, SHUT_RDWR);
        if (SS_CONNECTING_0==state && !g_pNetworkMan->Delay(&m_Ctx, 0)) {
            AddRef();
        }
        return true;
    }
    return false;
}

void CSocketV::GetPeer(uint32 *pip, uint16* pport)
{
    SOCKADDR_IN sa;
    socklen_t namelen = sizeof(sa);
    getpeername(m_s, (LPSOCKADDR)&sa, &namelen);
    if (pport) {
        *pport = ntohs(sa.sin_port);
    }
    if (pip) {
        *pip = sa.sin_addr.s_addr;
    }
}

void CSocketV::GetLocal(uint32 *pip, uint16* pport)
{
    SOCKADDR_IN sa;
    socklen_t namelen = sizeof(sa);
    getsockname(m_s, (LPSOCKADDR)&sa, &namelen);
    if (pport) {
        *pport = ntohs(sa.sin_port);
    }
    if (pip) {
        *pip = sa.sin_addr.s_addr;
    }
}

APF_ERROR CSocketV::Create(bool bTCP, uint16 port, uint32 ip, uint32 flag)
{
    APF_ERROR err;
    ASSERT(!IsCreated());
    int retry = 0;
    SOCKADDR_IN sa;

retry1:
    m_s = WSASocket(AF_INET, bTCP ? SOCK_STREAM : SOCK_DGRAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (INVALID_SOCKET != m_s) {
        atomic_add(&s_nSockets, 1);
        SetReuseAddr(m_s);
    }
    else {
        err = WSAGetLastError();
        TRACE0("WSASocket() error: %d(%s)\n", err, strerror(err));
        if (retry++ < 10) {
            
            Sleep(500);
            TRACE0("Retry %d\n", retry);
            goto retry1;
        }
        else {
            TRACE0("Total sockets: %d\n", AtomicGetValue(s_nSockets));
            return err;
        }
    }
    
retry2:
    retry = 0;
    ZeroMemory(&sa, sizeof(SOCKADDR_IN));
    sa.sin_port = htons(port);
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = ip;
    
    UNLIKELY_IF (SOCKET_ERROR == bind(m_s, (LPSOCKADDR)&sa, sizeof(sa))) {
        err = WSAGetLastError();
        TRACE0("Error in bind(%s, %d): %s\n", bTCP?"TCP":"UDP", port, strerror(err));

        retry ++;
        if (retry<3 || (0==port && retry<10)) {
            Sleep(500);
            TRACE0("Retry %d\n", retry);
            goto retry2;
        }

        
        closesocket(m_s);
        m_s = INVALID_SOCKET;
        return err;
    }
    if (0 == port) {
        socklen_t namelen = sizeof(sa);
        getsockname(m_s, (LPSOCKADDR)&sa, &namelen);
        port = ntohs(sa.sin_port);
    }
    
    InitNetContext(&m_Ctx, m_s, this, flag);
    m_Ctx.ip = ip;
    m_Ctx.port = port;
    
    SetName("Socket(%x)", m_s);
    TRACE4("%s %s created, port=%d\n", m_name, bTCP?"TCP":"UDP", port);
    m_port = port;
    AtomicSetValue(m_type, ST_CREATED);
    if (!bTCP) {
        m_Ctx.flag |= SF_UDP;
        g_pNetworkMan->Monitor(&m_Ctx);
    }
    return APFE_OK;
}

APF_ERROR CSocketV::Create(SOCKET s, uint16 port, uint32 flag)
{
    ASSERT(!IsCreated());

    s_nSockets ++;
    m_s = s;
    m_port = port;
    InitNetContext(&m_Ctx, m_s, this, flag);
    m_Ctx.port = port;

    AtomicSetValue(m_type, ST_CREATED);
    SetName("Socket(%x)", m_s);
    g_pNetworkMan->Monitor(&m_Ctx);
    return APFE_OK; 
}

void CSocketV::Listen(int backlog)
{
    ASSERT(IsCreated());

    AtomicSetValue(m_type, ST_LISTEN);
    g_pNetworkMan->Listen(&m_Ctx, backlog);
}

void CSocketV::Connect(uint32 ip, uint16 port, uint32 timeout)
{
    
    ASSERT(IsCreated());
    g_pNetworkMan->Connect(&m_Ctx, ip, port, NULL, 0, timeout, &m_Req);
}

void CSocketV::ConnectAndWrite(uint32 ip, uint16 port, void* pBuf, int count, uint32 timeout)
{
    
    ASSERT(IsCreated());
    g_pNetworkMan->Connect(&m_Ctx, ip, port, pBuf, count, timeout, &m_Req);
}

void CSocketV::ReConnect(uint32 nMilliseconds)
{
    m_Ctx.pWriteReq = &m_Req;
    m_Ctx.nDelayRead = 0;
    m_Ctx.tRead.Init();
    m_Ctx.tWrite.Init();
    m_Ctx.state = SS_CONNECTING_0;
    g_pNetworkMan->Delay(&m_Ctx, nMilliseconds);
}

void CSocketV::Read(void* pBuf, int count, int request, NetRequest* pReq)
{
    

    g_pNetworkMan->Read(&m_Ctx, pBuf, count, request, pReq);
}

void CSocketV::Read(void* pBuf, int count, NetRequest* pReq)
{
    

    g_pNetworkMan->Read(&m_Ctx, pBuf, count, 0, pReq);
}

void CSocketV::Write(void* pBuf, int count, NetRequest* pReq)
{
    

    g_pNetworkMan->Write(&m_Ctx, pBuf, count, pReq);
}
 
void CSocketV::Write(BufHandle* pHdl, NetRequest* pReq)
{
    

    pReq->buf = NULL;
    pReq->len = 0;
    pReq->request = 0;
    pReq->pHdl = pHdl;
    g_pNetworkMan->Write(&m_Ctx, pReq);
}

void CSocketV::WriteTo(uint32 ip, uint16 port, void* pBuf, int count, NetRequest* pReq)
{
    

    pReq->ip = ip;
    pReq->port = port;
    g_pNetworkMan->Write(&m_Ctx, pBuf, count, pReq);
}

void CSocketV::WriteTo(uint32 ip, uint16 port, BufHandle* pHdl, NetRequest* pReq)
{
    

    pReq->ip = ip;
    pReq->port = port;
    pReq->buf = NULL;
    pReq->len = 0;
    pReq->request = 0;
    pReq->pHdl = pHdl;
    g_pNetworkMan->Write(&m_Ctx, pReq);
}

void CSocketV::OnCompletion(AsyncContext* pContext)
{
    NetRequest* pReq = (NetRequest*)pContext;
    int nAction = pContext->nAction;
    switch (nAction) {
    case AA_READ:
        OnRead(pContext->nErrCode, pReq);
        break;
        
    case AA_WRITE:
        OnWritten(pContext->nErrCode, pReq);
        break;
    
    case AA_LISTEN:
        if (0 == pContext->nErrCode) {
            while (1) {
                SOCKADDR_IN sa;
                socklen_t saLen = sizeof(sa);
            
                TRACE7("To accept(%x)\n", m_s);
                SOCKET s = accept(m_s, (LPSOCKADDR)&sa, &saLen);
                if (-1 != s) {
                    TRACE1("Accepted(%x) %s:%d = %x\n", m_s, in_ntoa(sa.sin_addr.s_addr), ntohs(sa.sin_port), s);
                    OnListened(s, &sa);
                }
                else {
                    TRACE7("Error in accept(%x)\n", m_s);
                    break;
                }
            }
        }
        delete pReq;
        break;
        
    case AA_CONNECT:
        AtomicSetValue(m_type, ST_CONNECTED);
        TRACE1("OnConnected(%x): %d, %p\n", m_s, pContext->nErrCode, pContext);
        if (0 == pContext->nErrCode) {
            OnConnected(0);
            break;
        }
        else if (OnConnected(pContext->nErrCode)) {
            break;
        }
        
        pContext->nErrCode = APFE_CONNECT_ERROR;
        Release();
        
    case AA_BROKEN:
        TRACE5("CSocket::OnBroken(%x): %d\n", m_s, pContext->nErrCode);
        AtomicSetValue(m_type, ST_ERROR);
        ASSERT(NULL == m_Ctx.pReadReq);
        ASSERT(NULL == m_Ctx.pWriteReq);

        m_Ctx.tWrite.Set(LARGE_ENOUGH_NEGATIVE);
        m_Ctx.tRead.Set(LARGE_ENOUGH_NEGATIVE);
        {
            NetRequest* pReq1;
            while (NULL != (pReq1=m_Ctx.qWrite.pop_front())) {
                OnWritten(pContext->nErrCode, pReq1);
                Release();
            }
            while (NULL != (pReq1=m_Ctx.qRead.pop_front())) {
                OnRead(pContext->nErrCode, pReq1);
                Release();
            }
        }
        OnBroken(pContext->nErrCode);
        
        if (nAction == AA_BROKEN)
            delete pReq;
        break;
    }
}

_KYLIN_PACKAGE_END_
