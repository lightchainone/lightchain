#ifndef __ZCACHE_H__
#define __ZCACHE_H__

#include <stdlib.h>
#include "zcachedef.h"



const unsigned int ZCACHEAPI_DEFAULT_LOCAL_CACHE_NUM=0;                 

const unsigned int ZCACHEAPI_DEFAULT_LOCAL_CACHE_MIN_BUSY_COUNT=3;      


const unsigned int ZCACHEAPI_DEFAULT_REFETCH_TIME=3000;                  

const unsigned int ZCACHEAPI_DEFAULT_CONNECT_TIMEOUT=1000;              

const unsigned int ZCACHEAPI_DEFAULT_READ_TIMEOUT=5000;                 

const unsigned int ZCACHEAPI_DEFAULT_WRITE_TIMEOUT=5000;                

const unsigned int ZCACHEAPI_DEFAULT_CSCHECK_TIME=1000000;                

const unsigned int ZCACHEAPI_MIN_CSCHECK_TIME=10000;                



const unsigned int ZCACHEAPI_DEFAULT_FD_NUM= 1024;                      

const unsigned int  ZCACHEAPI_DEFAULT_FD_TIMEOUT = 60000;               

const unsigned int ZCACHEAPI_LOCAL_TIMEOUT_MS = 500;                    

typedef struct _single_zcache_t single_zcache_t;
typedef struct _zcache_t zcache_t;



typedef struct _zcache_status_t zcache_status_t;		  




typedef void (*zcache_op_cb_t)(zcache_t *cache,
                               zcache_err_t err,
                               unsigned int logid,
                               zcache_op_t optype,
                               zcache_item_t *key, zcache_item_t *slckey,
                               zcache_item_t *value,
                               void *arg, unsigned int left_time_ms);


const char *zcache_strerror(zcache_err_t err);



int zcache_status(zcache_t *zcache, unsigned int idx, unsigned int node_id, zcache_status_t *buf);


int zcache_get_node_id(zcache_t *zcache, unsigned int idx, zcache_item_t *key);


zcache_t *zcache_create(const char *service_addrs,
                        const char *product_name,
                        unsigned int max_fd_num=ZCACHEAPI_DEFAULT_FD_NUM,
                        unsigned int max_fd_timeout_ms=ZCACHEAPI_DEFAULT_FD_TIMEOUT,
                        unsigned int cscheck_time_us=ZCACHEAPI_DEFAULT_CSCHECK_TIME
                       );



void  zcache_destroy(zcache_t *cache);


int zcache_set_local_cache(zcache_t *cache,
                           unsigned int max_local_cache_num,
                           unsigned int min_busy_count=ZCACHEAPI_DEFAULT_LOCAL_CACHE_MIN_BUSY_COUNT);



void zcache_set_param(zcache_t *cache,
                      unsigned int connect_timeout_ms,
                      unsigned int ms_read_timeout_ms,
                      unsigned int ms_write_timeout_ms,
                      unsigned int ms_refetch_time_ms,
                      unsigned int cscheck_time_us = 0);



int zcache_auth(zcache_t *cache,const char *token,bool isblock,zcache_err_t *err_out=NULL);



int zcache_search(zcache_t *cache,
                  zcache_item_t *key, zcache_item_t *slckey,
                  zcache_item_t *value_out, zcache_err_t *err_out,
                  unsigned int logid,
                  unsigned int timeout_ms,
                  unsigned int *left_time_ms = NULL);



int zcache_insert(zcache_t *cache,
                  zcache_item_t *key, zcache_item_t *slckey,
                  zcache_item_t *value,
                  unsigned int delay_time_ms,
                  zcache_err_t *err_out,
                  unsigned int logid,
                  unsigned int timeout_ms);



int zcache_update(zcache_t *cache,
                  zcache_item_t *key, zcache_item_t *slckey,
                  zcache_item_t *value,
                  unsigned int delay_time_ms,
                  zcache_err_t *err_out,
                  unsigned int logid,
                  unsigned int timeout_ms);



int zcache_delete(zcache_t *cache,
                  zcache_item_t *key, zcache_item_t *slckey,
                  unsigned int delay_time_ms,
                  zcache_err_t *err_out,
                  unsigned int logid,
                  unsigned int timeout_ms);





int zcache_op(zcache_t *cache, unsigned int count,
              zcache_op_t *optypes,
              zcache_item_t *keys, zcache_item_t *slckeys,
              zcache_item_t *values,
              unsigned int* delay_time_mss,
              zcache_err_t *errs,
              unsigned int logid, unsigned int timeout_ms,
              unsigned int *left_time_mss = NULL);



int zcache_op_async(zcache_t *cache,
                    zcache_op_t optype,
                    zcache_item_t *key, zcache_item_t *slckey,
                    zcache_item_t *value,
                    unsigned int delay_time_ms,
                    unsigned int logid,
                    unsigned int timeout_ms,
                    void *arg,
                    zcache_op_cb_t cb);

#endif
