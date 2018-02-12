
#include "com_log.h"
#include "comlog/appender/fileappender.h"
#include "ul_log.h"
#include <unistd.h>
namespace comspace{
	extern int __log_switcher__;
}

int main(){
	int i;

	assert(com_logstatus() == LOG_NOT_DEFINED);
	com_loadlog("./", "test.conf");
	for(int j = 0; j < 1000; j++){
		ul_writelog(COMLOG_WARNING, "1 - UL [%d/%d]", i, j);
		com_writelog(UL_LOG_FATAL, "1 - COM [%d/%d]", i, j);
		sleep(1);
	}
	assert(com_logstatus() == USING_COMLOG);
	com_closelog(0);
	assert(com_logstatus() == LOG_NOT_DEFINED);
	return 0;
}















/* vim: set ts=4 sw=4 sts=4 tw=100 noet: */
