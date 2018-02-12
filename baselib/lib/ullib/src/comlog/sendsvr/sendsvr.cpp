
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include "ul_net.h"
#include "sendsvr.h"
#include "../comlog.h"
#include "../xutils.h"
#include "../namemg.h"
#include "../xdebug.h"
#include "../appender/appender.h"
#include "../category.h"

namespace comspace
{

static void * _sendsvr_callback(void *param)
{
	while (((SendSvr *)(param))->isRun() ){
		((SendSvr *)(param))->run();
	}
	if(com_logstatus() == USING_COMLOG){
		//_com_error("Leaving sendsvr's thread");
		com_closelog_r();
	}
	return NULL;
}

int SendSvr::registAppender(Appender *app)
{
	xAutoLock __lock(&_lock);
	for (int i=0; i<_inappsize; ++i) {
		if (_inappender[i] == app) {
			return 0;
		}
	}
	if (_inappsize >= DEFMAXAPPENDER) return -1;
	_inappender[_inappsize++] = app;
	return 0;
}

SendSvr::SendSvr()
{
	for (u_int i=0; i<COM_MAXDUPSERVER; ++i) {
		_svrs[i].isvalid = false;
		_svrs[i].sock = -1;
	}
	_svrsiz = 0;
	_inappsize = 0;
	_wtto = 500;//ms
	_rdto = 500;
	_coto = 200;
	_run = 0;

	pthread_mutex_init(&_lock, NULL);
}

int SendSvr::erasedata(int items)
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

int SendSvr::stop(){
	pthread_mutex_lock(&_queue._mutex);
	if(_run){
		_run = 0;
		pthread_cond_signal(&_queue._cond);
		pthread_mutex_unlock(&_queue._mutex);
		pthread_join(_tid, NULL);
	}else{
		pthread_mutex_unlock(&_queue._mutex);
	}
	return  _run;
}


SendSvr::~SendSvr() 
{
	stop();
	for (int j=0; j<_svrsiz; ++j) {
		close_safe(j);
	}
	int items;
	while(! _queue._queue.empty()){
		items = _queue._queue.pop_backs(_readbuf, sizeof(_readbuf)/sizeof(_readbuf[0]));
		erasedata(items);
	}
	_queue.destroy();
	pthread_mutex_destroy(&_lock);
}

int SendSvr::push(vptr_t dat)
{
	return _queue.push_front(dat);
}

int SendSvr::createServer(const char *ip, bool bCheckAlive)
{
	if (ip == NULL) {
		return -1;
	}

	if (initServer(ip) != 0) {
		return -1;
	}

	//check alive
	if(bCheckAlive){
		int ret = checkalive(200);	//200ms
		if (ret != 0) {
			_com_error("can't find valid server");
		}

		ret = comlog_get_gcheckSvr()->registerServer(this);
		if (ret != 0) {
			return -1;
		}
	}

	if (_queue.create(DEFQUEUESIZE) != 0) {
		_com_error("create queue error");
		return -1;
	}

	_run = 1;
	if ( pthread_create(&_tid, NULL, _sendsvr_callback, this) != 0 ){
		_run = 0;
		_com_error("Failed to create new thread for SendSvr [%s]", ip);
		return -1;
	}
	_com_debug("start sendserver[%s]", ip);
	return 0;
}

int SendSvr::_checkalive(int ms)
{
	//_com_debug("_svrsiz[%d] start to checkalive", _svrsiz);
	int alive = -1;
	for (int j=0; j<_svrsiz; ++j) {
		if (_svrs[j].isvalid == false) {
			//connect again
			int timeout = ms;//us
			//如果被开启过,关闭
			close_safe(j);
			_svrs[j].sock = ul_tcpconnecto_ms(_svrs[j].host, _svrs[j].port, timeout);
            if (_svrs[j].sock > 0) {
                /*
                int on = 1;
                if (setsockopt(_svrs[j].sock, IPPROTO_TCP,TCP_NODELAY,&on, sizeof(on)) < 0) {
                    _com_error("set NODELAY fail");
                    ::close(_svrs[j].sock);
                    return -1; 
                } */
				_svrs[j].isvalid = true;
				alive = 0;
				_com_debug("get sock[%d]", _svrs[j].sock);
				break;
			} else {
				_com_error("connect[%d] %s:%d err[%m]", j, _svrs[j].host, _svrs[j].port);
			}
		} else {
			//check again
			//这里不能对这个sock做操作，不然会线程不安全的
			//_com_debug("that ok j[%d]", j);
			alive = 0;
			break;
		}
	}
	return alive;
}

int SendSvr::checkalive(int ms)
{
	xAutoLock __lock(&_lock);
	return _checkalive(ms);
}

int SendSvr::tryalive(int ms)
{
	xAutoLock __lock(&_lock);
	for (int i=0; i<_svrsiz; ++i) {
		if (_svrs[i].isvalid) {
			return 0;
		}
	}
	return _checkalive(ms);
}

int SendSvr::close_safe(int pos)
{
	if (pos < 0) return 0;
	if (_svrs[pos].sock > 0) {
		close(_svrs[pos].sock);
	}
	_svrs[pos].isvalid = false;
	_svrs[pos].sock = -1;
	return 0;
}

//pthread_mutex_t SendSvr::g_sendsvr_lock = PTHREAD_MUTEX_INITIALIZER;
//CheckSvr SendSvr::g_checkSvr;
static pthread_mutex_t comlog_sendsvr_sendsvr_lock = PTHREAD_MUTEX_INITIALIZER;
static CheckSvr * g_checkSvr=NULL;

pthread_mutex_t * SendSvr::get_sendsvr_lock() {
	return &comlog_sendsvr_sendsvr_lock;
}
CheckSvr * SendSvr::comlog_get_gcheckSvr() {
	if (NULL == g_checkSvr) {
		g_checkSvr = new CheckSvr();
	}
	return g_checkSvr;
}
void SendSvr::comlog_del_gcheckSvr() {
	if (g_checkSvr) {
		delete g_checkSvr;
		g_checkSvr = NULL;
	}
}

}
/* vim: set ts=4 sw=4 sts=4 tw=100 */
