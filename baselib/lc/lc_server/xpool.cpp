#include <stdio.h>
#include <lc_log.h>
#include <ul_net.h>

#ifdef USE_SYSEPOLL
#include <sys/epoll.h>
#else
#include "epoll.h"
#endif

#include "lc_misc.h"
#include "lc_util.h"
#include "lcserver_core.h"
#include "xpool.h"
#include "pool.h"

#include "LcMonitor.h"



static int xpool_epoll_create()
{
	return epoll_create(1);
}

static int xpool_epoll_add(int epfd, int fd)
{
	struct epoll_event ev;
	ev.data.fd = fd;
	ev.events = EPOLLIN | EPOLLHUP | EPOLLERR;
	return epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);	
}

static int xpool_epoll_del(int epfd, int fd)
{
	return epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);	
}


static int xpool_epoll_wait(int epfd, int roto)
{
	struct epoll_event ready_ev;
	int ret = epoll_wait(epfd, &ready_ev, 1, roto);
	if(ret > 0) {
		return 1;
	} else if(ret == 0) {
		return 0;
	} else {
		LC_LOG_WARNING("epoll_wait failed, errno %d", errno);
		return -1;
	}
}

void *_xpool_main(void *param)
{
	lc_server_pdata_t *data = (lc_server_pdata_t *)param;
	lc_server_t *sev = (lc_server_t *)data->parent;
	xpool_t *pool = (xpool_t *)sev->pool;
	struct sockaddr_in caddr;
	socklen_t clen = sizeof(caddr);


	set_pthread_data(data);
	lc_log_initthread(sev->server_name);
	_LC_P_START;


	
	data->epfd = xpool_epoll_create();
	if(data->epfd < 0) {
		LC_LOG_FATAL("epoll_create failed, errno %d", errno);
		goto EXIT;
	}

	while (sev->run) {
		
		pthread_mutex_lock(&pool->lock);
		
		while (sev->run && ul_sreadable_ms(sev->sev_sock, LCSVR_MONITOR_TIME)<=0) {
			
		}
		if (!sev->run) {
			pthread_mutex_unlock(&pool->lock);
			break;
		}
		data->fd = ul_accept(sev->sev_sock, (struct sockaddr *)&caddr, &clen);
		pthread_mutex_unlock(&pool->lock);

		
		
        if (sev->monitor) {
            sev->monitor->add_request_connect_number(sev->server_name);
        }
		if (data->fd < 0) {
			LC_LOG_WARNING("accept fd[%d] < 0 err[%m]", data->fd);
			data->fd = -1;
			continue;
		}
		lc_setsockopt(data->fd, sev);

		
		data->ip = caddr.sin_addr.s_addr;

        lc_auth_t *auth = lc_server_get_ip_auth(sev);
        if (auth != NULL) {
			if(lc_auth_ip_int(auth, data->ip) == 0) {
			
			    char ipbuf[32];
				LC_LOG_MONITOR(EMSG_AUTH"invalid ip[%s] accessed!", 
						lc_inet_ntoa_r(caddr.sin_addr, ipbuf, sizeof(ipbuf)));
				close(data->fd);
				data->fd = -1;
				lc_auth_delref(auth);
				continue; 
			}

			
			lc_auth_delref(auth);
		}

		
		
		
		
		int roto = lc_server_get_read_timeout();
		

		int ret = xpool_epoll_add(data->epfd, data->fd);
		if(ret < 0) {
			LC_LOG_FATAL("foo_epoll_add failed, errno %d", errno);
			break;
		}

		while (sev->run) {
            if (sev->monitor) {
                sev->monitor->add_request_number_success(sev->server_name);
            }

			
			ret = xpool_epoll_wait(data->epfd, roto);

            if (ret < 0) {
                LC_LOG_MONITOR("wait for readable error[%d]", ret);
                break;
            } else if (ret == 0) {
                LC_LOG_MONITOR("wait for readable timeout[%d]", roto);

				in_addr addr;
				char ipbuf[32];
				addr.s_addr = data->ip;
				LC_LOG_WARNING("Connection timeout, socket[%d], ip[%s], timeout[%d]", 
					data->fd, 
					lc_inet_ntoa_r(addr, ipbuf, sizeof(ipbuf)),
					roto);
                break;
            }

			if (sev->cb_native() != 0) break;
			if (lc_get_session_connect_type_then_reset(sev) == LCSVR_SHORT_CONNECT) break;
		}

		xpool_epoll_del(data->epfd, data->fd);
		close(data->fd);
		data->fd = -1;
	}
EXIT:
	_LC_P_END;

	if(data->epfd > 0)
		close(data->epfd);

	lc_log_closethread();
    LC_LOG_DEBUG("xpool work thread stop");
	return NULL;
}

int xpool_init(lc_server_t *sev)
{
	if (sev->pool == NULL) {
		sev->pool = malloc(sizeof(xpool_t));
		if (sev->pool == NULL) {
			LC_LOG_WARNING("malloc size[%d] err for xpool", (int)sizeof(xpool_t));
			return -1;
		}
	}
	pthread_mutex_init(&((xpool_t *)sev->pool)->lock, NULL);
	LC_LOG_NOTICE("run xpool model");
	return 0;
}
int xpool_run(lc_server_t *sev)
{
	int i=0;
	sev->real_thread_num = 0;
	for (i=0; i<sev->pthread_num; ++i) {
		sev->data[i].id = i;

        int ret = 0;

        if (sev->stack_size > 0) {
			pthread_attr_t thread_attr;
			if (pthread_attr_init(&thread_attr) != 0) {
				LC_LOG_WARNING("init pthread_attr_t error[%m]");
				return -1;
			}
            if (pthread_attr_setstacksize(&thread_attr, sev->stack_size) != 0) {
                LC_LOG_WARNING("set pthread_attr_t stack error[%m]");
                return -1;
            }
			ret = pthread_create(&sev->data[i].pid, &thread_attr, _xpool_main, &sev->data[i]);
            pthread_attr_destroy(&thread_attr);
        } else {
			ret = pthread_create(&sev->data[i].pid, NULL, _xpool_main, &sev->data[i]);
        }
		if (ret != 0) {
			LC_LOG_WARNING("create thread err[%m] for xpool");
			return -1;
		}
		++ sev->real_thread_num;
	}
	return 0;
}

int xpool_join(lc_server_t *sev)
{
	int i=0;
	for (i=0; i<sev->real_thread_num; ++i) {
		pthread_join(sev->data[i].pid, NULL);
		LC_LOG_DEBUG("join %ld(%d) success in xpool", (long)sev->data[i].pid, i);
	}
	
	return 0;
}

long long xpool_get_socknum(lc_server_t *) {
    return 0;
}

long long xpool_get_queuenum(lc_server_t *) {
    return 0;
}

int xpool_destroy(lc_server_t *sev)
{
	xpool_t *pool = (xpool_t *)sev->pool;
	if (pool == NULL) return 0;

	pthread_mutex_destroy(&pool->lock);
	if (sev->pool != NULL)
		free(sev->pool);
	sev->pool = NULL;
    LC_LOG_DEBUG("xpool destroy");
	return 0;
}


