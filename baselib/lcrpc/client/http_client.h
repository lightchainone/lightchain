#ifndef __LCRPC_HTTP_CLIENT_H
#define __LCRPC_HTTP_CLIENT_H

#include <stdlib.h>
#include <vector>
#include <string>
#include <string.h>
#include <Lsc/containers/string/Lsc_string.h>
#include <Lsc/map.h>
#include "block_client.h"

namespace lcrpc{
class HttpSender;
class HttpHeader
{
plclic:
    void add_header(const char * key, const char * value);
    HttpHeader(HttpSender * sender) : _httpsender(sender)
    {
    }
protected:
    HttpSender * _httpsender;
};

class HttpVarClient : plclic Asyncable
{
plclic:
	
	HttpVarClient(lc::LcClientManager * lcmgr, Lsc::ResourcePool & rp)
        : Asyncable(lcmgr), _deser(rp)
	{
        if (_url_map.create(1024) != 0)
        {
            throw Lsc::BadArgumentException() << BSL_EARG;
        }
        _event = new (std::nothrow)RpcLcClientHttpEvent(this);
        if (NULL == _event) {
            throw Lsc::BadArgumentException() << BSL_EARG;
        }
        _id = (long)pthread_self() ^ (long)time(NULL);
        int temp_opt = 1;
        _ser.set_opt(Lsc::var::TMP_BUFFER_SIZE, (void *)&temp_opt, sizeof(temp_opt));
        _deser.set_opt(Lsc::var::TMP_BUFFER_SIZE, (void *)&temp_opt, sizeof(temp_opt));
	}
	
	virtual ~HttpVarClient()
	{
        _url_map.destroy();
        _event->release();
	}
	
    int call(const char *service, const char *function, Lsc::var::IVar &req, Lsc::var::Ref &res);
	
		
protected:
    virtual void write_header(HttpHeader &)
    {
    }
	
	virtual int onError(const mc_pack_t * )
	{
		CWARNING_LOG("rpc server error: %s, %s", getErrorMessage(), getErrorData());
		return 0;
	}
	
	virtual int postpare();

    
    virtual int onNetError()
    {
        _error_msg = _event->status_to_string(_event->get_sock_status());
        return 0;
    }
plclic:
    void set_charset(const char * charset)
    {
        _charset = charset;
    }
private:
	int64_t gen_id()
	{
		_id++;
		if(_id < 0)
		{
			_id = -_id;
		}
		return _id;
	}
	friend class NonblockClient;		  
    class RpcLcClientHttpEvent : plclic lc::LcClientHttpEvent
    {
    plclic:
        RpcLcClientHttpEvent(HttpVarClient * clnt) : _clnt(clnt)
        {

        }
        HttpVarClient * getClient()
        {
            return _clnt;
        }
    private:
        HttpVarClient * _clnt;
    };
    Lsc::string _charset;
    RpcLcClientHttpEvent *_event;
	Buffer _request_buf;		  
	Buffer _response_buf;		  
	Lsc::var::Ref * _res;		  
	int64_t _id;		  
    Lsc::var::McPackDeserializer _deser;         
    Lsc::var::McPackSerializer _ser;
    Lsc::hashmap<Lsc::string, Lsc::string> _url_map;
	
    int prepare(const char *service, const char *function, Lsc::var::IVar &req,
            Lsc::var::Ref &res);
	
			
	
    int parse_result(Lsc::var::Ref *res);
	
	
    int make_request(const char *function, HttpSender *sender, const Lsc::var::IVar &req);
	
	
	
	
	
};

}

#endif



