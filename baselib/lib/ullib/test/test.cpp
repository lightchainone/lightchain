
#include "com_log.h"
#include "comlog/appender/fileappender.h"
#include "ul_log.h"
#include <unistd.h>
namespace comspace{
	extern int __log_switcher__;
}

int main(){
	ul_openlog(NULL, NULL, NULL, 1024, NULL);
	ul_writelog(UL_LOG_WARNING, "0 - UL");
	com_writelog(UL_LOG_NOTICE, "0 - COM");
	ul_closelog(0);
	//comspace::__log_switcher__ = 2;
	int i;

	int ret;
	com_logstat_t logstat;

	logstat.sysevents = 16;
	snprintf(logstat.userevents, sizeof(logstat.userevents), "xiaowei,faint");

	com_device_t dev[1];
	snprintf(dev[0].host, sizeof(dev[0].host), "log");
	snprintf(dev[0].file, sizeof(dev[0].file), "test.log");
	strcpy(dev[0].type, "FILE");
	dev[0].log_size = 2048;
	dev[0].splite_type = COM_DATECUT;
	dev[0].cuttime = 1;
	COMLOG_SETSYSLOG(dev[0]);
	for(i = 0; i < 1; i++){
		//com_loadlog("./", "test.conf");
		ret = com_openlog("comtest", dev, 1, &logstat);
		//		fprintf(stderr, "[%03d]ret = %d\n", i, ret);

		for(int j = 0; j < 1000; j++){
			ul_writelog(COMLOG_WARNING, "1 - UL [%d/%d]", i, j);
			com_writelog(UL_LOG_FATAL, "1 - COM [%d/%d]", i, j);
			sleep(1);
		}
		com_closelog(0);
	}
	if(fork() == 0){
		com_closelog(1000);
		com_openlog("newproc", dev, 1, &logstat);
		for(i = 0; i < 10; i++){
			ul_writelog(COMLOG_WARNING, "1 - UL [%d]", i);
			com_writelog(UL_LOG_FATAL, "1 - COM [%d]", i);
		}

	}
	/*	
		for(i = 0; i < 2000; i++){
		char s[10];
		memset(s, 0, sizeof(s));
		snprintf(s,10, "A%03d", i);
		int ret = com_registappender(s, comspace::FileAppender::getAppender, comspace::FileAppender::tryAppender);
		fprintf(stderr, "[%03d]ret = %d\n", i, ret);
		}
		*/

	//comspace::__log_switcher__ = 1;
	ul_writelog(COMLOG_WARNING, "2 - UL");
	com_writelog(UL_LOG_NOTICE, "2 - COM");
	return 0;
}















/* vim: set ts=4 sw=4 sts=4 tw=100 noet: */
