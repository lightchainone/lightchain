



#ifndef  __LCCLIENT_STRUCT_H_
#define  __LCCLIENT_STRUCT_H_

#include <bsl/containers/string/bsl_string.h>
#include <nshead.h>
#include <bsl/ResourcePool.h>
#include <bsl/var/IVar.h>
#include <lc/lc2.h>

#include "lcclient_define.h"

namespace lc {

    class LcClientConnection;
    class LcClientServer;
    class LcClientManager;
    class LcClientEventPool;
    class LcClientPool;
    class nshead_talkwith_t;

    
    enum {
        LCCLIENT_NSHEAD = 0,          
        LCCLIENT_COMMON,              
    };

    
    typedef int (*lc_client_callbacktalkwith)(void *talk);
    
    
    typedef struct _default_server_arg_t {
        
        
        
        char range[LCCLIENT_FILE_LEN];        
        char version[LCCLIENT_FILE_LEN];    
        int master;               
        int key;                  
    }default_server_arg_t;

    
    class lcclient_talkwith_t
    {
        plclic:
            int type;                       
            int success;                    
			IEvent *event;                  
            lc::NetReactor *reactor;        
        
            default_server_arg_t defaultserverarg;        
            lc_client_callbacktalkwith talkwithcallback;          
            void *callarg;        

			bsl::string servicename;          

            int retry;        
            bsl::var::IVar *conf;         
            bsl::ResourcePool rp;         

            int wait;         
            long long dusep_currno;       
        plclic:
            
            lcclient_talkwith_t() {
                type = -1;
                success = 0;
                event = NULL;
                reactor = NULL;
                retry = 0;

                memset(defaultserverarg.range, 0, LCCLIENT_FILE_LEN);
                memset(defaultserverarg.version, 0, LCCLIENT_FILE_LEN);
                defaultserverarg.master = 1;
                defaultserverarg.key = -1;
                wait = 0;
                dusep_currno = 0;
            }
            
            virtual ~lcclient_talkwith_t(){}
    };

    class common_talkwith_t : plclic lcclient_talkwith_t
	{
        plclic:
            void *serverArg;                  

            int readtimeout;                  
            int writetimeout;                 
            LcClientConnection *conn;         
            LcClientServer *server;           
            LcClientPool *pool;               
            LcClientManager *mgr;             
            LcClientEventPool *eventpool;     

            int ipnum;                        

            IEvent::cb_t user_callback;       
            void *user_arg;       
        plclic:
            
            common_talkwith_t() {
                type = LCCLIENT_COMMON;
                success = 0;
                event = NULL;
                reactor = NULL;

                talkwithcallback = NULL;
                callarg = NULL;
                serverArg = NULL;

                readtimeout = 0;
                writetimeout = 0;
                conn = NULL;
                server = NULL;
                pool = NULL;
                mgr = NULL;
                eventpool = NULL;
                ipnum = 0;

                user_callback = NULL;
                user_arg = NULL;
            }

            
            virtual ~common_talkwith_t(){}
	};

    
    typedef struct _server_conf_info_t {
        int readtimeout;         
        int writetimeout;        
        int reserve1;             
        int reserve2;             
        int reserve3;             
        long long reserve4;      
    }server_conf_info_t;

    
    typedef struct _lcclient_nshead_atalk_t
    {
        nshead_talkwith_t *talk;          
        int readtimeout;                  
        int writetimeout;                 
        LcClientConnection *conn;         
        LcClientServer *server;           
        LcClientPool *pool;               
        LcClientManager *mgr;             
        LcClientEventPool *eventpool;     

		int ipnum;                        
        nshead_atalk_t slctalk;           

    }lcclient_nshead_atalk_t;
}









#endif  


