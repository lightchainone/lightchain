
#include "ttyappender.h"
#include "../layout.h"
#include "../event.h"
#include "../namemg.h"
#include "../xdebug.h"

namespace comspace
{

int TtyAppender::open(void *)
{
	return 0;
}

int TtyAppender::close(void *)
{
	return 0;
}

int TtyAppender::print(Event *evt)
{
	if (unInMask(evt->_log_level)) {
		return 0;
	}
	_layout->format(evt);
	//_com_debug("layout add %lx", (long)_layout);
	fprintf(stderr, "%s", evt->_render_msgbuf);
	return 0;
}

int TtyAppender::binprint(void *val, int siz)
{
	fwrite(val, 1, siz, stderr);
	return 0;
}

Appender * TtyAppender::getAppender(com_device_t &dev)
{
	char name[64];
	if (dev.layout == NULL) {
		//_com_debug("change ly[%lx] to def[%lx]", (long)dev.layout, &g_def_layout);
		dev.layout = comlog_get_def_layout();
	}
	snprintf(name, sizeof(name), "tty:tty.%lx", (long)dev.layout);
	_com_debug("get name[%s]", name);
	Appender *app = (Appender *)comlog_get_nameMg()->get(type_appender, name);
	if (app == NULL) {
		app = new TtyAppender;
		app->setDeviceInfo(dev);
		_com_debug("get layout=%lx", app->_layout);
		comlog_get_nameMg()->set(name, app);
	}
	return app;
}

Appender * TtyAppender::tryAppender(com_device_t &dev)
{
	char name[64];
	snprintf(name, sizeof(name), "tty:tty.%lx", (long)dev.layout);
	Appender *app = (Appender *)comlog_get_nameMg()->get(type_appender, name);
	return app;
}

TtyAppender::TtyAppender()
{
	_com_debug("layout=%lx", (long)_layout);
}
}
/* vim: set ts=4 sw=4 sts=4 tw=100 */
