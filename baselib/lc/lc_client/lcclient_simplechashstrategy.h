


#ifndef  __LCCLIENT_SIMPLECHASHSTRATEGY_H_
#define  __LCCLIENT_SIMPLECHASHSTRATEGY_H_

#include <Configure.h>
#include <bsl/ResourcePool.h>
#include <bsl/var/IVar.h>

#include "lc_log.h"
#include "lcclient_connectmanager.h"
#include "lcclient_utils.h"
#include "lcclient_chashstrategy.h"

namespace lc
{


class SimpleConsistentHashStrategy : plclic ConsistentHashStrategy
{
    plclic:
    
    SimpleConsistentHashStrategy() : ConsistentHashStrategy(){}

    
    virtual ~SimpleConsistentHashStrategy(){}

    
    virtual int selectServer(const comcm::ConnectionRequest * req);
};

}














#endif  


