
#include "lcinlog.h"
#include "lcutils.h"

namespace lc
{
	static char g_findMaxTable[256];
	static int genFindMaxTable() {
		int pos = 0;
		for (int i=0; i<256; ++i) {
			if (i >= (1<<pos)) {
				++pos;
			}
			g_findMaxTable[i] = pos - 1;
			
		}
		return 0;
	}
	static int ________111 = genFindMaxTable();
	int findMax2Bin(unsigned int t)
	{
		if (t < (1<<8)) {
			return g_findMaxTable[t];
		}
		if (t < (1<<16)) {
			return g_findMaxTable[t>>8] + 8;
		}
		if (t < (1<<24)) {
			return g_findMaxTable[t>>16] + 16;
		}
		return g_findMaxTable[t>>24] + 24;
	}

	const comcfg::ConfigUnit & Flag::get (const char *key, char flag) {
		if (opt().hasOption(key)) {
			LC_IN_DEBUG("using option %s", key);
			return opt(key);
		}
		std::vector<Lsc::string> vec;
		for (int i=0; key[i] != 0; ++i) {
			char s[1024];
			int pos = 0;
			while (key[i] != 0 && key[i] != flag) {
				s[pos++] = key[i];
				++ i;
			}
			if (key[i] == flag || key[i] == 0) {
				if (pos > 0) {
					s[pos] = 0;
					vec.push_back(s);
					LC_IN_DEBUG("push %s into vec", s);
				}
			}
			if (key[i] == 0) {
				break;
			}
		}
		if (vec.size() == 0) {
			return cfg(key);
		}
		const comcfg::ConfigUnit * unit = & cfg(vec[0].c_str());
		for (size_t i=1; i<vec.size(); ++i) {
			unit = &((*unit)[vec[i].c_str()]);
		}
		return *unit;
	}
};



