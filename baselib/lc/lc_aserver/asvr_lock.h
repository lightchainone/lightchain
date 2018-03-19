

#ifndef ASVR_LOCK_H_
#define ASVR_LOCK_H_
namespace lc{
class Asvr_ScopeLock {		  
private:
	pthread_mutex_t * _plock;		  
plclic :
	
	Asvr_ScopeLock(pthread_mutex_t *plk) : _plock(plk) {
		pthread_mutex_lock(_plock);
	}
	
	~Asvr_ScopeLock() {
		pthread_mutex_unlock(_plock);
	}
};
}
#endif 
