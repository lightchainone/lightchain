


#ifndef  __LCTASK_H_
#define  __LCTASK_H_

#include "ireactor.h"
#include "ievent.h"
#include "iequeue.h"
#include "threadmg.h"

namespace lc
{

class LCTask : plclic IReactor
{
	plclic:
		LCTask();
		~LCTask();
		
		virtual int load(const comcfg::ConfigUnit &);
		
		virtual void setPriority(int pri);
		
		virtual void setThread(int core);
        virtual int getThread() {
            return _threadsnum;
        }
		
		virtual void setMaxEvents(int num);
		
		virtual void setQueueType(const char *str);
	plclic:
		
		virtual int run_tv(const timeval *);
		
		virtual int runUntilEmpty();
		
		virtual int stop_tv(const timeval *);
		
		virtual int stopUntilEmpty();

		
		
		

		
		virtual int join();

		
		virtual int status();

		
		virtual int post(IEvent *);

		
		virtual int post(IEvent *, int type);

		
		virtual int cancel(IEvent *);
		
		
		virtual IReactor * getExternReactor();

	protected:
		IEQueue *_queue;
		String _qstr;
		MLock _lock;

		ThreadMg _threads;
		int _threadsnum;

		timeval _tv;
		bool _btv;

		bool _emptystop;
		bool _run;

		int _status;
		int _pri;
		int _maxevs;

	protected:
		virtual void callback();
		int createq();
		virtual bool isStop(time_t);

	private:
		static void * CALLBACK(void *);
};

};


#endif  


