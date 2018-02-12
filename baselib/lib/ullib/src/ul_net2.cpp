//******************************************************************
//  This library include most functions.include some wrap funtions.
//  You can use this functions freely.
//******************************************************************
// include area 
#include <assert.h>

#include "ul_net.h"
#include "ul_log.h"
#include "ul_conf.h"


//******************************************************************
// Wrap function about gethostname()
//******************************************************************
int
ul_gethostname(char *name, size_t len)
{
	int val = gethostname(name, len);
	if (val == -1) {
		ul_writelog(UL_LOG_WARNING, "gethostname(%ld) call failed.error[%d] info is %s.",
					(unsigned long) len, errno, strerror(errno));
	}
	return val;
}

//**************************************************************
// Wrap function about getservbyname()
// Return value: 0  --success.
//**************************************************************
int
ul_getservbyname_r(const char *name, const char *proto, struct servent *result_buf,
				   char *buf, size_t buflen, struct servent **result)
{
	int val = getservbyname_r(name, proto, result_buf, buf, buflen, result);
	if (val != 0) {
		errno = val;
		ul_writelog(UL_LOG_WARNING, "getservbyname_r(%s,%s) call failed.error[%d] info is %s.",
					name, proto, errno, strerror(errno));
	}
	return val;
}

//**************************************************************
// Wrap function about getservbyname()
// Return value: 0  --success.
//**************************************************************
int
ul_getservbyport_r(int port, const char *proto, struct servent *result_buf,
				   char *buf, size_t buflen, struct servent **result)
{
	int val = getservbyport_r(port, proto, result_buf, buf, buflen, result);
	if (val != 0) {
		errno = val;
		ul_writelog(UL_LOG_WARNING, "getservbyport_r(%d,%s) call failed.error[%d] info is %s.",
					port, proto, errno, strerror(errno));
	}
	return val;
}


//********************************************************************
//  TcpConnecto:
//      Time out version.(millisecond)
//      Build up a TCP connection to host at port.  This is used at
//      client part, returns the socket fd on success, or -1 on error
//********************************************************************
int
ul_tcpconnecto_ms(char *host, int port, int msecs)
{
	int fd;
	struct sockaddr_in soin;
	char buf[8192];
	struct hostent he, *p = NULL;
	int err = 0, ret = 0;

	if ((fd = ul_socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		return -1;
	}
// resolv host to sockaddr_in
	bzero(&soin, sizeof(soin));

//try A.B.C.D numeric IP format address first
	if ((soin.sin_addr.s_addr = inet_addr(host)) == INADDR_NONE) {
//not numeric IP format, try text format

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
		memcpy(&soin.sin_addr.s_addr, he.h_addr, sizeof(soin.sin_addr.s_addr));
	}
// here we assume we are always under IPv4 environment.
// If one day shifted to other network environment, such as IPv6,
// following code should also be under modification
	soin.sin_family = AF_INET;
	soin.sin_port = htons((uint16_t)port);
//connect
	if (ul_connecto_sclose_ms(fd, (struct sockaddr *) &soin, sizeof(soin), msecs) < 0) {
		// do not close sockfd,ul_connecto_sclose has closed it
		// ul_sclose(fd);
		// modified by qzd 2006/05/19
		return -1;
	}
	return fd;

}




//**************************************
//  Get datat struct hostent of one host 
//  mutithread safe and for LINUX version
//  note: you should define the size of buf was less then 8192  
//      error code was returned by point h_err.
//  the error code is in err:
//  ERANGE:  buf size too small
//  HOST_NOT_FOUND: Authoritive Answer Host not found
//  TRY_AGAIN:   Non-Authoritive Host not found, or SERVERFAIL
//  NO_RECOVERY: Non recoverable errors, FORMERR, REFUSED, NOTIMP
//  NO_DATA:     Valid name, no data record of requested type
//  NO_ADDRESS:  no address, look for MX record
//  MutliThread:    safe
//**************************************
int
ul_gethostbyname_r(const char *hostname, struct hostent *result_buf, char *buf, int buflen,
				   struct hostent **result, int *h_err)
{
	int val = 0;
	*h_err = 0;
	*result = NULL;
	if ((val = gethostbyname_r(hostname, result_buf, buf, (size_t)buflen, result, h_err)) != 0) {
		ul_writelog(UL_LOG_WARNING,
					"gethostbyname_r(%s) call failed.return value is %d,error[%d] info is %s",
					hostname, val, *h_err, hstrerror(*h_err));
		return -1;

	}
	if (*h_err != 0 || *result == NULL) {
		ul_writelog(UL_LOG_WARNING,
					"gethostbyname_r(%s) call failed.return value is %d,error[%d] info is %s",
					hostname, val, *h_err, hstrerror(*h_err));
		return -1;
	}
	return 0;
}

//*******************************************************
//  Get IP address of one host. Return the h_addr item.
//  Return value:   0 :success;
//                  -1:hostname is error.
//                  -2:gethostbyname_r() call is error. 
//                  You can look the errno number by Global Variable errno.
//  Out Mode:       #define BD_ASCII    0       
//                  #define BD_NET      1
//  MutliThread:    safe
//******************************************************
int
ul_gethostipbyname_r(const char *hostname, struct sockaddr_in *soin, int mode, char *asc_str)
{
	struct hostent result_buf, *result;
	int err;
	char buf[8192];
//    char    **pptr;

	if (hostname == NULL || strlen(hostname) == 0)
		return -1;
	if (ul_gethostbyname_r(hostname, &result_buf, buf, sizeof(buf), &result, &err) == -1) {
		h_errno = err;
		return -2;
	}
	if (mode == BD_ASCII) {
		switch (result_buf.h_addrtype) {
#ifdef  AF_INET6
		case AF_INET6:
			inet_ntop(result_buf.h_addrtype, result_buf.h_addr, asc_str, INET6_ADDRSTRLEN);
			break;
#endif
		case AF_INET:
			inet_ntop(result_buf.h_addrtype, result_buf.h_addr, asc_str, INET_ADDRSTRLEN);
			break;
		default:
			break;
		}
	} else {
		bzero(soin, sizeof(soin));
		memcpy(&soin->sin_addr.s_addr, result_buf.h_addr, sizeof(soin->sin_addr.s_addr));
	}
	return 0;
}

//******************************************************************************
//      Get IP address of one host.     Return the h_addr item and ip group.
//      Return value:   0 :success;
//                                      -1:hostname is error.
//                                      -2:gethostbyname_r() call is error.
//                                      You can look the errno number by Global Variable errno.
//      Out Mode:               #define BD_ASCII        0
//                              #define BD_NET          1
//      data struct :
//           # define IP_GROUP_NUM      10
//           typedef struct site_ip_group {
//                  int   addrtype;
//                  uint  ip[IP_GROUP_NUM ];
//           } sip_group ;
//  MutliThread:        safe
//*****************************************************************************

int
ul_gethostallipbyname_r(const char *hostname, struct sockaddr_in *soin,
						int mode, char *asc_str, sip_group * sip)
{
	struct hostent result_buf, *result;
	int err, i;
	char buf[8192];
	if (hostname == NULL || strlen(hostname) == 0)
		return -1;
	if (ul_gethostbyname_r(hostname, &result_buf, buf, sizeof(buf), &result, &err) == -1) {
		h_errno = err;
		return -2;
	}
	if (mode == BD_ASCII) {
		switch (result_buf.h_addrtype) {
#ifdef  AF_INET6
		case AF_INET6:
			inet_ntop(result_buf.h_addrtype, result_buf.h_addr, asc_str, INET6_ADDRSTRLEN);
			break;
#endif
		case AF_INET:
			inet_ntop(result_buf.h_addrtype, result_buf.h_addr, asc_str, INET_ADDRSTRLEN);
			break;
		default:
			break;
		}
	} else {
		bzero(soin, sizeof(soin));
		memcpy(&soin->sin_addr.s_addr, result_buf.h_addr, sizeof(soin->sin_addr.s_addr));
	}
	sip->addrtype = result_buf.h_addrtype;
	char **pptr;
	pptr = result_buf.h_addr_list;
	for (i = 0; pptr[i] != NULL && i < IP_GROUP_NUM; i++) {
		memcpy(&(sip->ip[i]), pptr[i], sizeof(soin->sin_addr.s_addr));
	}

	return 0;
}


//****************************************************************
// Get the port by server name
// Return value: on success,return the port. Otherwise,return -1.
//****************************************************************
int
ul_getportbyserv(const char *name, const char *proto, int *port)
{
	struct servent result_buf, **result;
	char buf[4096];

	if ((name == NULL || !strlen(name)) || (proto == NULL || !strlen(proto)))
		return -1;
	bzero(&result, sizeof(result));
	if ((ul_getservbyname_r(name, proto, &result_buf, buf, 4096, result)) != 0)
		return -1;
	*port = result_buf.s_port;
	return result_buf.s_port;
}
