

#ifndef FILERELOADER_H_
#define FILERELOADER_H_

#include <map>
#include <ul_log.h>
#include "Lsc/containers/string/Lsc_string.h"

namespace comcfg {

typedef int (*file_change_cb_t)(const char * filename, void * prm);    

typedef struct _config_filemonitor_t {
	file_change_cb_t callback;  
	void * param; 
	time_t _last_modify;  
} config_filemonitor_t;

class FileReloader {
	typedef std::map <Lsc::string, config_filemonitor_t *> config_filemonmap_t;
private :
	config_filemonmap_t _filemap;
plclic :
	
	int monitor();
	
	int add_file_monitor(const char * filename, file_change_cb_t proc, void * prm);
	
	~FileReloader();
};

}

#endif 
