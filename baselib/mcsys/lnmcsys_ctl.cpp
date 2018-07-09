#include "lnmcsys_mgr.h"
#include "lnmcsys_signal.h"

namespace lnsys {

	void print_version()
	{
		fprintf(stdout, "%s\n", "Module:\t\tLNMCSYS");
		fprintf(stdout, "Version:\t%s\n", VERSION);
		fprintf(stdout, "Build Date:\t%s %s\n", __DATE__, __TIME__);
		lc_safe_exit();
	}

	void show_usage()
	{
		fprintf(stderr, "usage: ./lnmcsys\n");
		lc_safe_exit();
	}


	static LnmcsysManager _lnmcsys_chain; 			
	LnmcsysManager::LnmcsysManager(){

		_is_init = false;
		_is_running = false;
		_lc_fw = NULL;
		_lc_server = NULL;
		_lc_svr_vec = NULL;
		_mysql_worker = NULL;  
		_lnmcsys_serv = NULL;
		memset(&_lnmcsys_conf, 0x00, sizeof(_lnmcsys_conf_t));
	}

	LnmcsysManager::~LnmcsysManager(){
	     destroy();

	}
         
	void LnmcsysManager::run()
	{
	
	
		ASSERT_SYS(true == _is_init, "%s", "run LnmcsysManager before init");
		ASSERT_SYS(false == _is_running, "%s", "LnmcsysManager is running now");
		LCFW_ASSERT(lc_svr_vec_run(_lc_svr_vec) == 0, "Run server vectors error.");
		_is_running = true;
		TRACE("run LnmcsysManager succ.");
	}

	void LnmcsysManager::stop(){
 	

		if(_is_running)
		{
			lc_server_stop(_lc_server);
			_is_running = false;
			TRACE("Start to stop LnmcsysManager succ.");
		}
		else
		{
			TRACE("LnmcsysManager has been stopped before.");
		}
	}
	void LnmcsysManager::join(){
		lc_svr_vec_join(_lc_svr_vec);
	}
	void LnmcsysManager::destroy(){
		DO_IF(_is_running, stop(), _is_running = false);
		join();
		
		SAFE_DESTROY(_mysql_worker);
	
	
		DO_IF(NULL != _lc_svr_vec, lc_svr_vec_destroy(_lc_svr_vec), _lc_svr_vec = NULL, _lc_server = NULL);
		DO_IF(NULL != _lc_server, lc_server_destroy(_lc_server), _lc_server = NULL);
		DO_IF(NULL != _lc_fw, lc_close(_lc_fw), _lc_fw = NULL);
		SAFE_DELETE(_lnmcsys_serv);
		DO_IF(_is_init, lc_log_close(), LnmcsysDb::lnmcsys_db_destroy_key());
		memset(&_lnmcsys_conf, 0x00, sizeof(lnmcsys_conf_t));
		_is_init = false;
	}

	
	
	LnmcsysManager *LnmcsysManager::get_lnmcsys_chain()
	{
		return &_lnmcsys_chain;
	}
	
	
	
	
	const lnmcsys_conf_t *LnmcsysManager::get_lnmcsys_conf()
	{
		return &_lnmcsys_conf;
	}

	
	
	
	void LnmcsysManager::init(int argc, char** argv)
	{
		try{
		ASSERT_SYS(false == _is_init, "%s", "Object LnmcsysManager has been init before");
		
		_lc_fw = lc_init("lnmcsys", argc, argv, print_version, show_usage);
		ASSERT_SYS(NULL != _lc_fw, "init _lc_fw failed");
		
		lc_conf_data_t *conf = lc_open_conf(_lc_fw);
		ASSERT_SYS(NULL != conf, "lc open conf failed [path: %s, file: %s]", _lc_fw->path, _lc_fw->file);
		ASSERT_SYS(0 == lc_load_log(_lc_fw), "lc load log failed");

		_lc_svr_vec = lc_svr_vec_create();
		ASSERT_SYS(NULL != _lc_svr_vec, "create lc_server_vec_error");

		_lc_server = lc_load_svr(_lc_fw, "lnmcsys");
		ASSERT_SYS(_lc_fw->conf_build || _lc_server != NULL, "create lnmcsys server error");	
		lc_server_setoptsock(_lc_server, LCSVR_NODELAY);
		lc_svr_vec_add(_lc_svr_vec, _lc_server, _lc_fw);

		lc_builddone_exit(_lc_fw);
		
		deal_with_signals(4, SIGINT, SIGTERM, SIGQUIT, SIGPIPE);
		auto_mask mask(4, SIGINT, SIGHUP, SIGTERM, SIGQUIT);

       		_init_lnmcsys_conf(*(conf->conf_new));
		
		
		_init_redis_handler((*(conf->conf_new))["redis"]);

		_lnmcsys_serv = new (std::nothrow) LnmcsysServer(_mysql_worker, &_lnmcsys_conf);
		ASSERT_SYS(NULL != _lnmcsys_serv, "canno allocate memory for _lnmcsys_serv");
		_lnmcsys_serv->registerWithLc(_lc_server);

		LnmcsysDb::lnmcsys_db_init_key();
		
		_is_init = true;

		srand(time(NULL));
		
		TRACE("init LnmcsysManager object succ.");
		}catch(...){
		  FATAL("init fail");
		  exit(-1);
		}
	}

	
	
	
	void LnmcsysManager::_init_mysql_worker(const comcfg::ConfigUnit &conf)
	{
		SAFE_DELETE(_mysql_worker);
		_mysql_worker = new(std::nothrow) LNSYSMySQLWorker();
		ASSERT_SYS(NULL != _mysql_worker, "%s", "allocate memory for _mysql_worker failed");
		int ret = _mysql_worker->init(conf);
		ASSERT_MYSQL(LNSYS_E_OK == ret, "%s", "init _mysql_worker failed");
	}	


	void LnmcsysManager::_init_redis_handler(const comcfg::ConfigUnit &conf)
	{

		redis::RedisClientPool*  redis_pool=redis::RedisClientPool::getInstance();
		ASSERT_SYS(NULL != redis_pool,"cannot getIntance of RedisClientPool");
		int ret=redis_pool->init(conf);
		ASSERT_REDIS(0==ret, "init redis pool Configure fail");
	}	


	
	
	void LnmcsysManager::_init_lnmcsys_conf(const comcfg::ConfigUnit &conf)
	{
		_lnmcsys_conf.chainkey_invalid_timeout = conf["lnmcsys"]["chainkey_invalid_timeout"].to_uint32(NULL);
		_lnmcsys_conf.mysql_conn_timeout = conf["lnmcsys"]["mysql_conn_timeout"].to_uint32(NULL);
		_lnmcsys_conf.mysql_num = conf["mysql_worker"]["server_num"].to_uint32(NULL);
		_lnmcsys_conf.mysql_partition_num = conf["mysql_worker"]["partition_num"].to_uint32(NULL);
		SNPRINTF(_lnmcsys_conf.team, "%s", conf["lnmcsys"]["team"].to_cstr());
		SNPRINTF(_lnmcsys_conf.product, "%s", conf["lnmcsys"]["product"].to_cstr());
        _lnmcsys_conf.customer_id = conf["lnmcsys"]["customer_id"].to_uint16(NULL);

		
		TRACE("%s", "load config succ");
		_lnmcsys_conf.dump();
	}
	
}

