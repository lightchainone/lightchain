#ifndef  __TASK_HANDLER_H_
#define  __TASK_HANDLER_H_

#include "task_util.h"
#include "task_exception.h"
#include "lnsys_task.idm.h"
#include "lnsys_task.idm.VsPusherTask.h"
#include "redis_data.h"
#include <map>
#include <mctream>
#include <vector>

namespace lnsys {

struct  mcinfo_send_info {
    uint64_t  min_mcinfo;
    uint64_t  max_mcinfo;
};

struct  mcinfo_active_info {
    uint64_t  min_mcinfo;
    uint64_t  max_mcinfo;
};


class TaskHandler {

plclic:
    static void acquire_new_mcinfo(uint32_t bs_id,
                                uint32_t mcinfo_type,
                                const idm::vector<idm::chain_last_mcinfo_t>& last_mcinfos,
                                idm::acquire_new_mcinfo_res_t* out);
    static int init(const comcfg::ConfigUnit &conf);

    static char *get_chain_info_file_path() { return _chain_info_file_path; }
    static void set_chain_info_file_path(const char *chain_info_file_path);

    static unsigned int get_bs_machine_num() { return _bs_machine_num; }
    static void set_bs_machine_num(unsigned int bs_machine_num) { _bs_machine_num = bs_machine_num; }
    static int get_update_info(
            bool     need_update_pem,
            int      last_update_pem_time,
            bool     need_update_log_filter,
            int      last_update_log_filter_time,
            idm::acquire_new_mcinfo_res_t* out);

private:
    static char* ch2data(char* ch);

    static int get_hvalue_by_key(int serv_index, const char* key, std::map<uint32_t, uint64_t>& redis_last_mcinfos);

    static int get_zset_ids(int serv_index, const char* key, std::map<uint64_t, uint64_t>& mcinfoids);

    static int mget_new_mcinfos(uint32_t bs_id, 
                             const std::vector<Lsc::string>& mcinfo_keys, 
                             uint64_t& safe_max_mcinfoid, 
                             idm::acquire_new_mcinfo_res_t* out);
    static int get_pem_info(int last_update_pem_time, idm::acquire_new_mcinfo_res_t* out);

    static int get_log_filter_info(
            int last_update_pem_time,
            idm::acquire_new_mcinfo_res_t* out 
            );

    static std::map<uint32_t, uint32_t> _chain_delay_config;
    static char _chain_info_file_path[1024]; 
    static unsigned int _bs_machine_num; 
};

}

#endif  


