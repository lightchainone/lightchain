

#ifndef CONFIGRELOADER_H_
#define CONFIGRELOADER_H_

#include "Configure.h"

namespace comcfg {

typedef int (*key_change_cb_t)(const ConfigUnit &keyold, \
		const ConfigUnit &keynew, void * prm);    

typedef struct _load_param_t {
	Lsc::string path;    
	Lsc::string filename;    
	Lsc::string rangename;  
	int version;  
}load_param_t ;

typedef struct _config_monitor_t {
	key_change_cb_t callback;
	void * param;
} config_monitor_t;

class ConfigReloader {
	typedef std::map <Lsc::string, config_monitor_t *> config_monmap_t;
	typedef std::map <Lsc::string, config_monitor_t *> :: iterator config_monmap_itr_t;
private:
	int _finit; 
	load_param_t _load_param;  
	Configure * _config[2];  
	int _config_curid;  
	time_t _last_modify;  
	std::vector <config_monitor_t *> _filecblist;  
	std::map <Lsc::string, config_monitor_t *> _chrcbmap;  
	std::map <Lsc::string, config_monitor_t *> _strcbmap;  
	std::map <Lsc::string, config_monitor_t *> _intcbmap;  
	std::map <Lsc::string, config_monitor_t *> _uint64cbmap;  
	std::map <Lsc::string, config_monitor_t *> _floatcbmap;  
	std::map <Lsc::string, config_monitor_t *> _groupcbmap;  

	
	int _add_key_monitor(config_monmap_t * map, const char * key, key_change_cb_t proc, void * prm);
	
	void _free_map(std::map <Lsc::string, config_monitor_t *> *map);
	
	void _free_list(std::vector <config_monitor_t *> *list);

	
	int reload();
	
	int check();
plclic :
	
	int init(const char * path, const char * conf, const char * range = NULL, int version = CONFIG_VERSION_1_0);
	
	int monitor();
	
	int add_key_int(const char * key, key_change_cb_t proc, void * prm);
	
	int add_key_uint64(const char * key, key_change_cb_t proc, void * prm);
	
	int add_key_float(const char * key, key_change_cb_t proc, void * prm);
	
	int add_key_char(const char * key, key_change_cb_t proc, void * prm);
	
	int add_key_string(const char * key, key_change_cb_t proc, void * prm);
	
	int add_group_monitor(const char * group, key_change_cb_t proc, void * prm);
	
	int add_file_monitor(key_change_cb_t proc, void * prm);
	
	Configure * get_config();

	ConfigReloader();
	~ConfigReloader();
};



}


#endif 
