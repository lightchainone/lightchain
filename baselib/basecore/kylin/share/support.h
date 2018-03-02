
#ifndef _SUPPORT_H_
#define _SUPPORT_H_

#include "extypes.h"
#include <linux/unistd.h>
#include <sys/time.h>

#ifndef __NR_gettid
#	define __NR_gettid				224
#endif

#ifndef __NR_futex
#	define FUTEX_WAIT				(0)
#	define FUTEX_WAKE				(1) 
#	define FUTEX_FD					(2)
#	define __NR_futex				240
#else
#	include <linux/futex.h>
#endif

#ifndef __NR_epoll_create
#	define __NR_epoll_create		254
#	define __NR_epoll_ctl			255
#	define __NR_epoll_wait			256

enum EPOLL_EVENTS
{
	EPOLLIN = 0x001,
#define EPOLLIN EPOLLIN
	EPOLLPRI = 0x002,
#define EPOLLPRI EPOLLPRI
	EPOLLOUT = 0x004,
#define EPOLLOUT EPOLLOUT
	EPOLLRDNORM = 0x040,
#define EPOLLRDNORM EPOLLRDNORM
	EPOLLRDBAND = 0x080,
#define EPOLLRDBAND EPOLLRDBAND
	EPOLLWRNORM = 0x100,
#define EPOLLWRNORM EPOLLWRNORM
	EPOLLWRBAND = 0x200,
#define EPOLLWRBAND EPOLLWRBAND
	EPOLLMSG = 0x400,
#define EPOLLMSG EPOLLMSG
	EPOLLERR = 0x008,
#define EPOLLERR EPOLLERR
	EPOLLHUP = 0x010,
#define EPOLLHUP EPOLLHUP
	EPOLLONESHOT = (1 << 30),
#define EPOLLONESHOT EPOLLONESHOT
	EPOLLET = (1 << 31)
#define EPOLLET EPOLLET
};



#define EPOLL_CTL_ADD 1	
#define EPOLL_CTL_DEL 2	
#define EPOLL_CTL_MOD 3	


typedef union epoll_data
{
	void *ptr;
	int fd;
	uint32 u32;
	uint64 u64;
} epoll_data_t;

struct epoll_event
{
	uint32 events;	
	epoll_data_t data;	
};

#else	
#	include <sys/epoll.h>
#endif

#define __NR_sys_futex				__NR_futex
#define __NR_sys_epoll_create		__NR_epoll_create
#define __NR_sys_epoll_ctl			__NR_epoll_ctl
#define __NR_sys_epoll_wait			__NR_epoll_wait
#define __NR_sys_gettid				__NR_gettid


#ifdef	__cplusplus
extern "C" {
#endif
int strcasecmp (__const char *__s1, __const char *__s2);


static inline pid_t sys_gettid()
{
    return syscall(__NR_gettid);
}



static inline int sys_futex(volatile int* futex, int op, int val, struct timespec* rel)
{
    return syscall(__NR_futex, futex, op, val, rel);
}



static inline int sys_epoll_create(int __size)
{
    return syscall(__NR_epoll_create, __size);
}



static inline int sys_epoll_ctl(int __epfd, int __op, int __fd, struct epoll_event* __event)
{
    return syscall(__NR_epoll_ctl, __epfd, __op, __fd, __event);
}



static inline int sys_epoll_wait(int __epfd, struct epoll_event* __events, int __maxevents, int __timeout)
{
    return syscall(__NR_epoll_wait, __epfd, __events, __maxevents, __timeout);
}


#ifdef	__cplusplus
}
#endif

#ifndef O_DIRECT
#define O_DIRECT        040000
#endif

#ifndef O_DIRECTORY     
#define O_DIRECTORY     0200000
#endif

#endif
