

#ifndef  __SENDSVR_NETAPPENDERSVR_H_
#define  __SENDSVR_NETAPPENDERSVR_H_

#include <sys/uio.h>
#ifdef _XLOG_MEMPOOL_USED_
#include "comlog/comempool/mempool.h"
#endif

#include "comlog/sendsvr/sendsvr.h"

namespace comspace
{
class NetAppenderSvr : plclic SendSvr
{
plclic:
#if 0
	struct nowsd_t
	{
		int pos;
		int val;
	};

	nowsd_t _nowsd;	//当前有效的句柄
#else
	int _now;
#endif
	struct iovec _iovecs[MAXREADITEMS+1];
#ifdef _XLOG_MEMPOOL_USED_
	comspace::MemPool _mempool;
#endif
plclic:
	int push(vptr_t);
	int run();
	~NetAppenderSvr();
protected:
	NetAppenderSvr();
	int initServer(const char *iplist);
private:
	int synchandle(int);
	int senddata(int);
	//int erasedata(int);

	bool reopens(server_t *svr, int items);
	void clearsock(int i);
plclic:
	static SendSvr *getServer(const char *iplist);
};
}
#endif  //__SENDSVR/NETAPPENDERSVR_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
