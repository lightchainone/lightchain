
#include <assert.h>

#include "ul_net.h"
#include "ul_log.h"
#include "ul_conf.h"



extern int ul_getsockopt(int sockfd, int level, int optname, void *optval, socklen_t * optlen);

//****************************************************************
// ul_connecto_sclose():
//   time out version of connect().     
//   close socke while exit
//****************************************************************
int
ul_connecto_sclose(int sockfd, const struct sockaddr *saptr, socklen_t socklen, int secs)
{
	int flags;
	int n, error;
	socklen_t len;

	if (sockfd <= 0 || saptr == (struct sockaddr *) 0) {
		if (sockfd > 0)
			ul_sclose(sockfd);
		return -1;
	}
	if (secs < 0)
		secs = 0;
	error = 0;
	flags = fcntl(sockfd, F_GETFL, 0);
	fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

	if ((n = connect(sockfd, saptr, socklen)) < 0) {
		if (errno != EINPROGRESS) {
			ul_writelog(UL_LOG_WARNING,
						"connect(%d,<%d,%d,%u>,%d) call failed.error[%d] info is %s.",
						sockfd, ((struct sockaddr_in *) saptr)->sin_family,
						((struct sockaddr_in *) saptr)->sin_port,
						((struct sockaddr_in *) saptr)->sin_addr.s_addr,
						socklen, errno, strerror(errno));
			ul_sclose(sockfd);
			return (-1);
		}
	}
	if (n == 0)
		goto done;

	struct pollfd pfd;
	pfd.fd = sockfd;
	pfd.events = POLLIN | POLLOUT;

	if ( (n = ul_poll(&pfd, 1, secs ? secs * 1000 : -1)) == 0 ) {
		ul_sclose(sockfd);
		errno = ETIMEDOUT;
		return -1;
	}

	if ( (pfd.revents & POLLIN) || (pfd.revents & POLLOUT) ) {
		len = sizeof(error);
		if (ul_getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len) < 0) {
			ul_sclose(sockfd);
			return -1;
		}
	} else {
		ul_sclose(sockfd);
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

//********************************************************************
//  Read:
//  Read in nbytes from fd into ptr in BLOCK mode
//  return bytes read, or -1 (in fact returned from read()) on error
//********************************************************************
ssize_t
ul_sread(int fd, void *ptr1, size_t nbytes)
{
	ssize_t nread, nleft = nbytes;
	char *ptr = (char *) ptr1;
	while (nleft > 0) {
	  again:
		nread = read(fd, ptr, nleft);
		//  fprintf(stderr,"RRR%d,LLL%d",nread,nleft);         
		if (nread < 0) {
			if (errno == EINTR)
				goto again;
			ul_writelog(UL_LOG_WARNING,
						"read from socket(%d,%ld) failed.error[%d] info is %s.",
						fd, (unsigned long) nbytes, errno, strerror(errno));
			return -1;
		} else if (nread == 0)
			break;
		ptr += nread;
		nleft -= nread;
	}
	return (nbytes - nleft);
}

//********************************************************************
//  Reado()
//      timed out version of Read
//      return -1 on error, otherwise bytes # read
//********************************************************************
ssize_t
ul_sreado_ms(int fd, void *ptr1, size_t nbytes, int msecs)
{
	ssize_t nread, nleft = nbytes;
	char *ptr = (char *) ptr1;

	while (nleft > 0) {
	  again:
		nread = ul_reado_ms(fd, ptr, nleft, msecs);
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
	return (nbytes - nleft);
}

ssize_t
ul_sreado(int fd, void *ptr1, size_t nbytes, int secs)
{
	return ul_sreado_ms(fd, ptr1, nbytes, secs * 1000);
}



ssize_t
ul_sreado_ex(int fd, void *ptr1, size_t nbytes, int secs)
{
	return ul_sreado_ms_ex(fd, ptr1, nbytes, secs * 1000);
}


//********************************************************************
//  Write:
//  Write nbytes from ptr into fd in BLOCK mode
//  return bytes written, or -1 on error
//********************************************************************
ssize_t
ul_swrite(int fd, void *ptr1, size_t nbytes)
{
	int n, nleft = (int) nbytes;
	char *ptr = (char *) ptr1;
	while (nleft > 0) {
		n = write(fd, ptr, nleft);
		if (n <= 0) {
			if (errno == EINTR)
				continue;
			ul_writelog(UL_LOG_WARNING,
						"write to socket(%d,%ld) failed.error[%d] info is %s.",
						fd, (unsigned long) nbytes, errno, strerror(errno));
			return n;
		}
		nleft -= n;
		ptr += n;
	}
	return (nbytes - nleft);
}



//********************************************************************
//  Writeo()
//      timed out version of Write
//      return -1 on error, otherwise bytes # written 
//********************************************************************
ssize_t
ul_swriteo_ms(int fd, void *ptr, size_t nbytes, int mseconds)
{
	int err;
	err = ul_swriteable_ms(fd, mseconds);
	if (err < 0)
		return -1;
	else if (err == 0)
		return 0;

	return ul_swrite(fd, ptr, nbytes);
}

ssize_t
ul_swriteo(int fd, void *ptr, size_t nbytes, int seconds)
{
	return ul_swriteo_ms(fd, ptr, nbytes, seconds * 1000);
}



ssize_t
ul_swriteo_ex(int fd, void *ptr, size_t nbytes, int seconds)
{
	return ul_swriteo_ms_ex(fd, ptr, nbytes, seconds * 1000);
}



ssize_t
ul_reado(int fd, void *ptr, size_t nbytes, int seconds)
{
	return ul_reado_ms(fd, ptr, nbytes, seconds * 1000);
}


//********************************************************************
//  writeo()
//      timed out version of Write
//      return -1 on error, otherwise bytes # written 
//********************************************************************
ssize_t
ul_writeo_ms(int fd, void *ptr, size_t nbytes, int mseconds)
{
	int err;
	err = ul_swriteable_ms(fd, mseconds);
	if (err < 0) {
		return -1;
	} else if (err == 0) {
		return 0;
	} else {
		return write(fd, ptr, nbytes);
	}
}
ssize_t
ul_writeo(int fd, void *ptr, size_t nbytes, int seconds)
{
	return ul_writeo_ms(fd, ptr, nbytes, seconds * 1000);
}

int
ul_sreadable(int fd, int seconds)
{
	return ul_sreadable_ms(fd, seconds * 1000);
}

int
ul_swriteable(int fd, int seconds)
{
	return ul_swriteable_ms(fd, seconds * 1000);
}

//****************************************************************
// ul_connecto():
//   time out version of connect().     
//****************************************************************
int
ul_connecto(int sockfd, const struct sockaddr *saptr, socklen_t socklen, int secs)
{
	int flags;
	int n, error;
	socklen_t len;

	if (sockfd <= 0 || saptr == (struct sockaddr *) 0) {
		return -1;
	}
	if (secs < 0) {
		secs = 0;
	}
	error = 0;
	flags = fcntl(sockfd, F_GETFL, 0);
	fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

	n = connect(sockfd, saptr, socklen);
	if (n < 0) {
		if (errno != EINPROGRESS) {
			ul_writelog(UL_LOG_WARNING,
						"connect(%d,<%d,%d,%u>,%d) call failed.error[%d] info is %s.", sockfd,
						((struct sockaddr_in *) saptr)->sin_family,
						((struct sockaddr_in *) saptr)->sin_port,
						((struct sockaddr_in *) saptr)->sin_addr.s_addr, socklen, errno,
						strerror(errno));
			return (-1);
		}
	}
	if (n == 0) {
		goto done;
	}

	struct pollfd pfd;
	pfd.fd = sockfd;
	pfd.events = POLLIN | POLLOUT;

	n = ul_poll(&pfd, 1, secs ? secs * 1000 : -1);
	if (n == 0) {
		ul_sclose(sockfd);
		errno = ETIMEDOUT;
		return -1;
	}

	if ( (pfd.revents & POLLIN) || (pfd.revents & POLLOUT) ) {
		len = sizeof(error);
		if (ul_getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len) < 0) {
			return -1;
		}
	} else {
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

//*************************************************
//  ul_sreadline()
//      Read bytes from socket one line once time.
//*************************************************
ssize_t
ul_readline(int fd, void *vptr, size_t maxlen)
{
	ssize_t n = 0;
	ssize_t rc;
	char c;
	char *ptr;

	ptr = (char *) vptr;
	for (n = 1; n < (int) maxlen; n++) {
	  again:
		if ((rc = read(fd, &c, 1)) == 1) {
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

ssize_t
ul_myread(int fd, char *ptr, Rline * rlin, int sec)
{
	if (rlin->rl_cnt <= 0) {
	  again:
		if (sec == 0)
			rlin->rl_cnt = read(fd, rlin->rl_buf, sizeof(rlin->rl_buf));
		else
			rlin->rl_cnt = ul_reado(fd, rlin->rl_buf, sizeof(rlin->rl_buf), sec);
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
ul_readline_speed(int fd, void *vptr, size_t maxlen, Rline * rlin)
{
	ssize_t n;
	ssize_t rc;
	char c;
	char *ptr;

	ptr = (char *) vptr;
	for (n = 1; n < (int) maxlen; n++) {
	  again:
		if ((rc = ul_myread(fd, &c, rlin, 0)) == 1) {
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




// Time out version of readline_speed()
ssize_t
ul_readline_speedo(int fd, void *vptr, size_t maxlen, Rline * rlin, int secs)
{
	ssize_t n;
	ssize_t rc;
	char c;
	char *ptr;

	if (secs < 0)
		secs = 0;
	ptr = (char *) vptr;
	for (n = 1; n < (int) maxlen; n++) {
	  again:
		if ((rc = ul_myread(fd, &c, rlin, secs)) == 1) {
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


//********************************************************************
//  TcpConnect
//      Build up a TCP connection to host at port.  This is used at
//      client part, returns the socket fd on success, or -1 on error
//********************************************************************
int
ul_tcpconnect(char *host, int port)
{
	int fd;
	struct sockaddr_in sin;
	char buf[8192];

	if ((fd = ul_socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		return -1;
	}
// resolv host to sockaddr_in
	bzero(&sin, sizeof(sin));

//try A.B.C.D numeric IP format address first
	if ((sin.sin_addr.s_addr = inet_addr(host)) == INADDR_NONE) {
//not numeric IP format, try text format
		struct hostent he, *p;
		int err, ret;
//here we use **_r format, 'coz it might be run under multithreading
//environment.
//However, some such functions are not standardized yet, so we need
//know OS platform we are on.
#ifdef LINUX
//LINUX: gethostbyname_r needs six parameters 
//it return 0 on success, otherwise -1 returned, and 
//the error code is in err:
// ERANGE:  buf size too small
// HOST_NOT_FOUND: Authoritive Answer Host not found
// TRY_AGAIN:   Non-Authoritive Host not found, or SERVERFAIL
// NO_RECOVERY: Non recoverable errors, FORMERR, REFUSED, NOTIMP
// NO_DATA:     Valid name, no data record of requested type
// NO_ADDRESS:  no address, look for MX record
		if ((ret = ul_gethostbyname_r(host, &he, buf, sizeof(buf), &p, &err)) < 0) {
			close(fd);
			return -1;			//Resolve failed! :(
		}
#elif defined SOLARIS
//NOTE: be sure to link libnsl.o while compiling
//     that is, use -lnsl in gcc command options
//SOLARIS: gethostbyname_r takes five parameters, 
//it returns the pointer to he in p on success, otherwise NULL
//the error code is also in err, read above comments
		p = gethostbyname_r(host, &he, buf, sizeof(buf), &err);
		if (!p) {
			close(fd);
			ul_writelog(UL_LOG_WARNING, "gethostbyname_r(%s) call failed.error[%d] info is %s",
						host, err, hstrerror(err));
			return -1;
		}
#endif
		memcpy(&sin.sin_addr.s_addr, he.h_addr, sizeof(sin.sin_addr.s_addr));
	}
// here we assume we are always under IPv4 environment.
// If one day shifted to other network environment, such as IPv6,
// following code should also be under modification
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
//connect
	if (ul_connect(fd, (struct sockaddr *) &sin, sizeof(sin)) < 0) {
		ul_sclose(fd);
		return -1;
	}
	return fd;
}


//********************************************************************
//  TcpConnecto:
//      Time out version.
//      Build up a TCP connection to host at port.  This is used at
//      client part, returns the socket fd on success, or -1 on error
//********************************************************************
extern int
ul_tcpconnecto(char *host, int port, int secs)
{
	int fd;
	struct sockaddr_in sin;
	char buf[8192];

	if ((fd = ul_socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		return -1;
	}
// resolv host to sockaddr_in
	bzero(&sin, sizeof(sin));

//try A.B.C.D numeric IP format address first
	if ((sin.sin_addr.s_addr = inet_addr(host)) == INADDR_NONE) {
//not numeric IP format, try text format
		struct hostent he, *p;
		int err, ret;
//here we use **_r format, 'coz it might be run under multithreading
//environment.
//However, some such functions are not standardized yet, so we need
//know OS platform we are on.
#ifdef LINUX
//LINUX: gethostbyname_r needs six parameters 
//it return 0 on success, otherwise -1 returned, and 
//the error code is in err:
// ERANGE:  buf size too small
// HOST_NOT_FOUND: Authoritive Answer Host not found
// TRY_AGAIN:   Non-Authoritive Host not found, or SERVERFAIL
// NO_RECOVERY: Non recoverable errors, FORMERR, REFUSED, NOTIMP
// NO_DATA:     Valid name, no data record of requested type
// NO_ADDRESS:  no address, look for MX record
		if ((ret = ul_gethostbyname_r(host, &he, buf, sizeof(buf), &p, &err)) < 0) {
			close(fd);
			return -1;			//Resolve failed! :(
		}
#elif defined SOLARIS
//NOTE: be sure to link libnsl.o while compiling
//     that is, use -lnsl in gcc command options
//SOLARIS: gethostbyname_r takes five parameters, 
//it returns the pointer to he in p on success, otherwise NULL
//the error code is also in err, read above comments
		p = gethostbyname_r(host, &he, buf, sizeof(buf), &err);
		if (!p) {
			ul_writelog(UL_LOG_WARNING, "gethostbyname_r(%s) call failed.error[%d] info is %s",
						host, err, hstrerror(err));
			close(fd);
			return -1;
		}
#endif
		memcpy(&sin.sin_addr.s_addr, he.h_addr, sizeof(sin.sin_addr.s_addr));
	}
// here we assume we are always under IPv4 environment.
// If one day shifted to other network environment, such as IPv6,
// following code should also be under modification
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
//connect
	if (ul_connecto_sclose(fd, (struct sockaddr *) &sin, sizeof(sin), secs) < 0) {
		// do not close sockfd,ul_connecto_sclose has closed it
		// ul_sclose(fd);
		// modified by qzd 2006/05/19
		return -1;
	}
	return fd;

}


//*******************************************************
// Get IP address of one host.  Return the h_addr item.
// Return value:    0 :success;
//                  -1:hostname is error.
//                  -2:gethostbyname() call is error. 
// Note:This function is not safe in MT-thread process
//******************************************************
int
ul_gethostipbyname(const char *hostname, struct sockaddr_in *sin)
{
	struct hostent *result;

	if (hostname == NULL || strlen(hostname) == 0)
		return -1;
	if ((result = gethostbyname(hostname)) == NULL) {
		ul_writelog(UL_LOG_WARNING, "gethostbyname(%s) call failed.error is %s.", hostname,
					hstrerror(h_errno));
		return -2;
	}
	memcpy(&sin->sin_addr.s_addr, result->h_addr, sizeof(sin->sin_addr.s_addr));
	return 0;
}


/* vim: set ts=4 sw=4 sts=4 tw=100 */
