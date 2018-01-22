#ifndef _LCSERVER_REGISTER_H
#define _LCSERVER_REGISTER_H

#include "lc_server.h"

extern int xpool_init(lc_server_t *);
extern int xpool_run(lc_server_t *);
extern int xpool_join(lc_server_t *);
extern int xpool_destroy(lc_server_t *);
extern long long xpool_get_socknum(lc_server_t *);
extern long long xpool_get_queuenum(lc_server_t *);

extern int cpool_init(lc_server_t *);
extern int cpool_run(lc_server_t *);
extern int cpool_join(lc_server_t *);
extern int cpool_destroy(lc_server_t *);
extern long long cpool_get_socknum(lc_server_t *);
extern long long cpool_get_queuenum(lc_server_t *);

extern int eppool_init(lc_server_t *);
extern int eppool_run(lc_server_t *);
extern int eppool_join(lc_server_t *);
extern int eppool_destroy(lc_server_t *);
extern long long eppool_get_socknum(lc_server_t *);
extern long long eppool_get_queuenum(lc_server_t *);

extern int apool_init(lc_server_t *);
extern int apool_run(lc_server_t *);
extern int apool_join(lc_server_t *);
extern int apool_destroy(lc_server_t *);
extern long long apool_get_socknum(lc_server_t *);
extern long long apool_get_queuenum(lc_server_t *);

typedef int (*init_pool)(lc_server_t *);
typedef int (*run_pool)(lc_server_t *);
typedef int (*join_pool)(lc_server_t *);
typedef int (*destroy_pool)(lc_server_t *);
typedef long long (*get_pool_socknum)(lc_server_t *);
typedef long long (*get_pool_queuenum)(lc_server_t *);


struct _pool_t
{
	init_pool init;	
	run_pool run;	
	join_pool join;	
	destroy_pool destroy;	
    get_pool_socknum get_socknum;
    get_pool_queuenum get_queuenum;
};

static const struct _pool_t g_pool[] = {
	{xpool_init, xpool_run, xpool_join, xpool_destroy, xpool_get_socknum, xpool_get_queuenum},	
	{cpool_init, cpool_run, cpool_join, cpool_destroy, cpool_get_socknum, cpool_get_queuenum},
	{eppool_init, eppool_run, eppool_join, eppool_destroy, eppool_get_socknum, eppool_get_queuenum},
	{apool_init, apool_run, apool_join, apool_destroy, apool_get_socknum, apool_get_queuenum},
};

#endif

