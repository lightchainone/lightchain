#ifndef __ZCACHE_DEF_H__
#define __ZCACHE_DEF_H__


#define ZCACHEAPI_MODULE "zcacheapi"


const unsigned int ZCACHE_MAX_PRODUCT_NUM  =    8191;


const unsigned int ZCACHE_MAX_BLOCK_NUM   =     131071;


const unsigned int ZCACHE_MAX_NODE_NUM   =      1024;


const unsigned int ZCACHE_MAX_KEY_LEN    =      255;


const unsigned int ZCACHE_MAX_VALUE_LEN  =      (1024*1024*16-1);


const unsigned int ZCACHE_MAX_PRODUCT_NAME_LEN = 64;


const unsigned int ZCACHE_MAX_TOKEN_LEN   =     64;


const unsigned int ZCACHE_MAX_OP_NUM    =       128;


const unsigned int ZCACHE_MAX_MSG_BODY_LEN = (ZCACHE_MAX_VALUE_LEN+1024*1024);

typedef enum _zcache_op_t
{
    ZCACHE_OP_NULL_BEGIN = 0,
    ZCACHE_INSERT = 1,
    ZCACHE_DELETE = 2,
    ZCACHE_UPDATE = 3,
    ZCACHE_SEARCH = 4,
    ZCACHE_OP_NULL_END
}zcache_op_t;


typedef enum _zcache_err_t
{
    ZCACHE_OK                   =           0,
    ZCACHE_ERR_PARAM            =           1,                          
    ZCACHE_ERR_NOT_AUTH         =           2,                          
    ZCACHE_ERR_BUF_NOT_ENOUGH   =           3,                          
    ZCACHE_ERR_EXIST            =           4,                          

    ZCACHE_ERR_NOT_EXIST        =           5,                          
    ZCACHE_ERR_BLOCK_NOT_EXIST  =           6,                          
    ZCACHE_ERR_PRODUCT_NOT_EXIST=           7,
    ZCACHE_ERR_BUSY             =           8,                          
    ZCACHE_ERR_FROZEN_DELETE    =           9,                          

    ZCACHE_ERR_BLOCK_UPDATED    =           10,                         
    ZCACHE_ERR_TIMEOUT          =           11,                         
    ZCACHE_ERR_NET              =           12,                         
    ZCACHE_ERR_MEM              =           13,                         
    ZCACHE_ERR_DISK             =           14,                         

    ZCACHE_ERR_METASERVER       =           15,
    ZCACHE_ERR_CACHESERVER      =           16,
    ZCACHE_ERR_LIB              =           17,

    ZCACHE_ERR_PART_SUC         =           18,                         

    ZCACHE_ERR_BLOCK_WRONG_STATE=           19,                         

    ZCACHE_APIPLUS_INIT_FAIL    =           20,                         

    ZCACHE_ERR_END              =           21
}zcache_err_t;


typedef struct _zcache_item_t
{
    char *item;
    unsigned int len;
}zcache_item_t;


typedef struct _cs_mem_stat_t
{
    unsigned int memused;                                               
    unsigned int memused_res;                                           
    unsigned int item_num;                                              
    unsigned int reserved;                                              
}cs_mem_stat_t;


typedef struct _cs_disk_stat_t
{
    unsigned long long total_size;                                      
    unsigned long long avail_size;                                      
}cs_disk_stat_t;


typedef struct _prudoct_stat_info_t
{
    unsigned long long insert_count;                                    
    unsigned long long remove_count;                                    
    unsigned long long update_count;                                    
    unsigned long long search_count;                                    
    unsigned long long total_num;                                       
    unsigned long long min_invalid_num;                                 
    unsigned long long max_invalid_num;                                 
    unsigned long long lru_die_out_count;                               
    unsigned long long cache_hit_count[3];                              
    cs_mem_stat_t      mem_stat[4];                                     
    unsigned int       avg_op_load[4];                                  
}product_stat_info_t;


typedef struct _cacheserver_stat_info_t
{
    product_stat_info_t products_stat_info;                             
    unsigned long long  query_count;                                    
    unsigned long long  succ_query_count;                               
    unsigned long long  net_read_flowload;                              
    unsigned long long  net_write_flowload;                             
    unsigned long long  disk_read_count;                                
    unsigned long long  disk_read_loads;                                
    unsigned long long  disk_write_count;                               
    unsigned long long  disk_write_loads;                               
    unsigned long long  mem_total;                                      
    cs_disk_stat_t      disk_stat;                                      
}cacheserver_stat_info_t;



typedef enum cache_level
{
    CACHE_LOW_LEVEL = 0,
    CACHE_DISK_LEVEL,
    CACHE_HIGH_LEVEL,
    CACHE_FAKE_LEVEL
} ____dummy_cache_level_type;


typedef enum CS_OP
{
    CS_INSERT = 0,
    CS_DELETE,
    CS_UPDATE,
    CS_SEARCH
} ____dummy_CS_OP_type;

#endif
