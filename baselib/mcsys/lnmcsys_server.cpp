#include "lnmcsys_server.h"
#include "lnmcsys_db.h"
#include <Configure.h>
#include "lnmcsys_handler.h"
#include "redis_data.h"

using namespace lnsys;

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



