
#include "lcclient_selfstrategy.h"

#include <lc_log.h>

#include "lcclient_connectserver.h"
namespace lc
{
    int LcClientSelfStrategy :: parseIdConf()
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
        bsl::ResourcePool rp;
        int error_num = 0;
        str_int_multimap *temp_map1 = NULL;
        str_int_multimap *temp_map2 = NULL;
        if (_id_map_tag % 2 == 0) {
            temp_map1 = &_id_map;
            temp_map2 = &_id_map_back;
        } else {
            temp_map1 = &_id_map_back;
            temp_map2 = &_id_map;
        }
        str_int_multimap &temp_map = *temp_map1;

        for (int i=0; i<server_num; i++) {
            LcClientServer *svr = dynamic_cast<LcClientServer *>(_mgr->getServerInfo(i));
            bsl::var::IVar &server_conf = svr->getConf(&rp, &error_num);
            if (0 != error_num) {
                LC_LOG_WARNING("get conf from server error, ip[%s], port[%d]", svr->getIP(), svr->getPort());
                goto Fail;
            }
            
            try {
                const char *id = server_conf[CONF_SERVER_ID].c_str();
                if (NULL == id) {
                    LC_LOG_WARNING("server conf[id] is NULL");
                    goto Fail;
                }
                
                if (parseStr(id, temp_map, i) != 0) {
                    LC_LOG_WARNING("parse id[%s] error", id);
                    goto Fail;
                }
            
            } catch (bsl::Exception &e) {
                LC_LOG_WARNING("catch bsl exception:%s\n", e.what());
                goto Fail;
            }
        }
        
        _confLock.write();
        ++ _id_map_tag;
		_confLock.unlock();
        
        temp_map2->clear();
        return 0;
    Fail:
        
        clearMap();
        return -1;
    }

    int LcClientSelfStrategy :: load(const comcfg::ConfigUnit & )
    {
        
        int ret = parseIdConf();
        return ret;
    }

    int LcClientSelfStrategy :: reload(const comcfg::ConfigUnit & )
    {
        
        int ret = parseIdConf();
        
        return ret;
    }

    int LcClientSelfStrategy :: filterServer(
                                        ConnectionRequestex *req,
                                        bsl::var::IVar & confVar,      
                                        ConnectionRes *serverSelectedInfo
                                        )
    {
        if (NULL == req || NULL == serverSelectedInfo) {
            return -1;
        }

        bsl::string str_reqid;
        try {
            const char *req_id = confVar[CONF_SERVER_ID].c_str();
            if (NULL == req_id) {
                LC_LOG_WARNING("reqid in var parma is NULL");
                return -1;
            }
            str_reqid = req_id;
        } catch (bsl::Exception &e) {
            LC_LOG_WARNING("gte reqid from var param is error : %s", e.what());
            return -1;
        } catch (...) {
            LC_LOG_WARNING("unknown error occur");
            return -1;
        }
        comcm::AutoRead lock(&_confLock);
        str_int_multimap *temp_map1;
        if (_id_map_tag % 2 == 0) {
            temp_map1 = &_id_map_back;
        } else {
            temp_map1 = &_id_map;
        }
        str_int_multimap &temp_map = *temp_map1;
        str_int_multimap::iterator it_begin;
        str_int_multimap::iterator it_end;
        it_begin = temp_map.lower_bound(str_reqid);
        it_end = temp_map.upper_bound(str_reqid);
        if (it_begin == it_end) {
            return -1;
        }
        while (it_begin != it_end) {
            serverSelectedInfo->resultServerID.push_back(it_begin->second);
            it_begin ++;
        }
        return 0;
    }

    int LcClientSelfStrategy :: balanceServer(
                            ConnectionRequestex *req,
                            bsl::var::IVar & ,      
                            const ConnectionRes &serverSelectedInfo
                            )
    {
        if (req == NULL) {
            return -1;
        }
        if (0 == serverSelectedInfo.resultServerID.size()) {
            LC_LOG_WARNING("Server List is empty");
            return -1;
        }

        req->serverID = 0;

        
        int maxNum = serverSelectedInfo.resultServerID.size();
        int retry = 0;
        int hashIndex = req->key;
        if (-1 == hashIndex) {
            hashIndex = rand();
        }
        
        unsigned int unHashKey = (unsigned int)hashIndex;
        while (retry < 3) {
            unsigned int realIndex = 0;
            if (retry == 0) {
                realIndex = unHashKey % maxNum;
            } else if (retry == 1) {
                if (maxNum == 1) {
                    break;
                }
                realIndex = unHashKey % (maxNum-1);
            } else {
                
                
                for (int i=0; i<maxNum; i++) {
                    LcClientServer *svr = dynamic_cast<LcClientServer *>(
                            _mgr->getServerInfo(serverSelectedInfo.resultServerID[i]));
                    if (NULL == svr) {
                        continue;
                    }
                    if (!svr->isEnable() || !svr->isHealthy() || svr->isFull()) {
                        continue;
                    }
                    return serverSelectedInfo.resultServerID[i];
                }
                return -1;
            }
            int selectedId = serverSelectedInfo.resultServerID[realIndex];
            
            LcClientServer *svr = dynamic_cast<LcClientServer *>(_mgr->getServerInfo(selectedId));
            if (NULL == svr) {
                ++ retry;
                continue;
            } else if (!svr->isEnable() || !svr->isHealthy() || svr->isFull()) {
                ++ retry;
                continue;
            } else {
                return selectedId;
            }
        }        

        return -1;
    }

    int LcClientSelfStrategy :: parseStr(const char *str, str_int_multimap &id_map, int idx)
    {
        if (NULL == str) {
            LC_LOG_WARNING("id string is NULL");
            return -1;
        }

        int str_count = 0;
        char c = str[str_count++];
        char temp_buffer[1024];
        temp_buffer[0] = ' ';
        int count = 0;
        while (c != '\0') {
            if (c == ' ') {
                if (temp_buffer[0] == '\0') {
                    c = str[str_count++];
                    count = 0;
                    continue;
                } else {
                    temp_buffer[count] = '\0';
                    id_map.insert(std::pair<bsl::string, int>(temp_buffer, idx));
                    temp_buffer[0] = '\0';
                    count = 0;
                    continue;
                }
            }
            temp_buffer[count++] = c;
            c = str[str_count++];
        }
        if (temp_buffer[0] != '\0') {
            temp_buffer[count] = '\0';
            id_map.insert(std::pair<bsl::string, int>(temp_buffer, idx));
        }

        return 0;
    }

}




















