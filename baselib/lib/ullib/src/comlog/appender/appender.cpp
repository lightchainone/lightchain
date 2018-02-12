#include "appender.h"
#include "../xutils.h"
#if 0
#include "fileappender.h"
#include "netappender.h"
#include "ttyappender.h"
#include "ullogappender.h"
#include "asyncfileappender.h"
#endif
#include "../layout.h"
#include "../xdebug.h"

namespace comspace
{

Appender::Appender()
{
	_id = -1;
	_layout = comlog_get_def_layout();
	_core_debug("init layout %lx", _layout);
	_mask = 0;
	_open = 1;
	resetMask();
	pthread_mutex_init(&_lock, NULL);
	_bkappender = NULL;
	setLayout(NULL);
}

Appender::~Appender()
{
}

int Appender::setLayout(Layout *layout)
{
	if (layout == NULL) {
		layout = comlog_get_def_layout();
	} else {
		_layout = layout;
	}
	_core_debug("init setlayout %lx", _layout);
	return 0;
}

int Appender::emptyMask()
{
	_mask = 0;
	return 0;
}

int Appender::resetMask()
{
	emptyMask();
	addMask(COMLOG_FATAL);
	addMask(COMLOG_WARNING);
	addMask(COMLOG_NOTICE);
	addMask(COMLOG_TRACE);
	addMask(COMLOG_DEBUG);
	return 0;
}

int Appender::addMask(int id)
{
	_mask = _mask | (unsigned long long)((unsigned long long)(1) << (unsigned long long)(id));
	return 0;
}

int Appender::delMask(int id)
{
	_mask = _mask & (unsigned long long)(~((unsigned long long)((unsigned long long)(1) << (unsigned long long)(id))));
	return 0;
}

bool Appender::unInMask(int id)
{
	return (_mask & (unsigned long long)((unsigned long long)(1) << (unsigned long long)(id))) == (unsigned long long)0;
}

int Appender::syncid(void *)
{
	return 0;
}

int Appender::setDeviceInfo(com_device_t &dev)
{
	_device = dev;
	_mask = dev.log_mask;
	return 0;
}

//static pthread_mutex_t comlog_appender_getlock = PTHREAD_MUTEX_INITIALIZER;

#define __CHECKINT(val, min, max, def) \
{ \
	if ((val) > (max)) { \
		val = (def); \
	} \
	if ((val) < (min)) { \
		val = (def); \
	} \
}

#if 0
Appender * Appender::getAppender(com_device_t &dev)
{
	xAutoLock lock(&comlog_appender_getlock);

	//dev的参数检查
	__CHECKINT(dev.log_size, 1, COM_MAXLOGFILE, COM_MAXLOGFILE);
	
	Appender *app = NULL; 
	switch (dev.type) {
		case COM_FILEDEVICE:
			_com_debug("create file appender");
			app = FileAppender::getAppender(dev);
			break;
		case COM_NETDEVICE:
			_com_debug("create net appender");
			app = NetAppender::getAppender(dev);
			break;
		case COM_TTYDEVICE:
			_com_debug("create tty appender");
			app = TtyAppender::getAppender(dev);
			break;
		case COM_ULLOGDEVICE:
		case COM_ULNETDEVICE:
			_com_debug("create ul appender");
			app = UlLogAppender::getAppender(dev);
			break;
		case COM_AFILEDEVICE:
			_com_debug("create async file appender");
			app = AsyncFileAppender::getAppender(dev);
			break;
		default:
			_com_error("sorry i can't know the type[%d]", dev.type);
			break;
	}
	if (app != NULL) {
		app->setLayout(dev.layout);
		_core_debug("dev =%lx appender layout=%lx", dev.layout, app->_layout);
		app->_open = dev.open;
	}

	return app;
}
#endif
int Appender::flush()
{
	return 0;
}
#if 0
Appender * Appender::tryAppender(com_device_t &dev)
{
	xAutoLock lock(&comlog_appender_getlock);
	
	Appender *app = NULL; 
	switch (dev.type) {
		case COM_FILEDEVICE:
			app = FileAppender::tryAppender(dev);
			break;
		case COM_NETDEVICE:
			app = NetAppender::tryAppender(dev);
			break;
		case COM_TTYDEVICE:
			app = TtyAppender::tryAppender(dev);
			break;
		case COM_ULLOGDEVICE:
			app = UlLogAppender::tryAppender(dev);
			break;
		case COM_AFILEDEVICE:
			app = AsyncFileAppender::tryAppender(dev);
			break;
	}

	return app;
}
#endif
}
/* vim: set ts=4 sw=4 sts=4 tw=100 */
