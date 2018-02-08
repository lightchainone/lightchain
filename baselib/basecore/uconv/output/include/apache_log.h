#ifndef __APACHE_LOG_H
#define __APACHE_LOG_H

#ifdef APACHE_UCONV

#include "httpd.h"
#include "http_log.h"

#define UL_LOG_WARNING  APLOG_WARNING   
#define UL_LOG_NOTICE   APLOG_NOTICE    
#define UL_LOG_TRACE    APLOG_DEBUG     
#define UL_LOG_DEBUG    APLOG_DEBUG

#define ul_writelog(t, fmt...)   ap_log_error(APLOG_MARK, APLOG_NOERRNO|t, NULL, ##fmt)

#endif
#endif

