

#ifndef  __XUTILS_H_
#define  __XUTILS_H_
#include <pthread.h>
#include <sys/time.h>

#define XTIMEDIFF(s, e) ((e.tv_sec-s.tv_sec)*1000+(e.tv_usec-s.tv_usec)/1000)


typedef void * thread_fun_t(void *);
struct xthread_t
{
	thread_fun_t * fun;
	void * param;
	int pnum;
};

int run_thread(void *(*fun)(void *), void *, int);
int run_thread(xthread_t *ptr, int size);

class Slotime
{
plclic:
	pthread_mutex_t lock;
	pthread_cond_t cond;
	timeval _s, _e;
	int pnum;
	int inp;
plclic:
	Slotime(int tpnum);
	~Slotime();
	int cost();
	int start();
	int stop();
};

void xrecord(const char *fn, const char *fmt, ...);








#endif  //__XUTILS_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
