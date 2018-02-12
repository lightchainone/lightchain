
#include "category.h"
#include "namemg.h"
#include "xutils.h"
#include "xdebug.h"

namespace comspace
{

log_callback_t Category::_g_callback = NULL;
void * Category::_g_callback_param = NULL;

Category::Category(Category *parent) : push(this)
{
	for (u_int i=0; i<COM_MAXDEVICENUM; ++i) {
		_appenders[i] = NULL;
		_appender_mask[i] = 0;
		_mask_flag = 0;
	}

	_appnum = 0;
	if (parent != NULL) {
		_logstat = parent->_logstat;
		_appnum = parent->_appnum;
		for (int i=0; i<_appnum; ++i) {
			_appenders[i] = parent->_appenders[i];
		}
	} else {
		_logstat = comlog_get_def_logstat();
	}
	_event = NULL;
	_parent = parent;
}

Category::~Category()
{
}

int Category::setLogStat(LogStat *logstat)
{
	if (logstat == NULL) {
		_logstat = comlog_get_def_logstat();
	} else {
		_logstat = logstat;
	}
	return 0;
}

int Category::addAppender(Appender *app)
{
	if (_appnum >= (int)COM_MAXDEVICENUM) {
		return -1;
	}
	if (app == NULL) {
		return -1;
	}
	for (int i=0; i<_appnum; ++i) {
		if (app == _appenders[i]) {
			return 0;
		}
	}
	_appenders[_appnum++] = app;
	return 0;
}

int Category::removeAppender(Appender *app)
{
	int s = 0;
	for (int i=0; i<_appnum; ++i) {
		if (_appenders[i] != app) {
			_appenders[s++] = _appenders[i];
		}
	}
	_appnum = s;
	return 0;
}

int Category::clearAppender()
{
	_appnum = 0;
	return 0;
}

int Category::addAppenderMask(char *appender_type){
	
	if( _mask_flag == 1 ){
		return 0;
	}

	int i;
        for( i = 0 ; i < _appnum ; i++ ){
                if( strcmp( appender_type , _appenders[i]->_device.type ) == 0 ){
                        _appender_mask[i] = 1;
                }       
        }                       

	_mask_flag = 1;

	return 0;
}

//
int Category::print(u_int level, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	int ret = print(level, fmt, args);
	va_end(args);

	return ret; 
}

Category::Push & Category::Push::operator()(const char *key, const char *fmt, ...)
{
	//这样就不会改变event原来的指了
	Event *event = _cate->_event;
	if (event == NULL) {
		event = Event::getEvent();
		if (event == NULL) {
			return *this;
		}
	}

	char val[64];
	va_list args;
	va_start(args, fmt);
	vsnprintf(val, sizeof(val), fmt, args);
	va_end(args);
	event->push(key, val);

	//_com_debug("push key[%s]:val[%s]", key, val);

	return *this;
}

int Category::Push::operator()(int level, const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	int ret = _cate->print(level, fmt, args);
	va_end(args);

	return ret; 
}

Category::Push::Push(Category *cate)
{
	_cate = cate;
}

int Category::print(u_int level, const char *fmt, va_list args)
{
	if (_logstat == NULL) {
		_logstat = comlog_get_def_logstat();
	}
	if (_logstat->unAccept(level)) {
		//_com_debug("no eccept this level[%d] %llx", level, _logstat->_level);
		//_com_debug("%llx", ((unsigned long long)1<<(unsigned long long)level) & _logstat->_level);
		return 0;
	}
    
    if(_g_callback)
    {
        _g_callback(level, fmt, args, _g_callback_param);
    }

	Event *event = _event;
	if (event == NULL) {
		event = Event::getEvent();
		if (event == NULL) {
			return -1;
		}
	}
	gettimeofday(&event->_print_time, NULL);
	event->_log_level = level;

	event->_msgbuf_len = vsnprintf(event->_msgbuf, event->_msgbuf_size, fmt, args);
	if (event->_msgbuf_len < 0) {
		event->_msgbuf_len = snprintf(event->_msgbuf, event->_msgbuf_size, 
			"comlog: one message was lost because vsnprintf failed");
		if (event->_msgbuf_len < 0)
			event->_msgbuf_len = 0;
	}
	if (event->_msgbuf_len >= event->_msgbuf_size) {
		event->_msgbuf_len = event->_msgbuf_size - 1;
	}

	//_com_debug("msg[%s] appnum[%d]", event->_msgbuf, _appnum);

	for (int i=0; i<_appnum; ++i) {
		if (_appenders[i]->_open ) {
			if( _appender_mask[i] == 0 )
				_appenders[i]->print(event);
			else if( _appenders[i]->_bkappender != NULL )
				_appenders[i]->_bkappender->print(event);
		}
	}

	event->reset();
	return 0;
}

static bool getFather(const char *name, char *father, int size)
{
	int start = 0;
	snprintf(father, size, "%s", name);
	for (int i=0; father[i]!=0; ++i) {
		if (father[i] == '.') {
			start = i;
		}
	}
	if (start > 0) {
		father[start] = 0;
		return true;
	}
	return false;
}

Category * Category::getInstance(const char *name)
{
	if (name[0] == '.') {
		return NULL;
	}
	Category *ptr = (Category *)comlog_get_nameMg()->get(type_category, name);
	if (ptr == NULL) {
		char fat[COM_MAXCATEGORYNAMESIZE];
		Category *pnt = NULL;
		if (getFather(name, fat, sizeof(fat))) {
#if 0
			//父亲不存在就失败
			pnt = (Category *)comlog_get_nameMg()->get(type_category, fat);
#else
			//如果父亲不存在,自动创建
			pnt = Category::getInstance(fat);
#endif
			if (pnt == NULL) {
				_com_error("get father err [%s] with father[%s]", name, fat);
				return NULL;
			}
		}

		ptr = new Category(pnt);
		//已经被设置
		//线程安全
		int ret = comlog_get_nameMg()->set(name, ptr);
		if (ret != 0) {
			delete ptr;
			ptr = (Category *)comlog_get_nameMg()->get(type_category, name);
		}
		//_com_debug("new category[%s] suc", name);
	}
	return ptr;
}

//pthread_mutex_t Category::glock = PTHREAD_MUTEX_INITIALIZER;

}
/* vim: set ts=4 sw=4 sts=4 tw=100 */
