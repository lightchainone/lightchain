
#include "com_log.h"
#include "ul_log.h"

int main(){
	const char * s = "123456789.123456789.123456789.123456789.123456789.123456789.";
	int ret = com_loadlog("./", "test5.conf");
	printf("loadlog ret = %d\n", ret);
	ul_writelog(COMLOG_WARNING, "%s", s);
	ul_writelog(COMLOG_NOTICE, "%s", s);
	ul_writelog(COMLOG_TRACE, "%s", s);
	return 0;
}




















/* vim: set ts=4 sw=4 sts=4 tw=100 */
