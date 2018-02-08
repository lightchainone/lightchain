#include "lcfw.h"


#if 0
static int update_server_callback()
{
    nshead_t *req_head = (nshead_t *) lc_server_get_read_buf();
    
	
	

    lc_log_setbasic(LC_LOG_REQSVR, "%s", req_head->provider);
    lc_log_setbasic(LC_LOG_SVRNAME, "%s", "update");
    

	
	return 0;
}
#endif


static int query_server_callback()
{
    nshead_t *req_head = (nshead_t *) lc_server_get_read_buf();
    
	
	

    lc_log_setbasic(LC_LOG_REQSVR, "%s", req_head->provider);
    lc_log_setbasic(LC_LOG_SVRNAME, "%s", "query");
    

	
	return 0;
}


int main(int argc, char **argv)
{
	lc_t * fw = lc_init("test", argc, argv);	
	if (fw == NULL) { 	
		return 0; 
	}
	lc_conf_data_t *conf = lc_open_conf(fw);	
	LCFW_ASSERT(conf != NULL, "open configure %s/%s error", fw->path, fw->file);

	
	LCFW_ASSERT(lc_load_log(fw) == 0, "open log error");

	
	
	lc_svr_vec_t *svrvec = lc_svr_vec_create();	
	LCFW_ASSERT(svrvec != NULL, "create lc_server_vec_error");
	lc_server_t *querysvr = lc_load_svr(fw, "query");	
	LCFW_ASSERT(fw->conf_build || querysvr != NULL, "create query server error");
	lc_server_set_callback(querysvr, query_server_callback);	
	lc_svr_vec_add(svrvec, querysvr, fw);	

#if 0
	lc_server_t *updatesvr = lc_load_svr(fw, "update");	
	LCFW_ASSERT(fw->conf_build || updatesvr != NULL, "create update server error");
	lc_server_set_callback(updatesvr, update_server_callback);	
	lc_svr_vec_add(svrvec, updatesvr, fw);	
#endif

	
	
	

	

	
	lc_builddone_exit(fw);

	LCFW_ASSERT(lc_svr_vec_run(svrvec) == 0, "run server vectors error");	
	lc_svr_vec_join(svrvec);	
	lc_svr_vec_destroy(svrvec);	

	
	lc_log_close();

	lc_close(fw);
	return 0;
}


