

#ifndef  __EPPOOL_H_
#define  __EPPOOL_H_

#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>

#ifdef USE_SYSEPOLL
#include <sys/epoll.h>
#else
#include "epoll.h"
#endif

#include "lcserver_core.h"
#include "lc_util.h"

#define EPPOOL_LISTENER_PRIORITY	10
#define EPPOOL_WORKER_PRIORITY		5 


int eppool_init(lc_server_t *);


int eppool_run(lc_server_t *);


int eppool_join(lc_server_t *);


int eppool_destroy(lc_server_t *);

long long eppool_get_socknum(lc_server_t *);

long long eppool_get_queuenum(lc_server_t *);


void * _eppool_main(void *);


void * _eppool_workers(void *);

typedef struct _eppool_sock_item_t
{
	int status;
	
	int sock;
	int last_active;
	in_addr_t ip;
} eppool_sock_item_t;

typedef struct _eppool_t
{
	eppool_sock_item_t *_sockets;
	struct epoll_event *_ep_events;
	u_int _size;
    long long _using_size;

	int _epfd;

	u_int _timeout;	
	u_int _check_interval;	
	time_t _next_check_time;

	int *_run;
	int _sev_sock_id;

	queue_t _queue;
	pthread_mutex_t _ready_mutex;
	pthread_cond_t _ready_cond;

	pthread_t _main;
} eppool_t;


int eppool_produce(lc_server_t *sev, struct sockaddr *addr, socklen_t *addrlen);


int eppool_consume(eppool_t *pool, lc_server_pdata_t *data);


int eppool_check_timeout(eppool_t *pool);


int eppool_add(eppool_t *pool, int sock, in_addr_t ip);


int eppool_del(eppool_t *pool, int idx, int keep_alive);


int eppool_epoll_add(eppool_t *pool, int idx);

int eppool_epoll_del(eppool_t *pool, int idx);


int eppool_put(eppool_t *pool, int idx);

int eppool_get(eppool_t *pool, int *idx);


int eppool_pthread_cond_timewait(eppool_t *pool);

#endif  


