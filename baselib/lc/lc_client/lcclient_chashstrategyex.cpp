
#include <algorithm>
#include <openssl/md5.h>

#include "lcclient_chashstrategyex.h"

namespace lc
{
    const unsigned int ConsistentHashStrategyEx::CHASH_MAX_SPLIT;
    const unsigned int ConsistentHashStrategyEx::DEFAULT_SPLIT_NUM;

    int ConsistentHashStrategyEx :: prepareSplit(const comcfg::ConfigUnit &conf)
    {
        
        svr_state_vec psvrst_vec_back[ConsistentHashStrategyEx::CHASH_MAX_SPLIT];
        unsigned int split_num_back = 0;
        unsigned int split_len_back = 0;

        if (conf["SplitNum"].selfType() == comcfg::CONFIG_ERROR_TYPE) {
            LC_LOG_WARNING("using default split num : %u", ConsistentHashStrategyEx::DEFAULT_SPLIT_NUM);
            split_num_back = ConsistentHashStrategyEx::DEFAULT_SPLIT_NUM;
        } else {
            conf["SplitNum"].get_uint32(&split_num_back, ConsistentHashStrategyEx::DEFAULT_SPLIT_NUM);
        }
        
        if (split_num_back == 0 || split_num_back >= ConsistentHashStrategyEx::CHASH_MAX_SPLIT) {
            LC_LOG_WARNING("SplitNum[%u] is zero or too big, using default value:%u", split_num_back,
                    ConsistentHashStrategyEx::DEFAULT_SPLIT_NUM);
            split_num_back = ConsistentHashStrategyEx::DEFAULT_SPLIT_NUM;
        }

        split_len_back = UINT_MAX / split_num_back;
        unsigned int tmp_split = 0;
        for (unsigned int i=0; i<split_num_back; i++) {
            tmp_split = i*split_len_back;
            int n = _mgr->getServerSize();
            unsigned int idx = i;
            for (int j=0; j<n; j++) {
			    comcm::Server *svr = _mgr->getServerInfo(j);
				
				if (!svr->isEnable() || !svr->isHealthy()) {
                    strate_t tmp_s;
                    tmp_s.idx = j;
                    tmp_s.sign = 0;
					psvrst_vec_back[idx].push_back(tmp_s);
					continue;
				}
				char buff[128];
				int len = snprintf(buff, sizeof(buff), "%s%u%d", svr->getIP(), svr->getPort(), tmp_split);
				unsigned int md5res[4];
				unsigned char *md5 = (unsigned char*)(md5res);
				
				MD5((unsigned char*)buff,len, md5);
                strate_t tmp_s;
                tmp_s.idx = j;
                tmp_s.sign = md5res[0]+md5res[1]+md5res[2]+md5res[3];
				psvrst_vec_back[idx].push_back(tmp_s);
			}
			std::sort(psvrst_vec_back[idx].begin(), psvrst_vec_back[idx].end(), strate_cmp());
        }

        comcm::AutoWrite auto_write(&_split_lock);
        for (unsigned int i=0; i<split_num_back; i++) {
            _psvrst_vec[i] = psvrst_vec_back[i];
        }
        _split_num = split_num_back;
        _split_len = split_len_back;
        return 0;
    }

    int ConsistentHashStrategyEx :: load(const comcfg::ConfigUnit & conf)
    {
        
        prepareSplit(conf);

        return ConsistentHashStrategy :: load(conf);
    }

    int ConsistentHashStrategyEx :: reload(const comcfg::ConfigUnit & conf) {
        prepareSplit(conf);
        return 0;
    }

    int ConsistentHashStrategyEx :: selectServer(const comcm::ConnectionRequest * req)
    {
        unsigned int key = (unsigned int)(req->key);
        int retry = req->nthRetry;
        
        comcm::AutoRead auto_read(&_split_lock);
        int tmp_split_num = (key/_split_len);

        if (tmp_split_num >= (int)_split_num) {
            LC_LOG_WARNING("req hash_key[%u] is bigger than MAX[%u], using rand()", key, UINT_MAX);
            tmp_split_num = rand() % _split_num;
        }

        int n = _mgr->getServerSize();
		for (int j = retry; j < n; ++j) {
			if (0 == _psvrst_vec[tmp_split_num][j].sign) {
				_log->warning("[%s:%d] [%d][%d] sign is zero", __FILE__, __LINE__, j, _psvrst_vec[tmp_split_num][j].idx);
				
				break;
			}
			
			if (is_use_select_server(_psvrst_vec[tmp_split_num][j].idx)) {
				_log->debug("[%s:%d] select Server [%d] is [%d]", __FILE__, __LINE__, _psvrst_vec[tmp_split_num][j].idx, _psvrst_vec[tmp_split_num][j].sign);
				return _psvrst_vec[tmp_split_num][j].idx; 
			}
			_log->warning("[%s:%d] [%d][%d] is not selected", __FILE__, __LINE__, j, _psvrst_vec[tmp_split_num][j].idx);
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




















