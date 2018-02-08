#ifndef __ZCACHE_DATA_H__
#define __ZCACHE_DATA_H__
#include <pthread.h>
#include <vector>
#include <sys/poll.h>
#include "productinfo.h"
#include "localcache.h"
#include "link.h"
#include "asyncremoteinvoker.h"

typedef struct _zcacheapi_node_t
{
    char ip[64];
    unsigned short port;
    unsigned short err_count;
    DLINK link;
}zcacheapi_node_t;

typedef struct _single_zcache_t
{
    DLINK mslist_head;                                                  
    unsigned int ms_num;                                                
    pthread_mutex_t mutex;

    pthread_t update_thread;                                            
    pthread_t cscheck_thread;                                           

    unsigned int ms_refetch_time_ms;                                    
    unsigned int cscheck_time_us;                                       
    unsigned int connect_timeout_ms;                                    
    unsigned int ms_read_timeout_ms;                                    
    unsigned int ms_write_timeout_ms;                                   

    ProductInfo *product;
    bool is_block_fetched;                                              
    bool is_running;
    char *reqbuf;
    unsigned int reqlen;
    char *resbuf;
    unsigned int reslen;

    LocalCache *localcache;
    AsyncRemoteMgr *mgr;
}single_zcache_t;

typedef struct _zcache_t
{
    unsigned int       redundant_count;                                 
    single_zcache_t    **zcaches;                                       
    AsyncRemoteMgr     *mgr;                                            
}zcache_t;

single_zcache_t* zcachedata_create(unsigned int cscheck_time_us);

void zcachedata_destroy(single_zcache_t *cache);

int zcache_connect_ms(single_zcache_t *cache);

int zcache_load_msaddrs(single_zcache_t *cache,const char *addrs);

#endif
