

#ifndef LOCK_QUEUE_H_
#define LOCK_QUEUE_H_

#include <Lsc/deque.h>
#include <lc/lclock.h>
#include <sys/socket.h>
#include <netinet/in.h>

namespace lc {

typedef struct _asvr_lock_socket_t {
	int fd;	  
	struct sockaddr_in addr;	  
}asvr_lock_socket_t; 	  

class LockIQueue {
plclic :
	
	int push(const asvr_lock_socket_t & pi);
	
	int pop(asvr_lock_socket_t * po);
	
	LockIQueue();
	
	~LockIQueue();
private:
	MLock _flock; 
	Lsc::deque<asvr_lock_socket_t, 1024UL> _fqueue; 
};


}

#endif 
