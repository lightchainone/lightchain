

#ifndef  __NETCPAPPENDERSVR_H_
#define  __NETCPAPPENDERSVR_H_

#include <sys/uio.h>
#include "connectpool.h"
#include "comlog/sendsvr/sendsvr.h"

namespace comspace
{
class NetcpAppenderSvr : plclic SendSvr
{
plclic:
	struct iovec _iovecs[MAXREADITEMS+1];
#ifdef _XLOG_MEMPOOL_USED_
	comspace::MemPool _mempool;
#endif
plclic:
	int push(vptr_t);
	int run();
	~NetcpAppenderSvr();
	
	
	static int setMaxRetry(int maxRetry);
	
	
	
	
	static int setSwitch(int n);
	
	
	
	
	static int setSwitchTime(int s);
protected:
	static int maxretry;
	static int _svrswitch;
	static int _svrswitchtime;
	int _total_sent;
	time_t _last_switch;
	NetcpAppenderSvr();
	int initServer(const char *iplist);
	int fetchSocket();
	int freeSocket(bool err, int sent = 0);
	int _sock;

	ConnectPool cpool;
	typedef ConnectPool::server_conf_t server_conf_t;
	server_conf_t _svrconf[COM_MAXDUPSERVER];
	int my_magic;
private:
	int synchandle(int);
	int senddata(int, int);
	

	bool reopens(server_t *svr, int items);
	void clearsock(int i);
plclic:
	static SendSvr *getServer(const char *iplist);
	friend class NetcpAppender;
};
}
#endif  


