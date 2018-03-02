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
	BufHandle* m_pHdl;

    void OnBroken(APF_ERROR nErrCode) {
        TRACE0("\x1b[37mdelete %p[%s, m_nRef=%d, err=%d, state=%d]\n\x1b[0m", this, m_name, AtomicGetValue(m_nRef), nErrCode, m_Ctx.state);
	Close();

	Release();
    }

    void OnListened(SOCKET s, SOCKADDR_IN* pSockAddrIn) {
        CTestSocket* pClient = new CTestSocket;
        
		
        if (APFE_OK != pClient->Create(s, pSockAddrIn->sin_port, false)) {
            pClient->Release();
            TRACE0("Failed to create client\n");
        }
        else {
            
            TRACE0("%s create client %p on %d\n", Time(), pClient, s);
        }
    }

    void OnRead(APF_ERROR nErrCode, NetRequest* pReq) {
        
		if(0 == nErrCode) {
			
			(*((int*)m_buf))++;


			Write(m_buf, 4, &m_Req);

		}
		else if (APFE_NOTIFY == nErrCode) {
			delete pReq;
			Read(m_buf, 4096, &m_Req); 
		}
		else {
            TRACE0("Error onRead() err=%d\n", nErrCode);
		}
        
    }

	void OnWritten(APF_ERROR nErrCode, NetRequest* pReq) {
		if(0 == nErrCode) {
			
			
		
			Shutdown();
		}
		else {
            TRACE0("Error onWriten() err=%d\n", nErrCode);
		}
	}
		

plclic:
    CTestSocket() { 
		InitAsyncContext(&m_DelayCtx); 
		m_pHdl = NULL;
		AddRef();
	}
    ~CTestSocket() {
		m_pHdl = NULL;
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
        TRACE0("%s Timeout. close socket %p\n", Time(), this);
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
    
    ts.CreateAndListen(true, 9100);

    while (1) {
        Sleep(1000);
    }
}
