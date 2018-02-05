
#include "lcclient_connectmanager.h"

#include <lc_log.h>
#include <connection.h>

#include "lcclient_connection.h"
#include "lcclient_strategy.h"
#include "lcclient_healthycheck.h"
#include "lcclient_define.h"
#include "lcclient_ioc.h"
#include "lcclient_connectserver.h"
#include "lcclient_manager.h"

namespace lc {
    
    LcClientConnectManager :: LcClientConnectManager()
    {
        _default_server_conf.readtimeout = 0;
        _default_server_conf.writetimeout = 0;
        _default_server_conf.reserve1 = 0;
        _default_server_conf.reserve2 = 0;
        _default_server_conf.reserve3 = 0;
        _defaultLinger = 0;
        _defaultReqBufLen = 0;
        _defaultResBufLen = 0;
        _defaultWaitNum = 0;
        _isConnAll = 0;
        _log->setLevel(UL_LOG_NOTICE);
        _reload_tag = 0;
    }
        
    LcClientConnectManager :: ~LcClientConnectManager ()
    {
    }

	
	LcClientConnection * LcClientConnectManager :: fetchConnectionex(
        const default_server_arg_t &serverArg, bsl::var::IVar & selectServerVar, LcClientManager *mgr)
	{
		ConnectionRequestex req;
        req.key = serverArg.key;
		req.nthRetry = 0;
		req.serverID = -1;
		req.err = 0;
		req.conn = NULL;
        req.serverArg = serverArg;

        ConnectionRes res;
        comcm::mstime_t reqtime = comcm::ms_time();
		
		

        LC_LOG_DEBUG("start fetch connection");
		while(req.nthRetry < _defaultRetryTime) {
			_rwlock.read(); 
			
			LcClientStrategy* lcclientstt = dynamic_cast<LcClientStrategy *>(_stt);
			int svrid = lcclientstt->fetchServer(&req, selectServerVar, &res);    
			_rwlock.unlock();
			if(svrid < 0 || svrid >= _svrnum){
				LC_LOG_WARNING("fetch server error");
				return NULL;
			} else {
				if (res.selectedServerID.push_back(svrid) != 0) {
                    LC_LOG_WARNING("reach server max size : %d", LCCLIENT_SELECTEDSERVER_SIZE);
                    return NULL;
                }
				req.serverID = svrid;
                while (1) {
                    LC_LOG_DEBUG("fetch connection [%d]", req.nthRetry);
                    req.conn = _svr[svrid]->fetchConnection(&req.err);
                    LC_LOG_DEBUG("fetch connection [%d] end", req.nthRetry);
                    if(comcm::REQ_OK == req.err){
                        lcclientstt->setServerArgAfterConn(dynamic_cast<LcClientServer*>(_svr[svrid]), 0);
                        LC_LOG_DEBUG("fetch ok");
                        return dynamic_cast<LcClientConnection *>(req.conn);
                    } else if (comcm::REQ_SVR_FULL == req.err) {
                        
                        if (mgr) {
                            mgr->add_monitor_info_ipport("CONNECT_FAIL_NUMBER", _svr[svrid]->getIP(), _svr[svrid]->getPort(), getServiceName());
                        }

                        
                        LC_LOG_DEBUG("fetch : server is full");
                        if (comcm::ms_time() > 
                            (reqtime + _defaultFetchConnTime)) {
                            LC_LOG_WARNING("fetch conn full and timeout,waittime[%d] server[%s:%d]",
                                _defaultFetchConnTime, _svr[svrid]->getIP(), _svr[svrid]->getPort());
                            
                            lcclientstt->setServerArgAfterConn(dynamic_cast<LcClientServer*>(_svr[svrid]), -1);
                            break;
                        }
                        comcm::ms_sleep(LCCLIENT_FETCHCONN_SLEEPTIME);
                        continue;
                    } else {
                        
                        if (mgr) {
                            mgr->add_monitor_info_ipport("CONNECT_FAIL_NUMBER", _svr[svrid]->getIP(), _svr[svrid]->getPort(), getServiceName());
                        }

                        
                        LC_LOG_WARNING("fetch conn failed, server[%s:%d]",
                                _svr[svrid]->getIP(), _svr[svrid]->getPort());
                        lcclientstt->setServerArgAfterConn(dynamic_cast<LcClientServer*>(_svr[svrid]), -2);
                        break;
                    }
                }
			}
			++ req.nthRetry;
		}
	    LC_LOG_WARNING("fetch error");
        return NULL;
	}

	int LcClientConnectManager :: init(const comcfg::ConfigUnit & conf) 
	{
		if(NULL == conn_creater || NULL == svr_creater){
			return -1;
		}

        conf["Name"].get_bsl_string(&_name, "empty");

		int ret = 0;       
		comcm::Server ** svrs = readConfigData(conf, &ret);
		if(ret != 0){
			LC_LOG_WARNING("init server from conf error");
			return -1;
		}

        if (conf[LCCLIENT_CONF_SERVER].selfType() == comcfg::CONFIG_ERROR_TYPE) {
            return 0;
		}
		int svrsize = (int)conf[LCCLIENT_CONF_SERVER].size();
        if (0 == svrsize) {
            return 0;
        }

		ret = init(svrs, svrsize, _conn_type==comcm::SHORT_CONN);
		if(svrs){
			for (int i = 0; i < svrsize; ++i) {
				if (svrs[i]) {
					delete svrs[i];
				}
			}
			delete [] svrs;
		}
        LC_IN_NOTICE("Service[%s] DefaultConnectType:%s",
                      conf[LCCLIENT_CONF_SERVICE_NAME].to_cstr(), _conn_type==comcm::LONG_CONN?"LONG":"SHORT");
        LC_IN_NOTICE("Service[%s] DefaultRetry:%d",
                      conf[LCCLIENT_CONF_SERVICE_NAME].to_cstr(), _defaultRetryTime);
        LC_IN_NOTICE("Service[%s] DefaultFetchConnMaxTime:%d",
                      conf[LCCLIENT_CONF_SERVICE_NAME].to_cstr(), _defaultFetchConnTime);
        LC_IN_NOTICE("Service[%s] ConnectAll:%d",
                      conf[LCCLIENT_CONF_SERVICE_NAME].to_cstr(), _isConnAll);
        LC_IN_NOTICE("Service[%s] DefaultReadTimeout:%d",
                      conf[LCCLIENT_CONF_SERVICE_NAME].to_cstr(), _default_server_conf.readtimeout);
        LC_IN_NOTICE("Service[%s] DefaultWriteTimeout:%d",
                      conf[LCCLIENT_CONF_SERVICE_NAME].to_cstr(), _default_server_conf.writetimeout);
        LC_IN_NOTICE("Service[%s] DefaultLinger:%d",
                      conf[LCCLIENT_CONF_SERVICE_NAME].to_cstr(), _defaultLinger);
        LC_IN_NOTICE("Service[%s] ReqBuf:%d",
                      conf[LCCLIENT_CONF_SERVICE_NAME].to_cstr(), _defaultReqBufLen);
        LC_IN_NOTICE("Service[%s] ResBuf:%d",
                      conf[LCCLIENT_CONF_SERVICE_NAME].to_cstr(), _defaultResBufLen);
		return ret;
	}
	
    comcm::Server ** LcClientConnectManager :: readConfigData(const comcfg::ConfigUnit & conf, 
															int	*err)
	{
        LC_LOG_DEBUG("slc class : [%s]", __func__);

		if (NULL == err) {
			return NULL;
		}
		*err = 0;

		char ctype[12];
		conf[LCCLIENT_CONF_SERVICE_RETRY].get_int32(&_defaultRetryTime, 
                                                LCCLIENT_DEFAULT_RETRY_TIME);
		if(conf[LCCLIENT_CONF_SERVICE_CONNECTTYPE].get_cstr(ctype, sizeof(ctype)) != 0){
			_conn_type = comcm::SHORT_CONN;
		}
		else{
			if(strcmp(ctype, LCCLIENT_CONNTYPE_LONG) == 0){
				_conn_type = comcm::LONG_CONN;
			}
			else if(strcmp(ctype, LCCLIENT_CONNTYPE_SHORT) == 0){
				_conn_type = comcm::SHORT_CONN;
			}
			else{
                LC_LOG_WARNING("conn type set error[%s] using short[SHORT]", ctype);
				_conn_type = comcm::SHORT_CONN;
			}
		}

		conf[LCCLIENT_CONF_SERVICE_MAXCONNECT].get_int32(&_defaultCapacityForServer, 
                                                    LCCLIENT_DEFAULT_CAPACITY);

        LC_LOG_DEBUG("defaultCapacity : [%d]", _defaultCapacityForServer);
		conf[LCCLIENT_CONF_SERVICE_CONNECT].get_int32(&_defaultCtimeoutForServer, 
                                                    LCCLIENT_DEFAULT_CONNTO);
        int ret;
        if ((ret=conf[LCCLIENT_CONF_SERVICE_PORT].get_int32(&_defaultPortForServer, 
                                                        LCCLIENT_DEFAULT_PORT)) != 0) {
            LC_LOG_DEBUG("DefaultPort not exist, errno : [%d]", ret);
        }
        LC_LOG_DEBUG("default port : %d", _defaultPortForServer);

        conf[LCCLIENT_CONF_SERVICE_READ].get_int32(&_default_server_conf.readtimeout, 
                                                    LCCLIENT_DEFAULT_READTO);
		conf[LCCLIENT_CONF_SERVICE_WRITE].get_int32(&_default_server_conf.writetimeout, 
                                                    LCCLIENT_DEFAULT_WRITETO);
		conf[LCCLIENT_CONF_SERVICE_LINGER].get_int32(&_defaultLinger, LCCLIENT_DEFALUT_LINGER);
		conf[LCCLIENT_CONF_SERVICE_SENDBUF].get_int32(&_defaultReqBufLen, 
                                                    LCCLIENT_DEFAULT_REQBUFLEN);
		conf[LCCLIENT_CONF_SERVICE_RECVBUF].get_int32(&_defaultResBufLen, 
                                                    LCCLIENT_DEFALUT_REABUFLEN);
        conf[LCCLIENT_CONF_SERVICE_WAITNUM].get_int32(&_defaultWaitNum, LCCLIENT_SERVER_WAITNUM);   

        conf[LCCLIENT_CONF_SERVICE_FETCHCONNTIME].get_int32(&_defaultFetchConnTime, 
                                                            LCCLIENT_FETCHCONN_TIME);
        if (_defaultFetchConnTime < 0) {
            LC_LOG_WARNING("_defaultFetchConnTime[%d] < 0", _defaultFetchConnTime);
            *err = -1;
            return NULL;
        }
        
        conf[LCCLIENT_CONF_SERVICE_CONNALL].get_int32(&_isConnAll, 0); 

       if (conf[LCCLIENT_CONF_SERVER].selfType() == comcfg::CONFIG_ERROR_TYPE) {
			LC_LOG_WARNING("[%s] not exist in conf", LCCLIENT_CONF_SERVER);
            return 0;
			
		}
		int svrsize = (int)conf[LCCLIENT_CONF_SERVER].size();
        
        if (0 == svrsize) {
            LC_LOG_WARNING("server not exist in conf");
            return 0;
            
        }

		comcm::Server ** svr;
        svr = new (std::nothrow) comcm::Server*[svrsize];
		
		
		if(NULL == svr){
			*err = -1;
			return NULL;
		}

		for(int i = 0; i < svrsize; ++i){
			svr[i] = svr_creater();
			if (svr[i] != NULL) {
				svr[i]->setManager(this);
				if( svr[i]->init(conf[LCCLIENT_CONF_SERVER][i]) != 0 ){
					*err = -1;
					LC_LOG_WARNING("init server[%d] error", i);
				}
                LcClientStrategy *stra = dynamic_cast<LcClientStrategy *>(_stt);
                if (NULL == stra) {
                    
                } else {
                    LcClientServer *server = dynamic_cast<LcClientServer *>(svr[i]);
                    if (stra->isServerRangeOK(server->getRange()) != 0) {
                        *err = -1;
                        LC_LOG_WARNING("init server[%d] error : Range[%s] error", i, server->getRange());
                    }
                }
			} else {
				*err = -1;
			}
		}
		if(*err){
			
			if (svr) {
				for (int i = 0; i < svrsize; ++i) {
					if (svr[i]) {
						delete svr[i];
					}
				}
				delete [] svr;
			}
			svr = NULL;
		}
		return svr; 
	}

	int LcClientConnectManager :: reload(const comcfg::ConfigUnit & conf)
	{
		int ret = 0;
		if (conf[LCCLIENT_CONF_SERVER].selfType() == comcfg::CONFIG_ERROR_TYPE) {
			LC_LOG_WARNING("[%s] not exist in conf", LCCLIENT_CONF_SERVER);
		    reload(NULL, 0);
            return 0;
        }
		int svrsize = (int)conf[LCCLIENT_CONF_SERVER].size();
        LC_LOG_DEBUG("svrsize : [%d]", svrsize);
        if (0 != svrsize) {
		    comcm::Server ** svrs = readConfigData(conf, &ret);
		    if(ret){
			    LC_LOG_WARNING("init server from conf error");
			    return -1;
		    }
		    ret = reload(svrs, svrsize);
		    if (svrs) {
			    for (int i = 0; i < svrsize; ++i) {
				    if (svrs[i]) {
					    delete svrs[i];
				    }
			    }
			    delete [] svrs;
		    }
		    return ret;
        } else {
            ret = reload(NULL, 0);
            return ret;
        }
	}

	int LcClientConnectManager :: getReqAndResBuf(int &reqBufLen, int &resBufLen)
	{
		reqBufLen = _defaultReqBufLen;
		resBufLen = _defaultResBufLen;

		return 0;
	}

    int LcClientConnectManager :: getReqBufLen()
    {
        return _defaultReqBufLen;
    }

    int LcClientConnectManager :: getResBufLen()
    {
        return _defaultResBufLen;
    }

	int LcClientConnectManager :: reload(comcm::Server ** svrs, int svrn)
	{
		if(NULL == svrs && svrn == 0) {
            _rwlock.write();
            for(int i = 0; i < _svrnum; ++i){
                _svr[i]->disable();
            }
            _rwlock.unlock();
            return 0;
		}
        if (NULL == svrs && svrn > 0) {
            return -1;
        }
		if (_svrnum >= comcm::MAX_SVR_NUM) {
			LC_LOG_WARNING("reach max server num[%d]", comcm::MAX_SVR_NUM);
			return -1;
		}
		int eq[_svrnum];
		int isnew[svrn];
		int cnt = svrn;
		memset(eq, -1, sizeof(eq));
		memset(isnew, -1, sizeof(isnew));
		for(int i = 0; i < svrn; ++i) {
			for(int j = 0; j < _svrnum; ++j) {
				
				if(_svr[j]->equal(*svrs[i])) {
                    LC_LOG_DEBUG("equal");
					eq[j] = i;
					isnew[i] = 0;
					--cnt;
					break;
				} else {
                    LC_LOG_DEBUG("not equal");
                }
			}
		}
		if(_svrnum + cnt >= comcm::MAX_SVR_NUM) {
			LC_LOG_WARNING("reach max server num[%d]", comcm::MAX_SVR_NUM);
			
			return -1;
		}
		
		
		int ret = 0;
		int _new_svrnum = _svrnum;
		for(int i = 0; i < svrn; ++i){
			if(isnew[i]){
				int curID = _new_svrnum;
				_svr[curID] = svr_creater();
				if (NULL == _svr[curID]) {
					ret = -1;
					break;
				}
				_svr[curID]->setID(_new_svrnum);
				_svr[curID]->setManager(this);
				++ _new_svrnum;
				if( _svr[curID]->clone(svrs[i]) != 0 ){
					ret = -1;
					break;
				}
                if (isConnAll()) {
                    LcClientServer *server = dynamic_cast<LcClientServer *>(_svr[curID]);
                    if (NULL != server) {
                        server->connectAll();
                    }
                }
			}
		}
		if(ret){
			for(int i = _svrnum; i < _new_svrnum; ++i){
				if (_svr[i]) {
					delete _svr[i];
				}
				_svr[i] = NULL;
			}
			return -1;
		}

        _rwlock.write();
		
		for(int i = 0; i < _svrnum; ++i){
			
			if(eq[i] != -1){
				if( _svr[i]->clone(svrs[eq[i]]) != 0 ){
					ret = -1;
					break;
				}
			}
		}
		if(ret){
			return -1;
		}
		
		
        
		for(int i = 0; i < _svrnum; ++i){
			if(-1 == eq[i]){
				_svr[i]->disable();
			}
			else{
				_svr[i]->enable();
			}
		}
		for(int i = _svrnum; i < _new_svrnum; ++i){
			_svr[i]->enable();
		}
		_svrnum = _new_svrnum;
		
		reloadTag(_svr, _svrnum);		
        ++ _reload_tag;
		_rwlock.unlock();
		_lastReloadTime = time(0);
		return 0;
	}

	

	int LcClientConnectManager :: setConnType(int connType)
	{
		
		
		_rwlock.read();
		if (connType == _conn_type) {
			_rwlock.unlock();
			return 0;
		}
		int i = 0;
		for (i=0; i<_svrnum; i++) {
			_svr[i]->closeExtraConnection(_svr[i]->liveConnectionNum());
		}
		_rwlock.unlock();

		comcm::AutoWrite autolock(&_rwlock);
		_conn_type = connType;

		return 0;
	}

	int LcClientConnectManager :: init(comcm::Server ** svrs, int svrnum, bool shortConnection)
	{
		if(NULL == conn_creater || NULL == svr_creater){
			return -1;
		}
		if(NULL == svrs || 0 == svrnum){
			return -1;
		}
		for(int i = 0; i < svrnum; ++i){
			if(NULL == _svr[i]){
			    _svr[i] = svr_creater();
			    if (NULL == _svr[i]) {
				    LC_LOG_WARNING("create server using set callback error");
				    return -1;
			    }
		    }
			_svr[i]->clone(svrs[i]);
			_svr[i]->setManager(this);
			_svr[i]->setID(i);
		}
		_svrnum = svrnum;
		if(shortConnection){
			_conn_type = comcm::SHORT_CONN;
		}
		else{
			_conn_type = comcm::LONG_CONN;
		}
		
		initTag(_svr, svrnum);

		_lastReloadTime = time(0);
		return 0; 
	}    

    LcClientStrategy *LcClientConnectManager :: getStrategy()
    {
        if (NULL == _stt) {
            return NULL;
        }

        return dynamic_cast<LcClientStrategy*>(_stt);
    }

    int LcClientConnectManager :: fetchConnectionAsync(lcclient_talkwith_t *talkmsg, 
            bsl::var::IVar &selectServerVar, lc::IEvent *event, lc::NetReactor *reactor)
    {
        talkmsg->event = event;
        talkmsg->reactor = reactor;
        ConnectionRequestex req;
        talkmsg->conf = &selectServerVar;
        req.key = talkmsg->defaultserverarg.key;
		req.nthRetry = talkmsg->retry;
		req.serverID = -1;
		req.err = 0;
		req.conn = NULL;
        req.serverArg = talkmsg->defaultserverarg;

        ConnectionRes res;

        
		_rwlock.read(); 
		
		LcClientStrategy* lcclientstt = dynamic_cast<LcClientStrategy *>(_stt);
        if (NULL == lcclientstt) {
            _rwlock.unlock();
            return -1;
        }
		int svrid = lcclientstt->fetchServer(&req, selectServerVar, &res); 
		_rwlock.unlock();
		if(svrid < 0 || svrid >= _svrnum){
			LC_LOG_WARNING("get server id error");
			return -1;
		} else {
            LcClientServer *server = dynamic_cast<LcClientServer*>(_svr[svrid]);
            int ret = server->fetchConnectionAsync(talkmsg, event, reactor);
		    return ret;
        }

	    LC_LOG_WARNING("fetch connect async error");
        return -1;
    }

    int LcClientConnectManager :: connectAll()
    {
        comcm::AutoRead auto_read(&_rwlock);

        LcClientServer *server = NULL;
        for (int i=0; i<_svrnum; i++) {
            server = dynamic_cast<LcClientServer *>(_svr[i]);
            if (NULL != server) {
                server->connectAll();
            }
        }

        return 0;
    }

    int LcClientConnectManager :: fetchConnectionRetry(
                                                lcclient_talkwith_t *talkmsg,
                                                bsl::var::IVar & selectServerVar,
                                                lc::IEvent *event,
                                                lc::NetReactor *reactor
                                                    )
    {
        int ret = 0;
        while (talkmsg->retry < _defaultRetryTime) {
            talkmsg->retry ++;
            if (talkmsg->retry > 1) {
                LC_LOG_WARNING("Connect Server Retry[%d:%d]", talkmsg->retry, _defaultRetryTime);
            }
            ret = fetchConnectionAsync(talkmsg, selectServerVar, event, reactor);
            if (ret != 0) {
                continue;
            } else {
                
                return 0;
            }
        }
        
        
        return -1;
    }


    int LcClientConnectManager :: getRealServerSize()
    {
        comcm::AutoRead auto_lock(&_rwlock);
        int real_num = 0;
        for (int i=0; i<_svrnum; i++) {
            if (_svr[i]->isEnable()) {
                ++ real_num;
            }
        }
        return real_num;
    }
}










