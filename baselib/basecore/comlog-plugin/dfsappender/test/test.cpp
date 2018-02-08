
#include <comlog/comlog.h>
#include "dfsappender.h"
#include "dfsappendersvr.h"
#include "ul_log.h"
#include <poll.h>
#include <unistd.h>

void *run(void *arg)
{

	char buff[1024];
	memset(buff, 0, sizeof(buff));
	int i = *((int*)arg);
	for (int k = 0; k < 800; ++k) {
		buff[k] = rand()%26+'a';
	}
	

	int j = 0;
	while (1) {
		int ret = snprintf(buff, sizeof(buff), "[%d/%d] ", i, j);
		buff[ret] = '|';
		com_writelog(UL_LOG_DEBUG, "%s", buff);
		poll(0,0,1);
		++j;

	}
	

	com_closelog_r();
	return NULL;
}

int main(){
	int i = 0;;

	int ret;
	com_logstat_t logstat;
	signal(SIGPIPE,SIG_IGN);
	com_registappender("DFSPROXY", comspace::DfsAppender::getAppender, comspace::DfsAppender::tryAppender);

	ret = com_loadlog("./", "comlogdfs.conf");
	if (ret < 0) {
		return -1;
	}
	
	
	
	
	pthread_t pid[100];
	int thr[100];
	for (int i = 0; i < 50; ++i) {
		thr[i] = i;
		pthread_create(&pid[i], NULL, run, thr+i);
	}
	for (int i = 0; i < 50; ++i) {
		pthread_join(pid[i], NULL);
	}
	fprintf(stderr, "I'm Gonna sleep...\n", ret);
	fprintf(stderr, "Gonna closelog\n", ret);
	com_closelog(1000);
	return 0;
}
















