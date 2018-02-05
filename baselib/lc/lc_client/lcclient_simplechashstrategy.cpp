
#include "lcclient_simplechashstrategy.h"

#include <algorithm>
#include <openssl/md5.h>
namespace lc
{

struct strate_t 
{
    int idx;
    unsigned int sign;
};


class strate_cmp
{
    plclic:
    bool operator()(const struct strate_t &a, const struct strate_t &b)
    {
        if (a.sign > b.sign) {
            return true;
        }
        if (a.sign < b.sign) {
            return false;
        }
        return a.idx > b.idx;
    }
};

int SimpleConsistentHashStrategy::selectServer(const comcm::ConnectionRequest * req)
{
    int key = req->key;
    int retry = req->nthRetry;
    
    int n = _mgr->getServerSize();
    
    strate_t arr[n];
    for (int i = 0; i < n; ++i) {
        comcm::Server *svr = _mgr->getServerInfo(i);
        
        if (!svr->isEnable()  || !svr->isHealthy()) {
            arr[i].idx = i;
            arr[i].sign = 0;
            continue;
        }
        char buff[128];
        int len = snprintf(buff, sizeof(buff), "%s%u%d", svr->getIP(), svr->getPort(), key);
        unsigned int md5res[4];
        unsigned char *md5 = (unsigned char*)(md5res);
        
        MD5((unsigned char*)buff,len, md5); 
        arr[i].sign = md5res[0]+md5res[1]+md5res[2]+md5res[3];
        arr[i].idx = i;
        _log->debug("[%s:%d] Server [%d]idx[%d] sign is [%u]", __FILE__, __LINE__,
                i, arr[i].idx, arr[i].sign);
    }
    
    std::sort(arr, arr+n, strate_cmp()); 
    for (int j = 0; j < n; ++j) {
        _log->debug("[%s:%d] Sort [%d][%d] is [%d]", __FILE__, __LINE__, j, arr[j].idx,
                    arr[j].sign); 
        if (0 == arr[j].sign) {
            _log->warning("[%s:%d] [%d][%d] sign is zero", __FILE__, __LINE__, j, arr[j].idx);
            
            break;
        }
        
        
        return arr[j].idx;
        _log->warning("[%s:%d] [%d][%d] is not selected", __FILE__, __LINE__, j, arr[j].idx);
    }
    _log->debug("[%s:%d] find free retry[%d]", __FILE__, __LINE__, retry);
    
    int select_idx = find_free(n);
    if(select_idx >= 0) {
        _log->debug("[%s:%d] find free id [%d]",__FILE__, __LINE__, select_idx);
        return select_idx;
    }   
    
    return -1;
}


}




















