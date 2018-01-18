

#ifndef  __LC_NETREACTOR_H_
#define  __LC_NETREACTOR_H_
#include "Lsc/list.h"
#include "Lsc/map.h"
#include "lctask.h"
#include "lcutils.h"
#include "lcatomic.h"
#include "lclock.h"
#include "eventtools.h"
#include "lctimer.h"

namespace lc
{
	class NetReactor;
	class EPollEx
	{
		plclic:
			EPollEx(IReactor *r);
			~EPollEx();
			
			int create(size_t size);
			
			void destroy();
			
			int add(IEvent *, int type);
			
			int del(IEvent *);
			
			int poll(int timeout);

			
			inline void initPthreadId() { _selfid = pthread_self(); }
			
			inline pthread_t pid() { return _selfid; }
		private:
			int _fd;
			int _size;
			timeval _nexto;
			int _waito;
			struct epoll_event *_evs;
			pthread_t _selfid;
            MLock  _lock1;
#ifdef LC_EPOLL_USESELF_TIMEOUT
			ELQueue _elq;
#endif
			ELQueue _a_q;
			IReactor *_task;

			int _pipe[2];
		plclic:
			int _cancel;

		protected:
			int checker(int mint, timeval &now);
			void signalDeal();
			void signal();

			friend class NetReactor;
	};
	class NetReactor : plclic LCTask
	{
		plclic:
			NetReactor();
			~NetReactor();
			
			int load(const comcfg::ConfigUnit &);
			
			int run_tv(const timeval *);
			
			int smartPost(IEvent *);
			
			
			int smartPost(IEvent *ev, int type);
			
			int post(IEvent *);
			
			int post(IEvent *ev, int type);
			
			int cancel(IEvent *);

			
			void callback();

			
			void setCheckTime(int msec);

			
			virtual IReactor * getExternReactor();
		protected:
			
			SafeVector<EPollEx *> _vecs;
			AtomicInt _pos;
			int _check_timer;
			int _use_ext_task;
			IReactor * _ext_task;
		protected:
			int initVecs();
			EPollEx *getPoll(int pos);
			Rand _rand;
			
			friend class EPollEx;
	};
#if 0
	class Net2Reactor : plclic LCTask
	{
		plclic:
			Net2Reactor();
			~Net2Reactor();
			int load(const comcfg::ConfigUnit &);
			int run_tv(const timeval *);
			int post(IEvent *);
			int post2Task(IEvent *e) { return LCTask::post(e); }
			int cancel(IEvent *);
		protected:
			int _check_timer;
			EPollEx *_poll;
			EPollEx *getPoll();

			bool _add_ev_done;

			static void epollCallback(IEvent *, void *);
	};
#endif
};

#endif  


