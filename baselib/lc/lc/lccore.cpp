
#include "lc2.h"

namespace lc
{
	
	BasicLogAdapter g_lclog;
	IOC<IReactor> IReactorIoc;
	IOC<IEQueue> IEQueueIOC;

namespace 
{


LC_GEN_IOC_CREATE(LockEQueue)


LC_GEN_IOC_CREATE(LCTask)
LC_GEN_IOC_CREATE(NetReactor);


static int Register_IOC()
{
	
	LC_ADD_IOC_CLASS(IEQueueIOC, LockEQueue);

	
	LC_ADD_IOC_CLASS(IReactorIoc, LCTask);
	LC_ADD_IOC_CLASS(IReactorIoc, NetReactor);
	
	return 0;
}

static const int g_register_ioc = Register_IOC();

}

size_t IEQueue::pushOne_tv(IEvent *ev, const timeval *tv)
{
	return this->push_tv(&ev, 1, tv);
}

size_t IEQueue::push_ms(IEvent **ev, size_t items, int msec)
{
	timeval tv;
	tvABS(&tv, msec);
	return this->push_tv(ev, items, &tv);
}

size_t IEQueue::pushOne_ms(IEvent *ev, int msec)
{
	return this->push_ms(&ev, 1, msec);
}

size_t IEQueue::push(IEvent **ev, size_t items)
{
	return this->push_tv(ev, items, NULL);
}

size_t IEQueue::pushOne(IEvent *ev)
{
	return this->push_tv(&ev, 1, NULL);
}

size_t IEQueue::pop_ms(IEvent **ev, size_t items, int msec)
{
	timeval tv;
	tvABS(&tv, msec);
	return this->pop_tv(ev, items, &tv);
}

size_t IEQueue::pop(IEvent **ev, size_t items)
{
	return this->pop_tv(ev, items, NULL);
}

void IEvent::setTimeout(int msec) 
{
	timeval tv;
	tvABS(&tv, msec);
	return this->setTimeout_tv(&tv);
}

int IReactor::run() 
{ 
	return this->run_tv(NULL); 
}

int IReactor::run_ms(int msec) 
{
	timeval tv;
	tvABS(&tv, msec);
	return this->run_tv(&tv);
}

int IReactor::stop() 
{
	return this->stop_tv(NULL);
}

int IReactor::stop_ms(int msec) 
{
	timeval tv;
	tvABS(&tv, msec);
	return this->stop_tv(&tv);
}

bool IEvent::isError()
{
	if (this->result() & ERROR) {
		LC_IN_WARNING("unknow error in event happend[%m]");
	} else if (this->result() & CLOSESOCK) {
	//	LC_IN_WARNING("peer closed %m to handle[%d][%m]", this->handle());
	} else if (this->result() & TIMEOUT) {
	//	LC_IN_DEBUG("timeout happend for handle[%d]", this->handle());
	} else if (this->result() & CANCELED) {
	//	LC_IN_DEBUG("canceld the events %d", this->handle());
		return true;
	
	} else {
		return false;
	}
	return true;
}

int IReactor::smartPost(IEvent *ev) {
	return this->post(ev);
}

int IReactor::smartPost(IEvent *ev, int type) {
	return this->post(ev, type);
}

};


