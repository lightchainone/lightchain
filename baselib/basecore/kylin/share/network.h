
#ifndef _NETWORK_H_
#define _NETWORK_H_

#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "extypes.h"
#include "windef.h"
#include "Trace.h"
#include "SyncObj.h"


static inline uint32 
in_aton(char *str)
{
    uint32 l;
    unsigned int val;
    int i;

    l = 0;
    for (i = 0; i < 4; i++) {
        l <<= 8;
        if (*str != '\0') {
            val = 0;
            while (*str != '\0' && *str != '.') {
                val *= 10;
                val += *str - '0';
                str++;
            }
            l |= val;
            if (*str != '\0') 
                str++;
        }
    }
    return htonl(l);
}

#define S_DOTTED_IP             "%d.%d.%d.%d"
#define DOTTED_IP(ip)           (int)(((ip)&0x000000ffUL)),          \
                                (int)(((ip)&0x0000ff00UL) >> 8),     \
                                (int)(((ip)&0x00ff0000UL) >> 16),    \
                                (int)(((ip)&0xff000000UL) >> 24)
static inline char*
in_ntoa(uint32 in)
{
    static char buff[18];
    char *p;

    p = (char *) &in;
    sprintf(buff, "%d.%d.%d.%d",
        (p[0] & 255), (p[1] & 255), (p[2] & 255), (p[3] & 255));
    return buff;
}

static inline bool
SetNonBlock(int fd)
{
    int arg;
    if (0 > (arg=fcntl(fd, F_GETFL, NULL))) { 
        PERROR("fcntl");
        return false;
    } 
    arg |= O_NONBLOCK; 
    if (0 > fcntl(fd, F_SETFL, arg)) { 
        PERROR("fcntl");
        return false;
    } 
    return true;
}

static inline bool
SetReuseAddr(SOCKET s)
{
    int flag = 1;
    if (0 == setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *)&flag, sizeof(flag)))
        return true;
    PERROR("setsockopt");
    return false;
}

static inline bool
SetNoDelay(SOCKET s)
{
    int flag = 1;
    if (0 == setsockopt(s, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(flag)))
        return true;
    PERROR("setsockopt");
    return false;
}

static inline bool
SetQuickAck(SOCKET s, bool enable=true)
{
    int flag = enable ? 1 : 0;
    if (0 == setsockopt(s, IPPROTO_TCP, TCP_QUICKACK, (char *)&flag, sizeof(flag)))
        return true;
    PERROR("setsockopt");
    return false;
}

static inline bool
SetLingerOff(SOCKET s)
{
    struct linger lingr = { 0, 0 };
    if (0 == setsockopt(s, SOL_SOCKET, SO_LINGER, (char *)&lingr, sizeof(lingr)))
        return true;
    PERROR("setsockopt");
    return false;
}

static inline uint32
GetIp(char* ip_or_name)
{
	uint32 ip;
    static CMutex lock;

    if (ip_or_name) {
	    ip = inet_addr(ip_or_name);
	    if (INADDR_NONE == ip) {
		    TLocalLock<CMutex> guard(lock);
            struct hostent *hostp = NULL;
            
            UNLIKELY_IF (NULL == (hostp=gethostbyname(ip_or_name))) {
			    return INADDR_NONE;          
            }
		    ASSERT(hostp->h_length == 4);
		    memcpy(&ip, hostp->h_addr, hostp->h_length);
	    }
	    return ip;
    }
    return INADDR_NONE;
}

#endif
