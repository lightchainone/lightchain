
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "ul_net.h"

void foo(char * s, int n){
	for(int i = 0; i < n; ++i){
		s[i] = toupper(s[i]);
	}
}

void work(int fd, int type = 0){
	ssize_t n;
	char buf[1024];
	while( (n = read(fd, buf, 1024)) > 0){
		foo(buf, n);
		write(fd, buf, n);
	}
again:
	if(n < 0 && errno == EINTR){
		goto again;
	}
	else if(n < 0){
		printf("read error");
	}
}

int main(int argc, char * argv[]){
	int listenfd, connfd;
	pid_t childpid;
	socklen_t clilen;
	struct sockaddr_in cliaddr;

	int port = 65535;
	if(argc > 1){
		sscanf(argv[1], "%d", &port);
	}
	if(port == 65535){
		printf("port err\n");
		return -1;
	}else{
		printf("start server at port : %d\n", port);
	}

	listenfd = ul_tcplisten(port, 50);
	if(listenfd < 0){
		printf("listen port %d err.\n", port);
		return -1;
	}

	while(1){
		clilen = sizeof(cliaddr);
		connfd = accept(listenfd, (sockaddr *)&cliaddr, &clilen);
		if( (childpid = fork()) == 0){
			close(listenfd);
			work(connfd);
			exit(0);
		}
		close(connfd);
	}
	return 0;
}





















