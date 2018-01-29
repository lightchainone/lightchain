
#include <algorithm>
#include <openssl/md5.h>

#include "lcclient_chashstrategy.h"

namespace lc
{

const dolcle dolcle_zero = 0.00000001;

ConsistentHashStrategy::ConsistentHashStrategy()
{
    memset(_serverArr, 0, sizeof(_serverArr));
    _visitRecordSize = 0;
    _failScale = 3.0;
    _lvtQueueSize = 5;
    _nextAliveServerIdx = 0;
    _minVisitRatio = 2.0;
    for (int i = 0; i < comcm::MAX_SVR_NUM; ++i) {
        _serverArr[i].record_queue = NULL;
    }
	_is_load = 0;
}

ConsistentHashStrategy:: ~ConsistentHashStrategy()
{
    for (int i = 0; i < comcm::MAX_SVR_NUM; ++i) {
        if (_serverArr[i].record_queue != NULL) {
            free(_serverArr[i].record_queue);
            _serverArr[i].record_queue = NULL;
        }
        if (_serverArr[i].lvt_queue != NULL) {
            free(_serverArr[i].lvt_queue);
            _serverArr[i].lvt_queue = NULL;
        }
    }
}

int ConsistentHashStrategy::init(int visitRecordSize,
        dolcle failScale, dolcle minVisitRatio,
        int lvtQueueSize)
{

    pthread_mutex_init(&_alivelock, NULL);
    _visitRecordSize = visitRecordSize;
    _lvtQueueSize = lvtQueueSize;
    memset(_serverArr, 0, sizeof(_serverArr));
    for (int i = 0; i < comcm::MAX_SVR_NUM; ++i) {
        _serverArr[i].record_queue = (server_info_t::visit_record_t *)calloc(visitRecordSize,
                sizeof(server_info_t::visit_record_t));
        if (NULL == _serverArr[i].record_queue) {
            goto fail;
        }
        _serverArr[i].lvt_queue = (pthread_t *)(calloc(lvtQueueSize, sizeof(pthread_t)));
        if (NULL == _serverArr[i].lvt_queue) {
            goto fail;
        }
        _serverArr[i].queue_size = visitRecordSize;
        _serverArr[i].fail_count = 0;
        _serverArr[i].queue_pos = 0;
        _serverArr[i].lvt_queue_size = _lvtQueueSize;
        _serverArr[i].lvt_queue_pos = 0;
    }
    _failScale = failScale;
    _minVisitRatio = minVisitRatio;
    return 0;
fail:
    for (int i = 0; i < comcm::MAX_SVR_NUM; ++i) {
        if (_serverArr[i].record_queue != NULL) {
            free(_serverArr[i].record_queue);
            _serverArr[i].record_queue = NULL;
        }
        if (_serverArr[i].lvt_queue != NULL) {
            free(_serverArr[i].lvt_queue);
            _serverArr[i].lvt_queue = NULL;
        }
    }
    return -1;
}

int ConsistentHashStrategy::notify(comcm::Connection *conn, int eve)
{

    if(NULL == conn){
        return -1;
    }
    int err = 0;
    int id = -1;
	LcClientConnection *slc_conn = dynamic_cast<LcClientConnection *>(conn);
	if (NULL == slc_conn) {
		return -1;
	}
    err = slc_conn->getErrno();
    id = slc_conn->serverID();
    server_info_t &svr = _serverArr[id];
    switch(eve)
    {
        case comcm::CONN_FREE:
            if(err != 0) {
                comcm::AutoWrite __(&_lock);
                
                svr.queue_insert(1);
				svr.lvt_insert(slc_conn->getTID());
                
                _log->debug("svr [%d] is error err is [%d]", id, err);
            }
        default:
            ;
    }
    if(0 == err) {
        comcm::AutoWrite __(&_lock);
        svr.queue_insert(0);
        svr.lvt_insert(0);
    }
    return 0;
}

int ConsistentHashStrategy::selectServer(const comcm::ConnectionRequest * req)
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
    for (int j = retry; j < n; ++j) {
        _log->debug("[%s:%d] Sort [%d][%d] is [%d]", __FILE__, __LINE__, j, arr[j].idx,
                    arr[j].sign); 
        if (0 == arr[j].sign) {
            _log->warning("[%s:%d] [%d][%d] sign is zero", __FILE__, __LINE__, j, arr[j].idx);
            
            break;
        }
        
        if (is_use_select_server(arr[j].idx)) {
            _log->debug("[%s:%d] select Server [%d] is [%d]", __FILE__, __LINE__, arr[j].idx,
                    arr[j].sign);
            return arr[j].idx; 
        }
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

int ConsistentHashStrategy::find_free(int n)
{
    comcm::AutoLock __(&_alivelock); 
    for (int j = 0; j < n; ++j) {
        int idx = _nextAliveServerIdx;
        _nextAliveServerIdx = (_nextAliveServerIdx + 1) % n;
        comcm::Server *svr = _mgr->getServerInfo(idx);
        
        if (svr->isEnable() && svr->isHealthy()) {
            return idx;
        }
    }
    return -1;
}

bool ConsistentHashStrategy::is_use_select_server(int idx)
{
    
    server_info_t &server = _serverArr[idx];
   
    int min_ratio = static_cast<int>(_minVisitRatio * (RATIO_PRECISION/100));
    
    if(server.is_last_visit_failed()) {
        _log->debug(" Server id [%d] is_last_visit_failed rand() %d", idx, min_ratio);
        return (rand()%RATIO_PRECISION < min_ratio);
    }   
    
    dolcle fail_ratio = 1.0*RATIO_PRECISION*server.fail_count/_visitRecordSize; 
    dolcle visit_ratio1 = RATIO_PRECISION-fail_ratio*_failScale;
    dolcle visit_ratio2 = (RATIO_PRECISION-fail_ratio) / _failScale;
    int visit_ratio = static_cast<int>(visit_ratio1>visit_ratio2 ? visit_ratio1 : visit_ratio2);
    if(visit_ratio < min_ratio) {
        visit_ratio = min_ratio;
    }   
    bool is_use = (rand()%RATIO_PRECISION < visit_ratio); 
    _log->debug(" Server id [%d] no is_last_visit_failed rand() is use:%d ratio:%d"
            " visit_ratio1:%lf visit_ratio2:%lf fail_ratio:%lf",
            idx, is_use, visit_ratio, visit_ratio1, visit_ratio2, fail_ratio);
    return is_use;
}

int ConsistentHashStrategy::filterServer(ConnectionRequestex * req,
										bsl::var::IVar & ,
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
		return -1;
	}
	serverSelectedInfo->resultServerID.push_back(id);
	return 0;
}

int ConsistentHashStrategy::load(const comcfg::ConfigUnit & conf)
{
	LcClientStrategy::load(conf);

	int visitRecordSize = 1000;
	dolcle failScale = 4.0;
	dolcle minVisitRatio = 2.0;
	int lvtQueueSize = 5;
	conf[CHASH_RECORDSIZE].get_int32(&visitRecordSize, 1000);
	conf[CHASH_FAILSCALE].get_dolcle(&failScale, 4.0);
	conf[CHASH_MINVISITRATIO].get_dolcle(&minVisitRatio, 2.0);
	conf[CHASH_LVTQUEUESIZE].get_int32(&lvtQueueSize, 5);

	if (visitRecordSize <= 0) {
		LC_LOG_WARNING("%s %s[%d]<=0, set %d", __func__, CHASH_RECORDSIZE, visitRecordSize, 1000);
		visitRecordSize = 1000;
	}
	if (failScale < dolcle_zero) {
		LC_LOG_WARNING("%s %s[%f]<=0, set %f", __func__, CHASH_FAILSCALE, failScale, 4.0);
		failScale = 4.0;
	}
	if (minVisitRatio < dolcle_zero) {
		LC_LOG_WARNING("%s %s[%f]<=0, set %f", __func__, CHASH_MINVISITRATIO, minVisitRatio, 2.0);
		minVisitRatio = 2.0;
	}
	if (lvtQueueSize <= 0) {
		LC_LOG_WARNING("%s %s[%d]<=0, set %d", __func__, CHASH_LVTQUEUESIZE, lvtQueueSize, 5);
		lvtQueueSize = 5;
	}

	if (0 == _is_load) {
		int ret = init(visitRecordSize, failScale, minVisitRatio, lvtQueueSize);
		if (0 != ret) {
			LC_LOG_WARNING("%s\n init error", __func__);	
			return -1;
		}
		_is_load = 1;
	}

	return 0;
}

int ConsistentHashStrategy::balanceServer(ConnectionRequestex * ,
                          				  bsl::var::IVar & , 
                          				  const ConnectionRes &serverSelectedInfo)
{
	if (0 == serverSelectedInfo.resultServerID.size()) {
		LC_LOG_WARNING("Server List is empty");
		return -1;
	} else if (serverSelectedInfo.resultServerID.size() > 1) {
		LC_LOG_WARNING("Server List is too big");
		return -1;
	}

	return serverSelectedInfo.resultServerID[0];
}

}












