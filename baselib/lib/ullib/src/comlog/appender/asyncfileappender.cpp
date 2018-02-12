

#include "asyncfileappender.h"

#include "../xdebug.h"
#include "../xutils.h"
#include "../layout.h"
#include "../event.h"
#include "../namemg.h"
#include <stdint.h>

namespace comspace
{

static void * _asyncfile_callback(void *param)
{
	while (1) {
		_com_debug("Running Callback....");
		int ret = ((AsyncFileAppender *)(param))->writeData();
		if(ret == 1)return NULL;
	}
	return NULL;
}


int AsyncFileAppender::syncid(void *p)
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
			uint64_t nowsiz = ((uint64_t)(_device.log_size)) << 20;
			if ((uint64_t)(fst.st_size) >= nowsiz) {
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

int AsyncFileAppender::open(void *p)
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
	_run = 1;
    //fprintf(stderr, "set _run = 1\n");
	//create deamon thread to write data
	return fstat(_id, &_stat);
}

int AsyncFileAppender::close(void *)
{
	if (_id < 0) return 0;
	::close(_id);
	_id = -1;
	return 0;
}
#if 0
int AsyncFileAppender::print(Event *evt)
{
	//_com_debug("in find id[%d]", _id);
	if (unInMask(evt->_log_level)) {
		//_com_debug("can't suport this level, [%d][%llx]", evt->_log_level, _mask);
		return 0;
	}
	_layout->format(evt);
	if (syncid(NULL) != 0) {
		_com_error("%s", evt->_render_msgbuf);
		close(NULL);
		return -1;
	}
	int ret = write(_id, evt->_render_msgbuf, evt->_render_msgbuf_len);
	if (ret != evt->_render_msgbuf_len) {
		_com_error("ret[%d] != len[%d] %m", ret, evt->_render_msgbuf_len);
		close(NULL);
		return -1;
	}
	return 0;
}
#endif


//Actually, it is push~!
int AsyncFileAppender::print(Event *dat)
{
	//pthread_mutex_lock (&_queue._mutex);
	xAutoLock __lock(&_queue._mutex);
	Event *evt = (Event *)dat;
	if(dat == NULL || dat->_render_msgbuf == NULL){
		return -1;
	}
	if(unInMask(evt->_log_level)){
		return 0;
	}
	evt->_nowapp = this;
	_layout->format(evt);
	int ret = 0;
	_netdata_t *val = (_netdata_t *) 
#ifdef _XLOG_MEMPOOL_USED_
	_mempool.alloc(sizeof(_netdata_t) + evt->_render_msgbuf_len + 1);
#else
	malloc(sizeof(_netdata_t) + evt->_render_msgbuf_len + 1);
#endif
	if (val == 0) {
		ret = -1;
		goto end;
	}

	val->appender = evt->_nowapp;
	val->item.handle = val->appender->_id;

	val->item.time = (u_int)evt->_print_time.tv_sec;
	val->item.size = (u_int)evt->_render_msgbuf_len;
	memcpy(val->item.log, evt->_render_msgbuf, val->item.size);

	ret = _queue._queue.push_front(val);
	_com_debug("sizeof queue: %d", _queue._queue.size());
	pthread_cond_signal(&_queue._cond);
	//fprintf(stderr, "ret = %d\n", ret);
	if (ret != 0) {
#ifdef _XLOG_MEMPOOL_USED_
		_mempool.dealloc(val);
#else
		free (val);
#endif
	}
end:
	//pthread_mutex_unlock(&_queue._mutex);
	return ret;
}

int AsyncFileAppender::binprint(void *buf, int siz)
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


int AsyncFileAppender::writeData()
{
	//fprintf(stderr, "Running WriteData.....");
	_com_debug("Running WriteData....");
	if(_run == 0){
		return 0;
	}
    if (_stop == 1) {
        return 1;
    }
	int items = _queue.pop_backs(_readbuf, sizeof(_readbuf)/sizeof(_readbuf[0]), AFILE_DEFPOPTO);
	//fprintf(stderr, "items=%d\n", items);
	_com_debug("items=%d, _run=%d", items, _run);
	if(items == 0 && _run == -1){
		return 1;
	}
	if(items > 0){
		if(syncid(NULL) != 0){
			_com_error("In [AsyncFileAppender::writeData]:syncid failed.");
			close(NULL);
			goto WriteFail;
		}
		int ret = 0;
		_netdata_t *ndt;
		for (int i=0; i<items; ++i) {
			ndt = (_netdata_t *)(_readbuf[i]);
			afio[i].iov_base = ndt->item.log;
			afio[i].iov_len = ndt->item.size;
			ret += ndt->item.size;
		}

		if(ret != writev(_id, afio, items)){
			_com_error("write failed.");
			goto WriteFail;
		}

		erasedata(items);
	}
	return 0;
WriteFail:
	erasedata(items);
	return -1;
}

int AsyncFileAppender::erasedata(int items)
{
	if (items < 0) return 0;
	pthread_mutex_lock (&_queue._mutex);
	for (int i=0; i<items; ++i) {
#ifdef _XLOG_MEMPOOL_USED_
		_mempool.dealloc(_readbuf[i]);
#else
		free (_readbuf[i]);
#endif
	}
	pthread_mutex_unlock(&_queue._mutex);
	return 0;
}

AsyncFileAppender::AsyncFileAppender()
{
	_page = 0;
	_file[0] = 0;
	_lastcutime = time(0);
	_run = 0;
    _stop = 0;
}

AsyncFileAppender::~AsyncFileAppender(){
	_com_debug("Running AFA destruct...");
	if(_run){
		_run = -1;
        _stop = 1;
		pthread_mutex_lock(&_queue._mutex);
		_queue._isWorking = false;
		pthread_cond_signal(&_queue._cond);
		pthread_mutex_unlock(&_queue._mutex);
        //fprintf(stderr, "_run is %d\n", _run);
		pthread_join(_tid, NULL);
	}
	close(NULL);
	int items;
	while(! _queue._queue.empty()){
		items = _queue._queue.pop_backs(_readbuf, sizeof(_readbuf)/sizeof(_readbuf[0]));
		erasedata(items);
	}
	_queue.destroy();
}
Appender * AsyncFileAppender::getAppender(com_device_t &dev)
{
	_com_debug("wanna get file appender");

	char name[1024];
	snprintf(name, sizeof(name), "FILE:%s/%s", dev.host, dev.file);
	Appender *app = (Appender *)comlog_get_nameMg()->get(type_appender, name);
	if (app == NULL) {
		app = new AsyncFileAppender;
		app->setDeviceInfo(dev);
		if (app->syncid(NULL) != 0) {
			_com_error("syncid log[%s] err %m", name);
			delete app;
			return NULL;
		}
		((AsyncFileAppender *)app)->_queue.create(AFILE_QUEUESIZE);
		int ret = pthread_create(&(((AsyncFileAppender *)app)->_tid), NULL, _asyncfile_callback, app);
		if(ret != 0) {
			delete app;
			return NULL;
		}
		_com_debug("create thread for AsyncFileAppender (%s)", name);
		comlog_get_nameMg()->set(name, app);
	}
	_com_debug("get async file appender");
	return app;
}

Appender * AsyncFileAppender::tryAppender(com_device_t &dev)
{
	_com_debug("wanna get file appender");

	char name[1024];
	snprintf(name, sizeof(name), "FILE:%s/%s", dev.host, dev.file);
	Appender *app = (Appender *)comlog_get_nameMg()->get(type_appender, name);
	return app;
}

}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
