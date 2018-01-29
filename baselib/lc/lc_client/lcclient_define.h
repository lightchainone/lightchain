


#ifndef  __LCCLIENT_DEFINE_H_
#define  __LCCLIENT_DEFINE_H_

namespace lc {
    
    enum {
        CLASS_NSHEAD = 1,		  
        CLASS_HTTP = 2,		  
    };

	
	
	
	const char LCCLIENT_CONF_CONFIG[] = "LcClientConfig";		
	
	const char LCCLIENT_CONF_TYPE[] = "ConfType";			
	
	const char LCCLIENT_CONF_GALILEO[] = "Galileo";
		
	const char LCCLIENT_CONF_GALILEO_HOST[] = "GALILEO_HOST";
	const char LCCLIENT_CONF_GALILEO_CLUSTER[] = "GALILEO_CLUSTER";
	
	const char LCCLIENT_CONF_GALILEO_DIR[] = "DIR";
	
	const char LCCLIENT_CONF_GALILEO_FILE[] = "FILE";
	
	const char LCCLIENT_CONF_GALILEO_LOG[]      = "GALILEO_ZOO_LOG";
	const char LCCLIENT_CONF_GALILEO_LOGLEVEL[] = "GALILEO_ZOO_LOGLEVEL";
	
	
	const char LCCLIENT_CONF_GALILEO_DATAPACKSIZE[] = "GALILEO_DATA_PACK_SIZE";
	
	const char LCCLIENT_CONF_GALILEO_REQSES[] = "REQUEST_RESOURCE";
	
	const char LCCLIENT_CONF_GALILEO_REQSES_RESADDR[] = "RES_ARR";
	
	const char LCCLIENT_CONF_GALILEO_REQSES_RESADDR_NAME[] = "NAME";
	
	const char LCCLIENT_CONF_GALILEO_REQSES_RESADDR_ADDR[] = "ADDR";
	
	const char LCCLIENT_CONF_GALILEO_REQSES_RESADDR_PROXY[] = "PROXY";
	
	const char LCCLIENT_CONF_GALILEO_REGSES[] = "REGISTER_RESOURCE";
	
	const char LCCLIENT_CONF_GALILEO_REGSES_RESADDR[] = "RES_ARR";
	
	const char LCCLIENT_CONF_GALILEO_REGSES_RESADDR_ADDR[] = "ADDR";
	
	const char LCCLIENT_CONF_GALILEO_REGSES_RESADDR_DATA[] = "DATA";

	
	const char GALILEO_CONF_GALILEO_ABSTRACT[] = "abstract";
	
	const char GALILEO_CONF_GALILEO_ENTITY[] = "entity";
	
	const char GALILEO_CONF_GALILEO_SERVICE[] = "services";
	
	const char GALILEO_CONF_GALILEO_SERVICE_NAME[] = "service_name";
		
	const char GALILEO_CONF_GALILEO_SERVICE_PORT[] = "service_port";
	
	const char GALILEO_CONF_GALILEO_CONNTYPE[] = "service_conn_type";
	
	const char GALILEO_CONF_GALILEO_CONNTO[] = "service_ctimeout";
	
	const char GALILEO_CONF_GALILEO_READTO[] = "service_rtimeout";
		
	const char GALILEO_CONF_GALILEO_WRITETO[] = "service_wtimeout";
		
	const char GALILEO_CONF_GALILEO_NAME[] = "name";
	
	const char GALILEO_CONF_GALILEO_IP[] = "ip";
	
	const char GALILEO_CONF_GALILEO_HOST[] = "hostname";
	
	const char GALILEO_CONF_GALILEO_VERSION[] = "version";
	
	const char GALILEO_CONF_GALILEO_MASTER[] = "master";
	
	const char GALILEO_CONF_GALILEO_RANGE[] = "data_range";
	
	const char GALILEO_CONF_GALILEO_RANGE_NAME[] = "range";	  
	
	
	const char LCCLIENT_CONF_LCCLIENT[] = "LcClient";
	
	const char LCCLIENT_CONF_SERVICE[] = "Service";
		
	const char LCCLIENT_CONF_SERVICE_NAME[] = "Name";
	
	const char LCCLIENT_CONF_SERVICE_PORT[] = "DefaultPort";
	
	const char LCCLIENT_CONF_SERVICE_CONNECT[] = "DefaultConnectTimeOut";
	
	const char LCCLIENT_CONF_SERVICE_READ[] = "DefaultReadTimeOut";
	
	const char LCCLIENT_CONF_SERVICE_WRITE[] = "DefaultWriteTimeOut";
		
	const char LCCLIENT_CONF_SERVICE_MAXCONNECT[] = "DefaultMaxConnect";
	
	const char LCCLIENT_CONF_SERVICE_RETRY[] = "DefaultRetry";
	
	const char LCCLIENT_CONF_SERVICE_CONNECTTYPE[] = "DefaultConnectType";
	
	const char LCCLIENT_CONF_SERVICE_LINGER[] = "DefaultLinger";
	
	const char LCCLIENT_CONF_SERVICE_SENDBUF[] = "SendBuf";
    	
	const char LCCLIENT_CONF_SERVICE_RECVBUF[] = "RecvBuf";
		
	const char LCCLIENT_CONF_SERVICE_WAITNUM[] = "DefaultAsyncWaitingNum";
	
	const char LCCLIENT_CONF_SERVICE_CONNALL[] = "ConnectAll";	 	
	
	const char LCCLIENT_CONF_SERVICE_FETCHCONNTIME[] = "DefaultFetchConnMaxTime";

	
	const char LCCLIENT_CONF_STRATEGY[] = "CurrStrategy";
	
	const char LCCLIENT_CONF_STRATEGY_NAME[] = "ClassName";		  

	
	const char LCCLIENT_CONF_HEALTHY[] = "CurrHealthy";
	
	const char LCCLIENT_CONF_HEALTHY_NAME[] = "ClassName";	

	
	const char LCCLIENT_CONF_SERVER[] = "Server";
	
	const char LCCLIENT_CONF_SERVER_IP[] = "IP";
	
	const char LCCLIENT_CONF_SERVER_PORT[] = "Port";	
		
	const char LCCLIENT_CONF_SERVER_CONNECT[] = "ConnectTimeOut";
	
	const char LCCLIENT_CONF_SERVER_READ[] = "ReadTimeOut";
	
	const char LCCLIENT_CONF_SERVER_WRITE[] = "WriteTimeOut";
		
	const char LCCLIENT_CONF_SERVER_MAXCONNECT[] = "MaxConnect";
	
	const char LCCLIENT_CONF_SERVER_LINGER[] = "Linger";
		
	const char LCCLIENT_CONF_SERVER_RANGE[] = "Range";
	
	const char LCCLIENT_CONF_SERVER_VERSION[] = "Version";
		
	const char LCCLIENT_CONF_SERVER_MASTER[] = "Master";
	
	const char LCCLIENT_CONF_SERVER_TAG[] = "Tag";
	
	const char LCCLIENT_CONF_SERVER_WAITNUM[] = "AsyncWaitingNum";    
	
	const char LCCLIENT_CONF_REACTOR[] = "Reactor";		  
	
	const char LCCLIENT_DEFAULT_CONFDIR[] = "./conf/";
	
	const char LCCLIENT_DEFAULT_CONFFILE[] = "lc_client.conf";
	
	const char LCCLIENT_DEFAULT_GALILEOCONF[] = "./galileo_backup.conf"; 
	
	const char LCCLIENT_DEFAULT_STRATEGY[] = "LcClientStrategy";
		
	const char LCCLIENT_DEFAULT_HEALTHY[] = "LcClientHealthyChecker";

	
	const char LCCLIENT_CONNTYPE_LONG[] = "LONG";
		
	const char LCCLIENT_CONNTYPE_SHORT[] = "SHORT";		  
	
	
	const int GALILEO_BUFFER_SIZE = 131072;
		
	const int LCCLIENT_IP_LEN = 16;		  		  
	
	
	const int LCCLIENT_FETCHCONN_SLEEPTIME = 1;		  
	
	
	const int LCCLIENT_DEFAULT_RETRY_TIME = 5;
		
	const int LCCLIENT_DEFAULT_CAPACITY = 50;
		
	const int LCCLIENT_DEFAULT_CONNTO = 100;
		
	const int LCCLIENT_DEFAULT_PORT = -1;
		
	const int LCCLIENT_DEFAULT_READTO = 500;
       	 	
	const int LCCLIENT_DEFAULT_WRITETO = 500;
		
	const int LCCLIENT_DEFALUT_LINGER = 0;
		
	const int LCCLIENT_DEFAULT_REQBUFLEN = 1048576;
	
	const int LCCLIENT_DEFALUT_REABUFLEN = 1048576;		  

	
	const int LCCLIENT_PATH_LEN = 1024;		          
	
	const int LCCLIENT_CONFSAVE_LEN = 10240;		  

	
	const int LCCLIENT_MANAGER_WAITMSTIME = 10;	          
	
	const int LCCLIENT_HASHMAP_SIZE = 1024;		  	  
	
	
	const int LCCLIENT_CONFTYPE_GALILEO = 0;
		
	const int LCCLIENT_CONFTYPE_LOCAL = 1;

	
	const int LCCLIENT_CONFTYPE_HYBRID1 = 2;
	
	const int LCCLIENT_CONFTYPE_HYBRID2 = 3;
	
	
	const int LCCLIENT_SERVER_WAITNUM = 80;	

	
	const int LCCLIENT_SELECTEDSERVER_SIZE = 1000;

	
	const int LCCLIENT_DIR_LEN = 256;

	
	const int LCCLIENT_FILE_LEN = 256;

	
	const int LCCLIENT_SERVER_DEFAULT_MASTER = 1;

	
	const int LCCLIENT_SERVERNAME_LEN = 1024;

	
	const int LCCLIENT_FETCHCONN_TIME = 0;

	
	const int LCCLIENT_HOSTNAME_STR_LEN = 128;

	
	const char CHASH_RECORDSIZE[] = "RecordSize";

	
	const char CHASH_FAILSCALE[] = "FailScale";

	
	const char CHASH_MINVISITRATIO[] = "MinVisitRatio";

	
	const char CHASH_LVTQUEUESIZE[] = "LvtQueueSize";

    
    const char CONF_SERVER_ID[] = "reqid";

    
    const int GALILEO_QUERY_SLEEP_S = 1;

    
    const int GALILEO_QUERY_RETRY = 2;
}








#endif  


