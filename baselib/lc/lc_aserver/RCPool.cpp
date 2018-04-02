
#include "RCPool.h"

namespace lc {



int LCRCPool :: init(void *resrc[], int size) {
	if (0>=size) {
		LC_IN_WARNING("LCRCPool(init) size<=0");
		return -1;
	}

	AutoMLock slock(_mrclock);










	_resrc_list.clear();
	for (int i=0; i < size; ++i ) {
		_resrc_list.push_back(resrc[i]);
	}
	return 0;
}



int LCRCPool :: get_resource(void ** prsrc) {
	if (NULL == prsrc) {
		LC_IN_WARNING("LCRCPool(get_resource) prsrc == NULL");
		return -1;
	}

	AutoMLock slock(_mrclock);



	if (_resrc_list.empty()) {
		LC_IN_WARNING("LCRCPool(get_resource) resource empty");
		return -1;
	}
	*prsrc = _resrc_list.back();
	_resrc_list.pop_back();




	return 0;
}



int LCRCPool :: drop_resource(void * src){

	AutoMLock slock(_mrclock);
	int ret = 0;
	IEvent * ev;
	timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	if (1 == _event_queue.pop_tv(&ev, 1, &tv)) {
		ret = _reactor->post(ev);
		ev->delRef();
		if (0 != ret) {
			LC_IN_WARNING("LCRCPool(drop_resource), post event(%p) failed.", ev);
			register_event(ev);
		}
	}
	_resrc_list.push_back(src);

	return ret;
}

int LCRCPool :: register_event(IEvent * ev){
	timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	int ret = _event_queue.push_tv(&ev, 1, &tv);
	if (1 == ret) {
		ev->addRef();
	}
	return ret;
}

LCRCPool :: LCRCPool(IReactor * act) :  _reactor(act) { 

}
LCRCPool :: ~LCRCPool() {

}

}


