#include "lc_log.h"
#include <Configure.h>
#include <comlogplugin.h>
int lc_init_comlog(const comcfg::ConfigUnit & conf ){
	lc_log_keyonce();
	if(0 != comlog_init(conf)){
		fprintf(stderr, "lc_init_comlog failed...");
		return -1;
	}
    return notice_specific_init();
}
