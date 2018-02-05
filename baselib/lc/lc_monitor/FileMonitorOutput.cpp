
#include "FileMonitorOutput.h"
#include "LcMonitor.h"
#include "lc_log.h"

#include "util.hpp"


#include <bsl/var/String.h>
#include <bsl/var/JsonSerializer.h>
namespace lc
{
int FileMonitorOutput :: init(ul_confdata_t * conf)
{
    
    char c_conf[1024];

    if (ul_getconfnstr(conf, "lcmonitor_file_path", c_conf, sizeof(c_conf)) != 1) {
        LC_LOG_NOTICE("using default lcmonitor_file_path : %s, need to create yourself", "./info");
        snprintf(c_conf, sizeof(c_conf), "%s", "./info");
    }
    _path = c_conf;

    if (ul_getconfnstr(conf, "lcmonitor_file_name", c_conf, sizeof(c_conf)) != 1) {
        LC_LOG_NOTICE("using default lcmonitor_file_name : %s", "file.out");
        snprintf(c_conf, sizeof(c_conf), "%s", "file.out");
    }
    _name = c_conf;

    int i_conf = 1024000;
    if (ul_getconfint(conf, "lcmonitor_single_monitor_size", &i_conf) != 1) {
        LC_LOG_NOTICE("using default lcmonitor_single_monitor_size : %d", 1024000);
        i_conf = 1024000;
    }
    _single_monitor_size = i_conf;

    _tmp_buf = (char *)_rp.create_raw(_single_monitor_size);
    if (NULL == _tmp_buf) {
        LC_LOG_WARNING("create tmp buffer error %m");
        return -1;
    }
    memset(_tmp_buf, ' ', _single_monitor_size);

    return 0;
}


int FileMonitorOutput :: run()
{
    
    int ret = self_openfile();
    if (ret != 0) {
        return -1;
    }    

    return pthread_create(&_pid, NULL, work_thread, this);
}

int FileMonitorOutput :: stop()
{
    _run = 0;
    return 0;
}

int FileMonitorOutput :: join()
{
    pthread_join(_pid, NULL);
    if (_filefd >= 0) {
        close(_filefd);
        _filefd = -1; 
    }
    return 0;
}

void * FileMonitorOutput :: work_thread(void *arg)
{
    FileMonitorOutput *output = (FileMonitorOutput*)arg;
    output->do_work();
    return NULL;
}

void FileMonitorOutput :: do_work()
{
    lc_log_initthread("FileMonitorOutput thread");
    while (_run) {
        ms_sleep(_monitor->get_monitor_time());
        bsl::ResourcePool rp;
        int err_code = 0;
        bsl::var::IVar &monitor_info = _monitor->query_path_outer("ALL", rp, &err_code);
        if (err_code != 0) {
            LC_LOG_WARNING("query_path : ALL error");
            continue;
        }

        bsl::AutoBuffer buffer;
        
        convert_var2json(monitor_info, buffer);
        
        
        if (_filefd < 0) {
            if (self_openfile()) {
                continue;
            }
        }

		lseek(_filefd, 0L, SEEK_SET);
        if (_last_size > 0) {
            if (_last_size <= _single_monitor_size) {
                if (self_write(_tmp_buf, _last_size) ) {
                    continue;
                }
            } else {
                char *tmp_buf = new(std::nothrow) char[_last_size];
                if (NULL == tmp_buf) {
                    LC_LOG_WARNING("new buf len:%d error %m", _last_size);
                    continue;
                }
                memset(tmp_buf, ' ', _last_size);
                if (self_write(tmp_buf, _last_size)) {
                    delete []tmp_buf;
                    continue;
                }
                delete []tmp_buf;
            }
		    lseek(_filefd, 0L, SEEK_SET);
        }
		if (self_write(buffer.c_str(), buffer.size())) {
			continue;
        }
		if (self_write("\n", 1)) {
            continue;
        }
        _last_size = buffer.size() + 1;
    }
    lc_log_closethread();
}

char *FileMonitorOutput :: self_fprintf(char *buffer, size_t &buf_size, const char *input)
{
    int ret = snprintf(buffer, buf_size, "%s", input);
    if (ret <= (int)buf_size) {
        buf_size -= ret;
	    buffer += ret;
	    return buffer;
    } else {
        buf_size = 0;
	    return NULL;
    }

    return NULL;
}

int FileMonitorOutput :: convert_var2json(bsl::var::IVar &info, bsl::AutoBuffer &buffer)
{
    
    
    bsl::var::Dict::dict_iterator iter = info.dict_begin();
    bsl::var::Dict::dict_iterator end = info.dict_end();
    
    bsl::ResourcePool rp;
    bsl::var::Ref output = rp.create<bsl::var::Dict>();
    for( ; iter != end; ++iter) {
        if (iter->value().is_null()) {
            continue;
        }

        bsl::var::IVar &node = iter->value();
		
		if (node.is_array()) {
			try {
				if(node[1].to_int32() == 0) {
					output[iter->key()] = call_function_object(rp, node, iter->key());
				}
			} catch (bsl::Exception &e) {
				LC_LOG_WARNING("catch bsl exception:%s", e.all());
				continue;
			} catch (std::exception &e) {
				LC_LOG_WARNING("catch stl exception:%s", e.what());
				continue;
			} catch (...) {
				continue;
			}
        } else if (node.is_dict()){
            output[iter->key()] = rp.create<bsl::var::Dict>();
            merge_var(output[iter->key()], node, rp);
        } else {
            output[iter->key()] = iter->value();
        }
    }

    
    struct timeval &tm = _monitor->get_start_running_time();
    struct timeval currtm;
    gettimeofday(&currtm, NULL);
    long using_time = (currtm.tv_sec-tm.tv_sec)*1000 + (currtm.tv_usec-tm.tv_usec)/1000;
    try {
        if (output["__global"].is_null()) {
            output["__global"] = rp.create<bsl::var::Dict>();
        }
        output["__global"]["RUNNING_TIME"] = rp.create<bsl::var::Int64>(using_time);
    } catch (bsl::Exception &e) {
        LC_LOG_WARNING("catch bsl exception:%s", e.all());
        return -1;
    } catch (...) {
        LC_LOG_WARNING("catch unknown exception");
        return -1;
    }

    bsl::var::IVar &output1 = rp.create<bsl::var::Dict>();
    output1[get_proc_name()] = output;

    bsl::var::JsonSerializer json_ses;
    json_ses.serialize(output1, buffer);

    return 0;
}

int FileMonitorOutput :: merge_var(bsl::var::IVar &output, bsl::var::IVar &input, bsl::ResourcePool &rp)
{
    
    bsl::var::Dict::dict_iterator iter = input.dict_begin();
    bsl::var::Dict::dict_iterator end = input.dict_end();

    for( ; iter != end; ++iter) {
        if (iter->value().is_null()) {
            continue;
        }
		bsl::var::IVar &node = iter->value();
		
			
		if (node.is_array()) {
		
			try {
				if(node[1].to_int32() == 0) {
					output[iter->key()] = call_function_object(rp, node, iter->key());
				}
			} catch (bsl::Exception &e) {
				LC_LOG_WARNING("catch bsl exception:%s", e.all());
				continue;
			} catch (std::exception &e) {
				LC_LOG_WARNING("catch stl exception:%s", e.what());
				continue;
			} catch (...) {
				continue;
			}
		} else if (node.is_dict()) {
			output[iter->key()] = rp.create<bsl::var::Dict>();
			merge_var(output[iter->key()], iter->value(), rp);
		} else {
			output[iter->key()] = iter->value();
		}
    }
    return 0;
}
}





















