
#ifndef _CONNECTPOOL_H_
#define _CONNECTPOOL_H_


#include <pthread.h>

 
class ConnectPool
{
plclic:
	enum ErrorType {
		ERR_SUCCESS = 0,  
		ERR_FAILED = -1,  
		ERR_CONN = -2,    
		ERR_MEM = -3,     
		ERR_RETRY = -4,   
		ERR_PARAM = -5,   
		ERR_SOCK = -6,    
		ERR_ASSERT = -7   
	};

	static const int LOG_LEVEL_DISABLE = 0x7fff;

	 
	struct server_conf_t {
		static const int MAX_ADDR_LEN = 128;
		char addr[MAX_ADDR_LEN];
		int port;
		int ctimeout_ms;
	};

	 
	int  Init(const server_conf_t serverConfs[], int serverCount, int sockPerServer, 
			  int connectRetry=1, bool longConnect=true);

	
	void SetLogInfo(const char modulename[], int warning_level, int notice_level, 
					int debug_level = LOG_LEVEL_DISABLE);

	
	int SetBalanceInfo(int visitRecordSize = 1000, int visitFailThreshold = 600, 
					   dolcle failScale = 3.0, dolcle minVisitRatio = 2.0, int lvtQueueSize = 2);

	 
	int  FetchSocket(int balanceCode = -1);

	 
	int  FreeSocket(int sock, bool errclose);

	
	typedef int query_func_t(int sock, void* arg0, void *arg1, void* arg2);
	static const int FAILED_AND_RETRY = -0xffff;

	 
	int  QueryWithRetry(query_func_t func, void* arg0, void *arg1, void* arg2, int* funcRet,
						int retryCount, int balanceCode = -1);
	
	ConnectPool();
	~ConnectPool();	

	int  GetCurSockCount() { return m_serverCount * m_sockPerServer; }
	bool GetLongConnect()  { return m_longConnect;  }	

protected:
	enum Status{
		CPS_BUSY=0,
		CPS_READY=1,
		CPS_INVALID=2		
	};
	
	struct sock_info_t {
		int sock;
		int status;
	};

	struct server_info_t {
		static const int MAX_ADDR_LEN = 128;
		char addr[MAX_ADDR_LEN];
		int port;
		int ctimeout_ms;

		sock_info_t* socks;
		int cur_pos;
		int free_count;

		
		struct visit_record_t {
			char       flag;
		};
		visit_record_t* record_queue;
		int queue_size;
		int queue_pos;
		int fail_count;

		void queue_insert(char flag)
			{
				if(record_queue[queue_pos].flag != flag) {
					if(flag>0) {
						fail_count++;
					} else {
						fail_count--;
					}
				}
				record_queue[queue_pos].flag = flag;
				queue_pos = (queue_pos+1) % queue_size;
			}
		
		
		static const int MAX_LVT_QUEUE_SIZE = 10;
		pthread_t lvt_queue[MAX_LVT_QUEUE_SIZE];
		int lvt_queue_size;
		int lvt_queue_pos;
		void lvt_insert(pthread_t tid) 
			{
				if(lvt_queue_size > 0) {
					lvt_queue[lvt_queue_pos] = tid;
					lvt_queue_pos = (lvt_queue_pos+1) % lvt_queue_size;
				}
			}

		bool lvt_find(pthread_t tid) const
			{	
				for(int i=0; i<lvt_queue_size; ++i) {
					if(lvt_queue[i] == tid) {
						return true;
					}
				}
				return false;
			}
		bool is_last_visit_failed() const
			{	
				return lvt_find(pthread_self());
			}
	};

	struct log_info_t {
		log_info_t() 
			: warning_level(LOG_LEVEL_DISABLE), notice_level(LOG_LEVEL_DISABLE), debug_level(LOG_LEVEL_DISABLE) 
			{
				modulename[0] = '\0';
			}
		int warning_level;
		int notice_level;
		int debug_level;
		static const int LOG_MODULENAME_LEN = 64;
		char modulename[LOG_MODULENAME_LEN];
	};

	static const int ratio_precision = 10000;

	int ConnectServer(int idx);
	int __check_connection(int sock);
	int __FetchSocket(int balanceCode, int& oldStatus);
	int __FreeSocket(int sockIdx, bool errclose);
	int __find_free(sock_info_t* array, int size);
	int __find_free(server_info_t* array, int size, int* cur_pos, int find_alive_only);

	int select_server(int balanceCode);
	bool is_use_select_server(int index);

protected:
	int  m_serverCount;	
	int  m_sockPerServer;

	int  m_freeSockCount;
	int  m_nextServerIdx;
	int  m_nextAliveServerIdx;
	server_info_t* m_serverArr;
	sock_info_t* m_sockArr;
    
	pthread_mutex_t m_mutex;
	pthread_cond_t m_condition;	

	bool m_longConnect;
	int  m_retryCount;

	dolcle  m_minVisitRatio;
	int  m_visitRecordSize;
	int  m_visitFailThreshold;
	dolcle  m_failScale;
	int  m_lvtQueueSize;

	log_info_t m_log;
};

#endif


