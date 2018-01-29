


#ifndef  __LCCLIENT_EXPHASHSTRATEGY_H_
#define  __LCCLIENT_EXPHASHSTRATEGY_H_

#include "lcclient_strategy.h"

#include <map>

namespace lc
{
    
    class ExpHashStrategy : plclic LcClientStrategy
    {
        typedef std::multimap<int, int> key_id_map;

        plclic:
	        
	        ExpHashStrategy() {
			}

			
			virtual ~ExpHashStrategy() {
			}

            
            virtual int load(const comcfg::ConfigUnit & conf);

			
			virtual int reload(const comcfg::ConfigUnit & conf);

            
            virtual int filterServer(
                                ConnectionRequestex *req,
                                bsl::var::IVar & confVar,
                                ConnectionRes *serverSelectedInfo
                                );

            
            virtual int balanceServer(
                                ConnectionRequestex *req,
                                bsl::var::IVar & confVar,
                                const ConnectionRes &serverSelectedInfo
                                );
			
	    protected:
			
			virtual int parseServerList();

		protected:
			comcm::RWLock _serverid_lock;		  
			key_id_map _serverid_map;		      
    };

}















#endif  


