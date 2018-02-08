#ifndef __GALILEO_ADMIN_H__
#define __GALILEO_ADMIN_H__

#include <stdio.h>
#include <unistd.h>

#include <Configure.h>
#include "lleo.h"


#if defined(__DATE__) && defined(__TIME__)
	
    #define BUILD_DATE  (__DATE__ " " __TIME__)
#else
	
    #define BUILD_DATE  "unknown"
#endif



typedef struct _admin_req_t{
	char req_type;								
	char host[GALILEO_DEFAULT_PATH_LEN];		
	char path[GALILEO_DEFAULT_PATH_LEN];        
	char data_pack[GALILEO_DEFAULT_BUF_SIZE];   
	char acl_pack[GALILEO_DEFAULT_BUF_SIZE];    
	char id[GALILEO_ACL_ID_LEN];

	char conf[GALILEO_DEFAULT_PATH_LEN];        
} admin_req_t;

#endif

