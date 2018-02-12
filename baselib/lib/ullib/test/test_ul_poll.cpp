/**
	write by renjie in 2011.5.30
*/

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <errno.h>
#include <string.h>
#include <signal.h>


#include "ul_net.h"

const int SERVER_PORT = 8813;

void* server(void *args)
{
	char strbuf[128];
	int serverfd = ul_socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(SERVER_PORT);
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);
	ul_bind(serverfd, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in));

printf("start server\n");
	ul_listen(serverfd, 1);
printf("listen pass\n");
	int clientfd = ul_accept(serverfd, NULL, NULL);
printf("accept\n");
	sleep(10);
	write(clientfd, "hello world", 11);	
//	sleep(10);
	read(clientfd, strbuf, sizeof(strbuf));
	printf("%s\n", strbuf);
	ul_sclose(clientfd);
	
	ul_sclose(serverfd);
	return (void *)0;
} 

void *client(void *args)
{
	char strbuf[128];
	int clientfd = ul_socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in saddr;
	socklen_t socklen;
	
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(SERVER_PORT);
	//inet_pton(AF_INET, "10.30.138.10", &saddr.sin_addr);  // time out connect test point
	inet_pton(AF_INET, "127.0.0.1", &saddr.sin_addr);
//printf("file %d\n", clientfd);
	
	// 1. check point, connect with time out
	while ( ul_connecto_ms_ex(clientfd, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in), 2000) == -1 ) {
		if ( errno == ETIMEDOUT ) {
			printf("connect timeout\n");
		} else {
			printf("connect error %s\n", strerror(errno));
		}
		sleep(2);
		close(clientfd);
		clientfd = ul_socket(AF_INET, SOCK_STREAM, 0);
	}
	printf("connect in client\n");	

	// 2. check point, readable
	while ( ul_sreadable_tv(clientfd, NULL) == 0 ) {
		printf("no read data\n");
		struct timeval tval;
		gettimeofday(&tval, NULL);
		printf("%d:%d\n", tval.tv_sec, tval.tv_usec);
	}
	read(clientfd, strbuf, sizeof(strbuf));
	
	// 3. check point, writable
	while ( ul_swriteable_ms(clientfd, 2000) == 0 ) {
		printf("write buffer is full\n");
	}
	write(clientfd, "helloworld\0", 11);
	
	// 4. check point, ul_swriteable_tv,  
	// call function link
	// ul_swriteo_ms_ex -> ul_writeo_tv call -> ul_poll
	ul_swriteo_ms_ex(clientfd, strbuf, sizeof(strbuf), -1);

	ul_sclose(clientfd);
	return (void *)0;
}

void* soy(void *args)
{
	int sockfd = ul_socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = ntohs(SERVER_PORT);
	inet_pton(AF_INET, "127.0.0.1", &saddr.sin_addr);
	
	ul_connect(sockfd, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in));
	printf("soy connected\n");
	//sleep(10);	
	for (;;) {}
	printf("soy exit\n");
	ul_sclose(sockfd);	
}

void test1()
{
	pthread_t sid, cid;

	pthread_create(&cid, NULL, client, NULL);	
	sleep(10);
	pthread_create(&sid, NULL, server, NULL);
	
	pthread_join(cid, NULL);
	pthread_join(sid, NULL);
	
}

void test2()
{

	pthread_t sid, cid[10], cid2;
	pthread_create(&sid, NULL, server, NULL);
	sleep(1);
	for ( int i = 0; i < 10; i++ ) {
		pthread_create(&cid[i], NULL, soy, NULL);
	}
	sleep(6);
	pthread_create(&cid2, NULL, client, NULL);

	pthread_join(sid, NULL);
	//pthread_join(cid1, NULL);
	for ( int i = 0; i < 10; i++ ) {
		pthread_join(cid[i], NULL);
	}
	pthread_join(cid2, NULL);	
	
}

void test3() 
{	
	struct pollfd fd;
	memset(&fd, 0, sizeof(struct pollfd));
	fd.fd = STDIN_FILENO;
	fd.events = POLLIN;
	while ( ul_sreadable_tv(STDIN_FILENO, NULL) == 0 ) {
			printf("illstate\n");
	}	
	
//	printf("%d : %d", fd.events, fd.revents);
	while ( ul_poll(&fd, 1, 2000) == 0 ) {
		printf("no read data\n");
//		printf("%d : %d", fd.events, fd.revents);
	}
	
}




void sigint(int signo)
{
	printf("sigint\n");
}

int main(int argc, char *argv[])
{
	signal(SIGINT, sigint);
//	test1();
//	test2();
	test3();
	return 0;
}


