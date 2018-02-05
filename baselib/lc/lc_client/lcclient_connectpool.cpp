
#include "lcclient_connectpool.h"

#include <connection.h>
#include <lc_log.h>

#include "lcclient_connection.h"
#include "lcclient_strategy.h"
#include "lcclient_healthycheck.h"
#include "lcclient_define.h"
#include "lcclient_ioc.h"
#include "lcclient_connectserver.h"
#include "lcclient_manager.h"

namespace lc
{
	LcClientPool :: LcClientPool()
	{
		_mgr = &_manager;
		_def_stt = NULL;
		_def_checker = NULL;
		_stt = NULL;
		_checker = NULL;
		_enable = 1;
		_reloadTag = 0;

		_confSave = NULL;
		_oldConfSaveLen = 0;
		_confSaveLen = 0;
	}

	LcClientPool :: ~LcClientPool()
	{

		strategy_ioc *strategyIoc = lcclient_get_strategyioc();
		healthychecker_ioc *healthyCheckerIoc = lcclient_get_healthycheckerioc();

		if(NULL != _def_stt){
			delete _def_stt;
		} else {
			if (NULL != _stt) {
				strategyIoc->destroy(_stt);
			}
		}
		if(NULL != _def_checker){
			delete _def_checker;
		} else {
			if (NULL != _checker) {
				healthyCheckerIoc->destroy(_checker);
			}
		}

		

		if (NULL != _confSave) {
			delete [] _confSave;
			_confSave = NULL;
		}

	}

	int LcClientPool :: isEnable()
	{
		
		return _enable;
	}

	int LcClientPool :: setEnable(int enable) 
	{
		
		_enable = enable;
		
		return 0;
	}

	int LcClientPool :: init(const comcfg::ConfigUnit & conf)
	{
		strategy_ioc *strategyIoc = lcclient_get_strategyioc();
		if (NULL == strategyIoc) {
			LC_LOG_WARNING("get strategyIoc is NULL");
			return -1;
		}
		healthychecker_ioc *healthyCheckerIoc = lcclient_get_healthycheckerioc();
		if (NULL == healthyCheckerIoc) {
			LC_LOG_WARNING("get healthyCheckerIoc is NULL");
			return -1;
		}
		int ret;

		_mgr->setCreater(lcclient_connection_creater, lcclient_server_creater);

		
		if (conf[LCCLIENT_CONF_STRATEGY][LCCLIENT_CONF_STRATEGY_NAME].get_bsl_string(
					&_sttName, LCCLIENT_DEFAULT_STRATEGY) != 0) {

			LC_LOG_WARNING("uisng default strategy");
		}
		if (conf[LCCLIENT_CONF_HEALTHY][LCCLIENT_CONF_HEALTHY_NAME].get_bsl_string(
					&_checkerName, LCCLIENT_DEFAULT_HEALTHY) != 0) {
			LC_LOG_WARNING("uisng default healthyChecker");
		}
      
		ret = _mgr->init(conf);   
		if (0 != ret) {
			LC_LOG_WARNING("mgr init error");
			goto fail;
		}
		if (_mgr->isConnAll()) {
			_mgr->connectAll();
		}

		_stt = strategyIoc->create(_sttName);
		_checker = healthyCheckerIoc->create(_checkerName);

		if (NULL == _stt){   
			LC_LOG_WARNING("init strategy [%s] error", _sttName.c_str());
			_def_stt = new (std::nothrow) LcClientStrategy;
			if (NULL == _def_stt) {
				LC_LOG_WARNING("new LcClientStrategy error");
				goto fail;
			}
			_stt = _def_stt;
		    _mgr->setStrategy(_stt);
		} else {
		    _mgr->setStrategy(_stt);
			ret = _stt->load(conf[LCCLIENT_CONF_STRATEGY]);
			if (0 != ret) {
				LC_LOG_WARNING("strategy load conf error [%d]", ret);
			}
		}

		if (NULL == _checker){
			LC_LOG_WARNING("init healthyChecker [%s] error", _checkerName.c_str());
			_def_checker = new (std::nothrow) LcClientHealthyChecker;
			if (NULL == _def_checker) {    
				LC_LOG_WARNING("new LcClientHealthyChecker error");
				goto fail;
			}
			_checker = _def_checker;
		} else {
			ret = _checker->load(conf[LCCLIENT_CONF_HEALTHY]);
			if (0 != ret) {
				LC_LOG_WARNING("healthyChecker load conf error");
			}
		}

		_mgr->setHealthyChecker(_checker);

		ret = saveConf(conf);
		if (0 != ret) {
			LC_LOG_WARNING("saveConf error");
			goto fail;
		}

		return 0;

	fail:
		if (_def_stt != NULL) {
			delete _def_stt;
			_def_stt = NULL;
			_stt = NULL;
		} else {
			strategyIoc->destroy(_stt);
			_stt = NULL;
		}

		if (_def_checker != NULL) {
			delete _def_checker;
			_def_checker = NULL;
			_checker = NULL;
		} else {
			healthyCheckerIoc->destroy(_checker);
			_checker = NULL;
		}
		return -1;
	}


	int LcClientPool :: setStrategy(LcClientStrategy * stt)
	{
		int ret = _mgr->setStrategy(stt);
		if(ret == 0){
			_stt = stt;
		}
		return ret;
	}

	int LcClientPool :: setHealthyChecker(LcClientHealthyChecker * hchk)
	{
		int ret =  _mgr->setHealthyChecker(hchk);
		if(ret == 0){
			_checker = hchk;
		}
		return ret;
	}


	int LcClientPool :: reload(const comcfg::ConfigUnit & conf)
	{
		int ret = saveConf(conf);
		if (0 != ret) {
			return -1;
		}    

		ret = _mgr->reload(conf);

		if (0 != ret) {
			return -1;
		} else {
			if (_stt) {
				_stt->reload(conf);
				_stt->update();
			}
			if (_checker) {
				_checker->reload(conf);
			}
			return 0;
		}
		return 0;
	}

	

	LcClientConnection *LcClientPool :: fetchConnection(const default_server_arg_t &serverArg,
			bsl::var::IVar & info, int *errNo, LcClientManager *mgr)
	{
		if (NULL == errNo) {
			return NULL;
		}

		LcClientStrategy* lcclientstt = dynamic_cast<LcClientStrategy *>(_stt);
		if (NULL == lcclientstt) {
			*errNo = -1;
			return NULL;
		}
		int ret = lcclientstt->isInputRangeOK(serverArg.range);
		if (0 != ret) {
			*errNo = LC_TALK_RANGE_ERROR;
			return NULL;
		}

		LcClientConnection * conn = dynamic_cast<LcClientConnection*>
			(_mgr->fetchConnectionex(serverArg, info, mgr));

		if (NULL != conn) {
			*errNo = 0;
			return conn;
		} else {
			*errNo = -1;
			return NULL;
		}
	}

	int LcClientPool :: freeConnection(LcClientConnection *conn)
	{
		LC_LOG_DEBUG("freeConn");
		return _mgr->freeConnection(conn);
	}

	int LcClientPool :: fetchConnectionAsync(lcclient_talkwith_t *talkmsg, bsl::var::IVar &confVar, 
						lc::IEvent * event, lc::NetReactor *reactor)
	{
		LC_LOG_DEBUG("%s", __func__);

		if (NULL == talkmsg || NULL == event || NULL == reactor) {
			return -1;
		}

		talkmsg->retry = 0;
		LcClientStrategy* lcclientstt = dynamic_cast<LcClientStrategy *>(_stt);
		if (NULL == lcclientstt) {
			return -1;
		}
		int ret = lcclientstt->isInputRangeOK(talkmsg->defaultserverarg.range);
		if (0 != ret) {
			return LC_TALK_RANGE_ERROR;
		}
		ret = _mgr->fetchConnectionRetry(talkmsg, confVar, event, reactor);
		
		if (ret < 0) {
			LC_LOG_WARNING("fetch conn async error");
			return ret;
		}

		return 0;
	}

	LcClientConnectManager * LcClientPool :: getManager()
	{
		return _mgr;
	}

	int LcClientPool :: getServerCount()
	{
		return _mgr->getServerSize();
	}

	int LcClientPool :: getRealServerCount()
	{
		return _mgr->getRealServerSize();
	}


	LcClientServer* LcClientPool :: getServerByID(int id)
	{
		LcClientServer *server = dynamic_cast<LcClientServer *>(_mgr->getServerInfo(id));
		return server;
	}

	bsl::var::IVar & LcClientPool :: getConf(bsl::ResourcePool *rp, int *errNo)
	{
		LC_LOG_DEBUG("%s", __func__);
		static bsl::var::Null emptyVar;
		if (NULL == errNo) {
			return emptyVar;
		}

		if (NULL == rp ) {
			*errNo = -1;
			return emptyVar;
		}

		comcm::AutoRead autoread(&_poollock);
		if (0 == _confSaveLen) {
			*errNo = -1;
			return emptyVar;
		}
		comcfg::Configure tempConf;
		char *tempBuf = tempConf.getRebuildBuffer(_confSaveLen);
		if (NULL == tempBuf) {
			LC_LOG_WARNING("getRebuildBuffer of conf error");
			*errNo = -1;
			return emptyVar;
		}
		snprintf(tempBuf, _confSaveLen, "%s", _confSave);
		int ret_build = tempConf.rebuild();
        if (0 != ret_build) {
            LC_LOG_WARNING("build conf from string error:%d", ret_build);
            *errNo = -1;
            return emptyVar;
        }
		bsl::var::IVar & conf = tempConf.to_IVar(rp, errNo);
		if (*errNo != 0) {
			LC_LOG_WARNING("conf to var error");
			*errNo = -1;
			return emptyVar;
		}

		*errNo = 0;
		return conf;
	}

	int LcClientPool :: setReloadTag(int tag)
	{
		_reloadTag = tag;

		return 0;
	}

	int LcClientPool :: getReloadTag()
	{
		return _reloadTag;
	}

	int LcClientPool :: healthyCheck()
	{
		_checker->healthyCheck();
		return 0;
	}

	int LcClientPool :: getReqAndResBufLen(int &reqBufLen, int &resBufLen)
	{
		return _mgr->getReqAndResBuf(reqBufLen, resBufLen);
	}

	int LcClientPool :: getReqBufLen()
	{
		return _mgr->getReqBufLen();
	}

	int LcClientPool :: getResBufLen()
	{
		return _mgr->getResBufLen();
	}

	int LcClientPool :: setServerArg(LcClientServer *server, const nshead_talkwith_t *talk)
	{
		if (NULL == server || NULL == talk) {
			return -1;
		}

		if (NULL == _stt) {
			LC_LOG_WARNING("_stt is NULL");
			return -1;
		}

		return _stt->setServerArg(server, talk);
	}
 
	int LcClientPool :: setServerArgAfterConn(LcClientServer *server, int errNo)
	{
		if (NULL == server) {
			return -1;
		}

		if (NULL == _stt) {
			LC_LOG_WARNING("_stt is NULL");
			return -1;
		}

		return _stt->setServerArgAfterConn(server, errNo);
	}

	int LcClientPool :: saveConf(const comcfg::ConfigUnit & conf)
	{
		_poollock.write();

		bsl::ResourcePool rp;
		int errNo;
		bsl::var::IVar &tempVar = conf.to_IVar(&rp, &errNo);
		if (0 != errNo) {
			_poollock.unlock();
			LC_LOG_WARNING("conf to var error");
			return -1;
		}
		comcfg::Configure tempConf;
		int ret = tempConf.loadIVar(tempVar);
		if (0 != ret) {
			_poollock.unlock();
			LC_LOG_WARNING("var to conf error [%d]", ret);
			return -1;
		}

		const char *strSave;
		size_t strSaveLen = 0;       
		strSave = tempConf.dump(&strSaveLen);
		if (NULL == strSave) {
			_poollock.unlock();
			LC_LOG_WARNING("conf dump error");
			return -1;
		}

		if (_oldConfSaveLen <= (int)strSaveLen) {
			if (NULL != _confSave) {    
				delete [] _confSave;
				_confSave = NULL;
				_oldConfSaveLen = 0;
				_confSaveLen = 0;
			}
			_oldConfSaveLen = (int)strSaveLen + 1;
			_confSaveLen = (int)strSaveLen + 1;
			_confSave = new (std::nothrow)char [_oldConfSaveLen];
			if (NULL == _confSave) {
				_oldConfSaveLen = 0;    
				_confSaveLen = 0;
				_poollock.unlock();
				LC_LOG_WARNING("new char for save conf error");
				return -1;
			}
		} else {
			_confSaveLen = (int)strSaveLen+1;
		}
		snprintf(_confSave, _confSaveLen, "%s", strSave);
		_poollock.unlock();

		return 0;
	}

	bool LcClientPool :: getConnectType()
	{
		int ret = _mgr->getConnectType();
		if (ret == 1) {
			return false;
		}
		return true;
	}

	comcm::Server * lcclient_server_creater() 
	{
		return new (std::nothrow)LcClientServer;
	}

	int LcClientPool :: setConnType(int connType)
	{
		return _mgr->setConnType(connType);
	}

	comcm::Connection * lcclient_connection_creater(){
		return new (std::nothrow)LcClientConnection;
	}

    bsl::var::IVar & LcClientPool :: getPoolUsage(bsl::ResourcePool &rp)
    {
		int server_num = _mgr->getServerSize();
        bsl::var::IVar &output = rp.create<bsl::var::Dict>();
		for (int j=0; j<server_num; j++) {
			LcClientServer *server = dynamic_cast<LcClientServer*>(_mgr->getServerInfo(j));
			if (NULL == server) {
				return bsl::var::Null::null;
			}
            char server_name[32];
            snprintf(server_name, sizeof(server_name), "%s_%d", server->getIP(), server->getPort());
            lc_client_convert_ip(server_name);
            output[server_name] = rp.create<bsl::var::Int64>(server->getPoolUsage());
		}
        return output;   
	}

    bsl::var::IVar & LcClientPool :: getPoolUsingNum(bsl::ResourcePool &rp)
    {
		int server_num = _mgr->getServerSize();
        bsl::var::IVar &output = rp.create<bsl::var::Dict>();
		for (int j=0; j<server_num; j++) {
			LcClientServer *server = dynamic_cast<LcClientServer*>(_mgr->getServerInfo(j));
			if (NULL == server) {
				return bsl::var::Null::null;
			}
            char server_name[32];
            snprintf(server_name, sizeof(server_name), "%s_%d", server->getIP(), server->getPort());
            lc_client_convert_ip(server_name);
            output[server_name] = rp.create<bsl::var::Int64>(server->getPoolUsingNum());
		}
        return output;   
	}

    bsl::var::IVar & LcClientPool :: getPoolMaxNum(bsl::ResourcePool &rp)
    {
		int server_num = _mgr->getServerSize();
        bsl::var::IVar &output = rp.create<bsl::var::Dict>();
		for (int j=0; j<server_num; j++) {
			LcClientServer *server = dynamic_cast<LcClientServer*>(_mgr->getServerInfo(j));
			if (NULL == server) {
				return bsl::var::Null::null;
			}
            char server_name[32];
            snprintf(server_name, sizeof(server_name), "%s_%d", server->getIP(), server->getPort());
            lc_client_convert_ip(server_name);
            output[server_name] = rp.create<bsl::var::Int64>(server->getPoolMaxNum());
		}
        return output;   
	}


}


