#include "lcclient_healthycheck.h"

#include <lc_log.h>

#include "lcclient_connectserver.h"
#include "lcclient_connectpool.h"

namespace lc {

    LcClientHealthyChecker :: LcClientHealthyChecker()
    {
        _lcclient_manager = NULL;

        _confSaveStr[0] = '\0';
        _confSaveStrLen = 0;
    }

    LcClientHealthyChecker :: ~LcClientHealthyChecker()
    {
        
    }

    int LcClientHealthyChecker :: load(const comcfg::ConfigUnit & conf)
    {
        comcm::AutoRead autoread(&_confLock);
        bsl::ResourcePool rp;
        int errNo;
        bsl::var::IVar &tempVar = conf.to_IVar(&rp, &errNo);
        if (0 != errNo) {
            LC_LOG_WARNING("conf to var error : [%d]", errNo);
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

    int LcClientHealthyChecker :: reload(const comcfg::ConfigUnit & conf)
    {
        return load(conf);
    }

    int LcClientHealthyChecker :: healthyCheck()
    {
        LC_LOG_DEBUG("healthyCheck()");
        int n = _mgr->getServerSize();
        int ret = 0;
        for(int i = 0; i < n; ++i){
            LcClientServer * svr = dynamic_cast<LcClientServer *>(_mgr->getServerInfo(i));

            if((svr != NULL) && svr->isEnable() && (svr->isHealthy() == false)){       
                LC_LOG_DEBUG("check server");
                int err = 0;
                comcm::Connection * c = svr->fetchConnection(&err);
                if(c != NULL && err == comcm::REQ_OK){
                    svr->setHealthy(true);
                    svr->freeConnection(c);
                    ++ret;
                }
            }
        }
    
        return ret;
    }

    bsl::var::IVar & LcClientHealthyChecker :: getConf(bsl::ResourcePool *rp, int *errNo)
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
            LC_LOG_WARNING("conf is empty");
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
        if (errNo != 0) {
            LC_LOG_WARNING("conf to var error[%d]", *errNo);
            *errNo = -1;
            return emptyVar;
        }

        *errNo = 0;
        return conf;
    }

}













