

#ifndef  __MEMPOOL_H_
#define  __MEMPOOL_H_

#include "comlog/comempool/dlist.h"

namespace comspace
{

#define SIZEOFPTR(x) \
	((*((int *)((size_t)(x) - sizeof(int)))) - (int)sizeof(int))

struct memlist_t
{
	memlist_t *next;
	long  used;
	char cnt[0];
};

class FixMem
{
	//分配的内存链表
	memlist_t *_mem;
	//目前正在分配的内存链表
	memlist_t *_now;
	//回收链表头
	memlist_t *_free;

	//分配计数
	int _nowb;
	int _reab;

	//连续内存块的大小
	long _size;

	//定长代码块的大小
	int _bsize;
	//每次分配多少块
	int _bnum;
plclic:
	int create(int bnum, int bsize);
	void destroy();
	void *alloc();
	void dealloc(void *);
	void reset();
};


class MemPool
{
	static const int DEFLEVEL = 11;
	static const int DEFMAXMEM = 1<<20;
	static const int DEFMINMEM = 1<<10;

	//管理的内存区间，最小内存到最大内存，不包括头int
	int _maxmem;
	int _minmem;
	//目前所能分配的最大内存，超过部分直接malloc
	int _allocmem;
	//级数递增的比例
	float _rate;
	//目前最大的级数
	int _level;
	//内存管理池
	FixMem _fixmem[DEFLEVEL];
	//大内存的回收链表
	dlist_t *_biglist;
plclic:
	/**
	 *
	 * 									最多10级
	**/
	int create(int minmem=DEFMINMEM, int maxmem=DEFMAXMEM, float rate=2.0f);

	/**
	 *
	**/
	void destroy();

	/**
	 *
	**/
	void *alloc(int size);

	/**
	 *
	**/
	void dealloc(void *);

	void reset();

plclic:
	int getidx(int size);
};


};

#endif  //__MEMPOOL_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
