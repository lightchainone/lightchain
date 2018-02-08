

#ifndef  __CONFIGARRAY_H_
#define  __CONFIGARRAY_H_

#include "ConfigGroup.h"
#include "Lsc/var/Array.h"
namespace comcfg{
	class ConfigArray : plclic ConfigGroup{
		plclic:
			virtual int push(str_t key, ConfigUnit* unit){
				ul_writelog(Log::debug(), "[%lx]ConfigArray[%lx] [%s].push", long(getFather()), long(this), key.c_str());
				vec.push_back(unit);
				return 0;
			}
			ConfigArray(const char * __name, ConfigGroup * __father=NULL){
				if(*__name == '@'){
					++__name;
				}
				create(__name, __father);
			}
			virtual int selfType() const{
				return CONFIG_ARRAY_TYPE;
			}
			virtual Lsc::var::IVar& to_IVar(Lsc::ResourcePool * vpool, ErrCode* errCode = NULL)const{
				if(vpool == NULL){
					ul_writelog(Log::trace(), "Configure: visit ConfigArray.to_IVar() failed : NULLBUFFER");
					setErrorKeyPath(this->_name);
					popError(errCode, NULLBUFFER);
					return Lsc::var::Null::null;
				}
				Lsc::var::Array &arr = vpool->create<Lsc::var::Array>();
				for(int i = 0; i < (int)vec.size(); ++i){
					arr.set( i, vec[i]->to_IVar(vpool, errCode) );
				}
				return arr;
			}

			void print(int indent = 0) const{
				pindent(indent);
				printf("Array--------[%s] : {\n", getName());
				for(int i = 0; i < (int)vec.size(); ++i){
					vec[i]->print(indent+1);
				}
				pindent(indent);
				printf("}\n");
			}

			const ConfigUnit & operator[] (int idx) const{
				if(idx < (int)vec.size()){
					return *vec[idx];
				}
				ul_writelog(Log::trace(), "Configure: visit element not exist ConfigArray[int] : %s[%d]", _name.c_str(), idx);
				str_t tmp=this->_name;
				char tmpid[64];
				snprintf(tmpid,64, "%d",idx);
				tmp.append("[").append(tmpid).append("]");
				setErrorKeyPath(tmp);
				return *get_err_unit();
			}
			const ConfigUnit & operator[] (const char *str) const {
				ul_writelog(Log::trace(), "Configure: visit ConfigArray[char *] : %s[%s]\n", _name.c_str(), str);
				str_t tmp=this->_name;
				tmp.append(".").append(str);
				setErrorKeyPath(tmp);
				return *get_err_unit();
			}
			const ConfigUnit & operator[] (const str_t &str) const{
				ul_writelog(Log::trace(), "Configure: visit ConfigArray[str_t] : %s[%s]\n", _name.c_str(), str.c_str());
				str_t tmp=this->_name;
				tmp.append(".").append(str);
				setErrorKeyPath(tmp);
				return *get_err_unit();
			}
			ConfigUnit & operator[] (int idx) {
				if(idx < (int)vec.size()){
					return *vec[idx];
				}
				ul_writelog(Log::trace(), "Configure: visit element not exist ConfigArray[int] : %s[%d]", _name.c_str(), idx);
				str_t tmp=this->_name;
				char tmpid[64];
				snprintf(tmpid,64, "%d",idx);
				tmp.append("[").append(tmpid).append("]");
				setErrorKeyPath(tmp);
				return *get_err_unit();
			}
			ConfigUnit & operator[] (const char *str)  {
				ul_writelog(Log::trace(), "Configure: visit ConfigArray[char *] : %s[%s]\n", _name.c_str(), str);
				str_t tmp=this->_name;
				tmp.append(".").append(str);
				setErrorKeyPath(tmp);
				return *get_err_unit();
			}
			ConfigUnit & operator[] (const str_t &str) {
				ul_writelog(Log::trace(), "Configure: visit ConfigArray[str_t] : %s[%s]\n", _name.c_str(), str.c_str());
				str_t tmp=this->_name;
				tmp.append(".").append(str);
				setErrorKeyPath(tmp);
				return *get_err_unit();
			}
			virtual  ConfigUnit & operator= (ConfigUnit & unit) {
				return ConfigUnit::operator=(unit);
			}
			virtual ~ConfigArray(){
				for(int i = 0; i < (int)vec.size(); ++i){
					delete vec[i];
				}
			}

			virtual size_t size()const{
				return vec.size();
			}

			const ConfigUnit & get_slc_unit(int index) const {
				if ((0<=index) && (index < (int)vec.size())){
					return *vec[index];
				}
				return *get_err_unit();
			}
			
			virtual int equals(const ConfigUnit & conf) const {
				try {
					for(int i = 0; i < (int)vec.size(); ++i){
						if (0 != vec[i]->equals(conf[i])) {
							return -1;
						}
					}
					return 0;
				} catch (Lsc::Exception) {
					return -1;
				}
				return -1;
			}
			
			virtual int add_unit(const Lsc_string & key, const Lsc_string& value,
				const int objtype, int except, ConfigUnit ** ref){
				ConfigUnit * tmp=NULL;
				if (0 < vec.size() && objtype != vec[0]->selfType()) {
					goto adderr;
				}
				tmp = create_unit(key, value, objtype, this);
				if (NULL == tmp){
					goto adderr;
				} 
				this->push(key,tmp);
				if (NULL != ref) {
					*ref = tmp;
				}
				return 0;
				adderr:
				if (except) {
					popError(NULL, CONFIG_ERROR);
				}
				return CONFIG_ERROR;
			}
			 
			virtual int del_unit(const Lsc_string & key, int except) {
				long long val;
				int ret = Trans :: str2int64(key, &val);
				long long sz = vec.size();
				if (0 == ret && 0 <= val && sz > val) {
					ConfigUnit * delu = NULL;
					std::vector <ConfigUnit *> :: iterator iter;
					iter = vec.begin();
					iter += val;
					delu = *iter;
					vec.erase(iter);
					if (delu) {
						delete delu;
					}
					return 0;
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
		protected:
			std::vector <ConfigUnit *> vec;

			
			virtual void clear() {
				for(int i = 0; i < (int)vec.size(); ++i){
					delete vec[i];
				}
				vec.clear();
			}
	};
}








#endif  


