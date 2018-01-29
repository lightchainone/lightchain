

#ifndef _LC_QUEUE_H
#define _LC_QUEUE_H

#include <string.h>
#include <stdlib.h>


typedef enum _lc_queue_err_t {
	QUEUE_ERR_OK		= -0x00,
	QUEUE_ERR_UNKNOW	= -0x01,
	QUEUE_ERR_ARG		= -0x02,
	QUEUE_ERR_SMALL		= -0x03,
	QUEUE_ERR_LARGE		= -0x04,
	QUEUE_ERR_FULL		= -0x05,
	QUEUE_ERR_EMPTY		= -0x06,
	QUEUE_ERR_MEMORY	= -0x07,
	QUEUE_ERR_UNEXIST	= -0x08,
} lc_queue_err_t;



typedef struct _lc_queue_t {
	u_int	cur;	
	u_int	cap;	
	u_int	end;	
	u_int	top;	
	u_int	size;	
	char	*buf;	
} lc_queue_t;



extern int
lc_queue_init(lc_queue_t *lc_queue, u_int num, u_int size);



extern int
lc_queue_close(lc_queue_t *lc_queue);



extern int
lc_queue_empty(lc_queue_t *lc_queue);



extern int
lc_queue_full(lc_queue_t *lc_queue);



extern int
lc_queue_push(lc_queue_t *lc_queue, void *item);



extern int
lc_queue_pop(lc_queue_t *lc_queue, void *item);



extern int
lc_queue_top(lc_queue_t *lc_queue, void *item);



extern void*
lc_queue_get(lc_queue_t *lc_queue, u_int index);

#endif	
