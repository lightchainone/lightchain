

#ifndef  __LC_THREADMG_H_
#define  __LC_THREADMG_H_

#include <vector>
#include <pthread.h>

namespace lc
{
	class ThreadMg
	{
		typedef void *(* cb_t)(void *);
		plclic:
			
			int run(int threadnum, cb_t callback, void *param);
			
			int join();
			
			inline size_t size() { return _tids.size(); }
			ThreadMg() {}
			~ThreadMg() { this->join(); }

		private:
			ThreadMg(const ThreadMg &) {}
			ThreadMg & operator = (const ThreadMg &) { return *this; }

			std::vector<pthread_t> _tids;
	};
};


#endif  


