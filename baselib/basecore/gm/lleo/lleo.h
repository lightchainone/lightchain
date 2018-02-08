

#ifndef __GALILEO_H__
#define __GALILEO_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <ul_log.h>
#include <connectmgr.h>
#include <Lsc/var/implement.h>
#include <Lsc/ResourcePool.h>
#include <Lsc/var/JsonDeserializer.h>
#include <Lsc/var/JsonSerializer.h>
#include <Lsc/var/CompackDeserializer.h>
#include <Lsc/var/CompackSerializer.h>

#include "lleo_def.h"
#include "lleo_util.h"



lleo_t *lleo_init( const char *lleo_svr_list, const int sesn_expi_time = 30000, int delay_watch_time = 10);


int lleo_add_auth( lleo_t *lleo_handler, struct ACL_vector *acl_vec);


int lleo_set_buf_size( lleo_t *lleo_handler, int buf_size );



int lleo_set_log( const char* log_path, int log_level );


void lleo_close_log(void);


void lleo_destroy( lleo_t *lleo_handler );


int lleo_register_resource( lleo_t *lleo_handler,
        const char *path, Lsc::var::IVar& node_data, int *tag_no, const int flag = 0);


int lleo_query_resource( const lleo_t* lleo_handler,
        const char *path, Lsc::var::IVar& res_data, Lsc::ResourcePool& rp);


typedef int (*lleo_proc_callback)( int watch_type, const char *realpath,
        Lsc::var::IVar& res_data, void *cb_ctx );


int lleo_watch_resource( lleo_t* lleo_handler, const char *path,
        lleo_proc_callback cb, void *ctx );


int lleo_delete_watch( lleo_t* lleo_handler, const char *path );


int lleo_update_resource( lleo_t *lleo_handler, const char *path,
        Lsc::var::IVar& node_data, const int tag_no );


int lleo_delete_resource( lleo_t  *lleo_handler, const char *path,
                             const int tag_no );


int lleo_admin_create_abstract( const lleo_t* lleo_handler,
        const char *path, Lsc::var::IVar& node_data, Lsc::var::IVar& node_acl );


int lleo_admin_get_abstract( const lleo_t* lleo_handler, const char *path,
        Lsc::var::IVar& node_data, Lsc::var::IVar& node_acl, Lsc::ResourcePool& rp);



int lleo_admin_update_abstract( const lleo_t* lleo_handler,
        const char *path, Lsc::var::IVar& node_data, Lsc::var::IVar& node_acl );



int lleo_admin_delete_abstract( const lleo_t* lleo_handler, const char *path );



const char* lleo_err2str( const int grc );


int lleo_set_acl(Lsc::var::IVar& acl_dict, struct ACL *pACL);


Lsc::var::IVar * lleo_get_acl_from_data(Lsc::var::IVar& data);


void lleo_set_getchildren_retry_time(int new_retry_time = 0, int new_retry_interval = 1); 

#endif


