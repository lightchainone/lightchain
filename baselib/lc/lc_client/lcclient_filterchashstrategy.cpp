
#include "lcclient_filterchashstrategy.h"

namespace lc
{
    int FilterConsistentHashStrategy :: filterServer(ConnectionRequestex *req,
                                               bsl::var::IVar & confVar, 
                                               ConnectionRes *serverSelectedInfo)
    {
		if (NULL == req || NULL == serverSelectedInfo) {
			LC_LOG_WARNING("input NULL");
			return -1;
		}

		comcm::ConnectionRequest rs;
		rs.key = req->key;
		rs.nthRetry = req->nthRetry;
		int id = selectServer(&rs);
		if (-1 == id) {
            goto Fail;
		}

		
        if (confVar.is_null() || confVar["reqip"].is_null() || confVar["reqport"].is_null()) {
            goto Success;
        } else {
            comcm::Server *svr = _mgr->getServerInfo(id);
            if (NULL == svr) {
                goto Fail;
            }
            const char *ip = svr->getIP();
            int port = svr->getPort();
            bsl::var::IVar &req_ip_var = confVar["reqip"];
            bsl::var::IVar &req_port_var = confVar["reqport"];
            int ip_num = confVar["reqip"].size();
            int port_num = confVar["reqport"].size();
            if (ip_num != port_num) {
                LC_LOG_WARNING("reqip'num != reqport'num, can not filter by ip, so do not");
                goto Success;
            }

            for (int i=0; i<ip_num; i++) {
                if (strcmp(ip, req_ip_var[i].c_str()) == 0 && req_port_var[i].to_int32() == port)
                {
                    goto Fail;
                }
            }
        }
    Success:
        serverSelectedInfo->resultServerID.push_back(id);
        return 0;
    Fail:
        return -1;
    }

}





















