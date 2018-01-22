#ifndef _POOL_H
#define _POOL_H



#include "lcserver_core.h"

int set_pthread_data(lc_server_pdata_t *data);

lc_server_pdata_t  *get_pdata();
lc_server_t *get_server();
int lc_get_session_connect_type_then_reset(lc_server_t *sev);

int lc_setsockopt(int fd, lc_server_t *sev);
int lc_setsockopt_ex(int fd, lc_server_t *sev);


#define _LC_P_START \
{\
	if (sev->p_start) { \
		if (sev->p_start() != 0) { \
			LC_LOG_WARNING("run init function in thread start error ret!=0"); \
			return NULL; \
		} \
	} \
}

#define _LC_P_END \
{\
	if (sev->p_end) { \
		sev->p_end();\
	}\
}

#ifdef LC_DEBUG_OPEN
static const int LCSVR_MONITOR_TIME = 1;
#else
static const int LCSVR_MONITOR_TIME = 100;
#endif 

#endif


