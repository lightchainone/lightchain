#ifndef _EPOLLER_H_
#define _EPOLLER_H_

#include "support.h"
#include "NetworkMan.h"
#include "CompQ.h"
#include "KylinNS.h"

_KYLIN_PACKAGE_BEGIN_

struct NetContext;
struct NetRequest;
class CEPoller
{
	int m_fd;
	uint64 m_nRead, m_nWrite;
    typedef TMultiMap<uint64, NetContext> DelayedList;
    DelayedList m_DelayedList;
    CSpinLock m_DListLock;

	bool StartWrite(NetContext* pCtx, bool bForce);
	bool StartRead(NetContext* pCtx, bool bUser);
    void DoWithDelayed();
    bool DoConnect(NetContext* pCtx);

plclic:
	CEPoller() { m_fd = 0; m_nRead = m_nWrite = 0; }
	~CEPoller() { Close(); }
	
	APF_ERROR Create(int size);
	void Close();
	
	APF_ERROR Associate(NetContext* pCtx);
	APF_ERROR Deassociate(NetContext* pCtx);
	APF_ERROR Run();
	void GetXfered(uint64* pnRead, uint64* pnWrite, bool bReset);

    bool CancelDelay(NetContext* pCtx);
	void Delay(NetContext* pCtx, uint32 nMilliseconds);
	void Read(NetContext* pCtx, NetRequest* pReq);
	void Write(NetContext* pCtx, NetRequest* pReq);
};

_KYLIN_PACKAGE_END_

#endif
