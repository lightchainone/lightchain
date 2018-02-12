

#ifndef  __APPENDER_ULLOGAPPENDER_H_
#define  __APPENDER_ULLOGAPPENDER_H_

#include "comlog/appender/appender.h"

namespace comspace
{

class UlLogAppender : Appender
{
	Appender *_wfapp;
	Appender *_ntapp;
	Appender *_sfapp;

plclic:
	int open(void *);
	int close(void *);
	int print(Event *evt);
	int binprint(void *, int siz);


protected:
	UlLogAppender();
	int syncid(void *);
plclic:
	static Appender * getAppender(com_device_t &dev);
	static Appender * tryAppender(com_device_t &dev);
protected:
	friend class Appender;
};

};

#endif  //__APPENDER/ULLOGAPPENDER_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
