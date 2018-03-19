

#ifndef LcAServer_H_
#define LcAServer_H_

#include <lc/netreactor.h>
#include <lc/lcatomic.h>
#include "SvrEvent.h"
#include "lock_iqueue.h"
#include <lc_auth.h>
#include <lc/lcthreadmem.h>
#include "LcMonitor.h"

namespace lc {

class LcEvent;
class SvrEvent;

class LcAServer {		  
protected :
    NetReactor * _accept_act;
	NetReactor * _fnet_act_list;		  
	lc_auth_t * _ip_auth;		  
	int _fconnect_type;		  
	int _fport;		  
	int _fread_timeout;		  
	int _fwrite_timeout;		  
	int _fconnect_timeout;		  
	int _fsession_timeout;		  
	unsigned int _fmax_buf_size;		  
	unsigned int _fmax_readbuf_size;		  
	unsigned int _fmax_writebuf_size;		  
	unsigned int _fmax_sessionlimit;		  
	unsigned int _fbacklog;		  
	unsigned int _fmemory_type;  
	unsigned int _fmemory_blksize; 
	Lsc::string _svr_name;		  
	SvrEvent * _facc_event;		  
	int _fsock_opt;  
	int _facc_fd;  
	LockIQueue _facc_queue; 
	AtomicInt _fsession_count;
	Lsc::mempool * _fmemory_manager; 
	
	virtual void session_begin(LcEvent * event);

    LcMonitor *_monitor;
    Lsc::var::Ref _monitor_info;
    Lsc::ResourcePool _monitor_rp;
    int _monitor_threadnum;
plclic:
	
	int load(const comcfg :: ConfigUnit & cfg);
	
	int set_connect_type(int type);
	
	int set_port(int port);
	
	NetReactor * get_reactor();
	
	virtual void on_accept(LcEvent * event)=0;
	
	virtual int session_done(LcEvent * session);

	
	int get_read_timeout(){
		return _fread_timeout;
	}
	
	int get_write_timeout(){
		return _fwrite_timeout;
	}
	
	int get_connect_timeout(){
		return _fconnect_timeout;
	}
	
	unsigned int get_max_bufsize() {
		return _fmax_buf_size;
	}
	
	void set_max_bufsize(unsigned int mbsize) {
		_fmax_buf_size = mbsize;
	}
	
	lc_auth_t * get_ipauth() {
		return _ip_auth;
	}

	
	const Lsc::string & get_svrname();
	
	virtual int getready();
	
	virtual int getready(int listenfd);
	
	virtual int stop();
	
	int restart();
	
	LcAServer(NetReactor * reactor) : _accept_act(reactor), _fnet_act_list(reactor),_ip_auth(NULL),_fbacklog(2048), _facc_event(NULL), _fsock_opt(0), _facc_fd(-1), _monitor(NULL), _monitor_threadnum(0){
        _monitor_info = Lsc::var::Null::null;
	}

    
    LcAServer(NetReactor * reactor, NetReactor * acc_reactor) : _accept_act(acc_reactor), _fnet_act_list(reactor),_ip_auth(NULL),_fbacklog(2048), _facc_event(NULL), _fsock_opt(0), _facc_fd(-1), _monitor(NULL), _monitor_threadnum(0){
        _monitor_info = Lsc::var::Null::null;
	}

	
	virtual ~LcAServer();
	
	void  set_sock_opt(int opt){
		_fsock_opt = opt;
	}
	
	void accept_porcess(SvrEvent *se);
	
	unsigned int get_max_writebufsize();
	
	unsigned int get_max_readbufsize();
	
	int is_running();
	
	Lsc::mempool * get_memmgr();

    Lsc::var::IVar &get_monitor_info() {
        return _monitor_info;
    }

    int set_monitor(LcMonitor *monitor, int thread_num) {
        if (NULL == monitor || thread_num < 0) {
            return -1;
        }
        _monitor = monitor;
        _monitor_threadnum = thread_num;
        return 0;
    }

    LcMonitor *get_monitor() {
        return _monitor;
    }

    int set_monitor_request_num_success();

    int set_monitor_request_num_fail();

    int set_monitor_request_time(int req_time);

    int register_monitor_callback();

    static Lsc::var::IVar &lc_aserver_monitor_get_thread_pool_usage(Lsc::var::IVar &query,
                                                                    Lsc::ResourcePool &rp);

    static Lsc::var::IVar &lc_aserver_monitor_get_queue_size(Lsc::var::IVar &query,
                                                             Lsc::ResourcePool &rp);

    void gen_monitor_info();
};


}

#endif 
