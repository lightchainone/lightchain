

#ifndef  __LCLOCK_H_
#define  __LCLOCK_H_
#include <sys/time.h>
#include <pthread.h>
#include "lcinlog.h"

namespace lc
{
	class MLock
	{
		plclic:
			inline MLock() {
				pthread_mutex_init(&_lock, NULL);
			}
			inline ~MLock() {
				pthread_mutex_destroy(&_lock);
			}
			inline int lock() {
				return pthread_mutex_lock(&_lock);
			}
			inline int unlock() {
				return pthread_mutex_unlock(&_lock);
			}
			inline int trylock() {
				return pthread_mutex_trylock(&_lock);
			}

		private:
			pthread_mutex_t _lock;

			friend class MCondition;
	};
#if 0
	class RWLock
	{
		plclic:
			inline RWLock() {
				pthread_rwlock_init(&_lock);
			}
			inline ~RWLock() {
				pthread_rwlock_destroy(&_lock);
			}
			inline int rlock() {
				return pthread_rwlock_rdlock(&_lock);
			}
			inline int tryrlock() {
				return pthread_rwlock_tryrdlock(&_lock);
			}
			inline int rlock_ms(const struct timespec *abs_time) {
				return pthread_rwlock_timedrdlock(&_lock, abs_time);
			}
			inline int wlock() {
				return pthread_rwlock_wrlock(&_lock):
			}
			inline int trywlock() {
				return pthread_rwlock_trywrlock(&_lock);
			}
		private:
			pthread_rwlock_t _lock;
	};
#endif
	class SpinLock
	{
		plclic:
			inline SpinLock() {
				pthread_spin_init(&_lock, PTHREAD_PROCESS_PRIVATE);
			}
			inline ~SpinLock() {
				pthread_spin_destroy(&_lock);
			}
			inline int lock() {
				return pthread_spin_lock(&_lock);
			}
			inline int unlock() {
				return pthread_spin_unlock(&_lock);
			}
			inline int trylock() {
				return pthread_spin_trylock(&_lock);
			}

		private:
			pthread_spinlock_t _lock;
	};

	template <typename Lock>
	class AutoLock
	{
		plclic:
			inline AutoLock(Lock &l) : _lock(l) {
				l.lock();
                start_lock_time_ = time(NULL);
			}
			inline ~AutoLock() {
				_lock.unlock();
                time_t end_lock_time = time(NULL);
                if(end_lock_time - start_lock_time_ > 3){
                    LC_IN_NOTICE("lock_time[%ld] start_lock_time[%ld] end_lock_time[%ld] too long", 
                            end_lock_time-start_lock_time_, start_lock_time_, end_lock_time);
                }
			}
		private:
			Lock &_lock;
            time_t start_lock_time_;
	};

	typedef AutoLock<MLock> AutoMLock;
	typedef AutoLock<SpinLock> AutoSLock;

	class MCondition
	{
		plclic:
			MCondition(MLock &l) : _lock(l._lock), _waits(0) {
				pthread_cond_init(&_cond, NULL);
			}
			MCondition(pthread_mutex_t &l) : _lock(l), _waits(0) {
				pthread_cond_init(&_cond, NULL);
			}
			~MCondition() {
				pthread_cond_destroy(&_cond);
			}
			inline int wait(const timeval *tv) {
				++ _waits;
				int ret = 0;
				if (tv) {
					timespec ts;
					ts.tv_sec = tv->tv_sec;
					ts.tv_nsec = tv->tv_usec * 1000;
					ret = pthread_cond_timedwait(&_cond, &_lock, &ts);
					--_waits;
					return ret;
				}
				ret = pthread_cond_wait(&_cond, &_lock);
				-- _waits;
				return ret;
			}
			inline void signal() {
				pthread_cond_signal(&_cond);
			}
			inline void signalAll() {
				pthread_cond_broadcast(&_cond);
			}
			inline size_t waits() { return _waits; }
		private:
			pthread_cond_t _cond;
			pthread_mutex_t &_lock;
			size_t _waits;
	};
};


#endif  


