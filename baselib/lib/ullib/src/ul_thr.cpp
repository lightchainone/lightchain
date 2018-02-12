//**********************************************************
//          Wrap system nornal function Utility 1.0
//
//  Description:
//    This utility defines thread functions which were wraped.
//**********************************************************

/* include area */
#include "ul_thr.h"
#include "ul_log.h"

//*******************************************************
// Wrap function about pthread_attr_init()
//*******************************************************
int	ul_pthread_attr_init(pthread_attr_t *attr)
{
	int	n;

	if ((n = pthread_attr_init(attr))) {
		errno = n;
		ul_writelog(UL_LOG_WARNING, "pthread_attr_init() call failed.error[%d] info is %s.",
        errno,strerror(errno));
	}
	return n;
}

//*******************************************************
// Wrap function about pthread_attr_destroy()
//*******************************************************
int	ul_pthread_attr_destroy(pthread_attr_t *attr)
{
	int	n;

	if ((n = pthread_attr_destroy(attr))) {
		errno = n;
		ul_writelog(UL_LOG_WARNING, "pthread_attr_destroy() call failed.error[%d] info is %s.",
        errno, strerror(errno));
	}
	return n;
}

//*******************************************************
// Wrap function about pthread_attr_setdetachstate()
//*******************************************************
int	ul_pthread_attr_setdetachstate(pthread_attr_t *attr, int detach)
{
	int	n;

	if ((n = pthread_attr_setdetachstate(attr, detach))) {
		errno = n;
		ul_writelog(UL_LOG_WARNING, "pthread_attr_setdetachstate() call failed.error[%d] info is %s.",
        errno,strerror(errno));
	}
	return n;
}

//*******************************************************
// Wrap function about pthread_attr_setscope()
//*******************************************************
int	ul_pthread_attr_setscope(pthread_attr_t *attr, int scope)
{
	int	n;

	if ((n = pthread_attr_setscope(attr, scope))) {
		errno = n;
		ul_writelog(UL_LOG_WARNING, "pthread_attr_setscope(%d) call failed.error[%d] info is %s.",
        scope, errno, strerror(errno));
	}
	return n;
}

//*******************************************************
// Wrap function about pthread_create()
//*******************************************************
int ul_pthread_create(pthread_t *tid, const pthread_attr_t *attr,
			   void * (*func)(void *), void *arg)
{
	int	n;

	if ((n = pthread_create(tid, attr, func, arg)) != 0) {
		errno = n;
		ul_writelog(UL_LOG_WARNING, "pthread_create() call failed.error[%d] info is %s.",
        errno, strerror(errno));
	}
	return n;
}

//*******************************************************
// Wrap function about pthread_join()
//*******************************************************
int	ul_pthread_join(pthread_t tid, void **status)
{
	int	n;

	if ((n = pthread_join(tid, status)) != 0) {
		errno = n;
		ul_writelog(UL_LOG_WARNING,"pthread_join(%ld) call failed.error[%d] info is %s.",
        tid, errno, strerror(errno));
	}
	return n;
}

//*******************************************************
// Wrap function about pthread_detach()
//*******************************************************
int	ul_pthread_detach(pthread_t tid)
{
	int	n;

	if ( (n = pthread_detach(tid)) != 0) {
		errno = n;
		ul_writelog(UL_LOG_WARNING,"pthread_detach(%ld) call failed.error[%d] info is %s.",
        tid, errno, strerror(errno));
	}
	return n;
}

//*******************************************************
// Wrap function about pthread_kill()
//*******************************************************
int ul_pthread_kill(pthread_t tid, int signo)
{
	int	n;

	if ((n = pthread_kill(tid, signo))) {
		errno = n;
		ul_writelog(UL_LOG_WARNING, "pthread_kill(%ld,%d) call failed.error[%d] info is %s.",
        tid, signo, errno, strerror(errno));
	}
	return n;
}

//*******************************************************
// Wrap function about pthread_self()
//*******************************************************
pthread_t ul_pthread_self(void)
{
	return (pthread_self());
}
//*******************************************************
// Wrap function about pthread_exit()
//*******************************************************
void ul_pthread_exit(void *status)
{
	pthread_exit(status);
}
//******************************************************
// Wrap function about pthread_exit()
//*******************************************************
int	ul_pthread_setcancelstate(int state, int *oldstate)
{
	int	n;

	if ((n = pthread_setcancelstate(state, oldstate))) {
		errno = n;
		ul_writelog(UL_LOG_WARNING, "pthread_setcancelstate(%d) call failed.error[%d] info is %s.",
        state, errno, strerror(errno));
	}
	return n;
}

//*******************************************************
// Wrap function about pthread_mutex_init()
//*******************************************************
int ul_pthread_mutex_init(pthread_mutex_t *mptr, pthread_mutexattr_t *attr)
{
	int	n;

	if ((n = pthread_mutex_init(mptr, attr))) {
		errno = n;
		ul_writelog(UL_LOG_WARNING, "pthread_mutex_init() call failed.error[%d] info is %s.",
        errno, strerror(errno));
	}
	return n;
}

//*******************************************************
// Wrap function about pthread_mutex_destroy()
//*******************************************************
int	ul_pthread_mutex_destroy(pthread_mutex_t *mptr)
{
	int	n;

	if ((n = pthread_mutex_destroy(mptr))) {
		errno = n;
		ul_writelog(UL_LOG_WARNING, "pthread_mutex_destroy() call failed.error[%d] info is %s.",
        errno, strerror(errno));
	}
	return n;
}

//*******************************************************
// Wrap function about pthread_mutex_lock()
//*******************************************************
int ul_pthread_mutex_lock(pthread_mutex_t *mptr)
{
	int	n;

	if ((n = pthread_mutex_lock(mptr))) {
		errno = n;
		ul_writelog(UL_LOG_WARNING, "ul_pthread_mutex_lock() call failed.error[%d] info is %s.",
        errno, strerror(errno));
	}
	return	n;
}

//*******************************************************
// Wrap function about pthread_mutex_unlock()
//*******************************************************
int	ul_pthread_mutex_unlock(pthread_mutex_t *mptr)
{
	int	n;

	if ((n = pthread_mutex_unlock(mptr))) {
		errno = n;
		ul_writelog(UL_LOG_WARNING, "pthread_mutex_unlock() call failed.error[%d] info is  %s.",
        errno, strerror(errno));
	}
	return	n;
}

//*******************************************************
// Wrap function about pthread_cond_init()
//*******************************************************
int	ul_pthread_cond_init(pthread_cond_t *cptr, pthread_condattr_t *cond_attr)
{
	int	n;

	if ((n = pthread_cond_init(cptr, cond_attr))) {
		errno = n;
		ul_writelog(UL_LOG_WARNING, "pthread_cond_init() call failed.error[%d] info is %s.",
        errno, strerror(errno));
	}
	return	n;
}

//*******************************************************
// Wrap function about pthread_cond_destroy()
//*******************************************************
int	ul_pthread_cond_destroy(pthread_cond_t *cptr)
{
	int	n;

	if ((n = pthread_cond_destroy(cptr))) {
		errno = n;
		ul_writelog(UL_LOG_WARNING, "pthread_cond_destroy() call failed.error[%d] info is %s.",
        errno, strerror(errno));
	}
	return	n;
}

//*******************************************************
// Wrap function about pthread_cond_broadcast()
//*******************************************************
int	ul_pthread_cond_broadcast(pthread_cond_t *cptr)
{
	int	n;

	if ((n = pthread_cond_broadcast(cptr))) {
		errno = n;
		ul_writelog(UL_LOG_WARNING, "pthread_cond_broadcast() call failed.error[%d] info is %s.",
        errno, strerror(errno));
	}
	return	n;
}

//*******************************************************
// Wrap function about pthread_cond_signal()
//*******************************************************
int	ul_pthread_cond_signal(pthread_cond_t *cptr)
{
	int	n;

	if ((n = pthread_cond_signal(cptr))) {
		errno = n;
		ul_writelog(UL_LOG_WARNING, "pthread_cond_signal() call failed.error[%d] info is %s.",
        errno, strerror(errno));
	}
	return	n;
}

//*******************************************************
// Wrap function about pthread_cond_wait()
//*******************************************************
int	ul_pthread_cond_wait(pthread_cond_t *cptr, pthread_mutex_t *mptr)
{
	int	n;

	if ((n = pthread_cond_wait(cptr, mptr))) {
		errno = n;
		ul_writelog(UL_LOG_WARNING, "pthread_cond_wait() call failed.error[%d] info is %s.",
        errno, strerror(errno));
	}
	return	n;
}

//*******************************************************
// Wrap function about pthread_cond_timedwait()
//*******************************************************
int ul_pthread_cond_timedwait(pthread_cond_t *cptr, pthread_mutex_t *mptr,
					   const struct timespec *tsptr)
{
	int	n;

	if ((n = pthread_cond_timedwait(cptr, mptr, tsptr))) {
		errno = n;
		ul_writelog(UL_LOG_WARNING, "pthread_cond_timedwait() call failed.error[%d] info is %s.",
        errno, strerror(errno));
	}
	return n;
}

//*******************************************************
// Wrap function about pthread_mutexattr_init()
//*******************************************************
int	ul_pthread_mutexattr_init(pthread_mutexattr_t *attr)
{
	int	n;

	if ((n = pthread_mutexattr_init(attr))) {
		errno = n;
		ul_writelog(UL_LOG_WARNING, "pthread_mutexattr_init() call failed.error[%d] info is %s.",
        errno, strerror(errno));
	}
	return n;
}

//*******************************************************
// Wrap function about pthread_mutex_destroy()
//*******************************************************
int	ul_pthread_mutexattr_destroy(pthread_mutexattr_t *attr)
{
	int	n;

	if ((n = pthread_mutexattr_destroy(attr))) {
		errno = n;
		ul_writelog(UL_LOG_WARNING, "pthread_mutexattr_destroy() call failed.error[%d] info is %s.",
        errno, strerror(errno));
	}
	return n;
}

//*******************************************************
// Wrap function about pthread_condattr_init()
//*******************************************************
int	ul_pthread_condattr_init(pthread_condattr_t *attr)
{
	int	n;

	if ((n = pthread_condattr_init(attr))) {
		errno = n;
		ul_writelog(UL_LOG_WARNING, "pthread_condattr_init() call failed.error[%d] info is %s.",
        errno, strerror(errno));
	}
	return n;
}

//*******************************************************
// Wrap function about pthread_condattr_destroy()
//*******************************************************
int	ul_pthread_condattr_destroy(pthread_condattr_t *attr)
{
	int n;

	if ((n = pthread_condattr_destroy(attr))) {
		errno = n;
		ul_writelog(UL_LOG_WARNING, "pthread_condattr_destroy() call failed.error[%d] info is %s.",
        errno, strerror(errno));
	}
	return n;
}

//*******************************************************
// Wrap function about pthread_once()
//*******************************************************
int	ul_pthread_once(pthread_once_t *ptr, void (*func)(void))
{
	int	n;

	if ((n = pthread_once(ptr, func))) {
		errno = n;
		ul_writelog(UL_LOG_WARNING, "pthread_once() call failed.error[%d] info is %s.",
        errno, strerror(errno));
	}
	return n;
}

//*******************************************************
// Wrap function about pthread_key_create()
//*******************************************************
int	ul_pthread_key_create(pthread_key_t *key, void (*func)(void *))
{
	int	n;

	if ( (n = pthread_key_create(key, func))) {
		errno = n;
		ul_writelog(UL_LOG_WARNING, "pthread_key_create() call failed.error[%d] info is %s.",
        errno, strerror(errno));
	}
	return n;
}

//*******************************************************
// Wrap function about pthread_setspecific()
//*******************************************************
int	ul_pthread_setspecific(pthread_key_t key, const void *value)
{
	int	n;

	if ((n = pthread_setspecific(key, value))) {
		errno = n;
		ul_writelog(UL_LOG_WARNING, "pthread_setspecific() call failed.error[%d] info is %s.",
        errno, strerror(errno));
	}
	return n;
}

//*******************************************************
// Wrap function about pthread_getspecific()
//*******************************************************
void *ul_pthread_getspecific(pthread_key_t key)
{
	return(pthread_getspecific(key));
}
/* vim: set ts=4 sw=4 sts=4 tw=100 noet: */
