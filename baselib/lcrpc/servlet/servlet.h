
#ifndef  __SERVLET_H_
#define  __SERVLET_H_

#include <cstring>
#include <map>
#include <Lsc/pool.h>
#include <Lsc/ResourcePool.h>
#include <Lsc/containers/string/Lsc_string.h>
#include <lc_server.h>
#include <mc_pack.h>
#include <lnhead.h>

namespace lcrpc
{

const int SUCCESS = 0;
const int UNKNOWN_ERROR = 1;

extern const char * const ERROR_MSG[UNKNOWN_ERROR + 1];

class Servlet
{
	
private:
	Lsc::syspool _default_pool;
	Lsc::mempool * _pool;
	Lsc::string _error_msg_cpy;
	Lsc::string _error_data_cpy;
	const char * _error_msg;
	const char * _error_data;
	
	
    

    long long _logid;
    long long _id;
plclic:
	
	Servlet() : _pool(&_default_pool), _error_msg(0), _error_data(0), _logid(0), _id(0) {}
	
	virtual ~Servlet()
	{
	}
	
	virtual int process(void * input, unsigned int input_length, void * output, unsigned int out_length);

    long long getLogid() {return _logid;}

    void setLogid(long long logid) {_logid = logid;}

    long long getId() {return _id;}
protected:
	
	void setErrorMsg(const char * msg, const char * error_data)
	{
		if(msg != NULL)
		{
			_error_msg_cpy = msg;
			_error_msg = _error_msg_cpy.c_str();
			if(error_data != NULL)
			{
				_error_data_cpy = error_data;
				_error_data = _error_data_cpy.c_str();
			}
			else
			{
				_error_data = 0;
			}
		}
		else
		{
			_error_msg = 0;
			_error_data = 0;
		}
	}
	
	void setErrorMsgPtr(const char * msg, const char * error_data)
	{
		if(msg != NULL)
		{
			_error_msg = msg;
			if(error_data != NULL)
			{
				_error_data_cpy = error_data;
				_error_data = _error_data_cpy.c_str();
			}
			else
			{
				_error_data = 0;
			}
		}
		else
		{
			_error_data = 0;
			_error_msg = 0;
		}
	}
	
	bool isError()
	{
		return _error_msg != 0;
	}

	
	virtual int writeError(mc_pack_t * response);
	
	virtual int dispatch(int64_t id, const char * function, const mc_pack_t * param, mc_pack_t * response) = 0;
	
	virtual int call(const mc_pack_t * request, mc_pack_t * response);
	
	Lsc::mempool * getMempool()
	{
		if(_pool == 0)
		{
			throw Lsc::NullPointerException() << BSL_EARG;
		}
		return _pool;
	}

	
	void setMempool(Lsc::mempool * pool)
	{
		_pool = pool;
	}
	
	virtual int readHeader(const mc_pack_t * )
	{
		
		return 0;
	}
	
	virtual int writeHeader(mc_pack_t * )
	{
		
		return 0;
	}
};


class LcRpcServer
{
plclic:
	
	void registerWithLc(lc_server_t * lc);
	
    virtual Servlet * createServlet() = 0;
protected:
	virtual int lc_call_back();
private:
	Servlet * getServlet();
	static void initServlet();
	static void deleteServlet(void * ptr);
	static pthread_key_t rl_key;
	static pthread_once_t rl_once;
	static int lc_call_back_entry();
};


}

#endif  


