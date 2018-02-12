#include "ul_udpdns.h"
#include "ul_log.h"
#include "ul_net.h"
#include "ul_string.h"

/*
  ** create a sockfd for listening on a port by udp protocol
  ** function include "socket() and bind() "
  ** if the function running failed, -1 was returned.
*/
int
ul_udplisten(int udp_port)
{
	int sockfd;
	char where[] = "ul_udplisten";
	struct sockaddr_in servaddr;

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		ul_writelog(UL_LOG_WARNING, "%s socket() failed. errno is %d.", where, errno);
		return -1;
	}

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons((uint16_t)udp_port);

	if (bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) == -1) {
		ul_writelog(UL_LOG_WARNING, "%s bind(%d) failed. errno is %d.", where, sockfd, errno);
		close(sockfd);
		return -1;
	}

	return sockfd;
}

/*
  ** connect to udp server
  ** argument: host / port  --- for udp server
*/
int
ul_udpconnect(char *host, int udp_port, struct sockaddr_in *servaddr)
{
	int sockfd;

	memset(servaddr, 0, sizeof(struct sockaddr_in));
	servaddr->sin_family = AF_INET;
	int ret = inet_pton(AF_INET, host, &servaddr->sin_addr);
	if (ret <= 0) {
		ul_writelog(UL_LOG_WARNING, "call inet_pton with AF_INET host[%s] err[%m]", host);
		return -1;
	}
	servaddr->sin_port = htons((uint16_t)udp_port);

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		ul_writelog(UL_LOG_WARNING, "in ul_udpconnect(). socket(%s/%d) failed.errno is %d.",
					host, udp_port, errno);
		return -1;
	}

	return sockfd;
}


/*
 ** send a udp packet to server and receive the response
 ** return value is the length of the response packet.
*/
int
send_recv_msgo_tv(int sockfd, struct sockaddr *pservaddr, char *datamsg, int datalen, timeval * tv)
{
	int recvlen = 0;
	int sendlen = 0;
	socklen_t servlen;

	servlen = (socklen_t) sizeof(struct sockaddr_in);
	sendlen = sendto(sockfd, datamsg, (size_t)datalen, 0, pservaddr, servlen);
	if (sendlen < 0) {
		ul_writelog(UL_LOG_WARNING, "sendto(%d) function failed.errno is %d.", sockfd, errno);
		return -1;
	}

	int retval = ul_sreadable_tv(sockfd, tv);

	if (retval > 0) {
		recvlen = recvfrom(sockfd, datamsg, UDP_MSGLEN, 0, NULL, NULL);
		if (recvlen >= 0) {
			*(datamsg + recvlen) = '\0';
			return recvlen;
		} else {
			ul_writelog(UL_LOG_WARNING, "recvfrom(%d) function failed.errno is %d.", sockfd, errno);
			return -1;
		}
	} else
		recvlen = -1;

	return recvlen;
}


/*
 ** send a udp packet to server ans receive the response with overtime handling.
*/
int
send_recv_msgo(int sockfd, struct sockaddr *pservaddr, char *datamsg, int datalen, int sec,
			   int usec)
{
	timeval tv;
	tv.tv_sec = sec;
	tv.tv_usec = usec;
	return send_recv_msgo_tv(sockfd, pservaddr, datamsg, datalen, &tv);
}


/*
 ** Next was function which will get the ip of a domain with overtime handling by udp socket descriptor.
 ** the return value was stored in the variable -ip-
 ** -1 means that the function was failed.
*/

int
gethostwithfd_r_tv(int sockfd, struct sockaddr *servaddr, char *domain, char *ip, timeval * tv)
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
	snprintf(sendbuf, sizeof(sendbuf), "%s", domain);
	int recvlen = send_recv_msgo_tv(sockfd, servaddr, sendbuf, UDP_MSGLEN, tv);

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
			size_t tlen = (size_t)(p - sendbuf);
			if (tlen >= sizeof(domain_r)) {
				tlen = sizeof(domain_r) - 1;
			}
			strncpy(domain_r, sendbuf, tlen);
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

int
gethostwithfd_r_o(int sockfd, struct sockaddr *servaddr, char *domain, char *ip, int sec, int usec)
{
	timeval tv;
	tv.tv_sec = sec;
	tv.tv_usec = usec;
	return gethostwithfd_r_tv(sockfd, servaddr, domain, ip, &tv);
}


/*
 ** get ip of domain by dns server host and udp listen port with overtime handling.
 ** -1 mean failed. 
*/
int
gethostipbyname_r_tv(char *dnshost, int udp_port, char *domain, char *ip, timeval * tv)
{
	int sockfd;
	struct sockaddr_in servaddr;

	if ((sockfd = ul_udpconnect(dnshost, udp_port, &servaddr)) == -1)
		return -1;

	int ret = gethostwithfd_r_tv(sockfd, (struct sockaddr *) &servaddr, domain, ip, tv);

	close(sockfd);
	return ret;
}

int
gethostipbyname_r_o(char *dnshost, int udp_port, char *domain, char *ip, int sec, int usec)
{
	timeval tv;
	tv.tv_sec = sec;
	tv.tv_usec = usec;
	return gethostipbyname_r_tv(dnshost, udp_port, domain, ip, &tv);
}
