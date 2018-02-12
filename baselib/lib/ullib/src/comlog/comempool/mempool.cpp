#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mempool.h"

namespace comspace
{


#ifdef __XDEBUG__
#define _debug(fmt, arg...) \
	fprintf(stderr, "[%s:%d]"fmt"\n", __FILE__, __LINE__, ##arg);
#else
#define _debug(fmt, arg...)  {}
#endif

void * FixMem::alloc()
{
	_debug("addr[%lx], bsize[%d] nowb[%d] reab[%d] used[%ld]", _now->cnt, _bsize, _nowb, _reab, 
			(_now)?_now->used:0);
	//_debug("size[%d] offset[%ld], empty[%ld]", sizeof(memlist_t), (_now)?((long)_now->cnt-(long)_now):0, (_now)?_now->empty:0);
	++ _nowb;

again:
	if (_now == NULL) {
		//分配新内存重新再来
		_now = (memlist_t *)malloc(sizeof(memlist_t) + _size);
		if (_now == NULL) {
			_debug("wo kao zerro[%d]%m", sizeof(memlist_t) + _size);
			goto fail;
		}
		memset(_now, 0, sizeof(memlist_t));
		_debug("crate a block[%d]", sizeof(memlist_t) + _size);
		_reab += _bnum;
		_now->next = _mem;
		_now->used = 0;
		_mem = _now;

		_debug("addr[%lx], bsize[%d] nowb[%d] reab[%d] used[%ld]", _now->cnt, _bsize, _nowb, _reab, 
				(_now)?_now->used:0);
	}

	//还有可用内存
	if (_now->used < _size) {
		//将连续内存切割
		void *ptr = (void *)(_now->cnt + _now->used);
		_now->used += (long)_bsize;
		_debug("cut memory ptr[%lx] cnt[%lx] used[%ld] bsize[%ld]", ptr, 
				_now->cnt,  _now->used, (long)_bsize);
		return ptr;
	}
	//空闲内存被用光的前提下，看还有没有事先分配好的内存
	if (_now->next == NULL || _now->next->used == _size) {//实现分配好的内存用光
		//回收列表用光
		if (_free == NULL) {
			_now = NULL;
			_debug("free is empty [%d]", _bsize);
			goto again;
		} else {
			//返回空闲列表
			memlist_t *ret = _free;
			_free = _free->next;
			
			_debug("get free [%lx][%lx][%d]", ret, _free, _bsize);
			return (void *)ret;
		}
	} else {
		//获取新的空闲内存，再来
		_now = _now->next;
		_debug("get free block [%d]", _bsize);
		goto again;
	}
fail:
	-- _nowb;
	return NULL;
}

void FixMem::dealloc(void *ptr)
{
	_debug("change _free [%lx] to [%lx] at %ld", _free, ptr, _size);
	((memlist_t *)ptr)->next = _free;
	_free = (memlist_t *)ptr;
	-- _nowb;
}

int FixMem::create(int bnum, int bsize)
{
	_bsize = bsize;
	_bnum = bnum;
	_size = bsize * bnum;
	_nowb = 0;
	_reab = 0;
	_mem = _now = _free = NULL;
	return 0;
}

void FixMem::destroy()
{
	memlist_t *ptr = _mem;
	while (ptr != NULL) {
		ptr = _mem->next;
		free (_mem);
		_mem = ptr;
	}
}

void FixMem::reset()
{
	_now = _mem;
	_free = NULL;
	_nowb = 0;
	//不回收已经分配的内存
	for (memlist_t *ptr = _mem; ptr != NULL; ptr = ptr->next) {
		ptr->used = 0;
	}
}

int MemPool::create(int minmem, int maxmem, float rate)
{
	if (rate <= 1.01f) {
		rate = 2.0f;
	}
	if (minmem < DEFMINMEM) {
		minmem = DEFMINMEM;
	}
	if (maxmem < DEFMAXMEM) {
		maxmem = DEFMAXMEM;
	}
	if (minmem >= maxmem) {
		return -1;
	}
	_minmem = minmem;
	_maxmem = maxmem;

	_allocmem = _maxmem;
	_rate = rate;
	_level = 0;
	_biglist = NULL;

	int mem = _minmem;
	for (int i=0; i<DEFLEVEL; ++i) {
		int rsize = mem + sizeof(int);
		if (rsize <= _maxmem + (int)sizeof(int)) {
			if (_fixmem[i].create(_maxmem/mem, rsize) != 0) {
				return -1;
			}
			_debug("create bnum[%d] rsize[%d] level[%d] meminfo", _maxmem/mem, rsize, _level);
			++ _level;
		} else {
			break;
		}
		_allocmem = rsize;
		mem = (int)(mem * rate);
	}
	return 0;
}

void MemPool::destroy()
{
	for (int i=0; i<_level; ++i) {
		_fixmem[i].destroy();
	}
}

void MemPool::reset()
{
	for (int i=0; i<_level; ++i) {
		_fixmem[i].reset();
	}
	dlist_walk(_biglist, free);
	_biglist = NULL;
}

int MemPool::getidx(int size)
{
	int ptr = 0;
	int base = _minmem;
	while (size > base + (int)sizeof(int)) {
		++ ptr;
		base = (int)(base * _rate);
		_debug("alloc base[%d] ptr[%d]", base, ptr);
	}
	return ptr;
}

void *MemPool::alloc(int size)
{
	int rsize = (size + sizeof(int) - 1) / sizeof(int) * sizeof(int) + sizeof(int);

	_debug("alloc rsize[%d] allocmem[%d] base[%d]", rsize, _allocmem, _minmem);
	if (rsize > _allocmem) {
		dlist_t *lst = (dlist_t *)malloc(rsize + sizeof(dlist_t));
		lst->size[0] = rsize;
		_biglist = dlist_insert(_biglist, lst);
		return (void *)(lst->size+1);
	}
	int *ret = (int *)_fixmem[getidx(rsize)].alloc();
	*ret = rsize;
	return ret+1;
}

void MemPool::dealloc(void *ptr)
{
	int *node = (int *)ptr - 1;
	int rsize = *node;
	if (rsize > _allocmem) {
		dlist_t *lst = (dlist_t *)((size_t)node - sizeof(dlist_t));
		_biglist = dlist_erase(_biglist, lst);
		free (lst);
	} else {
		_fixmem[getidx(rsize)].dealloc(node);
	}
}
};

/* vim: set ts=4 sw=4 sts=4 tw=100 */
