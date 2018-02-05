


#ifndef  __LCCLIENT_RSTRATEGY_H_
#define  __LCCLIENT_RSTRATEGY_H_

#include "lcclient_strategy.h"

namespace lc
{
    
    class LcClientRStrategy : plclic LcClientStrategy
    {
        plclic:
            
            LcClientRStrategy(){}

            
            virtual ~LcClientRStrategy(){}
            
            
            virtual int balanceServer(ConnectionRequestex *req,
                                      bsl::var::IVar & confVar, 
                                      const ConnectionRes &serverSelectedInfo);
    };
}















#endif  


