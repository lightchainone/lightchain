#include "lcclient_strategy.h"

#include <lc_log.h>

#include "lcclient_connectserver.h"

namespace lc {

    LcClientStrategy :: LcClientStrategy()
    {
        _confSaveStr[0] = '\0';
        _confSaveStrLen = 0;
        srand(time(0) ^ pthread_self());
    }

    LcClientStrategy :: ~LcClientStrategy()
    {
        
    }

    int LcClientStrategy :: load(const comcfg::ConfigUnit & conf)
    {
        comcm::AutoWrite autowrite(&_confLock);

        bsl::ResourcePool rp;
        int errNo = 0;
        bsl::var::IVar &tempVar = conf.to_IVar(&rp, &errNo);
        if (0 != errNo) {
            LC_LOG_WARNING("convert conf to var error [%d]", errNo);
            return -1;
        }
        comcfg::Configure tempConf;
        int ret1 = tempConf.loadIVar(tempVar);
        if (0 != ret1) {
            LC_LOG_WARNING("var to conf error [%d]", ret1);
            return -1;
        }
        size_t strSaveLen = 0;
        char *strSave = tempConf.dump(&strSaveLen);
        if (NULL == strSave) {
            LC_LOG_WARNING("get conf error");
            return -1;
        }

        
        _confSaveStrLen = snprintf(_confSaveStr, sizeof(_confSaveStr), "%s", strSave);
        if (_confSaveStrLen >= (int)sizeof(_confSaveStr)) {
            LC_LOG_WARNING("real conf size[%d] > default Size[%d]", _confSaveStrLen,
                           (int)sizeof(_confSaveStr));
            _confSaveStr[0] = '\0';
            _confSaveStrLen = 0;
            return -1;
        }
        return 0;
    }

    int LcClientStrategy :: reload(const comcfg::ConfigUnit & conf)
    {
        
        return load(conf);
    }

    int LcClientStrategy :: filterServer(ConnectionRequestex *req,
                                         bsl::var::IVar & ,
                                         ConnectionRes *serverSelectedInfo)
    {
        if (NULL == req|| NULL == serverSelectedInfo) {
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
            if(NULL != svr && svr->isEnable()) {
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

    int LcClientStrategy :: balanceServer(ConnectionRequestex *req,
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

        int totalNum = _mgr->getServerSize();
        if (totalNum <= 0) {
            LC_LOG_WARNING("server num is 0");
            return -1;
        }
        
        int maxNum = serverSelectedInfo.resultServerID.size();
        int retry = 0;
        int hashIndex = req->key;
        if (-1 == hashIndex) {
            hashIndex = rand() % maxNum;
            return serverSelectedInfo.resultServerID[hashIndex];
        }
        
        unsigned int unHashKey = (unsigned int)hashIndex;
        while (retry < 3) {
            unsigned int realIndex = 0;
            if (retry == 0) {
                realIndex = unHashKey % totalNum;
            } else if (retry == 1) {
                if (totalNum == 1) {
                    break;
                }
                realIndex = unHashKey % (totalNum-1);
            } else {
                realIndex = rand() % maxNum;
                return serverSelectedInfo.resultServerID[realIndex];
            }
            for (int i=0; i<maxNum; i++) {
                if (serverSelectedInfo.resultServerID[i] == (int)realIndex) {
                    return realIndex;
                }
            }
            ++ retry;
        }        

        return -1;
    }

    int LcClientStrategy :: fetchServer(ConnectionRequestex *req,
                                        bsl::var::IVar & confVar,
                                        ConnectionRes *serverSelectedInfo)
    {
        if (req == NULL || NULL == serverSelectedInfo) {
            return -1;
        }

        int ret = 0;

        ret = filterServer(req, confVar, serverSelectedInfo);
        if (ret != 0) {
            LC_LOG_WARNING("filterServer error");
            return -1;
        }

        ret = balanceServer(req, confVar, *serverSelectedInfo);
        if (ret < 0) {
            LC_LOG_WARNING("balanceServer error");
            return -1;
        }
        serverSelectedInfo->resultServerID.clear();
        
        return ret;
    }

    bsl::var::IVar & LcClientStrategy :: getConf(bsl::ResourcePool *rp, int *errNo)
    {
        static bsl::var::Null emptyVar;

        if (NULL == errNo) {
            return emptyVar;
        }

        if (NULL == rp) {
            *errNo = -1;
            return emptyVar;
        }

        comcm::AutoRead autoread(&_confLock);
        if (0 == _confSaveStrLen) {
            *errNo = -1;
            return emptyVar;
        }
        comcfg::Configure tempConf;
        char *tempBuf = tempConf.getRebuildBuffer(_confSaveStrLen); 
        if (NULL == tempBuf) {
            LC_LOG_WARNING("getRebuildBuffer of conf error");
            *errNo = -1;
            return emptyVar;
        }
        snprintf(tempBuf, _confSaveStrLen, "%s", _confSaveStr);
        int ret_build = tempConf.rebuild();
        if (0 != ret_build) {
            LC_LOG_WARNING("build conf from string error:%d", ret_build);
            *errNo = -1;
            return emptyVar;
        }
        bsl::var::IVar & conf = tempConf.to_IVar(rp, errNo); 
        if (*errNo != 0) {
            LC_LOG_WARNING("conf to var error[%d]", *errNo);
            *errNo = -1;
            return emptyVar;
        }        

        *errNo = 0;
        return conf;
    }

    int LcClientStrategy :: setServerArg(LcClientServer *server, const nshead_talkwith_t *talk) 
    {
        LC_LOG_DEBUG("[%s]", __func__);

        if (NULL == server || NULL == talk) {
            return -1;
        }

        return 0;
    }

    int LcClientStrategy :: setServerArgAfterConn(LcClientServer * , int ) 
    {
        return 0;
    }

}













