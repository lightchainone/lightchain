#ifndef _SOCKET_H_
#define _SOCKET_H_

#include "Async.h"
#include "NetworkMan.h"



_KYLIN_PACKAGE_BEGIN_

enum SockType {
	ST_NONE = 0,
	ST_CREATED,
	ST_LISTEN,
	ST_CONNECTED,
	ST_ACCEPTED,
	ST_ERROR
};

class CSocketV : plclic CAsyncClient
{
protected:
	SOCKET m_s;
	uint16 m_port;
	NetContext m_Ctx;
	NetRequest m_Req;
	volatile int m_type;
    AsyncContext* m_pConnCtx;
 
	CSocketV() {
		m_s = INVALID_SOCKET; 
		m_port = 0;
		m_type = ST_NONE;
        m_pConnCtx = NULL;
        InitAsyncContext(&m_Req.async);
	}
	virtual ~CSocketV() {
		Close();
	}

	virtual void OnCompletion(AsyncContext* pContext);

	
	virtual bool OnConnected(APF_ERROR nErrCode) { return 0==nErrCode; }
	virtual void OnListened(SOCKET s, SOCKADDR_IN* ) { closesocket(s); }
	
	virtual void OnWritten(APF_ERROR , NetRequest* ) {}
	virtual void OnRead(APF_ERROR , NetRequest* ) {}

	virtual void OnBroken(APF_ERROR nErrCode) = 0;
    bool Close();

plclic:
	virtual int Release() {
		int n = CAsyncClient::Release();
		if (n == 0) {
			delete this;
		}
		return n;
	}

	uint16 GetPort() const { return m_port; }
	void GetPeer(OPTIONAL OUT uint32 *pip=NULL, OPTIONAL OUT uint16* pport=NULL);
    void GetLocal(OPTIONAL OUT uint32 *pip=NULL, OPTIONAL OUT uint16* pport=NULL);
	
	bool IsCreated() { int n = AtomicGetValue(m_type); return n!=ST_NONE && n!=ST_ERROR; }
    bool IsBorken() { return ST_ERROR == AtomicGetValue(m_type); }
	SOCKET GetSocket() const { return m_s; }

	APF_ERROR Create(bool bTCP, uint16 port=0, uint32 ip=0, uint32 flag=0); 
	APF_ERROR Create(SOCKET s, uint16 port, uint32 flag=0);
	
	void Listen(int backlog=64);
	void Connect(uint32 ip, uint16 port, uint32 timeout=INFINITE);
	void ConnectAndWrite(uint32 ip, uint16 port, void* pBuf, int count, uint32 timeout=INFINITE);
    void ReConnect(uint32 nMilliseconds);
	bool Shutdown(bool bLingerOff=true);

    void Read(void* pBuf, int count, int request, NetRequest* pReq);
	void Read(void* pBuf, int count, NetRequest* pReq);
	void Write(void* pBuf, int count, NetRequest* pReq);
    void Write(BufHandle* pHdl, NetRequest* pReq);
    void WriteTo(uint32 ip, uint16 port, void* pBuf, int count, NetRequest* pReq);
    void WriteTo(uint32 ip, uint16 port, BufHandle* pHdl, NetRequest* pReq);

#ifdef _UNITTEST	
    NetContext* GetCtx() { return &m_Ctx; } 
#endif	
};

_KYLIN_PACKAGE_END_
#endif
