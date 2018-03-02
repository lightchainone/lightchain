
#ifndef _WINDEF_H_
#define _WINDEF_H_

#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stddef.h>
#include <time.h>
#include <string.h>

#define SOCKET_ERROR			-1
#define INVALID_SOCKET			-1
#define WSA_FLAG_OVERLAPPED		0


typedef int						HANDLE;
typedef int						SOCKET;
typedef struct sockaddr			SOCKADDR;
typedef struct sockaddr_in		SOCKADDR_IN;
typedef struct sockaddr*		LPSOCKADDR;
typedef struct sockaddr_in*		LPSOCKADDR_IN;

#define GetCurrentThreadId		thread_getid
#define closesocket				close
#define WSASocket(a,b,c,d,e,f)	socket(a, b, c)

#define _stricmp                strcasecmp
#define strnicmp	            strncasecmp

#define WSAGetLastError()		(errno)
#define GetLastError()			(errno)
#define SetLastError(e)         (errno=(e))


#ifndef CONTAINING_RECORD
#define CONTAINING_RECORD(address, type, field) ((type *)( 						\
                                                 (char*)(address) - 			\
                                                 offsetof(type, field)))
                                                 
#endif

static inline int 
Sleep(unsigned int nMilliseconds)
{
    struct timespec ts;
    ts.tv_sec = nMilliseconds / 1000;
    ts.tv_nsec = (nMilliseconds % 1000) * 1000000;

    return nanosleep(&ts, NULL);
}

#ifdef __linux__
#define LIKELY(x)           (__builtin_expect((x),1))
#define UNLIKELY(x)         (__builtin_expect((x),0))
#define UNLIKELY_IF(x)      if (__builtin_expect((x),0))

#else
#define LIKELY(x)           (x)
#define UNLIKELY(x)         (x)
#define UNLIKELY_IF(x)      if (x)
#endif

#endif
