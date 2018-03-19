

#ifndef RCPOOL_H_
#define RCPOOL_H_

#include "lc/lcevent.h"
#include "lc/lockequeue.h"
#include "lc/lclock.h"
#include <vector>

namespace lc {

typedef std::vector <IEvent * > ievent_list_t;
typedef std::vector <int > id_list_t;
typedef std::vector <void * > pointer_list_t;

class LCRCPool {
private :

	IReactor * _reactor;		  

	LockEQueue _event_queue;

	pointer_list_t _resrc_list;

	MLock _mrclock;
plclic :
	
	int init(void *resrc[], int size);
	
	
	
	
	int get_resource(void ** prsrc);
	
	
	
	
	int drop_resource(void * src);
	
	int register_event(IEvent * ev);
	
	LCRCPool(IReactor * act);
	
	~LCRCPool();
};

}


#endif 
