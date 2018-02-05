
#include "lcclient_rstrategy.h"

namespace lc
{
    int LcClientRStrategy :: balanceServer(ConnectionRequestex *req,
                                           bsl::var::IVar & , 
                                           const ConnectionRes &serverSelectedInfo)
    {
        if (req == NULL) {
            return -1;
        }
        if (0 == serverSelectedInfo.resultServerID.size()) {
            LC_LOG_WARNING("Server List is empty");
            return -1;
        }

        req->serverID = 0;
        int totalNum = (dynamic_cast<LcClientConnectManager*>(_mgr))->getRealServerSize();
        if (totalNum <= 0) {
            LC_LOG_WARNING("server num is 0");
            return -1;
        }
        int allServerNum = _mgr->getServerSize();        

        int maxNum = serverSelectedInfo.resultServerID.size();
        int hashIndex = req->key;
        if (-1 == hashIndex) {
            hashIndex = rand();
        }
        unsigned int unHashKey = (unsigned int)hashIndex;
		unsigned int realIndex = 0;
		realIndex = unHashKey % totalNum;
		int aliveServerIndex = -1;
		for (int i=0; i<allServerNum; i++) {
			if (_mgr->getServerInfo(i)->isEnable()) {
				if (0 == realIndex) {
					aliveServerIndex = i;
					break;
				}
				realIndex --;
			}
		}
		for (int i=0; i<maxNum; i++) {
			if (serverSelectedInfo.resultServerID[i] == aliveServerIndex) {
				return aliveServerIndex;
			}
		}        

        return -1;

    }
}




















