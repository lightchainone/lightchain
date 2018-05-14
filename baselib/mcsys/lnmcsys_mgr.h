#ifndef  __LNMCSYS_MGR_H_
#define  __LNMCSYS_MGR_H_


#include "lc.h"
#include "lc/netreactor.h"
#include "lnmcsys_server.h"
#include "redis_data.h"

namespace lnsys {
    class LnmcsysManager
    {
        plclic:
            LnmcsysManager();
            virtual ~LnmcsysManager();

            void init(int argc, char** argv);

            void run();

            void stop();

            void join();

            void destroy();
        plclic:
            
            
            
            LNSYSMySQLWorker* get_mysql_worker();
            
            
            
            const lnmcsys_conf_t *get_lnmcsys_conf();
            
            
            
            static LnmcsysManager *get_lnmcsys_chain();

        private:
            
            
            
            void _init_mysql_worker(const comcfg::ConfigUnit &conf);

            void _init_redis_handler(const comcfg::ConfigUnit &conf);
            
            
            
            
            
            
            
            void _init_lnmcsys_conf(const comcfg::ConfigUnit &conf);


        private:
            lc_t* _lc_fw; 
            lc_conf_data_t *_lc_conf;
            lc_server_t *_lc_server;
            lc_svr_vec_t *_lc_svr_vec; 
            LnmcsysServer *_lnmcsys_serv;
            bool _is_init;                                              
            bool _is_running;                                       
            LNSYSMySQLWorker *_mysql_worker;     
            lnmcsys_conf_t _lnmcsys_conf; 

    };

};


#endif  


