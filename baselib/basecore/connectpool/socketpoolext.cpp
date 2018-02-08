
#include "socketpoolext.h"

int SocketExtStrategy :: selectServer(const comcm::ConnectionRequest * req)
{
    int server_num = _mgr->getServerSize();
	if (server_num <= 0) {
        _log->warning("[%s:%d] server num:%d error", __FILE__, __LINE__, server_num);
		return -1;
	}
	
    std::vector<int> healthy_server_vec;

    for (int i=0; i<server_num; i++) {
        comcm::Server * svr = _mgr->getServerInfo(i);
        if (svr != NULL && svr->isEnable() && svr->isHealthy()) {
            healthy_server_vec.push_back(i);
        }
    }

    int healthy_server_num = healthy_server_vec.size();
    if (0 == healthy_server_num) {
        _log->warning("[%s:%d] has no healthy server", __FILE__, __LINE__);
        return -1;
    }

    int key = req->key;
    if (key < 0) {
        key = rand();
        return healthy_server_vec[ key % healthy_server_num ];
    }    

    
    int retry = 0;
    while (retry < 3) {
		int real_index = 0;
		if (retry == 0) {
			real_index = key % server_num;
		} else if (retry == 1) {
			if (server_num == 1) {
				return healthy_server_vec[0];
			}
			real_index = key % (server_num-1);
		} else {
			real_index = rand() % healthy_server_num;
			return healthy_server_vec[real_index];
		}
		for (int i=0; i<healthy_server_num; i++) {
			if (healthy_server_vec[i] == real_index) {
				return real_index;
			}
		}
		++ retry;
	}

    return -1;
}













