
#include "com_log.h"
#include "signal.h"
#include "xutils.h"

int main(int argc, char **argv)
{
	signal(SIGPIPE, SIG_IGN);
	int ret = com_loadlog("./", "test.conf");
	if (ret != 0) {
		fprintf(stderr, "load err\n");
		return -1;
	}
	//com_device_t dev;
	//COMLOG_CLEARMASK(dev);
	//dev.type = COM_TTYDEVICE;
	//com_setdevices(&dev, 1, "faint");
	//int ft = com_getlogid("faint");
	//fprintf(stderr, "get logid[%d]\n", ft);
	//fprintf(stderr, "______________________start________________________\n");
	int num = 10;
	timeval s, e;
	com_logstat_t logstat;
	logstat.sysevents = 1;

	com_reglogstat("./", "test.conf");

	gettimeofday(&s, NULL);
	for (int i=0; i<num; ++i) {
		com_writelog(COMLOG_NOTICE, "hello comlog1v1");
		com_pushlog("a", "%d", 1)("b", "%d", 2)("name", "xiaowei")(COMLOG_NOTICE, "hello comlog1v1");
		com_writelog(COMLOG_DEBUG, "hello debughello comlog1v1");
		com_writelog(COMLOG_WARNING, "this is warning notice");
		com_writelog("xiaowei", "this is xiaowei speakinghello comlog1v1");
		com_pushlog("now_cnt", "%d", (i+1)*5)(COMLOG_WARNING, "thathello comlog1v1");
		com_changelogstat(&logstat);
		//com_changelogstat();
		//com_writelog(ft, "faint id");
		//sleep(1);
	}
	com_writelog(COMLOG_NOTICE, "END");
	gettimeofday(&e, NULL);
	int a = (e.tv_sec-s.tv_sec)*1000 + (e.tv_usec-s.tv_usec)/1000;
	float b = (float)(num*5)/(float)(a);
	//sleep(5);
	
	gettimeofday(&s, NULL);
	com_closelog(60000);
	gettimeofday(&e, NULL);
	int c = (e.tv_sec-s.tv_sec)*1000 + (e.tv_usec-s.tv_usec)/1000;

	fprintf(stderr, "%s: const time[%d]ms, speed[%f]/ms sleep[%d]ms \n", "", a, b, c);

	char xmfttest[3];
	ret = comspace::xfmtime(xmfttest, sizeof(xmfttest), e);
	com_writelog("xiaowei", "%s ret=%d", xmfttest, ret);

	//ret = comspace::save_backup("./0", xmfttest, sizeof(xmfttest));
	com_writelog("xiaowei", "%s ret=%d", xmfttest, ret);

	return 0;
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
