#ifndef __LCRPC_BLOCK_CLIENT_H
#define __LCRPC_BLOCK_CLIENT_H
#include <mcpack_idm.h>
#include <com_log.h>
#include <lcclient_include.h>
#include <vector>
#include <stdlib.h>
#include <Lsc/var/IVar.h>
#include <Lsc/var/implement.h>
#include <Lsc/var/McPackDeserializer.h>
#include <Lsc/var/McPackSerializer.h>
#include <Lsc/map.h>

#include "buffer.h"

namespace lcrpc
{

class Asyncable
{
plclic:
	
	const char * getErrorMessage()
	{
		return _error_msg.c_str();
	}
	
	const char * getErrorData()
	{
		return _error_data.c_str();
	}
	virtual int postpare() = 0;
	
	virtual int onNetError() = 0;
	Asyncable(lc::LcClientManager * lcmgr) : _lcmgr(lcmgr)
	{
		if(lcmgr == 0)
		{
			throw Lsc::BadArgumentException() << BSL_EARG;
		}
	}
	virtual ~Asyncable()
	{
	}
protected:
	lc::LcClientManager *_lcmgr;		  
	Lsc::string _error_msg;		  
	Lsc::string _error_data;		  
	int32_t _error_code;		  
};


class NsheadClientBase : plclic Asyncable
{
plclic:
	
	int getError()
	{
		if(_talk.success != 0)
		{
			return _talk.success;
		}
		if(_error_code != 0)
		{
			return _error_code;
		}
		return 0;
	}
	
	virtual ~NsheadClientBase(){}
	
	void setLogId(int logid)
	{
		_logid = logid;
	}
	
	void setProvider(const char * provider)
	{
		_provider = provider;
	}

	
	void setServer(int master, int key, const char * range, const char * version)
	{
		if(range != 0)
		{
			memcpy(_talk.defaultserverarg.range, range, strlen(range) + 1);
		}
		else
		{
			_talk.defaultserverarg.range[0] = 0;
		}
		if(version != 0)
		{
			memcpy(_talk.defaultserverarg.version, version, strlen(version) + 1);
		}
		else
		{
			_talk.defaultserverarg.version[0] = 0;
		}
		_talk.defaultserverarg.master = master;
		_talk.defaultserverarg.key = key;

	}

	
	const lc::lnhead_talkwith_t & getImpl()
	{
		return _talk;
	}
	
    
    void checkID(bool checkid = true)
    {   
        _checkid = checkid;
    }
protected:
	
	NsheadClientBase(lc::LcClientManager * lcmgr) : Asyncable(lcmgr), _talk(this), _checkid(true)
	{
		_id = (long)pthread_self() ^ (long)time(NULL);
	}
	
	virtual int postpare() = 0;
	
	virtual int onNetError()
	{
		_error_msg = lc::get_talk_errorinfo(_talk.success);
		return 0;
	}
	
	class rpc_talkwith_t : plclic lc::lnhead_talkwith_t
	{
	plclic:
		
		rpc_talkwith_t(NsheadClientBase * client) : _client(client)
		{
		}
		NsheadClientBase * getClient()
		{
			return _client;
		}
	protected:
		NsheadClientBase * _client;		  
	};
	
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
	
	rpc_talkwith_t _talk;		  
	int64_t _id;		  
	int _logid;		  
	Lsc::string _provider;		  
    bool _checkid;
};


enum {
    PROTO_RPC = 0,
    PROTO_NRPC = 1,
};


class Client : plclic NsheadClientBase
{
plclic:
	
	Client(lc::LcClientManager * lcmgr) : NsheadClientBase(lcmgr), _proto_type(0), _mcpack_version(2)
	{
        if (_name_map.create(1024) != 0)
        {
            throw Lsc::BadArgumentException() << BSL_EARG;
        }
	}
	
	virtual ~Client()
	{
        _name_map.destroy();
	}
	
	int call(const char * service, const char * function, idm::Bean * req, 
			idm::Bean * res);

    
    void setProtoType(int type){_proto_type = type;}

    
    int getProtoType(){return _proto_type;}

    
    void setMcpackVersion(int version) {_mcpack_version = version;}

    
    int getMcpackVersion() {return _mcpack_version;}

    
    const char *get_low_buffer(unsigned int *len) {
        if (NULL == len) {
            return NULL;
        }
        *len = _response_buf.size();
        return _response_buf.start();
    }
protected:
	
	virtual int writer_header(const char * , mc_pack_t * )
	{
		return 0;
	}
	
	virtual int onError(const mc_pack_t * )
	{
		CWARNING_LOG("rpc server error: %s, %s", getErrorMessage(), getErrorData());
		return 0;
	}
	
	virtual int postpare();
private:
	friend class NonblockClient;		  
	Buffer _request_buf;		  
	Buffer _response_buf;		  
	idm::Bean * _res;		  

    int _proto_type;		  
    int _mcpack_version;		  

    Lsc::hashmap<Lsc::string, Lsc::string> _name_map;
	
	int prepare(const char * service, const char * function, idm::Bean * req, 
			idm::Bean * res);
	
	int parse_result(idm::Bean * res);
	
	int make_request(const char * service, const char * function, idm::Bean * req);
};


class VarClient : plclic NsheadClientBase
{
plclic:
	
	VarClient(lc::LcClientManager * lcmgr, Lsc::ResourcePool & rp) 
		: NsheadClientBase(lcmgr), _deser(rp)
	{
        if (_name_map.create(1024) != 0)
        {
            throw Lsc::BadArgumentException() << BSL_EARG;
        }
        int temp_opt = 1;
        _ser.set_opt(Lsc::var::TMP_BUFFER_SIZE, (void *)&temp_opt, sizeof(temp_opt));
        _deser.set_opt(Lsc::var::TMP_BUFFER_SIZE, (void *)&temp_opt, sizeof(temp_opt));
	}
	
	virtual ~VarClient()
	{
        _name_map.destroy();
	}
	
	int call(const char * service, const char * function, 
			const Lsc::var::IVar & req, Lsc::var::Ref & res);
	
	Lsc::var::McPackSerializer & getSerializer()
	{
		return _ser;
	}
	
	Lsc::var::McPackDeserializer & getDeserializer()
	{
		return _deser;
	}

    
    const char *get_low_buffer(unsigned int *len) {
        if (NULL == len) {
            return NULL;
        }
        *len = _response_buf.size();
        return _response_buf.start();
    }
protected:
	
	virtual int writer_header(const char * , mc_pack_t * )
	{
		return 0;
	}
	
	virtual int onError(const mc_pack_t * )
	{
		CWARNING_LOG("rpc server error: %s, %s", getErrorMessage(), getErrorData());
		return 0;
	}
	
	virtual int postpare();
private:
	
	int prepare(const char * service, const char * function, 
			const Lsc::var::IVar & req, Lsc::var::Ref & res);
	
	int parse_result(Lsc::var::Ref * res);
	
	int make_request(const char * service, const char * function, 
			const Lsc::var::IVar & var);
	friend class NonblockClient;		  
	Lsc::var::McPackSerializer _ser;		  
	Lsc::var::McPackDeserializer _deser;		  
	Lsc::var::Ref * _res;		  
	Buffer _request_buf;		  
	Buffer _response_buf;		  
    Lsc::hashmap<Lsc::string, Lsc::string> _name_map;
};
}

#endif 
