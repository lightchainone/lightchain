#include "HttpEvent.h"
#include "event_log.h"
namespace lc {

static const char G_HTTP_CRLF[] = "\r\n"; 
static const char G_HTTP_CRLFCRLF[] = "\r\n\r\n"; 
static const int G_HTTP_INT_CRLFCRLF = *(int *)G_HTTP_CRLFCRLF; 
static const char G_HTTP_CONTENTLEN[] = "Content-Length"; 



static char * skip_http_blank(char * str) {
	if (NULL == str) {
		return NULL;
	}
	char * ret = str;
	while (*ret && (*ret == ' ')|| (*ret == '\t')) {
		ret++;
	}
	if (*ret) {
		return ret;
	}
	return NULL;
}


static char * http_find_field_nocase(char *src, const char *dest,int destlen) {
	if (NULL == src || NULL == dest) {
		return NULL;
	}
	char * srcstr = skip_http_blank(src);
    if (!srcstr) {
        return NULL;
    }
	for (;*srcstr;) {
		if (0 == strncasecmp(srcstr, dest, destlen)) {
			char * mh = skip_http_blank(srcstr + destlen);
			if (NULL ==mh) {
				return NULL;
			}
			if (':' == *mh) {
				return mh;
			}
		}
		LC_IN_DEBUG("http_find_field_nocase: srcstr:%s", srcstr);
		char * ctl = strstr(srcstr, G_HTTP_CRLF);
		if (NULL == ctl) {
			LC_IN_DEBUG("http_find_field_nocase: %s do not have %s", src, dest);
			return NULL;
		}
		srcstr = ctl +2;
	}
	return NULL;
}


static void lchttpevent_readhead_callback(IEvent *e, void * ) {
	HttpEvent *ev = static_cast<HttpEvent *>(e);
	ev->http_readheader_callback();
}

static void lchttpevent_readsection_callback(IEvent *e, void * ) {
	HttpEvent *ev = static_cast<HttpEvent *>(e);
	ev->http_readsection_callback();
}

static void lchttpevent_readpackage_callback(IEvent *e, void * ) {
	HttpEvent *ev = static_cast<HttpEvent *>(e);
	ev->http_readpackage_callback();
}

static void lchttpevent_readpackagedone_callback(IEvent *e, void * ) {
	HttpEvent *ev = static_cast<HttpEvent *>(e);
	ev->restore_callback();
	lc_timecount_t * timer = &(ev->get_sock_data()->timer);
	timeval tv;
	
	gettimeofday(&tv, NULL);
	timer->read += tvTv2Ms(tvSlc(tv, timer->count));
	
	timer->proccount = tv;
	ev->set_sock_status(LcEvent::STATUS_READ);
	if (ev->isError()) {
		LCEVENT_WARNING(ev,"LcEvent(lchttpevent_readpackagedone_callback) read error[%m]");
		ev->event_error_callback();
		return;
	}
	ev->read_done_callback();
	gettimeofday(&tv, NULL);
	timer->proc += tvTv2Ms(tvSlc(tv, timer->proccount));
	timer->count = tv;
}



int HttpEvent :: http_find_crlfcrlf(int rdlen) {
	int len = sock_data.read_buf_used - rdlen -3;
	if (0 > len) {
		len = 0;
		rdlen = sock_data.read_buf_used -3;
	}
	char * head = sock_data.read_buf + len;
	
	for (int i=0; i< rdlen; ++i, ++head) {
		if ( G_HTTP_INT_CRLFCRLF == (*(int *)head)) {
			return len + i+4;
		}
	}
	return -1;
}

void HttpEvent :: http_get_contentlength() {






	char *endptr = NULL;
	long long num;
	char * line;
	char * mh = http_find_field_nocase(sock_data.read_buf, G_HTTP_CONTENTLEN, strlen(G_HTTP_CONTENTLEN));
	if (NULL == mh) {
		LCEVENT_TRACE(this,"HttpEvent(http_get_contentlength): no Content-Length field.");
		goto NOLENGTH;
	}
	line = strstr(mh, G_HTTP_CRLF);
	if (NULL == mh || (line-mh <=0)) {
		LCEVENT_TRACE(this,"HttpEvent(http_get_contentlength): Content-Length, lack number value");
		goto NOLENGTH;
	}
	*line = 0;
	errno = 0;
	num = strtoll(mh+1, &endptr, 10);
	if (errno == ERANGE) {
		LCEVENT_TRACE(this,"HttpEvent(http_get_contentlength): Content-Length, out of range");
		*line = '\r';
		goto NOLENGTH;
	}
	if(*endptr != '\0'){
		LCEVENT_TRACE(this,"HttpEvent(http_get_contentlength): Content-Length, invalid number(%s)", mh+1);
		*line = '\r';
		goto NOLENGTH;
	}
	*line = '\r';
	LCEVENT_DEBUG(this,"HttpEvent(http_get_contentlength): set body length(%lld)", num);
	_fbody_length = num;
	return;
NOLENGTH:
	this->_fbody_length = 0;
}

int HttpEvent :: http_readheader_process(cb_t proc_cb) {
	int ret;
	if (isError()) {
		set_sock_status(LcEvent::STATUS_READ);
		goto END;
	}
	ret = recv(this->handle(), sock_data.read_buf + sock_data.read_buf_used ,
			sock_data.read_buf_len - sock_data.read_buf_used, MSG_DONTWAIT);
	if (ret > 0) {
		sock_data.read_buf_used += ret;
		if (sock_data.read_buf_used == sock_data.read_buf_len) {
			if (NULL == get_read_buffer(sock_data.read_buf_len * 2, 1)) {
				setResult(IEvent::ERROR);
				set_sock_status(LcEvent::STATUS_READ | LcEvent::STATUS_MEMERROR);
				LCEVENT_WARNING(this,"HttpEvent(http_readheader_process) : get readbuf error");
				goto END;
			}
		}
		_fheader_length = http_find_crlfcrlf(ret);
	} else {
		if (errno == EAGAIN) {
			if (http_readagain(proc_cb) != 0) {
				this->setResult(ERROR);
				set_sock_status(LcEvent::STATUS_READ | LcEvent::STATUS_POSTERROR);
				goto END;
			}
			return 0;
		}
		if (ret == 0) {
			this->setResult(CLOSESOCK);
			set_sock_status(LcEvent::STATUS_READ);
			goto END;
		}
		if (4 > _fheader_length) {
			this->setResult(ERROR);
			set_sock_status(LcEvent::STATUS_READ);
			LCEVENT_WARNING(this,"HttpEvent(http_readheader_process) : read http header error[%m]");
			goto END;
		}
	}
	if (0 < _fheader_length) {
		goto END;
	}
	if (http_readagain(proc_cb) != 0) {
		this->setResult(ERROR);
		set_sock_status(LcEvent::STATUS_READ | LcEvent::STATUS_POSTERROR);
		goto END;
	}
	return 0;
END:
	if (isError()) {


		return 1;
	}
	set_sock_status(LcEvent::STATUS_READ);
	http_get_contentlength();
	
	_fbody_readdone = sock_data.read_buf_used - _fheader_length;
	LCEVENT_DEBUG(this,"HttpEvent(http_readheader_process)[_fbody_readdone:%d][bodylen:%d]",
		_fbody_readdone, _fbody_length);
	return 1;
}

void HttpEvent :: http_readheader_callback() {
	int rdres = http_readheader_process(lchttpevent_readhead_callback);
	if (rdres) {
		if (isError()) {
			LCEVENT_WARNING(this,"HttpEvent(http_readpackage_callback) http read error[%m]");
			event_error_callback();
			return;
		}
		
		_fbody_readdone = sock_data.read_buf_used - _fheader_length;

		LCEVENT_DEBUG(this,"HttpEvent(http_readheader_callback)[_fbody_readdone:%d][bodylen:%d]",
				_fbody_readdone, _fbody_length);
		restore_callback();
		timeval tv;
		
		gettimeofday(&tv, NULL);
		sock_data.timer.read += tvTv2Ms(tvSlc(tv, sock_data.timer.count));
		
		sock_data.timer.proccount = tv;
		http_read_header_done();
		gettimeofday(&tv, NULL);
		sock_data.timer.proc += tvTv2Ms(tvSlc(tv, sock_data.timer.proccount));
		sock_data.timer.count = tv;
	} else {
		if (isError()) {
			timeval tv;
			gettimeofday(&tv, NULL);
			sock_data.timer.read += tvTv2Ms(tvSlc(tv, sock_data.timer.count));

			LCEVENT_WARNING(this,"HttpEvent(http_readpackage_callback) http read error[%m]");
			restore_callback();
			event_error_callback();
			return;
		}
	}


























































}

int HttpEvent :: http_readagain(cb_t again_cb) {
	setType(IOREADABLE);
	setCallback(again_cb, NULL);
	return repost();
}

int HttpEvent :: http_package_read() {
	sock_data.read_buf_used = 0;
	_fheader_length = -1;
	timeval tv;
	gettimeofday(&tv, NULL);
	sock_data.timer.count = tv;
	backup_callback();
	judge_read_timeout(tv);
	_fbody_readdone = 0;
	LCEVENT_DEBUG(this,"HttpEvent(http_package_read)package read");
	return http_readagain(lchttpevent_readpackage_callback);
}
int HttpEvent :: http_head_read() {
	sock_data.read_buf_used = 0;
	_fheader_length = -1;
	timeval tv;
	gettimeofday(&tv, NULL);
	sock_data.timer.count = tv;
	backup_callback();
	judge_read_timeout(tv);
	_fbody_readdone = 0;
	return http_readagain(lchttpevent_readhead_callback);
}
int HttpEvent :: http_section_read(int size) {
	if (0 != size) {
		_fsection_size = size;
	}
	if (0 == _fsection_size) {
		setResult(IEvent::ERROR);
		set_sock_status(LcEvent::STATUS_READ);
		LCEVENT_WARNING(this,"HttpEvent(http_section_read) : invalid section size(%d).", size);
		return -1;
	}
	if (NULL == get_read_buffer(_fheader_length + _fbody_readdone + _fsection_size, 1)) {
		setResult(IEvent::ERROR);
		set_sock_status(LcEvent::STATUS_READ | LcEvent::STATUS_MEMERROR);
		LCEVENT_WARNING(this,"HttpEvent(http_section_read) : get section readbuf error");
		event_error_callback();
		return -1;
	}
	timeval tv;
	gettimeofday(&tv, NULL);
	sock_data.timer.count = tv;
	backup_callback();
	this->setCallback(lchttpevent_readsection_callback, NULL);
	if (_fsection_size > (_fbody_length - _fbody_readdone)) {
		_fsection_size = _fbody_length - _fbody_readdone;
	}
	if (0 >= _fsection_size) {
		set_sock_status(LcEvent::STATUS_READ);
		LCEVENT_TRACE(this,"HttpEvent(http_section_read) : pre read done(include body(%d)).", _fbody_length);
        http_readsection_callback();
		
	}
	this->read(this->handle(), sock_data.read_buf + _fheader_length + _fbody_readdone, _fsection_size);
	sock_data.read_buf_used = _fheader_length + _fbody_readdone + _fsection_size;
	judge_read_timeout(tv);
	return repost();
}

void HttpEvent :: http_readsection_callback() {
	_fbody_readdone += _fsection_size;
	restore_callback();
	timeval tv;
	
	gettimeofday(&tv, NULL);
	sock_data.timer.read += tvTv2Ms(tvSlc(tv, sock_data.timer.count));
	
	sock_data.timer.proccount = tv;
	set_sock_status(LcEvent::STATUS_READ);
	if (isError()) {
		LCEVENT_WARNING(this,"HttpEvent(http_readsection_callback) http read section error[%m]");
		event_error_callback();
		return;
	}
	http_read_section_done();
	gettimeofday(&tv, NULL);
	sock_data.timer.proc += tvTv2Ms(tvSlc(tv, sock_data.timer.proccount));
	sock_data.timer.count = tv;
}

void HttpEvent :: http_readpackage_callback() {
	LCEVENT_DEBUG(this,"HttpEvent(http_readpackage_callback)package callback");
	int rdres = http_readheader_process(lchttpevent_readpackage_callback);
	if (rdres) {
		
		restore_callback();
		timeval tv;
		
		gettimeofday(&tv, NULL);
		sock_data.timer.read += tvTv2Ms(tvSlc(tv, sock_data.timer.count));


		if (isError()) {
			LCEVENT_WARNING(this,"HttpEvent(http_readpackage_callback) http read error[%m]");
			event_error_callback();
			return;
		}



        
        
		if (NULL == get_read_buffer(_fbody_length+_fheader_length + 1, 1)) {
			setResult(IEvent::ERROR);
			set_sock_status(LcEvent::STATUS_READ | LcEvent::STATUS_MEMERROR);
			LCEVENT_WARNING(this,"HttpEvent(http_readpackage_callback) : get readbuf error");
			event_error_callback();
			return;
		}
		sock_data.timer.count = tv;
		if (_fbody_readdone >= _fbody_length) {
			lchttpevent_readpackagedone_callback(this, NULL);
			return;
		}

		this->setCallback(lchttpevent_readpackagedone_callback, NULL);
		this->read(this->handle(), sock_data.read_buf+_fheader_length + _fbody_readdone,
				_fbody_length -_fbody_readdone);
		sock_data.read_buf_used = _fbody_length+_fheader_length;
		judge_read_timeout(tv);
		if (0 != smart_repost()) {
			setResult(IEvent::ERROR);
			set_sock_status(LcEvent::STATUS_READ | LcEvent::STATUS_POSTERROR);
			LCEVENT_WARNING(this,"HttpEvent(http_readpackage_callback) : repost error");
			event_error_callback();
			return;
		}
	} else {
		
		if (isError()) {
			timeval tv;
			gettimeofday(&tv, NULL);
			sock_data.timer.read += tvTv2Ms(tvSlc(tv, sock_data.timer.count));
			set_sock_status(LcEvent::STATUS_READ);
			LCEVENT_WARNING(this,"HttpEvent(http_readpackage_callback) http read error[%m]");
			restore_callback();
			event_error_callback();
			return;
		}
	}
}

int HttpEvent :: get_http_headlen() {
	return _fheader_length;
}
int HttpEvent :: get_http_bodylen() {
	return _fbody_length;
}

int HttpEvent :: http_package_finished() {
	return _fbody_readdone >= _fbody_length;
}

int HttpEvent :: event_read() {
	return this->http_package_read();
}

}


