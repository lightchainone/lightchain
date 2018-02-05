
#include "lcclient_connectserver.h"

#include <lc_log.h>

#include "lcclient_connection.h"
#include "lcclient_utils.h"
#include "lcclient_struct.h"
#include "lcclient_connectmanager.h"
#include "lcclient_define.h"
#include "lcclient_strategy.h"
#include "lcclient_nshead.h"
#include "lcclient_http.h"
#include "../lc_aserver/LcSockEvent.h"
#include "lcclient_manager.h"

namespace lc
{
	class LcClientConnectManager;

	LcClientServer :: LcClientServer()
	{
		_conf_info.readtimeout = 0;
		_conf_info.writetimeout = 0;
		_conf_info.reserve1 = -1;
		_conf_info.reserve2 = -1;
		_conf_info.reserve3 = -1;
		_conf_info.reserve4 = -1;
		_master = 0;
		_confSave = NULL;
		_oldConfSaveLen = 0;
		_confSaveLen = 0;
		_retryTime = 0;
		_linger = 0;
		_maxWaitingNum = 0;
	}

	LcClientServer :: ~LcClientServer()
	{
		if (NULL != _confSave) {
			delete [] _confSave;
		}

		clearIpVec();
	}

	int LcClientServer :: init(const comcfg::ConfigUnit & conf)
	{
		if (NULL == _mgr) {
			return -1;
		}
		
		LcClientConnectManager* currMgr = dynamic_cast<LcClientConnectManager*>(_mgr);
		if (NULL == currMgr) {
			LC_LOG_WARNING("LcClientConnectManager is NULL");
			return -1;
		}

		
		
		conf[LCCLIENT_CONF_SERVER_READ].get_int32(&_conf_info.readtimeout,
				currMgr->_default_server_conf.readtimeout);
		conf[LCCLIENT_CONF_SERVER_WRITE].get_int32(&_conf_info.writetimeout,
				currMgr->_default_server_conf.writetimeout);

        
		if (conf[LCCLIENT_CONF_SERVER_PORT].get_int32(&_port, currMgr->_defaultPortForServer)!=0 &&
                conf["port"].get_int32(&_port, currMgr->_defaultPortForServer)!=0) {
			_port = currMgr->_defaultPortForServer;
		}

		LC_LOG_DEBUG("server port : %d", _port);
		if (-1 == _port) {
			LC_LOG_WARNING("not set port both for Service and Server");
			return -1;
		}
		conf[LCCLIENT_CONF_SERVER_RANGE].get_bsl_string(&_range, "");
		conf[LCCLIENT_CONF_SERVER_VERSION].get_bsl_string(&_version, "");
		conf[LCCLIENT_CONF_SERVER_MASTER].get_int32(&_master, LCCLIENT_SERVER_DEFAULT_MASTER);
		LC_LOG_DEBUG("[%s] [%s] [%d]", _range.c_str(), _version.c_str(), _master);

		conf[LCCLIENT_CONF_SERVER_MAXCONNECT].get_int32(&_capacity,
														currMgr->_defaultCapacityForServer);
		LC_LOG_DEBUG("server max conn : [%d]", _capacity);
		conf[LCCLIENT_CONF_SERVER_CONNECT].get_int32(&_ctimeout,
													currMgr->_defaultCtimeoutForServer);
		conf[LCCLIENT_CONF_SERVER_LINGER].get_int32(&_linger, currMgr->_defaultLinger);
		conf[LCCLIENT_CONF_SERVER_WAITNUM].get_int32(&_maxWaitingNum, currMgr->_defaultWaitNum);

		LC_LOG_DEBUG("waiting num : [%d]", _maxWaitingNum);
		if (_conf_info.writetimeout < 0 || _conf_info.readtimeout < 0 
			|| _port < 0 || _capacity < 0 || _ctimeout < 0 || _maxWaitingNum < 0 ) {
			
			LC_LOG_WARNING("conf error writetimeout[%d], readtimeout[%d], port[%d], \
						   capacity[%d], connecttimeout[%d], _maxWaitingNum[%d]",
						   _conf_info.writetimeout, _conf_info.readtimeout, _port, 
						   _capacity, _ctimeout, _maxWaitingNum);
			return -1;
		}
		if (_capacity > (int)_conns.size()) {
			_conns.resize(_capacity, NULL);
		}
		if (conf[LCCLIENT_CONF_SERVER_TAG].selfType() != comcfg::CONFIG_ARRAY_TYPE 
				&& conf[LCCLIENT_CONF_SERVER_TAG].selfType() != comcfg::CONFIG_ERROR_TYPE) {
		    LC_LOG_WARNING("[%s:%d]Server init failed : tag config should be array .",
                                        __FILE__, __LINE__);
        } else {
			int err = 0;
			for (int i = 0; i < (int)conf[LCCLIENT_CONF_SERVER_TAG].size(); ++i ) {
				const char * tg = conf[LCCLIENT_CONF_SERVER_TAG][i].to_cstr(&err);
				if(err == 0) {
					_tag.push_back(bsl::string(tg));
				}
			}
		}

		bsl::ResourcePool rp;
		int errNo;
		bsl::var::IVar &tempVar = conf.to_IVar(&rp, &errNo);
		if (0 != errNo) {
			LC_LOG_WARNING("convert conf to IVar error[%d]", errNo);
			return -1;
		}

		comcfg::Configure tempConf;
		int ret1 = tempConf.loadIVar(tempVar);
		if (0 != ret1) {
			LC_LOG_WARNING("var to conf error[%d]", ret1);
			return -1;
		}
		size_t strSaveLen = 0;
		char *strSave = tempConf.dump(&strSaveLen);
		if (NULL == strSave) {
			LC_LOG_WARNING("dump error");
			return -1;
		}

		
		if (copySaveBuffer(strSave, (int)strSaveLen +1 ) != 0) {
			return -1;
		}

		
		bsl::string bslIpList;
		if (conf[LCCLIENT_CONF_SERVER_IP].get_bsl_string(&bslIpList) != 0){
			LC_LOG_WARNING("[%s] not exist in Conf", LCCLIENT_CONF_SERVER_IP);
			return -1;
		}

		const char *ipList = bslIpList.c_str();
		int ipListLen = bslIpList.size();
		int i = 0;
		char tempIP[LCCLIENT_IP_LEN];
		memset(tempIP, 0, sizeof(tempIP));
		int tempIPPos = 0;
		while (i < ipListLen) {
			if (' ' == ipList[i]) {
				if (0 == tempIPPos) {
					i ++;
					continue;
				}
				
				if (tempIPPos > (int)(sizeof(tempIP)-1)) {
					LC_LOG_WARNING("ip error [%s]", ipList);
					clearIpVec();
					return -1;
				}
				tempIP[tempIPPos] = '\0';
				if (comcm::ip_to_int(tempIP) == 0) {
					LC_LOG_WARNING("ip[%s] is valid", tempIP);
					clearIpVec();
					return -1;
				}
				LC_LOG_DEBUG("currIP : %s", tempIP);
				char *newIP = new (std::nothrow)char[strlen(tempIP)+1];
				if (NULL == newIP) {
					LC_LOG_WARNING("new buffer for IP error [%m]");
					clearIpVec();
					return -1;
				}
				snprintf(newIP, strlen(tempIP)+1, "%s", tempIP);
				_ip_vec.push_back(newIP);
				memset(tempIP, 0, sizeof(tempIP));
				tempIPPos = 0;
				i ++;
			} else {
				tempIP[tempIPPos] = ipList[i];
				tempIPPos ++;
				i ++;
			}
		}
		if (i > 0) {
			if (ipList[i-1] != ' ') {
				if (tempIPPos <= (int)(sizeof(tempIP)-1)) {
					tempIP[tempIPPos] = '\0';
					if (comcm::ip_to_int(tempIP) == 0) {
						LC_LOG_WARNING("ip[%s] is valid", tempIP);
						clearIpVec();
						return -1;
					}
					LC_LOG_DEBUG("last ip : [%s]", tempIP);
					char *newIP = new (std::nothrow)char[strlen(tempIP)+1];
					if (NULL == newIP) {
						LC_LOG_WARNING("new buffer for IP error [%m]");
						clearIpVec();
						return -1;
					}
					snprintf(newIP, strlen(tempIP)+1, "%s", tempIP);
					_ip_vec.push_back(newIP);
				} else {
					LC_LOG_WARNING("the last ip error [%s]", ipList);
					clearIpVec();
					return -1;
				}
			}
		} else {
			LC_LOG_WARNING("ipList in conf error");
			clearIpVec();
			return -1;
		}
		if (_ip_vec.size() == 0) {
			LC_LOG_WARNING("ipList is empty");
			return -1;
		}
		snprintf(_ip, sizeof(_ip), "%s", _ip_vec[0]);
		if ((_ip_int = comcm::ip_to_int(_ip)) == 0) {
			LC_LOG_WARNING("convert ip[%s] to int error", _ip);
			clearIpVec();
			return -1;
		} 
		
		return 0;
	}

	bool LcClientServer :: equal(const comcm::Server & other)
	{
		
		comcm::AutoRead auto_lock(&_svrlock);

		const LcClientServer *tempOther = dynamic_cast<const LcClientServer *>(&other);    
		if (NULL == tempOther) {
			return false;
		}
		int cloneIpNum = tempOther->_ip_vec.size();
		int currIpNum = _ip_vec.size();
		if (cloneIpNum != currIpNum) {
			return false;
		}
		int i;
		int j;
		int equalCount = 0;
		for (i=0; i<currIpNum; i++) {
			for (j=0; j<cloneIpNum; j++) {
				if (strcmp(_ip_vec[i], tempOther->_ip_vec[j]) == 0) {
					equalCount ++;
					break; 
				}
			}
			if (equalCount <= i) {
				return false;
			}
		}

		if (_port != tempOther->_port) {
			return false;
		}

		return true;
	}

	int LcClientServer :: clone(const comcm::Server * oth)
	{
		LC_LOG_DEBUG("%s", __func__);

		if (NULL == oth) {
			return -1;
		}
		
		comcm::AutoWrite auto_lock(&_svrlock);
		const LcClientServer *tempOth = dynamic_cast<const LcClientServer *>(oth);
		if (NULL == tempOth) {
			return -1;
		}
		snprintf(_ip, sizeof(_ip), "%s", tempOth->_ip);
		_ip_int = tempOth->_ip_int;
		_port = tempOth->_port;
		if (_capacity < tempOth->_capacity) {
			_capacity = tempOth->_capacity;
			if(_capacity > (int)_conns.size()){
				_conns.resize(_capacity, NULL);
			}
		}
		_ctimeout = tempOth->_ctimeout;
		_tag.clear();
		_tag = tempOth->_tag;

		_conf_info.readtimeout = tempOth->_conf_info.readtimeout;
		_conf_info.writetimeout = tempOth->_conf_info.writetimeout;
		_conf_info.reserve1 = tempOth->_conf_info.reserve1;
		
		
		if (_ip_vec.size() == 0) {
			char *newIP = NULL;
			for (int i=0; i<(int)tempOth->_ip_vec.size(); i++) {
				newIP = new (std::nothrow)char[strlen(tempOth->_ip_vec[i]) + 1];
				if (NULL == newIP) {
					LC_LOG_WARNING("new buffer for server in clone error [%m]");
					clearIpVec();
					return -1;
				}
				snprintf(newIP, strlen(tempOth->_ip_vec[i])+1, "%s", tempOth->_ip_vec[i]);
				_ip_vec.push_back(newIP);
			}
		}

		if (0 < tempOth->_confSaveLen) {
			if (copySaveBuffer(tempOth->_confSave, tempOth->_confSaveLen) != 0) {
				return -1;
			}
		} else {
			_confSaveLen = 0;
		}
		_range = tempOth->_range;
		_version = tempOth->_version;
		_master = tempOth->_master;
		_retryTime = tempOth->_retryTime;
		_linger = tempOth->_linger;
		_maxWaitingNum = tempOth->_maxWaitingNum;		

		return 0; 
	}

	LcClientConnectManager * LcClientServer :: getManager()
	{
		comcm::AutoRead auto_lock(&_svrlock);

		if (NULL == _mgr) {
			return NULL;
		}
		return dynamic_cast<LcClientConnectManager*>(_mgr);
	}

	void LcClientServer :: lc_server_callback(lc::IEvent *e, void *param)
	{
		
		
		if (NULL == e || NULL == param) {
			return;
		}
		lc::nshead_atalk_t *atalk = (lc::nshead_atalk_t *)param;
		lcclient_nshead_atalk_t *currTalk = (lcclient_nshead_atalk_t *)(atalk->param);

		
		if (NULL == currTalk->conn) {
			currTalk->talk->success = lc::NSHEAD_UNKNOW_ERROR;
			currTalk->talk->talkwithcallback(currTalk->talk);
			return;
		}

		if (0 != currTalk->slctalk.err) {
			LC_LOG_DEBUG("talk error[%d] log_id[%u]", currTalk->slctalk.err,
						 currTalk->talk->reqhead.log_id);
			currTalk->talk->success = currTalk->slctalk.err;
			if (currTalk->talk->success != NSHEAD_READ_BODY_BUF_NO_ENOUGH) {
				if (NULL != currTalk->conn) {
					LC_LOG_DEBUG("conn error log_id[%u]", currTalk->talk->reqhead.log_id);
					currTalk->conn->setErrno(currTalk->talk->success);
				} 
			}   
		} else {
			LC_LOG_DEBUG("talk success log_id[%u]", currTalk->talk->reqhead.log_id);
			currTalk->talk->success = 0;
			if (NULL != atalk->alloccallback) {
                currTalk->talk->resbuf = atalk->resbuf;
            } 
		}

		if (NULL == currTalk->conn) {
			LC_LOG_WARNING("conn is NULL");
		} else {
			int handleError = 0;
			socklen_t len = 0;
			len = sizeof(handleError);
			int *curr_handle = (int *)currTalk->conn->getHandle();
			int curr_fd = *curr_handle;
			int val = getsockopt(curr_fd, SOL_SOCKET, SO_ERROR, &handleError, &len);
			if (-1 == val) {
				LC_LOG_DEBUG("nshead_talk : get sockFD[%d] error[%d] log_id[%u]", handleError,
							handleError, currTalk->talk->reqhead.log_id);
				currTalk->conn->setErrno(lc::LC_CONN_FD_ERROR);
			}

			
            if (currTalk->talk->rpccallback1 != NULL && currTalk->conn->getErrno() == 0) {
                int ret = currTalk->talk->rpccallback1((void *)currTalk->talk);
                if (ret == 1) {
                    
                    currTalk->conn->setErrno(lc::LC_RPC_CLOSE_CONN);
                }
            }
		}

		server_talk_returninfo_t currReturnInfo;
		currReturnInfo.realreadtime = currTalk->slctalk.realReadTime;
		currReturnInfo.realwritetime = currTalk->slctalk.realWriteTime;

		currTalk->talk->returninfo = currReturnInfo;
		currTalk->talk->resbuf = currTalk->slctalk.resbuf;

		LcClientConnectManager *connMgr = NULL;

		LcClientServer *server = currTalk->server;
		if (NULL == server) {
			LC_LOG_WARNING("something wrong of talk for Server or Manager log_id[%u]",
							currTalk->talk->reqhead.log_id);
			goto End;
		}
		if (NULL != currTalk->conn) {
			server->freeConnectionAsync(currTalk->conn);
			currTalk->conn = NULL;
		}

		connMgr = server->getManager();
		if (NULL != connMgr) {
			LcClientStrategy *currStt = connMgr->getStrategy();
			if (NULL != currStt) {
				currStt->setServerArg(server, currTalk->talk);
			}	   
			currTalk->talk->talkwithcallback(currTalk->talk);
		}
		return;

	End:
		
		currTalk->talk->success = lc::NSHEAD_UNKNOW_ERROR;
		currTalk->talk->talkwithcallback(currTalk->talk);

		return ;

	}

	void LcClientServer :: lc_server_connect_callback(lc::IEvent *e, void *param)
	{
		LC_LOG_DEBUG("%s", __func__);

		if (NULL == e || NULL == param) {
			LC_LOG_WARNING("input error");
			return;
		}

		lc::SockEvent *sockEvent = dynamic_cast<lc::SockEvent *>(e);
		if (NULL == sockEvent) {
			return;
		}

		lcclient_nshead_atalk_t *currTalk = (lcclient_nshead_atalk_t *)param;
		LcClientServer *server = currTalk->server;
		if (NULL == server) {
			LC_LOG_WARNING("server is NULL log_id[%u]", currTalk->talk->reqhead.log_id);
			currTalk->talk->success = lc::NSHEAD_UNKNOW_ERROR;
			currTalk->talk->talkwithcallback(currTalk->talk);
			return;
		}

		if (sockEvent->isError()) {
			LC_LOG_WARNING("connect error happened ip[%d] err[%d] log_id[%u] %m", currTalk->ipnum, 
							sockEvent->result(), currTalk->talk->reqhead.log_id);
			postConn(currTalk, server, sockEvent);
			return;
		}
		int currFD = sockEvent->handle();
		if (currFD >= 0) {
			
			LcClientManager *clt_mgr = currTalk->mgr;
			if (clt_mgr) {
				clt_mgr->add_monitor_info_ipport("QUERY_CONNECT_NUMBER", currFD, currTalk->server->getManager()->getServiceName());
			}
            

            int on = 1;
            if (setsockopt(currFD, IPPROTO_TCP,TCP_NODELAY, &on, sizeof(on)) < 0) {
                LC_LOG_WARNING("setsockopt nodelay error log_id[%u] [%m]", 
								currTalk->talk->reqhead.log_id);
				postConn(currTalk, server, sockEvent);
                return;
            }
            if (server->isLinger()) {
                struct linger li;
                memset(&li, 0, sizeof(li));
                li.l_onoff = 1;
                li.l_linger = 0;
                if (setsockopt(currFD, SOL_SOCKET, SO_LINGER, (const char*)&li, sizeof(li)) < 0 ) {
                    LC_LOG_WARNING("setsockopt so_linger error log_id[%u] [%m]", 
									currTalk->talk->reqhead.log_id);
					postConn(currTalk, server, sockEvent);
                    return;
                }
            }
        } else {
			LC_LOG_WARNING("get fd error log_id[%u]", currTalk->talk->reqhead.log_id);
			postConn(currTalk, server, sockEvent);
			return;
		}
			
		LcClientStrategy *currStt = server->getManager()->getStrategy();
		currStt->setServerArgAfterConn(server, 0);

		currTalk->conn->setHandle(currFD);
		currTalk->slctalk.cb = lc_server_callback;
		sockEvent->setHandle(currFD);
		sockEvent->setTimeout(server->getWriteTimeout());
		lc::nshead_atalkwith1(sockEvent, &currTalk->slctalk);

		if (currTalk->talk->reactor->post(sockEvent) != 0) {
			LC_LOG_WARNING("post event error log_id[%u]", currTalk->talk->reqhead.log_id);
			sockEvent->setHandle(-1);
			currTalk->conn->setErrno(-1);
			server->freeConnectionAsync(currTalk->conn);
			currTalk->talk->success = LC_NETREACTOR_POST_ERROR;
			
			LcClientConnectManager *mgr = server->getManager();
			int fret = mgr->fetchConnectionRetry(currTalk->talk, *currTalk->talk->conf, sockEvent,
									dynamic_cast<NetReactor*>(sockEvent->reactor()));
			if (0 != fret) {
                currTalk->talk->talkwithcallback(currTalk->talk);
            }
			return; 
		}

		return;
	}

	int LcClientServer :: freeConnectionAsync(LcClientConnection *conn, int tag)
	{
		LC_LOG_DEBUG("%s", __func__);

		if (NULL == conn) {
			return -1;
		}
		bool needReturn = false;
		if(conn->getErrno() != 0){
			if (!isHealthy() || !isEnable()) {
				
				
				LC_LOG_DEBUG("server error");
				_svrlock.write();
				while (!_waitingTalkList.empty()) {
					LC_LOG_DEBUG("post -1");
					lcclient_talkwith_t *baseTalk = _waitingTalkList.front();
					_waitingTalkList.pop_front();
					if (baseTalk->type == LCCLIENT_NSHEAD) {
						nshead_talkwith_t *talkmsg = dynamic_cast<nshead_talkwith_t*>(baseTalk);
						lcclient_nshead_atalk_t *currTalk = &talkmsg->innerTalk;
						currTalk->conn = NULL;
						currTalk->talk = talkmsg;
						currTalk->server = NULL;
						set_lcnshead_atalk(currTalk, talkmsg, lc_server_callback, currTalk);

						if (talkmsg->reactor->post(talkmsg->event, lc::IReactor::CPU) != 0) {
							LC_LOG_WARNING("post talkwith error");
							talkmsg->success = lc::LC_NETREACTOR_POST_ERROR;
							_svrlock.unlock();
							talkmsg->talkwithcallback(talkmsg);
							_svrlock.write();
						}
						talkmsg->event->release();
					} else if (baseTalk->type == LCCLIENT_COMMON) {
						common_talkwith_t *talkmsg = dynamic_cast<common_talkwith_t*>(baseTalk);
						talkmsg->conn = NULL;
						talkmsg->server = NULL;
                        _svrlock.unlock();
                        talkmsg->talkwithcallback(talkmsg);
						talkmsg->event->release();
                        _svrlock.write();
                        
					} else {
						
						LC_LOG_WARNING("talkwith type[%d] error", baseTalk->type);
						baseTalk->success = lc::LC_ERROR_TYPE;
					}
				}
				_svrlock.unlock();
			}
		}

		if(_mgr->getConnectType() == comcm::LONG_CONN){
			if (conn->getErrno() != 0) {
				
				_svrlock.write();
				_conns[conn->_id_in_svr] = NULL;
				-- _liveConn;
				needReturn = true;
				_svrlock.unlock();
			} else{
				
                int error = 0;
                if (tag == 0) {
				if (conn->ping() != 0) {
					
					_svrlock.write();
					_conns[conn->_id_in_svr] = NULL;
					-- _liveConn;
					needReturn = true;
					_svrlock.unlock();
                    error = 1;
				}
                }
                if (error == 0){
					_svrlock.write();
					lcclient_talkwith_t *baseTalk = NULL;
					if (!_waitingTalkList.empty()) {
						
						baseTalk = _waitingTalkList.front();
						_waitingTalkList.pop_front();
					}
					_svrlock.unlock();
					if (NULL != baseTalk) {
						if (baseTalk->type == LCCLIENT_NSHEAD) {
							nshead_talkwith_t *talkmsg = dynamic_cast<nshead_talkwith_t*>(baseTalk);
							lcclient_nshead_atalk_t *currTalk = &talkmsg->innerTalk;
							currTalk->conn = conn;
							currTalk->talk = talkmsg;
							currTalk->server = this;
							currTalk->readtimeout = getReadTimeout();
							currTalk->writetimeout = getWriteTimeout();
							set_lcnshead_atalk(currTalk, talkmsg, lc_server_callback, currTalk);

							lc::SockEvent *sockEvent = dynamic_cast<lc::SockEvent*>(currTalk->talk->event);
							int *strFD = (int*)(conn->getHandle());
							int currFD = *strFD;

							sockEvent->setHandle(currFD);
							sockEvent->setTimeout(currTalk->writetimeout);
							lc::nshead_atalkwith1(sockEvent, &currTalk->slctalk);

							if (currTalk->talk->reactor->post(sockEvent) != 0) {
								LC_LOG_WARNING("post event error");
								_svrlock.write();
								_conns[conn->_id_in_svr] = NULL;
								-- _liveConn;
								_svrlock.unlock();
                                notify_strategy(conn);
								_mgr->returnConnection(conn);
								sockEvent->setHandle(-1);
								talkmsg->talkwithcallback(talkmsg);
								return 0;
							}
							talkmsg->event->release();
						} else if (baseTalk->type == LCCLIENT_COMMON) {
							common_talkwith_t *talkmsg = dynamic_cast<common_talkwith_t*>(baseTalk);
							talkmsg->conn = conn;
							talkmsg->server = this;
							talkmsg->readtimeout = getReadTimeout();
							talkmsg->writetimeout = getWriteTimeout();

							LcEvent *lcevent = dynamic_cast<LcEvent*>(talkmsg->event);
							int *strFD = (int*)(conn->getHandle());
							int currFD = *strFD;
							lcevent->setHandle(currFD);
                            
                            if (talkmsg->wait == DUSEP_TALK_NOWAIT || talkmsg->wait == DUSEP_TALK_WAIT) {
                                LcClientInDuSepEvent *in_event = conn->getInEvent();
                                in_event->setReactor(talkmsg->reactor);
                                in_event->setWriteEvent(dynamic_cast<LcClientDuSepNsheadEvent*>(lcevent));
                                lcevent->setTimeout(talkmsg->writetimeout);
                                in_event->session_begin();
                                lcevent->release();
                            } else {
							    lcevent->set_firstreactor(talkmsg->reactor);
							    lcevent->session_begin();
							    lcevent->release();
                            }
						} else {
							
							LC_LOG_WARNING("talkwith type[%d] error", baseTalk->type);
							baseTalk->success = lc::LC_ERROR_TYPE;
						}
					} else {
                        notify_strategy(conn);
						freeConnection(conn);
					}
					return 0;
				}
			}
		} else{
			comcm::AutoWrite auto_lock(&_svrlock);
			_conns[conn->_id_in_svr] = NULL;
			-- _liveConn;
			needReturn = true;
		}

		if(needReturn){
            notify_strategy(conn);
			_mgr->returnConnection(conn);
			_svrlock.write();
			if (_waitingTalkList.size() > 0) {
				lcclient_talkwith_t *baseTalk = _waitingTalkList.front();
				_waitingTalkList.pop_front();
				_svrlock.unlock();
				if (NULL != baseTalk) {
					LcClientConnectManager *mgr = getManager();
					int fret = mgr->fetchConnectionRetry(baseTalk, *baseTalk->conf, baseTalk->event,
                                                         baseTalk->reactor);
					if (fret != 0) {
						baseTalk->talkwithcallback(baseTalk);
					}
					baseTalk->event->release();
				}
			} else {
				_svrlock.unlock();
			}
		}
		return 0;
	}

	int LcClientServer :: fetchConnectionAsync(lcclient_talkwith_t *basetalk, lc::IEvent * event, 
						   lc::NetReactor *reactor)
	{
		LC_LOG_DEBUG("%s", __func__);

		if (NULL == basetalk || NULL == event || NULL == reactor) {
			LC_LOG_WARNING("input NULL");
			return -1;
		}

		lc::SockEvent *sockEvent = dynamic_cast<lc::SockEvent *>(event);
		if (NULL == sockEvent) {
			LC_LOG_WARNING("input event error");
			return -1;
		}

		if((!isEnable()) || NULL == _mgr){
			LC_LOG_WARNING("Server is not enable!");
			return -1;
		}

		if(_mgr->getConnectType() == comcm::LONG_CONN) {
			
			LcClientConnection * tmp = NULL;
			_svrlock.write();
			if(_liveConn > 0 && _pendingLongConn > 0){
                
                if (basetalk->wait == DUSEP_TALK_WAIT || basetalk->wait == DUSEP_TALK_NOWAIT) {
                    if (_liveConn >= _capacity) {
                        
                        int temp_index = rand() % _capacity;
                        int temp_count = 0;
                        while (temp_count < _capacity) {
                            int curr_index = temp_index % _capacity;
                            if (NULL != _conns[curr_index] && 
                                    _conns[curr_index]->getKernelStatus() == comcm::CONN_IS_CONNECTED) {
							    tmp = dynamic_cast<LcClientConnection*>(_conns[curr_index]);
							    tmp->setKernelStatus(comcm::CONN_IS_USING);
							    -- _pendingLongConn; 
							    break;
                            }
                            temp_index ++;
                            temp_count ++;
                        }
                    } else {
                        
                    }
                } else {
				for(int i = 0; i < _capacity; ++i){
					if(NULL!=_conns[i] && _conns[i]->getKernelStatus()==comcm::CONN_IS_CONNECTED){
						
						int pret = _conns[i]->ping();
						if(pret != 0){
							_mgr->returnConnection(_conns[i]);
							_conns[i] = NULL;
							-- _liveConn; 
							-- _pendingLongConn; 
						} else {
							tmp = dynamic_cast<LcClientConnection*>(_conns[i]);
							tmp->setKernelStatus(comcm::CONN_IS_USING);
							-- _pendingLongConn; 
							break;
						}
					}
				}
                }
			}
			_svrlock.unlock();

			if(NULL != tmp){
				LC_LOG_DEBUG("using exsit connetion..");
				
                return postEvent(event, reactor, basetalk, tmp);
			}
		}

		
		LcClientConnection * p = NULL;

		p = dynamic_cast<LcClientConnection*>(_mgr->borrowConnection());
		if (NULL == p) {
			LC_LOG_WARNING("borrowConnection error");
			event->setResult(IEvent::ERROR);
			return -1;
		}

		p->_serverID = _id;

		_svrlock.write();
		if(_liveConn < _capacity){
			int i;
			for(i = 0; i < _capacity; ++i){
				if(NULL == _conns[i]){
					_conns[i] = p;
					p->_id_in_svr = i;
					p->setKernelStatus(comcm::CONN_IS_USING);
					++ _liveConn;
					break;
				}
			}
			if (i < _capacity) {
				_svrlock.unlock();
				
				
				
				
				
				
				if (basetalk->type == LCCLIENT_NSHEAD) {
					nshead_talkwith_t *talkmsg = dynamic_cast<nshead_talkwith_t*>(basetalk);
					if (NULL == talkmsg) {
						LC_LOG_WARNING("nshead_talk type is error");
						
						freeConnection(p);
						return -1;
					}
					talkmsg->event = event;
					talkmsg->reactor = reactor;
					lcclient_nshead_atalk_t *currTalk = &talkmsg->innerTalk;
					currTalk->conn = p;
					currTalk->talk = talkmsg;
					currTalk->server = this;
					currTalk->readtimeout = getReadTimeout();
					currTalk->writetimeout = getWriteTimeout();
					set_lcnshead_atalk(currTalk, talkmsg, lc_server_connect_callback, currTalk);

					if (_ip_vec.size() <= 0) {
						LC_LOG_WARNING("server ip is empty, log_id:%u", talkmsg->reqhead.log_id);
						p->setErrno(-1);
						
						freeConnection(p);
						return -1;
					}
					currTalk->ipnum = 0;
					while (currTalk->ipnum < (int)_ip_vec.size()) {
						if (sockEvent->tcpConnect(_ip_vec[currTalk->ipnum], _port) != 0) {
							LC_LOG_WARNING("tcpConnect error ip:%s, port:%d, log_id:%u [%m]",
											_ip_vec[currTalk->ipnum], _port,
											talkmsg->reqhead.log_id);
							LcClientManager *t_mgr = currTalk->mgr;
							if (t_mgr) {
								t_mgr->add_monitor_info_ipport("CONNECT_FAIL_NUMBER", getIP(currTalk->ipnum), getPort(), getManager()->getServiceName());
							}

							currTalk->ipnum ++;
							continue;
						} else {
							break;
						}
					}
				
					if (currTalk->ipnum >= (int)_ip_vec.size()) {
						p->setErrno(-1);
						setHealthy(false);
						freeConnection(p);
						return -1;
					}
					
					sockEvent->setTimeout(_ctimeout);
					sockEvent->setCallback(lc_server_connect_callback, currTalk);
					if (reactor->post(sockEvent) != 0) {
						LC_LOG_WARNING("post event error, log_id:%u",
										talkmsg->reqhead.log_id);
						p->setErrno(-1);
						freeConnection(p);
						return -1;
					}
					return 0;
				} else if (LCCLIENT_COMMON == basetalk->type) {
					common_talkwith_t *talkmsg = dynamic_cast<common_talkwith_t*>(basetalk);
					if (NULL == talkmsg) {
						LC_LOG_WARNING("common_talk type is error");
						freeConnection(p);
						return -1;
					}
					LcEvent *baseEvent = dynamic_cast<LcEvent*>(sockEvent);
					if (NULL == baseEvent) {
						LC_LOG_WARNING("common_talk event is error");
						freeConnection(p);
						return -1;
					}
					struct timeval tv;
    				gettimeofday(&tv, NULL);
					baseEvent->get_sock_data()->timer.start = tv;
					talkmsg->event = event;
					talkmsg->reactor = reactor;  
					talkmsg->conn = p;
					talkmsg->server = this;
					talkmsg->readtimeout = getReadTimeout();
					talkmsg->writetimeout = getWriteTimeout();

					if (_ip_vec.size() <= 0) {
						LC_LOG_WARNING("server ip is empty, logid:%d", baseEvent->get_logid());
						p->setErrno(-1);
						freeConnection(p);
						baseEvent->set_sock_status(STATUS_CONNERROR);
						return -1;
					}
					talkmsg->ipnum = 0;
					while (talkmsg->ipnum < (int)_ip_vec.size()) {
						if (baseEvent->tcpConnect(_ip_vec[talkmsg->ipnum], _port) != 0) {
							LC_LOG_WARNING("tcpConnect error ip:%s, port:%d, logid:%d [%m]",
											_ip_vec[talkmsg->ipnum], _port, baseEvent->get_logid());
							LcClientManager *t_mgr = talkmsg->mgr;
							if (t_mgr) {
								t_mgr->add_monitor_info_ipport("CONNECT_FAIL_NUMBER", getIP(talkmsg->ipnum), getPort(), getManager()->getServiceName());
							}

							talkmsg->ipnum ++;
							continue;
						} else {
							break;
						}
					}
				
					if (talkmsg->ipnum >= (int)_ip_vec.size()) {
						LC_LOG_WARNING("connect all ip error, logid:%d", baseEvent->get_logid());
						p->setErrno(-1);
						setHealthy(false);
						freeConnection(p);
						baseEvent->set_sock_status(STATUS_CONNERROR);
						return -1;
					}
					lc_socket_t *sock_info = baseEvent->get_sock_data();
                    sock_info->connect_timeout = _ctimeout;
					baseEvent->setTimeout(_ctimeout);
                    if (talkmsg->wait == DUSEP_TALK_NOWAIT || talkmsg->wait == DUSEP_TALK_WAIT) {
                        LcClientInDuSepEvent *in_event = p->getInEvent();
                        in_event->setReactor(reactor);
                    } else {
					    baseEvent->set_firstreactor(reactor);
                    }
					if (baseEvent->activating_session() != 0) {
						LC_LOG_WARNING("post event error, logid:%d", baseEvent->get_logid());
						p->setErrno(-1);
						p->setHandle(baseEvent->handle());
						freeConnection(p);
						baseEvent->set_sock_status(STATUS_CONNERROR);
						return -1;
					}
					return 0;
				} else {
					basetalk->success = lc::LC_ERROR_TYPE;
					
					freeConnection(p);
					return -1;
				}
			} else {
				_svrlock.unlock();
				p->setErrno(-1);
				
				freeConnection(p);
				return -1;
			}
		} else{
			_mgr->returnConnection(p);
            if (basetalk->wait == DUSEP_TALK_NOWAIT) {
                _svrlock.unlock();
                return -1;
            } 
            
			if ((int)_waitingTalkList.size() >= _maxWaitingNum) {
				_svrlock.unlock();
				LC_LOG_WARNING("waiting talk reaches the max num [%d]", _maxWaitingNum);
				basetalk->success = lc::LC_ASYNCLIST_FULL;
				if (basetalk->type == LCCLIENT_COMMON) {
					LcEvent *baseEvent = dynamic_cast<LcEvent*>(sockEvent);
					if (NULL != baseEvent) {
						baseEvent->set_sock_status(STATUS_CONNERROR);
					}
				}
				return LC_ASYNCLIST_FULL;
			} else {
				if (basetalk->type == LCCLIENT_NSHEAD) {
					nshead_talkwith_t *talkmsg = dynamic_cast<nshead_talkwith_t*>(basetalk);
					if (NULL == talkmsg) {
                		_svrlock.unlock();
						LC_LOG_WARNING("nshead_talk type error");
                		return -1;
					}
					event->addRef();
                    event->setReactor(reactor);
					talkmsg->event = event;
					talkmsg->reactor = reactor;
					_waitingTalkList.push_back(talkmsg);
					_svrlock.unlock();
				} else if (basetalk->type == LCCLIENT_COMMON) {
					common_talkwith_t *talkmsg = dynamic_cast<common_talkwith_t*>(basetalk);
					if (NULL == talkmsg) {
						_svrlock.unlock();
						LC_LOG_WARNING("common_talk type error");
						return -1;
					}
					event->addRef();
					talkmsg->event = event;
					talkmsg->reactor = reactor;
					_waitingTalkList.push_back(talkmsg);
					_svrlock.unlock();
				} else {
					
					_svrlock.unlock();
					basetalk->success = lc::LC_ERROR_TYPE;
					return -1;
				}
				return 0;
			}
		}

		_svrlock.unlock();
		return 0;

	}

	const char * LcClientServer :: getIP(int num)
	{
		
		if (num < 0 || num >= int(_ip_vec.size())) {
			LC_LOG_WARNING("num[%d] small than zero or big than ip_vec size[%d]", 
						num, int(_ip_vec.size()));
			return NULL;
		}
		return _ip_vec[num];
	}

	int LcClientServer :: getIPCount()
	{
		
		return _ip_vec.size();
	}

	int LcClientServer :: getReadTimeout()
	{
		
		return _conf_info.readtimeout;
	}

	int LcClientServer :: setReadTimeout(int readTimeout)
	{
		if (readTimeout < 0) {
			return -1;
		}
		
		_conf_info.readtimeout = readTimeout;

		return 0;
	}

	int LcClientServer :: getWriteTimeout()
	{
		
		return _conf_info.writetimeout;
	}

	int LcClientServer :: setWriteTimeout(int writeTimeout)
	{
		if (writeTimeout < 0) {
			return -1;
		}
		
		_conf_info.writetimeout = writeTimeout;

		return 0;
	}

	int LcClientServer :: getServerArg(server_conf_info_t & servConf)
	{
		comcm::AutoRead auto_lock(&_svrlock);
		servConf = _conf_info;

		return 0;
	}

	int LcClientServer ::setServerArg(const server_conf_info_t & serverArg)
	{
		if (serverArg.readtimeout < 0 || serverArg.writetimeout < 0) {
			return -1;
		}
		comcm::AutoWrite auto_lock(&_svrlock);
		_conf_info = serverArg;

		return 0;
	}

	const char *LcClientServer :: getServerConfAll()
	{
		
		return _confSave;
	}

	const char *LcClientServer :: getRange()
	{
		comcm::AutoRead auto_lock(&_svrlock);
		return _range.c_str();
	}

	const char *LcClientServer :: getVersion()
	{
		comcm::AutoRead auto_lock(&_svrlock);
		return _version.c_str();
	}

	int LcClientServer :: getMaster()
	{
		
		return _master;
	}

	bsl::var::IVar & LcClientServer :: getConf(bsl::ResourcePool *rp, int *errNo)
	{
		static bsl::var::Null emptyVar;    

		if (NULL == errNo) {
			return emptyVar;
		}    

		if (NULL == rp) {
			*errNo = -1;
			return emptyVar;
		}

		comcm::AutoRead autoread(&_svrlock);
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
			LC_LOG_WARNING("conf to var error [%d]", *errNo);
			*errNo = -1;
			return emptyVar;
		}

		*errNo = 0;
		return conf;
	}

	int LcClientServer :: copySaveBuffer(const char* resBuf, int len)
	{
		if (0 >= len || NULL == resBuf) {
			return -1;
		}

		if (_oldConfSaveLen < len) {
			if (NULL != _confSave) {    
				delete [] _confSave;
				_confSave = NULL;
				_oldConfSaveLen = 0;
				_confSaveLen = 0;
			}
			_oldConfSaveLen = len;
			_confSaveLen = len;
			_confSave = new (std::nothrow)char [_oldConfSaveLen];
			if (NULL == _confSave) {
				LC_LOG_WARNING("new char for save conf error");
				_oldConfSaveLen = 0;
				_confSaveLen = 0;
				return -1;
			}
		} else {
			_confSaveLen = len;
		}

		snprintf(_confSave, _oldConfSaveLen, "%s", resBuf);

		return 0;
	}

	int LcClientServer :: connectAll()
	{
		LC_LOG_DEBUG("%s", __func__);
		
		if((!isEnable()) || NULL == _mgr){
			LC_LOG_WARNING("server is error");
			setHealthy(false);
			return -1;
		}
		if(_mgr->getConnectType() == comcm::LONG_CONN) {
			comcm::Connection *p = NULL;
			while (_liveConn < _capacity) {
				p = _mgr->borrowConnection();
				if (NULL == p) {
					LC_LOG_WARNING("server [%s : %d] is full", getIP(), getPort());
					return -4;
				}
				p->_serverID = _id;
				int ret = p->connect(this);
				if (0 != ret) {
					LC_LOG_WARNING("connection [%d] error", _liveConn);
					setHealthy(false);
					_mgr->returnConnection(p);
					return -3;
				} else {
					LC_LOG_DEBUG("connect ok");
					for(int i = 0; i < _capacity; ++i) {
						if(NULL == _conns[i]) {
							_conns[i] = p;
							p->_id_in_svr = i;
							p->setKernelStatus(comcm::CONN_IS_CONNECTED);
							++ _pendingLongConn;
							++ _liveConn;
							break;
						}
					}
				}
			}
			return 0;

		} else {
			LC_LOG_WARNING("server is for short connection");
			return -2;
		}

		return 0;
	}

	void LcClientServer :: clearIpVec()
	{
		char *ipInVec = NULL;
		for (int i=0; i<(int)_ip_vec.size(); i++) {
			ipInVec = _ip_vec[i];
			if (NULL != ipInVec) {
				delete []ipInVec;
				ipInVec = NULL;
			}
			_ip_vec[i] = NULL;
		}
		_ip_vec.clear();
		
		return;
	}
	
	void LcClientServer :: postConn(lcclient_nshead_atalk_t *currTalk, LcClientServer *server,
									lc::SockEvent *sockEvent)
	{
        LcClientManager *t_mgr = currTalk->mgr;
        if (t_mgr) {
            t_mgr->add_monitor_info_ipport("CONNECT_FAIL_NUMBER", server->getIP(currTalk->ipnum), server->getPort(), server->getManager()->getServiceName());
        }


		if (currTalk->ipnum+1 >= (int)server->_ip_vec.size()) {
			LC_LOG_WARNING("connect all ip error");
			currTalk->conn->setErrno(-1);
			server->setHealthy(false);  
			server->freeConnectionAsync(currTalk->conn);
			currTalk->talk->success = NSHEAD_TCP_CONNECT_ERROR;
			
			LcClientStrategy *currStt = server->getManager()->getStrategy();
			currStt->setServerArgAfterConn(server, -2);
			LcClientConnectManager *mgr = server->getManager();
			int fret = mgr->fetchConnectionRetry(currTalk->talk, *currTalk->talk->conf, sockEvent,
									dynamic_cast<NetReactor*>(sockEvent->reactor()));
			if (0 != fret) {
                currTalk->talk->talkwithcallback(currTalk->talk);
            }
			return;
		} else {
			currTalk->ipnum ++;
			while (currTalk->ipnum < (int)server->_ip_vec.size()) {
				if (sockEvent->tcpConnect(server->_ip_vec[currTalk->ipnum], 
										server->_port) != 0) {
					LC_LOG_DEBUG("tcpConnect error : ip:%s,port:%d [%m]", 
								server->_ip_vec[currTalk->ipnum], server->_port);
					currTalk->ipnum ++;
					continue;
				} else {
					break;
				}
			}
			if (currTalk->ipnum >= (int)server->_ip_vec.size()) {
				LC_LOG_WARNING("all tcpConnect error ip:%s port:%d", server->_ip_vec[0],
								server->_port); 
				currTalk->conn->setErrno(-1);
				server->setHealthy(false);  
				server->freeConnectionAsync(currTalk->conn);
				currTalk->talk->success = NSHEAD_TCP_CONNECT_ERROR;
				
				LcClientStrategy *currStt = server->getManager()->getStrategy();
				currStt->setServerArgAfterConn(server, -2);
				
				LcClientConnectManager *mgr = server->getManager();
				int fret = mgr->fetchConnectionRetry(currTalk->talk, *currTalk->talk->conf, sockEvent,
										dynamic_cast<NetReactor*>(sockEvent->reactor()));
				if (0 != fret) {
                	currTalk->talk->talkwithcallback(currTalk->talk);
            	}
				return;
			}
			LC_LOG_DEBUG("tcpConnect success : ip:%s,port:%d", 
						server->_ip_vec[currTalk->ipnum], server->_port);
			sockEvent->setTimeout(server->_ctimeout);
			sockEvent->setCallback(lc_server_connect_callback, currTalk);
			if (currTalk->talk->reactor->post(sockEvent) != 0) {
				LC_LOG_WARNING("post event error, logid:%u", currTalk->talk->reqhead.body_len);
				currTalk->conn->setErrno(-1);
				server->freeConnectionAsync(currTalk->conn);
				currTalk->talk->success = NSHEAD_TCP_CONNECT_ERROR;
				currTalk->talk->talkwithcallback(currTalk->talk);
				LcClientConnectManager *mgr = server->getManager();
				int fret = mgr->fetchConnectionRetry(currTalk->talk, *currTalk->talk->conf, sockEvent,
										dynamic_cast<NetReactor*>(sockEvent->reactor()));
				if (0 != fret) {
                	currTalk->talk->talkwithcallback(currTalk->talk);
            	}
				return;
			}
			return;
		}
		return;
	}
	
	void LcClientServer :: common_error_callback(IEvent *ev, void *talk)
	{
		if (NULL == ev || NULL == talk) {
			return ;
		}
		common_talkwith_t * curr_talk = (common_talkwith_t *)talk;
        if (NULL != curr_talk->user_callback) {
		    curr_talk->user_callback(curr_talk->event, talk);
        }

		return;
	}
    int LcClientServer :: postEvent(IEvent *event, NetReactor *reactor, lcclient_talkwith_t *basetalk,
            LcClientConnection *conn)
    {
        if (NULL == event || NULL == reactor || NULL == basetalk || NULL == conn) {
            LC_LOG_WARNING("input include NULL");
            return -1;
        }
		lc::SockEvent *sockEvent = dynamic_cast<lc::SockEvent *>(event);
		if (NULL == sockEvent) {
			LC_LOG_WARNING("input event error");
			return -1;
		}
	    if (basetalk->type == LCCLIENT_NSHEAD) {
		    nshead_talkwith_t *talkmsg = dynamic_cast<nshead_talkwith_t*>(basetalk);
			if (NULL == talkmsg) {
				LC_LOG_WARNING("nshead_talk: type is error");
				freeConnection(conn);
				
				
				return -1;
			}
			talkmsg->event = event;
			talkmsg->reactor = reactor;
			lcclient_nshead_atalk_t *currTalk = &talkmsg->innerTalk;
			currTalk->conn = conn;
			currTalk->talk = talkmsg;
			currTalk->server = this;
			currTalk->readtimeout = getReadTimeout();
			currTalk->writetimeout = getWriteTimeout();
			set_lcnshead_atalk(currTalk, talkmsg, lc_server_callback, currTalk);

			int *strFD = (int*)(conn->getHandle());
			int currFD = *strFD;
			if (currFD < 0) {
				talkmsg->success = lc::NSHEAD_TCP_CONNECT_ERROR;
				LC_LOG_WARNING("nshead_talk: socket fd error, logid:%u", talkmsg->reqhead.body_len);
				conn->setErrno(-1);
				
				freeConnection(conn);
				return -1;
			}

			sockEvent->setHandle(currFD);
			sockEvent->setTimeout(currTalk->writetimeout);
			lc::nshead_atalkwith1(sockEvent, &currTalk->slctalk);

			if (reactor->post(sockEvent) != 0) {
				LC_LOG_WARNING("post event error, logid:%u", talkmsg->reqhead.body_len);
				conn->setErrno(-1);
				
				freeConnection(conn);
				sockEvent->setHandle(-1);
				return -1;
			}
			return 0;
		} else if (basetalk->type == LCCLIENT_COMMON) {
			common_talkwith_t *talkmsg = dynamic_cast<common_talkwith_t*>(basetalk);
			if (NULL == talkmsg) {
				LC_LOG_WARNING("common_talk: type is error");
				
				freeConnection(conn);
				return -1;
			}
			LcEvent *lcevent = dynamic_cast<LcEvent*>(event);
			if (NULL == lcevent) {
				talkmsg->success = lc::LC_ERROR_TYPE;
				LC_LOG_WARNING("common_talk: event is error");
				freeConnection(conn);
				return -1;
			}
			lcevent->setStatus(0);
			timeval tv;
			gettimeofday(&tv, NULL);
			lcevent->get_sock_data()->timer.start = tv;
			talkmsg->event = event;
			talkmsg->reactor = reactor;
			talkmsg->conn = conn;
			talkmsg->server = this;
			talkmsg->readtimeout = getReadTimeout();
			talkmsg->writetimeout = getWriteTimeout();

			int *strFD = (int*)(conn->getHandle());
			int currFD = *strFD;
			if (currFD < 0) {
				talkmsg->success = lc::NSHEAD_TCP_CONNECT_ERROR;
				lcevent->set_sock_status(STATUS_CONNERROR);
				LC_LOG_WARNING("nshead_talk: socket fd error, logid:%d", lcevent->get_logid());
				conn->setErrno(-1);
				freeConnection(conn);
				return -1;
			}
			lcevent->setHandle(currFD);
			lcevent->setTimeout(talkmsg->writetimeout);

            
            if (talkmsg->wait == DUSEP_TALK_NOWAIT || talkmsg->wait == DUSEP_TALK_WAIT) {
                LcClientInDuSepEvent *in_event = conn->getInEvent();
                in_event->setHandle(currFD);
                in_event->setReactor(reactor);
                in_event->setWriteEvent(dynamic_cast<LcClientDuSepNsheadEvent*>(lcevent));
                lcevent->setTimeout(talkmsg->writetimeout);
                in_event->session_begin();
            } else {
			    lcevent->set_firstreactor(reactor);
			    lcevent->session_begin();
            }
			return 0;	
		} else {
			
			basetalk->success = lc::LC_ERROR_TYPE;
			
			freeConnection(conn);
			return -1;
		}
        return -1;
    }

    void LcClientServer :: notify_strategy(LcClientConnection *conn)
    {
        LcClientConnectManager *mgr = dynamic_cast<LcClientConnectManager*>(_mgr);
        if (NULL == mgr) {
            LC_LOG_WARNING("_mgr from connectpool is not LcClientConnectManager");
            return;
        }
		LcClientStrategy *temp_stt = mgr->getStrategy();
		if (NULL != temp_stt) {
		    temp_stt->notify(conn, comcm::CONN_FREE);
		}
    }

    int LcClientServer :: getPoolUsage()
    {
        return (int)((((float)_liveConn)/(float)getCapacity()) * 100);
    }

    int LcClientServer :: getPoolUsingNum()
    {
        return _liveConn;
    }

    int LcClientServer :: getPoolMaxNum()
    {
        return getCapacity();
    }
}


