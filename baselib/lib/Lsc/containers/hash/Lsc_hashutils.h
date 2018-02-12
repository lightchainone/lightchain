

#ifndef  __BSL_HASHUTILS_H_
#define  __BSL_HASHUTILS_H_
#include <algorithm>

#include "Lsc_hash_fun.h"

namespace Lsc
{

enum {
	HASH_EXIST = 0xffff,	//hash值存在
	HASH_NOEXIST,	//hash值不存在
	HASH_OVERWRITE,	//覆盖原有的hash值
	HASH_INSERT_SEC,	//插入成功
};

#if 0
template <class _Tp>
struct xhash
{
	inline size_t operator () (const _Tp &_1) const {
		return size_t(_1);
	}
};
#endif

enum { __Lsc_num_primes = 28 };

static const unsigned long __Lsc_prime_list[__Lsc_num_primes] =
{
	53ul,         97ul,         193ul,       389ul,       769ul,
	1543ul,       3079ul,       6151ul,      12289ul,     24593ul,
	49157ul,      98317ul,      196613ul,    393241ul,    786433ul,
	1572869ul,    3145739ul,    6291469ul,   12582917ul,  25165843ul,
	50331653ul,   100663319ul,  201326611ul, 402653189ul, 805306457ul, 
	1610612741ul, 3221225473ul, 4294967291ul
};

inline unsigned long __Lsc_next_prime(unsigned long __n)
{
	const unsigned long* __first = __Lsc_prime_list;
	const unsigned long* __last = __Lsc_prime_list + (int)__Lsc_num_primes;
	const unsigned long* pos = std::lower_bound(__first, __last, __n);
	return pos == __last ? *(__last - 1) : *pos;
}

};


#ifndef BSL_USER_LOCK
#define BSL_RWLOCK_T pthread_rwlock_t
#define BSL_RWLOCK_INIT(lock) pthread_rwlock_init(lock, NULL)
#define BSL_RWLOCK_DESTROY(lock) pthread_rwlock_destroy(lock)
#define BSL_RWLOCK_RDLOCK(lock) pthread_rwlock_rdlock(lock)
#define BSL_RWLOCK_WRLOCK(lock) pthread_rwlock_wrlock(lock)
#define BSL_RWLOCK_UNLOCK(lock) pthread_rwlock_unlock(lock)
#endif











#endif  //__BSL_HASHUTILS_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
