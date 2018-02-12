//******************************************************************
//  This library include most functions.include some wrap funtions.
//  You can use this functions freely.
//******************************************************************
// include area 
#include <assert.h>

#include "ul_net.h"
#include "ul_log.h"
#include "ul_conf.h"
#include "ul_string.h"

char ul_ipaccess_grant[IPACCESS_GRANT_LEN + 1] = "";
//***************************************************************
// Note: keep the number of globle variables as few as possible
// for globle variables define
// Model: int ul_xxx;
//***************************************************************

//*******************************************************************
// for function group a
// Note: keep the number of remarks in your code as many as possible
// Model:
// int ul_xxx(int a, char* b)
// {
//      ...
// }
//*******************************************************************

//**********************************************************************
// Wrap function. This function will create an endpoint for communication
//***********************************************************************

static void
cnvt_ms_timeval(const int msec, struct timeval *ptv)
{
	if (ptv == NULL) {
		return;
	}
	ptv->tv_sec = msec / 1000;
	ptv->tv_usec = (msec % 1000) * 1000;
}

static int
cnvt_timeval_ms(const struct timeval *ptv)
{
	if ( ptv == NULL ) {
		return -1;
	}
	return ptv->tv_sec * 1000 + ptv->tv_usec / 1000;
}



int
ul_socket(int family, int type, int protocol)
{
	int val = socket(family, type, protocol);
	if (val == -1) {
		ul_writelog(UL_LOG_WARNING, "socket(%d,%d,%d) call failed.error[%d] info is %s.",
					family, type, protocol, errno, strerror(errno));
	}
	return val;
}

//**********************************************************************
// Wrap function about connect() system call
//***********************************************************************
int
ul_connect(int sockfd, const struct sockaddr *sa, socklen_t addrlen)
{
	int val = connect(sockfd, sa, addrlen);
	if (val == -1) {
		ul_writelog(UL_LOG_WARNING, "connect(%d,<%d,%d,%u>,%d) call failed.error[%d] info is %s.",
					sockfd, ((struct sockaddr_in *) sa)->sin_family,
					((struct sockaddr_in *) sa)->sin_port,
					((struct sockaddr_in *) sa)->sin_addr.s_addr, addrlen, errno, strerror(errno));
	}
	return val;
}

//**********************************************************************
// Wrap function about bind() system call
//***********************************************************************
int
ul_bind(int sockfd, const struct sockaddr *myaddr, socklen_t addrlen)
{
	int val = bind(sockfd, myaddr, addrlen);
	if (val == -1) {
		ul_writelog(UL_LOG_WARNING, "bind(%d,<%d,%d,%u>,%d) call failed.error[%d] info is %s.",
					sockfd, ((struct sockaddr_in *) myaddr)->sin_family,
					((struct sockaddr_in *) myaddr)->sin_port,
					((struct sockaddr_in *) myaddr)->sin_addr.s_addr, addrlen, errno,
					strerror(errno));
	}
	return val;
}

//**********************************************************************
// Wrap function about Listen() system call
//***********************************************************************
int
ul_listen(int sockfd, int backlog)
{
	int val = listen(sockfd, backlog);
	if (val == -1) {
		ul_writelog(UL_LOG_WARNING, "listen(%d,%d) call failed.error[%d] info is %s.", sockfd,
					backlog, errno, strerror(errno));
	}
	return val;
}

/*
** read configure item "ip-access-grant" from configure file * 
** in this item,all fields were delimited by signure ';'
*/

int
ul_read_ipaccess_grant(ul_confdata_t * pd_conf)
{
	memset(ul_ipaccess_grant, 0, sizeof(ul_ipaccess_grant));
	if (pd_conf != NULL) {
		ul_getconfnstr(pd_conf, "ip-access-grant", ul_ipaccess_grant, sizeof(ul_ipaccess_grant));
	}
	if (ul_ipaccess_grant[0] == 0) {
		ul_strlcpy(ul_ipaccess_grant, "127.0.0.1;192.168.1.*", sizeof(ul_ipaccess_grant));
	}
	return 0;
}


/* judge if a part of a ip is fit the expression 
 * fromip is a number,end with \0 or .
 * part is in following format,end with \0 or .
 * number
 * number1-number2 (number1 < number2)
 * *
 * return 0, if not in
 * 1 if in
 * 2 if arguments format error
 */
static int
is_onepart_fit(const char *fromip, const char *part)
{
	unsigned int part_low;
	unsigned int part_high;
	unsigned int from;
	char *end;
	char *tmp;
	if ((fromip == NULL) || (part == NULL)) {
		return 2;
	}
	if (strncmp(part, "*", 1) == 0) {
		return 1;
	}
	from = strtoul(fromip, &end, 10);
	if (((end != NULL) && (*end != '.') && (*end != '\0')) || (end == fromip)) {
		return 2;
	}
	part_low = strtoul(part, &end, 10);
	if (((end != NULL) && (*end != '.') && (*end != '-') && (*end != '\0')) || (end == part)) {
		return 2;
	}
	if ((end != NULL) && (*end == '-')) {
		tmp = end + 1;
		part_high = strtoul(tmp, &end, 10);
		if (((end != NULL) && (*end != '.') && (*end != '\0')) || (end == tmp)) {
			return 2;
		}
	} else {
		part_high = part_low;
	}
	if ((from >= part_low) && (from <= part_high)) {
		return 1;
	} else {
		return 0;
	}
}

/* judge the ip is in the sunbet given by slcnet 
 * se judge_onepart for detail
 * return 0 if not in
 *        1 if in
 *        2 if ip or pattern format error
 */
static int
is_in_slcnet(const char *ip, const char *slcnet)
{
	int i;
	const char *bip = ip;
	const char *Lsccnet = slcnet;
	int ret;
	for (i = 0; i < 4; i++) {
		if ((bip == NULL) || (Lsccnet == NULL)) {
			return 2;
		}
		ret = is_onepart_fit(bip, Lsccnet);
		if (ret == 0) {
			return 0;
		} else if (ret == 2) {
			return 2;
		}
		if (i < 3) {
			bip = strchr(bip, '.');
			Lsccnet = strchr(Lsccnet, '.');
			if (bip != NULL) {
				bip++;
			}
			if (Lsccnet != NULL) {
				Lsccnet++;
			}
		}
	}
	return 1;
}

/* judge ip is fit a listing pattern or not
 * 1 if in
 * 0 if not in
 * if pattern is empty, any ip will be allowed
 */
static int
is_ip_allowed(const char *ip, const char *pattern)
{
	char slcnet[256];
	const char *bsnet = pattern;
	const char *end;

	if ((bsnet == NULL) || (bsnet[0] == 0)) {
		//empty pattern will allow any ip.
		return 1;
	}

	while ((bsnet != NULL) && (bsnet[0] != 0)) {
		end = strchr(bsnet, ';');
		if (end != NULL) {
			if (end - bsnet > (int) sizeof(slcnet) - 1) {
				bsnet = end + 1;
				continue;
			}
			//build a slcnet
			memcpy(slcnet, bsnet, (size_t)(end - bsnet));
			slcnet[end - bsnet] = 0;

			if (is_in_slcnet(ip, slcnet) == 1) {
				return 1;
			}
			bsnet = end + 1;
		} else {
			if (is_in_slcnet(ip, bsnet) == 1) {
				return 1;
			}
			bsnet = NULL;
		}
	}
	return 0;

}

//**********************************************************************
// Wrap function about Accept() system call
//***********************************************************************
int
ul_accept(int sockfd, struct sockaddr *sa, socklen_t * addrlen)
{
	int connfd = 0;
  again:
	connfd = accept(sockfd, sa, addrlen);
	if (connfd < 0) {
#ifdef  EPROTO
		if (errno == EPROTO || errno == ECONNABORTED) {
#else
		if (errno == ECONNABORTED) {
#endif
			goto again;
		} else {
			ul_writelog(UL_LOG_WARNING, "accept(%d) call failed.error[%d] info is %s.", sockfd,
						errno, strerror(errno));
			return -1;
		}
	}

	if (ul_ipaccess_grant == NULL || ul_ipaccess_grant[0] == 0) {
		return (connfd);
	}

	struct sockaddr_in peeraddr;
	struct sockaddr_in localaddr;
	socklen_t adrlen = sizeof(struct sockaddr_in);

	char ip_net[INET_ADDRSTRLEN];
	memset(ip_net, 0, INET_ADDRSTRLEN);

	int peer = 0;
	peer = ul_getpeername(connfd, (struct sockaddr *) &peeraddr, &adrlen);
	if (peer < 0) {
		close(connfd);
		return -1;
	}

	adrlen = sizeof(struct sockaddr_in);
	ul_getsockname(connfd, (struct sockaddr *) &localaddr, &adrlen);

	struct in_addr in_val;
	in_val.s_addr = peeraddr.sin_addr.s_addr;
	inet_ntop(AF_INET, &in_val, ip_net, INET_ADDRSTRLEN);

	if (ip_net[0] != 0) {
		if (is_ip_allowed(ip_net, ul_ipaccess_grant) == 0) {
			goto close_connect;
		}
	} else {
		goto close_connect;
	}

	return (connfd);

  close_connect:
	close(connfd);
	ul_writelog(UL_LOG_WARNING,
				"When server listened on port %d a client from ip %s try to connect.",
				ntohs(localaddr.sin_port), ip_net);
	return -1;

}

//*******************************************
//function about get the local socket address family
//*******************************************
int
ul_getsockname(int sockfd, struct sockaddr *localaddr, socklen_t * addrlen)
{
	int val = getsockname(sockfd, localaddr, addrlen);
	if (val == -1) {
		ul_writelog(UL_LOG_WARNING, "getsockname(%d) call failed.error[%d] info is %s.",
					sockfd, errno, strerror(errno));
	}
	return val;
}

//*******************************************
// function about get the peer socket address family
//*******************************************
int
ul_getpeername(int sockfd, struct sockaddr *peeraddr, socklen_t * addrlen)
{
	int val = getpeername(sockfd, peeraddr, addrlen);
	if (val == -1) {
		ul_writelog(UL_LOG_WARNING, "getpeername(%d) call failed.error[%d] info is %s.\n",
					sockfd, errno, strerror(errno));
	}
	return val;
}

//********************************************
// Wrap function about setsockopt()
//********************************************
int
ul_setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen)
{
	int val = setsockopt(sockfd, level, optname, optval, optlen);
	if (val == -1) {
		ul_writelog(UL_LOG_WARNING, "setsockopt(%d,%d,%d) call failed.error[%d] info is %s.",
					sockfd, level, optname, errno, strerror(errno));
	}
	return val;
}

//********************************************
// Wrap function about getsockopt()
//********************************************
int
ul_getsockopt(int sockfd, int level, int optname, void *optval, socklen_t * optlen)
{
	int val = getsockopt(sockfd, level, optname, optval, optlen);
	if (val == -1) {
		ul_writelog(UL_LOG_WARNING, "setsockopt(%d,%d,%d) call failed.error[%d] info is %s.",
					sockfd, level, optname, errno, strerror(errno));
	}
	return val;
}




//********************************************************************
//  ul_sclose oen socket
//********************************************************************
int
ul_sclose(int fd)
{
	return close(fd);
}

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

//*************************************************************
// Get host byte order.
// Return value:    BIG_ENDIAN<1>  
//                  LITTLE_ENDIAN <0>
//                  -1 :    error;
//*************************************************************
int
ul_gethostbyteorder()
{
	union {
		short s;
		char c[sizeof(short)];
	} un;
	un.s = 0x0102;
	if (sizeof(short) == 2) {
		if (un.c[0] == 1 && un.c[1] == 2) {
			return BIG_ENDIAN;
		} else if (un.c[0] == 2 && un.c[1] == 1) {
			return LITTLE_ENDIAN;
		} else {
			return -1;
		}
	} else {
		return -1;
	}
}

//*******************************************************************
//  Select
//      a wrap of unix select
//      return -1 on error, 0 on timedout, fd# on success
//*******************************************************************
int
ul_select(int nfds, fd_set * readfds, fd_set * writefds, fd_set * exceptfds,
		  struct timeval *timeout)
{
	int val;
  again:
	val = select(nfds, readfds, writefds, exceptfds, timeout);
	if (val < 0) {
		if (errno == EINTR) {
			if (timeout != NULL) {
				/*            ul_writelog(
				 * UL_LOG_WARNING,
				 * "select() call error.error is EINTR.%d:%d",
				 * timeout->tv_sec,timeout->tv_usec); */
			}
			goto again;
		}
		ul_writelog(UL_LOG_WARNING, "select() call error.error[%d] info is %s", errno,
					strerror(errno));
	}

	if (val == 0) {
		errno = ETIMEDOUT;
	}

	return (val);
}

int
ul_poll(struct pollfd *fdarray, unsigned int nfds, int timeout)
{

	int val, otimeout = timeout;
	struct timeval ftv, btv;
	gettimeofday(&ftv, NULL);
	for (;;) {
		val = poll(fdarray, nfds, timeout);
		if (val < 0) {
			if (errno == EINTR) {
				if ( otimeout >= 0 ) {
					gettimeofday(&btv, NULL);
					timeout = otimeout - ((btv.tv_sec - ftv.tv_sec) * 1000 + (btv.tv_usec - ftv.tv_usec) / 1000);
					if ( timeout <= 0 ) {
						val = 0;
						errno = ETIMEDOUT;
						break;
					}
				}
				continue;
			}
			ul_writelog(UL_LOG_WARNING, "poll() call error.error[%d] info is %s", errno,
						strerror(errno));
		}
		else if (val == 0) {
			errno = ETIMEDOUT;
		}
		return (val);
	}
	return (val);
}


//******************************************************
// Operate the receive buff of socket.
// para:
//      s:the socket id;
//      optval: the address which store the buffer size.
//      mode:   operate mode.one is UL_GET and the other is UL_SET.
//******************************************************
int
ul_opsockopt_rcvbuf(int s, void *optval, char mode)
{
	int val = 0, oplen = sizeof(int);
	if (s <= 0 || (mode != UL_GET && mode != UL_SET)) {
		return -1;
	}
	switch (mode) {
	case UL_GET:
		val = ul_getsockopt(s, SOL_SOCKET, SO_RCVBUF, optval, (socklen_t *) & oplen);
		break;
	case UL_SET:
		val = ul_setsockopt(s, SOL_SOCKET, SO_RCVBUF, optval, (socklen_t) oplen);
		break;
	}
	return val;
}

//******************************************************
// Operate the send buff of socket.
// para:
//      s:the socket id;
//      optval: the address which store the buffer size.
//      mode:   operate mode.one is UL_GET and the other is UL_SET.
//******************************************************
int
ul_opsockopt_sndbuf(int s, void *optval, char mode)
{
	int val = 0, oplen = sizeof(int);
	if (s <= 0 || (mode != UL_GET && mode != UL_SET)) {
		return -1;
	}
	switch (mode) {
	case UL_GET:
		val = ul_getsockopt(s, SOL_SOCKET, SO_SNDBUF, optval, (socklen_t *) & oplen);
		break;
	case UL_SET:
		val = ul_setsockopt(s, SOL_SOCKET, SO_SNDBUF, optval, (socklen_t) oplen);
		break;
	}
	return val;
}

//******************************************************
// Operate the switch SO_REUSEADDR. Use this function you could 
// find the state<OPEN/ul_sclose> of this switch 
// para:
//      s:      the socket id;
//      optval: the address which store the operation.the relevant value is OPEN/ul_sclose
//      mode:   operate mode.one is GET and the other is SET.
//******************************************************
int
ul_opsockopt_ruseaddr(int s, int *op, char mode)
{
	int val = 0;
	int oplen = sizeof(int);
	if (s <= 0 || (mode != UL_GET && mode != UL_SET)) {
		return -1;
	}
	switch (mode) {
	case UL_GET:
		val = ul_getsockopt(s, SOL_SOCKET, SO_REUSEADDR, (void *) op, (socklen_t *) & oplen);
		break;
	case UL_SET:
		val = ul_setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (void *) op, (socklen_t) oplen);
		break;
	}
	return val;
}

#if SO_REUSEPORT
int
ul_opsockopt_ruseport(int s, int *op, char mode)
{
	int val = 0;
	int oplen = sizeof(int);
	if (s <= 0 || (mode != UL_GET && mode != UL_SET)) {
		return -1;
	}
	switch (mode) {
	case UL_GET:
		val = ul_getsockopt(s, SOL_SOCKET, SO_REUSEPORT, (void *) op, (socklen_t *) & oplen);
		break;
	case UL_SET:
		val = ul_setsockopt(s, SOL_SOCKET, SO_REUSEPORT, (void *) op, (socklen_t) oplen);
		break;
	}
	return val;
}
#endif

//******************************************************
// Operate the switch SO_KEEPALIVE. Use this function you could 
// find the state<OPEN/ul_sclose> of this switch 
// para:
//      s:      the socket id;
//      optval: the address which store the operation.the relevant
//              value is OPEN/ul_sclose
//      mode:   operate mode.one is GET and the other is SET.
//******************************************************
int
ul_opsockopt_keepalive(int s, int *op, char mode)
{
	int val = 0;
	int oplen = sizeof(int);
	if (s <= 0 || (mode != UL_GET && mode != UL_SET)) {
		return -1;
	}
	switch (mode) {
	case UL_GET:
		val = ul_getsockopt(s, SOL_SOCKET, SO_KEEPALIVE, (void *) op, (socklen_t *) & oplen);
		break;
	case UL_SET:
		val = ul_setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, (void *) op, (socklen_t) oplen);
		break;
	}
	return val;
}

//******************************************************
// /Get/Set socket read timeout 
// Operate the option SO_RCVTIMEO. Use this function you could
// find the current value of this option
// para:
//      sockfd:      the socket id;
//      struct timeval : the time struct which store the time.
//      mode:   operate mode.one is GET and the other is SET.
//******************************************************
int
ul_opsockopt_rcvtimeo(int sockfd, struct timeval *tv, char mode)
{
	int val = 0;
	int oplen = sizeof(struct timeval);

	if (sockfd <= 0 || (mode != UL_GET && mode != UL_SET)) {
		return -1;
	}
	switch (mode) {
	case UL_GET:
		val = ul_getsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, tv, (socklen_t *) & oplen);
		break;
	case UL_SET:
		val = ul_setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, tv, (socklen_t)oplen);
		break;
	}
	return val;
}

//******************************************************
// Operate the option SO_SNDTIMEO. Use this function you could
// find the current value of this option
// para:
//      sockfd:      the socket id;
//      struct timeval : the time struct which store the time.
//      mode:   operate mode.one is GET and the other is SET.
//******************************************************
int
ul_opsockopt_sndtimeo(int sockfd, struct timeval *tv, char mode)
{
	int val = 0;
	int oplen = sizeof(struct timeval);

	if (sockfd <= 0 || (mode != UL_GET && mode != UL_SET))
		return -1;
	switch (mode) {
	case UL_GET:
		val = ul_getsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, tv, (socklen_t *) & oplen);
		break;
	case UL_SET:
		val = ul_setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, tv, (socklen_t)oplen);
		break;
	}
	return val;
}

//********************************************************************
//  TcpListen:
//      Start a server on local machine at port number
//********************************************************************
int
ul_tcplisten(int port, int queue)
{
	int listenfd;
	const int on = 1;
	struct sockaddr_in soin;

	if ((listenfd = ul_socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		return -1;
	}

	ul_setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	bzero(&soin, sizeof(soin));
	soin.sin_family = AF_INET;
	soin.sin_addr.s_addr = htonl(INADDR_ANY);
	soin.sin_port = htons((uint16_t)port);
	if (ul_bind(listenfd, (struct sockaddr *) &soin, sizeof(soin)) < 0) {
		ul_sclose(listenfd);
		return -1;
	}

	(queue <= 0) ? queue = 5 : queue;
	if (ul_listen(listenfd, queue) < 0) {
		ul_sclose(listenfd);
		return -1;
	}

	return listenfd;
}

//********************************************************************
// Fill data struct <struct sockaddr_in> of IPV4.
//********************************************************************
int
ul_fillsa4(struct sockaddr_in *soin, char *ip, int port)
{
	bzero(soin, sizeof(struct sockaddr_in));
	soin->sin_family = AF_INET;
	if (ip == NULL) {
		soin->sin_addr.s_addr = htonl(INADDR_ANY);
	} else {
		if ((soin->sin_addr.s_addr = inet_addr(ip)) == INADDR_NONE) {
			return -1;
		}
	}
	soin->sin_port = htons((uint16_t)port);
	return 0;
}

//******************************************************
// Check if one descriptor is a socket 
// Return value: is -- 1 ; no -- 0 ; failed -- -1 .
//******************************************************
int
ul_issocket(int fd)
{
	struct stat buf;
	if (fstat(fd, &buf) < 0) {
		return -1;
	}
	if ((buf.st_mode & S_IFMT) == S_IFSOCK) {
		return 1;
	} else {
		return 0;
	}
}

/*
 */
int
ul_xconnecto_tv(int sockfd, const sockaddr * saptr, socklen_t socklen, timeval * tv, int isclose)
{
	int flags;
	int n, error;
	socklen_t len;

	if (sockfd <= 0 || saptr == (struct sockaddr *) 0) {
		if (sockfd > 0) {
			ul_sclose(sockfd);
		}
		return -1;
	}

	error = 0;
	flags = fcntl(sockfd, F_GETFL, 0);
	fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

	n = connect(sockfd, saptr, (socklen_t)socklen);
	if (n < 0) {
		if (errno != EINPROGRESS) {
			ul_writelog(UL_LOG_WARNING,
						"connect(%d,<%d,%d,%u>,%d) call failed.error[%d] info is %s.", sockfd,
						((struct sockaddr_in *) saptr)->sin_family,
						((struct sockaddr_in *) saptr)->sin_port,
						((struct sockaddr_in *) saptr)->sin_addr.s_addr, socklen, errno,
						strerror(errno));
			if (isclose) {
				ul_sclose(sockfd);
			}
			return (-1);
		}
	}
	if (n == 0) {
		goto done;
	}

	struct pollfd pfd;
	pfd.fd = sockfd;
	pfd.events = POLLIN | POLLOUT;
	if ( tv == NULL ) {
		n = ul_poll(&pfd, 1, -1);
	} else {
		struct timeval before, after;
		gettimeofday(&before, NULL);
		n = ul_poll(&pfd, 1, cnvt_timeval_ms(tv));
		gettimeofday(&after, NULL);

		tv->tv_sec = before.tv_sec + tv->tv_sec - after.tv_sec;
		tv->tv_usec = before.tv_usec + tv->tv_usec - after.tv_usec;
		while ( tv->tv_usec < 0 ) {
			tv->tv_sec = tv->tv_sec - 1;
			tv->tv_usec = 1000000 + tv->tv_usec;
		}
		if ( tv->tv_sec < 0 ) {
			tv->tv_sec = tv->tv_usec = 0;
		}

	}

	if (n == 0) {
		ul_sclose(sockfd);
		errno = ETIMEDOUT;
		return -1;
	}

	if ( (pfd.revents & POLLIN) || (pfd.revents & POLLOUT) ) {
		len = sizeof(error);
		if (ul_getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len) < 0) {
			if (isclose) {
				ul_sclose(sockfd);
			}
			return -1;
		}
	} else {
		if (isclose) {
			ul_sclose(sockfd);
		}
		return -1;
	}

  done:
	fcntl(sockfd, F_SETFL, flags);
	if (error) {
		ul_sclose(sockfd);
		errno = error;
		return -1;
	}
	return 0;
}

//****************************************************************
// ul_connecto_sclose_ms:
//   time out version of connect() .  
//   surport millisecond time out   
//   close socke while exit
//
//****************************************************************
int
ul_connecto_sclose_ms(int sockfd, const struct sockaddr *saptr, socklen_t socklen, int msecs)
{
	if (msecs <= 0) {
		return ul_xconnecto_tv(sockfd, saptr, socklen, NULL, 1);
	} else {
		timeval tv;
		tv.tv_sec = msecs / 1000;
		tv.tv_usec = (msecs % 1000) * 1000;
		return ul_xconnecto_tv(sockfd, saptr, socklen, &tv, 1);
	}
	return 0;
}

//****************************************************************
// ul_connecto():
//   time out version of connect().     
//****************************************************************
int
ul_connecto_ms_ex(int sockfd, const struct sockaddr *saptr, socklen_t socklen, int msecs)
{
	if (msecs <= 0) {
		return ul_xconnecto_tv(sockfd, saptr, socklen, NULL, 0);
	} else {
		timeval tv;
		tv.tv_sec = msecs / 1000;
		tv.tv_usec = (msecs % 1000) * 1000;
		return ul_xconnecto_tv(sockfd, saptr, socklen, &tv, 0);
	}
	return 0;
}


//*************************************************************
//  TcpConnect2
//      build up connection using sockaddr_in information
//      instead of host&port
//*************************************************************
int
ul_tcpconnect2(struct sockaddr_in *psin)
{
	int fd = ul_socket(AF_INET, SOCK_STREAM, 0);

	if (fd < 0) {
		return -1;
	}
	if (ul_connect(fd, (struct sockaddr *) psin, sizeof(struct sockaddr)) < 0) {
		ul_sclose(fd);
		return -1;
	}

	return fd;
}

//******************************************************
// Start a unix domain server on a path.
// Return value:
// On success,return a socket descriptor.Otherwise,return -1.
//*******************************************************
int
ul_tcpdomainlisten(char *path, int queue)
{
	int sockfd;
	struct sockaddr_un servaddr;

	if (path == NULL || path[0] == 0) {
		return -1;
	}
	unlink(path);

	sockfd = ul_socket(AF_LOCAL, SOCK_STREAM, 0);
	if (sockfd == -1) {
		return -1;
	}
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sun_family = AF_LOCAL;
	strncpy(servaddr.sun_path, path, 100);

	if (ul_bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) == -1) {
		ul_sclose(sockfd);
		return -1;
	}
	(queue <= 0) ? queue = 5 : queue;
	if (ul_listen(sockfd, queue) < 0) {
		ul_sclose(sockfd);
		return -1;
	}
	return sockfd;
}

//********************************************************************
// Bulid up a unix domain connection.
// On success,return the socket descriptor.Otherwise,return -1.
//********************************************************************
int
ul_tcpdomainconnect(char *path)
{
	int sockfd;
	struct sockaddr_un cliaddr;

	if ((sockfd = ul_socket(AF_LOCAL, SOCK_STREAM, 0)) == -1) {
		return -1;
	}
	bzero(&cliaddr, sizeof(cliaddr));
	cliaddr.sun_family = AF_LOCAL;
	strncpy(cliaddr.sun_path, path, 100);

	if (ul_connect(sockfd, (struct sockaddr *) &cliaddr, sizeof(cliaddr)) < 0) {
		ul_sclose(sockfd);
		return -1;
	}
	return sockfd;
}

//////////////////////////////////////////////////////////////////////////////
//                        Next is socket I/O funtions                       //
//////////////////////////////////////////////////////////////////////////////



//********************************************************************
//  Wrap function about recv()
//********************************************************************
ssize_t
ul_recv(int sockfd, void *buf, size_t len, int flags)
{
	int val = recv(sockfd, buf, len, flags);
	if (val == -1) {
		ul_writelog(UL_LOG_WARNING, "recv(%d,%ld,%d) call failed.error[%d] info is %s.",
					sockfd, (unsigned long) len, flags, errno, strerror(errno));
	}
	return val;
}

//********************************************************************
//  Wrap function about send()
//********************************************************************
ssize_t
ul_send(int sockfd, const void *buf, size_t len, int flags)
{
	int val = send(sockfd, buf, len, flags);
	if (val == -1) {
		ul_writelog(UL_LOG_WARNING, "send(%d,%ld,%d) call failed.error[%d] info is %s.",
					sockfd, (unsigned long) len, flags, errno, strerror(errno));
	}
	return val;
}

//********************************************************************
//  Readableo
//      return -1 on error, 0 on timeout, fd# otherwise
//********************************************************************


int
ul_sreadable_ms(int fd, int mseconds)
{
	struct pollfd pfd;
	pfd.fd = fd;
	pfd.events = POLLIN;

	if (mseconds >= 0) {
		return ul_poll(&pfd, 1, mseconds);
	} else {
		return ul_poll(&pfd, 1, -1);
	}
}

int
ul_sreadable_tv(int fd, struct timeval *tv)
{

	struct pollfd pfd;
	pfd.fd = fd;
	pfd.events = POLLIN;

	if ( tv == NULL ) {
		return ul_poll(&pfd, 1, -1);
	} else {

		struct timeval before, after;
		gettimeofday(&before, NULL);
		int result = (ul_poll(&pfd, 1, cnvt_timeval_ms(tv)));
		gettimeofday(&after, NULL);
		tv->tv_sec = before.tv_sec + tv->tv_sec - after.tv_sec;
		tv->tv_usec = before.tv_usec + tv->tv_usec - after.tv_usec; 
		while ( tv->tv_usec < 0 ) {
			tv->tv_sec = tv->tv_sec - 1;
			tv->tv_usec = 1000000 + tv->tv_usec;
		}
		if ( tv->tv_sec < 0 ) {
			tv->tv_sec = tv->tv_usec = 0;
		}
		return result;
	}

}

//********************************************************************
//  Writeable
//      return -1 on error, 0 on timeout, fd# otherwise
//********************************************************************

int
ul_swriteable_ms(int fd, int mseconds)
{

	struct pollfd pfd;
	pfd.fd = fd;
	pfd.events = POLLOUT;

	if ( mseconds < 0 ) {
		return ul_poll(&pfd, 1, -1);
	} else {
		return (ul_poll(&pfd, 1, mseconds));
	}

}

//********************************************************************
//  reado()
//      timed out version of Read
//      return -3 if select socket error.
//      return -2 if timeout
//      return -1 on read error, otherwise bytes # read
//********************************************************************
ssize_t
ul_reado_ms(int fd, void *ptr, size_t nbytes, int mseconds)
{
	int err;
	err = ul_sreadable_ms(fd, mseconds);
	if (err < 0) {
		return -3;
	} else if (err == 0) {
		return -2;
	} else {
		return read(fd, ptr, nbytes);
	}
}


ssize_t
ul_reado_tv(int fd, void *ptr, size_t nbytes, struct timeval * tv)
{
	int err;
	err = ul_sreadable_tv(fd, tv);
	if (err < 0) {
		return -3;
	} else if (err == 0) {
		return -2;
	} else {
		return read(fd, ptr, nbytes);
	}
}

//********************************************************************
//  Reado()
//      timed out version of Read, exact time out
//      return -1 on error, otherwise bytes # read
//********************************************************************

ssize_t
ul_sreado_ms_ex(int fd, void *ptr1, size_t nbytes, int msecs)
{
	ssize_t nread;
	size_t nleft = nbytes;
	char *ptr = (char *) ptr1;
	struct timeval tv;
	struct timeval *tp;

	if (msecs < 0) {
		tp = NULL;
	} else {
		cnvt_ms_timeval(msecs, &tv);
		tp = &tv;
	}


	while (nleft > 0) {
	  again:
		nread = ul_reado_tv(fd, ptr, nleft, tp);
		if (nread < 0) {
			if ((nread == -1) && (errno == EINTR)) {
				goto again;
			} else if (nread == -2) {
				errno = ETIMEDOUT;
				return -1;
			} else if (nread == -3) {
				errno = EIO;
				return -1;
			} else {
				return -1;
			}
		} else if (nread == 0) {
			break;
		} else {
			ptr += nread;
			nleft -= nread;
		}
	}
	return (ssize_t)(nbytes - nleft);
}


int
ul_getsockflag(int sd)
{
	int flag = -1;

	while ((flag = fcntl(sd, F_GETFL, 0)) == -1) {
		if (errno == EINTR) {
			continue;
		}
		ul_writelog(UL_LOG_WARNING, "fcntl(%d,F_GETFL) call failed.error[%d] info is %s.", sd,
					errno, strerror(errno));
		return -1;
	}

	return flag;
}

int
ul_swriteable_tv(int fd, struct timeval *tv)
{
	struct pollfd pfd;
	pfd.fd = fd;
	pfd.events = POLLOUT;
	if ( tv == NULL ) {
		return ul_poll(&pfd, 1, -1);
	} else {
		struct timeval before, after;
		gettimeofday(&before, NULL);
		int result = ul_poll(&pfd, 1, cnvt_timeval_ms(tv));
		gettimeofday(&after, NULL);
		tv->tv_sec = before.tv_sec + tv->tv_sec - after.tv_sec;
		tv->tv_usec = before.tv_usec + tv->tv_usec - after.tv_usec;
		while ( tv->tv_usec < 0 ) {
			tv->tv_sec = tv->tv_sec - 1;
			tv->tv_usec = 1000000 + tv->tv_usec;
		}
		if ( tv->tv_sec < 0 ) {
			tv->tv_sec = tv->tv_usec = 0;
		}
		return result;
	}
}

ssize_t
ul_writeo_tv(int fd, void *ptr, size_t nbytes, struct timeval * tv)
{
	int err;
	err = ul_swriteable_tv(fd, tv);
	if (err < 0) {
		return -1;
	} else if (err == 0) {
		return -2;
	}

	return write(fd, ptr, nbytes);
}

ssize_t
ul_swriteo_ms_ex(int fd, void *ptr1, size_t nbytes, int msecs)
{
	int sockflag;
	ssize_t n;
	size_t nleft = nbytes;
	char *ptr = (char *) ptr1;
	struct timeval tv;
	struct timeval *tp;

	if (msecs < 0) {
		tp = NULL;
	} else {
		cnvt_ms_timeval(msecs, &tv);
		tp = &tv;
	}

	sockflag = ul_getsockflag(fd);
	if (sockflag < 0) {
		return -1;
	}

	if (!(sockflag & O_NONBLOCK)) {
		if (ul_setsocktonoblock(fd) < 0) {
			return -1;
		}
	}

	while (nleft > 0) {
		n = ul_writeo_tv(fd, ptr, (size_t)nleft, tp);
		if ((n == -1) && (errno == EINTR)) {
			continue;
		}
		if (n <= 0) {
			if (n == -2) {
				errno = ETIMEDOUT;
			}

			if (!(sockflag & O_NONBLOCK)) {
				ul_setsocktoblock(fd);
			}

			return -1;
		}
		nleft -= n;
		ptr += n;
	}
	if (!(sockflag & O_NONBLOCK)) {
		ul_setsocktoblock(fd);
	}
	return (ssize_t)(nbytes - nleft);
}




//*****************************************************
//  ul_readlineo()
//      Read information from socket one line once time with timeout mode.
//*****************************************************
size_t
ul_readlineo_ms(int fd, void *vptr, size_t maxlen, int msecs)
{
	ssize_t n;
	ssize_t rc;
	char c;
	char *ptr;

	ptr = (char *) vptr;
	for (n = 1; n < (int) maxlen; n++) {
	  again:
		if ((rc = ul_sreado_ms_ex(fd, &c, 1, msecs)) == 1) {
			*ptr++ = c;
			if (c == '\n')
				break;
		} else if (rc == 0) {
			if (n == 1)
				return (0);
			else
				break;
		} else {
			if ((rc == -1) && (errno == EINTR))
				goto again;
			return (size_t)rc;
		}
	}
	*ptr = 0;
	return (size_t)n;
}

size_t
ul_readlineo(int fd, void *vptr, size_t maxlen, int secs)
{
	return ul_readlineo_ms(fd, vptr, maxlen, secs * 1000);
}

//*******************************************************************
//  Readline().
//      Read one line from sockid once time. This is the speedup code .
//  This code is mutithread safe.
//*******************************************************************


ssize_t
ul_myread_tv(int fd, char *ptr, Rline * rlin, struct timeval * tv)
{
	if (rlin->rl_cnt <= 0) {
	  again:
		rlin->rl_cnt = ul_reado_tv(fd, rlin->rl_buf, sizeof(rlin->rl_buf), tv);
		if (rlin->rl_cnt < 0) {
			if (errno == EINTR)
				goto again;
			return -1;
		} else if (rlin->rl_cnt == 0)
			return 0;
		rlin->read_ptr = rlin->rl_buf;
	}
	rlin->rl_cnt--;
	*ptr = *rlin->read_ptr++;
	return 1;
}


ssize_t
ul_readline_speedo_ms_ex(int fd, void *vptr, size_t maxlen, Rline * rlin, int msecs)
{
	ssize_t n;
	ssize_t rc;
	char c;
	char *ptr;
	struct timeval tv;
	struct timeval *tp;

	if (msecs <= 0)
		tp = NULL;
	else {
		cnvt_ms_timeval(msecs, &tv);
		tp = &tv;
	}


	ptr = (char *) vptr;
	for (n = 1; n < (int) maxlen; n++) {
	  again:
		if ((rc = ul_myread_tv(fd, &c, rlin, tp)) == 1) {
			*ptr++ = c;
			if (c == '\n')
				break;
		} else if (rc == 0) {
			if (n == 1)
				return (0);
			else
				break;
		} else {
			if (errno == EINTR)
				goto again;
			return -1;
		}
	}
	*ptr = 0;
	return n;
}

//******************************************************* 
// ul_sclose one socket and force FIN created and sent whenever this socket was
// been open by other process.
// argument howto include:
//              SHUT_RD<0>//SHUT_WR<1>//SHUT_RDWR<2>
//*******************************************************
int
ul_shutdown(int sockid, int howto)
{
	int val = 0;
	if ((val = shutdown(sockid, howto)) == -1)
		ul_writelog(UL_LOG_WARNING, "shutdown(%d,%d) call failed.error[%d] info is %s.", sockid,
					howto, errno, strerror(errno));
	return val;
}




//********************************************************
// Set a socket descriptor's flag about block
// Return value: 
//      0:success;      -1:failed.
//********************************************************
int
ul_setsocktoblock(int sd)
{
	int val = 0;
	int flags, flag_t;
	while ((flags = fcntl(sd, F_GETFL, 0)) == -1) {
		if (errno == EINTR)
			continue;
		ul_writelog(UL_LOG_WARNING, "fcntl(%d,F_GETFL) call failed.error[%d] info is %s.", sd,
					errno, strerror(errno));
		return -1;
	}
	flag_t = flags;
	if (!(flag_t & O_NONBLOCK))
		return 0;
	flags &= ~O_NONBLOCK;
	while ((val = fcntl(sd, F_SETFL, flags)) == -1) {
		if (errno == EINTR)
			continue;
		ul_writelog(UL_LOG_WARNING, "fcntl(%d,F_SETFL,%d) call failed.error[%d] info is %s.", sd,
					flags, errno, strerror(errno));
		return -1;
	}
	return val;
}


//********************************************************
// Set a socket descriptor's flag about non_block
// Return value: 
//      0:success;      -1:failed.
//********************************************************
int
ul_setsocktonoblock(int sd)
{
	int val = 0;
	int flags, flag_t;
	while ((flags = fcntl(sd, F_GETFL, 0)) == -1) {
		if (errno == EINTR)
			continue;
		ul_writelog(UL_LOG_WARNING, "fcntl(%d,F_GETFL) call failed.error[%d] info is %s.", sd,
					errno, strerror(errno));
		return -1;
	}
	flag_t = flags;
	if (flag_t & O_NONBLOCK)
		return 0;
	flags |= O_NONBLOCK;
	while ((val = fcntl(sd, F_SETFL, flags)) == -1) {
		if (errno == EINTR)
			continue;
		ul_writelog(UL_LOG_WARNING, "fcntl(%d,F_SETFL,%d) call failed.error[%d] info is %s.", sd,
					flags, errno, strerror(errno));
		return -1;
	}
	return val;
}


//**************************************************************
// Check the size of data in receive queue.
// Return value:   
//  on success,return the data size.Otherwise return -1 
//**************************************************************
ssize_t
ul_getrecvqueuesize(int sd)
{
	ssize_t val = 0;
	char buf[65536];
	int flags = MSG_DONTWAIT | MSG_PEEK;
	while ((val = recv(sd, buf, 65535, flags)) == -1 && errno == EINTR);
	return val;
}

ssize_t 
ul_sreado_ms_ex2(int sock, void *ptr, size_t nbytes, int msecs)
{
    struct timeval tv;
    struct timeval old_tv;
    ssize_t nread;
    int sockflag;
    int ret;
    int timeuse = 0;
    socklen_t oplen = (socklen_t)sizeof(tv);
    
    nread = recv(sock, ptr, nbytes, MSG_DONTWAIT);
    UL_RETURN_VAL_IF(nread==(ssize_t)nbytes, nread);
    UL_RETURN_VAL_IF((nread<0)&&(errno!=EAGAIN)&&(errno!=EINTR), -1);
    if (nread < 0) {
        nread = 0;
    }

    //0的时候不希望会被堵塞住
    if (0 == msecs) {
        errno = ETIMEDOUT;
        return -1;
    }
    //负数的时候我们要堵塞
    if (msecs < 0) {
       msecs = INT_MAX; 
    }

    UL_RETURN_VAL_IF(getsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &old_tv, &oplen)<0, -1);
    UL_RETURN_VAL_IF((sockflag = fcntl(sock, F_GETFL, 0))<0, -1);
    if (sockflag&O_NONBLOCK) {
        UL_RETURN_VAL_IF(fcntl(sock, F_SETFL, (sockflag)&(~O_NONBLOCK))<0, -1);
    }
    struct timeval cur;
    struct timeval last;
    do {
      tv.tv_sec = msecs/1000;
      tv.tv_usec = (msecs%1000)*1000;
      UL_RETURN_VAL_IF(setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, oplen)<0, -1); 
      gettimeofday(&cur, NULL);
      do {
        ret = recv(sock, (char*)ptr+nread, nbytes-(size_t)nread, MSG_WAITALL); 
      } while (ret < 0 && errno == EINTR);
      if (ret<0 && EAGAIN==errno) {
        //没数据，超时了
        errno = ETIMEDOUT;
      }
      if (ret>0 && nread + ret!=(ssize_t)nbytes) {
        gettimeofday(&last, NULL);
        //判断是不是真的超时了
        timeuse = ((last.tv_usec - cur.tv_usec)/1000+(last.tv_sec - cur.tv_sec)*1000);
        if (timeuse >= msecs) {
          //真超时了
          errno = ETIMEDOUT;
          ret = -1;
          nread = -1;
        } else {
          //被中断了？继续读
          nread += ret;
        }
        
      } else if (ret < 0) {
        nread = -1;
      } else {
        nread += ret;
      }
      //没写完，继续
    } while (ret > 0 && nbytes != (size_t)nread);
    if (sockflag&O_NONBLOCK) {
        UL_RETURN_VAL_IF(fcntl(sock, F_SETFL, sockflag)<0, -1);
    }
    UL_RETURN_VAL_IF(setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &old_tv, oplen)<0, -1);
    return nread;
}


ssize_t 
ul_swriteo_ms_ex2(int sock, const void *ptr, size_t nbytes, int msecs)
{
    struct timeval tv;
    struct timeval old_tv;
    ssize_t nwrite = 0;
    int ret = 0;
    int sockflag = 0;
    int timeuse = 0;
    socklen_t oplen = sizeof(tv);

    nwrite = send(sock, ptr, nbytes, MSG_DONTWAIT);
    UL_RETURN_VAL_IF((nwrite==(ssize_t)nbytes), nwrite);
    UL_RETURN_VAL_IF((nwrite<0)&&(errno!=EAGAIN)&&(errno!=EWOULDBLOCK)&&(errno!=EINTR), -1);
    if (nwrite < 0) {
        nwrite = 0;
    }

    //0的时候不希望会被堵塞住
    if (0 == msecs) {
        errno = ETIMEDOUT;
        return -1;
    }
    //负数的时候我们要堵塞
    if (msecs < 0) {
       msecs = INT_MAX; 
    }

    UL_RETURN_VAL_IF(getsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &old_tv, &oplen)<0, -1);
    UL_RETURN_VAL_IF((sockflag = fcntl(sock, F_GETFL, 0))<0, -1);
    if (sockflag&O_NONBLOCK) {
        UL_RETURN_VAL_IF(fcntl(sock, F_SETFL, (sockflag)&(~O_NONBLOCK))<0, -1);
    }
    oplen = sizeof(tv);
    struct timeval cur;
    struct timeval last;
    do {
        tv.tv_sec = msecs/1000;
        tv.tv_usec = (msecs%1000)*1000;
        //set都已经失败了,对sock的其它操作已经没有多大意义了
        gettimeofday(&cur, NULL);
        UL_RETURN_VAL_IF(setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &tv, oplen)<0, -1); 
        do { 
            ret = send(sock, (char*)ptr + nwrite, nbytes-(size_t)nwrite, MSG_WAITALL); 
        } while (ret < 0 && EINTR == errno);

        if (ret > 0 && nwrite+ret < (ssize_t)nbytes) {
            //判断是否真是超时
            gettimeofday(&last, NULL);
            timeuse = ((last.tv_usec - cur.tv_usec)/1000+(last.tv_sec - cur.tv_sec)*1000);
            if (timeuse >= msecs) {
                //真的超时了
                errno = ETIMEDOUT;
                ret = -1;
                nwrite = -1;
            } else {
                //不是超时，一般是被中断了, 继续写
                msecs -= timeuse;
                nwrite += ret;

            }
        } else if (ret < 0) {
            nwrite = -1;
        } else {
            nwrite += ret;
        }
        //没搞定继续写
    } while (ret > 0 && nbytes != (size_t)nwrite);
    if (sockflag & O_NONBLOCK) {
        UL_RETURN_VAL_IF(fcntl(sock, F_SETFL, sockflag)<0, -1);
    }
    UL_RETURN_VAL_IF(setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &old_tv, oplen)<0, -1);
    
    return nwrite;
}

/**
 *
 * 由于每次读或写不能保证一次处理完毕,需要根据结果再对struct iovec*
 * 进行处理,以便再次读写
 *
**/
static struct iovec *
iovec_offset(struct iovec *ve, int *count, size_t offset)
{
    struct iovec *sve = ve;
    size_t n = offset;
    UL_RETURN_VAL_IF(count != NULL && *count <= 0, NULL);
    while (n >= sve[0].iov_len) {
        n -= sve[0].iov_len;
        sve++;
        (*count)--;
    }
    if (n > 0) {
        sve[0].iov_base = ((char*)sve[0].iov_base) + n; 
        sve[0].iov_len -= n;
    }
    return sve;
}

ssize_t 
ul_sreadv_ms(int sock, const struct iovec *ve, int count, int msecs)
{
    struct timeval tv;
    struct timeval old_tv;
    ssize_t nread;
    ssize_t nleft = 0;
    int sockflag;
    ssize_t nbytes = 0;
    socklen_t oplen = sizeof(tv);
    struct msghdr msg;
    struct timeval cur;
    struct timeval last; 
    struct iovec *sve = NULL;
    int timeuse = 0;
    int scount = count;
    if (0 == msecs) { //0的时候不希望会被堵塞住
        errno = ETIMEDOUT;
        return -1;
    }
    if (msecs < 0) { //负数的时候我们要堵塞
       msecs = INT_MAX; 
    }
    if (count >= UIO_MAXIOV || 0 == count) { //最大值限制
       errno = EINVAL;
       return -1;
    }
    for (int i = 0; i < count; ++i) { //计算总的长度
       nleft += ve[i].iov_len;
    }
    nbytes = (size_t)nleft;
    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    msg.msg_control = NULL;
    msg.msg_controllen = 0;
    msg.msg_flags = MSG_DONTWAIT;
    msg.msg_iov = (struct iovec*)ve;
    msg.msg_iovlen = scount;
    nread = recvmsg(sock, &msg, MSG_DONTWAIT); //先非堵塞读一下
    UL_RETURN_VAL_IF((nread==(ssize_t)nbytes), nread);
    UL_RETURN_VAL_IF((nread<0)&&(errno!=EAGAIN)&&(errno!=EINTR), -1);
    msg.msg_flags = MSG_WAITALL;//改堵塞
    if (nread < 0) {
        nread = 0;
    }
    nleft -= nread;
    struct iovec vec[scount]; //前面做了count的限制,所以这里是安全的
    sve = vec;
    //一次写不完 会被修改 这里copy一份 这里ve不会是NULL, 如果是NULL sendmsg会报错
    memcpy(sve, ve, sizeof(struct iovec)*scount);
    sve = iovec_offset(sve, &scount, nread);//调整
    UL_RETURN_VAL_IF(getsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &old_tv, &oplen)<0, -1);
    UL_RETURN_VAL_IF((sockflag = fcntl(sock, F_GETFL, 0))<0, -1);
    if (sockflag&O_NONBLOCK) {
        UL_RETURN_VAL_IF(fcntl(sock, F_SETFL, (sockflag)&(~O_NONBLOCK))<0, -1);
    }
    do {
        tv.tv_sec = msecs/1000;
        tv.tv_usec = (msecs%1000)*1000;
        UL_RETURN_VAL_IF(setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, oplen)<0, -1); 
        msg.msg_iov = sve;
        msg.msg_iovlen = scount;
        gettimeofday(&cur, NULL);
        do {
            nread = recvmsg(sock, &msg, MSG_WAITALL); 
        } while (nread < 0 && EINTR == errno);
        if (0 == nread) {
          break;
        }
        if (-1 == nread && EAGAIN == errno) {
            errno = ETIMEDOUT;
        }
        if (nread>0) {
            nleft -= nread;
            if (nleft > 0) { //先判断是不是真超时
                gettimeofday(&last, NULL);
                timeuse = ((last.tv_usec - cur.tv_usec)/1000+(last.tv_sec - cur.tv_sec)*1000);
                if (timeuse >= msecs) { //真超时了,
                    errno = ETIMEDOUT;
                    nread = -1;
                } else {
                    msecs -= timeuse;
                    //没有超时，是被中断了，处理的操作复杂了一些
                    if (nread > 0) {
                        sve = iovec_offset(sve, &scount, nread);
                    }
                }
            }
        }
    } while(nleft > 0 && nread >= 0);
    if (sockflag & O_NONBLOCK) {
        UL_RETURN_VAL_IF(fcntl(sock, F_SETFL, sockflag)<0, -1);
    }
    UL_RETURN_VAL_IF(setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &old_tv, oplen)<0, -1);
    if (nread >= 0) {
        return nbytes - nleft;
    }
    return nread;
}


ssize_t 
ul_swritev_ms(int sock, const struct iovec *ve, int count, int msecs)
{
    struct timeval tv;
    struct timeval old_tv;
    ssize_t nwrite;
    size_t nbytes = 0;
    ssize_t nleft = 0; 
    struct timeval cur;
    struct timeval last; 
    struct iovec *sve = NULL; 
    socklen_t oplen = sizeof(tv);
    int sockflag;
    struct msghdr msg;
    int scount = count;
    int timeuse = 0;
    //0的时候不希望会被堵塞住
    if (0 == msecs) {
        errno = ETIMEDOUT;
        return -1;
    }
    //负数的时候我们要堵塞
    if (msecs < 0) {
       msecs = INT_MAX; 
    }
    if (count >= UIO_MAXIOV || 0 == count) {
       errno = EINVAL;
       return -1;
    }
    //计算总的长度
    for (int i = 0; i < count; ++i) {
       nleft += ve[i].iov_len;
    }
    nbytes = (size_t)nleft;
    //先非堵塞写一下
    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    msg.msg_control = NULL;
    msg.msg_controllen = 0;
    msg.msg_flags = MSG_DONTWAIT;
    msg.msg_iov = (struct iovec*)ve;
    msg.msg_iovlen = scount;
    nwrite = sendmsg(sock, &msg, MSG_DONTWAIT);
    UL_RETURN_VAL_IF((nwrite==(ssize_t)nbytes), nwrite);
    UL_RETURN_VAL_IF((nwrite<0)&&(errno!=EAGAIN)&&(errno!=EWOULDBLOCK)&&(errno!=EINTR), -1);
    msg.msg_flags = MSG_WAITALL;//改堵塞
    if (nwrite < 0) {
        nwrite = 0;
    }
    nleft -= nwrite;
    struct iovec vec[scount]; //前面做了count的限制,所以这里是安全的
    sve = vec;
    //一次写不完
    //会被修改 这里copy一份
    //这里ve不会是NULL, 如果是NULL sendmsg会报错
    memcpy(sve, ve, sizeof(struct iovec)*scount);
    sve = iovec_offset(sve, &scount, nwrite);//调整
    UL_RETURN_VAL_IF(getsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &old_tv, &oplen)<0, -1);
    UL_RETURN_VAL_IF((sockflag = fcntl(sock, F_GETFL, 0))<0, -1);
    if (sockflag&O_NONBLOCK) {
        UL_RETURN_VAL_IF(fcntl(sock, F_SETFL, (sockflag)&(~O_NONBLOCK))<0, -1);
    }
    do {
        tv.tv_sec = msecs/1000;
        tv.tv_usec = (msecs%1000)*1000;
        UL_RETURN_VAL_IF(setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &tv, oplen)<0, -1);
        msg.msg_iov = sve;
        msg.msg_iovlen = scount;
        gettimeofday(&cur, NULL);
        do {
            nwrite = sendmsg(sock, &msg, MSG_WAITALL);
        } while (nwrite < 0 && EINTR == errno);
        if (0 == nwrite) {
            break;
        }
        if (nwrite>0) {
            nleft -= nwrite;
            if (nleft > 0) {
                //先判断是不是真超时
                gettimeofday(&last, NULL);
                timeuse = ((last.tv_usec - cur.tv_usec)/1000+(last.tv_sec - cur.tv_sec)*1000);
                if (timeuse >= msecs) {
                    //真超时了,
                    errno = ETIMEDOUT;
                    nwrite = -1;
                } else {
                    msecs -= timeuse;
                    sve = iovec_offset(sve, &scount, nwrite);
                }
            }
        }
    } while(nleft > 0 && nwrite >= 0);
    if (sockflag & O_NONBLOCK) {
        UL_RETURN_VAL_IF(fcntl(sock, F_SETFL, sockflag)<0, -1);
    }
    UL_RETURN_VAL_IF(setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &old_tv, oplen)<0, -1);
    if (nwrite >= 0) {
        return nbytes - nleft;
    }
    return nwrite;
}

