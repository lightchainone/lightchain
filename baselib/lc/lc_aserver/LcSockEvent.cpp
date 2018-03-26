#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <stdarg.h>
#include "LcSockEvent.h"
#include <lc/lcutils.h>
#include "event_log.h"
namespace lc {


static inline void event_tvABS(timeval *tv, int msec)
{
	tv->tv_sec += msec/1000;
	tv->tv_usec += (msec%1000) * 1000;
	if (tv->tv_usec > 1000000) {
		tv->tv_sec ++;
		tv->tv_usec -= 1000000;
	}
}

int LcEvent :: repost() {
	if (_fstart_ss) {
		_fstart_ss = 0;
		return rand_repost();
	} else {
		return smart_repost();
	}
}

int LcEvent :: rand_repost() {
	if (0 == if_post()) {
		setResult(IEvent::ERROR);
		return -2;
	}
	int ret = 0;
	if (NULL == sock_data.first_reactor) {
		IReactor * r = reactor();
		if (NULL != r) {
			ret = r->post(this);
		} else {
			ret = -1;
		}
		if (0 != ret) {
			setResult(IEvent::ERROR);
		}
		return ret;
	}
	ret = sock_data.first_reactor->post(this);
	if (0 != ret) {
		setResult(IEvent::ERROR);
		set_sock_status(LcEvent::STATUS_POSTERROR);
	}
	return ret;
}

int LcEvent :: smart_repost() {
	if (0 == if_post()) {
		setResult(IEvent::ERROR);
		return -2;
	}
	int ret = 0;
	if (NULL == sock_data.first_reactor) {
		IReactor * r = reactor();
		if (NULL != r) {
			ret = r->smartPost(this);
		} else {
			ret = -1;
		}
		if (0 != ret) {
			setResult(IEvent::ERROR);
		}
		return ret;
	}
	ret = sock_data.first_reactor->smartPost(this);
	if (0 != ret) {
		setResult(IEvent::ERROR);
		set_sock_status(LcEvent::STATUS_POSTERROR);
	}
	return ret;
}

unsigned int LcEvent :: get_sock_status() {
	return sock_data.status;
}

void LcEvent :: set_sock_status(unsigned int st) {
	unsigned int newst = st;
	int err = result();
	if (err & TIMEOUT) {
		newst |= STATUS_TIMEOUT;
	} else if (err & CLOSESOCK) {
		newst |= STATUS_CLOSESOCK;
	} else if (err & ERROR) {
		newst |= STATUS_ERROR;
	} else if (err & CANCELED) {
		newst |= STATUS_CANCEL;
	}
	sock_data.status = newst;
}

static void lcsockevent_readdone_callback(IEvent *e, void *) {
	LcEvent *ev = static_cast<LcEvent *>(e);
	ev->restore_callback();
	lc_timecount_t * timer = &(ev->get_sock_data()->timer);
	timeval tv;
	
	gettimeofday(&tv, NULL);
	LCEVENT_DEBUG(ev,"[time: %ld, %ld]" ,tv.tv_sec, tv.tv_usec);
	timer->read += tvTv2Ms(tvSlc(tv, timer->count));
	
	timer->proccount = tv;
	ev->set_sock_status(LcEvent::STATUS_READ);
	if (ev->isError()) {
		LCEVENT_WARNING(ev,"LcEvent(lcsockevent_readdone_callback) read error[%m]");
		ev->event_error_callback();
		return;
	}
	ev->read_done_callback();
	gettimeofday(&tv, NULL);
	LCEVENT_DEBUG(ev,"[time: %ld, %ld]" ,tv.tv_sec, tv.tv_usec);
	timer->proc += tvTv2Ms(tvSlc(tv, timer->proccount));
	timer->count = tv;
}

static void lcsockevent_writedone_callback(IEvent *e, void *) {
	LcEvent *ev = static_cast<LcEvent *>(e);
	ev->restore_callback();
	lc_timecount_t * timer = &(ev->get_sock_data()->timer);
	timeval tv;
	
	gettimeofday(&tv, NULL);
	LCEVENT_DEBUG(ev,"[time: %ld, %ld]" ,tv.tv_sec, tv.tv_usec);
	timer->write += tvTv2Ms(tvSlc(tv, timer->count));
	if (ev->isError()) {
		ev->set_sock_status(LcEvent::STATUS_WRITE);
		LCEVENT_WARNING(ev,"lcsockevent_writedone_callback event(%p) error", ev);
		ev->event_error_callback();
		return;
	}
	
	timer->proccount = tv;
	ev->set_sock_status(LcEvent::STATUS_WRITE);
	ev->write_done_callback();
	gettimeofday(&tv, NULL);
	LCEVENT_DEBUG(ev,"[time: %ld, %ld]" ,tv.tv_sec, tv.tv_usec);
	timer->proc += tvTv2Ms(tvSlc(tv, timer->proccount));
	timer->count = tv;
}

static void * lc_event_malloc(size_t size, Lsc::mempool * mgr)
{
	void * result;
	if (mgr) {
		result = mgr->malloc(size);
	} else {
		result = malloc(size);
	}
	if(NULL == result) {
		LC_IN_WARNING("lc_event malloc memory(size:%lu) failed", (unsigned long)size);
		return NULL;
	}

	LC_IN_DEBUG("lc_event malloc memory(p:%p, size:%lu)",result, (unsigned long)size);
	return result;
}

static void lc_event_free(void * mem, Lsc::mempool * mgr, size_t msize) {
	if (mgr) {
		mgr->free(mem, msize);
	} else {
		free(mem);
	}
}

void LcEvent :: backup_callback() {
	this->getCallback(&_fold_cb, &_fold_cbp);
}
void LcEvent :: restore_callback() {
	this->setCallback(_fold_cb, _fold_cbp);
}
const char * LcEvent :: get_read_buffer(unsigned int size, int copy) {
	char * retbuf = NULL;
	if (0 == size) {
		return sock_data.read_buf;
	}
	if (sock_data.ext_readbuf) {
		if (size > sock_data.read_buf_len) {
			LCEVENT_WARNING(this,"LcEvent(get_read_buffer): can not get buffer(%u), ext len(%u)",
				size, sock_data.read_buf_len);
			retbuf =  NULL;
			goto END;
		}
		retbuf =   sock_data.read_buf;
		goto END;
	}
	if ((0 < sock_data.max_buffer_size) && (size > sock_data.max_buffer_size)) {
		LCEVENT_WARNING(this,"LcEvent(get_read_buffer): can not get buffer(%u), max(%u)",
				size, sock_data.max_buffer_size);
		retbuf =  NULL;
		goto END;
	}
	if ((0 < sock_data.max_readbuf_size) && (size > sock_data.max_readbuf_size)) {
		LCEVENT_WARNING(this,"LcEvent(get_read_buffer): can not get buffer(%u), max(%u)",
				size, sock_data.max_readbuf_size);
		retbuf =  NULL;
		goto END;
	}

	if (size > sock_data.read_buf_len) {
		retbuf = (char *)lc_event_malloc(size, _fmemory_manager);
		if ((0 != copy) && (NULL != retbuf)) {
			memcpy(retbuf, sock_data.read_buf, sock_data.read_buf_used);
		}
		if (NULL != retbuf) {
			if (sock_data.small_readbuf != sock_data.read_buf) {
				lc_event_free(sock_data.read_buf, _fmemory_manager, sock_data.read_buf_len);
			}
			sock_data.read_buf = retbuf;
			sock_data.read_buf_len = size;
		}
	} else {
		retbuf = sock_data.read_buf;
	}
END:
	if (NULL != retbuf) {
	} else {
		setResult(IEvent::ERROR);
		set_sock_status(LcEvent::STATUS_READ | LcEvent::STATUS_MEMERROR);
	}
	return retbuf;
}
int LcEvent :: get_readbuf_size() {
	return sock_data.read_buf_used;
}
char * LcEvent :: get_write_buffer(unsigned int size) {
	char * retbuf = NULL;
	if (sock_data.ext_writebuf) {
		if (size > sock_data.write_buf_len) {
			LCEVENT_WARNING(this,"LcEvent(get_write_buffer): can not get buffer(%u), ext len(%u)",
				size, sock_data.write_buf_len);
			return NULL;
		}
		retbuf = sock_data.write_buf;
		goto END;
	}
	if ((0 < sock_data.max_buffer_size) && (size > sock_data.max_buffer_size)) {
		LCEVENT_WARNING(this,"LcEvent(get_write_buffer): can not get buffer(%u), max(%u)",
				size, sock_data.max_buffer_size);
		retbuf =  NULL;
		goto END;
	}
	if ((0 < sock_data.max_writebuf_size) && (size > sock_data.max_writebuf_size)) {
		LCEVENT_WARNING(this,"LcEvent(get_write_buffer): can not get buffer(%u), max(%u)",
				size, sock_data.max_writebuf_size);
		retbuf =  NULL;
		goto END;
	}

	if (size > sock_data.write_buf_len) {
		retbuf = (char *)lc_event_malloc(size, _fmemory_manager);
		if (NULL != retbuf) {
			if (sock_data.small_writebuf != sock_data.write_buf) {
				lc_event_free(sock_data.write_buf, _fmemory_manager, sock_data.write_buf_len);
			}
			sock_data.write_buf = retbuf;
			sock_data.write_buf_len = size;
		}
	} else {
		retbuf = sock_data.write_buf;
	}
END:
	if (NULL != retbuf) {
		sock_data.write_buf_used = size;
	} else {
		setResult(IEvent::ERROR);
		set_sock_status(LcEvent::STATUS_WRITE | LcEvent::STATUS_MEMERROR);
	}
	return retbuf;
}
void LcEvent :: set_writebuf_size(int size) {
	if (0 > size) {
		return;
	}
	if (size > (int)sock_data.write_buf_len) {
		sock_data.write_buf_used = sock_data.write_buf_len;
		return;
	}
	sock_data.write_buf_used = size;
}


int LcEvent :: aread(int size) {
	int ret = 0;
	if (0 > size) {
		LCEVENT_WARNING(this,"LcEvent(aread) : size(%d) < 0", size);
		return -1;
	}
	sock_data.read_buf_used = 0;
	if (NULL == get_read_buffer(size)) {
		LCEVENT_WARNING(this,"LcEvent(aread) : get buffer failed.");
		ret = -1;
	} else {
		timeval tv;
		gettimeofday(&tv, NULL);
		LCEVENT_DEBUG(this,"[time: %ld, %ld]" ,tv.tv_sec, tv.tv_usec);
		sock_data.timer.count = tv;
		backup_callback();
		this->setCallback(lcsockevent_readdone_callback, NULL);
		this->read(this->handle(), sock_data.read_buf, size);
		sock_data.read_buf_used = size;
		judge_read_timeout(tv);
		ret = repost();
	}
	return ret;
}


int LcEvent :: trigle_read() {
	setType(IOREADABLE);
	timeval tv;
	gettimeofday(&tv, NULL);
	LCEVENT_DEBUG(this,"[time: %ld, %ld]" ,tv.tv_sec, tv.tv_usec);
	sock_data.timer.count = tv;
	backup_callback();
	setCallback(lcsockevent_readdone_callback, NULL);
	judge_read_timeout(tv);
	return repost();
}


int LcEvent :: trigger_read() {
	setType(IOREADABLE);
	timeval tv;
	gettimeofday(&tv, NULL);
	LCEVENT_DEBUG(this,"[time: %ld, %ld]" ,tv.tv_sec, tv.tv_usec);
	sock_data.timer.count = tv;
	backup_callback();
	setCallback(lcsockevent_readdone_callback, NULL);
	judge_read_timeout(tv);
	return repost();
}

int LcEvent :: trigger_write() {
	setType(IOWRITEABLE);
	timeval tv;
	gettimeofday(&tv, NULL);
	LCEVENT_DEBUG(this,"[time: %ld, %ld]" ,tv.tv_sec, tv.tv_usec);
	sock_data.timer.count = tv;
	backup_callback();
	this->setCallback(lcsockevent_writedone_callback, NULL);
	judge_write_timeout(tv);
	return repost();
}


int LcEvent :: awrite(int size) {
	int ret =0;
	if (0!= size) {
		sock_data.write_buf_used = size;
	}
	if (0 >= sock_data.write_buf_used || sock_data.write_buf_used > sock_data.write_buf_len) {
		LCEVENT_WARNING(this,"LcEvent(awrite) : to write invalid size(%d), buf_len(%d)",
				sock_data.write_buf_used, sock_data.write_buf_len);
		ret = -1;
	} else {
		timeval tv;
		gettimeofday(&tv, NULL);
		LCEVENT_DEBUG(this,"[time: %ld, %ld]" ,tv.tv_sec, tv.tv_usec);
		sock_data.timer.count = tv;
		backup_callback();
		this->setCallback(lcsockevent_writedone_callback, NULL);
		this->write(this->handle(), sock_data.write_buf, sock_data.write_buf_used);
		judge_write_timeout(tv);
		ret = repost();
	}
	return ret;
}

void LcEvent :: release_read_buf() {
	if ((sock_data.small_readbuf != sock_data.read_buf) && (NULL != sock_data.read_buf)
			&& (0 == sock_data.ext_readbuf)) {
		lc_event_free(sock_data.read_buf, _fmemory_manager, sock_data.read_buf_len);
	}
	sock_data.read_buf = sock_data.small_readbuf;
	sock_data.read_buf_len = sizeof(sock_data.small_readbuf);
	sock_data.read_buf_used = 0;
}
void LcEvent :: release_write_buf() {
	if ((sock_data.small_writebuf != sock_data.write_buf) && (NULL != sock_data.write_buf)
			&& (0 == sock_data.ext_writebuf)) {
		lc_event_free(sock_data.write_buf, _fmemory_manager, sock_data.write_buf_len);
	}
	sock_data.write_buf = sock_data.small_writebuf;
	sock_data.write_buf_len = sizeof(sock_data.small_writebuf);
	sock_data.write_buf_used = 0;
}

void LcEvent :: release_sock_buf() {
	release_read_buf();
	release_write_buf();
}

LcEvent :: ~LcEvent() {
	release_sock_buf();
	if (0 <= handle()) {
		::close(handle());
	}
}

Lsc::string LcEvent :: get_notice_string(){
	unsigned char *ip = (unsigned char *)&sock_data.client_addr.sin_addr.s_addr;
	Lsc::string ret;
	ret.setf( "[event:%p][ proctime:total:%u(ms) activate:%u+read:%u+proc:%u+write:%u ]" \
			"[ reqip:%u.%u.%u.%u ]",this,
		    sock_data.timer.total, sock_data.timer.connect,
		    sock_data.timer.read, sock_data.timer.proc, sock_data.timer.write,
		    ip[0], ip[1], ip[2], ip[3]);
	return ret.append(_fnotice_string);

}

void LcEvent :: push_notice_string(const char * fmt, ...) {
	va_list args;
	va_start(args, fmt);
	_fnotice_string.vappendf(fmt,args);
	va_end(args);
}

void LcEvent :: push_notice_pair(const char *key, const char *value) {
	_fnotice_string.appendf("[%s:%s]",key,value);
}















int LcEvent :: set_connecttype(int tp) {
	if (tp != LCSVR_LONG_CONNECT  &&  tp != LCSVR_SHORT_CONNECT) {
		LCEVENT_WARNING(this,"LcEvent(set_connecttype) : invalid type");
		return -1;
	}
	sock_data.connect_type = tp;
	return 0;
}
int LcEvent :: get_connecttype() {
	return sock_data.connect_type;
}

void LcEvent :: clear_notice_string(){
	_fnotice_string = "";
}

int LcEvent :: set_read_timeout(int to) {
	if (to<0) {
		LCEVENT_WARNING(this,"LcEvent(set_read_timeout) : timeout(%d)<0", to);
		return -1;
	}
	sock_data.read_timeout = to;
	return 0;
}
int LcEvent :: set_write_timeout(int to) {
	if (to<0) {
		LCEVENT_WARNING(this,"LcEvent(set_write_timeout) : timeout(%d)<0", to);
		return -1;
	}
	sock_data.write_timeout = to;
	return 0;
}

int LcEvent :: judge_read_timeout(timeval &now) {
	int rto = sock_data.read_timeout;
	int procto = sock_data.process_timeout;
	int pto = 0;
	if (_first_rwto) {
		rto = sock_data.connect_timeout;
	} else {
		if (procto >0 ) {
			pto = tvTv2Ms(tvSlc(now, sock_data.timer.start));
			pto = procto - pto;
		}
		if (0 < pto && (pto < rto || 0 >= rto)) {
			rto = pto;
		}
	}
	if (0 < rto) {
		timeval rtotv = now;
		event_tvABS(&rtotv, rto);






		this->setTimeout_tv(&rtotv);
	}
	return 0;
}
int LcEvent :: judge_write_timeout(timeval &now) {
	int rto = sock_data.write_timeout;
	int procto = sock_data.process_timeout;
	int pto = 0;
	if (_first_rwto) {
		rto = sock_data.connect_timeout;
	} else {
		if (procto >0 ) {
			pto = tvTv2Ms(tvSlc(now, sock_data.timer.start));
			pto = procto - pto;
		}
		if (0 < pto && (pto < rto || 0 >= rto)) {
			rto = pto;
		}
	}
	if (0 < rto) {
		timeval rtotv = now;
		event_tvABS(&rtotv, rto);






		this->setTimeout_tv(&rtotv);
	}
	return 0;
}

LcEvent :: LcEvent() : _fstart_ss(1),_fpre_result(0),_fmemory_manager(NULL),
		_freaddone_callback(NULL) {
	memset(&sock_data, 0, sizeof(sock_data));
	sock_data.event = this;
	sock_data.read_buf = sock_data.small_readbuf;
	sock_data.read_buf_len = sizeof(sock_data.small_readbuf);
	sock_data.read_buf_used = 0;
	sock_data.write_buf = sock_data.small_writebuf;
	sock_data.write_buf_len = sizeof(sock_data.small_writebuf);
	sock_data.write_buf_used = 0;
	sock_data.sock_opt = 0;
	sock_data.status = 0;
	sock_data.connect_type = -1;
    _flogid = 0;
}

LcEvent :: LcEvent(IReactor * first) :_fstart_ss(1) ,_fpre_result(0), _fmemory_manager(NULL),
		_freaddone_callback(NULL){
	memset(&sock_data, 0, sizeof(sock_data));
	sock_data.event = this;
	sock_data.read_buf = sock_data.small_readbuf;
	sock_data.read_buf_len = sizeof(sock_data.small_readbuf);
	sock_data.read_buf_used = 0;
	sock_data.write_buf = sock_data.small_writebuf;
	sock_data.write_buf_len = sizeof(sock_data.small_writebuf);
	sock_data.write_buf_used = 0;
	sock_data.sock_opt = 0;
	sock_data.status = 0;
	sock_data.connect_type = -1;
	sock_data.first_reactor = first;
    _flogid = 0;
}

void LcEvent :: set_firstreactor(IReactor *rct) {
	sock_data.first_reactor = rct;
}

void LcEvent :: init(int read_to, int write_to, int connect_to, int proc_to, unsigned int max_bufsize) {
	sock_data.read_timeout = read_to;
	sock_data.write_timeout = write_to;
	sock_data.process_timeout = proc_to;
	sock_data.connect_timeout = connect_to;
	sock_data.max_buffer_size = max_bufsize;
}

void LcEvent :: event_error_callback() {
	LCEVENT_WARNING(this,"LcEvent(event_error_callback)");
	::close(handle());
	this->setHandle(-1);
}

Lsc::string LcEvent :: status_to_string(unsigned int st) {
	Lsc::string res;
	unsigned sockst = st & 0xFF;
	switch (sockst) {
	case STATUS_READ:
		res = "READ";
		break;
	case STATUS_WRITE:
		res = "WRITE";
		break;
	case STATUS_ACCEPTED:
		res = "ACCEPT";
		break;
	default :
		res = "UNKNOWN_STATUS";
		break;
	}
	if (st & STATUS_ERROR) {
		res.append("|ERROR");
	}
	if (st & STATUS_TIMEOUT) {
		res.append("|TIMEOUT");
	}
	if (st & STATUS_CANCEL) {
		res.append("|CANCEL");
	}
	if (st & STATUS_CLOSESOCK) {
		res.append("|CLOSE_SOCKET");
	}
	if (st & STATUS_POSTERROR) {
		res.append("|POST_ERROR");
	}
	return res;
}

void LcEvent :: active_sock_opt(){
	if (sock_data.sock_opt & LCSVR_NODELAY) {
		int on = 1;
		setsockopt(handle(), IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on));
	}
	if (sock_data.sock_opt & LCSVR_LINGER_CLOSE) {
		struct linger li;
		memset(&li, 0, sizeof(li));
		li.l_onoff = 1;
		li.l_linger = 1;
		setsockopt(handle(), SOL_SOCKET, SO_LINGER, (const char*)&li, sizeof(li) );
	}
}

void LcEvent :: set_post_type(int ty) {
	_fstart_ss = ty;
}

void LcEvent :: set_read_maxbuf(unsigned int rdmax) {
	sock_data.max_readbuf_size = rdmax;
}
void LcEvent :: set_write_maxbuf(unsigned int wtmax) {
	sock_data.max_writebuf_size = wtmax;
}

void LcEvent :: set_readbuf(char * buf, unsigned int size) {
	release_read_buf();
	sock_data.ext_readbuf = 1;
	sock_data.read_buf = buf;
	sock_data.read_buf_len = size;
	sock_data.read_buf_used = 0;
}
void LcEvent :: set_writebuf(char * buf, unsigned int size) {
	release_write_buf();
	sock_data.ext_writebuf = 1;
	sock_data.write_buf = buf;
	sock_data.write_buf_len = size;
	sock_data.write_buf_used = 0;
}

void LcEvent :: use_inter_readbuf() {
	sock_data.ext_readbuf = 0;
	sock_data.read_buf = NULL;
	release_read_buf();
}
void LcEvent :: use_inter_writebuf() {
	sock_data.ext_writebuf = 0;
	sock_data.write_buf =  NULL;
	release_write_buf();
}

int LcEvent :: event_read() {
	return -1;
}

void LcEvent :: session_begin() {
	LCEVENT_DEBUG(this,"LcEvent(session_begin)[handle:%d]", handle());
	this->_fpre_result = session_prebegin();
	if (0!=_fpre_result) {
		LCEVENT_WARNING(this,"LcEvent(session_begin) : pre begin error");
		setType(IEvent::ERROR);
		setResult(IEvent::ERROR);
		set_sock_status(LcEvent::STATUS_ACCEPTED |LcEvent::STATUS_PRESESSIONERROR);
		event_error_callback();
		return;
	}
	
	if (0 > this->event_read()) {
		LCEVENT_WARNING(this,"LcEvent(session_begin) : event read error");
		setType(IEvent::ERROR);
		setResult(IEvent::ERROR);
		set_sock_status(LcEvent::STATUS_ACCEPTED |LcEvent::STATUS_POSTERROR);
		event_error_callback();
		return;
	}
}

int LcEvent :: session_prebegin() {
	return 0;
}

int LcEvent :: get_presession_result() {
	return _fpre_result;
}

int LcEvent :: if_post() {
	return 1;
}

Lsc::mempool * LcEvent :: get_memmgr() {
	return _fmemory_manager;
}

void  LcEvent :: set_memmgr(Lsc::mempool *mgr) {
	_fmemory_manager = mgr;
}

}


