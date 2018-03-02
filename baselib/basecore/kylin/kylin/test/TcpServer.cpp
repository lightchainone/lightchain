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

#define DELAY_CLOSE         AA_USER_BEGIN

char* Time()
{
    static char buf[32];
    PrintNow(buf, sizeof(buf));
    return buf;
}

class CTestSocket : plclic CSocketV
{
    AsyncContext m_DelayCtx;
    char m_buf[4096];

    void OnBroken(APF_ERROR nErrCode) {
        Close();
        Release();
    }

    void OnListened(SOCKET s, SOCKADDR_IN* pSockAddrIn) {
        CTestSocket* pClient = new CTestSocket;
        
        if (APFE_OK != pClient->Create(s, pSockAddrIn->sin_port)) {
            pClient->Release();
            TRACE0("Failed to create client\n");
        }
    }

    void OnWritten(APF_ERROR nErrCode, NetRequest* pReq) {
        free(pReq->buf);
        free(pReq);
    }

    void OnRead(APF_ERROR nErrCode, NetRequest* pReq) {
        switch (nErrCode) {
        case APFE_OK:
            if (pReq->xfered > 0) {
                fwrite(pReq->buf, pReq->xfered, 1, stderr);
                Write(strndup((char*)pReq->buf, pReq->xfered+1), pReq->xfered+1, ZeroAlloc<NetRequest>());
            }
            Read(m_buf, 4096, &m_Req);
            break;

        case APFE_NOTIFY:
            SetTimeout();      
            TRACE0("%s %p set timeout\n", Time(), this);
            Read(m_buf, 4096, &m_Req);
            delete pReq;
            return;
        }

        int err = g_pExecMan->CancelExec(&m_DelayCtx);
        if (APFE_OK == err) {
            if (APFE_OK == nErrCode) {
                TRACE0("%s Reset timeout\n", Time());
                SetTimeout();               
            }
        }
    }

plclic:
    CTestSocket() { 
	    AddRef();
	    InitAsyncContext(&m_DelayCtx); 
        TRACE0("CTestSocket %p\n", this);
    }
    ~CTestSocket() {
        TRACE0("~CTestSocket %p\n", this);
    }

    void SetTimeout(uint32 nMilliseconds=10*1000) {
        g_pExecMan->DelayExec(DELAY_CLOSE, this, nMilliseconds, &m_DelayCtx);
    }

    APF_ERROR CreateAndListen(bool bTCP, uint16 port) {
        APF_ERROR err = CSocketV::Create(bTCP, port);
        if (APFE_OK == err) {
            Listen();
        }
        return err;
    }

    void OnDelayed() {
        TRACE0("%s %p timeout, close socket\n", Time(), this);
        Shutdown();
    }

    virtual void OnCompletion(AsyncContext* pContext) {
        switch (pContext->nAction) {
        case DELAY_CLOSE:
            OnDelayed();
            break;
        default:
            CSocketV::OnCompletion(pContext);
            break;
        }
    }
};


TEST_F(DETest, Case1) 
{
    CTestSocket ts;
    ts.CreateAndListen(true, 9000);

    while (1) {
        Sleep(1000);
    }
}
