#include <stdio.h>
#include <stdlib.h>

#include "lc_util.h"


int is_empty_q(queue_t *q)
{
	return q->size == 0;
}

int is_full_q(queue_t *q)
{
	return q->size == q->capacity;
}

int empty_q(queue_t *q)
{
	q->size = 0;
	q->front = 0;
	q->rear = 1;
	return 0;
}

int create_q(queue_t *q, int qcap)
{
	++ qcap;
	if (qcap < 2) return -1;

	q->capacity = qcap;
	empty_q(q);
	q->array = (int *)malloc(sizeof(int) * qcap);
	if (q->array == NULL) {
		return -1;
	}
	return 0;
}

int push_q(queue_t *q, int val)
{
	if (q->size == q->capacity) return -1;
	q->array[q->rear] = val;
	++ q->size;
	if (++ q->rear >= q->capacity) q->rear = 0;
	return 0;
}

int pop_q(queue_t *q, int *val)
{
	if (q->size == 0) return -1;
	++ q->front;
	if (q->front >= q->capacity) q->front = 0;
	*val = q->array[q->front];
	-- q->size;
	return 0;
}

int destroy_q(queue_t *q)
{
	q->capacity = 0;
	free(q->array);
	return 0;
}

char * lc_inet_ntoa_r(struct in_addr inaddr, char *buf, u_int len)
{
	unsigned char *ip = (unsigned char *)&inaddr.s_addr;
	snprintf(buf, len, "%u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);
	return buf;
}

