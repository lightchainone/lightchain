
#include "com_log_if.h"

int PNUM = 50;
int NUM = 1000;

static void * _run_ (void *)
{
	com_logstat_t logstat;
	logstat.sysevents = 1;
	for (int i=0; i<NUM; ++i) {
		com_writelog(COMLOG_NOTICE, "open-r that.s[%ld] - %d111111111111111111111111111111111111111111111111111111111111111111111111111", (long)pthread_self(), i);
		com_writelog(COMLOG_WARNING, "open-r that.s[%ld] - %d111111111111111111111111111111111111111111111111111111111111111111111111111", (long)pthread_self(), i);
		com_writelog("xiaowei", "open-r that.s[%ld] - %d111111111111111111111111111111111111111111111111111111111111111111111111111", (long)pthread_self(), i);
		//com_changelogstat(&logstat);
		com_changelogstat();

	}
	return NULL;
}


int main(int argc, char **argv)
{
	char *filename = "ptest.conf";
	if(argc > 1) filename = argv[1];
	if(argc > 2) sscanf(argv[2], "%d", &PNUM);
	if(argc > 3) sscanf(argv[3], "%d", &NUM);
	if(PNUM <= 0 || PNUM > 1000)PNUM = 1;
	if(NUM <= 0 || NUM > 1000000) NUM = 1;
	fprintf(stderr, "filename = %s, thread num = %d, log num = %d\n", filename, PNUM, NUM);
	int ret = 0;
	ret = com_loadlog("./", filename);
	if (ret != 0) {
		fprintf(stderr, "load err\n");
		return -1;
	}
	com_reglogstat("./", filename);
	pthread_t pid[PNUM];
	timeval s, e;
	gettimeofday(&s, NULL);
	for (int i=0; i<PNUM; ++i) {
		pthread_create(pid+i, NULL, _run_, NULL);
		com_writelog(COMLOG_NOTICE, "creat %d", i);
	}
	for (int i=0; i<PNUM; ++i) {
		pthread_join(pid[i], NULL);
	}
	gettimeofday(&e, NULL);
	int a = (e.tv_sec-s.tv_sec)*1000 + (e.tv_usec-s.tv_usec)/1000;
	float b = (float)(PNUM * NUM)/(float)(a);
	fprintf(stderr, "const time[%d]ms, speed[%f]/ms\n", a, b);
	com_closelog();
	return 0;
}


















/* vim: set ts=4 sw=4 sts=4 tw=100 */
