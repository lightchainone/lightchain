

#ifndef  __UL_NET_DEP_H_
#define  __UL_NET_DEP_H_

#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <endian.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <netinet/in.h>

typedef struct Rline Rline;


/*
 * 这里的不推荐的函数都有更好的版本代替
 */

//****************************************************************
// function : a  time-out version of connect().
// while exit on error,close sockfd
// sockfd,saptr,socklen, as the same as connect
// secs, the ovet time, in second
//****************************************************************
/**
 *
 */
extern int ul_connecto_sclose(int sockfd, const struct sockaddr *saptr, socklen_t socklen, int secs);


/**
 * 从socket读取指定长度的数据，直到读够，或者socket出错或对方关闭(block mode)
 *
 *
 */
extern ssize_t ul_sread(int fd, void *ptr1, size_t nbytes);



/**
 *
 */
extern ssize_t ul_sreado_ms(int fd, void *ptr, size_t nbytes, int mseconds);



/**
 *
 *
 */
extern ssize_t ul_sreado(int fd, void *ptr, size_t nbytes, int seconds);

/**
 *
 *
 */
extern ssize_t ul_sreado_ex(int fd, void *ptr, size_t nbytes, int seconds);


/**
 *
 *
 */
extern ssize_t ul_swriteo_ms(int fd, void *ptr, size_t nbytes, int mseconds);


/**
 *
 *
 */
extern ssize_t ul_swriteo(int fd, void *ptr, size_t nbytes, int seconds);

/**
 *
 *
 */
extern ssize_t ul_swriteo_ex(int fd, void *ptr, size_t nbytes, int seconds);


/**
 *
 */
extern ssize_t ul_reado(int fd, void *ptr1, size_t nbytes, int secs);


/**
 *
 */
extern ssize_t ul_writeo_ms(int fd, void *ptr, size_t nbytes, int mseconds);


/**
 *
 */
extern ssize_t ul_writeo(int fd, void *ptr, size_t nbytes, int seconds);


/**
 * 检查socket在指定时间内是否可读
 *
 */
extern int ul_sreadable(int fd, int seconds);


/**
 * 检查socket在指定时间内是否可写
 *
 */
extern int ul_swriteable(int fd, int seconds);


//****************************************************************
// function : a  time-out version of connect().
// sockfd,saptr,socklen, as the same as connect
// secs, the ovet time, in second
//****************************************************************
/**
 * 带超时控制的connect()版本
 *
 */
extern int ul_connecto(int sockfd, const struct sockaddr *saptr, socklen_t socklen, int secs);


/**
 * 从socket中读取一行字符串，直到缓冲区满，或者socket出错或者关闭
 *
 */
ssize_t ul_readline(int fd, void *vptr, size_t maxlen);


ssize_t ul_readline_speed(int fd, void *vptr, size_t maxlen, Rline * rlin);
ssize_t ul_readline_speedo(int fd, void *vptr, size_t maxlen, Rline * rlin, int secs);


/**
 * 参数使用更加方便的connect()版本
 *
 */
extern int ul_tcpconnect(char *host, int port);

/**
 *
 */
extern int ul_tcpconnecto(char *host, int port, int secs);

//*******************************************************
// Get IP address of one host.  Return the h_addr item.
// Return value:    0 :success;
//                  -1:hostname is error.
//                  -2:gethostbyname() call is error.
// Note:This function is not safe in MT-thread process
//******************************************************
extern int ul_gethostipbyname(const char *hostname, struct sockaddr_in *sin);




#endif  //__UL_NET_DEP_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
