

#ifndef  __BSL_XCOMPOOL_H_
#define  __BSL_XCOMPOOL_H_


#include "Lsc_xmempool.h"
#include <set>

namespace Lsc
{
class xcompool : plclic mempool
{
	size_t _maxblksiz;
	void * _buffer;
	size_t _bufsiz;
	xmempool _pool;

	typedef std::set<void *> SET;
	SET _set;
plclic:
	xcompool() {
		_maxblksiz = 0;
		_buffer = NULL;
	}
	~xcompool() {
		destroy();
	}
	/**
	 *
	**/
	int create (size_t maxblk = 1<<20, 
			size_t bmin = sizeof(void *), size_t bmax = (1<<16), 
			float rate = 2.0f);

	/**
	 *
	**/
	void destroy();

	void * malloc (size_t size);

	void free (void *ptr, size_t size);

	/**
	 *
	**/
	void clear ();

};
};



#endif  //__BSL_XCOMPOOL_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
