#include <lc_log.h>
#include <ul_log.h>
#include <ul_net.h>

#include "cpool.h"
#include "pool.h"
#include "lc_util.h"
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

int cpool_init(lc_server_t *sev)
{
	int qsiz = lc_server_get_cpool_queue_size(sev);
	int ssiz = lc_server_get_cpool_socket_size(sev);
	cpool_t *pool = NULL;

	if (qsiz <=0 || ssiz<=0) {
		qsiz = DEFAULT_QUEUE_LEN;
		ssiz = DEFAULT_SOCK_NUM;
		lc_server_set_cpool_queue_size(sev, qsiz);
		lc_server_set_cpool_socket_size(sev, ssiz);
	}

	if (sev->pool == NULL) {
		sev->pool = malloc(sizeof(cpool_t));
		if (sev->pool == NULL) {
			LC_LOG_WARNING("malloc size[%d] err for cpool", (int)sizeof(cpool_t));
			return -1;
		}
		memset(sev->pool, 0, sizeof(cpool_t));
	}
	pool = (cpool_t *)sev->pool;
	pthread_mutex_init(&pool->_ready_mutex, NULL);
	pthread_cond_init(&pool->_ready_cond, NULL);
	if (create_q(&pool->_queue, qsiz) != 0) {
		LC_LOG_WARNING("create queue err, at size[%d]", qsiz);
		return -1;
	}
	pool->_size = ssiz;
	pool->_sockets = (socket_item *)malloc(sizeof(socket_item)*pool->_size);
	if (pool->_sockets == NULL) {
		LC_LOG_WARNING("malloc socket item size[%d] err", (int)sizeof(socket_item)*pool->_size);
		return -1;
	}
	memset(pool->_sockets, 0, sizeof(socket_item)*pool->_size);

	pool->_timeout = lc_server_get_cpool_timeout(sev);
	if (pool->_timeout <= 0) {
		pool->_timeout = DEFAULT_TIMEOUT;
	}
	pool->_check_interval.tv_sec = DEFAULT_CHECK_INTERVAL/1000;
	pool->_check_interval.tv_usec = (DEFAULT_CHECK_INTERVAL%1000) * 1000;
	pool->_run = &sev->run;
    pool->_using_size = 0;
	LC_LOG_NOTICE("run cpool model");
	return 0;
}

int cpool_run(lc_server_t *sev)
{
	int i=0;
	cpool_t *pool = (cpool_t *)sev->pool;

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
        ret = pthread_create(&pool->_main, &thread_attr, _cpool_main, sev);
        pthread_attr_destroy(&thread_attr);
    } else {
        ret = pthread_create(&pool->_main, NULL, _cpool_main, sev);
    }

	if (ret != 0) {
		LC_LOG_WARNING("create thread for cpool_main err[%m]");
		return -1;
	}

	sev->real_thread_num = 0;
	for (i=0; i<sev->pthread_num; ++i) {
		sev->data[i].id = i;

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
		    ret = pthread_create(&sev->data[i].pid, &thread_attr, _cpool_workers, &sev->data[i]);
            pthread_attr_destroy(&thread_attr);
        } else {
		    ret = pthread_create(&sev->data[i].pid, NULL, _cpool_workers, &sev->data[i]);
        }
		if (ret != 0) {
			LC_LOG_WARNING("create thread for cpool_workers err[%m] but has create pthreadnum[%d]", i+1);
			return -1;
		}
		++ sev->real_thread_num;
	}
	return 0;
}

long long cpool_get_socknum(lc_server_t *sev)
{
    if (sev == NULL) return -1;
    cpool_t *pool = (cpool_t *)sev->pool;
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

long long cpool_get_queuenum(lc_server_t *sev)
{
    if (sev == NULL) return -1;
    cpool_t *pool = (cpool_t *)sev->pool;
    if (pool == NULL) return -1;
    return (long long)pool->_queue.size;
}

int cpool_join(lc_server_t *sev)
{
	int i=0;
	if (sev == NULL) return -1;

	cpool_t *pool = (cpool_t *)sev->pool;
	if (pool == NULL) return -1;

    if (sev->real_thread_num >= 0) {
  	    pthread_join(pool->_main, NULL);
	    for (i=0; i<sev->real_thread_num; ++i) {
		    pthread_join(sev->data[i].pid, NULL);
	    }
    }
	return 0;
}
int cpool_destroy(lc_server_t *sev)
{
	cpool_t *pool = (cpool_t *)sev->pool;
	if (pool == NULL) return 0;

	pthread_mutex_destroy(&pool->_ready_mutex);
	pthread_cond_destroy(&pool->_ready_cond);
	destroy_q(&pool->_queue);
	if (pool->_sockets != NULL)
		free (pool->_sockets);
	free (sev->pool);
	sev->pool = NULL;
    LC_LOG_DEBUG("cpool destroy");
	return 0;
}

static void _cpool_close_pool_sockets(cpool_t *pool)
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

void *_cpool_main(void *param)
{
	lc_server_t *sev = (lc_server_t *)param;
	struct sockaddr_in caddr;
	socklen_t clen = sizeof(caddr);
	
	lc_log_initthread(sev->server_name);

	ul_setsocktonoblock(sev->sev_sock);
	while (sev->run) {
		cpool_produce(sev, (struct sockaddr *)&caddr, &clen);
	}

	cpool_t *pool = (cpool_t *)sev->pool;
	_cpool_close_pool_sockets(pool);

    LC_LOG_DEBUG("cpool work thread stop");
	lc_log_closethread();
	pthread_exit(NULL);
	return NULL;
}

int cpool_produce(lc_server_t *sev, struct sockaddr *addr, socklen_t *addrlen)
{
	cpool_t *pool = (cpool_t *)sev->pool;
	int sev_sock = sev->sev_sock;

	FD_ZERO(&pool->_rfs);
	FD_SET(sev_sock, &pool->_rfs);
	int maxsock = 0;
	for (int i=0; i<(int)pool->_size; ++i) {
		if (pool->_sockets[i].status == READY) {
			FD_SET(pool->_sockets[i].sock, &pool->_rfs);
			if (pool->_sockets[i].sock > maxsock)
				maxsock = pool->_sockets[i].sock;
		}
	}
	struct timeval timeout = pool->_check_interval;
	int ret = select(_MAX(maxsock, sev_sock)+1, &pool->_rfs, 0, 0, &timeout);
	if (ret <=0) {
		return ret;
	}
	if (FD_ISSET(sev_sock, &pool->_rfs)) {
		int cli_sock = ul_accept(sev_sock, addr, addrlen);
        if (sev->monitor) {
            sev->monitor->add_request_connect_number(sev->server_name);
        }
		if (cli_sock >= 0) {
			lc_setsockopt(cli_sock, sev);
			in_addr ipad = ((struct sockaddr_in *)addr)->sin_addr;
			char ipbuf[32];
			
            lc_auth_t *auth = lc_server_get_ip_auth(sev);

			if(auth == NULL || lc_auth_ip_int(auth, ipad.s_addr) != 0) {
			
				if(auth)
					lc_auth_delref(auth);

				if (cpool_add(pool, cli_sock, ipad.s_addr) == 0) {
					FD_SET(cli_sock, &pool->_rfs);
				} else {
					ret = -1;
					LC_LOG_WARNING("close socke[%d] at ip[%s] because of full queue", cli_sock, lc_inet_ntoa_r(ipad, ipbuf, sizeof(ipbuf)));
					close(cli_sock);
				}

			} else {
			
				lc_auth_delref(auth);
				LC_LOG_WARNING("invalid ip[%s]", lc_inet_ntoa_r(ipad, ipbuf, sizeof(ipbuf)));
				close(cli_sock);
				ret = 0;
			} 
		} else {
			ret = -1;
		}
	}
	cpool_check(pool);
	return ret;
}

int cpool_add(cpool_t *pool, int sock, in_addr_t ip)
{
	struct timeval tv;
	gettimeofday(&tv, 0);

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
	pool->_sockets[idx].sock = sock;
	pool->_sockets[idx].last_active = tv.tv_sec;
	pool->_sockets[idx].status = READY;
	pool->_sockets[idx].ip = ip;
    pool->_using_size ++;

	return 0;
}

void cpool_check(cpool_t *pool)
{
	struct timeval tv;
	char ipbuf[32];
	in_addr addr;

	gettimeofday(&tv, 0);
	int sec = tv.tv_sec;

	for (int i=0; i<(int)pool->_size; ++i) {
		switch(pool->_sockets[i].status) {
			case IDLE:
				break;
			case READY:
				if (FD_ISSET(pool->_sockets[i].sock, &pool->_rfs)) {
					if (cpool_put(pool, i) != 0) {
						addr.s_addr = pool->_sockets[i].ip;
						LC_LOG_WARNING("pool full, so skip the socket[%d] at ip[%s]", pool->_sockets[i].sock,
								lc_inet_ntoa_r(addr, ipbuf, sizeof(ipbuf)));
						cpool_del(pool, i, 0);
                        pool->_using_size --;
					}
				} else if (sec - pool->_sockets[i].last_active > pool->_timeout) {
					addr.s_addr = pool->_sockets[i].ip;
					LC_LOG_WARNING("Connection timeout, socket[%d], ip[%s], timeout[%d]", 
						pool->_sockets[i].sock,
						lc_inet_ntoa_r(addr, ipbuf, sizeof(ipbuf)), 
						pool->_timeout);
					
					cpool_del(pool, i, 0);
                    pool->_using_size --;
				}
				break;
			case BUSY:
				pool->_sockets[i].last_active = sec;
				break;
			default:
				break;
		}
	}
}

int cpool_put(cpool_t *pool, int handle)
{
	pthread_mutex_lock(&pool->_ready_mutex);
	if (is_full_q(&pool->_queue)) {
		pthread_mutex_unlock(&pool->_ready_mutex);
		return -1;
	}
	push_q(&pool->_queue, handle);
	pool->_sockets[handle].status = BUSY;
	pthread_cond_signal(&pool->_ready_cond);
	pthread_mutex_unlock(&pool->_ready_mutex);

	return 0;
}

void cpool_del(cpool_t *pool, int handle, int keep_alive)
{
	if (keep_alive == 0) {
		if (pool->_sockets[handle].sock >= 0) {
			close(pool->_sockets[handle].sock);
			pool->_sockets[handle].sock = -1;
			pool->_sockets[handle].status = IDLE;
		}
	} else {
		if (pool->_sockets[handle].status == BUSY) {
			pool->_sockets[handle].status = READY;
			pool->_sockets[handle].last_active = time(0);
		}
	}
}

void *_cpool_workers(void *param)
{
	lc_server_pdata_t *data = (lc_server_pdata_t *)param;
	lc_server_t *sev = (lc_server_t *)data->parent;

	lc_log_initthread(sev->server_name);
	set_pthread_data(data);

	_LC_P_START;

	while (sev->run) {
		cpool_consume((cpool_t *)sev->pool, data);
	}

	_LC_P_END;

	lc_log_closethread();
	pthread_exit(NULL);
	return NULL;
}

void cpool_consume(cpool_t *pool, lc_server_pdata_t *data)
{
	int handle, ret;
	lc_server_t *sev = (lc_server_t *)data->parent;

	if (cpool_get(pool, &handle, &data->fd) != 0)
		return;
	data->ip = pool->_sockets[handle].ip;
	
	ret = sev->cb_native();
	if (ret == 0 && lc_get_session_connect_type_then_reset(sev) == LCSVR_LONG_CONNECT) {
		cpool_del(pool, handle, 1);
	} else {
		cpool_del(pool, handle, 0);
        data->sock_num_dec ++;
		data->fd = -1;
	}
}

int cpool_get(cpool_t *pool, int *handle, int *sock)
{
	pthread_mutex_lock(&pool->_ready_mutex);
	while (is_empty_q(&pool->_queue) && *pool->_run) {
		struct timeval now;
		struct timespec timeout;
		gettimeofday(&now, 0);
		timeout.tv_sec = now.tv_sec + 5;
		timeout.tv_nsec = now.tv_usec * 1000;
		pthread_cond_timedwait(&pool->_ready_cond, &pool->_ready_mutex, &timeout);
	}
	if (*pool->_run == 0) {
		pthread_mutex_unlock(&pool->_ready_mutex);
		return -1;
	}
	pop_q(&pool->_queue, handle);
	*sock = pool->_sockets[*handle].sock;
	pthread_mutex_unlock(&pool->_ready_mutex);

	return 0;
}


