#ifndef APIPLUS_H_
#define APIPLUS_H_
#include "zcacheapiplus_struct.h"



int init_zcache(zcacheplus_t * zcacheplus_handel, const char *master_service_addrs,
		const char *slave_service_addrs, const char *product_name, const char *token,
		unsigned int connect_timeout_ms, unsigned int read_timeout_ms,
		unsigned int write_timeout_ms, unsigned int refetch_time_ms,
		unsigned int health_check_time_ms, unsigned int ping_time, int is_tranfer_select,
		int is_frozen_server);



int insert_zcache(zcacheplus_t * zcacheplus_handel, zcache_item_t * key,
		zcache_item_t *slckey, zcache_item_t * value, unsigned int expiration_ms,
		zcache_err_t *err_out, unsigned int logid, unsigned int op_timeout_ms);


int update_zcache(zcacheplus_t * zcacheplus_handel, zcache_item_t *key,
		zcache_item_t *slckey, zcache_item_t *value, unsigned int expiration_ms,
		zcache_err_t *err_out, unsigned int logid, unsigned int op_timeout_ms);



int delete_zcache(zcacheplus_t *zcacheplus_handel, zcache_item_t *key,
		zcache_item_t *slckey, unsigned int delay_time_ms, zcache_err_t *err_out,
		unsigned int logid, unsigned int op_timeout_ms);




int search_zcache(zcacheplus_t * zcacheplus_handel, zcache_item_t *key,
		zcache_item_t *slckey, zcache_item_t *value_out, zcache_err_t *err_out,
		unsigned int logid, unsigned int op_timeout_ms, unsigned int * left_time_ms =
				NULL);


void destroy_zcache(zcacheplus_t *zcacheplus_handel);

#endif 
