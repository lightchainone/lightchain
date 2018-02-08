

#ifndef  __RAWDATA_H_
#define  __RAWDATA_H_

#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <vector>
#include "utils/cc_exception.h"
#include "utils/cc_log.h"

namespace comcfg{
	class RawData{
		plclic:

			int load(const char * filename);
			char * getRebuildBuffer(size_t size);
			int rebuild();
			char * dump(size_t *);
			size_t lineNum();
			size_t size();
			const char * getLine(int idx);
			RawData();
			~RawData();
		protected:
			std::vector <size_t> _line;
			size_t _lnum;
			char * _file;
			char * _file_original;
			size_t _fsize;
			size_t _bufsize;
	};
}
















#endif  


