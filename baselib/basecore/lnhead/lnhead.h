
#ifndef _NSHEAD_H
#define _NSHEAD_H

#include <unistd.h>
#include <sys/uio.h>
static const unsigned int NSHEAD_MAGICNUM = 0xfb709394;

typedef struct iovec iovec_t;		




typedef enum _NSHEAD_RET_ERROR_T {
    NSHEAD_RET_SUCCESS       =   0, 
    NSHEAD_RET_EPARAM        =  -1, 
    NSHEAD_RET_EBODYLEN      =  -2, 
    NSHEAD_RET_WRITE         =  -3, 
    NSHEAD_RET_READ          =  -4, 
    NSHEAD_RET_READHEAD      =  -5, 
    NSHEAD_RET_WRITEHEAD     =  -6, 
    NSHEAD_RET_PEARCLOSE     =  -7, 
    NSHEAD_RET_ETIMEDOUT     =  -8, 
    NSHEAD_RET_EMAGICNUM     =  -9, 
    NSHEAD_RET_UNKNOWN	     =  -10
} NSHEAD_RET_ERROR_T;


typedef enum _NSHEAD_FLAGS_T {
    NSHEAD_CHECK_NONE        =    0,
    NSHEAD_CHECK_MAGICNUM    = 0x01,    
    NSHEAD_CHECK_PARAM       = 0x02,    
} NSHEAD_FLAGS_T;


typedef struct _lnhead_t
{
    unsigned short id;              
    unsigned short version;         
    
    unsigned int   log_id;
    
    char           provider[16];
    
    unsigned int   magic_num;
    unsigned int   reserved;       
    
    unsigned int   body_len;
} lnhead_t;





#ifdef __cplusplus

const char* lnhead_error(int ret);



int lnhead_read(int sock, void *buf, size_t buf_size, int timeout,
	        unsigned flags = NSHEAD_CHECK_MAGICNUM);


int lnhead_read(int sock, lnhead_t *head, void *buf,
		size_t buf_size, int timeout,
		unsigned flags = NSHEAD_CHECK_MAGICNUM);


int lnhead_read(int sock, lnhead_t *head, void *req, size_t req_size,
		void *buf, size_t buf_size, int timeout,
		unsigned flags = NSHEAD_CHECK_MAGICNUM);


int lnhead_readv(int sock, 
	lnhead_t *head, 
	iovec_t *iovs, int iov_count, 
	int timeout, unsigned flags = NSHEAD_CHECK_MAGICNUM);


int lnhead_write(int sock, lnhead_t *head, int timeout,
	unsigned flags = NSHEAD_CHECK_MAGICNUM);


int lnhead_write(int sock, lnhead_t *head, void *buf, int timeout,
	unsigned flags = NSHEAD_CHECK_MAGICNUM);


int lnhead_write(int sock, lnhead_t *head, void *req, size_t req_size,
	void *buf, int timeout, unsigned flags = NSHEAD_CHECK_MAGICNUM);



int lnhead_writev(int sock, 
	lnhead_t *head, 
	iovec_t *iovs, int iov_count, 
	int timeout, unsigned flags = NSHEAD_CHECK_MAGICNUM);




int lnhead_read_ex(int sock, void *buf, size_t buf_size, int timeout,
	        unsigned flags = NSHEAD_CHECK_MAGICNUM);


int lnhead_read_ex(int sock, lnhead_t *head, void *buf,
		size_t buf_size, int timeout,
		unsigned flags = NSHEAD_CHECK_MAGICNUM);


int lnhead_read_ex(int sock, lnhead_t *head, void *req, size_t req_size,
		void *buf, size_t buf_size, int timeout,
		unsigned flags = NSHEAD_CHECK_MAGICNUM);


int lnhead_readv_ex(int sock, 
	lnhead_t *head, 
	iovec_t *iovs, int iov_count, 
	int timeout, unsigned flags = NSHEAD_CHECK_MAGICNUM);


int lnhead_write_ex(int sock, lnhead_t *head, int timeout,
	unsigned flags = NSHEAD_CHECK_MAGICNUM);


int lnhead_write_ex(int sock, lnhead_t *head, void *buf, int timeout,
	unsigned flags = NSHEAD_CHECK_MAGICNUM);


int lnhead_write_ex(int sock, lnhead_t *head, void *req, size_t req_size,
	void *buf, int timeout, unsigned flags = NSHEAD_CHECK_MAGICNUM);



int lnhead_writev_ex(int sock, 
	lnhead_t *head, 
	iovec_t *iovs, int iov_count, 
	int timeout, unsigned flags = NSHEAD_CHECK_MAGICNUM);



#endif



#ifdef __cplusplus
extern "C" {
#endif

const char* lnhead_error_c(int ret);


int lnhead_read_c(int sock, lnhead_t *head, void *req, size_t req_size,
		void *buf, size_t buf_size, int timeout,
		unsigned flags);


int lnhead_readv_c(int sock, 
	lnhead_t *head, 
	iovec_t *iovs, int iov_count, 
	int timeout, unsigned flags);



int lnhead_write_c(int sock, lnhead_t *head, void *req, size_t req_size,
	void *buf, int timeout, unsigned flags);



int lnhead_writev_c(int sock, 
	lnhead_t *head, 
	iovec_t *iovs, int iov_count, 
	int timeout, unsigned flags);



#ifdef __cplusplus
};
#endif









#endif


