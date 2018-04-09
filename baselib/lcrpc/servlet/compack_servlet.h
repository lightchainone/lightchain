


#ifndef  __COMPACK_SERVLET_H_
#define  __COMPACK_SERVLET_H_

#include <cstring>
#include <map>
#include <Lsc/pool.h>
#include <Lsc/ResourcePool.h>
#include <Lsc/containers/string/Lsc_string.h>
#include <lc_server.h>
#include <mc_pack.h>
#include <lnhead.h>

#include "servlet.h"

namespace lcrpc
{
class CompackServlet : plclic Servlet
{
private:
	Lsc::syspool _default_pool;
	Lsc::mempool * _pool;
	Lsc::string _error_msg_cpy;
	Lsc::string _error_data_cpy;
	const char * _error_msg;
	const char * _error_data;
    
	
	
plclic:
	
	CompackServlet() : _pool(&_default_pool), _error_msg(0), _error_data(0) {}
	
	virtual ~CompackServlet()
	{
	}
	
	virtual int process(void * input, unsigned int input_length, void * output, unsigned int out_length);
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

	
	virtual int writeError(compack::buffer::Object * response);
	
	virtual int dispatch(int64_t id, const char * function, const mc_pack_t * param, compack::buffer::Object * response) = 0;

    
	virtual int dispatch(int64_t , const char * , const mc_pack_t * , mc_pack_t * )
    {
        return 0;
    }
	
	virtual int call(const mc_pack_t * request, compack::buffer::Object * response);
	
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
	
	virtual int writeHeader(compack::buffer::Object * )
	{
		
		return 0;
	}

};
}













#endif  


