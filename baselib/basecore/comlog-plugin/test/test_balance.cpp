

#include "com_log.h"
#include "comlog/appender/fileappender.h"
#include "netcpappender.h"
#include "netcpappendersvr.h"
#include "ul_log.h"
#include <unistd.h>

int main(){
	int ret;
	com_logstat_t logstat;
	signal(SIGPIPE,SIG_IGN);
	com_registappender("NETCP", comspace::NetcpAppender::getAppender, comspace::NetcpAppender::tryAppender);
	logstat.sysevents = 16;
	snprintf(logstat.userevents, sizeof(logstat.userevents), "xiaowei,faint");
	com_device_t dev[1];
	snprintf(dev[0].host, sizeof(dev[0].host), "127.0.0.1:9898/BalanceTest 127.0.0.1:9897");
	snprintf(dev[0].file, sizeof(dev[0].file), "Balance.log");
	strcpy(dev[0].type, "NETCP");
	strcpy(dev[0].reserved1, "TRUE");
	dev[0].log_size = 2048;
	dev[0].splite_type = COM_DATECUT;
	dev[0].cuttime = 1;
	COMLOG_SETSYSLOG(dev[0]);

	ret = comspace::NetcpAppenderSvr::setMaxRetry(5);
	comspace::NetcpAppenderSvr::setSwitch(10);
	comspace::NetcpAppenderSvr::setSwitchTime(5);
	ret = com_openlog("comtest", dev, 1, &logstat);
	fprintf(stderr, "[openlog]ret = %d\n", ret);

	for(int i = 0; i < 200; ++i){
		ul_writelog(COMLOG_WARNING, "Test SetSwitch [%d]", i);
		sleep(1);
	}

	for(int i = 0; i < 20; ++i){
		ul_writelog(COMLOG_FATAL, "Test setSwitchTime [%d]", i);
		sleep(2);
	}

	fprintf(stderr, "ok....");
	return 0;
}





















	
