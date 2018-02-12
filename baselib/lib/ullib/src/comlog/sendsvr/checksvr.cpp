#include <limits.h>
#include "checksvr.h"
#include "sendsvr.h"
#include "../xdebug.h"
#include "../xutils.h"

namespace comspace
{

CheckSvr::CheckSvr()
{
	for (u_int i=0; i<COM_MAXSERVERSUPPORT; ++i) {
		_servers[i] = 0;
	}
	_size = 0;
	pthread_mutex_init(&_lock, NULL);
	_tid = 0;
	_sleeptime = 10;
}

static void * _check_(void *p)
{
	do {
		((CheckSvr *)(p))->check();
	} while (1);
	return NULL;
}

int CheckSvr::registerServer(SendSvr *svr)
{
	int ret = 0;
	pthread_mutex_lock(&_lock);
	if (_size >= (int)COM_MAXSERVERSUPPORT) {
		ret = -1;
		goto end;
	}
	_servers[_size] = svr;

	if (_size == 0 && _tid == 0) {
		pthread_create(&_tid, NULL, _check_, this);
		_com_debug("start a check server");
	}

	++ _size;
end:
	pthread_mutex_unlock(&_lock);
	return ret;
}

int CheckSvr::check()
{
	{
		xAutoLock __lock(&_lock);
		//_com_debug("start, check size[%d] sleep[%d]", _size, _sleeptime);
		for (int i=0; i<_size; ++i) {
			//_com_debug("check size[%d] sleep[%d]", _size, _sleeptime);
			if (_servers[i]->checkalive(1000) != 0) {	//10s
				_com_error("[%d][%s]server invalid", i, _servers[i]->_name);
			}
		}
	}
	safe_msleep(_sleeptime * 1000);
	//sleep(_sleeptime);
	return 0;
}



int CheckSvr::checkOver(int waittime)
{
	xAutoLock __lock(&_lock);
	if (_size <= 0) return 0;
	static const int msec = 100;	//ясЁы100ms
    
    if (waittime < 0) {
        waittime = INT_MAX;
    }
	while (waittime > 0) {
		bool ret = true;
		for (int i=0; i<_size; ++i) {
			if (_servers[i]->_queue.size() > 0) {
				ret = false;
				break;
			}
		}
		if (ret) {
			goto end;
		}
		waittime -= msec;
		//sleep msec
		safe_msleep(msec);
	}
end:
	{
		safe_msleep(msec);
	}
	return 0;
}

void CheckSvr::clear(){
	xAutoLock __lock(&_lock);
	_size = 0;
}

}
/* vim: set ts=4 sw=4 sts=4 tw=100 */
