#include "ul_net.h"
#include "ul_thr.h"
#include "zcachedef.h"
#include "zcachedata.h"
#include "log.h"
#include "hash.h"
#include "zcacheapi.h"
#include "zcacheapiplus_util.h"
#include "zcacheapiplus.h"

int init_zcache(zcacheplus_t * zcacheplus_handel, const char *master_service_addrs,
		const char *slave_service_addrs, const char *product_name, const char *token,
		unsigned int connect_timeout_ms, unsigned int read_timeout_ms,
		unsigned int write_timeout_ms, unsigned int refetch_time_ms,
		unsigned int health_check_time_ms, unsigned int ping_time, int is_tranfer_select,
		int is_frozen_server)
{
	int ret = 0;
	
	{
		zcache_t * zcache_handel = zcache_create(master_service_addrs, product_name);
		if (zcache_handel == NULL)
		{
			WARNING_LOG("zcacheapi+::init_zcache():master zcache create error");
			ret = (-1*ZCACHE_APIPLUS_INIT_FAIL);
			return ret;
		}
		DEBUG_LOG("zcacheapi+::init_zcache():master zcache create ok!");
		zcache_set_param(zcache_handel, connect_timeout_ms, read_timeout_ms,
				write_timeout_ms, refetch_time_ms);
		DEBUG_LOG("zcacheapi+::init_zcache():master zcache set param ok!");
		ret = zcache_auth(zcache_handel, token, true);
		DEBUG_LOG("zcacheapi+::init_zcache():master zcache auth ok!");
		if (0 != ret)
		{
			WARNING_LOG("zcacheapi+::init_zcache():master zcache self author error:%d",
					ret);
			return ret;
		}
		zcacheplus_handel->master_zc_handle = zcache_handel;
		DEBUG_LOG("zcacheapi+::init_zcache():master zcache author ok!");
	}
	{
		zcache_t * zcache_handel = zcache_create(slave_service_addrs, product_name);
		if (zcache_handel == NULL)
		{
			WARNING_LOG("zcacheapi+::init_zcache():slave zcache create error");
			ret = (-1*ZCACHE_APIPLUS_INIT_FAIL);
		}
		DEBUG_LOG("zcacheapi+::init_zcache():slave zcache create ok!");
		zcache_set_param(zcache_handel, connect_timeout_ms, read_timeout_ms,
				write_timeout_ms, refetch_time_ms);
		DEBUG_LOG("zcacheapi+::init_zcache():slave zcache set param ok!");
		ret = zcache_auth(zcache_handel, token, true);
		DEBUG_LOG("zcacheapi+::init_zcache():slave zcache auth ok!");
		if (0 != ret)
		{
			WARNING_LOG("zcacheapi+::init_zcache():slave zcache self author error:%d",
					ret);
			return ret;
		}
		zcacheplus_handel->slave_zc_handle = zcache_handel;
		DEBUG_LOG("zcacheapi+::init_zcache():slave zcache author ok!");
	}
	
	{
		if (NULL == zcacheplus_handel->master_zc_handle->zcaches [0] || NULL
				== zcacheplus_handel->master_zc_handle->zcaches [0]->product || NULL
				== zcacheplus_handel->slave_zc_handle->zcaches [0] || NULL
				== zcacheplus_handel->slave_zc_handle->zcaches [0]->product)
		{
			WARNING_LOG("zcacheapi+::init_zcache() master/slave zcache_handel init fail!");
			ret = (-1*ZCACHE_APIPLUS_INIT_FAIL);
			return ret;
		}
		unsigned
				max_block_num =
						zcacheplus_handel->master_zc_handle->zcaches [0]->product->get_max_block_num();
		if (max_block_num
				!= zcacheplus_handel->slave_zc_handle->zcaches [0]->product->get_max_block_num()
				|| 0 == max_block_num)
		{
			WARNING_LOG("zcacheapi+::init_zcache() master/slave zcache_handel max_block_num not equal or zero!");
			ret = (-1*ZCACHE_APIPLUS_INIT_FAIL);
			return ret;
		}
		zcacheplus_handel->max_block_num = max_block_num;
		try
		{
			zcacheplus_handel->blocks = new product_block_t [max_block_num];
			zcacheplus_handel->nodes = new std::vector<zcache_node_stat_t>;
		}
		catch (std::exception &e)
		{
			WARNING_LOG("%s,[max_block_num:%u]",
					e.what(),max_block_num);
			delete zcacheplus_handel->nodes;
			delete [] zcacheplus_handel->blocks;
			zcacheplus_handel->nodes = NULL;
			zcacheplus_handel->blocks = NULL;
			WARNING_LOG("zcacheapi+::init_zcache() malloc blocks/nodes fail,没内存啊,干不了活啊!!");
			ret = (-1*ZCACHE_APIPLUS_INIT_FAIL);
			return ret;
		}
		if (NULL == zcacheplus_handel->blocks || NULL == zcacheplus_handel->nodes)
		{
			WARNING_LOG("zcacheapi+::init_zcache() malloc blocks/nodes fail,没内存啊,干不了活啊!!");
			ret = (-1*ZCACHE_APIPLUS_INIT_FAIL);
			return ret;
		}
		pthread_rwlock_init(&(zcacheplus_handel->rw_mutex), NULL);
	}
	zcacheplus_handel->is_frozen_server = is_frozen_server;
	zcacheplus_handel->is_tranfer_select = is_tranfer_select;
	zcacheplus_handel->ping_time = ping_time;
	
	copy_block_info(zcacheplus_handel);
	
	zcacheplus_handel->health_check_time_ms = health_check_time_ms;
	zcacheplus_handel->is_running=1;
	if (0 != ul_pthread_create(&zcacheplus_handel->health_check_handle, NULL,
			health_check_thread, (void *) zcacheplus_handel))
	{
		WARNING_LOG("zcacheapi+::init_zcache() run daemon server failed!");
		ret = (-1*ZCACHE_APIPLUS_INIT_FAIL);
		return ret;
	}
	DEBUG_LOG("zcacheapi+::init_zcache():init ok!");
	return ret;
}

int insert_zcache(zcacheplus_t * zcacheplus_handel, zcache_item_t * key,
		zcache_item_t * slckey, zcache_item_t * value, unsigned int expiration_ms,
		zcache_err_t *err_out, unsigned int logid, unsigned int op_timeout_ms)
{
	zlog_set_thlogid(logid);
	int ret = 0;
	if (zcacheplus_handel == NULL || NULL == zcacheplus_handel->master_zc_handle)
	{
		WARNING_LOG("zcacheapi+::insert_zcache():param error");
		*err_out = ZCACHE_ERR_PARAM;
		return (-1 * ZCACHE_ERR_PARAM);
	}

	ret = zcache_insert(zcacheplus_handel->master_zc_handle, key, slckey, value,
			expiration_ms, err_out, logid, op_timeout_ms);
	return ret;
}

int update_zcache(zcacheplus_t * zcacheplus_handel, zcache_item_t *key,
		zcache_item_t *slckey, zcache_item_t *value, unsigned int expiration_ms,
		zcache_err_t *err_out, unsigned int logid, unsigned int op_timeout_ms)
{
	zlog_set_thlogid(logid);
	int ret = 0;
	if (zcacheplus_handel == NULL || NULL == zcacheplus_handel->master_zc_handle)
	{
		WARNING_LOG("zcacheapi+::update_zcache():param error");
		*err_out = ZCACHE_ERR_PARAM;
		return (-1 * ZCACHE_ERR_PARAM);
	}
	ret = zcache_update(zcacheplus_handel->master_zc_handle, key, slckey, value,
			expiration_ms, err_out, logid, op_timeout_ms);
	return ret;
}

int delete_zcache(zcacheplus_t *zcacheplus_handel, zcache_item_t *key,
		zcache_item_t *slckey, unsigned int delay_time_ms, zcache_err_t *err_out,
		unsigned int logid, unsigned int op_timeout_ms)
{
	zlog_set_thlogid(logid);
	int ret = 0;
	if (zcacheplus_handel == NULL || NULL == zcacheplus_handel->master_zc_handle)
	{
		WARNING_LOG("zcacheapi+::delete_zcache():param error");
		*err_out = ZCACHE_ERR_PARAM;
		return (-1 * ZCACHE_ERR_PARAM);
	}

	ret = zcache_delete(zcacheplus_handel->master_zc_handle, key, slckey, delay_time_ms,
			err_out, logid, op_timeout_ms);
	return ret;
}

int search_zcache(zcacheplus_t * zcacheplus_handel, zcache_item_t *key,
		zcache_item_t *slckey, zcache_item_t *value_out, zcache_err_t *err_out,
		unsigned int logid, unsigned int op_timeout_ms, unsigned int * left_time_ms)
{
	zlog_set_thlogid(logid);
	int ret = 0;
	if (zcacheplus_handel == NULL || key == NULL || NULL
			== zcacheplus_handel->master_zc_handle || NULL
			== zcacheplus_handel->slave_zc_handle)
	{
		WARNING_LOG("zcacheapi+::search_zcache():param error");
		*err_out = ZCACHE_ERR_PARAM;
		return (-1 * ZCACHE_ERR_PARAM);
	}
	
	if (key->item == NULL || key->len == 0 || key->len > ZCACHE_MAX_KEY_LEN)
	{
		WARNING_LOG("zcacheapi+::search_zcache():param error");
		return (-1 * ZCACHE_ERR_PARAM);
	}
	unsigned int blockid = hash(key->item, key->len);
	blockid %= zcacheplus_handel->max_block_num;
	
	char block_stat = 0;
	pthread_rwlock_rdlock(&zcacheplus_handel->rw_mutex);
	int nodeid = zcacheplus_handel->blocks [blockid].nodeid;
	if (nodeid >= 0)
	{
		block_stat = (*zcacheplus_handel->nodes) [nodeid].stat;
		DEBUG_LOG("zcacheapi+::search_zcache():find related node id:[%d],ip0:[%u],ip1:[%u],port:[%u];stat is:[%u]",
				nodeid,(*zcacheplus_handel->nodes)[nodeid].node.ip[0],
				(*zcacheplus_handel->nodes)[nodeid].node.ip[1],
				(*zcacheplus_handel->nodes)[nodeid].node.port,(int)block_stat);
	}
	else
	{
		
		block_stat = ZCACHEAPI_NODE_STAT_WORKING;
		DEBUG_LOG("zcacheapi+::search_zcache():not find related block!");
	}
	pthread_rwlock_unlock(&zcacheplus_handel->rw_mutex);

	if (ZCACHEAPI_NODE_STAT_FROZEN == block_stat && 1
			== zcacheplus_handel->is_tranfer_select)
	{
		ret = zcache_search(zcacheplus_handel->slave_zc_handle, key, slckey, value_out,
				err_out, logid, op_timeout_ms, left_time_ms);
		DEBUG_LOG("zcacheapi+::search_zcache():search slave zcache!");
	}
	else
	{
		ret = zcache_search(zcacheplus_handel->master_zc_handle, key, slckey, value_out,
				err_out, logid, op_timeout_ms, left_time_ms);
		DEBUG_LOG("zcacheapi+::search_zcache():search master zcache!");
	}
	return ret;

}

void destroy_zcache(zcacheplus_t *zcacheplus_handel)
{
	if (NULL != zcacheplus_handel)
	{
		if (zcacheplus_handel->health_check_handle&&1==zcacheplus_handel->is_running)
		{
			
			zcacheplus_handel->is_running=0;
			pthread_join(zcacheplus_handel->health_check_handle, NULL);
			DEBUG_LOG("zcacheapi+::destroy_zcache():join health check!");
		}
		zcache_destroy(zcacheplus_handel->master_zc_handle);
		zcacheplus_handel->master_zc_handle = NULL;
		zcache_destroy(zcacheplus_handel->slave_zc_handle);
		zcacheplus_handel->slave_zc_handle = NULL;
		DEBUG_LOG("zcacheapi+::destroy_zcache():destroy master/slaver handler!");

		if (NULL != zcacheplus_handel->blocks)
		{
			delete [] zcacheplus_handel->blocks;
			zcacheplus_handel->blocks = NULL;
			DEBUG_LOG("zcacheapi+::destroy_zcache():delete blocks!");
		}
		if (NULL != zcacheplus_handel->nodes)
		{
			delete zcacheplus_handel->nodes;
			zcacheplus_handel->nodes = NULL;
			DEBUG_LOG("zcacheapi+::destroy_zcache():delete nodes!");
		}
		pthread_rwlock_destroy(&zcacheplus_handel->rw_mutex);
		DEBUG_LOG("zcacheapi+::destroy_zcache():destroy lock!");
		zcacheplus_handel = NULL;
	}
	else
	{
		DEBUG_LOG("zcacheapi+::destroy_zcache():zcacheplus_handel is is NULL");
	}
	DEBUG_LOG("zcacheapi+::destroy_zcache():destroy OK!");
}

