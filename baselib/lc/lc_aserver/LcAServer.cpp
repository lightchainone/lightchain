
#include "LcAServer.h"
#include "SvrEvent.h"
#include <ul_net.h>
#include <lc_server.h>
#include "event_log.h"
#include <Lsc/var/String.h>

namespace lc {

static char * lc_ip_string(struct in_addr inaddr, char *buf, u_int len)
{
	unsigned char *ip = (unsigned char *)&inaddr.s_addr;
	snprintf(buf, len, "%u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);
	return buf;
}

static void lcaserver_accept_callback(IEvent *ev, void *) {
	SvrEvent *se = static_cast<SvrEvent*>(ev);
	se->get_server()->accept_porcess(se);
}

void LcAServer :: accept_porcess(SvrEvent *se) {
    if (_monitor) {
        _monitor->add_request_connect_number(_svr_name.c_str());
    }

	int acc = 0;
	if (se->isError()) {
		if (se->status() == IEvent::CANCELED) {
			LCEVENT_WARNING(se,"LcAServer(accept_porcess) : "\
				"server(%s) stop.",
				se->get_server()->get_svrname().c_str());
			int h = se->retVal();
			if (h>=0) {
				::close(h);
			}
			h = se->handle();
			if (h>=0) {
				::close(h);
			}
			return;
		}
		int rst = se->repost();
		if (0 != rst) {
			LCEVENT_WARNING(se,"LcAServer(accept_porcess) : "\
					"post accept event error, server(%s) stop.",
					se->get_server()->get_svrname().c_str());
		}
		return;
	}
	char ipbuf[32];
	if (_fmax_sessionlimit >0 && (int)_fmax_sessionlimit < _fsession_count.get()) {
		LCEVENT_WARNING(se,"LcAServer(accept_porcess) : "\
			"event count(%u) exceeds limit(%u). drop link(%d:%s)",
			_fsession_count.get(),_fmax_sessionlimit,se->retVal(),
			lc_ip_string(se->get_sockaddr()->sin_addr, ipbuf, sizeof(ipbuf)));
		::close(se->retVal());
	} else {
		lc_auth_t * auth = se->get_server()->get_ipauth();
		if ((NULL != auth) && (lc_auth_ip_int(auth, se->get_sockaddr()->sin_addr.s_addr) == 0)) {
			LC_LOG_MONITOR("__E_AUTH__invalid ip[%s] accessed!",
					lc_ip_string(se->get_sockaddr()->sin_addr, ipbuf, sizeof(ipbuf)));
			::close(se->retVal());
		} else {
			se->set_sock_status(LcEvent::STATUS_ACCEPTED);
			asvr_lock_socket_t sk;
			sk.fd = se->retVal();
			sk.addr = *(se->get_sockaddr());
			_facc_queue.push(sk);
			acc =1;
		}
	}
	int ret = se->repost();
	if (0 != ret) {
		LCEVENT_WARNING(se,"LcAServer(accept_porcess) : "\
			"post accept event error, server(%s) stop.",
			_svr_name.c_str());
	}
	if (acc) {
		this->on_accept(se);
	}
}

const Lsc::string & LcAServer :: get_svrname() {
	return _svr_name;
}

NetReactor * LcAServer :: get_reactor(){
	return this->_fnet_act_list;
}


int LcAServer :: load(const comcfg :: ConfigUnit& cfg) {
	
	cfg["name"].get_Lsc_string(&_svr_name, (Lsc::string)"");
	LC_IN_NOTICE("servername:%s", _svr_name.c_str());
	cfg["port"].get_int32(&_fport, 11111);
	LC_IN_NOTICE("[server:%s] port:%d",_svr_name.c_str(), _fport);
	cfg["connecttype"].get_int32(&_fconnect_type, 0);
	LC_IN_NOTICE("[server:%s] connect_type:%d",_svr_name.c_str(), _fconnect_type);
	cfg["connecttimeout"].get_int32(&_fconnect_timeout, -1);
	LC_IN_NOTICE("[server:%s] connect_timeout:%d",_svr_name.c_str(), _fconnect_timeout);
	cfg["readtimeout"].get_int32(&_fread_timeout, -1);
	LC_IN_NOTICE("[server:%s] read_timeout:%d",_svr_name.c_str(), _fread_timeout);
	cfg["writetimeout"].get_int32(&_fwrite_timeout, -1);
	LC_IN_NOTICE("[server:%s] write_timeout:%d",_svr_name.c_str(), _fwrite_timeout);
	cfg["sessiontimeout"].get_int32(&_fsession_timeout, -1);
	LC_IN_NOTICE("[server:%s] session_timeout:%d",_svr_name.c_str(), _fsession_timeout);
	cfg["max_bufsize"].get_uint32(&(_fmax_buf_size), 1048576);
	LC_IN_NOTICE("[server:%s] max_buf_size:%u",_svr_name.c_str(), _fmax_buf_size);
	cfg["max_read_bufsize"].get_uint32(&(_fmax_readbuf_size), 0);
	LC_IN_NOTICE("[server:%s] max_readbuf_size:%u",_svr_name.c_str(), _fmax_readbuf_size);
	cfg["max_write_bufsize"].get_uint32(&(_fmax_writebuf_size), 0);
	LC_IN_NOTICE("[server:%s] max_writebuf_size:%u",_svr_name.c_str(), _fmax_writebuf_size);
	cfg["max_session_number"].get_uint32(&(_fmax_sessionlimit), 0);
	LC_IN_NOTICE("[server:%s] max_sessionlimit:%u",_svr_name.c_str(), _fmax_sessionlimit);
	LC_IN_DEBUG("load port: %d, connect type:%d", _fport, _fconnect_type);
	cfg["memorytype"].get_uint32(&_fmemory_type, 0);
	LC_IN_NOTICE("[server:%s] memorytype:%d",_svr_name.c_str(), _fmemory_type);
	cfg["memoryblocksize"].get_uint32(&_fmemory_blksize, 1048576);
	LC_IN_NOTICE("[server:%s] memoryblocksize:%d",_svr_name.c_str(), _fmemory_blksize);
	if (0 == _fmemory_type) {
		this->_fmemory_manager = new LcMallocMemManager();
	} else {
		_fmemory_manager = new LcThreadMemManager(_fmemory_blksize);
	}
	Lsc::string authfile;
	cfg["auth"].get_Lsc_string(&authfile, (Lsc::string)"");
	LC_IN_NOTICE("[server:%s] authfile:%s",_svr_name.c_str(), authfile.c_str());
	_ip_auth = NULL;
	if (authfile != "") {
		lc_auth_t *ip = lc_auth_create();
		if (ip != NULL) {
			if (lc_auth_load_ip(ip, authfile.c_str()) == 0) {
				lc_auth_compile(ip);
				_ip_auth = ip;
			} else {
				lc_auth_destroy(ip);
			}
		}
	}
	return 0;
}
int LcAServer :: set_connect_type(int type) {
	if (LCSVR_LONG_CONNECT != type && LCSVR_SHORT_CONNECT != type) {
		LC_IN_WARNING("LcAServer(set_connect_type) : invalid type");
		return -1;
	}
	_fconnect_type = type;
	return 0;
}
int LcAServer :: set_port(int port){
	if (0 > port) {
		LC_IN_WARNING("LcAServer(set_port) : port(%d)<0", port);
		return -1;
	}
	_fport = port;
	return 0;
}

void LcAServer :: session_begin(LcEvent * event) {
	if (NULL == event) {
		asvr_lock_socket_t dropsk;
		dropsk.fd = -1;
		LC_IN_WARNING("LcAServer(session_begin) : "
				"NULL session event, drop one request");
		_facc_queue.pop(&dropsk);
		return;
	}
	_fsession_count.incrementAndGet();
	event->set_tag(this) ;
	event->set_firstreactor(_fnet_act_list);
	event->set_memmgr(_fmemory_manager);
	event->clear_notice_string();
	
	timeval tv;
	gettimeofday(&tv, NULL);
	LCEVENT_DEBUG(event,"[time: %ld, %ld]" ,tv.tv_sec, tv.tv_usec);
	event->get_sock_data()->timer.start = tv;
	



	
	if (0> event->handle()) {
		asvr_lock_socket_t sk;
		sk.fd = -1;
		int res = _facc_queue.pop(&sk);
		if (0 != res) {
			return;
		}
		event->setHandle(sk.fd);
		
		event->get_sock_data()->client_addr= sk.addr;
	}
	event->set_sock_opt(_fsock_opt);
	event->active_sock_opt();

	event->init(_fread_timeout, _fwrite_timeout, _fconnect_timeout, _fsession_timeout, _fmax_buf_size);
	event->set_read_maxbuf(_fmax_readbuf_size);
	event->set_write_maxbuf(_fmax_writebuf_size);
	event->activating_session();
}

int LcAServer :: session_done(LcEvent * session) {
	int ret =0;
	if (NULL == session) {
		LC_IN_WARNING("LcAServer(session_done) : NULL session event");
		return -1;
	}
	_fsession_count.decrementAndGet();
	if (session->isError()) {
		LCEVENT_WARNING(session,"LcAServer(session_done) : error session(fd:%d)", session->handle());
		if (0 <= session->handle()) {
			::close(session->handle());
		}
		session->setHandle(-1);
		session->session_done();
		return -1;
	}
	int sstp = session->session_done();
	if ((NULL != _facc_event) &&
			((LCSVR_LONG_CONNECT == sstp) || (0 > sstp && LCSVR_LONG_CONNECT == _fconnect_type))) {
		session_begin(session);
		LCEVENT_DEBUG(session,"LcAServer(session_done) : long connect, begin new session");
		ret = LCSVR_LONG_CONNECT;
	} else {
		ret = LCSVR_SHORT_CONNECT;
		if (0 <= session->handle()) {
			::close(session->handle());
		}
		session->setHandle(-1);
	}
	return ret;
}
int LcAServer :: getready(){
	if (NULL == _fnet_act_list) {
		LC_IN_WARNING("LcAServer(run) : NULL reactor");
		return -1;
	}
	int fd = ul_tcplisten(_fport, _fbacklog);
	if (fd < 0) {
		LC_IN_WARNING("can't start tcplisten at port[%d] backlog[%d] err[%m]", _fport, _fbacklog);
		return -1;
	}
	LC_IN_DEBUG("start acc:%d", fd);
	SvrEventPtr sep;
	sep->set_server(this);
	sep->accept(fd);
	sep->setHandle(fd);
	sep->setCallback(lcaserver_accept_callback, NULL);
	_facc_event = &sep;
	_facc_event->addRef();
	_facc_fd = fd;
	_fsession_count.set(0);

    if (_monitor) {
        _monitor->register_service(_svr_name.c_str(), _fnet_act_list->getThread() + _monitor_threadnum);
        register_monitor_callback();
        gen_monitor_info();
    }

	
    return _accept_act->post(&sep);
}

int LcAServer :: getready(int listenfd) {
	if (NULL == _fnet_act_list) {
		LC_IN_WARNING("LcAServer(run) : NULL reactor");
		return -1;
	}
	int fd = listenfd;
	if (fd < 0) {
		LC_IN_WARNING("error listenfd[%d]", fd);
		return -1;
	}
	LC_IN_DEBUG("start acc fd:%d", fd);
	SvrEventPtr sep;
	sep->set_server(this);
	sep->accept(fd);
	sep->setHandle(fd);
	sep->setCallback(lcaserver_accept_callback, NULL);
	_facc_event = &sep;
	_facc_event->addRef();
	_facc_fd = fd;
	_fsession_count.set(0);

    if (_monitor) {
        _monitor->register_service(_svr_name.c_str(), _fnet_act_list->getThread() + _monitor_threadnum);
        register_monitor_callback();
        gen_monitor_info();
    }

	
    return _accept_act->post(&sep);
}
int LcAServer :: stop(){
	int ret=0;

	LC_IN_WARNING("LcAServer(stop) :stop");

	if (NULL != _facc_event) {
		ret = _accept_act->cancel(_facc_event);
		LC_IN_DEBUG("close acc:%d", _facc_event->handle());
		_facc_event->release();
		_facc_event = NULL;
	}
	return ret;
}

int LcAServer :: restart() {
	LC_IN_DEBUG("restart");











	return getready();
}

unsigned int LcAServer :: get_max_writebufsize() {
	return _fmax_writebuf_size;
}
unsigned int LcAServer :: get_max_readbufsize() {
	return _fmax_readbuf_size;
}

LcAServer :: ~LcAServer() {
	if (_facc_event) {
		int fd = _facc_event->handle();
		if (0 <= fd) {
			::close(fd);
			_facc_event->setHandle(-1);
		}
		_facc_event->release();
		_facc_event = NULL;
	}



	if (NULL != _ip_auth) {
		lc_auth_destroy(_ip_auth);
		_ip_auth = NULL;
	}
	if (NULL != _fmemory_manager) {
		delete _fmemory_manager;
	}
}

int LcAServer :: is_running() {
	if (NULL == _facc_event) {
		return 0;
	}
	return 1;
}

Lsc::mempool * LcAServer :: get_memmgr() {
	return _fmemory_manager;
}

int LcAServer :: set_monitor_request_num_success()
{
    if (_monitor) {
        _monitor->add_request_number_success(_svr_name.c_str());
    }
    return 0;
}

int LcAServer :: set_monitor_request_num_fail()
{
    if (_monitor) {
        _monitor->add_request_number_failed(_svr_name.c_str());
    }
    return 0;
}

int LcAServer :: set_monitor_request_time(int req_time)
{
    if (_monitor) {
        _monitor->add_compute_time(_svr_name.c_str(), req_time);
    }

    return 0;
}

Lsc::var::IVar & LcAServer::lc_aserver_monitor_get_queue_size(Lsc::var::IVar & ,
                                                              Lsc::ResourcePool &rp)
{
    try {
        Lsc::var::IVar &output = rp.create<Lsc::var::Dict>();
        output["SOCKET_SIZE"] = rp.create<Lsc::var::Int64>(1);
        return output["SOCKET_SIZE"];
    } catch (Lsc::Exception &e) {
        LC_LOG_WARNING("catch Lsc exception:%s", e.all());
        return Lsc::var::Null::null;
    } catch (std::exception &e) {
        LC_LOG_WARNING("catch stl exception:%s", e.what());
        return Lsc::var::Null::null;
    } catch (...) {
        LC_LOG_WARNING("catch unknown exception");
        return Lsc::var::Null::null;
    }
}

Lsc::var::IVar &LcAServer::lc_aserver_monitor_get_thread_pool_usage(Lsc::var::IVar & ,
                                                                    Lsc::ResourcePool &rp)
{
    try {
        Lsc::var::IVar &output = rp.create<Lsc::var::Dict>();
        output["REQUEST_QUEUE_SIZE"] = rp.create<Lsc::var::Int64>(1);
        return output["REQUEST_QUEUE_SIZE"];
    } catch (Lsc::Exception &e) {
        LC_LOG_WARNING("catch Lsc exception:%s", e.all());
        return Lsc::var::Null::null;
    } catch (std::exception &e) {
        LC_LOG_WARNING("catch stl exception:%s", e.what());
        return Lsc::var::Null::null;
    } catch (...) {
        LC_LOG_WARNING("catch unknown exception");
        return Lsc::var::Null::null;
    }

}

int LcAServer :: register_monitor_callback()
{
    if (_monitor) {
        Lsc::string tmp_name = _svr_name;
        if (_monitor->register_monitor_item((tmp_name.append(".REQUEST_QUEUE_SIZE")).c_str(),
                lc_aserver_monitor_get_queue_size, (void *)this) != 0) {
            return -1;
        }
        tmp_name = _svr_name;
        if (_monitor->register_monitor_item((tmp_name.append(".THREAD_POOL_USAGE")).c_str(),
                lc_aserver_monitor_get_thread_pool_usage, (void *)this) != 0) {
            return -1;
        }
        return 0;
    } else {
        return -1;
    }
    return 0;
}

void LcAServer :: gen_monitor_info()
{
    
    _monitor_info = _monitor_rp.create<Lsc::var::Dict>();
    _monitor_info[_svr_name] = _monitor_rp.create<Lsc::var::Dict>();
    Lsc::var::IVar &serv_info = _monitor_info[_svr_name];
    serv_info["REQUEST_NUMBER"] = _monitor_rp.create<Lsc::var::String>("");
    serv_info["REQUEST_QUEUE_SIZE"] = _monitor_rp.create<Lsc::var::String>("");
    serv_info["THREAD_POOL_USAGE"] = _monitor_rp.create<Lsc::var::String>("");
    serv_info["REQUEST_CONNECT_NUM"] = _monitor_rp.create<Lsc::var::String>("");
    serv_info["COMPUTE_TIME"] = _monitor_rp.create<Lsc::var::String>("");
}

}
