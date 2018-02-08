
#include "com_log.h"
#include "comlog/appender/fileappender.h"
#include "netcpappender.h"
#include "netcpappendersvr.h"
#include "ul_log.h"
#include <unistd.h>

int main(){
	int i;

	int ret;
	com_logstat_t logstat;
	signal(SIGPIPE,SIG_IGN);
	com_registappender("NETCP", comspace::NetcpAppender::getAppender, comspace::NetcpAppender::tryAppender);
	logstat.sysevents = 16;
	snprintf(logstat.userevents, sizeof(logstat.userevents), "xiaowei,faint");

	com_device_t dev[1];
	snprintf(dev[0].host, sizeof(dev[0].host), "127.0.0.1:9898/0001 127.0.0.1:9897");
	snprintf(dev[0].file, sizeof(dev[0].file), "netcpNoBakup");
	strcpy(dev[0].type, "NETCP");
	strcpy(dev[0].reserved1, "TRUE");
	dev[0].log_size = 2048;
	dev[0].splite_type = COM_DATECUT;
	dev[0].cuttime = 1;
	COMLOG_SETSYSLOG(dev[0]);
	for(i = 0; i < 1; i++){
		
		ret = com_openlog("comtest", dev, 1, &logstat);
		fprintf(stderr, "[openlog]ret = %d\n", ret);
		ret = comspace::NetcpAppenderSvr::setMaxRetry(5);
		fprintf(stderr, "[MaxRetry]ret = %d\n", ret);
		for(int j = 0; j < 30; j++){
			fprintf(stderr, "Gonna print log idx = %d\n", j);
			ul_writelog(COMLOG_WARNING, "1 - UL [%d/%d]", i, j);
			com_writelog(UL_LOG_FATAL, "1 - COM [%d/%d]", i, j);
			sleep(1);
		}
		fprintf(stderr, "I'm Gonna sleep...\n", ret);
		sleep(10);
		fprintf(stderr, "Gonna closelog\n", ret);
		com_closelog(1000);
	}
	return 0;
}
















