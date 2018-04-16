
#include "compack_client.h"
#include <mc_pack_rp.h>
namespace lcrpc
{
int CompackClient::call(const char * service, const char * function, idm::CompackBean * req,
                        idm::CompackBean * res)
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
	if(_lcmgr->lnhead_singletalk(service, &_talk))
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

int CompackClient::prepare(const char * service, const char * function, idm::CompackBean * req,
                           idm::CompackBean * res)
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

int CompackClient::parse_result(idm::CompackBean * res)
{
	int result;
	Lsc::ResourcePool rp;
	mc_pack_t * pack = mc_pack_open_r_rp(_response_buf.start(), _response_buf.size(), &rp);
	if(MC_PACK_PTR_ERR(pack))
	{
		CWARNING_LOG("%s", mc_pack_perror((long)pack));
		return -1;
	}
#if 0
    char buf[1024];
    mc_pack_pack2json(pack, buf, sizeof(buf));
    printf("%s\n", buf);
#endif
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
		res->load((char *)mc_pack_get_buffer(content), mc_pack_get_size(content));
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
}

int CompackClient::make_request(const char * service, const char * function, idm::CompackBean * req)
{
    int buffer_size = 0;
    compack::buffer::buffer bufwrap(_request_buf.start(), _request_buf.size());
    try
    {
        compack::buffer::Object obj(&bufwrap);
        {
		    compack::buffer::Object header(&obj, "header");
			bool conn;
			if(_lcmgr->getServiceConnType(service, conn))
			{
				CWARNING_LOG("bad service");
				return -1;
			}
			header.putNum<bool>("connection", conn);
			if(writer_header(service, &header) != 0)
			{
				CWARNING_LOG("writer header error");
				return -1;
			} 
        }
        {
            compack::buffer::Array contents(&obj, "content");
            {
				compack::buffer::Object content(&contents);
				content.putString("service_name", service);
				content.putNum<int64_t>("id", gen_id());
				content.putString("method", function);
				compack::buffer::Object params(&content, "params");
	            try 
			    {
				    if(req != 0)
				    {
					    req->save(&params);
				    }
			    }
			    catch(Lsc::Exception &e)
			    {
				    CWARNING_LOG("idm save Lsc::Exception caught, what():%s, from %d:%s", 
							    e.what(), e.line(), e.file());
				    return -1;
			    }
			    catch(...)
			    {
				    CWARNING_LOG("%s", "unknown exception catched");
				    return -1;
			    }
            }
        }
        buffer_size = obj.size();
    }
    catch(Lsc::Exception &e)
    {
        CWARNING_LOG("compack throw Lsc::Exception caught, what():%s, from %d:%s", 
	            e.what(), e.line(), e.file());
		return -1;
	}
    catch(...)
	{
		CWARNING_LOG("%s", "unknown exception catched");
		return -1;
	}
	_request_buf.resize(buffer_size);
	return 0;
}

int CompackClient::postpare()
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




















