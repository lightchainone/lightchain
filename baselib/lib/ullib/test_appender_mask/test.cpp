#include <com_log.h>


int main(){
	
//	com_registappender("BIGPIPEPROXY", comspace::BigpipeAppender::getAppender, comspace::BigpipeAppender::tryAppender);
	
	int ret = com_loadlog("./conf","test.conf");
	if (ret != 0) {
		fprintf(stderr, "load err\n");
		return -1;
	}

	int ct = 0;


	while(1){
		com_writelog(COMLOG_NOTICE, "wzp%d",ct++);
		com_writelog(COMLOG_DEBUG, "wzp%d",ct++);
		com_writelog(COMLOG_WARNING, "wzp%d",ct++);
		com_writelog(COMLOG_FATAL, "wzp%d",ct++);
		//com_writelog(COMLOG_DEBUG, "wzp%d",ct++);
		
		sleep(1);
		com_setappendermask("FILE");
	}
	com_closelog(60000);

	return 0;
}
