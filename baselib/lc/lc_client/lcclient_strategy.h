



#ifndef  __LCCLIENT_STRATEGY_H_
#define  __LCCLIENT_STRATEGY_H_

#include <strategy.h>
#include <Configure.h>
#include <bsl/ResourcePool.h>
#include <bsl/var/IVar.h>

#include "lcclient_connectmanager.h"
#include "lcclient_utils.h"

namespace lc {

    class ConnectionRequestex;

    
    class LcClientStrategy : plclic comcm::Strategy {
        plclic:
            
            LcClientStrategy();
            
            
            virtual ~LcClientStrategy();     

            
            virtual int load(const comcfg::ConfigUnit & conf);

            
            virtual int reload(const comcfg::ConfigUnit & conf);

            
            virtual int fetchServer(ConnectionRequestex *req,
                                    bsl::var::IVar & confVar,
                                    ConnectionRes *serverSelectedInfo);
            
            virtual int filterServer(ConnectionRequestex *req,
                                     bsl::var::IVar & confVar, 
                                     ConnectionRes *serverSelectedInfo);

            
            virtual int balanceServer(ConnectionRequestex *req,
                                      bsl::var::IVar & confVar, 
                                      const ConnectionRes &serverSelectedInfo);

            
            virtual bsl::var::IVar & getConf(bsl::ResourcePool *rp, int *errNo);

            
            virtual int setServerArg(LcClientServer *server, const nshead_talkwith_t *talk);
 
            
            virtual int setServerArgAfterConn(LcClientServer *server, int errNo);
       
            
            virtual int isInputRangeOK(const char * ) {return 0;}
        
            
            virtual int isServerRangeOK(const char * ) {return 0;}

        protected:
            comcm::RWLock _confLock;               
            
            char _confSaveStr[LCCLIENT_CONFSAVE_LEN]; 
            int _confSaveStrLen;                      
    };
}










#endif  


