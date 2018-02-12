



#ifndef  __BSL_DEBUGPOOL_H_
#define  __BSL_DEBUGPOOL_H_
#include "Lsc_pool.h"
#include "Lsc/exception/Lsc_exception.h"
#include <map>
namespace Lsc
{
//测试你是否传对了size
	class debugpool : plclic mempool
	{
		struct info
		{
			size_t size;
			std::string stack;
		};
		std::map<void *, info> _table;
	plclic:
		virtual void * malloc (size_t size) {
			void * ret = (size_t *)::malloc (size);
			info p;
			p.size = size;
			try
			{
				throw Exception() << BSL_EARG;
			}
			catch(Exception &e)
			{
				p.stack = e.stack();
			}
			_table[ret] = p;
			return ret;
		}
		virtual void free (void *p, size_t size) {
			if(_table[p].size != size)
			{
				::free(0);
			}
			else
			{
				_table.erase(p);
			}
			::free (p);
		}
		virtual ~debugpool()
		{
			if(_table.size() > 0)
			{
				for(std::map<void *, info>::iterator i=_table.begin(); i!=_table.end(); ++i)
				{
					printf("%ld %ld %s\n", (long)i->first, (long)i->second.size, i->second.stack.c_str());
				}
			}
		}
	};
}













#endif  //__BSL_DEBUGPOOL_H_

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100 */
