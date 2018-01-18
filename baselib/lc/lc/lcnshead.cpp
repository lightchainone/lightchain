
#include <netinet/tcp.h>
#include "lcnshead.h"

namespace lc
{
	static void nshead_aread_default_callback2(IEvent *e, void *p)
	{
		SockEvent *ev = static_cast<SockEvent *>(e);
		nshead_asio_t *v = (nshead_asio_t *)p;
		if (ev->isError()) {
			v->err = NSHEAD_READ_BODY_ERROR;
			LC_IN_WARNING("nshead read body error[%m]");
			v->cb(e, p);
			return;
		}
		if ((int)ev->retCount() != (int)ev->needCount()) {
			v->err = NSHEAD_READ_BODY_ERROR;
			LC_IN_WARNING("nshead read checking bodylen error[%m]");
			v->cb(e, p);
			return;
		}
		
		v->cb(e, p);
	}

	static void nshead_aread_default_callback(IEvent *e, void *p)
	{
		SockEvent *ev = static_cast<SockEvent *>(e);
		nshead_asio_t *v = (nshead_asio_t *)p;
		v->err = NSHEAD_SUCCESS;
		if (ev->isError()) {
			v->err = NSHEAD_READ_HEAD_ERROR;
			//LC_IN_WARNING("nshead read head error[%m]");
			v->cb(e, p);
			return;
		}
		if ((int)ev->retCount() != (int)ev->needCount()) {
			LC_IN_WARNING("nshead read head error[%m]");
			v->err = NSHEAD_READ_HEAD_ERROR;
			v->cb(e, p);
			return;
		}
                // check magic num
                if (v->head->magic_num !=  NSHEAD_MAGICNUM){
			LC_IN_WARNING("nshead magic_num error[%u]", v->head->magic_num);
			v->err = NSHEAD_READ_HEAD_ERROR;
			v->cb(e, p);
			return;
                }
		
		if (v->head->body_len > v->len - sizeof(nshead_t)) {
			e->setResult(IEvent::ERROR);
			v->err = NSHEAD_READ_BODY_BUF_NO_ENOUGH;
			LC_IN_WARNING("nshead read head done ,but find not enought buffer wanna[%d] has[%ld]",
					v->head->body_len, (long)(v->len - sizeof(nshead_t)));
			v->cb(e, p);
			return;
		}

		if (v->head->body_len == 0) {
			v->cb(e, p);
			return;
		}

		ev->read(ev->handle(), v->head+1, v->head->body_len);
		ev->setCallback(nshead_aread_default_callback2, p);
		
		if (ev->reactor()->smartPost(ev) != 0) {
			LC_IN_WARNING("post error");
		}
	}

	int nshead_aread(SockEvent *se, nshead_asio_t *p)
	{
		p->head->magic_num = NSHEAD_MAGICNUM;
		se->read(se->handle(), p->head, sizeof(nshead_t));
		se->setCallback(nshead_aread_default_callback, p);
		return 0;
	}


	static void nshead_awrite_deault_callback(IEvent *e, void *p)
	{
		SockEvent *ev = static_cast<SockEvent *>(e);
		nshead_asio_t *v = (nshead_asio_t *)p;
		v->err = NSHEAD_SUCCESS;

		if (ev->isError()) {
			v->err = NSHEAD_WRITE_HEAD_ERROR;
			LC_IN_WARNING("write head error[%m]");
			v->cb(e, p);
			return;
		}
		if ((int)ev->retCount() != (int)ev->needCount()) {
			v->err = NSHEAD_WRITE_HEAD_ERROR;
			LC_IN_WARNING("write head error[%m]");
			v->cb(e, p);
			return;
		}

		v->cb(e, p);
	}

	int nshead_awrite(SockEvent *se, nshead_asio_t *p)
	{
		p->head->magic_num = NSHEAD_MAGICNUM;
		se->write(se->handle(), p->head, sizeof(nshead_t) + p->head->body_len);
		se->setCallback(nshead_awrite_deault_callback, p);
		return 0;
	}

	void set_nshead_asio(nshead_asio_t *p, nshead_t *head, 
			int len, IEvent::cb_t cb, void *param)
	{
		p->head = head;
		p->len = len;
		p->err = 0;
		p->cb = cb;
		p->param = param;
	}

	static void nshead_atalkwith_done_callback(IEvent *te, void *v)
	{
		nshead_asio_t *p = static_cast<nshead_asio_t *>(v);
		nshead_atalk_t *t = static_cast<nshead_atalk_t *>(p->param);
		SockEvent *e = static_cast<SockEvent *>(te);
		if (p->err != 0) {
			t->err = p->err;
		}
		t->cb(e, t);
	}

	static void nshead_atalkwith_default_callback(IEvent *te, void *v)
	{
		nshead_asio_t *p = static_cast<nshead_asio_t *>(v);
		nshead_atalk_t *t = static_cast<nshead_atalk_t *>(p->param);
		SockEvent *e = static_cast<SockEvent *>(te);
		if (e->isError()) {
			t->cb(e, t);
			t->err = NSHEAD_UNKNOW_ERROR;
			LC_IN_WARNING("unknow happend err(%d)", e->result());
			return;
		}
		set_nshead_asio(p, t->res, t->len, nshead_atalkwith_done_callback, t);
		nshead_aread(e, &t->tmp);
		if (e->reactor()->smartPost(e) != 0) {
			t->err = NSHEAD_POST_READ_ERROR;
			t->cb(e, t);
			LC_IN_WARNING("event post error when read nshead");
			return;
		}
	}

	int nshead_atalkwith(SockEvent *se, nshead_atalk_t *p)
	{
		p->req->magic_num = NSHEAD_MAGICNUM;
		p->res->magic_num = NSHEAD_MAGICNUM;
		set_nshead_asio(&p->tmp, p->req, 0, nshead_atalkwith_default_callback, p);
		return nshead_awrite(se, &p->tmp);
	}

	void set_nshead_atalk(nshead_atalk_t *p, nshead_t *req, nshead_t *res,
			int len, IEvent::cb_t cb, void *param)
	{
		p->req = req;
		p->res = res;
		p->len = len;
		p->err = 0;
		p->cb = cb;
		p->param = param;
	}

	void set_nshead_atalk_tcp_ip(nshead_atalk_tcp_t *p, const char *ip, int port,
			bool nodelay) 
	{
		snprintf(p->ip, sizeof(p->ip), "%s", ip);
		p->port = port;
		p->nodelay = nodelay;
	}

	static void nshead_default_tcpconnect_done(IEvent *te, void *v)
	{
		nshead_atalk_tcp_t *p = static_cast<nshead_atalk_tcp_t *>(v);
		SockEvent *e = static_cast<SockEvent *>(te);
		if (e->isError()) {
			p->err = NSHEAD_TCP_CONNECT_ERROR;
			p->cb(e, p);
			LC_IN_WARNING("tcp connect error %s:%d", p->ip, p->port);
			return;
		}
		nshead_atalkwith(e, (nshead_atalk_t *)(p));
		if (e->reactor()->smartPost(e) != 0) {
			LC_IN_WARNING("atalkwith error at ip[%s:%d] atfer listen",
					p->ip, p->port);
		}
	}

	int nshead_atalkwith_ip(SockEvent *se, nshead_atalk_tcp_t *p)
	{
		if (se->tcpConnect(p->ip, p->port) != 0) {
			LC_IN_WARNING("connect %s:%d error %m", p->ip, p->port);
			return -1;
		}

		if (p->nodelay) {
			const int on = 1;
			setsockopt(se->handle(), IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on));
		}

		se->setCallback(nshead_default_tcpconnect_done, p);
		p->err = NSHEAD_SUCCESS;
		return 0;
	}

	
	static void nshead_atalkwith_done_callback1(IEvent *te, void *v)
	{
		nshead_asio_t *p = static_cast<nshead_asio_t *>(v);
		nshead_atalk_t *t = static_cast<nshead_atalk_t *>(p->param);
		if (0 != p->err) {
			t->err = p->err;
		} else {
			t->err = NSHEAD_SUCCESS;
		}
		SockEvent *e = static_cast<SockEvent *>(te);
		t->cb(e, t);
	}

	static void nshead_aread_default_callback4(IEvent *e, void *p)
	{
		SockEvent *ev = static_cast<SockEvent *>(e);
		nshead_asio_t *v = (nshead_asio_t *)p;
		nshead_atalk_t *talk = (nshead_atalk_t *)(v->param);
	   
		timeval tm;
		gettimeofday(&tm, NULL);
		talk->realReadTime = int((tm.tv_sec*1000LL+tm.tv_usec/1000LL) - talk->startTime);

		if (ev->isError()) {
			LC_IN_WARNING("%s ev is error[%d] log_id[%u]", __func__, ev->result(),
						  talk->req->log_id);
			v->err = NSHEAD_READ_BODY_ERROR;
			if (NULL != talk->freecallback) {
				lc_callback_alloc_t allocarg;
				allocarg.buff = talk->resbuf;
				allocarg.size = v->head->body_len;
				allocarg.arg = talk->allocarg;
				talk->freecallback(&allocarg);
				talk->resbuf = NULL;
			}        
			v->cb(e, p);
			return;
		}
		if ((int)ev->retCount() != (int)ev->needCount()) {
			LC_IN_WARNING("%s ev ref is error ret[%d] need[%d] log_id[%u]", __func__, 
						  (int)ev->retCount(), (int)ev->needCount(), talk->req->log_id);
			v->err = NSHEAD_READ_BODY_ERROR;
			if (NULL != talk->freecallback) {
				lc_callback_alloc_t allocarg;
				allocarg.buff = talk->resbuf;
				allocarg.size = v->head->body_len;
				allocarg.arg = talk->allocarg;
				talk->freecallback(&allocarg);
				talk->resbuf = NULL;
			} 
			v->cb(e, p);
			return;
		}

		v->cb(e, p);
	}

	static void nshead_aread_default_callback3(IEvent *e, void *p)
	{
		SockEvent *ev = static_cast<SockEvent *>(e);
		nshead_asio_t *v = (nshead_asio_t *)p;
		nshead_atalk_t *talk = (nshead_atalk_t *)(v->param);
		v->err = NSHEAD_SUCCESS;
		if (ev->isError()) {
		    timeval tm;
		    gettimeofday(&tm, NULL);
		    talk->realReadTime = int((tm.tv_sec*1000LL+tm.tv_usec/1000LL) - talk->startTime);
			LC_IN_WARNING("%s ev->isError() ev(%ld) ref(%d) err(%d) log_id[%u] %m", __func__,
						  (long)ev, ev->getRefCnt(), ev->result(), talk->req->log_id);
			v->err = NSHEAD_READ_HEAD_ERROR;
			v->cb(e, p);
			return;
		}
		if ((int)ev->retCount() != (int)ev->needCount()) {
		    timeval tm;
		    gettimeofday(&tm, NULL);
		    talk->realReadTime = int((tm.tv_sec*1000LL+tm.tv_usec/1000LL) - talk->startTime);
			LC_IN_WARNING("%s :: recv head len error log_id[%u]", __func__, talk->req->log_id);
			v->err = NSHEAD_READ_HEAD_ERROR;
			v->cb(e, p);
			return;
		}

		if (NULL == talk->alloccallback) {
			if ((int)v->head->body_len > talk->len) {
		        timeval tm;
		        gettimeofday(&tm, NULL);
		        talk->realReadTime = int((tm.tv_sec*1000LL+tm.tv_usec/1000LL) - talk->startTime);
				LC_IN_WARNING("%s : revc body_len error reallen[%d] reslen[%d] log_id[%u]",
							  __func__, v->head->body_len, talk->len, talk->req->log_id);
				e->setResult(IEvent::ERROR);
				v->err = NSHEAD_READ_BODY_BUF_NO_ENOUGH;
			
				v->cb(e, p);
				return;
			}
		}

		if (v->head->body_len == 0) {
		    timeval tm;
		    gettimeofday(&tm, NULL);
		    talk->realReadTime = int((tm.tv_sec*1000LL+tm.tv_usec/1000LL) - talk->startTime);
			v->cb(e, p);
			return;
		}
	 
		
		
		if (NULL != talk->alloccallback) {
			lc_callback_alloc_t allocarg;
			allocarg.size = v->head->body_len;
			allocarg.buff = (void *)talk->resbuf;
			allocarg.arg = talk->allocarg;
			int ret = talk->alloccallback(&allocarg);
			if (ret != 0) {
		        timeval tm;
		        gettimeofday(&tm, NULL);
		        talk->realReadTime = int((tm.tv_sec*1000LL+tm.tv_usec/1000LL) - talk->startTime);
				v->err = NSHEAD_UNKNOW_ERROR;
				LC_IN_WARNING("%s user alloccallback error[%d] log_id[%u]", __func__, ret, 
								talk->req->log_id);
				v->cb(e, p);
				return;
			}
			
			talk->resbuf = (char *)allocarg.buff;
			
		}
		ev->read(ev->handle(), talk->resbuf, v->head->body_len);
		ev->setCallback(nshead_aread_default_callback4, p);
		if (ev->reactor()->smartPost(ev) != 0 ) {
		    timeval tm;
		    gettimeofday(&tm, NULL);
		    talk->realReadTime = int((tm.tv_sec*1000LL+tm.tv_usec/1000LL) - talk->startTime);
			LC_IN_WARNING("%s post error log_id[%u]", __func__, talk->req->log_id);
			talk->err = NSHEAD_POST_READ_ERROR;
			if (NULL != talk->freecallback) {
				lc_callback_alloc_t allocarg;
				allocarg.buff = talk->resbuf;
				allocarg.size = v->head->body_len;
				allocarg.arg = talk->allocarg;
				talk->freecallback(&allocarg);
				talk->resbuf = NULL;
			}        
			talk->cb(e, talk);
			return;
		}
	}

	int nshead_aread1(SockEvent *se, nshead_asio_t *p)
	{
		p->head->magic_num = NSHEAD_MAGICNUM;

		nshead_atalk_t * talk = (nshead_atalk_t*)(p->param);
		
		timeval tm;
		gettimeofday(&tm, NULL);
		talk->startTime = tm.tv_sec*1000LL + tm.tv_usec/1000LL;
		
		se->read(se->handle(), p->head, sizeof(nshead_t));
		se->setCallback(nshead_aread_default_callback3, p);
		return 0;
	}

	static void nshead_atalkwith_default_callback1(IEvent *te, void *v)
	{
		nshead_asio_t *p = static_cast<nshead_asio_t *>(v);
		nshead_atalk_t *t = static_cast<nshead_atalk_t *>(p->param);
		SockEvent *e = static_cast<SockEvent *>(te);
		
		if (0 != p->err) {
			t->err = p->err;
			t->cb(e, t);
			return;
		}
		set_nshead_asio(p, t->res, sizeof(nshead_t), nshead_atalkwith_done_callback1, t);
		
		e->setTimeout(t->readtimeout);
		
		nshead_aread1(e, p);
		if (e->reactor()->smartPost(e) != 0) {
			LC_IN_WARNING("%s post error log_id[%u]", __func__, t->req->log_id);
			t->err = NSHEAD_POST_READBODY_ERROR;
			t->cb(e, t);
			return;
		}
	}

	static void nshead_awrite_deault_callback2(IEvent *e, void *p)
	{
		SockEvent *ev = static_cast<SockEvent *>(e);
		nshead_asio_t *v = (nshead_asio_t *)p;
		nshead_atalk_t *talk = (nshead_atalk_t *)(v->param);

		timeval tm;
		gettimeofday(&tm, NULL);
		talk->realWriteTime = int((tm.tv_sec*1000LL + tm.tv_usec/1000LL) - talk->startTime);
		
		v->err = NSHEAD_SUCCESS;

		if (ev->isError()) {
			v->err = NSHEAD_WRITE_BODY_ERROR;
			LC_IN_WARNING("%s error[%d] log_id[%u]", __func__, ev->result(), talk->req->log_id);
			v->cb(e, p);
			return;
		}
		if ((int)ev->retCount() != (int)ev->needCount()) {
			v->err = NSHEAD_WRITE_BODY_ERROR;
			LC_IN_WARNING("%s error[buf not enough for body] log_id[%u]", __func__, talk->req->log_id);
			v->cb(e, p);
			return;
		}

		v->cb(e, p);
	}

	static void nshead_awrite_deault_callback1(IEvent *e, void *p)
	{
		SockEvent *se = static_cast<SockEvent *>(e);
		nshead_asio_t *v = (nshead_asio_t *)p;
		nshead_atalk_t *talk = (nshead_atalk_t *)(v->param);

		v->err = NSHEAD_SUCCESS;

		if (se->isError()) {
		    timeval tm;
		    gettimeofday(&tm, NULL);
		    talk->realWriteTime = int((tm.tv_sec*1000LL + tm.tv_usec/1000LL) - talk->startTime);
			LC_IN_WARNING("%s nshead.cpp : write head error err[%d] log_id[%u]", __func__, 
						  se->result(), talk->req->log_id);
			v->err = NSHEAD_WRITE_HEAD_ERROR;
			v->cb(e, p);
			return;
		}

		if ((int)se->retCount() != sizeof(nshead_t)) {
		    timeval tm;
		    gettimeofday(&tm, NULL);
		    talk->realWriteTime = int((tm.tv_sec*1000LL + tm.tv_usec/1000LL) - talk->startTime);
			LC_IN_WARNING("%s nshead.cpp : write head len error err[%d] log_id[%u]", __func__, 
						  se->result(), talk->req->log_id);
			v->err = NSHEAD_WRITE_HEAD_ERROR;
			v->cb(e, p);
			return;
		}

		
		if (0 == v->head->body_len) {
		    timeval tm;
		    gettimeofday(&tm, NULL);
		    talk->realWriteTime = int((tm.tv_sec*1000LL + tm.tv_usec/1000LL) - talk->startTime);
			v->cb(e,p);
			return;
		}
		se->write(se->handle(), talk->reqbuf, v->head->body_len);
		se->setCallback(nshead_awrite_deault_callback2, p);
		if (se->reactor()->smartPost(e) != 0) {
		    timeval tm;
		    gettimeofday(&tm, NULL);
		    talk->realWriteTime = int((tm.tv_sec*1000LL + tm.tv_usec/1000LL) - talk->startTime);
			LC_IN_WARNING("%s post error log_id[%u]", __func__, talk->req->log_id);
			talk->err = NSHEAD_POST_WRITEBODY_ERROR;
			talk->cb(e, talk);
			return;
		}
		
	}

	int nshead_awrite1(SockEvent *se, nshead_asio_t *p)
	{
		p->head->magic_num = NSHEAD_MAGICNUM;

		nshead_atalk_t *talk = (nshead_atalk_t *)(p->param);
		timeval tm;
		gettimeofday(&tm, NULL);
		talk->startTime = tm.tv_sec*1000LL + tm.tv_usec/1000LL;

		se->write(se->handle(), p->head, sizeof(nshead_t));
		se->setCallback(nshead_awrite_deault_callback1, p);
		return 0;
	}

	int nshead_atalkwith1(SockEvent *se, nshead_atalk_t *p)
	{
		p->req->magic_num = NSHEAD_MAGICNUM;
		p->res->magic_num = NSHEAD_MAGICNUM;
		set_nshead_asio(&p->tmp, p->req, sizeof(nshead_t), nshead_atalkwith_default_callback1, p);
		
		return nshead_awrite1(se, &p->tmp);
	}	

};



