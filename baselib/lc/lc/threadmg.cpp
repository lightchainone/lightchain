

#include "threadmg.h"

namespace lc
{
	int ThreadMg::run(int pnum, cb_t cb, void *param) 
	{
		for (int i=0; i<pnum; ++i) {
			pthread_t pid;
			if (pthread_create(&pid, NULL, cb, param) != 0) {
				return _tids.size();
			}
			_tids.push_back(pid);
		}
		return _tids.size();
	}

	int ThreadMg::join()
	{
		for (size_t i=0; i<_tids.size(); ++i) {
			pthread_join(_tids[i], NULL);
		}
		_tids.clear();
		return 0;
	}
}


