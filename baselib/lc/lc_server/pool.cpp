#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <assert.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "pool.h"
#include "apool.h"

static pthread_key_t g_pkey;
static pthread_once_t g_ponce = PTHREAD_ONCE_INIT;

static void create_key_once(void)
{
	pthread_key_create(&g_pkey, NULL);
}

int set_pthread_data(lc_server_pdata_t *data)
{
	void *ptr = NULL;
	pthread_once(&g_ponce, create_key_once);
	if ((ptr=pthread_getspecific(g_pkey)) == NULL) {
		ptr = data;
		pthread_setspecific(g_pkey, ptr);
	}
	return 0;
}

lc_server_pdata_t * get_pdata()
{
	void * ptr = pthread_getspecific(g_pkey);
	return (lc_server_pdata_t *)ptr;
}

void * lc_server_get_user_data()
{
	lc_server_pdata_t *ptr = get_pdata();
	if (ptr == NULL) return NULL;
	return ptr->user_buf;
}

u_int lc_server_get_userdata_size()
{
	lc_server_pdata_t *ptr = get_pdata();
	if (ptr == NULL) return 0;
	return ptr->user_siz;
}

void * lc_server_get_read_buf()
{
	lc_server_pdata_t *ptr = get_pdata();
	if (ptr == NULL) return NULL;
	if(((lc_server_t *)(ptr->parent))->server_type == 3)
	{
		apool_socket* sock = ptr->apoolsocket;
		return sock->read_buf;
	}
	else
	{
		return ptr->read_buf;
	}
}

u_int lc_server_get_read_size()
{
	lc_server_pdata_t *ptr = get_pdata();
	if (ptr == NULL) return 0;
	if(((lc_server_t *)(ptr->parent))->server_type == 3)
	{
		apool_socket* sock = ptr->apoolsocket;
		return sock->read_buf_len;
	}
	else
	{
		return ptr->read_siz;
	}
}

void * lc_server_get_write_buf(size_t size)
{
	lc_server_pdata_t *ptr = get_pdata();
	if (ptr == NULL) return NULL;
	if(((lc_server_t *)(ptr->parent))->server_type == 3)
	{
		apool_t *pool = (apool_t *)(((lc_server_t *)(ptr->parent))->pool);
		apool_socket* sock = ptr->apoolsocket;
		if(sock->write_buf_len != 0)
		{
			if(size != 0 || size != sock->write_buf_len)
			{
				LC_LOG_WARNING( "request write buffer twice, size %zu", size);
			}
			return sock->write_buf;
		}
		else
		{
			if(0 == size)
			{
				return apool_get_write_buf(pool, sock, ptr->write_siz);
			}
			else
			{
				return apool_get_write_buf(pool, sock, size);
			}
		}
	}
	else
	{
		return ptr->write_buf;
	}
}

u_int lc_server_get_write_size()
{
	lc_server_pdata_t *ptr = get_pdata();
	if (ptr == NULL) return 0;
	if(((lc_server_t *)(ptr->parent))->server_type == 3)
	{
		apool_t *pool = (apool_t *)(((lc_server_t *)(ptr->parent))->pool);
		apool_socket* sock = ptr->apoolsocket;
		if(0 == sock->write_buf_len)
		{
			apool_get_write_buf(pool, sock, ptr->write_siz);
			return sock->write_buf_len;
		}
		else
		{
			return sock->write_buf_len;
		}
	}
	else{
		return ptr->write_siz;
	}
}

lc_server_t * get_server()
{
	lc_server_pdata_t *ptr = get_pdata();
	if (ptr == NULL) return 0;
	return (lc_server_t *)(ptr->parent);
}
lc_server_t * lc_server_get_server()
{
	return get_server();
}

int lc_server_get_connect_timeout()
{
	lc_server_pdata_t *ptr = get_pdata();
	if (ptr == NULL) return 0;
	return ((lc_server_t *)(ptr->parent))->connect_to;
}
int lc_server_get_read_timeout()
{
	lc_server_pdata_t *ptr = get_pdata();
	if (ptr == NULL) return 0;
	return ((lc_server_t *)(ptr->parent))->read_to;
}
int lc_server_get_write_timeout()
{
	lc_server_pdata_t *ptr = get_pdata();
	if (ptr == NULL) return 0;
	return ((lc_server_t *)(ptr->parent))->write_to;
}

int lc_server_get_socket()
{
	lc_server_pdata_t *ptr = get_pdata();
	if (ptr == NULL) return -1;
	return ptr->fd;
}

int lc_setsockopt(int fd, lc_server_t *sev)
{
	if (sev->socktype & LCSVR_NODELAY) {
		int on = 1;
		setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on));
	}
	if (sev->socktype & LCSVR_LINGER_CLOSE) {
		struct linger li;
		memset(&li, 0, sizeof(li));
		li.l_onoff = 1;
		li.l_linger = 1;
		setsockopt(fd, SOL_SOCKET, SO_LINGER, (const char*)&li, sizeof(li) );
	}
	return 0;
}

int lc_setsockopt_ex(int fd, lc_server_t *sev)
{
    if (sev->socktype & LCSVR_DEFER_ACCEPT) {
        int timeout = sev->connect_to;
        setsockopt(fd, IPPROTO_TCP, TCP_DEFER_ACCEPT, &timeout, sizeof(timeout));
    }
	return 0;
}

in_addr_t lc_server_get_ip()
{
	lc_server_pdata_t *ptr = get_pdata();
	if (ptr == NULL) return (in_addr_t)(-1);
	return ptr->ip;
}

enum {
	QUELE_LEN_PARAM,
	SOCKET_SIZE_PARAM,	
	TIMEOUT_PARAM	
};

int lc_server_set_cpool_queue_size(lc_server_t *sev, int siz)
{
	sev->pool_data[QUELE_LEN_PARAM] = siz;
	return 0;
}

int lc_server_get_cpool_queue_size(lc_server_t *sev)
{
	return sev->pool_data[QUELE_LEN_PARAM];
}

int lc_server_set_cpool_socket_size(lc_server_t *sev, int siz)
{
	sev->pool_data[SOCKET_SIZE_PARAM] = siz;
	return 0;
}

int lc_server_get_cpool_socket_size(lc_server_t *sev)
{
	return sev->pool_data[SOCKET_SIZE_PARAM];
}

int lc_server_set_cpool_timeout(lc_server_t *sev, int timeout)
{
	sev->pool_data[TIMEOUT_PARAM] = timeout;
	return 0;
}

int lc_server_get_cpool_timeout(lc_server_t *sev)
{
	return sev->pool_data[TIMEOUT_PARAM];
}

static const int ___connect__base = 12345;
void lc_server_set_session_connect_type(int cotp)
{
	lc_server_pdata_t *ptr = get_pdata();
	if (ptr == NULL) return ;
	if(((lc_server_t *)(ptr->parent))->server_type == 3)
	{
		apool_socket* sock = ptr->apoolsocket;
		if (sock) {
			sock->self_type = cotp + ___connect__base;
		}
	}
	else
	{
		ptr->self_type = cotp + ___connect__base;
	}
}

int lc_server_get_session_connect_type(lc_server_t *sev)
{
	lc_server_pdata_t *ptr = get_pdata();
	if (ptr == NULL) return -1;
	int selftype = ptr->self_type - ___connect__base;
	if(((lc_server_t *)(ptr->parent))->server_type == 3)
	{
		apool_socket* sock = ptr->apoolsocket;
		if (sock) {
			selftype = sock->self_type - ___connect__base;
		} else {
			return -1;
		}
	}
	switch (selftype) {
		case LCSVR_SHORT_CONNECT:
			return LCSVR_SHORT_CONNECT;
		case LCSVR_LONG_CONNECT:
			return LCSVR_LONG_CONNECT;
		default:
			return sev->connect_type;
	}
}

int lc_get_session_connect_type_then_reset(lc_server_t *sev)
{
	int ret = lc_server_get_session_connect_type(sev);
	lc_server_set_session_connect_type(sev->connect_type);
	return ret;
}

struct iovec ** lc_server_get_write_iovec()
{
	lc_server_pdata_t *ptr = get_pdata();
	if (ptr == NULL) return NULL;
    return &ptr->iov;
}

size_t *lc_server_get_write_iovec_count()
{
	lc_server_pdata_t *ptr = get_pdata();
	if (ptr == NULL) return 0;
    return &ptr->iov_count;
}


