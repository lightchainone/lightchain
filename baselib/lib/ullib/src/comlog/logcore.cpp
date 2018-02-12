
#include <pthread.h>

#include "logcore.h"
#include "namemg.h"
#include "xutils.h"
#include "logstat.h"
#include "event.h"
#include "sendsvr/sendsvr.h"

extern pthread_key_t* comlog_key();
namespace comspace
{

extern pthread_key_t* event_key();
static char g_levelName[COM_LEVELNUM][COM_MAXLEVELNAMESIZE] = 
{
	"", //0
	"FATAL",	//1
	"WARNING", 	//2
	"",	//3
	"NOTICE", 	//4
	"",	//5
	"",	//6
	"",	//7
	"TRACE",	//8
	"",	//9
	"",	//10
	"",	//11
	"",	//12
	"",	//13
	"",	//14
	"",	//15
	"DEBUG",	//16
};

static u_int g_seflog = COM_USRLEVELSTART;
static bool g_is_init_log = false;
static pthread_mutex_t g_seflog_lock = PTHREAD_MUTEX_INITIALIZER;

const char *getLogName(u_int logid)
{
	return g_levelName[logid%COM_LEVELNUM];
}

u_int getLogId(const char *logname)
{
	for (u_int i=0; i<g_seflog; ++i) {
		if (strcmp(logname, g_levelName[i]) == 0) {
			return i;
		}
	}
	return 0;
}

int createSelfLog(const char *lvlname)
{
	xAutoLock lock(&g_seflog_lock);

	int id = getLogId(lvlname);
	if (id != 0) return id;

	if (g_seflog >= COM_LEVELNUM) {
		return 0;
	}

	snprintf(g_levelName[g_seflog], sizeof(g_levelName[g_seflog]), "%s", lvlname);
	
	return g_seflog++;
}


int init_log()
{
	xAutoLock lock(&g_seflog_lock);

	if (g_is_init_log) {
		return 0;
	}

	int ret = comlog_get_nameMg()->create();
	if (ret != 0) {
		_com_error("create name manage err");
		goto fail;
	}
	if (comlog_get_def_logstat() == NULL) {
//		g_def_logstat = LogStat::createLogStat();
		comlog_set_def_logstat(LogStat::createLogStat());
		if (comlog_get_def_logstat() == NULL) {
			_com_error("create def logstat err");
			goto fail1;
		}
	}

	g_is_init_log = true;
	return 0;
fail1:
	comlog_get_nameMg()->destroy();
fail:
	return -1;
}

bool is_init_log() 
{
	xAutoLock lock(&g_seflog_lock);
	return g_is_init_log;
}


int com_wait_close(int waittime)
{
	xAutoLock lock(&g_seflog_lock);
    if (waittime == -1) {
        waittime = INT_MAX;
    }

	if (waittime > 0) {
		//等待网络日志发完
		SendSvr::comlog_get_gcheckSvr()->checkOver(waittime);
	}
	SendSvr::comlog_get_gcheckSvr()->clear();
	return 0;
}

int close_log(int waittime){
	xAutoLock lock(&g_seflog_lock);
	comlog_get_nameMg()->destroy();
	pthread_key_t * key = comlog_key();
	com_thread_data_t *ptr = (com_thread_data_t *)pthread_getspecific(*key);
	if(ptr != NULL){
		delete ptr;
		pthread_setspecific(*key, NULL);
	}
	key = event_key();
	Event * evt = (Event*)pthread_getspecific(*key);
	if(evt != NULL){
		delete evt;
		pthread_setspecific(*key, NULL);
	}
	g_is_init_log = false;
//	g_def_logstat = NULL;
	comlog_set_def_logstat(NULL);
	return 0;
}

int reset_log()
{
	comlog_get_nameMg()->destroy();
	comlog_del_nameMg();
	g_is_init_log = false;
	comlog_set_def_logstat(NULL);

	return 0;
}

int com_close_appender(){
	return comlog_get_nameMg()->closeAppender();
}

int com_reset_appender(){
	return comlog_get_nameMg()->resetAppender();
}
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
