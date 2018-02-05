#ifndef __LC_LOG_H__
#define __LC_LOG_H__

#include <pthread.h>
#include "ul_log.h"
#include "com_log.h"













#define NOTICE_INFO_MAXLEN LOG_BUFF_SIZE_EX


#define LC_SPACE  ' '


typedef enum lc_notice_type{
    LC_LOG_ZERO = 0,		
    LC_LOG_LOGID,		
    LC_LOG_PROCTIME,	
    LC_LOG_REQIP,		
    LC_LOG_REQSVR,		
    LC_LOG_SVRNAME,		
    LC_LOG_CMDNO,		
    LC_LOG_ERRNO,		
    LC_LOG_END		    
} dummy_lc_notice_type;



int lc_log_init(const char* log_path = "./log", const char* log_file = "lc_log.",
	int max_size = 1000, int log_level = 16, int to_syslog = 0, int spec = 0, Ul_log_self *self = NULL);



int lc_open_comlog(const char *procname, com_device_t *dev, int dev_num, com_logstat_t *logstat);


int lc_load_comlog(const char *path, const char *file);
namespace comcfg {
	class ConfigUnit;
}


int lc_init_comlog(const comcfg::ConfigUnit & conf );


int lc_log_initthread(const char* thread_name, Ul_log_self *self = NULL);



void lc_log_close();



void lc_log_closethread();



unsigned int lc_log_pushnotice(const char* key, const char* valuefmt, ...);



unsigned int lc_log_setbasic(lc_notice_type type, const char* valuefmt, ...);



char* lc_log_getbasic(lc_notice_type type);



unsigned int lc_log_setlogid(unsigned int logid);



unsigned int lc_log_getlogid();



unsigned int lc_log_clearlogid();



char* lc_log_popnotice();



int lc_log_clearnotice();



unsigned int lc_log_getussecond();






int notice_specific_init();

void lc_log_keyonce();


#define LC_LOG_MONITOR(fmt, arg...)  \
    ul_writelog(UL_LOG_WARNING, "---LOG_MONITOR--- [ %clogid:%s %c][ %creqip:%s %c][%s:%d]" fmt,\
	    LC_SPACE, lc_log_getbasic(LC_LOG_LOGID), LC_SPACE, LC_SPACE, lc_log_getbasic(LC_LOG_REQIP), LC_SPACE,\
			__FILE__, __LINE__, ## arg)

#define LC_LOG_FATAL(fmt, arg...) do { \
    ul_writelog(UL_LOG_WARNING, "---LOG_MONITOR--- [ %clogid:%s %c][ %creqip:%s %c][%s:%d]" fmt,\
	    LC_SPACE, lc_log_getbasic(LC_LOG_LOGID), LC_SPACE, LC_SPACE, lc_log_getbasic(LC_LOG_REQIP), LC_SPACE,\
			__FILE__, __LINE__, ## arg); \
    ul_writelog(UL_LOG_FATAL, "[ %clogid:%s %c][ %creqip:%s %c][%s:%d]" fmt,\
	    LC_SPACE, lc_log_getbasic(LC_LOG_LOGID), LC_SPACE, LC_SPACE, lc_log_getbasic(LC_LOG_REQIP), LC_SPACE,\
			__FILE__, __LINE__, ## arg); \
} while (0)

#define LC_LOG_WARNING(fmt, arg...)  \
    ul_writelog(UL_LOG_WARNING, "[ %clogid:%s %c][ %creqip:%s %c][%s:%d]" fmt,\
	    LC_SPACE, lc_log_getbasic(LC_LOG_LOGID), LC_SPACE, LC_SPACE, lc_log_getbasic(LC_LOG_REQIP), LC_SPACE,\
			__FILE__, __LINE__, ## arg)

#define LC_LOG_NOTICE(fmt, arg...) do { \
    ul_writelog(UL_LOG_NOTICE, "[ %clogid:%s %c]" "[ %cproctime:%s %c]" "[ %creqip:%s %c]" "[ %creqsvr:%s %c]"\
	    "[ %ccmdno:%s %c]" "[ %csvrname:%s %c]" "[ %cerrno:%s %c]" "[ %c%s %c]" "[ %c%s %c]" "[ %c" fmt " %c]", \
	    LC_SPACE, lc_log_getbasic(LC_LOG_LOGID), LC_SPACE, LC_SPACE, lc_log_getbasic(LC_LOG_PROCTIME), LC_SPACE, \
	    LC_SPACE, lc_log_getbasic(LC_LOG_REQIP), LC_SPACE, LC_SPACE, lc_log_getbasic(LC_LOG_REQSVR), LC_SPACE, \
	    LC_SPACE, lc_log_getbasic(LC_LOG_CMDNO), LC_SPACE, LC_SPACE, lc_log_getbasic(LC_LOG_SVRNAME), LC_SPACE, \
		LC_SPACE, lc_log_getbasic(LC_LOG_ERRNO), LC_SPACE, LC_SPACE, "", LC_SPACE, \
		LC_SPACE, lc_log_popnotice(), LC_SPACE, LC_SPACE, ## arg, LC_SPACE);\
	    lc_log_clearnotice(); \
} while (0)

#define LC_LOG_TRACE(fmt, arg...)  \
    ul_writelog(UL_LOG_TRACE, "[ %clogid:%s %c]" fmt,\
	   LC_SPACE, lc_log_getbasic(LC_LOG_LOGID), LC_SPACE, ## arg)

#ifndef NO_LC_DEBUG
#define LC_LOG_DEBUG(fmt, arg...) \
    ul_writelog(UL_LOG_DEBUG, "[ %clogid:%s %c][%s:%d][time:%uus]" fmt,\
	    LC_SPACE, lc_log_getbasic(LC_LOG_LOGID), LC_SPACE, __FILE__, __LINE__,\
	    lc_log_getussecond(), ## arg)
#else
#define LC_LOG_DEBUG(fmt, arg...) \
    do {} while(0)
#endif


#endif 




