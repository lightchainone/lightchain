#include "stdafx.h"
#include "Kylin.h"
#include "UnitTest.h"
#include "Socket.h"

class DETest {
  plclic:
    void SetUp() {
        InitKylin(2, 2, 0);
    }
    void TearDown() {
    }
};

enum UserAction {
    UA_CONN_TIMEOUT    = AA_USER_BEGIN,
    UA_ACK_TIMEOUT,
    UA_TIMER,
};

enum FailReason {
    FR_OK           = 0,
    FR_CONN_FAIL,
    FR_CONN_TIMEOUT,
    FR_CONN_BROKEN,
    FR_ACK_TIMEOUT,
};

char* PrintReason(int nReason) 
{
    static char* str[] = {
        "Okay",
        "Connect fail",
        "Connect timeout",
        "Connection broken",
        "Ack timeout"
    };

    if (nReason>=0 && nReason<=FR_ACK_TIMEOUT) {
        return str[nReason];
    }
    return "Unknown";
}

char* Time()
{
    static char buf[32];
    PrintNow(buf, sizeof(buf));
    return buf;
}


class CClientManager;
class CClientSocket : plclic CSocketV
{
    bool m_bCancel;
    AsyncContext m_DelayCtx;
    CClientManager* m_pManager;
    char m_buf[4096];
    int m_nAckTimeout;
    bool m_bNotifyManager;

    void OnCompletion(AsyncContext* pContext);
    void OnBroken(APF_ERROR nErrCode);
    void OnRead(APF_ERROR nErrCode, NetRequest* pReq);
    void OnWritten(APF_ERROR nErrCode, NetRequest* pReq);
    bool OnConnected(APF_ERROR nErrCode);

plclic:
    CClientSocket(CClientManager* pManager) { 
	    AddRef();
        InitAsyncContext(&m_DelayCtx);
        m_bCancel = false;
        m_bNotifyManager = true;
        m_pManager = pManager;
        TRACE0("CClientSocket %p\n", this);
    }
    ~CClientSocket() {
        TRACE0("~CClientSocket %p\n", this);
    }

    void Connect(uint32 ip, uint16 port, int conn_timeout, int ack_timeout) {
        m_nAckTimeout = ack_timeout;
        CSocketV::Connect(ip, port, conn_timeout);
    }
};


class CClientManager : plclic CAsyncClient
{
    AsyncContext m_DelayCtx;
    uint32 m_ip;
    uint16 m_port;

    void DoStartClient() {
        CClientSocket* pClient = new CClientSocket(this);
        if (APFE_OK != pClient->Create(true)) {
            pClient->Release();
            TRACE0("Failed to create client\n");
            return;
        }
        TRACE0("%s Connect\n", Time());
        pClient->Connect(m_ip, m_port, 1, 3*1000);
    }

    void OnCompletion(AsyncContext* pCtx) {
        switch (pCtx->nAction) {
        case UA_TIMER:
            DoStartClient();
            g_pExecMan->DelayExec(UA_TIMER, this, 10*1000, &m_DelayCtx);
            break;
        }
    }

plclic:
    CClientManager() {
        InitAsyncContext(&m_DelayCtx);
    }
    ~CClientManager() {}

    void Start(uint32 ip, uint16 port) {
        m_ip = ip;
        m_port = port;
        g_pExecMan->DelayExec(UA_TIMER, this, 0, &m_DelayCtx);
    }

    void NotifyClientClose(CClientSocket* pClient, int nReason) {
        
        TRACE0("Client closed: %p, %s\n", pClient, PrintReason(nReason));
    }
};

void CClientSocket::OnRead(APF_ERROR nErrCode, NetRequest* pReq) 
{
    switch (nErrCode) {
    case APFE_OK:
        if (pReq->xfered > 0) {
            fwrite(pReq->buf, pReq->xfered, 1, stderr);
            if (m_bNotifyManager) {
                m_bNotifyManager = false;
                m_pManager->NotifyClientClose(this, FR_OK);
            }
            g_pExecMan->CancelExec(&m_DelayCtx);
            Shutdown();
        }
        
        break;

    case APFE_NOTIFY:
        Read(m_buf, 4096, &m_Req);
        delete pReq;
        break;
    }
}

void CClientSocket::OnBroken(APF_ERROR nErrCode)
{
    Close();
    if (m_bNotifyManager) {
        m_bNotifyManager = false;
        m_pManager->NotifyClientClose(this, FR_CONN_BROKEN);
    }
    Release();
}

void CClientSocket::OnWritten(APF_ERROR nErrCode, NetRequest* pReq) 
{
    free(pReq->buf);
}

bool CClientSocket::OnConnected(APF_ERROR nErrCode) 
{
    TRACE0("%p onconnected %d\n", this, nErrCode);
    if (APFE_OK == nErrCode) {
        g_pExecMan->DelayExec(UA_ACK_TIMEOUT, this, m_nAckTimeout, &m_DelayCtx);
        NetRequest* pReq = ZeroAlloc<NetRequest>();
        char* pBuf = strdup("hello world\n");
        Write(pBuf, 12, pReq);
        Read(m_buf, 4096, &m_Req);
        return true;
    }
    else {
        m_bCancel = true;
        ASSERT(m_bNotifyManager);
        m_bNotifyManager = false;
        m_pManager->NotifyClientClose(this, APFE_TIMEDOUT == nErrCode ? FR_CONN_TIMEOUT : FR_CONN_FAIL);
    }
    return false;
}

void CClientSocket::OnCompletion(AsyncContext* pContext) 
{
    switch (pContext->nAction) {
    case UA_ACK_TIMEOUT:
        if (!m_bCancel) {
            TRACE0("%s %p timeout\n", Time(), this);
            m_bCancel = true;
            Shutdown();
        }
        if (m_bNotifyManager) {
            m_bNotifyManager = false;
            m_pManager->NotifyClientClose(this, FR_ACK_TIMEOUT);
        }
        break;

    default:
        CSocketV::OnCompletion(pContext);
        break;
    }
}

TEST_F(DETest, Case1) 
{
    CClientManager cm;
    cm.Start(in_aton("127.0.0.1"), 9000);

    while (1) {
        Sleep(1000);
    }
}
