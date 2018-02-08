#ifndef __ZCACHE_MSGDEF_H__
#define __ZCACHE_MSGDEF_H__




#include "lnhead.h"
#include "common.h"
#include "zcachedef.h"


typedef enum _zcache_cmdtype_t
{
    ZCACHE_CMD_NONE               = 10000,                              
    ZCACHE_CMD_LIB_GETALLBLOCK    = 10001,                              
    ZCACHE_CMD_LIB_GETSOMEBLOCK   = 10002,                              


    ZCACHE_CMD_REPORT             = 10011,                              
    ZCACHE_CMD_HEARTBEAT          = 10012,                              
    ZCACHE_CMD_ASSIGNBLOCK        = 10013,                              
    ZCACHE_CMD_ASSIGNNODEPARAM    = 10014,                              
    ZCACHE_CMD_ASSIGNPRODUCTPARAM = 10015,                              
    ZCACHE_CMD_SEARCHBLOCK        = 10016,                              
    ZCACHE_CMD_DELPRODUCT         = 10017,                              

    ZCACHE_CMD_INSERT             = 10021,                              
    ZCACHE_CMD_UPDATE             = 10022,                              
    ZCACHE_CMD_DELETE             = 10023,                              
    ZCACHE_CMD_SEARCH             = 10024,                              
    ZCACHE_CMD_STATUS             = 10025,                              

    ZCACHE_CMD_CONTROLLOAD        = 10031,                              

    ZCACHE_CMD_GET_PRODUCTS_NODES = 10041,                              
    ZCACHE_CMD_GET_CACHESERVER_STAT_INFO = 10042,                       
    ZCACHE_CMD_GET_PRODUCT_STAT_INFO = 10043,                           

    ZCACHE_CMD_CONTROL            = 10051,                              
}zcache_cmdtype_t;


typedef struct _cmd_head_t
{
    unsigned short cmd_type;
    unsigned short cmd_ret;
    unsigned int body_len;
    char cmd_body[0];
}cmd_head_t;


typedef struct _zcache_head_t
{
    lnhead_t msghead;
    unsigned int cmd_num;
    cmd_head_t cmds[0];
}zcache_head_t;



typedef struct _zcache_cmd_lib_getallmeta_req_t
{
    char token[ZCACHE_MAX_TOKEN_LEN];
    char product[ZCACHE_MAX_PRODUCT_NAME_LEN];
}zcache_cmd_lib_getallmeta_req_t;



typedef struct _zcache_cmd_lib_getallmeta_res_t
{
    unsigned int node_num;
    unsigned int block_num;
    zcache_node_t nodes[0];
}zcache_cmd_lib_getallmeta_res_t;



typedef struct _zcache_cmd_lib_getsomemeta_req_t
{
    char token[ZCACHE_MAX_TOKEN_LEN];
    char product[ZCACHE_MAX_PRODUCT_NAME_LEN];
    unsigned int block_num;                                             
    unsigned int blocks[0];                                             
}zcache_cmd_lib_getsomemeta_req_t;



typedef struct _zcache_cmd_lib_getsomemeta_res_t
{
    unsigned int node_num;
    unsigned int block_num;
    zcache_node_t nodes[0];
}zcache_cmd_lib_getsomemeta_res_t;






typedef struct _zcache_cmd_report_req_t
{
    zcache_node_param_t node;
    unsigned int product_num;
    
    zcache_product_param_t products[0];
}zcache_cmd_report_req_t;

typedef struct _zcache_cmd_product_info_t
{
    unsigned int product_name_len;
    unsigned int block_num;
}zcache_cmd_product_info_t;



typedef struct _zcache_cmd_report_res_t
{
    unsigned int product_num;
    zcache_cmd_product_info_t products[0];                              
}zcache_cmd_report_res_t;



typedef struct _zcache_cmd_heartbeat_req_t
{
    unsigned int is_report_load;                                        
}zcache_cmd_heartbeat_req_t;



typedef struct _zcache_cmd_heartbeat_res_t
{
    zcache_load_t node_load;
    unsigned int product_num;
    char products[0];
}zcache_cmd_heartbeat_res_t;


typedef struct _zcache_cmd_status_req_t
{
    unsigned int is_report_detail;                                        
    unsigned int product_num;
    char products[ZCACHE_MAX_PRODUCT_NAME_LEN];
}zcache_cmd_status_req_t;


typedef struct _zcache_cmd_status_res_t
{
    unsigned int status_num;
    zcache_status_t status[1];
}zcache_cmd_status_res_t;





#define ZCACHE_BLOCK_REFRESH 2
#define ZCACHE_BLOCK_ACTIVATE 1
#define ZCACHE_BLOCK_DEACTIVATE 0


typedef struct _zcache_cmd_assignblock_req_t
{
    char product[ZCACHE_MAX_PRODUCT_NAME_LEN];
    unsigned int blockid;
    unsigned int block_state;                                           
    unsigned long long last_update_time;                                
}zcache_cmd_assignblock_req_t;


typedef struct _zcache_cmd_assignblock_res_t
{
    unsigned long long last_update_time;
}zcache_cmd_assignblock_res_t;


typedef struct _zcache_cmd_delproduct_req_t
{
    char product[ZCACHE_MAX_PRODUCT_NAME_LEN];
}zcache_cmd_delproduct_req_t;


typedef struct _zcache_cmd_delproduct_res_t
{
    
}zcache_cmd_delproduct_res_t;

typedef struct _zcache_cmd_searchblock_req_t
{
    char product[ZCACHE_MAX_PRODUCT_NAME_LEN];
    unsigned int blockid;
    unsigned int reserved;
}zcache_cmd_searchblock_req_t;


typedef struct _zcache_cmd_searchblock_res_t
{
    unsigned long long last_update_time;
}zcache_cmd_searchblock_res_t;



typedef struct _zcache_cmd_assignproductparam_req_t
{
    char product[ZCACHE_MAX_PRODUCT_NAME_LEN];
    zcache_product_param_t param;
}zcache_cmd_assignproductparam_req_t;

typedef struct _zcache_cmd_assignproductparam_res_t
{
    
}zcache_cmd_assignproductparam_res_t;



typedef struct _zcache_cmd_assignnodeparam_req_t
{
    zcache_node_param_t node;
}zcache_cmd_assignnodeparam_req_t;

typedef struct _zcache_cmd_assignnodeparam_res_t
{
    
}zcache_cmd_assignnodeparam_res_t;





typedef struct _zcache_cmd_lib_req_t
{
    char product[ZCACHE_MAX_PRODUCT_NAME_LEN];
    unsigned short key_len;
    unsigned short slckey_len;
    unsigned int value_len;
    unsigned long long delay_time;                                      
    unsigned long long gen_time;                                        
    char buf[0];                                                        
}zcache_cmd_lib_req_t;


typedef struct _zcache_cmd_lib_search_res_t
{
    unsigned long long left_time;                                       
    char buf[0];                                                        
}zcache_cmd_lib_search_res_t;





#define ZCACHE_RAND_TRANS 1

#define ZCACHE_SPEC_TRANS 2

#define ZCACHE_REFRESH_TRANS 3
typedef struct _zcache_cmd_controlload_req_t
{
    char product[ZCACHE_MAX_PRODUCT_NAME_LEN];
    unsigned int fromip;
    unsigned int fromport;
    unsigned int toip;
    unsigned int toport;
    unsigned short trans_type;
    unsigned short block_num;
    unsigned short assign_blks;
    unsigned int blocks[0];
}zcache_cmd_controlload_req_t;


typedef struct _zcache_cmd_controlload_res_t
{
    
}zcache_cmd_controlload_res_t;


typedef struct _zcache_cmd_control_req_t
{
    unsigned int type;
    int key;
}zcache_cmd_control_req_t;

typedef struct _zcache_cmd_control_res_t
{
    unsigned int type;
}zcache_cmd_control_res_t;



typedef struct _zcache_cmd_get_nodes_products_req_t
{
    
}zcache_cmd_get_nodes_products_req_t;



typedef struct _zcache_cmd_get_nodes_products_res_t
{
    unsigned int        product_num;                                    
    unsigned int        node_num;                                       
}zcache_cmd_get_nodes_products_res_t;




typedef struct _zcache_cmd_get_cacheserver_stat_req_t
{
    
}zcache_cmd_get_cacheserver_stat_req_t;


typedef struct _zcache_cmd_get_cacheserver_stat_res_t
{
    cacheserver_stat_info_t     info;
}zcache_cmd_get_cacheserver_stat_res_t;


typedef struct _zcache_cmd_get_product_stat_req_t
{
    char product[ZCACHE_MAX_PRODUCT_NAME_LEN];
}zcache_cmd_get_product_stat_req_t;


typedef struct _zcache_cmd_get_product_stat_res_t
{
    product_stat_info_t         info;
}zcache_cmd_get_product_stat_res_t;
#endif
