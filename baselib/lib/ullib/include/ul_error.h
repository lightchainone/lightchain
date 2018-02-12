

#ifndef _UL_ERROR_H
#define _UL_ERROR_H

enum {
	UL_SUCCESS = 0,				//处理成功
	UL_DETAIL_ERR,				//细节错误，ul内部将错误信息打印到线程缓冲区去
	UL_ALLOC_ERR,				//内存分配错误
	UL_MEMSEG,					//内存越界
	UL_READ_ERR,				//io读失败
	UL_WRITE_ERR,				//io写失败
	UL_INVALID_FD,				//无效的句柄
	UL_NETCONNET_ERR,			//网络连接失败
	UL_INVALID_PARAM,			//无效的传入参数
	UL_OPEN_FILE_ERR,			//打开文件句柄失败
};

/**
 *
**/
int ul_seterrno(int err);

/**
 *
**/
int ul_geterrno();

/**
 *
**/
const char *ul_geterrstr(int err);


/**
 *
**/
int ul_seterrbuf(const char *format, ...);


/**
 *
 * 						失败返回""字符串
 * 						不会返回NULL指针
**/
const char *ul_geterrbuf();

#endif
/* vim: set ts=4 sw=4 tw=100 noet: */
/* vim: set ts=4 sw=4 sts=4 tw=100 noet: */
