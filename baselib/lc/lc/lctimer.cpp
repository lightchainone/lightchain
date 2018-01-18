

#include "lctimer.h"

namespace lc
{
	namespace {
		struct IEvPtrOp
		{
			inline bool operator () (IEvent * &_1,  IEvent * &_2)
			{
				int ret = tvComp(*(_1->timeout()), *(_2->timeout()));
				if (ret == 0) {
					return _1 > _2;
				}
				if (ret > 0) {
					return true;
				}
				return false;
			}
		};
	}

int TimerHeap::push(IEvent *ev)
{
	if (ev->timeout()) {
		AutoMLock __lock(_lock);
		if (_timers.size() >= (size_t)_maxevs) {
			return -1;
		}
		_timers.push_back(ev);
		std::push_heap(_timers.begin(), _timers.end(), IEvPtrOp());
		return 0;
	}
	return -1;
}

IEvent *TimerHeap::pop() {
	AutoMLock __lock(_lock);
	if (_timers.size() == 0) {
		return NULL;
	}
	IEvent *ev = _timers.front();
	std::pop_heap(_timers.begin(), _timers.end(), IEvPtrOp());
	_timers.pop_back();
	return ev;
}

IEvent *TimerHeap::top()
{
	AutoMLock __lock(_lock);
	if (_timers.size() == 0) {
		return NULL;
	}
	return _timers.front();
}

int Timer::load(const comcfg::ConfigUnit &cfg) {
	cfg["ThreadNum"].get_int32(&_pnum, 0);
	int maxevs;
	cfg["MaxEvents"].get_int32(&maxevs, 1<<20);
	_heap.setMaxEvents(maxevs);
	return 0;
}

int Timer::run_tv(const timeval *tv) {
	if (tv) {
		_toev.setTimeout_tv(tv);
		_toev.setCallback(stop_callback, this);
		this->post(&_toev);
	}
	if (_pnum < 0) {
		return -1;
	}

	_run = true;
	if (_pnum == 0) {
		this->callback();
	}
	_status = IReactor::RUNNING;
	return _threads.run(_pnum, default_callback, this);
}

int Timer::runUntilEmpty() {
	_empty = true;
	return run_tv(NULL);
}

int Timer::stop_tv(const timeval *tv) {
	if (tv == NULL) {
		_run = false;
	} else {
		_toev.setTimeout_tv(tv);
		_toev.setCallback(stop_callback, this);
		this->post(&_toev);
	}
	return 0;
}
int Timer::stopUntilEmpty() {
	_empty = true;
	return 0;
}

int Timer::join()
{
	int ret = _threads.join();
	_status = IReactor::STOP;
	return ret;
}

void Timer::callback() {
	LC_IN_DEBUG("callback -- timer %d", (int)_run);
	while (_run) {
		IEvent *ev = _heap.pop();
		if (ev) {
			SmartEvent<IEvent> se(ev);
			timeval tv;
			gettimeofday(&tv, NULL);
			if (tvComp(tv, *(ev->timeout())) < 0) {
				timespec ts, to;
				tv = tvSlc(*(ev->timeout()), tv);
				ts.tv_sec = tv.tv_sec;
				ts.tv_nsec = tv.tv_usec * 1000;
				LC_IN_DEBUG("sleep %d:%d", (int)ts.tv_sec, (int)ts.tv_nsec);
				nanosleep(&ts, &to);
			} else {
				LC_IN_DEBUG("%d:%d > ", tv.tv_sec, tv.tv_usec);
				LC_IN_DEBUG("%d:%d", ev->timeout()->tv_sec, ev->timeout()->tv_usec);
			}
			ev->setResult(IEvent::TIMEOUT);
			ev->callback();
		} else if (_empty) {
			break;
		}
	}
}

int Timer::post(IEvent *ev) {
	if (NULL == ev) {
		LC_IN_WARNING("invalid param ev == NULL");
		return -1;
	}
	ev->setReactor(this);
	ev->setResult(0);
	ev->setStatus(IEvent::READY);
	ev->addRef();
	return _heap.push(ev); 
}

int Timer::post(IEvent *ev, int type)
{
	if (type != IReactor::CPU) {
		LC_IN_WARNING("invalid type %d for timer", type);
		return -1;
	}
	return this->post(ev);
}


Timer::~Timer() { 
	_toev.delRef(); 
	for (size_t i=0; i<_heap.size(); ++i) {
		if (_heap._timers[i]) {
			
			
			_heap._timers[i]->release();
			_heap._timers[i] = 0;
		}
	}
}

IReactor * Timer::getExternReactor() {
	return NULL;
}

};



