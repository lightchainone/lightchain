
#include "msocketpool.h"

namespace comcm
{

const char *MServer::getIP2()
{
    comcm::AutoRead auto_lock(&_svrlock);
    return _ip2;
}


int MServer::setIP2(const char *ip)
{
    snprintf(_ip2, sizeof(_ip2), "%s", ip);
    _ip_int2 = comcm::ip_to_int(_ip);
    if (0 == _ip_int2) {
        return -1;
    }
    return 0;
}


bool MServer::equal(const comcm::Server & other)
{
    comcm::AutoRead auto_lock(&_svrlock);
    const MServer *moth = dynamic_cast<const MServer*>(&other);
    if (moth != NULL) {
        if (_ip_int == moth->_ip_int && _port == moth->_port 
            && _ip_int2 == moth->_ip_int2 && _port == moth->_port) {
            return true;
        }
    }
    return false;
}


int MServer::clone(const comcm::Server * oth)
{

    const MServer *moth = dynamic_cast<const MServer*>(oth);
	if (NULL == moth) {
		return -1;
	}
    comcm::AutoWrite auto_lock(&_svrlock);
	snprintf(_ip, sizeof(_ip), "%s", moth->_ip);
	_ip_int = moth->_ip_int;
	_port = moth->_port;
	_capacity = moth->_capacity;
	if(_capacity > (int)_conns.size()){
		_conns.resize(_capacity, NULL);
	}
	_ctimeout = moth->_ctimeout;
	_tag.clear();
	_tag = moth->_tag;
    _ip_int2 = moth->_ip_int2;
    snprintf(_ip2, sizeof(_ip2), "%s", moth->_ip2);
    _vaild_list[0] = moth->_vaild_list[0];
    _vaild_list[1] = moth->_vaild_list[1];
	return 0;
}


int MServer::init(const comcfg::ConfigUnit &conf)
{
    return initMBasicValue(conf);
}


int MServer :: initMBasicValue(const comcfg::ConfigUnit & conf){
	if(NULL == _mgr || NULL == _log){
		return -1;
	}
    _vaild_list[0] = false;
    _vaild_list[1] = false;
	_log->debug("[%s:%d]initMBasicValue for Server", __FILE__, __LINE__);
	if( conf[CM_IP].get_cstr(_ip, sizeof(_ip)) != 0 || (_ip_int = comcm::ip_to_int(_ip)) == 0){
		_log->warning("[%s:%d]initMBasicValue for Server error: no ip or ip format err",
				__FILE__, __LINE__);
		return -1;
	}
    _vaild_list[0] = true;
    _ip2[0] = '\0';
    if (conf["ip2"].get_cstr(_ip2, sizeof(_ip2)) == 0)
    {
        _ip_int2 = comcm::ip_to_int(_ip2);
        if (0 == _ip_int2 && _ip2[0] != '\0') {
            _log->warning("[%s:%d]initMBasicValue for Server error: ip2 format err",
                    __FILE__, __LINE__);
            return -1;
        }
        if (_ip_int2 != 0) {
            _vaild_list[1] = true;
        }
    }
	conf[CM_PORT].get_int32(&_port, 0); 
	conf[CM_CAPACITY].get_int32(&_capacity, 20);
	conf[CM_CTIMEOUT].get_int32(&_ctimeout, 400);
    
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
			_tag.push_back(comcm::str_t(tg));
		}
	}
	return 0;
}


MSocketConnection::MSocketConnection()
{
    _last_connect = -1;
}



int MSocketConnection::connect(comcm::Server *csvr)
{
    MServer *svr = dynamic_cast<MServer *>(csvr);
    _log->debug("[%s:%d] Now connect to server : %s:%d",
            __FILE__, __LINE__, csvr->getIP(), csvr->getPort());
    char *iplist[2];
    iplist[0] = const_cast<char *>(csvr->getIP());
    iplist[1] = NULL;

    if (svr) {
        iplist[1] = const_cast<char *>(svr->getIP2());
        for (int i = 0; i < 2; ++i) {
            if (!svr->isValid(i)) {
                iplist[i] = NULL;
            }
        }
    }

    for (int i = 0; i < (int)(sizeof(iplist)/sizeof(iplist[0])); ++i) {
        if (iplist[i] != NULL) { 
            _sock = ul_tcpconnecto_ms(iplist[i], svr->getPort(), svr->getTimeout());
            if (_sock < 0) {
                _log->warning("[%s:%d]Connect failed to server %s:%d: [id:%d] %m",
                        __FILE__, __LINE__, iplist[i], svr->getPort(), i); 
            } else {
                _last_connect = i;
                break;
            }
        }
    }

    if (_sock >= 0) {
        int on = 1;
        if (setsockopt(_sock,IPPROTO_TCP,TCP_NODELAY,&on, sizeof(on)) < 0) {
            _log->warning("[%s:%d](setsockopt(fd,IPPROTO_TCP,TCP_NODELAY,&on, sizeof(on)) ERR(%m)", 
                    __FILE__, __LINE__);
        }
        return 0;
    }
    return -1;

}

int MSocketConnection::getCurrentIP()
{
    return _last_connect;
}
 

}


int MSocketPool :: init(const comcfg::ConfigUnit & conf){
	_mgr->setCreater(msocket_conn_creater, msocket_server_creater);
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




int MSocketPool :: init(comcm::Server ** svrs, int svrnum, bool shortConnection){

	_mgr->setCreater(msocket_conn_creater, msocket_server_creater);
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


MSocketPool :: ~MSocketPool()
{
}





comcm::Server * msocket_server_creater(){
    return new (std::nothrow)comcm::MServer;
}


comcm::Connection * msocket_conn_creater(){
	return new (std::nothrow)comcm::MSocketConnection;
}



















