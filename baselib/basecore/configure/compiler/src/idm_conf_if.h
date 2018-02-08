#ifndef _IDL_CONF_IF_H_
#define _IDL_CONF_IF_H_

#include <string>
#include <vector>
#include <list>
#include <map>
#include "Lsc/containers/string/Lsc_string.h"

namespace confIDL{

  typedef Lsc::string idm_str_t;
  struct meta_t
  {
    idm_str_t file;
    int lineno;
    idm_str_t data;
  };
  typedef std::list<meta_t*> meta_list_t;
  typedef std::vector<meta_t *>meta_vec_t;
  typedef std::list<meta_t> err_list_t;


  struct cons_func_t{
    meta_t func;
    meta_list_t arg_list;
    meta_vec_t arg_vec;
  };
  typedef cons_func_t cf_t;
  typedef std::list<cf_t*> cf_list_t;
  typedef std::map<idm_str_t,cf_t *>cf_map_t;

  typedef std::list<cf_list_t*> cf_list_p;
  typedef std::list<meta_list_t*> meta_list_p;


  struct var_t{
    meta_t name;
    meta_t type;
    cf_list_t cf_list;
    cf_map_t cf_map;
  };
  typedef std::list<var_t*>var_list_t;
  typedef std::map<idm_str_t,var_t*>var_map_t;

  typedef std::list<var_list_t*> var_list_p;


  struct group_t{
    meta_t name;
    meta_t parent_name;
    var_list_t var_list;
    var_map_t var_map;
  };
  typedef std::map<idm_str_t,group_t*>group_map_t;
  typedef std::list<group_t*>group_list_t;

  struct idm_t
  {
    group_map_t group_map;
    var_map_t var_map;
    group_list_t group_list; 
    var_list_t var_list;
    
    
    meta_list_t rc_meta_list;
    cf_list_t rc_cf_list;
    var_list_t rc_var_list;
    group_list_t rc_group_list;
  
	int cmdreserve;
    int retval;
    err_list_t err_list;

	cf_list_p parse_list; 
	meta_list_p parse_meta_list; 
	var_list_p parse_var_list; 
  };


  
  extern confIDL::idm_t *alloc_idm();

  
  extern void free_idm(confIDL::idm_t *idm);

  
  extern void load_idm(const char *file,confIDL::idm_t *idm);

  
  extern void print_idm(confIDL::idm_t *idm);

};

#endif

