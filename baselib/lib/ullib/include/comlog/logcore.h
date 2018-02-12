

#ifndef  __LOGCORE_H_
#define  __LOGCORE_H_

#include "comlog.h"
#include "xdebug.h"

namespace comspace
{

//根据日志顶级获取日志名
/**
 *
**/
const char *getLogName(u_int logid);
/**
 *
**/
u_int getLogId(const char *logname);
//返回对应的日志等级
int createSelfLog(const char *lvlname);

bool is_init_log();
int init_log();
int init_log_r();
int com_wait_close(int waittime);
int com_close_appender();
int com_reset_appender();
int close_log(int atonce=1000);
int reset_log();
int close_log_r();

}



#endif  //__LOGCORE_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
