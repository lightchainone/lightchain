#include <set>
#include "Constraint.h"
#include "ConstraintFunction.h"
#include "utils/cc_utils.h"
#include "idm_conf_if.h"

#define ARRAY_FUNC "array"
#define CONSTRAINT_FUNC "constraint"
namespace comcfg{
	static const str_t baseType[] = {
		CC_CHAR,
		CC_UCHAR,
		CC_INT16,
		CC_UINT16,
		CC_INT32,
		CC_UINT32,
		CC_INT64,
		CC_UINT64,
		CC_FLOAT,
		CC_DOLCLE,
		CC_STRING
	};
	static std::set <str_t> baseSet(baseType, baseType + (sizeof(baseType)/sizeof(baseType[0])));

	
	
	bool isBaseType(const str_t & s){
		return baseSet.find(s) != baseSet.end();
	}

	
	class CF_cmp_t{
		plclic:
			int operator()(const confIDL::cons_func_t * a, const confIDL::cons_func_t * b)const{
				return ConstraintLibrary :: getLevel_static(a->func.data) < 
					ConstraintLibrary :: getLevel_static(b->func.data);
			}
	};

	str_t showIDLMsg(const confIDL::meta_t& meta){
		str_t s;
		s.appendf("[File:%s Line:%d]=[%s]", meta.file.c_str(), meta.lineno, meta.data.c_str());
		return s;
	}

	int Constraint :: printError(){
		if(idm == NULL){
			return -1;
		}
		ul_writelog(Log::warning(), " =========== We have error in Constraint file, see below ============");
		confIDL:: err_list_t :: iterator itr;
		for(itr = idm->err_list.begin(); itr != idm->err_list.end(); ++itr){
			ul_writelog(Log::warning(), "Constraint Error : %s", showIDLMsg((*itr)).c_str());
		}
		return 0;
	}

	int Constraint :: run(confIDL::idm_t * __idm, Configure * __conf){
		idm = __idm;
		conf = __conf;

		if(idm == NULL || conf == NULL){
			return -1;
		}
		if(idm->retval != 0){
			printError();
			return -1;
		}

		confIDL:: var_list_t :: iterator itr;
		
		
		for(itr = idm->rc_var_list.begin(); itr != idm->rc_var_list.end(); ++itr){
			(*itr)->cf_list.sort(CF_cmp_t());
		}

		
		int result = 0;
		for(itr = idm->var_list.begin(); itr != idm->var_list.end(); ++itr){
			try{
				if( runVar(*itr, conf) != 0 ){
					throw ConfigException();
				}
			}
			catch(...){
				result = -1;
			}
		}
		return result;
	}

	int Constraint :: runVar(confIDL::var_t* var, ConfigGroup * father, ConfigUnit * self){
		int ret = 0;

		
		confIDL :: cf_map_t :: iterator cfitr = var->cf_map.find(str_t(CONSTRAINT_FUNC));
		if(cfitr != var->cf_map.end() && cfitr->second->arg_vec.size() > 0 ){
			ul_writelog(Log::trace(), "Re-constraint %s to [%s]", 
					showIDLMsg(var->name).c_str(), cfitr->second->arg_vec[0]->data.c_str());
			var->type.data = cfitr->second->arg_vec[0]->data;
		}
		const ConfigUnit & myref = (self == NULL) ? (*father)[var->name.data] : *self;
		confIDL:: cf_list_t :: const_iterator itr = var->cf_list.begin(); 
		str_t plog;
		for(itr = var->cf_list.begin(); itr != var->cf_list.end(); ++itr){
			plog.append((*itr)->func.data).append("();");
		}
		ul_writelog(Log::trace(), "Config : var[%s], func[%s]", showIDLMsg(var->name).c_str(), plog.c_str());
		for(; itr != var->cf_list.end(); ++itr){
			if( (*itr)->func.data == str_t(ARRAY_FUNC)){
				try{
					ul_writelog(Log::debug(), "Pre-process array constraint : %s", showIDLMsg(var->name).c_str());
					ret = ConstraintLibrary :: getInstance() -> check(*(*itr), *var, father, self);
				}catch(ConfigException){
					
					ul_writelog(Log::warning(), "Pre-process Array error...");
					return -1;
				}
				if(ret){
					return ret;
				}
				break;
			}
		}
#if 1
		
		const ConfigUnit & arr = myref;
		if(arr.selfType() == CONFIG_ARRAY_TYPE){
			ul_writelog(Log::trace(), "Constraint : check array elements[%d]", (int)arr.size());
			confIDL::var_t xvar = *var;
			if(xvar.cf_map.find(str_t(ARRAY_FUNC)) == xvar.cf_map.end()){
				ul_writelog(Log::warning(), "Constraint : %s should not be array! %s", 
						arr[0].info().c_str(), showIDLMsg(xvar.name).c_str());
				return -1;
			}
			xvar.cf_map.erase(str_t(ARRAY_FUNC));
			
			for(confIDL::cf_list_t::iterator arritr = xvar.cf_list.begin(); arritr != xvar.cf_list.end(); ++arritr){
				if((*arritr)->func.data == str_t(ARRAY_FUNC)){
					xvar.cf_list.erase(arritr);
					break;
				}
			}
			for(int i = 0; i < (int)arr.size(); ++i){
				ConfigUnit * unit = const_cast<ConfigUnit*>(&arr[i]);
				try{
					if( runVar(&xvar, NULL, unit) != 0 ){
						ret = -1;
					}
				}catch(...){
					ul_writelog(Log::warning(), "Constaint : invalid value %s : %s", 
							unit->info().c_str(), showIDLMsg(xvar.name).c_str());
					ret = -1;
				}
			}
		}
#endif
		else{
			if( isBaseType(var->type.data) ){
				ret = runKey(var, father, self);
			}
			else{
				ret = runGroup(var, father, self);
			}
		}
		return ret;
	}

	int Constraint :: runGroup(confIDL::var_t* var, ConfigGroup * father, ConfigUnit * self){
		ul_writelog(Log::trace(), "Constaint : working as group : %s [type:%s]", showIDLMsg(var->name).c_str(), var->type.data.c_str());
#if 0
		if(father != NULL){
			printf("----father----\n");
			father->print();
		}
		if(self != NULL){
			printf("----self----\n");
			self->print();
		}
#endif
		confIDL:: group_map_t :: iterator mpitr = idm->group_map.find(var->type.data);
		int ret = 0;
		if( mpitr == idm->group_map.end() ){ 
			ul_writelog(Log::warning(), "Constraint : error constraint type : %s", showIDLMsg(var->type).c_str());
			throw ConfigException();
		}
		else{
			
			ConfigUnit * gunit;
			if(father == NULL){ 
				ul_writelog(Log::trace(), "%s is one of group array", var->name.data.c_str());
				gunit = self;
			}
			else{
				gunit = const_cast <ConfigUnit *> (&((*father)[var->name.data]));
			}

			if(gunit == NULL || gunit->selfType() == CONFIG_ERROR_TYPE){
				ul_writelog(Log::notice(), "No such section in config : %s . try to create.", 
						showIDLMsg(var->name).c_str());
				gunit = new ConfigGroup(var->name.data.c_str(), father);
				father->push(var->name.data, gunit);
			}
			if(gunit->selfType() != CONFIG_GROUP_TYPE){
				ul_writelog(Log::warning(), "%s should be a Group type.", var->name.data.c_str());
				return -1;
			}
			confIDL:: var_list_t :: iterator gitr;
			for(gitr = mpitr->second->var_list.begin(); gitr != mpitr->second->var_list.end(); ++gitr){
				try{
					if( runVar(*gitr, (ConfigGroup *)gunit, NULL) != 0){
						throw ConfigException();
					}
				}
				catch(...){
					ret = -1;
				}
			}
		}
		return ret;
	}

	int Constraint :: runKey(confIDL::var_t* var, ConfigGroup * father, ConfigUnit * self){
		
		ul_writelog(Log::trace(), "Constaint : working as simple type : %s", showIDLMsg(var->name).c_str());
#if 0
		if(father != NULL){
			printf("----father----\n");
			father->print();
		}
		if(self != NULL){
			printf("----self----\n");
			self->print();
		}
#endif
		int ret = 0;
		confIDL:: cf_list_t :: const_iterator itr;
		for(itr = var->cf_list.begin(); itr != var->cf_list.end(); ++itr){
			if(var->cf_map.find((*itr)->func.data) == var->cf_map.end()){
				continue;
			}
			try{
				ul_writelog(Log::debug(), "Constaint : %s", (*itr)->func.data.c_str());
				ret = ConstraintLibrary :: getInstance() -> check(*(*itr), *var, father, self);
				if(ret){
					return ret;
				}
			}catch(ConfigException){
				const ConfigUnit *unit = self;
				if(unit == NULL){
					unit = &(*father)[var->name.data];
				}
				ul_writelog(Log::warning(), "Constaint : invalid value %s : %s", 
						unit->info().c_str(), showIDLMsg(var->name).c_str());
				ret = -1;
			}
			catch(...){
				ul_writelog(Log::warning(), "Constaint : Unknown exception");
				ret = -1;
			}

		}
		if(ret){
			return ret;
		}
		const ConfigUnit *unit = self;
		if(unit == NULL){
			unit = &(*father)[var->name.data];
		}
		ret = runType(var->type.data, *unit);
		if(ret){
			ul_writelog(Log::warning(), "Constaint : Value is not a valid [%s]type %s : %s", 
					var->type.data.c_str(), unit->info().c_str(), showIDLMsg(var->name).c_str());
		}
		return ret;
	}


	int Constraint :: runType(const str_t& type, const ConfigUnit & unit){
		try{
			if(strcmp(type.c_str(), CC_CHAR) == 0){
				unit.to_char();
				return 0;
			}

			if(strcmp(type.c_str(), CC_UCHAR) == 0){
				unit.to_uchar();
				return 0;
			}

			if(strcmp(type.c_str(), CC_INT16) == 0){
				unit.to_int16();
				return 0;
			}

			if(strcmp(type.c_str(), CC_UINT16) == 0){
				unit.to_uint16();
				return 0;
			}

			if(strcmp(type.c_str(), CC_INT32) == 0){
				unit.to_int32();
				return 0;
			}

			if(strcmp(type.c_str(), CC_UINT32) == 0){
				unit.to_uint32();
				return 0;
			}

			if(strcmp(type.c_str(), CC_INT64) == 0){
				unit.to_int64();
				return 0;
			}

			if(strcmp(type.c_str(), CC_UINT64) == 0){
				unit.to_uint64();
				return 0;
			}

			if(strcmp(type.c_str(), CC_FLOAT) == 0){
				unit.to_float();
				return 0;
			}

			if(strcmp(type.c_str(), CC_DOLCLE) == 0){
				unit.to_dolcle();
				return 0;
			}

			if(strcmp(type.c_str(), CC_STRING) == 0){
				unit.to_Lsc_string();
				return 0;
			}
		}
		catch(...){
			return -1;
		}
		return -1;
	}
}

















