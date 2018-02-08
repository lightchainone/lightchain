#include "myclient_include.h"



MYSQL * MysqlConnection::getMysqlHandle()
{
    return _sql;
}


void * MysqlConnection::getHandle()
{
    return (void *)(&_sql);
}


MysqlConnection::MysqlConnection()
{
    _sql = NULL;
    _profilingLevel=0;
	_lastCost=0;
	_failHandler=NULL;
	_sqlBuf=NULL;
	_sqlBufSize=DEFAULT_QUERYF_BUFFER_SIZE;
  _lastMysqlErrno=0;
}

MysqlConnection::~MysqlConnection()
{
   if(NULL!=_sqlBuf)
   {
	   free(_sqlBuf);
	   _sqlBuf=NULL;
   }
}

int MysqlConnection::ping()
{
    if (_sql)
    {
        return mysql_ping(_sql);
    }
    else
    {
        return -1;
    }
    return 0;
}


int MysqlConnection::connect(comcm::Server * svr)
{
    _log->debug("Try to connect to mysql");
    if (_sql != NULL)
    {
        _log->warning("_sql is not NULL but connection status is %d!", this->getKernelStatus());
        return -1;
    }
    MysqlServer * msvr= NULL;
    try
    {
        msvr = dynamic_cast <MysqlServer *> (svr);
    }
    catch(...)
    {
        _log->warning("MysqlConnection : wrong type of server");
        return -1;
    }
    int ret = 0;
    if (_sql!=NULL)
    {
        this->disconnect();
    }
    _sql = mysql_init(NULL);
    if (_sql)
    {
        mysql_options(_sql, MYSQL_OPT_CONNECT_TIMEOUT,
            (char *)&(msvr->connect_timeout));
        mysql_options(_sql, MYSQL_OPT_READ_TIMEOUT,
            (char *)&(msvr->read_timeout));
        mysql_options(_sql, MYSQL_OPT_WRITE_TIMEOUT,
            (char *)&(msvr->write_timeout));
        if (NULL != mysql_real_connect(_sql, msvr->ip, msvr->username, msvr->password,
              msvr->dbname, msvr->port, NULL, 0))
        {
            if ( 0 != mysql_set_character_set(_sql, msvr->charset) )
            {
                _log->warning("failed set charset to %s,[%d][%s]",
                    msvr->charset, mysql_errno(_sql),
                    mysql_error(_sql));
                ret = -1;
            }
        }
        else
        {
            _log->warning(
                "failed to connect to mysql server %s:%d [%s]",
                msvr->ip, msvr->port, mysql_error(_sql));
            ret = -1;
        }
    }
    else
    {
        _log->warning("failed to init MYSQL connection");
        ret = -1;
    }
    if (ret == -1&& _sql)
    {
        mysql_close(_sql);
        _sql = NULL;
    }
    
    _serverID=svr->getID();
    
    return ret;
}


int MysqlConnection::disconnect()
{
    if (_sql)
    {
        mysql_close(_sql);
        _sql = NULL;
        return 0;
    }
    return -1;
}

const char * MysqlConnection::getError(int err_no)
{
    return mc_get_err_dest(err_no);
}


int MysqlConnection::connect2(MyclientServerConf *conf)
{
    int ret=0;
    if(NULL==conf)
    {
    	return inputparam_error;
    }
    MysqlServer svr;

    snprintf(svr.ip, sizeof(svr.ip),"%s",conf->ip);
    svr.port = conf->port;
    snprintf(svr.username,sizeof(svr.username),"%s",conf->username);
    snprintf(svr.password,sizeof(svr.password),"%s",conf->password);
    snprintf(svr.dbname,sizeof(svr.dbname),"%s",conf->dbname);
    snprintf(svr.charset,sizeof(svr.charset),"%s",conf->charset);
    svr.min_connection = conf->min_connection;
    svr.max_connection = conf->max_connection;
    svr.connect_timeout=conf->connect_timeout;
    svr.write_timeout=conf->write_timeout;
    svr.read_timeout=conf->read_timeout;
    ret=this->connect(&svr);
    if(ret!=0)
    {
        return connection_connect_fail;
    }
    return ret;
}


bool MysqlConnection::isconnected()
{
    if (this->_sql)
    {
        if (0==mysql_ping(_sql))
        {
            return true;
        }
    }
    return false;
}


int MysqlConnection::escapeString(char *to, const char *from,
    unsigned long to_length,unsigned long long from_length)
{
    if (NULL==to || NULL==from || to_length<2*from_length+1 )
    {
        return inputparam_error;
    }
    if (NULL==this->_sql)
    {
        return connection_not_init;
    }
    unsigned int ret= mysql_real_escape_string(this->_sql, to, from,
        from_length);
    if (ret>=from_length)
    {
        
    	return ret;
    }
    return connection_escape_fail;
}


int MysqlConnection::getErrno()
{
    if (NULL==this->_sql)
    {
        return connection_not_init;
    }
    return mysql_errno(this->_sql);
}


const char *  MysqlConnection::getErrnoDes()
{
    if (NULL==this->_sql)
    {
        return "";
    }
    return mysql_error(this->_sql);
}


int MysqlConnection::query(const char *sql, MyclientRes *res,
    bool store)
{
    if(NULL==sql)
    {
        return inputparam_error;
    }
    struct timeval tv_A, tv_B;
    if (_profilingLevel > 0)
    {
        gettimeofday(&tv_A, NULL);
    }
    _log->debug("MysqlConnection query:[%s]",sql);
    int ret = mysql_query(_sql, sql);
    if (_profilingLevel > 0)
    {
        gettimeofday(&tv_B, NULL);
        _lastCost = (tv_B.tv_sec - tv_A.tv_sec)*1000000
            + (tv_B.tv_usec - tv_A.tv_usec);
    }

    if (ret != 0)
    {
        _runFailHandler(sql);
        return -1;
    }

    
    if (mysql_field_count(_sql) == 0)
    {
        return (int)mysql_affected_rows(_sql);
    }

    MYSQL_RES *tmp_res = store ? mysql_store_result(_sql)
        : mysql_use_result(_sql);
    if (!tmp_res)
    {
        _runFailHandler(sql);
        return -1;
    }

    if (!res)
    {
        mysql_free_result(tmp_res);
        _runFailHandler(sql);
        return -1;
    }

    res->_wrap(tmp_res, store);
    return res->getRowsCount();
}

int MysqlConnection::selectCount(const char *sql)
{
    MyclientRes res;

    
    if (this->query(sql, &res) < 0|| !res.getRawObj())
    {
        return -1;
    }

    MYSQL_ROW row = res[0];
    if (!row)
    {
        return -1;
    }
    return atoi(row[0]);
}

int MysqlConnection::getAffectedRows()
{
    return (int)mysql_affected_rows(_sql);
}

unsigned MysqlConnection::getInsertID()
{
    return (unsigned)mysql_insert_id(_sql);
}



unsigned MysqlConnection::getProfilingLevel()
{
    return _profilingLevel;
}

void MysqlConnection::setProfilingLevel(unsigned level)
{
    _profilingLevel = level;
}

unsigned MysqlConnection::getLastCost()
{
    return _lastCost;
}



bool MysqlConnection::startTransaction()
{
    const char *sql = "START TRANSACTION";
    return (this->query(sql) >= 0);
}

bool MysqlConnection::commit()
{
    return (mysql_commit(this->_sql) == 0);
}

bool MysqlConnection::rollback()
{
    
    this-> _lastMysqlErrno=mysql_errno(this->_sql);
    
    return (mysql_rollback(this->_sql) == 0);
}

bool MysqlConnection::setAutoCommit(int on)
{
    return (mysql_autocommit(this->_sql, (my_bool)(on != 0)) == 0);
}

int MysqlConnection::getAutoCommit()
{
    const char *sql = "SELECT @@autocommit";
    MyclientRes res;

    if (this->query(sql, &res) < 0|| !res.getRawObj())
    {
        return -1;
    }

    return atoi(res[0][0]);
}

unsigned int MysqlConnection::getMysqlErrno()
{
    return mysql_errno(this->_sql);
}

const char * MysqlConnection::getMysqlError()
{
    return mysql_error(this->_sql);
}






inline bool MysqlConnection::setCharset(const char *charset)
{
	return (mysql_set_character_set(_sql, charset) == 0);
}

const char * MysqlConnection::getCharset()
{
    return mysql_character_set_name(_sql);
}



myclientconnection_fail_handler_t MysqlConnection::setFailHandler(
    myclientconnection_fail_handler_t newHandler)
{
    myclientconnection_fail_handler_t oldHandler = _failHandler;
    _failHandler = newHandler;
    return oldHandler;
}

myclientconnection_fail_handler_t MysqlConnection::getFailHandler()
{
    return _failHandler;
}


int MysqlConnection::queryInTransaction(const char **sql,
    unsigned count)
{
	if(NULL==sql||0==count)
	{
		return inputparam_error;
	}
    MyclientRes res;

    if (!this->startTransaction())
    {
        return connection_query_fail;
    }

    for (unsigned i = 0; i != count; i++)
    {
        if (this->query(sql[i], &res) < 0)
        {
            this->rollback();
            return connection_query_fail;
        }
    }
    return this->commit() ? 0 : connection_query_fail;
}

int MysqlConnection::queryInTransaction(const char *sql, ...)
{
	if(NULL==sql)
	{
		return inputparam_error;
	}
    MyclientRes res;
    if (!this->startTransaction())
    {
        return connection_query_fail;
    }

    if (this->query(sql, &res) < 0)
    {
        this->rollback();
        return connection_query_fail;
    }

    va_list ap;
    va_start(ap, sql);
    while((sql = va_arg(ap,
          const char*)))
    {
        if(this->query(sql, &res) < 0)
        {
            this->rollback();
            va_end(ap);
            return connection_query_fail;
        }
    }
    va_end(ap);
    return this->commit() ? 0 : connection_query_fail;
}

int MysqlConnection::queryf(MyclientRes *res,
    bool store, const char *fmt, ...)
{
    if (!this->_sqlBuf)
    {
        this->_sqlBuf
            = (char*)malloc(this->_sqlBufSize);
        if (!this->_sqlBuf)
        {
            return -1;
        }
    }
    if(NULL==fmt)
    {
        return inputparam_error;
    }

    unsigned leftSize = this->_sqlBufSize;
    char *wp = this->_sqlBuf;
    const char *token;
    unsigned cpsize;
    char c;
    const char *v_str;
    int v_int32;
    long long v_int64;
    unsigned v_uint32;
    unsigned long long v_uint64;


    int ret;
    va_list ap;

    va_start(ap, fmt);

    while ((token = strchr(fmt, '%')))
    {
        cpsize = token - fmt;
        if (cpsize > leftSize)
        {
            goto safe_abort;
        }
        memcpy(wp, fmt, cpsize);
        leftSize -= cpsize;
        wp += cpsize;

        c = *(token+1);
        switch (c)
        {
            
            case 's':
                v_str = va_arg(ap, const char*);
                cpsize = strlen(v_str);
                if (cpsize*2+1> leftSize)
                {
                    goto safe_abort;
                }
                cpsize = this->escapeString(wp,
                    v_str,leftSize,cpsize);
                leftSize -= cpsize;
                wp += cpsize;
                break;

                
            case 'S':
                v_str = va_arg(ap, const char*);
                cpsize = strlen(v_str);
                if (cpsize > leftSize)
                {
                    goto safe_abort;
                }
                memcpy(wp, v_str, cpsize);
                leftSize -= cpsize;
                wp += cpsize;
                break;

                
            case 'd':
                v_int32 = va_arg(ap, int);
                if (leftSize
                    < MysqlConnection::MIN_NUM_FMT_BUF_SIZE)
                {
                    goto safe_abort;
                }

                ret = snprintf(wp, leftSize,
                    "%d", v_int32);
                if (ret < 0)
                {
                    goto safe_abort;
                }
                leftSize -= ret;
                wp += ret;
                break;

                
            case 'D':
                v_int64 = va_arg(ap, long long);
                if (leftSize
                    < MysqlConnection::MIN_NUM_FMT_BUF_SIZE)
                {
                    goto safe_abort;
                }

                ret = snprintf(wp, leftSize,
                    "%lld", v_int64);
                if (ret < 0)
                {
                    goto safe_abort;
                }
                leftSize -= ret;
                wp += ret;
                break;

                
            case 'u':
                v_uint32 = va_arg(ap, unsigned int);
                if (leftSize
                    < MysqlConnection::MIN_NUM_FMT_BUF_SIZE)
                {
                    goto safe_abort;
                }

                ret = snprintf(wp, leftSize,
                    "%u", v_uint32);
                if (ret < 0)
                {
                    goto safe_abort;
                }
                leftSize -= ret;
                wp += ret;
                break;

                
            case 'U':
                v_uint64 = va_arg(ap, unsigned long long);
                if (leftSize
                    < MysqlConnection::MIN_NUM_FMT_BUF_SIZE)
                {
                    goto safe_abort;
                }

                ret = snprintf(wp, leftSize,
                    "%llu", v_uint64);
                if (ret < 0)
                {
                    goto safe_abort;
                }
                leftSize -= ret;
                wp += ret;
                break;

            case '%':
                if (leftSize == 0)
                {
                    goto safe_abort;
                }
                *wp = '%';
                wp++;
                leftSize--;
                break;

            default:
                goto safe_abort;
        }

        fmt = token + 2;
    }
    
    cpsize = strlen(fmt);
    if (cpsize >= leftSize)
    {
        goto safe_abort;
    }
    memcpy(wp, fmt, cpsize);
    wp[cpsize] = 0;

    

    va_end(ap);
    return this->query(this->_sqlBuf, res,
        store);

safe_abort: va_end(ap);
            return -1;
}


int MysqlConnection::setParam(unsigned plevel, unsigned sqlBufSize, myconn_fail_handler_t failHandler )
{
	this->_profilingLevel=plevel;
	this->_sqlBufSize=sqlBufSize;
	this->_failHandler=failHandler;
	return 0;
}
