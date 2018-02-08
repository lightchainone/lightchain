#ifndef __LOG_H__
#define __LOG_H__

#include "lc_log.h"

int  zlog_open(const char *path,const char *proc_name,int level,int filelen, bool is_log_split = true);


int zlog_open_r(const char *thread_name);


int zlog_close();


int zlog_close_r();




inline int zlog_set_thlogid(unsigned int logid)
{
    return lc_log_setlogid(logid);
}


inline unsigned int zlog_get_thlogid()
{
    return lc_log_getlogid();
}

#define MONITOR_LOG(fmt, arg...)  do { \
         ul_writelog(UL_LOG_NOTICE, "MONITOR [logid:%u][%s:%d:%s]" fmt,\
                zlog_get_thlogid(),__FILE__, __LINE__, __FUNCTION__,## arg); \
}while(0)

#define STATIC_LOG(fmt, arg...)  do { \
         ul_writelog(UL_LOG_NOTICE, "STATIC [logid:%u][%s:%d:%s]" fmt,\
                zlog_get_thlogid(),__FILE__, __LINE__, __FUNCTION__,## arg); \
}while(0)

#define FATAL_LOG(fmt, arg...) do { \
           ul_writelog(UL_LOG_FATAL, "[logid:%u][%s:%d:%s]" fmt, \
                           zlog_get_thlogid(), __FILE__, __LINE__,__FUNCTION__, ## arg); \
                ul_writelog(UL_LOG_DEBUG, "[logid:%u][%s:%d:%s]" fmt,zlog_get_thlogid(),__FILE__, __LINE__,__FUNCTION__, ## arg); \
}while (0)

#define URGENT_LOG(fmt, arg...) do { \
           ul_writelog(UL_LOG_FATAL, "URGENT_FATAL [logid:%u][%s:%d:%s]" fmt, \
                           zlog_get_thlogid(), __FILE__, __LINE__,__FUNCTION__, ## arg); \
                ul_writelog(UL_LOG_DEBUG, "URGENT [logid:%u][%s:%d:%s]" fmt,zlog_get_thlogid(),__FILE__, __LINE__,__FUNCTION__, ## arg); \
}while (0)

#define WARNING_LOG(fmt, arg...) do { \
        ul_writelog(UL_LOG_WARNING, "[logid:%u][%s:%d:%s]" fmt,  \
                           zlog_get_thlogid(),__FILE__, __LINE__,__FUNCTION__, ## arg); \
        ul_writelog(UL_LOG_DEBUG, "[logid:%u][%s:%d:%s]" fmt,zlog_get_thlogid(),__FILE__, __LINE__,__FUNCTION__, ## arg); \
} while (0)

#define STATIC_WARNING_LOG(fmt, arg...) do { \
        ul_writelog(UL_LOG_WARNING, "STATIC [logid:%u][%s:%d:%s]" fmt,  \
                           zlog_get_thlogid(),__FILE__, __LINE__,__FUNCTION__, ## arg); \
        ul_writelog(UL_LOG_DEBUG, "[logid:%u][%s:%d:%s]" fmt,zlog_get_thlogid(),__FILE__, __LINE__,__FUNCTION__, ## arg); \
} while (0)

#define NOTICE_LOG(fmt, arg...) do { \
          ul_writelog(UL_LOG_NOTICE, "[logid:%u][%s:%d:%s]" fmt, zlog_get_thlogid(),__FILE__, __LINE__,__FUNCTION__,## arg); \
} while (0)

#define TRACE_LOG(fmt, arg...) do { \
        ul_writelog(UL_LOG_TRACE, "[logid:%u][%s:%d:%s]" fmt, zlog_get_thlogid(),__FILE__, __LINE__,__FUNCTION__, ## arg); \
} while (0)

#define DEBUG_LOG(fmt, arg...) do { \
           ul_writelog(UL_LOG_DEBUG, "[logid:%u][%s:%d:%s]" fmt, \
                           zlog_get_thlogid(),__FILE__, __LINE__,__FUNCTION__, ## arg); \
} while (0)

#endif

