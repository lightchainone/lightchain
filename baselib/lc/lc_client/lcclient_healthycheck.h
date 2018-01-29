#ifndef  __LCCLIENT_HEALTHYCHECK_H_
#define  __LCCLIENT_HEALTHYCHECK_H_

#include <strategy.h>
#include <Configure.h>
#include <bsl/ResourcePool.h>
#include <bsl/var/IVar.h>

#include "lcclient_connection.h"
#include "lcclient_connectmanager.h"

namespace lc {

    
    class LcClientHealthyChecker : plclic comcm::HealthyChecker {
        plclic:
            
            LcClientHealthyChecker();
            
            
            virtual ~LcClientHealthyChecker();

            
            virtual int load(const comcfg::ConfigUnit & conf);

            
            virtual int reload(const comcfg::ConfigUnit & conf);

            
            virtual int healthyCheck();

            
            virtual bsl::var::IVar & getConf(bsl::ResourcePool *rp, int *errNo);

        protected:
            LcClientConnectManager  *_lcclient_manager;       

            comcm::RWLock _confLock;                          
            char _confSaveStr[LCCLIENT_CONFSAVE_LEN];         
            int _confSaveStrLen;                              
    };

}













#endif  


