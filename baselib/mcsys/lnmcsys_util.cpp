#include "lnmcsys_util.h"
#include <openssl/md5.h>
#include <iomanip>
#include <sstream>
using namespace std;

namespace lnsys{

    
    uint64_t  LnmcsysUtil::get_key(const char* deviceId){
        char akey[1024];
        unsigned s1,s2;
        snprintf(akey, sizeof(akey), "%s", deviceId);
        creat_sign_f64(akey,strlen(akey),&s1,&s2);
        uint64_t ret;
        uint32_t *p1=(uint32_t*)&ret;
        uint32_t *p2=(uint32_t*)&ret+1;
        *p1=s1;
        *p2=s2;
        return ret;
    };

    string LnmcsysUtil::get_mc_num_key(const string& chainkey, uint32_t key){
        stringstream ss;
        ss<<chainkey<<key;
        string data = ss.str();
        ss.str("");
        unsigned char md[16];
        MD5_CTX ctx;
        MD5_Init(&ctx); 
        MD5_Update(&ctx,data.c_str(),data.size());
        MD5_Final(md,&ctx); 
        for(int  i=0; i<16; i++ ){
            ss<<setw(2)<<setfill('0')<<hex<<int(md[i]);
        }
        return ss.str();
    };

   uint64_t LnmcsysUtil::get_partition_table(uint64_t mysql_key,int partition_num){
        return  mysql_key%partition_num +1 ;
   };
  
    uint64_t  LnmcsysUtil::get_channel_chainkey(const char* deviceId,uint8_t customerId){
    
        uint64_t  chainkey=0;

        TRACE("deviceId:[%s]",deviceId);

        time_t timestamp = 0;
        ASSERT_SYS(-1 != (timestamp = time(NULL)), "get current timestamp failed [error_info: %m]");

        int rand_num=rand()%1000;

        unsigned long long part1 = timestamp;
        unsigned long long part2 = rand_num;
        char tmp[512];
        int ret_len=snprintf(tmp, 512, "%s%010llu%03llu",deviceId, part1, part2);
        tmp[ret_len]='\0';

        unsigned  tmp_len=strlen(tmp);
        TRACE("tmp:[%s],ret_len:%d,strlen:%u",tmp,ret_len,tmp_len);
        unsigned int s1,s2;

        creat_sign_f64(tmp,strlen(tmp),&s1,&s2);
        uint32_t *p1=(uint32_t*)&chainkey;
        uint32_t *p2=(uint32_t*)&chainkey + 1;
        *p1=s1;
        *p2=s2;
        
        //chainkey=chainkey&0x7FFFFFFFFFFFFFFF; 
        //
        uint64_t llTmp  = customerId ;
        llTmp = llTmp << (sizeof(uint64_t) - sizeof(uint8_t)) * 8 ;
        chainkey = chainkey&0x00FFFFFFFFFFFFFF;
        chainkey = chainkey|llTmp;

        TRACE("generate channel_chainkey succ [device_id: %s, timestamp: %lu, \
                rand_num:%d, s1: %u,s2:%u,  \
                chainkey: %lu, tmp: %s].",deviceId, timestamp, rand_num,s1,s2,chainkey,tmp);
        return chainkey;
    }

}

