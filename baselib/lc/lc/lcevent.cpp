

#include "lcevent.h"
#include "ul_net.h"

namespace lc
{
int EventBase::isDevided() {
	return _devided;
}
void EventBase::setDevided(int div) {
	_devided = div;
}
int EventBase::getCallback(cb_t *pcb, void ** pprm) {
	if (NULL == pcb || NULL == pprm) {
		return -1;
	}
	*pcb = _cb;
	*pprm = _cbp;
	return 0;
}
void EventBase::set(int hd, int evs, cb_t cb, void *p)
{
	this->setHandle(hd);
	this->setType(evs);
	this->setCallback(cb, p);
}
void EventBase::setCallback(cb_t cb, void *p) 
{
	_cb = cb;
	_cbp = p;
}
timeval * EventBase::timeout() 
{ 
	if (_btv) { return &_tv; }
	return NULL;
}
void EventBase::setTimeout_tv(const timeval *tv) 
{ 
	if (tv == NULL) {
		_btv = false;
	} else {
		_btv = true;
		_tv = *tv;
	}
}
bool EventBase::release() 
{
	if (_cnt.decrementAndGet() <= 0) {
		Lsc::mempool *p = _pool;
		if (p) {
			this->~EventBase();
			p->free(this, sizeof(*this));
		} else {
			delete this;
		}
		return true;
	} else {
		return false;
	}
}
EventBase::EventBase(Lsc::mempool *p) 
	: _pri(0), _handle(-1), _type(0)
	, _events(0), _status(IEvent::INIT), _cnt(1)
	, _btv(false), _reactor(0)
	, _cb(IEvent::default_callback), _cbp(0), _dev(0)
	, _pool(p)
	, _pre(0), _next(0), _evbuf(0), _evbufsiz(0)
	, _devided(0)
{
}

EventBase::~EventBase() {
	if (_evbuf) {
		free (_evbuf);
		_evbuf = 0;
	}
	if (_cnt.get() > 0) {
		LC_IN_FATAL("address(%lx) free, but some thread or postion hold, so free is unsafe", (long)this);
	}
	_handle = -1;
	_pre = _next = 0;
}

SockEventBase::SockEventBase()
	: _first_rwto(0)
	, _types(BASE)
	, _ret(0)
	, _needrcy(false)
	, _buf(0)
	, _cnt(0)
	, _readcnt(0)
{
}

void SockEventBase::callback()
{
	if (_first_rwto) {
		this->setTimeout(_first_rwto);
		_first_rwto = 0;
	}
	switch (_types) {
		case BASE:
			LC_IN_DEBUG("SockEventBase BASE__call");
			return EventBase::callback();
		case ACCEPT:
			
			return accept_callback();
		case READ:
			
			return rw_callback();
		case WRITE:
			
			return rw_callback();
		case TCPCONNECT:
			
			return tcpconnect_callback();
		default:
			
			return EventBase::callback();
	}
}

int SockEventBase::accept(int fd) {
	if (fd < 0) {
		LC_IN_WARNING("invalid accept fd[%d]", fd);
		return -1;
	}
	this->setHandle(fd);
	this->setType(IEvent::IOREADABLE);
	_types = ACCEPT;
	_ret = 0;
	return 0;
}

void SockEventBase::accept_callback()
{
	if (this->isError()) {
		_ret = -1;
		this->clear();
		return EventBase::callback();
	}
	_len = sizeof(_addr);
	_ret = ul_accept(this->handle(), (struct sockaddr *)&_addr, &_len);

	EventBase::callback();
}

int SockEventBase::read(int fd, void *buf, size_t count)
{
	this->setHandle(fd);
	this->setType(IOREADABLE);
	_buf = buf;
	_cnt = count;
	_ret = 0;
	_readcnt = 0;
#if 0
	_ioflags = fcntl(fd, F_GETFL, 0);
	if (!(_ioflags & O_NONBLOCK)) {
		fcntl(fd, F_SETFL, _ioflags | O_NONBLOCK);
	}
#endif
	_types = READ;
	return 0;
}

void SockEventBase::rw_callback()
{
	int ret = 0;
	int closesockflag = 0;
	if (this->isError()) {
		goto END;
	}
	if (_types == WRITE) {
		
		ret = ::send(this->handle(), ((char *)(_buf)) + _readcnt, _cnt - _readcnt, MSG_DONTWAIT);
		if (0 == ret) {
			LC_IN_TRACE("send 0, hd:%d, errno:%d", handle(), errno);
		}
	} else {
		
		ret = ::recv(this->handle(), ((char *)(_buf)) + _readcnt, _cnt - _readcnt, MSG_DONTWAIT);
		if (0 == ret) {
			LC_IN_TRACE("recv 0, hd:%d, errno:%d", handle(), errno);
			closesockflag = 1;
		}
	}
	if (ret > 0) {
		_readcnt += ret;
    } else {
		if (closesockflag) {
			this->setResult(CLOSESOCK);
			goto END;
		}

        if (ret == 0) {
		    if (_readcnt < (ssize_t)_cnt) {
			    if (this->reactor()->post(this) != 0) {
				    this->setResult(ERROR);
				    goto END;
                }
                return;
			} 
			goto END;
        }

		if (errno == EAGAIN) {
			if (this->reactor()->post(this) != 0) {
				this->setResult(ERROR);
				goto END;
			}
			return;
		}
		if (_readcnt < (ssize_t)_cnt) {
			this->setResult(ERROR);
			goto END;
		}
	}
	if (_readcnt == (ssize_t)_cnt) {
		goto END;
	}
	if (this->reactor()->post(this) != 0) {
		this->setResult(ERROR);
		goto END;
	}
	return;

END:
#if 0
	if (!(_ioflags & O_NONBLOCK)) {
		fcntl(this->handle(), F_SETFL, _ioflags);
	}
#endif
	return EventBase::callback();
}

int SockEventBase::write(int fd, const void *buf, size_t count)
{
	this->setHandle(fd);
	this->setType(IOWRITEABLE);
	_buf = (void *)(buf);
	_cnt = count;
	_ret = 0;
	_readcnt = 0;
#if 0
	_ioflags = fcntl(fd, F_GETFL, 0);
	if (!(_ioflags & O_NONBLOCK)) {
		fcntl(fd, F_SETFL, _ioflags | O_NONBLOCK);
	}
#endif
	_types = WRITE;

	return 0;
}

int SockEventBase::tcpConnectBind(const char *host, int port, const char *client_host, int client_port)
{
	bzero(&_addr, sizeof(_addr));
	_addr.sin_addr.s_addr = inet_addr(host);
	if ((_addr.sin_addr.s_addr) == INADDR_NONE) {
		LC_IN_WARNING("invalid addr %s:%d", host, port);
		return -1;
	}
	_addr.sin_family = AF_INET;
	_addr.sin_port = htons(port);

	int fd = ul_socket(AF_INET, SOCK_STREAM, 0);

	if (fd < 0) {
		LC_IN_WARNING("create socket AF_INET, SOCK_STREAM, 0 error %m");
		return -1;
	}

	_ioflags = fcntl(fd, F_GETFL, 0);
	fcntl(fd, F_SETFL, _ioflags | O_NONBLOCK);

    
    struct sockaddr_in client_addr;
    bzero(&client_addr, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    if(inet_aton(client_host , &client_addr.sin_addr) == 0 ) {
        return -1;
    }
    client_addr.sin_port = htons(client_port);
    if (bind(fd, (const struct sockaddr*)&client_addr, sizeof(client_addr)) != 0 ) {
        return -1;
    }
	
	int n = 0;
	_len = sizeof(socklen_t);
	if ( (n = connect(fd, (struct sockaddr *)&_addr, sizeof(_addr))) < 0) {
		if (errno != EINPROGRESS) {
			LC_IN_WARNING("connect error error(%m)");
			::close(fd);
			return -1;
		}
	}
	this->setHandle(fd);
	this->setType(IOWRITEABLE|IOREADABLE);

	_needrcy = true;
	_types = TCPCONNECT;

	return 0;
}

int SockEventBase::tcpConnect(const char *host, int port)
{
	bzero(&_addr, sizeof(_addr));
	_addr.sin_addr.s_addr = inet_addr(host);
	if ((_addr.sin_addr.s_addr) == INADDR_NONE) {
		LC_IN_WARNING("invalid addr %s:%d", host, port);
		return -1;
	}
	_addr.sin_family = AF_INET;
	_addr.sin_port = htons(port);

	int fd = ul_socket(AF_INET, SOCK_STREAM, 0);

	if (fd < 0) {
		LC_IN_WARNING("create socket AF_INET, SOCK_STREAM, 0 error %m");
		return -1;
	}

	_ioflags = fcntl(fd, F_GETFL, 0);
	fcntl(fd, F_SETFL, _ioflags | O_NONBLOCK);
	
	int n = 0;
	_len = sizeof(socklen_t);
	if ( (n = connect(fd, (struct sockaddr *)&_addr, sizeof(_addr))) < 0) {
		if (errno != EINPROGRESS) {
			LC_IN_WARNING("connect error error(%m)");
			::close(fd);
			return -1;
		}
	}
	this->setHandle(fd);
	this->setType(IOWRITEABLE|IOREADABLE);

	_needrcy = true;
	_types = TCPCONNECT;

	return 0;
}

void SockEventBase::tcpconnect_callback()
{
	if (this->isError()) {
		::close(this->handle());
		this->setHandle(-1);
		goto END;
	}
	fcntl(this->handle(), F_SETFL, _ioflags);
END:
	_needrcy = false;
	return EventBase::callback();
}

int SockEventBase::clear()
{
	if (_needrcy) {
		if (_types == TCPCONNECT) {
			if (this->handle() > 0) {
				::close(this->handle());
				this->setHandle(-1);
			}
		}
	}
	_needrcy = false;
	return 0;
}

SockEventBase::~SockEventBase()
{
	this->clear();
}

void SockEventBase::setType(int t)
{
	EventBase::setType(t);
	_types = BASE;
}

int EventBase::setEventBuffer(int size)
{
	if (size <= 0) {
		return -1;
	}
	_evbufsiz = 0;
	if (_evbuf) {
		free (_evbuf);
		_evbuf = 0;
	}
	_evbuf = malloc(size);
	if (_evbuf == NULL) {
		return -1;
	}
	_evbufsiz = size;
	return 0;
}

void * EventBase::getEventBuffer()
{
	return _evbuf;
}

int EventBase::getEventBufferSize()
{
	return _evbufsiz;
}

}


