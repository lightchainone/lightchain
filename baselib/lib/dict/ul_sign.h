/**
 */
//***********************************************************************
//			Signature Utilities (SU) 1.0
//
//   programmed by Ming Lei, Jul, 2000
//***********************************************************************


#ifndef __UL_SIGN_H__
#define __UL_SIGN_H__

#include <ul_func.h>

/**
 *  为字符串创建唯一的64位数据签名
 *  
 * - 1   成功
 * - -1  失败
 * - if slen<0，有可能出现程序异常；
 * - if slen==0，sign = 0:0；
 */
int creat_sign_f64 (char* psrc, int slen, unsigned int* sign1 , unsigned int* sign2);



/**
 *  为一段data创建唯一的64位数据签名
 *  
 * - 1   成功
 * - -1  失败
 * - if slen<0，有可能出现程序异常；
 * - if slen==0，sign = 0:0；
 */
int creat_sign_fs64 (char* psrc, int slen, unsigned int* sign1 , unsigned int* sign2);



/**
 *  为字符串创建唯一的56位数据签名
 *  
 * - 1   成功
 * - -1  失败
 * - if slen<0，有可能出现程序异常；
 * - if slen==0，sign = 0:0；
 */
int creat_sign_7_host (char* psrc, int slen, unsigned int* sign1 , unsigned int* sign2);



/**
 *  为字符串创建唯一的md5 64位数据签名
 *  
 * -  1   成功
 */
int creat_sign_md64(char* psrc,int slen,unsigned int* sign1,unsigned int*sign2);

/**
 *  为一段数据创建唯一的md5 64位数据签名
 *  
 * - 1   成功
 */
int creat_sign_mds64(char* psrc,int slen,unsigned int* sign1,unsigned int*sign2);

/**
 *  为字符串创建唯一的murmurhash 64位数据签名
 *  
 * -  1   成功
 */
int creat_sign_murmur64(const char* psrc,int slen,unsigned int* sign1,unsigned int*sign2,unsigned int seed=0);

/**
 *  为一段数据创建唯一的murmurhash 64位数据签名
 *  
 * - 1   成功
 */
int creat_sign_murmurs64(const char* psrc,int slen,unsigned int *sign1,unsigned int *sign2,unsigned int seed=0);

#endif
