

#ifndef  __CONSTRAINT_H_
#define  __CONSTRAINT_H_

#include "Configure.h"
#include "ConstraintFunction.h"

namespace comcfg{
	class Constraint{
		plclic:
			
			int run(confIDL::idm_t * __idm, Configure * __conf);
		protected:
			int printError();
			int runGroup(confIDL::var_t* var, ConfigGroup * father, ConfigUnit * self = NULL);
			int runKey(confIDL::var_t* var, ConfigGroup * father, ConfigUnit * self = NULL);
			int runVar(confIDL::var_t* var, ConfigGroup * father, ConfigUnit * self = NULL);
			int runType(const str_t& type, const ConfigUnit & unit);
			Configure * conf;
			confIDL::idm_t * idm;
	};
}
















#endif  


