#ifndef _IDL_H_
#define _IDL_H_

#include "idm_conf_if.h"
using namespace confIDL;


extern meta_t *alloc_meta(idm_t *idm);


extern cf_t *alloc_cf(idm_t *idm);


extern var_t *alloc_var(idm_t *idm);



extern group_t *alloc_group(idm_t *idm);



extern void add_idm_error(idm_t *idm,meta_t &errinfo);


extern void add_idm_var(idm_t *idm,var_t *var);


extern void add_idm_group(idm_t *idm,group_t *grp);


extern void assemble_idm_cf(cf_t *cf,meta_t *fn,meta_list_t *arg_list);


extern void assemble_idm_var(idm_t *idm,var_t *var,meta_t *id,meta_t *type,cf_list_t *cf_list);




extern void assemble_idm_group(idm_t *idm,group_t *group,meta_t *id,\
			       var_list_t *varlist);


extern void overwrite_idm_group(idm_t *idm,group_t *group,\
				meta_t *new_id,meta_t *old_id,\
				var_list_t *varlist);


#endif

