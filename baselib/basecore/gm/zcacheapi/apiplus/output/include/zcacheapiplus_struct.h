
#ifndef APIPLUS_STRUCT_H_
#define APIPLUS_STRUCT_H_

#include "productinfo.h"
#include "zcacheapi.h"


#define ZCACHE_APIPLUS_IP3 (256*256*256)

#define ZCACHE_APIPLUS_IP2 (256*256)

#define ZCACHE_APIPLUS_IP1 (256)


typedef enum _zcache_node_stat_type_t
{
	ZCACHEAPI_NODE_STAT_WORKING=0,
	ZCACHEAPI_NODE_STAT_PING_FAIL=-1,
	ZCACHEAPI_NODE_STAT_FROZEN=-2,
}zcache_node_stat_type_t;

typedef struct _zcache_node_stat_t
{
	zcache_node_t node;
	char stat;
} zcache_node_stat_t;

typedef struct _zcacheplus_conf_t
{
	unsigned int connect_timeout_ms;
	unsigned int read_timeout_ms;
	unsigned int write_timeout_ms;
	unsigned int refetch_time_ms;
	unsigned int health_check_time_ms;
	int is_tranfer_select;
	int is_frozen_server;
	unsigned int ping_time;
} zcacheplus_conf_t;

typedef struct _zcacheplus_t
{
	zcache_t * master_zc_handle;

	zcache_t * slave_zc_handle;

	pthread_t health_check_handle;

	unsigned int health_check_time_ms;

	unsigned int ping_time;

	bool is_change_op;

	std::vector<zcache_node_stat_t> * nodes; 

	product_block_t * blocks;

	unsigned int max_block_num;

	pthread_rwlock_t rw_mutex;

	int is_tranfer_select;

	int is_frozen_server;

	int is_running;

} zcacheplus_t;

#endif 

