#ifndef LC_UTIL_H
#define LC_UTIL_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

typedef struct _queue_t
{
	int capacity;
	int size;
	int front;
	int rear;
	int *array;
} queue_t;
int is_empty_q(queue_t *q);
int is_full_q(queue_t *q);
int empty_q(queue_t *q);
int create_q(queue_t *q, int qcap);
int push_q(queue_t *q, int val);
int pop_q(queue_t *q, int *val);
int destroy_q(queue_t *q);


char *lc_inet_ntoa_r(struct in_addr inaddr, char *buf, u_int len);

#ifdef LCDEBUG
#define LC_DEBUG(fmt, arg...) LC_LOG_DEBUG(fmt, ##arg)
#else
#define LC_DEBUG(fmt, arg...) 
#endif

#endif
