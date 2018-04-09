
#include <com_log.h>
#include <lc_server/lcserver_core.h>
#include <compack/compack.h>
#include <mc_pack_rp.h>

#include "compack_servlet.h"

namespace lcrpc
{
int CompackServlet::process(void * input, unsigned int input_length, 
                            void * output, unsigned int out_length)
{
    Lsc::ResourcePool rp;
	mc_pack_t * pack = mc_pack_open_rw_rp((char *)input, input_length, 
			&rp);
	if(MC_PACK_PTR_ERR(pack))
	{
		CWARNING_LOG("%s", mc_pack_perror((long)pack));
		return -1;
	}
#ifdef DEBUG
	char buf[1024];
	mc_pack_pack2json(pack, buf, sizeof(buf));
	printf("%s\n", buf);
#endif
	
    try {
        compack::buffer::buffer bufwrap((char *)output, out_length);
        compack::buffer::Object result(&bufwrap);
	    mc_pack_t * header = mc_pack_get_object(pack, "header"); 
	    if(!MC_PACK_PTR_ERR(header))
	    {
		    readHeader(header);
	    }
	    bool conn_type;
	    if(mc_pack_get_bool(header, "connection", &conn_type) == 0)
	    {
		    if(conn_type)
		    {
		 	    lc_server_set_session_connect_type(LCSVR_LONG_CONNECT);
		    }
		    else
		    {
			    lc_server_set_session_connect_type(LCSVR_SHORT_CONNECT);
		    }
	    }
        {
        compack::buffer::Array result_contents(&result, "content");
	    mc_pack_t * contents = mc_pack_get_array(pack, "content");
	    if(MC_PACK_PTR_ERR(contents))
	    {
		    CWARNING_LOG("%s", mc_pack_perror((long)contents));
            mc_pack_close(pack);
		    return -1;
	    }
	    mc_pack_item_t item;
	    if(mc_pack_first_item(contents, &item) != 0)
	    {
		    CWARNING_LOG("%s", "bad data");
            mc_pack_close(pack);
		    return -1;
	    }
	    do
	    {
		    const mc_pack_t * inputpack;
		    if(mc_pack_get_pack_from_item(&item, &inputpack) == 0)
		    {
                compack::buffer::Object fun_res(&result_contents);
			    _error_msg = 0;
			    _error_data = 0;
			    if(call(inputpack, &fun_res) < 0)
			    {
				    CWARNING_LOG("%s", "callee error");
                    mc_pack_close(pack);
				    return -1;
			    }
		    }
		    else
		    {
			    CWARNING_LOG("%s", "read data error");
                mc_pack_close(pack);
			    return -1;
		    }
	    }
	    while(mc_pack_next_item(&item, &item) == 0);
        }
        {
        compack::buffer::Object res_header(&result, "header");
	    if(writeHeader(&res_header))
		{
			CWARNING_LOG("%s", "write res_header error");
            mc_pack_close(pack);
			return -1;
		}
        }
    
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

    mc_pack_close(pack);
	return 0;
}

int CompackServlet::call(const mc_pack_t * request, compack::buffer::Object * response)
{
    try
    {
	    int64_t id;
	    if(mc_pack_get_int64(request, "id", (mc_int64_t *)&id))
	    {
		    CWARNING_LOG("%s", "no id found");
		    return -1;
	    }
        response->putNum<int64_t>("id", id);
	    const char * func = mc_pack_get_str(request, "method");
	    if(MC_PACK_PTR_ERR(func))
	    {
		    CWARNING_LOG("%s", mc_pack_perror((long)func));
		    return -1;
	    }
	    const mc_pack_t * param = mc_pack_get_object(request, "params");
	    char buf[512];
	    if((int)(long)param == MC_PE_NOT_FOUND || (int)(long)param == MC_PE_NULL_VALUE)
	    {
		    param = mc_pack_open_w(mc_pack_get_version(request), buf, sizeof(buf)/2, 
			buf + sizeof(buf)/2, sizeof(buf)/2);
	    }
	    else if(MC_PACK_PTR_ERR(param))
	    {
		    CWARNING_LOG("%s", mc_pack_perror((long)param));
		    return -1;
	    }
	    try
	    {
		    return dispatch(id, func, param, response);
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
int CompackServlet::writeError(compack::buffer::Object * response)
{
    try
    {
        compack::buffer::Object err(response, "error");
        err.putString("message", _error_msg);
        if(_error_data != 0)
	    {
            err.putString("data", _error_data);
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
	return 0;
}
}


















