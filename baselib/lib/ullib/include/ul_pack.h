
#ifndef __PACK_UL_H__
#define __PACK_UL_H__

#include <errno.h>
#include <unistd.h>

#include "ul_log.h"
#include "ul_net.h"
#include "ul_func.h"

#include "dep/dep_pack.h"

#define MAX_PACK_ITEM_NUM  200			/**< 可以插入的name:value对最大数量       */
#define MAX_ITEM_SIZE  2048				/**< name和value的最大长度       */
#define SIZE_NUM  20
#define MAX_HEAD_SIZE  26000				/**< 传输头最大长度 */

typedef struct ul_package {
    char name[MAX_PACK_ITEM_NUM][MAX_ITEM_SIZE];
    char value[MAX_PACK_ITEM_NUM][MAX_ITEM_SIZE];
    int pairnum;      /**< name:value 对的个数，不超过MAX_PACK_ITEM_NUM       */
    int bufsize;      /**< 传输的buff长度       */
    char *buf;        /**< 传输的buff，使用时需要外部分配后传入函数       */
} ul_package_t;

//typedef struct ul_package_t ul_package_t;

/**
 *
 *  
 *  - NULL   表示失败
 *  - 非NULL 表示成功
 */
ul_package_t *ul_pack_init();

/**
 * 
 *
 *  - 0   表示成功
 */
int ul_pack_free(ul_package_t *ppack);

/**
 *  
 *  - 0   表示成功
 *  - <0  表示失败
 */
int ul_pack_putbuf(ul_package_t *ppack, char *buffer, int size);

/**
 *  
 *  - 非NULL   表示成功
 *  - NULL     表示失败
 */ 
char *ul_pack_getbuf(ul_package_t *ppack);

/**
 *  
 *  - 1  名字已存在，添加成功
 *  - 0  成功
 *  - -1 名字或值超过允许长度，失败
 *  - -2 pack没有空间，失败
 */  
int ul_pack_putvalue(ul_package_t *ppack, const char *pname, const char *pvalue);

/**
 *  
 *  - 1  成功
 *  - 0  失败
 */ 
int ul_pack_getvalue_ex(ul_package_t *ppack, const char *pname, char *pvalue, size_t size);

/**
 *
 * -1 : 输入错误 或者 key不存在
 * 0 : 成功
**/
int ul_pack_delvalue(ul_package_t *ppack, const char *pname);

/**
 *  
 *  - >=0  pack包体的大小
 *  - <0   失败
 *
 */
int ul_pack_reado_ms(int sockfd, ul_package_t *ppack, int msec);

/**
 *  
 *  - 0   成功
 *  - -1  失败
 *  - -2  句柄出错
 *  
 */
int ul_pack_writeo_ms(int sockfd, const ul_package_t *ppack, int msec);


/**
 *  写pack包，先将pack包输出到临时缓冲区，再将缓冲区的内容通过连接句柄写出,支持毫秒级超时
 *  
 *  - 0    成功
 *  - -1   失败
 */ 
int ul_pack_write_mo_ms(int sockfd, ul_package_t *ppack, int msec);

/**
 *  序列化pack包，将pack包输出到内存buffer
 *  
 *  - >=0    实际使用的buffer长度
 *  - -1   失败
 */ 
int ul_pack_serialization(void *buf, int size, ul_package_t *ppack);

/**
 *  反序列化内存buffer，将内存数据解析成pack包
 *  
 *  - >=0    实际使用的buffer长度
 *  - -1   失败
 */ 
int ul_pack_deserialization(void *buf, int size, ul_package_t *ppack);

/**
 *  
 *  - 1    成功
 *  - -1   失败
 */
int ul_pack_cleanhead(ul_package_t* ppack);

 
/**
 *  
 *  - 1    成功
 *  - -1   失败
 */ 
int ul_pack_cleanall(ul_package_t* ppack);


#endif
/* vim: set ts=4 sw=4 sts=4 tw=100 noet: */
