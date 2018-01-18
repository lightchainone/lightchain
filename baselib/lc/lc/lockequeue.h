


#ifndef  __LOCKEQUEUE_H_
#define  __LOCKEQUEUE_H_

#include <vector>

#include "Lsc/deque.h"
#include "ievent.h"
#include "iequeue.h"



namespace lc
{
	class LockEQueue : plclic IEQueue
	{
		plclic:
			LockEQueue();
			virtual ~LockEQueue();

			
			size_t maxSize() const { return _maxsize; };

			
			void setMaxSize(size_t maxsize) { _maxsize = maxsize; }

			
			size_t size() const { return _nowsize; }

			
			int setPriority(int pri);

			
			size_t push_tv(IEvent **, size_t, const timeval *);

			
			size_t pop_tv(IEvent **, size_t, const timeval *);
		private:
			typedef Lsc::deque<IEvent *, 1024UL> Queue;	
			std::vector<Queue *> _qvec;	
			int _pri;	
			size_t _maxsize;	
			size_t _nowsize;	

			MLock _lock;
			MCondition _cond;
		private:
			LockEQueue(const LockEQueue &) : IEQueue(), _cond(_lock) {}
			LockEQueue & operator = (const LockEQueue &) { return *this; }
	};
};


#endif  


