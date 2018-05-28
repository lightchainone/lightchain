#ifndef  __LNMCSYS_DEFINE_H_
#define  __LNMCSYS_DEFINE_H_

#include "utils.h"

namespace lnsys
{
    #define STRTOLL(ret, str, e_idx, fmt...) \
        do \
        { \
            int trans_ret = 0;\
            ATOLL(ret, str, trans_ret = -1); \
            if(0 == e_idx) { ASSERT_PARAM(!(-1 == trans_ret), ##fmt);} \
            else {ASSERT_SYS(!(-1 == trans_ret), ##fmt);} \
        }while(false)

    #define STRTOU(ret, str, e_idx, fmt...) \
        do \
        { \
            int trans_ret = 0;\
            ATOU(ret, str, trans_ret = -1); \
            if(0 == e_idx) { ASSERT_PARAM(!(-1 == trans_ret), ##fmt);} \
            else {ASSERT_SYS(!(-1 == trans_ret), ##fmt);} \
        }while(false)
        
    #define STRTOULL(ret, str, e_idx, fmt...) \
        do \
        { \
            int trans_ret = 0;\
            ATOULL(ret, str, trans_ret = -1); \
            if(0 == e_idx) { ASSERT_PARAM(!(-1 == trans_ret), ##fmt);} \
            else {ASSERT_SYS(!(-1 == trans_ret), ##fmt);} \
        }while(false)
        
    #define STRTOL(ret, str, e_idx, fmt...) \
        do \
        { \
            int trans_ret = 0;\
            ATOL(ret, str, trans_ret = -1); \
            if(0 == e_idx) { ASSERT_PARAM(!(-1 == trans_ret), ##fmt);} \
            else {ASSERT_SYS(!(-1 == trans_ret), ##fmt);} \
        }while(false)
        

    #define STRTOI(ret, str, e_idx, fmt...) \
        do \
        { \
            int trans_ret = 0;\
            ATOI(ret, str, trans_ret = -1); \
            if(0 == e_idx) { ASSERT_PARAM(!(-1 == trans_ret), ##fmt);} \
            else {ASSERT_SYS(!(-1 == trans_ret), ##fmt);} \
        }while(false)
        
    
    
    
    typedef struct _lnmcsys_conf_t
    {
        _lnmcsys_conf_t()
        {
            team[0] = 0x00;                 
            product[0] = 0x00;              
            customer_id = 0x00 ;
            chainkey_invalid_timeout = 0;      
            mysql_conn_timeout = 0;         
            mysql_num = 0;                  
            mysql_partition_num = 0;        
        }
        
        
        
        void dump()
        {
            TRACE("get conf team: %s.", team);
            TRACE("get conf product: %s.", product);
            TRACE("get conf customer_id: %hhu.", customer_id);
            TRACE("get conf chainkey_invalid_timeout: %u.", chainkey_invalid_timeout);
            TRACE("get conf mysql_conn_timeout: %u.", mysql_conn_timeout);
            TRACE("get conf mysql_num: %u.", mysql_num);
            TRACE("get conf mysql_partition_num: %u.", mysql_partition_num);
        }
        char team[128 + 2];                 
        char product[128 + 2];
        unsigned char customer_id;
        unsigned int chainkey_invalid_timeout;
        unsigned int mysql_conn_timeout;
        unsigned int mysql_num;
        unsigned int mysql_partition_num;

    }lnmcsys_conf_t;

    
    
    
    enum lnmcsys_chain_status_t
    {
        REGISTER_APP_STATUS_OK = 0,             
        REGISTER_APP_STATUS_DELETED = 1         
    };

    enum lnmcsys_user_status_t
    {
        REGISTER_USER_STATUS_OK = 0,
        REGISTER_USER_STATUS_DELETED = 1
    };


    enum lnmcsys_group_status_t
    {
        REGISTER_GROUP_STATUS_OK = 0,           
        REGISTER_GROUP_STATUS_DELETED = 1              
    };

    enum lnmcsys_group_bind_status_t
    {
        REGISTER_GROUP_BIND_STATUS_OK = 0,          
        REGISTER_GROUP_BIND_STATUS_DELETED = 1              
    };

    enum lnmcsys_group_type_t
    {
        REGISTER_GROUP_BROARDCAST_DEFAULT_APP_TYPE = 0,     
        REGISTER_GROUP_BROARDCAST_PULL_TYPE = 1,
        REGISTET_GROUP_BROARDCAST_PUSH_TYPE = 2,
        REGISTER_GROUP_BROARDCAST_GENERAL_TYPE = 127,       
        REGISTER_GROUP_BROARDCAST_ALL_TYPE = 255        
    };
    
    enum lnmcsys_chain_nps_status_t
    {
        REGISTER_APP_NPS_STATUS_OK = 0,             
        REGISTER_APP_NPS_STATUS_DELETED = 1                     
    };
    
    enum lnmcsys_dev_mc_status_t
    {
        REGISTER_DEV_IOS_STATUS_OK = 0,
        REGISTER_DEV_IOS_STATUS_DELETED = 1
    };

    enum lnmcsys_dev_wp_status_t
    {
        REGISTER_DEV_WP_STATUS_OK = 0,
        REGISTER_DEV_WP_STATUS_DELETED = 1
    };

    enum lnmcsys_group_user_bind_cache_status_t
    {
        REGISTER_GROUP_USER_BIND_CACHE_BUILDING = 1,
        REGISTER_GROUP_USER_BIND_CACHE_NORMAL,
    };

    const Lsc::string REGISTER_GROUP_ID_ALLOCATOR_KEY = "lnmcsys_group_id_allocator"; 

    const unsigned long long REGISTER_MAX_GROUP_BIND_NUM = 20;

    const unsigned int REGISTR_DB_QUERY_PAGE_SIZE = 100;
};



#endif  


