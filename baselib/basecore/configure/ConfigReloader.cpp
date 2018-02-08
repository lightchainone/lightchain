

#include "ConfigReloader.h"
#include <time.h>
#include <Lsc/exception/Lsc_exception.h>

namespace comcfg {

#define COMCFG_SAFEFREE(obj) do {if (NULL != (obj)) { delete (obj); (obj)=NULL; }} while(0)

ConfigReloader :: ConfigReloader() : _finit(0), _last_modify(0)  {
	_config[0] = 0;
	_config[1] = 0;
	_config_curid = 0;
	_load_param.filename = "";
	_load_param.rangename = "";
	_load_param.path = "";
	_load_param.version = CONFIG_VERSION_1_0;
}


int ConfigReloader :: init(const char * path, const char * conf,
		const char * range, int version) {
	if(path == NULL || conf == NULL){
		ul_writelog(Log::warning(), "ConfigReloader.init : path or filename error...");
		return -1;
	}
	if (_finit) {
		ul_writelog(Log::warning(), "ConfigReloader.init : already init. Can not init twice.");
		return -1;
	}
	_load_param.filename = conf;
	if (NULL != range) {
		_load_param.rangename = range;
	} else {
		_load_param.rangename = "";
	}
	_load_param.path = path;
	_load_param.version = version;
	int ret=0;
	_config[_config_curid] = new comcfg::Configure;
	ret = _config[_config_curid]->load(path, conf,range,version);
	if (0 != ret) {
		COMCFG_SAFEFREE(_config[_config_curid]);
		return ret;
	}
	_last_modify = _config[_config_curid]->lastConfigModify();
	if (0 == _last_modify) {
		ul_writelog(Log::warning(), "ConfigReloader.init : get last modify time error...");
		COMCFG_SAFEFREE(_config[_config_curid]);
		return -1;
	}
	if (0 == ret) {
		_finit = 1;
	}
	return ret;
}

int ConfigReloader :: _add_key_monitor(config_monmap_t * map, const char * key,
		key_change_cb_t proc, void * prm) {
	if(key == NULL || NULL == proc){
		ul_writelog(Log::warning(), "ConfigReloader._add_key_monitor : NULL key or callback");
		return -1;
	}
	Lsc::string kstr = key;
	config_monitor_t * pmon = new config_monitor_t;
	pmon->callback = proc;
	pmon->param = prm;
	config_monmap_itr_t itr;
	itr = map->find(kstr);
	if(itr != map->end()) {
		if (NULL != itr->second) {
			delete itr->second;
		}
		map->erase(itr);
		ul_writelog(Log::warning(), "ConfigReloader._add_key_monitor : "
				"same key exist, update callback(%p), param(%p)",proc, prm);
	}
	(*map)[kstr] = pmon;
	return 0;
}

int ConfigReloader :: reload() {
	int nextid = (_config_curid + 1) % 2;
	COMCFG_SAFEFREE(_config[nextid]);
	_config[nextid] = new comcfg :: Configure;
	int ret = _config[nextid]->load(_load_param.path.c_str(), _load_param.filename.c_str(),
			_load_param.rangename.c_str(), _load_param.version);
	if (0 != ret) {
		COMCFG_SAFEFREE(_config[nextid]);
		return -1;
	}
	_config_curid = nextid;
	return ret;
}

int ConfigReloader :: check() {
	int oldid = (_config_curid + 1) % 2;
	try {
		int ret = 0;
		std::map <Lsc::string, config_monitor_t *> :: iterator  itr;
		
		for(itr = _chrcbmap.begin(); itr != _chrcbmap.end(); ++itr){
			_config[_config_curid]->deepGet(itr->first).to_char();
		}
		for (itr = _strcbmap.begin(); itr != _strcbmap.end(); ++itr) {
			_config[_config_curid]->deepGet(itr->first).to_Lsc_string();
		}
		for (itr = _intcbmap.begin(); itr != _intcbmap.end(); ++itr) {
			_config[_config_curid]->deepGet(itr->first).to_int64();
		}
		for (itr = _uint64cbmap.begin(); itr != _uint64cbmap.end(); ++itr) {
			_config[_config_curid]->deepGet(itr->first).to_uint64();
		}
		for (itr = _floatcbmap.begin(); itr != _floatcbmap.end(); ++itr) {
			_config[_config_curid]->deepGet(itr->first).to_dolcle();
		}
		for (itr = _groupcbmap.begin(); itr != _groupcbmap.end(); ++itr) {
			int ty = _config[_config_curid]->deepGet(itr->first).selfType();
			if (CONFIG_GROUP_TYPE != ty && CONFIG_ARRAY_TYPE != ty) {
				ul_writelog(Log::warning(), "ConfigReloader.add_group_monitor :"
						" (%s) is not group or array", itr->first.c_str());
				return -1;
			}
		}


		
		for(itr = _chrcbmap.begin(); itr != _chrcbmap.end(); ++itr){
			if (NULL != itr->second) {
				config_monitor_t * pmon = (config_monitor_t *) (itr->second);
				if (_config[_config_curid]->deepGet(itr->first).to_char() !=
						_config[oldid]->deepGet(itr->first).to_char()) {
					pmon->callback(_config[oldid]->deepGet(itr->first),
							_config[_config_curid]->deepGet(itr->first), pmon->param);
					++ret;
				}
			}
		}
		for (itr = _strcbmap.begin(); itr != _strcbmap.end(); ++itr) {
			if (NULL != itr->second) {
				config_monitor_t * pmon = (config_monitor_t *) (itr->second);
				if (_config[_config_curid]->deepGet(itr->first).to_Lsc_string()
						!= _config[oldid]->deepGet(itr->first).to_Lsc_string()) {
					pmon->callback(_config[oldid]->deepGet(itr->first),
							_config[_config_curid]->deepGet(itr->first),
							pmon->param);
					++ret;
				}
			}
		}
		for (itr = _intcbmap.begin(); itr != _intcbmap.end(); ++itr) {
			if (NULL != itr->second) {
				config_monitor_t * pmon = (config_monitor_t *) (itr->second);
				if (_config[_config_curid]->deepGet(itr->first).to_int64()
						!= _config[oldid]->deepGet(itr->first).to_int64()) {
					pmon->callback(_config[oldid]->deepGet(itr->first),
							_config[_config_curid]->deepGet(itr->first),
							pmon->param);
					++ret;
				}
			}
		}
		for (itr = _uint64cbmap.begin(); itr != _uint64cbmap.end(); ++itr) {
			if (NULL != itr->second) {
				config_monitor_t * pmon = (config_monitor_t *) (itr->second);
				if (_config[_config_curid]->deepGet(itr->first).to_uint64()
						!= _config[oldid]->deepGet(itr->first).to_uint64()) {
					pmon->callback(_config[oldid]->deepGet(itr->first),
							_config[_config_curid]->deepGet(itr->first),
							pmon->param);
					++ret;
				}
			}
		}
		for (itr = _floatcbmap.begin(); itr != _floatcbmap.end(); ++itr) {
			if (NULL != itr->second) {
				config_monitor_t * pmon = (config_monitor_t *) (itr->second);
				if (_config[_config_curid]->deepGet(itr->first).to_dolcle()
						!= _config[oldid]->deepGet(itr->first).to_dolcle()) {
					pmon->callback(_config[oldid]->deepGet(itr->first),
							_config[_config_curid]->deepGet(itr->first),
							pmon->param);
					++ret;
				}
			}
		}
		for (itr = _groupcbmap.begin(); itr != _groupcbmap.end(); ++itr) {
			if (NULL != itr->second) {
				config_monitor_t * pmon = (config_monitor_t *) (itr->second);
				if (0 != _config[_config_curid]->deepGet(itr->first)
						.equals(_config[oldid]->deepGet(itr->first)) ) {
					pmon->callback(_config[oldid]->deepGet(itr->first),
							_config[_config_curid]->deepGet(itr->first),
							pmon->param);
					++ret;
				}
			}
		}
		for(int i = 0; i < (int)_filecblist.size(); ++i){
			config_monitor_t * pmon =  _filecblist[i];
			if (NULL != pmon && NULL!= pmon->callback) {
				pmon->callback(*(_config[oldid]), *(_config[_config_curid]), pmon->param);
				++ret;
			}
		}
		return ret;
	} catch (Lsc::Exception &e) {
		ul_writelog(Log::warning(), "ConfigReloader.check : error(%s)", e.what());
		return -1;
	} catch (...) {
		ul_writelog(Log::warning(), "ConfigReloader.check : unknown error");
		return -1;
	}
	ul_writelog(Log::warning(), "ConfigReloader.check : unknown error, the codes lost road.");
	return -1;
}


int ConfigReloader :: monitor() {
	if (NULL == _config[_config_curid]) {
		ul_writelog(Log::warning(), "ConfigReloader.monitor : NULL config(%d), init failed?", _config_curid);
		return -2;
	}
	time_t modtime = _config[_config_curid]->lastConfigModify();
	if (modtime > _last_modify)  {
		if (0 != reload()) {
			ul_writelog(Log::warning(), "ConfigReloader.monitor : reload error(%s)", _load_param.filename.c_str());
			return -1;
		}
		int ret =  check();
		if (-1 == ret) {
			ul_writelog(Log::warning(), "ConfigReloader.monitor : check error(%s)", _load_param.filename.c_str());
			int oldid = _config_curid;
			_config_curid = (_config_curid + 1) % 2;
			COMCFG_SAFEFREE(_config[oldid]);
			return -2;
		}
		_last_modify = modtime;
		return ret;
	} else {
		return 0;
	}
	ul_writelog(Log::warning(), "ConfigReloader.monitor : unknown error(%s)", _load_param.filename.c_str());
	return -2;
}

int ConfigReloader :: add_key_int(const char * key, key_change_cb_t proc, void * prm) {
	try {
		if (NULL == key) {
			ul_writelog(Log::warning(), "ConfigReloader.add_key_int : NULL key.");
			return -1;
		}
		if (NULL == _config[_config_curid]) {
			ul_writelog(Log::warning(), "ConfigReloader.add_key_int : NULL config. init failed?");
			return -1;
		}
		_config[_config_curid]->deepGet(key).to_int64();
	} catch (Lsc::Exception &e) {
		ul_writelog(Log::warning(), "ConfigReloader.add_key_int : error(%s)", e.what());
		return -1;
	} catch (...) {
		ul_writelog(Log::warning(), "ConfigReloader.add_key_int : unknown error");
		return -1;
	}
	return _add_key_monitor(&_intcbmap, key, proc, prm);
}

int ConfigReloader :: add_key_uint64(const char * key, key_change_cb_t proc, void * prm) {
	try {
		if (NULL == key) {
			ul_writelog(Log::warning(), "ConfigReloader.add_key_uint64 : NULL key.");
			return -1;
		}
		if (NULL == _config[_config_curid]) {
			ul_writelog(Log::warning(), "ConfigReloader.add_key_uint64 : NULL config. init failed?");
			return -1;
		}
		_config[_config_curid]->deepGet(key).to_uint64();
	} catch (Lsc::Exception &e) {
		ul_writelog(Log::warning(), "ConfigReloader.add_key_uint64 : error(%s)", e.what());
		return -1;
	} catch (...) {
		ul_writelog(Log::warning(), "ConfigReloader.add_key_uint64 : unknown error");
		return -1;
	}
	return _add_key_monitor(&_uint64cbmap, key, proc, prm);
}

int ConfigReloader :: add_key_float(const char * key, key_change_cb_t proc, void * prm) {
	try {
		if (NULL == key) {
			ul_writelog(Log::warning(), "ConfigReloader.add_key_float : NULL key.");
			return -1;
		}
		if (NULL == _config[_config_curid]) {
			ul_writelog(Log::warning(), "ConfigReloader.add_key_float : NULL config. init failed?");
			return -1;
		}
		_config[_config_curid]->deepGet(key).to_dolcle();
	} catch (Lsc::Exception &e) {
		ul_writelog(Log::warning(), "ConfigReloader.add_key_float : error(%s)", e.what());
		return -1;
	} catch (...) {
		ul_writelog(Log::warning(), "ConfigReloader.add_key_float : unknown error");
		return -1;
	}
	return _add_key_monitor(&_floatcbmap, key, proc, prm);
}

int ConfigReloader :: add_key_char(const char * key, key_change_cb_t proc, void * prm) {
	try {
		if (NULL == key) {
			ul_writelog(Log::warning(), "ConfigReloader.add_key_char : NULL key.");
			return -1;
		}
		if (NULL == _config[_config_curid]) {
			ul_writelog(Log::warning(), "ConfigReloader.add_key_char : NULL config. init failed?");
			return -1;
		}
		_config[_config_curid]->deepGet(key).to_char();
	} catch (Lsc::Exception &e) {
		ul_writelog(Log::warning(), "ConfigReloader.add_key_char : error(%s)", e.what());
		return -1;
	} catch (...) {
		ul_writelog(Log::warning(), "ConfigReloader.add_key_char : unknown error");
		return -1;
	}
	return _add_key_monitor(&_chrcbmap, key, proc, prm);
}

int ConfigReloader :: add_key_string(const char * key, key_change_cb_t proc, void * prm) {
	try {
		if (NULL == key) {
			ul_writelog(Log::warning(), "ConfigReloader.add_key_string : NULL key.");
			return -1;
		}
		if (NULL == _config[_config_curid]) {
			ul_writelog(Log::warning(), "ConfigReloader.add_key_string : NULL config. init failed?");
			return -1;
		}
		_config[_config_curid]->deepGet(key).to_Lsc_string();
	} catch (Lsc::Exception &e) {
		ul_writelog(Log::warning(), "ConfigReloader.add_key_string : error(%s)", e.what());
		return -1;
	} catch (...) {
		ul_writelog(Log::warning(), "ConfigReloader.add_key_string : unknown error");
		return -1;
	}
	return _add_key_monitor(&_strcbmap, key, proc, prm);
}

int ConfigReloader :: add_group_monitor(const char * group, key_change_cb_t proc, void * prm) {
	try {
		if (NULL == group) {
			ul_writelog(Log::warning(), "ConfigReloader.add_group_monitor : NULL key.");
			return -1;
		}
		if (NULL == _config[_config_curid]) {
			ul_writelog(Log::warning(), "ConfigReloader.add_group_monitor : NULL config. init failed?");
			return -1;
		}
		int ty =_config[_config_curid]->deepGet(group).selfType();
		if (CONFIG_GROUP_TYPE != ty && CONFIG_ARRAY_TYPE != ty) {
			ul_writelog(Log::warning(), "ConfigReloader.add_group_monitor :"
					" (%s) is not group or array", group);
			return -1;
		}
	} catch (Lsc::Exception &e) {
		ul_writelog(Log::warning(), "ConfigReloader.add_group_monitor : error(%s)", e.what());
		return -1;
	} catch (...) {
		ul_writelog(Log::warning(), "ConfigReloader.add_group_monitor : unknown error");
		return -1;
	}
	return _add_key_monitor(&_groupcbmap, group, proc, prm);
}

int ConfigReloader :: add_file_monitor(key_change_cb_t proc, void * prm) {
	if (NULL == _config[_config_curid]) {
		ul_writelog(Log::warning(), "ConfigReloader.add_file_monitor : NULL config. init failed?");
		return -1;
	}
	if(NULL == proc) {
		ul_writelog(Log::warning(), "ConfigReloader.add_file_monitor : NULL callback");
		return -1;
	}

	for(int i = 0; i < (int)_filecblist.size(); ++i){
		config_monitor_t * pmon =  _filecblist[i];
		if (NULL != pmon && proc == pmon->callback) {
			pmon->param = prm;
			ul_writelog(Log::warning(), "ConfigReloader.add_file_monitor : "
					"same callback exist, updated param to %p", prm);
			return -1;
		}
	}

	config_monitor_t * pmon = new config_monitor_t;
	pmon->callback = proc;
	pmon->param = prm;
	_filecblist.push_back(pmon);
	return 0;
}

Configure * ConfigReloader :: get_config() {
	return _config[_config_curid];
}


void ConfigReloader :: _free_map(std::map <Lsc::string, config_monitor_t *> *map) {
	std::map <Lsc::string, config_monitor_t *> :: iterator  itr;
	for(itr = map->begin(); itr != map->end(); ++itr){
		if (NULL != itr->second) {
			delete itr->second;
		}
	}
}
void ConfigReloader :: _free_list(std::vector <config_monitor_t *> *list) {
	for(int i = 0; i < (int)list->size(); ++i){
		if (NULL != (*list)[i]) {
			delete (*list)[i];
		}
	}
}
ConfigReloader :: ~ConfigReloader() {
	_free_map(&_chrcbmap);
	_free_map(&_strcbmap);
	_free_map(&_intcbmap);
	_free_map(&_uint64cbmap);
	_free_map(&_floatcbmap);
	_free_map(&_groupcbmap);
	_free_list(&_filecblist);
	COMCFG_SAFEFREE(_config[0]);
	COMCFG_SAFEFREE(_config[1]);
}

}
