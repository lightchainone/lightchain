#ifndef __LC_CONF_H__
#define __LC_CONF_H__

#include "ul_conf.h"


#ifdef __cplusplus
extern "C"{
#endif 

#define LC_CONF_SUCCESS 0								  
#define LC_CONF_DEFAULT -1								  
#define LC_CONF_LOADFAIL -2									
#define LC_CONF_OVERFLOW -3									
#define LC_CONF_INVALID_CHARACTER -4						
#define LC_CONF_LOST -5										
#define LC_CONF_CHECKSUCCESS -6								
#define LC_CONF_CHECKFAIL -7								
#define LC_CONF_SETMULTIPLE -8								
#define LC_CONF_NULL -9											

#define LC_CONF_MAX_CLONE 4

#define LC_CONF_DEFAULT_ITEMNUM 1024 
#define LC_CONF_IPMAX (LC_CONF_DEFAULT_ITEMNUM/8)
#define LC_CONF_IPLENMAX 16

#define LC_CONF_READCONF 0

enum {
	LC_CONF_VERSION_DEFAULT = 0,
	LC_CONF_VERSION_1 = 1,
	LC_CONF_VERSION_2 = 2
};
namespace comcfg {
class Configure;
}
typedef struct _lc_conf_data_t {
	Ul_confdata	*option;      								
	Ul_confdata	*range;       								
	char		filename[LINE_SIZE]; 						
	FILE		*conf_file;									
	int			build;                						
	
	comcfg::Configure *conf_new;
} lc_conf_data_t;


typedef struct _lc_svr_ip_t {
	u_int 			num;  										
	char 			name[LC_CONF_MAX_CLONE][WORD_SIZE];  	
} lc_svr_ip_t;


typedef struct _lc_svr_t {									
	char				svr_name[WORD_SIZE];				
	u_int				port;                               
	u_int				read_timeout;                       
	u_int				write_timeout;                      
	u_int				thread_num;                         
	u_int				connect_type;                       
	u_int				server_type;						
	u_int				queue_size;							
	u_int				sock_size;							

	u_int				connect_timeout;		  			
	u_int				read_buf_size;						
	u_int				write_buf_size;			  			
	u_int				user_buf_size;		  				

	char				auth[256];		  					

	u_int               galileo_register_enable;            
	char                galileo_register_resdata[1024];      
	
	u_int               galileo_acl_enable;                 
	char                galileo_svrlist[256];               
	char                galileo_path[256];                  
    
    u_int               keepalive_timeout;
    u_int               stack_size;
    u_int               using_threadsche;
    int                 listen_prio;
    int                 worker_prio;
} lc_svr_t;                              	


typedef struct _lc_request_svr_t {							
	char				svr_name[WORD_SIZE];				
	u_int				num;								
	lc_svr_ip_t			ip_list[LC_CONF_IPMAX];				
	u_int				port;                               
	u_int				read_timeout;                       
	u_int				write_timeout;                      
	u_int				connect_timeout;                    
	u_int				max_connect;                        
	u_int				retry;                              
	u_int				connect_type;                       
	u_int               linger;	    

	u_int				no_magic_checker;						
} lc_request_svr_t;                              	


int lc_conf_setversion(int version);

 
lc_conf_data_t * lc_conf_init(const char *path, const char *filename, 
		const int build = LC_CONF_READCONF, const int num = LC_CONF_DEFAULT_ITEMNUM,
		const int version = LC_CONF_VERSION_DEFAULT); 


 
int lc_conf_close(lc_conf_data_t *conf);


 
int lc_conf_getnstr(const lc_conf_data_t *conf, const char *conf_name,
		char *conf_value,  const size_t n, const char *comment,  const char * default_value=NULL); 


 
int lc_conf_getint(const lc_conf_data_t *conf, const char *conf_name, int *conf_value,  
		const char *comment,  const int * default_value=NULL); 


 

int lc_conf_getuint(const lc_conf_data_t *conf, const char *conf_name, 
		unsigned int *conf_value,  const char *comment, const unsigned int *default_value=NULL);

 

int lc_conf_getint64(const lc_conf_data_t *conf, const char *conf_name, 
		long long *conf_value,  const char *comment, const long long *default_value=NULL);


 



int lc_conf_getuint64(const lc_conf_data_t *conf, const char *conf_name, 
		unsigned long long *conf_value, const char *comment, const unsigned long long *default_value=NULL);

 



int lc_conf_getfloat(const lc_conf_data_t *conf, const char *conf_name, 
		float *conf_value, const char *comment, const float *default_value=NULL);



 

int lc_conf_getsvr(const lc_conf_data_t *conf, const char *product_name, 
		const char *module_name, lc_svr_t *conf_value, const char *comment); 


 

int lc_conf_getreqsvr(const lc_conf_data_t *conf, const char *c_name,
		const char *module_name, lc_request_svr_t *conf_value, const char *comment);

 

int lc_conf_getonenstr(const char *path, const char *filename, 
		const char *conf_name, char *conf_value, const size_t n,  const char *default_value = NULL);


 

int lc_conf_getoneint (const char *path, const char *filename, 
		const char *conf_name, int *conf_value, const int *default_value = NULL);


 

int lc_conf_getoneuint(const char *path, const char *filename, 
		const char *conf_name, unsigned int *conf_value, const unsigned  int *default_value = NULL );

 


int lc_conf_getoneint64(const char *path, const char *filename, 
		const char *conf_name, long long *conf_value, const long long *default_value = NULL);


 

int lc_conf_getoneuint64(const char *path, const char *filename, 
		const char *conf_name, unsigned long long *conf_value, const unsigned long long *default_value = NULL);

 

int lc_conf_getonefloat(const char *path, const char *filename, 
		const char *conf_name, float *conf_value, const float *default_value = NULL);



 


int lc_conf_getonesvr(const char *path, const char *filename, const char *product_name, 
		const char *module_name, lc_svr_t *conf_value);

 


int lc_conf_getonereqsvr(const char *path, const char *filename, const char *product_name, 
		const char *module_name, lc_request_svr_t *conf_value);




#define LC_CONF_GETNSTR(conf, conf_name, conf_value, n, comment) do {\
	if (lc_conf_getnstr(conf, conf_name, conf_value, n, comment) != LC_CONF_SUCCESS) {\
		exit(1); \
	} \
}while(0) 


#define LC_CONF_GETINT(conf, conf_name, conf_value, comment) do {\
	if (lc_conf_getint(conf, conf_name, conf_value,comment) != LC_CONF_SUCCESS) { \
		exit(1); \
	} \
}while(0)

#define LC_CONF_GETUINT(conf, conf_name, conf_value, comment) do {\
	if (lc_conf_getuint(conf, conf_name, conf_value,comment) != LC_CONF_SUCCESS) { \
		exit(1); \
	} \
}while(0)


#define LC_CONF_GETINT64(conf, conf_name, conf_value, comment) do {\
	if (lc_conf_getint64(conf, conf_name, conf_value,comment) != LC_CONF_SUCCESS) { \
	exit(1); \
} \
}while(0)



#define LC_CONF_GETUINT64(conf, conf_name, conf_value, comment) do {\
	if (lc_conf_getuint64(conf, conf_name, conf_value,comment) != LC_CONF_SUCCESS) { \
	exit(1); \
} \
}while(0)

#define LC_CONF_GETFLOAT(conf, conf_name, conf_value, comment) do {\
	if (lc_conf_getfloat(conf, conf_name, conf_value,comment) != LC_CONF_SUCCESS) { \
	exit(1); \
} \
}while(0)


#define LC_CONF_GETSVR(conf, product_name, module_name, conf_value, comment) do {\
	if (lc_conf_getsvr(conf, product_name, module_name, conf_value, comment) != LC_CONF_SUCCESS) {\
		exit(1);\
	} \
}while(0)

#define LC_CONF_GETREQSVR(conf, product_name, module_name, conf_value, comment) do {\
	if (lc_conf_getreqsvr(conf, product_name, module_name, conf_value, comment) != LC_CONF_SUCCESS) {\
		exit(1);\
	} \
}while(0)


#define LC_CONF_GETNSTR_DEFAULT(conf, conf_name, conf_value, n, comment, default_value) do {\
	int ret = lc_conf_getnstr(conf, conf_name, conf_value, n, comment, default_value); \
	if (ret != LC_CONF_SUCCESS && ret != LC_CONF_DEFAULT) {\
		exit(1); \
	} \
}while(0)


#define LC_CONF_GETINT_DEFAULT(conf, conf_name, conf_value, comment, default_value) do {\
	int default_int_value = (default_value);\
	int ret = lc_conf_getint(conf, conf_name, conf_value,comment, &default_int_value);\
	if (ret != LC_CONF_SUCCESS && ret != LC_CONF_DEFAULT) {\
		exit(1); \
	} \
}while(0)


#define LC_CONF_GETUINT_DEFAULT(conf, conf_name, conf_value, comment, default_value) do {\
	unsigned int default_uint_value = (default_value);\
	int ret = lc_conf_getuint(conf, conf_name, conf_value, comment, &default_uint_value);\
	if (ret != LC_CONF_SUCCESS && ret != LC_CONF_DEFAULT) {\
		exit(1); \
	} \
}while(0)


#define LC_CONF_GETINT64_DEFAULT(conf, conf_name, conf_value, comment, default_value) do {\
	long long default_int64_value = (default_value);\
	int ret = lc_conf_getint64(conf, conf_name, conf_value,comment, &default_int64_value);\
	if (ret != LC_CONF_SUCCESS && ret != LC_CONF_DEFAULT) {\
		exit(1); \
	} \
}while(0)


#define LC_CONF_GETUINT64_DEFAULT(conf, conf_name, conf_value, comment, default_value) do {\
	unsigned long long default_uint64_value = (default_value);\
	int ret = lc_conf_getuint64(conf, conf_name, conf_value, comment, &default_uint64_value);\
	if (ret != LC_CONF_SUCCESS && ret != LC_CONF_DEFAULT) {\
		exit(1); \
	} \
}while(0)

#define LC_CONF_GETFLOAT_DEFAULT(conf, conf_name, conf_value, comment, default_value) do {\
	float default_float_value = (default_value);\
	int ret = lc_conf_getfloat(conf, conf_name, conf_value, comment, &default_float_value);\
	if (ret != LC_CONF_SUCCESS && ret != LC_CONF_DEFAULT) {\
		exit(1); \
	} \
}while(0)



#define LC_CONF_GETONESTR(path, filename, conf_name, n, conf_value) do {\
	if (lc_conf_getonenstr(path, filename, conf_name, n, conf_value) != LC_CONF_SUCCESS) {\
		exit(1); \
	} \
}while(0)


#define LC_CONF_GETONEINT(path, filename, conf_name, conf_value) do {\
	if (lc_conf_getoneint(path, filename, conf_name, conf_value) != LC_CONF_SUCCESS) {\
		exit(1); \
	} \
}while(0)


#define LC_CONF_GETONEUINT(path, filename, conf_name, conf_value) do {\
	if (lc_conf_getoneuint(path, filename, conf_name, conf_value) != LC_CONF_SUCCESS) {\
		exit(1); \
	} \
}while(0)


#define LC_CONF_GETONEINT64(path, filename, conf_name, conf_value) do {\
	if (lc_conf_getoneint64(path, filename, conf_name, conf_value) != LC_CONF_SUCCESS) {\
		exit(1); \
	} \
}while(0)


#define LC_CONF_GETONEUINT64(path, filename, conf_name, conf_value) do {\
	if (lc_conf_getoneuint64(path, filename, conf_name, conf_value) != LC_CONF_SUCCESS) {\
		exit(1); \
	} \
}while(0)

#define LC_CONF_GETONEFLOAT(path, filename, conf_name, conf_value) do {\
	if (lc_conf_getonefloat(path, filename, conf_name, conf_value) != LC_CONF_SUCCESS) {\
		exit(1); \
	} \
}while(0)


#define LC_CONF_GETONESVR(path, filename, product_name, module_name, comment) do {\
	if (lc_conf_getonesvr(path, filename, product_name, module_name, comment) != LC_CONF_SUCCESS) {\
		exit(1);\
	} \
}while(0)

#define LC_CONF_GETONEREQSVR(path, filename, product_name, module_name, comment) do {\
	if (lc_conf_getonereqsvr(path, filename, product_name, module_name, comment) != LC_CONF_SUCCESS) {\
		exit(1);\
	} \
}while(0)


 
int lc_conf_getsvr_ex(const lc_conf_data_t *conf, const char *product_name, 
		const char *module_name, lc_svr_t *conf_value, const char *comment); 

 
int lc_conf_getreqsvr_ex(const lc_conf_data_t *conf, const char *c_name,
		const char *module_name, lc_request_svr_t *conf_value, const char *comment);


#ifdef __cplusplus
}
#endif  

#endif  




