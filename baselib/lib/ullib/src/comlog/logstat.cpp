
#include <sys/time.h>

#include "logstat.h"
#include "comlog.h"
#include "namemg.h"
#include "xutils.h"
#include "logcore.h"

namespace comspace
{

LogStat::LogStat() 
{
	_conf_file[0] = 0;
	_level = (1<<COMLOG_FATAL) | (1<<COMLOG_WARNING) | 
		(1<<COMLOG_NOTICE) | (1<<COMLOG_TRACE) | (1<<COMLOG_DEBUG);
	_tty = 0;
}

int LogStat::setLevel(u_int sysmask, char *userlevel)
{
	char selflvl[COM_SELFLEVELNUM][COM_MAXLEVELNAMESIZE];
	int ret =0;
	if (userlevel != NULL) {
		ret = xparserSelfLog(userlevel, selflvl, sizeof(selflvl)/sizeof(selflvl[0]));
	} else {
	}
	unsigned long long level = sysmask;	//出于线程安全得考虑
	for (int i=0; i<ret; ++i) {
		u_int id = getLogId(selflvl[i]);
		if (id >= COM_USRLEVELSTART) {
			level |= (((unsigned long long)1)<<id);
		}
	}
	if (ret == 0) {
		for (u_int i=COM_USRLEVELSTART; i<COM_LEVELNUM; ++i) {
			level |= (((unsigned long long)1)<<i);
		}
	}

	_level = level;
	_com_debug("set level[%llx]", _level);
	return 0;
}

LogStat *LogStat::createLogStat()
{
	LogStat *logstat = new LogStat();
	char name[1024];
	do {
		timeval tv;
		gettimeofday(&tv, NULL);
		snprintf(name, sizeof(name), "%ld%ld%d", tv.tv_sec, tv.tv_usec, rand());
	} while (comlog_get_nameMg()->set(name, logstat) != 0);//没有设置成功可能已经有句柄重名
	return logstat;
}

static LogStat * g_def_logstat = NULL;
LogStat * comlog_get_def_logstat() {
	return g_def_logstat;
}
void comlog_set_def_logstat(LogStat *st) {
	g_def_logstat = st;
}

}
/* vim: set ts=4 sw=4 sts=4 tw=100 */
