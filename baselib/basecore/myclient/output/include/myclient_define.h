
#ifndef MYCLIENT_DEFINE_H_
#define MYCLIENT_DEFINE_H_


#define MYCLIENT_MASTER_HOST_TYPE                   "mh"

#define MYCLIENT_SLAVE_LOCAL_HOST_TYPE              "slh"

#define MYCLIENT_SLAVE_REMOTE_HOST_TYPE            "srh"

#define MYCLIENT_AVERAGE_HOST_TYPE                  "ah"




#define MAX_SERVER_NUM 64

#define DEFAULT_MONITOR_TIME 3

#define DEFAULT_QUERYF_BUFFER_SIZE 512

#define MAX_ERR_DESC_SIZE 64

#define MAX_CONF_NAME 128


enum myclient_error{
	success=0,
	
	inputparam_error=-1,
	filesystem_error=-2,
	confparam_error=-3,
	
	pool_init_fail=-101,
	pool_getconnection_fail=-102,
	pool_has_not_init=-103,
	
	connection_connect_fail=-201,
	connection_not_init=-202,
	connection_escape_fail=-203,
	connection_query_fail=-204,
	
	
};

#endif 
