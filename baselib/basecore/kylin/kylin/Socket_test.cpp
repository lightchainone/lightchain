#include <arpa/inet.h>
#include "stdafx.h"
#include "Kylin.h"
#include "Socket.h"
#include "UnitTest.h"
#include "../test/KylinUtils4Test.h"

#ifdef EXEC_TEST_WORKERS
#undef EXEC_TEST_WORKERS
#endif

#ifdef NET_TEST_WORKERS
#undef NET_TEST_WORKERS
#endif


#define EXEC_TEST_WORKERS 2
#define NET_TEST_WORKERS  2
#define LOCAL_HOST_IP inet_addr("127.0.0.1")

enum DelayAction {
	DELAY_CLOSE = AA_USER_BEGIN,
	DELAY_WRITE,
	DELAY_RETRY
};

static bool s_bStartKylin = true;
static bool s_bStopKylin = false;
static bool s_bPrintSysLog = false;
FILE* s_fpLog = stderr;

const static int s_ListeningPort = 46464;
const static int s_InvalidListeningPort = 46465;
const static int s_ConnectPort = 46466;

static inline char* Time()
{
    static char buf[128];
    PrintNow(buf, sizeof(buf));
    return buf;
}

class SocketTest {
plclic:
	void SetUp() {
		if(s_bStartKylin) {
			if(s_bPrintSysLog)
				KLSetLog(s_fpLog, 7, NULL);
			else
				KLSetLog(s_fpLog, 0, NULL);

			ASSERT(APFE_OK == InitKylin(EXEC_TEST_WORKERS, NET_TEST_WORKERS, 0)); 
		}
		ASSERT(true == IsKylinRunning());
	}

	void TearDown() {
		ASSERT(true == IsKylinRunning());
		if(s_bStopKylin) {
			while(APFE_OK != StopKylin()) 
				Sleep(100);
		}
	}
};


class CListenTester : plclic CSocketV
{
plclic:
	CListenTester() {  }

protected:
    virtual void OnBroken(APF_ERROR nErrCode) {
		ColorTrace(TC_Magenta, "%s broken[err=%d, m_nRef=%d].\n", m_name, nErrCode, m_nRef);
		Close();
		Release();
    }
};



TEST_F(SocketTest, Case1_test_Listen)
{
    s_bStartKylin = false;
	
    CListenTester* pClient1 = new CListenTester;
	pClient1->Create(true, s_ListeningPort, LOCAL_HOST_IP, false);
	pClient1->Listen();

	pClient1->Shutdown();
	Sleep(200);
}


static CCond s_Case2Cond;

class CConnectTester : plclic CSocketV
{
plclic:

	CConnectTester() { AddRef(); }

	~CConnectTester() { s_Case2Cond.Signal(); }
	
protected:
    virtual void OnBroken(APF_ERROR nErrCode) {
		ColorTrace(TC_Magenta, "%s broken[err=%d, m_nRef=%d]\n", m_name, nErrCode, m_nRef);
		ASSERT(2 == m_nRef);
		Close();
		Release();
    }

};




TEST_F(SocketTest, Case2_test_Connect)
{

	s_Case2Cond.Init(1);

	ColorPrintf(TC_Red, "test Connect to an invalid server.\n");

    CConnectTester* pClient1 = new CConnectTester();
	pClient1->Create(true, s_ConnectPort, LOCAL_HOST_IP, false);
	pClient1->Connect(LOCAL_HOST_IP, s_InvalidListeningPort);

	s_Case2Cond.Wait();

	Sleep(200);
}



static CToken s_LCToken; 

class CNormalListener: plclic CSocketV
{
plclic:
	CNormalListener() {  }
	
protected:

	virtual void OnListened(SOCKET s, SOCKADDR_IN* pSockAddrIn) {
		ColorTrace(TC_Magenta, "%s OnListened() connection from %s : %d.\n", m_name, in_ntoa(pSockAddrIn->sin_addr.s_addr), ntohs(pSockAddrIn->sin_port));
		while(0 != s_LCToken.TryAcquire(1))
			thread_yield();; 

		
		ASSERT(s_ConnectPort == ntohs(pSockAddrIn->sin_port));
		ASSERT(LOCAL_HOST_IP == pSockAddrIn->sin_addr.s_addr);
		ASSERT(SS_LISTENING == AtomicGetValue(m_Ctx.state));
		ASSERT(ST_LISTEN == AtomicGetValue(m_type));

		ASSERT(1 == s_LCToken.Transfer(1, 2)); 
		
		
		while(3 != s_LCToken.Check())
			thread_yield(); 
		ASSERT(3 == s_LCToken.Transfer(3, 4)); 
		

		closesocket(s);
	}

    virtual void OnBroken(APF_ERROR nErrCode) {
		ColorTrace(TC_Magenta, "normal listener(%s) broken[err=%d].\n", m_name, nErrCode);
		Close();
		Release();
    }

private:
	CCond m_ListenedCond;
};


class CNormalConnector: plclic CSocketV
{
plclic:
	CNormalConnector() { AddRef(); }

protected:

	virtual bool OnConnected(APF_ERROR nErrCode) {
		if(0 == nErrCode) {
			ColorTrace(TC_Magenta, "%s OnConnected() connect to %s : %d.\n", m_name, in_ntoa(m_Ctx.ip), m_Ctx.port);
			while(2 != s_LCToken.Check())
				thread_yield(); 
		
			
			ASSERT(3 == GetRef());
			ASSERT(SS_CONNECTED == AtomicGetValue(m_Ctx.state));
			ASSERT(ST_CONNECTED == AtomicGetValue(m_type));

			ASSERT(2 == s_LCToken.Transfer(2, 3)); 
			

			Shutdown();
			return true;
		}
		else {
			Shutdown();
			return false;
		}
	}

    virtual void OnBroken(APF_ERROR nErrCode) {
		ColorTrace(TC_Magenta, "normal connector(%s) broken[err=%d].\n", m_name, nErrCode);
		Close();
		Release();
	}
};



static void* ConnectingProc(void* arg)
{
	ColorTrace(TC_Magenta, "Connecting thread start...\n");

	
	CNormalConnector* pClient = new CNormalConnector;
	pClient->Create(true, s_ConnectPort, LOCAL_HOST_IP, true);
	pClient->Connect(LOCAL_HOST_IP, s_ListeningPort); 

	

	
	while(4 != s_LCToken.Check())
		thread_yield(); 
	s_LCToken.Release(4); 
	return NULL;
}



TEST_F(SocketTest, Case3_test_Listen_and_Connect)
{

	ColorPrintf(TC_Red, "test normal Listen && Connect.\n");

	s_LCToken.Init();

	
	CNormalListener* pServer = new CNormalListener;	
	pServer->Create(true, s_ListeningPort, LOCAL_HOST_IP, true);
	pServer->Listen();

	
	while(SS_LISTENING != AtomicGetValue(pServer->GetCtx()->state))
		thread_yield(); 
	ASSERT(SS_LISTENING == AtomicGetValue(pServer->GetCtx()->state));	

	
	thread_t tid;
	thread_create(&tid, NULL, ConnectingProc, NULL);

	thread_join(tid, NULL); 

	pServer->Shutdown(); 

	Sleep(300);

}



class CReConnector: plclic CSocketV
{
plclic:
	CReConnector() { AddRef(); }

protected:
	virtual bool OnConnected(APF_ERROR nErrCode) {
		if(0 == nErrCode) {
			ColorTrace(TC_Magenta, "%s OnConnected() connect to %s : %d.\n", m_name, in_ntoa(m_Ctx.ip), m_Ctx.port);
			while(2 != s_LCToken.Check())					
				thread_yield(); 

				
			ASSERT(3 == GetRef());
			ASSERT(SS_CONNECTED == AtomicGetValue(m_Ctx.state));
			ASSERT(ST_CONNECTED == AtomicGetValue(m_type));
			ASSERT(2 == s_LCToken.Transfer(2, 3)); 
			
			Shutdown(); 
			return true;
		}
		else {				
			ColorTrace(TC_Magenta, "%s try to connect(%s:%d) Failed, ReConnect(%s:%d) in 500ms.\n", m_name, in_ntoa(m_Ctx.ip), m_Ctx.port, in_ntoa(m_Ctx.ip), s_ListeningPort);

			
			ASSERT(3== GetRef());
			ASSERT(APFE_NET_ERROR == nErrCode);				
			ASSERT(SS_ERROR == AtomicGetValue(m_Ctx.state));
			ASSERT(ST_CONNECTED == AtomicGetValue(m_type));

			m_Ctx.port = s_ListeningPort; 

			ReConnect(500);	
			s_LCToken.Release(9); 
			return true;
		}
	}

	virtual void OnBroken(APF_ERROR nErrCode) {
		ColorTrace(TC_Magenta, "ReConnector(%s) broken[err=%d].\n", m_name, nErrCode);
		Close();
		Release();
	}
};



static void* ReConnectProc(void* arg)
{
	ColorTrace(TC_Magenta, "ReConnect thread start...\n");

	
	CReConnector* pClient = new CReConnector;
	pClient->Create(true, s_ConnectPort, LOCAL_HOST_IP, false);
	pClient->Connect(LOCAL_HOST_IP, s_InvalidListeningPort); 

	

	
	while(4 != s_LCToken.Check())
		thread_yield(); 

	s_LCToken.Release(4); 

	return NULL;
}



TEST_F(SocketTest, Case4_test_ReConnect)
{
	ColorPrintf(TC_Red, "test ReConnect.\n");

	s_LCToken.Init(9); 

	
	CNormalListener* pServer = new CNormalListener;	
	pServer->Create(true, s_ListeningPort, LOCAL_HOST_IP, false);
	pServer->Listen();

	
	while(SS_LISTENING != AtomicGetValue(pServer->GetCtx()->state))
		thread_yield(); 
	ASSERT(SS_LISTENING == AtomicGetValue(pServer->GetCtx()->state));	

	
	thread_t tid;
	thread_create(&tid, NULL, ReConnectProc, NULL);
	thread_join(tid, NULL); 

	pServer->Shutdown();

	Sleep(200);

}



template<typename TSlave>
class CNormalServer: plclic CSocketV
{
plclic:
	CNormalServer() {
		
	}

	virtual void OnBroken(APF_ERROR nErrCode) {
		ColorTrace(TC_Red, "Normal Server broken[err=%d, m_nRef=%d]\n", nErrCode, AtomicGetValue(m_nRef));
		Close();
		Release();
	}

protected:

	virtual void OnListened(SOCKET s, SOCKADDR_IN* pSockAddrIn) {
		

		TSlave* pNewSlave = new TSlave;

		if (APFE_OK != pNewSlave->Create(s, 0, true)) {
			ColorTrace(TC_Red, "Failed to create slave.\n");
			pNewSlave->Release();
		}
		
	}

};




static CCond s_ClientCond;





class CServerSlave: plclic CSocketV
{
plclic:
	CServerSlave() {
		AddRef();
		m_pHdl = NULL;
	}

protected:

    virtual void OnBroken(APF_ERROR nErrCode) {
		
		Close();
		Release();
    }

	virtual void OnRead(APF_ERROR nErrCode, NetRequest* pReq) {
		if(0 == nErrCode) {
			m_nDataLen = pReq->xfered;
			ASSERT(m_nDataLen == sizeof(m_Data));

			
			

			m_Data += 1;

			
			m_pHdl = AllocateHdl(false, (char*)&m_Data, sizeof(m_Data), NULL);
			ASSERT(NULL != m_pHdl);

			Write(m_pHdl, &m_Req);

			m_nDataLen = 0;
		}
		else if (APFE_NOTIFY == nErrCode) {
			delete pReq; 
			Read((void*)&m_Data, sizeof(m_Data), &m_Req);
		}
		else {
			ColorTrace(TC_Red, "slave[%s] read ERROR[err=%d].\n", m_name, nErrCode);
		}
	}

	virtual void OnWritten(APF_ERROR nErrCode, NetRequest* pReq) {
		if(0 == nErrCode) {
			
			ChainFreeHdl(m_pHdl, NULL);
			m_pHdl = NULL;

			m_nDataLen = pReq->xfered;
			
			

			Shutdown(); 
		}
		else {
			ColorTrace(TC_Red, "slave[%s] write ERROR[err=%d].\n", m_name, nErrCode);
		}
	}
		
private:
	int m_Data;
	int m_nDataLen;
	BufHandle* m_pHdl;
	uint32 m_PeerIp;
	uint16 m_PeerPort;
};


static int s_n = 0;

class CNormalClient: plclic CSocketV
{

plclic:
	CNormalClient() {
		AddRef(); 
		m_Data = s_n++;
		m_nDataLen = 0;
		m_pHdl = NULL;
	}

protected:

	virtual bool OnConnected(APF_ERROR nErrCode) {
		if(0 == nErrCode) {
			
			
			
			m_pHdl = AllocateHdl(false, (char*)&m_Data, sizeof(m_Data), NULL);
			ASSERT(NULL != m_pHdl);

			
			

			Write(m_pHdl, &m_Req);	

			return true;
		}
		else {
			ColorTrace(TC_Red, "Error: normal client[%s] connect error.\n", m_name);
			return false;
		}
	}

	virtual void OnWritten(APF_ERROR nErrCode, NetRequest* pReq) {
		if(0 == nErrCode) {
			
			ChainFreeHdl(m_pHdl, NULL);
			m_pHdl = NULL;
			m_nDataLen = pReq->xfered;
			
			
			

			Read((void*)&m_Read, sizeof(m_Read), &m_Req);
			m_nDataLen = 0;
		}
		else {
			ColorTrace(TC_Red, "normal client[%s] write ERROR[err=%d].\n", m_name, nErrCode);
		}
	}

	virtual void OnRead(APF_ERROR nErrCode, NetRequest* pReq) {
		if(0 == nErrCode) {
			m_nDataLen = pReq->xfered;
			
			
			
			ASSERT(m_Read == m_Data + 1);

			Shutdown(); 
		}
		else {
			ColorTrace(TC_Red, "ERROR: normal client[%s] read ERROR[err=%d].\n", m_name, nErrCode);
		}

		
		

		s_ClientCond.Signal();
	}

    virtual void OnBroken(APF_ERROR nErrCode) {
		
		Close();
		Release();
    }

private:
	int m_nDataLen;
	int m_Data, m_Read;
	CCond m_RCond;
	BufHandle* m_pHdl;
	uint32 m_PeerIp;
	uint16 m_PeerPort;
};



static void* NormalClientProc(void* arg)
{
	const int nClientNum = 100;
	
	s_ClientCond.Init(nClientNum);

	ColorTrace(TC_Magenta, "normal-client2 thread start...\n");
	
	CNormalClient *pClient[nClientNum];	

	for(int i = 0; i < nClientNum; ++i) {
		
		pClient[i] = new CNormalClient;
		pClient[i]->Create(true, 0, LOCAL_HOST_IP, true);
		pClient[i]->Connect(LOCAL_HOST_IP, s_ListeningPort); 
	}

	s_ClientCond.Wait();

	
	

	return NULL;
}


TEST_F(SocketTest, Case5_A_Lager_Scale_Read_and_Write_test)
{
	
	

	
	CNormalServer<CServerSlave>* pServer = new CNormalServer<CServerSlave>;	
	pServer->Create(true, s_ListeningPort, LOCAL_HOST_IP, true);
	pServer->Listen();

	
	while(SS_LISTENING != AtomicGetValue(pServer->GetCtx()->state))
		thread_yield(); 
	ASSERT(SS_LISTENING == AtomicGetValue(pServer->GetCtx()->state));	

	
	thread_t tid;
	thread_create(&tid, NULL, NormalClientProc, NULL);

	thread_join(tid, NULL); 

	pServer->Shutdown();

	Sleep(300);

	
	
}


static CCond s_DelayCond;


template<typename TSlave>
class CServerWithTimeout: plclic CSocketV
{
plclic:

	CServerWithTimeout() {
		
	}
private:
	void OnBroken(APF_ERROR nErrCode) {
		Close();
		Release();
	}

	void OnListened(SOCKET s, SOCKADDR_IN* pSockAddrIn) {

		TSlave* pSlave = new TSlave;
		
        if(APFE_OK != pSlave->Create(s, 0)) {
            pSlave->Release();
            ColorTrace(TC_Red, "server[%s] failed to create slave.\n", m_name);
        }
        else{
            ColorTrace(TC_Magenta, "server[%s] create slave [%s] at %s\n", m_name, pSlave->GetName(), Time());
            pSlave->SetTimeout(2000);      
        }
	}
	
};


class CSlaveWithTimeout: plclic CSocketV
{
plclic:
	CSlaveWithTimeout() {
		AddRef();
		InitAsyncContext(&m_DelayCtx);
		AtomicSetValue(m_bAlreadyDelayed, false);
	}

    void SetTimeout(uint32 nMilliseconds=10*1000) {
		ColorTrace(TC_Magenta, "slave[%s] SetTimeout(%d)\n", m_name, nMilliseconds);
		InitAsyncContext(&m_DelayCtx);
        ASSERT(APFE_OK == g_pExecMan->DelayExec(DELAY_CLOSE, this, nMilliseconds, &m_DelayCtx));
		AtomicSetValue(m_bAlreadyDelayed, true);
    }

    void OnDelayed() {
        ColorTrace(TC_Red, "slave[%s] timeout at %s. close.\n", m_name, Time());
        Shutdown();
	}

    virtual void OnCompletion(AsyncContext* pContext) {
        switch (pContext->nAction) {
        case DELAY_CLOSE:
			ASSERT(true == AtomicGetValue(m_bAlreadyDelayed));
			AtomicSetValue(m_bAlreadyDelayed, false);
            OnDelayed();
            break;
        default:
            CSocketV::OnCompletion(pContext);
            break;
        }
    }

protected:

	virtual void OnRead(APF_ERROR nErrCode, NetRequest* pReq) {

		if(0 == nErrCode) {
			if(true == AtomicGetValue(m_bAlreadyDelayed)) {
				int err = g_pExecMan->CancelExec(&m_DelayCtx);
				AtomicSetValue(m_bAlreadyDelayed, false);

				if (APFE_OK == err) {
					SetTimeout(2000);               
				}
				else {
				}
			}

			ASSERT(pReq->xfered == sizeof(m_data));
			ColorTrace(TC_Magenta, "slave[%s] have read \"%d\" from client.\n", m_name, m_data);
			m_data += 1;
			InitAsyncContext(&m_Req.async);
			Write((void*)&m_data, sizeof(m_data), &m_Req);
		}
		else if (APFE_NOTIFY == nErrCode) {
			
			delete pReq; 
			Read((void*)&m_data, sizeof(m_data), &m_Req);
		}
		else {
			ColorTrace(TC_Red, "slave[%s] read ERROR[err=%d].\n", m_name, nErrCode);
		}
	}

	virtual void OnWritten(APF_ERROR nErrCode, NetRequest* pReq) {
		if(0 == nErrCode) {
			ColorTrace(TC_Magenta, "slave[%s] have written \"%d\" to client.\n", m_name, m_data);
			ASSERT(sizeof(m_data) == pReq->xfered);

			Shutdown(); 
		}
		else {
			ColorTrace(TC_Red, "slave[%s] write ERROR[err=%d].\n", m_name, nErrCode);
		}
	}

private:
	void OnBroken(APF_ERROR nErrCode) {
		Close();
		Release();

		if(true == AtomicGetValue(m_bAlreadyDelayed)) {
			int err = g_pExecMan->CancelExec(&m_DelayCtx);
			AtomicSetValue(m_bAlreadyDelayed, false);

			if (APFE_OK == err) 
				ColorTrace(TC_Magenta, "slave[%s] cancel the timeout event.\n", m_name);
			else 
				ColorTrace(TC_Red, "slave[%s] failed to cancel EXEC[err=%d].\n", m_name, err);
		}	

	}
	
private:
	int m_data;
	AsyncContext m_DelayCtx;
	bool m_bAlreadyDelayed;
};




class CDelayWriteClient: plclic CSocketV
{
plclic:
	CDelayWriteClient(uint32 nSleepTime=1000): m_nSleepTime(nSleepTime) {
		AddRef(); 
		m_write = s_n++;
	    InitAsyncContext(&m_DelayCtx); 
		AtomicSetValue(m_bDelayed, false);
	}

	~CDelayWriteClient() {
		
	}

protected:

	virtual bool OnConnected(APF_ERROR nErrCode) {
		if(0 == nErrCode) {
			ASSERT(false == AtomicGetValue(m_bDelayed));
		    ASSERT(APFE_OK == g_pExecMan->DelayExec(DELAY_WRITE, this, m_nSleepTime, &m_DelayCtx));
			AtomicSetValue(m_bDelayed, true);
			return true;
		}
		else {
			ColorTrace(TC_Red, "ERROR: client[%s] connect failed[err=%d].\n", m_name, nErrCode);
			Shutdown();
			return false;
		}
	}

	virtual void OnWritten(APF_ERROR nErrCode, NetRequest* pReq) {
		if(0 == nErrCode) {
			ASSERT(sizeof(m_write) == pReq->xfered);
			ColorTrace(TC_White, "client(%s) have written \"%d\" to server.\n", m_name, m_write);
			InitAsyncContext(&m_Req.async);
			Read((void*)&m_read, sizeof(m_read), &m_Req);
		}
		else {
			ColorTrace(TC_Red, "client[%s] write ERROR[err=%d].\n", m_name, nErrCode);
		}
	}

	virtual void OnRead(APF_ERROR nErrCode, NetRequest* pReq) {
		if(0 == nErrCode) {
			ASSERT(sizeof(m_read) == pReq->xfered);
			
			ASSERT(m_read == m_write + 1);

			ColorTrace(TC_White, "client(%s) have read \"%d\" from server.\n", m_name, m_read);

			Shutdown(); 
		}
		else {
			ColorTrace(TC_Red, "ERROR: client[%s] read ERROR[err=%d].\n", m_name, nErrCode);
		}

		s_DelayCond.Signal();
	}

    virtual void OnBroken(APF_ERROR nErrCode) {
		ColorTrace(TC_White, "client[%s] onBroken.\n", m_name);
		Close();
		Release();

		if(true == AtomicGetValue(m_bDelayed)) {
			int err = g_pExecMan->CancelExec(&m_DelayCtx);
			AtomicSetValue(m_bDelayed, false);

			if (APFE_OK == err) 
				ColorTrace(TC_Magenta, "client[%s] cancel the timeout event.\n", m_name);
			else 
				ColorTrace(TC_White, "client[%s] failed to cancel EXEC[err=%d].\n", m_name, err);
			s_DelayCond.Signal(); 
		}	
    }

    virtual void OnCompletion(AsyncContext* pContext) {
        switch (pContext->nAction) {
        case DELAY_WRITE:
			ASSERT(true == AtomicSetValueIf(m_bDelayed, false, true));
            OnDelayed();
            break;
        default:
            CSocketV::OnCompletion(pContext);
            break;
        }
    }

private:
	
	void OnDelayed() {
		ASSERT(false == AtomicGetValue(m_bDelayed));
		InitAsyncContext(&m_Req.async);
		Write((void*)&m_write, sizeof(m_write), &m_Req);
	}

private:
	int m_write, m_read;
	uint32 m_nSleepTime;
	AsyncContext m_DelayCtx;
	bool m_bDelayed;
};


static void* DelayWriteProc(void* arg)
{
	s_DelayCond.Init(3);

	

	
	
	CDelayWriteClient* pClient_1 = new CDelayWriteClient(1000);
	pClient_1->Create(true, 0, LOCAL_HOST_IP, false);
	pClient_1->Connect(LOCAL_HOST_IP, s_ListeningPort); 

	
	
	CDelayWriteClient* pClient_2 = new CDelayWriteClient(2000);
	pClient_2->Create(true, 0, LOCAL_HOST_IP, false);
	pClient_2->Connect(LOCAL_HOST_IP, s_ListeningPort); 
	
	
	
	
	CDelayWriteClient* pClient_3 = new CDelayWriteClient(3000);
	pClient_3->Create(true, 0, LOCAL_HOST_IP, false);
	pClient_3->Connect(LOCAL_HOST_IP, s_ListeningPort); 

	s_DelayCond.Wait();
	return NULL;
}


TEST_F(SocketTest, Case6_test_delay)
{
	s_n = 0;
	CServerWithTimeout<CSlaveWithTimeout>* pServer = new CServerWithTimeout<CSlaveWithTimeout>;
	pServer->Create(true, s_ListeningPort, LOCAL_HOST_IP, false);
	pServer->Listen();

	while(SS_LISTENING != AtomicGetValue(pServer->GetCtx()->state))
		thread_yield(); 
	ASSERT(SS_LISTENING == AtomicGetValue(pServer->GetCtx()->state));	

	thread_t tid;
	thread_create(&tid, NULL, DelayWriteProc, NULL);
	thread_join(tid, NULL); 

	pServer->Shutdown();
	Sleep(500);
}



static CCond s_ReConnectCond;

class CClientWithReconnect : plclic CSocketV
{
plclic:
	CClientWithReconnect(uint32 nRetryInterval=2000): m_nRetryInterval(nRetryInterval) {
		AddRef();
		m_nRetryTime = 1;
	}
private:

	virtual bool  OnConnected(APF_ERROR nErrCode) {
		if(0 == nErrCode) {
			ColorTrace(TC_White, "client[%s] connect success at %s.\n", m_name, Time());
			Shutdown();
		}
		else {
			ColorTrace(TC_White, "client[%s] connect failed at %s, retry %d time after %dms.\n", m_name, Time(), m_nRetryTime++, m_nRetryInterval);
			ReConnect(m_nRetryInterval);
		}

		return true;
	}

    virtual void OnBroken(APF_ERROR nErrCode) {
		ColorTrace(TC_White, "client[%s] onBroken.\n", m_name);
		Close();
		Release();
		s_ReConnectCond.Signal();
    }

private:
	int m_nRetryTime;
	uint32 m_nRetryInterval;
}; 


class CServerWithNoSlave: plclic CSocketV
{
plclic:
	CServerWithNoSlave() {
		ColorTrace(TC_Magenta, "server[%s] created at %s.\n", m_name, Time());
		
	}

private:

	

	void OnBroken(APF_ERROR nErrCode) {
		Close();
		Release();
	}
};


static void* ReConnectClientProc(void* arg) {

	s_ReConnectCond.Init(1);

	CClientWithReconnect* pClient = new CClientWithReconnect; 
	pClient->Create(true, 0, LOCAL_HOST_IP, false);
	pClient->Connect(LOCAL_HOST_IP, s_ListeningPort); 

	s_ReConnectCond.Wait();
	return NULL;
}


TEST_F(SocketTest, Case7_test_ReConnect_2)
{
	s_bStartKylin = false;

	thread_t tid;
	thread_create(&tid, NULL, ReConnectClientProc, NULL);

	Sleep(5000); 

	CServerWithNoSlave* pServer = new CServerWithNoSlave;
	pServer->Create(true, s_ListeningPort, LOCAL_HOST_IP, false);
	pServer->Listen();

	thread_join(tid, NULL); 

	pServer->Shutdown();

	Sleep(300);
}




static uint64 s_uint64 = 1;
static CCond s_CWCond;

class CCWClient: plclic CSocketV
{
plclic:
	CCWClient() { 
		AddRef(); 
		m_write = s_uint64++; 
		m_read = 0;
	}

	void CAndW() {
		ConnectAndWrite(LOCAL_HOST_IP, s_ListeningPort, (void*)&m_write, sizeof(m_write));
	}

private:

	virtual bool OnConnected(APF_ERROR nErrCode) {
		if(0 == nErrCode) {
			ColorTrace(TC_White, "client[%s] onConnected sucess.\n", m_name);
			Read((void*)&m_read, sizeof(m_read), &m_Req);
			return true;
		}
		else {
			ColorTrace(TC_Red, "ERROR: client[%s] onConnected failed[err=%d].\n", m_name, nErrCode);
			return false;
		}
	}

	virtual void OnRead(APF_ERROR nErrCode, NetRequest* pReq) {
		if(0 == nErrCode) {
			ASSERT(sizeof(m_read) == pReq->xfered);
			
			ASSERT(m_read == m_write + 1);

			ColorTrace(TC_White, "client[%s] have read \"%lld\" from server.\n", m_name, m_read);

			Shutdown(); 
		}
		else {
			ColorTrace(TC_Red, "ERROR: client[%s] read ERROR[err=%d].\n", m_name, nErrCode);
		}

		s_CWCond.Signal();
	}


	virtual void OnBroken(APF_ERROR nErrCode) {
		ColorTrace(TC_Red, "client[%s] onBroken.\n", m_name);
		Close();
		Release();
	}

private:
	uint64 m_write, m_read;
};


class CSlave2: plclic CSocketV
{
plclic:
	CSlave2() { AddRef(); }

protected:

    virtual void OnBroken(APF_ERROR nErrCode) {
		ColorTrace(TC_Red, "slave[%s] broken[err=%d].\n", m_name, nErrCode);
		Close();
		Release();
    }

	virtual void OnRead(APF_ERROR nErrCode, NetRequest* pReq) {
		if(0 == nErrCode) {
			ASSERT(pReq->xfered == sizeof(m_data));

			ColorTrace(TC_Magenta, "slave[%s] have read \"%lld\".\n", m_name, m_data);

			m_data += 1;

			Write((void*)&m_data, sizeof(m_data), &m_Req);
		}
		else if (APFE_NOTIFY == nErrCode) {
			delete pReq; 
			Read((void*)&m_data, sizeof(m_data), &m_Req);
		}
		else {
			ColorTrace(TC_Red, "slave[%s] read ERROR[err=%d].\n", m_name, nErrCode);
		}
	}

	virtual void OnWritten(APF_ERROR nErrCode, NetRequest* pReq) {
		if(0 == nErrCode) {
			ASSERT(sizeof(m_data) == pReq->xfered);
			ColorTrace(TC_Magenta, "slave[%s] have writen \"%lld\".\n", m_name, m_data);

			Shutdown(); 
		}
		else {
			ColorTrace(TC_Red, "slave[%s] write ERROR[err=%d].\n", m_name, nErrCode);
		}
	}
		
private:
	uint64 m_data;
};


static void* CWClientProc(void* arg) {

	const int nClientNum = 10;
	
	s_CWCond.Init(nClientNum);

	CCWClient* pClient[nClientNum];

	for(int i = 0; i < nClientNum; ++i) {
		pClient[i] = new CCWClient; 
		ASSERT(APFE_OK == pClient[i]->Create(true, 0, LOCAL_HOST_IP, false));
		pClient[i]->CAndW();
	}

	s_CWCond.Wait();
	return NULL;
}


TEST_F(SocketTest, Case8_test_ConnectAndWrite)
{
	s_uint64 = 1;
	CNormalServer<CSlave2>* pServer = new CNormalServer<CSlave2>;	
	pServer->Create(true, s_ListeningPort, LOCAL_HOST_IP, true);
	pServer->Listen();

	
	while(SS_LISTENING != AtomicGetValue(pServer->GetCtx()->state))
		thread_yield(); 
	ASSERT(SS_LISTENING == AtomicGetValue(pServer->GetCtx()->state));	

	
	thread_t tid;
	thread_create(&tid, NULL, CWClientProc, NULL);

	thread_join(tid, NULL); 
	
	pServer->Shutdown();
	Sleep(300);

	s_bStopKylin = true; 
}

#undef EXEC_TEST_WORKERS
#undef NET_TEST_WORKERS
#undef LOCAL_HOST_IP 
