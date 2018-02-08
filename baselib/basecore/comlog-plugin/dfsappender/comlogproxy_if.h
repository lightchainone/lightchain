



#ifndef  __PROXYLOGSVR_IF_H_
#define  __PROXYLOGSVR_IF_H_

#include <stdlib.h>
#include <stdint.h>

namespace comspace {
namespace comlogplugin {
namespace dfsappender {
const int COMLOG_PROXY_VERSION = 1;     


enum comlog_proxy_command_type_t {
    COMLOG_PROXY_CREATE = 1,
    COMLOG_PROXY_CLOSE = 2,
    COMLOG_PROXY_FLUSH = 3,
    COMLOG_PROXY_PRINT = 6, 
    COMLOG_PROXY_SHUTDONW = 8, 
    COMLOG_PROXY_STATUS = 9, 
};


enum comlog_proxy_error_type_t {
    COMLOG_PROXY_SUCCESS = 0,
    COMLOG_PROXY_FAIL = -1,
};

#pragma pack(push, 4)


const int COMLOG_PROXY_MASTER_MAXLEN = 128;
const int COMLOG_PROXY_MODULE_MAXLEN = 2560;             
const int COMLOG_PROXY_FILENAME_MAXLEN = 255;            
const int COMLOG_PROXY_USER_SIZE = 64;             
const int COMLOG_PROXY_PASSWD_SIZE = 64;           
const int COMLOG_PROXY_COMPRESS_SIZE = 32;
const int COMLOG_PROXY_COMPRESS_ARG_SIZE = 64;
const int COMLOG_PROXY_CHARSET = 32;
const int COMLOG_PROXY_FMT_SIZE = 32; 


struct comlog_proxy_create_req_t {
    int32_t cmd;
    uint32_t dfs_splite_type;
    uint32_t dfs_cuttime;
    char master[COMLOG_PROXY_MASTER_MAXLEN];
    char path[COMLOG_PROXY_MODULE_MAXLEN];       
    char filename[COMLOG_PROXY_FILENAME_MAXLEN]; 
    char user[COMLOG_PROXY_USER_SIZE];
    char passwd[COMLOG_PROXY_PASSWD_SIZE];
    char dfscompress[COMLOG_PROXY_COMPRESS_SIZE];
    char dfscompress_arg[COMLOG_PROXY_COMPRESS_ARG_SIZE];
    char charset[COMLOG_PROXY_CHARSET];
    char fmt[COMLOG_PROXY_FMT_SIZE];
};


struct comlogproxy_handle_t
{
    uint64_t high;        
    uint64_t low;         
};




struct comlog_proxy_flush_req_t {
    int cmd;
    struct comlogproxy_handle_t handle;
};


struct comlog_proxy_close_req_t {
    int cmd;
    struct comlogproxy_handle_t handle;
};

static const uint32_t COMLOG_PROXY_PRINT_MAGIC_NUM = 0x71C78375;          



typedef struct _comlog_proxy_print_req_t {
    int cmd;                                
    struct comlogproxy_handle_t handle;     
    uint32_t magic_num;                     
    uint32_t time;                          
    uint32_t num;                           
    uint32_t size;                          
    uint32_t reserved;                      
    char logdata[0];                        
} comlog_proxy_print_req_t;



struct comlog_proxy_res_t {
    int err_no; 
    struct comlogproxy_handle_t handle;     
    uint32_t err_len; 
    char err_info[0]; 
};
#pragma pack(pop)

} 
} 
} 

#endif  


