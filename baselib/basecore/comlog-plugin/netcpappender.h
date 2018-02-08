

#ifndef  __NETCPAPPENDER_H_
#define  __NETCPAPPENDER_H_

#include "comlog/comlogsvr.h"
#include "comlog/sendsvr/loghead.h"

#include "comlog/appender/appender.h"

namespace comspace
{

class SendSvr;


#define NETCP_TRUE "TRUE"		
#define NETCP_FALSE "FALSE"		

class NetcpAppender : plclic Appender
{
	static const int NETTALKTIMEOUT = 1000;
plclic:
	struct netdata_t
	{
		Appender *appender;
		logitem_t item;
	};

	
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
	
	req_t _req;
	res_t _res;
	SendSvr *_sendsvr;

plclic:
	int open(void *);
	int close(void *);
	int print(Event *evt);
	int binprint(void *, int siz);
	bool diskBackup();
	int stop();
	~NetcpAppender();
protected:
	bool disk_backup;
	int syncid(void *);
	NetcpAppender();

plclic:
	static Appender *getAppender(com_device_t &dev);
	static Appender *tryAppender(com_device_t &dev);
protected:
	friend class Appender;
	
	friend class UlLogAppender;
};

};
#endif  


