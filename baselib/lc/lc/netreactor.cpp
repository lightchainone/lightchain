
#ifdef __i386
#include "../lc_server/epoll.h"
#else
#include <sys/epoll.h>
#endif
#include "netreactor.h"
#include "ievent.h"

namespace lc
{
EPollEx::EPollEx(IReactor *r) : _fd(-1), _size(0)
	, _waito(10)
	, _evs(0)
	, _selfid(0), _task(r), _cancel(0)
{
	gettimeofday(&_nexto, NULL);
}
EPollEx::~EPollEx()
{
	destroy();
}

int EPollEx::create(size_t size)
{	
	struct epoll_event ev;
	int ret = 0;
	int flags = 0;
	if (size < 1) {
		LC_IN_WARNING("size error %d", (int)size);
		return -1;
	}
	if (::pipe(_pipe) < 0) {
		LC_IN_WARNING("create pipe error %m");
		goto fail;
	}
	flags = fcntl(_pipe[0], F_GETFL, 0);
	fcntl(_pipe[0], F_SETFL, flags|O_NONBLOCK);
	flags = fcntl(_pipe[1], F_GETFL, 0);
	fcntl(_pipe[1], F_SETFL, flags|O_NONBLOCK);

	_evs = static_cast<epoll_event *>
		(::malloc(sizeof(epoll_event) * size));
	if (_evs == NULL) {
		LC_IN_WARNING("malloc memory %d * %d error", (int)sizeof(epoll_event), (int) size);
		goto fail;
	}

	_fd = epoll_create(size);
	if (_fd <= 0) {
		LC_IN_WARNING("create epoll error %m");
		goto fail;
	}
	_size = size;

	ev.data.ptr = (void *)(-1);
	ev.events = EPOLLHUP | EPOLLERR | EPOLLIN;
	ret = epoll_ctl(_fd, EPOLL_CTL_ADD, _pipe[0], &ev);
	if (ret != 0) {
		LC_IN_WARNING("using pipe monitor error %m");
		goto fail;
	}

	return 0;
fail:
	destroy();
	return -1;

}

void EPollEx::destroy()
{
	IEvent *ev = _elq.pop();
	while (NULL != ev) {
		
		
		ev->release();
		ev = _elq.pop();
	}
	ev = _a_q.pop();
	while (NULL != ev) {
		ev->release();
		ev = _a_q.pop();
	}

	if (_fd > 0) {
		::close(_fd);
		_fd = -1;
	}
	if (_evs) {
		::free(_evs);
		_evs = 0;
	}
	if (_pipe[0] >=0) {
		::close(_pipe[0]);
		::close(_pipe[1]);
		_pipe[0] = _pipe[1] = -1;
	}

}

int EPollEx::checker(int to, timeval &tv)
{
#if 1
	if (_cancel == 0) {
	_waito = tvTv2Ms(tvSlc(_nexto, tv));
	LC_IN_DEBUG("%ld:%ld %ld:%ld to[%d]", _nexto.tv_sec, _nexto.tv_usec,
		tv.tv_sec, tv.tv_usec, _waito);

	if (_waito > 0) {
		if (_waito > to) {
			_waito = to;
		}
		LC_IN_DEBUG("not checking  %d", _waito);
		return 0;
	}

	_nexto = tv;
	_nexto.tv_usec += to * 1000;
	if (_nexto.tv_usec > 1000000) {
		_nexto.tv_sec ++;
		_nexto.tv_usec -= 1000000;
	}
	}
#endif
#ifdef LC_EPOLL_USESELF_TIMEOUT
	{
		LC_IN_DEBUG("start to check _elq %d", _elq.empty());
		IEvent *ev = NULL;
		{
			AutoMLock ___lock(_lock1);
			ev = _elq.begin();
		}
		while (ev != NULL) {
			IEvent * next = NULL;
			LC_IN_DEBUG("checking ev[%ld] status[%x]", (long)ev, ev->status());
			bool bcb = false;
			{
				AutoMLock ___lock(_lock1);
				next = ev->next();
			}
			if (ev->status() == IEvent::CANCELED) {
				bcb = true;
				ev->setResult(IEvent::CANCELED);
				if (this->del(ev) != 0) {
			        LC_IN_FATAL("del ev[%p] from epoll error, handle[%d]", ev, ev->handle());
				}
			} else if (ev->timeout()) {
				if (tvComp(tv, *(ev->timeout())) >= 0) {
					bcb = true;
					ev->setResult(IEvent::TIMEOUT);
					if (this->del(ev) != 0) {
					    LC_IN_FATAL("del ev[%p] from epoll error, handle[%d]", ev, ev->handle());
					}
				} 
#if 0
				else if (tvComp(_nexto, *(ev->timeout())) > 0) {
					_nexto = *(ev->timeout());
				}
#endif
			}

			if (bcb) {
				_elq.erase(ev);
#if 0
				if (_task) {
					if (static_cast<Net2Reactor *>(_task)->post2Task(ev) != 0) {
						ev->callback();
					}
				} else {
					ev->callback();
				}
#else
				ev->callback();
#endif
				ev->release();
			}
			ev = next;
		}
	}
#endif

	_cancel = 0;
	_waito = to;
	
	

	
	

	return 0;
}

int EPollEx::add(IEvent *iev, int itype)
{
	iev->setDev(this);

	if (itype == IReactor::CPU) {
		_a_q.push(iev);
		this->signal();
		return 0;
	}
	if (itype != IReactor::NET) {
		LC_IN_WARNING("unsupport %d in netreactor", itype);
		return -1;
	}
	struct epoll_event ev;
	ev.data.ptr = iev;
	ev.events = EPOLLHUP | EPOLLERR;
	int type = iev->type();
	if (type & IEvent::IOREADABLE) {
		ev.events = ev.events | EPOLLIN;
	}
	if (type & IEvent::IOWRITEABLE) {
		ev.events = ev.events | EPOLLOUT;
	}
	int ret = 0;
	{
		AutoMLock ____lock(_lock1);
#ifdef LC_EPOLL_USESELF_TIMEOUT
	    _elq.push(iev);
#endif
	    ret = epoll_ctl(_fd, EPOLL_CTL_ADD, iev->handle(), &ev);
#ifdef LC_EPOLL_USESELF_TIMEOUT
	    if (ret != 0) {
		    LC_IN_WARNING("epoll [%d] add ev[%lx] fd[%d] ret[%d] error (%m)", _fd, (long)&ev, iev->handle(), ret);
		    _elq.erase(iev);
	    }
#endif
	}
	return ret;
}

int EPollEx::del(IEvent *ev)
{
	int ret = epoll_ctl(_fd, EPOLL_CTL_DEL, ev->handle(), NULL);
	if (0 != ret) {
	    ret = epoll_ctl(_fd, EPOLL_CTL_DEL, ev->handle(), NULL);    
	}

	return ret;
}
void EPollEx::signal()
{
	::write(_pipe[1], "a", 1);
}

void EPollEx::signalDeal()
{
	do {
		char buf[64];
		int ret = ::read(_pipe[0], buf, sizeof(buf));
		if (ret < (int)sizeof(buf)) {
			break;
		}
	} while (true);
	IEvent *ev = _a_q.pop();
	while (NULL != ev) {
		ev->callback();
		ev->release();
		ev = _a_q.pop();
	}
}


int EPollEx::poll(int timeout)
{

	if (_waito > timeout) { _waito = timeout; }
	if (_waito <= 0) { _waito = 1; }
#if 0
	timeval now;
	gettimeofday(&now, NULL);
	LC_IN_DEBUG("start wait %d	-	%ld:%ld", _waito,
			now.tv_sec, now.tv_usec);
#endif
	int num = epoll_wait(_fd, _evs, _size, _waito);
	
	{
		for (int i=0; i<num; ++i) {
			
			IEvent *iev = static_cast<IEvent *>(_evs[i].data.ptr);
			if (iev == (IEvent *)(-1)) {
				this->signalDeal();
				continue;
			}

			int events = 0;
			LC_IN_DEBUG("events %d, hd:%d", events, iev->handle());
			if (_evs[i].events & EPOLLHUP) {
				events = events | IEvent::CLOSESOCK;
				int err;
				socklen_t len = sizeof(err);
				int val = getsockopt(iev->handle(), SOL_SOCKET, SO_ERROR, &err, &len);
				LC_IN_WARNING("EPOLLHUP fd[%d] error[%d] ret[%d]", iev->handle(), err, val);
			}
			if (_evs[i].events & EPOLLERR) {
				events = events | IEvent::CLOSESOCK; 
				int err;
				socklen_t len = sizeof(err);
				int val = getsockopt(iev->handle(), SOL_SOCKET, SO_ERROR, &err, &len);
				LC_IN_WARNING("EPOLLERR fd error[%d] ret[%d]", err, val);
			}
			if (_evs[i].events & EPOLLIN) {
				events = events | IEvent::IOREADABLE;
			}
			if (_evs[i].events & EPOLLOUT) {
				events = events | IEvent::IOWRITEABLE;
			}

			if (iev->status() == IEvent::CANCELED) {
				events = events | IEvent::CANCELED;
			}
			iev->setResult(events);

			int ret = epoll_ctl(_fd, EPOLL_CTL_DEL, iev->handle(), NULL);
			if (ret != 0) {
				LC_IN_WARNING("epoll del %d error (%m)", iev->handle());
			}
#ifdef LC_EPOLL_USESELF_TIMEOUT
			_elq.erase(iev);
#endif
			if ((NULL !=_task->getExternReactor()) && (iev->isDevided())) {
				_task->getExternReactor()->post(iev);
			} else {
				iev->callback();
			}
			iev->release();
		}
#ifdef LC_EPOLL_USESELF_TIMEOUT
		
		timeval tv;
		gettimeofday(&tv, NULL);
		this->checker(timeout, tv);
#endif
	}
	return num;
}

NetReactor::NetReactor() : _check_timer(100), _ext_task(0)
{
	_maxevs = 10000;
	_threadsnum = 1;
}
NetReactor::~NetReactor() 
{
	if (NULL != _ext_task) {
		lc::IReactorIoc.destroy(_ext_task);
	}
	for (size_t i=0; i<_vecs.size(); ++i) {
		if (_vecs[i]) {
			_vecs[i]->destroy();
			delete _vecs[i];
			_vecs[i] = 0;
		}
	}
}

IReactor * NetReactor::getExternReactor() {
	if (0 !=_use_ext_task && NULL != _ext_task) {
		return _ext_task;
	}
	return NULL;
}

int NetReactor::load(const comcfg::ConfigUnit &cfg)
{
	cfg["QueueType"].get_Lsc_string(&_qstr, "LockEQueue");
	cfg["ThreadNum"].get_int32(&_threadsnum, 1);
	LC_IN_DEBUG("set ThreadNum %d", _threadsnum);
	cfg["Priority"].get_int32(&_pri, 1);
	cfg["MaxEvents"].get_int32(&_maxevs, 10000);
	cfg["CheckTime"].get_int32(&_check_timer, 100);
	cfg["Usetask"].get_int32(&_use_ext_task, 0);
	if (0 != _use_ext_task) {
		_ext_task = lc::IReactorIoc.create(cfg["task"]["Type"].to_Lsc_string());
		if (NULL == _ext_task) {
			int errcode;
			LC_IN_WARNING("create reactor %s, error _ext_task %lx",
				cfg["task"]["Type"].to_cstr(&errcode, "unknown"), (long)_ext_task);
			return -1;
		}
		if (0!= _ext_task->load(cfg["task"])) {
			_ext_task = NULL;
			return -1;
		}
	}
	return 0;
}

int NetReactor::run_tv(const timeval *tv)
{
	_pos.set(0);
	if (0 !=_use_ext_task && NULL != _ext_task) {
		_ext_task->run_tv(tv);
	}
	return LCTask::run_tv(tv);
}

void NetReactor::callback()
{
	initVecs();
	int pos = _pos.getAndIncrement();
	EPollEx *poll = getPoll(pos);
	if (poll == NULL) {
		LC_IN_WARNING("getPoll err at %d", pos);
		return;
	}
	poll->initPthreadId();

	LC_IN_DEBUG("NetReactor Debug");
	{
		static const int fetch = 100;
		IEvent *iev[fetch];
		while (_queue->size() > 0) {
			timeval tv;
			tv.tv_sec = 0;
			tv.tv_usec = 0;
			int ret = _queue->pop_tv(iev, fetch, &tv);
			for (int i=0; i<ret; ++i) {
				long type = (long)(iev[i]->dev());
				this->post(iev[i], type);
				iev[i]->release();
			}
		}
	}

	while (_run) {
		int ms = _check_timer;
		
		poll->poll(ms);
		
		if (isStop(time(0))) {
			break;
		}
	}
}

int NetReactor::initVecs()
{
	AutoMLock __lock(_lock);
	int ts = _threadsnum;
	if (ts <= 0) { ts = 1; }
	if ((int)_vecs.size() != ts) {
		for (int i=ts; i<(int)_vecs.size(); ++i) {
			if (_vecs[i]) {
				_vecs[i]->destroy();
				delete _vecs[i];
				_vecs[i] = 0;
			}
		}
		_vecs.resize(ts);
	}
	return 0;
}

EPollEx *NetReactor::getPoll(int pos)
{
	if ((int)_vecs.size() <= pos) {
		initVecs();
		if ((int)_vecs.size() <= pos) {
			return NULL;
		}
	}
	if (_vecs[pos] == NULL) {
		AutoMLock __lock(_lock);
		if (_vecs[pos] == NULL) {
			EPollEx * tmp = new EPollEx(this);
			if (tmp->create(_maxevs) != 0) {
				delete tmp;
				LC_IN_WARNING("create EPollEx fail in callback");
				return NULL;
			}
			_vecs[pos] = tmp;
		}
	}
	return _vecs[pos];
}

int NetReactor::smartPost(IEvent *ev)
{
	return this->smartPost(ev, IReactor::NET);
}
int NetReactor::smartPost(IEvent *ev, int type)
{
	if (NULL == ev) {
		LC_IN_WARNING("invalid param ev == NULL");
		return -1;
	}
	if (this->status() != IReactor::RUNNING) {
		return this->post(ev);
	}	
	ev->setResult(0);
	ev->setStatus(IEvent::READY);
	if (_queue && (int)_queue->size() >= _maxevs) {
		LC_IN_WARNING("more events %d", _maxevs);
		return -1;
	}
	ev->setReactor(this);

	if ((0 !=_use_ext_task && NULL != _ext_task) && (ev->isDevided())) {
		return _ext_task->post(ev);
	}

	pthread_t pid = pthread_self();
	for (int i=0; i<_pos.get(); ++i) {
		if (_vecs[i] && _vecs[i]->pid() == pid) {
			ev->setResult(0);
			ev->addRef();
			if (_vecs[i]->add(ev, type) != 0) {
				LC_IN_WARNING("add event error in smartPost");
				ev->release();
				return this->post(ev);
			}
			return 0;
		}
	}
	return this->post(ev);
}

int NetReactor::post(IEvent *ev)
{
	return this->post(ev, IReactor::NET);
}
int NetReactor::post(IEvent *ev, int type)
{
	if (ev == NULL) {
		LC_IN_WARNING("invalid param ev == NULL");
		return -1;
	}
	ev->setResult(0);
	ev->setStatus(IEvent::READY);

	if (_queue && (int)_queue->size() >= _maxevs) {
		LC_IN_WARNING("more events %d", _maxevs);
		return -1;
	}

	ev->setReactor(this);

	if (this->status() != IReactor::RUNNING) {
		LC_IN_DEBUG("push ev[%lx] into queue", long(ev));
		ev->setDev((void *)((long)type));
		return LCTask::post(ev);
	}
	ev->addRef();
	int rd = 0;
	if (_threadsnum > 0) { rd = _rand.rand() % _threadsnum; }

	if ((0 !=_use_ext_task && NULL != _ext_task) && (ev->isDevided())) {
		return _ext_task->post(ev);
	}

	for (int i=rd; i>=0; --i) {
		EPollEx *poll = getPoll(i);
		if (poll && poll->add(ev, type) == 0) {
			LC_IN_DEBUG("push_ev[0x%lx][%d] into %d poll", (long)(ev), ev->handle(), i);
			return 0;
		} else {
			LC_IN_WARNING("push_ev[0x%lx][%d] into %d poll[%lx] error %m", (long)(ev), ev->handle(), i, (long)poll);
		}
	}
	for (int i= rd+1; i<_threadsnum; ++i) {
		EPollEx *poll = getPoll(i);
		if (poll && poll->add(ev, type) == 0) {
			LC_IN_DEBUG("push_ev[0x%lx][%d] into %d poll", (long)(ev), ev->handle(), i);
			return 0;
		} else {
			LC_IN_WARNING("push_ev[0x%lx][%d] into %d poll[%lx] error %m", (long)(ev), ev->handle(), i, (long)poll);
		}
	}
	ev->release();
	LC_IN_DEBUG("push ev[%lx] error", long(ev));
	return -1;
}

int NetReactor::cancel(IEvent *ev)
{
	if (NULL == ev) {
		LC_IN_DEBUG("invalid param ev == NULL");
		return -1;
	}
	if (this->status() != IReactor::RUNNING) {
		return LCTask::cancel(ev);
	}
	
	EPollEx *poll = static_cast<EPollEx *>(ev->dev());
	if (poll == NULL) {
		return -1;
	}
	poll->_cancel = 1;
	ev->setStatus(IEvent::CANCELED);
	return 0;
}

void NetReactor::setCheckTime(int msec)
{
	if (msec >= 0) {
		_check_timer = msec;
	}
}

#if 0
Net2Reactor::Net2Reactor() : _check_timer(100)
{
	_maxevs = 10000;
	_threadsnum = 1;
	_poll = NULL;
	_add_ev_done = false;
}

Net2Reactor::~Net2Reactor()
{
	if (_poll) {
		_poll->destroy();
		delete _poll;
		_poll = 0;
	}
}

int Net2Reactor::load(const comcfg::ConfigUnit &cfg)
{
	cfg["QueueType"].get_Lsc_string(&_qstr, "LockEQueue");
	cfg["ThreadNum"].get_int32(&_threadsnum, 1);
	LC_IN_DEBUG("set ThreadNum %d", _threadsnum);
	cfg["Priority"].get_int32(&_pri, 1);
	cfg["MaxEvents"].get_int32(&_maxevs, 10000);
	return 0;
}

void Net2Reactor::epollCallback(IEvent *ev, void *p)
{
	Net2Reactor *r = static_cast<Net2Reactor *>(p);
	r->_poll->poll(r->_check_timer);

	while (r->post2Task(ev) != 0) {
		r->_poll->poll(r->_check_timer);
	}
}

int Net2Reactor::run_tv(const timeval *tv)
{
	if (!_add_ev_done) {
		lc::EventPtr ev;
		ev->setCallback(Net2Reactor::epollCallback, this);
		LCTask::post(&ev);
		_add_ev_done = true;
	}
	return LCTask::run_tv(tv);
}

EPollEx * Net2Reactor::getPoll() 
{
	if (_poll == NULL) {
		AutoMLock __lock(_lock);
		if (_poll == NULL) {
			_poll = new EPollEx(this);
			if (_poll->create(_maxevs) != 0) {
				delete _poll;
				_poll = 0;
				LC_IN_WARNING("create EPollEx fail in callback");
				return NULL;
			}
		}
	}
	return _poll;
}

int Net2Reactor::post(IEvent *ev)
{
	if ((ev->type() & IEvent::IOWRITEABLE)
			|| (ev->type() & IEvent::IOREADABLE)) {
		ev->addRef();
		EPollEx *p = getPoll();
		if (p == NULL) {
			LC_IN_WARNING("get poll error %m");
			ev->release();
			return -1;
		}
		if (p->add(ev) != 0) {
			LC_IN_WARNING("add to poll error %m");
			ev->release();
			return -1;
		}
		return 0;
	}
	return LCTask::post(ev);
}

int Net2Reactor::cancel(IEvent *ev)
{
	if ((ev->type() & IEvent::IOWRITEABLE) 
			|| (ev->type() & IEvent::IOREADABLE)) {
		if (ev->timeout()) {
			return -1;
		}
		EPollEx *poll = static_cast<EPollEx *>(ev->dev());
		if (poll == NULL) {
			return -1;
		}
		ev->setStatus(IEvent::CANCELED);
		return poll->del(ev);
	}
	return LCTask::cancel(ev);
}
#endif
};


