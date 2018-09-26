#include "task_util.h"

namespace lnsys {

void  TaskUtil::get_newest_mcinfo_key(uint32_t type, char* key)
{
    snprintf(key, 1024, "BEST%u", type);
}

uint32_t  TaskUtil::get_key(uint64_t key, uint32_t partition_size)
{
    return key % partition_size + 1;
}

void  TaskUtil::get_mcinfo_key(uint32_t bs_id, uint32_t partition_id, char* result)
{
}

void TaskUtil::get_bs_scan_key(uint32_t bs_id, char* key)
{
    snprintf(key, 1024, "%s%u", "ABS", bs_id);
}

void  TaskUtil::get_acquire_bs_scan_key(uint32_t bs_id, char* key)
{
    snprintf(key, 1024, "%s%u", "QBS", bs_id);
}

void TaskUtil::read_from_mcinfo_key(const char* mcinfo_key, uint32_t& type, uint32_t& chainid, uint64_t& mcinfo_id)
{
}

void TaskUtil::read_from_mcinfo_key(const char* mcinfo_key, uint32_t& type, uint32_t& chainid, uint64_t& mcinfo_id, uint64_t&  user_key)
{
}

void TaskUtil::set_mcinfo_key(char* mcinfo_key, const uint32_t type, const uint32_t chainid, const uint64_t mcinfo_id)
{
}

void TaskUtil::get_mcinfo_info_from_value(const char* mcinfo_value, int64_t& create_time,
        int64_t&  expire_time, uint32_t& qps, uint32_t& delay, char* mcinfoContent)
{
    char  local_mcinfo_value[MAX_REDIS_VALUE_LEN];
    snprintf(local_mcinfo_value, MAX_REDIS_VALUE_LEN, "%s", mcinfo_value);
    char* saveptr = NULL;
    char* p = strtok_r(local_mcinfo_value, delim, &saveptr);

    ASSERT_SYS(NULL != p, "mcinfo value  format error,cannot  get create_time");
    sscanf(p, "%ld", &create_time);
    p = strtok_r(NULL, delim, &saveptr);
    ASSERT_SYS(NULL != p, "mcinfo value  format error,cannot  get expire_time");
    sscanf(p, "%ld", &expire_time);

    p = strtok_r(NULL, delim, &saveptr);
    ASSERT_SYS(NULL != p, "mcinfo value  format error,cannot  get qps");
    sscanf(p, "%u", &qps);

    p = strtok_r(NULL, delim, &saveptr);
    ASSERT_SYS(NULL != p, "mcinfo value  format error,cannot  get delay");
    sscanf(p, "%u", &delay);

    p += strlen(p) + 1;
    ASSERT_SYS(NULL != p, "mcinfo value  format error, cannnot  get  mcinfoContent");
    snprintf(mcinfoContent, MAX_REDIS_VALUE_LEN, "%s", p);
    DEBUG("create_time=%ld expire_time=%ld qps=%u  delay=%u mcinfo_content=%s", 
            create_time, expire_time, qps, delay, mcinfoContent);
}

}




















