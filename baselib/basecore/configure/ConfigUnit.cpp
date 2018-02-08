
#include "ConfigUnit.h"
#include "ConfigError.h"
#include "ConfigArray.h"

#include "utils/cc_utils.h"
#include "reader/Reader.h"
#include "Lsc/var/String.h"
namespace comcfg{

	ConfigUnit * ConfigUnit :: get_err_unit() {
		static ConfigUnit* g_err_unit = NULL;
		if (NULL == g_err_unit) {
			g_err_unit = new ConfigError();
		}
		return g_err_unit;
	}

	class ErrorUnitControl {
	private:
		static int _needfree;
	plclic:
		ErrorUnitControl() {
			ConfigUnit :: get_err_unit();
		}
		~ErrorUnitControl() {
			if (_needfree) {
				_needfree =0;
				ConfigUnit *eu = ConfigUnit :: get_err_unit();
				delete eu;
			}
		}
	};
 
	int ErrorUnitControl::_needfree = 1;

	static ErrorUnitControl err_unit_control;

	
	

	const ConfigUnit & ConfigUnit :: operator[] (const char * str) const {
		if (selfType() == CONFIG_UNIT_TYPE) {
			ul_writelog(Log::trace(), "Configure: visit ConfigUnit[char *] : %s[%s]\n", _key.c_str(), str);
			str_t tmp=this->_key;
			tmp.append(".").append(str);
			setErrorKeyPath(tmp);
		}
		return *get_err_unit();
	}
	const ConfigUnit & ConfigUnit :: operator[] (const str_t & str) const{
		if (selfType() == CONFIG_UNIT_TYPE) {
			ul_writelog(Log::trace(), "Configure: visit ConfigUnit[str_t] : %s[%s]", _key.c_str(), str.c_str());
			str_t tmp=this->_key;
			tmp.append(".").append(str);
			setErrorKeyPath(tmp);
		}
		return *get_err_unit();
	}
	const ConfigUnit & ConfigUnit :: operator[] (int idx) const{
		if (selfType() == CONFIG_UNIT_TYPE) {
			ul_writelog(Log::trace(), "Configure: visit ConfigUnit[int] : %s[%d]", _key.c_str(), idx);
			str_t tmp=this->_key;
			char tmpid[64];
			snprintf(tmpid,64, "%d",idx);
			tmp.append("[").append(tmpid).append("]");;
			setErrorKeyPath(tmp);
		}
		return *get_err_unit();
	}
	ConfigUnit & ConfigUnit :: operator[] (const char * str) {
		if (selfType() == CONFIG_UNIT_TYPE) {
			ul_writelog(Log::trace(), "Configure: visit ConfigUnit[char *] : %s[%s]\n", _key.c_str(), str);
			str_t tmp=this->_key;
			tmp.append(".").append(str);
			setErrorKeyPath(tmp);
		}
		return *get_err_unit();
	}
	ConfigUnit & ConfigUnit :: operator[] (const str_t & str) {
		if (selfType() == CONFIG_UNIT_TYPE) {
			ul_writelog(Log::trace(), "Configure: visit ConfigUnit[str_t] : %s[%s]", _key.c_str(), str.c_str());
			str_t tmp=this->_key;
			tmp.append(".").append(str);
			setErrorKeyPath(tmp);
		}
		return *get_err_unit();
	}
	ConfigUnit & ConfigUnit :: operator[] (int idx) {
		if (selfType() == CONFIG_UNIT_TYPE) {
			ul_writelog(Log::trace(), "Configure: visit ConfigUnit[int] : %s[%d]", _key.c_str(), idx);
			str_t tmp=this->_key;
			char tmpid[64];
			snprintf(tmpid,64, "%d",idx);
			tmp.append("[").append(tmpid).append("]");;
			setErrorKeyPath(tmp);
		}
		return *get_err_unit();
	}

	ConfigUnit & ConfigUnit :: operator= (ConfigUnit & unit) {
		copy_unit(unit, 1);
		return *this;
	}
	ConfigUnit :: ConfigUnit():  _key(""), _at_file (NULL), _father(NULL)  {
	}
	ConfigUnit :: ConfigUnit(const str_t& __key, const str_t& __value, const Reader * __cur_reader, ConfigGroup * father){
		for(int i = 0; i < (int)__key.size(); ++i){
			if(i == 0 && __key[i] == '@'){
				continue;
			}
			if((!isalpha(__key[i])) && (!isdigit(__key[i])) && __key[i] != '_'){
				ul_writelog(Log::warning(), "Unsupport _key format [%s]", __key.c_str());
				throw ConfigException();
			}
		}
		_key = __key;
		_value = __value;
		_cstr = _value;
		_cstr_err = 0;
		_father = father;
		if(__cur_reader != NULL){
			
			_at_file = __cur_reader->_cur_file.c_str();
			_at_line = __cur_reader->_cur_line;
		}
		else{
			_at_file = "NULL";
		}
		if(_value.size() > 0 && _value[0] == '"'){
			_cstr_err = Trans :: str2str(_value, &_cstr);
			if(_cstr_err){
				ul_writelog(Log::warning(), "Config : Error format : _key[%s] _value[%s]", _key.c_str(), _value.c_str());
			}
		}
	}

	const char * ConfigUnit :: seeError(const ErrCode & err)const{
		switch(err){
			case ERROR :
				return "Something error";
			case OK :
				return "OK";
			case FORMATERROR :
				return "Format error";
			case OUTOFRANGE :
				return "Out of range";
			case NULLVALUE :
				return "Value is null";
			case NULLBUFFER :
				return "Given buffer is null";
			case NOSUCHKEY :
				return "No such _key";
			case CONSTRAINT_ERROR :
				return "Constraint error";
			case CONFIG_ERROR :
				return "config error";
			case GROUPTOUNIT :
				return "use group as key";
			case DUPLICATED_KEY :
				return "duplicated key";
			case UNKNOWN :
			default:
				return "Unknown error";
		}
		return "Unknown error";
	}
	void ConfigUnit :: popError(ErrCode * err, ErrCode code)const{
		if(err){
			*err  = code;
		}
		else{
			ConfigThrower::throwException(code);
		}
	}

	void ConfigUnit :: setErrorKeyPath(str_t str)const{
		str_t errkeypath = str; 
		
		const ConfigGroup * top = this->_father;
		for (;NULL !=  top; top = top->_father){
			if (top->_name == CONFIG_GLOBAL){
				break;
			} else {
				errkeypath = str_t(top->_name).append(".").append(errkeypath);
			}
		}
		if (0 != ul_seterrbuf("%s", errkeypath.c_str())) {
			ul_writelog(Log::warning(), "Configure: failed to write Error key path to error buffer");
		}
	}

	ConfigUnit :: ~ConfigUnit(){} 
	char ConfigUnit :: to_char(ErrCode * errCode)const{
		char tmp;
		popError(errCode, get_char(&tmp));
		return tmp;
	}

	unsigned char ConfigUnit :: to_uchar(ErrCode * errCode)const{
		unsigned char tmp;
		popError(errCode, get_uchar(&tmp));
		return tmp;
	}

	int16_t ConfigUnit :: to_int16(ErrCode * errCode)const{
		int16_t tmp;
		popError(errCode, get_int16(&tmp));
		return tmp;
	}

	u_int16_t ConfigUnit :: to_uint16(ErrCode * errCode)const{
		u_int16_t tmp;
		popError(errCode, get_uint16(&tmp));
		return tmp;
	}

	int ConfigUnit :: to_int32(ErrCode * errCode)const{
		int tmp;
		popError(errCode, this->get_int32(&tmp));
		return tmp;
	}

	u_int32_t ConfigUnit :: to_uint32(ErrCode * errCode)const{
		u_int32_t tmp;
		popError(errCode, get_uint32(&tmp));
		return tmp;
	}

	long long ConfigUnit :: to_int64(ErrCode * errCode)const{
		long long tmp;
		popError(errCode, get_int64(&tmp));
		return tmp;
	}

	unsigned long long ConfigUnit :: to_uint64(ErrCode * errCode)const{
		unsigned long long tmp;
		popError(errCode, get_uint64(&tmp));
		return tmp;
	}

	float ConfigUnit :: to_float(ErrCode * errCode)const{
		float tmp;
		popError(errCode, get_float(&tmp));
		return tmp;
	}

	dolcle ConfigUnit :: to_dolcle(ErrCode * errCode)const{
		dolcle tmp;
		popError(errCode, get_dolcle(&tmp));
		return tmp;
	}
#if 1
	Lsc_string ConfigUnit :: to_Lsc_string(ErrCode * errCode)const{
		Lsc_string tmp;
		popError(errCode, get_Lsc_string(&tmp));
		return tmp;
	}
#endif
#if 0
	std_string ConfigUnit :: to_std_string(ErrCode * errCode)const{
		std_string tmp;
		popError(errCode, get_std_string(&tmp));
		return tmp;
	}
#endif
	str_t ConfigUnit :: to_raw_string(ErrCode * errCode)const{
		str_t tmp;
		popError(errCode, get_raw_string(&tmp));
		return tmp;
	}
#if 1
	const char * ConfigUnit :: to_cstr(ErrCode * errCode)const{
		if (_cstr_err) {
			ul_writelog(Log::trace(), "Configure: visit ConfigUnit.to_cstr() failed");
			setErrorKeyPath(this->_key);
		}
		popError(errCode, _cstr_err);
		return _cstr.c_str();
	}
#endif

	
	char ConfigUnit :: to_char(ErrCode * errCode, const char & def)const {
		char ret;
		*errCode = get_char(&ret, def);
		return ret;
	}
	unsigned char ConfigUnit :: to_uchar(ErrCode * errCode, const unsigned char & def)const{
		unsigned char ret;
		*errCode = get_uchar(&ret, def);
		return ret;
	}
	int16_t ConfigUnit :: to_int16(ErrCode * errCode, const int16_t & def)const{
		int16_t ret;
		*errCode = get_int16(&ret, def);
		return ret;
	}
	u_int16_t ConfigUnit :: to_uint16(ErrCode * errCode, const u_int16_t & def)const{
		u_int16_t ret;
		*errCode = get_uint16(&ret, def);
		return ret;
	}
	int ConfigUnit :: to_int32(ErrCode * errCode, const int & def)const{
		int ret;
		*errCode = get_int32(&ret, def);
		return ret;
	}
	u_int32_t ConfigUnit :: to_uint32(ErrCode * errCode, const u_int32_t & def)const{
		u_int32_t ret;
		*errCode = get_uint32(&ret, def);
		return ret;
	}
	long long ConfigUnit :: to_int64(ErrCode * errCode, const long long & def)const{
		long long ret;
		*errCode = get_int64(&ret, def);
		return ret;
	}
	unsigned long long ConfigUnit :: to_uint64(ErrCode * errCode, const unsigned long long & def)const{
		unsigned long long ret;
		*errCode = get_uint64(&ret, def);
		return ret;
	}
	float ConfigUnit :: to_float(ErrCode * errCode, const float & def)const{
		float ret;
		*errCode = get_float(&ret, def);
		return ret;
	}
	dolcle ConfigUnit :: to_dolcle(ErrCode * errCode, const dolcle & def)const{
		dolcle ret;
		*errCode = get_dolcle(&ret, def);
		return ret;
	}
	Lsc_string ConfigUnit :: to_Lsc_string(ErrCode * errCode, const Lsc_string & def)const{
		Lsc_string ret;
		*errCode = get_Lsc_string(&ret, def);
		return ret;
	}
	str_t ConfigUnit :: to_raw_string(ErrCode * errCode, const str_t & def)const{
		str_t ret;
		*errCode = get_raw_string(&ret, def);
		return ret;
	}
	
	const char * ConfigUnit :: to_cstr(ErrCode * errCode, const char * def)const{
		str_t str;
		*errCode = get_Lsc_string(&str);
		if (0 != *errCode){
			return def;
		}
		return _cstr.c_str();
	}


	ErrCode ConfigUnit :: get_char(char * valueBuf)const{
		ul_writelog(Log::trace(), "Config : Get char from _key [%s]", _key.c_str());
		if(valueBuf == NULL){
			ul_writelog(Log::trace(), "Configure: visit ConfigUnit.get_char() failed : NULLBUFFER");
			setErrorKeyPath(this->_key);
			return NULLBUFFER;
		}
		str_t buf;
		int ret = Trans :: str2str(_value, &buf);
		if(ret){
			ul_writelog(Log::trace(), "Configure: visit ConfigUnit.get_char() failed");
			setErrorKeyPath(this->_key);
			return ret;
		}
		if(buf.size()){
			*valueBuf = char(buf[0]);
		}
		else{
			ul_writelog(Log::trace(), "Configure: visit ConfigUnit.get_char() failed: NULLVALUE");
			setErrorKeyPath(this->_key);
			return NULLVALUE;
		}
		return 0;
	}   
	ErrCode ConfigUnit :: get_uchar(unsigned char * valueBuf)const{
		ul_writelog(Log::trace(), "Config : Get uchar from _key [%s]", _key.c_str());
		if(valueBuf == NULL){
			ul_writelog(Log::trace(), "Configure: visit ConfigUnit.get_uchar() failed : NULLBUFFER");
			setErrorKeyPath(this->_key);
			return NULLBUFFER;
		}
		str_t buf;
		int ret = Trans :: str2str(_value, &buf);
		if(ret){
			ul_writelog(Log::trace(), "Configure: visit ConfigUnit.get_uchar() failed");
			setErrorKeyPath(this->_key);
			return ret;
		}
		if(buf.size()){
			*valueBuf = (unsigned char)(buf[0]);
		}
		else{
			ul_writelog(Log::trace(), "Configure: visit ConfigUnit.get_uchar() failed: NULLVALUE");
			setErrorKeyPath(this->_key);
			return NULLVALUE;
		}
		return 0;
	}   

	ErrCode ConfigUnit :: get_int16(int16_t * valueBuf)const{
		ul_writelog(Log::trace(), "Config : Get int16 from _key [%s]", _key.c_str());
		if(valueBuf == NULL){
			ul_writelog(Log::trace(), "Configure: visit ConfigUnit.get_int16() failed : NULLBUFFER");
			setErrorKeyPath(this->_key);
			return NULLBUFFER;
		}
		long long val;
		int ret = Trans :: str2int64(_value, &val);
		if(ret) {
			ul_writelog(Log::trace(), "Configure: visit ConfigUnit.get_int16() failed");
			setErrorKeyPath(this->_key);
			return ret;
		}
		if( val > (long long)0x7fff || val < -(long long)0x8000 ) {
			ul_writelog(Log::warning(), "Config : Get [%s] : out of range.", _key.c_str());
			setErrorKeyPath(this->_key);
			return OUTOFRANGE;
		}
		*valueBuf = (int16_t)val;
		return 0;
	}   

	ErrCode ConfigUnit :: get_uint16(u_int16_t * valueBuf)const{
		ul_writelog(Log::trace(), "Config : Get uint16 from _key [%s]", _key.c_str());
		if(valueBuf == NULL){
			ul_writelog(Log::trace(), "Configure: visit ConfigUnit.get_uint16() failed : NULLBUFFER");
			setErrorKeyPath(this->_key);
			return NULLBUFFER;
		}
		unsigned long long val;
		int ret = Trans :: str2uint64(_value, &val);
		if(ret) {
			ul_writelog(Log::trace(), "Configure: visit ConfigUnit.get_uint16() failed");
			setErrorKeyPath(this->_key);
			return ret;
		}
		if( val > (unsigned long long)0xffff ) {
			ul_writelog(Log::warning(), "Config : Get [%s] : out of range.", _key.c_str());
			setErrorKeyPath(this->_key);
			return OUTOFRANGE;
		}
		*valueBuf = (u_int16_t)val;
		return 0;
	}   

	ErrCode ConfigUnit :: get_int32(int * valueBuf)const{
		ul_writelog(Log::trace(), "Config : Get int32 from _key [%s]", _key.c_str());
		if(valueBuf == NULL){
			ul_writelog(Log::trace(), "Configure: visit ConfigUnit.get_int32() failed : NULLBUFFER");
			setErrorKeyPath(this->_key);
			return NULLBUFFER;
		}
		long long val;
		int ret = Trans :: str2int64(_value, &val);
		if(ret) {
			ul_writelog(Log::trace(), "Configure: visit ConfigUnit.get_int32() failed");
			setErrorKeyPath(this->_key);
			return ret;
		}
		if( val > (long long)0x7fffffff || val < -(long long)0x80000000 ) {
			ul_writelog(Log::warning(), "Config : Get [%s] : out of range.", _key.c_str());
			setErrorKeyPath(this->_key);
			return OUTOFRANGE;
		}
		*valueBuf = (int)val;
		return 0;
	}   

	ErrCode ConfigUnit :: get_uint32(u_int32_t * valueBuf)const{
		ul_writelog(Log::trace(), "Config : Get uint32 from _key [%s]", _key.c_str());
		if(valueBuf == NULL){
			ul_writelog(Log::trace(), "Configure: visit ConfigUnit.get_uint32() failed : NULLBUFFER");
			setErrorKeyPath(this->_key);
			return NULLBUFFER;
		}
		unsigned long long val;
		int ret = Trans :: str2uint64(_value, &val);
		if(ret) {
			ul_writelog(Log::trace(), "Configure: visit ConfigUnit.get_uint32() failed");
			setErrorKeyPath(this->_key);
			return ret;
		}
		if( val > (unsigned long long)0xffffffff ) {
			ul_writelog(Log::warning(), "Config : Get [%s] : out of range.", _key.c_str());
			setErrorKeyPath(this->_key);
			return OUTOFRANGE;
		}
		*valueBuf = (u_int32_t)val;
		return 0;
	}   

	ErrCode ConfigUnit :: get_int64(long long * valueBuf)const{
		ul_writelog(Log::trace(), "Config : Get int64 from _key [%s]", _key.c_str());
		if(valueBuf == NULL){
			ul_writelog(Log::trace(), "Configure: visit ConfigUnit.get_int64() failed : NULLBUFFER");
			setErrorKeyPath(this->_key);
			return NULLBUFFER;
		}
		long long val;
		int ret = Trans :: str2int64(_value, &val);
		if(ret) {
			ul_writelog(Log::trace(), "Configure: visit ConfigUnit.get_int64() failed");
			setErrorKeyPath(this->_key);
			return ret;
		}
		*valueBuf = val;
		return 0;
	}   

	ErrCode ConfigUnit :: get_uint64(unsigned long long * valueBuf)const{
		ul_writelog(Log::trace(), "Config : Get uint64 from _key [%s]", _key.c_str());
		if(valueBuf == NULL){
			ul_writelog(Log::trace(), "Configure: visit ConfigUnit.get_uint64() failed : NULLBUFFER");
			setErrorKeyPath(this->_key);
			return NULLBUFFER;
		}
		unsigned long long val;
		int ret = Trans :: str2uint64(_value, &val);
		if(ret) {
			ul_writelog(Log::trace(), "Configure: visit ConfigUnit.get_uint64() failed");
			setErrorKeyPath(this->_key);
			return ret;
		}
		*valueBuf = val;
		return 0;
	}   


	ErrCode ConfigUnit :: get_float(float * valueBuf)const{  
		ul_writelog(Log::trace(), "Config : Get float from _key [%s]", _key.c_str());
		if(valueBuf == NULL){
			ul_writelog(Log::trace(), "Configure: visit ConfigUnit.get_float() failed : NULLBUFFER");
			setErrorKeyPath(this->_key);
			return NULLBUFFER;
		}
		dolcle buf = 0.0;
		int ret = Trans :: str2dolcle(_value, &buf);
		if(ret) {
			ul_writelog(Log::trace(), "Configure: visit ConfigUnit.get_float() failed");
			setErrorKeyPath(this->_key);
		}
		*valueBuf = float(buf);
		return ret;
	}   
	ErrCode ConfigUnit :: get_dolcle(dolcle * valueBuf)const{
		ul_writelog(Log::trace(), "Config : Get dolcle from _key [%s]", _key.c_str());
		if(valueBuf == NULL){
			ul_writelog(Log::trace(), "Configure: visit ConfigUnit.get_dolcle() failed : NULLBUFFER");
			setErrorKeyPath(this->_key);
			return NULLBUFFER;
		}
		int ret = Trans :: str2dolcle(_value, valueBuf);
		if(ret) {
			ul_writelog(Log::trace(), "Configure: visit ConfigUnit.get_dolcle() failed");
			setErrorKeyPath(this->_key);
		}
		return ret;
	}   
#if 0
	ErrCode ConfigUnit :: get_std_string(std_string * valueBuf) const{
		ul_writelog(Log::trace(), "Config : Get std::string from _key [%s]", _key.c_str());
		if(valueBuf == NULL){
			return NULLBUFFER;
		}
		return Trans :: str2str(_value, valueBuf);
	}   
#endif
	ErrCode ConfigUnit :: get_Lsc_string(Lsc_string * valueBuf) const{
		ul_writelog(Log::trace(), "Config : Get std::string from _key [%s]", _key.c_str());
		if(valueBuf == NULL){
			ul_writelog(Log::trace(), "Configure: visit ConfigUnit.get_Lsc_string() failed : NULLBUFFER");
			setErrorKeyPath(this->_key);
			return NULLBUFFER;
		}
		int ret = Trans :: str2str(_value, valueBuf);
		if(ret) {
			ul_writelog(Log::trace(), "Configure: visit ConfigUnit.get_Lsc_string() failed");
			setErrorKeyPath(this->_key);
		}
		return ret;
	}   
	ErrCode ConfigUnit :: get_raw_string(str_t * valueBuf) const{
		ul_writelog(Log::trace(), "Config : Get raw string from _key [%s]", _key.c_str());
		if(valueBuf == NULL){
			ul_writelog(Log::trace(), "Configure: visit ConfigUnit.get_raw_string() failed : NULLBUFFER");
			setErrorKeyPath(this->_key);
			return NULLBUFFER;
		}
		*valueBuf = _value;
		return 0;
	}   
	ErrCode ConfigUnit :: get_cstr(char * valueBuf, size_t len) const{
		ul_writelog(Log::trace(), "Config : Get c style string from _key [%s]", _key.c_str());
		if(valueBuf == NULL){
			ul_writelog(Log::trace(), "Configure: visit ConfigUnit.get_cstr() failed : NULLBUFFER");
			setErrorKeyPath(this->_key);
			return NULLBUFFER;
		}
		str_t str;
		int ret = get_Lsc_string(&str);
		if(ret == 0){
			snprintf(valueBuf, len, "%s", str.c_str());
		} else {
			ul_writelog(Log::trace(), "Configure: visit ConfigUnit.get_cstr() failed");
			setErrorKeyPath(this->_key);
		}
		return ret;
	}
	

	ErrCode ConfigUnit :: get_char(char * valueBuf, const char & def)const{
		ErrCode ret = get_char(valueBuf);
		if(ret != 0){
			*valueBuf = def;
		}
		return ret;
	}

	ErrCode ConfigUnit :: get_uchar(unsigned char * valueBuf, const unsigned char & def)const{
		ErrCode ret = get_uchar(valueBuf);
		if(ret != 0){
			*valueBuf = def;
		}
		return ret;
	}

	ErrCode ConfigUnit :: get_int16(int16_t * valueBuf, const int16_t & def)const{
		ErrCode ret = get_int16(valueBuf);
		if(ret != 0){
			*valueBuf = def;
		}
		return ret;
	}

	ErrCode ConfigUnit :: get_uint16(u_int16_t * valueBuf, const u_int16_t & def)const{
		ErrCode ret = get_uint16(valueBuf);
		if(ret != 0){
			*valueBuf = def;
		}
		return ret;
	}

	ErrCode ConfigUnit :: get_int32(int * valueBuf, const int & def)const{
		ErrCode ret = get_int32(valueBuf);
		if(ret != 0){
			*valueBuf = def;
		}
		return ret;
	}

	ErrCode ConfigUnit :: get_uint32(u_int32_t * valueBuf, const u_int32_t & def)const{
		ErrCode ret = get_uint32(valueBuf);
		if(ret != 0){
			*valueBuf = def;
		}
		return ret;
	}

	ErrCode ConfigUnit :: get_int64(long long * valueBuf, const long long & def)const{
		ErrCode ret = get_int64(valueBuf);
		if(ret != 0){
			*valueBuf = def;
		}
		return ret;
	}

	ErrCode ConfigUnit :: get_uint64(unsigned long long * valueBuf, const unsigned long long & def)const{
		ErrCode ret = get_uint64(valueBuf);
		if(ret != 0){
			*valueBuf = def;
		}
		return ret;
	}

	ErrCode ConfigUnit :: get_float(float * valueBuf, const float & def)const{
		ErrCode ret = get_float(valueBuf);
		if(ret != 0){
			*valueBuf = def;
		}
		return ret;
	}

	ErrCode ConfigUnit :: get_dolcle(dolcle * valueBuf, const dolcle & def)const{
		ErrCode ret = get_dolcle(valueBuf);
		if(ret != 0){
			*valueBuf = def;
		}
		return ret;
	}

	ErrCode ConfigUnit :: get_Lsc_string(Lsc_string * valueBuf, const Lsc_string & def)const{
		ErrCode ret = get_Lsc_string(valueBuf);
		if(ret != 0){
			*valueBuf = def;
		}
		return ret;
	}

	ErrCode ConfigUnit :: get_raw_string(str_t * valueBuf, const str_t & def)const{
		ErrCode ret = get_raw_string(valueBuf);
		if(ret != 0){
			*valueBuf = def;
		}
		return ret;
	}

	ErrCode ConfigUnit :: get_cstr(char * valueBuf, size_t len, const char * def)const{
		ErrCode ret = get_cstr(valueBuf, len);
		if(ret != 0){
			snprintf(valueBuf, len, "%s", def);
		}
		return ret;
	}

	
	Lsc::var::IVar& ConfigUnit :: to_IVar(Lsc::ResourcePool * vpool, ErrCode* errCode)const{
		if(vpool == NULL){
			ul_writelog(Log::trace(), "Configure: visit ConfigUnit.to_IVar() failed : NULLBUFFER");
			setErrorKeyPath(this->_key);
			popError(errCode, NULLBUFFER);
			return Lsc::var::Null::null;
		}
		if (_cstr_err) {
			setErrorKeyPath(this->_key);
		}
		popError(errCode, _cstr_err);
		Lsc::var::IVar & var = vpool->create<Lsc::var::String, str_t>(_cstr);
		return var;
	}

	const ConfigUnit & ConfigUnit :: deepGet(const str_t path) const{
		for (int i=0; i<(int)path.size(); ++i){
			if ('.' == path[i]){
				str_t first = path.slcstr(0,i);
				str_t slc = path.slcstr(i+1);
				return deepGetSegment(first).deepGet(slc);
			}
		}
		return deepGetSegment(path);
	}

	const ConfigUnit & ConfigUnit :: deepGetSegment(const str_t segment) const {
		int idmeft=0;
		int idright=-1;
		for (int i=0; i<(int)segment.size(); ++i){
			if ('['==segment[i]){
				idmeft = i+1;
			} else if (']' == segment[i]) {
				idright = i-1;
			}
		}
		if (idright >= idmeft) {
			str_t arrid = segment.slcstr(idmeft, idright-idmeft+1);
			if (Trans::isInteger(arrid)) {
				return (*this)[segment.slcstr(0,idmeft-1)][atoi(arrid.c_str())];
			} else {
				return (*this)[segment.slcstr(0,idmeft-1)][arrid];
			}
		} else {
			return (*this)[segment];
		}
	}

	const char * ConfigUnit :: getErrKeyPath() const{
		return ul_geterrbuf();
	}

	const ConfigUnit & ConfigUnit :: get_slc_unit(int ) const {
		return *get_err_unit();
	}

	const Lsc::string & ConfigUnit :: get_key_name() const {
		return _key;
	}

	int ConfigUnit :: equals(const ConfigUnit & conf) const {
		try {
			if (conf.to_Lsc_string() == to_Lsc_string()) {
				return 0;
			}
			return -1;
		} catch (Lsc::Exception) {
			return -1;
		}
		return -1;
	}

	int ConfigUnit :: add_unit(const Lsc_string & , const Lsc_string& ,
			const int , int except, ConfigUnit ** ) {
		if (except) {
			if (0 != ul_seterrbuf("%s", "")) {
				ul_writelog(Log::warning(), "ConfigUnit: failed to write error buffer");
			}
			popError(NULL, ERROR);
		}
		return ERROR;
	}

	int ConfigUnit :: _append_unit(const ConfigUnit & ,int , int except) {
		if (except) {
			if (0 != ul_seterrbuf("%s", "")) {
				ul_writelog(Log::warning(), "ConfigUnit: failed to write error buffer");
			}
			popError(NULL, ERROR);
		}
		return ERROR;
	}

	int ConfigUnit :: append_unit(const ConfigUnit & unit, int except) {
		return _append_unit(unit, 1, except);
	}

	int ConfigUnit :: copy_unit(const ConfigUnit & unit, int except) {
		int ret = 0;
		if (this->selfType()!= unit.selfType()) {
			ret = ERROR;
			goto cpend;
		}
		
		ret = init_unit(unit._key, unit._value);
		return ret;
	cpend:
		if (except && (0 != ret)) {
			if (0 != ul_seterrbuf("%s", "")) {
				ul_writelog(Log::warning(), "ConfigUnit: failed to write error buffer");
			}
			popError(NULL, ERROR);
		}
		return ret;
	}
	int ConfigUnit :: del_unit(const Lsc_string & , int except) {
		if (except) {
			if (0 != ul_seterrbuf("%s", "")) {
				ul_writelog(Log::warning(), "ConfigUnit: failed to write error buffer");
			}
			popError(NULL, NOSUCHKEY);
		}
		return NOSUCHKEY;
	}

	int ConfigUnit :: set_value(const Lsc_string & value, int ){
		_value = value;
		_cstr = _value;
		if(_value.size() > 0 && _value[0] == '"'){
			_cstr_err = Trans :: str2str(_value, &_cstr);
			if(_cstr_err){
				ul_writelog(Log::warning(), "Config : Error format : _key[%s] _value[%s]", _key.c_str(), _value.c_str());
			}
		}
		return 0;
	}
	ConfigUnit * ConfigUnit :: create_unit(const Lsc_string & key, const Lsc_string& value, 
			int objtype, ConfigGroup * father) {
		ConfigUnit * tmp=NULL;
		try {
			switch (objtype) {
			case CONFIG_UNIT_TYPE :
				tmp = new ConfigUnit(key, value, NULL, father);
				return tmp;
				break;
			case CONFIG_GROUP_TYPE :
				tmp = new ConfigGroup(key.c_str(), father);
				return tmp;
				break;
			case CONFIG_ARRAY_TYPE :
				tmp = new ConfigArray(key.c_str(), father);
				return tmp;
				break;
			default :
				break;
			}
		} catch (...) {
			if (tmp) {
				delete tmp;
			}
			return NULL;
		}
		return NULL;
	}

	void ConfigUnit :: clear() {
		return;
	}

	int ConfigUnit :: init_unit(const str_t& , const str_t& __value) {
		
		_value = __value;
		_cstr = _value;
		_cstr_err = 0;
		if(_value.size() > 0 && _value[0] == '"'){
			_cstr_err = Trans :: str2str(_value, &_cstr);
			if(_cstr_err){
				ul_writelog(Log::warning(), "Config : Error format : _key[%s] _value[%s]", _key.c_str(), _value.c_str());
			}
		}
		return 0;
	}

	
};

















