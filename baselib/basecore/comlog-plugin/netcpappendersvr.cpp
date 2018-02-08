
#include <algorithm>
#include "sendsvr/loghead.h"
#include "netcpappender.h"
#include "netcpappendersvr.h"

#include "ul_net.h"
#include "xutils.h"
#include "namemg.h"
#include "event.h"
#include "xdebug.h"
#include "time.h"
#include <netinet/tcp.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

namespace comspace
{

int NetcpAppenderSvr::maxretry = 3;
int NetcpAppenderSvr::_svrswitch = 10000;
int NetcpAppenderSvr::_svrswitchtime = 60;



static bool _netapender_sort_cmp_ (const SendSvr::vptr_t & _1, const  SendSvr::vptr_t & _2 )
{
	NetcpAppender::netdata_t *v1 = (NetcpAppender::netdata_t *)_1;
	NetcpAppender::netdata_t *v2 = (NetcpAppender::netdata_t *)_2;
	if (v1->appender == v2->appender) {
		return v1->item.time < v2->item.time;
	} 
	return v1->appender < v2->appender;
}


bool NetcpAppenderSvr::reopens(comspace::SendSvr::server_t *svr, int)
{
	for (int j=0; j<_inappsize; ++j) {
		if (_inappender[j]->open(svr) != 0) {
			_com_error("can't open %s server", _name);
			return false;
		}
	}
	return true;
}

int NetcpAppenderSvr::senddata(int items, int sendsock)
{
	
	
	
	if(sendsock < 0){
		_com_error("No Available socket.");
		return -1;
	}
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
		NetcpAppender::netdata_t *ndt = (NetcpAppender::netdata_t *)(_readbuf[i]);

		if (ndt->appender->_id == -1) {
			_com_error("debug- faint _id=%d handle=%d when senddata at i[%d]", ndt->appender->_id, ndt->item.handle, i);
			
#if 1
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
#if 0
		_com_error("send err %m sock[%d] now[%d] svr{[%d][%d]}, ret[%d]",
				sendsock, _now, _svrs[_now].sock, _svrs[_now].isvalid, ret);
#endif
		return -1;
	}
	_com_debug("writev bytes[%d] bodylen[%d]", ret, bodylen);

	return 0;
}

int NetcpAppenderSvr::run()
{
	_debug_time_def;

	_debug_getstart;
	int sendsock;
	int items = _queue.pop_backs(_readbuf, sizeof(_readbuf)/sizeof(_readbuf[0]), DEFPOPTO);
	

	if (items > 0) {
		
		std::sort(_readbuf, _readbuf+items, _netapender_sort_cmp_);
		
#if 0
		if (synchandle(items) != 0) {
			
			goto fail;
		}
#endif
		_debug_getend;
		_com_debug("get data used %d ms", _debug_time);
		int retry = maxretry;
		sendsock = fetchSocket();
		if(sendsock < 0){
			goto fail;
		}
		while(senddata(items, sendsock) != 0) {
			freeSocket(true, 0);
			--retry;
			if(retry <= 0){
				goto fail;
			}
			sendsock = fetchSocket();
		}
		freeSocket(false, items);
		_debug_getstart;
	}
	_debug_getend;
	_com_debug("send data used time[%d]", _debug_time);
	
	
	erasedata(items);
	return 0;
fail:
	_com_error("Failed to senddata, [MaxRetry]=%d", maxretry);
	{
		
		for (int i=0; i<items; ++i) {
			NetcpAppender::netdata_t *ndt = (NetcpAppender::netdata_t *)(_readbuf[i]);
			if (((NetcpAppender *)(ndt->appender))->diskBackup() 
					&& ndt->appender->_bkappender) {
				ndt->appender->_bkappender->binprint(ndt->item.log, ndt->item.size);
			}
		}
	}
	erasedata(items);
	return 0;
}

int NetcpAppenderSvr::setMaxRetry(int maxRetry){
	if(maxRetry >= 0 && maxRetry <= 100){
		maxretry = maxRetry;
	}
	return maxretry;
}

int NetcpAppenderSvr::setSwitch(int n){
	_svrswitch = n;
	return _svrswitch;
}

int NetcpAppenderSvr::setSwitchTime(int s){
	_svrswitchtime = s;
	return _svrswitchtime;
}

NetcpAppenderSvr::NetcpAppenderSvr()
{
#if 0
	_nowsd.pos = -1;
	_nowsd.val = -1;
#endif
	_sock = -1;
	my_magic = -1;
	maxretry = 3;
	_total_sent = 0;
	_last_switch = 0;
	int fd; 
	struct ifreq buf;
	struct ifconf ifc; 
	unsigned int seed = (unsigned int)time(0);
	if ((fd = socket (AF_INET, SOCK_DGRAM, 0)) >= 0){ 
		ifc.ifc_len = sizeof(buf);
		ifc.ifc_buf = (caddr_t) &buf; 
		if (!ioctl (fd, SIOCGIFCONF, (char *) &ifc)){
			if(!ioctl (fd, SIOCGIFADDR, (char *) &buf)){
				seed ^= *(unsigned int *)&(((struct sockaddr_in*)(&buf.ifr_addr))->sin_addr) ;
			}    
		}    
	}
	seed ^= (unsigned int)getpid(); 
	_com_debug("seed = %u", seed);
	srand(seed);
#ifdef _XLOG_MEMPOOL_USED_
	_mempool.create();
#endif
}

NetcpAppenderSvr::~NetcpAppenderSvr(){
	stop();
#ifdef _XLOG_MEMPOOL_USED_
	_mempool.destroy();
#endif
}

int NetcpAppenderSvr::initServer(const char *iplist)
{
	
	int items = xparserSvrConf(iplist, _svrs, sizeof(_svrs)/sizeof(_svrs[0]));
	if (items <= 0) {
		_com_error("parser %s err", iplist);
		return -1;
	}
	_svrsiz = items;
	for(int i = 0; i < items; ++i){
		snprintf(_svrconf[i].addr, server_conf_t::MAX_ADDR_LEN, "%s", _svrs[i].host);
		_svrconf[i].port = _svrs[i].port;
		_svrconf[i].ctimeout_ms = 200;
		
		
	}
	int ret = cpool.Init(_svrconf, items, 1 );
	if( ret < 0 ){
		_com_error("Failed to create connectpool [items = %d][ret = %d]", items, ret);
		return -1;
	}
	while(my_magic == -1){
		my_magic = rand();
	}

	snprintf(_name, sizeof(_name), "%s", iplist);
	_com_debug("initserver svrsiz[%d]", items);
	return 0;
}

int NetcpAppenderSvr :: fetchSocket(){
	if(_sock >= 0){
		if(_svrswitchtime >= 0){
			if(time(NULL) >= _last_switch + _svrswitchtime){
				cpool.FreeSocket(_sock, false);
				goto fetchnew;
			}
		}
		if(_svrswitch >= 0){
			if(_total_sent >= _svrswitch){
				cpool.FreeSocket(_sock, false);
				goto fetchnew;
			}
		}
		char buf[1];
		errno = 0;
		ssize_t ret = recv(_sock, buf, sizeof(buf), MSG_DONTWAIT);
		while(ret == -1 && errno == EINTR){
			ret = recv(_sock, buf, sizeof(buf), MSG_DONTWAIT);
		}
		if(ret == -1 && errno == EAGAIN){ 
			return _sock;
		}
		cpool.FreeSocket(_sock, true);
	}
fetchnew:
	_sock = cpool.FetchSocket();
	if(_sock >= 0){
		_last_switch = time(NULL);
		_total_sent = 0;
		_svrs[0].sock = _sock;
		reopens(&_svrs[0], 0);
	}
	return _sock;
}

int NetcpAppenderSvr :: freeSocket(bool err, int sent){
	_total_sent += sent;
	if(err){
		cpool.FreeSocket(_sock, true);
		_sock = -1;
	}
	return 0;
}
	
int NetcpAppenderSvr::push(vptr_t dat)
{
	_debug_time_def;

	_debug_getstart;

	Event *evt = (Event *)dat;
	int ret = 0;
	NetcpAppender::netdata_t *val = (NetcpAppender::netdata_t *) 
#ifdef _XLOG_MEMPOOL_USED_
			 _mempool.alloc(sizeof(NetcpAppender::netdata_t) + evt->_render_msgbuf_len + 1);
#else
			 malloc(sizeof(NetcpAppender::netdata_t) + evt->_render_msgbuf_len + 1);
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
	
	

	pthread_mutex_lock (&_queue._mutex);
	ret = _queue._queue.push_front(val);
	pthread_cond_signal(&_queue._cond);
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

	

	return ret;
}

SendSvr *NetcpAppenderSvr::getServer(const char *iplist)
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
		_com_debug("start to create netcpappender svr");
		svr = new NetcpAppenderSvr();
		int ret = svr->createServer(iplist, false);
		if (ret != 0) {
			delete svr;
			_com_error("create server err[%s]", iplist);
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
#if 0
void NetcpAppenderSvr::clearsock(int i)
{
	close_safe(i);
#if 0
	_nowsd.pos = -1;
	_nowsd.val = -1;
#endif
	_now = -1;
}
#endif
}



