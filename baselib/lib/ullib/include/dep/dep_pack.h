
#ifndef __DEP_PACK_H__
#define __DEP_PACK_H__



/**
 *
 *
 */
typedef struct ul_package ul_package;

typedef struct ul_package ul_package_t;
/**
 *  
 *  - 1  成功
 *  - 0  失败
 */ 
int ul_pack_getvalue(ul_package_t *ppack, char *pname, char *pvalue);


/**
 *  
 *  - >=0  pack包体的大小
 *  - <0   失败
 */
int ul_pack_read(int sockfd, ul_package_t *ppack);

 
/**
 *  
 *  - >0  实际读取的字节数
 *  - <=0   失败
 */ 
int ul_pack_readall(int fd, void *ptr1, size_t nbytes);

/**
 *  
 *  - 0  成功
 *  - -1 失败
 */ 
int ul_pack_readend(int sockfd);


/**
 *  
 *  - 0   成功
 *  - -1  失败
 */ 
int ul_pack_write(int sockfd, ul_package_t *ppack);

 
/**
 *  
 *  - >0    实际写出的字节数
 *  - <＝0  失败
 */
int ul_pack_writeall(int sockfd, char *buf, int n);



/**
 *  再将缓冲区的内容通过连接句柄写出, 因不带超时限制,不推荐使用
 *  
 *  - 0    成功
 *  - -1   失败
 */ 
int ul_pack_write_m(int sockfd, ul_package_t *ppack);


 
/**
 *  
 *  - >=0  pack包体的大小
 *  - <0   失败
 */
int ul_pack_reado(int sockfd, ul_package_t *ppack, int over_time);

 
/**
 *  
 *  - 0   成功
 *  - -1  失败
 */
int ul_pack_writeo(int sockfd, ul_package_t *ppack, int sec);


/**
 *  
 *  - >0    实际写出的字节数
 *  - <＝0  失败
 */
int ul_pack_writeallo(int sockfd, char *buf, int n, int sec);


/**
 *  写pack包，先将pack包输出到临时缓冲区，再将缓冲区的内容通过连接句柄写出,支持秒级超时
 *  
 *  - 0    成功
 *  - -1   失败
 */ 
int ul_pack_write_mo(int sockfd, ul_package_t *ppack, int sec);


/**
 *  
 *  - >0  实际读取的字节数
 *  - <=0   失败
 */
int ul_pack_readallo_ms(int fd, void *ptr1, size_t nbytes, int msec);

/**
 *  
 *  - 0   成功
 *  - -1  失败
 */ 
int ul_pack_readendo_ms(int sockfd, int msec);


/**
 *  
 *  - >0    实际写出的字节数
 *  - <＝0  失败
 */
int ul_pack_writeallo_ms(int sockfd, char *buf, int n, int msec);



#endif
/* vim: set ts=4 sw=4 sts=4 tw=100 noet: */
