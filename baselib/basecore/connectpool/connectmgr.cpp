#include "connectmgr.h"
#include <string.h>
namespace comcm{

	const int TMP_BUF_SIZE=32;
    const int MAX_TAG_NUM = 32;
	BasicLogAdapter ConnectManager :: _defaultLogAdapter;
	static Strategy g_defaultStrategy;
	static HealthyChecker g_defaultHealthyChecker;

	ConnectManager :: ConnectManager(){
		_defaultRetryTime = DEFAULT_RETRY_TIME;
		_log = &_defaultLogAdapter;
		_stt = &g_defaultStrategy;
		_hchk = &g_defaultHealthyChecker;
		_conn_type = SHORT_CONN;
		_defaultCapacityForServer = DEFAULT_CAPACITY;
		_defaultCtimeoutForServer = DEFAULT_CTIMEOUT;
		_defaultPortForServer = 0;
		conn_creater = NULL;
		svr_creater = NULL;
		_lastReloadTime = 0;
		_svrnum = 0;
		memset(_svr, 0, sizeof(_svr));
		for(int i = 0; i < CONN_LIST_NUM; ++i){
			pthread_mutex_init(&_list_lock[i], NULL);
		}
	}

	void ConnectManager :: close(){
		for(int i = 0; i < _svrnum; ++i){
			if(_svr[i]){
				delete _svr[i];
				_svr[i] = NULL;
			}
		}
		_svrnum = 0;
		for(int i = 0; i < CONN_LIST_NUM; ++i){
			AutoLock _lock(&_list_lock[i]);
			while(_conn_list[i].size() != 0){
				Connection *p = _conn_list[i].front();
				_conn_list[i].pop_front();
				delete  p;
			}
		}
	}
	ConnectManager :: ~ConnectManager(){
		for(int i = 0; i < (int)_tagvec.size(); ++i){
			delete _tagvec[i];
		}
		close();
	}

	Server ** ConnectManager :: readConfigData(const comcfg::ConfigUnit & conf, int * err){
		int svrsize = (int)conf[CM_SERVER].size();
		*err = 0;
		if(0 == svrsize){
			*err = -1;
			_log->warning("[%s:%d]No server at all..", __FILE__, __LINE__);
			return NULL;
		}
		conf[CM_DEF_CAPACITY].get_int32(&_defaultCapacityForServer);
		conf[CM_DEF_CTIMEOUT].get_int32(&_defaultCtimeoutForServer);
		conf[CM_DEF_PORT].get_int32(&_defaultPortForServer);
		Server ** svr;
#if 0
		try{
			svr = svr_creater(svrsize);
		}catch(...){
			_log->warning("[%s:%d]Creater failed...", __FILE__, __LINE__);
			return NULL;
		}
#endif

        svr = new (std::nothrow) Server*[svrsize];
        
		
		if(NULL == svr){
			_log->warning("[%s:%d]Creat server error..", __FILE__, __LINE__);
			*err = -1;
			return NULL;
		}
        for(int i = 0; i < svrsize; ++i) {
            svr[i] = NULL;
        }

		for(int i = 0; i < svrsize; ++i){
            svr[i] = svr_creater();
            if (svr[i] != NULL) {
                svr[i]->setManager(this);
                if( svr[i]->init(conf[CM_SERVER][i]) != 0 ){
                    _log->warning("load server[%d] failed..", i);
                    *err = -1;
                }
            } else {
                _log->warning("[%s:%d]Creat server[%d] error..", __FILE__, __LINE__, i);
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
                delete []svr;
            }
			svr = NULL;
		}
		return svr;
	}

	int ConnectManager :: init(const comcfg::ConfigUnit & conf){
		if(NULL == conn_creater || NULL == svr_creater){
			_log->warning("[%s:%d] call setCreater before init!", __FILE__, __LINE__);
			return -1;
		}
		int ret = 0;
		int svrsize = (int)conf[CM_SERVER].size();
		char ctype[TMP_BUF_SIZE];
		conf[CM_DEF_RETRY].get_int32(&_defaultRetryTime);
		if(conf[CM_CONN_TYPE].get_cstr(ctype, TMP_BUF_SIZE) != 0){
			_log->warning("[%s:%d] Read connect_type failed, use default = SHORT", __FILE__, __LINE__);
			_conn_type = SHORT_CONN;
		}
		else{
			if(strcmp(ctype, VALUE_LONG) == 0){
				_conn_type = LONG_CONN;
			}
			else if(strcmp(ctype, VALUE_SHORT) == 0){
				_conn_type = SHORT_CONN;
			}
			else{
				_log->warning("[%s:%d] Read connect_type failed, use default = SHORT", __FILE__, __LINE__);
				_conn_type = SHORT_CONN;
			}
		}

		Server ** svrs = readConfigData(conf, &ret);
		if(ret != 0){
			_log->warning("[%s:%d]init : read data from Configure err..", __FILE__, __LINE__);
			return -1;
		}
		ret = init(svrs, svrsize, _conn_type == SHORT_CONN);
        if(svrs){
            for (int i = 0; i < svrsize; ++i) {
                if (svrs[i]) {
                    delete svrs[i];
                }
            }
            delete [] svrs;
        }
        return ret;
	}

	int ConnectManager :: initTag(Server ** svrs, int svrnum){
		int tags = 0;
		_tagmap.clear();
		_taglist.clear();
		_tagvec.clear();
		for(int i = 0; i < svrnum; ++i){
			int n = svrs[i]->getTagSize();
			int id = svrs[i]->getID();
			tags += n;
			for(int j = 0; j < n; ++j){
                if (svrs[i] != NULL) {
                    str_t p = svrs[i]->getTag(j);
                    if(_tagmap.find(p) != _tagmap.end()){
                        int x = _tagmap[p];
                        _tagvec[x]->push_back(id);
                    }
                    else{
                        Array *vec = new Array;
                        vec->reserve(MAX_TAG_NUM);
                        _tagmap[p] = (int)_taglist.size();
                        _taglist.push_back(p);
                        vec->push_back(id);
                        _tagvec.push_back(vec);
                    }
                } else {
                    _log->warning("[%s:%d] init Tag [%d] is NULL", __FILE__, __LINE__, i);
                }
			}
		}
		return 0;
	}

	int ConnectManager :: reloadTag(Server ** svrs, int svrnum) {
		int tags = 0;

		_tagmap.clear();
		_taglist.clear();
		for(int i = 0; i < (int)_tagvec.size(); ++i){
			delete _tagvec[i];
		}
		_tagvec.clear();
		for(int i = 0; i < svrnum; ++i){
			int n = svrs[i]->getTagSize();
			int id = svrs[i]->getID();
			tags += n;
			for(int j = 0; j < n; ++j){
                if (svrs[i] != NULL) {
                    str_t p = svrs[i]->getTag(j);
                    if(_tagmap.find(p) != _tagmap.end()){
                        int x = _tagmap[p];
                        _tagvec[x]->push_back(id);
                    }
                    else{
                        Array *vec = new Array;
                        vec->reserve(MAX_TAG_NUM);
                        _tagmap[p] = (int)_taglist.size();
                        _taglist.push_back(p);
                        vec->push_back(id);
                        _tagvec.push_back(vec);
                    }
                } else {
                    _log->warning("[%s:%d] reload Tag [%d] is NULL", __FILE__, __LINE__, i);
                }
			}
		}
		return 0;
    }

	int ConnectManager :: init(Server ** svrs, int svrnum, bool shortConnection){
		if(NULL == conn_creater || NULL == svr_creater){
			_log->warning("[%s:%d] call setCreater before init!", __FILE__, __LINE__);
			return -1;
		}
		if(NULL == svrs && svrnum != 0){
			return -1;
		}
        if (svrnum < 0) {
            return -1;
        }
		for(int i = 0; i < svrnum; ++i){
			if(NULL == _svr[i]){
                #if 0
				try{
					_svr[i] = svr_creater(1);
				}catch(...){
					_log->warning("[%s:%d]No server at all..", __FILE__, __LINE__);
					return -1;
				}
                #endif

                _svr[i] = svr_creater();
                if (NULL == _svr[i]) {
                    _log->warning("[%s:%d]No server at all..", __FILE__, __LINE__);
                    return -1;
                }
			}
			_svr[i]->clone(svrs[i]);
			_svr[i]->setManager(this);
			_svr[i]->setID(i);
		}
		_svrnum = svrnum;
		if(shortConnection){
			_conn_type = SHORT_CONN;
		}
		else{
			_conn_type = LONG_CONN;
		}
		
		initTag(_svr, _svrnum);

		_lastReloadTime = time(0);
		return 0;
	}

	int ConnectManager :: reload(const comcfg::ConfigUnit & conf){
		int ret = 0;
		int svrsize = (int)conf[CM_SERVER].size();
		Server ** svrs = readConfigData(conf, &ret);
		if(NULL == svrs){
			_log->warning("[%s:%d]init : read data from Configure err..", __FILE__, __LINE__);
			return -1;
		}
		ret = reload(svrs, svrsize);
        if (-1 == ret) {
            _log->warning("[%s:%d]init : reload from Configure err..", __FILE__, __LINE__);
        }
        if (svrs) {
            for (int i = 0; i < svrsize; ++i) {
                if (svrs[i]) {
                    delete svrs[i];
                }
            }
            delete [] svrs;
        }
		return ret;
	}

	int ConnectManager :: reload(Server ** svrs, int svrn){
		if(NULL == svrs && svrn != 0){
			return -1;
		}
        if (svrn < 0) {
            return -1;
        }
        if (_svrnum >= MAX_SVR_NUM) {
            _log->warning("[%s:%d] _svrnum is too many servers. ", __FILE__, __LINE__);
            return -1;
        }
        if (svrn >= MAX_SVR_NUM) {
            _log->warning("[%s:%d] svrn is too many [%d]", __FILE__, __LINE__, svrn);
        }

		int eq[_svrnum];
		int isnew[svrn];
		int cnt = svrn;
		memset(eq, -1, sizeof(eq));
		memset(isnew, -1, sizeof(isnew));
		for(int i = 0; i < svrn; ++i){
			for(int j = 0; j < _svrnum; ++j){
				
				if(_svr[j]->equal(*svrs[i])){
					eq[j] = i;
					isnew[i] = 0;
					--cnt;
					break;
				}
			}
		}
		if(_svrnum + cnt >= MAX_SVR_NUM){
			
			_log->warning("[%s:%d] too many servers.. ", __FILE__, __LINE__);
			return -1;
		}
		
		
		int ret = 0;
		int _new_svrnum = _svrnum;
		for(int i = 0; i < svrn; ++i){
			if(isnew[i]){
				int curID = _new_svrnum;
                _svr[curID] = svr_creater();
                if (NULL == _svr[curID]) {
                    _log->warning("[%s:%d]Creater new server failed...", __FILE__, __LINE__);
                    ret = -1;
                    break;
                }
                
				_svr[curID]->setID(_new_svrnum);
				_svr[curID]->setManager(this);
				++ _new_svrnum;
				if( _svr[curID]->clone(svrs[i]) != 0 ){
					_log->warning("[%s:%d]Clone the new server at[%d] failed..", 
							__FILE__, __LINE__, i);
					ret = -1;
					break;
				}
				_log->debug("[%s:%d] Add new Server : [%d] %s : %d", __FILE__, __LINE__,
						_svr[curID]->getID(), _svr[curID]->getIP(), _svr[curID]->getPort());
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
		
		for(int i = 0; i < _svrnum; ++i){
			
			if(eq[i] != -1){
				if( _svr[i]->clone(svrs[eq[i]]) != 0 ){
					_log->warning("[%s:%d]Clone the exist but updated server at[%d] failed..", 
							__FILE__, __LINE__, i);
					ret = -1;
					break;
				}
			}
		}
		if(ret){
			return -1;
		}
		
		_rwlock.write();
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
		_rwlock.unlock();
		_lastReloadTime = time(0);
		return 0;
	}

    int ConnectManager :: addServer(Server *svr)
    {
	    if(NULL == conn_creater || NULL == svr_creater){
			_log->warning("[%s:%d] call setCreater before init!", __FILE__, __LINE__);
			return -1;
		}
        if (NULL == svr) {
            return -1;
        }
		
        for (int i = 0; i < _svrnum; ++i) {
            
            if (svr->equal(*_svr[i])) {
                 if (_svr[i]->clone(svr) != 0) {
                    
                    return -1;
                 }
                 return 0;
            }
        }
        int curID = _svrnum;
        
        if (curID >= MAX_SVR_NUM) {
            return -1;
        }
        _svr[curID] = svr_creater();
        if (NULL == _svr[curID]) {
            _log->warning("[%s:%d]Creater new server failed...", __FILE__, __LINE__);
            return -1;
        }
        _svr[curID]->setID(curID);
        _svr[curID]->setManager(this);
        if( _svr[curID]->clone(svr) != 0 ){
            _log->warning("[%s:%d]Clone the new server failed..", 
                    __FILE__, __LINE__);
            delete _svr[curID];
            return -1;
        }
        _log->debug("[%s:%d] Add new Server : [%d] %s : %d", __FILE__, __LINE__,
                _svr[curID]->getID(), _svr[curID]->getIP(), _svr[curID]->getPort());

	    int n = _svr[curID]->getTagSize();
        int id = _svr[curID]->getID();
        for(int j = 0; j < n; ++j) {
            str_t p = _svr[curID]->getTag(j);
            if(_tagmap.find(p) != _tagmap.end()){
                int x = _tagmap[p];
                _tagvec[x]->push_back(id);
            }
            else{
                Array *vec = new (std::nothrow) Array;
                vec->reserve(MAX_TAG_NUM);
                _tagmap[p] = (int)_taglist.size();
                _taglist.push_back(p);
                vec->push_back(id);
                _tagvec.push_back(vec);
            }
        } 
        ++_svrnum;
        
        return 0;
    }

	int ConnectManager :: getConnectType(){
		return _conn_type;
	}

	int ConnectManager :: setStrategy(Strategy * stt){
		if(NULL == stt){
			return -1;
		}
		_stt = stt;
		_stt->setManager(this);
		return 0;
	}

	int ConnectManager :: setHealthyChecker( HealthyChecker * hchk){
		if(NULL == hchk){
			return -1;
		}
		_hchk = hchk;
		_hchk->setManager(this);
		return 0;
	}

	int ConnectManager :: setLogAdapter(BasicLogAdapter * logger){
		if(NULL == logger){
			return -1;
		}
		_log = logger;
		return 0;
	}

	BasicLogAdapter * ConnectManager :: getLogAdapter(){
		return _log;
	}

	BasicLogAdapter * ConnectManager :: defaultLogAdapter(){
		return &_defaultLogAdapter;
	}


    Connection * ConnectManager :: fetchConnection(int key, int waittime) {
        return fetchConnection(key, waittime, NULL);
    }

	Connection * ConnectManager :: fetchConnection(int key, int waittime, int *err){
		mstime_t reqtime = ms_time();
		ConnectionRequest req;
		req.key = key;
		req.nthRetry = 0;
		req.serverID = -1;
		req.err = 0;
		req.conn = NULL;
		do{
			while(req.nthRetry < _defaultRetryTime){
				_rwlock.read();	
				int svrid = _stt->selectServer(&req);
				_rwlock.unlock();
				if(svrid < 0 || svrid >= _svrnum){
					_log->warning("[%s:%d]fetch Connection failed", __FILE__, __LINE__);
                    if (err) {
                        *err = REQ_SVR_SELECT_FAIL; 
                    }
					return NULL;
				}
				_log->debug("[%s:%d] Selected Server %d : %s : %d",
                        __FILE__, __LINE__,
                        svrid, _svr[svrid]->getIP(), _svr[svrid]->getPort());
				req.serverID = svrid;
				req.conn = _svr[svrid]->fetchConnection(&req.err);
				if(REQ_OK == req.err){
					_log->debug("[%s:%d] Successful on server %d : %s : %d",
                            __FILE__, __LINE__,
                            svrid, _svr[svrid]->getIP(), _svr[svrid]->getPort());
					return req.conn;
				}
				_log->debug("[%s:%d] Request faied on server %d : %s : %d",
                        __FILE__, __LINE__,
                        svrid, _svr[svrid]->getIP(), _svr[svrid]->getPort());
				++ req.nthRetry;
			}
			if(waittime > 100){
				ms_sleep(100);
			}
		}while(ms_time() < reqtime + waittime);
		_log->warning("[%s:%d]fetch Connection failed.. already retry %d times", __FILE__, __LINE__, _defaultRetryTime);
        if (err != NULL) {
            *err = req.err;
        }
		return NULL;
	}

	int ConnectManager :: freeConnection(Connection * c){
		if(NULL == c){
			return -1;
		}
		int svrid = c->serverID();
		if(svrid < 0 || svrid > _svrnum){
			_log->warning("[%s:%d] free an error Connection",
                    __FILE__, __LINE__);
			return -1;
		}
		AutoRead auto_lock(&_rwlock);
		_stt->notify(c, CONN_FREE);
		_svr[svrid]->freeConnection(c);
		return 0;
	}

	Connection * ConnectManager :: borrowConnection(){
		Connection * p = NULL;
		int id = rand() % CONN_LIST_NUM;
		AutoLock _lock(&_list_lock[id]);
		if(_conn_list[id].size() != 0){
			p = _conn_list[id].front();
			_conn_list[id].pop_front();
		}
		else{
            p= conn_creater();
            if (p != NULL) {
                    p->_log = getLogAdapter();
                    p->_id_in_cm = id;
            } else {
                _log->warning("[%s:%d] Connection creater fail",
                        __FILE__, __LINE__);
            }
		}
		return p;
	}

	void ConnectManager :: returnConnection(Connection * c){
		c->clear();
		int id = c->_id_in_cm;
		AutoLock _lock(&_list_lock[id]);
		_conn_list[id].push_back(c);
		return ;
	}

	int ConnectManager :: getServerSize() { 
		AutoRead auto_lock(&_rwlock);
		return _svrnum;
	}

	Server * ConnectManager :: getServerInfo(int id) {
		AutoRead auto_lock(&_rwlock);
		if(id < 0 || id >= _svrnum){
			return NULL;
		}
		return _svr[id];
	}

	int ConnectManager :: getTagSize(){
		AutoRead auto_lock(&_rwlock);
		return (int)_taglist.size();
	}

	const char * ConnectManager :: getTag(int id){
		AutoRead auto_lock(&_rwlock);
		if(id < 0 || id >= (int)_taglist.size()){
			return NULL;
		}
		return _taglist[id].c_str();
	}

	const int * ConnectManager :: getServerByTag(const char * tag, int *siz){
        if(NULL == siz) {
            return NULL;
        }
		if(NULL == tag){
			*siz = 0;
			return NULL;
		}
		AutoRead auto_lock(&_rwlock);
		str_t p = str_t(tag);
		if(_tagmap.find(p) != _tagmap.end()){
			int i = _tagmap[p];
			*siz = _tagvec[i]->size();
			return &( (*_tagvec[i])[0] );
		}
		return NULL;
	}

    int ConnectManager :: getServerByTagDeep(const char * tag, std::vector<int> &server_list) {
		if(NULL == tag){
			return -1;
		}
        int list_size = 0;
		AutoRead auto_lock(&_rwlock);
		str_t p = str_t(tag);
		if(_tagmap.find(p) != _tagmap.end()){
			int i = _tagmap[p];
			list_size = _tagvec[i]->size();
			server_list = (*_tagvec[i]);
            return list_size;
		}
		return -1;
    }

	time_t ConnectManager :: lastReload(){ 
		AutoRead auto_lock(&_rwlock);
		return _lastReloadTime;
	}

	int ConnectManager :: setCreater(connection_creater_t a, server_creater_t b){
		conn_creater = a;
		svr_creater = b;
		return 0;
	}

    int ConnectManager :: setRetryTime(int retry_time)
    {
       _defaultRetryTime = retry_time;
       return 0;
    }
    
    int ConnectManager :: getRetryTime()
    {
        return _defaultRetryTime;
    }

    int ConnectManager :: getRealServerSize()
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

    int ConnectManager :: getHealthyServerSize()
    {
        comcm::AutoRead auto_lock(&_rwlock);
        int real_num = 0;
        for (int i=0; i<_svrnum; i++) {
            if (_svr[i]->isEnable() && _svr[i]->isHealthy()) {
                ++ real_num;
            }
        }
        return real_num;

    }
}












