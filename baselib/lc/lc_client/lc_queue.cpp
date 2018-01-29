

#include "lc_queue.h"

#define min(x, y)	((x) > (y) ? (y) : (x))

int
lc_queue_init(lc_queue_t *lc_queue, u_int num, u_int size)
{
	if (NULL == lc_queue || 0 == num || 0 == size) {
		return QUEUE_ERR_ARG;
	}

	lc_queue->buf	= (char *)calloc(num, size);
	if (NULL == lc_queue->buf) {
		return QUEUE_ERR_MEMORY;
	}

	lc_queue->cur	= 0;
	lc_queue->cap	= num;
	lc_queue->size	= size;
	lc_queue->end   = 0;
	lc_queue->top   = 0;

	return QUEUE_ERR_OK;
}

int
lc_queue_close(lc_queue_t *lc_queue)
{
	if (NULL == lc_queue) {
		return QUEUE_ERR_ARG;
	}

	if (NULL == lc_queue->buf) {
		return QUEUE_ERR_MEMORY;
	}

	free(lc_queue->buf);
	lc_queue->buf = NULL;

	return QUEUE_ERR_OK;
}


int
lc_queue_empty(lc_queue_t *lc_queue)
{
	return (NULL == lc_queue || 0 == lc_queue->cur) ? 1 : 0;
}


int
lc_queue_full(lc_queue_t *lc_queue)
{
	return (NULL != lc_queue && lc_queue->cur >= lc_queue->cap) ? 1 : 0;
}


int
lc_queue_push(lc_queue_t *lc_queue, void *item)
{
	if (NULL == lc_queue || NULL == item) {
		return QUEUE_ERR_ARG;
	}

	if (1 == lc_queue_full(lc_queue)) {
		return QUEUE_ERR_FULL;
	}

	memmove(lc_queue->buf + lc_queue->size * lc_queue->top, item, lc_queue->size);
	++lc_queue->cur;
	lc_queue->top = (lc_queue->top + 1)%lc_queue->cap;

	return QUEUE_ERR_OK;
}

int
lc_queue_pop(lc_queue_t *lc_queue, void *item)
{
	void* tmp;

	if (NULL == lc_queue || NULL == item) {
		return QUEUE_ERR_ARG;
	}

	if(lc_queue_empty(lc_queue)){
		return QUEUE_ERR_EMPTY;
	}

	
	if (NULL == (tmp = lc_queue_get(lc_queue, 0))) {
		return QUEUE_ERR_EMPTY;
	}

	memmove(item, tmp, lc_queue->size);

	--lc_queue->cur;
	lc_queue->end = (lc_queue->end + 1)%lc_queue->cap;

	return QUEUE_ERR_OK;
}

int
lc_queue_top(lc_queue_t *lc_queue, void *item)
{
	void *tmp;

	if(lc_queue_empty(lc_queue)){
		return QUEUE_ERR_EMPTY;
	}

	if (NULL == (tmp = lc_queue_get(lc_queue, lc_queue->cur - 1))) {
		return QUEUE_ERR_EMPTY;
	}

	memmove(item, tmp, lc_queue->size);

	return QUEUE_ERR_OK;
}

	void*
lc_queue_get(lc_queue_t *lc_queue, u_int indexi)
{
	u_int real_index;
	if (NULL == lc_queue) {
		return NULL;
	}

	if (indexi >= lc_queue->cur ) {
		return NULL;
	}

	real_index = (lc_queue->end + indexi) % lc_queue->cap;

	return  lc_queue->buf + lc_queue->size * real_index;

}
