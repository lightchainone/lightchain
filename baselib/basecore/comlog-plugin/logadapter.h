

#ifndef  __LOGADAPTER_H_
#define  __LOGADAPTER_H_

#include "com_log.h"
#include "Configure.h"


#define COMLOG_MASK(mask, level) do{\
	(mask) |= (unsigned int)(1) << (level);\
}while(0)


#define COMLOG_DEFAULT_MASK(mask) do{\
	COMLOG_MASK(mask, COMLOG_FATAL);\
	COMLOG_MASK(mask, COMLOG_WARNING);\
	COMLOG_MASK(mask, COMLOG_NOTICE);\
	COMLOG_MASK(mask, COMLOG_TRACE);\
	COMLOG_MASK(mask, COMLOG_DEBUG);\
}while(0)


class LogAdapter{
	protected:
		
		comspace :: Category * _cate;
		
		int _level;
	plclic:
		LogAdapter();
		
		int setCategory(comspace::Category * cate);
		
		int setDevice(com_device_t * dev, int num);
		
		int setDevice(const comcfg::ConfigUnit & conf);
		
		int setLogStat(comspace::LogStat * logstat);
		
		int setLevel(int level);
		
		int write(int level, const char * fmt, ...);
		
		int write(const char * level, const char * fmt, ...);
		
		
		int fatal(const char * fmt, ...);
		
		int warning(const char * fmt, ...);
		
		int notice(const char * fmt, ...);
		
		int trace(const char * fmt, ...);
		
		int debug(const char * fmt, ...);
	protected:
		
		int write_args(int level, const char * fmt, va_list args);
		comspace :: Category * tryCategory();
		static int _instance;
		static pthread_mutex_t _lock;
};

















#endif  


