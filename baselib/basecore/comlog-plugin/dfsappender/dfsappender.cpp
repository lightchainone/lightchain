
#include <ul_ullib.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <comlog/comlog.h>
#include <comlog/layout.h>
#include <comlog/event.h>
#include <comlog/namemg.h>
#include <comlog/xutils.h>
#include <comlog/xdebug.h>
#include <comlog/sendsvr/loghead.h>
#include <comlog/appender/fileappender.h>

#include "dfsappender_utils.h"
#include "dfsappender_disklog.h"

#include "dfsappender.h"
#include "dfsappendersvr.h"


namespace comspace
{

using namespace comspace::comlogplugin::dfsappender;
const char DFSAPPENDERPORT[] = "9898";        
const char DFSAPPENDERIP[] = "127.0.0.1";         

int DfsAppender::open(void *host)
{
	
	if (host == NULL) {
        
        if (!_device.open) {
            return 0;
        }
        
        
        if (get_dev_confnstr(&_device, "PORT", this->_port, sizeof(this->_port)) < 0) {
            snprintf(this->_port, sizeof(this->_port), "%s", DFSAPPENDERPORT);
        }

        snprintf(_device.host, sizeof(_device.host), "127.0.0.1:%s", this->_port);
        _com_debug("Appender is %p _sendsvr is %p", this, _sendsvr);

        get_dev_confnstr(&_device, "USER", this->_user, sizeof(this->_user));
        get_dev_confnstr(&_device, "PASSWD", this->_passwd, sizeof(this->_passwd));
        get_dev_confnstr(&_device, "PATH", this->_path, sizeof(this->_path));
        get_dev_confnstr(&_device, "NAME", this->_file, sizeof(this->_file));

        get_dev_confnstr(&_device, "MASTER", this->_master, sizeof(this->_master));
        get_dev_confnstr(&_device, "CHARSET", this->_charset, sizeof(this->_charset));

        get_dev_confnstr(&_device, "DATA_FMT", this->_data_fmt, sizeof(this->_data_fmt));
        get_dev_confnstr(&_device, "DFSCOMPRESS", this->_dfscompress, sizeof(this->_dfscompress));
        get_dev_confnstr(&_device, "DFSCOMPRESS_ARG", this->_dfscompress_arg, sizeof(this->_dfscompress_arg));


        
        get_dev_confnstr(&_device, "LOGBAKPATH", _disklog.path, sizeof(_disklog.path));
        get_dev_confnstr(&_device, "LOGBAKFILE", _disklog.file, sizeof(_disklog.file));

        uint32_t max_size;
        if (get_dev_confuint32(&_device, "LOGBAKMAX_SIZE", &max_size) < 0) {
            max_size = 200;
        } 
        _disklog.max_size = max_size;
        _disklog.max_size *= 1048576; 

        if (get_dev_confuint32(&_device, "LOGQUEUE", &_disklog.queue_size) < 0) {
            _disklog.queue_size = 20*1024; 
        }
        if (_disklog.queue_size < 2) {
            _disklog.queue_size = 2;
        }
        if (get_dev_confuint32(&_device, "REMOVE", &_disklog.remove) < 0) {
            _disklog.remove = 1;
        }
        char dfspath[COMLOG_PROXY_MODULE_MAXLEN];
        
        if (ul_fullpath(this->_path, this->_file, dfspath, sizeof(dfspath)) < 0) {
            _com_error("path[%s] file[%s] too long", 
                    this->_path, this->_file);
            return -1;
        }
        
        normalize_file(dfspath);
        
        handle_sign(dfspath, &(this->_handle));

		_core_debug("sendsvr[%lx] _layout[%lx] handle [%llu, %llu]", _sendsvr, _layout,
                _handle.high, _handle.low);
		
		_sendsvr = DfsAppenderSvr::getServer(_device.host, &_disklog);
		if (_sendsvr == NULL || _sendsvr->registAppender(this) != 0) {
			_com_error("sorry get server err[%s] or regist appender err", _device.host);
			return -1;
		}
        _com_debug("get sendserver[%s]", _device.host);
        return 0;
	} else {
        
        int sd = -1;
        if (_sendsvr->checkalive(200) < 0) {
            return -1;
        }
        sd = ((SendSvr::server_t *)host)->sock;
        int on = 1;
        
        if (setsockopt(sd,IPPROTO_TCP,TCP_NODELAY,&on, sizeof(on)) < 0) {
            _com_error("set NODELAY fail");
            ::close(sd);
            return -1;
        }

        _req.loghead.id = 0;
        _req.loghead.version = COMLOG_PROXY_VERSION;
        snprintf(_req.loghead.provider, sizeof(_req.loghead.provider),
                "%s", "dfsappender");
        memset(&_req.loghead, 0, sizeof(_req.loghead));
        _req.loghead.log_id  = (int)pthread_self();
        _req.loghead.reserved = 0;
        _req.loghead.body_len = sizeof(_req.req.create);
        _com_debug("body_len is %d", _req.loghead.body_len);
        memset(&(_req.req.create), 0, sizeof(comlog_proxy_create_req_t));
        
		snprintf(_req.req.create.filename, sizeof(_req.req.create.filename), "%s", this->_file);
		snprintf(_req.req.create.path, sizeof(_req.req.create.path), "%s", this->_path);
        snprintf(_req.req.create.master, sizeof(_req.req.create.master), "%s", this->_master);
        snprintf(_req.req.create.user, sizeof(_req.req.create.user), "%s", this->_user);
        snprintf(_req.req.create.passwd, sizeof(_req.req.create.passwd), "%s", this->_passwd);
        snprintf(_req.req.create.dfscompress, sizeof(_req.req.create.dfscompress), "%s",
                this->_dfscompress);
        snprintf(_req.req.create.dfscompress_arg, sizeof(_req.req.create.dfscompress_arg), "%s",
                this->_dfscompress_arg);
        snprintf(_req.req.create.charset, sizeof(_req.req.create.charset), "%s", 
                this->_charset);
        snprintf(_req.req.create.fmt, sizeof(_req.req.create.fmt), "%s",
                this->_data_fmt);
        _req.req.create.dfs_splite_type = _device.splite_type;
        if (_device.cuttime <= 0) {
            _req.req.create.dfs_cuttime = INT_MAX;
        } else {
            _req.req.create.dfs_cuttime = _device.cuttime * 60; 
        }
        
        _req.req.create.cmd = COMLOG_PROXY_CREATE; 
		_com_debug("create fn[%s] mod[%s] sd[%d]",  _req.req.create.filename, _req.req.create.path, sd);
		int ret = loghead_write(sd, (loghead_t *)&_req, NETTALKTIMEOUT);
		if (ret != 0) {
			this->logerror("open:write dfsappend fd[%d] err[%m]", sd);
			return -1;
		}
		ret = loghead_read(sd, (loghead_t *)&_res, sizeof(_res), NETTALKTIMEOUT);
		if (_res.loghead.body_len < sizeof(_res.res)) {
			this->logerror("open:read dfsappend fd[%d] err[%m]", sd);
			return -1;
		}
        
        _com_debug("recv size is %d", _res.loghead.body_len);
		if (_res.res.err_no != 0) {
			this->logerror("open:create log err[%s %s] errno[%d] [%.*s]",
                    _device.auth,
                    _device.file, 
                    _res.res.err_no,
                    _res.res.err_len, _res.res.err_info);
			return -1;
		}
		this->_handle = _res.res.handle;
        
        _id = 0;
		_com_debug("get handle [%llu %llu]", this->_handle.high, this->_handle.low);
	}
	return 0;
}

int DfsAppender::close(void *)
{
    _id = -1;
	return 0;
}

int DfsAppender::stop(){
	if(_sendsvr){
		_sendsvr->stop();
	}
	return 0;
}

int DfsAppender::logerror(const char*fmt, ...)
{
    char buff[LOG_BUFF_SIZE_EX];
    va_list args;
    if (this->_bkappender) {
        char now[20];
        time_t tt;
        struct tm vtm; 
        time(&tt);
        localtime_r(&tt, &vtm);
        snprintf(now, sizeof(now), "%02d-%02d %02d:%02d:%02d",
                vtm.tm_mon+1, vtm.tm_mday, vtm.tm_hour, vtm.tm_min, vtm.tm_sec);
        int ret =
            snprintf(buff, sizeof(buff), "WARNING: %s:  %s * %lu ",
                    now, get_proc_name(),  pthread_self());

        va_start(args, fmt);
        int pos = vsnprintf(buff+ret, sizeof(buff)-ret, fmt, args);
        va_end(args);
        if (pos + ret != sizeof(buff)) {
            buff[pos+ret] = '\n';
            buff[pos+ret+1] = '\0';
            ++pos;
        } else {
            buff[pos+ret-1] = '\n';
        }
        this->_bkappender->binprint(buff, ret+pos);
    }
    return 0;
}

int DfsAppender::print(Event *evt)
{
	_core_debug("sendsvr[%lx] _layout[%lx] def[%lx]", _sendsvr, _layout, &g_def_layout);
	
	if (unInMask(evt->_log_level)) {
		
		return 0;
	}
	if (_sendsvr == NULL) {
		_sendsvr = DfsAppenderSvr::getServer(_device.host,
                &_disklog);
	}
	if (_sendsvr == NULL) {
		_com_error("can't get sendsever");
		return -1;
	}
	_layout->format(evt);

	_com_debug("dfsappender %s", evt->_render_msgbuf);
	evt->_nowapp = this;
	int ret = -1;
    
    ret = _sendsvr->push(evt);

	if (ret != 0) {
		_core_debug("faint");
    }
	return ret;
}

int DfsAppender::binprint(void *, int )
{
	_com_error("DfsAppender::binprint() is not implemented.");
	return 0;
}

int DfsAppender::syncid(void *)
{
	return 0;
}

DfsAppender::DfsAppender()
{
	_sendsvr = NULL;
    memset(&_req, 0, sizeof(_req));
}

DfsAppender::~DfsAppender(){
	close(NULL);
}

Appender * DfsAppender::getAppender(com_device_t &dev)
{
	char name[1024];
	snprintf(name, sizeof(name), "DFSPROXY:%s/%s", dev.host, dev.file);
	Appender *app = (Appender *)comlog_get_nameMg()->get(type_appender, name);
	if (app == NULL) {
		_com_debug("create dfsappender %s", name);
		app = new DfsAppender();
		app->setDeviceInfo(dev);
		_com_debug("try to open the dfsappender[%s] [%p]", name, app);
		if (app->open(NULL) != 0) {
			_com_error("sorry fail to open %s", name);
			delete app;
			return NULL;
		}
        com_device_t dev2;
        ul_strlcpy(dev2.type, "FILE", sizeof(dev2.type));
        dev2.open = dev.open;
        dev2.log_size = 2048;
        dev2.splite_type = COM_TRUNCT;
        dev2.log_mask = dev.log_mask;
        char file[WORD_SIZE];
        char path[WORD_SIZE];
        get_dev_confnstr(&dev, "WARNINGLOG_FILE", file, sizeof(file));
        get_dev_confnstr(&dev, "WARNINGLOG_PATH", path, sizeof(path));
        app->_bkappender = NULL;
        if (file[0] != '\0' && path[0] != '\0') {
            snprintf(dev2.file, sizeof(dev2.file), "%s", file);
            snprintf(dev2.host, sizeof(dev2.host), "%s", path);
            _com_debug("open for waring log [%s]", dev2.file);
            
            app->_bkappender = FileAppender::getAppender(dev2);
            if (app->_bkappender != NULL) { 
                app->_bkappender->setLayout(NULL);
                app->_bkappender->_open = dev2.open;
            }       
        }
        comlog_get_nameMg()->set(name, app);
	}
	return app;
}

Appender * DfsAppender::tryAppender(com_device_t &dev)
{
	char name[1024];
	snprintf(name, sizeof(name), "DFSPROXY:%s/%s", dev.host, dev.file);
	Appender *app = (Appender *)comlog_get_nameMg()->get(type_appender, name);
	return app;
}




} 


