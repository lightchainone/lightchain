#include "zcachedata.h"

#define ZCACHE_BLOCK_MAX_PERFETCH 1024   


int get_all_meta(single_zcache_t *cache,zcache_err_t *err_out);

int get_some_meta(single_zcache_t *cache,char *reqbuf,int reqlen,char *resbuf,int reslen,zcache_err_t *err_out);

int auth_with_meta(single_zcache_t *cache,zcache_err_t *err_out);


void *block_update_routine(void *arg);
