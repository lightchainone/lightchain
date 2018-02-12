

#ifndef  __APPENDER_NETAPPENDER_H_
#define  __APPENDER_NETAPPENDER_H_

#include "comlog/comlogsvr.h"
#include "comlog/sendsvr/loghead.h"

#include "comlog/appender/appender.h"

namespace comspace
{

class SendSvr;

class NetAppender : plclic Appender
{
	static const int NETTALKTIMEOUT = 1000;
plclic:
	struct netdata_t
	{
		Appender *appender;
		logitem_t item;
	};

	//向网络发送，需要四字节对齐
#pragma pack(push, 4)
	struct req_t
	{
		loghead_t loghead;
		logsvr_req_t req;
	};
	struct res_t
	{
		loghead_t loghead;
		logsvr_res_t res;
	};
#pragma pack(pop)
plclic:
	//这个是单线程发送的数据 
	req_t _req;
	res_t _res;
	SendSvr *_sendsvr;

plclic:
	int open(void *);
	int close(void *);
	int print(Event *evt);
	int binprint(void *, int siz);
	int stop();

	~NetAppender();
protected:
	int syncid(void *);
	NetAppender();

plclic:
	static Appender *getAppender(com_device_t &dev);
	static Appender *tryAppender(com_device_t &dev);
protected:
	friend class Appender;
	//friend class UlNetAppender;
	friend class UlLogAppender;
};

};
#endif  //__APPENDER/NETAPPENDER_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
