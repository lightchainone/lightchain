#include "myclient_include.h"
#include "myclient_define.h"
#if 0

comcm::Connection** mysql_conn_creater(int n)
{

	return new MysqlConnection[n];
}


comcm::Server* mysql_server_creater(int n)
{
	return new MysqlServer[n];
}
#endif


comcm::Connection* mysql_conn_creater()
{
	MysqlConnection * p = new MysqlConnection();
	return p;
}


comcm::Server* mysql_server_creater()
{
	MysqlServer * p = new MysqlServer();
	return p;
}



MysqlPool::MysqlPool()
{
	_mgr = NULL;
	_def_stt = NULL;
	_def_checker = NULL;
}


MysqlPool::MysqlPool(unsigned plevel, unsigned sqlBufSize,
        myconn_fail_handler_t failHandler, bool forceInit)
{
    _mgr = NULL;
    _def_stt = NULL;
    _def_checker = NULL;
    this->_plevel = plevel;
    this->_sqlBufSize = sqlBufSize;
    this->_failHandler = failHandler;
    this->_forceInit = forceInit;
}


MysqlPool::~MysqlPool()
{
	if (_mgr)
	{
		delete _mgr;
		_mgr = NULL;
	}
	if (_def_stt)
	{
		delete _def_stt;
		_def_stt = NULL;
	}
	if (_def_checker)
	{
		delete _def_checker;
		_def_checker = NULL;
	}
}


int MysqlPool::setStrategy(comcm::Strategy * p)
{
	if (_mgr == NULL)
	{
		return -1;
	}
	int ret = _mgr->setStrategy(p);
	if (p)
	{
		_stt = p;
	}
	return ret;
}


int MysqlPool::setHealthyChecker(comcm::HealthyChecker * p)
{
	if (_mgr == NULL)
	{
		return -1;
	}
	int ret = _mgr->setHealthyChecker(p);
	if (p)
	{
		_checker = p;
	}
	return ret;
}


comcm::Strategy * MysqlPool::getStrategy()
{
	return _stt;
}


comcm::HealthyChecker * MysqlPool::getHealthyChecker()
{
	return _checker;
}


comcm::ConnectManager * MysqlPool::getManager()
{
	return _mgr;
}


int MysqlPool::init(MysqlServer * svrs, int svrnum, int threadCheckTime)
{
	if (_mgr == NULL)
	{
		_mgr = new comcm::ConnectManager;
		_def_checker = new MysqlDefaultChecker;
		_def_stt = new MysqlDefaultStrategy;
		_stt = _def_stt;
		_checker = _def_checker;
	}
	_mgr->setCreater(mysql_conn_creater, mysql_server_creater);
	
	_mgr->setHealthyChecker(_def_checker);
	_mgr->setStrategy(_def_stt);
	
	comcm::Server * svrs_points [MAX_SERVER_NUM];
	for (int i = 0; i < svrnum; i++)
	{
		svrs_points [i] = &svrs [i];
	}
	int ret = _mgr->init(svrs_points, svrnum, comcm::LONG_CONN);
	
	
	
	int n = (int) _mgr->getServerSize();
	for (int i = 0; i < n; ++i)
	{
		MysqlServer * svr = NULL;
		try
		{
			svr = dynamic_cast<MysqlServer *> (_mgr->getServerInfo(i));
		}
		catch (...)
		{
			
		}
		if (svr)
		{
			svr->init(_plevel, _sqlBufSize, _failHandler); 
			ret = svr->refreshConnection(false);
            
			if (ret < 0 && this->_forceInit == false)
			{
				
				return ret;
			}
		}
	}
	
    
    if (threadCheckTime > 0)
    {
        _def_checker->startSingleThread(threadCheckTime);
	}
	return 0;
}


MysqlConnection * MysqlPool::FetchConnection(const char * tag, int timeout_ms, int * err)
{
	comcm::ConnectionRequest req;
	*err = 0;
	req.tag = tag;
	req.err = success;
	
	req.key = -1;
	

	int id = _stt->selectServer(&req);
	if (id < 0)
	{
        
        
		*err = pool_getconnection_fail;
		return NULL;
	}
	req.conn = _mgr->getServerInfo(id)->fetchConnection(&req.err);
	if (req.err != 0)
	{
        
        
		*err = pool_getconnection_fail;
	}
	MysqlConnection * ret;
	try
	{
        
        
		ret = dynamic_cast<MysqlConnection *> (req.conn);
	}
	catch (...)
	{
		if (err)
		{
			*err = pool_getconnection_fail;
		}
		return NULL;
	}
	return ret;
}


int MysqlPool::FreeConnection(MysqlConnection * c, int err)
{
	if (c == NULL)
	{
		return -1;
	}
	if (0 != err)
	{
		c->setErrno(err);
	}
	else
	{
		c->setErrno(0);
	}
	_mgr->getServerInfo(c->serverID())->freeConnection(c);
	return 0;
}


int MysqlPool::addServer(MysqlServer * svr)
{
	if (NULL == svr)
	{
		return inputparam_error;
	}
	int ret = _mgr->addServer(svr);
	if (ret < 0)
	{
		ret = pool_init_fail;
	}
	return ret;
}



MysqlServer::MysqlServer()
{
	pthread_mutex_init(&_lock, NULL);
	keepRate = 0.2;
	maxFreeTime = 60;
	min_connection = max_connection = 0;
	read_timeout = 2000;
	write_timeout = 1000;
	connect_timeout = 1000;
	_last_ping = 0;
	_is_inited = false;
    _peak_using = 0;
	
	if (NULL != this->ip)
	{
		this->ip [0] = 0;
	}
	if (NULL != this->username)
	{
		this->username [0] = 0;
	}
	if (NULL != this->dbname)
	{
		this->dbname [0] = 0;
	}
	if (NULL != this->charset)
	{
		this->charset [0] = 0;
	}
	if (NULL != this->password)
	{
		this->password [0] = 0;
	}
	
}


MysqlServer::~MysqlServer()
{
	if (_conns.size() > 0 && _mgr)
	{
		for (int i = 0; i < (int) _conns.size(); ++i)
		{
			if (_conns [i])
			{
				_mgr->returnConnection(_conns [i]);
				_conns [i] = NULL;
			}
		}
		_conns.resize(0);
	}
}


comcm::Connection * MysqlServer::fetchConnection(int * err)
{
	comcm::AutoLock __(&_lock);
	if (_conns.size() == 0)
	{
        
        _log->warning("Server disable! conns.size is 0!");
		*err = comcm::REQ_SVR_DISABLE;
		return NULL;
	}
	for (int i = 0; i < (int) _conns.size(); ++i)
	{
		if (_conns [i] && _conns [i]->getKernelStatus() == comcm::CONN_IS_CONNECTED)
		{
			_conns [i]->setKernelStatus(comcm::CONN_IS_USING);
			*err = comcm::REQ_OK;
			--_is_connected;
			++_is_using;
			return _conns [i];
		}
	}
    
    _log->warning("no connection is is_connected! is_connected:%d, is_using:%d!", _is_connected, _is_using);
	*err = comcm::REQ_SVR_FULL;
	return NULL;
}


int MysqlServer::freeConnection(comcm::Connection * c)
{
	if (NULL == c)
	{
		return -1;
	}
	comcm::AutoLock __(&_lock);
	if (0 != c->getErrno())
	{
		c->setKernelStatus(comcm::CONN_BROKEN);
		--_is_using;
		++_is_broken;
	}
	else
	{
		c->setKernelStatus(comcm::CONN_IS_CONNECTED);
		--_is_using;
		++_is_connected;
	}
	return 0;
}


int MysqlServer::init(unsigned plevel, unsigned sqlBufSize,
		myconn_fail_handler_t failHandler)
{
	if (this->_is_inited)
	{
		return 0;
	}
	if (_mgr)
	{
		_conns.resize(max_connection, NULL);
		for (int i = 0; i < (int) _conns.size(); ++i)
		{
			_conns [i] = _mgr->borrowConnection();
			MysqlConnection * conn = dynamic_cast<MysqlConnection *> (_conns [i]);
			conn->setParam(plevel, sqlBufSize, failHandler);
		}
	}
	_is_using = _is_connected = _is_broken = _peak_using = 0;
	_not_used = (int) _conns.size();
	this->_is_inited = true;
	return 0;
}


int MysqlServer::clone(const comcm::Server * oth)
{
	if (oth)
	{
		Server::clone(oth);
		const MysqlServer * o = dynamic_cast<const MysqlServer *> (oth);
		snprintf(ip, sizeof(ip), "%s", o->ip);
		port = o->port;
		read_timeout = o->read_timeout;
		write_timeout = o->write_timeout;
		connect_timeout = o->connect_timeout;
		min_connection = o->min_connection;
		max_connection = o->max_connection;
		snprintf(username, sizeof(username), "%s", o->username);
		snprintf(password, sizeof(password), "%s", o->password);
		snprintf(dbname, sizeof(dbname), "%s", o->dbname);
		snprintf(charset, sizeof(charset), "%s", o->charset);
	}
	else
	{
		return inputparam_error;
	}
	return 0;
}


bool MysqlServer::equal(const comcm::Server & other)
{
	const MysqlServer& oth = dynamic_cast<const MysqlServer&> (other);
	if (NULL == this->ip || NULL == this->dbname || NULL == oth.ip || NULL == oth.dbname
			|| NULL == oth.username || NULL == this->username)
	{
		return false;
	}
	comcm::AutoRead auto_lock(&_svrlock);
	if (this->port == oth.port && 0 == strcmp(this->ip, oth.ip) && 0 == strcmp(
			this->dbname, oth.dbname) && 0 == strcmp(this->username, oth.username))
	{
		return true;
	}
	return false;
}

bool MysqlServer::has_free_connection()
{
	bool has = true;
	pthread_mutex_lock(&_lock);
	if (_is_connected <= 0)
	{
		has = false;
	}
	pthread_mutex_unlock(&_lock);
	return has;
}


int MysqlServer::refreshConnection(bool need_ping)
{
	int ret = 0;
	if (_conns.size() == 0)
	{
		init();
	}
	_log->debug("start to refresh mysql server  %s:%d", ip, port);
	std::vector<int> broken_list;
	
	if (need_ping)
	{
		_log->debug("need ping  %s:%d", ip, port);
		int _ping_id = (_last_ping++) % (int) _conns.size();
		pthread_mutex_lock(&_lock);
		if (_conns [_ping_id] && _conns [_ping_id]->getKernelStatus()
				== comcm::CONN_IS_CONNECTED)
	 	{
			_conns [_ping_id] -> setKernelStatus(comcm::CONN_IS_USING);
			
			--_is_connected;
			++_is_using;
			
			pthread_mutex_unlock(&_lock);
			int ping_ret = _conns [_ping_id] -> ping();
			pthread_mutex_lock(&_lock);
			if (ping_ret == 0)
			{ 
				
				_conns [_ping_id] -> setKernelStatus(comcm::CONN_IS_CONNECTED);
				
				++_is_connected;
				--_is_using;
			    
			}
			else
			{
                
                _log->warning("Healthy check found some connetion broken!");
				_conns [_ping_id] -> setKernelStatus(comcm::CONN_BROKEN);
				
				--_is_using;
				++_is_broken;
			}
 		}
		pthread_mutex_unlock(&_lock);
 	}
	
	int conn_num;
	int conn_need;
	pthread_mutex_lock(&_lock);
	conn_num = _is_using + _is_connected;
	conn_need = (int) ((dolcle) _peak_using / (1.0 - keepRate) + 0.9999);
    _log->trace("The conn_need count is %d,\n the peak_using count is %d, the conn_num is %d,\
the is_using is %d, the is_connected is %d\n", conn_need, _peak_using, conn_num, _is_using, _is_connected);
    _peak_using = 0;
	if (conn_need < min_connection)
	{
		conn_need = min_connection;
	}
	if (conn_need > max_connection)
	{
		conn_need = max_connection;
	}
	if (_is_broken>0)
	{
		for (int i = 0; i < (int) _conns.size(); ++i)
		{
			if (_conns [i] && _conns [i]->getKernelStatus() == comcm::CONN_BROKEN)
			{
				
				_conns [i]->setKernelStatus(comcm::CONN_NOT_USED);
				broken_list.push_back(i);
				
				_not_used++;
				
			}
		}
		_is_broken = 0;
	}
	pthread_mutex_unlock(&_lock);
	
	for (int i = 0; i < (int) broken_list.size(); ++i)
	{
		
		_conns [broken_list [i]] -> setErrno(0);
		
		_conns [broken_list [i]] -> disconnect();
	}
	
	if (conn_need > conn_num)
 	{
		int need_connect = conn_need - conn_num;
		std::vector<int> cur_conn;
		std::vector<int> conn_res;
		
		pthread_mutex_lock(&_lock);
		for (int i = 0; i < (int) _conns.size() && need_connect > 0; ++i)
		{
			if (_conns [i] && _conns [i]->getKernelStatus() == comcm::CONN_NOT_USED)
			{
				cur_conn.push_back(i);
				--need_connect;
			}
		}
		pthread_mutex_unlock(&_lock);
		
		for (int i = 0; i < (int) cur_conn.size(); ++i)
		{
			
			ret = 0;
            
			ret = (_conns [cur_conn [i]])->connect(this);
			if (ret < 0)
			{
                
                for (int j = i; j >= 0; --j)
                {
                    _conns [cur_conn [j]]->disconnect();
                }
				return ret;
			}
			else
			{
				conn_res.push_back(ret);
			}
			
			
		}
		
		comcm::AutoLock __(&_lock);
		for (int i = 0; i < (int) cur_conn.size(); ++i)
		{
			if (conn_res [i] == 0)
			{
				_conns [cur_conn [i]]->setKernelStatus(comcm::CONN_IS_CONNECTED);
				++_is_connected;
				
				--_not_used;
				
			}
		}
	}
	else if (conn_need < conn_num)
	{
		
		int need_disconnect = conn_num - conn_need;
		std::vector<int> cur_disconn;
		
		pthread_mutex_lock(&_lock);
		for (int i = (int) _conns.size() - 1; i >= 0 && need_disconnect > 0; --i)
		{
			
			
			if (_conns [i] && _conns [i]->getKernelStatus() == comcm::CONN_IS_CONNECTED)
			{
				_conns [i]->setKernelStatus(comcm::CONN_NOT_USED);
				cur_disconn.push_back(i);
				--need_disconnect;
				
				--_is_connected;
				++_not_used;
				
			}
		}
		pthread_mutex_unlock(&_lock);
		
		for (int i = 0; i < (int) cur_disconn.size(); ++i)
		{
			_conns [cur_disconn [i]]->disconnect();
		}
 	}
	return 0;
}


int MysqlServer::get_is_using()
{
    return this->_is_using;
}


int MysqlServer::get_peak_using()
{
    return this->_peak_using;
}


int MysqlServer::set_peak_using(int peak_using)
{
    this->_peak_using = peak_using;
    return 0;
}



int MysqlDefaultChecker::healthyCheck()
{
	if (_mgr)
	{
		_log->debug("Start healthy check for mysql servers");
		int n = (int) _mgr->getServerSize();
		for (int i = 0; i < n; ++i)
		{
			MysqlServer * svr = NULL;
			try
			{
				svr = dynamic_cast<MysqlServer *> (_mgr->getServerInfo(i));
			}
			catch (...)
			{
				_log->warning(
						" MysqlDefaultChecker::healthyCheck():  dynamic_cast<MysqlServer *> fail!!!");
			}
			if (svr)
			{
				svr->refreshConnection();
			}
		}
 	}
	return 0;
}


MysqlDefaultStrategy::MysqlDefaultStrategy()
{
	
}


MysqlDefaultStrategy::~MysqlDefaultStrategy()
{
	_log->debug("delete MysqlDefaultStrategy ");
}


int MysqlDefaultStrategy::selectServer(const comcm::ConnectionRequest * req)
{
	if (_mgr == NULL || req == NULL)
	{
        _log->warning("Invalid input params!");
		return -1;
	}
	const comcm::ConnectionRequest * r = req;
	int key = r->key;
	if (key < 0)
	{
		key = rand();
	}
	if (r->tag == NULL)
	{
		return key % _mgr->getServerSize();
	}
	int sz = 0;
	const int * svr = _mgr->getServerByTag(r->tag, &sz);
	if (svr == NULL || sz == 0)
	{
		_log->warning("No server of tag [%s]", r->tag);
		return -1;
	}
	
	int index = key % sz;
	for (int i = 0; i < sz; i++)
	{
		
	    MysqlServer * temp_server = (MysqlServer *) _mgr->getServerInfo(svr [index]);
        
        int is_using = temp_server->get_is_using();
        int peak_using = temp_server->get_peak_using();
        if (peak_using < is_using)
        {
            temp_server->set_peak_using(is_using);
        }
		if (temp_server->has_free_connection())
		{
			return svr [index];
		}
		else
		{
            
            
			index = (index + 1) % sz;
		}
	}
    _log->warning("No available server!");
	return -1;
	
	
}



MyclientPool::MyclientPool()
{
	this->myclient_conf.monitor_reconnection_time = DEFAULT_MONITOR_TIME;
	this->myclient_conf.plevel = 0;
	this->myclient_conf.sqlBufSize = DEFAULT_QUERYF_BUFFER_SIZE;
	this->myclient_conf.failHandler = NULL;
	this->is_init_ok = false;
}


MyclientPool::MyclientPool(MyclientPoolConf * conf)
{
	if (NULL != conf)
	{
		this->myclient_conf.monitor_reconnection_time = conf->monitor_reconnection_time;
		this->myclient_conf.plevel = conf->plevel;
		this->myclient_conf.sqlBufSize = conf->sqlBufSize;
		this->myclient_conf.failHandler = conf->failHandler;
	}
	else
	{
		this->myclient_conf.monitor_reconnection_time = DEFAULT_MONITOR_TIME;
		this->myclient_conf.plevel = 0;
		this->myclient_conf.sqlBufSize = 0;
		this->myclient_conf.failHandler = NULL;
	}
	this->is_init_ok = false;
}


MyclientPool::~MyclientPool()
{
	this->is_init_ok = false;
	if (this->mysql_pool)
	{
		delete this->mysql_pool;
	}
}

int MyclientPool::init(bool forceInit)
{
	this->mysql_pool = new MysqlPool(this->myclient_conf.plevel,
			this->myclient_conf.sqlBufSize, this->myclient_conf.failHandler, forceInit);
	if (NULL == this->mysql_pool)
	{
		return pool_init_fail;
	}
	return 0;
}


int MyclientPool::addServer(const char * config_path, const char * config_file)
{
	int ret = 0;
	int err = 0;
	struct stat st;
	comcfg::Configure conf_loader;
	MyclientServerConf * pool_conf = NULL;
	unsigned int server_num = 0;
	if (config_path == NULL || config_file == NULL)
	{
		ret = inputparam_error;
		goto out;
	}
	
	if (!(stat(config_path, &st) == 0 && (st.st_mode & S_IFDIR)))
	{
		ret = inputparam_error;
		goto out;
	}
	
	err = conf_loader.load(config_path, config_file);
	if (err != 0)
	{
		ret = filesystem_error;
		goto out;
	}
	server_num = conf_loader ["server_num"].to_uint32(&err);
	if (err != 0 || 0 == server_num)
	{
		ret = confparam_error;
		goto out;
	}

	pool_conf = new MyclientServerConf [server_num];
	for (unsigned int i = 0; i < server_num; i++)
	{
		char temp_name [MAX_CONF_NAME];
		snprintf(temp_name, sizeof(temp_name), "%s%u%s", "server", i, "_charset");
		snprintf(pool_conf [i].charset, sizeof(pool_conf [i].charset), "%s",
				conf_loader [temp_name].to_cstr(&err));
		if (err != 0)
		{
			ret = confparam_error;
			goto out;
		}
		snprintf(temp_name, sizeof(temp_name), "%s%u%s", "server", i, "_tag");
		snprintf(pool_conf [i].tag, sizeof(pool_conf [i].tag), "%s",
				conf_loader [temp_name].to_cstr(&err));
		if (err != 0)
		{
			ret = confparam_error;
			goto out;
		}
		snprintf(temp_name, sizeof(temp_name), "%s%u%s", "server", i, "_dbname");
		snprintf(pool_conf [i].dbname, sizeof(pool_conf [i].dbname), "%s",
				conf_loader [temp_name].to_cstr(&err));
		if (err != 0)
		{
			ret = confparam_error;
			goto out;
		}
		snprintf(temp_name, sizeof(temp_name), "%s%u%s", "server", i, "_username");
		snprintf(pool_conf [i].username, sizeof(pool_conf [i].username), "%s",
				conf_loader [temp_name].to_cstr(&err));
		if (err != 0)
		{
			ret = confparam_error;
			goto out;
		}
		snprintf(temp_name, sizeof(temp_name), "%s%u%s", "server", i, "_password");
		snprintf(pool_conf [i].password, sizeof(pool_conf [i].password), "%s",
				conf_loader [temp_name].to_cstr(&err));
		if (err != 0)
		{
			ret = confparam_error;
			goto out;
		}
		snprintf(temp_name, sizeof(temp_name), "%s%u%s", "server", i, "_ip");
		snprintf(pool_conf [i].ip, sizeof(pool_conf [i].ip), "%s",
				conf_loader [temp_name].to_cstr(&err));
		if (err != 0)
		{
			ret = confparam_error;
			goto out;
		}
		snprintf(temp_name, sizeof(temp_name), "%s%u%s", "server", i, "_port");
		pool_conf [i].port = conf_loader [temp_name].to_int32(&err);
		if (err != 0)
		{
			ret = confparam_error;
			goto out;
		}
		snprintf(temp_name, sizeof(temp_name), "%s%u%s", "server", i, "_read_timeout");
		pool_conf [i].read_timeout = conf_loader [temp_name].to_int32(&err);
		if (err != 0)
		{
			ret = confparam_error;
			goto out;
		}
		snprintf(temp_name, sizeof(temp_name), "%s%u%s", "server", i, "_write_timeout");
		pool_conf [i].write_timeout = conf_loader [temp_name].to_int32(&err);
		if (err != 0)
		{
			ret = confparam_error;
			goto out;
		}
		snprintf(temp_name, sizeof(temp_name), "%s%u%s", "server", i, "_connect_timeout");
		pool_conf [i].connect_timeout = conf_loader [temp_name].to_int32(&err);
		if (err != 0)
		{
			ret = confparam_error;
			goto out;
		}
		snprintf(temp_name, sizeof(temp_name), "%s%u%s", "server", i, "_min_connection");
		pool_conf [i].min_connection = conf_loader [temp_name].to_int32(&err);
		if (err != 0)
		{
			ret = confparam_error;
			goto out;
		}
		snprintf(temp_name, sizeof(temp_name), "%s%u%s", "server", i, "_max_connection");
		pool_conf [i].max_connection = conf_loader [temp_name].to_int32(&err);
		if (err != 0)
		{
			ret = confparam_error;
			goto out;
		}
		if (pool_conf [i].max_connection <= 0 || pool_conf [i].min_connection <= 0
				|| pool_conf [i].connect_timeout <= 0 || pool_conf [i].write_timeout <= 0
				|| pool_conf [i].read_timeout <= 0 || pool_conf [i].connect_timeout <= 0
				|| pool_conf [i].max_connection < pool_conf [i].min_connection)
		{
			ret = confparam_error;
			goto out;
		}

	}
	ret = this->addServer(pool_conf, server_num);
	delete [] pool_conf;
	pool_conf = NULL;
	out: if (NULL != pool_conf)
	{
		delete [] pool_conf;
		pool_conf = NULL;
	}
	return ret;
}


int MyclientPool::addServer(MyclientServerConf * conf, unsigned int num)
{
	int ret = 0;
	if (num <= 0 || num > MAX_SERVER_NUM || NULL == conf)
	{
		return inputparam_error;
	}
	MysqlServer * svr = new MysqlServer [num];
	for (unsigned int i = 0; i < num; i++)
	{
		snprintf(svr [i].ip, sizeof(svr [i].ip), "%s", conf [i].ip);
		svr [i].port = conf [i].port;
		snprintf(svr [i].username, sizeof(svr [i].username), "%s", conf [i].username);
		snprintf(svr [i].password, sizeof(svr [i].password), "%s", conf [i].password);
		snprintf(svr [i].dbname, sizeof(svr [i].dbname), "%s", conf [i].dbname);
		snprintf(svr [i].charset, sizeof(svr [i].charset), "%s", conf [i].charset);
		svr [i].min_connection = conf [i].min_connection;
		svr [i].max_connection = conf [i].max_connection;
		svr [i].connect_timeout = conf [i].connect_timeout;
		svr [i].write_timeout = conf [i].write_timeout;
		svr [i].read_timeout = conf [i].read_timeout;
		ret = svr [i].setTag(conf [i].tag);
		if (ret < 0)
		{
			ret = inputparam_error;
			goto out;
		}
	}
	ret = this->mysql_pool->init(svr, num, this->myclient_conf.monitor_reconnection_time);
	if (ret != 0)
	{
		ret = pool_init_fail;
		goto out;
	}
	delete [] svr;
	svr = NULL;
	this->is_init_ok = true;
	out: if (svr != NULL)
	{
		delete [] svr;
		svr = NULL;
	}
	return ret;
}


MysqlConnection * MyclientPool::fetchConnection(const char * tag, int * err_no,
		int timeout_startegy)
{
	int ret = 0;
	if (NULL == err_no)
	{
		return NULL;
	}
	(*err_no) = success;
	if (NULL == tag)
	{
		(*err_no) = inputparam_error;
		return NULL;
	}
	if (false == this->is_init_ok)
	{
		(*err_no) = pool_init_fail;
        
	}
	MysqlConnection * mysql_connection = this->mysql_pool->FetchConnection(tag,
			timeout_startegy, &ret);
	if (ret != 0)
	{
		(*err_no) = pool_getconnection_fail;
		
		if (0 == strcmp(tag, MYCLIENT_SLAVE_LOCAL_HOST_TYPE))
		{
			ret = 0;
			mysql_connection = this->mysql_pool->FetchConnection(
					MYCLIENT_SLAVE_REMOTE_HOST_TYPE, timeout_startegy, &ret);
			if (0 == ret)
			{
				(*err_no) = success;
				return mysql_connection;
			}
		}
		return NULL;
	}
	return mysql_connection;
}


int MyclientPool::putBackConnection(MysqlConnection * connection, bool is_need_reconnect)
{
	int ret = 0;
	if (is_need_reconnect)
	{
		ret = this->mysql_pool->FreeConnection(connection, -1);
	}
	else
	{
		ret = this->mysql_pool->FreeConnection(connection);
	}
	return ret;
}


const char * MyclientPool::getError(int err_no)
{
	return mc_get_err_dest(err_no);
}


MysqlPool * MyclientPool::__get_mysql_pool()
{
	return this->mysql_pool;
}


int MyclientPool::destroy()
{
	if (false == this->is_init_ok)
	{
		return pool_has_not_init;
	}
	if (NULL != this->mysql_pool)
	{
		comcm::HealthyChecker * health_check = this->mysql_pool->getHealthyChecker();
		if (NULL != health_check)
		{
			health_check->joinThread();
		}
	}
	return success;
}


int MyclientPool::addNewServer(const char * config_path, const char * config_file)
{
	int ret = 0;
	int err = 0;
	struct stat st;
	comcfg::Configure conf_loader;
	MyclientServerConf * pool_conf = NULL;
	unsigned int server_num = 0;
	if (config_path == NULL || config_file == NULL)
	{
		ret = inputparam_error;
		goto out;
	}
	
	if (!(stat(config_path, &st) == 0 && (st.st_mode & S_IFDIR)))
	{
		ret = inputparam_error;
		goto out;
	}
	
	err = conf_loader.load(config_path, config_file);
	if (err != 0)
	{
		ret = filesystem_error;
		goto out;
	}
	server_num = conf_loader ["server_num"].to_uint32(&err);
	if (err != 0 || 0 == server_num)
	{
		ret = confparam_error;
		goto out;
	}

	pool_conf = new MyclientServerConf [server_num];
	for (unsigned int i = 0; i < server_num; i++)
	{
		char temp_name [MAX_CONF_NAME];
		snprintf(temp_name, sizeof(temp_name), "%s%u%s", "server", i, "_charset");
		snprintf(pool_conf [i].charset, sizeof(pool_conf [i].charset), "%s",
				conf_loader [temp_name].to_cstr(&err));
		if (err != 0)
		{
			ret = confparam_error;
			goto out;
		}
		snprintf(temp_name, sizeof(temp_name), "%s%u%s", "server", i, "_tag");
		snprintf(pool_conf [i].tag, sizeof(pool_conf [i].tag), "%s",
				conf_loader [temp_name].to_cstr(&err));
		if (err != 0)
		{
			ret = confparam_error;
			goto out;
		}
		snprintf(temp_name, sizeof(temp_name), "%s%u%s", "server", i, "_dbname");
		snprintf(pool_conf [i].dbname, sizeof(pool_conf [i].dbname), "%s",
				conf_loader [temp_name].to_cstr(&err));
		if (err != 0)
		{
			ret = confparam_error;
			goto out;
		}
		snprintf(temp_name, sizeof(temp_name), "%s%u%s", "server", i, "_username");
		snprintf(pool_conf [i].username, sizeof(pool_conf [i].username), "%s",
				conf_loader [temp_name].to_cstr(&err));
		if (err != 0)
		{
			ret = confparam_error;
			goto out;
		}
		snprintf(temp_name, sizeof(temp_name), "%s%u%s", "server", i, "_password");
		snprintf(pool_conf [i].password, sizeof(pool_conf [i].password), "%s",
				conf_loader [temp_name].to_cstr(&err));
		if (err != 0)
		{
			ret = confparam_error;
			goto out;
		}
		snprintf(temp_name, sizeof(temp_name), "%s%u%s", "server", i, "_ip");
		snprintf(pool_conf [i].ip, sizeof(pool_conf [i].ip), "%s",
				conf_loader [temp_name].to_cstr(&err));
		if (err != 0)
		{
			ret = confparam_error;
			goto out;
		}
		snprintf(temp_name, sizeof(temp_name), "%s%u%s", "server", i, "_port");
		pool_conf [i].port = conf_loader [temp_name].to_int32(&err);
		if (err != 0)
		{
			ret = confparam_error;
			goto out;
		}
		snprintf(temp_name, sizeof(temp_name), "%s%u%s", "server", i, "_read_timeout");
		pool_conf [i].read_timeout = conf_loader [temp_name].to_int32(&err);
		if (err != 0)
		{
			ret = confparam_error;
			goto out;
		}
		snprintf(temp_name, sizeof(temp_name), "%s%u%s", "server", i, "_write_timeout");
		pool_conf [i].write_timeout = conf_loader [temp_name].to_int32(&err);
		if (err != 0)
		{
			ret = confparam_error;
			goto out;
		}
		snprintf(temp_name, sizeof(temp_name), "%s%u%s", "server", i, "_connect_timeout");
		pool_conf [i].connect_timeout = conf_loader [temp_name].to_int32(&err);
		if (err != 0)
		{
			ret = confparam_error;
			goto out;
		}
		snprintf(temp_name, sizeof(temp_name), "%s%u%s", "server", i, "_min_connection");
		pool_conf [i].min_connection = conf_loader [temp_name].to_int32(&err);
		if (err != 0)
		{
			ret = confparam_error;
			goto out;
		}
		snprintf(temp_name, sizeof(temp_name), "%s%u%s", "server", i, "_max_connection");
		pool_conf [i].max_connection = conf_loader [temp_name].to_int32(&err);
		if (err != 0)
		{
			ret = confparam_error;
			goto out;
		}
		if (pool_conf [i].max_connection <= 0 || pool_conf [i].min_connection <= 0
				|| pool_conf [i].connect_timeout <= 0 || pool_conf [i].write_timeout <= 0
				|| pool_conf [i].read_timeout <= 0 || pool_conf [i].connect_timeout <= 0
				|| pool_conf [i].max_connection < pool_conf [i].min_connection)
		{
			ret = confparam_error;
			goto out;
		}

	}
	ret = this->addNewServer(pool_conf, server_num);
	delete [] pool_conf;
	pool_conf = NULL;
	out: if (NULL != pool_conf)
	{
		delete [] pool_conf;
		pool_conf = NULL;
	}
	return ret;
}


int MyclientPool::addNewServer(MyclientServerConf * conf, unsigned int num)
{
	if (NULL == conf || 0 == num)
	{
		return inputparam_error;
	}
	int ret = 0;
	MysqlServer * svr = new MysqlServer [num];
	for (unsigned int i = 0; i < num; i++)
	{
		snprintf(svr [i].ip, sizeof(svr [i].ip), "%s", conf [i].ip);
		svr [i].port = conf [i].port;
		snprintf(svr [i].username, sizeof(svr [i].username), "%s", conf [i].username);
		snprintf(svr [i].password, sizeof(svr [i].password), "%s", conf [i].password);
		snprintf(svr [i].dbname, sizeof(svr [i].dbname), "%s", conf [i].dbname);
		snprintf(svr [i].charset, sizeof(svr [i].charset), "%s", conf [i].charset);
		svr [i].min_connection = conf [i].min_connection;
		svr [i].max_connection = conf [i].max_connection;
		svr [i].connect_timeout = conf [i].connect_timeout;
		svr [i].write_timeout = conf [i].write_timeout;
		svr [i].read_timeout = conf [i].read_timeout;
		ret = svr [i].setTag(conf [i].tag);
		if (ret < 0)
		{
			ret = inputparam_error;
			goto out;
		}
		ret = this->mysql_pool->addServer(&svr [i]);
		if (ret < 0)
		{
			ret = inputparam_error;
			goto out;
		}
	}
	if (ret != 0)
	{
		ret = pool_init_fail;
		goto out;
	}
	delete [] svr;
	svr = NULL;
	this->is_init_ok = true;
	out: if (svr != NULL)
	{
		delete [] svr;
		svr = NULL;
	}
	return ret;
}
