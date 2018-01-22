#include "apool.h"
#include "pool.h"
#include "lc_server.h"
#include "lc_miscdef.h"
#include "lcserver_app.h"
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

int apool_set_callback(lc_server_t *sev, int (*io)(apool_t * pool, int idx, uint32_t events),
	int (*proc)(apool_t *pool, lc_server_pdata_t *data))
{
	if(io == NULL || proc ==NULL)
	{
		LC_LOG_FATAL("io_callback and proc_callback must be used\
			cooperatively");
		return -1;
	}
	sev->io_callback = io;
	sev->proc_callback = proc;
	return 0;
}

int apool_init(lc_server_t *sev)
{
	int qsiz = lc_server_get_cpool_queue_size(sev);
	int ssiz = lc_server_get_cpool_socket_size(sev);
	apool_t *pool = NULL;

	if (qsiz <=0 || ssiz<=0) {
		qsiz = DEFAULT_QUEUE_LEN;
		ssiz = DEFAULT_SOCK_NUM;
		lc_server_set_cpool_queue_size(sev, qsiz);
		lc_server_set_cpool_socket_size(sev, ssiz);
	}
	if(sev->cb_native != default_native)
	{
		LC_LOG_FATAL("native callback cannot be used with apool,\
			use apool_set_callback instead");		
		return -1;
	}
	if(sev->io_callback == NULL)
		apool_set_callback(sev, apool_native, apool_consume);
	if (NULL == sev->pool) {
		sev->pool = malloc(sizeof(apool_t));
		if (NULL == sev->pool) {
			LC_LOG_WARNING("malloc size[%d] err for eppool", (int)sizeof(apool_t));
			return -1;
		}
		memset(sev->pool, 0, sizeof(apool_t));
		pool = (apool_t *)sev->pool;
		int result;
		if((result = pthread_mutex_init(&pool->_ready_mutex, NULL)) < 0)
		{
			LC_LOG_WARNING("pthread_mutex_init error, code %d", result);
			apool_destroy(sev);
			return -1;
		}	
		if((result = pthread_cond_init(&pool->_ready_cond, NULL)) < 0 )
		{
			LC_LOG_WARNING("pthread_mutex_init error, code %d", result);
			apool_destroy(sev);
			return -1;
		}	
	}
	pool->read_small_buf_len = 128;
	pool->write_small_buf_len = 128;
	
	if (create_q(&pool->_queue, qsiz) != 0) {
		LC_LOG_WARNING("create queue err, at size[%d]", qsiz);
		return -1;
	}

	pool->_size = ssiz;

	
	pool->_sockets = (apool_socket *)malloc(sizeof(apool_socket)*pool->_size);
	if (NULL == pool->_sockets) {
		LC_LOG_WARNING("malloc socket item size[%d] err", (int)sizeof(apool_socket)*pool->_size);
		apool_destroy(sev);
		return -1;
	}
	memset(pool->_sockets, 0, sizeof(apool_socket) * pool->_size);
	for(unsigned int i = 0; i < pool->_size; i++)
	{
		apool_reset_socket(pool, i);
		pool->_sockets[i].sock = -1;
	}

	
	pool->_ep_events = (struct epoll_event *)malloc(sizeof(struct epoll_event) * pool->_size);
	if (NULL == pool->_ep_events) {
		LC_LOG_WARNING("malloc epoll events[%d] for eppool err[%m]", (int)sizeof(struct epoll_event) * pool->_size);
		apool_destroy(sev);
		return -1;
	}
	memset(pool->_ep_events, 0, pool->_size * sizeof(struct epoll_event));
	
	
	
	
	pool->_free_socket = (int *)malloc(sizeof(int) * pool->_size);
	if (NULL == pool->_free_socket) {
		LC_LOG_WARNING("malloc _free_socket size[%d] err", (int)sizeof(int)*pool->_size);
		apool_destroy(sev);
		return -1;
	}
	for(unsigned int i = 0; i < pool->_size; i++)
	{
		pool->_free_socket[i] = i;
	}
	pool->_top_free_socket = 0;
	
	
	pool->_epfd = epoll_create(pool->_size);
	if (pool->_epfd < 0) {
		LC_LOG_WARNING("creat epoll[%d] err[%m]", pool->_epfd);
		apool_destroy(sev);
		return -1;
	}

	pool->_timeout = lc_server_get_cpool_timeout(sev);
	if (pool->_timeout <= 0) {
		pool->_timeout = DEFAULT_TIMEOUT;
	}
	pool->lc_server = sev;
	pool->_check_interval = DEFAULT_CHECK_INTERVAL;
	pool->_run = &sev->run;

	LC_LOG_NOTICE("run eppool model");
	return 0;

}

long long apool_get_queuenum(lc_server_t *sev)
{
    if (sev == NULL) return -1;

    apool_t *pool = (apool_t *)sev->pool;
    if (pool == NULL) return -1;

    return (long long)pool->_queue.size;
}

long long apool_get_socknum(lc_server_t *sev)
{
    if (sev == NULL) return -1;

    apool_t *pool = (apool_t *)sev->pool;
    if (pool == NULL) return -1;

    return (long long)pool->_top_free_socket;
}

int apool_run(lc_server_t *sev)
{
	int i=0;
	apool_t *pool = (apool_t *)sev->pool;
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
        ret = pthread_create(&pool->_main, &thread_attr, _apool_main, sev);
        pthread_attr_destroy(&thread_attr);
    } else {
        ret = pthread_create(&pool->_main, NULL, _apool_main, sev);
    }

	if (ret != 0) {
		LC_LOG_WARNING("create thread for eppool_main err[%m]");
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
		    ret = pthread_create(&sev->data[i].pid, &thread_attr, _apool_workers, &sev->data[i]);
            pthread_attr_destroy(&thread_attr);
        } else {
		    ret = pthread_create(&sev->data[i].pid, NULL, _apool_workers, &sev->data[i]);
        }
		if (ret != 0) {
			LC_LOG_WARNING("create thread for eppool_workers err[%m] but has create pthreadnum[%d]", i+1);
			return -1;
		}
		++ sev->real_thread_num;
	}

	return 0;
}

int apool_join(lc_server_t *sev)
{
	apool_t *pool = (apool_t *)sev->pool;
    if (sev->real_thread_num >= 0) {
	    pthread_join(pool->_main, NULL);
	    for (int i=0; i<sev->real_thread_num; ++i) {
		    pthread_join(sev->data[i].pid, NULL);
	    }
    }
	return 0;
}

int apool_destroy(lc_server_t *sev)
{
	apool_t *pool = (apool_t *)sev->pool;
	if (NULL == pool) return 0;
	pthread_mutex_destroy(&pool->_ready_mutex);
	pthread_cond_destroy(&pool->_ready_cond);
















	if (pool->_sockets != NULL) {
		for(unsigned int i =0; i<pool->_size; i++){
			apool_reset_socket(pool, i);
			if (pool->_sockets[i].sock >= 0)
			{
				close(pool->_sockets[i].sock);
				pool->_sockets[i].sock = -1;
			}
		}
		free (pool->_sockets);
		pool->_sockets = NULL;
	}
	if (pool->_ep_events != NULL) {
		free (pool->_ep_events);
		pool->_ep_events = NULL;
	}
	if (pool->_free_socket){
		free (pool->_free_socket);
		pool->_free_socket = NULL;		
	}
	if (pool->_epfd > 0) {
		close(pool->_epfd);
	}
	destroy_q(&pool->_queue);
	free (sev->pool);
	sev->pool = NULL;
    LC_LOG_DEBUG("apool destroy");
	return 0;
}

void * _apool_main(void *param)
{
	lc_server_t *sev = (lc_server_t *)param;
	struct sockaddr_in caddr;
	socklen_t clen = sizeof(caddr);

	lc_log_initthread(sev->server_name);
	ul_setsocktonoblock(sev->sev_sock);
	apool_t *pool = (apool_t *)sev->pool;
	struct epoll_event * event = &pool->_server_ep_events;
	event->events = EPOLLIN | EPOLLHUP | EPOLLERR;
	event->data.u32 = (unsigned int)-1;
	if (epoll_ctl(pool->_epfd, EPOLL_CTL_ADD, sev->sev_sock, event) < 0) {
		LC_LOG_FATAL("add accept socket err sock[%d][%m]", sev->sev_sock);
		goto out;
	}
	while (sev->run) {
		apool_produce(sev, (struct sockaddr *)&caddr, &clen);
	}

out:
    LC_LOG_DEBUG("apool work thread stop");
	lc_log_closethread();
	pthread_exit(NULL);
	return NULL;
}

int apool_check_timeout(apool_t *pool)
{
	time_t curtime = time(NULL);
	if (curtime < pool->_next_check_time) {
		return 0;
	}
	pool->_next_check_time = curtime + pool->_timeout;
	time_t tmp = 0;
	for (int i = pool->_top_free_socket-1; i > 0; i--) {
		int idx = pool->_free_socket[i];
		switch (pool->_sockets[idx].state) {
			case IDLE:
			case PROCESSING:
			case ERROR:
				break;
			default:
				tmp = pool->_sockets[idx].last_active + pool->_timeout;
				if (curtime >= tmp) {
					apool_close_socket(pool, idx);

					in_addr addr;
					char ipbuf[32];
					addr.s_addr = pool->_sockets[idx].ip;
					LC_LOG_WARNING("Connection timeout, socket[%d], ip[%s], timeout[%d]", 
						pool->_sockets[idx].sock, 
						lc_inet_ntoa_r(addr, ipbuf, sizeof(ipbuf)),
						pool->_timeout);

					
				} else if (pool->_next_check_time > tmp) {
					pool->_next_check_time = tmp;
				}
				break;
		}
	}
	return 0;
}

int apool_produce(lc_server_t *sev, struct sockaddr *addr, socklen_t *addrlen)
{
	int ret = 0;
	apool_t *pool = (apool_t *)sev->pool;
	int sev_sock = sev->sev_sock;

	apool_check_timeout(pool);

	int num = epoll_wait(pool->_epfd, pool->_ep_events, pool->_size, pool->_check_interval);
	if (num < 0) {
		LC_LOG_WARNING("epoll error %m");
		return ret;
	}
	if (0 == num)
		return ret;
	for (int i=0; i<num; ++i) {
		
		if (pool->_ep_events[i].data.u32 == UINT_MAX)
		{
			int cli_sock = ul_accept(sev_sock, addr, addrlen);
			if (cli_sock < 0) {
				continue;
			}
            if (sev->monitor) {
                sev->monitor->add_request_connect_number(sev->server_name);
            }
			lc_setsockopt(cli_sock, sev);
			in_addr ipad = ((struct sockaddr_in *)addr)->sin_addr;

			
            lc_auth_t *auth = lc_server_get_ip_auth(sev);
			if(auth == NULL || lc_auth_ip_int(auth, ipad.s_addr) != 0) {
				if(auth)
					lc_auth_delref(auth);
				
				if (apool_add_socket(pool, cli_sock, ipad.s_addr) < 0) {
					LC_LOG_WARNING("add sock[%d] into queue err, queue full", cli_sock);
					close(cli_sock);
					ret = -1;
				}
			} else {
				lc_auth_delref(auth);
				char ipbuf[32];
				LC_LOG_WARNING("invalid connect ip[%s]", lc_inet_ntoa_r(ipad, ipbuf, sizeof(ipbuf)));
				close(cli_sock);
				ret = -1;
				continue;
			} 
		} 
		else
		{
			int idx = pool->_ep_events[i].data.u32;
			
			if (pool->_ep_events[i].events & EPOLLHUP)
			{
				LC_LOG_WARNING("sock[%d] EPOLLHUP in epoll, id %d", pool->_sockets[idx].sock, idx);
				apool_close_socket(pool, idx);
			}
			else if (pool->_ep_events[i].events & EPOLLERR)
			{
				LC_LOG_WARNING("sock[%d] EPOLLERR in epoll", pool->_sockets[idx].sock);
				apool_close_socket(pool, idx);
			}
			else if (pool->_ep_events[i].events & EPOLLIN || pool->_ep_events[i].events & EPOLLOUT)
			{
				sev->io_callback(pool, idx, pool->_ep_events[i].events);
			}
			else
			{
				apool_close_socket(pool, idx);
			}
		}
	}
	return ret;
}


uint apool_get_us(struct timeval *result)
{
	return result->tv_sec * 1000000 + result->tv_usec;
}

int apool_add_socket(apool_t *pool, int sock, in_addr_t ip)
{
	if(pool->_top_free_socket == (int)pool->_size)
	{
		LC_LOG_WARNING("no socket buffer for new require[%d], buffersize[%d]", sock, pool->_size);
		return -1;
	}
	int idx = pool->_free_socket[pool->_top_free_socket];
	
	pool->_sockets[idx].state = IDLE;
	pool->_sockets[idx].last_active = time(NULL);
	pool->_sockets[idx].sock = sock;
	pool->_sockets[idx].ip = ip;
	if (apool_epoll_add(pool, idx, EPOLLIN)) {
		LC_LOG_WARNING( "epoll_ctl add socket %d failed. %m", sock);
		return -1;
	}
	pool->_top_free_socket++;
	return idx;
}

int apool_epoll_add(apool_t *pool, int idx, uint32_t events)
{
	
	if (pool->_epfd < 0) {
		LC_LOG_FATAL("invalid epool fd %d", pool->_epfd);
		return -1;
	}
	struct epoll_event event;
	memset(&event, 0, sizeof(event));
	event.events = events | EPOLLHUP | EPOLLERR;
	event.data.u32 = idx;
	int sock = pool->_sockets[idx].sock;

	if (epoll_ctl(pool->_epfd, EPOLL_CTL_ADD, sock, &event) < 0) {
		LC_LOG_WARNING( "epoll_ctl add socket %d failed. %m", sock);
		return -1;
	}
	return 0;
}

int apool_epoll_mod(apool_t *pool, int idx, uint32_t events)
{
	
	if (pool->_epfd < 0) {
		LC_LOG_FATAL("invalid epool fd %d", pool->_epfd);
		return -1;
	}
	struct epoll_event event;
	memset(&event, 0, sizeof(event));
	event.events = events | EPOLLHUP | EPOLLERR;
	event.data.u32 = idx;
	int sock = pool->_sockets[idx].sock;

	if (epoll_ctl(pool->_epfd, EPOLL_CTL_MOD, sock, &event) < 0) {
		LC_LOG_WARNING( "epoll_ctl mod socket %d failed. %m", sock);
		return -1;
	}
	return 0;
}

int apool_epoll_del(apool_t *pool, int idx)
{
	
	if (pool->_epfd < 0) {
		LC_LOG_FATAL("invalid epool fd %d", pool->_epfd);
		return -1;
	}
	int sock = pool->_sockets[idx].sock;
	if (epoll_ctl(pool->_epfd, EPOLL_CTL_DEL, sock, NULL) < 0) {
		LC_LOG_WARNING( "epoll_ctl del socket %d id %d failed. %m", sock, idx);
		return -1;
	}

	return 0;
}
int apool_put_queue(apool_t *pool, int idx)
{
	pthread_mutex_lock(&pool->_ready_mutex);
	if (is_full_q(&pool->_queue)) {
		LC_LOG_WARNING("queue is full");
		pthread_mutex_unlock(&pool->_ready_mutex);
		return -1;
	}
	push_q(&pool->_queue, idx);
	pool->_sockets[idx].state = PROCESSING;
	pthread_cond_signal(&pool->_ready_cond);
	pthread_mutex_unlock(&pool->_ready_mutex);

	return 0;
}

void *_apool_workers(void *param)
{
	lc_server_pdata_t *data = (lc_server_pdata_t *)param;
	lc_server_t *sev = (lc_server_t *)data->parent;
	lc_log_initthread(sev->server_name);
	set_pthread_data(data);
	_LC_P_START;
	
	while (sev->run) {
		sev->proc_callback((apool_t *)sev->pool, data);
	}
	_LC_P_END;
	lc_log_closethread();
	pthread_exit(NULL);
	return NULL;
}

int apool_time_interval(struct timeval *result)
{
	timeval t;
	gettimeofday(&t, NULL);
	result->tv_sec = t.tv_sec - result->tv_sec;
	result->tv_usec = t.tv_usec - result->tv_usec;
	if(result->tv_usec < 0)    {
		result->tv_sec--;
		result->tv_usec += 1000000;
	}    
	return 0;
}

int apool_pthread_cond_timewait(apool_t *pool)
{
	struct timeval now;
	struct timespec timeout;
	gettimeofday(&now, 0);
	timeout.tv_sec = now.tv_sec + 5;
	timeout.tv_nsec = now.tv_usec * 1000;
	pthread_cond_timedwait(&pool->_ready_cond, &pool->_ready_mutex, &timeout);
	return 0;
}

int apool_get_queue(apool_t *pool, int *idx)
{
	pthread_mutex_lock(&pool->_ready_mutex);
	while (is_empty_q(&pool->_queue) && (*pool->_run)) {
		apool_pthread_cond_timewait(pool);
	}
	if (0 == *pool->_run) {
		pthread_mutex_unlock(&pool->_ready_mutex);
		return -1;
	}
	pop_q(&pool->_queue, idx);
	pthread_mutex_unlock(&pool->_ready_mutex);

	return 0;
}
int apool_reset_socket(apool_t *pool, int idx)
{
	struct apool_socket *sock = &pool->_sockets[idx];

	if(sock->read_buf != NULL && sock->read_buf != sock->read_small_buf)
	{
		apool_free(pool, sock->read_buf);
		
	}
	sock->read_buf = 0;
	if(sock->write_buf != NULL && sock->write_buf != sock->write_small_buf)
	{
		apool_free(pool, sock->write_buf);
	}
	sock->write_buf = 0;
	sock->state = IDLE;
	sock->read_buf_used = 0;
	sock->read_buf_len = 0;
	sock->write_buf_used = 0;
	sock->write_buf_len = 0;
	return 0;
}

int apool_close_socket(apool_t *pool, int idx)
{

	apool_epoll_del(pool, idx);
	apool_reset_socket(pool, idx);
	if (pool->_sockets[idx].sock >= 0)
	{
		close(pool->_sockets[idx].sock);
		pool->_sockets[idx].sock = -1;
	}
	pool->_top_free_socket--;
	pool->_free_socket[pool->_top_free_socket] = idx;
	return 0;
}

char * apool_get_read_buf(apool_t *pool, apool_socket* sock, size_t size)
{
	if(sock->read_buf == NULL)
	{
		if(size > pool->read_small_buf_len)
		{
			sock->read_buf = (char *)apool_malloc(pool, size);
		}
		else
		{
			sock->read_buf = sock->write_small_buf;	

		}
		if(sock->read_buf)
			sock->read_buf_len = size;
	}
	else
	{
		LC_LOG_WARNING( "request read buffer twice, size %zu socket %d", size, sock->sock);
	}
	return sock->write_buf;
}

char * apool_get_write_buf(apool_t *pool, apool_socket* sock, size_t size)
{
	if(sock->write_buf == NULL)
	{
	
		if(size > pool->write_small_buf_len)
		{
			sock->write_buf = (char *)apool_malloc(pool, size);
		}
		else
		{
			sock->write_buf = sock->write_small_buf;
		}
		if(sock->write_buf)
			sock->write_buf_len = size;
	}
	else
	{
		LC_LOG_WARNING( "request write buffer twice, size %zu, socket %d", size, sock->sock );
	}
	return sock->write_buf;
}


void * apool_malloc(apool_t *pool, size_t size)
{
	void * result = malloc(size);
	if(NULL == result)
		LC_LOG_WARNING("Malloc memory for content failed");
	
	return result;
}

void apool_free(apool_t *pool, char * buf)
{
	free(buf);
}

int apool_noblock_write_buff(int sock, char *buff, int *cur_size, int size)
{
	int ret;
	if (size < *cur_size) 
		return -1;
	if (size == *cur_size) {
		return 0;
	}
	while (1) {
		ret = send(sock, buff+(*cur_size), size-(*cur_size), MSG_DONTWAIT);
		if (-1 == ret && (EINTR == errno))
			continue;
		break;
	}
	if (ret <= 0) {
		if (EAGAIN == errno) return 1; 
		LC_LOG_WARNING("noblock write buff error [%m]");
		return -1;
	}
	*cur_size += ret;
	if (*cur_size == size) {
		return 0;
	}
	return 1;
}

int apool_noblock_read_buff(int sock, char *buff, int *cur_size, const int size)
{
	int ret;
	if (size < *cur_size) {
		return -1;
	}
	if (size == *cur_size) {
		return 0;
	}
	while (1) {
		ret = recv(sock, buff+(*cur_size), size-(*cur_size), MSG_DONTWAIT);
		if (-1 == ret && (EINTR == errno)) {
			continue;
		}
		break;
	}

	if (ret <= 0) {
		if (EAGAIN == errno) return 1;
		return -1;
	}
	*cur_size += ret;
	if (*cur_size == size) {
		return 0;
	}
	return 1;
}



