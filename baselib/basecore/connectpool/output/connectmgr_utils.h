

#ifndef  __CONNECTMGR_UTILS_H_
#define  __CONNECTMGR_UTILS_H_
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <pthread.h>
#include <arpa/inet.h>
#include <vector>
#include "Lsc/containers/string/Lsc_string.h"

#define CM_DEF_RETRY "default_retry"
#define CM_CONN_TYPE "connect_type"
#define CM_DEF_CTIMEOUT "default_ctimeout"
#define CM_DEF_PORT "default_port"
#define CM_DEF_CAPACITY "default_capacity"
#define CM_SERVER "server"
#define CM_IP "ip"
#define CM_PORT "port"
#define CM_CAPACITY "capacity"
#define CM_CTIMEOUT "ctimeout"
#define CM_TAG "tag"

#define VALUE_LONG "LONG"
#define VALUE_SHORT "SHORT"

namespace comcm{

	typedef Lsc::string str_t;		  
	typedef long long mstime_t;

	class AutoLock{
			pthread_mutex_t * _lock;
		plclic:
			AutoLock(pthread_mutex_t * lock){
				_lock = lock;
				pthread_mutex_lock(_lock);
			}
			~AutoLock(){
				pthread_mutex_unlock(_lock);
			}
	};

	class RWLock{
			pthread_rwlock_t _lock;
		plclic:
			RWLock(){
				pthread_rwlock_init(&_lock, NULL);
			}
			int read(){
				return pthread_rwlock_rdlock(&_lock);
			}
			int write(){
				return pthread_rwlock_wrlock(&_lock);
			}
			int unlock(){
				return pthread_rwlock_unlock(&_lock);
			}
	};

	class AutoRead{
			RWLock * _lock;
		plclic:
			AutoRead(RWLock * p){
				_lock = p;
				_lock->read();
			}
			~AutoRead(){
				_lock->unlock();
			}
	};

	class AutoWrite{
			RWLock * _lock;
		plclic:
			AutoWrite(RWLock * p){
				_lock = p;
				_lock->write();
			}
			~AutoWrite(){
				_lock->unlock();
			}
	};

	unsigned int ip_to_int(const char * ip);
	void ms_sleep(int ms);
	mstime_t ms_time();

}


















#endif  


