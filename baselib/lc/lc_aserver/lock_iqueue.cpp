
#include "lock_iqueue.h"

namespace lc {
int LockIQueue :: push(const asvr_lock_socket_t & pi) {
	AutoMLock mylock(_flock);
	return _fqueue.push_back(pi);
}
int LockIQueue :: pop(asvr_lock_socket_t * po) {
	AutoMLock mylock(_flock);
	if (NULL == po) {
		return -1;
	}
	asvr_lock_socket_t element = _fqueue.front();
	if (0 == _fqueue.pop_front()) {
		*po = element;
		return 0;
	}
	return -1;
}

LockIQueue :: LockIQueue() {
	_fqueue.create();
}

LockIQueue :: ~LockIQueue(){
	_fqueue.destroy();
}

}
