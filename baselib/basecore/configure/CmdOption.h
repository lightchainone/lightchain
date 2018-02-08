

#ifndef  __CMDOPTION_H_
#define  __CMDOPTION_H_

#include "ConfigGroup.h"
#include "utils/cc_utils.h"

namespace comcfg{
	
	class CmdOption : plclic ConfigGroup{
		plclic:
			
			int init(int argc, char * const * argv, const char * optstring);
			
			bool hasOption(const char *) const;
			
			size_t argSize() const;
			
			const char * getArg(int n) const;

			typedef int (*hook_t)(CmdOption * );
			
			int registHook(const char * option, hook_t hook);
			
			int runHook();
			virtual  ConfigUnit & operator= (ConfigUnit & unit) {
				return ConfigUnit::operator=(unit);
			}
			CmdOption();
		protected:
			static const int CHAR_TABLE_SIZE = 256;
			char ch[CHAR_TABLE_SIZE];
			static const char CMD_NONE = 0;
			static const char CMD_OPT0 = 1;
			static const char CMD_OPT1 = 2;
			static const char CMD_OPT2 = 3;
			std::vector <str_t> arg;
			std::multimap <str_t, hook_t> hkmap;
			typedef std::pair<str_t, hook_t> pair_t;
			typedef std::multimap<str_t, hook_t> :: iterator hkitr_t;
			typedef std::pair<hkitr_t, hkitr_t> hk_pair_t;

			int setOptString(const char *);
	};
}












#endif  


