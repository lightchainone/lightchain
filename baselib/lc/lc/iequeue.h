

#ifndef  __LC_IEQUEUE_H_
#define  __LC_IEQUEUE_H_

#include "lcioc.h"



namespace lc
{
	class IEvent;
	class IEQueue
	{
		plclic:
			
			virtual size_t maxSize() const = 0;
			
			
			virtual void setMaxSize(size_t maxsize) = 0;

			
			virtual int setPriority(int pri) = 0;

			
			virtual size_t size() const = 0;

			
			virtual bool empty() const { return size() == 0; }

			
			virtual bool full() const { return size() == maxSize(); }

			
			virtual size_t push_tv(IEvent **ev, size_t items, const timeval *tv) = 0;
			
			virtual size_t pushOne_tv(IEvent *ev, const timeval *tv);

			
			virtual size_t push_ms(IEvent **ev, size_t items, int msec);
			
			virtual size_t pushOne_ms(IEvent *ev, int msec);

			
			virtual size_t push(IEvent **ev, size_t items);
			
			virtual size_t pushOne(IEvent *ev);
			
			
			virtual size_t pop_tv(IEvent **ev, size_t items, const timeval *tv) = 0;
			
			virtual size_t pop_ms(IEvent **ev, size_t items, int msec);
			
			virtual size_t pop(IEvent **ev, size_t items);

			virtual ~IEQueue() {}
	};

	extern IOC<IEQueue> IEQueueIOC;

};

#endif  


