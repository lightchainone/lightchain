
#include "eventtools.h"
#include "lcinlog.h"

namespace lc
{


	void EventMonitorPool::callback(IEvent *ev, void *p)
	{
		EventMonitorPool *pool = static_cast<EventMonitorPool *>(p);
		int ret = pool->add_tv(ev, NULL);
		if (ret != 0) {
			LC_IN_WARNING("add %lx into eventMonitorPool error %d", (long)ev, ret);
		}
	}

	EventMonitorPool::EventMonitorPool() :
		_queue(0), _qname("LockEQueue")
	{
	}

	EventMonitorPool::~EventMonitorPool()
	{
		if (_queue) {
			static const int max_fetch = 128;
			IEvent *iev[max_fetch];
			int ret = 1;
			while (_queue && _queue->size() > 0 && ret > 0) {
				ret = _queue->pop_tv(iev, max_fetch, NULL);
				for (int i=0; i<ret; ++i) {
					iev[i]->release();
				}
			}
			IEQueueIOC.destroy(_queue);
		}
		_queue = 0;
	}

	int EventMonitorPool::createQ()
	{
		AutoMLock __lock(_lock);
		if (_queue == NULL) {
			_queue = IEQueueIOC.create(_qname);
			if (_queue == NULL) {
				LC_IN_WARNING("create queue[%s] error", _qname.c_str());
				return -1;
			}
		}
		return 0;
	}

	int EventMonitorPool::add_tv(IEvent *ev, timeval *tv)
	{
		if (ev == NULL) {
			LC_IN_WARNING("invalid param ev == NULL");
			return -1;
		}
		if (_queue == NULL) {
			if (createQ() != 0) {
				LC_IN_WARNING("can't create queue[%s] in feture pool", _qname.c_str());
				return -1;
			}
		}
		ev->addRef();
		int ret = _queue->push_tv(&ev, 1, tv);
		if (ret != 1) {
			LC_IN_WARNING("can't push ev[%lx] into queue[%s]", long(ev), _qname.c_str());
			ev->delRef();
			return -1;
		}

		return 0;
	}

	int EventMonitorPool::size() {
		if (_queue) {
			return _queue->size();
		}
		return 0;
	}

	int EventMonitorPool::poll_tv(IEvent**ev, int items, timeval *tv) 
	{
		if (ev == NULL) {
			LC_IN_WARNING("invalid param ev == NULL");
			return -1;
		}
		if (_queue == NULL) {
			if (createQ() != 0) {
				return -1;
			}
		}
		int ret = _queue->pop_tv(ev, items, tv);
		for (int i=0; i<ret; ++i) {
			ev[i]->release();
		}
		return ret;
	}

	int EventMonitorPool::add(IEvent *ev, int msec)
	{
		if (ev == NULL) {
			LC_IN_WARNING("invalid param ev == NULL");
			return -1;
		}
		timeval tv;
		tvABS(&tv, msec);
		return add_tv(ev, &tv);

	}

	int EventMonitorPool::poll(IEvent **ev, int items, int msec)
	{
		if (ev == NULL) {
			LC_IN_WARNING("invalid param ev == NULL");
			return -1;
		}
		timeval tv;
		tvABS(&tv, msec);
		return poll_tv(ev, items, &tv);
	}
#if 1

	void EQueue::push(IEvent *ev)
	{
		if (ev) {
			if (_end) {
				ev->setNext(NULL);
				_end->setNext(ev);
				ev->setPrevious(_end);
				_end = ev;
			} else {
				ev->setPrevious(NULL);
				ev->setNext(NULL);
				_begin = ev;
				_end = ev;
			}
		}
	}

	IEvent *EQueue::pop() {
		IEvent *p = _begin;
		if (_begin) {
			_begin = _begin->next();
			if (_begin == NULL) {
				_end = NULL;
			} else {
				_begin->setPrevious(NULL);
			}
		}
		if (p) {
			p->setNext(NULL);
			p->setPrevious(NULL);
		}
		return p;
	}

	void EQueue::erase(IEvent *ev) {
		if (ev == NULL) { return; }
		if (ev == _begin) {
			_begin = _begin->next();
			if (_begin == NULL) {
				_end = NULL;
			} else {
				_begin->setPrevious(NULL);
			}
		} else if (ev == _end) {
			_end = _end->previous();
			if (_end == NULL) {
				_begin = NULL;
			} else {
				_end->setNext(NULL);
			}
		} else {
			if (ev->previous()) {
				ev->previous()->setNext(ev->next());
			}
			if (ev->next()) {
				ev->next()->setPrevious(ev->previous());
			}
		}
		ev->setNext(NULL);
		ev->setPrevious(NULL);
	}
#endif
};


