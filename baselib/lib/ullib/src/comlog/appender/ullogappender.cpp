
#include <stdio.h>

#include "ullogappender.h"
#include "fileappender.h"
#include "netappender.h"
#include "../xdebug.h"
#include "../namemg.h"
#include "../event.h"

namespace comspace
{

UlLogAppender::UlLogAppender()
{
	_wfapp = NULL;
	_ntapp = NULL;
	_sfapp = NULL;
}

int UlLogAppender::open(void *) 
{
#if 0
	if (_wfapp) {
		if (_wfapp->open(NULL) != 0) {
			_com_error("open wfapp err, dev[%s][%s]", dev.host, dev.file);
			return -1;
		}
	}

	if (_ntapp) {
		if (_ntapp->open(NULL) != 0) {
			_com_error("open ntapp err, dev[%s][%s]", dev.host, dev.file);
			return -1;
		}
	}

	if (_sfapp) {
		if (_sfapp->open(NULL) != 0) {
			_com_error("open sfapp err, dev[%s][%s]", dev.host, dev.file);
			return -1;
		}
	}
#endif
	return 0;
};

int UlLogAppender::close(void *)
{
#if 0
	if (_wfapp) _wfapp->close(NULL);
	if (_ntapp) _ntapp->close(NULL);
	if (_sfapp) _sfapp->close(NULL);
#endif
	return 0;
}

int UlLogAppender::print(Event *evt)
{
	if (unInMask(evt->_log_level)) {
		return 0;
	}
	if (_ntapp) {
		if (_ntapp->print(evt) != 0) {
			return -1;
		}
	}
	if (_wfapp) {
		if (_wfapp->print(evt) != 0) {
			return -1;
		}
	}

	if (_sfapp) {
		if (_sfapp->print(evt) != 0) {
			return -1;
		}
	}

	return 0;
}

int UlLogAppender::binprint(void *val, int siz)
{
	if (_ntapp) {
		if (_ntapp->binprint(val, siz) != 0) {
			return -1;
		}
	}
	if (_wfapp) {
		if (_wfapp->binprint(val, siz) != 0) {
			return -1;
		}
	}
	if (_sfapp) {
		if (_sfapp->binprint(val, siz) != 0) {
			return -1;
		}
	}
	return 0;
}

int UlLogAppender::syncid(void *)
{
#if 0
	if (_ntapp) {
		if (_ntapp->syncid(NULL) != 0) {
			return -1;
		}
	}
	if (_wfapp) {
		if (_ntapp->syncid(NULL) != 0) {
			return -1;
		}
	}
	if (_sfapp) {
		if (_sfapp->syncid(NULL) != 0) {
			return -1;
		}
	}
#endif
	return 0;
}

#if 0
class UlFileAppender : FileAppender
{
plclic:
	static Appender * getAppender(com_device_t &dev) 
	{
		_com_debug("wanna get ulfile appender");

		char name[1024];
		snprintf(name, sizeof(name), "ULFILE:%s/%s", dev.host, dev.file);
		Appender *app = (Appender *)comlog_get_nameMg()->get(type_appender, name);
		if (app == NULL) {
			app = new FileAppender;
			app->setDeviceInfo(dev);
			if (app->syncid(NULL) != 0) {
				_com_error("syncid log[%s] err %m", name);
				delete app;
				return NULL;
			}
			comlog_get_nameMg()->set(name, app);
		}
		_com_debug("get file appender");
		return app;
	}
};

class UlNetAppender : NetAppender
{	
plclic:
	static Appender * getAppender(com_device_t &dev)
	{
		char name[1024];
		snprintf(name, sizeof(name), "ULNET:%s/%s", dev.host, dev.file);
		Appender *app = (Appender *)comlog_get_nameMg()->get(type_appender, name);
		if (app == NULL) {
			_com_debug("create net appender %s", name);
			app = new NetAppender;
			app->setDeviceInfo(dev);
			_com_debug("try to open the netappender[%s]", name);
			if (app->open(NULL) != 0) {
				_com_error("sorry failt to open %s", name);
				delete app;
				return NULL;
			}
			comlog_get_nameMg()->set(name, app);
		}
		return app;
	}
};
#endif

Appender * UlLogAppender::getAppender(com_device_t &dev)
{
	char name[1024];
	snprintf(name, sizeof(name), "ULLOG[%s]:%s/%s", dev.type, dev.host, dev.file);
	UlLogAppender *app = (UlLogAppender *)comlog_get_nameMg()->get(type_appender, name);
	if (app == NULL) {
		app = new UlLogAppender;
		app->setDeviceInfo(dev);
		com_device_t tmpd = dev;

		//notice
		tmpd.log_mask = 0;
		COMLOG_ADDMASK(tmpd, COMLOG_NOTICE);
		COMLOG_ADDMASK(tmpd, COMLOG_TRACE);
		COMLOG_ADDMASK(tmpd, COMLOG_DEBUG);
		if (strcmp(dev.type, "ULLOG") == 0) {
			//app->_ntapp = UlFileAppender::getAppender(tmpd);
			app->_ntapp = FileAppender::getAppender(tmpd);
		} else {
			//app->_ntapp = UlNetAppender::getAppender(tmpd);
			app->_ntapp = NetAppender::getAppender(tmpd);
		}
		if (app->_ntapp == NULL) {
			delete app;
			_com_error("get ntapp err");
			return NULL;
		}
		app->_ntapp->setLayout(dev.layout);

		tmpd.log_mask = 0;
		snprintf(tmpd.file, sizeof(tmpd.file), "%s.wf", dev.file);
		COMLOG_ADDMASK(tmpd, COMLOG_FATAL);
		COMLOG_ADDMASK(tmpd, COMLOG_WARNING);
		if (strcmp(dev.type, "ULLOG") == 0) {
			//app->_wfapp = UlFileAppender::getAppender(tmpd);
			app->_wfapp = FileAppender::getAppender(tmpd);
		} else {
			//app->_wfapp = UlNetAppender::getAppender(tmpd);
			app->_wfapp = NetAppender::getAppender(tmpd);
		}
		if (app->_wfapp == NULL) {
			delete app;
			_com_error("get wfapp error");
			return NULL;
		}
		app->_wfapp->setLayout(dev.layout);

		tmpd.log_mask = (dev.log_mask >> 32) << 32;
		if (tmpd.log_mask != 0) {
			snprintf(tmpd.file, sizeof(tmpd.file), "%s.sf", dev.file);
			if (strcmp(dev.type, "ULLOG") == 0) {
				//app->_sfapp = UlFileAppender::getAppender(tmpd);
				app->_sfapp = FileAppender::getAppender(tmpd);
			} else {
				//app->_sfapp = UlNetAppender::getAppender(tmpd);
				app->_sfapp = NetAppender::getAppender(tmpd);
			}
			if (app->_sfapp == NULL) {
				delete app;
				_com_error("get sfapp error");
				return NULL;
			}
			app->_sfapp->setLayout(dev.layout);
		}

		comlog_get_nameMg()->set(name, app);
	}

	return app;
}

Appender * UlLogAppender::tryAppender(com_device_t &dev)
{
	char name[1024];
	snprintf(name, sizeof(name), "ULLOG[%s]:%s/%s", dev.type, dev.host, dev.file);
	UlLogAppender *app = (UlLogAppender *)comlog_get_nameMg()->get(type_appender, name);
	return app;
}

}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
