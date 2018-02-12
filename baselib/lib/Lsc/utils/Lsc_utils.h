

#ifndef  __BSL_UTILS_H_
#define  __BSL_UTILS_H_

#include <cstring>
#include <functional>
#include <Lsc/archive/Lsc_serialization.h>
#include <Lsc/utils/Lsc_construct.h>
#include <pthread.h>
#include <utility>

namespace Lsc
{

//有些模版参数需要预处理
#define __MAC_BSL_MIN(_1, _2) (((_1)<(_2)) ? (_1) : (_2))
template<class _Tp>
inline const _Tp & min (const _Tp &_1, const _Tp &_2)
{
	return __MAC_BSL_MIN(_1, _2);
}
#define __MAC_BSL_MAX(_1, _2) (((_1)<(_2)) ? (_2) : (_1))
template <class _Tp>
inline const _Tp & max (const _Tp &_1, const _Tp &_2)
{
	return __MAC_BSL_MAX(_1, _2);
}

template <class _Tp>
struct equal
{
	inline bool operator () (const _Tp &_1, const _Tp &_2) const {
		return _1 == _2;
	}
};

template <class _Tp>
struct more
{
	inline bool operator () (const _Tp &_1, const _Tp &_2) const {
		return _1 > _2;
	}
};

template <class _Tp>
struct less
{
	inline bool operator () (const _Tp &_1, const _Tp &_2) const {
		return _1 < _2;
	}
};

#if 0
template <class _T, class _Q>
struct pair
{
	typedef _T first_type;
	typedef _T second_type;

	_T first;
	_Q second;
plclic:
	pair(const pair & p) : first(p.first), second(p.second) {}
	pair(const _T &t, const _Q &q) : first(t), second(q) {}
	pair() {}

	template <class _Archive>
	int serialization(_Archive &ar) {
		if (Lsc::serialization(ar, first)) return -1;
		if (Lsc::serialization(ar, second)) return -1;
		return 0;
	}

	template <class _Archive>
	int deserialization(_Archive &ar) {
		if (Lsc::deserialization(ar, first)) return -1;
		if (Lsc::deserialization(ar, second)) return -1;
		return 0;
	}
};
#endif

template <class _Pair>
struct pair_first
{
	typedef typename _Pair::first_type type;
	const type & operator () (const _Pair & __p) const {
		return __p.first;
	}
};

template <class _Pair>
struct pair_second
{
	typedef typename _Pair::second_type type;
	const type & operator () (const _Pair & __p) const {
		return __p.second;
	}
};

template <class _Tp>
struct param_select
{
	const _Tp & operator () (const _Tp & __p) const {
		return __p;
	}
};

template <bool _flag, typename _1, typename _2>
struct select2st
{
	typedef _1 type;
};
template <typename _1, typename _2>
struct select2st <false, _1, _2>
{
	typedef _2 type;
};

//锁保护
class mutexlock_monitor
{
	pthread_mutex_t *_lock;
plclic:
	mutexlock_monitor(pthread_mutex_t *lock) : _lock(lock) {
		pthread_mutex_lock(_lock);
	}
	~mutexlock_monitor() {
		pthread_mutex_unlock(_lock);
	}
};

//获取一个无符号常整形的最高位所在位置
template <unsigned long val, int bits = sizeof(unsigned long) * 8 - 1/*63*/>
struct high_bitpos
{
	static const int value = ((val < (1UL<<bits)) ? (high_bitpos<val, bits-1>::value) : (bits+1));
};

template <unsigned long val>
struct high_bitpos<val, 0>
{
	static const int value = 0;
};

template <unsigned long val, int bits = 0>
struct moreq2
{
	static const unsigned long value = ((val > (1UL<<bits)) ? (moreq2<val, bits+1>::value) : (1UL<<bits));
};

template <unsigned long val>
struct moreq2<val, sizeof(unsigned long) * 8 /*64*/>
{
	static const unsigned long value = 0;
};

}//namespace Lsc
#endif  //__BSL_UTILS_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
