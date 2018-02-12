
#include <ul_log.h>


#define PNUM  5
#define NUM 1000000

ul_logstat_t logstat;
static void * _run_ (void *)
{
	ul_openlog_r("ultest", &logstat);
	for (int i=0; i<NUM; ++i) {
		ul_writelog(UL_LOG_NOTICE, "open-r that.s[%ld] - %d111111111111111111111111111111111111111111111111111111111111111111111111111", (long)pthread_self(), i);
	}
	return NULL;
}

int main()
{
	memset(&logstat, 0, sizeof(logstat));
	logstat.events = 16;
	int ret = ul_openlog("./log/", "ultest.", &logstat, 2000);
	if (ret != 0) {
		printf("stderr");
		return -1;
	}
	pthread_t pid[PNUM];
	timeval s, e;
	gettimeofday(&s, NULL);
	for (int i=0; i<PNUM; ++i) {
		pthread_create(pid+i, NULL, _run_, NULL);
	}
	for (int i=0; i<PNUM; ++i) {
		pthread_join(pid[i], NULL);
	}
	gettimeofday(&e, NULL);
	int a = (e.tv_sec-s.tv_sec)*1000 + (e.tv_usec-s.tv_usec)/1000;
	float b = (float)(PNUM * NUM)/(float)(a);
	fprintf(stderr, "const time[%d]ms, speed[%f]/ms\n", a, b);
	return 0;
}




/* vim: set ts=4 sw=4 sts=4 tw=100 */
