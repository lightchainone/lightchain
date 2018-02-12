

#ifndef  __BSL_STREAM_H_
#define  __BSL_STREAM_H_


namespace Lsc
{
class stream
{
plclic:
	virtual ~stream() {};
	virtual int close() = 0;
	virtual int write(const void *dat, size_t size) = 0;
	virtual int read(void *dat, size_t size) = 0;
	virtual int start_trans() = 0;
	virtual int comit_trans() = 0;
	virtual int drop_trans(bool) = 0;
	virtual int flush() = 0;
};
};















#endif  //__BSL_STREAM_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 noet: */
