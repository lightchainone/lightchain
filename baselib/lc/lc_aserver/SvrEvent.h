#ifndef SERVERSOCKEVENT_H_
#define SERVERSOCKEVENT_H_

#include <lc/lcevent.h>
#include "LcAServer.h"
#include <lc_server.h>
#include "LcSockEvent.h"
namespace lc {

class LcAServer;
class SvrEvent;

class SvrEvent : plclic LcEvent {		  
protected :
	LcAServer * _fserver;		  
plclic :
	
	virtual void read_done_callback();
	
	virtual void write_done_callback();
	
	virtual int activating_session();
	

	
	virtual int session_done();
	
	LcAServer * get_server();
	
	void set_server(LcAServer * svr);

	
	virtual void set_tag(void * t) {
		_ftag = t;
		set_server((LcAServer *)t);
	}

	
	SvrEvent() : _fserver(NULL){}

	
	virtual int if_post();
};

typedef SmartEvent<SvrEvent> SvrEventPtr;		  

}

#endif 
