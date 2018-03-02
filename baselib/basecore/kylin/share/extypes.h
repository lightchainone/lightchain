

#ifndef _EX_TYPES_H
#define _EX_TYPES_H


typedef unsigned char uint8;
typedef unsigned char byte;
typedef char int8;
typedef unsigned short uint16;
typedef short int16;
typedef unsigned int uint32;
typedef int int32;
typedef void* ref;

typedef void* ptr;

#ifdef __POSIX__
typedef unsigned int boolean;
#endif

#if defined(__POSIX__) || defined(__linux__)
typedef unsigned long long uint64;
typedef long long int64;
#else
typedef unsigned __int64 uint64;
typedef __int64 int64;
#endif

#define BYTES_OF_UINT8			1
#define BYTES_OF_UINT16			2
#define BYTES_OF_UINT32			4
#define BYTES_OF_UINT64			8

#define BYTES_OF_INT8			1
#define BYTES_OF_INT16			2
#define BYTES_OF_INT32			4
#define BYTES_OF_INT64			8

#define BYTES_OF_REF			4

#ifndef NULL
#define NULL					0
#endif

#define EX_ERROR				(-1)

#ifndef __cplusplus
#define true 1
#define false 0
typedef uint8   bool;
#define inline __inline
#endif


typedef struct _RefArray {
    uint32 len;
    ref value[1];
} RefArray;

#ifndef IN
#define IN
#endif
#ifndef OUT
#define OUT
#endif
#ifndef INOUT
#define INOUT
#endif
#ifndef OPTIONAL
#define OPTIONAL
#endif

#endif 
