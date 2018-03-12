


#ifndef _MYNET_H
#define _MYNET_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "ul_log.h"




int MyConnectO(const char *host, int port, int *timeout, const char *name);


int MyReadO(int sock, void *buf, ssize_t len, int *timeout, const char *name);


int MyReadOS(int sock, void *buf, ssize_t len, int *timeout, const char *name,
	     const char *stop);


int MyWriteO(int sock, void *buf, ssize_t len, int *timeout,
	     const char *name);


int MyClose(int fd);


int MyTcpListen(int port, int queue);

#endif 

