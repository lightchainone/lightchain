
#include "lockequeue.h"
namespace lc
{


LockEQueue::LockEQueue()
	: _pri(0)
	, _maxsize(size_t(-1))
	, _nowsize(0)
	, _cond(_lock)
{
	setPriority(1);
}

LockEQueue::~LockEQueue()
{
	for (size_t i=0; i<_qvec.size(); ++i) {
		if (_qvec[i] != NULL) {
			_qvec[i]->destroy();
			delete _qvec[i];
		}
	}
	_qvec.clear();
}

int LockEQueue::setPriority(int pri)
{
	if (pri <= 0 || pri >= 16) {
		LC_IN_WARNING("invalid pri[%d]", pri);
		return -1;
	}
	if (pri > (int)_qvec.size()) {
		_qvec.resize(pri);
	} else {
		for (size_t i=pri; i<_qvec.size(); ++i) {
			_qvec[i]->destroy();
			delete _qvec[i];
		}
		_qvec.resize(pri);
	}
	return 0;
}

size_t LockEQueue::push_tv(IEvent **ev, size_t items, const timeval *tv)
{
	AutoMLock __lock(_lock);
	if (full()) {
		if (tv && tv->tv_usec == 0 && tv->tv_sec == 0) {
			return 0;
		}
		if (_cond.wait(tv) != 0) {
			return 0;
		}
	}
	size_t ret = 0;
	for (size_t i=0; i<items; ++i) {
		int pri = ev[i]->priority();
		if (_maxsize <= _nowsize + ret) {
			break;
		}
		if ( pri >= (int)_qvec.size() ) {
			pri = (int)_qvec.size() - 1;
			if (pri < 0) {
				pri = 0;
			}
		}
		if (_qvec[pri] == NULL) {
			_qvec[pri] = new Queue;
			if (_qvec[pri]->create() != 0) {
				delete _qvec[pri];
				_qvec[pri] = 0;
				break;
			}
		}
		if (pri < _pri)  {
			_pri = pri;
		}
		if (_qvec[pri]->push_back(ev[i]) == 0) {
			
			++ ret;
		} else {
			break;
		}
	}
	_nowsize += ret;
	if (_cond.waits() == 1) {
		_cond.signal();
	} else if (_cond.waits() > 1) {
		_cond.signalAll();
	}

	return ret;
}

size_t LockEQueue::pop_tv(IEvent **ev, size_t items, const timeval *tv)
{
	AutoMLock __lock(_lock);
	if (empty()) {
		if (tv && tv->tv_usec == 0 && tv->tv_sec == 0) {
			return 0;
		}
		if (_cond.wait(tv) != 0) {
			return 0;
		}
	}
	size_t ret = 0;
	for (size_t i=_pri; i<_qvec.size(); ++i) {
		if (NULL == _qvec[i]) {
			++ _pri;
			continue;
		}
		while (_qvec[i]->size() > 0 && ret < items) {
			ev[ret] = _qvec[i]->front();
			
			_qvec[i]->pop_front();
			++ ret;
		}
		if (ret == items) {
			break;
		} else {
			++ _pri;
		}
	}
	_nowsize = _nowsize -  ret;

	if (_cond.waits() == 1) {
		_cond.signal();
	} else if (_cond.waits() > 1) {
		_cond.signalAll();
	}

	return ret;
}

}


