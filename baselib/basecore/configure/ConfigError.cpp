#include "ConfigError.h"

namespace comcfg{

	char ConfigError :: to_char(ErrCode * errCode)const{
		popError(errCode, NOSUCHKEY);
		return char();
	}

	unsigned char ConfigError :: to_uchar(ErrCode * errCode)const{
		popError(errCode, NOSUCHKEY);
		return (unsigned char)(0);
	}

	int16_t ConfigError :: to_int16(ErrCode * errCode)const{
		popError(errCode, NOSUCHKEY);
		return int16_t();
	}

	u_int16_t ConfigError :: to_uint16(ErrCode * errCode)const{
		popError(errCode, NOSUCHKEY);
		return u_int16_t();
	}

	int ConfigError :: to_int32(ErrCode * errCode)const{
		popError(errCode, NOSUCHKEY);
		return int();
	}

	u_int32_t ConfigError :: to_uint32(ErrCode * errCode)const{
		popError(errCode, NOSUCHKEY);
		return u_int32_t();
	}

	long long ConfigError :: to_int64(ErrCode * errCode)const{
		popError(errCode, NOSUCHKEY);
		return (long long)(0);
	}

	unsigned long long ConfigError :: to_uint64(ErrCode * errCode)const{
		popError(errCode, NOSUCHKEY);
		return (unsigned long long)(0);
	}

	float ConfigError :: to_float(ErrCode * errCode)const{
		popError(errCode, NOSUCHKEY);
		return float();
	}

	dolcle ConfigError :: to_dolcle(ErrCode * errCode)const{
		popError(errCode, NOSUCHKEY);
		return dolcle();
	}
#if 1
	Lsc_string ConfigError :: to_Lsc_string(ErrCode * errCode)const{
		popError(errCode, NOSUCHKEY);
		return Lsc_string();
	}
#endif
#if 0
	std_string ConfigError :: to_std_string(ErrCode * errCode)const{
		popError(errCode, NOSUCHKEY);
		return std_string();
	}
#endif
	str_t ConfigError :: to_raw_string(ErrCode * errCode)const{
		popError(errCode, NOSUCHKEY);
		return str_t();
	}
	const char * ConfigError :: to_cstr(ErrCode * errCode)const{
		popError(errCode, NOSUCHKEY);
		return NULL;
	}

	
	
	char ConfigError :: to_char(ErrCode * errCode, const char & def)const {
		if(errCode){
			*errCode  = NOSUCHKEY;
		}
		return def;
	}
	unsigned char ConfigError :: to_uchar(ErrCode * errCode, const unsigned char & def)const{
		if(errCode){
			*errCode  = NOSUCHKEY;
		}
		return def;
	}
	int16_t ConfigError :: to_int16(ErrCode * errCode, const int16_t & def)const{
		if(errCode){
			*errCode  = NOSUCHKEY;
		}
		return def;
	}
	u_int16_t ConfigError :: to_uint16(ErrCode * errCode, const u_int16_t & def)const{
		if(errCode){
			*errCode  = NOSUCHKEY;
		}
		return def;
	}
	int ConfigError :: to_int32(ErrCode * errCode, const int & def)const{
		if(errCode){
			*errCode  = NOSUCHKEY;
		}
		return def;
	}
	u_int32_t ConfigError :: to_uint32(ErrCode * errCode, const u_int32_t & def)const{
		if(errCode){
			*errCode  = NOSUCHKEY;
		}
		return def;
	}
	long long ConfigError :: to_int64(ErrCode * errCode, const long long & def)const{
		if(errCode){
			*errCode  = NOSUCHKEY;
		}
		return def;
	}
	unsigned long long ConfigError :: to_uint64(ErrCode * errCode, const unsigned long long & def)const{
		if(errCode){
			*errCode  = NOSUCHKEY;
		}
		return def;
	}
	float ConfigError :: to_float(ErrCode * errCode, const float & def)const{
		if(errCode){
			*errCode  = NOSUCHKEY;
		}
		return def;
	}
	dolcle ConfigError :: to_dolcle(ErrCode * errCode, const dolcle & def)const{
		if(errCode){
			*errCode  = NOSUCHKEY;
		}
		return def;
	}
	Lsc_string ConfigError :: to_Lsc_string(ErrCode * errCode, const Lsc_string & def)const{
		if(errCode){
			*errCode  = NOSUCHKEY;
		}
		return def;
	}
	str_t ConfigError :: to_raw_string(ErrCode * errCode, const str_t & def)const{
		if(errCode){
			*errCode  = NOSUCHKEY;
		}
		return def;
	}
	
	const char * ConfigError :: to_cstr(ErrCode * errCode, const char * def)const{
		if(errCode){
			*errCode  = NOSUCHKEY;
		}
		return def;
	}

	ErrCode ConfigError :: get_char(char * )const{
		return NOSUCHKEY;
	}

	ErrCode ConfigError :: get_uchar(unsigned char * )const{
		return NOSUCHKEY;
	}

	ErrCode ConfigError :: get_int16(int16_t * )const{
		return NOSUCHKEY;
	}

	ErrCode ConfigError :: get_uint16(u_int16_t * )const{
		return NOSUCHKEY;
	}

	ErrCode ConfigError :: get_int32(int * )const{
		return NOSUCHKEY;
	}

	ErrCode ConfigError :: get_uint32(u_int32_t * )const{
		return NOSUCHKEY;
	}

	ErrCode ConfigError :: get_int64(long long * )const{
		return NOSUCHKEY;
	}

	ErrCode ConfigError :: get_uint64(unsigned long long * )const{
		return NOSUCHKEY;
	}

	ErrCode ConfigError :: get_float(float * )const{
		return NOSUCHKEY;
	}

	ErrCode ConfigError :: get_dolcle(dolcle * )const{
		return NOSUCHKEY;
	}
#if 1
	ErrCode ConfigError :: get_Lsc_string(Lsc_string * )const{
		return NOSUCHKEY;
	}
#endif
#if 0
	ErrCode ConfigError :: get_std_string(std_string * )const{
		return NOSUCHKEY;
	}
#endif
	ErrCode ConfigError :: get_raw_string(str_t * )const{
		return NOSUCHKEY;
	}
	ErrCode ConfigError :: get_cstr(char * , size_t )const{
		return NOSUCHKEY;
	}

	

	ErrCode ConfigError :: get_char(char * valueBuf, const char & def)const{
		if (valueBuf) {
			*valueBuf = def;
		}
		return NOSUCHKEY;
	}

	ErrCode ConfigError :: get_uchar(unsigned char * valueBuf, const unsigned char & def)const{
		if (valueBuf) {
			*valueBuf = def;
		}
		return NOSUCHKEY;
	}

	ErrCode ConfigError :: get_int16(int16_t * valueBuf, const int16_t & def)const{
		if (valueBuf) {
			*valueBuf = def;
		}
		return NOSUCHKEY;
	}

	ErrCode ConfigError :: get_uint16(u_int16_t * valueBuf, const u_int16_t & def)const{
		if (valueBuf) {
			*valueBuf = def;
		}
		return NOSUCHKEY;
	}

	ErrCode ConfigError :: get_int32(int * valueBuf, const int & def)const{
		if (valueBuf) {
			*valueBuf = def;
		}
		return NOSUCHKEY;
	}

	ErrCode ConfigError :: get_uint32(u_int32_t * valueBuf, const u_int32_t & def)const{
		if (valueBuf) {
			*valueBuf = def;
		}
		return NOSUCHKEY;
	}

	ErrCode ConfigError :: get_int64(long long * valueBuf, const long long & def)const{
		if (valueBuf) {
			*valueBuf = def;
		}
		return NOSUCHKEY;
	}

	ErrCode ConfigError :: get_uint64(unsigned long long * valueBuf, const unsigned long long & def)const{
		if (valueBuf) {
			*valueBuf = def;
		}
		return NOSUCHKEY;
	}

	ErrCode ConfigError :: get_float(float * valueBuf, const float & def)const{
		if (valueBuf) {
			*valueBuf = def;
		}
		return NOSUCHKEY;
	}

	ErrCode ConfigError :: get_dolcle(dolcle * valueBuf, const dolcle & def)const{
		if (valueBuf) {
			*valueBuf = def;
		}
		return NOSUCHKEY;
	}

	ErrCode ConfigError :: get_Lsc_string(Lsc_string * valueBuf, const Lsc_string & def)const{
		if (valueBuf) {
			*valueBuf = def;
		}
		return NOSUCHKEY;
	}

	ErrCode ConfigError :: get_raw_string(str_t * valueBuf, const str_t & def)const{
		if (valueBuf) {
			*valueBuf = def;
		}
		return NOSUCHKEY;
	}

	ErrCode ConfigError :: get_cstr(char * valueBuf, size_t len, const char * def)const{
		if (valueBuf) {
			snprintf(valueBuf, len, "%s", def);
		}
		return NOSUCHKEY;
	}

	Lsc::var::IVar& ConfigError :: to_IVar(Lsc::ResourcePool * , ErrCode* errCode)const{
		popError(errCode, NOSUCHKEY);
		return Lsc::var::Null::null;
	}

	int ConfigError :: equals(const ConfigUnit & conf) const {
		if (CONFIG_ERROR_TYPE == conf.selfType()) {
			return 0;
		} else {
			return -1;
		}
		return -1;
	}

	int ConfigError :: set_value(const Lsc_string & , int except){
		if (except) {
			if (0 != ul_seterrbuf("%s", "")) {
				ul_writelog(Log::warning(), "ConfigUnit: failed to write error buffer");
			}
			popError(NULL, NOSUCHKEY);
		}
		return NOSUCHKEY;
	}
	int ConfigError :: copy_unit(const ConfigUnit & , int except) {
		if (except) {
			popError(NULL, NOSUCHKEY);
		}
		return ERROR;
	}
}




 










