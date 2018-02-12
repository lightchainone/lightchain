
#include "com_log.h"
#include "comlog/appender/fileappender.h"
#include "ul_log.h"
namespace comspace{
extern int __log_switcher__;
}

int main(){
	ul_writelog(0, "0 - UL");
	com_writelog(0, "0 - COM");

	//comspace::__log_switcher__ = 2;
	int i;
#if 0
	int ret;
	com_logstat_t logstat;

	logstat.sysevents = 16;
	snprintf(logstat.userevents, sizeof(logstat.userevents), "xiaowei,faint");

	com_device_t dev[1];
	snprintf(dev[0].host, sizeof(dev[0].host), "log");
	snprintf(dev[0].file, sizeof(dev[0].file), "test.log");
	strcpy(dev[0].type, "FILE");
	dev[0].log_size = 2048;
	COMLOG_SETSYSLOG(dev[0]);
#endif
	com_loadlog(".", "test_others.conf");
	for(i = 0; i < 300; ++i){
		com_writelog(COMLOG_WARNING, "HAHA");
	//	sleep(1);
	}
	com_closelog(1000);
#if 0
	for(i = 0; i < 1; i++){
		//com_loadlog("./", "test.conf");
		ret = com_openlog("name_1", dev, 1, &logstat);
		fprintf(stderr, "[%03d]ret = %d\n", i, ret);
		ul_writelog(COMLOG_WARNING, "1 - UL [%d]", i);
		com_writelog(UL_LOG_FATAL, "1 - COM [%d]", i);
		com_closelog(0);
	}
#endif
#if 0	
	for(i = 0; i < 2000; i++){
		char s[10];
		memset(s, 0, sizeof(s));
		snprintf(s,10, "A%03d", i);
		int ret = com_registappender(s, comspace::FileAppender::getAppender, comspace::FileAppender::tryAppender);
		fprintf(stderr, "[%03d]ret = %d\n", i, ret);
	}
	comspace::__log_switcher__ = 1;
	ul_writelog(COMLOG_WARNING, "2 - UL");
	com_writelog(UL_LOG_NOTICE, "2 - COM");
#endif
	return 0;
}















/* vim: set ts=4 sw=4 sts=4 tw=100 noet: */
