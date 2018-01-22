#include <stdlib.h>
#include <string.h>
#include <ul_net.h>
#include <lc_auth.h>
#include <Lsc/var/String.h>

#include "nshead.h"
#include "lcserver_register.h"
#include "pool.h"
#include "lcserver_app.h"
#include "lcserver_core.h"
#include "LcMonitor.h"
#include "lc_server_monitor.h"

#include "acl_manager.h"
#include "lleo_register.h"

#define LCSVR_CONF_FLAG "lcserver"

static int lc_server_default_callback()
{
	nshead_t *shead = (nshead_t *)lc_server_get_write_buf();
	shead->body_len = 0;
	return 0;
}

lc_server_t *lc_server_create(const char *sevname)
{
	lc_server_t *sev = (lc_server_t *)malloc(sizeof(lc_server_t));
	if (sev == NULL) return NULL;
	sev->err_no = 0;
	sev->server_type = LCSVR_XPOOL;
	sev->connect_type = LCSVR_SHORT_CONNECT;

	sev->connect_to = 100;
	sev->read_to = 500;
	sev->write_to = 500;

	sev->pthread_num = 0;
	sev->real_thread_num = -1;
	sev->need_join = 0;

	sev->listen_port = 0;
	sev->backlog = 2048;
	sev->read_siz = 0;
	sev->write_siz = 0;
	sev->usersef_siz = 0;

	sev->run = 1;
	sev->sev_sock = -1;

	memset(sev->pool_data, 0, sizeof(sev->pool_data));
	sev->pool = NULL;

	sev->data = NULL;
	sev->user_rdbuf = NULL;
	sev->user_wtbuf = NULL;
	sev->usersef_buf = NULL;
	sev->io_callback = NULL;
	sev->proc_callback = NULL;
	sev->cb_native = NULL;
	sev->cb_drastic = NULL;
	sev->cb_check = NULL;
	sev->ip_auth_0 = 0;
	sev->ip_auth_1 = 0;
	sev->ip_auth_tag = 0;
	sev->ip_auth_cry = 0;
	sev->acl_manager = NULL;
	sev->lleo_register = NULL;
	sev->socktype = 0;

	if (sevname == NULL) {
		lc_server_set_server_name(sev, "default_lcserver_name");
	} else {
		lc_server_set_server_name(sev, sevname);
	}

	sev->p_start = NULL;
	sev->p_end = NULL;

	sev->log_print = 1;
    sev->monitor = NULL;
    
    sev->stack_size = 0;
    sev->using_threadsche = 0;
    sev->listen_prio = 10;
    sev->worker_prio = 5;

	return sev;
}

int lc_server_load(lc_server_t *sev, lc_svr_t *svrconf)
{
	
	
	lc_server_set_server_name(sev, svrconf->svr_name);
	lc_server_set_listen_port(sev, svrconf->port);
	
	lc_server_set_timeout(sev, 1000, svrconf->read_timeout, svrconf->write_timeout);
	
	if (svrconf->connect_type == 0) { 
		lc_server_set_connect_type(sev, LCSVR_SHORT_CONNECT);
		
		lc_server_set_server_type(sev, LCSVR_XPOOL);
	} else { 
		lc_server_set_connect_type(sev, LCSVR_LONG_CONNECT);
		
		lc_server_set_server_type(sev, LCSVR_CPOOL);
	}
	
	lc_server_set_server_type(sev, svrconf->server_type);

	
	lc_server_set_pthread_num(sev, svrconf->thread_num);

	
		lc_server_set_cpool_queue_size(sev, svrconf->queue_size);
		lc_server_set_cpool_socket_size(sev, svrconf->sock_size);
	
	

	return 0;
}

int lc_server_load_ex(lc_server_t *sev, lc_svr_t *svrconf)
{
	if (sev == NULL) return -1;
	
	
	lc_server_set_server_name(sev, svrconf->svr_name);
	lc_server_set_listen_port(sev, svrconf->port);
	
	lc_server_set_timeout(sev, svrconf->connect_timeout, svrconf->read_timeout, svrconf->write_timeout);
    lc_server_set_cpool_timeout(sev, (int)svrconf->keepalive_timeout);
	
	if (svrconf->connect_type == 0) { 
		lc_server_set_connect_type(sev, LCSVR_SHORT_CONNECT);
		
		lc_server_set_server_type(sev, LCSVR_XPOOL);
	} else { 
		lc_server_set_connect_type(sev, LCSVR_LONG_CONNECT);
		
		lc_server_set_server_type(sev, LCSVR_CPOOL);
	}
	
	lc_server_set_server_type(sev, svrconf->server_type);

	
	lc_server_set_pthread_num(sev, svrconf->thread_num);

	lc_server_set_cpool_queue_size(sev, svrconf->queue_size);
	lc_server_set_cpool_socket_size(sev, svrconf->sock_size);
	
	
	lc_server_set_native_callback(sev, lc_server_default_callback);

	lc_server_set_buffer(sev, NULL, svrconf->read_buf_size, 
			NULL, svrconf->write_buf_size);
	if (svrconf->user_buf_size > 0) {
		lc_server_set_user_data(sev, NULL, svrconf->user_buf_size);
	}

	sev->acl_manager = new AclManager(sev);
	sev->acl_manager->handle_acl(svrconf);

	if(svrconf->lleo_register_enable) {
		if(svrconf->lleo_svrlist && svrconf->lleo_path && svrconf->lleo_register_resdata) {
			sev->lleo_register = new GalileoRegister(svrconf->lleo_svrlist, svrconf->lleo_path, svrconf->lleo_register_resdata);
		}
	}

    sev->stack_size = svrconf->stack_size;
    u_int perm_statck_size = 2097152; 
    u_int def_stack_size = 10485760;  
    if ((sev->stack_size < perm_statck_size) && (sev->stack_size > (u_int)0)) {
        LC_LOG_WARNING("stack size : %u small %u, using %u", sev->stack_size, perm_statck_size, def_stack_size);
        sev->stack_size = def_stack_size;
    }

    sev->using_threadsche = svrconf->using_threadsche;
    sev->listen_prio = svrconf->listen_prio;
    sev->worker_prio = svrconf->worker_prio;

	return 0;
}

#if 0
int lc_server_conf_build(lc_server_t *sev, const char *path, const char *file)
{
	lc_conf_data_t *conf = lc_conf_init(path, file, 1);
	if (conf == NULL) {
		LC_LOG_FATAL("can't create configure to [%s/%s]", path, file);
		return -1;
	}
	lc_svr_t svrconf;
	
	svrconf.num = 0;
	svrconf.port = sev->listen_port;
	svrconf.read_timeout = sev->read_to;
	svrconf.write_timeout = sev->write_to;
	svrconf.max_connect = 0;
	svrconf.thread_num = sev->pthread_num;
	svrconf.connect_type = sev->connect_type;

	int ret = lc_conf_getsvr(conf, LCSVR_CONF_FLAG, &svrconf, "configure web server module");
	if (ret != LC_CONF_SUCCESS) {
		LC_LOG_WARNING("can't create lcserver configure[%s] err[%d]", LCSVR_CONF_FLAG, ret);
		goto fail;
	}
	lc_conf_close(conf);
	return 0;
fail:
	lc_conf_close(conf);
	return -1;
}
#endif

int lc_server_destroy(lc_server_t *sev)
{
	lc_server_stop(sev);
	lc_server_join(sev);

	int i = 0;

	if (sev->sev_sock >= 0)
		close(sev->sev_sock);
	g_pool[sev->server_type].destroy(sev);

	if (sev->user_rdbuf) {
		free (sev->user_rdbuf);
		sev->user_rdbuf = NULL;
	}
	
	if (sev->user_wtbuf) {
		free (sev->user_wtbuf);
		sev->user_wtbuf = NULL;
	}

	
	bool userbuf_flag = false;
	if (sev->usersef_buf) {
		userbuf_flag = true;
		free (sev->usersef_buf);
		sev->usersef_buf = NULL;
	}

	
	if (sev->data != 0) {
		
		for (i=0; i<sev->pthread_num; ++i) {
			if (sev->data[i].read_buf != NULL)
				free(sev->data[i].read_buf);
		}
		
		for (i=0; i<sev->pthread_num; ++i) {
			if (sev->data[i].write_buf != NULL)
				free(sev->data[i].write_buf);
		}
	
		if(!userbuf_flag) {
			
			for (i=0; i<sev->pthread_num; ++i) {
				if (sev->data[i].user_buf != NULL) {
					free(sev->data[i].user_buf);
				}
			}
		}

		free(sev->data);
	}

	if (sev->ip_auth_0 != NULL && sev->ip_auth_cry != 0) {
		lc_auth_destroy(sev->ip_auth_0);
		sev->ip_auth_0 = NULL;
		sev->ip_auth_cry = 0;
	}
	if(sev->acl_manager != NULL) {
		delete sev->acl_manager;
		sev->acl_manager = NULL;
	}
	if(sev->lleo_register != NULL) {
		delete sev->lleo_register;
		sev->lleo_register = NULL;
	}

	free(sev);
	return 0;
}

int lc_server_run(lc_server_t *sev)
{
	int ret;
	
	
	if(sev->lleo_register) {	
		ret = sev->lleo_register->register_resource();
		if(ret != 0)
			return -1;
	}

	ret = lc_server_bind(sev);
	if (ret != 0) return -1;
	sev->need_join = 1;
	return g_pool[sev->server_type].run(sev); 
} 

int lc_server_run_test(lc_server_t *sev)
{
	int i=0, ret=0;

	if (sev->pthread_num <= 0) {
		LC_LOG_WARNING("pthread_num[%d] is invalid", sev->pthread_num);
		return -1;
	}
	sev->data = (lc_server_pdata_t *)malloc(sizeof(lc_server_pdata_t) * sev->pthread_num);
	if (sev->data == NULL) {
		LC_LOG_WARNING("can't malloc pthread data memsiz[%d] for lc_server", (int)sizeof(lc_server_pdata_t) * sev->pthread_num);
		return -1;
	}

	memset(sev->data, 0, sizeof(lc_server_pdata_t) * sev->pthread_num);
	for (i=0; i<sev->pthread_num; ++i) {
		if (sev->read_siz > 0) {	
			sev->data[i].read_siz = sev->read_siz;

			if (sev->user_rdbuf == NULL) {	
				sev->data[i].read_buf = malloc(sev->data[i].read_siz);
				if (sev->data[i].read_buf == NULL) {
					LC_LOG_WARNING("can't malloc read buffer[%d] for lc_lcserver", sev->data[i].read_siz);
					return -1;
				}
				memset(sev->data[i].read_buf, 0, sev->data[i].read_siz);
			} else {
				
				sev->data[i].read_buf = sev->user_rdbuf[i];
			}
		}
		if (sev->write_siz > 0) {	
			sev->data[i].write_siz = sev->write_siz;
			
			if (sev->user_wtbuf == NULL) {
				sev->data[i].write_buf = malloc(sev->data[i].write_siz);
				if (sev->data[i].write_buf == NULL) {
					LC_LOG_WARNING("can't malloc write buffer[%d] for lc_server", sev->data[i].write_siz);
					return -1;
				}
				memset(sev->data[i].write_buf, 0, sev->data[i].write_siz);
			} else {
				
				sev->data[i].write_buf = sev->user_wtbuf[i];
			}
		}
		
		sev->data[i].parent = sev;
	}

	
	for (i=0; i<sev->pthread_num; ++i) {
		if (sev->usersef_buf == NULL) {
			if (sev->usersef_siz > 0) {
				sev->data[i].user_buf = malloc(sev->usersef_siz);
				if (sev->data[i].user_buf == NULL) {
					LC_LOG_WARNING("can't malloc for userdata[%d]", sev->usersef_siz);
					return -1;
				}
				memset(sev->data[i].user_buf, 0, sev->usersef_siz);
				sev->data[i].user_siz = sev->usersef_siz;
			} 
		} else {
			sev->data[i].user_buf = sev->usersef_buf[i];
			sev->data[i].user_siz = sev->usersef_siz;
		}
	}
	
	ret = g_pool[sev->server_type].init(sev);
	if (ret != 0) {
		LC_LOG_WARNING("init pool err");
		return -1;
	}

    
    set_pthread_data(&sev->data[0]);

    return 0;
}

int lc_server_stop(lc_server_t *sev)
{
	sev->run = 0;
	return 0;
}

int lc_server_join(lc_server_t *sev) { 
	if (sev == NULL) {
		return -1;
	}
	if (!sev->need_join) return 0;
	sev->need_join = 0;
	return g_pool[sev->server_type].join(sev);
}

int lc_server_bind(lc_server_t *sev)
{
	int i=0, ret=0;

	if (sev->pthread_num <= 0) {
		LC_LOG_WARNING("pthread_num[%d] is invalid", sev->pthread_num);
		return -1;
	}
	sev->data = (lc_server_pdata_t *)malloc(sizeof(lc_server_pdata_t) * sev->pthread_num);
	if (sev->data == NULL) {
		LC_LOG_WARNING("can't malloc pthread data memsiz[%d] for lc_server", (int)sizeof(lc_server_pdata_t) * sev->pthread_num);
		return -1;
	}

	memset(sev->data, 0, sizeof(lc_server_pdata_t) * sev->pthread_num);
	for (i=0; i<sev->pthread_num; ++i) {
		if (sev->read_siz > 0) {	
			sev->data[i].read_siz = sev->read_siz;

			if (sev->user_rdbuf == NULL) {	
				sev->data[i].read_buf = malloc(sev->data[i].read_siz);
				if (sev->data[i].read_buf == NULL) {
					LC_LOG_WARNING("can't malloc read buffer[%d] for lc_lcserver", sev->data[i].read_siz);
					return -1;
				}
				memset(sev->data[i].read_buf, 0, sev->data[i].read_siz);
			} else {
				
				sev->data[i].read_buf = sev->user_rdbuf[i];
			}
		}
		if (sev->write_siz > 0) {	
			sev->data[i].write_siz = sev->write_siz;
			
			if (sev->user_wtbuf == NULL) {
				sev->data[i].write_buf = malloc(sev->data[i].write_siz);
				if (sev->data[i].write_buf == NULL) {
					LC_LOG_WARNING("can't malloc write buffer[%d] for lc_server", sev->data[i].write_siz);
					return -1;
				}
				memset(sev->data[i].write_buf, 0, sev->data[i].write_siz);
			} else {
				
				sev->data[i].write_buf = sev->user_wtbuf[i];
			}
		}
		
		sev->data[i].parent = sev;
        sev->data[i].sock_num_dec = 0;
	}

	
	for (i=0; i<sev->pthread_num; ++i) {
		if (sev->usersef_buf == NULL) {
			
			if (sev->usersef_siz > 0) {
				
				sev->data[i].user_buf = malloc(sev->usersef_siz);
				if (sev->data[i].user_buf == NULL) {
					LC_LOG_WARNING("can't malloc for userdata[%d]", sev->usersef_siz);
					return -1;
				}
				memset(sev->data[i].user_buf, 0, sev->usersef_siz);
				sev->data[i].user_siz = sev->usersef_siz;
			} 
		} else {
			sev->data[i].user_buf = sev->usersef_buf[i];
			sev->data[i].user_siz = sev->usersef_siz;
		}
	}
	
	ret = g_pool[sev->server_type].init(sev);
	if (ret != 0) {
		LC_LOG_WARNING("init pool err");
		return -1;
	}

	if (0 > sev->sev_sock) {
		sev->sev_sock = ul_tcplisten(sev->listen_port, sev->backlog);
	}
	if (sev->sev_sock < 0) {
		LC_LOG_WARNING("can't start tcplisten at port[%d] backlog[%d] err[%m]", sev->listen_port, sev->backlog);
		return -1;
	}
	lc_setsockopt(sev->sev_sock, sev);
	lc_setsockopt_ex(sev->sev_sock, sev);
	return 0;
}

int lc_server_set_native_callback(lc_server_t *sev, callback_proc cb)
{
	if (sev == NULL) return -1;
	sev->cb_native = cb;
	return 0;
}

int lc_server_set_callback(lc_server_t *sev, callback_proc cb_deal)
{
	if (sev == NULL) return -1;
	sev->cb_native = default_native;
	sev->cb_drastic = cb_deal;
	return 0;
}

int lc_server_set_callback_writev(lc_server_t *sev, callback_proc cb_deal)
{
    if (sev == NULL) return -1;
    sev->cb_native = writev_default_native;
    sev->cb_drastic = cb_deal;
    return 0;
}

int lc_server_set_callback_writev(lc_server_t *sev, callback_proc cb_deal, callback_writedone cb_write)
{
    if (sev == NULL) return -1;
    sev->cb_native = writev_default_native;
    sev->cb_drastic = cb_deal;
    sev->cb_writedone = cb_write;
    return 0;
}

int lc_server_set_callback(lc_server_t *sev, callback_proc cb_deal, callback_check cb_check)
{
	if (sev == NULL) return -1;
	sev->cb_check = cb_check;
	return lc_server_set_callback(sev, cb_deal);
}

int lc_server_set_callback_writev(lc_server_t *sev, callback_proc cb_deal, callback_check cb_check)
{
    if (sev == NULL) return -1;
    sev->cb_check = cb_check;
    return lc_server_set_callback_writev(sev, cb_deal);
}

int lc_server_set_callback_writev(lc_server_t *sev, callback_proc cb_deal, callback_check cb_check, callback_writedone cb_write)
{
    if (sev == NULL) return -1;
    sev->cb_check = cb_check;
    return lc_server_set_callback_writev(sev, cb_deal, cb_write);

}

#define copy_voidpp(to, src, num) \
do { \
	if ((num) == 0) {	\
		LC_LOG_FATAL("invalid src size[0]"); \
		return -1; \
	} \
	if (to != NULL) { \
		free (to); \
		to = NULL; \
	} \
	if (src == NULL) break; \
	to = (void **) malloc ((num) * sizeof(void *)); \
	if (to == NULL) { \
		LC_LOG_FATAL("copy voidpp err"); \
		return -1; \
	} \
	for (int i=0; i<num; ++i) { \
		to[i] = src[i]; \
	} \
} while (0)

int lc_server_set_buffer(lc_server_t *sev, void **rdbuf, int read_siz, void **wtbuf, int write_siz)
{	
	if (sev == NULL) return -1;
	if (sev->pthread_num <= 0) {
		LC_LOG_FATAL("set thread num first");
		return -1;
	}

	sev->read_siz = read_siz;
	sev->write_siz = write_siz;

	copy_voidpp(sev->user_rdbuf, rdbuf, sev->pthread_num);
	copy_voidpp(sev->user_wtbuf, wtbuf, sev->pthread_num);

	return 0;
}

int lc_server_set_buffer(lc_server_t *sev, int threadnum, void **rdbuf, int read_siz, void **wtbuf, int write_siz)
{
	if (sev == NULL) return -1;
	if (threadnum > 0) {
		sev->pthread_num = threadnum;
	}
	return lc_server_set_buffer(sev, rdbuf, read_siz, wtbuf, write_siz);
}

int lc_server_set_user_data(lc_server_t *sev, void **usersefbuf, u_int usersefsiz)
{
	if (sev == NULL) return -1;
	if (sev->pthread_num <= 0) {
		LC_LOG_FATAL("set thread num first");
		return -1;
	}

	sev->usersef_siz = usersefsiz;
	copy_voidpp(sev->usersef_buf, usersefbuf, sev->pthread_num);

	return 0;
}

int lc_server_set_user_data(lc_server_t *sev, int threadnum, void **buf, u_int siz)
{
	if (sev == NULL) return -1;
	if (threadnum > 0) {
		sev->pthread_num = threadnum;
	}
	return lc_server_set_user_data(sev, buf, siz);
}

int lc_server_set_pthread_num(lc_server_t *sev, int pnum)
{
	if (sev == NULL) return -1;
	if (pnum <= 0) {
		LC_LOG_WARNING("you set threadnum zero");
		return -1;
	}
	if (pnum == sev->pthread_num) {
		return 0;
	}
	sev->pthread_num = pnum;
	return 0;
}

int lc_server_set_timeout(lc_server_t *sev, int connect_timeout, int read_timeout, int write_timeout)
{
	if (sev == NULL) return -1;
	sev->connect_to = connect_timeout;
	sev->read_to = read_timeout;
	sev->write_to = write_timeout;

	return 0;
}

int lc_server_set_listen_port(lc_server_t *sev, int port)
{
	if (sev == NULL) return -1;
	sev->listen_port = port;
	return 0;
}
int lc_server_set_listen_fd(lc_server_t *sev, int listenfd) {
	if (sev == NULL) {
		return -1;
	}
	sev->sev_sock = listenfd;
	return 0;
}

int lc_server_set_server_type(lc_server_t *sev, int type)
{
	if (sev == NULL) return -1;
	sev->server_type = type;
	return 0;
}

int lc_server_set_server_name(lc_server_t *sev, const char *name)
{
	if (sev == NULL) return -1;
	strncpy(sev->server_name, name, sizeof(sev->server_name));
	sev->server_name[sizeof(sev->server_name) - 1] = '\0';
	return 0;
}

const char *lc_server_get_server_name(lc_server_t *sev)
{
    if (sev == NULL) return "";
    return sev->server_name;
}

int lc_server_set_connect_type(lc_server_t *sev, int type)
{
	if (sev == NULL) return -1;
	sev->connect_type = type;
	return 0;
}

int lc_server_get_listen_port(lc_server_t *sev)
{
	if (sev == NULL) return -1;
	return sev->listen_port;
}

int lc_server_get_threadnum(lc_server_t *sev){
	if (sev == NULL) return -1;
	return sev->pthread_num;
}

int lc_server_set_ip_auth(lc_server_t *sev, lc_auth_t *auth)
{
    if (sev == NULL) return -1;
    if (sev->ip_auth_tag % 2 == 0) {
        sev->ip_auth_0 = auth;
    } else {
        sev->ip_auth_1 = auth;
    }
    ++ sev->ip_auth_tag;
    return 0;
}

lc_auth_t *lc_server_get_ip_auth(lc_server_t *sev)
{
    if (NULL == sev) return NULL;
    lc_auth_t *auth = NULL;
	if (sev->ip_auth_tag % 2 == 0) {
		auth = sev->ip_auth_1;
    } else {
		auth = sev->ip_auth_0;
    }
	if(auth) {
		lc_auth_addref(auth);
	}
    return auth;
}

int lc_server_setoptsock(lc_server_t *sev, int type)
{
	if (sev == NULL) return -1;
	sev->socktype = type;
	return 0;
}


int lc_server_set_threadstartfun(lc_server_t *sev, lc_handle_fun_t op)
{
	if (sev == NULL) return -1;
	sev->p_start = op;
	return 0;
}


int lc_server_set_threadstopfun(lc_server_t *sev, lc_handle_fun_t op) {
	if (sev == NULL) return -1;
	sev->p_end = op;
	return 0;
}

int lc_server_get_logprint(lc_server_t *sev){
	if (sev == NULL) return -1;
	return sev->log_print;
}
int lc_server_set_logprint(lc_server_t *sev, int logprint){
	if (sev == NULL) return -1;
	sev->log_print = logprint;
	return 0;
}

long long lc_server_get_usingqueue_size(lc_server_t *sev) {
    if (sev == NULL) return -1;
    return g_pool[sev->server_type].get_queuenum(sev);

}

long long lc_server_get_usingsocket_size(lc_server_t *sev) {
    if (sev == NULL) return -1;
    return g_pool[sev->server_type].get_socknum(sev);
}

int lc_server_set_monitor(lc_server_t *sev, lc::LcMonitor *monitor) {
    if (sev == NULL) return -1;
    sev->monitor = monitor;

    if (sev->monitor) {
        sev->monitor->register_service(sev->server_name, lc_server_get_threadnum(sev) + 4);

        if (lc_server_register_monitor_callback(sev) != 0) {
            LC_LOG_WARNING("register monitor callback error");
            return -1;
        }
    }

    return 0;
}

int lc_server_get_thread_pool_usage(lc_server_t * ) {
    return 100;
}

Lsc::var::IVar &lc_server_monitor_get_queue_size(Lsc::var::IVar &query, Lsc::ResourcePool &rp) {
    try {
        const char *tmp_buf = query["Arg"].c_str();
        long lp;
        sscanf(tmp_buf, "%ld", &lp);
        lc_server_t *sev = (lc_server_t*)((void *)lp);

        Lsc::var::IVar &output = rp.create<Lsc::var::Dict>();
        output["SOCKET_SIZE"] = rp.create<Lsc::var::Int64>(lc_server_get_usingsocket_size(sev));
        output["QUEUE_SIZE"] = rp.create<Lsc::var::Int64>(lc_server_get_usingqueue_size(sev));
        return output;
    } catch (Lsc::Exception &e) {
        LC_LOG_WARNING("catch Lsc exception:%s", e.all());
        return Lsc::var::Null::null;
    } catch (std::exception &e) {
        LC_LOG_WARNING("catch stl exception:%s", e.what());
        return Lsc::var::Null::null;
    } catch (...) {
        LC_LOG_WARNING("catch unknown exception");
        return Lsc::var::Null::null;
    }

}

Lsc::var::IVar &lc_server_monitor_get_thread_pool_usage(Lsc::var::IVar &query, Lsc::ResourcePool &rp) {
    try {
        const char *tmp_buf = query["Arg"].c_str();
        long lp;
        sscanf(tmp_buf, "%ld", &lp);
        lc_server_t *sev = (lc_server_t*)((void *)lp);

        Lsc::var::IVar &output = rp.create<Lsc::var::Dict>();
        output["REQUEST_QUEUE_SIZE"] = rp.create<Lsc::var::Int64>(lc_server_get_thread_pool_usage(sev));
        return output["REQUEST_QUEUE_SIZE"];
    } catch (Lsc::Exception &e) {
        LC_LOG_WARNING("catch Lsc exception:%s", e.all());
        return Lsc::var::Null::null;
    } catch (std::exception &e) {
        LC_LOG_WARNING("catch stl exception:%s", e.what());
        return Lsc::var::Null::null;
    } catch (...) {
        LC_LOG_WARNING("catch unknown exception");
        return Lsc::var::Null::null;
    }

}

int lc_server_register_monitor_callback(lc_server_t *sev)
{
    
    if (sev->monitor) {
        Lsc::string sev_name = sev->server_name;
        Lsc::string tmp_name = sev_name;
        if (sev->monitor->register_monitor_item((tmp_name.append(".REQUEST_QUEUE_SIZE")).c_str(), lc_server_monitor_get_queue_size, (void *)sev) != 0) {
            return -1;
        }
        tmp_name = sev_name;
        if (sev->monitor->register_monitor_item((tmp_name.append(".THREAD_POOL_USAGE")).c_str(), lc_server_monitor_get_thread_pool_usage, (void *)sev) != 0) {
            return -1;
        }
        return 0;
    } else {
        return -1;
    }
    return 0;
}

Lsc::var::IVar &get_monitor_list(Lsc::ResourcePool &rp)
{
    try {
		
		Lsc::var::IVar &service_info = rp.create<Lsc::var::Dict>();

		service_info["REQUEST_NUMBER"] = rp.create<Lsc::var::Dict>();
		Lsc::var::IVar &req_num_info = service_info["REQUEST_NUMBER"];
		req_num_info["SUCCESS"] = rp.create<Lsc::var::String>("Successed requests per unit.");
		req_num_info["FAILED"] = rp.create<Lsc::var::String>("Failed requests per unit.");
		req_num_info["TOTAL_SUCCESS"] = rp.create<Lsc::var::String>("Total successed requests.");
		req_num_info["TOTAL_FAILED"] = rp.create<Lsc::var::String>("Total failed requests.");

		service_info["REQUEST_QUEUE_SIZE"] = rp.create<Lsc::var::String>("Requests in queue.");

		service_info["THREAD_POOL_USAGE"] = rp.create<Lsc::var::String>("Thread pool usage.");

		service_info["REQUEST_CONNECT_NUMBER"] = rp.create<Lsc::var::String>("Request connect number per unit.");
		service_info["REQUEST_CONNECT_TOTAL_NUMBER"] = rp.create<Lsc::var::String>("Total reqeust connect number.");

		service_info["QUERY_CONNECT_NUMBER"] = rp.create<Lsc::var::Dict>();

		service_info["CONNECT_POOL_USAGE"] = rp.create<Lsc::var::Dict>();

		service_info["CONNECT_TIME"] = rp.create<Lsc::var::Dict>();

		service_info["CONNECT_FAIL_NUMBER"] = rp.create<Lsc::var::Dict>();

		service_info["READ_ERROR_NUMBER"] = rp.create<Lsc::var::String>("Read error number per unit.");
		service_info["WRITE_ERROR_NUMBER"] = rp.create<Lsc::var::String>("Write error number per unit.");

		service_info["COMPUTE_TIME"] = rp.create<Lsc::var::String>("Compute time per unit.");

		service_info["SELF_PROCESS_TIME"] = rp.create<Lsc::var::String>("Self compute time per unit.");
		service_info["SELF_ACTION_TIME"] = rp.create<Lsc::var::String>("User action time per unit.");

		service_info["IO_READ_TIME"] = rp.create<Lsc::var::String>("IO read time per unit.");
		service_info["IO_WRITE_TIME"] = rp.create<Lsc::var::String>("IO write time per unit.");

		service_info["FATAL_NUMEBER"] = rp.create<Lsc::var::String>("Fatal number per unit.");
    
        return service_info;
    } catch(Lsc::Exception &e) {
        LC_LOG_WARNING("catch Lsc exception:%s", e.all());
        return Lsc::var::Null::null;
    } catch(...) {
        LC_LOG_WARNING("catch unknown exception");
        return Lsc::var::Null::null;
    }
}

int lc_server_get_listen_priority(lc_server_t *sev)
{
    if (!sev) return 0;
    return sev->listen_prio;
}

int lc_server_get_worker_priority(lc_server_t *sev)
{
    if (!sev) return 0;
    return sev->worker_prio;
}

bool lc_server_threadsched_used(lc_server_t *sev)
{
    if (!sev) return false;
    return sev->using_threadsche==0?false:true;
}


