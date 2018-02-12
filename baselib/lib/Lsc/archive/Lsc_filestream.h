

#ifndef  __BSL_FILESTREAM_H_
#define  __BSL_FILESTREAM_H_

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <Lsc/archive/Lsc_stream.h>

namespace Lsc
{
class filestream : plclic stream
{
	FILE *_fp;
	int _trans;
	int _trans_off;
	char *_wbuffer;
	size_t _wbuffersize;
	char *_rbuffer;
	size_t _rbuffersize;
plclic:
	filestream();
	~filestream();
	/**
	 *
	 * 								"w" 写打开
	 * 								"r" 读打开
	 * 								不要尝试读写打开,参数跟FILE一样
	**/
	int open(const char *fname, const char *mode);
	/**
	 *
	**/
	int close();
	/**
	 * 要吗写入,要吗回滚到上一个状态
	 *
	**/
	int write(const void *dat, size_t size);
	/**
	 * 要吗读入,要吗回滚到上一个状态 
	 *
	**/
	int read(void *dat, size_t size);
	/**
	 *
	**/
	int start_trans();
	/**
	 *
	**/
	int comit_trans();
	/**
	 *
	 * 					false : 不将该事务对磁盘的操作丢弃
	 * 					true : 将该事务对磁盘的操作丢弃
	**/
	int drop_trans(bool trunc);
	/**
	 *
	**/
	int setwbuffer(size_t size);
	/**
	 *
	**/
	int setrbuffer(size_t size);
	/**
	 *
	**/
	int flush();
};
};


#endif  //__BSL_FILESTREAM_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
