#include "block_client.h"
#include "http_client.h"
#include "nonblock_client.h"

#include <mc_pack_rp.h>

namespace lcrpc
{


int Client::call(const char * service, const char * function, idm::Bean * req, idm::Bean * res)
{
    if (_proto_type == 0)
    {
	    if(service == 0 || function == 0)
	    {
		    CWARNING_LOG("bad param");
		    return -1;
	    }
    }
    else
    {
	    if(service == 0)
	    {
		    CWARNING_LOG("bad param");
		    return -1;
	    }
    }  
	if(prepare(service, function, req, res))
	{
		return -1;
	}
	if(_lcmgr->lnhead_singletalk(service, &_talk, Lsc::var::Null::null))
	{
		CWARNING_LOG("network error: %s", lc::get_talk_errorinfo(_talk.success));
		onNetError();
		return -1;
	}
	if(postpare())
	{
		return -1;
	}
	return 0;
}

int Client::prepare(const char * service, const char * function, idm::Bean * req, idm::Bean * res)
{
	_error_msg = 0;
	_error_code = 0;
	int reqLen;
	int resLen;
	_error_code = -1;
	_res = res;
	if(_lcmgr->getReqAndResBufLen(service, reqLen, resLen))
	{
		CWARNING_LOG("bad service");
		return -1;
	}
	_request_buf.resize(reqLen);
	_response_buf.resize(resLen);
	if(make_request(service, function, req))
	{
		CWARNING_LOG("make request error");
		return -1;
	}
	this->_talk.reqhead.version = 1000;
	_talk.reqhead.log_id = _logid;
	strncpy(_talk.reqhead.provider, _provider.c_str(), 15);
	_talk.reqhead.provider[16] = 0;
	_talk.reqhead.body_len = _request_buf.size();
	_talk.reqbuf = _request_buf.start();

	_talk.maxreslen = _response_buf.size();
	_talk.resbuf = _response_buf.start();
	return 0;
}

int Client::parse_result(idm::Bean * res)
{
	int result;
	Lsc::ResourcePool rp;
	mc_pack_t * pack = mc_pack_open_r_rp(_response_buf.start(), _response_buf.size(), &rp);
	if(MC_PACK_PTR_ERR(pack))
	{
		CWARNING_LOG("%s", mc_pack_perror((long)pack));
		return -1;
	}
    if (0 == _proto_type) {
	    int64_t id;
		mc_pack_t * contents = mc_pack_get_array(pack, "content");
		if(MC_PACK_PTR_ERR(contents))
		{
			CWARNING_LOG("%s", mc_pack_perror((long)contents));
			return -1;
		}
		mc_pack_t * content = mc_pack_get_object_arr(contents, 0);
		if(MC_PACK_PTR_ERR(content))
		{
			CWARNING_LOG("%s", mc_pack_perror((long)content));
			return -1;
		}
		if((result = mc_pack_get_int64(content, "id", (mc_int64_t*)&id)))
		{
			CWARNING_LOG("%s", mc_pack_perror(result));
			return -1;
		}

        if (_checkid) {
		    if(id != _id)
		    {
			    CWARNING_LOG("unmatched id: %ld", id);
			    return -1;
		    }
        }

		const mc_pack_t * err;
		err = mc_pack_get_object(content, "error");
		if(!MC_PACK_PTR_ERR(err))
		{
			if(mc_pack_get_int32(err, "code", &_error_code))
			{
			}
			const char * str = mc_pack_get_str_def(err, "message", "");
			_error_msg = str;
			str = mc_pack_get_str_def(err, "data", "");
			_error_data = str;
			onError(err);
			return -1;
		}
		if(res == 0)
		{
			return 0;
		}
		try
		{
			res->load(content);
			res->detach();
			_error_code = 0;
			return 0;
		}
		catch(Lsc::Exception &e)
		{
			CWARNING_LOG("Lsc::Exception caught, what():%s, from %d:%s", 
						e.what(), e.line(), e.file());
			return -1;
		}
		catch(...)
		{
			CWARNING_LOG("%s", "unknown exception catched");
			return -1;
		}
    } else {
        if (res == 0)
        {
            mc_pack_close(pack);
            return 0;
        }
        try
		{
			res->load(pack);
			res->detach();
			_error_code = 0;
            mc_pack_close(pack);
			return 0;
		}
		catch(Lsc::Exception &e)
		{
			CWARNING_LOG("Lsc::Exception caught, what():%s, from %d:%s", 
						e.what(), e.line(), e.file());
            mc_pack_close(pack);
			return -1;
		}
		catch(...)
		{
			CWARNING_LOG("%s", "unknown exception catched");
            mc_pack_close(pack);
			return -1;
		}

    }
}

int Client::make_request(const char * service, const char * function, idm::Bean * req)
{
	int result;
	Lsc::ResourcePool rp;
    mc_pack_t *pack = NULL;
    if (0 == _proto_type && 2 != _mcpack_version) {
        CWARNING_LOG("%s", "not support mcpack1 and lcrpc toghter");
        return -1;
    }
    if (2 == _mcpack_version) {
	    pack = mc_pack_open_w_rp(2, _request_buf.start(), _request_buf.size(), &rp);
    } else {
	    pack = mc_pack_open_w_rp(1, _request_buf.start(), _request_buf.size(), &rp);
    }
	if(MC_PACK_PTR_ERR(pack))
	{
		CWARNING_LOG("%s", mc_pack_perror((long)pack));
		return -1;
	}
    if (_proto_type == 0) {
	mc_pack_t * header = mc_pack_put_object(pack, "header");
	if(MC_PACK_PTR_ERR(header))
	{
		CWARNING_LOG("%s", mc_pack_perror((long)header));
		return -1;
	}
    
	bool conn;
	if(_lcmgr->getServiceConnType(service, conn))
	{
		CWARNING_LOG("bad service");
		return -1;
	}
	if((result = mc_pack_put_bool(header, "connection", conn)))
	{
		CWARNING_LOG("%s", mc_pack_perror((long)result));
	}

    Lsc::string real_name;
    Lsc::string str_service = service;
    if (_name_map.get(str_service, &real_name) == Lsc::HASH_NOEXIST) {
        Lsc::ResourcePool rp;
        int error_num = 0;
        Lsc::var::IVar &lcclient_conf = _lcmgr->getServiceConf(service, &rp, &error_num);
        if (0 != error_num)
        {
            CWARNING_LOG("get RpcServiceName from LcClientManager error");
            _error_msg = "get RpcServiceName from LcClientManager error";
            return -1;
        }
        if (lcclient_conf.get("RpcServiceName").is_null())
        {
            real_name = str_service;
        }
        else
        {
            real_name = lcclient_conf["RpcServiceName"].c_str();
        }
        _name_map.set(str_service, real_name);
    }

	if(writer_header(service, header) != 0)
	{
		CWARNING_LOG("writer header error");
		return -1;
	}
	mc_pack_t * contents = mc_pack_put_array(pack, "content");
	if(MC_PACK_PTR_ERR(contents))
	{
		CWARNING_LOG("%s", mc_pack_perror((long)contents));
		return -1;
	}
	mc_pack_t * content = mc_pack_put_object(contents, NULL);
	if(MC_PACK_PTR_ERR(content))
	{
		CWARNING_LOG("%s", mc_pack_perror((long)content));
		return -1;
	}
	if((result = mc_pack_put_str(content, "service_name", real_name.c_str())))
	{
		CWARNING_LOG("%s", mc_pack_perror((long)result));
		return -1;
	}
	if((result = mc_pack_put_int64(content, "id", gen_id())))
	{
		CWARNING_LOG("%s", mc_pack_perror(result));
		return -1;
	}
	if((result = mc_pack_put_str(content, "method", function)))
	{
		CWARNING_LOG("%s", mc_pack_perror(result));
		return -1;
	}
	mc_pack_t * params = mc_pack_put_object(content, "params");
	if(MC_PACK_PTR_ERR(params))
	{
		CWARNING_LOG("%s", mc_pack_perror((long)params));
		return -1;
	}
	try
	{
		if(req != 0)
		{
			req->save(params);
		}
	}
	catch(Lsc::Exception &e)
	{
		CWARNING_LOG("Lsc::Exception caught, what():%s, from %d:%s", 
				e.what(), e.line(), e.file());
		return -1;
	}
	catch(...)
	{
		CWARNING_LOG("%s", "unknown exception catched");
		return -1;
	}
    } else {
        try
        {
            if(req != 0)
            {
                req->save(pack);
            }
        }
		catch(Lsc::Exception &e)
		{
            mc_pack_close(pack);
			CWARNING_LOG("Lsc::Exception caught, what():%s, from %d:%s", 
						e.what(), e.line(), e.file());
			return -1;
		}
		catch(...)
		{
            mc_pack_close(pack);
			CWARNING_LOG("%s", "unknown exception catched");
			return -1;
		}
    }
	_request_buf.resize(mc_pack_get_size(pack));
    mc_pack_close(pack);
	return 0;
}

int Client::postpare()
{
	if(_talk.success != 0)
	{
		return -1;
	}
	_response_buf.resize(_talk.reshead.body_len);
	if(parse_result(_res))
	{
		CWARNING_LOG("parse result error");
		return -1;
	}
	return 0;
}

int NonblockClient::call(Client * client, const char * service, const char * function, idm::Bean * req, idm::Bean * res)
{
	if(client == 0 || service == 0 || function == 0)
	{
		CWARNING_LOG("bad parameter");
		return -1;
	}
	if(client->prepare(service, function, req, res))
	{
		CWARNING_LOG("make request failed");
		return -1;
	}
	if(_lcmgr->lnhead_nonblock_singletalk(service, &client->_talk, Lsc::var::Null::null, &_event_pool))
	{
		CWARNING_LOG("bad parameter");
		client->onNetError();
		return -1;
	}
	return 0;
}

int NonblockClient::call(CompackClient * client, const char * service, const char * function, idm::CompackBean * req, idm::CompackBean * res)
{
	if(client == 0 || service == 0 || function == 0)
	{
		CWARNING_LOG("bad parameter");
		return -1;
	}
	if(client->prepare(service, function, req, res))
	{
		CWARNING_LOG("make request failed");
		return -1;
	}
	if(_lcmgr->lnhead_nonblock_singletalk(service, &client->_talk, Lsc::var::Null::null, &_event_pool))
	{
		CWARNING_LOG("bad parameter");
		client->onNetError();
		return -1;
	}
	return 0;
}

int NonblockClient::call(HttpVarClient * client, const char * service, const char * function, Lsc::var::IVar &req, Lsc::var::Ref &res)
{
	if(client == 0 || service == 0 || function == 0)
	{
		CWARNING_LOG("bad parameter");
		return -1;
	}
	if(client->prepare(service, function, req, res))
	{
		CWARNING_LOG("make request failed");
		return -1;
	}
	if(_lcmgr->common_nonblock_singletalk(service, client->_event, Lsc::var::Null::null, &_event_pool))
	{
		CWARNING_LOG("bad parameter");
		client->onNetError();
		return -1;
	}
	return 0;
}

int NonblockClient::call(VarClient * client, const char * service, const char * function, const Lsc::var::IVar & req, Lsc::var::Ref & res)
{
	if(client == 0 || service == 0 || function == 0)
	{
		CWARNING_LOG("bad parameter");
		return -1;
	}
	if(client->prepare(service, function, req, res))
	{
		CWARNING_LOG("make request failed");
		return -1;
	}
	if(_lcmgr->lnhead_nonblock_singletalk(service, &client->_talk, Lsc::var::Null::null, &_event_pool))
	{
		CWARNING_LOG("bad parameter");
		client->onNetError();
		return -1;
	}
	return 0;
}


int NonblockClient::waitAll()
{
	int error = 0;
	while(true)
	{
		int result = _lcmgr->wait(-1, &_event_pool);
		if(result == -15)
		{
			return error;
		}
		else if(result <= 0)
		{
			return result;
		}
		Client::rpc_talkwith_t * talk;
		while((talk = (NsheadClientBase::rpc_talkwith_t *)_lcmgr->fetchFirstReadyReq(&_event_pool)))
		{
			
			if(talk->success == 0)
			{
				int res;
				if((res = talk->getClient()->postpare()))
				{
					error = res;
					CWARNING_LOG("%s", "read response error");
				}
			}
			else
			{
				CWARNING_LOG("network error: %s", lc::get_talk_errorinfo(talk->success));
				talk->getClient()->onNetError();
				error = talk->success;
			}
		}
		HttpVarClient::RpcLcClientHttpEvent * event;
		while((event = (HttpVarClient::RpcLcClientHttpEvent *)_lcmgr
				->fetchFirstReadyReqForCommon(&_event_pool)))
		{
			if(event->isError() == false)
			{
				int res;
				if((res = event->getClient()->postpare()))
				{
					error = res;
					CWARNING_LOG("%s", "read response error");
				}
			}
			else
			{
				CWARNING_LOG("network error: %s", 
						event->status_to_string(event->get_sock_status()).c_str());
				event->getClient()->onNetError();
				error = event->get_sock_status();
			}
		}
	}
}


int VarClient::call(const char * service, const char * function, const Lsc::var::IVar & req, Lsc::var::Ref & res)
{
	if(service == 0 || function == 0)
	{
		CWARNING_LOG("bad param");
		return -1;
	}
	if(prepare(service, function, req, res))
	{
		return -1;
	}
	if(_lcmgr->lnhead_singletalk(service, &_talk, Lsc::var::Null::null))
	{
		CWARNING_LOG("network error: %s", lc::get_talk_errorinfo(_talk.success));
		onNetError();
		return -1;
	}
	if(postpare())
	{
		return -1;
	}
	return 0;
}

int VarClient::prepare(const char * service, const char * function, const Lsc::var::IVar & req, Lsc::var::Ref & res)
{
	_error_msg = 0;
	_error_code = 0;
	int reqLen, resLen;
	_error_code = -1;
	_res = &res;
	if(_lcmgr->getReqAndResBufLen(service, reqLen, resLen))
	{
		CWARNING_LOG("bad service");
		return -1;
	}
	_request_buf.resize(reqLen);
	_response_buf.resize(resLen);
	if(make_request(service, function, req))
	{
		CWARNING_LOG("make request error");
		return -1;
	}
	this->_talk.reqhead.version = 1000;
	_talk.reqhead.log_id = _logid;
	strncpy(_talk.reqhead.provider, _provider.c_str(), 15);
	_talk.reqhead.provider[16] = 0;
	_talk.reqhead.body_len = _request_buf.size();
	_talk.reqbuf = _request_buf.start();

	_talk.maxreslen = _response_buf.size();
	_talk.resbuf = _response_buf.start();
	return 0;
}

int VarClient::make_request(const char * service, const char * function, const Lsc::var::IVar & req)
{
	int result;
	Lsc::ResourcePool rp;
	mc_pack_t * pack = mc_pack_open_w_rp(2, _request_buf.start(), _request_buf.size(), &rp);
	if(MC_PACK_PTR_ERR(pack))
	{
		CWARNING_LOG("%s", mc_pack_perror((long)pack));
		return -1;
	}
	mc_pack_t * header = mc_pack_put_object(pack, "header");
	if(MC_PACK_PTR_ERR(header))
	{
		CWARNING_LOG("%s", mc_pack_perror((long)header));
		return -1;
	}
    
	bool conn;
	if(_lcmgr->getServiceConnType(service, conn))
	{
		CWARNING_LOG("bad service");
		return -1;
	}
	if((result = mc_pack_put_bool(header, "connection", conn)))
	{
		CWARNING_LOG("%s", mc_pack_perror((long)result));
	}
	if(writer_header(service, header) != 0)
	{
		CWARNING_LOG("writer header error");
		return -1;
	}
    Lsc::string real_name;
    Lsc::string str_service = service;
    if (_name_map.get(str_service, &real_name) == Lsc::HASH_NOEXIST) {
        Lsc::ResourcePool rp;
        int error_num = 0;
        Lsc::var::IVar &lcclient_conf = _lcmgr->getServiceConf(service, &rp, &error_num);
        if (0 != error_num)
        {
            CWARNING_LOG("get RpcServiceName from LcClientManager error");
            _error_msg = "get RpcServiceName from LcClientManager error";
            return -1;
        }
        if (lcclient_conf.get("RpcServiceName").is_null())
        {
            real_name = str_service;
        }
        else
        {
            real_name = lcclient_conf["RpcServiceName"].c_str();
        }
        _name_map.set(str_service, real_name);
    }

	mc_pack_t * contents = mc_pack_put_array(pack, "content");
	if(MC_PACK_PTR_ERR(contents))
	{
		CWARNING_LOG("%s", mc_pack_perror((long)contents));
		return -1;
	}

	mc_pack_t * content = mc_pack_put_object(contents, NULL);
	if(MC_PACK_PTR_ERR(content))
	{
		CWARNING_LOG("%s", mc_pack_perror((long)content));
		return -1;
	}

	if((result = mc_pack_put_str(content, "service_name", real_name.c_str())))
	{
		CWARNING_LOG("%s", mc_pack_perror((long)result));
		return -1;
	}
	if((result = mc_pack_put_int64(content, "id", gen_id())))
	{
		CWARNING_LOG("%s", mc_pack_perror(result));
		return -1;
	}
	if((result = mc_pack_put_str(content, "method", function)))
	{
		CWARNING_LOG("%s", mc_pack_perror(result));
		return -1;
	}
	mc_pack_t * params = mc_pack_put_object(content, "params");
	if(MC_PACK_PTR_ERR(params))
	{
		CWARNING_LOG("%s", mc_pack_perror((long)params));
		return -1;
	}

	try
	{
		if(!req.is_null())
		{
			_ser.serialize(req, params);
		}
	}
	catch(Lsc::Exception &e)
	{
		CWARNING_LOG("Lsc::Exception caught, what():%s, from %d:%s", 
				e.what(), e.line(), e.file());
		return -1;
	}
	catch(...)
	{
		CWARNING_LOG("%s", "unknown exception catched");
		return -1;
	}
	_request_buf.resize(mc_pack_get_size(pack));
	return 0;
}

int VarClient::parse_result(Lsc::var::Ref * res)
{
	int result;
	Lsc::ResourcePool rp;
	mc_pack_t * pack = mc_pack_open_r_rp(_response_buf.start(), _response_buf.size(), &rp);
	if(MC_PACK_PTR_ERR(pack))
	{
		CWARNING_LOG("%s", mc_pack_perror((long)pack));
		return -1;
	}
	int64_t id;
	mc_pack_t * contents = mc_pack_get_array(pack, "content");
	if(MC_PACK_PTR_ERR(contents))
	{
		CWARNING_LOG("%s", mc_pack_perror((long)contents));
		return -1;
	}
	mc_pack_t * content = mc_pack_get_object_arr(contents, 0);
	if(MC_PACK_PTR_ERR(content))
	{
		CWARNING_LOG("%s", mc_pack_perror((long)content));
		return -1;
	}
	if((result = mc_pack_get_int64(content, "id", (mc_int64_t*)&id)))
	{
		CWARNING_LOG("%s", mc_pack_perror(result));
		return -1;
	}

    if (_checkid) {
	    if(id != _id)
	    {
		    CWARNING_LOG("unmatched id: %ld", id);
		    return -1;
	    }
    }

	const mc_pack_t * err;
	err = mc_pack_get_object(content, "error");
	if(!MC_PACK_PTR_ERR(err))
	{
		if(mc_pack_get_int32(err, "code", &_error_code))
		{
		}
		const char * str = mc_pack_get_str_def(err, "message", "");
		_error_msg = str;
		str = mc_pack_get_str_def(err, "data", "");
		_error_data = str;
		onError(err);
		return -1;
	}
	try
	{
		*res = _deser.deserialize(content);
		_error_code = 0;
		return 0;
	}
	catch(Lsc::Exception &e)
	{
		CWARNING_LOG("Lsc::Exception caught, what():%s, from %d:%s", 
				e.what(), e.line(), e.file());
		return -1;
	}
	catch(...)
	{
		CWARNING_LOG("%s", "unknown exception catched");
		return -1;
	}
}

int VarClient::postpare()
{
	if(_talk.success != 0)
	{
		return -1;
	}
	_response_buf.resize(_talk.reshead.body_len);
	if(parse_result(_res))
	{
		CWARNING_LOG("parse result error");
		return -1;
	}
	return 0;
}



}
