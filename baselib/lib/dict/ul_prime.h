/**
 */
#ifndef _UL_PRIME_H_
#define _UL_PRIME_H_

#define PRIME_USED_31   2147483647

#define PRIME_USED_24_1 16777213

#define PRIME_USED_24_2 16777199

#define PRIME_USED_24_3 16777183

/**
 *  组装字符串64位数据签名的第一部分
 *  
 */
unsigned int getsign_24_1(char* str);

/**
 *  组装字符串64位数据签名的第二部分
 *  
 */
unsigned int getsign_24_2(char* str);


/**
 *  组装一段数据的64位数据签名的第1部分
 *  
 */
unsigned int getsigns_24_1(char* str,int slen);

/**
 *  组装一段数据的64位数据签名的第2部分
 *  
 */
unsigned int getsigns_24_2(char* str,int slen);


#endif
