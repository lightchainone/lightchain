

#include "netcpappender.h"
#include "netcpappendersvr.h"

#include "appender/fileappender.h"
#include "layout.h"
#include "event.h"
#include "namemg.h"
#include "xutils.h"

#include "xdebug.h"

#include "sendsvr/loghead.h"

namespace comspace
{

int NetcpAppender::open(void *host)
{
	
	if (host == NULL) {
		_sendsvr = NetcpAppenderSvr::getServer(_device.host);
		if (_sendsvr == NULL || _sendsvr->registAppender(this) != 0) {
			_com_error("sorry get server err[%s] or regist appender err", _device.host);
			return -1;
		}
		_core_debug("sendsvr[%lx] _layout[%lx]", _sendsvr, _layout);
		_com_debug("get sendserver[%s]", _device.host);
	} else {
		
		_req.loghead.log_id  = 123;
		_req.loghead.body_len = sizeof(_req.req);
		snprintf(_req.req.filename, sizeof(_req.req.filename), "%s", _device.file);
		snprintf(_req.req.auth, sizeof(_req.req.auth), "%s", _device.auth);
		if (_device.compress) {
			_req.req.compress = COMLOG_COMPRESS;
		} else {
			_req.req.compress = COMLOG_UNCOMPRESS;
		}
		_req.loghead.reserved = COMLOG_CREATE;
		
		
		int sd = ((SendSvr::server_t *)host)->sock;
		snprintf(_req.req.path, sizeof(_req.req.path), "%s", ((SendSvr::server_t *)host)->path);

		_com_debug("create auth[%s] fn[%s] mod[%s]", _req.req.auth, _req.req.filename, _req.req.path);
		int ret = loghead_write(sd, (loghead_t *)&_req, NETTALKTIMEOUT);
		if (ret != 0) {
			_com_error("open:write netappend fd[%d] err[%m]", sd);
			return -1;
		}
		ret = loghead_read(sd, (loghead_t *)&_res, sizeof(_res), NETTALKTIMEOUT);
		if (ret != 0) {
			_com_error("open:read netappend fd[%d] err[%m]", sd);
			return -1;
		}

		if (_res.res.error_code != 0) {
			_com_error("open log err[%s %s] errno[%d]", _device.auth, _device.file, _res.res.error_code);
			return -1;
		}
		_id = _res.res.handle;
		_com_debug("get handle id[%d]", _id);
		
		
		
	}
	return 0;
}

int NetcpAppender::close(void *)
{
	_id = -1;
	return 0;
}

int NetcpAppender::stop(){
	if(_sendsvr){
		_sendsvr->stop();
	}
	return 0;
}

int NetcpAppender::print(Event *evt)
{
	
	
	if (unInMask(evt->_log_level)) {
		
		return 0;
	}
	if (_sendsvr == NULL) {
		_sendsvr = NetcpAppenderSvr::getServer(_device.host);
	}
	if (_sendsvr == NULL) {
		
		return -1;
	}
	_layout->format(evt);

	
	evt->_nowapp = this;
	int ret = _sendsvr->push(evt);
	if (ret != 0 && _bkappender && diskBackup()) {
		_core_debug("faint");
		ret = _bkappender->print(evt);
	} else if (ret != 0) {
		
	}
	return ret;
}

int NetcpAppender::binprint(void *, int siz)
{
	_com_error("NetcpAppender::binprint() is not implemented.");
	return 0;
}

int NetcpAppender::syncid(void *)
{
	return 0;
}

bool NetcpAppender::diskBackup(){
	return disk_backup;
}

NetcpAppender::NetcpAppender()
{
	_sendsvr = NULL;
	disk_backup = true;
}

NetcpAppender::~NetcpAppender(){
	close(NULL);
}

Appender * NetcpAppender::getAppender(com_device_t &dev)
{
	char name[1024];
	snprintf(name, sizeof(name), "NETCP:%s/%s", dev.host, dev.file);
	Appender *app = (Appender *)comlog_get_nameMg()->get(type_appender, name);
	if (app == NULL) {
		_com_debug("create net appender %s", name);
		app = new NetcpAppender;
		app->setDeviceInfo(dev);
		if(strstr(dev.reserved1, NETCP_FALSE) != NULL){
			((NetcpAppender *)app)->disk_backup = false;
		}
		_com_debug("try to open the netappender[%s]", name);
		if (app->open(NULL) != 0) {
			_com_error("sorry failt to open %s", name);
			delete app;
			return NULL;
		}
		com_device_t dev2;
		if (deviceNetToFile(dev, dev2) == 0) {
			app->_bkappender = FileAppender::getAppender(dev2);
			
			
			if (app->_bkappender != NULL) {
				app->_bkappender->setLayout(dev2.layout);
				app->_bkappender->_open = dev2.open;
			}
		}
		comlog_get_nameMg()->set(name, app);
	}
	return app;
}

Appender * NetcpAppender::tryAppender(com_device_t &dev)
{
	char name[1024];
	snprintf(name, sizeof(name), "NETCP:%s/%s", dev.host, dev.file);
	Appender *app = (Appender *)comlog_get_nameMg()->get(type_appender, name);
	return app;
}
}

