#include "apool.h"
#include "pool.h"
#include "nshead.h"
#include "lc_miscdef.h"
#include "LcMonitor.h"
enum {
	NSHEADER = IDLE + 1,
	READING,
	WRITING,
};



int apool_consume(apool_t *pool, lc_server_pdata_t *data)
{
	int idx;
	lc_server_t *sev = (lc_server_t *)data->parent;
	if (apool_get_queue(pool, &idx) != 0) {
		return 0;
	}
	struct apool_socket *sock = &pool->_sockets[idx];
	data->fd = sock->sock;
	data->ip = sock->ip;
	
	data->apoolsocket = &pool->_sockets[idx];
	gettimeofday(&sock->process_time, NULL);
	int ret = 0;
	nshead_t * head = (nshead_t *)sock->read_buf;
	if (strcmp(LC_MONITOR_PROVIDER, head->provider) != 0)
	{
		if (sev->cb_check != NULL) {
			ret = sev->cb_check(sock->read_buf, sock->read_buf_len);
			if (ret != 0) {
				LC_LOG_WARNING("check the req err");
			}
		}
		if( 0 == ret)
		{
			ret = sev->cb_drastic();
		}
		if (ret != 0) {
            if (sev->monitor) {
                sev->monitor->add_request_number_failed(sev->server_name);
			}
            ret = -1;
		}
		else if(sock->write_buf != 0 && sock->write_buf_len >= sizeof(nshead_t))
		{
            if (sev->monitor) {
                sev->monitor->add_request_number_success(sev->server_name);
			}
			sock->write_buf_len = sizeof(nshead_t) + ((nshead_t *)sock->write_buf)->body_len;
			
			nshead_t * res = (nshead_t *)sock->write_buf;
			if(res->body_len > sock->write_buf_len + sizeof(nshead_t))
			{
				LC_LOG_WARNING("invalied body_len %d in write buffer with length %d", res->body_len, sock->write_buf_len);
				return -1;
			}
			
			sock->write_buf_len = sizeof(nshead_t) + res->body_len;
			res->log_id = head->log_id;
			res->magic_num = NSHEAD_MAGICNUM;
			memcpy(res->provider, head->provider, sizeof(res->provider));
		}
		else
		{
            if (sev->monitor) {
                sev->monitor->add_request_number_success(sev->server_name);
			}
			ret = -1;
		}
	}
	else
	{
		if(NULL != apool_get_write_buf(pool,sock,sizeof(nshead_t)))
		{
			memcpy(sock->write_buf, sock->read_buf, sizeof(nshead_t));
			ret = 0;
		}
		else
		{
			ret = -1;
		}
	}
	apool_time_interval(&sock->process_time);
	apool_time_interval(&sock->total_time);
	char reqip[32];
	in_addr addr;
	addr.s_addr = sock->ip;
	lc_log_setbasic(LC_LOG_REQIP, "%s", lc_inet_ntoa_r(addr, reqip, sizeof(reqip)));
	lc_log_setbasic(LC_LOG_LOGID, "%u", head->log_id);

    if (sev->monitor) {
        sev->monitor->add_compute_time(sev->server_name, apool_get_us(&sock->total_time));

    }

	lc_log_setbasic(LC_LOG_PROCTIME, "total:%u(us) rev:%u+proc:%u+write:%u",
		apool_get_us(&sock->total_time), apool_get_us(&sock->read_time),
		apool_get_us(&sock->process_time), 0);
	
	if (sev->log_print) {
		LC_LOG_NOTICE("");
	}
	lc_log_setbasic(LC_LOG_PROCTIME, "");
	if(0 == ret)
	{
		sock->last_active = time(NULL);
		sock->state = WRITING;
	}
	else
	{
		sock->state = ERROR;
		
	}
	apool_epoll_add(pool, idx, EPOLLOUT);
	return 0;
}

int apool_native(apool_t * pool, int idx, uint32_t events){
	int result;
	
	if(events & EPOLLIN)
	{
		switch(pool->_sockets[idx].state){
			case IDLE:
				pool->_sockets[idx].state = NSHEADER;
				gettimeofday(&pool->_sockets[idx].read_time, NULL);
				gettimeofday(&pool->_sockets[idx].total_time, NULL);
			case NSHEADER:
				result = apool_read_head(pool, idx);
				if(result != 0){
					break;
				}
			case READING:
				result = apool_read_content(pool, idx);
				break;
			default:
				LC_LOG_FATAL("invalid read apool state in native callback sock %d id %d, state %d", pool->_sockets[idx].sock, idx, pool->_sockets[idx].state );
				apool_close_socket(pool, idx);
		}
	}
	
	else if(events & EPOLLOUT)
	{
		switch(pool->_sockets[idx].state)
		{
			case WRITING:
				result = apool_write(pool, idx);
				break;
			case ERROR:
				LC_LOG_WARNING("process error %d id %d, state %d", pool->_sockets[idx].sock, idx, pool->_sockets[idx].state );
				apool_close_socket(pool, idx);
				break;
			default:
				LC_LOG_FATAL("invalid write apool state in native callback sock %d id %d, state %d", pool->_sockets[idx].sock, idx, pool->_sockets[idx].state );
				apool_close_socket(pool, idx);
		}
	}
	return 0;
}



int apool_read_head(apool_t *pool, int idx)
{
	struct apool_socket *sock = &pool->_sockets[idx];
	if(sock->state != NSHEADER || sock->read_buf_used > sizeof(nshead_t)){
		LC_LOG_FATAL("invalid apool state while reading header %d", idx);
		return -1;
		}
	int result = apool_noblock_read_buff(sock->sock, (char *)&sock->read_small_buf,
		(int*)&sock->read_buf_used, sizeof(nshead_t));
	sock->last_active = time(NULL);
	if(0 == result){
		nshead_t *head = (nshead_t *)&sock->read_small_buf;
		if(head->magic_num != NSHEAD_MAGICNUM)
		{
			ul_writelog(UL_LOG_WARNING,
					"<%u> nshead_read magic num mismatch: ret %x want %x",
					head->log_id, head->magic_num, NSHEAD_MAGICNUM);
			apool_close_socket(pool, idx);
			return -1;
		}
		else
		{
			
			if ( head->body_len > pool->read_small_buf_len - sock->read_buf_used){
				if(head->body_len <= pool->lc_server->read_siz)
				{
					sock->read_buf = (char *)apool_malloc(pool, head->body_len + sizeof(nshead_t));
					if(sock->read_buf != 0){
						memcpy(sock->read_buf, (char *)head, sizeof(nshead_t));
					}
					else
					{
						ul_writelog(UL_LOG_WARNING,
								"malloc failed for nshead: %d, size %d",	head->log_id, head->body_len);
						apool_close_socket(pool, idx);
						return -1;
					}
				}
				else
				{
					ul_writelog(UL_LOG_WARNING,
						"invalid body_len in nshead: %d, size %d",	head->log_id, head->body_len);
					apool_close_socket(pool, idx);
					return -1;					
				}
			}
			else
			{
				sock->read_buf = (char *) head;
			}
			sock->read_buf_len = head->body_len + sizeof(nshead_t);
			sock->state = READING;
			return 0;
		}
	}
	else if (-1 == result)
	{
		LC_LOG_WARNING( "read head failed id %d", idx);
		apool_close_socket(pool, idx);
	}
	return result;
}

int apool_read_content(apool_t *pool, int idx)
{
	struct apool_socket *sock = &pool->_sockets[idx];
	if(sock->state != READING || sock->read_buf_used > sock->read_buf_len){
		LC_LOG_FATAL("invalid apool state while reading content %d", idx);
		return -1;
		}
	int result =  apool_noblock_read_buff(sock->sock, sock->read_buf, 
		(int *)&sock->read_buf_used, sock->read_buf_len);
	sock->last_active = time(NULL);
	
	if (0 == result)
	{
		
		apool_time_interval(&sock->read_time);
		sock->state = PROCESSING;
		if (-1 == apool_epoll_del(pool, idx)) {
			LC_LOG_WARNING( "read content failed id %d", idx);
			apool_close_socket(pool, idx);
			return -1;
		}
		if (-1 == apool_put_queue(pool, idx)) {
			LC_LOG_WARNING( "read content failed id %d", idx);
			apool_close_socket(pool, idx);
			return -1;
		}
	}
	else if (-1 == result)
	{
		LC_LOG_WARNING( "read content failed id %d", idx);
		apool_close_socket(pool, idx);
	}
	return result;
}

int apool_write(apool_t *pool, int idx)
{
	int result = apool_noblock_write_buff(pool->_sockets[idx].sock, pool->_sockets[idx].write_buf, 
		(int*)&pool->_sockets[idx].write_buf_used, pool->_sockets[idx].write_buf_len);
	if(0 == result)
	{
		
		
		lc_server_t *sev = (lc_server_t *)pool->lc_server;
		if (lc_get_session_connect_type_then_reset(sev) == LCSVR_LONG_CONNECT)
		{
			apool_reset_socket(pool, idx);
			apool_epoll_mod(pool, idx, EPOLLIN);
		}
		else
		{
			apool_close_socket(pool, idx);
		}
		
	}
	else if(-1 == result)
	{
		LC_LOG_WARNING( "write content failed id %d", idx);
		apool_close_socket(pool, idx);
	}
	return result;
}

