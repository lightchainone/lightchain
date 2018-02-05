
#include "SocketMonitorOutput.h"
#include "LcMonitor.h"

#include "util.hpp"

#include <lc_server.h>
#include <bsl/var/String.h>
#include <nshead.h>
#include <compack/compack.h>
#include <bsl/var/McPackSerializer.h>
namespace lc
{
int SocketMonitorOutput :: init_conf(ul_confdata_t *conf)
{
    int i_conf;
    int i_conf1;
    int i_conf2;
    char c_conf[1024];
    
    if (ul_getconfint(conf, "lcmonitor_socket_port", &i_conf) != 1) {
        LC_LOG_WARNING("lcmonitor_socket_port not exist in conf, forbidden");
        goto Fail;
    }
    lc_server_set_listen_port(_server, i_conf);

    if (ul_getconfnstr(conf, "lcmonitor_socket_name", c_conf, sizeof(c_conf)) != 1) {
        snprintf(c_conf, sizeof(c_conf), "%s", "NONE");
    }
    lc_server_set_server_name(_server, c_conf);

    if (ul_getconfint(conf, "lcmonitor_socket_servertype", &i_conf) != 1) {
        i_conf = 2;
        LC_LOG_NOTICE("using default lcmonitor_socket_servertype : %d", i_conf);
    }
    lc_server_set_server_type(_server, i_conf);

    if (ul_getconfint(conf, "lcmonitor_socket_threadnum", &i_conf2) != 1) {
        i_conf2 = 5;
        LC_LOG_NOTICE("using default lcmonitor_socket_threadnum : %d", i_conf2);
    }
    if (ul_getconfint(conf, "lcmonitor_socket_read_bufsize", &i_conf) != 1) {
        i_conf = 1048576;
        LC_LOG_NOTICE("using default lcmonitor_socket_threadnum : %d", i_conf);
    }
    if (ul_getconfint(conf, "lcmonitor_socket_write_bufsize", &i_conf1) != 1) {
        i_conf1 = 1048576;
        LC_LOG_NOTICE("using default lcmonitor_socket_write_bufsize : %d", i_conf1);
    }
    lc_server_set_buffer(_server, i_conf2, NULL, i_conf, NULL, i_conf1);

    if (ul_getconfint(conf, "lcmonitor_socket_userbufsize", &i_conf) != 1) {
        i_conf = 0;
        LC_LOG_NOTICE("using default lcmonitor_socket_userbufsize : %d", i_conf);
    }
    lc_server_set_user_data(_server, NULL, i_conf);

    if (ul_getconfint(conf, "lcmonitor_socket_connecttimeout", &i_conf) != 1) {
        i_conf = 500;
        LC_LOG_NOTICE("using default lcmonitor_socket_connecttimeout : %d", i_conf);
    }
    if (ul_getconfint(conf, "lcmonitor_socket_readtimeout", &i_conf1) != 1) {
        i_conf1 = 500;
        LC_LOG_NOTICE("using default lcmonitor_socket_readtimeout : %d", i_conf1);
    } 
    if (ul_getconfint(conf, "lcmonitor_socket_writetimeout", &i_conf2) != 1) {
        i_conf2 = 500;
        LC_LOG_NOTICE("using default lcmonitor_socket_writetimeout : %d", i_conf2);
    } 
    lc_server_set_timeout(_server, i_conf, i_conf1, i_conf2);

    if (ul_getconfint(conf, "lcmonitor_socket_connecttype", &i_conf) != 1) {
        i_conf = 0;
        LC_LOG_NOTICE("using default lcmonitor_socket_connecttype : %d", i_conf);
    }
    lc_server_set_connect_type(_server, i_conf);

    if (ul_getconfint(conf, "lcmonitor_socket_quenesize", &i_conf) != 1) {
        i_conf = 100;
        LC_LOG_NOTICE("using default lcmonitor_socket_quenesize : %d", i_conf);
    }
    lc_server_set_cpool_queue_size(_server, i_conf);

    if (ul_getconfint(conf, "lcmonitor_socket_socksize", &i_conf) != 1) {
        i_conf = 500;
        LC_LOG_NOTICE("using default lcmonitor_socket_socksize : %d", i_conf);
    }
    lc_server_set_cpool_socket_size(_server, i_conf);

    if (ul_getconfnstr(conf, "lcmonitor_socket_auth", c_conf, sizeof(c_conf)) == 1) {
        _auth = lc_auth_create();
        if (NULL == _auth) {
            LC_LOG_WARNING("creat ip_auth error[%m]");
        } else {
            if (lc_auth_load_ip(_auth, c_conf) != 0) {
                LC_LOG_WARNING("load ip_auth file error");
            } else {
                if (lc_auth_compile(_auth) != 0) {
                    LC_LOG_WARNING("compile ip_auth error");
                } else {
                    lc_server_set_ip_auth(_server, _auth);
                }
            }
        }
    }

    return 0;
Fail:
    return -1;
}

int SocketMonitorOutput :: init(ul_confdata_t *conf)
{
    if (_server != NULL) {
        return -1;
    }
    _server = lc_server_create();
    if (NULL == _server) {
        LC_LOG_WARNING("lc_server create error[%m]");
        return -1;
    }

    if (init_conf(conf) != 0) {
        lc_server_destroy(_server);
        _server = NULL;
        return -1;
    }

    int thread_num = lc_server_get_threadnum(_server);
    void *tmp_vec[thread_num];
    for (int i=0; i<thread_num; i++) {
        tmp_vec[i] = (void *)this;
    }
    lc_server_set_user_data(_server, tmp_vec, thread_num);
   
    return 0;
}

int SocketMonitorOutput :: run()
{
    lc_server_set_callback(_server, query_server_callback);
    return lc_server_run(_server);
}

int SocketMonitorOutput :: stop()
{
    return lc_server_stop(_server);
}

int SocketMonitorOutput :: join()
{
    return lc_server_join(_server);
}

int SocketMonitorOutput :: query_server_callback()
{
    
    SocketMonitorOutput *monitor = (SocketMonitorOutput *)lc_server_get_user_data();
    if (NULL == monitor) {
        return -1;
    }

    int ret = monitor->parse_query();
    if (0 != ret) {
        
        return -1;
    }

    return 0;
}

int SocketMonitorOutput :: parse_query()
{
    
    nshead_t *req_head = (nshead_t*)lc_server_get_read_buf();
    char *req_buf = (char *)(req_head+1);
    int req_buf_size = req_head->body_len;
    
    

    compack::buffer::buffer bufwrap(req_buf, req_buf_size);
    try {
        compack::buffer::Reader reader(bufwrap);
        compack::ObjectIterator tmp_it1;
        reader.find("query", tmp_it1);
        compack::buffer::Reader reader0(reader, tmp_it1);
        compack::Iterator it;
        if (reader0.next(it)) {
            compack::buffer::Reader reader1(reader0, it);
            compack::ObjectIterator objit;
            if (reader1.find("cmd", objit)) {
                return parse_manage_cmd();
            } else {
                return parse_query_cmd();
            }
            return 0;
        }
        return -1;

    } catch(bsl::Exception &e) {
        LC_LOG_WARNING("catch bsl exception:%s", e.all());
        return -1;
    } catch(std::exception &e) {
        LC_LOG_WARNING("catch stl exception:%s", e.what());
        return -1;
    } catch (...) {
        LC_LOG_WARNING("catch unknown exception");
        return -1;
    }
    return 0;
}

int SocketMonitorOutput :: parse_manage_cmd()
{
    
    nshead_t *req_head = (nshead_t*)lc_server_get_read_buf();
    char *req_buf = (char *)(req_head+1);
    int req_buf_size = req_head->body_len;
  
    compack::buffer::buffer bufwrap(req_buf, req_buf_size);
    try {
        compack::buffer::Reader reader0(bufwrap);
        compack::ObjectIterator tmp_it1;
        reader0.find("query", tmp_it1);
        compack::buffer::Reader reader(reader0, tmp_it1);
        compack::Iterator it;
        if (reader.next(it)) {
            compack::buffer::Reader reader1(reader, it);
            compack::ObjectIterator objit;
            if (reader1.find("cmd", objit)) {
                const char *cmd = objit.getAsString();
                if (strcmp(cmd, "adjust_time") == 0) {
                    compack::ObjectIterator objit1;
                    if (reader1.find("time", objit1)) {
                        _monitor->set_compute_time(objit1.getAsNum<int>());
		                nshead_t *res_head = (nshead_t*)lc_server_get_write_buf();
                        char *res_buf = (char *)(res_head+1);
                        int res_buf_size = lc_server_get_write_size() - sizeof(nshead_t);
                        compack::buffer::buffer bufwrap(res_buf, res_buf_size);
                        compack::buffer::Object obj(&bufwrap);
                        obj.putString("adjust_time", "success");
                        obj.close();
                        res_head->body_len = obj.size();
                        return 0;
                    }

                    return -1;
                } else if (strcmp(cmd, "query_item") == 0){
		            bsl::ResourcePool rp;
                    bsl::var::IVar &res = _monitor->query_item(rp);
                    if (res.is_null()) {
                        return -1;
                    }
		            nshead_t *res_head = (nshead_t*)lc_server_get_write_buf();
                    char *res_buf = (char *)(res_head+1);
                    int res_buf_size = lc_server_get_write_size() - sizeof(nshead_t);
		            
		            int len = convert_var2mcpack(res, res_buf, res_buf_size);
                    if (len < 0) {
                        return -1;
                    }
		            res_head->body_len = len;
                    return 0;
                } else {
                    LC_LOG_WARNING("not support cmd:%s", cmd);
                    return -1;
                }
            }
        }
    } catch(bsl::Exception &e) {
        LC_LOG_WARNING("catch bsl exception:%s", e.all());
        return -1;
    } catch(std::exception &e) {
        LC_LOG_WARNING("catch stl exception:%s", e.what());
        return -1;
    }
    return -1;
}

int SocketMonitorOutput :: parse_query_cmd()
{
    nshead_t *req_head = (nshead_t*)lc_server_get_read_buf();
    char *req_buf = (char *)(req_head+1);
    int req_buf_size = lc_server_get_read_size();
    
    nshead_t *res_head = (nshead_t*)lc_server_get_write_buf();
    char *res_buf = (char *)(res_head+1);
    int res_buf_size = lc_server_get_write_size()- sizeof(nshead_t);
    int res_len = 0;

    compack::buffer::buffer bufwrap(req_buf, req_buf_size);
    try {
        compack::buffer::Reader reader0(bufwrap);
        compack::ObjectIterator tmp_it1;
        reader0.find("query", tmp_it1);
        compack::buffer::Reader reader(reader0, tmp_it1);
        compack::Iterator it;
        int all_tag = 0;
        while (reader.next(it)) {
            compack::buffer::Reader reader1(reader, it);
            compack::ObjectIterator objit;
            if (reader1.find("query_path", objit)) {
                const char *cmd = objit.getAsString();
                
                bsl::ResourcePool rp;
                int err_code = 0;
                bsl::var::Ref path_value;

				
                const char *split = strstr(cmd, ".");
				char lastname[256];
				const char *p_lastname = lastname;
                if (NULL == split) {
					p_lastname = get_proc_name();
                    path_value = _monitor->query_path_outer("ALL", rp, &err_code);
                    all_tag = 1;
                } else {
                    path_value = _monitor->query_path_outer(split+1, rp, &err_code, lastname, sizeof(lastname));
                }
                if (err_code != 0 || path_value.is_null()) {
                    LC_LOG_WARNING("socket query_path:%s error", cmd);
                    break;
                }
                
                int len = convert_var2mcpack_ex(p_lastname, path_value, res_buf, res_buf_size, all_tag);
                if (len > 0) {
                    
                    
                    res_len += len;
                    break;
                }
            }
        }
    } catch(bsl::Exception &e) {
        LC_LOG_WARNING("catch bsl exception:%s", e.all());
        return -1;
    } catch(std::exception &e) {
        LC_LOG_WARNING("catch stl exception:%s", e.what());
        return -1;
    }
    
    
    res_head->body_len = res_len;
    res_head->magic_num = req_head->magic_num;
    res_head->log_id = req_head->log_id;
    return 0;
}





int SocketMonitorOutput::convert_var2mcpack_ex(
	const char *key, bsl::var::IVar &node, char *buffer, size_t buf_size, int all)
{
    bsl::ResourcePool rp;
    bsl::var::Ref output = rp.create<bsl::var::Dict>();

    if (node.is_null() || buf_size <= 0) {
        return -1;
    }

	if(node.is_number()) {
		output[key] = node;
	} else if(node.is_array()) {
		try {
			output[key] = call_function_object(rp, node, key);
		} catch (bsl::Exception &e) {
			LC_LOG_WARNING("catch bsl exception:%s", e.all());
			return -1;
		} catch (std::exception &e) {
			LC_LOG_WARNING("catch stl exception:%s", e.what());
			return -1;
		} catch (...) {
			return -1;
		}	
	} else if(node.is_dict()) {
		output[key] = rp.create<bsl::var::Dict>();
		
	    
	    bsl::var::Dict::dict_iterator iter = node.dict_begin();
	    bsl::var::Dict::dict_iterator end = node.dict_end();

	    for( ; iter != end; ++iter) {
	        if (iter->value().is_null()) {
	            continue;
		    }

			if (iter->value().is_array()) {
				try {
					output[key][iter->key()] = call_function_object(rp, iter->value(), iter->key());
				} catch (bsl::Exception &e) {
					LC_LOG_WARNING("catch bsl exception:%s", e.all());
					continue;
				} catch (std::exception &e) {
					LC_LOG_WARNING("catch stl exception:%s", e.what());
					continue;
				} catch (...) {
					continue;
				}

			} else if (iter->value().is_dict()){
				output[key][iter->key()] = rp.create<bsl::var::Dict>();
				merge_var(output[key][iter->key()], iter->value(), rp);
			} else {
				output[key][iter->key()] = iter->value();
			}
	    }
	}

    if (all == 1) {
		
		struct timeval &tm = _monitor->get_start_running_time();
		struct timeval currtm;
		gettimeofday(&currtm, NULL);
		long using_time = (currtm.tv_sec-tm.tv_sec)*1000 + (currtm.tv_usec-tm.tv_usec)/1000;
		try {
			if (output[key]["__global"].is_null()) {
				output[key]["__global"] = rp.create<bsl::var::Dict>();
			}
			output[key]["__global"]["RUNNING_TIME"] = rp.create<bsl::var::Int64>(using_time); 
			
			
			

			
			
			bsl::var::McPackSerializer mcpack_ses;
			int ret = mcpack_ses.serialize(output, buffer, buf_size);
			return ret;

		} catch (bsl::Exception &e) {
			LC_LOG_WARNING("catch bsl exception:%s", e.all());
			return -1;
		} catch (...) {
			LC_LOG_WARNING("catch unknown exception");
			return -1;
		}
    } else {
		bsl::var::McPackSerializer mcpack_ses;
		int ret = mcpack_ses.serialize(output, buffer, buf_size);
		return ret;
    }
    
    return -1;
}





int SocketMonitorOutput :: convert_var2mcpack(bsl::var::IVar &info, char *buffer, size_t buf_size, int all)
{
    bsl::ResourcePool rp;
    bsl::var::Ref output = rp.create<bsl::var::Dict>();

    if (info.is_null() || buf_size <= 0) {
        return -1;
    }
    
    bsl::var::Dict::dict_iterator iter = info.dict_begin();
    bsl::var::Dict::dict_iterator end = info.dict_end();

    for( ; iter != end; ++iter) {
        if (iter->value().is_null()) {
            continue;
	    }

		
		if (iter->value().is_array()) {
			try {
				output[iter->key()] = call_function_object(rp, iter->value(), iter->key());
			} catch (bsl::Exception &e) {
				LC_LOG_WARNING("catch bsl exception:%s", e.all());
				continue;
			} catch (std::exception &e) {
				LC_LOG_WARNING("catch stl exception:%s", e.what());
				continue;
			} catch (...) {
				continue;
			}

		} else if (iter->value().is_dict()){
			output[iter->key()] = rp.create<bsl::var::Dict>();
			merge_var(output[iter->key()], iter->value(), rp);
		} else {
			output[iter->key()] = iter->value();
		}
    }

    if (all == 1) {
		
		struct timeval &tm = _monitor->get_start_running_time();
		struct timeval currtm;
		gettimeofday(&currtm, NULL);
		long using_time = (currtm.tv_sec-tm.tv_sec)*1000 + (currtm.tv_usec-tm.tv_usec)/1000;
		try {
			if (output["__global"].is_null()) {
				output["__global"] = rp.create<bsl::var::Dict>();
			}
			output["__global"]["RUNNING_TIME"] = rp.create<bsl::var::Int64>(using_time); 
			bsl::var::IVar &output1 = rp.create<bsl::var::Dict>();
			output1[get_proc_name()] = output;

			
			
			bsl::var::McPackSerializer mcpack_ses;
			int ret = mcpack_ses.serialize(output1, buffer, buf_size);
			return ret;

		} catch (bsl::Exception &e) {
			LC_LOG_WARNING("catch bsl exception:%s", e.all());
			return -1;
		} catch (...) {
			LC_LOG_WARNING("catch unknown exception");
			return -1;
		}
    } else {
		bsl::var::McPackSerializer mcpack_ses;
		int ret = mcpack_ses.serialize(output, buffer, buf_size);
		return ret;
    }
    
    return -1;
}


int SocketMonitorOutput :: merge_var(bsl::var::IVar &output, bsl::var::IVar &input, bsl::ResourcePool &rp)
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
				output[iter->key()] = call_function_object(rp, node, iter->key());

			} catch (bsl::Exception &e) {
				LC_LOG_WARNING("catch bsl exception:%s", e.all());
				continue;
			} catch (std::exception &e) {
				LC_LOG_WARNING("catch stl exception:%s", e.what());
				continue;
			} catch (...) {
				continue;
			}
		} else if(node.is_dict()) {
			output[iter->key()] = rp.create<bsl::var::Dict>();
			merge_var(output[iter->key()], iter->value(), rp);			
		} else {
			output[iter->key()] = iter->value();
		}

    }
    return 0;
}

SocketMonitorOutput :: ~SocketMonitorOutput()
{
	if (_server) {
		lc_server_destroy(_server);
		_server = NULL;
	}
	if (_auth) {
		lc_auth_destroy(_auth);
		_auth = NULL;
	}
}

}




















