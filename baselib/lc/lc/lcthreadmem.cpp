
#include "lcthreadmem.h"
#include <limits.h>
#include <stdlib.h>

#undef LC_NO_DEBUG
#include "lcinlog.h"
namespace lc {


pthread_once_t  LcThreadMemManager :: thread_once = PTHREAD_ONCE_INIT;
pthread_key_t   LcThreadMemManager :: thread_key = PTHREAD_KEYS_MAX; 



void * LcMallocMemManager :: malloc(size_t msize) {
	return ::malloc(msize);
}

void LcMallocMemManager :: free(void *p, size_t ) {
	::free(p);
}

static void lcthread_mgrfree(lcthread_mem_manager_t * mgr) {
	if (NULL == mgr) {
		return;
	}
	lcthread_blocklist_t * list;
	lcthread_blocklist_t * nl;
	for (list = mgr->bigfree->next; NULL != list;) {
		nl = list->next;
		free(list->memory);
		free(list);
		list = nl;
	}
	free(mgr->bigfree);
	for (list = mgr->bigbusy->next; NULL != list;) {
		nl = list->next;
		free(list->memory);
		free(list);
		list = nl;
	}
	free(mgr->bigbusy);
	for (list = mgr->busy->next; NULL != list;) {
		nl = list->next;
		free(list->memory);
		free(list);
		list = nl;
	}
	free(mgr->busy);
	for (list = mgr->free->next; NULL != list;) {
		nl = list->next;
		free(list->memory);
		free(list);
		list = nl;
	}
	free(mgr->free);
	free(mgr);
}


static void lcthread_delkey(void* pvoid)
{
	lcthread_mem_manager_t * mgr = (lcthread_mem_manager_t *)pvoid;
	lcthread_mgrfree(mgr);
}

static void lcthread_genkey() {
	pthread_key_create(&LcThreadMemManager :: thread_key, lcthread_delkey);
}


LcThreadMemManager :: LcThreadMemManager(int blocksize) {
	pthread_once(&thread_once, lcthread_genkey);
	this->_fblocksize = blocksize;
	if (_fblocksize <= (size_t)_BLOCKHEADSIZE) {
		_fblocksize = 1048576;
	}
}

void  LcThreadMemManager :: add_list(lcthread_blocklist_t *list, lcthread_blocklist_t *node) {
	if (NULL == list || NULL == node) {
		return;
	}
	node->next = list->next;
	node->pre = list;
	if (NULL != list->next) {
		list->next->pre = node;
	}
	list->next = node;
}

void  LcThreadMemManager :: del_node(lcthread_blocklist_t *node) {
	if (NULL == node) {
		return;
	}
	if (node->pre) {
		node->pre->next = node->next;
	}
	if (node->next) {
		node->next->pre = node->pre;
	}
}


void * LcThreadMemManager :: thread_init() {
	lcthread_mem_manager_t * mng = (lcthread_mem_manager_t *)pthread_getspecific(thread_key);
	if(NULL == mng)
	{
		mng = (lcthread_mem_manager_t *)calloc(sizeof(lcthread_mem_manager_t), 1);
		if (NULL == mng) {
			return NULL;
		}
		lcthread_blocklist_t * list = (lcthread_blocklist_t *)alloc_listnode(0);
		if (NULL==list) {
			::free(mng);
			return NULL;
		}
		mng->free = list;
		list = (lcthread_blocklist_t *)alloc_listnode(0);
		if (NULL==list) {
			::free(mng->free);
			::free(mng);
			return NULL;
		}
		mng->busy = list;
		list = (lcthread_blocklist_t *)alloc_listnode(0);
		if (NULL==list) {
			::free(mng->busy);
			::free(mng->free);
			::free(mng);
			return NULL;
		}
		mng->bigbusy = list;
		list = (lcthread_blocklist_t *)alloc_listnode(0);
		if (NULL==list) {
			::free(mng->bigbusy);
			::free(mng->busy);
			::free(mng->free);
			::free(mng);
			return NULL;
		}
		mng->bigfree = list;
		pthread_setspecific(thread_key, (const void*)mng);
	}
	return mng;
}

void * LcThreadMemManager ::  alloc_listnode(int bksize) {
	lcthread_blocklist_t * list = (lcthread_blocklist_t *)calloc(sizeof(lcthread_blocklist_t), 1);
	if(NULL == list) {
		return NULL;
	}
	if (0 == bksize) {
		return list;
	}
	list->memory = (char *)::malloc(bksize);
	if(NULL == list->memory) {
		::free(list);
		return NULL;
	}
	list->blksize = bksize;
	list->used = 0;
	return list;
}


void * LcThreadMemManager ::  malloc(size_t msize) {
	if (0 >= msize) {
		return NULL;
	}
	lcthread_mem_manager_t * mng = (lcthread_mem_manager_t *)thread_init();
	if (NULL == mng) {
		return NULL;
	}
	AutoMLock alock(mng->thdlock);
	lcthread_blocklist_t * list;
	if (this->_fblocksize >= msize + _BLOCKHEADSIZE) {
		for (list = mng->free->next; NULL != list; list = mng->free->next) {
			if (list->blksize - list->left_offset >= msize + _BLOCKHEADSIZE) {
				lcthread_mem_t * membk = (lcthread_mem_t *)(list->memory + list->left_offset);
				membk->size = msize;
				membk->block = list;

				membk->manager = mng;
				list->left_offset += msize + _BLOCKHEADSIZE;
				list->used += msize + _BLOCKHEADSIZE;
				LC_IN_DEBUG("alloc [%p][%p: %d]", membk->mem, list, (int)msize);
				return membk->mem;
			} else {
				LC_IN_DEBUG("mov %p to busy", list);
				del_node(list);
				add_list(mng->busy, list);
			}
		}
		lcthread_blocklist_t * nextlist = (lcthread_blocklist_t *)alloc_listnode(_fblocksize);
		if (NULL==nextlist) {
			return NULL;
		}
		add_list(mng->free, nextlist);
		lcthread_mem_t * memnew = (lcthread_mem_t *)(nextlist->memory);
		memnew->size = msize;
		memnew->block = nextlist;

		memnew->manager = mng;
		nextlist->left_offset = msize + _BLOCKHEADSIZE;
		nextlist->used = msize + _BLOCKHEADSIZE;
		LC_IN_DEBUG("alloc new [%p][%p: %d]", memnew->mem, nextlist, (int)msize);
		return memnew->mem;
	} else {
		lcthread_blocklist_t * fblist;
		for (fblist = mng->bigfree->next; NULL != fblist; fblist = mng->bigfree->next) {
			if (fblist->blksize - fblist->left_offset >= msize + _BLOCKHEADSIZE) {
				lcthread_mem_t * membk = (lcthread_mem_t *)(fblist->memory + fblist->left_offset);
				membk->size = msize;
				membk->block = fblist;

				membk->manager = mng;
				fblist->left_offset += msize + _BLOCKHEADSIZE;
				fblist->used += msize + _BLOCKHEADSIZE;
				LC_IN_DEBUG("alloc big [%p: %d]", fblist, (int)msize);
				return membk->mem;
			} else {
				LC_IN_DEBUG("mov %p to bigbusy", fblist);
				del_node(fblist);
				add_list(mng->bigbusy, fblist);
			}
		}
		lcthread_blocklist_t * blist = (lcthread_blocklist_t *)alloc_listnode(msize + _BLOCKHEADSIZE);
		if (NULL==blist) {
			return NULL;
		}
		lcthread_mem_t * memnew = (lcthread_mem_t *)(blist->memory);
		memnew->size = msize;
		memnew->block = blist;

		memnew->manager = mng;
		blist->left_offset = msize + _BLOCKHEADSIZE;
		blist->used = msize + _BLOCKHEADSIZE;
		LC_IN_DEBUG("alloc big new [%p: %d]", blist, (int)msize);
		add_list(mng->bigbusy, blist);
		return memnew->mem;
	}
	return NULL;
}

void LcThreadMemManager ::  free(void *mem, size_t ) {




	lcthread_mem_t * thdmem = (lcthread_mem_t *)((char *)mem - _BLOCKHEADSIZE);
	AutoMLock alock(thdmem->manager->thdlock);

	thdmem->block->used -= (thdmem->size + _BLOCKHEADSIZE);
	LC_IN_DEBUG("LcThreadMemManager free [%p][%p]%d", mem, thdmem->block, thdmem->size);
	if (0 == thdmem->block->used) {
		if (this->_fblocksize == thdmem->block->blksize) {
			LC_IN_DEBUG("mov %p to free %p", thdmem->block, thdmem->manager->free);
			del_node(thdmem->block);
			thdmem->block->left_offset = 0;
			add_list(thdmem->manager->free, thdmem->block);
		} else if (this->_fblocksize < thdmem->block->blksize) {
			LC_IN_DEBUG("mov %p to bigfree %p", thdmem->block, thdmem->manager->bigfree);
			del_node(thdmem->block);
			thdmem->block->left_offset = 0;
			add_list(thdmem->manager->bigfree, thdmem->block);
		}
	}
}

LcThreadMemManager :: ~LcThreadMemManager() {
	lcthread_mem_manager_t * mng = (lcthread_mem_manager_t *)pthread_getspecific(thread_key);
	pthread_setspecific(thread_key, NULL);
	lcthread_mgrfree(mng);
}

}


