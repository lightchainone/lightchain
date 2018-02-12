

#ifndef  __BASICLOGADAPTER_H_
#define  __BASICLOGADAPTER_H_

#include "com_log.h"

//为mask添加一个日志等级,mask应该是一个uint32_t
#define COMLOG_MASK(mask, level) do{\
	(mask) |= (unsigned int)(1) << (level);\
}while(0)

//为mask添加5个默认的系统日志等级,mask是一个uint32_t
#define COMLOG_DEFAULT_MASK(mask) do{\
	COMLOG_MASK(mask, COMLOG_FATAL);\
	COMLOG_MASK(mask, COMLOG_WARNING);\
	COMLOG_MASK(mask, COMLOG_NOTICE);\
	COMLOG_MASK(mask, COMLOG_TRACE);\
	COMLOG_MASK(mask, COMLOG_DEBUG);\
}while(0)

/**
 *        BasicLogAdapter不会释放任何资源（包括自己创建的资源）
 *        一般来说，BasicLogAdapter的生命周期是整个进程
 *        如要释放，只能使用com_closelog()
 *
 * 		  程序默认使用ullog打印日志
 *        当调用setCategory, setDevice, setLayout等函数，
 *        会自动切换到comlog的规则
 *
 *        setLevel是总控开关，对comlog/ullog均生效。（自定义日志不受约束）
 *
 *        如果要打印自定义等级的日志，必须显式调用setLogStat，示例：
 *        BasicLogAdapter *p = new BasicLogAdapter();
 *        p->setDevice(xxx);//注册Device
 *        comspace::LogStat lstat = comspace::LogStat::createLogStat();//需要在setDevice之后调用
 *        //因为createLogStat必须在comlog被初始化之后才可以使用，所以必须先执行setDevice，里面会做初始化
 *        uint32_t level = 0;
 *        COMLOG_DEFAULT_MASK(level);
 *        lstat.setLevel(level, "SelfName1, SelfName2"); //这个SelfName1也需要在配置文件中的selfdefine中定义
 *        p->setLogStat(lstat);
 *        p->write("SelfName1", "This is a self level log...");
 *        //或者：
 *        int self = comspace::getLogId("SelfName1");
 *        p->write(self, "This is also a self level log...");
 *        
 */
class BasicLogAdapter{
	protected:
		//实际的日志打印句柄
		comspace :: Category * _cate;
		//系统日志等级
		int _level;
	plclic:
		BasicLogAdapter();
		/**
		 *
		**/
		int setCategory(comspace::Category * cate);
		/**
		 *
		**/
		int setDevice(com_device_t * dev, int num);
		/**
		 *
		**/
		int setLogStat(comspace::LogStat * logstat);
		/**
		 *
		**/
		int setLevel(int level);
        /**
         *
        **/
        int getLevel();
		/**
		 *
		**/
		int write(int level, const char * fmt, ...) __attribute__ ((format (printf,3,4)));
		/**
		 *
		**/
		int write(const char * level, const char * fmt, ...) __attribute__ ((format (printf,3,4)));
		//----- 以下接口相当于write的包装---
		/**
		 *
		**/
		int fatal(const char * fmt, ...) __attribute__ ((format (printf,2,3)));
		/**
		 *
		**/
		int warning(const char * fmt, ...) __attribute__ ((format (printf,2,3)));
		/**
		 *
		**/
		int notice(const char * fmt, ...) __attribute__ ((format (printf,2,3)));
		/**
		 *
		**/
		int trace(const char * fmt, ...) __attribute__ ((format (printf,2,3)));
		/**
		 *
		**/
		int debug(const char * fmt, ...) __attribute__ ((format (printf,2,3)));
	protected:
		/**
		 *
		**/
		int write_args(int level, const char * fmt, va_list args);
		comspace :: Category * tryCategory();
		static int _instance;
		static pthread_mutex_t _lock;
};

















#endif  //__BASICLOGADAPTER_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
