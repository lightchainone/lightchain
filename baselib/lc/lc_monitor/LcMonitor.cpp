
#include "LcMonitor.h"
#include "SocketMonitorOutput.h"
#include "FileMonitorOutput.h"
#include "util.hpp"

#include <lc_log.h>
#include <ul_conf.h>
#include <bsl/var/String.h>
namespace lc
{

int LcMonitor :: init(const char *path, const char *file)
{
    if (NULL == path || NULL == file) {
        LC_LOG_WARNING("path and file is NULL, forbidden");
        return -1;
    }

    ul_confdata_t *tmp_conf = ul_initconf(1024);
    if (NULL == tmp_conf) {
        LC_LOG_WARNING("open conf file:%s/%s error", path, file);
        return -1;
    }
    if (ul_readconf_ex(path, file, tmp_conf) == -1) {
        LC_LOG_WARNING("load conf file:%s/%s error", path, file);
        ul_freeconf(tmp_conf);
        return -1;
    }

    int ret = init(tmp_conf);
    ul_freeconf(tmp_conf);
    return ret;
}

int LcMonitor :: init(ul_confdata_t *tmp_conf)
{
    
    int device_num = 0;
    if (ul_getconfint(tmp_conf, "lcmonitor_output_num", &device_num) == 0) {
        LC_LOG_WARNING("no output device in config, forbidden");
        return -1;
    }

    if (device_num <= 0) {
        LC_LOG_WARNING("device num < 0, forbidden");
        return -1;
    }

    int i_conf = 0;
    if (ul_getconfint(tmp_conf, "lcmonitor_compute_time_ms", &i_conf) != 1) {
        LC_LOG_NOTICE("using default lcmonitor_compute_time_ms : %d", 5000);
        i_conf = 5000;
    }
    _compute_time_ms = i_conf;
 
    if (ul_getconfint(tmp_conf, "lcmonitor_monitor_time_ms", &i_conf) != 1) {
        LC_LOG_NOTICE("using default lcmonitor_monitor_time_ms : %d", 60000);
        i_conf = 60000;
    }
    _monitor_time_ms = i_conf;
   
    char tmp_device_key[128];
    for (int i=0; i<device_num; i++) {
        char tmp_device_name[128];
        snprintf(tmp_device_key, sizeof(tmp_device_key), "%s%d", "lcmonitor_device", i);
        if (ul_getconfnstr(tmp_conf, tmp_device_key, tmp_device_name, sizeof(tmp_device_name)) == 0) {
            
            LC_LOG_WARNING("%s value get error,forbidden", tmp_device_key);
            goto Fail;
        }
        MonitorOutput *output = NULL;
        if (strcmp(tmp_device_name, "SOCKET") == 0) {
            output = new (std::nothrow)SocketMonitorOutput(this);
        } else if (strcmp(tmp_device_name, "FILE") == 0) {
            output = new (std::nothrow)FileMonitorOutput(this);
        } else {
            LC_LOG_WARNING("Monitor Output Type:%s not support", tmp_device_name);
            continue;
        }
        if (NULL == output) {
            LC_LOG_WARNING("new MonitorOutput error[%m]");
            goto Fail;
        }
        if (output->init(tmp_conf) != 0) {
            LC_LOG_WARNING("MonitorOutput init error");
            delete output;
            output = NULL;
            goto Fail;
        }
        _output_list.push_back(output);
    }
    gen_compute_thread_info();
    return 0;
Fail:
    int item_size = _output_list.size();
    for (int i=0; i<item_size; i++) {
        MonitorOutput *output = _output_list[i];
        if (output) {
            delete output;
            output = NULL;
        }
    }
    _output_list.clear();
    return -1;
}

int LcMonitor :: register_service(const char *service_name, int thread_num)
{
    
    if (NULL == service_name || strcmp(service_name, "") == 0) {
        LC_LOG_WARNING("service_name is NULL or empty, forbidden");
        return -1;
    }

    if (thread_num <= 0) {
        LC_LOG_WARNING("thread num:%d <= 0", thread_num);
        return -1;
    }

    int errcode = 0;

	
	
	
	
    bsl::var::IVar &global_wellknow_tree = gen_service_monitor_var(_rp, &errcode);
    if (errcode != 0) {
        LC_LOG_WARNING("gen service monitor info error");
        return -1;
    }
    bsl::var::IVar &global_wellknow_total_tree = gen_service_monitor_var(_rp, &errcode);
    if (errcode != 0) {
        LC_LOG_WARNING("gen service monitor info error");
        return -1;
    }
    bsl::var::IVar &global_wellknow_time_tree = gen_service_monitor_var1(_rp, &errcode);
    if (errcode != 0) {
        LC_LOG_WARNING("get service monitor info from time error");
        return -1;
    }

	
	
    if (_service_thread_map.set(service_name, thread_num, 1) == -1) {
        LC_LOG_WARNING("add service_name:%s thread_num:%d error", service_name, thread_num);
        return -1;
    }


	
	

	
	

    int list_size = _thread_info_list.size();
    int max_size = list_size<thread_num?list_size:thread_num;

    for (int i=0; i<max_size; i++) {
        thread_info *tinfo = _thread_info_list[i];
        if (NULL == tinfo) {
            continue;
        }

		bsl::var::IVar &thread_wellknow_tree = gen_service_monitor_var(tinfo->rp, &errcode);
		if (errcode != 0) {
			LC_LOG_WARNING("gen service monitor info for thread error");
			return -1;
		}
		tinfo->monitor_info[service_name] = thread_wellknow_tree;

		bsl::var::IVar &thread_wellknow_time_tree = gen_service_monitor_var1(tinfo->rp, &errcode);
		if (errcode != 0) {
			LC_LOG_WARNING("gen service monitor info for thread error");
			return -1;
		}
		tinfo->monitor_info_time[service_name] = thread_wellknow_time_tree;
    }

    if (list_size < thread_num) {
        for (int i=0; i<thread_num-list_size; i++) {
            try {
                thread_info &tinfo = _rp.create<thread_info>();
                tinfo.monitor_info = tinfo.rp.create<bsl::var::Dict>();

                bsl::var::IVar &thread_wellknow_tree = gen_service_monitor_var(tinfo.rp, &errcode);
                if (errcode != 0) {
					LC_LOG_WARNING("gen service monitor info for thread error");
					return -1;
				}
                tinfo.monitor_info[service_name] = thread_wellknow_tree;

                tinfo.monitor_info_time = tinfo.rp.create<bsl::var::Dict>();

				
                bsl::var::IVar &thread_wellknow_time_tree = gen_service_monitor_var1(tinfo.rp, &errcode);
                if (errcode != 0) {
					LC_LOG_WARNING("gen service monitor info for thread error");
					return -1;
				}
                tinfo.monitor_info_time[service_name] = thread_wellknow_time_tree;

                _thread_info_list.push_back(&tinfo);
            } catch (bsl::Exception &e) {
                LC_LOG_WARNING("ResourcePool create failed[%m]:%s", e.all());
                return -1;
            } catch (...) {
                LC_LOG_WARNING("catch unknown exception");
                return -1;
            }
        }
    }

    _monitor_info[service_name]       = global_wellknow_tree;
    _monitor_info_total[service_name] = global_wellknow_total_tree;
    _monitor_info_time[service_name]  = global_wellknow_time_tree;
    
    

    return 0;
}

int LcMonitor :: register_monitor_item(const char *path, bsl::var::Function::function_type callback, void *arg, int type)
{
    
    if (NULL == path || NULL == callback) {
        LC_LOG_WARNING("paht or callback is NULL, forbidden");
        return -1;
    }

    if(type < 0 || type > 1) {
        LC_LOG_WARNING("type must be [0, 1]");
        return -1;
    }

    int err_code = 0;
    char last_name[256];
    bsl::var::IVar &path_item = query_path(_monitor_info, path, _rp, &err_code, 1, last_name, sizeof(last_name));
    if (err_code != 0) {
        LC_LOG_WARNING("register path:%s errro", path);
        return -1;
    }
    try {
		path_item[last_name] = create_function_object(_rp, callback, arg, type);
    } catch (bsl::Exception &e) {
        LC_LOG_WARNING("%s", e.all());
        return -1;
    }
    return 0;
}



int LcMonitor :: gen_compute_thread_info()
{
    if (set_service_thread_key("REQUEST_NUMBER.SUCCESS") != 0) {
        goto Fail;
    }
    if (set_service_thread_key("REQUEST_NUMBER.FAILED") != 0) {
        goto Fail;
    }
    if (set_service_thread_key("REQUEST_NUMBER.TOTAL_SUCCESS") != 0) {
        goto Fail;
    }
    if (set_service_thread_key("REQUEST_NUMBER.TOTAL_FAILED") != 0) {
        goto Fail;
    }

    if (set_service_thread_key("REQUEST_CONNECT_NUMBER") != 0) {
        goto Fail;
    }

    if (set_service_thread_key("REQUEST_CONNECT_TOTAL_NUMBER") != 0) {
        goto Fail;
    }

    if (set_service_thread_key("QUERY_CONNECT_NUMBER") != 0) {
        goto Fail;
    }

    if (set_service_thread_key("CONNECT_FAIL_NUMBER") != 0) {
        goto Fail;
    }

    if (set_service_thread_key("READ_ERROR_NUMBER") != 0) {
        goto Fail;
    }

    if (set_service_thread_key("WRITE_ERROR_NUMBER") != 0) {
        goto Fail;
    }

    if (set_service_thread_key("COMPUTE_TIME") != 0) {
        goto Fail;
    }

    if (set_service_thread_key("SELF_PROCESS_TIME") != 0) {
        goto Fail;
    }

    if (set_service_thread_key("SELF_ACTION_TIME") != 0) {
        goto Fail;
    }

    if (set_service_thread_key("IO_READ_TIME") != 0) {
        goto Fail;
    }

    if (set_service_thread_key("IO_WRITE_TIME") != 0) {
        goto Fail;
    }

    if (set_service_thread_key("FATAL_NUMBER") != 0) {
        goto Fail;
    }
    return 0;
Fail:
    return -1;
}

int LcMonitor :: set_service_thread_key(const char *key, long long value)
{
    bsl::hashmap<bsl::string, long long> :: iterator it1;
    for (it1=_compute_thread_info.begin(); it1!=_compute_thread_info.end(); it1++) {
        if (strstr(key, it1->first.c_str()) != NULL || strstr(it1->first.c_str(), key) != NULL) {
            _compute_thread_info.set(it1->first, -1, 1);
        }
    }

    if (_compute_thread_info.set(key, value, 1) == -1) {
        LC_LOG_WARNING("set %s:%lld error", key, value);
        return -1;
    }
    return 0;
}

void *LcMonitor :: compute_func(void *arg)
{
    LcMonitor *monitor = (LcMonitor*)arg;
    monitor->do_compute();
    return NULL;
}

void LcMonitor :: do_compute()
{
    lc_log_initthread("compute_func");
	
    
    
    while (_compute_run) {
        
        bsl::hashmap<bsl::string, int>::iterator it = _service_thread_map.begin();
        for(; it!=_service_thread_map.end(); ++it) {
            
            int thread_num = (*it).second;
            for (int i=0; i<thread_num; i++) {
                thread_info *tinfo = _thread_info_list[i];
                bsl::hashmap<bsl::string, long long>::iterator it1 = _compute_thread_info.begin();
                int errcode = 0;
                for (;it1!=_compute_thread_info.end(); ++it1) {
                    if ((*it1).second < 0) {
                        continue;
                    }
                    bsl::string query_name = (*it).first.c_str();
					query_name = query_name.append(".").append((*it1).first.c_str());
					
                    bsl::var::IVar &global_node_total = query_path(_monitor_info_total, query_name.c_str(), _rp, &errcode);
                    if (errcode != 0) {
                        continue;
                    }

                    bsl::var::IVar &thread_node = query_path(tinfo->monitor_info, query_name.c_str(), tinfo->rp, &errcode);
                    if (errcode != 0) {
                        continue;
                    }

                    merge_var(global_node_total, thread_node);

                    bsl::var::IVar &global_node_time = query_path(_monitor_info_time, query_name.c_str(), _rp, &errcode);
                    if (errcode != 0) {
                        continue;
                    }

                    bsl::var::IVar &thread_node_time = query_path(tinfo->monitor_info_time, query_name.c_str(), tinfo->rp, &errcode);
                    if (errcode != 0) {
                        continue;
                    }
                   
                    merge_var(global_node_time, thread_node_time, 1);
                }
            }
        }

        do_average();
        ms_sleep(_compute_time_ms);
    }

    lc_log_closethread();
    return;
}

void LcMonitor :: do_average()
{
    
    
    bsl::hashmap<bsl::string, int>::iterator it = _service_thread_map.begin();
    for(; it!=_service_thread_map.end(); ++it) {
        
        bsl::hashmap<bsl::string, long long>::iterator it1 = _compute_thread_info.begin();
        int err_code = 0;
        for (;it1!=_compute_thread_info.end(); ++it1) {
            
            if ((*it1).second < 0) {
                continue;
            }
            bsl::string query_name = (*it).first.c_str();
        
            

            char l_name[1024];
            bsl::var::IVar &tmp_monitor_info = query_path(_monitor_info_total, query_name.append(".").append((*it1).first.c_str()).c_str(), _rp, &err_code, 0, l_name, sizeof(l_name));
            if (err_code != 0) {
                continue;
            }
            bsl::var::IVar &real_monitor_info = query_path(_monitor_info, query_name.c_str(), _rp, &err_code);
            if (err_code != 0) {
                continue;
            }
            bsl::var::IVar &time_monitor_info = query_path(_monitor_info_time, query_name.c_str(), _rp, &err_code);
            if (err_code != 0) {
                continue;
            }

            
            
                
            
            if (strstr(l_name, "TOTAL") != 0) {
                do_average_leaf(tmp_monitor_info[l_name], real_monitor_info[l_name], time_monitor_info[l_name], 1);
            } else {
                do_average_leaf(tmp_monitor_info[l_name], real_monitor_info[l_name], time_monitor_info[l_name], 0);
            }
            
                
            
            
            
        }
    }
    return;
}

void LcMonitor::compute_average_value(bsl::var::IVar &output_info, bsl::var::IVar &total_info, bsl::var::IVar &times_info, int total) 
{
    if (total == 0) {
        long long times = times_info.to_int64();

        if (times < 0) {
            
            int interval = get_compute_time() / 1000;
            if (interval <= 0) {
                interval = 1;
            } 
            output_info = total_info.to_int64()/interval;
            total_info = 0;
        } else if (times > 0) {
            
            output_info = total_info.to_int64() / times;
            total_info = 0;
            times_info = 0; 
        } else { 
            
            output_info = 0;
            total_info = 0;
        }
    } else {
        
        output_info = total_info.to_int64();
    }
}

void LcMonitor :: do_average_leaf(bsl::var::IVar &monitor_info, bsl::var::IVar &real_monitor_info, bsl::var::IVar & time_monitor_info, int total)
{
    
    if (real_monitor_info.is_dict()) {
        bsl::var::IVar::dict_iterator iter = real_monitor_info.dict_begin();
        bsl::var::IVar::dict_iterator end  = real_monitor_info.dict_end();
        for (; iter != end; ++iter) {
            bsl::var::IVar &child = iter->value();
            bsl::var::IVar::mask_type mask = child.get_mask();
            int total_flag = 0;
			if (strstr(iter->key().c_str(), "TOTAL") != 0)
                total_flag = 1;

            if (mask & bsl::var::IVar::IS_NUMBER) {
                try {
                    compute_average_value(child, monitor_info[iter->key()], time_monitor_info[iter->key()], total_flag);
				} catch(...) {
                    continue;
                }
            } else {
                do_average_leaf(child, monitor_info[iter->key()], time_monitor_info[iter->key()], total_flag);
            }
        }
    } else if (real_monitor_info.is_number() && monitor_info.is_number() && time_monitor_info.is_number()){
        compute_average_value(real_monitor_info, monitor_info, time_monitor_info, total); 
    } else if (real_monitor_info.is_null() || real_monitor_info.is_array()){
        
        return;
    } else {
        LC_LOG_WARNING("not support undict");
        return;
    }
}

bsl::var::IVar &
LcMonitor :: 
query_path(bsl::var::IVar &total, const char *path, bsl::ResourcePool &rp, int *err_code, int exp_tag, char *last_name, int last_name_len)
{
    

    if (NULL == err_code) {
        return bsl::var::Null::null;
    }

    *err_code = 0;

    if (NULL == path) {
        *err_code = -1;
        return bsl::var::Null::null;
    }

    if (strcmp(path, "ALL") == 0) {
        return _monitor_info;
    }

    
    char path_item[256];
    int path_item_len = 0;
    const char *path_split = NULL;
    path_split = strstr(path, ".");
    if (NULL == path_split) {
        path_item_len = strlen(path);
    } else {
        path_item_len = path_split - path;
    }
    if (path_item_len >= (int)sizeof(path_item) || path_item_len <= 0) {
        LC_LOG_WARNING("path_item:%s long than 256 or exist .., forbidden", path);
        *err_code = -1;
        return bsl::var::Null::null;
    }
    memcpy(path_item, path, path_item_len);
    path_item[path_item_len] = '\0';

    try {
        if (exp_tag != 0) {
            if (!total.is_dict()) {
                total = rp.create<bsl::var::Dict>();
            }
        }

		if (total[path_item].is_null()) {
			if (exp_tag == 0) {
				
				*err_code = -1;
				return bsl::var::Null::null;
			} else {
				total[path_item] = rp.create<bsl::var::Dict>();
			}
		}
		if (NULL == path_split) {
			if (last_name) {
				snprintf(last_name, last_name_len, "%s", path_item);
			}
			
			return total;
		} else {
            if (path_split == path+strlen(path)-1) {
                LC_LOG_WARNING(". is the last one, forbidden");
                *err_code = -1;
                return bsl::var::Null::null;
            }
			return query_path(total[path_item], path_split+1, rp, err_code, exp_tag, last_name, last_name_len);
		}
    } catch (...) {
        *err_code = -1;
        return bsl::var::Null::null;
    }
}



bsl::var::IVar& LcMonitor:: 
query_path_ex(bsl::var::IVar &total, const char *path, int *err_code, char *last_name, int last_name_len)
{
    if (NULL == err_code) {
        return bsl::var::Null::null;
    }

    *err_code = 0;

    if (NULL == path) {
        *err_code = -1;
        return bsl::var::Null::null;
    }

    if (strcmp(path, "ALL") == 0) {
        return _monitor_info;
    }

    
    char path_item[256];
    int path_item_len = 0;
    const char *path_split = NULL;
    path_split = strstr(path, ".");
    if (NULL == path_split) {
        path_item_len = strlen(path);
    } else {
        path_item_len = path_split - path;
    }
    if (path_item_len >= (int)sizeof(path_item) || path_item_len <= 0) {
        LC_LOG_WARNING("path_item:%s long than 256 or exist .., forbidden", path);
        *err_code = -1;
        return bsl::var::Null::null;
    }
    memcpy(path_item, path, path_item_len);
    path_item[path_item_len] = '\0';

    try {
		if (total[path_item].is_null()) {
			*err_code = -1;
			return bsl::var::Null::null;
		}
		if (NULL == path_split) {
			if (last_name) {
				snprintf(last_name, last_name_len, "%s", path_item);
			}
			return total[path_item];
		} else {
            if (path_split == path+strlen(path)-1) {
                LC_LOG_WARNING(". is the last one, forbidden");
                *err_code = -1;
                return bsl::var::Null::null;
            }
			return query_path_ex(total[path_item], path_split+1, err_code, last_name, last_name_len);
		}
    } catch (...) {
        *err_code = -1;
        return bsl::var::Null::null;
    }
}


bsl::var::IVar &
LcMonitor ::
query_path_outer(const char *path, bsl::ResourcePool & , int *err_code, char *last_name, int last_name_len)
{
    
    return query_path_ex(_monitor_info, path, err_code, last_name, last_name_len);
}

bsl::var::IVar &
LcMonitor::
query_path_father(bsl::var::IVar &total, const char *path, bsl::ResourcePool &rp, int *err_code, char *last_name,int last_name_len)
{
    return query_path(total, path, rp, err_code, 0, last_name, last_name_len);
}

bsl::var::IVar &
LcMonitor::
gen_service_monitor_var(bsl::ResourcePool &rp, int *err_code)
{
    if (NULL == err_code) {
        return bsl::var::Null::null;
    }

    try {
		
		bsl::var::IVar &service_info = rp.create<bsl::var::Dict>();

		service_info["REQUEST_NUMBER"] = rp.create<bsl::var::Dict>();
		bsl::var::IVar &req_num_info = service_info["REQUEST_NUMBER"];
		req_num_info["SUCCESS"] = rp.create<bsl::var::Int64>(0);
		req_num_info["FAILED"] = rp.create<bsl::var::Int64>(0);
		req_num_info["TOTAL_SUCCESS"] = rp.create<bsl::var::Int64>(0);
		req_num_info["TOTAL_FAILED"] = rp.create<bsl::var::Int64>(0);

		service_info["REQUEST_QUEUE_SIZE"] = rp.create<bsl::var::Int64>(0);

		service_info["THREAD_POOL_USAGE"] = rp.create<bsl::var::Int64>(0);

		service_info["REQUEST_CONNECT_NUMBER"] = rp.create<bsl::var::Int64>(0);
		service_info["REQUEST_CONNECT_TOTAL_NUMBER"] = rp.create<bsl::var::Int64>(0);

		service_info["QUERY_CONNECT_NUMBER"] = rp.create<bsl::var::Dict>();

		service_info["CONNECT_POOL_USAGE"] = rp.create<bsl::var::Dict>();

		service_info["CONNECT_TIME"] = rp.create<bsl::var::Dict>();

		service_info["CONNECT_FAIL_NUMBER"] = rp.create<bsl::var::Dict>();

		service_info["READ_ERROR_NUMBER"] = rp.create<bsl::var::Int64>(0);

		service_info["WRITE_ERROR_NUMBER"] = rp.create<bsl::var::Int64>(0);

		service_info["COMPUTE_TIME"] = rp.create<bsl::var::Int64>(0);

		service_info["SELF_PROCESS_TIME"] = rp.create<bsl::var::Int64>(0);

		service_info["SELF_ACTION_TIME"] = rp.create<bsl::var::Int64>(0);

		service_info["IO_READ_TIME"] = rp.create<bsl::var::Int64>(0);

		service_info["IO_WRITE_TIME"] = rp.create<bsl::var::Int64>(0);

		service_info["FATAL_NUMBER"] = rp.create<bsl::var::Int64>(0);
    
        *err_code = 0;
        return service_info;
    } catch(bsl::Exception &e) {
        LC_LOG_WARNING("catch bsl exception:%s", e.all());
        *err_code = -1;
        return bsl::var::Null::null;
    } catch(...) {
        LC_LOG_WARNING("catch unknown exception");
        *err_code = -1;
        return bsl::var::Null::null;
    }
}

bsl::var::IVar &
LcMonitor::
gen_service_monitor_var1(bsl::ResourcePool &rp, int *err_code)
{
    if (NULL == err_code) {
        return bsl::var::Null::null;
    }

    try {
		
		bsl::var::IVar &service_info = rp.create<bsl::var::Dict>();

		service_info["REQUEST_NUMBER"] = rp.create<bsl::var::Dict>();
		bsl::var::IVar &req_num_info = service_info["REQUEST_NUMBER"];
		req_num_info["SUCCESS"] = rp.create<bsl::var::Int64>(-1);
		req_num_info["FAILED"] = rp.create<bsl::var::Int64>(-1);
		req_num_info["TOTAL_SUCCESS"] = rp.create<bsl::var::Int64>(-1);
		req_num_info["TOTAL_FAILED"] = rp.create<bsl::var::Int64>(-1);

		service_info["REQUEST_QUEUE_SIZE"] = rp.create<bsl::var::Int64>(0);

		service_info["THREAD_POOL_USAGE"] = rp.create<bsl::var::Int64>(0);

		service_info["REQUEST_CONNECT_NUMBER"] = rp.create<bsl::var::Int64>(-1);
		service_info["REQUEST_CONNECT_TOTAL_NUMBER"] = rp.create<bsl::var::Int64>(-1);

		service_info["QUERY_CONNECT_NUMBER"] = rp.create<bsl::var::Dict>();

		service_info["CONNECT_POOL_USAGE"] = rp.create<bsl::var::Dict>();

		service_info["CONNECT_TIME"] = rp.create<bsl::var::Dict>();

		service_info["CONNECT_FAIL_NUMBER"] = rp.create<bsl::var::Dict>();

		service_info["READ_ERROR_NUMBER"] = rp.create<bsl::var::Int64>(-1);

		service_info["WRITE_ERROR_NUMBER"] = rp.create<bsl::var::Int64>(-1);

		service_info["COMPUTE_TIME"] = rp.create<bsl::var::Int64>(0);

		service_info["SELF_PROCESS_TIME"] = rp.create<bsl::var::Int64>(0);

		service_info["SELF_ACTION_TIME"] = rp.create<bsl::var::Int64>(0);

		service_info["IO_READ_TIME"] = rp.create<bsl::var::Int64>(0);

		service_info["IO_WRITE_TIME"] = rp.create<bsl::var::Int64>(0);

		service_info["FATAL_NUMBER"] = rp.create<bsl::var::Int64>(-1);
    
        *err_code = 0;
        return service_info;
    } catch(bsl::Exception &e) {
        LC_LOG_WARNING("catch bsl exception:%s", e.all());
        *err_code = -1;
        return bsl::var::Null::null;
    } catch(...) {
        LC_LOG_WARNING("catch unknown exception");
        *err_code = -1;
        return bsl::var::Null::null;
    }
}


int LcMonitor :: merge_var(bsl::var::IVar &result, bsl::var::IVar &input, int tag)
{
    if (result.is_number() && input.is_number()) {
        if (1 == tag) {
            if (result.to_int64() >= 0) {
                result = result.to_int64() + input.to_int64();
                input = 0;
            }
        } else {
            result = result.to_int64() + input.to_int64();
            input = 0;
        }
        return 0;
    }

    if (!result.is_dict() || !input.is_dict()) {
        return -1;
    }

    bsl::var::IVar::dict_iterator iter = input.dict_begin();
    bsl::var::IVar::dict_iterator end  = input.dict_end();
    for( ; iter != end; ++iter ){
        const char *key = iter->key().c_str();
        if (result[key].is_null()) {
            result[key] = _rp.create<bsl::var::Dict>();
        }
        merge_var(result[key], iter->value(), tag);
    }
    return 0;
}



thread_info *LcMonitor::get_thread_info()
{
    pthread_mutex_lock(&_thread_list_lock);
    int curr_num = _thread_list_usingnum;
    if (curr_num >= (int)_thread_info_list.size()) {
        pthread_mutex_unlock(&_thread_list_lock);
        LC_LOG_WARNING("service thread_num is small than using, forbidden");
        return NULL;
    }
    _thread_list_usingnum++;
    pthread_mutex_unlock(&_thread_list_lock);
    return _thread_info_list[curr_num];
}

int LcMonitor :: run()
{
    int num = _output_list.size();
    int using_num = 0;
    for (int i=0; i<num; i++) {
        MonitorOutput *monitor = _output_list[i];
        if (monitor->run()) {
            LC_LOG_WARNING("monitor:%d run error", i);
            goto Fail;
        }
        ++ using_num;
    }

    if (pthread_create(&_compute_threadid, NULL, compute_func, this) != 0) {
        LC_LOG_WARNING("create compute thread error");
        goto Fail;
    }
    _need_join = 1;
    return 0;
Fail:
    stop();
    for (int i=0; i<using_num; i++) {
        MonitorOutput *monitor = _output_list[i];
        monitor->join();
    }
    return -1;
}

int LcMonitor :: stop()
{
    int num = _output_list.size();
    for (int i=0; i<num; i++) {
        MonitorOutput *monitor = _output_list[i];
        monitor->stop();
    }
    _compute_run = 0;
    return 0;
}

int LcMonitor :: join()
{
    if (_need_join) {
		_need_join = 0;
		int num = _output_list.size();
		for (int i=0; i<num; i++) {
			MonitorOutput *monitor = _output_list[i];
			monitor->join();
		}
		pthread_join(_compute_threadid, NULL);
    }
    return 0;
}

void ms_sleep(int ms)
{
	struct timespec t;
	struct timespec s;
	t.tv_sec = ms/1000;
	t.tv_nsec = (ms%1000) * 1000000;

	nanosleep(&t, &s);
}






int LcMonitor :: register_item(const char *path, int compute_tag)
{
    if (NULL == path) {
        return -1;
    }

    char service[256];
    const char *split = strstr(path, ".");
    if (NULL == split || split == path + strlen(path)-1 || split == path) {
        LC_LOG_WARNING("register_item not support only service and not support . is last token, using register_service");
        return -1;
    }

    if (split-path >= (int)sizeof(service)) {
        LC_LOG_WARNING("service name long than %d", (int)sizeof(service));
        return -1;
    }
    memcpy(service, path, split-path);
    service[split-path] = '\0';

    int thread_num = 0;
    if (_service_thread_map.get(service, &thread_num) == bsl::HASH_NOEXIST) {
        LC_LOG_WARNING("service_name[%s] not register", service);
        return -1;
    }

    int err_code = 0;
    char last_name[256];
    bsl::var::IVar &var_ret = query_path(_monitor_info_total, path, _rp, &err_code, 1, last_name, sizeof(last_name));
    if (err_code != 0) {
        LC_LOG_WARNING("register path:%s error", path);
        return -1;
    }
    var_ret[last_name] = _rp.create<bsl::var::Int64>(0);

    bsl::var::IVar &var_ret1 = query_path(_monitor_info, path, _rp, &err_code, 1, last_name, sizeof(last_name));
    if (err_code != 0) {
        LC_LOG_WARNING("register path:%s error", path);
        return -1;
    }
    var_ret1[last_name] = _rp.create<bsl::var::Int64>(0);

    bsl::var::IVar &var_ret2 = query_path(_monitor_info_time, path, _rp, &err_code, 1, last_name, sizeof(last_name));
    if (err_code != 0) {
        LC_LOG_WARNING("register path:%s error", path);
        return -1;
    }
    if (0 == compute_tag) {
        
        var_ret2[last_name] = _rp.create<bsl::var::Int64>(-1);
    } else {
        
        var_ret2[last_name] = _rp.create<bsl::var::Int64>(0);
    }

    for (int i=0; i<thread_num; i++) {
        thread_info *info = _thread_info_list[i];
        bsl::var::IVar &var_ret2 = query_path(info->monitor_info, path, info->rp, &err_code, 1, last_name, sizeof(last_name));
        if (err_code != 0) {
            LC_LOG_WARNING("register path:%s into thread info error", path);
            return -1;
        }
        var_ret2[last_name] = info->rp.create<bsl::var::Int64>(0);

        bsl::var::IVar &var_ret3 = query_path(info->monitor_info_time, path, info->rp, &err_code, 1, last_name, sizeof(last_name));
        if (err_code != 0) {
            LC_LOG_WARNING("register path:%s into thread info error", path);
            return -1;
        }
        if (0 == compute_tag) {
            var_ret3[last_name] = info->rp.create<bsl::var::Int64>(-1);
        } else {
            var_ret3[last_name] = info->rp.create<bsl::var::Int64>(0);
        }
    }

    if (set_service_thread_key(split+1) != 0) {
        LC_LOG_WARNING("register path:%s into thread compute error", split+1);
        return -1;
    }
    return 0;
}

int LcMonitor :: add_path(const char *path, signed long long num)
{
    if (NULL == path) {
        return -1;
    }    

    thread_info *tmp_thread_info = create_thread_info();
    if (NULL == tmp_thread_info) {
        return -1;
    }

    
    int err_code = 0;
    char last_name[256];
    bsl::var::IVar &path_item = query_path(tmp_thread_info->monitor_info, path, tmp_thread_info->rp,
            &err_code, 0, last_name, sizeof(last_name));
    if (err_code != 0) {
        LC_LOG_WARNING("query path:%s error", path);
        return -1;
    }

    bsl::var::IVar &path_item1 = query_path(tmp_thread_info->monitor_info_time, path, tmp_thread_info->rp,
            &err_code, 0, last_name, sizeof(last_name));
    if (err_code != 0) {
        LC_LOG_WARNING("query path:%s error", path);
        return -1;
    }

    if (path_item[last_name].is_number()) {
        if (path_item1[last_name].is_number()) {
            path_item[last_name] = path_item[last_name].to_int64() + num;
            if (path_item1[last_name].to_int64() >= 0) {
                path_item1[last_name] = path_item1[last_name].to_int64() + 1; 
            }
        } else {
            LC_LOG_WARNING("add path:%s is not number", path);
            return -1;
        }
        return 0;
    } else {
        LC_LOG_WARNING("add path:%s is not number", path);
        return -1;
    }
    
    return 0;
}


int LcMonitor :: set_start_running_time(struct timeval &tm)
{
    _start_time = tm;
    return 0;
}

struct timeval &LcMonitor :: get_start_running_time()
{
    return _start_time;
}

int LcMonitor :: set_program_version(const char *program_version)
{
    if (NULL == program_version) {
        return -1;
    }

    _program_version = program_version;
    try {
        if (_monitor_info["__global"].is_null()) {
            _monitor_info["__global"] = _rp.create<bsl::var::Dict>();
        }
        _monitor_info["__global"]["PROGRAM_VERSION"] = _rp.create<bsl::var::String>(program_version);
    } catch (bsl::Exception &e) {
        LC_LOG_WARNING("catch bsl exception:%s", e.all());
        return -1;
    } catch (...) {
        LC_LOG_WARNING("catch unknown exception");
        return -1;
    }
    return 0;
}

int LcMonitor :: set_lib_version(const char *lib_version)
{
    if (NULL == lib_version) {
        return -1;
    } 

    _lib_version = lib_version;
    try {
        if (_monitor_info["__global"].is_null()) {
            _monitor_info["__global"] = _rp.create<bsl::var::Dict>();
        }
        _monitor_info["__global"]["LIB_VERSION"] = _rp.create<bsl::var::String>(lib_version);
    } catch (bsl::Exception &e) {
        LC_LOG_WARNING("catch bsl exception:%s", e.all());
        return -1;
    } catch (...) {
        LC_LOG_WARNING("catch unknown exception");
        return -1;
    }

    return 0;
}

int LcMonitor :: set_data_version(const char *data_version)
{
    if (NULL == data_version) {
        return -1;
    }

    _data_version = data_version;
    try {
        if (_monitor_info["__global"].is_null()) {
            _monitor_info["__global"] = _rp.create<bsl::var::Dict>();
        }
        _monitor_info["__global"]["DATA_VERSION"] = _rp.create<bsl::var::String>(data_version);
    } catch (bsl::Exception &e) {
        LC_LOG_WARNING("catch bsl exception:%s", e.all());
        return -1;
    } catch (...) {
        LC_LOG_WARNING("catch unknown exception");
        return -1;
    }

    return 0;
}

thread_info * LcMonitor :: create_thread_info()
{
    thread_info * tinfo = (thread_info *)pthread_getspecific(_thread_key);
    if (NULL == tinfo) {
        tinfo = get_thread_info();
        if (NULL == tinfo) {
            return NULL;
        }
        pthread_setspecific(_thread_key, (void *)tinfo);
    }
    return tinfo;
}

int LcMonitor :: add_request_number_success(const char *service_name)
{
    if (NULL == service_name) {
        return -1;
    }

    
    thread_info *tmp_thread_info = create_thread_info();
    if (NULL == tmp_thread_info) {
        LC_LOG_WARNING("get thread_info error, maybe service:%s thread_num be setting is small than using, forbidden", service_name);
        return -1;
    }

    if (tmp_thread_info->monitor_info[service_name].is_null()) {
        LC_LOG_WARNING("service_name:%s is not set", service_name);
        return -1;
    }
    if (tmp_thread_info->monitor_info[service_name]["REQUEST_NUMBER"]["SUCCESS"].is_null()) {
        LC_LOG_WARNING("leaf SUCCESS is NULL");
        return -1;
    }
    try {
        tmp_thread_info->monitor_info[service_name]["REQUEST_NUMBER"]["SUCCESS"] = tmp_thread_info->monitor_info[service_name]["REQUEST_NUMBER"]["SUCCESS"].to_int64() + 1;
        tmp_thread_info->monitor_info[service_name]["REQUEST_NUMBER"]["TOTAL_SUCCESS"] = tmp_thread_info->monitor_info[service_name]["REQUEST_NUMBER"]["TOTAL_SUCCESS"].to_int64() + 1;
    } catch (bsl::Exception &e) {
        LC_LOG_WARNING("catch bsl exception:%s", e.all());
        return -1;
    } catch (...) {
        LC_LOG_WARNING("catch unknown exception");
        return -1;
    }

    return 0;
}

int LcMonitor :: add_request_number_failed(const char *service_name)
{
    if (NULL == service_name) {
        return -1;
    }

    
    thread_info *tmp_thread_info = create_thread_info();
    if (NULL == tmp_thread_info) {
        LC_LOG_WARNING("get thread_info error, maybe service:%s thread_num be setting is small than using, forbidden", service_name);
        return -1;
    }

    if (tmp_thread_info->monitor_info[service_name].is_null()) {
        LC_LOG_WARNING("service_name:%s is not set", service_name);
        return -1;
    }
	if (tmp_thread_info->monitor_info[service_name]["REQUEST_NUMBER"]["FAILED"].is_null()) {
        LC_LOG_WARNING("leaf FAILED is NULL");
        return -1;
    }

    try {
        tmp_thread_info->monitor_info[service_name]["REQUEST_NUMBER"]["FAILED"] = tmp_thread_info->monitor_info[service_name]["REQUEST_NUMBER"]["FAILED"].to_int64() + 1;
        tmp_thread_info->monitor_info[service_name]["REQUEST_NUMBER"]["TOTAL_FAILED"] = tmp_thread_info->monitor_info[service_name]["REQUEST_NUMBER"]["TOTAL_FAILED"].to_int64() + 1;
    } catch (bsl::Exception &e) {
        LC_LOG_WARNING("catch bsl exception:%s", e.all());
        return -1;
    } catch (...) {
        LC_LOG_WARNING("catch unknown exception");
        return -1;
    }

    return 0;
}

int LcMonitor :: add_request_connect_number(const char *service_name)
{
    if (NULL == service_name) {
        return -1;
    }

    
    thread_info *tmp_thread_info = create_thread_info();
    if (NULL == tmp_thread_info) {
        LC_LOG_WARNING("get thread_info error, maybe service:%s thread_num be setting is small than using, forbidden", service_name);
        return -1;
    }

    if (tmp_thread_info->monitor_info[service_name].is_null()) {
        LC_LOG_WARNING("service_name:%s is not set", service_name);
        return -1;
    }
	if (tmp_thread_info->monitor_info[service_name]["REQUEST_CONNECT_NUMBER"].is_null()) {
        LC_LOG_WARNING("leaf REQUEST_CONNECT_NUMBER is NULL");
        return -1;
    }

    try {
        tmp_thread_info->monitor_info[service_name]["REQUEST_CONNECT_NUMBER"] = tmp_thread_info->monitor_info[service_name]["REQUEST_CONNECT_NUMBER"].to_int64() + 1;
        tmp_thread_info->monitor_info[service_name]["REQUEST_CONNECT_TOTAL_NUMBER"] = tmp_thread_info->monitor_info[service_name]["REQUEST_CONNECT_TOTAL_NUMBER"].to_int64() + 1;
    } catch (bsl::Exception &e) {
        LC_LOG_WARNING("catch bsl exception:%s", e.all());
        return -1;
    } catch (...) {
        LC_LOG_WARNING("catch unknown exception");
        return -1;
    }

    return 0;
}

int LcMonitor :: add_compute_time(const char *service_name, int compute_time)
{
    if (NULL == service_name) {
        return -1;
    }

    
    thread_info *tmp_thread_info = create_thread_info();
    if (NULL == tmp_thread_info) {
        LC_LOG_WARNING("get thread_info error, maybe service:%s thread_num be setting is small than using, forbidden", service_name);
        return -1;
    }

    if (tmp_thread_info->monitor_info[service_name].is_null()) {
        LC_LOG_WARNING("service_name:%s is not set", service_name);
        return -1;
    }
	if (tmp_thread_info->monitor_info[service_name]["COMPUTE_TIME"].is_null()) {
        LC_LOG_WARNING("leaf COMPUTE_TIME is NULL");
        return -1;
    }

    if (tmp_thread_info->monitor_info_time[service_name].is_null()) {
        LC_LOG_WARNING("service_name:%s is not set", service_name);
        return -1;
    }
	if (tmp_thread_info->monitor_info_time[service_name]["COMPUTE_TIME"].is_null()) {
        LC_LOG_WARNING("leaf COMPUTE_TIME is NULL");
        return -1;
    }

    try {
        tmp_thread_info->monitor_info[service_name]["COMPUTE_TIME"] = tmp_thread_info->monitor_info[service_name]["COMPUTE_TIME"].to_int64() + compute_time;
        tmp_thread_info->monitor_info_time[service_name]["COMPUTE_TIME"] = tmp_thread_info->monitor_info_time[service_name]["COMPUTE_TIME"].to_int64() + 1;
    } catch (bsl::Exception &e) {
        LC_LOG_WARNING("catch bsl exception:%s", e.all());
        return -1;
    } catch (...) {
        LC_LOG_WARNING("catch unknown exception");
        return -1;
    }
    
    return 0;
}

int LcMonitor :: add_self_process_time(const char *service_name, int self_process_time)
{
    if (NULL == service_name) {
        return -1;
    }

    
    thread_info *tmp_thread_info = create_thread_info();
    if (NULL == tmp_thread_info) {
        LC_LOG_WARNING("get thread_info error, maybe service:%s thread_num be setting is small than using, forbidden", service_name);
        return -1;
    }

    if (tmp_thread_info->monitor_info[service_name].is_null()) {
        LC_LOG_WARNING("service_name:%s is not set", service_name);
        return -1;
    }
	if (tmp_thread_info->monitor_info[service_name]["SELF_PROCESS_TIME"].is_null()) {
        LC_LOG_WARNING("leaf SELF_PROCESS_TIME is NULL");
        return -1;
    }

    if (tmp_thread_info->monitor_info_time[service_name].is_null()) {
        LC_LOG_WARNING("service_name:%s is not set", service_name);
        return -1;
    }
	if (tmp_thread_info->monitor_info_time[service_name]["SELF_PROCESS_TIME"].is_null()) {
        LC_LOG_WARNING("leaf SELF_PROCESS_TIME is NULL");
        return -1;
    }

    try {
        tmp_thread_info->monitor_info[service_name]["SELF_PROCESS_TIME"] = tmp_thread_info->monitor_info[service_name]["SELF_PROCESS_TIME"].to_int64() + self_process_time;
        tmp_thread_info->monitor_info_time[service_name]["SELF_PROCESS_TIME"] = tmp_thread_info->monitor_info_time[service_name]["SELF_PROCESS_TIME"].to_int64() + 1;
    } catch (bsl::Exception &e) {
        LC_LOG_WARNING("catch bsl exception:%s", e.all());
        return -1;
    } catch (...) {
        LC_LOG_WARNING("catch unknown exception");
        return -1;
    }
   
    return 0;
}

int LcMonitor :: add_self_action_time(const char *service_name, int self_action_time)
{
    if (NULL == service_name) {
        return -1;
    }

    
    thread_info *tmp_thread_info = create_thread_info();
    if (NULL == tmp_thread_info) {
        LC_LOG_WARNING("get thread_info error, maybe service:%s thread_num be setting is small than using, forbidden", service_name);
        return -1;
    }

    if (tmp_thread_info->monitor_info[service_name].is_null()) {
        LC_LOG_WARNING("service_name:%s is not set", service_name);
        return -1;
    }
	if (tmp_thread_info->monitor_info[service_name]["SELF_ACTION_TIME"].is_null()) {
        LC_LOG_WARNING("leaf SELF_ACTION_TIME is NULL");
        return -1;
    }

    if (tmp_thread_info->monitor_info_time[service_name].is_null()) {
        LC_LOG_WARNING("service_name:%s is not set", service_name);
        return -1;
    }
	if (tmp_thread_info->monitor_info_time[service_name]["SELF_ACTION_TIME"].is_null()) {
        LC_LOG_WARNING("leaf SELF_ACTION_TIME is NULL");
        return -1;
    }

    try {
        tmp_thread_info->monitor_info[service_name]["SELF_ACTION_TIME"] = tmp_thread_info->monitor_info[service_name]["SELF_ACTION_TIME"].to_int64() + self_action_time;
        tmp_thread_info->monitor_info_time[service_name]["SELF_ACTION_TIME"] = tmp_thread_info->monitor_info_time[service_name]["SELF_ACTION_TIME"].to_int64() + 1;
    } catch (bsl::Exception &e) {
        LC_LOG_WARNING("catch bsl exception:%s", e.all());
        return -1;
    } catch (...) {
        LC_LOG_WARNING("catch unknown exception");
        return -1;
    } 
    
    return 0;
}

int LcMonitor :: add_io_read_time(const char * service_name, int io_read_time)
{
    if (NULL == service_name) {
        return -1;
    }

    
    thread_info *tmp_thread_info = create_thread_info();
    if (NULL == tmp_thread_info) {
        LC_LOG_WARNING("get thread_info error, maybe service:%s thread_num be setting is small than using, forbidden", service_name);
        return -1;
    }

    if (tmp_thread_info->monitor_info[service_name].is_null()) {
        LC_LOG_WARNING("service_name:%s is not set", service_name);
        return -1;
    }
    if (tmp_thread_info->monitor_info[service_name]["IO_READ_TIME"].is_null()) {
        LC_LOG_WARNING("leaf IO_READ_TIEM is NULL");
        return -1;
    }

    if (tmp_thread_info->monitor_info_time[service_name].is_null()) {
        LC_LOG_WARNING("service_name:%s is not set", service_name);
        return -1;
    }
    if (tmp_thread_info->monitor_info_time[service_name]["IO_READ_TIME"].is_null()) {
        LC_LOG_WARNING("leaf IO_READ_TIEM is NULL");
        return -1;
    }

    try {
        tmp_thread_info->monitor_info[service_name]["IO_READ_TIME"] = tmp_thread_info->monitor_info[service_name]["IO_READ_TIME"].to_int64() + io_read_time;
        tmp_thread_info->monitor_info_time[service_name]["IO_READ_TIME"] = tmp_thread_info->monitor_info_time[service_name]["IO_READ_TIME"].to_int64() + 1;
    } catch (bsl::Exception &e) {
        LC_LOG_WARNING("catch bsl exception:%s", e.all());
        return -1;
    } catch (...) {
        LC_LOG_WARNING("catch unknown exception");
        return -1;
    }

    return 0;
}

int LcMonitor :: add_io_write_time(const char * service_name, int io_write_time)
{
    if (NULL == service_name) {
        return -1;
    }

    
    thread_info *tmp_thread_info = create_thread_info();
    if (NULL == tmp_thread_info) {
        LC_LOG_WARNING("get thread_info error, maybe service:%s thread_num be setting is small than using, forbidden", service_name);
        return -1;
    }

    if (tmp_thread_info->monitor_info[service_name].is_null()) {
        LC_LOG_WARNING("service_name:%s is not set", service_name);
        return -1;
    }
    if (tmp_thread_info->monitor_info[service_name]["IO_WRITE_TIME"].is_null()) {
        LC_LOG_WARNING("leaf IO_READ_TIEM is NULL");
        return -1;
    }

    if (tmp_thread_info->monitor_info_time[service_name].is_null()) {
        LC_LOG_WARNING("service_name:%s is not set", service_name);
        return -1;
    }
    if (tmp_thread_info->monitor_info_time[service_name]["IO_WRITE_TIME"].is_null()) {
        LC_LOG_WARNING("leaf IO_READ_TIEM is NULL");
        return -1;
    }

    try {
        tmp_thread_info->monitor_info[service_name]["IO_WRITE_TIME"] = tmp_thread_info->monitor_info[service_name]["IO_WRITE_TIME"].to_int64() + io_write_time;
        tmp_thread_info->monitor_info_time[service_name]["IO_WRITE_TIME"] = tmp_thread_info->monitor_info_time[service_name]["IO_WRITE_TIME"].to_int64() + 1;
    } catch (bsl::Exception &e) {
        LC_LOG_WARNING("catch bsl exception:%s", e.all());
        return -1;
    } catch (...) {
        LC_LOG_WARNING("catch unknown exception");
        return -1;
    }

    return 0;
}

int LcMonitor :: add_fatal_number(const char *service_name)
{
    if (NULL == service_name) {
        return -1;
    }

    
    thread_info *tmp_thread_info = create_thread_info();
    if (NULL == tmp_thread_info) {
        LC_LOG_WARNING("get thread_info error, maybe service:%s thread_num be setting is small than using, forbidden", service_name);
        return -1;
    }

    if (tmp_thread_info->monitor_info[service_name].is_null()) {
        LC_LOG_WARNING("service_name:%s is not set", service_name);
        return -1;
    }
	if (tmp_thread_info->monitor_info[service_name]["FATAL_NUMBER"].is_null()) {
        LC_LOG_WARNING("leaf FATAL_NUMBER is NULL");
        return -1;
    }

    try {
        tmp_thread_info->monitor_info[service_name]["FATAL_NUMBER"] = tmp_thread_info->monitor_info[service_name]["FATAL_NUMBER"].to_int64() + 1;
    } catch (bsl::Exception &e) {
        LC_LOG_WARNING("catch bsl exception:%s", e.all());
        return -1;
    } catch (...) {
        LC_LOG_WARNING("catch unknown exception");
        return -1;
    }
   
    return 0;
}

int LcMonitor :: add_read_error_number(const char *service_name)
{
    if (NULL == service_name) {
        return -1;
    }

    
    thread_info *tmp_thread_info = create_thread_info();
    if (NULL == tmp_thread_info) {
        LC_LOG_WARNING("get thread_info error, maybe service:%s thread_num be setting is small than using, forbidden", service_name);
        return -1;
    }

    if (tmp_thread_info->monitor_info[service_name].is_null()) {
        LC_LOG_WARNING("service_name:%s is not set", service_name);
        return -1;
    }
	if (tmp_thread_info->monitor_info[service_name]["READ_ERROR_NUMBER"].is_null()) {
        LC_LOG_WARNING("leaf READ_ERROR_NUMBER is NULL");
        return -1;
    }

    try {
        tmp_thread_info->monitor_info[service_name]["READ_ERROR_NUMBER"] = tmp_thread_info->monitor_info[service_name]["READ_ERROR_NUMBER"].to_int64() + 1;
    } catch (bsl::Exception &e) {
        LC_LOG_WARNING("catch bsl exception:%s", e.all());
        return -1;
    } catch (...) {
        LC_LOG_WARNING("catch unknown exception");
        return -1;
    }
   
    return 0;

}

int LcMonitor :: add_write_error_number(const char *service_name)
{
    if (NULL == service_name) {
        return -1;
    }

    
    thread_info *tmp_thread_info = create_thread_info();
    if (NULL == tmp_thread_info) {
        LC_LOG_WARNING("get thread_info error, maybe service:%s thread_num be setting is small than using, forbidden", service_name);
        return -1;
    }

    if (tmp_thread_info->monitor_info[service_name].is_null()) {
        LC_LOG_WARNING("service_name:%s is not set", service_name);
        return -1;
    }
	if (tmp_thread_info->monitor_info[service_name]["WRITE_ERROR_NUMBER"].is_null()) {
        LC_LOG_WARNING("leaf WRITE_ERROR_NUMBER is NULL");
        return -1;
    }

    try {
        tmp_thread_info->monitor_info[service_name]["WRITE_ERROR_NUMBER"] = tmp_thread_info->monitor_info[service_name]["WRITE_ERROR_NUMBER"].to_int64() + 1;
    } catch (bsl::Exception &e) {
        LC_LOG_WARNING("catch bsl exception:%s", e.all());
        return -1;
    } catch (...) {
        LC_LOG_WARNING("catch unknown exception");
        return -1;
    }
   
    return 0;

}


bsl::ResourcePool *LcMonitor :: get_set_monitor_info_rp()
{
    return &_rp;
}

int LcMonitor :: set_monitor_info(const char *path, bsl::var::IVar &info)
{
    
    int err_code = 0;
    char last_name[256];
    bsl::var::IVar &path_item = query_path(_monitor_info, path, _rp, &err_code, 1, last_name, sizeof(last_name));
    if (err_code != 0) {
        LC_LOG_WARNING("query path:%s error", path);
        return -1;
    }

    try {
        path_item[last_name] = info;
    } catch (bsl::Exception &e) {
        LC_LOG_WARNING("catch bsl exception:%s", e.all());
        return -1;
    } catch (...) {
        LC_LOG_WARNING("catch unknown exception");
        return -1;
    }

    return 0;
}


int LcMonitor::register_static_item(const char *path, signed long long init_value)
{
    int err_code = 0;
	char last_name[256];
    bsl::var::IVar &path_item = query_path(_monitor_info, path, _rp, &err_code, 1, last_name, sizeof(last_name)); 
    if (err_code != 0) {
        LC_LOG_WARNING("query path:%s error", path);
        return -1;
    }
    try {
        path_item[last_name] = _rp.create<bsl::var::Number<signed long long> >(init_value);
    } catch (bsl::Exception &e) {
        LC_LOG_WARNING("catch bsl exception:%s", e.all());
        return -1;
    } catch (...) {
        LC_LOG_WARNING("catch unknown exception");
        return -1;
    }	
    return 0;
}


int LcMonitor::set_static_item(const char *path, signed long long value)
{
    int err_code = 0;
    char last_name[256];
    bsl::var::IVar &path_item = query_path(_monitor_info, path, _rp, &err_code, 0, last_name, sizeof(last_name));
    if (err_code != 0) {
        LC_LOG_WARNING("query path:%s error", path);
        return -1;
    }

    try {
        path_item[last_name] = _rp.create<bsl::var::Number<signed long long> >(value);
    } catch (bsl::Exception &e) {
        LC_LOG_WARNING("catch bsl exception:%s", e.all());
        return -1;
    } catch (...) {
        LC_LOG_WARNING("catch unknown exception");
        return -1;
    }

    return 0;
}

}




