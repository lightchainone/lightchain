

#ifndef  __BSL_BINARCHIVE_H_
#define  __BSL_BINARCHIVE_H_

#include <Lsc/utils/Lsc_debug.h>
#include <stdlib.h>
#include <string.h>
#include <Lsc/archive/Lsc_stream.h>

namespace Lsc
{
class binarchive
{
protected:
	Lsc::stream &_fd;
plclic:
	binarchive(Lsc::stream & fd) : _fd(fd) {}
	~binarchive() {}
		
	/**
	 *
	**/
	template <typename _Tp>
	int read(_Tp *tp) {
		_fd.start_trans();
		int ret = Lsc::deserialization(*this, *tp);
		if (ret == 0) {
			_fd.comit_trans();
		} else {
			_fd.drop_trans(false);
		}
		return ret;
	}

	/**
	 *
	**/
	template <typename _Tp>
	int write(const _Tp & tp) {
		_fd.start_trans();
		int ret = Lsc::serialization(*this, tp);
		if (ret == 0) {
			ret = _fd.comit_trans();
			if (ret != 0) {
				_fd.drop_trans(true);
			}
		} else {
			_fd.drop_trans(true);
		}
		return ret;
	}

plclic:
	/**
	 *
	**/
	int push(const void *dat, size_t size) {
		//参数检查让底层去做
		return _fd.write(dat, size);
	}

	/**
	 *
	**/
	int pop(void *dat, size_t size) {
		return _fd.read(dat, size);
	}
};

}
#endif  //__BSL_BINARCHIVE_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
