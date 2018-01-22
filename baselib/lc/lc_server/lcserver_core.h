#ifndef _LCSERVER_CORE_H
#define _LCSERVER_CORE_H

#include <pthread.h>
#include <ul_conf.h>
#include <ul_log.h>
#include <lc_auth.h>
#include "lc_server.h"


struct apool_socket;


typedef struct _lc_server_pdata_t
{
	pthread_t pid;	
	u_int id;	
	in_addr_t ip;			
	int fd;			

	void *read_buf;		
	u_int read_siz;	
	void *write_buf;	
	u_int write_siz;	
	void *user_buf;		
	u_int user_siz;		
	apool_socket * apoolsocket; 
	void *parent;	

	int self_type;		  

    long long sock_num_dec;
    iovec *iov;
    size_t iov_count;

	int   epfd;    
}lc_server_pdata_t;		  


struct apool_t;
class AclManager;
class GalileoRegister;

struct _lc_server_t
{
	u_int err_no;		
	u_int server_type;	
	char  server_name[WORD_SIZE]; 
	u_int connect_type;	

	
	int connect_to;	
	int read_to;		
	int write_to;		

	int pthread_num;			
	int real_thread_num;	
	int need_join;	

	u_int listen_port;	
	u_int backlog;		
	int run;					
	int sev_sock;				

	
	void *pool;					
	int pool_data[256];			

	
	lc_server_pdata_t *data;		
	void **user_rdbuf;			
	void **user_wtbuf;			
	u_int read_siz;		
	u_int write_siz;		
	void **usersef_buf;	
	u_int usersef_siz;	

	
	callback_proc cb_native;			
	callback_proc cb_drastic;		
	callback_check cb_check;			
    callback_writedone cb_writedone;
	int (*io_callback)(apool_t * pool, int idx, uint32_t events);   
	int (*proc_callback)(apool_t *pool, lc_server_pdata_t *data);  

	
    lc_auth_t *ip_auth_0;         
    lc_auth_t *ip_auth_1;         
    size_t ip_auth_tag;
	int ip_auth_cry;		      

	AclManager      *acl_manager;
	GalileoRegister *lleo_register;
	
	int socktype;		          

	
	lc_handle_fun_t p_start;      
	lc_handle_fun_t p_end;		  

	
	int log_print; 

    lc::LcMonitor *monitor;

    u_int stack_size;

    u_int using_threadsche;
    int listen_prio;
    int worker_prio;
};

int lc_server_bind(lc_server_t *);

#endif


