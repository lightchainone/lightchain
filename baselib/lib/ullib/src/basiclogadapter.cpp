
#include "basiclogadapter.h"
#include "ul_log.h"
//À´×Ôullib
extern int ul_writelog_args(const int event, const char* fmt, va_list args);

int BasicLogAdapter :: _instance = 0;
pthread_mutex_t BasicLogAdapter :: _lock = PTHREAD_MUTEX_INITIALIZER;

BasicLogAdapter :: BasicLogAdapter(){
	_cate = NULL;
	_level = 16;
}

int BasicLogAdapter :: setCategory(comspace::Category * cate){
	_cate = cate;
	return 0;
}

comspace :: Category * BasicLogAdapter :: tryCategory(){
	if(_cate == NULL){
		if(! comspace :: is_init_log()){
			comspace :: init_log();
		}
		comspace :: xAutoLock autolock(&_lock);
		char buf[128];
		snprintf(buf, sizeof(buf), "BasicLogAdapter%d", _instance);
		++_instance;
		_cate = comspace::Category::getInstance(buf);
	}
	if(_cate == NULL){
		ul_writelog(UL_LOG_WARNING, "No available category!");
	}
	return _cate;
}

int BasicLogAdapter :: setDevice(com_device_t * dev, int num){
	if(dev == NULL){
		ul_writelog(UL_LOG_WARNING, "setDevice : dev = NULL");
		return -1;
	}
	if(tryCategory() == NULL){
		return -1;
	}

	comspace::Appender * app = NULL;
	int cnt = 0;
	for(int i = 0; i < num; ++i){
		app = comspace::AppenderFactory::tryAppender(dev[i]);
		if(app == NULL){
			app = comspace::AppenderFactory::getAppender(dev[i]);
		}
		if(app != NULL){
			if( _cate->addAppender(app) == 0 ){
				++cnt;
			}
		}
	}

	return cnt;
}

int BasicLogAdapter :: setLogStat(comspace::LogStat * logstat){
	if(tryCategory() == NULL){
		return -1;
	}
	return _cate->setLogStat(logstat);
}


int BasicLogAdapter :: setLevel(int level){
	_level = level;
	return 0;
}

int BasicLogAdapter :: getLevel()
{
    return _level;
}

int BasicLogAdapter :: write_args(int level, const char * fmt, va_list args){
	if(level < 32 && level > _level){
		return 0;
	}
	if(_cate == NULL){
		return ul_writelog_args(level, fmt, args);
	}
	else{
		return _cate->print(level, fmt, args);
	}
}

int BasicLogAdapter :: write(int level, const char * fmt, ...){
	va_list args;
	va_start(args, fmt);
	int ret = write_args(level, fmt, args);
	va_end(args);
	return ret;
}

int BasicLogAdapter :: write(const char * level, const char * fmt, ...){
	va_list args;
	va_start(args, fmt);
	int logid = comspace :: getLogId(level);
	int ret = write_args(logid, fmt, args);
	va_end(args);
	return ret;
}

int BasicLogAdapter :: fatal(const char * fmt, ...){
	va_list args;
	va_start(args, fmt);
	int ret = write_args(UL_LOG_FATAL, fmt, args);
	va_end(args);
	return ret;
}


int BasicLogAdapter :: warning(const char * fmt, ...){
	va_list args;
	va_start(args, fmt);
	int ret = write_args(UL_LOG_WARNING, fmt, args);
	va_end(args);
	return ret;
}

int BasicLogAdapter :: notice(const char * fmt, ...){
	va_list args;
	va_start(args, fmt);
	int ret = write_args(UL_LOG_NOTICE, fmt, args);
	va_end(args);
	return ret;
}

int BasicLogAdapter :: trace(const char * fmt, ...){
	va_list args;
	va_start(args, fmt);
	int ret = write_args(UL_LOG_TRACE, fmt, args);
	va_end(args);
	return ret;
}

int BasicLogAdapter :: debug(const char * fmt, ...){
	va_list args;
	va_start(args, fmt);
	int ret = write_args(UL_LOG_DEBUG, fmt, args);
	va_end(args);
	return ret;
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
