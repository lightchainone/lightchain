

#ifndef  __SENDSVR_NETAPPENDERSVR_H_
#define  __SENDSVR_NETAPPENDERSVR_H_

#include <sys/uio.h>
#ifdef _XLOG_MEMPOOL_USED_
#include <comlog/comempool/mempool.h>
#endif

#include <comlog/sendsvr/sendsvr.h>
#include "comlogproxy_if.h"
#include "dfsappender_disklog.h"

namespace comspace
{
class DfsAppenderSvr : plclic SendSvr
{
plclic:
	int push(vptr_t);
	int run();
	~DfsAppenderSvr();
    int queue_size;
protected:
	DfsAppenderSvr();
	int initServer(const char *iplist);
private:
    static const int SNED_BUFF_MAX_SIZE = 512*1024;
    static const int MAX_SEND_ITEMS = 1024;
	int synchandle(int);
	int senddata(struct iovec *vec, int count);
	bool reopens(server_t *svr, int items);
	void clearsock(int i);
    int next_iovecs();

	int _now;
	struct iovec _iovecs[MAX_SEND_ITEMS];
    int _iosiz;
    int _log_id;
    vptr_t _readbuf[MAX_SEND_ITEMS-16];
    int _cur_item;
    int _readbufnum;
    size_t _sendbuf_size;
    int _disk_buff_size;
    int _disk_least_buff_size;
    int _disk_cur_pos;

    
    char _sendbuf[SNED_BUFF_MAX_SIZE];
plclic:

    int createQueue(uint32_t queue_size);
    int createServer(const char *ip);
    comspace::comlogplugin::dfsappender::DfsAppenderDiskLog *_diskbaklog;
    comspace::comlogplugin::dfsappender::DfsAppenderDiskLog _disklog;
	static SendSvr *getServer(const char *iplist,
            comspace::comlogplugin::dfsappender::disk_log_conf_t *disklog);
    char _bkpath[1024];
    char _bkfile[1024];
    int64_t _bklimit;
    uint32_t _bkremove;


};
}
#endif  


