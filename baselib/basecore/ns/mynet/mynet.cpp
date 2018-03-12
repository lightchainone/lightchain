

#include "mynet.h"


static int
Connect(int sockfd, const struct sockaddr *serv_addr, socklen_t addrlen)
{
    assert(serv_addr != NULL);

    return connect(sockfd, serv_addr, addrlen);
}



int
Close(int fd)
{
    return close(fd);
}



int
MyConnectO(const char *host, int port, int *timeout, const char *name)
{
    char buf[1024];
    fd_set rset, wset;
    struct hostent he, *p;
    struct sockaddr_in sin;
    socklen_t len = 0;
    int sock = -1, ret = 0, err = 0, flags = 0, on = 1;
    struct timeval t;

    assert(host != NULL);

    if (*timeout < 0)
	*timeout = 0;
    int old_timeout = *timeout;

    
    if (host[0] == 0 || port <= 0 || port > 65535) {
	ul_writelog(UL_LOG_WARNING,
		    "find invalid host %s or port %d while connecting to %s",
		    host, port, name);
	return -1;
    }

    if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
	ul_writelog(UL_LOG_WARNING,
		    "cannot create socket while connecting to %s(%s:%d)",
		    name, host, port);
	return -1;
    }

    bzero(&sin, sizeof(sin));
    sin.sin_addr.s_addr = inet_addr(host);
    if ((sin.sin_addr.s_addr) == INADDR_NONE) {	
	ret = gethostbyname_r(host, &he, buf, sizeof(buf), &p, &err);
	if (ret < 0 || err != 0 || p == NULL) {
	    ul_writelog(UL_LOG_WARNING,
			"cannot resolve hostname while connecting to %s(%s:%d)",
			name, host, port);
	    close(sock);
	    return -1;
	}
	memcpy(&sin.sin_addr.s_addr, he.h_addr, sizeof(sin.sin_addr.s_addr));
    }
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);

    flags = fcntl(sock, F_GETFL, 0);
    fcntl(sock, F_SETFL, flags | O_NONBLOCK);

    setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on));
    ret = Connect(sock, (struct sockaddr *) &sin, sizeof(sin));
    if (ret == 0) {
	fcntl(sock, F_SETFL, flags);
	ul_writelog(UL_LOG_TRACE, "connect to %s(%s:%d) OK", name, host,
		    port);
	return sock;
    } else {
	if (errno != EINPROGRESS) {	
	    ul_writelog(UL_LOG_WARNING, "fail while connecting to %s(%s:%d)",
			name, host, port);
	    close(sock);
	    return -1;
	}

	FD_ZERO(&rset);
	FD_SET(sock, &rset);
	wset = rset;
	t.tv_sec = *timeout / 1000000, t.tv_usec = *timeout % 1000000;
	ret = select(sock + 1, &rset, &wset, NULL, &t);
	if (t.tv_sec * 1000000 + t.tv_usec < 10)
	    t.tv_sec = 0, t.tv_usec = 0;
	*timeout = t.tv_sec * 1000000 + t.tv_usec;
	if (ret == 0) {
	    ul_writelog(UL_LOG_WARNING,
			"timeout(%d us) while connecting to %s(%s:%d)",
			old_timeout, name, host, port);
	    close(sock);
	    return -1;
	}

	if (FD_ISSET(sock, &rset) || FD_ISSET(sock, &wset)) {
	    len = sizeof(err);
	    ret = getsockopt(sock, SOL_SOCKET, SO_ERROR, &err, &len);
	    if (ret < 0) {
		if (err)
		    errno = err;
		ul_writelog(UL_LOG_WARNING,
			    "getsockopt fail while connecting to %s(%s:%d)",
			    name, host, port);
		close(sock);
		return -1;
	    }
	    if (err) {
		errno = err;
		ul_writelog(UL_LOG_WARNING,
			    "sockselect fail while connecting to %s(%s:%d)",
			    name, host, port);
		close(sock);
		return -1;
	    }
	    fcntl(sock, F_SETFL, flags);
	    ul_writelog(UL_LOG_TRACE, "connect to %s(%s:%d) OK", name, host,
			port);
	    return sock;
	} else {
	    ul_writelog(UL_LOG_WARNING,
			"select fail while connecting to %s(%s:%d)", name,
			host, port);
	    close(sock);
	    return -1;
	}
    }

    return -1;
}


int
MyReadO(int sock, void *buf, ssize_t len, int *timeout, const char *name)
{
    int ret;
    struct timeval t;
    fd_set rset;
    ssize_t nleft, nread;
    char *ptr;

    if (*timeout < 0)
	*timeout = 0;
    int old_timeout = *timeout;

    
    if (sock < 0) {
	ul_writelog(UL_LOG_WARNING, "find invalid sock while read from %s:%d",
		    name, sock);
	return -1;
    }

    nleft = len, nread = 0, ptr = (char *) buf;
    t.tv_sec = *timeout / 1000000, t.tv_usec = *timeout % 1000000;

    while (nleft > 0) {
	
	FD_ZERO(&rset);
	FD_SET(sock, &rset);
	ret = select(sock + 1, &rset, NULL, NULL, &t);
	if (t.tv_sec * 1000000 + t.tv_usec < 10)
	    t.tv_sec = 0, t.tv_usec = 0;
	if (ret <= 0) {
	    if (ret != 0)
		ul_writelog(UL_LOG_WARNING,
			    "select fail while read from %s:%d", name, sock);
	    else
		ul_writelog(UL_LOG_WARNING,
			    "timeout(%d us) while read from %s:%d",
			    old_timeout, name, sock);
	    break;
	}
	
	nread = read(sock, ptr, nleft);
	if (nread < 0) {
	    if (errno == EINTR) {
		continue;
	    }
	    ul_writelog(UL_LOG_WARNING, "read fail from %s:%d", name, sock);
	    break;
	} else if (nread == 0) {
	    ul_writelog(UL_LOG_WARNING, "read fail from %s:%d(disconnect)",
			name, sock);
	    break;
	}
	ptr += nread;
	nleft -= nread;
    }
    *timeout = t.tv_sec * 1000000 + t.tv_usec;

    
    ul_writelog(UL_LOG_TRACE, "read %d bytes(want %d bytes) from %s:%d %s",
		int(len - nleft), (int)len, name, sock, (nleft == 0) ? "OK" : "FAIL");

    return (len - nleft);
}


int
MyReadOS(int sock, void *buf, ssize_t len, int *timeout, const char *name,
	 const char *stop)
{
    int ret;
    struct timeval t;
    fd_set rset;
    ssize_t nleft, nread;
    char *ptr;

    if (*timeout < 0)
	*timeout = 0;
    int old_timeout = *timeout;

    
    if (sock < 0) {
	ul_writelog(UL_LOG_WARNING, "find invalid sock while read from %s:%d",
		    name, sock);
	return -1;
    }

    nleft = len, nread = 0, ptr = (char *) buf;
    t.tv_sec = *timeout / 1000000, t.tv_usec = *timeout % 1000000;

    while (nleft > 0) {
	
	FD_ZERO(&rset);
	FD_SET(sock, &rset);
	ret = select(sock + 1, &rset, NULL, NULL, &t);
	if (t.tv_sec * 1000000 + t.tv_usec < 10)
	    t.tv_sec = 0, t.tv_usec = 0;
	if (ret <= 0) {
	    if (ret != 0)
		ul_writelog(UL_LOG_WARNING,
			    "select fail while read from %s:%d", name, sock);
	    else
		ul_writelog(UL_LOG_WARNING,
			    "timeout(%d us) while read from %s:%d",
			    old_timeout, name, sock);
	    break;
	}
	
	nread = read(sock, ptr, nleft);
	if (nread < 0) {
	    if (errno == EINTR) {
		continue;
	    }
	    ul_writelog(UL_LOG_WARNING, "read fail from %s:%d", name, sock);
	    break;
	} else if (nread == 0) {
	    ul_writelog(UL_LOG_WARNING, "read fail from %s:%d(disconnect)",
			name, sock);
	    break;
	}
	ptr += nread;
	nleft -= nread;

	
	if (stop != NULL && strstr((char *) buf, stop) != NULL) {
	    break;
	}
    }
    *timeout = t.tv_sec * 1000000 + t.tv_usec;

    
    ul_writelog(UL_LOG_TRACE, "read %d bytes(want %d bytes) from %s:%d %s",
		int(len - nleft), (int)len, name, sock, (nleft == 0) ? "OK" : "FAIL");

    return (len - nleft);
}


int
MyWriteO(int sock, void *buf, ssize_t len, int *timeout, const char *name)
{
    int ret;
    struct timeval t;
    fd_set wset;
    ssize_t nleft, nwrite;
    char *ptr;


    if (*timeout < 0)
	*timeout = 0;
    int old_timeout = *timeout;

    
    if (sock < 0) {
	ul_writelog(UL_LOG_WARNING, "find invalid sock while write to %s:%d",
		    name, sock);
	return -1;
    }

    nleft = len, nwrite = 0, ptr = (char *) buf;
    t.tv_sec = *timeout / 1000000, t.tv_usec = *timeout % 1000000;

    while (nleft > 0) {
	
	FD_ZERO(&wset);
	FD_SET(sock, &wset);
	ret = select(sock + 1, NULL, &wset, NULL, &t);
	if (t.tv_sec * 1000000 + t.tv_usec < 10)
	    t.tv_sec = 0, t.tv_usec = 0;
	if (ret <= 0) {
	    if (ret != 0)
		ul_writelog(UL_LOG_WARNING,
			    "select fail while write to %s:%d", name, sock);
	    else
		ul_writelog(UL_LOG_WARNING,
			    "timeout(%d us) while write to %s:%d",
			    old_timeout, name, sock);
	    return -1;
	}
	
	nwrite = write(sock, ptr, nleft);
	if (nwrite < 0) {
	    if (errno == EINTR) {
		continue;
	    }
	    ul_writelog(UL_LOG_WARNING, "write fail to %s:%d", name, sock);
	    break;
	} else if (nwrite == 0) {
	    ul_writelog(UL_LOG_WARNING, "write fail to %s:%d(disconnect)",
			name, sock);
	    break;
	}
	ptr += nwrite;
	nleft -= nwrite;
    }
    *timeout = t.tv_sec * 1000000 + t.tv_usec;

    
    ul_writelog(UL_LOG_TRACE, "write %d bytes(want %d bytes) to %s:%d %s",
		int(len - nleft), (int)len, name, sock, (nleft == 0) ? "OK" : "FAIL");
    return (len - nleft);
}


int
MyClose(int fd)
{
    ul_writelog(UL_LOG_TRACE, "close socket %d", fd);
    return Close(fd);
}


int
MyTcpListen(int port, int queue)
{
    int listenfd;
    const int on = 1;
    struct sockaddr_in sin;

    if ((listenfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
	ul_writelog(UL_LOG_TRACE,
		    "cannot create socket for MyTcpListen [%d:%d]", port,
		    queue);
	return -1;
    }

    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    bzero(&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_port = htons(port);
    if (bind(listenfd, (struct sockaddr *) &sin, sizeof(sin)) < 0) {
	close(listenfd);
	ul_writelog(UL_LOG_TRACE, "cannot bind socket to port [%d]", port);
	return -1;
    }

    (queue <= 0) ? queue = 5 : queue;
    if (listen(listenfd, queue) < 0) {
	close(listenfd);
	ul_writelog(UL_LOG_TRACE, "cannot listen on [%d:%d]", port, queue);
	return -1;
    }

    return listenfd;
}



