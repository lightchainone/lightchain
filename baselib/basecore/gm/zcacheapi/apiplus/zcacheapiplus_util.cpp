#include "ul_net.h"
#include "ul_thr.h"
#include "zcachedef.h"
#include "zcachedata.h"
#include "log.h"
#include "zcacheapiplus_struct.h"


int ms_sleep(int mstime)
{
	struct timeval tv;
	if (mstime < 0)
	{
		return 0;
	}
	tv.tv_sec = mstime / 1000;
	tv.tv_usec = (mstime % 1000) * 1000;
	return select(1, NULL, NULL, NULL, &tv);
}


static int changeu32_2ip(unsigned int ip_u32, char * ip_chr, unsigned int ip_chr_size)
{
	if (0 == ip_u32 || NULL == ip_chr || 16 > ip_chr_size)
	{
		return -1;
	}
	unsigned int ip3 = ip_u32 / ZCACHE_APIPLUS_IP3;
	ip_u32 = ip_u32 - ip3 * ZCACHE_APIPLUS_IP3;

	unsigned int ip2 = ip_u32 / ZCACHE_APIPLUS_IP2;
	ip_u32 = ip_u32 - ip2 * ZCACHE_APIPLUS_IP2;

	unsigned int ip1 = ip_u32 / ZCACHE_APIPLUS_IP1;
	ip_u32 = ip_u32 - ip1 * ZCACHE_APIPLUS_IP1;

	snprintf(ip_chr, ip_chr_size, "%u%s%u%s%u%s%u", ip_u32, ".", ip1, ".", ip2, ".", ip3);
	return 0;
}


int copy_block_info(zcacheplus_t * zcacheplus_handel)
{
	if (NULL == zcacheplus_handel || NULL
			== zcacheplus_handel->master_zc_handle->zcaches [0] || NULL
			== zcacheplus_handel->master_zc_handle->zcaches [0]->product || NULL
			== zcacheplus_handel->nodes || NULL == zcacheplus_handel->blocks)
	{
		WARNING_LOG("zcacheapi+::copy_block_info() g_data is error, not init!!");
		return -1;
	}
	
	unsigned int node_num =
			zcacheplus_handel->master_zc_handle->zcaches [0]->product->get_node_num();
	if (0 == node_num)
	{
		WARNING_LOG("zcacheapi+::copy_block_info() master zcache node num is zero!!!");
		return 0;
	}
	unsigned int local_node_num = zcacheplus_handel->nodes->size();
	
	if (node_num > local_node_num)
	{
		for (unsigned int i = local_node_num; i < node_num; i++)
		{
			zcache_node_stat_t node;
			if (0 == zcacheplus_handel->master_zc_handle->zcaches [0]->product->get_node(
					(unsigned int) i, &node.node))
			{
				try
				{
					node.stat = ZCACHEAPI_NODE_STAT_WORKING;
					pthread_rwlock_wrlock(&zcacheplus_handel->rw_mutex);
					zcacheplus_handel->nodes->push_back(node);
					pthread_rwlock_unlock(&zcacheplus_handel->rw_mutex);
				}
				catch (std::exception& e)
				{
					WARNING_LOG("%s,[ip:%u,%u,port:%d]",e.what(),node.node.ip[0],node.node.ip[1],node.node.port);
					return -1;
				}
				DEBUG_LOG("Add node [ip0:%u, ip1:%u, port:%u, add_idx:%u, cur_nodes_num:%u]",
						node.node.ip[0],node.node.ip[1],node.node.port, i, node_num);
			}
		}
	}
	
	for (unsigned int i = 0; i < zcacheplus_handel->max_block_num; i++)
	{
		int temp_nodeid =
				zcacheplus_handel->master_zc_handle->zcaches [0]->product->get_nodeid(i);
		
		if (temp_nodeid < 0)
		{
			
			WARNING_LOG("zcacheapi+::copy_block_info() api nodeid is <0 (咋可能呢，见不到这条日志)!!!");
			pthread_rwlock_wrlock(&zcacheplus_handel->rw_mutex);
			zcacheplus_handel->blocks [i].nodeid = -1;
			pthread_rwlock_unlock(&zcacheplus_handel->rw_mutex);
		}
		
		else
		{
			zcache_node_stat_t node;
			
			unsigned int local_node_count = zcacheplus_handel->nodes->size();
			if ((unsigned int) temp_nodeid >= local_node_count)
			{
				
				pthread_rwlock_wrlock(&zcacheplus_handel->rw_mutex);
				zcacheplus_handel->blocks [i].nodeid = -1;
				pthread_rwlock_unlock(&zcacheplus_handel->rw_mutex);
			}
			
			else
			{
				
				if (1 == zcacheplus_handel->is_frozen_server
						&& ZCACHEAPI_NODE_STAT_FROZEN
								== (*zcacheplus_handel->nodes) [temp_nodeid].stat)
				{
					
				}
				else
				{
					
					pthread_rwlock_wrlock(&zcacheplus_handel->rw_mutex);
					zcacheplus_handel->blocks [i].nodeid = temp_nodeid;
					pthread_rwlock_unlock(&zcacheplus_handel->rw_mutex);
				}
			}
		}
	}
	return 0;
}


static int ping_server(char * ip, unsigned int port)
{
	if (0 == ip || port < 1024)
	{
		WARNING_LOG("zcacheapi+::ping_server() input param error!");
		return -1;
	}
	int sockfd = 0;
	sockfd = ul_tcpconnecto_ms(ip, port, 1000);
	if (sockfd < 0)
	{
		WARNING_LOG("zcacheapi+::ping_server() ping server create sock fail!! ip:%s port:%u",ip,port);
		return -2;
	}
	else
	{
		
		DEBUG_LOG("zcacheapi+::ping_server() ping server create sock ok!! ip:%s port:%u",ip,port);
		close(sockfd);
	}
	return 0;
}


void * health_check_thread(void *p)
{
	zcacheplus_t * zcacheplus_handel = (zcacheplus_t *) p;
	if (NULL == zcacheplus_handel)
	{
		WARNING_LOG("zcacheapi+::health_check_thread() input param error!");
		return NULL;
	}
	int ret = 0;
	zlog_open_r("api+::health_check_thread");
	while (1==zcacheplus_handel->is_running)
	{
		
		ms_sleep(zcacheplus_handel->health_check_time_ms);
		
		ret = copy_block_info(zcacheplus_handel);
		if (ret != 0)
		{
			WARNING_LOG("zcacheapi+::health_check_thread() copy_block_info fail!");
		}
		
		char ip_chr [16];
		for (unsigned int i = 0; i < zcacheplus_handel->nodes->size(); i++)
		{
			if (0 != (*zcacheplus_handel->nodes) [i].node.ip [0])
			{
				if (0 != changeu32_2ip((*zcacheplus_handel->nodes) [i].node.ip [0],
						ip_chr, sizeof(ip_chr)))
				{
					WARNING_LOG("zcacheapi+::health_check_thread() node ip err!! ip:%u",(*zcacheplus_handel->nodes) [i].node.ip [0]);
					continue;
				}
			}
			else
			{
				if (0 != changeu32_2ip((*zcacheplus_handel->nodes) [i].node.ip [1],
						ip_chr, sizeof(ip_chr)))
				{
					WARNING_LOG("zcacheapi+::health_check_thread() node ip err!! ip:%u",(*zcacheplus_handel->nodes) [i].node.ip [0]);
					continue;
				}
			}
			
			
			if (-2 == ping_server(ip_chr, (*zcacheplus_handel->nodes) [i].node.port))
			{
				pthread_rwlock_wrlock(&zcacheplus_handel->rw_mutex);
				
				if ((*zcacheplus_handel->nodes) [i].stat >= 0)
				{
					(*zcacheplus_handel->nodes) [i].stat += 1;
					
					(*zcacheplus_handel->nodes) [i].stat=(*zcacheplus_handel->nodes) [i].stat%100;
					if ((*zcacheplus_handel->nodes) [i].stat
							>= (int) zcacheplus_handel->ping_time)
					{
						(*zcacheplus_handel->nodes) [i].stat = ZCACHEAPI_NODE_STAT_FROZEN;
						WARNING_LOG("zcacheapi+::health_check_thread() frozen node : ip1:%u ,ip2:%u , "
								"port:%u",(*zcacheplus_handel->nodes) [i].node.ip[0],
								(*zcacheplus_handel->nodes) [i].node.ip[1],
								(*zcacheplus_handel->nodes) [i].node.port);
					}
				}
				
				else if((*zcacheplus_handel->nodes) [i].stat < 0)
				{
					(*zcacheplus_handel->nodes) [i].stat=ZCACHEAPI_NODE_STAT_FROZEN;
					WARNING_LOG("zcacheapi+::health_check_thread() node is frozen: ip1:%u ,ip2:%u , "
													"port:%u",(*zcacheplus_handel->nodes) [i].node.ip[0],
													(*zcacheplus_handel->nodes) [i].node.ip[1],
													(*zcacheplus_handel->nodes) [i].node.port);
				}
				pthread_rwlock_unlock(&zcacheplus_handel->rw_mutex);
			}
			
			else
			{
				pthread_rwlock_wrlock(&zcacheplus_handel->rw_mutex);
				(*zcacheplus_handel->nodes) [i].stat = ZCACHEAPI_NODE_STAT_WORKING;
				pthread_rwlock_unlock(&zcacheplus_handel->rw_mutex);
				DEBUG_LOG("zcacheapi+::health_check_thread() node is ping ok: ip1:%u ,ip2:%u , "
												"port:%u",(*zcacheplus_handel->nodes) [i].node.ip[0],
												(*zcacheplus_handel->nodes) [i].node.ip[1],
												(*zcacheplus_handel->nodes) [i].node.port);
			}
		}
	}
	return NULL;
}

















































