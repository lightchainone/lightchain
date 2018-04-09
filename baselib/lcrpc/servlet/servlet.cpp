#include "servlet.h"
#include <com_log.h>
#include <lc_server/lcserver_core.h>
#include <mc_pack_rp.h>



namespace lcrpc
{

const char * const ERROR_MSG[] = {
	"success",
	"unknown"
};

int Servlet::process(void * input, unsigned int input_length, 
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
	int version = mc_pack_get_version(pack);
	mc_pack_t * result = mc_pack_open_w_rp(version, (char *)output, out_length, 
			&rp);
	if(MC_PACK_PTR_ERR(result))
	{
		CWARNING_LOG("%s", mc_pack_perror((long)result));
	    mc_pack_close(pack);
		return -1;
	}

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
	mc_pack_t * result_contents = mc_pack_put_array(result, "content");
	if(MC_PACK_PTR_ERR(result_contents))
	{
		CWARNING_LOG("%s", mc_pack_perror((long)result_contents));
	    mc_pack_close(result);
	    mc_pack_close(pack);
		return -1;
	}


	mc_pack_t * contents = mc_pack_get_array(pack, "content");
	if(MC_PACK_PTR_ERR(contents))
	{
		CWARNING_LOG("%s", mc_pack_perror((long)contents));
	    mc_pack_close(result);
	    mc_pack_close(pack);
		return -1;
	}

	mc_pack_item_t item;
	if(mc_pack_first_item(contents, &item) != 0)
	{
		CWARNING_LOG("%s", "bad data");
	    mc_pack_close(result);
	    mc_pack_close(pack);
		return -1;
	}
	do
	{
		const mc_pack_t * inputpack;
		if(mc_pack_get_pack_from_item(&item, &inputpack) == 0)
		{
			mc_pack_t * fun_res = mc_pack_put_object(result_contents, NULL);
			if(MC_PACK_PTR_ERR(fun_res))
			{
				CWARNING_LOG("%s", mc_pack_perror((long)contents));
	            mc_pack_close(result);
	            mc_pack_close(pack);
				return -1;
			}
			_error_msg = 0;
			_error_data = 0;
			if(call(inputpack, fun_res) < 0)
			{
				CWARNING_LOG("%s", "callee error");
	            mc_pack_close(result);
	            mc_pack_close(pack);
				return -1;
			}
		}
		else
		{
			CWARNING_LOG("%s", "read data error");
	        mc_pack_close(result);
	        mc_pack_close(pack);
			return -1;
		}
	}
	while(mc_pack_next_item(&item, &item) == 0);
	mc_pack_t * res_header = mc_pack_put_object(result, "header");
	if(MC_PACK_PTR_ERR(res_header))
	{
		CWARNING_LOG("%s", "res_header");
	    mc_pack_close(result);
	    mc_pack_close(pack);
		return -1;
	}
	if(writeHeader(res_header))
	{
		CWARNING_LOG("%s", "write res_header error");
	    mc_pack_close(result);
	    mc_pack_close(pack);
		return -1;
	}
#ifdef DEBUG
	mc_pack_pack2json(result, buf, sizeof(buf));
	printf("%s\n", buf);
#endif
	mc_pack_close(result);
	mc_pack_close(pack);
	return 0;
}

int Servlet::call(const mc_pack_t * request, mc_pack_t * response)
{
	int64_t id;
	if(mc_pack_get_int64(request, "id", (mc_int64_t *)&id))
	{
		CWARNING_LOG("%s", "no id found");
		return -1;
	}
    _id = id;
	if(mc_pack_put_int64(response, "id", id))
	{
		CWARNING_LOG("%s", "write id error");
		return -1;
	}
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
#if 0
	const mc_pack_t * result_param = mc_pack_get_array(request, "result_params");
	if(MC_PACK_PTR_ERR(result_param))
	{
		result_param = 0;
	}
#endif
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
int Servlet::writeError(mc_pack_t * response)
{
	int result;
	mc_pack_t * err = mc_pack_put_object(response, "error");
	if(MC_PACK_PTR_ERR(err))
	{
		CWARNING_LOG("%s", mc_pack_perror((long)err));
		return -1;
	}
	if((result = mc_pack_put_str(err, "message", _error_msg))< 0)
	{
		CWARNING_LOG("%s", mc_pack_perror(result));
		return -1;
	}
	if(_error_data != 0)
	{
		if((result = mc_pack_put_str(err, "data", _error_data)) < 0)
		{
			CWARNING_LOG("%s", mc_pack_perror(result));
			return -1;
		}
	}
	return 0;
}

pthread_key_t LcRpcServer::rl_key;
pthread_once_t LcRpcServer::rl_once = PTHREAD_ONCE_INIT;

static const int LCPOOLDATAINDEX=10;

void LcRpcServer::registerWithLc(lc_server_t * lc) 
{
	
	
	
	lc_server_set_callback(lc, lc_call_back_entry);
	*(LcRpcServer **)&lc->pool_data[LCPOOLDATAINDEX] = this;

}

int LcRpcServer::lc_call_back_entry()
{
	lc_server_t * lc = lc_server_get_server();
	LcRpcServer * sev = *(LcRpcServer **)&lc->pool_data[LCPOOLDATAINDEX];
	return sev->lc_call_back();
}

int LcRpcServer::lc_call_back()
{
	Servlet * servlet = getServlet();
	lnhead_t *req_head = (lnhead_t *) lc_server_get_read_buf();
    if (NULL != req_head && NULL != servlet) {
        servlet->setLogid(req_head->log_id);
    }
	unsigned int readbuf_len = lc_server_get_read_size() - sizeof(lnhead_t);
	lnhead_t *res_head = (lnhead_t *) lc_server_get_write_buf();
	unsigned int writebuf_len = lc_server_get_write_size() - sizeof(lnhead_t);
	int result = servlet->process(req_head + 1, readbuf_len, 
			res_head + 1, writebuf_len);
	if(result < 0)
	{
		return result;
	}
	int len = mc_pack_get_length_partial((const char *)(res_head + 1), 
			writebuf_len - sizeof(lnhead_t));
	if(len <= 0)
	{
		CWARNING_LOG("%s", "bad data");
		return -1;
	}
	res_head->body_len = len;
	return result;
}

Servlet * LcRpcServer::getServlet()
{
	pthread_once(&rl_once, initServlet);
	void * ptr;
	ptr = pthread_getspecific(rl_key);
	if(ptr == NULL)
	{
		ptr = createServlet();
		pthread_setspecific(rl_key, ptr);
	}
	return (Servlet*)ptr;
}

void LcRpcServer::initServlet()
{
	pthread_key_create(&rl_key, deleteServlet);
}

void LcRpcServer::deleteServlet(void * ptr)
{
	if(ptr != NULL)
	{
		delete (Servlet *)ptr;
	}

}

}




