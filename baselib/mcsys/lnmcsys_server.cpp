#include "lnmcsys_server.h"
#include "lnmcsys_db.h"
#include <Configure.h>
#include "lnmcsys_handler.h"
#include "redis_data.h"

using namespace lnsys;
int LnmcsysServlet::verify_channel(const idm::VsPusherLnmcsys_verify_channel_params &in, 
        idm::VsPusherLnmcsys_verify_channel_response &out)
{
    lc_timer_t timer;
    lc_timer_init(&timer);

    idm::verify_channel_res_t* outres = out.m_result_params()->m_verify_channel_res(); 
    
    char in_out_log[EXCEPTION_STR_MAX_LEN];
    in_out_log[0] = 0x00;
    uint64_t  retKey;

    FUNC_START
    const idm::verify_channel_req_t &req_params = in.verify_channel_req();
    uint64_t  chainkey = req_params.chainkey();
    const char *deviceid = req_params.deviceid();
    uint32_t bs_id = req_params.bs_id();
    uint8_t third_sys_type = req_params.third_sys_type(); 
    snprintf(in_out_log, sizeof(in_out_log),
            "service: %s, device_id: %s, chainkey: %lu ,bs_id:%u",
            __FUNCTION__, deviceid, chainkey,bs_id);

    ASSERT_SYS(0 != chainkey ,"error req chainkey is 0");
    ASSERT_SYS(NULL !=  deviceid ,"error req deviceid is NULL ");

    TRACE("START SERVICE params[%s].", in_out_log);

    LnmcsysHandler::verify_channel_chainkey_id(this, third_sys_type,deviceid,chainkey,bs_id,retKey,req_params.chain_info_list(), outres);

    FUNC_END(TRACE, WARNING)

    int result = LNSYS_E_OK;
    FUNC_GET_ERROR(result);
    out.m_result_params()->m_verify_channel_res()->set_result(result);
    out.m_result_params()->m_verify_channel_res()->set_key(retKey);
    if(LNSYS_E_OK == result)
    {
        TRACE("SUCC END SERVICE params[%s] results[result: %d].", in_out_log, result);
    }
    else
    {
        WARNING("FAIL END SERVICE params[%s] results[result: %d]", in_out_log, result);
    }
    return LNSYS_E_OK;
}

int LnmcsysServlet::mc_register_chain(const idm::VsPusherLnmcsys_mc_register_chain_params &in, 
        idm::VsPusherLnmcsys_mc_register_chain_response &out) {

    char in_out_log[EXCEPTION_STR_MAX_LEN];
    in_out_log[0] = 0x00; 
    string chainkey = "";
    string ichainkey = "";
    FUNC_START
    const idm::mc_register_chain_req_t &req_params = in.register_chain_req();
    uint32_t key = req_params.key();
    chainkey = req_params.chainkey();
    string old_chainkey = "";
    uint32_t old_key = 0; 
    bool login = false;
    if(req_params.has_old_chainkey()){
        old_chainkey = req_params.old_chainkey();
        if(old_chainkey == chainkey){
            old_chainkey = "";
        }
    }

    if(req_params.has_old_key()){
        old_key = req_params.old_key();
        if(old_key == key){
            old_key = 0;
        }
    }

    if(req_params.has_ichainkey()){
        ichainkey = req_params.ichainkey();
    }

    if(req_params.has_login()){
        login = req_params.login();
    }

    LnmcsysHandler::save_mc_chainkey(chainkey, old_chainkey, key, old_key, ichainkey, login);
    FUNC_END(TRACE, WARNING)

    int result = STATUS_OK;
    FUNC_GET_ERROR(result)
    out.m_result_params()->m_register_chain_res()->set_result(result);
    out.m_result_params()->m_register_chain_res()->set_ichainkey(ichainkey.c_str(), ichainkey.size());
    if(STATUS_OK != result){
        WARNING("FAIL END SERVICE params[%s] results[result: %d,  chainkey: %s].", 
                in_out_log,result,chainkey.c_str());
    }
    return STATUS_OK;
}

int LnmcsysServlet::register_chain(const idm::VsPusherLnmcsys_register_chain_params &in, 
        idm::VsPusherLnmcsys_register_chain_response &out) {

    
    char in_out_log[EXCEPTION_STR_MAX_LEN];
    in_out_log[0] = 0x00; 
    time_t cur_time;
    time(&cur_time);

    unsigned int chainkey_invalid_time=_lnmcsys_conf->chainkey_invalid_timeout;
    time_t expired_time=cur_time+chainkey_invalid_time;
    uint64_t chainkey=0;  
    FUNC_START
    const idm::register_chain_req_t &req_params = in.register_chain_req();
    const char *deviceid = req_params.deviceid();
    const idm::vector<uint32_t> chainids=req_params.chainid();
    ASSERT_PARAM(NULL!=deviceid,"deviceid is NULL");
    if(req_params.has_chainkey()){
        chainkey = req_params.chainkey();
    }

    idm::register_chain_res_t  *register_chain_res = 
        out.m_result_params()->m_register_chain_res();
    LnmcsysHandler::save_channel_chainkey(this,deviceid,_lnmcsys_conf->customer_id,chainkey,cur_time,chainids, register_chain_res);
    FUNC_END(TRACE, WARNING)

    int result = STATUS_OK;
    FUNC_GET_ERROR(result)
    out.m_result_params()->m_register_chain_res()->set_result(result);
    if(STATUS_OK==result){
        out.m_result_params()->m_register_chain_res()->set_chainkey(chainkey);
       

        TRACE("SUCC END SERVICE params[%s] results[result: %d,  chainkey: %lu, expired_time: %ld].", 
              in_out_log,result,chainkey, expired_time);
    }else{

        WARNING("FAIL END SERVICE params[%s] results[result: %d,  chainkey: %lu, expired_time: %ld].", 
                in_out_log,result,chainkey, expired_time);
    }

    return STATUS_OK;

}

int LnmcsysServlet::amc_add_chain(const idm::VsPusherLnmcsys_amc_add_chain_params& in, 
        idm::VsPusherLnmcsys_amc_add_chain_response& out){
    idm::amc_add_chain_res_t* res = out.m_result_params()->m_amc_add_chain_res();
    const idm::amc_add_chain_req_t& req = in.amc_add_chain_req();
    const idm::vector<uint32_t>& req_chainlist = req.chainlist();

    FUNC_START
    LnmcsysHandler::amc_add_chain(this,req.chainkey(), req_chainlist);
    FUNC_END(TRACE, WARNING)

    int result = STATUS_OK;
    FUNC_GET_ERROR(result)
    res->set_result(result);
    idm::vector<uint32_t>* chainlist = res->m_add_chainlist();
    for(uint32_t i=0; i<req_chainlist.size();++i){
        chainlist->sys_back(req_chainlist[i]);
    }
    return STATUS_OK;
}

int LnmcsysServlet::bind_alias(const idm::VsPusherLnmcsys_bind_alias_params& in,
                                          idm::VsPusherLnmcsys_bind_alias_response& out)
{
    idm::bind_alias_res_t* res = out.m_result_params()->m_bind_alias_res();

    FUNC_START

    const idm::bind_alias_req_t& req = in.bind_alias_req();
    LnmcsysHandler::bind_alias(this,req.chainkey(), req.chainid(), req.alias_name());
    FUNC_END(TRACE, WARNING)

    int result = STATUS_OK;
    FUNC_GET_ERROR(result)
    res->set_result(result);
    return STATUS_OK;
}

int LnmcsysServlet::unbind_alias(const idm::VsPusherLnmcsys_unbind_alias_params& in,
                                          idm::VsPusherLnmcsys_unbind_alias_response& out)
{
    idm::unbind_alias_res_t* res = out.m_result_params()->m_unbind_alias_res();

    FUNC_START

    const idm::unbind_alias_req_t& req = in.unbind_alias_req();
    LnmcsysHandler::unbind_alias(this,req.chainkey(), req.chainid(), req.alias_name(), req.unbind_all());
    FUNC_END(TRACE, WARNING)

    int result = STATUS_OK;
    FUNC_GET_ERROR(result)
    res->set_result(result);
    return STATUS_OK;
}

int LnmcsysServlet::mc_bind_alias(const idm::VsPusherLnmcsys_mc_bind_alias_params &in, 
        idm::VsPusherLnmcsys_mc_bind_alias_response &out)
{
    idm::mc_bind_alias_res_t* res = out.m_result_params()->m_bind_alias_res();

    FUNC_START

    const idm::mc_bind_alias_req_t& req = in.bind_alias_req();
    LnmcsysHandler::mc_bind_alias(req.chainkey(), req.chainid(), req.alias_name());
    FUNC_END(TRACE, WARNING)

    int result = STATUS_OK;
    FUNC_GET_ERROR(result)
    res->set_result(result);
    return STATUS_OK;
}

int LnmcsysServlet::mc_set_mc_num(const idm::VsPusherLnmcsys_mc_set_mc_num_params &in, 
        idm::VsPusherLnmcsys_mc_set_mc_num_response &out)
{
    idm::mc_set_mc_num_res_t* res = out.m_result_params()->m_set_mc_num_res();

    FUNC_START

    const idm::mc_set_mc_num_req_t& req = in.set_mc_num_req();
    LnmcsysHandler::mc_set_mc_num(req.chainkey(), req.chainid(), req.mc_num());
    FUNC_END(TRACE, WARNING)

    int result = STATUS_OK;
    FUNC_GET_ERROR(result)
    res->set_result(result);
    return STATUS_OK;
}


int LnmcsysServlet::mc_unbind_alias(const idm::VsPusherLnmcsys_mc_unbind_alias_params &in, 
        idm::VsPusherLnmcsys_mc_unbind_alias_response &out)
{
    idm::mc_unbind_alias_res_t* res = out.m_result_params()->m_unbind_alias_res();

    FUNC_START
    const idm::mc_unbind_alias_req_t& req = in.unbind_alias_req();
    LnmcsysHandler::mc_unbind_alias(req.chainkey(), req.chainid(), req.alias_name());
    FUNC_END(TRACE, WARNING)

    int result = STATUS_OK;
    FUNC_GET_ERROR(result)
    res->set_result(result);
    return STATUS_OK;
}

int LnmcsysServlet::mc_add_tag(const idm::VsPusherLnmcsys_mc_add_tag_params &in, 
        idm::VsPusherLnmcsys_mc_add_tag_response &out){

    idm::mc_add_tag_res_t* res = out.m_result_params()->m_mc_add_tag_res();
    FUNC_START
    const idm::mc_add_tag_req_t& req = in.mc_add_tag_req();
    LnmcsysHandler::mc_add_tag(req);
    FUNC_END(TRACE, WARNING)

    int result = STATUS_OK;
    FUNC_GET_ERROR(result)
    res->set_result(result);
    return STATUS_OK;
}

int LnmcsysServlet::mc_delete_tag(const idm::VsPusherLnmcsys_mc_delete_tag_params &in, 
        idm::VsPusherLnmcsys_mc_delete_tag_response &out)
{
    idm::mc_delete_tag_res_t* res = out.m_result_params()->m_mc_delete_tag_res();
    FUNC_START
    const idm::mc_delete_tag_req_t& req = in.mc_delete_tag_req();
    LnmcsysHandler::mc_delete_tag(req);
    FUNC_END(TRACE, WARNING)

    int result = STATUS_OK;
    FUNC_GET_ERROR(result)
    res->set_result(result);
    return STATUS_OK;
}

int LnmcsysServlet::get_target_mcinfos(const idm::VsPusherLnmcsys_get_target_mcinfos_params& in,
                                          idm::VsPusherLnmcsys_get_target_mcinfos_response& out)
{
    idm::get_target_mcinfo_res_t* res = out.m_result_params()->m_get_target_mcinfo_res();

    FUNC_START

    const idm::get_target_mcinfo_req_t& req = in.get_target_mcinfo_req();
    idm::vector<idm::target_mcinfo_t>* mcinfos = res->m_mcinfos();
    uint64_t key = req.key();

    map<uint32_t, uint64_t>  chain_info_list_map;
    for(unsigned int i=0; i<req.chain_info_list_size(); ++i){
        idm::chain_info_t chain_info  = req.chain_info_list(i);
        uint32_t chainid = chain_info.chainid();
        uint64_t expect_mcinfo_id = chain_info.expect_mcinfo_id();
        chain_info_list_map[chainid] = expect_mcinfo_id;
    }

    LnmcsysHandler::get_target_mcinfos(this,key,chain_info_list_map, mcinfos);
    FUNC_END(TRACE, WARNING)

    int result = STATUS_OK;
    FUNC_GET_ERROR(result)
    res->set_result(result);
    return STATUS_OK;
}

int LnmcsysServlet::get_max_mcinfo_id(const idm::VsPusherLnmcsys_get_max_mcinfo_id_params& in,
                                          idm::VsPusherLnmcsys_get_max_mcinfo_id_response& out)
{
    idm::get_max_mcinfo_id_res_t* res = out.m_result_params()->m_get_max_mcinfo_id_res();

    FUNC_START

    const idm::get_max_mcinfo_id_req_t& req = in.get_max_mcinfo_id_req();

    for(unsigned int i=0; i<req.chainid_size(); ++i){
        LnmcsysHandler::get_max_mcinfo_id(this,req,res);
    }

    FUNC_END(TRACE, WARNING)

    int result = STATUS_OK;
    FUNC_GET_ERROR(result)
    res->set_result(result);
    return STATUS_OK;
}

LNSYSMySQLWorker*  LnmcsysServlet::get_mysql_worker(){
    return _mysql_worker;
}

lnmcsys_conf_t*  LnmcsysServlet::get_lnmcsys_conf(){

    return _lnmcsys_conf;

}



