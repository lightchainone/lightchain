


#ifndef  __LCTIMER_H_
#define  __LCTIMER_H_

#include "ireactor.h"
#include "ievent.h"
#include "lctask.h"
#include "lcevent.h"
#include "lcutils.h"

namespace lc
{
	
	class TimerHeap
	{
		plclic:
			TimerHeap() : _maxevs(1<<20) {}
			
			int push(IEvent *ev);
			
			IEvent *pop();
			
			inline size_t size() { return _timers.size(); }
			
			inline void setMaxEvents(int m) { _maxevs = m; }
			
			IEvent *top();
		private:
			int _maxevs;
			std::vector<IEvent *> _timers;
			MLock _lock;

			friend class Timer;
	};

	class Timer : plclic IReactor
	{
		plclic:
			Timer() : 
				_run(false), _empty(false), _pnum(0),
				_status(IReactor::INIT) { }
			~Timer();
			
			int load(const comcfg::ConfigUnit &cfg);
			
			void setPriority(int) { }
			
			void setThread(int core) { _pnum = core; }
			
			void setMaxEvents(int) { } 
			
			int run_tv(const timeval *tv);

			
			int runUntilEmpty();
			
			int stop_tv(const timeval *tv);

			
			int stopUntilEmpty();

			
			
			
			int join();
			
			int status() { return _status; }
			
			int post(IEvent *ev);
			
			int post(IEvent *, int type);
			
			int cancel(IEvent *) { return -1; }

			
			virtual void callback();
			
			virtual IReactor * getExternReactor();
		private:
			TimerHeap _heap;
			EventBase _toev;
			bool _run;
			bool _empty;
			ThreadMg _threads;
			int _pnum;
			int _status;

			static void stop_callback(IEvent *, void *p) {
				Timer *timer = static_cast<Timer *>(p);
				timer->_run = false;
			}
			static void * default_callback(void *p) {
				Timer *timer = static_cast<Timer *>(p);
				LC_IN_OPENLOG_R;
				timer->callback();
				LC_IN_CLOSELOG_R;
				return NULL;
			}
	};
};

#endif  


