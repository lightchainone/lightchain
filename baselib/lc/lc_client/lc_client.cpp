#include <stdlib.h>
#include <ul_net.h>
#include <ul_log.h>
#include <netinet/tcp.h>
#include "lc_client.h"
#include "lc_queue.h"
#include "lc_string.h"
#include "nshead.h"


#define MAX_SERVERTYPE_NUM	64			
#define MAX_CONNECT_NUM		256			
#define MAX_PARALLEL_TAIL	36			

#define SafeCloseSock(s) do {if (s>0) { close(s); s=-1; }} while(0)






typedef struct _my_server_t{
	char					ip[LC_CONF_MAX_CLONE][LC_CONF_IPMAX]; 
	u_int					port;					
	u_int					read_timeout;			
	u_int					write_timeout;			
	u_int					connect_timeout;		
	u_int                   socktype;				
}my_server_t;



typedef struct _handle_t{
	int						sock;	
	size_t					index;	
}handle_t;



typedef struct _svr_t{
	my_server_t				myserver;	
	pthread_mutex_t			mutex;		
	pthread_cond_t			cond;		
	lc_queue_t				handle;		
	u_int					work;		
}svr_t;



typedef struct _pick_t{
	lc_queue_t				status;	
	u_int					retry;	
	u_int					alive;	
}pick_t;




typedef struct _mod_t{
	char					name[WORD_SIZE/8];	
	lc_request_svr_t				info;				
	lc_queue_t				svr;				
	pthread_mutex_t			mutex;				
	pick_t					pick;				
	lc_client_mod_attr_t	attr;				
}mod_t;



struct _lc_client_attr_t{
};



struct _lc_client_t{
	lc_queue_t				mod;	
	lc_client_attr_t		attr;	
};



enum sr_status_t {
	LC_STATUS_INIT = 0,
	LC_STATUS_SENDED = 1,
	LC_STATUS_RECEIVED = 2,
	LC_STATUS_ERROR =3
};


enum {
	LC_CLIENT_SYNC = 0,
	LC_CLIENT_ASYNC = 1
};

typedef struct _nshead_status_t {
	int status;
	int cur_size;
} nshead_status_t;

enum {
	LC_NSHEAD_NONE = 0,
	LC_NSHEAD_READ_HEAD,
	LC_NSHEAD_READ_BUFF,
	LC_NSHEAD_WRITE_HEAD,
	LC_NSHEAD_WRITE_BUFF
};



typedef struct _lc_interact_t{
	int						sock;				
	svr_t					*p_svr;				
	u_int					rtimeout;			
	u_int					wtimeout;			
	nshead_t				*reqhead;			
	void					*reqbuff;			
	nshead_t				*reshead;			
	void					*resbuff;			
	u_int					maxreslen;			
	u_int					keepalive;			
	lc_callback_check		checkcallback;      
	int					    can_send;			
	int					    is_send;			
	u_int                   nomagiccheck;       
	sr_status_t				status;
	nshead_status_t         nshead_status;
}lc_interact_t;


typedef struct _lc_recv_callback_t {
	lc_callback_alloc alloccallback;
	lc_callback_clear clearcallback;
	void *alloc_arg;
}lc_recv_callback_t;





static	lc_request_svr_t*
lc_fetchserverindex(lc_client_t *client, const char* name, u_int *indexi, mod_t **out_mod = NULL)
{
	u_int		i;
	mod_t	*p_mod;

	
	if(NULL == client ||NULL == name ||NULL == indexi){
		LC_LOG_FATAL("param null while translate servername to serverindex(client, name or indexi null?)");
		return NULL;
	}

	
	for(i = 0; i < client->mod.cur; ++i){
		if(NULL == (p_mod = (mod_t*)lc_queue_get(&client->mod, i))){
			LC_LOG_WARNING("internal error, get server index error");
			return NULL;
		}
		
		if(0 == strncmp(p_mod->name, name, sizeof(p_mod->name))){

			
			*indexi = i;

			if(NULL != out_mod){
				
				*out_mod =  p_mod;
			}

			
			return &p_mod->info;
		}
	}

	
	*indexi = 0;
	return NULL;
}



	static svr_t*
lc_fetchserver(lc_client_t *client, u_int serverindex, u_int hashindex, u_int* matrix, size_t maxsize, u_int* lastchoose)
{
	u_int		realindex;
	mod_t   *p_mod;
	u_int	i, ok_count, ok_number;

	
	if(NULL == client || NULL == matrix || NULL == lastchoose){
		LC_LOG_FATAL("param NULL or error in fetch socks");
		return NULL;
	}

	
	if(serverindex >= client->mod.cur){
		LC_LOG_FATAL("internal error in lc_client, serverindex:%d, rang is [0, %d]",
				serverindex, client->mod.cur);
		return NULL;
	}

	
	if(NULL == (p_mod = (mod_t*)lc_queue_get(&client->mod, serverindex))){
		LC_LOG_FATAL("internal error, fetch by serverindex error");
		return NULL;
	}

	
	if(p_mod->svr.cur <= 0 ||p_mod->svr.cur > maxsize){
		LC_LOG_WARNING("no svr is aviliable or parameter error");
		return NULL;
	}

	
	
	for(i = 0, ok_number = 0; i < p_mod->svr.cur; ++i){
		if(0 == matrix[i]){
			ok_number ++;
		}
	}

	if(ok_number == 0){
		LC_LOG_WARNING("no svr is allocatable");
		return NULL;
	}

	realindex = hashindex % ok_number;
	for(i = 0, ok_count = 0; i < p_mod->svr.cur; ++i){
		if(0 == matrix[i]){
			ok_count ++;
			if(realindex + 1 == ok_count){
				
				*lastchoose = i;
				return (svr_t*)lc_queue_get(&p_mod->svr, i);
			}
		}
	}

	
	LC_LOG_WARNING("internal error");
	return NULL;

}



	static int
lc_sockcheck(int *sock)
{
	char buf[100];
	ssize_t ret;

	if(NULL == sock){
		LC_LOG_FATAL("close nullsock");
		return -1;
	}

	ret = recv(*sock, buf, sizeof(buf), MSG_DONTWAIT);

	if(ret>0) {
		LC_LOG_WARNING("check connection failed, some data pending(not nshead?),%s", buf);
		SafeCloseSock(*sock);
		return -1;
	}
	else if (ret==0) {
		LC_LOG_TRACE("check connection failed, sock closed by server");
		SafeCloseSock(*sock);
		return -1;
	}
	else if(errno == EWOULDBLOCK) {
		
		return 0;
	}

	SafeCloseSock(*sock);
	return -1;
}

static int 
lc_tcpconnecto_ms(const char *host, int port, int msecs)
{
	
	struct sockaddr_in soin;
	int val = UL_OPEN;
	int fd = ul_socket(AF_INET, SOCK_STREAM, 0);

	if (fd < 0) {
		goto CONNECT_FAIL;
	}
	
	if (ul_opsockopt_ruseaddr(fd, &val, UL_SET) < 0) {
		SafeCloseSock(fd);
		goto CONNECT_FAIL;
	}

	memset(&soin, 0, sizeof(soin));
	if (inet_pton(AF_INET, host, &(soin.sin_addr)) < 0) {
		SafeCloseSock(fd);
		goto CONNECT_FAIL;
	}
	soin.sin_family = AF_INET;
	soin.sin_port = htons((uint16_t)(port));

	if (ul_connecto_sclose_ms(fd, (struct sockaddr *) &soin, sizeof(soin), msecs) < 0) {
		goto CONNECT_FAIL;
	}

	return fd;

CONNECT_FAIL:
	return -1;
}


	static int
lc_createconnection(my_server_t *svr)
{
	int	fd;
	int i = 0;

	
	if(NULL == svr){
		LC_LOG_FATAL("param NULL in create connection to server");
		return -1;
	}

	
	for(i = 0; i < LC_CONF_MAX_CLONE; ++i){
		if(svr->ip[i][0] == '\0')
			goto CONNECT_END;

		fd = lc_tcpconnecto_ms(svr->ip[i], svr->port, svr->connect_timeout);

		if(-1 != fd) {
			
			int on = 1;
			if(setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on)) < 0){
				LC_LOG_WARNING("set sockop for TCP_NODELAY failed");
				SafeCloseSock(fd);
				goto CONNECT_END;
			}

			if(svr->socktype & LC_CLIENT_LINGER_OPEN) {
				struct linger li;
				memset(&li, 0, sizeof(li));
				li.l_onoff = 1;
				li.l_linger = 0;
				if(setsockopt(fd, SOL_SOCKET, SO_LINGER, (const char*)&li, sizeof(li)) < 0) {
					LC_LOG_WARNING("set sockop for SO_LINGER failed");
					SafeCloseSock(fd);
					goto CONNECT_END;
				}
			}
			return fd;
		} else {
            LC_LOG_WARNING("tcp connect to server [%s:%u] error[%m]", svr->ip[i], svr->port);
        }
	}
CONNECT_END:
	if(i >= LC_CONF_MAX_CLONE || svr->ip[i][0] == '\0')
		LC_LOG_WARNING("create connection to server failed");
	return -1;
}




	static int
lc_fetchsock(svr_t *svr, lc_request_svr_t* p_lcsvr, handle_t *handle)
{
	int new_sock;
	int	ret = -1;
	int st = 0; 
	
	if(NULL == svr ||NULL == handle ||NULL == p_lcsvr){
		LC_LOG_FATAL("param NULL in lc_fetchsock");
		return -1;
	}

    pthread_mutex_lock(&svr->mutex);

	if (lc_queue_empty(&svr->handle)) {
		if(svr->work >= p_lcsvr->max_connect) {
			while(svr->work >= p_lcsvr->max_connect && 
				lc_queue_empty(&svr->handle)) {
				pthread_cond_wait(&svr->cond, &svr->mutex);
			}
			if (lc_queue_empty(&svr->handle)) {
				st = 1;
			}
		} else {
			st = 1;
		}
	}

	if (0 == st) {
		
		if(0 != lc_queue_pop(&svr->handle, handle)){
			LC_LOG_FATAL("internal erorr, get handle error ");
			ret = -1;
			goto END;
		}
		if(0 != lc_sockcheck(&handle->sock)){
			
			svr->work--;
		} else {
			
			ret = 0;
			goto END;
		}
	}

	
	svr->work++;
	pthread_mutex_unlock(&svr->mutex);
	if(-1 != (new_sock = lc_createconnection(&svr->myserver))){
		handle->sock = new_sock;
		ret = 0;
		return ret;
	}
	pthread_mutex_lock(&svr->mutex);
	svr->work--;
END:
	pthread_mutex_unlock(&svr->mutex);
	return ret;
}



	static int
lc_putbacksock(svr_t *svr, int sock, u_int keepalive)
{
	int	ret;
	handle_t handle;

	
	if(NULL == svr){
		LC_LOG_FATAL("param NULL in lc_putbacksock");
		return -1;
	}

	if(0 != keepalive && sock >=0 && 0 != lc_sockcheck(&sock)){
		keepalive = 0;
	}

	
	pthread_mutex_lock(&(svr->mutex));

	if(keepalive == 0){
		
		if(sock >= 0)
			SafeCloseSock(sock);
		svr->work --;
		pthread_cond_signal(&svr->cond);
		goto SUCCESS_OUT;
	}
	else{
		handle.sock = sock;
		
		if(0 != lc_queue_push(&svr->handle, &handle)){
			LC_LOG_FATAL("internal erorr, putback sock handle error");
			
			svr->work --;
			SafeCloseSock(sock);
			pthread_cond_signal(&svr->cond);
			goto FAIL_OUT;
		}
		else{
			pthread_cond_signal(&svr->cond);
			goto SUCCESS_OUT;
		}
	}
SUCCESS_OUT:
	pthread_mutex_unlock(&svr->mutex);
	ret = 0;
	goto END;
FAIL_OUT:
	pthread_mutex_unlock(&svr->mutex);
	ret = -1;
	goto END;
END:
	return ret;
}



int noblock_read_buff(int sock, char *buff, int *cur_size, const int size)
{
	int ret;
	while (1) {
		ret = recv(sock, buff+(*cur_size), size-(*cur_size), MSG_DONTWAIT);
		if (-1 == ret && (EINTR == errno)) {
			continue;
		}
		break;
	}

	if (ret < 0) {
		if (EAGAIN == errno) return 1;
		return -1;
	}
	if (0 == ret) {
		return -1;
	}
	*cur_size += ret;
	if (*cur_size == size) {
		return 0;
	}
	return 1;
}

int noblock_nshead_read(nshead_status_t *nshead_status, int sock, nshead_t *head, void **buff,
		const u_int buff_size, lc_recv_callback_t *callback, const u_int flags = NSHEAD_CHECK_MAGICNUM)
{
	int ret = -1;
	lc_callback_alloc_t  tmp_alloc;	
	switch(nshead_status->status) {
		case LC_NSHEAD_NONE: 
			nshead_status->cur_size = 0;
			nshead_status->status = LC_NSHEAD_READ_HEAD;
		case LC_NSHEAD_READ_HEAD:
            ret = noblock_read_buff(sock, (char*)head, &(nshead_status->cur_size), (int)sizeof(nshead_t));
			switch (ret) {
				case 0: 
					if (flags & NSHEAD_CHECK_MAGICNUM) {
						if (head->magic_num != NSHEAD_MAGICNUM) {
								ul_writelog(UL_LOG_WARNING,
										"<%u> nshead_read magic num mismatch: ret %x want %x",
										head->log_id, head->magic_num, NSHEAD_MAGICNUM);
								goto end;
						}
					}
					if (head->body_len > buff_size) {
						ul_writelog(UL_LOG_WARNING,
								"<%u> nshead_read body_len error: buff_size=%u body_len=%u",
								head->log_id, (unsigned int)buff_size, head->body_len);
						goto end;
					}
					if (0 == head->body_len) {
						goto end;
					}
					if (callback != NULL) {
						if (callback->alloccallback != NULL) {
							tmp_alloc.size = head->body_len;
							tmp_alloc.arg = callback->alloc_arg;
							tmp_alloc.buff = *buff;
							*buff = callback->alloccallback(&tmp_alloc);
							if (NULL == *buff) {
								LC_LOG_WARNING("callback->alloccallback size=%u fail", tmp_alloc.size);
								ret = -1;
								goto end;
							}
						}
					}
					nshead_status->cur_size = 0;
					nshead_status->status = LC_NSHEAD_READ_BUFF;
					break;
				case 1:
					return 1;
				default:
					goto end;
			}
		case LC_NSHEAD_READ_BUFF:
			ret = noblock_read_buff(sock, (char*)(*buff), &(nshead_status->cur_size), head->body_len);
			if (0 == ret) {
				nshead_status->status = LC_NSHEAD_NONE;
			}
			break;
		default:
			goto end;
	}
end:
	if (ret < 0) {
		if (callback != NULL) {
			if (callback->clearcallback != NULL) {
				tmp_alloc.size = head->body_len;
				tmp_alloc.arg = callback->alloc_arg;
				tmp_alloc.buff = *buff;
				callback->clearcallback(&tmp_alloc);
				*buff = NULL;
			}
		}
		nshead_status->status = LC_NSHEAD_NONE;
	}
	return ret;
}

int noblock_write_buff(int sock, char *buff, int *cur_size, int size)
{
	int ret;
	if (size < *cur_size) 
		return -1;
	while (1) {
		ret = send(sock, buff+(*cur_size), size-(*cur_size), MSG_DONTWAIT);
		if (-1 == ret && (EINTR == errno))
			continue;
		break;
	}
	if (ret < 0) {
		if (EAGAIN == errno) return 1; 
		LC_LOG_WARNING("noblock write nshead buff error [%m]");
		return -1;
	}
	if (0 == ret) {
		LC_LOG_WARNING("noblock write nshead buff error [%m]");
		return -1;
	}
	*cur_size += ret;
	if (*cur_size == size) {
		return 0;
	}
	return 1;
}

int noblock_nshead_write(nshead_status_t *nshead_status, int sock, nshead_t *head, void *buff,
		const u_int flags = NSHEAD_CHECK_MAGICNUM)
{
	int ret = -1;
	switch(nshead_status->status) {
		case LC_NSHEAD_NONE: 
			if (flags & NSHEAD_CHECK_MAGICNUM) {
				head->magic_num = NSHEAD_MAGICNUM;
			}
			nshead_status->cur_size = 0;
			nshead_status->status = LC_NSHEAD_WRITE_HEAD;
		case LC_NSHEAD_WRITE_HEAD:
            ret = noblock_write_buff(sock, (char*)head, &(nshead_status->cur_size), (int)sizeof(nshead_t));
			switch (ret) {
				case 0: 
					nshead_status->cur_size = 0;
					nshead_status->status = LC_NSHEAD_WRITE_BUFF;
					break;
				case 1:
					return 1;
				default:
					goto end;
			}
		case LC_NSHEAD_WRITE_BUFF:
			ret = noblock_write_buff(sock, (char*)buff, &(nshead_status->cur_size), head->body_len);
			if (0 == ret) {
				nshead_status->status = LC_NSHEAD_NONE;
			}
			break;
		default:
			goto end;
	}
end:
	if (ret < 0) {
		nshead_status->status = LC_NSHEAD_NONE;
	}
	return ret;
}

static int
lc_sendrequest(const int sock, nshead_t *reqhead, void *reqbuff,
		const u_int timeout, const u_int nomagiccheck,
		nshead_status_t *nshead_status = NULL)
{
	int ret;
	if (sock < 0 ||NULL == reqhead ||NULL == reqbuff) {
		LC_LOG_WARNING("invalid arg in send request to server");
		return -1;
	}
	if (NULL == nshead_status) {
		if(nomagiccheck == 1)
			ret = nshead_write(sock, reqhead, reqbuff, timeout, NSHEAD_CHECK_NONE);
		else
			ret = nshead_write(sock, reqhead, reqbuff, timeout);

	} else {
		if(nomagiccheck == 1)
			ret = noblock_nshead_write(nshead_status, sock, reqhead, reqbuff, NSHEAD_CHECK_NONE);
		else
			ret = noblock_nshead_write(nshead_status, sock, reqhead, reqbuff);
	}

	if ( ret < 0 ) {
		LC_LOG_WARNING("send request to server failed, ret:%d", ret);
		return -1;
	}
	return ret;
}




static int
lc_receiveresponse(const int sock, nshead_t *reshead, void **resbuff,
		u_int maxres, const u_int timeout, lc_callback_check check_callback,
		const u_int nomagiccheck, nshead_status_t *nshead_status = NULL,
		lc_recv_callback_t *callback = NULL)
{
	int ret;
	if (NULL == reshead ||NULL == resbuff ||sock < 0){
		LC_LOG_WARNING("invalid arg");
		return -1;
	}
	if (NULL == nshead_status) {
		if(nomagiccheck == 1)
			ret = nshead_read(sock, reshead, *resbuff, maxres, timeout, NSHEAD_CHECK_NONE);
		else
			ret = nshead_read(sock, reshead, *resbuff, maxres, timeout);
	} else {
		if(nomagiccheck == 1)
			ret = noblock_nshead_read(nshead_status, sock, reshead, resbuff,
					maxres, callback, NSHEAD_CHECK_NONE);
		else
			ret = noblock_nshead_read(nshead_status, sock, reshead, resbuff,
					maxres, callback);
	}
	if ( ret < 0 ) {
		LC_LOG_WARNING("read infomation from server failed, ret:%d %m", ret);
		return -1;
	}
	
	if((0 == ret) && (NULL != check_callback) && (0 != check_callback(*resbuff))){
		LC_LOG_TRACE("check_callback function failed");
		return -1;
	}
	return ret;
}

static int
lc_interactwith_server(lc_client_t *client, lc_parallel_t* parallel,
		lc_interact_t *interactor, u_int count, u_int ms_timeout = 0, u_int async = LC_CLIENT_SYNC,
		lc_recv_callback_t *callback = NULL)
{
	fd_set          send_set;           
	fd_set          receive_set;        
	u_int i;
	long total_timeout = 0;
	int need_send = 0;
	timeval timeout, starttime, curtime;
	long timediff, timeleft;
	
	int  onesuccess = 0;
	int maxsock = -1;
	int ret = -1;
	
	if(NULL == interactor || NULL == client){
		LC_LOG_FATAL("param NULL in lc_interactwith_server invitenshead call");
		return -1;
	}
	for(i = 0; i < count; ++i){
		if(NULL != parallel)
			parallel[i].successed = false;
		if(1 == interactor[i].can_send){
			need_send = 1;
			interactor[i].status = LC_STATUS_INIT;
			if (async != LC_CLIENT_SYNC) {
				interactor[i].nshead_status.cur_size = 0;
				interactor[i].nshead_status.status = LC_NSHEAD_NONE;
			}
			maxsock = maxsock > interactor[i].sock? maxsock:interactor[i].sock;
            total_timeout = (long)(total_timeout > (long)(interactor[i].rtimeout + interactor[i].wtimeout) ?
                             total_timeout:(interactor[i].rtimeout + interactor[i].wtimeout));
		}
	}
	
	if(ms_timeout != 0)
		total_timeout = ms_timeout;
	
	if(!need_send)
		return 0;
	gettimeofday(&starttime, NULL);
RECHECK:
	FD_ZERO(&send_set);
	FD_ZERO(&receive_set);
	
	for(i = 0; i < count; ++i){
		if(1 == interactor[i].can_send){
			
			if(interactor[i].status == LC_STATUS_INIT){
				if(interactor[i].sock >= 0)
					FD_SET(interactor[i].sock, &send_set);
				else
					LC_LOG_FATAL("lc_client internal error");
			}
			else if(interactor[i].status == LC_STATUS_SENDED){
				if(interactor[i].sock >= 0)
					FD_SET(interactor[i].sock, &receive_set);
				else
					LC_LOG_FATAL("lc_client internal error");
			}
		}
	}

	gettimeofday(&curtime, NULL);
	timediff = (curtime.tv_sec - starttime.tv_sec)*1000 + (curtime.tv_usec - starttime.tv_usec)/1000;
	if(total_timeout < timediff) {
		LC_LOG_WARNING("time out total_timeout is %ld\n", total_timeout);
		goto RW_END;
	} else {
		timeleft = total_timeout - timediff;
	}
	timeout.tv_sec = (timeleft)/1000; 
	timeout.tv_usec = (timeleft%1000)*1000; 
	if ((ul_select(maxsock + 1, &receive_set, &send_set, NULL, &timeout)) < 0) {
		goto RW_END;
	}

	if (async != LC_CLIENT_SYNC) {
		gettimeofday(&curtime, NULL);
		timediff = (curtime.tv_sec - starttime.tv_sec)*1000 + (curtime.tv_usec - starttime.tv_usec)/1000;
	}
	
	for(i = 0; i < count; ++i) {
		if(1 == interactor[i].can_send && interactor[i].status != LC_STATUS_ERROR) {
			
			if (interactor[i].status == LC_STATUS_INIT) {
				if ((async != LC_CLIENT_SYNC) && (timediff > (int)interactor[i].wtimeout)) {
					LC_LOG_WARNING("socket %d async write time out %s", interactor[i].sock, parallel[i].name);
					interactor[i].status = LC_STATUS_ERROR;
					
					SafeCloseSock(interactor[i].sock);
					lc_putbacksock(interactor[i].p_svr, -1, 0);
				} else if (FD_ISSET(interactor[i].sock, &send_set)) {
					if (async != LC_CLIENT_SYNC) {
						ret = lc_sendrequest(interactor[i].sock,
								interactor[i].reqhead, 
								interactor[i].reqbuff,
								interactor[i].wtimeout,
								interactor[i].nomagiccheck,
								&(interactor[i].nshead_status));
					} else {
						ret = lc_sendrequest(interactor[i].sock,
								interactor[i].reqhead, 
								interactor[i].reqbuff,
								interactor[i].wtimeout,
								interactor[i].nomagiccheck);
					}

					if (0 == ret) {
						interactor[i].status = LC_STATUS_SENDED;
					} else if (ret != 1) {
						interactor[i].status = LC_STATUS_ERROR;
						
						SafeCloseSock(interactor[i].sock);
						lc_putbacksock(interactor[i].p_svr, -1, 0);
					}
				}
			}
			
			else if (interactor[i].status == LC_STATUS_SENDED) { 
				if ((async != LC_CLIENT_SYNC) && (timediff >(int)(interactor[i].rtimeout + interactor[i].wtimeout))) {
					
					LC_LOG_WARNING("socket %d async read time out %s", interactor[i].sock, parallel[i].name);
					SafeCloseSock(interactor[i].sock);
					interactor[i].status = LC_STATUS_ERROR;
					lc_putbacksock(interactor[i].p_svr, -1, 0);
				} else if (FD_ISSET(interactor[i].sock, &receive_set)){
					if (async != LC_CLIENT_SYNC) {
						ret = lc_receiveresponse(
								interactor[i].sock,
								interactor[i].reshead, 
								&(parallel[i].resbuff),
								interactor[i].maxreslen,
								interactor[i].rtimeout,
								interactor[i].checkcallback,
								interactor[i].nomagiccheck,
								&(interactor[i].nshead_status),
								callback);
					} else {
						ret = lc_receiveresponse(
								interactor[i].sock,
								interactor[i].reshead, 
								&(interactor[i].resbuff),
								interactor[i].maxreslen, 
								interactor[i].rtimeout,
								interactor[i].checkcallback,
								interactor[i].nomagiccheck);
					}
					if (0 == ret) {
						interactor[i].status = LC_STATUS_RECEIVED;
						if(NULL != parallel) parallel[i].successed = true;
						onesuccess = 1;
						lc_putbacksock(interactor[i].p_svr, interactor[i].sock, interactor[i].keepalive);
					} else if (ret != 1) {
						
						SafeCloseSock(interactor[i].sock);
						interactor[i].status = LC_STATUS_ERROR;
						lc_putbacksock(interactor[i].p_svr, -1, 0);
					} 
				}
			}
		}
	}
	
	for(i = 0; i < count; ++i){
		
		if(1 == interactor[i].can_send && 
			(interactor[i].status == LC_STATUS_INIT || 
			 interactor[i].status == LC_STATUS_SENDED)){
			goto RECHECK;
		}
	}
RW_END:
	for(i = 0; i < count; ++i){
		if(1 == interactor[i].can_send && 
				(interactor[i].status == LC_STATUS_INIT || 
				 interactor[i].status == LC_STATUS_SENDED)){
			SafeCloseSock(interactor[i].sock);
			interactor[i].sock = -1;
			lc_putbacksock(interactor[i].p_svr, interactor[i].sock, interactor[i].keepalive);
		}
	}
	if(onesuccess) return 0;
	return -1;
}


static int
lc_fill_interactor(lc_client_t *client, const char *name, u_int hash_key,
		nshead_t *reqhead, void* reqbuff, nshead_t *reshead, void* resbuff,
		u_int maxreslen, lc_callback_check checkcallback, lc_interact_t *interactor,
		int resbuff_not_null = 0)
{
	svr_t *p_svr;
	u_int	server_index;
	lc_request_svr_t *p_lcsvr = NULL;
	handle_t handle;
	u_int retry_time = 0;
	u_int statusmaxtrix[MAX_CONNECT_NUM];
	u_int lastchoose = 0;
	mod_t* p_mod = NULL;

	
	if(NULL == client ||NULL == name ||NULL == reqhead ||NULL == reqbuff ||
		NULL == reshead||NULL == interactor || (NULL == resbuff && !resbuff_not_null)){
		LC_LOG_FATAL("param NULL in lc_client lc_fill_interactor call");
		return -1;
	}
	

	
	memset(statusmaxtrix, 0, sizeof(statusmaxtrix));

	
	interactor->can_send = 0;

	p_lcsvr = lc_fetchserverindex(client, name, &server_index, &p_mod);

	if(NULL == p_lcsvr || NULL == p_mod){
		LC_LOG_FATAL("get server by name error, no such server by name of %s", name);
		return -1;
	}
	
	retry_time = p_lcsvr->retry;
	do{
		if(NULL == (p_svr = lc_fetchserver(client, server_index, hash_key, statusmaxtrix, MAX_CONNECT_NUM, &lastchoose))){
			LC_LOG_WARNING("get server by hashkey error");
			return -1;
		}
		if(0 == lc_fetchsock(p_svr, p_lcsvr, &handle))
			break;
		else if(lastchoose < MAX_CONNECT_NUM){
			
			statusmaxtrix[lastchoose] = 1;
		}
		hash_key += rand();
	}
	
	while(--retry_time > 0 );

	if(retry_time == 0){
		LC_LOG_MONITOR("can't find one server by name of %s after %d times of trying", name, p_lcsvr->retry);
		return -1;
	} else {
		if(retry_time != p_lcsvr->retry) {
			LC_LOG_MONITOR("find one server by name of %s after %d times of trying", name, p_lcsvr->retry-retry_time+1);
		}
	}

	interactor->sock = handle.sock;
	interactor->p_svr = p_svr;
	interactor->rtimeout = p_lcsvr->read_timeout;
	interactor->wtimeout = p_lcsvr->write_timeout;
	interactor->reqhead = reqhead;
	interactor->reqbuff = reqbuff;
	interactor->reshead = reshead;
	interactor->resbuff = resbuff;
	interactor->maxreslen = maxreslen;
	interactor->keepalive = p_lcsvr->connect_type;
	interactor->checkcallback = checkcallback;
	interactor->can_send = 1;
	interactor->nomagiccheck = p_mod->attr.nomagiccheck;
	return 0;
}


static int
lc_client_blcblesortbyname(lc_parallel_t* parallel, u_int count)
{
	lc_parallel_t tmp;
	u_int i, j;
	int ok_done;

	if(NULL == parallel){
		LC_LOG_FATAL("parameter error in blcble sort");
		return -1;
	}

	for(i = 0; i < count; ++i){
		
		parallel[i].lc_client_reserved = i;
	}
	
	for(i = count; i > 0; --i){
		ok_done = 1;
		for(j = 0; j + 1 < i; ++j){
			if(0 < strncmp(parallel[j].name, parallel[j + 1].name, sizeof(parallel[0].name))){
				ok_done = 0;
				memcpy(&tmp, &parallel[j], sizeof(lc_parallel_t));
				memcpy(&parallel[j], &parallel[j + 1], sizeof(lc_parallel_t));
				memcpy(&parallel[j + 1], &tmp, sizeof(lc_parallel_t));
			}
		}
		if(1 == ok_done){
			return 0;
		}
	}
	return 0;
}


static int
lc_client_blcblesortbyid(lc_parallel_t* parallel, u_int count)
{
	lc_parallel_t tmp;
	u_int i, j;
	int ok_done;

	if(NULL == parallel){
		LC_LOG_FATAL("parameter error in blcble sort");
		return -1;
	}

	for(i = count; i > 0; --i){
		ok_done = 1;
		for(j = 0; j + 1 < i; ++j){
			if(parallel[j].lc_client_reserved > parallel[j + 1].lc_client_reserved ){
				ok_done = 0;
				memcpy(&tmp, &parallel[j], sizeof(lc_parallel_t));
				memcpy(&parallel[j], &parallel[j + 1], sizeof(lc_parallel_t));
				memcpy(&parallel[j + 1], &tmp, sizeof(lc_parallel_t));
			}
		}
		if(1 == ok_done){
			return 0;
		}
	}
	return 0;
}





lc_client_t*
lc_client_init(lc_client_attr_t *attr)
{
	lc_client_t* client = (lc_client_t*)calloc(1, sizeof(lc_client_t));
	
	if(NULL == client){
		LC_LOG_FATAL("alloc memory for lc_client_t failed");
		return NULL;
	}

	
	if(0 != lc_queue_init(&client->mod, MAX_SERVERTYPE_NUM, sizeof(mod_t))){
		LC_LOG_FATAL("error when alloc extra memory to lc_client_t");
		free(client);
		return NULL;
	}

	
	if(NULL != attr)
		memcpy(&client->attr, attr, sizeof(lc_client_attr_t));

	srand((unsigned int)time(NULL));
	return client;
}


int
lc_client_close(lc_client_t *client)
{
	mod_t mod;
	svr_t svr;
	handle_t handle;

	
	if(NULL == client){
		LC_LOG_FATAL("param NULL in lc_client close call");
		return -1;
	}
	while (lc_queue_pop(&client->mod, &mod) == 0) {
		while (lc_queue_pop(&mod.svr, &svr) == 0) {
			pthread_mutex_destroy(&svr.mutex);
			pthread_cond_destroy(&svr.cond);
			while (lc_queue_pop(&svr.handle, &handle) == 0) {
				SafeCloseSock(handle.sock);
			}
			lc_queue_close(&svr.handle);
		}
		pthread_mutex_destroy(&mod.mutex);
		lc_queue_close(&mod.pick.status);
		lc_queue_close(&mod.svr);
	}
	lc_queue_close(&client->mod);
	free(client);
	return 0;
}

	int
lc_client_add(lc_client_t *client, const lc_request_svr_t *svr,
		const char *name, const lc_client_mod_attr_t *attr)
{
	mod_t mod_toadd;
	u_int i, j, k;
	svr_t tmp_svr;

	
	if(NULL == client ||NULL == name ||NULL == svr){
		LC_LOG_FATAL("param NULL in lc_client add call");
		return -1;
	}

	if(NULL != lc_fetchserverindex(client, name, &i)){
		LC_LOG_FATAL("server %s already add to list", name);
		return -1;
	}

	
	if(lc_queue_full(&client->mod)){
		LC_LOG_FATAL("queue is full now");
		return -1;
	}

	
	strlcpy(mod_toadd.name, name, sizeof(mod_toadd.name));
	mod_toadd.name[sizeof(mod_toadd.name) - 1] = '\0';

	
	memcpy(&mod_toadd.info, svr, sizeof(mod_toadd.info));

	
	if(0 != lc_queue_init(&mod_toadd.svr, MAX_CONNECT_NUM, sizeof(svr_t))){
		LC_LOG_FATAL("error when alloc memory to client info");
		return -1;
	}

	if(svr->num <= 0){
		LC_LOG_FATAL("server number range error");
		return -1;
	}

	
	if(0 != pthread_mutex_init(&(mod_toadd.mutex), NULL))
	{
		LC_LOG_FATAL("init mod mutex error");
		return -1;
	}


	
	for(i = 0; i < svr->num; ++i){
		memset(&tmp_svr, 0 , sizeof(tmp_svr));
		
		if(0 != pthread_mutex_init(&(tmp_svr.mutex), NULL))
		{
			LC_LOG_FATAL("init server mutex error");
			return -1;
		}
		
		if(0 != pthread_cond_init(&(tmp_svr.cond), NULL))
		{
			LC_LOG_FATAL("init server condition error");
			return -1;
		}
		
		tmp_svr.work = 0;
		for(j = 0; j < LC_CONF_MAX_CLONE; ++j){
			tmp_svr.myserver.ip[j][0] = '\0';
		}

		if(svr->port <= 0){
			LC_LOG_FATAL("server port range error");
			return -1;
		}

		if(svr->read_timeout <= 0){
			LC_LOG_FATAL("server read timeout range error");
			return -1;
		}

		if(svr->write_timeout <= 0){
			LC_LOG_FATAL("server write timeout range error");
			return -1;
		}

		if(svr->connect_timeout	<= 0){
			LC_LOG_FATAL("server connect timeout range error");
			return -1;
		}


		
		
		if(svr->ip_list[i].num <= 0 ||svr->ip_list[i].num >= LC_CONF_MAX_CLONE){
			LC_LOG_FATAL("server iplist config error");
			return -1;
		}
		for(k = 0; k < svr->ip_list[i].num; ++k)
			strlcpy(tmp_svr.myserver.ip[k], svr->ip_list[i].name[k], sizeof(tmp_svr.myserver.ip[0]));

		tmp_svr.myserver.port	= svr->port;
		tmp_svr.myserver.read_timeout = svr->read_timeout;
		tmp_svr.myserver.write_timeout = svr->write_timeout;
		tmp_svr.myserver.connect_timeout = svr->connect_timeout;
		tmp_svr.myserver.socktype = 0;
		if (svr->linger) {
			tmp_svr.myserver.socktype |=  LC_CLIENT_LINGER_OPEN;
		}

		
		if(0 != lc_queue_init(&tmp_svr.handle, svr->max_connect, sizeof(handle_t))){
			LC_LOG_FATAL("error when alloc memory to client info");
			return -1;
		}

		if(0 != lc_queue_push(&mod_toadd.svr, &tmp_svr)){
			LC_LOG_FATAL("error when push info memory to client info");
			return -1;
		}
	}

	
	if(0 != lc_queue_init(&mod_toadd.pick.status, MAX_CONNECT_NUM, sizeof(int))){
		LC_LOG_FATAL("error when alloc memory to client info to store status");
		return -1;
	}

	
	if(svr->retry <= 0){
		LC_LOG_FATAL("server retry config error");
		return -1;
	}
	mod_toadd.pick.retry = svr->retry;

	
	if(svr->connect_type > 1){
		LC_LOG_FATAL("server connect_type config error");
		return -1;
	}
	mod_toadd.pick.alive = svr->connect_type;

	
	if(NULL != attr)
		memcpy(&mod_toadd.attr, attr, sizeof(lc_client_mod_attr_t));
	else
		memset(&mod_toadd.attr, 0, sizeof(lc_client_mod_attr_t));

	
	if(0 != lc_queue_push(&client->mod, &mod_toadd)){
		LC_LOG_FATAL("error when add server to lc_client_t");
		return -1;
	}

	return 0;
}


int
lc_client_orginvite(lc_client_t *client, const char *name, u_int hash_key,
		nshead_t *reqhead, void *reqbuff, nshead_t *reshead, void *resbuff,
		u_int maxreslen, lc_callback_check checkcallback)
{
	lc_interact_t interactor;
	
	if(NULL == client ||NULL == name ||NULL == reqbuff ||NULL == resbuff ||NULL == reqhead || NULL == reshead){
		LC_LOG_FATAL("param NULL in lc_client invitenshead call");
		return -1;
	}

	if (UINT_MAX == hash_key) {
	  hash_key = rand();
	}

	
	if(0 != lc_fill_interactor(client, name, hash_key, reqhead, reqbuff,
				reshead, resbuff, maxreslen, checkcallback, &interactor)){
		return -1;
	}

	if(0 != lc_interactwith_server(client, NULL, &interactor, 1)){
		return -1;
	}
	return 0;
}


int
lc_client_invite(lc_client_t *client, const char *name, u_int hash_key,
		nshead_t *reqhead, nshead_t *reshead,
		u_int maxreslen, lc_callback_check checkcallback)
{
	void *reqbuff, *resbuff;
	if(NULL == reqhead ||NULL == reshead){
		LC_LOG_MONITOR("lc_client_invite call parrameters error, reqhead or orshead is null");
		return -1;
	}
	reqbuff = (void*)&reqhead[1];
	resbuff = (void*)&reshead[1];
	return lc_client_orginvite(client, name, hash_key, reqhead, reqbuff, \
			reshead, resbuff, maxreslen, checkcallback);
}


int
lc_client_groupinvite(lc_client_t *client, lc_parallel_t* parallel, u_int ms_timeout)
{
	u_int i = 0;
	
	if(NULL == client ||NULL == parallel){
		LC_LOG_FATAL("param NULL in lc_client call");
		return -1;
	}

	
	while(parallel[i].name[0] != '\0')
		i++;

	
	lc_client_basicgroupinvite(client, parallel, i, ms_timeout);
	return 0;
}

static 
int lc_client_groupinvite_init(lc_client_t *client, lc_parallel_t* parallel,
		lc_interact_t *interactor, u_int count, u_int async = LC_CLIENT_SYNC)
{
	u_int	server_index;
	lc_request_svr_t  *p_lcsvr = NULL;
	u_int i = 0, k, l;
	mod_t*  my_mod;
	u_int phash_key;

	
	if(NULL == client ||NULL == parallel){
		LC_LOG_FATAL("param NULL in lc_client call");
		return -1;
	}

	if(count > MAX_PARALLEL_TAIL){
		LC_LOG_FATAL("lc_client is not support for count(%d) more then %d", count, MAX_PARALLEL_TAIL);
		return -1;
	}

	for(i = 0; i < count; ++i){
		interactor[i].can_send = 0;
		if (LC_CLIENT_SYNC == async) {
			if(NULL == parallel[i].reqbuff && NULL != parallel[i].reqhead) {
				parallel[i].reqbuff = (void*)&(parallel[i].reqhead[1]);
			}
			if(NULL == parallel[i].resbuff && NULL != parallel[i].reshead) {
				parallel[i].resbuff = (void*)&(parallel[i].reshead[1]);
			}
		}
	}

	lc_client_blcblesortbyname(parallel, count);

	
	for(i = 0; i < count; ++i){
		if('\0' == parallel[i].name[0]){
			LC_LOG_FATAL("server name null error");
			
			continue;
		}

		
		k = i + 1;
		while(k < count && 0 == strncmp(parallel[k].name, parallel[i].name, sizeof(parallel[0].name)))
			++k;

		
		if(k != i + 1){
			p_lcsvr = lc_fetchserverindex(client, parallel[i].name, &server_index, &my_mod);
			
			if(NULL == p_lcsvr){
				LC_LOG_FATAL("no server by name of %s exist in lc_client_t", parallel[i].name);
				i = k;
				continue;
			}
			
			if(k - i > p_lcsvr->max_connect){
				LC_LOG_FATAL("max_connect:%d of server:%s is lower then you wanted:%d, deadlock will happen,"
						"lc_client prevent it from happen", p_lcsvr->max_connect, parallel[i].name, k - i);
				
				i = k - 1;
				continue;
			}

			
			pthread_mutex_lock(&my_mod->mutex);

			for(l = i; l < k; ++l){
				
				if (UINT_MAX == parallel[l].hash_key) {
					phash_key = rand();
				} else {
					phash_key = parallel[l].hash_key;
				}


				
				p_lcsvr = lc_fetchserverindex(client, parallel[l].name, &server_index);
				if(NULL == p_lcsvr){
					LC_LOG_FATAL("no server by name of %s exist in lc_client_t", parallel[l].name);
					continue;
				}
				
				if(0 != lc_fill_interactor(client, parallel[l].name, phash_key,\
							parallel[l].reqhead, parallel[l].reqbuff, \
							parallel[l].reshead, parallel[l].resbuff, parallel[l].maxreslen,\
							parallel[l].checkcallback, &interactor[l], async)){
					LC_LOG_WARNING("prepare to connections to server:%s failed", parallel[l].name);
				}

			}
			
			pthread_mutex_unlock(&my_mod->mutex);
			i = k - 1;
		}
		else{
			if (UINT_MAX == parallel[i].hash_key) {
				phash_key = rand();
			} else {
				phash_key = parallel[i].hash_key;
			}
			
			if(0 != lc_fill_interactor(client, parallel[i].name, phash_key,\
						parallel[i].reqhead, parallel[i].reqbuff, parallel[i].reshead, \
						parallel[i].resbuff, parallel[i].maxreslen,\
						parallel[i].checkcallback, &interactor[i], async)){
				LC_LOG_WARNING("prepare to connections to server:%s failed", parallel[i].name);
			}
		}
	}
	return 0;
}

int
lc_client_basicgroupinvite(lc_client_t *client, lc_parallel_t* parallel, u_int count, u_int ms_timeout)
{

	lc_interact_t interactor[MAX_PARALLEL_TAIL];
	if (lc_client_groupinvite_init(client, parallel, interactor, count) < 0) {
		return -1;
	}
	
	if(0 != lc_interactwith_server(client, parallel, interactor, count, ms_timeout)){
		return -1;
	}
	lc_client_blcblesortbyid(parallel, count);

	return 0;
}
int
lc_client_basicgroupinvite_async(lc_client_t *client, lc_parallel_t* parallel,
		u_int count, u_int ms_timeout, lc_callback_alloc alloccallback,
		lc_callback_clear clearcallback, void *alloc_arg)
{
	lc_recv_callback_t callback;
	lc_interact_t interactor[MAX_PARALLEL_TAIL];
	if(alloccallback != NULL && NULL == clearcallback) {
		LC_LOG_FATAL("param alloccallback is set, clearcallback should be set"); 
		return -1;
	}
	if (lc_client_groupinvite_init(client, parallel, interactor, count, LC_CLIENT_ASYNC) < 0) {
		return -1;
	}
	callback.alloccallback = alloccallback;
	callback.clearcallback = clearcallback;
	callback.alloc_arg = alloc_arg;
	
	if(0 != lc_interactwith_server(client, parallel, interactor,
				count, ms_timeout, LC_CLIENT_ASYNC, &callback)){
		return -1;
	}
	lc_client_blcblesortbyid(parallel, count);
	return 0;
}


int
lc_client_groupinvite_async(lc_client_t *client, lc_parallel_t* parallel,
		u_int ms_timeout, lc_callback_alloc alloccallback,
		lc_callback_clear clearcallback, void *alloc_arg)
{
	u_int i = 0;
	
	if(NULL == client ||NULL == parallel){
		LC_LOG_FATAL("param NULL in lc_client call");
		return -1;
	}

	
	while(parallel[i].name[0] != '\0')
		i++;

	
	lc_client_basicgroupinvite_async(client, parallel, i, ms_timeout,
			alloccallback, clearcallback, alloc_arg);
	return 0;
}





