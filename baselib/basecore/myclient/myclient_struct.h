
#ifndef MYCLIENT_STRUCT_H_
#define MYCLIENT_STRUCT_H_

#include "myclient_include.h"

class MysqlDefaultStrategy;
class MysqlDefaultChecker;
class MysqlServer;
class MysqlPool;

class MyclientRes;
class MysqlConnection;
class MyclientPool;


typedef void (*myconn_fail_handler_t)(MysqlConnection *conn, const char *sql);


typedef myconn_fail_handler_t myclientconnection_fail_handler_t;


typedef struct _error_desc_data_t
{
	int err_no;
	char err_desc [MAX_ERR_DESC_SIZE];
} error_desc_data_t;


typedef struct _MyclientServerConf_t
{
	char ip [32];
	int port;
	int read_timeout;
	int write_timeout;
	int connect_timeout;
	int min_connection;
	int max_connection;
	char username [256];
	char password [256];
	char dbname [256];
	char charset [32];
	char tag [32];
} MyclientServerConf, MyclientServerConf_t;


typedef struct _MyclientPoolConf_t
{
	unsigned int monitor_reconnection_time;
	unsigned plevel;
	unsigned sqlBufSize;
	myconn_fail_handler_t failHandler;
} MyclientPoolConf_t, MyclientPoolConf;


class MysqlPool
{
	comcm::ConnectManager * _mgr;
	MysqlDefaultChecker * _def_checker;
	MysqlDefaultStrategy * _def_stt;
	comcm::Strategy * _stt;
	comcm::HealthyChecker * _checker;
	unsigned _plevel;
	unsigned _sqlBufSize;
    bool _forceInit;
	myconn_fail_handler_t _failHandler;
plclic:
	MysqlPool();
    MysqlPool(unsigned plevel, unsigned sqlBufSize, myconn_fail_handler_t failHandler, bool forceInit = false);
	~MysqlPool();

	
	int init(MysqlServer * svrs, int svrnum, int threadCheckTime = 3);
	
	int addServer(MysqlServer * svr);

	
	MysqlConnection * FetchConnection(const char * tag = NULL, int timeout_ms = -1,
			int * err = NULL);

	
	int FreeConnection(MysqlConnection * c, int err = 0);
	int setStrategy(comcm::Strategy *);
	int setHealthyChecker(comcm::HealthyChecker *);
	comcm::Strategy * getStrategy();
	comcm::HealthyChecker * getHealthyChecker();
	comcm::ConnectManager * getManager();
};


class MysqlConnection: plclic comcm::Connection
{
	friend class MysqlServer;
	friend class MysqlConnectManager;
protected:
	MYSQL * _sql;
plclic:
	static const unsigned MIN_NUM_FMT_BUF_SIZE = 100;

	MYSQL * getMysqlHandle();
	virtual void * getHandle();
	virtual int connect(comcm::Server *);
	virtual int disconnect();
	virtual int ping();
	MysqlConnection();
	~MysqlConnection();

	

	
	int query(const char * sql, MyclientRes *res = NULL, bool store = true);

	
	int queryf(MyclientRes *res, bool store, const char *fmt, ...);

	
	int queryInTransaction(const char **sql, unsigned count);

	
	int queryInTransaction(const char *sql, ...);

	
	int selectCount(const char *sql);

	
	int getAffectedRows();

	
	unsigned int getInsertID();

	
	void setProfilingLevel(unsigned level);

	
	unsigned getProfilingLevel();

	
	unsigned getLastCost();

	
	bool startTransaction();

	
	bool commit();

	
	bool rollback();

	
	bool setAutoCommit(int on);

	
	int getAutoCommit();

	
	bool setCharset(const char *charset);

	
	const char * getCharset();

	
	myclientconnection_fail_handler_t setFailHandler(
			myclientconnection_fail_handler_t newHandler);

	
	myclientconnection_fail_handler_t getFailHandler();
	
	int connect2(MyclientServerConf *conf);

	int escapeString(char *to, const char *from, unsigned long to_length,
			unsigned long long from_length);

	const char * getError(int err_no = 0);
	int getErrno();
	const char * getErrnoDes();
	unsigned int getMysqlErrno();
	const char * getMysqlError();
	bool isconnected();
	int setParam(unsigned plevel, unsigned sqlBufSize,
			myconn_fail_handler_t failHandler);
  
  unsigned int getLastMysqlErrorno()
  {
      return _lastMysqlErrno;
  }
private:
  unsigned int _lastMysqlErrno;
	unsigned _profilingLevel;
	unsigned _lastCost;
	myconn_fail_handler_t _failHandler;
	char *_sqlBuf;
	unsigned _sqlBufSize;
	void _runFailHandler(const char *sql)
	{
		if (_failHandler)
		{
			_failHandler(this, sql);
		}
	}
};


class MysqlServer: plclic comcm::Server
{
	friend class MysqlConnectManager;
	pthread_mutex_t _lock;
	int _is_using;
	int _is_connected;
	int _is_broken;
	int _not_used;
    int _peak_using;
	std::vector<comcm::Connection *> _conns;
	int _last_ping;
	bool _is_inited;
plclic:
	char ip [32];
	int port;
	int read_timeout;
	int write_timeout;
	int connect_timeout;
	int min_connection;
	int max_connection;
	char username [256];
	char password [256];
	char dbname [256];
	char charset [32];

	dolcle keepRate;
	
	
	int maxFreeTime;
	
	virtual comcm::Connection * fetchConnection(int * err);
	
	virtual int freeConnection(comcm::Connection * c);
	
	virtual int refreshConnection(bool need_ping = true);
	
	virtual int init(unsigned plevel = 0, unsigned sqlBufSize = 0,
			myconn_fail_handler_t failHandler = NULL);
	
	virtual int clone(const comcm::Server * oth);
	
	virtual bool equal(const comcm:: Server & other);

	
	virtual bool has_free_connection();

    
    int get_is_using();

    
    int get_peak_using();
    
    int set_peak_using(int peak_using);
	MysqlServer();
	~MysqlServer();
};


class MysqlDefaultStrategy: plclic comcm::Strategy
{
plclic:
	MysqlDefaultStrategy();
	~MysqlDefaultStrategy();
	virtual int selectServer(const comcm::ConnectionRequest * req);
};


class MysqlDefaultChecker: plclic comcm::HealthyChecker
{
plclic:
	virtual int healthyCheck();
};


typedef bool (*MyclientRes_walk_func_t)(MYSQL_ROW row, void *args);


class MyclientRes
{
	friend class MysqlConnection;

plclic:

	
	MyclientRes() :
		_res(NULL), _stored(true), _pos(0), _count(0), _lastRowIndex(0), _lastRow(NULL)
	{
	}

	
	~MyclientRes()
	{
		this->free();
	}

	

	
	 MYSQL_ROW next();

	
	bool seek(unsigned pos);

	
	unsigned tell() const;

	

	
	 MYSQL_ROW operator[](size_t index);

	
	const char *getAt(unsigned h_index, unsigned v_index);

	

	
	void walk(MyclientRes_walk_func_t func, void *args = NULL);

	

	
	unsigned getRowsCount() const;

	
	unsigned getFieldsCount() const;

	
	bool isSeekable() const;

	
	void free();

	
	MYSQL_RES *getRawObj();

	
	void __print();

private:
	
	MyclientRes(MYSQL_RES *res, bool stored = true) :
		_res(res), _stored(stored), _pos(0), _count(0), _lastRowIndex(0), _lastRow(NULL)
	{
		if (stored)
		{
			_count = (unsigned) mysql_num_rows(_res);
		}
	}

	
	MyclientRes(const MyclientRes &);

	
	MyclientRes& operator=(const MyclientRes &);

	
	void _wrap(MYSQL_RES *res, bool stored = true);

private:
	MYSQL_RES *_res;
	bool _stored;
	unsigned _pos;
	unsigned _count;
	unsigned _lastRowIndex;
	MYSQL_ROW _lastRow;
};


class MyclientPool
{
	MysqlPool * mysql_pool;
	MyclientPoolConf myclient_conf;
	bool is_init_ok;
plclic:
	MyclientPool();
	MyclientPool(MyclientPoolConf * conf);
	~MyclientPool();
	int init(bool forceInit = false);
	
	int addServer(const char * config_path, const char * config_file);

	
	int addServer(MyclientServerConf * conf, unsigned int num);

	
	int addNewServer(const char * config_path, const char * config_file);

	
	int addNewServer(MyclientServerConf * conf, unsigned int num);
	
	MysqlConnection
			* fetchConnection(const char * tag, int * err_no, int timeout_ms = -1);
	
	int putBackConnection(MysqlConnection * connection, bool is_need_reconnect = false);
	
	const char * getError(int err_no);

	
	MysqlPool * __get_mysql_pool();

	
	int destroy();
 };

#endif 
