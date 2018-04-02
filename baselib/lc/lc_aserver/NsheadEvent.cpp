
#include <lc/ievent.h>
#include <lc/lcevent.h>
#include "NsheadEvent.h"
#include <lc_miscdef.h>
#include "event_log.h"

namespace lc {

int NsheadEvent::set_allocator(allocator_t *ma)
{
	if(_setflag) {
		return -1;
	}
	if(ma == NULL || ma->allocate_func == NULL || ma->release_func == NULL) {
		return -1;
	}
	_allocator.allocate_func = ma->allocate_func;
	_allocator.release_func  = ma->release_func;
	_allocator.userdata      = ma->userdata;
	_setflag = true;
	return 0;
}

void* NsheadEvent::allocator_allocate(size_t size) 
{
	if(!_setflag)
		return NULL;
	return _allocator.allocate_func(size, _allocator.userdata);
}

void NsheadEvent::allocator_release(void *ptr, size_t size) 
{
	if(!_setflag || ptr == NULL)
		return;
	_allocator.release_func(ptr, size, _allocator.userdata);
}

bool NsheadEvent::has_allocator()
{
	return _setflag;
}

void* NsheadEvent::allocator_allocate_nsbody(size_t size) 
{
	_user_nsbody = allocator_allocate(size);
	if(_user_nsbody) {
		_user_nsbody_size = size;
	}
	return _user_nsbody;
}

void NsheadEvent::allocator_release_nsbody() 
{
	allocator_release(_user_nsbody, _user_nsbody_size);
	_user_nsbody = NULL;
	_user_nsbody_size = 0;
}

NsheadEvent::~NsheadEvent()
{
	if(_setflag && _user_nsbody) {
		allocator_release_nsbody();
	}
}


static int lnhead_aserver_post_monitor(NsheadEvent *ev) {
	lnhead_t * head = (lnhead_t *)ev->get_read_lnhead();
	if (strcmp(LC_MONITOR_PROVIDER, head->provider) == 0) {
		lnhead_t *res_head = (lnhead_t *)ev->get_write_buffer(sizeof(lnhead_t));
		if (NULL == res_head) {
			LCEVENT_WARNING(ev,"lnhead_aserver_post_monitor : [fd:%d]get_write_buffer(lnhead_t) failed.",
					ev->handle());
			return 0;
		}

		*res_head = *head;
		res_head->body_len =0;
		LCEVENT_TRACE(ev, "lnhead_aserver_post_monitor : return LC_MONITOR pack");
		if (0 != ev->lnhead_awrite() ) {
			ev->set_sock_status(LcEvent::STATUS_WRITE | LcEvent::STATUS_POSTERROR);
			LCEVENT_WARNING(ev,"lnhead_aserver_post_monitor : [fd:%d]post write event failed.",
					ev->handle());
		}
		return 1;
	}
	return 0;
}

void NsheadEvent :: set_reqsvrname(const char * svrname) {
	if (NULL == svrname) {
		_freq_svrname = "";
	} else {
		_freq_svrname = svrname;
	}
}

static void store_lnhead_msg(NsheadEvent *ev) {
	lnhead_t *req_head = (lnhead_t *) ev->get_read_lnhead();
	ev->set_logid(req_head->log_id);
	ev->set_reqsvrname(req_head->provider);
}

static void lnhead_aserver_aread_callback2(IEvent *e, void * )
{
	NsheadEvent *ev = static_cast<NsheadEvent *>(e);
	lc_timecount_t * timer = &(ev->get_sock_data()->timer);
	timeval tv;
	gettimeofday(&tv, NULL);
	LCEVENT_DEBUG(ev,"[time: %ld, %ld]" ,tv.tv_sec, tv.tv_usec);
	timer->read += tvTv2Ms(tvSlc(tv, timer->count));
	if (ev->isError()) {



		if(ev->has_allocator())
			ev->allocator_release_nsbody();

		ev->set_sock_status(LcEvent::STATUS_READ);
		LCEVENT_WARNING(ev,"lnhead read body error[%m]");
		ev->event_error_callback();
		return;
	}
	if ((int)ev->retCount() != (int)ev->needCount()) {

		if(ev->has_allocator())
			ev->allocator_release_nsbody();

		ev->setResult(IEvent::ERROR);
		ev->set_sock_status(LcEvent::STATUS_READ);
		LCEVENT_WARNING(ev,"lnhead read checking bodylen error[%m]");
		ev->event_error_callback();
		return;
	}

	LCEVENT_DEBUG(ev,"lnhead_aserver_aread_callback2 : done");

	if (ev->get_monitor()) {
		if (lnhead_aserver_post_monitor(ev)) {
			return;
		}
	}

	ev->setType(ev->type());
	store_lnhead_msg(ev);
	ev->set_sock_status(LcEvent::STATUS_READ);
	timer->proccount = tv;
	ev->read_done_callback();
	gettimeofday(&tv, NULL);
	
	timer->proc += tvTv2Ms(tvSlc(tv, timer->proccount));
	timer->count = tv;

	

}

static void lnhead_aserver_aread_callback(IEvent *e, void * )
{
	NsheadEvent *ev = static_cast<NsheadEvent *>(e);
	if (ev->isError()) {
		lc_timecount_t * timer = &(ev->get_sock_data()->timer);
		timeval tv;
		gettimeofday(&tv, NULL);
		LCEVENT_DEBUG(ev,"[time: %ld, %ld]" ,tv.tv_sec, tv.tv_usec);
		timer->read += tvTv2Ms(tvSlc(tv, timer->count));

		ev->nsrestore_callback();
		ev->set_sock_status(LcEvent::STATUS_READ);
		LCEVENT_WARNING(ev,"lnhead read head error[%m]");
		ev->event_error_callback();
		return;
	}

	if ((int)ev->retCount() != (int)ev->needCount()) {
		lc_timecount_t * timer = &(ev->get_sock_data()->timer);
		timeval tv;
		gettimeofday(&tv, NULL);
		LCEVENT_DEBUG(ev,"[time: %ld, %ld]" ,tv.tv_sec, tv.tv_usec);
		timer->read += tvTv2Ms(tvSlc(tv, timer->count));
		ev->setType(IEvent::ERROR);
		ev->setResult(IEvent::ERROR);
		ev->nsrestore_callback();
		ev->set_sock_status(LcEvent::STATUS_READ);
		LCEVENT_WARNING(ev,"lnhead read head error[%m]");
		ev->event_error_callback();
		return;
	}
	if (lnhead_aserver_post_monitor(ev)) {
		store_lnhead_msg(ev);
		return;
	}

	volatile lnhead_t * head = (lnhead_t *)ev->get_read_lnhead();
	ev->set_logid(head->log_id);

	
	if(head->body_len > 0 && ev->has_allocator()) {
		
		ev->allocator_release_nsbody();
		if(ev->allocator_allocate_nsbody(head->body_len) == NULL) {
			e->setResult(IEvent::ERROR);
			ev->nsrestore_callback();
			ev->set_sock_status(LcEvent::STATUS_READ | LcEvent::STATUS_MEMERROR);
			LCEVENT_WARNING(ev,"NsheadEvent: allocator_allocate_nsbody(%d) return null)", head->body_len);
			ev->event_error_callback();
			return;
		}
	}

	
	char *body = NULL;
	if(!ev->has_allocator() || head->body_len > 0) {
			body = (char *)ev->get_read_nsbody(sizeof(lnhead_t)+ head->body_len, 1);
			if (NULL == body) {
				e->setResult(IEvent::ERROR);
				ev->nsrestore_callback();
				ev->set_sock_status(LcEvent::STATUS_READ | LcEvent::STATUS_MEMERROR);
				LCEVENT_WARNING(ev,"NsheadEvent(lnhead_aserver_aread_callback) : get lnhead body readbuf error");
				ev->event_error_callback();
				return;
			}
	}

    
	head = (lnhead_t *)ev->get_read_lnhead();
	if (head->body_len == 0) {
		LCEVENT_TRACE(ev,"lnhead_aserver_aread_callback : only header");
		ev->setType(ev->type());
		ev->set_sock_status(LcEvent::STATUS_READ);
		ev->read_done_callback();
		
		return;
	}

	ev->read(ev->handle(), (void *)body, head->body_len);
	ev->setCallback(lnhead_aserver_aread_callback2, e);
	ev->get_sock_data()->read_buf_used = head->body_len + sizeof(lnhead_t);
	if ( 0!= ev->smart_repost()) {

		if(ev->has_allocator())
			ev->allocator_release_nsbody();


		ev->setType(IEvent::ERROR);
		ev->setResult(IEvent::ERROR);
		ev->nsrestore_callback();
		ev->set_sock_status(LcEvent::STATUS_READ | LcEvent::STATUS_POSTERROR);
		LCEVENT_WARNING(ev,"NsheadEvent(lnhead_aserver_aread_callback) : event(%p) post failed.", ev);
		ev->event_error_callback();
	}
}

int NsheadEvent :: lnhead_aread()
{
	nsbackup_callback();
	lnhead_t * head = (lnhead_t *)get_read_buffer(sizeof(lnhead_t));
	if (NULL == head) {
		LCEVENT_WARNING(this,"NsheadEvent(lnhead_aread) : get lnhead readbuf error");
		nsrestore_callback();
		return -1;
	}
	LCEVENT_DEBUG(this,"NsheadEvent(lnhead_aread) begin [handle:%d]", this->handle());
	head->magic_num = NSHEAD_MAGICNUM;
	setCallback(lc::lnhead_aserver_aread_callback, this);
	this->read(this->handle(), sock_data.read_buf, sizeof(lnhead_t));
	sock_data.read_buf_used = sizeof(lnhead_t);
	timeval tv;
	gettimeofday(&tv, NULL);
	LCEVENT_DEBUG(this,"[time: %ld, %ld]" ,tv.tv_sec, tv.tv_usec);
	sock_data.timer.count = tv;
	judge_read_timeout(tv);
	this->setDevided(0);
	_fread_div = 0;
	return repost();
}

























































































































































































































































































static void lnhead_aserver_awrite_callback(IEvent *e, void * )
{
	NsheadEvent *ev = static_cast<NsheadEvent *>(e);
	lc_timecount_t * timer = &(ev->get_sock_data()->timer);
	timeval tv;
	
	gettimeofday(&tv, NULL);
	LCEVENT_DEBUG(ev,"[time: %ld, %ld]" ,tv.tv_sec, tv.tv_usec);
	timer->write += tvTv2Ms(tvSlc(tv, timer->count));
	ev->nsrestore_callback();
	if (ev->isError()) {
		ev->set_sock_status(LcEvent::STATUS_WRITE);
		LCEVENT_WARNING(ev,"[fd:%d]write lnhead error[%m]", ev->handle());
		ev->event_error_callback();
		return;
	}
	if ((int)ev->retCount() != (int)ev->needCount()) {
		ev->setResult(IEvent::ERROR);
		ev->set_sock_status(LcEvent::STATUS_WRITE);
		LCEVENT_WARNING(ev,"[fd:%d][write:%d][needwrite:%d]write lnhead error[%m]",
				ev->handle(),(int)ev->retCount(),(int)ev->needCount());
		ev->event_error_callback();
		return;
	}
	ev->setType(ev->type());
	ev->set_sock_status(LcEvent::STATUS_WRITE);
	timer->proccount = tv;
	ev->write_done_callback();
	gettimeofday(&tv, NULL);
	LCEVENT_DEBUG(ev,"[time: %ld, %ld]" ,tv.tv_sec, tv.tv_usec);
	timer->proc += tvTv2Ms(tvSlc(tv, timer->proccount));
	timer->count = tv;
}

int NsheadEvent :: lnhead_awrite()
{
	lnhead_t * head = (lnhead_t *)sock_data.write_buf;
	if (sizeof(lnhead_t) > sock_data.write_buf_len) {
		LCEVENT_WARNING(this,"NsheadEvent(lnhead_awrite) buffer size(%d) < lnheader size", sock_data.write_buf_len);
		return -1;
	}
	sock_data.write_buf_used = sizeof(lnhead_t) + head->body_len;
	if (sock_data.write_buf_used > sock_data.write_buf_len) {
		LCEVENT_WARNING(this,"NsheadEvent(lnhead_awrite) buffer size(%d) < lnhead package size(%d)",
				sock_data.write_buf_len, sock_data.write_buf_used);
		return -1;
	}
	nsbackup_callback();
	head->magic_num = NSHEAD_MAGICNUM;
	this->setCallback(lc::lnhead_aserver_awrite_callback, this);
	this->write(this->handle(), sock_data.write_buf, sock_data.write_buf_used);
	timeval tv;
	gettimeofday(&tv, NULL);
	LCEVENT_DEBUG(this,"[time: %ld, %ld]" ,tv.tv_sec, tv.tv_usec);
	sock_data.timer.count = tv;
	judge_write_timeout(tv);
	_fwrite_div = 0;
	return repost();
}

void NsheadEvent :: nsbackup_callback() {
	this->getCallback(&_fold_nscb, &_fold_nscbp);
}
void NsheadEvent :: nsrestore_callback() {
	this->setCallback(_fold_nscb, _fold_nscbp);
}

void NsheadEvent :: set_cmdno(int cmdno) {
	_fcmdno = cmdno;
}
int NsheadEvent :: get_cmdno() {
	return _fcmdno;
}



Lsc::string NsheadEvent :: get_notice_string(){
	unsigned char *ip = (unsigned char *)&sock_data.client_addr.sin_addr.s_addr;
	Lsc::string ret;
	ret.appendf( "[ logid:%u  ]" "[ proctime:total:%u(ms) activate:%u+read:%u+proc:%u+write:%u ]" \
			"[ reqip:%u.%u.%u.%u ]" "[ reqsvr:%s ]"\
		    "[ cmdno:%d ]" "[ event:%p ]",_flogid,
		    sock_data.timer.total, sock_data.timer.connect,
		    sock_data.timer.read, sock_data.timer.proc, sock_data.timer.write,
		    ip[0], ip[1], ip[2], ip[3],
		    _freq_svrname.c_str(), _fcmdno,
		    this);
	return ret.append(_fnotice_string);

}

void NsheadEvent :: set_monitor(int mon) {
	this->_fmonitor = mon;
}

int NsheadEvent :: get_monitor() {
	return _fmonitor;
}

int NsheadEvent :: event_read() {
	return lnhead_aread();
}

const lnhead_t * NsheadEvent :: get_read_lnhead() {
	if (_fread_div) {
		return _fread_head;
	}
	return (const lnhead_t *) this->get_read_buffer();
}
const void * NsheadEvent :: get_read_nsbody(int pkgsize, int copy) {
	if(_setflag) {
		return _user_nsbody;
	}
	
	if (_fread_div) {
		
		if (int(_fread_body_buflen + sizeof(lnhead_t)) >= pkgsize) {
			return _fread_body;
		} else {
			return NULL;
		}
	}
	const lnhead_t * head = (const lnhead_t *) this->get_read_buffer(pkgsize, copy);
	if (NULL == head) {
		return NULL;
	}
	return (const void *)(head + 1);
}

lnhead_t * NsheadEvent :: get_write_lnhead() {
	if (_fwrite_div) {
		return _fwrite_head;
	}
	if (sizeof(lnhead_t) > sock_data.write_buf_len) {
		return NULL;
	}
	return (lnhead_t *) this->get_write_buffer();
}
void * NsheadEvent :: get_write_nsbody() {
	if (_fwrite_div) {
		return _fwrite_body;
	}
	if (sizeof(lnhead_t) > sock_data.write_buf_len) {
		return NULL;
	}
	lnhead_t * head = (lnhead_t *) this->get_write_buffer();
	return (void *)(head + 1);
}



int NsheadEvent :: lnhead_aread_2buf(lnhead_t *head, void * body, int bodylen) {
	if (NULL == head) {
		LCEVENT_WARNING(this,"NsheadEvent(lnhead_aread_2buf) : get lnhead readbuf error");
		setResult(IEvent::ERROR);
		set_sock_status(LcEvent::STATUS_POSTERROR);
		return -1;
	}
	LCEVENT_DEBUG(this,"NsheadEvent(lnhead_aread_2buf) begin [handle:%d]", this->handle());
	head->magic_num = NSHEAD_MAGICNUM;
	setCallback(lc::lnhead_aserver_aread_callback, this);
	this->read(this->handle(), head, sizeof(lnhead_t));
	timeval tv;
	gettimeofday(&tv, NULL);
	LCEVENT_DEBUG(this,"[time: %ld, %ld]" ,tv.tv_sec, tv.tv_usec);
	sock_data.timer.count = tv;
	judge_read_timeout(tv);
	this->setDevided(0);
	_fread_div = 1;
	_fread_head = head;
	_fread_body = body;
	_fread_body_buflen = bodylen;
	return repost();
}



static void lnhead_aserver_awrite2buf_callback(IEvent *e, void * )
{
	NsheadEvent *ev = static_cast<NsheadEvent *>(e);
	ev->lnhead_write2buf_callback();
}

void NsheadEvent :: lnhead_write2buf_callback() {
	lc_timecount_t * timer = &(sock_data.timer);
	if (this->isError()) {
		this->set_sock_status(LcEvent::STATUS_WRITE);
		LCEVENT_WARNING(this,"[fd:%d]write lnhead error[%m]", this->handle());
		this->event_error_callback();
		return;
	}
	if ((int)this->retCount() != (int)this->needCount()) {
		this->setResult(IEvent::ERROR);
		this->set_sock_status(LcEvent::STATUS_WRITE);
		LCEVENT_WARNING(this,"[fd:%d][write:%d][needwrite:%d]write lnhead error[%m]",
				this->handle(),(int)this->retCount(),(int)this->needCount());
		this->event_error_callback();
		return;
	}

	switch (_fwrite_div) {
	case 1 :
		_fwrite_div = 2;
		this->setCallback(lc::lnhead_aserver_awrite2buf_callback, this);
		this->write(this->handle(), _fwrite_body, _fwrite_head->body_len);
		if ( 0!= smart_repost()) {
			setType(IEvent::ERROR);
			setResult(IEvent::ERROR);
			set_sock_status(LcEvent::STATUS_WRITE | LcEvent::STATUS_POSTERROR);
			LCEVENT_WARNING(this,"NsheadEvent(lnhead_write2buf_callback) : event(%p) post failed.", this);
			event_error_callback();
		}
		break;
	case 2 :
		timeval tv;
		
		gettimeofday(&tv, NULL);
		LCEVENT_DEBUG(this,"[time: %ld, %ld]" ,tv.tv_sec, tv.tv_usec);
		timer->write += tvTv2Ms(tvSlc(tv, timer->count));
		this->setType(this->type());
		this->set_sock_status(LcEvent::STATUS_WRITE);
		timer->proccount = tv;
		this->write_done_callback();
		gettimeofday(&tv, NULL);
		LCEVENT_DEBUG(this,"[time: %ld, %ld]" ,tv.tv_sec, tv.tv_usec);
		timer->proc += tvTv2Ms(tvSlc(tv, timer->proccount));
		timer->count = tv;
		break;
	default :
		this->setResult(IEvent::ERROR);
		this->set_sock_status(LcEvent::STATUS_WRITE);
		LCEVENT_WARNING(this,"lnhead_write2buf_callback : wrong status %d",_fwrite_div);
		this->event_error_callback();
		break;
	}
}

int NsheadEvent :: lnhead_awrite_2buf(lnhead_t *head, void * body, int bodylen) {
	if ((int)head->body_len > bodylen) {
		LCEVENT_WARNING(this,"NsheadEvent(lnhead_awrite_2buf) buffer size(%d) < lnhead body size(%d)",
				bodylen, head->body_len);
		return -1;
	}
	head->magic_num = NSHEAD_MAGICNUM;
	this->setCallback(lc::lnhead_aserver_awrite2buf_callback, this);
	this->write(this->handle(), head, sizeof(lnhead_t));
	timeval tv;
	gettimeofday(&tv, NULL);
	LCEVENT_DEBUG(this,"lnhead_awrite_2buf[time: %ld, %ld]" ,tv.tv_sec, tv.tv_usec);
	sock_data.timer.count = tv;
	judge_write_timeout(tv);
	_fwrite_div = 1;
	_fwrite_head = head;
	_fwrite_body = body;
	_fwrite_body_buflen = bodylen;
	return repost();
}

}

