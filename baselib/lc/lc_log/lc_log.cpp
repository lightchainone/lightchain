#include "lc_log.h"
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#define LC_LOG_INT_LEN 128
#define LC_LOG_IP_LEN 128
#define LC_LOG_SERVNAME_LEN 128	    
#define LC_LOG_CMD_WITHEXP_LEN 128


typedef struct _notice_info_t{
    unsigned int extra_cur_len;						
    char extra_notice_str[NOTICE_INFO_MAXLEN];		
	char logid_str[LC_LOG_INT_LEN];					
	char processtime_str[LC_LOG_INT_LEN];			
	char requestip_str[LC_LOG_IP_LEN];				
	char requestserver_str[LC_LOG_SERVNAME_LEN];	
	char svrname_str[LC_LOG_SERVNAME_LEN];			
	char errno_str[LC_LOG_CMD_WITHEXP_LEN];			
	char cmd_str[LC_LOG_CMD_WITHEXP_LEN];			
	unsigned int logid;								
}notice_info_t, *pnotice_info_t;


static ul_logstat_t      log_stat;

static pthread_key_t   g_time_key = PTHREAD_KEYS_MAX ; 
static pthread_key_t   g_notice_key = PTHREAD_KEYS_MAX; 

static pthread_once_t  g_lclog_once=PTHREAD_ONCE_INIT;



static void lc_delete_lognoticemem(void* pvoid)
{
    pnotice_info_t pnotice;
    pnotice = (pnotice_info_t)pvoid;
	if(pnotice != NULL)
	{
		
		free(pnotice);
	}
}



static void lc_delete_logtimemem(void* pvoid)
{
	struct timeval *plasttime;
    plasttime = (struct timeval*)pvoid;
	if(plasttime != NULL)
	{
		
		free(plasttime);
	}
}



static void gen_key()
{
	pthread_key_create(&g_time_key, lc_delete_logtimemem);
	pthread_key_create(&g_notice_key, lc_delete_lognoticemem);
}


int notice_specific_init()
{
	pnotice_info_t pnotice;
	struct timeval *plasttime;
    pnotice = (pnotice_info_t)pthread_getspecific(g_notice_key);
	if(NULL == pnotice)
	{
		pnotice = (pnotice_info_t)calloc(sizeof(notice_info_t), 1);
		if(NULL == pnotice)
			return -1;
		pnotice->extra_cur_len = 0;
	}
	pthread_setspecific(g_notice_key, (const void*)pnotice);
    plasttime = (struct timeval*)pthread_getspecific(g_time_key);
    if (NULL == plasttime) 
    {
        plasttime = (struct timeval*)calloc(sizeof(struct timeval), 1);
        if (NULL == plasttime)
        {
            free(pnotice);
            return -1;
        }
    }
    gettimeofday(plasttime, NULL);
    pthread_setspecific(g_time_key, (const void*)plasttime);

    return 0;
}
int lc_log_init(const char* log_path, const char* log_file,
		int max_size, int log_level, int to_syslog, int spec, Ul_log_self *self)
{
	char real_path[256];
	char real_file[256];
	
	pthread_once(&g_lclog_once, gen_key);

	if(NULL == log_path){
		fprintf(stderr, "log_path parameter error.\n");
		return -1;
	}
	if(NULL == log_file){
		fprintf(stderr, "log_file parameter error.\n");
		return -1;
	}
	
	log_stat.events		= log_level;
	log_stat.to_syslog	= to_syslog;
	log_stat.spec		= spec;

	
	if('\0' == log_path[0]){
		snprintf(real_path, sizeof(real_path), "%s", "./conf/");
	}
	else{
        snprintf(real_path, sizeof(real_path), "%s", log_path);
	}

	if('\0' == log_file[0]){
        snprintf(real_file, sizeof(real_file), "%s", "lc_log.");
	}
	else{
        snprintf(real_file, sizeof(real_file), "%s", log_file);
	}

	
	if (0 != ul_openlog(log_path, log_file, &log_stat, max_size, self)) {
		fprintf(stderr, "open log[path=%s file=%s level=%d size=%d] failed, "
				"file[%s] line[%d] error[%m]\n",
				log_path, log_file, log_level, max_size,
				__FILE__, __LINE__);
		return -1;
	}
    return notice_specific_init();
}

int lc_open_comlog(const char *procname, com_device_t *dev, int dev_num, com_logstat_t *logstat){
	pthread_once(&g_lclog_once, gen_key);
	if(0 != com_openlog(procname, dev, dev_num, logstat) ){
		fprintf(stderr, "lc_open_comlog failed...");
		return -1;
	}
    return notice_specific_init();
}

int lc_load_comlog(const char *path, const char *file){
	pthread_once(&g_lclog_once, gen_key);
	if(0 != com_loadlog(path, file)){
		fprintf(stderr, "lc_load_comlog failed...");
		return -1;
	}
    return notice_specific_init();
}

int lc_log_initthread(const char* thread_info, Ul_log_self *self)
{
	pthread_once(&g_lclog_once, gen_key);
	if(NULL == thread_info)
		return -1;
	
	if (ul_openlog_r(thread_info, &log_stat, self) < 0)
	{
		fprintf(stderr, "open log in thread failed, "
				"error[%m], file[%s] line[%d]\n",
				__FILE__, __LINE__);
		return -1;
	}
    return notice_specific_init();
}


void lc_log_closethread()
{
	ul_closelog_r(0);
}


void lc_log_close()
{
	pnotice_info_t pnotice;
	struct timeval *plasttime;
	pnotice = (pnotice_info_t)pthread_getspecific(g_notice_key);
	plasttime = (struct timeval*)pthread_getspecific(g_time_key);
	lc_delete_logtimemem(plasttime);
	lc_delete_lognoticemem(pnotice);
	pthread_setspecific(g_notice_key, NULL);
	pthread_setspecific(g_time_key, NULL);
	ul_closelog(0);
}


int lc_log_clearnotice()
{
	pnotice_info_t pnotice;

	pnotice = (pnotice_info_t)pthread_getspecific(g_notice_key);
	if(NULL == pnotice)
	{
		return -1;
	}
	pnotice->logid_str[0] = '\0';
	pnotice->processtime_str[0] = '\0';
	pnotice->requestip_str[0] = '\0';
	pnotice->requestserver_str[0] = '\0';
	pnotice->errno_str[0] = '\0';
	pnotice->cmd_str[0] = '\0';
	pnotice->extra_cur_len = 0;

	return 0;
}

unsigned int lc_log_getussecond()
{
	struct timeval curtime;
	struct timeval *plasttime;
	long totaltimes;
	long totaltimeus;

	plasttime = (struct timeval*)pthread_getspecific(g_time_key);
	if(plasttime == NULL)
	{
		return 0;
	}
	gettimeofday(&curtime,NULL);
	totaltimes = curtime.tv_sec - plasttime->tv_sec;
	totaltimeus = curtime.tv_usec - plasttime->tv_usec;

	*plasttime = curtime;
	pthread_setspecific(g_time_key, (const void*)plasttime);
	return totaltimes * 1000000 + totaltimeus;
}


static int
my_strncat(char *dest, const char *src, size_t n)
{
	char *d, *end;
	d = dest;
	end = dest+n-1;

	for(; d < end  && *d != '\0'; d++);

	for(;d<end;d++,src++){
		if(!(*d = *src)){
			return 0;
		}
	}
	return -1;
}


static int
my_strnlen(char *str, size_t len)
{
	char *d, *end;
	u_int curlen = 0;
	end = str+len-1;

	d = str;
	if(NULL == str)
		return 0;

	for(;curlen<len && d<=end &&*d!= '\0';d++,curlen++);
	return curlen;
}


unsigned int lc_log_pushnotice(const char* key, const char* valuefmt, ...)
{
	char buff[NOTICE_INFO_MAXLEN];
	char buffvalue[NOTICE_INFO_MAXLEN];
	pnotice_info_t pnotice;
	size_t infolen = 0;
	va_list args;
	if(valuefmt == NULL || NULL == key)
		return 0;

	
	buff[0] = '\0';
	buffvalue[0] = '\0';
	my_strncat(buff, key, sizeof(buff));
	if(-1 == my_strncat(buff, ":", sizeof(buff)))
	{
		return 0;
	}
	buff[NOTICE_INFO_MAXLEN - 1] = '\0';

	
	va_start(args, valuefmt);
	vsnprintf(buffvalue, NOTICE_INFO_MAXLEN, valuefmt, args);
	buffvalue[sizeof(buffvalue) - 1] = '\0';
	va_end(args);
	if(-1 == my_strncat(buff, buffvalue, sizeof(buff)))
	{
		return 0;
	}
	if(-1 == my_strncat(buff, " ", sizeof(buff)))
	{
		return 0;
	}

	buff[NOTICE_INFO_MAXLEN - 1] = '\0';

	pnotice = (pnotice_info_t)pthread_getspecific(g_notice_key);
	if(NULL == pnotice)
	{
		return 0;
	}

	pnotice->extra_notice_str[pnotice->extra_cur_len] = '\0';
	if(my_strncat(&pnotice->extra_notice_str[pnotice->extra_cur_len], buff, \
				sizeof(pnotice->extra_notice_str)-pnotice->extra_cur_len) == 0)
	{
		infolen = my_strnlen(buff, NOTICE_INFO_MAXLEN);
		pnotice->extra_cur_len += infolen;
	}
	else
	{
		pnotice->extra_notice_str[pnotice->extra_cur_len] = '\0';
	}

	return infolen;
}

unsigned int lc_log_setlogid(unsigned int logid)
{
	pnotice_info_t pnotice;

	pnotice = (pnotice_info_t)pthread_getspecific(g_notice_key);
	if(NULL == pnotice)
	{
		return 0;
	}

	pnotice->logid = logid;
	return logid;
}

unsigned int lc_log_getlogid()
{
	pnotice_info_t pnotice;

	pnotice = (pnotice_info_t)pthread_getspecific(g_notice_key);
	if(NULL == pnotice)
	{
		return 0;
	}

	return pnotice->logid;
}

unsigned int lc_log_clearlogid()
{
	return lc_log_setlogid(0);
}

unsigned int lc_log_setbasic(lc_notice_type type, const char* valuefmt, ...)
{
	char buff[NOTICE_INFO_MAXLEN];
	pnotice_info_t pnotice;
    size_t ret = 0;
	va_list args;

	va_start(args, valuefmt);
	vsnprintf(buff, NOTICE_INFO_MAXLEN, valuefmt, args);
	buff[sizeof(buff) - 1] = '\0';
	va_end(args);

	if(type <= LC_LOG_ZERO || type >= LC_LOG_END)
	{
		return 0;
	}

	pnotice = (pnotice_info_t)pthread_getspecific(g_notice_key);
	if(NULL == pnotice)
	{
		return 0;
	}

	switch(type){
		case LC_LOG_LOGID:
			strncpy(pnotice->logid_str, buff ,sizeof(pnotice->logid_str) - 1);
			pnotice->logid_str[sizeof(pnotice->logid_str) - 1] = '\0';
			ret = strlen(pnotice->logid_str);
            break;

		case LC_LOG_PROCTIME:
			strncpy(pnotice->processtime_str, buff ,sizeof(pnotice->processtime_str) - 1);
			pnotice->processtime_str[sizeof(pnotice->processtime_str) - 1] = '\0';
            ret = strlen(pnotice->processtime_str);
			break;

		case LC_LOG_REQIP:
			strncpy(pnotice->requestip_str, buff ,sizeof(pnotice->requestip_str) - 1);
			pnotice->requestip_str[sizeof(pnotice->requestip_str) - 1] = '\0';
            ret = strlen(pnotice->requestip_str);
			break;

		case LC_LOG_REQSVR:
			strncpy(pnotice->requestserver_str, buff ,sizeof(pnotice->requestserver_str) - 1);
			pnotice->requestserver_str[sizeof(pnotice->requestserver_str) -1] = '\0';
            ret = strlen(pnotice->requestserver_str);
			break;

		case LC_LOG_ERRNO:
			strncpy(pnotice->errno_str, buff ,sizeof(pnotice->errno_str) - 1);
			pnotice->errno_str[sizeof(pnotice->errno_str) -1] = '\0';
            ret = strlen(pnotice->errno_str);
			break;

		case LC_LOG_CMDNO:
			strncpy(pnotice->cmd_str, buff ,sizeof(pnotice->cmd_str) - 1);
			pnotice->cmd_str[sizeof(pnotice->cmd_str) - 1] = '\0';
            ret = strlen(pnotice->cmd_str);
			break;

		case LC_LOG_SVRNAME:
			strncpy(pnotice->svrname_str, buff ,sizeof(pnotice->svrname_str) - 1);
			pnotice->svrname_str[sizeof(pnotice->svrname_str) - 1] = '\0';
            ret = strlen(pnotice->svrname_str);
			break;

		default:
			return 0;
	}
	return ret;
}


char* lc_log_getbasic(lc_notice_type type)
{
	pnotice_info_t pnotice;
	if(type <= LC_LOG_ZERO || type >= LC_LOG_END)
	{
		return "";
	}
	pnotice = (pnotice_info_t)pthread_getspecific(g_notice_key);
	if(NULL == pnotice)
	{
		return "";
	}

	switch(type){
		case LC_LOG_LOGID:
			return pnotice->logid_str;

		case LC_LOG_PROCTIME:
			return pnotice->processtime_str;

		case LC_LOG_REQIP:
			return pnotice->requestip_str;

		case LC_LOG_REQSVR:
			return pnotice->requestserver_str;

		case LC_LOG_ERRNO:
			return pnotice->errno_str;

		case LC_LOG_CMDNO:
			return pnotice->cmd_str;

		case LC_LOG_SVRNAME:
			return pnotice->svrname_str;
		default:
			return "";
	}
	return "";
}


char* lc_log_popnotice()
{
	pnotice_info_t pnotice;
	pnotice = (pnotice_info_t)pthread_getspecific(g_notice_key);
	if(NULL == pnotice)
		return "";

	if(pnotice->extra_cur_len >= NOTICE_INFO_MAXLEN)
		pnotice->extra_cur_len = NOTICE_INFO_MAXLEN - 1;

	pnotice->extra_notice_str[pnotice->extra_cur_len] = '\0';
	pnotice->extra_cur_len = 0;

	return pnotice->extra_notice_str;
}

void lc_log_keyonce(){
	pthread_once(&g_lclog_once, gen_key);
}
