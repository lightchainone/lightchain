
#include "lcclient_healthystrategy.h"

#include <lc_log.h>

#include "lcclient_connectserver.h"

namespace lc
{
    int LcClientStrategyWithHealthy :: filterServer(ConnectionRequestex *req, 
                                                	bsl::var::IVar & ,
                                                	ConnectionRes *serverSelectedInfo)
    {
        LC_LOG_DEBUG("%s", __func__);
        
        if (NULL == req || NULL == serverSelectedInfo) {
            return -1;
        }

        int serverNum = _mgr->getServerSize();
        if (serverNum <= 0) {
            LC_LOG_WARNING("getServerSize[%d] error", serverNum);
            return -1;
        }
        int i = 0;
        LC_LOG_DEBUG("server num : [%d]", serverNum);
        for (i=0; i<serverNum; i++) {
            LcClientServer *svr = dynamic_cast<LcClientServer *>(_mgr->getServerInfo(i));
            if(NULL != svr && svr->isEnable() && svr->isHealthy()) {
                int j = 0;
                for (j=0; j<serverSelectedInfo->selectedServerID.size(); j++) {
                    if (serverSelectedInfo->selectedServerID[j] == i) {
                        break;
                    }
                }
                if (j < serverSelectedInfo->selectedServerID.size()) {
                    continue;
                }
                LC_LOG_DEBUG("push [%d]", i);
                if (serverSelectedInfo->resultServerID.push_back(i) != 0) {
                    LC_LOG_WARNING("reach server list size : %d", LCCLIENT_SELECTEDSERVER_SIZE);
                    return -1;
                }
			}
        }

        return 0;
        
    }
}





















