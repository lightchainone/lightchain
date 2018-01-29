


#ifndef  __LCCLIENT_HEALTHYSTRATEGY_H_
#define  __LCCLIENT_HEALTHYSTRATEGY_H_

#include "lcclient_strategy.h"

namespace lc
{
	
	class LcClientStrategyWithHealthy : plclic LcClientStrategy
	{
		plclic:
			
			LcClientStrategyWithHealthy(){}
			
			
			virtual ~LcClientStrategyWithHealthy(){}

			
			virtual int filterServer(ConnectionRequestex *req, 
									bsl::var::IVar & confVar,
									ConnectionRes *serverSelectedInfo);
	};
	
}














#endif  


