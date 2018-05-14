#ifndef  __LNMCSYS_SERVER_H_
#define  __LNMCSYS_SERVER_H_

#include "def.h"
#include "utils.h"
#include "lnmcsys_util.h"
#include "lnmcsys_db.h"
#include "lnmcsys_define.h"
#include "lnsys_lnmcsys.idm.h"
#include "lnsys_lnmcsys.idm.VsPusherLnmcsys.h"


namespace lnsys
{
    class LnmcsysServlet: plclic idm::VsPusherLnmcsys_servlet_trunk
    {
        plclic:
            LnmcsysServlet(LNSYSMySQLWorker *mysql_worker, lnmcsys_conf_t *conf): _mysql_worker(mysql_worker),_lnmcsys_conf(conf) {}
            virtual ~LnmcsysServlet() {}
            int verify_channel(const idm::VsPusherLnmcsys_verify_channel_params &, idm::VsPusherLnmcsys_verify_channel_response &);
            int register_chain(const idm::VsPusherLnmcsys_register_chain_params &, idm::VsPusherLnmcsys_register_chain_response &);

            int get_target_mcinfos(const idm::VsPusherLnmcsys_get_target_mcinfos_params&, 
                                    idm::VsPusherLnmcsys_get_target_mcinfos_response&);

            int get_max_mcinfo_id(const idm::VsPusherLnmcsys_get_max_mcinfo_id_params& in,
                    idm::VsPusherLnmcsys_get_max_mcinfo_id_response& out);

            int bind_alias(const idm::VsPusherLnmcsys_bind_alias_params& in,
                    idm::VsPusherLnmcsys_bind_alias_response& out);

            int unbind_alias(const idm::VsPusherLnmcsys_unbind_alias_params& in,
                    idm::VsPusherLnmcsys_unbind_alias_response& out);
            int amc_add_chain(const idm::VsPusherLnmcsys_amc_add_chain_params& in, 
                    idm::VsPusherLnmcsys_amc_add_chain_response& out);

            //for mc

            int mc_register_chain(const idm::VsPusherLnmcsys_mc_register_chain_params &in, 
                    idm::VsPusherLnmcsys_mc_register_chain_response &out);

            int mc_bind_alias(const idm::VsPusherLnmcsys_mc_bind_alias_params &in, 
                    idm::VsPusherLnmcsys_mc_bind_alias_response &out);

            int mc_unbind_alias(const idm::VsPusherLnmcsys_mc_unbind_alias_params &in, 
                    idm::VsPusherLnmcsys_mc_unbind_alias_response &out);

            int mc_add_tag(const idm::VsPusherLnmcsys_mc_add_tag_params &in, 
                    idm::VsPusherLnmcsys_mc_add_tag_response &out);

            int mc_delete_tag(const idm::VsPusherLnmcsys_mc_delete_tag_params &in, 
                    idm::VsPusherLnmcsys_mc_delete_tag_response &out);

            int mc_set_mc_num(const idm::VsPusherLnmcsys_mc_set_mc_num_params &in, 
                    idm::VsPusherLnmcsys_mc_set_mc_num_response &out);

            LNSYSMySQLWorker* get_mysql_worker();

            lnmcsys_conf_t*  get_lnmcsys_conf();

        private:
            LNSYSMySQLWorker  *_mysql_worker;
            lnmcsys_conf_t *_lnmcsys_conf;        
    };

    class LnmcsysServer : plclic lcrpc::LcRpcServer {
        plclic:
            LnmcsysServer(LNSYSMySQLWorker *mysql_worker, lnmcsys_conf_t *conf): _mysql_worker(mysql_worker),_lnmcsys_conf(conf){}
            virtual ~LnmcsysServer() {}
            virtual lcrpc::Servlet * createServlet() {
                _servlet = new(std::nothrow) LnmcsysServlet(_mysql_worker,_lnmcsys_conf);
                assert(NULL != _servlet);
                return _servlet;
            }
        protected:
            LnmcsysServlet *_servlet;
            LNSYSMySQLWorker  *_mysql_worker;
            lnmcsys_conf_t *_lnmcsys_conf;        
    };
}




#endif  


