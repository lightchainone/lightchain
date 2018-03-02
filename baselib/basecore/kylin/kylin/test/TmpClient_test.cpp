#include <arpa/inet.h>
#include "stdafx.h"
#include "Kylin.h"
#include "Socket.h"
#include "UnitTest.h"

#ifdef EXEC_TEST_WORKERS
#undef EXEC_TEST_WORKERS
#endif

#ifdef NET_TEST_WORKERS
#undef NET_TEST_WORKERS
#endif

#define EXEC_TEST_WORKERS 1
#define NET_TEST_WORKERS  1
#define LOCAL_HOST_IP inet_addr("127.0.0.1")

static bool s_bStartKylin = true;
static bool s_bStopKylin = false;
static bool s_bPrintSysLog = false;
FILE* s_fpLog = stderr;

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

static int s_n = 0;
static CCond s_Cond;

class CNormalClient: plclic CSocketV
{

plclic:
	CNormalClient() {
		m_Data = s_n++;
		m_nDataLen = 0;
		m_pHdl = NULL;
	}

	~CNormalClient() {
		m_pHdl = NULL;
	}

protected:

	virtual bool OnConnected(APF_ERROR nErrCode) {
		if(0 == nErrCode) {
			
			m_pHdl = AllocateHdl(false, (char*)&m_Data, sizeof(m_Data), NULL);
			ASSERT(NULL != m_pHdl);

			GetPeer(&m_PeerIp, &m_PeerPort);
			TRACE0("client[%s] writing \"%d\" to [%s:%d]...\n", m_name, m_Data, in_ntoa(m_PeerIp), m_PeerPort);

			Write(m_pHdl, &m_Req);	

			return true;
		}
		else { 
			TRACE0("ERROR: client[%s] onConnnected() err=%d\n", m_name, nErrCode);
			return false;
		}
	}

	virtual void OnWritten(APF_ERROR nErrCode, NetRequest* pReq) {
		if(0 == nErrCode) {
			ChainFreeHdl(m_pHdl, NULL);
			m_pHdl = NULL;
			ASSERT(4 == pReq->xfered);
			GetPeer(&m_PeerIp, &m_PeerPort);
			TRACE0("client[%s] have writen \"%d\" to [%s:%d].\n", m_name, m_Data, in_ntoa(m_PeerIp), m_PeerPort);
			Read((void*)&m_Read, 4, &m_Req);
		}
		else {
			TRACE0("ERROR: client[%s] onWriten() err=%d.\n", m_name, nErrCode);
		}
	}

	virtual void OnRead(APF_ERROR nErrCode, NetRequest* pReq) {
		if(0 == nErrCode) {
			ASSERT(4 == pReq->xfered);
			GetPeer(&m_PeerIp, &m_PeerPort);
			TRACE0("client[%s] read \"%d\" from [%s:%d].\n", m_name, m_Read, in_ntoa(m_PeerIp), m_PeerPort);
			
			ASSERT(m_Read == m_Data + 1);
		}
		else {
			TRACE0("ERROR: client[%s] onRead() err=%d.\n", m_name, nErrCode);
		}
		s_Cond.Signal(); 
	}

    virtual void OnBroken(APF_ERROR nErrCode) {
		TRACE0("client[%s] OnBroken() err=%d.\n", m_name, nErrCode);
    }

private:
	int m_nDataLen;
	int m_Data;
	int m_Read;
	BufHandle* m_pHdl;
	uint32 m_PeerIp;
	uint16 m_PeerPort;
};



static void* NormalClient2Proc(void* arg)
{
	const int nClientNum = 50;
	
	
	
		s_Cond.Init(nClientNum);
	
		CNormalClient *pClient[nClientNum];	
		for(int i = 0; i < nClientNum; ++i) {
			TRACE0("i = %d\n", i);
			pClient[i] = new CNormalClient;
			pClient[i]->Create(true, 0, LOCAL_HOST_IP, true);
			pClient[i]->Connect(LOCAL_HOST_IP, 9100); 
		}

		s_Cond.Wait();
	

	return NULL;
}


TEST_F(SocketTest, Case6_A_Lager_Scale_Read_and_Write_test)
{
	s_bStartKylin = false;
	
	thread_t tid;
	thread_create(&tid, NULL, NormalClient2Proc, NULL);

	thread_join(tid, NULL); 

	s_bStopKylin = true;
}


#undef EXEC_TEST_WORKERS
#undef NET_TEST_WORKERS
#undef LOCAL_HOST_IP 
#undef VERIFY_POOL_STATE
