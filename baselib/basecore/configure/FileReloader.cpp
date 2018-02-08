
#include "FileReloader.h"
#include <time.h>

namespace comcfg {

static time_t get_file_time(const char * filename) {

	if (NULL == filename) {
		ul_writelog(UL_LOG_WARNING, "FileReloader.add_file_monitor : NULL filename");
		return -1;
	}
	time_t t = time_t(0);
	struct stat st;
	if(stat(filename, &st) == 0){
		if(t < st.st_mtime){
			t = st.st_mtime;
		}
	} else {
		ul_writelog(UL_LOG_WARNING, "FileReloader add_file_monitor : Can not stat file [%s]",
				filename);
		return -1;
	}
	return t;
}

int FileReloader :: monitor() {
	int ret = 0;
	std::map <Lsc::string, config_filemonitor_t *> :: iterator  itr;
	for(itr = _filemap.begin(); itr != _filemap.end(); ++itr){
		config_filemonitor_t * fm = itr->second;
		time_t tnow = get_file_time(itr->first.c_str());
		if (tnow<0) {
			return -1;
		}
		if (tnow > fm->_last_modify) {
			fm->_last_modify = tnow;
			fm->callback(itr->first.c_str(), fm->param);
			++ret;
		}
	}
	return ret;
}

int FileReloader :: add_file_monitor(const char * filename, file_change_cb_t proc, void * prm) {
	if (NULL == filename) {
		ul_writelog(UL_LOG_WARNING, "FileReloader.add_file_monitor : NULL filename");
		return -1;
	}
	if (NULL == proc) {
		ul_writelog(UL_LOG_WARNING, "FileReloader.add_file_monitor : NULL callback");
		return -1;
	}
	time_t t = get_file_time(filename);
	if (0 > t) {
		return -1;
	}

	config_filemonitor_t * pmon = new config_filemonitor_t;
	pmon->callback = proc;
	pmon->param = prm;
	pmon->_last_modify = t;

	Lsc::string kstr = filename;
	std::map <Lsc::string, config_filemonitor_t *> :: iterator itr;
	itr = _filemap.find(kstr);
	if(itr != _filemap.end()) {
		if (NULL != itr->second) {
			delete itr->second;
		}
		_filemap.erase(itr);
		ul_writelog(UL_LOG_WARNING, "FileReloader.add_file_monitor : "
				"same file(%s) exist, update callback(%p), param(%p)",filename, proc, prm);
	}

	_filemap[kstr] = pmon;
	return 0;
}

FileReloader :: ~FileReloader() {
	std::map <Lsc::string, config_filemonitor_t *> :: iterator  itr;
	for(itr = _filemap.begin(); itr != _filemap.end(); ++itr){
		if (NULL != itr->second) {
			delete itr->second;
		}
	}
}

}
