
#include "lcclient_connection.h"

#include <lc_log.h>
#include "lcclient_utils.h"
#include "lcclient_struct.h"
#include "lcclient_connectmanager.h"
#include "lcclient_define.h"
#include "lcclient_strategy.h"
#include "lcclient_nshead.h"
#include "lcclient_connectserver.h"

namespace lc {
    
    int LcClientConnection :: connect(comcm::Server *svr)
	{
		if (NULL == svr) {
            LC_LOG_WARNING("server is NULL");
			return -1;
		}
		
		LcClientServer *server = dynamic_cast<LcClientServer *>(svr);
		if (NULL == server) {
            LC_LOG_WARNING("server type error, not LcClientServer");
			return -1;
		}
		int i = 0;
		int ipCount = server->getIPCount();
		if (ipCount <= 0) {
			LC_LOG_WARNING("ipCount[%d] error", ipCount);
			return -1;
		}

		char *ip = NULL;                                  
		for (i=0; i<ipCount; i++) {
			ip = const_cast<char *>(server->getIP(i));
			if (ip != NULL) {
				_sock = ul_tcpconnecto_ms(ip, server->getPort(), server->getTimeout());
				if (_sock < 0) {
                    LC_LOG_WARNING("Connect failed to server %s:%d: %m", ip, server->getPort());
				} else {
					_last_connect = i;
					break;
				}
			}
		}

		if (_sock >= 0) {
			int on = 1;
			if (setsockopt(_sock,IPPROTO_TCP,TCP_NODELAY,&on, sizeof(on)) < 0) {
                LC_LOG_WARNING("setsockopt nodelay error [%m]");
                close (_sock);
                _sock = -1;
                return -1;
			}
			if (server->isLinger()) {
				struct linger li;
				memset(&li, 0, sizeof(li));
				li.l_onoff = 1;
				li.l_linger = 0;
				if (setsockopt(_sock, SOL_SOCKET, SO_LINGER, (const char*)&li, sizeof(li)) < 0 ) {
                    LC_LOG_WARNING("setsockopt so_linger error [%m]");
                    close (_sock);
                    _sock = -1;
                    return -1;
                }
			}
            
            snprintf(_peerip, sizeof(_peerip), "%s", svr->getIP());
            _peerport = svr->getPort();
			return 0;
		}
		return -1; 
	}

	int LcClientConnection :: getCurrIP()
	{
		return _last_connect;
	}

    int LcClientConnection :: setHandle(int fd) 
    {
        if (fd < 0) {
            return -1;
        }

        _sock = fd;
        return 0;
    }

    int LcClientConnection :: getIntFD()
    {
        int *handle = (int *)getHandle();
        int fd = *handle;
        return fd;
    }
}
















