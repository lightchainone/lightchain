#ifndef _CPOOL_H
#define _CPOOL_H

#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>

#include "lcserver_core.h"
#include "lc_util.h"

typedef struct _socket_item
{
	int sock;
	int last_active;
	int status;
	in_addr_t ip;
} socket_item;

typedef struct _cpool_t
{
	socket_item *_sockets;
	u_int _size;
    long long _using_size;

	int _timeout;
	int *_run;

	queue_t _queue;

	pthread_mutex_t 	_ready_mutex;
	pthread_cond_t 		_ready_cond;

	fd_set _rfs;
	struct timeval _check_interval;

	pthread_t _main;
}cpool_t;


int cpool_init(lc_server_t *);


int cpool_run(lc_server_t *);


int cpool_join(lc_server_t *);


int cpool_destroy(lc_server_t *);

long long cpool_get_socknum(lc_server_t *);

long long cpool_get_queuenum(lc_server_t *);


void * _cpool_main(void *);


void * _cpool_workers(void *);


int cpool_produce(lc_server_t *sev, struct sockaddr *addr, socklen_t *addrlen);


int cpool_add(cpool_t *pool, int sock, in_addr_t ip);


void cpool_check(cpool_t *pool);


int cpool_put(cpool_t *pool, int handle);


void cpool_del(cpool_t *pool, int handle, int keep_alive);


void cpool_consume(cpool_t *pool, lc_server_pdata_t *data);


int cpool_get(cpool_t *pool, int *handle, int *sock);
#endif


