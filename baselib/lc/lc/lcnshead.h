


#ifndef  __LCNSHEAD_H_
#define  __LCNSHEAD_H_


#include "ievent.h"
#include "ireactor.h"
#include "lcevent.h"

namespace lc
{

	enum {
		NSHEAD_SUCCESS = 0,
		NSHEAD_PARAM_ERROR = -1,
		NSHEAD_UNKNOW_ERROR = -2,	    		
		NSHEAD_READ_HEAD_ERROR = -3,			
		NSHEAD_READ_BODY_ERROR = -4,			
		NSHEAD_READ_BODY_BUF_NO_ENOUGH = -5,	
		NSHEAD_WRITE_HEAD_ERROR = -6,			
		NSHEAD_POST_READ_ERROR = -7,			
		NSHEAD_TCP_CONNECT_ERROR = -8,   		

		NSHEAD_WRITE_BODY_ERROR = -9,
		NSHEAD_POST_READBODY_ERROR = -10,
		NSHEAD_POST_WRITEBODY_ERROR = -11,

		NSHEAD_LC_INIT_ERROR = -12,
		NSHEAD_LC_FETCH_SERVER_ERROR = -13,
		NSHEAD_LC_OTHER_ERROR = -14,

		LC_WAIT_NO_MSG = -15,
	
		NSHEAD_LC_IN_PROCESSING = -16,
		NSHEAD_LC_USER_CALLBACK_ERROR = -17,
		LC_NETREACTOR_POST_ERROR = -18,
		LC_NETREACTOR_RUN_ERROR = -19,
		LC_REACTOR_NOTNULL = -20,
		LC_ASYNCLIST_FULL = -21,
		LC_TALK_RANGE_ERROR = -22,
		LC_RPC_CLOSE_CONN = -23,
		LC_CONN_FD_ERROR = -24,
		LC_ERROR_TYPE = -25,
	};


	class IReactor;
	struct nshead_asio_t
	{
		nshead_t *head;	
		int len;	
		int err;	
		IEvent::cb_t cb;	
		void *param;		
	};

	
	void set_nshead_asio(nshead_asio_t *t, nshead_t *head, 
			int len, IEvent::cb_t cb, void *p);

	
	int nshead_aread(SockEvent *e, nshead_asio_t *a);
	
	int nshead_awrite(SockEvent *e, nshead_asio_t *a);

	
	typedef struct _lc_callback_alloc_t {
    		u_int size;
    		void *arg;
    		void *buff;
	}lc_callback_alloc_t;
	typedef int (*lc_client_callbackalloc)(lc_callback_alloc_t *arg);
	typedef int (*lc_client_callbackfree)(lc_callback_alloc_t *arg);
	

	
	struct nshead_atalk_t
	{
		nshead_t *req;	
		nshead_t *res;	
		int len;	
		int err;	
		IEvent::cb_t cb;	
		void *param;		

		
		lc_client_callbackalloc alloccallback;
    	lc_client_callbackfree freecallback;
    	void *allocarg;

		int readtimeout;
		int writetimeout;

		char *reqbuf;
		char *resbuf;

		long long startTime;
		int realReadTime;
		int realWriteTime;

		

		
		nshead_asio_t tmp;
	};
	
	void set_nshead_atalk(nshead_atalk_t *a, nshead_t *req, nshead_t *res,
			int len, IEvent::cb_t cb, void *param);

	
	int nshead_atalkwith(SockEvent *e, nshead_atalk_t *a);

	
	int nshead_atalkwith1(SockEvent *e, nshead_atalk_t *a);

	
	struct nshead_atalk_tcp_t : plclic nshead_atalk_t
	{
		char ip[36];
		int port;
		bool nodelay;
	};

	
	void set_nshead_atalk_tcp_ip(nshead_atalk_tcp_t *a, const char *ip, int port, bool nodelay=true);
	
	int nshead_atalkwith_ip(SockEvent *e, nshead_atalk_tcp_t *a);
};

#endif  


