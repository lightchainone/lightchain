
#include "connectmgr.h"


namespace comcm{

	Connection :: Connection(){
		_userStatus = 0;
		_kernelStatus = CONN_NOT_USED;
		_err = 0;
		_serverID = -1;
		_self_data = NULL;
		_self_cleaner = NULL;
		_handle = NULL;
		_lastActive = 0;
        _log = ConnectManager::defaultLogAdapter(); 
	}

	Connection :: ~Connection(){
		
		if(CONN_IS_CONNECTED == _kernelStatus){
			disconnect();
			_kernelStatus = CONN_NOT_USED;
		}
		if(_self_data && _self_cleaner){
			_self_cleaner(_self_data);
		}
	}

	void Connection :: setStatus(int st){
		_userStatus = st;
	}

	int Connection :: getStatus(){
		return _userStatus;
	}

	void Connection :: setErrno(int err){
		_err = err;
	}

	int Connection :: getErrno(){
		return _err;
	}

	void Connection :: setKernelStatus(int st){
		_kernelStatus = st;
		_lastActive = time(0);
	}

	int Connection :: getKernelStatus(){
		return _kernelStatus;
	}

	int Connection :: serverID(){
		return _serverID;
	}

	time_t Connection :: lastActive(){
		return _lastActive;
	}

	void Connection :: clear(){
		disconnect();
		if(_self_cleaner && _self_data){
			_self_cleaner(_self_data);
		}
		_userStatus = 0;
		_kernelStatus = CONN_NOT_USED;
		_err = 0;
		_serverID = -1;
		_self_data = NULL;
		_self_cleaner = NULL;
		_handle = NULL;
		_lastActive = 0;
	}

	void Connection :: setSelfData(void * data, data_destructor_t _dest){
		if(_self_data && _self_cleaner){
			_self_cleaner(_self_data);
		}
		_self_data = data;
		_self_cleaner = _dest;
	}

	void * Connection :: getSelfData(){
		return _self_data;
	}

	void * Connection :: getHandle(){
		return _handle;
	}

	int Connection :: connect(Server *){ 
		return 0;
	}

	int Connection :: disconnect(){ 
		return 0;
	}

	int Connection :: ping(){ 
		return 0;
	}

	Server :: Server(){
		_id = 0;
		_capacity = 0;
		_ctimeout = 0;
		_liveConn = 0;
		_pendingLongConn = 0;
		_enable = true;
		_isHealthy = true;
		_mgr = NULL;
		_log = ConnectManager :: defaultLogAdapter();
	}

	Server :: ~Server(){
		closeExtraConnection(0);
	}

	int Server :: initBasicValue(const comcfg::ConfigUnit & conf){
		if(NULL == _mgr || NULL == _log){
			return -1;
		}
		_log->debug("[%s:%d]initBasicValue for Server", __FILE__, __LINE__);
		if( conf[CM_IP].get_cstr(_ip, sizeof(_ip)) != 0 || (_ip_int = ip_to_int(_ip)) == 0){
			_log->warning("[%s:%d]initBasicValue for Server error: no ip or ip format err",
					__FILE__, __LINE__);
			return -1;
		}
		
		conf[CM_PORT].get_int32(&_port, _mgr->_defaultPortForServer); 
		conf[CM_CAPACITY].get_int32(&_capacity, _mgr->_defaultCapacityForServer);
		conf[CM_CTIMEOUT].get_int32(&_ctimeout, _mgr->_defaultCtimeoutForServer);
		if(_port < 0 || _capacity < 0 || _ctimeout < 0){
			_log->warning("[%s:%d]initBasicValue failed : negative value", __FILE__, __LINE__);
			return -1;
		}
		if(_capacity > (int)_conns.size()){
			_conns.resize(_capacity, NULL);
		}
		if( conf[CM_TAG].selfType() != comcfg::CONFIG_ARRAY_TYPE 
				&& conf[CM_TAG].selfType() != comcfg::CONFIG_ERROR_TYPE){
			_log->warning("[%s:%d]initBasicValue failed : tag config should be array .",
					__FILE__, __LINE__);
		}
		int err = 0;
		for( int i = 0; i < (int)conf[CM_TAG].size(); ++i ){
			const char * tg = conf[CM_TAG][i].to_cstr(&err);
			if(err == 0){
				_tag.push_back(str_t(tg));
			}
		}
		return 0;
	}

	int Server :: init(const comcfg :: ConfigUnit & conf){
		return initBasicValue(conf);
	}

	bool Server :: equal(const Server & other){
		AutoRead auto_lock(&_svrlock);
		return _ip_int == other._ip_int && _port == other._port;
	}
	
	int Server :: clone(const Server * oth){
		if(oth == NULL){
			return -1;
		}
		AutoWrite auto_lock(&_svrlock);
		snprintf(_ip, sizeof(_ip), "%s", oth->_ip);
		_ip_int = oth->_ip_int;
		_port = oth->_port;
		_capacity = oth->_capacity;
		if(_capacity > (int)_conns.size()){
			_conns.resize(_capacity, NULL);
		}
		_ctimeout = oth->_ctimeout;
		_tag.clear();
		_tag = oth->_tag;
		return 0;
	}

	Connection * Server :: fetchConnection(int * err){
		if((! isEnable()) || NULL == _mgr){
			_log->trace("[%s:%d] Server is not enable! _mgr = %p",
                    __FILE__, __LINE__, _mgr);
			*err = REQ_SVR_DISABLE;
			return NULL;
		}
		_log->debug("[%s:%d] fetchConnection on Server [%s:%d]",
                __FILE__, __LINE__, getIP(), getPort());
		*err = REQ_OK;
		if(_mgr->getConnectType() == LONG_CONN){
			
			AutoWrite auto_lock(&_svrlock);
            _log->debug("[%s:%d] auto_lock svrlock, _liveConn[%d], _pendingLongConn[%d]", 
                    __FILE__, __LINE__,
                    _liveConn, _pendingLongConn);
			Connection * tmp = NULL;
			if(_liveConn > 0 && _pendingLongConn > 0){
				for(int i = 0; i < _capacity; ++i){
					if(_conns[i] != NULL && _conns[i]->getKernelStatus() == CONN_IS_CONNECTED){
						
						int pret = _conns[i]->ping();
						if(pret != 0){
							_mgr->returnConnection(_conns[i]);
							_conns[i] = NULL;
							-- _liveConn; 
							-- _pendingLongConn; 
						}
						else{
							tmp = _conns[i];
							tmp->setKernelStatus(CONN_IS_USING);
							-- _pendingLongConn; 
							break;
						}
					}
				}
			}
			if(tmp != NULL){
				_log->debug("[%s:%d]using exsit connetion..", __FILE__, __LINE__);
				return tmp;
			}
		}
		
		Connection * p = NULL;

        

        p = _mgr->borrowConnection();
        if (NULL == p) {
            *err = REQ_SVR_FULL;
            _log->debug("[%s:%d] Server [%s:%d] is full",
                    __FILE__, __LINE__, getIP(), getPort());
            return NULL;
        }

		p->_serverID = _id;
		_svrlock.write();
		if(_liveConn < _capacity){
			for(int i = 0; i < _capacity; ++i){
				if(NULL == _conns[i]){
                    _log->debug("[%s:%d] conns [%d] find", 
                            __FILE__, __LINE__, i);
					_conns[i] = p;
					p->_id_in_svr = i;
					p->setKernelStatus(CONN_IS_USING);
					++ _liveConn;
					break;
				}
			}
		}
		else{
			*err = REQ_SVR_FULL;
		}
		_svrlock.unlock();
		int res = 0;

		if(*err != REQ_OK){
			_log->debug("[%s:%d]Can't fetch new connection [_liveConn = %d, _capacity = %d]",
					__FILE__, __LINE__, _liveConn, _capacity);
			goto conn_failed;
		}

		res = p->connect(this);
		_log->debug("Server [%s:%d] on Line [%s:%d] connect res = %d", getIP(), getPort(), __FILE__, __LINE__, res);

		if(res != 0){
			

			_log->warning("[%s:%d] Server [%s:%d] connect failed.",__FILE__, __LINE__, getIP(), getPort());
			_svrlock.write();
			_conns[p->_id_in_svr] = NULL;
			-- _liveConn;
			*err = REQ_CONN_FAILED;
			_isHealthy = false;
			_svrlock.unlock();
		}
		else{
			
			
		}

conn_failed:
		if(*err != REQ_OK){
			_mgr->returnConnection(p);
			return NULL;
		}
		else{
			_log->debug("[%s:%d] Server [%s:%d] connect successfully.",
                    __FILE__, __LINE__, getIP(), getPort());
			return p;
		}
	}

	int Server :: freeConnection(Connection * c){
		bool needReturn = false;
        _log->debug("[%s:%d] freeConnection _liveConn[%d] _pendingLongConn[%d]",
                 __FILE__, __LINE__,
                _liveConn, _pendingLongConn);
		if(NULL == c){
			_log->warning("[%s:%d]Something err? free an NULL connection", __FILE__, __LINE__);
			return -1;
		}

        if (c->getErrno() != 0) {
            _log->warning("[%s:%d]freeConnection connection err[%d], ip[%s] port[%d]", __FILE__, __LINE__,
                          c->getErrno(), getIP(), getPort());
        }

		if(_mgr->getConnectType() == LONG_CONN){
			if(c->getErrno() != 0){
				AutoWrite auto_lock(&_svrlock);
				
				_conns[c->_id_in_svr] = NULL;
				-- _liveConn;
				needReturn = true;
			}
			else{
                AutoWrite auto_lock(&_svrlock);
				c->setKernelStatus(CONN_IS_CONNECTED);
				++ _pendingLongConn;
			}
		}
		else{
			AutoWrite auto_lock(&_svrlock);
			_conns[c->_id_in_svr] = NULL;
			-- _liveConn;
			needReturn = true;
		}
		if(needReturn){
			_mgr->returnConnection(c);
		}
		return 0;
	}

	int Server :: closeExtraConnection(int n){
		
		if(NULL == _mgr || _mgr->getConnectType() == SHORT_CONN){
			return 0;
		}
		AutoWrite auto_lock(&_svrlock);
		int ret = 0;
		if(_liveConn > n){
			int livecnt = _liveConn - n;
			for(int i = _capacity - 1; i >= 0 && livecnt > 0 && _pendingLongConn > 0; --i){
				if(_conns[i] != NULL && _conns[i]->getKernelStatus() == CONN_IS_CONNECTED){
					_mgr->returnConnection(_conns[i]);
					_conns[i] = NULL;
					-- _pendingLongConn;
					-- _liveConn;
					++ret;
				}
			}
		}
		return ret;
	}

	int Server :: liveConnectionNum(){
		AutoRead auto_lock(&_svrlock);
		return _liveConn;
	}

	Connection * Server :: getConnectionInfo(int id){
		if(id < 0 || id >= _capacity){
			return NULL;
		}
		AutoRead auto_lock(&_svrlock);
		return _conns[id];
	}

	
	
	const char * Server :: getIP() {
		AutoRead auto_lock(&_svrlock);
		return _ip;
	}

	int Server :: getPort(){
		AutoRead auto_lock(&_svrlock);
		return _port;
	}

    int Server :: getPendingLongConn(){
        AutoRead auto_lock(&_svrlock);
        return _pendingLongConn;
    }

	int Server :: getCapacity(){
		AutoRead auto_lock(&_svrlock);
		return _capacity;
	}

	int Server :: getTimeout(){
		AutoRead auto_lock(&_svrlock);
		return _ctimeout;
	}

	int Server :: setPort(int port){
		
		_port = port;
		return 0;
	}

	int Server :: setIP(const char * ip){
		
		snprintf(_ip, sizeof(_ip), "%s", ip);
		_ip_int = ip_to_int(_ip);
		if(0 == _ip_int){
			return -1;
		}
		return 0;
	}

	int Server :: setCapacity(int cap){
		AutoWrite auto_lock(&_svrlock);
		_capacity = cap;
		if(_capacity > (int)_conns.size()){
			_conns.resize(_capacity, NULL);
		}
		return 0;
	}

	int Server :: setTimeout(int tmout){
		AutoWrite auto_lock(&_svrlock);
		_ctimeout = tmout;
		return 0;
	}
	
	
	bool Server :: isHealthy(){
		AutoRead auto_lock(&_svrlock);
		return _isHealthy;
	}

	int Server :: setHealthy(bool health){
		AutoWrite auto_lock(&_svrlock);
		_isHealthy = health;
		return 0;
	}

	int Server :: disable(){
		closeExtraConnection(0);
		AutoWrite auto_lock(&_svrlock);
		_enable = false;
		return 0;
	}
	int Server :: enable(){
		AutoWrite auto_lock(&_svrlock);
		_enable = true;
		return 0;
	}
	
	bool Server :: isEnable(){
		AutoRead auto_lock(&_svrlock);
		return _enable;
	}

	
	int Server :: setID(int id){
		_id = id;
		return 0;
	}
	
	int Server :: getID(){
		return _id;
	}

	int Server :: setManager(ConnectManager * mgr){
		if(NULL == mgr){
			return -1;
		}
		_mgr = mgr;
		_log = _mgr->getLogAdapter();
		return 0;
	}

	int Server :: setTag(const char *tag){
		if(tag == NULL){
			return -1;
		}
		_tag.push_back(str_t(tag));
		return 0;
	}
	
	bool Server :: hasTag(const char * tag){
		for(int i = 0; i < (int)_tag.size(); ++i){
			if(strcmp(_tag[i].c_str(), tag) == 0){
				return true;
			}
		}
		return false;
	}

	int Server :: getTagSize(){
		return (int)_tag.size();
	}
	
	const char * Server :: getTag(int i){
		if(i < 0 || i >= (int)_tag.size()){
			return NULL;
		}
		return _tag[i].c_str();
	}
	
	int Server :: removeTag(const char * tag){
		if(NULL == tag){
			return -1;
		}
		int ret = -1;
		for(int i = 0; i < (int)_tag.size(); ++i){
			if(strcmp(_tag[i].c_str(), tag) == 0){
				ret = 0;
				_tag[i] = _tag[_tag.size() - 1];
				_tag.pop_back();
			}
		}
		return ret;
	}
}
















