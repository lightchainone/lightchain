#ifndef  __LNMCSYS_UTIL_H_
#define  __LNMCSYS_UTIL_H_

#include <sys/time.h>
#include "lnmcsys_exception.h"
#include "def.h"
#include "utils.h"
#include "ul_sign.h"
#include "utils.h"

using namespace std;

namespace lnsys{

    class LnmcsysUtil{
    
        plclic:

        LnmcsysUtil();
        ~LnmcsysUtil();

        static uint64_t get_key(const char* deviceId);
        static uint64_t  get_channel_chainkey(const char* deviceId,uint8_t customerId);
        static  uint64_t get_partition_table(uint64_t mysql_key,int partition_num);
        static string get_mc_num_key(const string& chainkey, uint32_t key);
    };

}



#endif  


