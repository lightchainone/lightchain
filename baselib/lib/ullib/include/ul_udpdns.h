/**
 */
#ifndef __UDPDNS_H__
#define __UDPDNS_H__

#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <netinet/in.h>


#define UDP_DNS_PORT              1280
#define SITE_NAME_LEN             64
#define UDP_MSGLEN                128

// struct 
struct host_struct {
	char domain[SITE_NAME_LEN + 1];
	char ip[INET_ADDRSTRLEN + 1];
};

#include "dep/dep_udpdns.h"

/**
 *  建立UDP服务端套节字(function include "socket and bind")
 *  
 * - >=0 UDP服务端套节字
 * - -1  失败
 */
extern int ul_udplisten(int udp_port);

/**
 *  建立UDP客户端套节字
 *  
 * - >=0 UDP客户端套节字
 * - -1  失败
 */
extern int ul_udpconnect(char *host, int udp_port, struct sockaddr_in *servaddr);

/**
 *  发送并接收UDP消息，支持超时
 *  
 * - >=0 接收到的数据长度
 * - -1  失败
 */
extern int send_recv_msgo(int sockfd, struct sockaddr *pservaddr, char *datamsg, int datalen,
						  int sec, int usec);

/**
 *  从DNS服务器获取域名对应的IP，支持超时
 *  
 * - 0   成功
 * - -1  失败
 */
extern int gethostwithfd_r_o(int sockfd, struct sockaddr *servaddr, char *domain, char *ip, int sec,
							 int usec);

/**
 *  从DNS服务器获取域名对应的IP，支持超时
 *  
 * - 0   成功
 * - -1  失败
 */
extern int gethostipbyname_r_o(char *dnshost, int port, char *domain, char *ip, int sec, int usec);

#endif
