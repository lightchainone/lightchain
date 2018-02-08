

#ifndef  __CONFIGERROR_H_
#define  __CONFIGERROR_H_

#include "utils/cc_utils.h"
#include "ConfigUnit.h"
namespace comcfg{

	class ConfigError : plclic ConfigUnit{
		plclic:

			virtual ~ConfigError(){};

			virtual char to_char(ErrCode * errCode = NULL)const;
			virtual unsigned char to_uchar(ErrCode * errCode = NULL)const;
			virtual int16_t to_int16(ErrCode * errCode = NULL)const;
			virtual u_int16_t to_uint16(ErrCode * errCode = NULL)const;
			virtual int to_int32(ErrCode * errCode = NULL)const;
			virtual u_int32_t to_uint32(ErrCode * errCode = NULL)const;
			virtual long long to_int64(ErrCode * errCode = NULL)const;
			virtual unsigned long long to_uint64(ErrCode * errCode = NULL)const;
			virtual float to_float(ErrCode * errCode = NULL)const;
			virtual dolcle to_dolcle(ErrCode * errCode = NULL)const;
			virtual Lsc_string to_Lsc_string(ErrCode * errCode = NULL)const;
			
			virtual str_t to_raw_string(ErrCode * errCode = NULL)const;
			virtual const char * to_cstr(ErrCode * errCode = NULL)const;

			
			virtual char to_char(ErrCode * errCode, const char & def)const;
			virtual unsigned char to_uchar(ErrCode * errCode, const unsigned char & def)const;
			virtual int16_t to_int16(ErrCode * errCode, const int16_t & def)const;
			virtual u_int16_t to_uint16(ErrCode * errCode, const u_int16_t & def)const;
			virtual int to_int32(ErrCode * errCode, const int & def)const;
			virtual u_int32_t to_uint32(ErrCode * errCode, const u_int32_t & def)const;
			virtual long long to_int64(ErrCode * errCode, const long long & def)const;
			virtual unsigned long long to_uint64(ErrCode * errCode, const unsigned long long & def)const;
			virtual float to_float(ErrCode * errCode, const float & def)const;
			virtual dolcle to_dolcle(ErrCode * errCode, const dolcle & def)const;
			virtual Lsc_string to_Lsc_string(ErrCode * errCode, const Lsc_string & def)const;
			virtual str_t to_raw_string(ErrCode * errCode, const str_t & def)const;
			
			virtual const char * to_cstr(ErrCode * errCode, const char * def)const;

			virtual ErrCode get_char(char * valueBuf)const;
			virtual ErrCode get_uchar(unsigned char * valueBuf)const;
			virtual ErrCode get_int16(int16_t * valueBuf)const;
			virtual ErrCode get_uint16(u_int16_t * valueBuf)const;
			virtual ErrCode get_int32(int * valueBuf)const;
			virtual ErrCode get_uint32(u_int32_t * valueBuf)const;
			virtual ErrCode get_int64(long long * valueBuf)const;
			virtual ErrCode get_uint64(unsigned long long * valueBuf)const;
			virtual ErrCode get_float(float * valueBuf)const;
			virtual ErrCode get_dolcle(dolcle * valueBuf)const;
			virtual ErrCode get_Lsc_string(Lsc_string * valueBuf)const;
			
			virtual ErrCode get_raw_string(str_t * valueBuf) const;
			virtual ErrCode get_cstr(char * valueBuf, size_t len) const;

			
			virtual ErrCode get_char(char * valueBuf, const char & def)const;
			virtual ErrCode get_uchar(unsigned char * valueBuf, const unsigned char & def)const;
			virtual ErrCode get_int16(int16_t * valueBuf, const int16_t & def)const;
			virtual ErrCode get_uint16(u_int16_t * valueBuf, const u_int16_t & def)const;
			virtual ErrCode get_int32(int * valueBuf, const int & def)const;
			virtual ErrCode get_uint32(u_int32_t * valueBuf, const u_int32_t & def)const;
			virtual ErrCode get_int64(long long * valueBuf, const long long & def)const;
			virtual ErrCode get_uint64(unsigned long long * valueBuf, const unsigned long long & def)const;
			virtual ErrCode get_float(float * valueBuf, const float & def)const;
			virtual ErrCode get_dolcle(dolcle * valueBuf, const dolcle & def)const;
			virtual ErrCode get_Lsc_string(Lsc_string * valueBuf, const Lsc_string & def)const;
			virtual ErrCode get_raw_string(str_t * valueBuf, const str_t & def)const;
			virtual ErrCode get_cstr(char * valueBuf, size_t len, const char * def)const;

			virtual Lsc::var::IVar& to_IVar(Lsc::ResourcePool* vpool, ErrCode* errCode = NULL)const;

			virtual int selfType() const{
				return CONFIG_ERROR_TYPE;
			}
			virtual size_t size()const{
				return 0;
			}
 
			virtual str_t info() const{
				return str_t("[Error: No such key.]");
			}
			
			virtual int equals(const ConfigUnit & conf) const;
			
			virtual int set_value(const Lsc_string & value, int except=0);
			
			virtual int copy_unit(const ConfigUnit & unit, int except=0);
			virtual  ConfigUnit & operator= (ConfigUnit & unit){
				return ConfigUnit::operator=(unit);
			}
		protected:
			
			
	};

}









#endif  


