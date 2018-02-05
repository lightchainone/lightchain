



#ifndef  __LCCLIENT_CONNECTMANAGER_H_
#define  __LCCLIENT_CONNECTMANAGER_H_

#include <sys/socket.h>
#include <netinet/tcp.h>

#include <connection.h>
#include <connectmgr.h>
#include <Configure.h>
#include <bsl/map.h>
#include <bsl/var/IVar.h>

#include "lcclient_connection.h"
#include "lcclient_nshead.h"
#include "lcclient_utils.h"

namespace lc {

    class LcClientManager;
    class LcClientStrategy;
    class LcClientHealthyChecker;

    
    class ConnectionRequestex
    {
        plclic:
            int key;                        
            int nthRetry;                   
            default_server_arg_t serverArg; 

            int serverID;                   
            int err;                        
            comcm::Connection *conn;        
            const char *tag;                
            
            int reserve1;                   
            int reserve2;                   
            void *reserve3;                 
            void *reserve4;                 

            ConnectionRequestex(){};        
            ~ConnectionRequestex(){};       
    };

    
    class ConnectionRes
    {
        plclic:
            IntVector selectedServerID;       
            IntVector resultServerID;         
    };
    
    
    class LcClientConnectManager : plclic comcm::ConnectManager 
    {
        friend class LcClientConnection;
        friend class LcClientServer;
        plclic:
            
            LcClientConnectManager(); 
            
            
            ~LcClientConnectManager();            

            
            LcClientConnection* fetchConnectionex(const default_server_arg_t &serverArg, 
                                                  bsl::var::IVar& selectServerVar, LcClientManager *mgr);

            
            int fetchConnectionAsync(lcclient_talkwith_t *talkmsg, bsl::var::IVar &selectServerVar, 
                                     lc::IEvent * event, lc::NetReactor *reactor);

            
            int fetchConnectionRetry(lcclient_talkwith_t *talkmsg, bsl::var::IVar &selectServerVar,
                                     lc::IEvent * event, lc::NetReactor *reactor);
            
            int reload(const comcfg::ConfigUnit & conf);

            
            int reload(comcm::Server ** svrs, int svrn);

            
            int setConnType(int connType);

            
            int init(const comcfg::ConfigUnit & conf);

            
            int init(comcm::Server ** svrs, int svrnum, bool shortConnection);

            
            int getReqAndResBuf(int &reqBufLen, int &resBufLen);

            
            int getReqBufLen();

            
            int getResBufLen();

            
            LcClientStrategy *getStrategy();

            
            void readLock(){_rwlock.read();}

            
            void unlock(){_rwlock.unlock();}  

            
            int isConnAll(){return _isConnAll;}

            
            int connectAll();

            long long getReloadTag(){return _reload_tag;}

            int getRealServerSize();

            const char *getServiceName() {return _name.c_str();}
        protected:
            
            comcm::Server ** readConfigData(const comcfg::ConfigUnit & conf, int *err);

        protected:
            server_conf_info_t _default_server_conf;    
            int _defaultLinger;         
            int _defaultReqBufLen;      
            int _defaultResBufLen;      
            int _defaultWaitNum;        
            int _defaultFetchConnTime;  
            int _isConnAll;             

            long long _reload_tag;

            bsl::string _name;
    };
}








#endif  


