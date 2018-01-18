

#ifndef LCTHREADMEM_H_
#define LCTHREADMEM_H_

#include <stddef.h>
#include <pthread.h>
#include <Lsc/pool/Lsc_pool.h>
#include "lclock.h"

namespace lc {






typedef struct _lcthread_blocklist_t lcthread_blocklist_t;

typedef struct _lcthread_mem_manager_t lcthread_mem_manager_t ;

typedef struct _lcthread_mem_t {
	lcthread_blocklist_t * block;
	lcthread_mem_manager_t * manager;
	unsigned int size; 
	char mem[0];
} lcthread_mem_t;

typedef struct _lcthread_blocklist_t {
	char * memory;
	unsigned int blksize;
	unsigned int used;
	unsigned int left_offset; 
	lcthread_blocklist_t * pre;
	lcthread_blocklist_t * next;
	int busy;
} lcthread_blocklist_t;

struct _lcthread_mem_manager_t {
	lcthread_blocklist_t * busy;
	lcthread_blocklist_t * free;
	lcthread_blocklist_t * bigfree;
	lcthread_blocklist_t * bigbusy;
	MLock thdlock; 
};

















class LcMallocMemManager : plclic Lsc::mempool {
	
	virtual void * malloc(size_t msize);
	
	virtual void free(void *p, size_t msize);
};

class LcThreadMemManager : plclic Lsc::mempool {
private:
	size_t _fblocksize; 
	static const int _BLOCKHEADSIZE = sizeof(lcthread_blocklist_t *) +
			sizeof(lcthread_mem_manager_t *) + sizeof(unsigned int);
	
	void * thread_init();
	
	void * alloc_listnode(int bksize);
	
	void  add_list(lcthread_blocklist_t *list, lcthread_blocklist_t *node);
	
	void  del_node(lcthread_blocklist_t *node);
plclic:
	static pthread_once_t  thread_once;
	static pthread_key_t   thread_key; 
	
	LcThreadMemManager(int blocksize = 1048576);
	
	virtual ~LcThreadMemManager();
	
	virtual void * malloc(size_t msize);
	
	virtual void free(void *mem, size_t msize);
};

}

#endif 
