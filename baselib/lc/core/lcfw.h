#ifndef __LCFW_H
#define __LCFW_H

#include "lc_server.h"
#include "lc_client.h"

namespace lc
{
    class LcMonitor;
}

static const int LC_PATHSIZE = 1024;

struct lc_t
{
	char name[LC_PATHSIZE];
	char file[LC_PATHSIZE];
	char path[LC_PATHSIZE];
	int conf_build;
	int conf_check;
	lc_conf_data_t *conf;
};

#ifndef LCFLAG_PROJECT
#define LCFLAG_PROJECT "Default"
#endif
#ifndef LCFLAG_VERSION
#define LCFLAG_VERSION "none"
#endif
#ifndef LCFLAG_CVSTAG
#define LCFLAG_CVSTAG "none"
#endif

#ifndef LCFLAG_BUILD_DATE
#define LCFLAG_BUILD_DATE  "unknown"
#endif

#define LCFW_ASSERT(op, fmt, arg...) \
{\
	if (!(op)) {\
		LC_LOG_WARNING(fmt, ##arg);\
		lc_safe_exit();\
	}\
}

static void
lc_default_print_version()
{
    printf("Project    :	%s\n", LCFLAG_PROJECT);
    printf("Version    :	%s\n", LCFLAG_VERSION);
    printf("CVSTag     :	%s\n", LCFLAG_CVSTAG);
    printf("BuildDate  :	%s\n", LCFLAG_BUILD_DATE);
   return;
}


void lc_safe_exit();


void lc_builddone_exit(lc_t *);


lc_t *lc_init(const char *name, int argc, char **argv, 
		void (*)()=lc_default_print_version, void (*)()=NULL);
void lc_close(lc_t *);


lc_conf_data_t *lc_open_conf(lc_t *, const int version = LC_CONF_VERSION_2);

void lc_close_conf(lc_t *);


lc_server_t *lc_load_svr(lc_t *u, const char *name);

lc_client_t *lc_load_clt(lc_t *u, const char *name, const char *cfg,
		lc_client_attr_t *attr=NULL);

int lc_load_log(lc_t *);

struct lc_svr_vec_t;	


lc_svr_vec_t *lc_svr_vec_create();

int lc_svr_vec_add(lc_svr_vec_t *vec, lc_server_t *svr, lc_t *fw=NULL);

int lc_svr_vec_run(lc_svr_vec_t *vec);	


int lc_svr_vec_stop(lc_svr_vec_t *vec);


int lc_svr_vec_join(lc_svr_vec_t *vec);

int lc_svr_vec_destroy(lc_svr_vec_t *vec);

lc::LcMonitor *lc_load_monitor(lc_t *fw);


int  lcfw_open_galileo_log(lc_t *fw);



void lcfw_close_galileo_log();

#endif

