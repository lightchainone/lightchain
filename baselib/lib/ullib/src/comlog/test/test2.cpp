#include "com_log_if.h"

int main()
{
	//设置接受的日志等级
	com_logstat_t logstat;
	logstat.sysevents = 16;
	snprintf(logstat.userevents, sizeof(logstat.userevents), "xiaowei,faint");
	//创建设备
	com_device_t dev[3];
	//本地日志
	snprintf(dev[0].host, sizeof(dev[0].host), "log");
	snprintf(dev[0].file, sizeof(dev[0].file), "test2.log");
	//dev[0].type = COM_ULLOGDEVICE;
	strcpy(dev[0].type, "ULLOG");
	dev[0].log_size = 2048;
	COMLOG_SETSYSLOG(dev[0]);
	//设置自定义日志支持xiaow和faint日志
	set_comdevice_selflog(dev, "xiaowei,faint");

	//网络日志
	snprintf(dev[1].host, sizeof(dev[1].host), "127.0.0.1:2121/log");
	snprintf(dev[1].file, sizeof(dev[1].file), "test2.log");
	//dev[1].type = COM_ULNETDEVICE;
	strcpy(dev[1].type, "ULNET");
	COMLOG_SETSYSLOG(dev[1]);
	set_comdevice_selflog(dev+1, "xiaowei,faint");

	//tty日志
	//dev[2].type = COM_TTYDEVICE;
	strcpy(dev[2].type, "TTY");
#if 1
	//打开设备
	int ret = com_openlog("test2", dev, 2, &logstat);
	if (ret != 0) {
		fprintf(stderr, "open log err");
		return -1;
	}
#endif
	com_writelog(COMLOG_NOTICE, "hello comlog");

	com_closelog();
	return 0;
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
