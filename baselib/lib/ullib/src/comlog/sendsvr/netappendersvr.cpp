
#include <algorithm>
#include "loghead.h"

#include "ul_net.h"
#include "netappendersvr.h"
#include "../xutils.h"
#include "../namemg.h"
#include "../event.h"
#include "../appender/netappender.h"
#include "../xdebug.h"

namespace comspace
{
/*
 * appender 句柄
 * id
 * 时间
 * 渲染数据
 * 渲染数据长度
 */

static bool _netapender_sort_cmp_ (const SendSvr::vptr_t & _1, const  SendSvr::vptr_t & _2 )
{
	NetAppender::netdata_t *v1 = (NetAppender::netdata_t *)_1;
	NetAppender::netdata_t *v2 = (NetAppender::netdata_t *)_2;
	if (v1->appender == v2->appender) {
		return v1->item.time < v2->item.time;
	} 
	return v1->appender < v2->appender;
}

bool NetAppenderSvr::reopens(server_t *svr, int)
{
	//for (int j=0; j<items; ++j) {
	for (int j=0; j<_inappsize; ++j) {
		//重新分配交互句柄
		if (_inappender[j]->open(svr) != 0) {
			_com_error("can't open %s server", _name);
			return false;
		}
	}
	return true;
}

int NetAppenderSvr::synchandle(int items)
{
	int i=0;
	int loop = 0;
again:
	for (i=0; i<_svrsiz; ++i) {
		_com_debug("ser valid[%d]", (int)_svrs[i].isvalid);
		if (_svrs[i].isvalid) {
			//如果最近得有效句柄跟上次发送得句柄不符合
			if (i != _now) {
				//check sock
				if (ul_swriteable_ms(_svrs[i].sock, CHECKTIMEOUT) > 0 &&
						//成功更新socket句柄
						reopens(_svrs+i, items)) 
				{
					if (_now >= 0) {
						close_safe(_now);
					}
					_now = i;
					break;
				} else {
					//如果交互失败,关闭sock
					_com_error("invalid sock[%d] or reopen err %m", _svrs[i].sock);
					close_safe(i);
				}
			} else {
				//如果符合，验证成功
				if (ul_swriteable_ms(_svrs[i].sock, CHECKTIMEOUT) > 0) {
					break;
				} else {
					_com_error("invalid sock[%d] %m", _svrs[i].sock);
					clearsock(_now);
				}
			}
		}
	}
	++ loop;

	if (_now < 0 || i >= _svrsiz) {
		_now = -1;
		if (loop == 1) {
			if (tryalive(1000) == 0) {	//1s
				goto again;
			}
		}
		_com_error("can't find valid sd _now[%d] i[%d]", _now, i);
		return -1;
	}

#ifdef __CHECK__
	static int __oce = 0;
	++ __oce;
	for (i=0; i<items; ++i) {
		NetAppender::netdata_t *ndt = (NetAppender::netdata_t *)_readbuf[i];
		assert (ndt->appender->_id >= 0);
	}
#endif
	return 0;
}

int NetAppenderSvr::senddata(int items)
{
	//发送
	//int sendsock = _svrs[_nowsd.pos].sock;
	int sendsock = _svrs[_now].sock;
	loghead_t *snd = (loghead_t *)_sendbuf;
	snd->magic_num = LOGHEAD_MAGICNUM;
	snd->body_len = 0;
	snd->reserved = COMLOG_PRINT;
	int iosiz = 0;
	_iovecs[iosiz].iov_base = snd;
	_iovecs[iosiz].iov_len = sizeof(*snd);
	++ iosiz;

	Appender *now = NULL;
	u_int *addpos = NULL;
	int bodylen = 0;
	for (int i=0; i<items; ++i) {
		NetAppender::netdata_t *ndt = (NetAppender::netdata_t *)(_readbuf[i]);

		if (ndt->appender->_id == -1) {
			_com_error("debug- faint _id=%d handle=%d when senddata at i[%d]", ndt->appender->_id, ndt->item.handle, i);
			//不可能是-1
#if 0
			return -1;
#endif
		}

		ndt->item.handle = ndt->appender->_id;

		if (now != ndt->appender) {
			_iovecs[iosiz].iov_base = &ndt->item;
			_iovecs[iosiz].iov_len = sizeof(ndt->item) + ndt->item.size;

			addpos = &ndt->item.size;
			bodylen += _iovecs[iosiz].iov_len;
			now = ndt->appender;

			++ iosiz;
		} else {
			_iovecs[iosiz].iov_base = ndt->item.log;
			_iovecs[iosiz].iov_len = ndt->item.size;

			*addpos += _iovecs[iosiz].iov_len;
			bodylen += _iovecs[iosiz].iov_len;

			++ iosiz;
		}
	}

	snd->body_len = bodylen;
	int ret = writev(sendsock, _iovecs, iosiz);
	if (ret < (int)(snd->body_len + sizeof(*snd))) {
		_com_error("send err %m sock[%d] now[%d] svr{[%d][%d]}, ret[%d]",
				sendsock, _now, _svrs[_now].sock, _svrs[_now].isvalid, ret);
		return -1;
	}
	_com_debug("writev bytes[%d] bodylen[%d]", ret, bodylen);

	return 0;
}

int NetAppenderSvr::run()
{
	_debug_time_def;

	_debug_getstart;
	int items = _queue.pop_backs(_readbuf, sizeof(_readbuf)/sizeof(_readbuf[0]), DEFPOPTO);
	//_core_debug("popbakcs items[%d]", items);

	if (items > 0) {
		//sort with sendat
		std::sort(_readbuf, _readbuf+items, _netapender_sort_cmp_);
		//fill data;

		if (synchandle(items) != 0) {
			//_com_error("synhandle err goto end");
			goto fail;
		}

		_debug_getend;
		_com_debug("get data used %d ms", _debug_time);
		if (senddata(items) != 0) {
			clearsock(_now);
			goto fail;
		}
		_debug_getstart;
	}
	_debug_getend;
	_com_debug("send data used time[%d]", _debug_time);
	
	//回收mempool分配的内存
	erasedata(items);
	return 0;
fail:
	{
		//失败写硬盘
		for (int i=0; i<items; ++i) {
			NetAppender::netdata_t *ndt = (NetAppender::netdata_t *)(_readbuf[i]);
			if (ndt->appender->_bkappender) {
				ndt->appender->_bkappender->binprint(ndt->item.log, ndt->item.size);
			}
		}
		erasedata(items);
	}
	return 0;
}

NetAppenderSvr::NetAppenderSvr()
{
#if 0
	_nowsd.pos = -1;
	_nowsd.val = -1;
#endif
	_now = -1;
#ifdef _XLOG_MEMPOOL_USED_
	_mempool.create();
#endif
}

NetAppenderSvr::~NetAppenderSvr(){
#ifdef _XLOG_MEMPOOL_USED_
	_mempool.destroy();
#endif
}

int NetAppenderSvr::initServer(const char *iplist)
{
	int items = xparserSvrConf(iplist, _svrs, sizeof(_svrs)/sizeof(_svrs[0]));
	if (items <= 0) {
		_com_error("parser %s err", iplist);
		return -1;
	}
	_svrsiz = items;
	snprintf(_name, sizeof(_name), "%s", iplist);
	_com_debug("initserver svrsiz[%d]", items);
	return 0;
}


int NetAppenderSvr::push(vptr_t dat)
{
	_debug_time_def;

	_debug_getstart;

	pthread_mutex_lock (&_queue._mutex);
	Event *evt = (Event *)dat;
	int ret = 0;
	NetAppender::netdata_t *val = (NetAppender::netdata_t *) 
#ifdef _XLOG_MEMPOOL_USED_
			 _mempool.alloc(sizeof(NetAppender::netdata_t) + evt->_render_msgbuf_len + 1);
#else
			 malloc(sizeof(NetAppender::netdata_t) + evt->_render_msgbuf_len + 1);
#endif
	if (val == 0) {
		ret = -1;
		goto end;
	}
	//_com_debug("get address %lx", val);

	val->appender = evt->_nowapp;
	val->item.handle = val->appender->_id;
	
	val->item.time = (u_int)evt->_print_time.tv_sec;
	val->item.size = (u_int)evt->_render_msgbuf_len;
	memcpy(val->item.log, evt->_render_msgbuf, val->item.size);
	//_com_debug("buflen=%d", val->item.size);
	//_com_bin_debug(val->item.log, val->item.size);

	ret = _queue._queue.push_front(val);
	//if (ret == 0) {
		pthread_cond_signal(&_queue._cond);
	//} else {
		//失败写硬盘
		//_com_debug("push msg error");
		//
	//}
	if (ret != 0) {
#ifdef _XLOG_MEMPOOL_USED_
		_mempool.dealloc(val);
#else
		free (val);
#endif
	}
end:

	pthread_mutex_unlock(&_queue._mutex);
	_debug_getend;

	//_com_debug("push cost time %d\n", _debug_time);

	return ret;
}

SendSvr *NetAppenderSvr::getServer(const char *iplist)
{
	xAutoLock autolock(get_sendsvr_lock());

	char name[MAXSVRNAMESIZE];
	server_t svrs[COM_MAXDUPSERVER];

	int items = xparserSvrConf(iplist, svrs, sizeof(svrs)/sizeof(svrs[0]));
	if (items <= 0) {
		_com_error("parser %s err", iplist);
		return NULL;
	}
	int len = 0;
	for (int i=0; i<items; ++i) {
		snprintf(name+len, sizeof(name)-len, "%s:%d", svrs[i].host, svrs[i].port);
		len = strlen(name);
		_com_debug("parser svr: %s:%d", svrs[i].host, svrs[i].port);
	}
	SendSvr *svr = (SendSvr *)comlog_get_nameMg()->get(type_sendsvr, name);
	if (svr == NULL) {
		_com_debug("start to create netappender svr");
		svr = new NetAppenderSvr();
		int ret = svr->createServer(iplist);
		if (ret != 0) {
			delete svr;
			_com_error("crete server err[%s]", iplist);
			return NULL;
		}
		if (comlog_get_nameMg()->set(name, svr) != 0) {
			delete svr;
			_com_error("set server err[%s]", name);
			return NULL;
		}
	}
	return svr;
}

void NetAppenderSvr::clearsock(int i)
{
	close_safe(i);
#if 0
	_nowsd.pos = -1;
	_nowsd.val = -1;
#endif
	_now = -1;
}
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */

