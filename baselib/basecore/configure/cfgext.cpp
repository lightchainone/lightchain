
#include "cfgext.h"
#include "idm_conf_if.h"
#include "Configure.h"

#define ARRAY_FUNC "array"
#define COMMENT_FUNC "comment"
#define DEFAULT_FUNC "default"

namespace comcfg{

	
	
	extern bool isBaseType(const str_t & s);

	void printBaseType(confIDL::idm_t * idm, confIDL::var_t * var, str_t * conf, str_t * section, bool isArray);
	void printGroup(confIDL::idm_t * idm, confIDL::var_t * var, str_t * conf, str_t * section, bool isArray);
	void printVar(confIDL::idm_t * idm, confIDL::var_t * var, str_t * conf, str_t * section);

	void printBaseType(confIDL::idm_t * , confIDL::var_t * var, str_t * conf, str_t * , bool isArray){
		
		confIDL::cf_map_t::iterator cfitr = var->cf_map.find(str_t(COMMENT_FUNC));
		if(cfitr != var->cf_map.end() && cfitr->second->arg_list.size() > 0){
			conf->appendf("#%s\n", cfitr->second->arg_vec[0]->data.c_str());
		}
		
		conf->appendf("%s%s : ", isArray?"@":"", var->name.data.c_str());

		
		cfitr = var->cf_map.find(str_t(DEFAULT_FUNC));
		if(cfitr != var->cf_map.end() && cfitr->second->arg_list.size() > 0){
			conf->appendf("%s", cfitr->second->arg_vec[0]->data.c_str());
		}
		
		conf->append("\n");
	}

	void printGroup(confIDL::idm_t * idm, confIDL::var_t * var, str_t * conf, str_t * section, bool isArray){
		confIDL::group_map_t::iterator gitr = idm->group_map.find(var->type.data);
		if(gitr == idm->group_map.end()){
			throw ConfigException() << BSL_EARG << "Unknown type : " << var->type.data.c_str() << " [ " 
				<< var->type.file.c_str() << " : " << var->type.lineno << " }";
		}
		if(*section == str_t(CONFIG_GLOBAL)){
			*section = "";
			section->appendf("%s%s", isArray?"@":"", var->name.data.c_str());
		}
		else{
			if(strstr(section->c_str(), "@") != NULL){
				
				str_t tmp;
				for(int i = 0; i < (int)section->size(); ++i){
					if((*section)[i] == '.'){
						tmp.append(".");
					}
				}
				*section = tmp;
			}
			section->appendf(".%s%s", isArray?"@":"", var->name.data.c_str());
		}
		conf->appendf("\n[%s]\n", section->c_str());
		confIDL::var_list_t::iterator vitr;
		str_t bak_section = *section;
		for(vitr = gitr->second->var_list.begin(); vitr != gitr->second->var_list.end(); ++vitr){
			if(isBaseType((*vitr)->type.data)){
				printVar(idm, *vitr, conf, section);
			}
		}
		for(vitr = gitr->second->var_list.begin(); vitr != gitr->second->var_list.end(); ++vitr){
			if(! isBaseType((*vitr)->type.data)){
				if(*section != bak_section){
					*section = bak_section;
					
				}
				printVar(idm, *vitr, conf, section);
			}
		}
	}	



	void printVar(confIDL::idm_t * idm, confIDL::var_t * var, str_t * conf, str_t * section){
		bool isArray = false;
		int arrSize = 1;

		confIDL::cf_map_t::iterator cfitr = var->cf_map.find(str_t(ARRAY_FUNC));
		if(cfitr != var->cf_map.end()){
			isArray = true;
			unsigned long long s;
			if(cfitr->second->arg_list.size() > 0){
				if(Trans::str2uint64(cfitr->second->arg_vec[0]->data, &s) == 0){
					arrSize = (int)s;
				}
			}
		}

		if(arrSize <= 0){
			arrSize = 1;
		}

		str_t bak_section = *section;
		
		if(isBaseType(var->type.data)){
			printBaseType(idm, var, conf, section, isArray);
		}
		else{
			printGroup(idm, var, conf, section, isArray);
		}
		
		if (1 < arrSize)
		{
			
			str_t cmt = "";
			str_t cmted = "";
			*section = bak_section;
			if(isBaseType(var->type.data)){
				printBaseType(idm, var, &cmt, section, isArray);
			}
			else{
				cmted.append("\n");
				printGroup(idm, var, &cmt, section, isArray);
			}
			cmt.append("\n");
			
			int prelinehead = 0;
			for (int i=0; i<(int) cmt.size(); ++i)
			{
				if ('\n'==cmt[i])
				{
					if (i>prelinehead){
						cmted.append("#").append(cmt.slcstr(prelinehead, i - prelinehead + 1));
					}
					prelinehead = i+1;
				}
			}
			
			conf->append(cmted);
		}
	}


	str_t autoConfigGen(const char * range_file){
		confIDL::idm_t * idm;
		idm = confIDL::alloc_idm();

		load_idm(range_file, idm);
		if(idm->retval != 0){
			confIDL:: err_list_t :: iterator itr;
			ConfigException e;
			e << range_file << ": Read File Error\n";
			for(itr = idm->err_list.begin(); itr != idm->err_list.end(); ++itr){
				e << itr->file.c_str() << ":" << itr->lineno << " " << itr->data.c_str() << "\n";
			}
			free_idm(idm);
			throw e;
		}

		confIDL :: var_list_t :: iterator vitr;
		str_t conf;
		str_t section = CONFIG_GLOBAL;
		for(vitr = idm->var_list.begin(); vitr != idm->var_list.end(); ++vitr){
			if(isBaseType((*vitr)->type.data) && section != CONFIG_GLOBAL){
				conf.append("\n[" CONFIG_GLOBAL "]\n");
			}
			section = CONFIG_GLOBAL;
			printVar(idm, *vitr, &conf, &section);
		}
		free_idm(idm);
		return conf;
	}












}


