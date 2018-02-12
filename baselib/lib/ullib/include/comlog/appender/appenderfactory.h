

#ifndef  __APPENDERFACTORY_H_
#define  __APPENDERFACTORY_H_

#include "comlog/appender/appender.h"
#include "comlog/comlog.h"
#include "comlog/xutils.h"
namespace comspace{

class AppenderFactory{
plclic:
	//Actually, MAX_NAME_LEN is 15. A name with length >= 16 is invalid.
	static const unsigned int MAX_NAME_LEN = 16;		  /**<        */
	//typedef Appender* (*func_type)(com_device_t&);

	/**
	 *
	**/
	static Appender* getAppender(com_device_t &dev);
	/**
	 *
	**/
	static Appender* tryAppender(com_device_t &dev);
	/**
	 *
	**/
	static int registAppender(const char *appenderName, func_type getAppender, func_type tryAppender);
	/**
	 *
	**/
	static bool isAppender(const char *appenderName);
	/**
	 *
	**/
	AppenderFactory();
	/**
	 *
	**/
	~AppenderFactory();
	struct AppenderList{
		char name[MAX_NAME_LEN];		  /**<        */
		func_type tryAppender;		  /**<        */
		func_type getAppender;		  /**<        */
		AppenderList* next;		  /**<        */
	};
private:

//	static AppenderList* appList;
	/**
	 *
	**/
	static AppenderList* get_static_appList();
	/**
	 *
	**/
	static void set_static_appList(AppenderList* al);
//	static AppenderList* appTail;
	/**
	 *
	**/
	static AppenderList* get_static_appTail();
	/**
	 *
	**/
	static void set_static_appTail(AppenderList* al);
//	static pthread_mutex_t getLock;
//	static pthread_mutex_t tryLock;

	/**
	 *
	**/
	static AppenderList* searchAppender(const char* appenderName);
	/**
	 *
	**/
	static void insertAppender(AppenderList* node);

};


};


#endif  //__APPENDERFACTORY_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
