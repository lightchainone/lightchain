
 
#ifndef  __CONFIGUNIT_H_
#define  __CONFIGUNIT_H_

#include "utils/cc_utils.h"
#include "Lsc/var/IVar.h"
#include "Lsc/var/Ref.h"
#include "Lsc/ResourcePool.h"
#include <vector>

namespace comcfg{
	enum{
		CONFIG_UNIT_TYPE = 0,
		CONFIG_GROUP_TYPE,
		CONFIG_ARRAY_TYPE,
		CONFIG_ERROR_TYPE
	};
	class Reader;
	class ConfigGroup;
	class ConfigUnit{
		plclic:
			
			virtual const ConfigUnit & operator[] (const char *) const;
			virtual const ConfigUnit & operator[] (const str_t &) const;
			virtual const ConfigUnit & operator[] (int) const;

			virtual  ConfigUnit & operator[] (const char *) ;
			virtual  ConfigUnit & operator[] (const str_t &) ;
			virtual  ConfigUnit & operator[] (int) ;
			virtual  ConfigUnit & operator= (ConfigUnit & unit) ;
			
			
			virtual const char * seeError(const ErrCode &) const;
			virtual ~ConfigUnit();

			
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
			
			virtual int selfType()const{
				return CONFIG_UNIT_TYPE;
			}
			
			
			virtual size_t size()const{
				return 1;
			}

			
			virtual const ConfigUnit & deepGet(const str_t path) const;

			
			const char * getErrKeyPath() const;

			
			virtual const ConfigUnit & get_slc_unit(int index) const;
			
			virtual int equals(const ConfigUnit & conf) const;

			
			
			
			virtual int add_unit(const Lsc_string & key, const Lsc_string& value, 
				const int objtype=CONFIG_UNIT_TYPE, int except=0, ConfigUnit ** ref=NULL);

			
			virtual int append_unit(const ConfigUnit & unit, int except=0);

			
			virtual int copy_unit(const ConfigUnit & unit, int except=0);
			
			
			virtual int del_unit(const Lsc_string & key, int except=0);
			
			virtual int set_value(const Lsc_string & value, int except=0);


			

			
			ConfigUnit();
			ConfigUnit(const str_t& __key, const str_t& __value, const Reader * __cur_reader = NULL, ConfigGroup * father = NULL);

			
			void pindent(int ind)const{
				while(ind--){
					printf("    ");
				}
			}
			
			virtual void print(int indent = 0)const{
				pindent(indent);
				printf("=[%s], _value=[%s]\n", _key.c_str(), _value.c_str());
			}


			
			virtual str_t info() const{
				
				
				str_t buf;
				buf.appendf("[File:%s Line:%d Key:%s Value:%s]", 
						_at_file ? _at_file : "NULL", _at_line, _key.c_str(), _value.c_str());
				return buf;
			}
			
			virtual ConfigUnit * relativeSection(str_t, int* ){
				return NULL;
			}

			static ConfigUnit* get_err_unit();
			virtual const Lsc::string & get_key_name() const;
		protected:
			virtual void popError(ErrCode *, ErrCode) const;
			void setErrorKeyPath(str_t str)const;
			const ConfigUnit & deepGetSegment(const str_t segment) const; 
			
			static const str_t g_unknown;
			str_t _key;
			str_t _value;
			str_t _cstr; 
			int _cstr_err;
			str_t _vstring;
			const char * _at_file;
			int _at_line;
			ConfigGroup * _father;
			
			ConfigUnit * create_unit(const Lsc_string & key, const Lsc_string& value, 
				int objtype, ConfigGroup * father);

			
			virtual void clear();
			
		plclic:
			
			virtual int _append_unit(const ConfigUnit & unit,int check=1, int except=0);
		private:
			int init_unit(const str_t& __key, const str_t& __value);
	};

}









#endif  


