
#include <assert.h>
#include "comlogplugin.h"

int main(){
	comcfg::Configure conf;
	assert(conf.load("./", "comlog.conf", "comlog.range") == 0);
	conf.print();
	int ret = comlog_init(conf["comlog"]);
	assert(ret == 0);
	ul_writelog(COMLOG_WARNING, "haha");
	ul_writelog(COMLOG_NOTICE, "123456789.123456789.123456789.123456789.123456789.123456789.");
	return 0;
}




















