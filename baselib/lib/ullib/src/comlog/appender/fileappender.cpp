
#include "fileappender.h"
#include "../xdebug.h"
#include "../xutils.h"
#include "../layout.h"
#include "../event.h"
#include "../namemg.h"
#include <stdint.h>

namespace comspace
{

int FileAppender::syncid(void *p)
{
	int ret = 0;
	char mvfile[1024];
	bool needdel = false;
	bool caseevt = false;

	pthread_mutex_lock(&_lock);
	//_com_debug("in syncid now with _id[%d]", _id);
	if (_id > 0) {
		//检查id跟文件名是否对应
		struct stat fst;
		ret = stat(_file, &fst);
		//文件不存在，open之
		if (ret != 0) {
			ret = open(p);
			goto end;
		}
		//_com_debug("%llu %llu", (unsigned long long)fst.st_ino, (unsigned long long)_stat.st_ino);
		//文件存在，但是已经跟当前句柄不符合
		if (fst.st_ino != _stat.st_ino) {
			_com_debug("st_info din't equl");
			ret = open(p);
			goto end;
		} else {
			//根据条件出发一些操作
			//如果日志长度超过指定长度直接回滚
			uint64_t nowsiz = ((uint64_t)_device.log_size) << 20;
			if ((uint64_t)fst.st_size >= nowsiz) {
				save_backup(_file, mvfile, sizeof(mvfile), _device.reserved1);
				ret = open(p);
				caseevt = true;
				_lastcutime = time(0);
			}
			switch (_device.splite_type) {
				case COM_TRUNCT: 
					needdel = true;
					goto end;

				case COM_SIZECUT:
					goto end;

				case COM_DATECUT:
					{
						time_t now = time(0);
						//周期配置单位是分钟
						if (now - _lastcutime > _device.cuttime * 60) {
							save_backup(_file, mvfile, sizeof(mvfile), _device.reserved1);
							ret = open(p);
							_lastcutime = now;
						}
					}
					break;

				default:
					break;
			}
		}
	} else {
		ret = open(p);
		goto end;
	}
end:
	pthread_mutex_unlock(&_lock);

	if (caseevt && needdel) {
		remove(mvfile);
	}
	return ret;
}

int FileAppender::open(void *p)
{
	_com_debug("before close / open id[%d]", _id);
	close(p);
	snprintf(_file, sizeof(_file), "%s/%s", _device.host, _device.file);
	mkdir (_device.host, 0700);
	int fd = ::open(_file, O_CREAT | O_APPEND | O_RDWR, 0666);
	if (fd < 0) {
		_com_error("can't open %s", _file);
		return -1;
	}
	_id = fd;
	_com_debug("-----------open log %s id[%d]", _file, _id);
	return fstat(_id, &_stat);
}

int FileAppender::close(void *)
{
	if (_id < 0) return 0;
	::close(_id);
	_id = -1;
	return 0;
}

int FileAppender::print(Event *evt)
{
	//_com_debug("in find id[%d]", _id);
	if (unInMask(evt->_log_level)) {
		//_com_debug("can't suport this level, [%d][%llx]", evt->_log_level, _mask);
		return 0;
	}
	_layout->format(evt);
	if (syncid(NULL) != 0) {
		_com_error("%s", evt->_render_msgbuf);
                pthread_mutex_lock(&_lock);
		close(NULL);
                pthread_mutex_unlock(&_lock);
		return -1;
	}
	int ret = write(_id, evt->_render_msgbuf, evt->_render_msgbuf_len);
	if (ret != evt->_render_msgbuf_len) {
		_com_error("ret[%d] != len[%d] %m", ret, evt->_render_msgbuf_len);
                pthread_mutex_lock(&_lock);
		close(NULL);
                pthread_mutex_unlock(&_lock);
		return -1;
	}
	return 0;
}

int FileAppender::binprint(void *buf, int siz)
{
	if (syncid(NULL) != 0) {
		return -1;
	}
	int ret = write(_id, buf, siz);
	if (ret != siz) {
		return -1;
	}
	return 0;
}

int FileAppender::reset()
{
	_page = 0;
	_file[0] = 0;
	close(NULL);
	return 0;
}

FileAppender::FileAppender()
{
	_page = 0;
	_file[0] = 0;
	_lastcutime = time(0);
}

FileAppender::~FileAppender(){
	close(NULL);
}

Appender * FileAppender::getAppender(com_device_t &dev)
{
	_com_debug("wanna get file appender");

	char name[1024];
	snprintf(name, sizeof(name), "FILE:%s/%s", dev.host, dev.file);
	Appender *app = (Appender *)comlog_get_nameMg()->get(type_appender, name);
	if (app == NULL) {
		app = new FileAppender;
		app->setDeviceInfo(dev);
		if (app->syncid(NULL) != 0) {
			_com_error("syncid log[%s] err %m", name);
			delete app;
			return NULL;
		}
		comlog_get_nameMg()->set(name, app);
	}
	_com_debug("get file appender");
	return app;
}

Appender * FileAppender::tryAppender(com_device_t &dev)
{
	_com_debug("wanna get file appender");

	char name[1024];
	snprintf(name, sizeof(name), "FILE:%s/%s", dev.host, dev.file);
	Appender *app = (Appender *)comlog_get_nameMg()->get(type_appender, name);
	return app;
}

}
/* vim: set ts=4 sw=4 sts=4 tw=100 */
