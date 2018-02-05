


#ifndef  __LCMONITOR_H_
#define  __LCMONITOR_H_

#include <bsl/var/IVar.h>
#include <bsl/var/Function.h>
#include <bsl/var/Int32.h>
#include <bsl/var/Int64.h>
#include <bsl/var/Ref.h>
#include <bsl/var/Dict.h>
#include <bsl/ResourcePool.h>
#include <bsl/deque.h>
#include <bsl/map.h>
#include <pthread.h>

#include <stdint.h>

#include "MonitorOutput.h"
#include "FileMonitorOutput.h"
#include "SocketMonitorOutput.h"

namespace lc
{

void ms_sleep(int ms);


typedef bsl::var::IVar &(*query_item_callback)(bsl::ResourcePool &pool);

typedef struct _thread_info {
	bsl::var::Ref monitor_info;
    bsl::var::Ref monitor_info_time;
	bsl::ResourcePool rp;
} thread_info;


class LcMonitor {
plclic:

    
    int register_static_item(const char *path, signed long long init_vlaue = 0);

	
    	
	int set_static_item(const char *path, signed long long value);

    
    int init(const char *path, const char *file);

    
    int init(ul_confdata_t *tmp_conf);

    
    
    
    
    
    int register_service(const char *service_name, int thread_num);
    
    
    int register_item(const char *path, int compute_tag = 0);
    
    
    int register_monitor_item(const char *path, bsl::var::Function::function_type callback, void *arg, int type = 0);

    
    
    

    
    int set_start_running_time(struct timeval &tm);

    
    struct timeval &get_start_running_time();

    
    int set_program_version(const char *program_version);

    
    int set_lib_version(const char *lib_version);

    
    int set_data_version(const char *data_version);

    
    bsl::ResourcePool *get_set_monitor_info_rp();

    
    int set_monitor_info(const char *path, bsl::var::IVar &info);

    
    void set_query_item_callback(query_item_callback callback) {
        if (NULL == callback) {
            return;
        }
        _query_item_callback = callback;
    }

    
    
    

    
    
    int add_request_number_success(const char *service_name);          

    
    int add_request_number_failed(const char *service_name);

    
    int add_request_connect_number(const char *service_name);

    
    int add_compute_time(const char *service_name, int compute_time);

    
    int add_self_process_time(const char *service_name, int self_process_time);

    
    int add_self_action_time(const char *service_name, int self_action_time);

    
    int add_io_read_time(const char *service_name, int io_read_time);

    
    int add_io_write_time(const char *service_name, int io_write_time);

    
    int add_fatal_number(const char *service_name);
	
    int add_read_error_number(const char *service_name);

    int add_write_error_number(const char *service_name);
    
    int add_path(const char *path, signed long long num = 1);


    
    
    

    
    int run();

    
    int stop();

    
    int join();

    
    
    
    
    
    bsl::var::IVar & query_path(bsl::var::IVar &total, const char *path, bsl::ResourcePool &rp, int *err_code, int exp_tag=0, char *last_name=NULL, int last_name_len=0);
	bsl::var::IVar& query_path_ex(bsl::var::IVar &total, const char *path, int *err_code, char *last_name=NULL, int last_name_len=0);

    
    bsl::var::IVar & query_path_father(bsl::var::IVar &total, const char *path, bsl::ResourcePool &rp, int *err_code, char *last_name, int last_name_len);

    
    bsl::var::IVar & query_path_outer(const char *path, bsl::ResourcePool &rp, int *err_code, char *last_name=NULL, int last_name_len=0);

    
    int get_compute_time() {
        return _compute_time_ms;
    }

    
    void set_compute_time(int compute_time) {
        _compute_time_ms = compute_time;
    }

    
    int get_monitor_time() {
        return _monitor_time_ms;
    }

    
    void set_monitor_time(int monitor_time) {
        _monitor_time_ms = monitor_time;
    }

    
    bsl::var::IVar &query_item(bsl::ResourcePool &rp) {
        if (NULL == _query_item_callback) {
            return bsl::var::Null::null;
        }
        return _query_item_callback(rp);
    }

    
    LcMonitor():
        _thread_list_usingnum(0),
        _compute_threadid(0),
        _compute_run(1),
        _compute_time_ms(5000),
        _monitor_time_ms(60000)
    {
        pthread_mutex_init(&_thread_list_lock, NULL);
        _monitor_info = _rp.create<bsl::var::Dict>();
        _monitor_info_total = _rp.create<bsl::var::Dict>();
        _monitor_info_time = _rp.create<bsl::var::Dict>();
        _thread_info_list.create();
        _output_list.create();
        _service_thread_map.create(1024);
        pthread_key_create(&_thread_key, NULL);
        _compute_thread_info.create(1024);
        _func_arg_map.create(1024);
        gettimeofday(&_start_time, NULL);
        _query_item_callback = NULL;
        _need_join = 0;
    }

    
    ~LcMonitor() {
        stop();
        join();
        pthread_mutex_destroy(&_thread_list_lock);
        _thread_info_list.destroy();
        int item_size = _output_list.size();
        for (int i=0; i<item_size; i++) {
            MonitorOutput *output = _output_list[i];
            if (output) {
                delete output;
                output = NULL;
            }
        }
        _output_list.clear();

        _output_list.destroy();
        _service_thread_map.destroy();
        pthread_setspecific(_thread_key, NULL);
        pthread_key_delete(_thread_key);
        _compute_thread_info.destroy();
        _func_arg_map.destroy();
    }

protected:
    
    static void *compute_func(void *arg);

    
    int merge_var(bsl::var::IVar &result, bsl::var::IVar &input, int tag = 0);
    
    
    bsl::var::IVar &gen_service_monitor_var(bsl::ResourcePool &rp, int *err_code);
    
    bsl::var::IVar &gen_service_monitor_var1(bsl::ResourcePool &rp, int *err_code);

    
    thread_info *get_thread_info();

    
    thread_info *create_thread_info();

    
    void do_compute();

    
    void do_average();

    
    void do_average_leaf(bsl::var::IVar &monitor_info, bsl::var::IVar &real_monitor_info, bsl::var::IVar &time_monitor_info, int total=0);

    
    int gen_compute_thread_info();

    
    int set_service_thread_key(const char *key, long long value=0);



private:
    
    void compute_average_value(bsl::var::IVar &output_info, bsl::var::IVar &total_info, bsl::var::IVar &times_info, int total); 

protected:
    pthread_mutex_t _thread_list_lock;		          
    int _thread_list_usingnum;		                  
    bsl::deque<thread_info*> _thread_info_list;		  
    pthread_key_t _thread_key;		                  

    bsl::var::Ref _monitor_info;		              
    bsl::hashmap<long, void *> _func_arg_map;		  
    bsl::var::Ref _monitor_info_total;		          
    bsl::ResourcePool _rp;		                      

    bsl::deque<MonitorOutput *> _output_list;		  
 
    pthread_t _compute_threadid;		              
    
    bsl::hashmap<bsl::string, long long> _compute_thread_info;
    int _compute_run;		                          
    int _compute_time_ms;		                      

    struct timeval _start_time;		                  
    bsl::string _program_version;		              
    bsl::string _lib_version;		                  
    bsl::string _data_version;		                  

    bsl::hashmap<bsl::string, int> _service_thread_map;		      
    
    query_item_callback _query_item_callback;		  

    int _monitor_time_ms;        		              

    int _need_join;

    bsl::var::Ref _monitor_info_time;
};

}

#endif


  


