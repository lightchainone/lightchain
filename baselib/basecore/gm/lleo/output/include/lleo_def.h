
#ifndef __GALILEO_DEF_H__
#define __GALILEO_DEF_H__

#include "zookeeper.h"
#include "myhashtable.h"
#include "lleo_def.h"
#include "zookeeper_log.h"





const unsigned int GALILEO_MAGICNUM = 0xfb726535;   

const int GALILEO_DEFAULT_SESN_EXPIRE_TIME = 30000; 
const int GALILEO_TMP_BUF_SIZE     = 128 * 1024;    
const int GALILEO_DEFAULT_BUF_SIZE = 128 * 1024;    
const int GALILEO_DEFAULT_PATH_LEN = 512;           
const int GALILEO_ENTITY_TAG_LEN   = 10;            

const int GALILEO_ACL_SCHEME_LEN   = 10;            
const int GALILEO_ACL_ID_LEN       = 64;            

const int GALILEO_PERM_READ   = 1 << 0;             
const int GALILEO_PERM_WRITE  = 1 << 1;             
const int GALILEO_PERM_CREATE = 1 << 2;             
const int GALILEO_PERM_DELETE = 1 << 3;             
const int GALILEO_PERM_ADMIN  = 1 << 4;             

const u_int GALILEO_MAX_CALLBACK_NUM = 1024;        

const int GALILEO_DEFAULT_CTIMEOUT = 100;           
const int GALILEO_DEFAULT_RTIMEOUT = 500;           
const int GALILEO_DEFAULT_WTIMEOUT = 500;           
const int GALILEO_DEFAULT_REWORK_TIME = 5;          
const int MAX_RETRY_TIMES = 5;


enum GALILEO_EVENT{
    GALILEO_EVENT_START,
    
    GALILEO_ABSTRACT_CHANGED_EVENT,     
    GALILEO_ENTITY_CHANGED_EVENT,       
    GALILEO_RESOURCE_UPDATE_EVENT,      
    
    GALILEO_EVENT_END,
};



enum GALILEO_ERRORS{
    GALILEO_E_OK = 0, 

    

    GALILEO_E_SYSERROR = -1,             
    GALILEO_E_RUNTIMEINCONSISTENCY = -2, 
    GALILEO_E_DATAINCONSISTENCY = -3,    
    GALILEO_E_CONNLOSS = -4,             
    GALILEO_E_MARSHALLINGERROR = -5,     
    GALILEO_E_UNIMPLEMENTED = -6,        
    GALILEO_E_OPERATIONTIMEOUT = -7,     
    GALILEO_E_BADPARAM = -8,             
    GALILEO_E_INVALIDSTATE = -9,         

    
    GALILEO_E_APIERROR = -100,
    GALILEO_E_NONODE = -101,             
    GALILEO_E_NOAUTH = -102,             
    GALILEO_E_BADVERSION = -103,         
    GALILEO_E_CHILDFOREPHEM = -108,      
    GALILEO_E_NODEEXISTS = -110,         
    GALILEO_E_NOTEMPTY = -111,           
    GALILEO_E_SESSIONEXPIRED = -112,     
    GALILEO_E_INVALIDCALLBACK = -113,    
    GALILEO_E_INVALIDACL = -114,         
    GALILEO_E_AUTHFAILED = -115,         
    GALILEO_E_CLOSING = -116,            
    GALILEO_E_NOTHING = -117,            
    GALILEO_E_REINIT  = -118,            

    GALILEO_E_UNKNOWN = -999,            
};


typedef struct _lleo_list_node_t
{
    char path[GALILEO_DEFAULT_PATH_LEN]; 
    char *data;                          
    int  data_len;                       
    struct _lleo_list_node_t* next;   
}lleo_list_node;


typedef struct _lleo_list_head_t
{
    pthread_rwlock_t work_rwlock;        
    int    totl_node;                    
    struct _lleo_list_node_t* next;   
}lleo_list_head;


typedef struct _lleo_t{
    char              lleo_svr_list[GALILEO_DEFAULT_PATH_LEN];
    int               is_finish;         
    int               pack_buff_size;    
    int               sesn_expi_time;    
    int               delay_watch_time;  
    unsigned int      magic_num;         

    zhandle_t         *zh;               
    pthread_t         zh_tid;            
    pthread_mutex_t   zh_lock;           
    pthread_cond_t    zh_cond;           
    myhashtable       *cb_table;         
    pthread_rwlock_t  cb_rwlock;         
    lleo_list_head *watch_res_list;   
    lleo_list_head *regi_res_list;    
    struct ACL_vector acl_vec;           
    pthread_mutex_t   ref_lock;         
    int               ref_cnt;          
}lleo_t;


typedef struct _cb_item_t{
    void *cb_func;
    void *cb_ctx;
}cb_item_t;



#define GALILEO_ABSTRACT   "abstract"   
#define GALILEO_ENTITY     "entity"     

#define GALILEO_ACL        "acl"        
#define GALILEO_ACL_IP     "acl_ip"     
#define GALILEO_ACL_ID     "acl_id"     
#define GALILEO_ACL_SCHEME "acl_scheme" 
#define GALILEO_ACL_MASK   "acl_mask"   
#define GALILEO_ACL_DIGEST "acl_digest" 
#define GALILEO_ACL_PERMS  "acl_perms"  
#define GALILEO_ACL_AUTH   "acl_auth"

#define GALILEO_ACL_NAME   "__IXINTUI_INF_COM_GALILEO_ACL__"


#define GALILEO_ABSTRACT_NAME         "name"              
#define GALILEO_ABSTRACT_SERVICES     "services"          
#define GALILEO_ABSTRACT_SVR_NAME     "service_name"      
#define GALILEO_ABSTRACT_SVR_PORT     "service_port"      
#define GALILEO_ABSTRACT_SVR_CONNTYPE "service_conn_type" 
#define GALILEO_ABSTRACT_SVR_CTIMEOUT "service_ctimeout"  
#define GALILEO_ABSTRACT_SVR_RTIMEOUT "service_rtimeout"  
#define GALILEO_ABSTRACT_SVR_WTIMEOUT "service_wtimeout"  
#define GALILEO_THRESHOLD             "threshold"       


#define GALILEO_ENTITY_NAME       "name"      
#define GALILEO_ENTITY_HOSTNAME   "hostname"  
#define GALILEO_ENTITY_IP         "ip"        


#define GALILEO_DEFAULT_ZOO_LOG   "./log/zoo.client.log"



#define GALILEO_INT_CHECK(flg, msg, rc) {\
    if (flg){\
        ul_writelog(UL_LOG_WARNING, "[%s:%d] %s, rc:%d", __FILE__, __LINE__, msg, rc);\
        return GALILEO_E_BADPARAM;\
    }\
}


#define GALILEO_INT_CHECK_PUT(hdlr, flg, msg, rc) {\
    if (flg){\
        lleo_put(hdlr);\
        ul_writelog(UL_LOG_WARNING, "[%s:%d] %s, rc:%d", __FILE__, __LINE__, msg, rc);\
        return GALILEO_E_BADPARAM;\
    }\
}

#define GALILEO_POINTER_CHECK(flg, msg) {\
    if (flg){\
        ul_writelog(UL_LOG_WARNING, "[%s:%d] %s", __FILE__, __LINE__, msg);\
        return GALILEO_E_BADPARAM;\
    }\
}


#define GALILEO_ZERROR_CHECK(zrc, msg, path){\
    if (ZOK != zrc){\
        grc = translate_lleo_error(zrc);\
        ul_writelog(UL_LOG_WARNING, "[%s:%d] %s [%s], rc:%d, error:%s",\
            __FILE__, __LINE__, msg, path, grc, lleo_err2str(grc));\
        return grc;\
    }\
}


#define GALILEO_ZERROR_CHECK_PUT(hdlr, zrc, msg, path){\
    if (ZOK != zrc){\
        grc = translate_lleo_error(zrc);\
        ul_writelog(UL_LOG_WARNING, "[%s:%d] %s [%s], rc:%d, error:%s",\
            __FILE__, __LINE__, msg, path, grc, lleo_err2str(grc));\
        lleo_put(hdlr); \
        return grc;\
    }\
}

#define GALILEO_SAFE_FREE(x) {\
    if (NULL != x){\
        free(x);\
        x = NULL;\
    }\
}


#define GALILEO_SAFE_FCLOSE(x) {\
    if (NULL != x){\
        fclose(x);\
        x = NULL;\
    }\
}





#endif

