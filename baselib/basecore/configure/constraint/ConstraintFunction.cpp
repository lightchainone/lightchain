
#include "ConstraintFunction.h"
#include "cc_default.h"

namespace comcfg{
    class ConsUnitControl {
        private:
            static int _needfree;
        plclic:
            ConsUnitControl() {
                ConfigUnit :: get_err_unit();
            }
        ~ConsUnitControl() {
                if (_needfree) {
                    _needfree =0;
                    ConstraintLibrary* cu = ConstraintLibrary :: getInstance();
                    delete cu;
                }
            }
    };
    int ConsUnitControl::_needfree = 1;

    static ConsUnitControl cons_unit_control;



	ConstraintLibrary * ConstraintLibrary :: lib = NULL;

	
	int ConstraintFunction :: check(const confIDL::cons_func_t&, const confIDL::var_t &, ConfigGroup*, ConfigUnit*){
		ul_writelog(Log::warning(), "Unknown function : calling virtual method");
		return 0;
	};
	void ConstraintFunction :: setLevel(int lv){
		_level = lv;
	}
	int ConstraintFunction :: getLevel(){
		return _level;
	}

	
	ConstraintLibrary :: ConstraintLibrary(){
		registConstraint(str_t("comment"), CFdefault :: cons_do_nothing, CF_VALUE);
		registConstraint(str_t("array"), CFdefault :: cons_array, CF_STRUCT);
		registConstraint(str_t("default"), CFdefault :: cons_default, CF_KEY);
		registConstraint(str_t("ip"), CFdefault :: cons_ip, CF_VALUE);
		registConstraint(str_t("length"), CFdefault :: cons_length, CF_VALUE);
		registConstraint(str_t("regexp"), CFdefault :: cons_regexp, CF_VALUE);
		registConstraint(str_t("enum"), CFdefault :: cons_enum, CF_VALUE);
		registConstraint(str_t("range"), CFdefault :: cons_range, CF_VALUE);
	};
	ConstraintLibrary :: ~ConstraintLibrary(){
		CFMAP_T :: iterator itr;
		for(itr = cfMap.begin(); itr != cfMap.end(); ++itr){
			delete itr->second;
		}
	}
	ConstraintLibrary* ConstraintLibrary :: getInstance(){
		if(lib == NULL){
			lib = new ConstraintLibrary();
		}
		return lib;
	}
	int ConstraintLibrary :: getLevel(const str_t& function){
		if(cfMap.find(function) != cfMap.end()){
			return cfMap[function]->getLevel();
		}
		return CF_DEFAULT;
	}
	int ConstraintLibrary :: getLevel_static(const str_t& function){
		return getInstance() -> getLevel(function);
	}
	int ConstraintLibrary :: check(const confIDL::cons_func_t& func, 
			const confIDL::var_t& param, ConfigGroup* father, ConfigUnit * self){
		if(cfMap.find(func.func.data) != cfMap.end()){
			return cfMap[func.func.data]->check(func, param, father, self);
		}
		ul_writelog(Log::warning(), "Unknown function : [File:%s, Line:%d]=[%s]", func.func.file.c_str(), 
				func.func.lineno, func.func.data.c_str());
		return -1;
	}

	int ConstraintLibrary :: registConstraint(const str_t& fname, ConstraintFunction* cons, bool overwrite)
	{
		if(false == overwrite && cfMap.find(fname) != cfMap.end()){
			return -1;
		}
		if(cfMap.find(fname) != cfMap.end()) {
			delete cfMap[fname];
		}
		cfMap[fname] = cons;
		return 0;
	}

	int ConstraintLibrary :: registConstraint(const str_t& fname, CF_T cf, int cfLevel, bool overwrite){
		WrapFunction * wf = new WrapFunction();
		wf->setCF(cf);
		wf->setLevel(cfLevel);
		return registConstraint(fname, wf, overwrite);
	}

}




















