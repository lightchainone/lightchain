
#include <stdlib.h>
#include <vector>
#include <string.h>
#include "http_client.h"
#include "http_parser.h"
#include <mc_pack_rp.h>
namespace lcrpc {

class HttpSender
{
plclic:
    void add_header(const char * key, const char * value)
    {
        print(key);
        print(": ");
        print(value);
        print("\r\n");
    }
    HttpSender(void * buffer, size_t length) 
        : _buffer((char *)buffer),
        _length(length),
        _pos(0)
    {
    }
    int post(const char * url)
    {
        if(url == 0)
        {
            CWARNING_LOG("bad parameter");
            return -1;
        }
        if (strncasecmp(url, "http://", 7) != 0) {
            CWARNING_LOG("bad url, must start by http://");
            return -1;
        }
        nprint("POST ", 5);
        const char * hostend = strchr(url+7, '/');
        if(hostend == NULL)
        {
            nprint("/ HTTP/1.1\r\n", sizeof("/ HTTP/1.1\r\n") - 1);
        }
        else
        {
            print(hostend);
            nprint(" HTTP/1.1\r\n", sizeof(" HTTP/1.1\r\n") - 1);
            nprint("Host: ", sizeof("Host: ") - 1);
            nprint(url+7, hostend - url-7);
            print("\r\n");
        }
        return 0;
    }
    void set_contenttype(const char * value)
    {
        nprint("Content-Type: ", sizeof("Content-Type: ") - 1);
        print(value);
        print("\r\n");
    }
    void set_contenttype(const char * value, const char * charset)
    {
        nprint("Content-Type: ", sizeof("Content-Type: ") - 1);
        print(value);
        nprint("; charset=", sizeof("; charset=") -1);
        print(charset);
        
        print("\r\n");
    }
    void * get_content_start()
    {
        print("Content-Length:           \r\n\r\n");
        _length_pos = _pos - 10;
        return _buffer + _pos;
    }
    size_t get_available_space()
    {
        return _length - _length_pos;
    }
    void update_content_size(size_t s)
    {
        int t = sprintf(_buffer + _length_pos, "%zu", s);
        *(_buffer + _length_pos + t)= ' ';
        _pos += s;
    }
    size_t size()
    {
        return _pos;
    }
protected:
    void print(const char * str)
    {
        if(str == 0)
        {
            CWARNING_LOG("bad parameter");
            throw Lsc::BadArgumentException() << BSL_EARG;;
        }
        size_t n = strlen(str);
        if(n + _pos >= _length)
        {
            CWARNING_LOG("not enough buffer");
            throw Lsc::OutOfBoundException() << BSL_EARG;;
        }
        memcpy(_buffer+_pos, str, n);
        _pos += n;
    }
    void nprint(const char * str, size_t n)
    {
        if(str == 0)
        {
            throw Lsc::BadArgumentException() << BSL_EARG;;
        }
        if(n + _pos >= _length)
        {
            CWARNING_LOG("not enough buffer");
            throw Lsc::OutOfBoundException() << BSL_EARG;;
        }
        memcpy(_buffer+_pos, str, n); 
        _pos += n;
    }
private:
    char * _buffer;
    size_t _length;
    size_t  _pos;
    size_t _length_pos;
};

void HttpHeader::add_header(const char * key, const char * value)
{
    if(_httpsender == 0)
    {
        CWARNING_LOG("bad parameter");
        throw Lsc::BadArgumentException() << BSL_EARG;
    }
    _httpsender->add_header(key, value);
}

int HttpVarClient::call(const char * service, const char * function, Lsc::var::IVar &req,
        Lsc::var::Ref &res)
{
	if(service == 0 || function == 0)
	{
		CWARNING_LOG("bad param");
        _error_msg = "bad param";
		return -1;
	}
	if(prepare(service, function, req, res))
	{
		return -1;
	}
	if(_lcmgr->common_singletalk(service, _event, Lsc::var::Null::null))
	{
        CWARNING_LOG("network error: %s", _event->status_to_string(_event->get_sock_status()).c_str());
		onNetError();
		return -1;
	}
	if(postpare())
	{
		return -1;
	}
	return 0;
}

int HttpVarClient::prepare(const char * service, const char * function, Lsc::var::IVar &req,
			Lsc::var::Ref &res)
{
	_error_msg = 0;
	_error_code = 0;
	int reqLen;
	int resLen;
	_error_code = -1;
	_res = &res;
	if(_lcmgr->getReqAndResBufLen(service, reqLen, resLen))
	{
		CWARNING_LOG("bad service");
        _error_msg = "bad service";
		return -1;
	}
	_request_buf.resize(reqLen);
	_response_buf.resize(resLen);
    HttpSender sender(_request_buf.start(), _request_buf.size());
    try
    {
        Lsc::string url;
        Lsc::string str_service = service;
        if (_url_map.get(str_service, &url) == Lsc::HASH_NOEXIST) {
            Lsc::ResourcePool rp;
            int error_num = 0;
            Lsc::var::IVar &lcclient_conf = _lcmgr->getServiceConf(service, &rp, &error_num);
            if (0 != error_num)
            {
                CWARNING_LOG("get url from LcClientManager error");
                _error_msg = "get url from LcClientManager error";
                return -1;
            }
            if (lcclient_conf.get("Httpurl").is_null())
            {
                CWARNING_LOG("Httpurl not exist in service[%s]", service);
                _error_msg = "Httpurl not exist in service";
                return -1;
            }
            url = lcclient_conf["Httpurl"].c_str();
            _url_map.set(str_service, url);
        }
        sender.post(url.c_str());
        if(_charset == "")
        {
            sender.set_contenttype("application/ixintui.mcpack-rpc", "GBK");
        }
        else
        {
            sender.set_contenttype("application/ixintui.mcpack-rpc", _charset.c_str());
        }
        sender.add_header("Connection", "close");
        HttpHeader httpheader(&sender);
        write_header(httpheader);
        if(make_request(function, &sender, req))
        {
            CWARNING_LOG("make request error");
            _error_msg = "make request error";
            return -1;
        }
    }
    catch(Lsc::Exception & e)
    {
		CWARNING_LOG("%s, %s", e.what(), e.stack());
        _error_msg = "prepare request error";
        _error_msg.append(e.what());
		return -1;
    }
	catch(...)
	{
		CWARNING_LOG("%s", "unknown exception catched");
        _error_msg = "unknown exception";
		return -1;
	}
    _event->set_writebuf(_request_buf.start(), reqLen);
    _event->set_readbuf(_response_buf.start(), resLen);
    _event->set_writebuf_size(sender.size());
	return 0;
}

int HttpVarClient::make_request(const char * function, HttpSender * sender, const Lsc::var::IVar &req)
{
	int result;
	Lsc::ResourcePool rp;
    char *start_pos = (char *)sender->get_content_start();
	mc_pack_t * content = mc_pack_open_w_rp(2, start_pos, sender->get_available_space(), &rp);
	if(MC_PACK_PTR_ERR(content))
	{
		CWARNING_LOG("%s", mc_pack_perror((long)content));
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
	if((result = mc_pack_put_str(content, "jsonrpc", "2.0")))
	{
		CWARNING_LOG("%s", mc_pack_perror(result));
		return -1;
	}

	mc_pack_t * params = mc_pack_put_array(content, "params");
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
    mc_pack_close(content);
	sender->update_content_size(mc_pack_get_size(content));
	return 0;
}

int HttpVarClient::postpare()
{
	if(_event->isError() != false)
	{
		return -1;
	}
	if(parse_result(_res))
	{
		CWARNING_LOG("parse result error");
		return -1;
	}
	return 0;
}

int HttpVarClient::parse_result(Lsc::var::Ref * res)
{
	int result;
	Lsc::ResourcePool rp;
    http_parser parser;
    if((int)parser.parse_response(_event->get_read_buffer(), _event->get_readbuf_size()) != _event->get_readbuf_size())
	{
		CWARNING_LOG("http response error");
        _error_msg = "http response error";
		return -1;
    }
    if(parser.get_status_code() != 200)
    {
		CWARNING_LOG("http response error %d", parser.get_status_code());
        char error_code[10];
        snprintf(error_code,sizeof(error_code), "%d", parser.get_status_code());
        _error_msg = "http error code :";
        _error_msg.append(error_code);
		return -1;
    }
	mc_pack_t * content = mc_pack_open_r_rp((const char *)_event->get_read_buffer() +
_event->get_http_headlen(), _event->get_http_bodylen(), &rp);
	int64_t id;
	if(MC_PACK_PTR_ERR(content))
	{
        _error_msg = mc_pack_perror((long)content);
		CWARNING_LOG("%s", _error_msg.c_str());
		return -1;
	}
	if((result = mc_pack_get_int64(content, "id", (mc_int64_t*)&id)))
	{
        _error_msg = mc_pack_perror(result);
		CWARNING_LOG("%s", _error_msg.c_str());
		return -1;
	}
	if(id != _id)
	{
		CWARNING_LOG("unmatched id: %ld", id);
        _error_msg = "unmatched id";
		return -1;
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
        Lsc::var::IVar &r = _deser.deserialize(content);
        *res = r["result"];
        _error_code = 0;
        return 0;
        
	}
	catch(Lsc::Exception &e)
	{
		CWARNING_LOG("Lsc::Exception caught, what():%s, from %d:%s", 
				e.what(), e.line(), e.file());
        _error_msg = "Lsc:: Exception caught";
		return -1;
	}
	catch(...)
	{
		CWARNING_LOG("%s", "unknown exception catched");
        _error_msg = "unknown exception catched";
		return -1;
	}
}

}

