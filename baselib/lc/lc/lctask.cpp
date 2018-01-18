
#include "lcutils.h"
#include "lctask.h"

namespace lc
{

LCTask::LCTask() : 
	_queue(0)
	, _qstr("LockEQueue")
	, _threadsnum(4)
	, _btv(false) 
	, _emptystop(false)
	, _run(false)
	, _status(IReactor::INIT)
	, _pri(1)
	, _maxevs(1<<30)
	{}
LCTask::~LCTask()
{
	_threads.join();
	if (_queue) {
		IEvent *ev[1];
		while (!_queue->empty()) {
			_queue->pop_tv(ev, 1, NULL);
			
			
			ev[0]->release();
		}
		IEQueueIOC.destroy(_queue);
		_queue = 0;
	}
}

IReactor * LCTask::getExternReactor() {
	return NULL;
}

void LCTask::setPriority(int pri)
{
	_pri = pri;
}

void LCTask::setMaxEvents(int num)
{
	_maxevs = num;
}
 
void LCTask::setThread(int core)
{
	_threadsnum = core;
};

int LCTask::run_tv(const timeval *tv)
{
	if (tv == NULL) {
		_btv = false;
	} else {
		_btv = true;
		_tv = *tv;
	}
	if (_threadsnum < 0) {
		LC_IN_WARNING("thread num error %d", _threadsnum);
		return -1;
	}
	if (_queue == NULL && createq() != 0) {
		LC_IN_WARNING("create queue erro in run function");
		return -1;
	}

	LC_IN_DEBUG("start task reactor thread num[%d]", _threadsnum);


	int ret = 0;
	_run = true;
	_status = IReactor::RUNNING;
	if (_threadsnum > 0) {
		ret = _threads.run(_threadsnum, CALLBACK, this);
	} else {
		this->callback();
	}
	return ret;
}

int LCTask::runUntilEmpty()
{
	_emptystop = true;
	int ret = this->run();
	if (ret <= 0) {
		_emptystop = false;
	}
	return ret;
}

int LCTask::stop_tv(const timeval *tv)
{
	if (tv == NULL) {
		_run = false;
	} else {
		_btv = true;
		_tv = *tv;
	}
	return 0;
}

int LCTask::stopUntilEmpty()
{
	_emptystop = true;
	return 0;
}

int LCTask::join() {
	_threads.join();
	_status = IReactor::STOP;
	return 0;
}

void * LCTask::CALLBACK(void *p)
{
	LCTask *task = static_cast<LCTask *>(p);
	LC_IN_OPENLOG_R;
	task->callback();
	LC_IN_CLOSELOG_R;
	return NULL;
}

void LCTask::callback()
{
	timeval tv;
	gettimeofday(&tv, NULL);

	while (_run) {
		IEvent *ev;
		
		tvABS(&tv, 100);
		if (_btv) {
			if (tvComp(tv, _tv) > 0) {
				tv = _tv;
			}
		}
		int rcnt = _queue->pop_tv(&ev, 1, &tv);
		
		time_t e = time(0);

		if (rcnt > 0) {
			SmartEvent<IEvent> se(ev);
			if (ev->status() == IEvent::CANCELED) {
				
				ev->setResult(ev->result() | IEvent::CANCELED);
			}
			if (ev->timeout() && e > ev->timeout()->tv_sec) {
				ev->setResult(ev->result() | IEvent::TIMEOUT);
			}
			ev->callback();
		}

		if (isStop(time(0))) {
			break;
		}
	}
}

bool LCTask::isStop(time_t e)
{
	if (_emptystop) {
		if(_queue->size() == 0) {
			return true;
		}
	}

	if (_btv) {
		if (e > _tv.tv_sec) {
			return true;
		}
		if (e == _tv.tv_sec) {
			timeval te;
			gettimeofday(&te, NULL);
			if (te.tv_usec >= _tv.tv_usec) {
				return true;
			}
		}
	}

	return false;
}



int LCTask::status() { return _status; }
int LCTask::post(IEvent *ev)
{
	ev->setResult(0);
	ev->setReactor(this);
	ev->setStatus(IEvent::READY);
	timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 0;

	if (_queue == NULL && createq() != 0) {
		LC_IN_WARNING("create queue error in post function");
		return -1;
	}

	ev->addRef();
	int ret = _queue->push_tv(&ev, 1, &tv);
	if (ret == 1) {
		return 0;
	}
	ev->release();
	return -1;
}
int LCTask::cancel(IEvent *ev)
{
	if (ev == NULL) {
		LC_IN_WARNING("invalid param ev == NULL");
		return -1;
	}
	ev->setStatus(IEvent::CANCELED);
	return 0;
}
void LCTask::setQueueType(const char *str)
{
	_qstr = str;
}

int LCTask::load(const comcfg::ConfigUnit &cfg)
{
	cfg["QueueType"].get_Lsc_string(&_qstr, "LockEQueue");
	cfg["ThreadNum"].get_int32(&_threadsnum, 4);
	cfg["Priority"].get_int32(&_pri, 1);
	cfg["MaxEvents"].get_int32(&_maxevs, 1<<30);
	return 0;
}

int LCTask::createq()
{
	AutoMLock __lock(_lock); 
	if (_queue == NULL) {
		_queue = IEQueueIOC.create(_qstr);
		if (_queue == NULL) {
			LC_IN_WARNING("create %s queue error", _qstr.c_str());
			return -1;
		}
		_queue->setPriority(_pri);
		_queue->setMaxSize(_maxevs);
	}
	return 0;
}

int LCTask::post(IEvent *ev, int type)
{
	if (type != IReactor::CPU) {
		LC_IN_WARNING("invalid type %d for lctask", type);
		return -1;
	}
	return this->post(ev);
}
};


