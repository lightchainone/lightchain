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
        TRACE0("%p onbroken %d\n", this, GetRef());
        Close();
        Release();
    }

    bool OnConnected(APF_ERROR nErrCode) {
        TRACE0("%p OnConnected %d\n", this, GetRef());
        if (0 == nErrCode) {
            Read(m_buf, 4096, &m_Req);
            return true;
        }
        return false;
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

    APF_ERROR CreateAndConnect(uint32 ip, uint16 port) {
        APF_ERROR err = CSocketV::Create(true);
        if (APFE_OK == err) {
            SetTimeout(5*1000);
            Connect(ip, port);
        }
        return err;
    }

    void OnDelayed() {
        TRACE0("%s %p timeout, close socket, %d\n", Time(), this, GetRef());
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
    CTestSocket* pts = new CTestSocket;
    pts->CreateAndConnect(in_aton("127.0.0.1"), 9000);

    while (1) {
        Sleep(1000);
    }
}
