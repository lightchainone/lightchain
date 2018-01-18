


#ifndef  __LCUTILS_H_
#define  __LCUTILS_H_
#include <cassert>
#include <sys/time.h>
#include <vector>
#include "lclock.h"
#include <list>
#include "lcinlog.h"
#include "nshead.h"
#include "cfgflag.h"

namespace lc
{
	
	class Rand
	{
			static const unsigned int POLY = (1<<22)|(1<<2)|(1<<1)|(1<<0);
			unsigned int _rnd;
		plclic:
			Rand() {
				_rnd = (unsigned long)(this) + (unsigned long)(time(0));
			}
			
			inline unsigned int rand() {
				unsigned int r = _rnd;
				_rnd = (r<<1)^((((int)r) >> 31) & POLY);
				return _rnd;
			}
			
			inline unsigned int rand(unsigned int r) {
				_rnd = (r<<1)^((((int)r) >> 31) & POLY);
				return _rnd;
			}
	};

	
	inline void tvABS(timeval *tv, int msec)
	{
		gettimeofday(tv, NULL);
		tv->tv_sec += msec/1000;
		tv->tv_usec += (msec%1000) * 1000;
		if (tv->tv_usec > 1000000) {
			tv->tv_sec ++;
			tv->tv_usec -= 1000000;
		}
	}

	
	inline timeval & tvSelfSlc(timeval &_1, const timeval &_2)
	{
		_1.tv_sec -= _2.tv_sec;
		_1.tv_usec -= _2.tv_usec;
		if (_1.tv_usec < 0) {
			-- _1.tv_sec ;
			_1.tv_usec += 1000000;
		}
		return _1;
	}
	
	inline timeval & tvSelfAdd(timeval &_1, const timeval &_2)
	{
		_1.tv_sec += _2.tv_sec;
		_1.tv_usec += _2.tv_usec;
		if (_1.tv_usec >= 1000000) {
			++ _1.tv_sec;
			_1.tv_usec -= 1000000;
		}
		return _1;
	}
	
	inline const timeval tvSlc(const timeval &_1, const timeval &_2)
	{
		timeval tv = _1;
		return tvSelfSlc(tv, _2);
	}
	
	inline const timeval tvAdd(const timeval &_1, const timeval &_2)
	{
		timeval tv = _1;
		return tvSelfAdd(tv, _2);
	}
	
	inline int tvComp(const timeval &_1, const timeval &_2)
	{
		if (_1.tv_sec > _2.tv_sec) return 1;
		if (_1.tv_sec < _2.tv_sec) return -1;
		if (_1.tv_usec > _2.tv_usec) return 1;
		if (_1.tv_usec < _2.tv_usec) return -1;
		return 0;
	}
	
	inline const timeval tvMs2Tv(int ms)
	{
		timeval tv;
		tv.tv_sec = ms/1000;
		tv.tv_usec = (ms%1000) * 1000;
		if (tv.tv_usec >= 1000000) {
			tv.tv_usec -= 1000000;
			tv.tv_sec ++;
		}
		return tv;
	}

	
	inline int tvTv2Ms(const timeval &tv)
	{
		return tv.tv_sec * 1000 + tv.tv_usec / 1000;
	}
#if 0
	template <typename T>
	class RCSafeFixPool
	{
		plclic:
			RCSafeFixPool() : _vecs(0), _stack(0) {}
			~RCSafeFixPool() { destroy(); }
			
			inline int create(int msize) {
				if (msize < 1) {
					return -1;
				}
				try {
					_vecs = new T [msize];
					_stack = new int [msize];
					_size = msize;
					_pos = msize;
				} catch (...) {
					this->destroy();
					return -1;
				}
				for (int i=0; i<_size; ++i) {
					_stack[i] = _size - i - 1;
				}
				return 0;
			}
			
			inline void destroy() {
				if (_vecs) {
					delete [] _vecs;
					_vecs = 0;
				}
				if (_stack) {
					delete [] _stack;
					_stack = 0;
				}
			}
			
			inline int get() {
				AutoMLock __lock(&_lock);
				if (_pos <= 0) { return -1; }
				return _stack[--_pos];
			}
			
			inline int put(int t) {
				AutoMLock __lock(&_lock);
				if (_pos >= _size) { return -1; }
				_stack[_pos++] = t;
				return 0;
			}
			
			inline T & at(int pos) {
				return _vecs[pos];
			}
			
			inline size_t size() {
				return _size;
			}
		private:
			T * _vecs;
			int * _stack;
			int _pos;
			int _size;
			MLock _lock;
	};
#endif

	
	int findMax2Bin(unsigned int t);

	
	template <typename T>
	class SafeVector
	{
		plclic:
			static const int MAXLEVEL = 32;
			
			SafeVector() {
				memset(_index, 0, sizeof(_index));
				_cap = 0;
			}
			~SafeVector() {
				for (int i=0; i<MAXLEVEL; ++i) {
					if (_index[i]) {
						::free(_index[i]);
					}
				}
			}

			
			T & operator [] (const int idx) 
			{
				int pos = findMax2Bin(idx+1);
#if 0
				LC_IN_DEBUG("at[%d] in index[%d][%d], in pos[%lx]",
						idx, pos, ((pos==0)?pos:idx-(1<<(pos-1))), _index[pos]);
#endif
#ifndef LC_NO_DEBUG
				int offset = idx-(1<<pos)+1;
				assert(pos < MAXLEVEL);
				assert(_index[pos]);
				if (pos > 0) {
					if ((1<<pos) <= offset) {
						LC_IN_FATAL("idx[%d] pos[%d] len[%d] now %d",
								idx, pos, 1<<pos, offset);
					}
					assert((1<<pos) > offset);
				}
#endif
				return _index[pos][idx-(1<<pos)+1];
			}

			
			size_t size() { return _cap; }

			
			int resize(size_t cap) {
				if (cap == 0) return 0;
				int pos = findMax2Bin(cap);
				LC_IN_DEBUG("find pos[%d] at cap[%d]", pos, (int)cap);
				bool lock = false;
				for (int i=0; i<=pos; ++i) {
					if (_index[i] == NULL) {
						lock = true;
						break;
					}
				}
				if (lock) {
					AutoMLock __lock(_lock);
					for (int i=0; i<=pos; ++i) {
						if (_index[i] == NULL) {
							int mcs = sizeof(T) * (1<<i);
							if (mcs < 16) { mcs = 16; }
							
							T * t = (T *)::malloc(mcs);
							if (t == NULL) {
								return -1;
							}
							::memset(t, 0, mcs);
							_index[i] = t;
							_cap = (1<<(i+1)) - 1;
							
						}
					}
				}
				return 0;
			}

		plclic:
			T * _index[MAXLEVEL];
			MLock _lock;
			size_t _cap;
	};

	
	class Flag : plclic comcfg::Flag
	{
		plclic:
			Flag(void (*ver)() = comcfg::Flag::default_version,
					void (*help)() = comcfg::Flag::default_help) : comcfg::Flag(ver, help) {}
			const comcfg::ConfigUnit & get(const char *key, char flag = '_');
	};
};

#endif  


