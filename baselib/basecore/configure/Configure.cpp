
#include "Configure.h"
#include "reader/Reader.h"
#include "constraint/Constraint.h"
#include "utils/cc_utils.h"
#include "idm_conf_if.h"
#include "Lsc/var/Dict.h"
#include "Lsc/var/Array.h"

namespace comcfg{

	using Lsc::var::IVar;

	
	int Configure :: load(const char * path, const char * conf, const char * range, int ){
		return load_1_0(path, conf, range);
	}

	int Configure :: load_1_0(const char * path, const char * conf, const char *range){
		int ret = 0;
		if(path == NULL || (conf == NULL && range == NULL)){
			ul_writelog(Log::warning(), "Configure.load : path or filename error...");
			return ERROR;
		}
		_path = path;
		if(_readers.size() != 0){
			ul_writelog(Log::warning(), "Configure.load : Configure can't be reused, create a new object for load.");
			return ERROR;
		}
#if 0
		if(conf != NULL){
			Reader * reader = new Reader();
			_cur_reader = reader;
			_cur_level = 0;
			_readers.push_back(reader);
			ret = reader->read(conf, this);
			_cur_reader = NULL;
		}
#endif
		size_t idx = 0;
		pushSlcReader(conf, 0);
		while(idx < _readers.size()){
			ReaderNode & r = (*_readers[idx]);
			_cur_reader = r.reader;
			_cur_level = r.level;
			_section = this;
			_depth = 0;
			ret = r.reader->read(r.filename.c_str(), this);
			if(ret != 0){
				ul_writelog(Log::warning(), "Reader configure file [%s] error. Stop.", r.filename.c_str());
				return CONFIG_ERROR;
			}
			++idx;
		}
		if(range != NULL && 0 != *range){
			_idm = confIDL::alloc_idm();
			str_t range_file;
			range_file.append(_path).append("/").append(range);
			load_idm(range_file.c_str(), _idm);
			Constraint cons;
			ret = cons.run(_idm, this);
			if(ret != 0){
				return CONSTRAINT_ERROR;
			}
			
		}
		
		return 0;
	}
	void Configure :: pushSlcReader(const char * conf, int level){
		if(conf == NULL){
			return;
		}
		if(level < 0){
			level = _cur_level + 1;
		}
		ul_writelog(Log::debug(), "Config $include : %s, MAX_DEPTH=%d, level=%d",
				conf, MAX_INCLUDE_DEPTH, level);
		if(level > MAX_INCLUDE_DEPTH){
			ul_writelog(Log::warning(), "Config error: Max $include level is %d [%s:%d]", 
					MAX_INCLUDE_DEPTH, _cur_reader->_cur_file.c_str(), _cur_reader->_cur_line);
			return;
		}
		ReaderNode * newNode = new ReaderNode();
		_readers.push_back(newNode);
		ReaderNode & r = *(_readers[_readers.size() - 1]);
		r.reader = new Reader();
		r.filename.append(_path).append("/").append(conf);
		r.level = level;
		ul_writelog(Log::trace(), "Config : $include : %s, level=%d", r.filename.c_str(), r.level);
	}

	char * Configure :: getRebuildBuffer(size_t __size){
		if(_readers.size() == 0){
			pushSlcReader("RebuildConfigure", 0);
		}
		else{
			ul_writelog(Log::warning(), "getRebuildBuffer : This Configure Object can't be re-used. Create a new one.");
			return NULL;
		}
		if(_readers.size() == 0){
			return NULL;
		}
		
		return _readers[0]->reader->getRebuildBuffer(__size);
	}

	int Configure :: rebuild(){
		if(_readers.size() == 0){
			return -1;
		}
		
		_cur_reader = _readers[0]->reader;
		_cur_level = 0;
		int ret = _cur_reader->rebuild(this);
		_cur_reader = NULL;
		return ret;
	}

	char * Configure :: dump(size_t * __size){
		
		const char * global = "\n[" CONFIG_GLOBAL "]\n";
		size_t len = strlen(global);
		if(_dump_buffer != NULL){
			if(__size){
				*__size = _dump_size;
			}
			return _dump_buffer;
		}
		if(_readers.size() == 0){
			return NULL;
		}
		_dump_size = 0;
		std::vector <const char *> bufs;
		std::vector <size_t> rsize;
		for(size_t i = 0; i < _readers.size(); ++i){
			size_t tmpsize = 0;
			
			char * p = _readers[i]->reader->dump(&tmpsize);
			rsize.push_back(tmpsize);
			bufs.push_back(p);
			
			ul_writelog(Log::notice(), "dump file : %s, size=%zu", _readers[i]->filename.c_str(), tmpsize);
			if(p == NULL){
				return NULL;
			}
			_dump_size += tmpsize;
			if(i){
				_dump_size += len;
			}
		}

		size_t bufsize = ((_dump_size >> 12) + 1) << 12;
		_dump_buffer = (char *)malloc(bufsize);
		if(_dump_buffer == NULL){
			ul_writelog(Log::warning(), "Can't build dump buffer");
			return NULL;
		}

		size_t pos = 0;
		for(size_t i = 0; i < _readers.size(); ++i){
			if(i){
				memcpy(_dump_buffer + pos, global, len);
				pos += len;
			}
			memcpy(_dump_buffer + pos, bufs[i], rsize[i]);
			pos += rsize[i];
		}
		if(_dump_size != pos){
			ul_writelog(Log::warning(), "What's wrong??? _dump_size(%zu) != real size(%zu)", 
					_dump_size, pos);
			return NULL;
		}
		_dump_buffer[_dump_size] = '\0';
		if(__size){
			*__size = _dump_size;
		}
		return _dump_buffer;
	}

	void Configure :: changeSection(str_t str){
		ul_writelog(Log::debug(), "Configure: This is [%s], current section[%s], depth=%d, father[%s]", 
				this->getName(), _section->getName(), _depth, _section->getFather()?_section->getFather()->getName():"NULL");
		if(str[0] != '.'){
			_section = this;
			_depth = 0;
		}
		else{
			int i;
			for(i = 0; i < (int)str.size(); ++i){
				if(str[i] != '.'){
					break;
				}
			}

			if((int)i > _depth){
				ul_writelog(Log::warning(), "Configure : Section Error [%s] Ignored.", str.c_str());
				throw ConfigException();
			}
			str = str.slcstr(i);
			
			i = _depth - i;
			while(i > 0){
				_section = (ConfigGroup *)_section->getFather();
				_depth--;
				i--;
			}
		}

		ConfigGroup * _bak_section = _section;
		ul_writelog(Log::trace(), "Config : set Section: New[%s] -> Father[%s]", str.c_str(), _section->getName());
		_section = (ConfigGroup *)_section->relativeSection(str, &_depth);
		if(_section == NULL){
			_section = _bak_section;
		}
		ul_writelog(Log::trace(), "Config : Now _section in : [%s]", _section?_section->getName():"NULL");
	}

	void Configure :: pushPair(const str_t& key, const str_t& value){
		ConfigUnit * tmp = new ConfigUnit(key, value, _cur_reader, _section);
		if(_section && _section->push(key, tmp)){
			throw ConfigException();
		}
	}

	time_t Configure :: lastConfigModify(){
		time_t t = time_t(0);
		struct stat st;
		for(int i = 0; i < (int)_readers.size(); ++i){
			
			if(_readers[i]->level <= MAX_INCLUDE_DEPTH){
				
				const char * f = _readers[i]->filename.c_str();
				if(stat(f, &st) == 0){
					if(t < st.st_mtime){
						t = st.st_mtime;
					}
				}
				else{
					ul_writelog(Log::warning(), "Check lastConfigModify : I can't stat file [%s]", f);
					
					
					
					
					
					return time_t(0);
				}
			}
		}
		return t;
	}


	enum _VarType{
		VAR_ARRAY = 0,
		VAR_DICT,
		VAR_UNIT
	};

	int varType(const IVar& ivar){
		if(ivar.is_array()){
			return VAR_ARRAY;
		}
		else if(ivar.is_dict()){
			return VAR_DICT;
		}
		return VAR_UNIT;
	}

	
	str_t seeVarUnit(const IVar & ivar, const char * key, bool isArray);
	str_t seeVarDict(const IVar & ivar, str_t * section);
	str_t seeVar(const IVar & ivar, str_t * section, const char * key = "");

	bool isVarUnit(const IVar & a, int dep = 1){
		if(dep < 0){
			throw ConfigException() << BSL_EARG << "Multi-dimension array is not supported.";
		}
		if(a.is_array()){
			return isVarUnit(a[0], dep-1);
		}
		if(a.is_dict()){
			return false;
		}
		return true;
	}

	str_t seeVarUnit(const IVar & ivar, const char * key, bool isArray){
		
		str_t ret;
		str_t value = ivar.to_string();
		str_t tmp;
		if (ivar.is_int32() || ivar.is_int64() || ivar.is_dolcle()) {
			tmp = value;
		} else {
			str_t strcontent="";
			int yinhao = 0;

			for(int i = 0; i < (int)value.size(); ++i){
				
				if(value[i] < 32 || value[i] >= 127 || value[i] == '\"' || value[i] == '\'' || value[i] == '\\'){
					strcontent.appendf("\\x%02x", value[i]);
					yinhao =1;
				}
				else{
					strcontent.appendf("%c", value[i]);
				}
			}
			if (yinhao) {
				tmp = "\"";
				tmp.append(strcontent);
				tmp.append("\"");
			} else {
				tmp = strcontent;
			}
		}
		ret.appendf("%s%s : %s\n", isArray?"@":"", key, tmp.c_str());
		return ret;
	}

	str_t nextSectionName(const str_t section, const char * key, bool isArray = false){
		str_t nsec;
		if(section == str_t(CONFIG_GLOBAL)){
			nsec = "";
			nsec.appendf("%s%s", isArray ? "@" : "", key);
		}
		else if(strstr(section.c_str(), "@") != NULL){
			nsec = ".";
			for(int i = 0; i < (int)section.size(); ++i){
				if(section[i] == '.'){
					nsec.appendf(".");
				}
			}
			nsec.appendf("%s%s", isArray ? "@" : "", key);
		}
		else{
			nsec = section;
			nsec.appendf("%s%s%s", nsec.size() == 0 ? "" : ".", isArray ? "@" : "", key);
		}
		return nsec;
	}


	str_t seeVarDict(const IVar & ivar, str_t * section){
		
		ul_writelog(Log::trace(), "Var %s is dict.", ivar.to_string().c_str());
		const IVar & dict = ivar; 
		Lsc::var::Dict::dict_const_iterator iter = dict.dict_begin();
		Lsc::var::Dict::dict_const_iterator end  = dict.dict_end();
		str_t bak_section = *section;
		str_t ret;
		ret.appendf("[%s]\n", section->c_str());
		for(; iter != end; ++ iter ){
			if( isVarUnit(iter->value()) ){
				ret.append( seeVar(iter->value(), section, iter->key().c_str()) );
			}
		}
		for(iter = dict.dict_begin(); iter != end; ++ iter ){
			if(! isVarUnit(iter->value()) ){
				ret.append( seeVar(iter->value(), section, iter->key().c_str()) );
				if(*section != bak_section){
					*section = bak_section;
				}
			}
		}
		return ret;
	}


	str_t seeVar(const IVar & ivar, str_t * section, const char * key){
		
		str_t ret;
		
		if(varType(ivar) == VAR_ARRAY){
			
			ul_writelog(Log::trace(), "Var %s is array.", ivar.to_string().c_str());
			if(varType(ivar[0]) == VAR_DICT ){
				*section = nextSectionName(*section, key, true);
			}
			for(int i = 0; i < (int)ivar.size(); ++i){
				if(varType(ivar[i]) == VAR_ARRAY){
					throw ConfigException() << BSL_EARG << "Multi-dimension array is not supported.";
				}
				if(varType(ivar[i]) != varType(ivar[0])){
					throw ConfigException() << BSL_EARG << "array elements should be the same type.";
				}
				if(varType(ivar[i]) == VAR_DICT){
					ret.append( seeVarDict(ivar[i], section) );
				}
				else{
					ret.append( seeVarUnit(ivar[i], key, true) );
				}
			}
		}
		else if(varType(ivar) == VAR_DICT){
			*section = nextSectionName(*section, key);
			ret.append( seeVarDict(ivar, section) );
		}
		else{
			ret.append( seeVarUnit(ivar, key, false) );
		}
		
		return ret;
	}


	int Configure :: loadIVar(const IVar & ivar){
#if 0
		if(ivar == NULL){
			ul_writelog(Log::warning(), "loadIVar : ivar = NULL");
			return -1;
		}
#endif
		try{
			if( ! ivar.is_dict()){
				ul_writelog(Log::warning(), "loadIVar : ivar should be a dict!");
				return -1;
			}

			str_t section = "";
			str_t s = seeVar(ivar, &section, CONFIG_GLOBAL);
			
			char * dest = getRebuildBuffer(s.size());
			if(dest == NULL){
				throw ConfigException() << BSL_EARG << "No memory???";
			}
			strcpy(dest, s.c_str());
			return rebuild();
		}catch(Lsc::Exception e){
			ul_writelog(Log::warning(), "loadIVar : catch exception : [%s:%d]%s", e.file(), e.line(), e.what());
			return -1;
		}catch(...){
			ul_writelog(Log::warning(), "loadIVar : other err...");
			return -1;
		}
		return 0;
	}


	Configure :: Configure(){
		create(CONFIG_GLOBAL);
		_section = this;
		_depth = 0;
		_idm = NULL;
		_cur_reader = NULL;
		_cur_level = -1;
		_dump_buffer = NULL;
		
	}

	Configure :: ~Configure(){
		for(size_t i = 0; i < _readers.size(); ++i){
			delete _readers[i]->reader;
			delete _readers[i];
		}
		if(_idm){
			free_idm(_idm);
		}
		if(_dump_buffer){
			free(_dump_buffer);
		}
	}

	int Configure :: check_once(const char * range, int ) {
		if(range == NULL){
			ul_writelog(Log::warning(), "Configure.check_once : range filename error...");
			return ERROR;
		}
		int chkret = OK;
		confIDL::idm_t * oneidm;
		oneidm = confIDL::alloc_idm();
		load_idm(range, oneidm);
		Constraint cons;
		int ret = cons.run(oneidm, this);
		if(ret != 0){
			chkret = CONSTRAINT_ERROR;
		}
		
		free_idm(oneidm);
		return chkret;
	}

}



















