
#include "cc_default.h"
#include "utils/cc_utils.h"
#include "Configure.h"
#include "ConfigUnit.h"
#include "ConfigGroup.h"
#include "ConfigArray.h"
#include "spreg.h"
namespace comcfg{

	extern str_t showIDLMsg(const confIDL::meta_t& meta);

	int CFdefault :: cons_do_nothing(const confIDL::cons_func_t& , const confIDL::var_t & , ConfigGroup * , ConfigUnit * ){
		return 0;
	}
	int CFdefault :: cons_array(const confIDL::cons_func_t& func, const confIDL::var_t & var, ConfigGroup * father, ConfigUnit * self){
		ul_writelog(Log::trace(), "In constraint function array(), var is %s", showIDLMsg(var.name).c_str());
		if(func.arg_list.size() > 1){
			ul_writelog(Log::warning(), "Constraint : Too many args for array(), in %s", showIDLMsg(var.name).c_str());
			return -1;
		}
		const ConfigUnit * unit = self;
		if(unit == NULL){
			unit = & (*father)[var.name.data];
		}
		if(unit->selfType() == CONFIG_ERROR_TYPE){
			if(father == NULL){
				ul_writelog(Log::warning(), "Constraint : Unknown error in array(). %s", showIDLMsg(var.name).c_str());
				return -1;
			}
			father->push(var.name.data, new ConfigArray(var.name.data.c_str(), father));
			ul_writelog(Log::notice(), "Constraint : create array, in %s", showIDLMsg(var.name).c_str());
		}
		else if(unit->selfType() != CONFIG_ARRAY_TYPE){
			ul_writelog(Log::warning(), "Constraint : variable exists but is not array. %s", showIDLMsg(var.name).c_str());
			return -1;
		}
		else{
			if(func.arg_list.size() == 0){
				return 0;
			}
			unsigned long long asize;
			if(Trans::str2uint64(func.arg_vec[0]->data, &asize) != 0 ){
				ul_writelog(Log::warning(), "Constraint : array size is invalid. Ignored! %s", showIDLMsg(var.name).c_str());
				return 0;
			}
			else{
				if(asize == 0){
					return 0;
				}
				if(asize < unit->size()){
					ul_writelog(Log::warning(), "Constraint : array has too many elements! %s [MaxSize=%llu][We have %zu elements]", 
							showIDLMsg(var.name).c_str(), asize, unit->size());
					return 0;
				}
			}
		}
		return 0;
	}

	int CFdefault :: cons_default(const confIDL::cons_func_t& func, const confIDL::var_t & var, ConfigGroup * father, ConfigUnit * self){
		ul_writelog(Log::trace(), "In constraint function default(), var is %s", showIDLMsg(var.name).c_str());
		if(func.arg_list.size() != 1){
			ul_writelog(Log::warning(), "Constraint : wrong args for default(), in %s", showIDLMsg(var.name).c_str());
			return -1;
		}
		const ConfigUnit * unit = self;
		if(unit == NULL){
			unit = & (*father)[var.name.data];
		}
		if(unit->selfType() == CONFIG_ERROR_TYPE){
			if(father == NULL){
				ul_writelog(Log::warning(), "Constraint : Unknown error in default(). %s", showIDLMsg(var.name).c_str());
				return -1;
			}
			father->push(var.name.data, new ConfigUnit(var.name.data.c_str(), func.arg_vec[0]->data, NULL, father));
			ul_writelog(Log::notice(), "Constraint : create default key, use %s", showIDLMsg(var.name).c_str());
			return 0;
		}
		return 0;
	}

	int CFdefault :: cons_length(const confIDL::cons_func_t& func, const confIDL::var_t & var, ConfigGroup * father, ConfigUnit * self){
		ul_writelog(Log::trace(), "In constraint function length(), var is %s", showIDLMsg(var.name).c_str());
		if(func.arg_list.size() != 1){
			ul_writelog(Log::warning(), "Constraint : wrong args for length(), in %s", showIDLMsg(var.name).c_str());
			return -1;
		}
		unsigned long long asize;
		if(Trans::str2uint64(func.arg_vec[0]->data, &asize) != 0 ){
			ul_writelog(Log::warning(), "Constraint : wrong args for length(). %s", showIDLMsg(var.name).c_str());
			return -1;
		}
		const ConfigUnit * unit = self;
		if(unit == NULL){
			unit = & (*father)[var.name.data];
		}
		if(unit->selfType() == CONFIG_UNIT_TYPE){
			str_t ref = unit->to_Lsc_string();
			if( ref.size() > asize){
				ul_writelog(Log::warning(), "Constraint : length() : value is too long.%s : %s", 
						unit->info().c_str(), showIDLMsg(var.name).c_str());
				return -1;
			}
		}
		return 0;
	}

	int CFdefault :: cons_enum(const confIDL::cons_func_t& func, const confIDL::var_t & var, ConfigGroup * father, ConfigUnit * self){
		ul_writelog(Log::trace(), "In constraint function enum(), var is %s", showIDLMsg(var.name).c_str());
		const ConfigUnit * unit = self;
		if(unit == NULL){
			unit = & (*father)[var.name.data];
		}
		if(unit->selfType() == CONFIG_UNIT_TYPE){
			str_t str = unit->to_Lsc_string();
			str_t buf;
			for( int i = 0; i < (int)func.arg_vec.size(); ++i){
				
				if( Trans::str2str(func.arg_vec[i]->data, &buf) != 0 ){
					buf = func.arg_vec[i]->data;
				}
				if(str == buf){
					return 0;
				}
			}
		}
		ul_writelog(Log::warning(), "Constraint : key not in enum.%s : %s", 
				unit->info().c_str(), showIDLMsg(var.name).c_str());
		return -1;
	}

	int CFdefault :: cons_regexp(const confIDL::cons_func_t& func, const confIDL::var_t & var, ConfigGroup * father, ConfigUnit * self){
		ul_writelog(Log::trace(), "In constraint function regexp(), var is %s", showIDLMsg(var.name).c_str());
		if(func.arg_list.size() != 1){
			ul_writelog(Log::warning(), "Constraint : wrong args for regexp(), in %s", showIDLMsg(var.name).c_str());
			return -1;
		}
		char const * err;
		str_t reg;
		spreg_t *re;
		int ret = Trans::str2str(func.arg_vec[0]->data, &reg);
		re = spreg_init(reg.c_str(), &err);
		if(ret != 0 || err != NULL){
			ul_writelog(Log::warning(), "Constraint : wrong args for regexp(). %s", showIDLMsg(var.name).c_str());
			return -1; 
		}

		const ConfigUnit * unit = self;
		if(unit == NULL){
			unit = & (*father)[var.name.data];
		}
		if(unit->selfType() == CONFIG_UNIT_TYPE){
			str_t str = unit->to_Lsc_string();
			if( spreg_match(re, str.c_str(), str.size()) > 0){
				ret = 0;
				goto end;
			}
			else{
				ul_writelog(Log::warning(), "Constraint : key not match regexp(). %s : %s", 
						unit->info().c_str(), showIDLMsg(var.name).c_str());
				ret = -1;
				goto end;
			}
		}
		ret = 0;
end:
		spreg_destroy(re);
		return ret;
	}

	int CFdefault :: cons_range(const confIDL::cons_func_t& func, const confIDL::var_t & var, ConfigGroup * father, ConfigUnit * self){
		ul_writelog(Log::trace(), "In constraint function range(), var is %s, type=%s", 
				showIDLMsg(var.name).c_str(), var.type.data.c_str());
		if(func.arg_list.size() != 2){
			ul_writelog(Log::warning(), "Constraint : wrong args for range(), in %s", showIDLMsg(var.name).c_str());
			return -1;
		}
		const ConfigUnit * unit = self;
		if(unit == NULL){
			unit = & (*father)[var.name.data];
		}
		if(unit->selfType() == CONFIG_UNIT_TYPE){
			if(var.type.data == CC_FLOAT || var.type.data == CC_DOLCLE){
				dolcle val = unit->to_dolcle();
				dolcle buf1, buf2;
				if( Trans::str2dolcle(func.arg_vec[0]->data, &buf1) || 
						Trans::str2dolcle(func.arg_vec[1]->data, &buf2)){
					ul_writelog(Log::warning(), "Constraint : wrong args in range(), in %s", showIDLMsg(var.name).c_str());
					return -1;
				}
				if(val < buf1 || val > buf2){
					ul_writelog(Log::warning(), "Constraint : Key not in range(), in %s : %s", 
							unit->info().c_str(), showIDLMsg(var.name).c_str());
					return -1;
				}
			}
			else if(var.type.data == CC_UINT64){
				unsigned long long val = unit->to_uint64();
				unsigned long long buf1, buf2;
				if( Trans::str2uint64(func.arg_vec[0]->data, &buf1) || 
						Trans::str2uint64(func.arg_vec[1]->data, &buf2)){
					ul_writelog(Log::warning(), "Constraint : wrong args in range(), in %s", showIDLMsg(var.name).c_str());
					return -1;
				}
				if(val < buf1 || val > buf2){
					ul_writelog(Log::warning(), "Constraint : Key not in range(), in %s : %s", 
							unit->info().c_str(), showIDLMsg(var.name).c_str());
					return -1;
				}
			}
			else if(var.type.data == CC_STRING){
				ul_writelog(Log::warning(), "Constraint : wrong usage of range(), in %s", showIDLMsg(var.name).c_str());
				return -1;
			}
			else{
				long long val = unit->to_int64();
				long long buf1, buf2;
				if( Trans::str2int64(func.arg_vec[0]->data, &buf1) || 
						Trans::str2int64(func.arg_vec[1]->data, &buf2)){
					ul_writelog(Log::warning(), "Constraint : wrong args in range(), in %s", showIDLMsg(var.name).c_str());
					return -1;
				}
				if(val < buf1 || val > buf2){
					ul_writelog(Log::warning(), "Constraint : Key not in range(), in %s : %s", 
							unit->info().c_str(), showIDLMsg(var.name).c_str());
					return -1;
				}
			}
		}
		return 0;
	}



	
	
	static bool check_ip(const str_t& str, int pos, int status, int val){
		if(status > 7){
			return false;
		}
		if(pos == (int)str.size()){
			return status == 7;
		}
		if(str[pos] == '.'){
			if(status % 2 == 0){
				ul_writelog(Log::debug(), "Unexpected '.' in ip [%s]", str.c_str());
				return false;
			}
			return check_ip(str, pos+1, status+1, 0);
		}
		else if (isdigit(str[pos])){
			if(status % 2 == 0){
				return check_ip(str, pos+1, status+1, str[pos] - '0');
			}
			else{
				val = val * 10 + str[pos] - '0';
				if(val > 255) {
					ul_writelog(Log::debug(), "Unexpected digits in ip [%s]", str.c_str());
					return false;
				}
				return check_ip(str, pos+1, status, val);
			}
		}
		else{
			ul_writelog(Log::debug(), "Unexpected char '%c' in ip [%s]", str[pos], str.c_str());
			return false;
		}
	}

	int CFdefault :: cons_ip(const confIDL::cons_func_t& func, const confIDL::var_t & var, ConfigGroup * father, ConfigUnit * self){
		ul_writelog(Log::trace(), "In constraint function ip(), var is %s", showIDLMsg(var.name).c_str());
		if(func.arg_list.size() != 0){
			ul_writelog(Log::warning(), "Constraint : wrong args for ip(), in %s", showIDLMsg(var.name).c_str());
			return -1;
		}
		const ConfigUnit * unit = self;
		if(unit == NULL){
			unit = & (*father)[var.name.data];
		}
		if(unit->selfType() == CONFIG_UNIT_TYPE){
			str_t str = unit->to_Lsc_string();
			if(check_ip(str, 0, 0, 0)){
				return 0;
			}
			ul_writelog(Log::warning(), "Constraint : Key is not ip(), in %s : %s", 
					unit->info().c_str(), showIDLMsg(var.name).c_str());
			return -1;
		}
		return 0;
	}

}
















