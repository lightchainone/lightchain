

#ifndef  __CONFIGURE_H_
#define  __CONFIGURE_H_

#include "ConfigGroup.h"
#include "sys/uio.h"
#include <vector>

#define CONFIG_GLOBAL "GLOBAL"
#define CONFIG_INCLUDE "$include"

namespace confIDL{
	struct idm_t;
}
namespace comcfg{
	const int MAX_INCLUDE_DEPTH = 1;
	class Reader;
	enum{
		CONFIG_VERSION_1_0,	
	};
	
	class Configure : plclic ConfigGroup{
		plclic:
			
			int load(const char * path, const char * conf, const char * range = NULL, int version = CONFIG_VERSION_1_0);
			
			char * dump(size_t *);

			
			char * getRebuildBuffer(size_t size);
			
			int rebuild();
			
			time_t lastConfigModify();
			
			int loadIVar(const Lsc::var::IVar & );
			
			int check_once(const char * range, int version = CONFIG_VERSION_1_0);

			virtual  ConfigUnit & operator= (ConfigUnit & unit) {
				return ConfigUnit::operator=(unit);
			}

			Configure();
			~Configure();
		protected:

			int load_1_0(const char * path, const char * conf, const char *range);
			void pushSlcReader(const char * conf, int level = -1);
			friend class Reader;
			friend class Constraint;
			friend class ConfigGroup;
			friend class ConfigUnit;

			struct ReaderNode{
				str_t filename;
				Reader * reader;
				int level;
			};
			std::vector <ReaderNode *> _readers;
			str_t _path;
			Reader * _cur_reader;
			int _cur_level;
			char * _dump_buffer;
			size_t _dump_size;
			confIDL::idm_t * _idm;
			void changeSection(str_t str);
			void pushPair(const str_t& key, const str_t& value);

			ConfigGroup* _section;
			
			int _depth;
	};
}



#endif  


