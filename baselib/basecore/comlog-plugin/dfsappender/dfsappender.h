

#ifndef  __APPENDER_NETAPPENDER_H_
#define  __APPENDER_NETAPPENDER_H_

#include <comlog/comlogsvr.h>
#include <comlog/sendsvr/loghead.h>

#include <comlog/appender/appender.h>


#include "comlogproxy_if.h"
#include "dfsappender_disklog.h"

namespace comspace
{

class SendSvr;

class DfsAppender : plclic Appender
{
	static const int NETTALKTIMEOUT = 1000;
    static const size_t SEND_MAX_SIZE_MIN = 256;
plclic:

#pragma pack(push, 4)
	struct netdata_t
	{
        comspace::comlogplugin::dfsappender::comlog_proxy_print_req_t item;
	};

	struct req_t
	{
		loghead_t loghead;
        union {
            comspace::comlogplugin::dfsappender::comlog_proxy_create_req_t create;
        } req;
	};
	struct res_t
	{
		loghead_t loghead;
        comspace::comlogplugin::dfsappender::comlog_proxy_res_t	res;
	};
#pragma pack(pop)
plclic:
	
    req_t _req;
	res_t _res;
	SendSvr *_sendsvr;
    comspace::comlogplugin::dfsappender::disk_log_conf_t _disklog;
    comspace::comlogplugin::dfsappender::comlogproxy_handle_t _handle;

plclic:
	int open(void *);
	int close(void *);
	int print(Event *evt);
	int binprint(void *, int siz);
	int stop();

    int logerror(const char*fmt, ...);
	~DfsAppender();
protected:
	int syncid(void *);
	DfsAppender();
    char _user[comspace::comlogplugin::dfsappender::COMLOG_PROXY_USER_SIZE];
    char _port[20];
    char _passwd[comspace::comlogplugin::dfsappender::COMLOG_PROXY_PASSWD_SIZE];
    char _path[comspace::comlogplugin::dfsappender::COMLOG_PROXY_MODULE_MAXLEN];
    char _file[comspace::comlogplugin::dfsappender::COMLOG_PROXY_FILENAME_MAXLEN];
    char _master[comspace::comlogplugin::dfsappender::COMLOG_PROXY_MASTER_MAXLEN];
    char _charset[comspace::comlogplugin::dfsappender::COMLOG_PROXY_CHARSET];
    char _data_fmt[WORD_SIZE];
    char _dfscompress[comspace::comlogplugin::dfsappender::COMLOG_PROXY_COMPRESS_SIZE];
    char _dfscompress_arg[comspace::comlogplugin::dfsappender::COMLOG_PROXY_COMPRESS_ARG_SIZE];


plclic:
	static Appender *getAppender(com_device_t &dev);
	static Appender *tryAppender(com_device_t &dev);
protected:
	friend class Appender;
	
};

};
#endif  


