#ifndef __ZCACHE_COMMON_H__
#define __ZCACHE_COMMON_H__

#include <pthread.h>
#include "zcachedef.h"


#define ZCACHE_MAX_FILE_LEN         256

#define ZCACHE_NAME                 "zcache"


#define ZCACHE_MAX_CONF_NAME_LEN    128


typedef struct _zcache_node_t
{
    unsigned int ip[2];                                                 
    unsigned int port;                                                  
}zcache_node_t;


typedef struct _zcache_node_param_t
{
    unsigned int is_flowcontrol;                                        
}zcache_node_param_t;


typedef struct _zcache_product_param_t
{
    unsigned int minimal_cache_item_num;                                
    unsigned int maximal_request_num_persec;                            
    unsigned int del_strategy;                                          
    unsigned int block_num;                                             
}zcache_product_param_t;

typedef struct _zcache_load_t
{
    unsigned int avg_load;                                              
    unsigned int avg_flow;                                              
    unsigned int op_info[4];                                            
}zcache_load_t;

typedef struct _zcache_status_t
{
    unsigned int        block_total;					
    unsigned long long  mem_total;					
    unsigned long long  disk_total;						
    unsigned long long  disk_avail_size;				
    unsigned long long  total_query;					
    unsigned long long  succ_query_count;				
    unsigned int        memused;					
    unsigned int        item_num;					
    unsigned long long  net_read_flowload; 				
    unsigned long long  net_write_flowload;				
    unsigned long long  disk_read_count;				
    unsigned long long  disk_write_count;				
    unsigned long long  cache_hit_count[3];				
    unsigned long long  insert_count;                                   
    unsigned long long  remove_count;                                   
    unsigned long long  update_count;                                   
    unsigned long long  search_count;                                   
    unsigned long long  lru_die_out_count;                              
    unsigned int        avg_op_load;					
    unsigned int            node_capacity;                   
    bool                is_valid;                       
    pthread_rwlock_t    lock;                           
}zcache_status_t;







#endif
