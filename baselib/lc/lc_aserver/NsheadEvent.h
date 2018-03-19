

#ifndef NSHEADEVENT_H_
#define NSHEADEVENT_H_


#include <lc/lcevent.h>
#include "LcSockEvent.h"
#include <Lsc/containers/string/Lsc_string.h>

namespace lc {

class NsheadEvent : plclic LcEvent {

plclic:
	typedef void* (*allocate_callback_t)(size_t size, void *userdata);
	typedef void  (*release_callback_t)(void *ptr, size_t size, void *userdata);

	
	typedef struct _allocator
	{
		allocate_callback_t allocate_func;	 
		release_callback_t	release_func;	 
		void				*userdata;		 

	} allocator_t;

plclic :
	
	virtual int event_read();
	
	int lnhead_aread();
	
	int lnhead_aread_2buf(lnhead_t *head, void * body, int bodylen);
	
	int lnhead_awrite();
	
	int lnhead_awrite_2buf(lnhead_t *head, void * body, int bodylen);
	
	void nsbackup_callback();
	
	void nsrestore_callback();
	
	virtual Lsc::string get_notice_string();
	
	void set_cmdno(int cmdno);
	
	int get_cmdno();
	
	void set_reqsvrname(const char * svrname);
	
	

	
	


	


	






	
	NsheadEvent() : _fold_nscb(NULL), _fold_nscbp(NULL), _freq_svrname(""), _fcmdno(0), _fmonitor(0){
		bzero(&_allocator, sizeof(allocator_t));
		_setflag = false;
		_user_nsbody   = NULL;
        _user_nsbody_size = 0;
	}

	~NsheadEvent();
	
	

	


	
	void lnhead_write2buf_callback();

	
	void set_monitor(int mon);
	
	int get_monitor();

	
	const lnhead_t * get_read_lnhead();
	
	const void * get_read_nsbody(int pkgsize=0, int copy =0);
	
	lnhead_t * get_write_lnhead();
	
	void * get_write_nsbody();

plclic:

	
	
	
	
	
	
	
	
	
	
	
	
	
	

	
	
    int   set_allocator(allocator_t *ma);

private:
	void* allocator_allocate(size_t size);
	void  allocator_release(void *ptr, size_t size);

plclic:
	
	void* allocator_allocate_nsbody(size_t size);
	void  allocator_release_nsbody();
	
	bool  has_allocator();


private :
	cb_t _fold_nscb;		  
	void * _fold_nscbp;		  
	
	Lsc::string _freq_svrname;		  
	int _fcmdno;		  
	int _fmonitor; 	  
	lnhead_t * _fread_head; 	  
	void * _fread_body; 	  
	int _fread_body_buflen; 	  
	int _fread_div;	  
	lnhead_t * _fwrite_head; 	  
	void * _fwrite_body; 	  
	int _fwrite_body_buflen; 	  
	int _fwrite_div;	  

	allocator_t     _allocator;       
	bool            _setflag;
	void            *_user_nsbody;     
	size_t          _user_nsbody_size; 

	
	int lnhead_read_process(cb_t proc_cb, unsigned int rdlen);
	
	int lnhead_readagain(cb_t again_cb);

};

}
#endif 
