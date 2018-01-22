

#ifndef _LC_SERVER_H
#define _LC_SERVER_H

#include <ul_log.h>
#include <lc_log.h>
#include <ul_net.h>
#include <lc_conf.h>
#include <lc_auth.h>
#include <ul_def.h>
#include <sys/uio.h>

namespace lc {
    class LcMonitor;
}



enum {
	LCSVR_SHORT_CONNECT = 0,	
	LCSVR_LONG_CONNECT		  
};



enum {
	LCSVR_LINGER_CLOSE = 0x1,		  
	LCSVR_NODELAY = 0x2,		  
    LCSVR_DEFER_ACCEPT = 0x4,
};



enum {
	LCSVR_XPOOL = 0,	
	LCSVR_CPOOL,		
	LCSVR_EPPOOL,		
	LCSVR_APPOOL,		
	LCSVR_POOL_NUM,	
};

typedef struct _lc_server_t lc_server_t;

typedef int (* callback_proc)();
typedef int (* callback_check)(void *req, int reqsiz);
typedef int (* callback_writedone)(int write_result);


lc_server_t *lc_server_create(const char *svrname = NULL);


int lc_server_load(lc_server_t *sev, lc_svr_t *svrconf);


int lc_server_load_ex(lc_server_t *sev, lc_svr_t *svrconf);


int lc_server_run(lc_server_t *);


int lc_server_run_test(lc_server_t *);

typedef int	(*lc_handle_fun_t)();

int lc_server_set_threadstartfun(lc_server_t *sev, lc_handle_fun_t op);


int lc_server_set_threadstopfun(lc_server_t *sev, lc_handle_fun_t op);


int lc_server_stop(lc_server_t *);


int lc_server_join(lc_server_t *);



int lc_server_destroy(lc_server_t *);




int lc_server_set_ip_auth(lc_server_t *sev, lc_auth_t *auth);


lc_auth_t *lc_server_get_ip_auth(lc_server_t *sev);


int lc_server_set_listen_port(lc_server_t *sev, int port);


int lc_server_set_listen_fd(lc_server_t *sev, int listenfd);


int lc_server_set_native_callback(lc_server_t *sev, callback_proc cb);


int lc_server_set_callback(lc_server_t *sev, callback_proc cb_deal);


int lc_server_set_callback_writev(lc_server_t *sev, callback_proc cb_deal);


int lc_server_set_callback_writev(lc_server_t *sev, callback_proc cb_deal, callback_writedone cb_write);


int lc_server_set_callback(lc_server_t *sev, callback_proc cb_deal, callback_check cb_check);


int lc_server_set_callback_writev(lc_server_t *sev, callback_proc cb_deal, callback_check cb_check);


int lc_server_set_callback_writev(lc_server_t *sev, callback_proc cb_deal, callback_check cb_check, callback_writedone cb_write);


int lc_server_set_buffer(lc_server_t *sev, void **rdbuf, int read_siz, void **wtbuf, int write_siz);


int lc_server_set_buffer(lc_server_t *sev, int threadnum, void **rdbuf, int read_siz, void **wtbuf, int write_siz);


int lc_server_set_user_data(lc_server_t *sev, void **usersefbuf, u_int usersefsiz);


int lc_server_set_user_data(lc_server_t *sev, int threadnum, void **, u_int);


int lc_server_set_pthread_num(lc_server_t *sev, int pnum);


int lc_server_set_timeout(lc_server_t *sev, int connect_timeout, int read_timeout, int write_timeout);


int lc_server_setoptsock(lc_server_t *sev, int type);



int lc_server_set_server_type(lc_server_t *sev, int type);


int lc_server_set_server_name(lc_server_t *sev, const char *name);


const char *lc_server_get_server_name(lc_server_t *sev);


int lc_server_set_connect_type(lc_server_t *sev, int type);




int lc_server_get_listen_port(lc_server_t *sev);


int lc_server_get_socket();


void *lc_server_get_read_buf();


void *lc_server_get_user_data();


u_int lc_server_get_read_size();


u_int lc_server_get_userdata_size();


void *lc_server_get_write_buf(size_t size = 0);


u_int lc_server_get_write_size();


int lc_server_get_connect_timeout();


int lc_server_get_read_timeout();


int lc_server_get_write_timeout();


in_addr_t lc_server_get_ip();




int lc_server_set_cpool_queue_size(lc_server_t *sev, int siz);


int lc_server_get_cpool_queue_size(lc_server_t *sev);


int lc_server_set_cpool_socket_size(lc_server_t *sev, int siz);


int lc_server_get_cpool_socket_size(lc_server_t *sev);


int lc_server_set_cpool_timeout(lc_server_t *sev, int timeout);

int lc_server_get_cpool_timeout(lc_server_t *sev);


int lc_server_get_threadnum(lc_server_t *sev);



lc_server_t *lc_server_get_server();



void lc_server_set_session_connect_type(int cotp);


int lc_server_get_session_connect_type(lc_server_t *sev);


int lc_server_get_logprint(lc_server_t *sev);

int lc_server_set_logprint(lc_server_t *sev, int logprint);


long long lc_server_get_usingqueue_size(lc_server_t *sev);


long long lc_server_get_usingsocket_size(lc_server_t *sev);


int lc_server_get_thread_pool_usage(lc_server_t *sev);


struct iovec **lc_server_get_write_iovec();


size_t *lc_server_get_write_iovec_count();

int lc_server_get_listen_priority(lc_server_t *sev);

int lc_server_get_worker_priority(lc_server_t *sev);

bool lc_server_threadsched_used(lc_server_t *sev);
#endif


