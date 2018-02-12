 
 
/**
 * 
 * 日志的使用如下
 * 
 * 多线程程序:
 *
 * 主进程              子线程 1         子线程 2 ...
 *
 *  ul_openlog
 *
 *      |
 *
 *      ------------------------------------
 *
 *      |               |               |
 *
 *  ul_writelog  ul_openlog_r       ul_openlog_r
 *
 *     ...              |               |
 *
 *     ...       ul_writelog        ul_writelog
 *
 *     ...           ...              ...
 *
 *               ul_closelog_r      ul_closelog_r
 *
 *                      |               |
 *
 *      --------------------------------------
 *
 *      |
 *
 *  ul_closelog
 *
 * 单进程
 *
 *  ul_openlog
 *
 *      |
 *
 *  ul_writelog
 *
 *     ...
 *
 *      |
 *
 *  ul_closelog
 *
 *  如果在ul_openlog前调用ul_writelog,
 *  日志信息会被输出到标准出错(stderr)上,日志库支持多线程呈现,但不支持多进程程序,
 *  多进程中使用请保证每个进程使用的是不用的日志文件
 *
 **/

#ifndef __UL_LOG_H__
#define __UL_LOG_H__

#include <sys/time.h>
#include <sys/timeb.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/wait.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <syslog.h>
#include <signal.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include <limits.h>
#include <fcntl.h>
#include <stdarg.h>
#include <time.h>
#include <unistd.h>
#include <assert.h>

#include "ul_def.h"
#include "dep/dep_log.h"

#define ERR_NOSPACE		-1
#define ERR_OPENFILE	-2
#define ERR_EVENT		-3
#define ERR_FILEFULL	-4

#define LOG_FILE_NUM	2048		      /**< 最多可同时打开的文件数      */
#define LOG_BUFF_SIZE_EX 2048			  /**< 一条日志buff大小       */

#define MAX_FILE_SIZE		2045		  /**< 最大文件大小单位(MB)      */
#define MAX_FILENAME_LEN	1024		  /**< 日志文件名的最大长度       */


#define UL_LOG_WFSTART	-2
#define UL_LOG_START	-1
#define UL_LOG_END		UL_LOG_START
#define UL_LOG_WFEND	UL_LOG_WFSTART

#define UL_LOG_NONE		0
#define UL_LOG_FATAL	0x01    /**<   fatal errors */
#define UL_LOG_WARNING	0x02    /**<   exceptional events */
#define UL_LOG_NOTICE   0x04    /**<   informational notices */
#define UL_LOG_TRACE	0x08    /**<   program tracing */
#define UL_LOG_DEBUG	0x10    /**<   full debugging */
#define UL_LOG_ALL		0xff    /**<   everything     */

#define UL_LOG_SELF_BEGIN   0x100
#define UL_LOG_SELF_END     0x107
#define UL_LOG_SELF_MASK    0xff

/* ul_log_t  log_sepc */
#define UL_LOGTTY		0x02    /**<   日志在输出到日志文件的同时输出到标准出错(stderr)中 */
#define UL_LOGNEWFILE	0x08    /**<   创建新的日志文件,可以使每个线程都把日志打到不同文件中*/
#define UL_LOGSIZESPLIT 0x10    /**<  按大小分割日志文件，不回滚*/
/* ul_file_t  flag */
#define UL_FILE_TRUNCATE	0x01
#define UL_FILE_FULL		0x02

#define UL_LOG_RECORD	15	

#define MAX_SELF_DEF_LOG 8		  /**< 单个线中自定义日志的日志数上限      */

struct ul_logstat_t {
        int events;		/**< 需要打的日志级别 0-15 */
        int to_syslog;	/**< 输出到syslog 中的日志级别, 0-15 */
};
typedef struct ul_logstat_t ul_logstat_t;

struct ul_file_t {
	FILE *fp;							/**< 文件句柄 */
	int  ref_cnt;						/**< 引用计数 */
	int  max_size;						/**< 文件可以记录的最大长度 */
	pthread_mutex_t file_lock;			/**< 写文件锁 */
	char file_name[MAX_FILENAME_LEN+1];	/**< 文件名字 */
};
typedef struct ul_file_t ul_file_t;

struct ul_log_t {
	char used;							/**< 0-未使用  1-已使用 */
	ul_file_t *pf;						/**< log */
	ul_file_t *pf_wf;					/**< log.wf */
	pthread_t tid;						/**< 线程id  实际使用上是使用gettid,而非pthread_self*/
	int  mask;							/**< 可以记录的事件的掩码 */
	int  log_syslog;					/**< 输出到系统日志的事件掩码 */
	int  log_spec;						/**< UL_LOGTTY | UL_LOGNEWFILE */
	ul_file_t *spf[MAX_SELF_DEF_LOG];	/**< 自定义日志文件句柄 */
};
typedef struct ul_log_t ul_log_t;

struct Ul_log_self {					/**< 兼容旧模式，定义不加 _t       */
	char name[MAX_SELF_DEF_LOG][PATH_SIZE];		/**< 自定义日志文件名，系统自动在文件名后加后缀.sdf */
	char flags[MAX_SELF_DEF_LOG];				/**<决定当前自定义的日志是否输出,设置为1则生成自定义日志,0则不生成 */
	int  log_number;							/**< 自定义文件的数目,当设置为0时,不生成自定义文件 */
}; //自定义日志的设置,可在ul_openlog以及ul_openlog_r中作为参数传入,设置UL_LOGNEWFILE时,自定义日志与正常日志一样,
			 //为线程文件,否则,自定义日志为进程级日志
typedef struct Ul_log_self ul_log_self_t;


/**
 *
 */
extern int ul_openlog(const char *log_path, const char *log_procname, ul_logstat_t  *l_stat,
		int maxlen, Ul_log_self *self = NULL);


/**
 *
 */
#ifdef  _UN_WRITELOG_FORMAT_CHECK
extern int ul_writelog(const int event, const char* fmt, ... );
#else
extern int ul_writelog(const int event, const char* fmt, ... ) __attribute__ ((format (printf,2,3)));
#endif

/**
 *
 */
extern int ul_closelog(int iserr);

/**
 *
 */
extern int ul_openlog_r(const char *threadname, ul_logstat_t  *l_stat, Ul_log_self *self = NULL);

/**
 *
 */
extern int ul_closelog_r(int iserr);

/**
 *
**/
extern int ul_get_log_level();

#endif
/* vim: set ts=4 sw=4 sts=4 tw=100 noet: */
