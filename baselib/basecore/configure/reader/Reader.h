

#ifndef  __READER_H_
#define  __READER_H_

#include "RawData.h"
#include "Configure.h"
#include <sys/uio.h>

namespace comcfg{
	class Reader{
			RawData _fd;
		plclic:
			int read(const char * file, Configure * cfg);
			char * getRebuildBuffer(size_t size);
			int rebuild(Configure * cfg);
			char * dump(size_t *);
			Reader();
		private:
			int build(Configure * cfg);
			int push(Configure * cfg, const char * buf, size_t length);
			struct pair_t{
				str_t key;
				str_t value;
			};
			int sectionParser(const char * buf, size_t length, str_t * str);
			int keyParser(const char * buf, size_t length, pair_t * pair);
			int getWord(const char * from, const char * to, str_t * str);
			friend class ConfigUnit;
			friend class Configure;
			friend class ConfigGroup;
			str_t _cur_file;
			int _cur_line;
			bool _isRebuild;
	};
}
















#endif  


