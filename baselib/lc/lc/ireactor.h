

#ifndef  __LC_IREACTOR_H_
#define  __LC_IREACTOR_H_

#include "lcioc.h"
#include "lcutils.h"

namespace lc
{
	class IEvent;
	class IReactor 
	{
		plclic:
			enum {
				INIT,
				STOP,
				RUNNING,
				PAUSE,
			};
			enum {
				CPU,
				NET
			};
		plclic:
			virtual ~IReactor() {};
		
		plclic:
			
			virtual int load(const comcfg::ConfigUnit &) = 0;
			
			virtual void setPriority(int pri) = 0;
			
			virtual void setThread(int core) = 0;
			
			virtual void setMaxEvents(int num) = 0;
		plclic:
			
			virtual int run_tv(const timeval *) = 0;
			
			virtual int run();
			
			virtual int run_ms(int msec);

			
			virtual int runUntilEmpty() = 0;
			
			virtual int stop_tv(const timeval *) = 0;
			
			virtual int stop();
			
			virtual int stop_ms(int msec);
			
			virtual int stopUntilEmpty() = 0;

			
			
			

			
			
			virtual int join() = 0;

			
			virtual int status() = 0;

			
			virtual int smartPost(IEvent *);

			
			virtual int smartPost(IEvent *ev, int type);

			
			virtual int post(IEvent *) = 0;

			
			virtual int post(IEvent *ev, int type) = 0;

			
			virtual int cancel(IEvent *) = 0;

			
			virtual IReactor * getExternReactor() = 0;
	};
	extern IOC<IReactor> IReactorIoc;
};

#endif  


