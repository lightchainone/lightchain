


#ifndef  __LC_EVENTTOOLS_H_
#define  __LC_EVENTTOOLS_H_
#include "lcinlog.h"
#include "lcutils.h"
#include "lclock.h"
#include "ievent.h"
#include "iequeue.h"

namespace lc
{

	
	class EventMonitorPool
	{
		plclic:
			EventMonitorPool();
			~EventMonitorPool();

			
			void setQueue(const String &n) { _qname = n; }
			
			int add_tv(IEvent *ev, timeval *tv);
			
			int add(IEvent *ev, int msec);
			
			int poll_tv(IEvent **ev, int items, timeval *tv);
			
			int poll(IEvent **ev, int items, int msec);
			
			int size();

			
			static void callback(IEvent *ev, void *pool);
		private:
			IEQueue *_queue;
			String _qname;
			MLock _lock;
		private:
			int createQ();
	};


	
	class EQueue
	{
		plclic:
			EQueue() : _begin(0), _end(0) {}
			~EQueue() {}
			
			inline void clear() { _begin = _end = 0; }
			
			inline bool empty() { return _begin == NULL; }
			
			
			void push(IEvent *ev);
			
			IEvent *pop();
			
			inline IEvent *begin() { return _begin; }
			
			void erase(IEvent *ev);
		plclic:
			IEvent *_begin;
			IEvent *_end;
	};

	
	class ELQueue
	{
		plclic:
			
			inline void clear() {
				AutoSLock __l(_lock);
				_queue.clear();
			}
			
			inline bool empty() {
				return _queue.empty();
			}
			
			
			inline void push(IEvent *ev) {
				AutoSLock __l(_lock);
				_queue.push(ev);
			}

			
			inline IEvent * pop() {
				AutoSLock __l(_lock);
				return _queue.pop();
			}
			
			inline IEvent *begin() {
				AutoSLock __l(_lock);
				return _queue.begin();
			}
			
			inline void erase(IEvent *ev) {
				AutoSLock __l(_lock);
				_queue.erase(ev);
			}

		private:
			SpinLock _lock;
			EQueue _queue;
	};
};

#endif  


