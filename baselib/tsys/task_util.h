#ifndef  __TASK_UTIL_H_
#define  __TASK_UTIL_H_

#include "task_exception.h"

namespace lnsys {

class TaskUtil {
plclic:
    static void get_newest_mcinfo_key(uint32_t, char*);
    static uint32_t  get_key(uint64_t, uint32_t);
    static void get_mcinfo_key(uint32_t, uint32_t, char*);
    static void get_bs_scan_key(uint32_t bs_id, char* key);
    static void get_acquire_bs_scan_key(uint32_t bs_id, char* key);
    static void read_from_mcinfo_key(const char* mcinfo_key, uint32_t& type, uint32_t& chainid, uint64_t&  mcinfo_id);
    static void read_from_mcinfo_key(const char* mcinfo_key, uint32_t& type, uint32_t& chainid, uint64_t&  mcinfo_id, uint64_t& user_key);
    static void set_mcinfo_key(char* mcinfo_key, const uint32_t type, const uint32_t chainid, const uint64_t mcinfo_id);
    static void get_mcinfo_info_from_value(const char* mcinfo_value, int64_t& create_time, 
            int64_t&  expire_time, uint32_t&  qps, uint32_t& delay,char* mcinfoContent);
};

}




#endif  


