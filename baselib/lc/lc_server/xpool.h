#ifndef _XPOOL_H
#define _XPOOL_H

#include <pthread.h>

#include "lcserver_core.h"

typedef struct _xpool_t
{
	pthread_mutex_t lock;
} xpool_t;


int xpool_init(lc_server_t *);


int xpool_run(lc_server_t *);


int xpool_join(lc_server_t *);


int xpool_destroy(lc_server_t *);

long long xpool_get_socknum(lc_server_t *);

long long xpool_get_queuenum(lc_server_t *);
#endif

