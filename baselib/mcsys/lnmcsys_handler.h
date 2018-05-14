#ifndef  __LNMCSYS_HANDLER_H_
#define  __LNMCSYS_HANDLER_H_

#include "def.h"
#include "utils.h"
#include "utils.h"
#include "lnmcsys_define.h"
#include "lnmcsys_server.h"
#include "redis_data.h"
#include <map>
#include "json/json.h"
using namespace std;
namespace lnsys
{
    
    
    
    class LnmcsysHandler
    {
    plclic:
        
        static int get_user_info(RedisData& redisUser, uint64_t chainkey, Json::Value& root, bool throw_exception=true);
        static void update_user_info(RedisData& redisUser, uint64_t chainkey,int expire_time, Json::Value& root);
        static void verify_channel_chainkey_id(lnsys::LnmcsysServlet* servlet, 
                uint8_t third_sys_type,
                const char*  deviceId, uint64_t chainkey, 
                uint32_t bs_id, uint64_t &retKey,
                 const idm::vector<idm::chaininfo_t>& chaininfo_list,
                idm::verify_channel_res_t* outres
                );

        static void LnmcsysHandler::save_channel_chainkey(
                LnmcsysServlet* servlet, const char* deviceId,uint8_t customerId, 
                uint64_t &chainkey, long  registerTime,
                const idm::vector<uint32_t>& chainids, 
                idm::register_chain_res_t  *register_res 
                );  

        static void  get_target_mcinfos(
                LnmcsysServlet *servlet, 
                uint64_t key, 
                const map<uint32_t, uint64_t>& chain_info_list,
                idm::vector<idm::target_mcinfo_t>* mcinfos
                ); 
        static void get_max_mcinfo_id(
                LnmcsysServlet *servlet, 
                const idm::get_max_mcinfo_id_req_t& req,
                idm::get_max_mcinfo_id_res_t*  res 
                );
        static void bind_alias(lnsys::LnmcsysServlet* servlet, 
            uint64_t chainkey,
            uint32_t key, 
            const char* alias_name);
        static void unbind_alias(lnsys::LnmcsysServlet* servlet, 
                uint64_t chainkey,
                uint32_t key, 
                const char* alias_name, bool unbind_all);
        static void amc_add_chain(lnsys::LnmcsysServlet* servlet,uint64_t chainkey, const idm::vector<uint32_t>& chainlist); 
        static char* ch2data(char*);
        static char* data2ch(char*);

        static void save_mc_chainkey(const string& chainkey, const string& old_chainkey, uint32_t key, uint32_t old_key, string& ichainkey, bool login);
        static void mc_bind_alias(const string& chainkey, uint32_t key, const string& alias_name);
        static void mc_unbind_alias(const string& chainkey, uint32_t key, const string& alias_name);
        static void mc_add_tag(const idm::mc_add_tag_req_t& req);
        static void mc_delete_tag(const idm::mc_delete_tag_req_t& req);
        static void mc_set_mc_num(const string& chainkey, uint32_t key, uint32_t mc_num);
    };

};



#endif  


