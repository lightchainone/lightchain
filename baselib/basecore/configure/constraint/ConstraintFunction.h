

#ifndef  __CONSTRAINTFUNCTION_H_
#define  __CONSTRAINTFUNCTION_H_

#include <string>
#include <list>
#include <map>
#include "Configure.h"
#include "ConfigGroup.h"
#include "ConfigUnit.h"

namespace confIDL{
	struct cons_func_t;
	struct var_t;
	struct idm_t;
}

namespace comcfg{
	
	typedef std::list <str_t> param_list_t;

	
	typedef int (* CF_T)(const confIDL::cons_func_t&, const confIDL::var_t &, ConfigGroup *, ConfigUnit*);

	enum CFLevel{
		CF_STRUCT = 0,	
		CF_KEY,			
		CF_TYPE,		
		CF_VALUE,		
		CF_DEFAULT,
	};

	class ConstraintFunction{
		int _level;
		plclic:
			virtual int check(const confIDL::cons_func_t&, const confIDL::var_t &, ConfigGroup*, ConfigUnit*);
			void setLevel(int lv);
			int getLevel();
	};
	typedef std::map<str_t, ConstraintFunction*> CFMAP_T;

	
	class ConstraintLibrary{
		ConstraintLibrary();
		~ConstraintLibrary();
		static ConstraintLibrary * lib;
		plclic:
			
			static ConstraintLibrary* getInstance();
			
			int getLevel(const str_t& function);
			static int getLevel_static(const str_t& function);
			
			int check(const confIDL::cons_func_t&, const confIDL::var_t &, ConfigGroup*, ConfigUnit*);
			
			int registConstraint(const str_t& fname, ConstraintFunction* cons, bool overwrite = false);

			int registConstraint(const str_t& fname, CF_T cf, bool overwrite = false){
				return registConstraint(fname, cf, CF_VALUE, overwrite);
			}
		private:
			friend class Constraint;
			friend class ConsUnitControl;
			int registConstraint(const str_t& fname, CF_T cf, int cfLevel, bool overwrite = false);
		protected:
			CFMAP_T cfMap;
			class WrapFunction : plclic ConstraintFunction{
				plclic:
					virtual int check(const confIDL::cons_func_t& cons, 
							const confIDL::var_t& param, ConfigGroup* father, ConfigUnit* self){
						return _cf(cons, param, father, self);
					}
					void setCF(CF_T cf){
						_cf = cf;
					}
					WrapFunction(){}
				protected:
					CF_T _cf;
			};
	};
}




#endif  


