
#ifndef  __LCCLIENT_HTTP_H_
#define  __LCCLIENT_HTTP_H_

#include "lcclient_nshead.h"
#include "LcSockEvent.h"
#include "HttpEvent.h"

namespace lc {
	
	enum {
		STATUS_CONNERROR = 0x2000000		  
	};
	class LcClientEvent{
		plclic:
	
			
			LcClientEvent(){}
			
			~LcClientEvent(){}


			
			static void conn_callback(IEvent *e, void *p);
		
			
			static int freeConn(common_talkwith_t *currTalk, int tag = 0);

            
            static int freeConnWithoutCallback(common_talkwith_t *currTalk, int tag);
		protected:
			
			static void postConn(common_talkwith_t *currTalk, LcClientServer *server,
								 lc::LcEvent *lcEvent);
	};

	
	class LcClientHttpEvent : plclic HttpEvent{
		plclic:
			
			LcClientHttpEvent();
			
			virtual ~LcClientHttpEvent(){}

			
			virtual int activating_session();

			
			virtual void read_done_callback();

			
			virtual void write_done_callback();

			
			virtual void session_begin();

			
			virtual int session_done();

			
			virtual void http_read_header_done();

			
			virtual void http_read_section_done();
	
			
			virtual common_talkwith_t* getTalk(){return &_talk;}

			
			virtual void event_error_callback();
			
			
			virtual bool isError();

			
			virtual bsl::string status_to_string(unsigned int st);
		protected:
			common_talkwith_t _talk;		  
            cb_t _callback_backup;		  
            void *_callback_arg;		  
            int _backtag;		  
	};

}














#endif  


