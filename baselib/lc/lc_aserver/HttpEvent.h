

#ifndef HTTPEVENT_H_
#define HTTPEVENT_H_

#include <lc/lcevent.h>
#include "LcSockEvent.h"

namespace lc {

class HttpEvent : plclic LcEvent {
plclic:
    HttpEvent():_fheader_length(0), _fsection_size(0), _fbody_length(0), _fbody_readdone(0){}

protected :
	int _fheader_length;   
	int _fsection_size;		  
	int _fbody_length;		  
	int _fbody_readdone;		  
	
	int http_readagain(cb_t again_cb);
	
	int http_find_crlfcrlf(int rdlen);
plclic :
	
	virtual void http_read_header_done()=0;
	
	virtual void http_read_section_done()=0;
	
	int http_package_read();
	
	int http_head_read();
	
	int http_section_read(int size = 0);
	
	int get_http_headlen();
	
	int get_http_bodylen();
	
	int http_package_finished();

	
	
	int http_readheader_process(cb_t proc_cb);
	
	void http_readheader_callback();
	
	void http_readsection_callback();
	
	void http_readpackage_callback();
	
	void http_get_contentlength();
	
	virtual int event_read();
};

} 



#endif 
