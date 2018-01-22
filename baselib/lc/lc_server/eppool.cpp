#include <sched.h>
#include <time.h>
#include "eppool.h"
#include "pool.h"
#include "lc_server.h"
#include "LcMonitor.h"
enum {
#ifdef LC_DEBUG_OPEN
	DEFAULT_CHECK_INTERVAL = 1,
#else
	DEFAULT_CHECK_INTERVAL = 10,  
#endif
	DEFAULT_TIMEOUT = 60,         
	DEFAULT_QUEUE_LEN = 100,      
	DEFAULT_SOCK_NUM = 500,       
};

enum {
	IDLE = 0, 
	READY, 
	BUSY
};

#define _MAX(a,b) (((a)>(b)) ? (a) : (b))

int eppool_init(lc_server_t *sev)
{
	int qsiz = lc_server_get_cpool_queue_size(sev);
	int ssiz = lc_server_get_cpool_socket_size(sev);
	eppool_t *pool = NULL;

	if (qsiz <=0 || ssiz<=0) {
		qsiz = DEFAULT_QUEUE_LEN;
		ssiz = DEFAULT_SOCK_NUM;
		lc_server_set_cpool_queue_size(sev, qsiz);
		lc_server_set_cpool_socket_size(sev, ssiz);
	}

	if (sev->pool == NULL) {
		sev->pool = malloc(sizeof(eppool_t));
		if (sev->pool == NULL) {
			LC_LOG_WARNING("malloc size[%d] err for eppool", (int)sizeof(eppool_t));
			return -1;
		}
		memset(sev->pool, 0, sizeof(eppool_t));
	}
	pool = (eppool_t *)sev->pool;
	pthread_mutex_init(&pool->_ready_mutex, NULL);
	pthread_cond_init(&pool->_ready_cond, NULL);
	
	if (create_q(&pool->_queue, qsiz) != 0) {
		LC_LOG_WARNING("create queue err, at size[%d]", qsiz);
		return -1;
	}
	
	pool->_size = ssiz;
	pool->_sockets = (eppool_sock_item_t *)malloc(sizeof(eppool_sock_item_t)*pool->_size);
	if (pool->_sockets == NULL) {
		LC_LOG_WARNING("malloc socket item size[%d] err", (int)sizeof(eppool_sock_item_t)*pool->_size);
		return -1;
	}
	memset(pool->_sockets, 0, sizeof(eppool_sock_item_t) * pool->_size);

	
	pool->_ep_events = (struct epoll_event *)malloc(sizeof(struct epoll_event) * pool->_size);
	if (pool->_ep_events == NULL) {
		LC_LOG_WARNING("malloc epoll events[%d] for eppool err[%m]", pool->_size * (int)sizeof(struct epoll_event));
		return -1;
	}
	memset(pool->_ep_events, 0, pool->_size * sizeof(struct epoll_event));
	
	
	pool->_epfd = epoll_create(pool->_size);
	if (pool->_epfd < 0) {
		LC_LOG_WARNING("creat epoll[%d] err[%m]", pool->_epfd);
		return -1;
	}

	pool->_timeout = lc_server_get_cpool_timeout(sev);
	if (pool->_timeout <= 0) {
		pool->_timeout = DEFAULT_TIMEOUT;
	}
	pool->_check_interval = DEFAULT_CHECK_INTERVAL;
	pool->_run = &sev->run;
    pool->_using_size = 0;

	LC_LOG_NOTICE("run eppool model");
	return 0;

}

long long eppool_get_socknum(lc_server_t *sev)
{
    if (sev == NULL) return -1;
    eppool_t *pool = (eppool_t *)sev->pool;
    if (pool == NULL) return -1;

    for (int i=0; i<sev->real_thread_num; i++) {
        pool->_using_size -= sev->data[i].sock_num_dec;
        sev->data[i].sock_num_dec = 0;
    }

    if (pool->_using_size < 0) {
        pool->_using_size = 0;
    }
    return pool->_using_size;
}

long long eppool_get_queuenum(lc_server_t *sev)
{
    if (sev == NULL) return -1;
    
    eppool_t *pool = (eppool_t *)sev->pool;
    if (pool == NULL) return -1;

    return (long long)pool->_queue.size;
}

int eppool_run(lc_server_t *sev)
{
	int i = 0;
    int ret = 0;
	eppool_t *pool = (eppool_t *)sev->pool;
    
    if (sev->stack_size > 0 || sev->using_threadsche) {
	    int retval = -1;
		struct sched_param param = { 0 };
		pthread_attr_t thread_attr;

        ret = pthread_attr_init(&thread_attr);
		if (ret) {
			LC_LOG_WARNING("init pthread_attr_t error:%d [%m]", ret);
			return -1;
		}

		if (sev->stack_size > 0) {
            ret = pthread_attr_setstacksize(&thread_attr, sev->stack_size);
			if (ret) {
				LC_LOG_WARNING("set pthread_attr_t stack error:%d [%m]", ret);
				return -1;
			}
        }

        if (sev->using_threadsche) {
			ret = pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_JOINABLE);
			if (ret) {
				LC_LOG_WARNING("set detach state failed:%d [%m]", ret);
				goto done;
			}
			ret = pthread_attr_setinheritsched(&thread_attr, PTHREAD_EXPLICIT_SCHED);
			if (ret) {
				LC_LOG_WARNING("set EXPLICIT sched failed:%d [%m]", ret);
				goto done;
			}
			ret = pthread_attr_setschedpolicy(&thread_attr, SCHED_FIFO);
			if (ret) {
				LC_LOG_WARNING("set FIFO sched failed:%d [%m]", ret);
				goto done;
			}
			param.sched_priority = sev->listen_prio;
			ret = pthread_attr_setschedparam(&thread_attr, &param);
			if (ret) {
				LC_LOG_WARNING("set sched priority=%d failed:%d [%m]", param.sched_priority, ret);
				goto done;
			}
        }

		ret = pthread_create(&pool->_main, &thread_attr, _eppool_main, sev);
		if (ret) {
            LC_LOG_WARNING("pthread_create main_thread error:%d [%m]", ret);
            goto done;
        }
		
        sev->real_thread_num = 0;
		if (sev->using_threadsche) {
			param.sched_priority = sev->worker_prio;
			ret = pthread_attr_setschedparam(&thread_attr, &param);
			if (ret) {
				LC_LOG_WARNING("set worker sched with prio=%d failed:%d", param.sched_priority, ret);
                goto done;
			}
        }

		
		for (i=0; i<sev->pthread_num; ++i) {
			sev->data[i].id = i;
			ret = pthread_create(&sev->data[i].pid, &thread_attr, _eppool_workers, &sev->data[i]);
			if (ret) {
				LC_LOG_WARNING("create thread for eppool_workers error:%d [%m] but has create pthreadnum[%d]", ret, i+1);
				goto done;
			}
			++ sev->real_thread_num;
		}
        retval = 0;;
done:
	    pthread_attr_destroy(&thread_attr);
	    return retval;
	} else {
		ret = pthread_create(&pool->_main, NULL, _eppool_main, sev);
		if (ret) {
            LC_LOG_WARNING("pthread_create main_thread error:%d [%m]", ret);
            return -1;
        }

        sev->real_thread_num = 0;
		
		for (i=0; i<sev->pthread_num; ++i) {
			sev->data[i].id = i;
			ret = pthread_create(&sev->data[i].pid, NULL, _eppool_workers, &sev->data[i]);
			if (ret) {
				LC_LOG_WARNING("create thread for eppool_workers error:%d [%m] but has create pthreadnum[%d]", ret, i+1);
				return -1;
			}
			++ sev->real_thread_num;
		}

        return 0;
	}

}

int eppool_join(lc_server_t *sev)
{
	eppool_t *pool = (eppool_t *)sev->pool;
    if (sev->real_thread_num >= 0) {
	    pthread_join(pool->_main, NULL);
	    for (int i=0; i<sev->real_thread_num; ++i) {
		    pthread_join(sev->data[i].pid, NULL);
	    }
    }
	return 0;
}

int eppool_destroy(lc_server_t *sev)
{
	eppool_t *pool = (eppool_t *)sev->pool;
	if (pool == NULL) return 0;
	pthread_mutex_destroy(&pool->_ready_mutex);
	pthread_cond_destroy(&pool->_ready_cond);
	destroy_q(&pool->_queue);
	if (pool->_sockets != NULL) {
		free (pool->_sockets);
		pool->_sockets = NULL;
	}
	if (pool->_ep_events != NULL) {
		free (pool->_ep_events);
		pool->_ep_events = NULL;
	}
	if (pool->_epfd > 0) {
		close(pool->_epfd);
	}
	free (sev->pool);
	sev->pool = NULL;
    LC_LOG_DEBUG("epool destroy");
	return 0;
}

static void _eppool_close_pool_sockets(eppool_t *pool)
{
	pthread_mutex_lock(&pool->_ready_mutex);
	while (!is_empty_q(&pool->_queue)) {
		int hd = 0;
		pop_q(&pool->_queue, &hd);
		close(pool->_sockets[hd].sock);
		pool->_sockets[hd].sock = -1;
		pool->_sockets[hd].status = IDLE;
	}
	pthread_mutex_unlock(&pool->_ready_mutex);

	for (u_int i=0; i<pool->_size; ++i) {
		if (pool->_sockets[i].status != IDLE) {
			pool->_sockets[i].status = IDLE;
			close(pool->_sockets[i].sock);
			pool->_sockets[i].sock = -1;
		}
	}
}

static inline void 
__print_sched_info(const char *msg, int policy, const struct sched_param *param)
{
	const char *policy_str = "unknown";
	
	if (msg == NULL) {
		msg = "";
	}

	if (param == NULL) {
		LC_LOG_WARNING("sched param is NULL");
		return;
	}

	switch (policy) {
	case SCHED_OTHER:
		policy_str = "OTHER";
		break;
	case SCHED_FIFO:
		policy_str = "FIFO";
		break;
	case SCHED_RR:
		policy_str = "Round Robin";
		break;
	}

	LC_LOG_WARNING("(%s)sched_policy:%s(%d),priority=%d",
			msg, policy_str, policy, param->sched_priority);
}

void * _eppool_main(void *param)
{
	lc_server_t *sev = (lc_server_t *)param;
	struct sockaddr_in caddr;
	socklen_t clen = sizeof(caddr);

	lc_log_initthread(sev->server_name);
	ul_setsocktonoblock(sev->sev_sock);
	eppool_t *pool = (eppool_t *)sev->pool;
	int idx = eppool_add(pool, sev->sev_sock, (in_addr_t)0);
	if (idx < 0) {
		LC_LOG_WARNING("add accept socket err sock[%d][%m]", sev->sev_sock);
		goto out;
	}
	
	pool->_sockets[idx].status = BUSY;
	pool->_sev_sock_id = idx;

    if (sev->using_threadsche) {
		struct sched_param sp; 
		int policy;
		int ret = pthread_getschedparam(pthread_self(), &policy, &sp);
		if (ret) {
			LC_LOG_WARNING("retrieve sched info failed:%d", ret);
		} else {
			__print_sched_info("listener", policy, &sp);
		}
    }

	while (sev->run) {
		eppool_produce(sev, (struct sockaddr *)&caddr, &clen);
	}
	_eppool_close_pool_sockets((eppool_t *)sev->pool);

out:
    LC_LOG_DEBUG("epool work thread stop");
	lc_log_closethread();

	pthread_exit(NULL);
	return NULL;
}

int eppool_check_timeout(eppool_t *pool)
{
	time_t curtime = time(NULL);
	if (curtime < pool->_next_check_time) {
		return 0;
	}
	pool->_next_check_time = curtime + pool->_timeout;
	time_t tmp = 0;
	for (int i=0; i<(int)pool->_size; ++i) {
		switch (pool->_sockets[i].status) {
			case READY:
				tmp = pool->_sockets[i].last_active + pool->_timeout;
				if (curtime >= tmp) {
					in_addr addr;
					char ipbuf[32];
					addr.s_addr = pool->_sockets[i].ip;
					LC_LOG_WARNING("Connection timeout, socket[%d], ip[%s], timeout[%d]",
						pool->_sockets[i].sock,
						lc_inet_ntoa_r(addr, ipbuf, sizeof(ipbuf)),
						pool->_timeout);
					
					eppool_del(pool, i, 0);
       				pool->_using_size --;
				} else if (pool->_next_check_time > tmp) {
					pool->_next_check_time = tmp;
				}
				break;
			case BUSY:
				pool->_sockets[i].last_active = curtime;
				break;
		}
	}
	return 0;
}

int eppool_produce(lc_server_t *sev, struct sockaddr *addr, socklen_t *addrlen)
{
	int ret = 0;
	eppool_t *pool = (eppool_t *)sev->pool;
	int sev_sock = sev->sev_sock;

	eppool_check_timeout(pool);

	int num = epoll_wait(pool->_epfd, pool->_ep_events, pool->_size, pool->_check_interval);
	if (num <= 0) {
		return ret;
	}
	for (int i=0; i<num; ++i) {
		
		if (pool->_ep_events[i].data.fd == pool->_sev_sock_id) {
			int cli_sock = ul_accept(sev_sock, addr, addrlen);
			if (cli_sock < 0) {
				continue;
			}
            if (sev->monitor) {
                sev->monitor->add_request_connect_number(sev->server_name);
            }
			lc_setsockopt(cli_sock, sev);
			in_addr ipad = ((struct sockaddr_in *)addr)->sin_addr;
			char ipbuf[32];
			
            lc_auth_t *auth = lc_server_get_ip_auth(sev);

			if(auth == NULL || lc_auth_ip_int(auth, ipad.s_addr) != 0 ) {
			
				if(auth)
					lc_auth_delref(auth);

				if (eppool_add(pool, cli_sock, ipad.s_addr) < 0) {
				
					LC_LOG_WARNING("add sock[%d] into queue err, queue full, at ip[%s]", cli_sock, lc_inet_ntoa_r(ipad, ipbuf, sizeof(ipbuf)));
					close(cli_sock);
					ret = -1;
				}

			} else {
			
				lc_auth_delref(auth);
				LC_LOG_WARNING("invalid connect ip[%s]", lc_inet_ntoa_r(ipad, ipbuf, sizeof(ipbuf)));
				close(cli_sock);
				ret = -1;
				continue;
			}
		} else if (pool->_ep_events[i].data.fd >= 0) {
			int idx = pool->_ep_events[i].data.fd;
			in_addr ipad;
			char ipbuf[32];
			
			if (pool->_ep_events[i].events & EPOLLHUP) {
				ipad.s_addr = pool->_sockets[idx].ip;
				LC_LOG_WARNING("invalid sock[%d][%s] in epoll", pool->_sockets[idx].sock, lc_inet_ntoa_r(ipad, ipbuf, sizeof(ipbuf)));
				eppool_del(pool, idx, 0);
                pool->_using_size --;
			} else if (pool->_ep_events[i].events & EPOLLERR) {
				ipad.s_addr = pool->_sockets[idx].ip;
				LC_LOG_WARNING("invalid sock[%d][%s] in epoll", pool->_sockets[idx].sock, lc_inet_ntoa_r(ipad, ipbuf, sizeof(ipbuf)));
				eppool_del(pool, idx, 0);
                pool->_using_size --;
			} else if (pool->_ep_events[i].events & EPOLLIN) {
				eppool_epoll_del(pool, idx);	
				
				if (eppool_put(pool, idx) != 0) {
					ipad.s_addr = pool->_sockets[idx].ip;
					LC_LOG_WARNING("sock[%d][%s] can't input to queue, queue full", pool->_sockets[idx].sock, lc_inet_ntoa_r(ipad, ipbuf, sizeof(ipbuf)));
					eppool_del(pool, idx, 0);
                    pool->_using_size --;
				}
			} else {	
				eppool_del(pool, idx, 0);
                pool->_using_size --;
			}
		}
	}
	return ret;
}

int eppool_add(eppool_t *pool, int sock, in_addr_t ip)
{
	int idx = -1;
	for (int i=0; i<(int)pool->_size; ++i) {
		if (pool->_sockets[i].status == IDLE) {
			idx = i;
			break;
		}
	}
	if (idx < 0) {
		LC_LOG_WARNING("no socket buffer for new require[%d], buffersize[%d]", sock, pool->_size);
		return -1;
	}

	pool->_sockets[idx].status = READY;
	pool->_sockets[idx].last_active = time(NULL);
	pool->_sockets[idx].sock = sock;
	pool->_sockets[idx].ip = ip;
    pool->_using_size++;
	
	if (eppool_epoll_add(pool, idx) != 0) {
		LC_LOG_WARNING( "epoll_ctl add socket %d failed. %m", sock);
		return -1;
	}

	return idx;
}

int eppool_epoll_add(eppool_t *pool, int idx)
{
	struct epoll_event ev;
	if (pool->_epfd < 0) {
		LC_LOG_FATAL("invalid epool fd %d", pool->_epfd);
		return -1;
	}
	ev.data.fd = idx;
	ev.events = EPOLLIN | EPOLLHUP | EPOLLERR;
	int sock = pool->_sockets[idx].sock;

	if (epoll_ctl(pool->_epfd, EPOLL_CTL_ADD, sock, &ev) < 0) {
		LC_LOG_WARNING( "epoll_ctl add socket %d failed. %m", sock);
		return -1;
	}

	return 0;
}

int eppool_epoll_del(eppool_t *pool, int idx)
{
	struct epoll_event ev;
	if (pool->_epfd < 0) {
		LC_LOG_FATAL("invalid epool fd %d", pool->_epfd);
		return -1;
	}
	ev.data.fd = idx;
	ev.events = EPOLLIN | EPOLLHUP | EPOLLERR;
	int sock = pool->_sockets[idx].sock;

	if (epoll_ctl(pool->_epfd, EPOLL_CTL_DEL, sock, &ev) < 0) {
		LC_LOG_WARNING( "epoll_ctl add socket %d failed. %m", sock);
		return -1;
	}

	return 0;
}

int eppool_del(eppool_t *pool, int idx, int keep_alive)
{
	if (keep_alive == 0) {
		if (pool->_sockets[idx].sock >= 0) {
			close(pool->_sockets[idx].sock);
		}
		pool->_sockets[idx].sock = -1;
		pool->_sockets[idx].status = IDLE;
	} else {
		pool->_sockets[idx].last_active = time(NULL);
		pool->_sockets[idx].status = READY;
		eppool_epoll_add(pool, idx);
	}

	return 0;
}

int eppool_put(eppool_t *pool, int idx)
{
	pthread_mutex_lock(&pool->_ready_mutex);
	if (is_full_q(&pool->_queue)) {
		pthread_mutex_unlock(&pool->_ready_mutex);
		return -1;
	}
	push_q(&pool->_queue, idx);
	pool->_sockets[idx].status = BUSY;
	pthread_cond_signal(&pool->_ready_cond);
	pthread_mutex_unlock(&pool->_ready_mutex);

	return 0;
}

void *_eppool_workers(void *param)
{
	lc_server_pdata_t *data = (lc_server_pdata_t *)param;
	lc_server_t *sev = (lc_server_t *)data->parent;

	lc_log_initthread(sev->server_name);
	set_pthread_data(data);

    if (sev->using_threadsche) {
		int policy;
		struct sched_param sp;
		int ret = pthread_getschedparam(pthread_self(), &policy, &sp);
		if (ret) {
			LC_LOG_WARNING("retrieve sched info failed:%d", ret);
		} else {
			__print_sched_info("worker", policy, &sp);
		}
    }

	_LC_P_START;
	while (sev->run) {
		eppool_consume((eppool_t *)sev->pool, data);
	}
	_LC_P_END;

	lc_log_closethread();
	pthread_exit(NULL);
	return NULL;
}

int eppool_pthread_cond_timewait(eppool_t *pool)
{
	struct timeval now;
	struct timespec timeout;
	gettimeofday(&now, 0);
	timeout.tv_sec = now.tv_sec + 5;
	timeout.tv_nsec = now.tv_usec * 1000;
	pthread_cond_timedwait(&pool->_ready_cond, &pool->_ready_mutex, &timeout);
	return 0;
}

int eppool_get(eppool_t *pool, int *idx)
{
	pthread_mutex_lock(&pool->_ready_mutex);
	while (is_empty_q(&pool->_queue) && (*pool->_run)) {
		eppool_pthread_cond_timewait(pool);
	}
	if (*pool->_run == 0) {
		pthread_mutex_unlock(&pool->_ready_mutex);
		return -1;
	}
	pop_q(&pool->_queue, idx);
	pthread_mutex_unlock(&pool->_ready_mutex);

	return 0;
}

int eppool_consume(eppool_t *pool, lc_server_pdata_t *data)
{
	int idx;
	lc_server_t *sev = (lc_server_t *)data->parent;
	if (eppool_get(pool, &idx) != 0) {
		return 0;
	}
	data->fd = pool->_sockets[idx].sock;
	data->ip = pool->_sockets[idx].ip;
	int ret = sev->cb_native();
	if (ret == 0 && lc_get_session_connect_type_then_reset(sev) == LCSVR_LONG_CONNECT) {
		eppool_del(pool, idx, 1);
	} else {
		eppool_del(pool, idx, 0);
		data->sock_num_dec ++;
		data->fd = -1;
	}
	return 0;
}


