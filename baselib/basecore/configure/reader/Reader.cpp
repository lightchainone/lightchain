
#include <ctype.h>
#include "utils/cc_utils.h"
#include "reader/Reader.h"

namespace comcfg{
	Reader :: Reader(){
		_isRebuild = false;
	}

	str_t get_middle(const str_t& str){
		int i, j;
		if(str.size() == 0){
			return str;
		}
		for(i = 0; i < (int)str.size(); ++i){
			if(! isspace(str[i])){
				break;
			}
		}
		for(j = (int)str.size() - 1; j >= 0; --j){
			if(! isspace(str[j])){
				break;
			}
		}
		return str.slcstr(i, j+1-i);
	}

	int Reader::read(const char * file, Configure * cfg){
		if( _fd.load(file) != 0 ){
			return -1;
		}
		_cur_file = file;
		return build(cfg);
	}
	int Reader :: build(Configure *cfg){
		int ret = 0;
		for(size_t i = 0; i < _fd.lineNum(); ++i){
			_cur_line = i+1;
			size_t length;
			if(i != _fd.lineNum() - 1){
				length = (size_t)_fd.getLine(i+1) - (size_t)_fd.getLine(i);
			}
			else{
				length = (size_t)_fd.size() - (size_t)_fd.getLine(i);
			}
			if(push(cfg, _fd.getLine(i), length) != 0){
				ul_writelog(Log::warning(), "Error [File:%s, Line:%d] %s", _cur_file.c_str(), (int)i+1, _fd.getLine(i));
				ret = -1;
			}
		}
		_cur_line = -1;
		return ret;
	}

	int Reader::push(Configure * cfg, const char * buf, size_t length){
		const char *p = buf;
		while(*p != 0 && isspace(*p)){
			++p;
		}
		try{
			if(*p == '#' || *p == 0){ 
				return 0;
			}
			else if (*p == '['){ 
				str_t str;
				if( sectionParser(p, length, &str) != 0 ){
					return -1;
				}
				ul_writelog(Log::trace(), "Configure Reader: Change section to : [%s]", str.c_str());
				cfg->changeSection(str);
			}
			else{ 
				pair_t pair;
				if( keyParser(p, length, &pair) != 0 ){
					return -1;
				}
				if(strcmp(pair.key.c_str(), CONFIG_INCLUDE) == 0){
					
					
					ul_writelog(Log::trace(), "Configure Reader: read $include : %s", pair.value.c_str());
					if(_isRebuild){
						ul_writelog(Log::trace(), "Configure : I am rebuilding, $include is ignored.");
					}else{
						str_t realfile;
						Trans::str2str(pair.value, &realfile);
						cfg->pushSlcReader(realfile.c_str());
					}
				}
				else{
					ul_writelog(Log::trace(), "Configure Reader: read key[%s] value[%s]", 
							pair.key.c_str(), pair.value.c_str());
					cfg->pushPair(pair.key, pair.value);
				}
			}
		}
		catch(ConfigException){
			return -1;
		}
		catch(...){
			ul_writelog(Log::warning(), "Configure Reader: Unknown exception.");
			return -1;
		}
		return 0;
	}

	int Reader::sectionParser(const char * buf, size_t , str_t * str){
		const char *p = buf;
		const char *end = buf;
		while(*end != 0 && *end != ']'){
			++end;
		}
		if(0 == *end || p+1 >= end){
			return -1;
		}
		if(getWord(p+1, end, str) != 0){
			return -1;
		}
		++end;
		
		while(*end){
			if(*end == '#'){
				return 0;
			}
			if(! isspace(*end)){
				return -1;
			}
			++end;
		}
		return 0;
	}


	int Reader::keyParser(const char * buf, size_t , pair_t * pair){
		const char *p = buf;
		while(*p != 0 && *p != ':'){
			++p;
		}
		if(0 == *p || p == buf){ 
			return -1;
		}
		if( getWord(buf, p, &(pair->key)) != 0 ){
			return -1;
		}

		pair->value = p+1;
		
		pair->value = get_middle(pair->value);
		
		return 0;
	}


	int Reader::getWord(const char * from, const char * to, str_t * str){
		
		int status = 0; 
		const char *f1 = from;
		const char *f2 = to;
		for(const char *i = from; i < to; ++i){
			if(isspace(*i)){
				if(status == 1){
					f2 = i;
					status = 2;
				}
			}
			else{
				if(status == 0){
					status = 1;
					f1 = i;
				}
				else if(status == 2){
					return -1;
				}
			}
		}
		if(f1 >= f2){
			return -1;
		}
		
		
		
		
		
		str_t tmp;
		tmp.append(f1, f2-f1);
		*str = tmp;
		return 0;
	}

	char * Reader :: getRebuildBuffer(size_t size){
		return _fd.getRebuildBuffer(size);
	}
	int Reader :: rebuild(Configure * cfg){
		_isRebuild = true;
		if( _fd.rebuild() != 0 ){
			return -1;
		}
		_cur_file = "rebuild";
		return build(cfg);
	}
	char * Reader :: dump(size_t * size){
		return _fd.dump(size);
	}
}












