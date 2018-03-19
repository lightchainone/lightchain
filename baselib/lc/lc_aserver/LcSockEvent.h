

#ifndef LCEVENT_H_
#define LCEVENT_H_


#include <lc/lcevent.h>
#include <lc/netreactor.h>
#include <lc_server.h>
#include <lc/lcthreadmem.h>
#include <Lsc/pool/Lsc_pool.h>

namespace lc {

class LcAServer;
class LcEvent;

static const int g_SMALLBUFSIZE = 256;

typedef struct _lc_timecount_t {		  
	timeval start;		  
	timeval count;		  
	timeval proccount;		  
	unsigned int proc;		  
	unsigned int write;		  
	unsigned int read;		  
	unsigned int connect;	  
	unsigned int total;		  
} lc_timecount_t;

typedef struct _lc_socket_t {		  
	LcEvent * event;		  
	IReactor * first_reactor;		  
	unsigned int max_buffer_size;		  
	unsigned int max_readbuf_size;		  
	unsigned int max_writebuf_size;		  
	char small_readbuf[g_SMALLBUFSIZE];	  
	char small_writebuf[g_SMALLBUFSIZE];		  
	char * read_buf;		  
	int read_timeout;		  
	unsigned int read_buf_len;     
	unsigned int read_buf_used;    
	char * write_buf;		  
	int write_timeout;		  
	unsigned int write_buf_len;     
	unsigned int write_buf_used;     

		int ext_readbuf; 
		int ext_writebuf; 


	
	int connect_type;		  
	int connect_timeout;   
	int process_timeout;   
	lc_timecount_t timer;		  
	struct sockaddr_in client_addr;		  
	unsigned int status;		  
	int sock_opt;	  
} lc_socket_t; 

class LcEvent : plclic SockEventBase {		  
plclic:
	typedef void (*_READDONE_CALLBACK_T)(LcEvent *);		  
protected :
	void * _ftag;  
	cb_t _fold_cb;		  
	void * _fold_cbp;		  
	int _fstart_ss; 
	int _fpre_result;
	int _fclass_type; 
	Lsc::mempool * _fmemory_manager; 
    unsigned int _flogid;
protected :
	lc_socket_t sock_data;		  
	Lsc::string _fnotice_string;		  
	_READDONE_CALLBACK_T _freaddone_callback;		  
	
	int judge_read_timeout(timeval &now);
	
	int judge_write_timeout(timeval &now);
plclic :
	enum {
		STATUS_READ = 0x0A,		      
		STATUS_WRITE,		          
		STATUS_ACCEPTED,		      
		STATUS_ERROR = 0x100,		  
		STATUS_TIMEOUT = 0x200,		  
		STATUS_CANCEL = 0x400,		  
		STATUS_CLOSESOCK = 0x800,	  
		STATUS_POSTERROR = 0x1000,	  
		STATUS_MEMERROR = 0x2000,	  
		STATUS_PRESESSIONERROR = 0x4000,  
		STATUS_USERDEF = 0x1000000		  
	};
	
	void backup_callback();
	
	void restore_callback();
	
	LcEvent();
	
	LcEvent(IReactor * first);
	
	~LcEvent();
	
	lc_socket_t * get_sock_data() {
		return &sock_data;
	}
	
	void release_sock_buf();
	
	const char * get_read_buffer(unsigned int size=0, int copy =0);
	
	int get_readbuf_size();
	
	char * get_write_buffer(unsigned int size=0);
	
	void set_writebuf_size(int size);
	
	int aread(int size);
	
	int awrite(int size=0);
	
	int trigle_read();
	
	int trigger_read();
	
	int trigger_write();

	
	virtual void read_done_callback()=0;

	
	virtual void write_done_callback()=0;
	
	virtual void event_error_callback();
	
	virtual int activating_session()=0;
	
	void set_readdone_callback(_READDONE_CALLBACK_T cb) {
		_freaddone_callback = cb;
	}
	
	virtual Lsc::string get_notice_string();
	
	void push_notice_string(const char * fmt, ...);
	
	void push_notice_pair(const char *key, const char *value);
	
	virtual unsigned int get_sock_status();
	
	virtual void set_sock_status(unsigned int st);
	
	virtual void session_begin();
	
	virtual int session_done()=0;
	
	int set_connecttype(int tp);
	
	int get_connecttype();
	
	void clear_notice_string();
	
	int get_read_timeout() {
		return sock_data.read_timeout;
	}
	
	int get_write_timeout() {
		return sock_data.write_timeout;
	}
	
	int set_read_timeout(int to);
	
	int set_write_timeout(int to);
	
	int repost();
	
	int rand_repost();
	
	int smart_repost();
	
	void set_firstreactor(IReactor *rct);
	
	void init(int read_to, int write_to, int connect_to, int proc_to, unsigned int max_bufsize);

	
	virtual Lsc::string status_to_string(unsigned int st);

	
	virtual void set_tag(void * t) {
		_ftag = t;
	}
	
	virtual void * get_tag(){
		return _ftag;
	}

	
	void  set_sock_opt(int opt){
		sock_data.sock_opt = opt;
	}

	
	void  active_sock_opt();
	
	void set_post_type(int ty);

	
	void set_read_maxbuf(unsigned int rdmax);
	
	void set_write_maxbuf(unsigned int wtmax);
	
	void set_readbuf(char * buf, unsigned int size);
	
	void set_writebuf(char * buf, unsigned int size);
	
	void use_inter_readbuf();
	
	void use_inter_writebuf();
	
	void release_read_buf();
	
	void release_write_buf();
	
	virtual int event_read();
	
	virtual int session_prebegin();
	
	int get_presession_result();
	
	virtual int if_post();

	int get_class_type() {
		return _fclass_type;
	}
	void set_class_type(int ty) {
		_fclass_type = ty;
	}
	
	Lsc::mempool * get_memmgr();
	
	void set_memmgr(Lsc::mempool *mgr);

    virtual void setHandle(int hd) {
        SockEvent::setHandle(hd);
        if (hd >= 0) {
            struct sockaddr_in addr;
            socklen_t addr_len = sizeof(addr);
            getpeername(hd, (struct sockaddr*)&addr, &addr_len);
            sock_data.client_addr = addr;
        }
    }

	
	void set_logid(int logid) {
        _flogid = logid;
    }

	
	int get_logid() {
        return _flogid;
    }

};


}


#endif 
