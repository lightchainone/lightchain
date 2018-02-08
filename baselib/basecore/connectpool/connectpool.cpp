
#include "connectpool.h"

#include "ul_net.h"
#include "ul_log.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <sys/socket.h>
#include <netinet/tcp.h>

#define SafeFree(p) if (p) { free(p); p=NULL; }
#define SafeCloseSock(s) if (s>=0) { int temp_s=s; s=-1; close(temp_s); }
#define SafeDelete(p) if (p) { delete (p); p=NULL; };

ConnectPool::ConnectPool()
{
	m_serverCount = 0;
	m_serverArr = NULL;
	m_sockArr = NULL;
	m_freeSockCount= 0;
	m_retryCount = 1;
	m_nextServerIdx = 0;
	m_nextAliveServerIdx = 0;

	m_minVisitRatio = 2.0;
	m_visitRecordSize = 1000;
	m_visitFailThreshold = 600;
	m_failScale = 3.0;
	m_lvtQueueSize = 2;

	pthread_mutex_init(&m_mutex, NULL);
    pthread_cond_init(&m_condition, NULL);
}

ConnectPool::~ConnectPool()
{
	if (m_sockArr) {
		for (int i=0; i<m_serverCount*m_sockPerServer; i++) {
			SafeCloseSock(m_sockArr[i].sock);
		}
	}
	if (m_serverArr) {
		for (int i=0; i<m_serverCount; i++) {
			SafeFree(m_serverArr[i].record_queue);
		}
	}
	SafeFree(m_serverArr);
	SafeFree(m_sockArr);
	
	pthread_cond_destroy(&m_condition);
	pthread_mutex_destroy(&m_mutex);
}

void ConnectPool::SetLogInfo(const char modulename[], int warning_level, int notice_level, int debug_level)
{
	m_log.warning_level = warning_level;
	m_log.notice_level = notice_level;
	m_log.debug_level = debug_level;

	if(modulename) {
		strncpy(m_log.modulename, modulename, sizeof(m_log.modulename)-1);
	} else {
		m_log.modulename[0] = '\0';
	}
}

int ConnectPool::SetBalanceInfo(int visitRecordSize, int visitFailThreshold, dolcle failScale, dolcle minVisitRatio, int lvtQueueSize)
{
	if(minVisitRatio < 0 || minVisitRatio > 100) {
		return ERR_PARAM;
	}
	m_minVisitRatio = minVisitRatio;

	if(visitFailThreshold <= 0 || visitRecordSize <= 0 || visitFailThreshold > visitRecordSize) {
		return ERR_PARAM;
	}
	m_visitRecordSize = visitRecordSize;
	m_visitFailThreshold = visitFailThreshold;

	if(failScale < 1.0) {
		return ERR_PARAM;
	}
	m_failScale = failScale;

	if(lvtQueueSize > server_info_t::MAX_LVT_QUEUE_SIZE || lvtQueueSize < 0) {
		return ERR_PARAM;
	}
	m_lvtQueueSize = lvtQueueSize;

	return 0;
}

int ConnectPool::Init(const server_conf_t serverConfs[], int serverCount, int sockPerServer, 
					  int connectRetry, bool longConnect)
{
	if(serverConfs==NULL || serverCount<=0 || sockPerServer <= 0 ) {
		return ERR_PARAM;
	}
	if(connectRetry < 0) {
		return ERR_PARAM;
	}

	SafeFree(m_serverArr);
	m_serverArr = (server_info_t*)calloc(serverCount, sizeof(server_info_t));
	if(!m_serverArr) {
		return ERR_MEM;
	}

	m_serverCount = serverCount;
	m_sockPerServer = sockPerServer;

	SafeFree(m_sockArr);
	m_sockArr = (sock_info_t*)calloc(m_serverCount*m_sockPerServer, sizeof(sock_info_t));
	if(!m_sockArr) {
		return ERR_MEM;
	}

	for(int i=0; i<m_serverCount; ++i) {
		m_serverArr[i].socks = m_sockArr + i*sockPerServer;
	}

	m_freeSockCount = m_serverCount*m_sockPerServer;
	for (int i=0; i<m_serverCount; ++i) {
		const server_conf_t& conf = serverConfs[i];
		server_info_t& server = m_serverArr[i];
		
		if(conf.addr[0]=='\0' || conf.port<=0) {
			return ERR_PARAM;
		}

		strncpy(server.addr, conf.addr, sizeof(server.addr));
		server.addr[sizeof(server.addr)-1] = 0;
		server.port = conf.port;
		server.ctimeout_ms = conf.ctimeout_ms;

		server.cur_pos = 0;
		server.free_count = m_sockPerServer;
		for(int j=0; j<m_sockPerServer; ++j) {
			server.socks[j].sock = -1;
			server.socks[j].status = CPS_INVALID;
		}
		server.queue_pos = 0;
		server.queue_size = m_visitRecordSize;
		server.record_queue = (server_info_t::visit_record_t*) calloc(server.queue_size, sizeof(server_info_t::visit_record_t));
		if(server.record_queue == NULL) {
			return ERR_MEM;
		}
		server.fail_count = 0;

		memset(server.lvt_queue, 0, sizeof(server.lvt_queue));
		server.lvt_queue_size = m_lvtQueueSize;
		server.lvt_queue_pos = 0;
	}

	m_longConnect = longConnect;
	m_retryCount = connectRetry;

	ul_writelog(m_log.debug_level, "[%s] ConnectPool Init, socket count [%d]", m_log.modulename, m_serverCount);
	return 0;
}

int ConnectPool::ConnectServer(int idx)
{
	if(idx<0 || idx>=m_serverCount*m_sockPerServer) {
		return ERR_ASSERT;
	}

	if(m_sockArr[idx].status != CPS_BUSY) {
		return ERR_ASSERT;
	}

	int ret = ERR_FAILED;

	server_info_t& server = m_serverArr[idx/m_sockPerServer];
	int sock = ul_tcpconnecto_ms(server.addr, server.port, server.ctimeout_ms);

	if (sock<0) {

		ul_writelog(m_log.warning_level, "[%s] ul_tcpconnecto_ms failed addr(%s) port(%d) timeout(%d) ERR[%m]",
						m_log.modulename, server.addr, server.port, server.ctimeout_ms);
		ret = ERR_CONN;
	} else {
		pthread_mutex_lock(&m_mutex);
		m_sockArr[idx].sock = sock;
		pthread_mutex_unlock(&m_mutex);

		int on =1;
		if (setsockopt(m_sockArr[idx].sock,IPPROTO_TCP,TCP_NODELAY,&on, sizeof(on)) < 0) {
			ul_writelog(m_log.warning_level, "(setsockopt(fd,IPPROTO_TCP,TCP_NODELAY,&on, sizeof(on)) ERR(%m)");
		}
		ret = 0;
	}
	
	return ret;
}


int ConnectPool::__find_free(sock_info_t* array, int size)
{
	for(int i=0; i<size; ++i) {
		if(array[i].status != CPS_BUSY) {
			return i;
		}
	}
	return -1;
}

int ConnectPool::__find_free(server_info_t* array, int size, int* cur_pos, int find_alive_only)
{
	for(int i=0; i<size; ++i) {
		int idx = i; 
		if(cur_pos) {
			idx = *cur_pos;
			*cur_pos = (*cur_pos+1) % size;
		}
		server_info_t& server = array[idx];
		if(server.free_count>0) {
			if(!find_alive_only) {
				return idx;
			} 

			if(server.fail_count>=m_visitFailThreshold || server.is_last_visit_failed()) {
				ul_writelog(m_log.debug_level, "[%s] __find_free_alive failed[%s]! server_idx[%d], fail_count[%d]", 
							m_log.modulename, server.fail_count>=m_visitFailThreshold ? "not alive" : "last visit failed",
							idx, server.fail_count);
				int min_ratio = static_cast<int>(m_minVisitRatio * (ratio_precision/100));
				if(rand()%ratio_precision < min_ratio) {
					return idx;
				} 
			} else {
				return idx;
			}
		}
	}
	return -1;
}

bool ConnectPool::is_use_select_server(int idx)
{
	server_info_t& server = m_serverArr[idx];

	if(server.free_count <= 0) {
		return false;
	}

	if(server.free_count == m_freeSockCount) {
		return true;
	}

	
	int min_ratio = static_cast<int>(m_minVisitRatio * (ratio_precision/100));

	
	if(server.is_last_visit_failed()) {
		ul_writelog(m_log.debug_level, "[%s] is_last_visit_failed! server_idx[%d]", 
					m_log.modulename, idx);
		return (rand()%ratio_precision < min_ratio);
	}

	
	dolcle fail_ratio = 1.0*ratio_precision*server.fail_count/m_visitRecordSize;
	dolcle visit_ratio1 = ratio_precision - fail_ratio*m_failScale;
	dolcle visit_ratio2 = (ratio_precision-fail_ratio) / m_failScale;

	int visit_ratio = static_cast<int>(visit_ratio1>visit_ratio2 ? visit_ratio1 : visit_ratio2);
	if(visit_ratio < min_ratio) {
		visit_ratio = min_ratio;
	}

	bool is_use = (rand()%ratio_precision < visit_ratio);
	ul_writelog(m_log.debug_level, "[%s] is_use_select_server[%d], visit_ratio[%d]", 
				m_log.modulename, is_use, visit_ratio);

	return is_use;
}

int ConnectPool::__check_connection(int sock)
{
	char buf[1];
	ssize_t ret = recv(sock, buf, sizeof(buf), MSG_DONTWAIT);
	if(ret>0) {
		ul_writelog(m_log.warning_level, "[%s] check_connection: some data pending, error accur! sock[%d]", 
					m_log.modulename, sock);
		return -1;
	} else if (ret==0) {
		ul_writelog(m_log.debug_level, "[%s] check_connection: connection close by peer! sock[%d]", 
					m_log.modulename, sock);
		return -1;
	} else {
		if(errno == EWOULDBLOCK) {
			return 0;
		} else {
			ul_writelog(m_log.warning_level, "[%s] check_connection: read error[%d] on sock[%d] ERR[%m]", 
						m_log.modulename, errno, sock);
			return -1;
		}
	}
}

int ConnectPool::FetchSocket(int balanceCode)
{
	if (m_serverCount==0) {
		ul_writelog(m_log.warning_level, "[%s] m_serverCount==0, Init may not be called", m_log.modulename);
		return ERR_ASSERT;
	}
	
	int retry = m_retryCount;

	while (retry >= 0) {
		int old_status;
		int sockIdx = __FetchSocket(balanceCode, old_status);
		if(sockIdx < 0) {
			return ERR_ASSERT;
		}

		
		if (m_longConnect) {
			if (old_status == CPS_READY) {
				ul_writelog(m_log.debug_level, "[%s] m_sockArr[sockIdx].status==CPS_READY ", m_log.modulename);
				return m_sockArr[sockIdx].sock;
			} else { 
				ul_writelog(m_log.debug_level, "[%s] m_sockArr[sockIdx].status==CPS_INVALID ", m_log.modulename);
				
				if (ConnectServer(sockIdx)>=0) {
					return m_sockArr[sockIdx].sock;
				} else {
					ul_writelog(m_log.warning_level, "[%s]Long ConnectServer(sockIdx [%d]) failed", m_log.modulename, sockIdx);	
					int ret = __FreeSocket(sockIdx, true);
					if(ret < 0) {
						ul_writelog(UL_LOG_FATAL, "[%s:%d] __FreeSocket failed, sockIdx[%d]", 
									m_log.modulename, __LINE__, sockIdx);
						return ERR_ASSERT;
					}
				}
			} 
					
		} else {
			if (ConnectServer(sockIdx)>=0) {
				return m_sockArr[sockIdx].sock;
			} else {
				ul_writelog(m_log.warning_level, "[%s]Long short ConnectServer(sockIdx [%d]) failed", m_log.modulename, sockIdx);	
				int ret = __FreeSocket(sockIdx, true);
				if(ret < 0) {
					ul_writelog(UL_LOG_FATAL, "[%s:%d] __FreeSocket failed, sockIdx[%d]", 
								m_log.modulename, __LINE__, sockIdx);
					return ret;
				}
			}
		}

		retry --;
	}

	return ERR_CONN;
}

int ConnectPool::select_server(int balanceCode)
{
	int select_idx = -1;
	if(balanceCode >= 0) {
		
		select_idx = balanceCode % m_serverCount;
		if(is_use_select_server(select_idx)) {
			return select_idx;
		}

		ul_writelog(m_log.notice_level, "[%s] __FetchSocket not use select server[%d] free_count[%d] fail_count[%d]", 
					m_log.modulename, select_idx, 
					m_serverArr[select_idx].free_count, m_serverArr[select_idx].fail_count);	

		assert(m_serverCount>1);

		
		int abandon_idx = select_idx;
		int second_idx = balanceCode % (m_serverCount-1);
		select_idx = second_idx<abandon_idx ? second_idx : second_idx+1;

		if(is_use_select_server(select_idx)) {
			return select_idx;
		}

		ul_writelog(m_log.notice_level, "[%s] __FetchSocket not use second select server[%d] free_count[%d] fail_count[%d]", 
					m_log.modulename, select_idx, 
					m_serverArr[select_idx].free_count, m_serverArr[select_idx].fail_count);	
	}

	
	select_idx = __find_free(m_serverArr, m_serverCount, &m_nextAliveServerIdx, true);
	if(select_idx >= 0) {
		return select_idx;
	}

	
	select_idx = __find_free(m_serverArr, m_serverCount, &m_nextServerIdx, false);
	assert(select_idx >= 0);

	return select_idx;
}

int ConnectPool::__FetchSocket(int balanceCode, int& old_status)
{
	pthread_mutex_lock(&m_mutex);
	
	while (m_freeSockCount<=0) {
		pthread_cond_wait(&m_condition, &m_mutex);
	}
		
	int select_idx = select_server(balanceCode);

	server_info_t& server = m_serverArr[select_idx];
	int sock_idx = __find_free(server.socks, m_sockPerServer);
	assert(sock_idx >= 0);
		
	old_status = server.socks[sock_idx].status;
	server.socks[sock_idx].status = CPS_BUSY;
	m_freeSockCount--;
	server.free_count--;
	
	if (old_status == CPS_READY) {
		if (__check_connection(server.socks[sock_idx].sock))  {
			SafeCloseSock(server.socks[sock_idx].sock);
			old_status = CPS_INVALID;
		}
	} 
		
	pthread_mutex_unlock(&m_mutex);

	ul_writelog(m_log.debug_level, "[%s]cur connect return server index[%d] socket index[%d]", 
				m_log.modulename, select_idx, sock_idx);	

	return sock_idx + select_idx*m_sockPerServer;
}

int ConnectPool::FreeSocket(int sock, bool errclose)
{
	int idx = m_serverCount*m_sockPerServer - 1;

	if (sock < 0) {
		return ERR_ASSERT;
	}

	
	
	while (idx>=0 && m_sockArr[idx].sock!=sock) {
		idx--;		
	}
	
	if (idx<0) {
		ul_writelog(m_log.warning_level, "[%s] ConnectPool::FreeSocket sock[%d] not found in pool", 
					m_log.modulename, sock);
		return ERR_SOCK;	
	}

	return __FreeSocket(idx, errclose);
}

int ConnectPool::__FreeSocket(int idx, bool errclose)
{
	assert(idx>=0 && idx<m_serverCount*m_sockPerServer);

	int ret = ERR_FAILED;

	pthread_mutex_lock(&m_mutex);
		
	if (m_sockArr[idx].status==CPS_BUSY) {
		
		server_info_t& server = m_serverArr[idx/m_sockPerServer];

		if (m_longConnect && !errclose) {
			m_sockArr[idx].status = CPS_READY;
			ul_writelog(m_log.debug_level, "[%s]  m_longConnect && !errclose  CPS_READY", m_log.modulename);
		} else {
			SafeCloseSock(m_sockArr[idx].sock);
			m_sockArr[idx].status = CPS_INVALID;
			ul_writelog(m_log.debug_level, "[%s]  CPS_INVALID", m_log.modulename);
		}

		server.queue_insert(errclose ? 1 : 0);
		server.lvt_insert(errclose ? pthread_self() : 0);
		
		m_freeSockCount++;
		server.free_count++;
		ret = 0;
	} else {
		ul_writelog(m_log.warning_level, "[%s] ConnectPool::FreeSocket m_sockArr[idx].status[%d]!=CPS_BUSY", 
					m_log.modulename, m_sockArr[idx].status);
		ret = ERR_SOCK;
	}
  	
	pthread_cond_signal(&m_condition);
	pthread_mutex_unlock(&m_mutex);

	return ret;
}

int ConnectPool::QueryWithRetry(query_func_t func, void* arg0, void *arg1, void* arg2, int* funcRet,
								  int retryCount, int balanceCode)
{
	if(func == NULL) {
		return ERR_PARAM;
	}

	int retry = retryCount;
	while(retry >= 0) {
		int ret = FetchSocket(balanceCode);
		if(ret < 0) {
			return ret;
		}

		int sock = ret;
		ret = func(sock, arg0, arg1, arg2);
		if(funcRet) {
			*funcRet = ret;
		}

		bool need_retry = (ret==FAILED_AND_RETRY);
		ret = FreeSocket(sock, need_retry);
		if(ret < 0) {
			return ret;
		}

		if(!need_retry) {
			return 0;
		}
		
		retry --;
	}

	ul_writelog(m_log.warning_level, "[%s] query_with_retry: all retry[%d] failed", 
				m_log.modulename, retryCount);
	return ERR_RETRY;
}
