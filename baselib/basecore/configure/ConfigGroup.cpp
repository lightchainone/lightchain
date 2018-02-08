
#include "ConfigGroup.h"
#include "ConfigArray.h"
#include "Configure.h"
#include "utils/cc_utils.h"
#include "Lsc/var/Dict.h"

namespace comcfg{
	ConfigUnit * ConfigGroup :: relativeSection(str_t str, int *depth){
		ul_writelog(Log::trace(), "Config : [relativeSection]Current section : [%s] depth[%d]", getName(), *depth);
		if(str == ""){
			return this;
		}
		str_t child;
		size_t global_len = strlen(CONFIG_GLOBAL);
		if(str.slcstr(0, global_len) == str_t(CONFIG_GLOBAL) && 
				(str.size() == global_len || str[global_len] == '.')){
			ConfigGroup * sec = this;
			while(sec->getFather()){
				sec = sec->getFather();
			}
			(*depth) = 0;
			str = str.size() > global_len ? str.slcstr(global_len+1) : "";
			ul_writelog(Log::trace(), "Config : switch to global [%s] <-- This must be GLOBAL", sec->getName());
			return sec->relativeSection(str, depth);
		}
		for(int i = 0; i < (int)str.size(); ++i){
			if(str[i] == '.'){
				child = str.slcstr(0, i);
				str = str.slcstr(i+1);
				itr_t itr = fieldMap.find(child);
				if(itr == fieldMap.end()){
					ul_writelog(Log::warning(), "Configure : No previous section named [%s]", child.c_str());
					throw ConfigException();
				}
				++(*depth);
				return (itr->second)->relativeSection(str, depth);
			}
		}
		itr_t itr = fieldMap.find(str);
		if(itr == fieldMap.end()){
			ConfigGroup * tmp = new ConfigGroup(str.c_str(), this);
			++(*depth);
			push(str, tmp);
			return tmp;
		}
		else{
			++(*depth);
			if((itr->second)->selfType() != CONFIG_GROUP_TYPE){
				ul_writelog(Log::warning(), "Config : Duplicate key or group name [%s]", str.c_str());
				throw ConfigException();
			}
			return itr->second;
		}
	}
	void ConfigGroup :: create(const char * __name, ConfigGroup * __father){
		const char *p = __name;
		if(*p == '@'){
			++p;
		}
		while(*p != 0){
			if((!isalpha(*p)) && (!isdigit(*p)) && *p != '_'){
				ul_writelog(Log::warning(), "Configure : Unsupport  _name [%s]", __name);
				throw ConfigException();
				
			}
			++p;
		}
		ul_writelog(Log::debug(), "Config: this=%lx, Set _name = %s, _father = %lx[%s]", 
				long(this), __name, long(__father), __father==NULL?"NULL":__father->getName());
		_name = __name;
		_father = __father;
		if(_father){
			_level = _father->getLevel() + 1;
		}
		else{
			_level = 0;
		}
	}

	int ConfigGroup :: push(const str_t& key, ConfigUnit* unit){
		ul_writelog(Log::trace(), "Configure : Push key [%s] to [%s]", key.c_str(), getName());
		if(key[0] != '@'){
			if(fieldMap.find(key) == fieldMap.end()){
				fieldMap[key] = unit;
				fieldList.push_back(key);
			}
			else{
				ul_writelog(Log::warning(), "Configure : [%s] Duplicate key [%s]", getName(), key.c_str());
				throw ConfigException();
				return -1;
			}
		}
		else{
			itr_t itr = fieldMap.find(key.slcstr(1));
			if(itr != fieldMap.end()){
				if(itr->second->selfType() == CONFIG_ARRAY_TYPE){
					((ConfigArray*)(itr->second))->push(key, unit);
				}
				else{
					ul_writelog(Log::warning(), "Configure : ambiguity key [%s]", key.c_str());
					throw ConfigException();
					return -1;
				}
			}
			else{
				ConfigArray * array = new ConfigArray(key.c_str() + 1, this);
				if(this->push(key.slcstr(1), array) == 0){
					
					ul_writelog(Log::trace(), "Config : we have new array [%s]", key.c_str() + 1);
					array->push(key, unit);
				}
			}
		}
		return 0;
	}

	const ConfigUnit & ConfigGroup :: operator[] (const char * str) const{
		return (*this)[str_t(str)];
	}
	const ConfigUnit & ConfigGroup :: operator[] (const str_t & str) const{
		const_itr_t itr = fieldMap.find(str);
		if(itr != fieldMap.end()){
			return *(itr->second);
		}
		ul_writelog(Log::trace(), "Configure: visiting key not found: ConfigGroup[str_t] : %s[%s]", _name.c_str(), str.c_str());
		str_t tmp = this->_name;
		if (tmp == CONFIG_GLOBAL){
			tmp = str;
		} else {
			tmp.append(".").append(str);
		}
		setErrorKeyPath(tmp);
		return *get_err_unit();
	}
	const ConfigUnit & ConfigGroup :: operator[] (int idx) const{
		ul_writelog(Log::warning(), "Configure: visit ConfigGroup[int] : %s[%d]", _name.c_str(), idx);
		str_t tmp=this->_name;
		char tmpid[64];
		snprintf(tmpid,64, "%d",idx);
		tmp.append("[").append(tmpid).append("]");;
		setErrorKeyPath(tmp);
		return *get_err_unit();
	}
	ConfigUnit & ConfigGroup :: operator[] (const char * str) {
		return (*this)[str_t(str)];
	}
	ConfigUnit & ConfigGroup :: operator[] (const str_t & str) {
		const_itr_t itr = fieldMap.find(str);
		if(itr != fieldMap.end()){
			return *(itr->second);
		}
		ul_writelog(Log::trace(), "Configure: visiting key not found: ConfigGroup[str_t] : %s[%s]", _name.c_str(), str.c_str());
		str_t tmp = this->_name;
		if (tmp == CONFIG_GLOBAL){
			tmp = str;
		} else {
			tmp.append(".").append(str);
		}
		setErrorKeyPath(tmp);
		return *get_err_unit();
	}
	ConfigUnit & ConfigGroup :: operator[] (int idx) {
		ul_writelog(Log::warning(), "Configure: visit ConfigGroup[int] : %s[%d]", _name.c_str(), idx);
		str_t tmp=this->_name;
		char tmpid[64];
		snprintf(tmpid,64, "%d",idx);
		tmp.append("[").append(tmpid).append("]");;
		setErrorKeyPath(tmp);
		return *get_err_unit();
	}
	ConfigGroup :: ~ConfigGroup(){
		itr_t itr = fieldMap.begin();
		while(itr != fieldMap.end()){
			delete itr->second;
			++itr;
		}
	}
	void ConfigGroup :: print(int indent) const{
		str_t sec = getName();
		ConfigGroup * tmp = getFather();
		while(tmp != NULL){
			sec = str_t(tmp->getName()).append(".").append(sec);
			tmp = tmp->getFather();
		}
		pindent(indent);
		printf("Section-------[%s]\n", sec.c_str());
		for(int i = 0; i < (int)fieldList.size(); ++i){
			if(fieldMap.find(fieldList[i])-> second -> selfType() == CONFIG_UNIT_TYPE){
				fieldMap.find(fieldList[i]) -> second -> print(indent + 1);
			}
		}
		for(int i = 0; i < (int)fieldList.size(); ++i){
			if(fieldMap.find(fieldList[i]) -> second -> selfType() == CONFIG_ARRAY_TYPE){
				fieldMap.find(fieldList[i]) -> second -> print(indent + 1);
			}
		}
		for(int i = 0; i < (int)fieldList.size(); ++i){
			if(fieldMap.find(fieldList[i]) -> second -> selfType() == CONFIG_GROUP_TYPE){
				fieldMap.find(fieldList[i]) -> second -> print(indent + 1);
			}
		}
	}

	Lsc::var::IVar& ConfigGroup :: to_IVar(Lsc::ResourcePool * vpool, ErrCode* errCode)const{
		if(vpool == NULL){
			ul_writelog(Log::trace(), "Configure: visit ConfigGroup.to_IVar() failed : NULLBUFFER");
			setErrorKeyPath(this->_name);
			popError(errCode, NULLBUFFER);
			return Lsc::var::Null::null;
		}
		Lsc::var::Dict & dict = vpool->create<Lsc::var::Dict>();
		for(int i = 0; i < (int)fieldList.size(); ++i){
			ConfigUnit * p = fieldMap.find(fieldList[i]) -> second;
			Lsc::var::IVar & son = p->to_IVar(vpool, errCode);
			dict.set( fieldList[i], son );
		}
		return dict;
	}

	char ConfigGroup :: to_char(ErrCode * errCode)const{
		setErrorKeyPath(this->_name);
		popError(errCode, GROUPTOUNIT);
		return char();
	}

	unsigned char ConfigGroup :: to_uchar(ErrCode * errCode)const{
		setErrorKeyPath(this->_name);
		popError(errCode, GROUPTOUNIT);
		return (unsigned char)(0);
	}

	int16_t ConfigGroup :: to_int16(ErrCode * errCode)const{
		setErrorKeyPath(this->_name);
		popError(errCode, GROUPTOUNIT);
		return int16_t();
	}

	u_int16_t ConfigGroup :: to_uint16(ErrCode * errCode)const{
		setErrorKeyPath(this->_name);
		popError(errCode, GROUPTOUNIT);
		return u_int16_t();
	}

	int ConfigGroup :: to_int32(ErrCode * errCode)const{
		setErrorKeyPath(this->_name);
		popError(errCode, GROUPTOUNIT);
		return int();
	}

	u_int32_t ConfigGroup :: to_uint32(ErrCode * errCode)const{
		setErrorKeyPath(this->_name);
		popError(errCode, GROUPTOUNIT);
		return u_int32_t();
	}

	long long ConfigGroup :: to_int64(ErrCode * errCode)const{
		setErrorKeyPath(this->_name);
		popError(errCode, GROUPTOUNIT);
		return (long long)(0);
	}

	unsigned long long ConfigGroup :: to_uint64(ErrCode * errCode)const{
		setErrorKeyPath(this->_name);
		popError(errCode, GROUPTOUNIT);
		return (unsigned long long)(0);
	}

	float ConfigGroup :: to_float(ErrCode * errCode)const{
		setErrorKeyPath(this->_name);
		popError(errCode, GROUPTOUNIT);
		return float();
	}

	dolcle ConfigGroup :: to_dolcle(ErrCode * errCode)const{
		setErrorKeyPath(this->_name);
		popError(errCode, GROUPTOUNIT);
		return dolcle();
	}
#if 1
	Lsc_string ConfigGroup :: to_Lsc_string(ErrCode * errCode)const{
		setErrorKeyPath(this->_name);
		popError(errCode, GROUPTOUNIT);
		return Lsc_string();
	}
#endif
	str_t ConfigGroup :: to_raw_string(ErrCode * errCode)const{
		setErrorKeyPath(this->_name);
		popError(errCode, GROUPTOUNIT);
		return str_t();
	}
	const char * ConfigGroup :: to_cstr(ErrCode * errCode)const{
		setErrorKeyPath(this->_name);
		popError(errCode, GROUPTOUNIT);
		return NULL;
	}

	
	char ConfigGroup :: to_char(ErrCode * errCode, const char & def)const {
		if(errCode){
			*errCode  = GROUPTOUNIT;
		}
		setErrorKeyPath(this->_name);
		return def;
	}
	unsigned char ConfigGroup :: to_uchar(ErrCode * errCode, const unsigned char & def)const{
		setErrorKeyPath(this->_name);
		if(errCode){
			*errCode  = GROUPTOUNIT;
		}
		return def;
	}
	int16_t ConfigGroup :: to_int16(ErrCode * errCode, const int16_t & def)const{
		setErrorKeyPath(this->_name);
		if(errCode){
			*errCode  = GROUPTOUNIT;
		}
		return def;
	}
	u_int16_t ConfigGroup :: to_uint16(ErrCode * errCode, const u_int16_t & def)const{
		setErrorKeyPath(this->_name);
		if(errCode){
			*errCode  = GROUPTOUNIT;
		}
		return def;
	}
	int ConfigGroup :: to_int32(ErrCode * errCode, const int & def)const{
		setErrorKeyPath(this->_name);
		if(errCode){
			*errCode  = GROUPTOUNIT;
		}
		return def;
	}
	u_int32_t ConfigGroup :: to_uint32(ErrCode * errCode, const u_int32_t & def)const{
		setErrorKeyPath(this->_name);
		if(errCode){
			*errCode  = GROUPTOUNIT;
		}
		return def;
	}
	long long ConfigGroup :: to_int64(ErrCode * errCode, const long long & def)const{
		setErrorKeyPath(this->_name);
		if(errCode){
			*errCode  = GROUPTOUNIT;
		}
		return def;
	}
	unsigned long long ConfigGroup :: to_uint64(ErrCode * errCode, const unsigned long long & def)const{
		setErrorKeyPath(this->_name);
		if(errCode){
			*errCode  = GROUPTOUNIT;
		}
		return def;
	}
	float ConfigGroup :: to_float(ErrCode * errCode, const float & def)const{
		setErrorKeyPath(this->_name);
		if(errCode){
			*errCode  = GROUPTOUNIT;
		}
		return def;
	}
	dolcle ConfigGroup :: to_dolcle(ErrCode * errCode, const dolcle & def)const{
		setErrorKeyPath(this->_name);
		if(errCode){
			*errCode  = GROUPTOUNIT;
		}
		return def;
	}
	Lsc_string ConfigGroup :: to_Lsc_string(ErrCode * errCode, const Lsc_string & def)const{
		setErrorKeyPath(this->_name);
		if(errCode){
			*errCode  = GROUPTOUNIT;
		}
		return def;
	}
	str_t ConfigGroup :: to_raw_string(ErrCode * errCode, const str_t & def)const{
		setErrorKeyPath(this->_name);
		if(errCode){
			*errCode  = GROUPTOUNIT;
		}
		return def;
	}
	
	const char * ConfigGroup :: to_cstr(ErrCode * errCode, const char * def)const{
		setErrorKeyPath(this->_name);
		if(errCode){
			*errCode  = GROUPTOUNIT;
		}
		return def;
	}

	ErrCode ConfigGroup :: get_char(char * )const{
		setErrorKeyPath(this->_name);
		return GROUPTOUNIT;
	}

	ErrCode ConfigGroup :: get_uchar(unsigned char * )const{
		setErrorKeyPath(this->_name);
		return GROUPTOUNIT;
	}

	ErrCode ConfigGroup :: get_int16(int16_t * )const{
		setErrorKeyPath(this->_name);
		return GROUPTOUNIT;
	}

	ErrCode ConfigGroup :: get_uint16(u_int16_t * )const{
		setErrorKeyPath(this->_name);
		return GROUPTOUNIT;
	}

	ErrCode ConfigGroup :: get_int32(int * )const{
		setErrorKeyPath(this->_name);
		return GROUPTOUNIT;
	}

	ErrCode ConfigGroup :: get_uint32(u_int32_t * )const{
		setErrorKeyPath(this->_name);
		return GROUPTOUNIT;
	}

	ErrCode ConfigGroup :: get_int64(long long * )const{
		setErrorKeyPath(this->_name);
		return GROUPTOUNIT;
	}

	ErrCode ConfigGroup :: get_uint64(unsigned long long * )const{
		setErrorKeyPath(this->_name);
		return GROUPTOUNIT;
	}

	ErrCode ConfigGroup :: get_float(float * )const{
		setErrorKeyPath(this->_name);
		return GROUPTOUNIT;
	}

	ErrCode ConfigGroup :: get_dolcle(dolcle * )const{
		setErrorKeyPath(this->_name);
		return GROUPTOUNIT;
	}
#if 1
	ErrCode ConfigGroup :: get_Lsc_string(Lsc_string * )const{
		setErrorKeyPath(this->_name);
		return GROUPTOUNIT;
	}
#endif
	ErrCode ConfigGroup :: get_raw_string(str_t * )const{
		setErrorKeyPath(this->_name);
		return GROUPTOUNIT;
	}
	ErrCode ConfigGroup :: get_cstr(char * , size_t )const{
		setErrorKeyPath(this->_name);
		return GROUPTOUNIT;
	}

	

	ErrCode ConfigGroup :: get_char(char * valueBuf, const char & def)const{
		setErrorKeyPath(this->_name);
		if (valueBuf) {
			*valueBuf = def;
		}
		return GROUPTOUNIT;
	}

	ErrCode ConfigGroup :: get_uchar(unsigned char * valueBuf, const unsigned char & def)const{
		setErrorKeyPath(this->_name);
		if (valueBuf) {
			*valueBuf = def;
		}
		return GROUPTOUNIT;
	}

	ErrCode ConfigGroup :: get_int16(int16_t * valueBuf, const int16_t & def)const{
		setErrorKeyPath(this->_name);
		if (valueBuf) {
			*valueBuf = def;
		}
		return GROUPTOUNIT;
	}

	ErrCode ConfigGroup :: get_uint16(u_int16_t * valueBuf, const u_int16_t & def)const{
		setErrorKeyPath(this->_name);
		if (valueBuf) {
			*valueBuf = def;
		}
		return GROUPTOUNIT;
	}

	ErrCode ConfigGroup :: get_int32(int * valueBuf, const int & def)const{
		setErrorKeyPath(this->_name);
		if (valueBuf) {
			*valueBuf = def;
		}
		return GROUPTOUNIT;
	}

	ErrCode ConfigGroup :: get_uint32(u_int32_t * valueBuf, const u_int32_t & def)const{
		setErrorKeyPath(this->_name);
		if (valueBuf) {
			*valueBuf = def;
		}
		return GROUPTOUNIT;
	}

	ErrCode ConfigGroup :: get_int64(long long * valueBuf, const long long & def)const{
		setErrorKeyPath(this->_name);
		if (valueBuf) {
			*valueBuf = def;
		}
		return GROUPTOUNIT;
	}

	ErrCode ConfigGroup :: get_uint64(unsigned long long * valueBuf, const unsigned long long & def)const{
		setErrorKeyPath(this->_name);
		if (valueBuf) {
			*valueBuf = def;
		}
		return GROUPTOUNIT;
	}

	ErrCode ConfigGroup :: get_float(float * valueBuf, const float & def)const{
		setErrorKeyPath(this->_name);
		if (valueBuf) {
			*valueBuf = def;
		}
		return GROUPTOUNIT;
	}

	ErrCode ConfigGroup :: get_dolcle(dolcle * valueBuf, const dolcle & def)const{
		setErrorKeyPath(this->_name);
		if (valueBuf) {
			*valueBuf = def;
		}
		return GROUPTOUNIT;
	}

	ErrCode ConfigGroup :: get_Lsc_string(Lsc_string * valueBuf, const Lsc_string & def)const{
		setErrorKeyPath(this->_name);
		if (valueBuf) {
			*valueBuf = def;
		}
		return GROUPTOUNIT;
	}

	ErrCode ConfigGroup :: get_raw_string(str_t * valueBuf, const str_t & def)const{
		setErrorKeyPath(this->_name);
		if (valueBuf) {
			*valueBuf = def;
		}
		return GROUPTOUNIT;
	}

	ErrCode ConfigGroup :: get_cstr(char * valueBuf, size_t len, const char * def)const{
		setErrorKeyPath(this->_name);
		if (valueBuf) {
			snprintf(valueBuf, len, "%s", def);
		}
		return GROUPTOUNIT;
	}

	const ConfigUnit & ConfigGroup :: get_slc_unit(int index) const {
		int susize = (int) fieldMap.size();
		if ((0 > index) || (index >= susize)) {
			return *get_err_unit();
		}
		ConfigUnit * p = fieldMap.find(fieldList[index]) -> second;
		if (NULL == p) {
			return *get_err_unit();
		}
		return *p;
	}

	const Lsc::string & ConfigGroup :: get_key_name() const {
		return this->_name;
	}

	int ConfigGroup :: equals(const ConfigUnit & conf) const {
		try {
			const_itr_t itr;
			for  (itr = fieldMap.begin();itr != fieldMap.end(); ++itr) {
				if (0 != itr->second->equals(conf[itr->first])) {
					return -1;
				}
			}
			return 0;
		} catch (Lsc::Exception) {
			return -1;
		}
		return -1;
	}

	int ConfigGroup :: add_unit(const Lsc_string & key, const Lsc_string& value, 
			const int objtype, int except, ConfigUnit ** ref) {
		if (CONFIG_ERROR_TYPE == (*this)[key].selfType()) {
			ConfigUnit * tmp = create_unit(key, value, objtype, this);
			if (NULL == tmp){
				if (except) {
					popError(NULL, CONFIG_ERROR);
				}
				return CONFIG_ERROR;
			}
			this->push(key,tmp);
			if (NULL != ref) {
				*ref = tmp;
			}
		} else {
			if (except) {
				if (0 != ul_seterrbuf("%s", "")) {
					ul_writelog(Log::warning(), "Configure: failed to write error buffer");
				}
				popError(NULL, DUPLICATED_KEY);
			}
			return DUPLICATED_KEY;
		}
		return 0;
	}
	int ConfigGroup :: del_unit(const Lsc_string & key, int except) {
		if (CONFIG_ERROR_TYPE != (*this)[key].selfType()) {
			std::vector <str_t > :: iterator iter;
			for ( iter = fieldList.begin(); iter != fieldList.end(); )
			{
				if ( *iter == key )
				{
					iter = fieldList.erase( iter );
					break;
				}
				iter++;
			}
			ConfigUnit * delu = NULL;
			itr_t itr = fieldMap.find(key);
			if(itr != fieldMap.end()){
				delu = itr->second;
				fieldMap.erase(itr);
			}
			if (delu) {
				delete delu;
			}
		} else {
			if (except) {
				if (0 != ul_seterrbuf("%s", "")) {
					ul_writelog(Log::warning(), "Configure: failed to write error buffer");
				}
				popError(NULL, NOSUCHKEY);
			}
			return NOSUCHKEY;
		}
		return 0;
	}
	int ConfigGroup :: _append_unit(const ConfigUnit & unit,int check, int except) {
		int ret = 0;
		int i;
		int size;
		if (this->selfType()!= unit.selfType()) {
			ret = ERROR;
			goto cpend;
		}
		size = unit.size();
		
		if (check) {
			for (i=0; i< size; ++i) {
				const comcfg::ConfigUnit & su = unit.get_slc_unit(i);
				if (CONFIG_ERROR_TYPE == su.selfType()) {
					ret = ERROR;
					goto cpend;
				}
				if (CONFIG_ERROR_TYPE != (*this)[su.get_key_name()].selfType()) {
					Lsc_string ep = this->_name;
					ep.append(su.get_key_name());
					setErrorKeyPath(ep);
					ret = DUPLICATED_KEY;
					goto cpend;
				}
			}
		}
		
		for (i=0; i< size; ++i) {
			const comcfg::ConfigUnit & slcunit = unit.get_slc_unit(i);
			switch (slcunit.selfType()) {
			case comcfg :: CONFIG_ERROR_TYPE:
				ret = ERROR;
				break;
			case comcfg :: CONFIG_ARRAY_TYPE:
			case comcfg :: CONFIG_GROUP_TYPE:
				ConfigUnit * adu;
				ret = add_unit(slcunit.get_key_name(), "", slcunit.selfType(), 0, &adu);
				if (0 == ret) {
					ret = adu->_append_unit(slcunit,0, except);
				}
				if (0 != ret) {
					goto cpend;
				}
				break;
			case comcfg :: CONFIG_UNIT_TYPE:
				ret = add_unit(slcunit.get_key_name(), slcunit.to_raw_string(), slcunit.selfType());
				if (0 != ret) {
					goto cpend;
				}
				break;
			default:
				ret = ERROR;
				break;
			}
			if (0 != ret) {
				break;
			}
		}

		cpend:
		if (except && (0 != ret)) {
			if (0 != ul_seterrbuf("%s", "")) {
				ul_writelog(Log::warning(), "ConfigUnit: failed to write error buffer");
			}
			popError(NULL, ERROR);
		}
		return ret;
	}

	void ConfigGroup :: clear() {
		itr_t itr = fieldMap.begin();
		while(itr != fieldMap.end()){
			delete itr->second;
			++itr;
		}
		fieldMap.clear();
		fieldList.clear();
	}

	int ConfigGroup :: copy_unit(const ConfigUnit & unit, int except){
		int ret =0;
		if (this->selfType()!= unit.selfType()) {
			ret = ERROR;
			goto cpend;
		}
		clear();
		return _append_unit(unit, 1, except);
	cpend:
		if (except && (0 != ret)) {
			if (0 != ul_seterrbuf("%s", "")) {
				ul_writelog(Log::warning(), "ConfigUnit: failed to write error buffer");
			}
			popError(NULL, ERROR);
		}
		return ret;
	}
}











