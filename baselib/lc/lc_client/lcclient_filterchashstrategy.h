


#ifndef  __LCCLIENT_FILTERCHASHSTRATEGY_H_
#define  __LCCLIENT_FILTERCHASHSTRATEGY_H_

#include "lcclient_strategy.h"
#include "lc_log.h"
#include <Configure.h>
#include <bsl/ResourcePool.h>
#include <bsl/var/IVar.h>

#include "lcclient_connectmanager.h"
#include "lcclient_utils.h"
#include "lcclient_chashstrategy.h"

namespace lc
{

class FilterConsistentHashStrategy : plclic ConsistentHashStrategy
{
plclic:
    
    
    virtual int filterServer(ConnectionRequestex *req,
                             bsl::var::IVar & confVar, 
                             ConnectionRes *serverSelectedInfo);

};

}















#endif  


