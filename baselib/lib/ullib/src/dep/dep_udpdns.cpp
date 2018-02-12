#include "ul_udpdns.h"
#include "ul_log.h"
#include "ul_net.h"



//²»ÍÆ¼ö
int
send_recv_msg(int sockfd, struct sockaddr *pservaddr, char *datamsg, int datalen)
{
	int recvlen;
	int sendlen;
	socklen_t servlen;

	servlen = (socklen_t) sizeof(struct sockaddr_in);
	sendlen = sendto(sockfd, datamsg, datalen, 0, pservaddr, servlen);
	if (sendlen < 0) {
		ul_writelog(UL_LOG_WARNING, "sendto(%d) function failed.errno is %d.", sockfd, errno);
		return -1;
	}

	recvlen = recvfrom(sockfd, datamsg, UDP_MSGLEN, 0, NULL, NULL);
	if (recvlen >= 0) {
		*(datamsg + recvlen) = '\0';
		return recvlen;
	} else {
		ul_writelog(UL_LOG_WARNING, "recvfrom(%d) function failed.errno is %d.", sockfd, errno);
		return -1;
	}
}


/*
 ** Next was function which will get the ip of a domain by udp socket descriptor
 ** the return value was stored in the variable -ip-
 ** -1 means that the function was failed.
*/
int
gethostwithfd_r(int sockfd, struct sockaddr *servaddr, char *domain, char *ip)
{
	int inum = 0;

	if (sockfd == -1 || servaddr == NULL)
		return -1;

	if (inet_addr(domain) != INADDR_NONE) {
		strcpy(ip, domain);
		return 0;
	}
	char sendbuf[UDP_MSGLEN + 1];
	memset(sendbuf, 0, UDP_MSGLEN + 1);
	sprintf(sendbuf, "%s", domain);
	int recvlen = send_recv_msg(sockfd, servaddr, sendbuf, UDP_MSGLEN);

	if (recvlen != UDP_MSGLEN)
		*ip = '\0';
	else {
		char *p;
		p = strchr(sendbuf, ' ');
		if (p == NULL) {
			*ip = 0;
			if (strcmp(sendbuf, domain) != 0)
				ip[0] = '\0';
		} else {
			char domain_r[SITE_NAME_LEN + 1];
			memset(domain_r, 0, SITE_NAME_LEN + 1);
			strncpy(domain_r, sendbuf, p - sendbuf);
			if (strcmp(domain_r, domain) != 0)
				ip[0] = 0;
			else {
				p++;
				while (*p != ' ' && *p && inum < INET_ADDRSTRLEN - 1)
					ip[inum++] = *(p++);
				ip[inum] = '\0';
			}
		}
	}

	if (ip[0] == 0)
		return -1;

	return 0;
}

/*
 ** get ip of domain by dns server host and udp listen port.
 ** -1 mean failed. 
*/
int
gethostipbyname_r(char *dnshost, int port, char *domain, char *ip)
{
	int sockfd;
	int inum = 0;
	struct sockaddr_in servaddr;

	if (inet_addr(domain) != INADDR_NONE) {
		strcpy(ip, domain);
		return 0;
	}

	if ((sockfd = ul_udpconnect(dnshost, port, &servaddr)) == -1)
		return -1;

	char sendbuf[UDP_MSGLEN + 1];
	memset(sendbuf, 0, UDP_MSGLEN + 1);
	sprintf(sendbuf, "%s", domain);
	int recvlen = send_recv_msg(sockfd, (struct sockaddr *) &servaddr, sendbuf, UDP_MSGLEN);

	if (recvlen != UDP_MSGLEN)
		*ip = '\0';
	else {
		char *p;
		p = strchr(sendbuf, ' ');
		if (p == NULL) {
			*ip = 0;
			if (strcmp(sendbuf, domain) != 0)
				ip[0] = '\0';
		} else {
			char domain_r[SITE_NAME_LEN + 1];
			memset(domain_r, 0, SITE_NAME_LEN + 1);
			strncpy(domain_r, sendbuf, p - sendbuf);
			if (strcmp(domain_r, domain) != 0)
				ip[0] = 0;
			else {
				p++;
				while (*p != ' ' && *p && inum < INET_ADDRSTRLEN - 1)
					ip[inum++] = *(p++);
				ip[inum] = '\0';
			}
		}
	}
	close(sockfd);

	if (ip[0] == 0)
		return -1;

	return 0;
}






/* vim: set ts=4 sw=4 sts=4 tw=100 */
