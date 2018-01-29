

#include "lcclient_exphashstrategy.h"
#include <bsl/ResourcePool.h>
#include "lcclient_connectserver.h"

namespace lc
{
    int ExpHashStrategy :: load(const comcfg::ConfigUnit & )
    {
        return parseServerList();
    }

    int ExpHashStrategy :: parseServerList()
    {
        if (NULL == _mgr) {
            LC_LOG_WARNING("LcClientConnectManager is NULL");
            return -1;
        }

        int server_num = _mgr->getServerSize();
        if (server_num <= 0) {
            LC_LOG_WARNING("getServerSize[%d] error", server_num);
            return -1;
        }

    	_serverid_lock.write();
	    _serverid_map.clear();

        bsl::ResourcePool rp;
        int error_num = 0;
        for (int i=0; i<server_num; i++) {
            LcClientServer *svr = dynamic_cast<LcClientServer *>(_mgr->getServerInfo(i));
            if (NULL == svr) {
                continue;
            }

            if (!svr->isEnable()) {
                continue;
            }

            bsl::var::IVar &server_conf = svr->getConf(&rp, &error_num);
            if (0 != error_num) {
                LC_LOG_WARNING("get conf from server[ip:%s, port:%d] error", svr->getIP(), svr->getPort());
                goto Fail;
            }
            
            try {
		        if (server_conf["hash_key"].is_null()) {
                    LC_LOG_WARNING("server[ip:%s, port:%d] not has hash_key", svr->getIP(), svr->getPort());
		            continue;
		        }
                int id = server_conf["hash_key"].to_int32();
                _serverid_map.insert(std::pair<int, int>(id, i));
            } catch (bsl::Exception &e) {
                LC_LOG_WARNING("catch bsl exception:%s", e.all());
                goto Fail;
            } catch (std::exception &e) {
                LC_LOG_WARNING("catch std exception:%s", e.what());
                goto Fail;
	        } catch (...) {
                LC_LOG_WARNING("catch unknown exception");
                goto Fail;
	        }
        }
        _serverid_lock.unlock();
	    return 0;
    Fail:
	    _serverid_map.clear();
	    _serverid_lock.unlock();
	    return -1;
    }

    int ExpHashStrategy :: reload(const comcfg::ConfigUnit & )
    {
        return parseServerList();
    }

    int ExpHashStrategy :: filterServer(
                                       ConnectionRequestex *req,
                                       bsl::var::IVar & ,
                                       ConnectionRes *serverSelectedInfo
                                       )
    {
        if (NULL == req || NULL == serverSelectedInfo) {
            return -1;
        }

        int hashIndex = req->key;
        int serverid = 0;

	    _serverid_lock.read();
        key_id_map::iterator beg = _serverid_map.lower_bound(hashIndex);
        key_id_map::iterator end = _serverid_map.upper_bound(hashIndex);
        for (; beg != end; beg++) {
            serverid = beg->second;
            LcClientServer *svr = dynamic_cast<LcClientServer *>(_mgr->getServerInfo(serverid));
            if(NULL == svr) {
                LC_LOG_WARNING("serverid[%d] error", serverid);
                continue;
            }
            if (!svr->isHealthy() || !svr->isEnable()) {
                continue;
            }

			int j = 0;
			for (j=0; j<serverSelectedInfo->selectedServerID.size(); j++) {
				if (serverSelectedInfo->selectedServerID[j] == serverid) {
					break;
				}
			}
			
			if (j < serverSelectedInfo->selectedServerID.size()) {
				continue;
			}

			if (serverSelectedInfo->resultServerID.push_back(serverid) != 0) {
				LC_LOG_WARNING("reach server list size : %d", LCCLIENT_SELECTEDSERVER_SIZE);
				return -1;
			}
        }
 	    _serverid_lock.unlock();

        return 0;	
    }

    int ExpHashStrategy :: balanceServer(
                                        ConnectionRequestex *req,
                                        bsl::var::IVar & ,
                                        const ConnectionRes &serverSelectedInfo
                                        )
    {
        if (NULL == req) {
            return -1;
        }

	    int server_num = serverSelectedInfo.resultServerID.size();
        if (0 == server_num) {
            LC_LOG_WARNING("ServerList Num[%d] is error", server_num);
            return -1;
        }
        
        return serverSelectedInfo.resultServerID[0];
    }
}





















