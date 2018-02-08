
#include "socketpool.h"
#include "connectmgr_utils.h"
#include "ul_net.h"

SocketPool :: SocketPool(){
	_mgr = &_manager;
	_log = _manager.getLogAdapter();
	_def_stt = NULL;
	_def_checker = NULL;
	_stt = NULL;
	_checker = NULL;
    _retry_time = comcm::DEFAULT_RETRY_TIME; 
}

SocketPool :: ~SocketPool(){
	if(_def_stt){
		delete _def_stt;
	}
	if(_def_checker){
		delete _def_checker;
	}
}

int SocketPool :: init(const comcfg::ConfigUnit & conf){
	_mgr->setCreater(socket_conn_creater, socket_server_creater);
	if(NULL == _stt){
		_def_stt = new (std::nothrow) SocketDefaultStrategy;
        if (NULL == _def_stt) {
            goto fail;
        }
	}
	if(NULL == _checker){
		_def_checker = new (std::nothrow) SocketDefaultChecker;
        if (NULL == _def_checker) {
            goto fail;
        }
	}
    _mgr->setStrategy(_def_stt);
    _stt = _def_stt;
    _mgr->setHealthyChecker(_def_checker);
    _checker = _def_checker;
    
	_sockmap.create(1024);
	return _mgr->init(conf);

fail:
    if (_def_stt != NULL) {
        delete _def_stt;
        _def_stt = NULL;
    }

    if (_def_checker != NULL) {
        delete _def_checker;
        _def_checker = NULL;
    }
    return -1;
}

int SocketPool :: init(comcm::Server ** svrs, int svrnum, bool shortConnection){

	_mgr->setCreater(socket_conn_creater, socket_server_creater);
	if(NULL == _stt){
		_def_stt = new (std::nothrow) SocketDefaultStrategy;
        if (NULL == _def_stt) {
            goto fail;
        }
	}
	if(NULL == _checker){
		_def_checker = new (std::nothrow) SocketDefaultChecker;
        if (NULL == _def_checker) {
            goto fail;
        }
	}
    _mgr->setStrategy(_def_stt);
    _stt = _def_stt;
    _mgr->setHealthyChecker(_def_checker);
    _checker = _def_checker;
	
	_sockmap.create(1024);
	return _mgr->init(svrs, svrnum, shortConnection);

fail:
    if (_def_stt != NULL) {
        delete _def_stt;
        _def_stt = NULL;
    }

    if (_def_checker != NULL) {
        delete _def_checker;
        _def_checker = NULL;
    }
    return -1;

}

int SocketPool :: setStrategy(comcm::Strategy * stt){
	int ret = _mgr->setStrategy(stt);
	if(ret == 0){
		_stt = stt;
	}
	return ret;
}

int SocketPool :: setHealthyChecker(comcm::HealthyChecker * hchk){
	int ret =  _mgr->setHealthyChecker(hchk);
	if(ret == 0){
		_checker = hchk;
	}
	return ret;
}

int SocketPool :: reload(const comcfg::ConfigUnit & conf){
	_log->trace("[%s:%d] ----------------begin to reload-----------", __FILE__, __LINE__);
	int ret = _mgr->reload(conf);
    if (_stt) {
        _stt->update();
    }
    return ret;
}

int SocketPool :: reload(comcm::Server ** svrs, int svrnum){
	_log->trace("[%s:%d] ----------------begin to reload-----------", __FILE__, __LINE__);
    int ret = _mgr->reload(svrs, svrnum);
    if (_stt) {
        _stt->update();
    }
    return ret;
}

int SocketPool :: FetchSocket(int key, int waittime, int *err){
	comcm::Connection * p = _mgr->fetchConnection(key, waittime, err);
	if(NULL == p){
		return -1;
	}
    void *s = p->getHandle();
    if (NULL == s) {
        return -1;
    }
	int sock = *(int *)s;
	_sockmap.set(sock, p, 1);
	return sock;
}

int SocketPool :: FreeSocket(int sock, bool errclose){
	comcm::Connection * p = getConnectionBySock(sock);
	if(p == NULL){
		return -1;
	}
	_sockmap.set(sock, NULL, 1);
	if(errclose && p->getErrno() == 0){
		p->setErrno(-1);
	}
	_mgr->freeConnection(p);
	return 0;
}

comcm::Connection * SocketPool :: getConnectionBySock(int sock){
	comcm::Connection * p;
	if( _sockmap.get(sock, &p) != Lsc::HASH_EXIST){
		return NULL;
	}
	return p;
}

int SocketPool :: setLogAdapter(BasicLogAdapter * logger){
	return _mgr->setLogAdapter(logger);
}

BasicLogAdapter * SocketPool :: getLogAdapter(){
	return _mgr->getLogAdapter();
}

comcm::ConnectManager * SocketPool :: getManager(){
	return _mgr;
}

SocketConnection :: SocketConnection()
{
    _sock = -1;
}

void * SocketConnection :: getHandle(){
	return (void *)&_sock;
}

int SocketConnection :: connect(comcm::Server * svr){
	_log->debug("[%s:%d] Now connect to server : %s:%d",
            __FILE__, __LINE__, svr->getIP(), svr->getPort());
	_sock = ul_tcpconnecto_ms(const_cast<char *>(svr->getIP()), svr->getPort(), svr->getTimeout());
	if(_sock < 0){
		_log->warning("[%s:%d]Connect failed to server %s:%d: %m",
                __FILE__, __LINE__, svr->getIP(), svr->getPort());
		return -1;
	}
	int on = 1;
	if (setsockopt(_sock,IPPROTO_TCP,TCP_NODELAY,&on, sizeof(on)) < 0) {
		_log->warning("[%s:%d](setsockopt(fd,IPPROTO_TCP,TCP_NODELAY,&on, sizeof(on)) ERR(%m)", 
				__FILE__, __LINE__);
	}

	
	bzero(_peerip, sizeof(_peerip));
	strcpy(_peerip, svr->getIP());
	_peerport = svr->getPort();
	return 0;
}

int SocketConnection :: disconnect(){
	if(_sock >= 0){
		int tmp_sk = _sock;
		_sock = -1;
		close(tmp_sk);
	}
	return 0;
}

int SocketConnection :: ping(){
	if(_sock < 0){
		return -1;
	}
	char buf[1];
	ssize_t ret = recv(_sock, buf, sizeof(buf), MSG_DONTWAIT);
	if(ret>0) {
		_log->warning("[%s:%d]ping: some dirty data pending, error accur! sock[%d], ip[%s:%d]", 
				__FILE__, __LINE__, _sock, _peerip, _peerport);
		return -1;
	} else if (ret==0) {
		_log->warning("[%s:%d]ping: connection close by peer! sock[%d], ip[%s:%d]", 
				__FILE__, __LINE__, _sock, _peerip, _peerport);
		return -1;
	} else {
		if(errno == EWOULDBLOCK || errno == EAGAIN) {
			return 0;
		} else {
			_log->warning("[%s:%d]ping: read error[%d] on sock[%d], ip[%s:%d], ERR[%m]", 
					__FILE__, __LINE__, errno, _sock, _peerip, _peerport);
			return -1;
		}
	}
	return 0;
}

SocketDefaultStrategy :: SocketDefaultStrategy(){
	_last_reload = 0;
	_avail_num = 0;
	memset(_svr_status, 0, sizeof(_svr_status));
	srand(time(0) ^ pthread_self());
    memset(_avail_list, 0, sizeof(_avail_list));
}

int SocketDefaultStrategy :: update(){
	_log->debug("[%s:%d]update SocketDefaultStrategy", __FILE__, __LINE__);
	comcm::AutoWrite __(&_lock);
	int n = _mgr->getServerSize();
	_avail_num = 0;
	for(int i = 0; i < n; ++i){
		comcm::Server * svr = _mgr->getServerInfo(i);
		if(svr != NULL && svr->isEnable()){
			_avail_list[_avail_num++] = i;
			_svr_status[i] = DEFAULT_WEIGHT;
		}
	}
	_last_reload = _mgr->lastReload();
    return 0;
}


int SocketDefaultStrategy :: selectServer(const comcm::ConnectionRequest * req){
	_lock.read();
	if(_last_reload < _mgr->lastReload()){
		_lock.unlock();
		update();
		_lock.read();
	}
	int key = req->key;
	int retry = req->nthRetry;
	int n = _avail_num;
	int last = req->serverID;
	if(retry > 0){
		_svr_status[last] /= 2;
	}
    if (_avail_num <= 0) {
        return -1;
    }
	int ret = 0;
	if(key < 0){
		key = rand();
	}
	_log->debug("[%s:%d] Request : key=%d, retry=%d, serverSize = %d",
            __FILE__, __LINE__, key, retry, n);
	int wt = 0;
	for(int i = 0; i < _avail_num; ++i){
		wt += _svr_status[_avail_list[i]];
	}
	wt /= _avail_num;
	if(wt == 0) wt = 1;

#if 1
	if(retry % 2){
		ret = key + retry;
	}
	else{
		ret = key - retry ;
	}
	ret = (ret % n + n) % n;
#endif
	while(rand() % wt > _svr_status[_avail_list[ret]]){
		_log->debug("[%s:%d] SocketDefaultStrategy : wt = %d, svr=%d(weight=%d)",
                __FILE__, __LINE__,
                wt, _avail_list[ret], _svr_status[_avail_list[ret]]);
		ret = (ret + ((rand()%2)?retry:-retry)) % n;
		++retry;
		if(ret < 0){
			ret += n;
		}
	}
	_lock.unlock();
	return _avail_list[ret];
}

int SocketDefaultStrategy :: notify(comcm::Connection * conn, int eve){
	if(NULL == conn){
		return -1;
	}
	int err = 0;
	int id = -1;
	err = conn->getErrno();
	id = conn->serverID();
	switch(eve){
		case comcm::CONN_FREE:
			if(err == comcm::ERR_DEAD_LIKE){
				_log->trace("[%s:%d] Server[%d] is dead like...[Weight %d->%d]",
                        __FILE__, __LINE__, id,
						_svr_status[id], _svr_status[id]/2);
				comcm::AutoWrite __(&_lock);
				_svr_status[id] /= 2;
			}
		default:
			;
	}
	if(err == 0 && _svr_status[id] < DEFAULT_WEIGHT){
		_log->trace("[%s:%d] Server[%d] weight (%d) back to default(%d)",
                __FILE__, __LINE__,
                id, _svr_status[id], 
				DEFAULT_WEIGHT);
		comcm::AutoWrite __(&_lock);
		_svr_status[id] = DEFAULT_WEIGHT;
	}
	return 0;
}

SocketDefaultChecker :: SocketDefaultChecker(){
}

int SocketDefaultChecker :: healthyCheck(){
	int n = _mgr->getServerSize();
	int ret = 0;
	for(int i = 0; i < n; ++i){
		comcm::Server * svr = _mgr->getServerInfo(i);
		if(svr != NULL && svr->isEnable() && svr->isHealthy() == false){
			int err = 0;
			comcm::Connection * c = svr->fetchConnection(&err);
			if(c != NULL && err == 0){
				svr->setHealthy(true);
				_log->debug("[%s:%d] The unhealthy server [%d] is back to health.",
                        __FILE__, __LINE__, i);
				svr->freeConnection(c);
				++ret;
			}
		}
	}
	return ret;
}

int SocketPool :: setRetryTime(int retry_time)
{
    _retry_time = retry_time;
    if (_mgr) {
        _mgr->setRetryTime(retry_time);
    }
    return 0;
}

int SocketPool :: getRetryTime()
{
    return _retry_time;
}

int SocketPool :: init(const ConnectPool::server_conf_t serverConfs[], int serverCount,
                          int sockPerServer, int connectRetry, bool longConnect)
{
    if(serverConfs==NULL || serverCount<=0 || sockPerServer<=0 ) {
        return -1;
    }
    if(connectRetry < 0) {
        return -1;
    }

    int err = 0;

    comcm::Server ** svr;
    svr = new (std::nothrow) comcm::Server*[serverCount];
    if (NULL == svr) {
        _log->warning("[%s:%d]Creat server error..", __FILE__, __LINE__);
        return -1;
    }

    for (int i=0; i<serverCount; i++) {
        svr[i] = socket_server_creater();
        if (svr[i] != NULL) {
            
            
            svr[i]->setIP(serverConfs[i].addr);
            svr[i]->setPort(serverConfs[i].port);
            svr[i]->setCapacity(sockPerServer);
            svr[i]->setTimeout(serverConfs[i].ctimeout_ms);
        } else {
            _log->warning("[%s:%d]Creat server[%d] error..", __FILE__, __LINE__, i);
            err = -1;
        }
    }

    if (0 == err) {
        err = init(svr, serverCount, !longConnect);
    }

    setRetryTime(connectRetry);

    if (svr) {
        for (int i=0; i<serverCount; i++) {
            if (svr[i]) {
                delete svr[i];
            }
        }
        delete []svr;
    }
    svr = NULL;
    return err;
}

comcm::Server * socket_server_creater(){
    return new (std::nothrow)comcm::Server;
}

comcm::Connection * socket_conn_creater(){
	return new (std::nothrow)SocketConnection;
}











