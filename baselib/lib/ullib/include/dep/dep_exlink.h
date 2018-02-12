


#ifndef __DEP_EXLINK_H__
#define __DEP_EXLINK_H__
#include "ul_url.h"

#define  UL_MAX_LINK_NUM     2000
#define  UL_MAX_TEXT_LEN   100

#define UL_METHOD_IXINTUI		0
#define UL_METHOD_WGET16	1
#define UL_METHOD_WGET18	2


typedef struct _link_info_t {
	char domain[UL_MAX_SITE_LEN];
	char port[UL_MAX_PORT_LEN];
	char path[UL_MAX_PATH_LEN];
	char text[UL_MAX_TEXT_LEN];
	char url[UL_MAX_URL_LEN];
} link_info_t;



/**
 * 根据url和相对的url，合成新的url
 * 
 * - 1  成功
 * - 0  失败
 */
int ul_absolute_url(char *url, char *relurl, char *absurl);



/**
 * 根据url对应的page内容，得到新的url(萃取连接)
 * 
 * - UL_METHOD_WGET16
 * - UL_METHOD_WGET18	
 * - >=0   成功，返回萃取成功的url数
 * - <0    失败
 */
int extract_link(char *url, char *page, int pagesize, link_info_t * link_info, int num, int method);
#endif
/* vim: set ts=4 sw=4 sts=4 tw=100 noet: */
