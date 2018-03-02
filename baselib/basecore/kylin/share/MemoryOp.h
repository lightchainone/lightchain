
#ifndef _MEMORY_OP_H_
#define _MEMORY_OP_H_

#define _XOPEN_SOURCE 600
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include "Trace.h"

#ifdef	__cplusplus
extern "C" {
#endif
void *memalign(size_t boundary, size_t size);
#ifdef	__cplusplus
}
#endif

static inline void* 
AlignAlloc(int nAlign, int nSize)
{	
	void *p;
#if 0
	if (0 == posix_memalign(&p, nAlign, nSize))
		return p;
	PERROR("posix_memalign");
	return NULL;
#else
	p = memalign(nAlign, nSize);
	if (NULL == p) {
		PERROR("memalign");
		return NULL;
	}
#ifdef _DEBUG
    memset(p, 0xCC, nSize);
#endif
    return p;
#endif
}

static inline void*
ZeroAlloc(int nSize)
{
	void* p = malloc(nSize);
	if (p) {
		memset(p, 0, nSize);
		return p;
	}
	PERROR("malloc");
	return NULL;
}

template <typename T>
static inline T*
ZeroAlloc()
{
    return (T*)ZeroAlloc(sizeof(T));
}

static inline void*
AlignAllocByPage(int nSize)
{
	static int ps = 4096;	
	return AlignAlloc(ps, nSize);
}

#define ZeroMemory(m, s)		memset(m, 0, s)
#define CopyMemory(d, s, n)		memcpy(d, s, n)

#endif
