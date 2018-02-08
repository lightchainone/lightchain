

#ifndef  __CONFIGGROUP_H_
#define  __CONFIGGROUP_H_

#include "ConfigUnit.h"
#include "ConfigError.h"
#include "vector"
namespace comcfg{

	
	class ConfigGroup : plclic ConfigError{
		plclic:
			ConfigGroup(){
				_name = "";
				_father = NULL;
				
			}
			ConfigGroup(const char * __name, ConfigGroup * __father = NULL){
				create(__name, __father);
			}
			virtual const ConfigUnit & operator[] (const char *) const;
			virtual const ConfigUnit & operator[] (const str_t &) const;
			virtual const ConfigUnit & operator[] (int idx) const;

			virtual  ConfigUnit & operator[] (const char *) ;
			virtual  ConfigUnit & operator[] (const str_t &) ;
			virtual  ConfigUnit & operator[] (int idx) ;

			virtual  ConfigUnit & operator= (ConfigUnit & unit){
				return ConfigUnit::operator=(unit);
			}

			virtual Lsc::var::IVar& to_IVar(Lsc::ResourcePool * vpool, ErrCode* errCode = NULL)const;

			virtual void print(int indent = 0) const;
			virtual ~ConfigGroup();
			virtual size_t size()const{
				return fieldMap.size();
			}

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

			virtual const ConfigUnit & get_slc_unit(int index) const;
			virtual const Lsc::string & get_key_name() const;

			
			virtual int equals(const ConfigUnit & conf) const;
			
			virtual int add_unit(const Lsc_string & key, const Lsc_string& value,  
				const int objtype=CONFIG_UNIT_TYPE,int except=0, ConfigUnit ** ref=NULL);
			
			
			virtual int del_unit(const Lsc_string & key, int except=0);
			
			virtual int copy_unit(const ConfigUnit & unit, int except=0);
		protected:
			friend class Reader;
			friend class Constraint;
			friend class Configure;
			friend class ConfigUnit;
			friend class CFdefault;
			void create(const char * __name, ConfigGroup * __father = NULL);
			ConfigUnit * relativeSection(str_t str, int * depth);
			int push(const str_t& key, ConfigUnit* unit);
			ConfigGroup * getFather() const{
				return _father;
			}
			const char * getName() const{
				return _name.c_str();
			}
			virtual int selfType() const{
				return CONFIG_GROUP_TYPE;
			}
			int getLevel() const{
				return _level;
			}

			int _level;

			
			str_t _name;
			std::vector <str_t > fieldList;
			typedef std::map <str_t, ConfigUnit*> :: iterator itr_t;
			typedef std::map <str_t, ConfigUnit*> :: const_iterator const_itr_t;
			std::map <str_t, ConfigUnit*> fieldMap;

			
			virtual void clear();
			
		plclic:
			
			virtual int _append_unit(const ConfigUnit & unit,int check=1, int except=0);

	};

}
















#endif  


