#ifndef  __APOOL_H_
#define  __APOOL_H_

#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>

#ifdef USE_SYSEPOLL
#include <sys/epoll.h>
#else
#include "epoll.h"
#endif
#include "nshead.h"
#include "lcserver_core.h"
#include "lc_util.h"
struct apool_t
{
	struct epoll_event *_ep_events;
	int * _free_socket;
	int _top_free_socket;
	struct apool_socket* _sockets;
	u_int _size;
	int _epfd;
	epoll_event _server_ep_events;
	u_int _timeout;	
	u_int _check_interval;	
	time_t _next_check_time;
	int *_run;
	int _sev_sock_id;
	queue_t _queue;
	pthread_t _main;
	pthread_mutex_t _ready_mutex;
	pthread_cond_t _ready_cond;
	lc_server_t * lc_server;
	size_t read_small_buf_len;	
	size_t write_small_buf_len;
};

struct apool_socket
{
	int sock;  
	in_addr_t ip;  
	int last_active;   
	int state;       
	char read_small_buf[128];   
	char write_small_buf[128];   
	char * read_buf;    
	char * write_buf;   
	unsigned int read_buf_len;   
	unsigned int read_buf_used;   
	unsigned int write_buf_len;  
	unsigned int write_buf_used;  
	timeval  total_time;
	timeval  read_time;
	timeval  process_time;
	
	int self_type;
};

enum {
	IDLE = 0,
	PROCESSING, 
	ERROR
};


int apool_init(lc_server_t *);

int apool_run(lc_server_t *);

int apool_join(lc_server_t *);

int apool_destroy(lc_server_t *);

void * _apool_main(void *param);

void *_apool_workers(void *param);


int apool_produce(lc_server_t *sev, struct sockaddr *addr, socklen_t *addrlen);


int apool_consume(apool_t *pool, lc_server_pdata_t *data);


int apool_check_timeout(apool_t *pool);

int apool_add_socket(apool_t *pool, int sock, in_addr_t ip);


int apool_reset_socket(apool_t *pool, int idx);


int apool_close_socket(apool_t *pool, int idx);

int apool_epoll_mod(apool_t *pool, int idx, uint32_t events);


int apool_epoll_add(apool_t *pool, int idx, uint32_t events);


int apool_epoll_del(apool_t *pool, int idx);

int apool_native(apool_t * pool, int idx, uint32_t events);

int apool_pthread_cond_timewait(apool_t *pool);


int apool_time_interval(struct timeval *result);

uint apool_get_us(struct timeval *result);



int apool_put_queue(apool_t *pool, int idx);

int apool_get_queue(apool_t *pool, int* idx);


void *apool_malloc(apool_t *pool, size_t size);

void apool_free(apool_t *pool, char * buf);


int apool_write(apool_t *pool, int idx);

int apool_read_head(apool_t *pool, int idx);

int apool_read_content(apool_t *pool, int idx);


char * apool_get_read_buf(apool_t *pool, apool_socket* socket, size_t size);

char * apool_get_write_buf(apool_t *pool, apool_socket* socket, size_t size);


int apool_noblock_write_buff(int socket, char *buff, int *cur_size, int size);

int apool_noblock_read_buff(int socket, char *buff, int *cur_size, const int size);


long long apool_get_socknum(lc_server_t *sev);

long long apool_get_queuenum(lc_server_t *sev);
#endif
