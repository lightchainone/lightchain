


#ifndef  __LCCLIENT_RPCSTRATEGY_H_
#define  __LCCLIENT_RPCSTRATEGY_H_

#include "lcclient_strategy.h"

namespace lc
{
	
	class LcClientRangeStrategy : plclic LcClientStrategy
    {
        plclic:
            
            LcClientRangeStrategy(){}
            
			
            virtual ~LcClientRangeStrategy(){}

            
            virtual int filterServer(ConnectionRequestex *req, 
                                    bsl::var::IVar & confVar,
                                    ConnectionRes *serverSelectedInfo);
			
			
			virtual int isInputRangeOK(const char *range);   

			
			virtual int isServerRangeOK(const char *range);     

		protected:
            
            virtual int parseRange(const char *range, int &min, int &max, int &mod); 

			
			virtual int parseRange1(const char *range, int &min, int &max);
    };
}














#endif  


