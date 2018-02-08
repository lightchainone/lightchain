

#ifndef  __STRATEGY_H_
#define  __STRATEGY_H_

#include "connection.h"

namespace comcm{
	enum{
		
		REQ_OK = 0,
		REQ_SVR_FULL,
		REQ_CONN_FAILED,
		REQ_SVR_DISABLE,
        REQ_SVR_SELECT_FAIL
	};

	enum{
		
		ERR_OK = 0,
		ERR_TALK_FAIL, 
		ERR_DEAD_LIKE, 
		ERR_OTHER
	};

	enum{
		CONN_FREE = 1
	};


	class ConnectionRequest{
		plclic:
		int key;		  
		int nthRetry;		  
		int serverID;		  
		int err;		  
		Connection * conn;		  
		const char * tag;		  
		ConnectionRequest(){};
		virtual ~ConnectionRequest(){};
	};


	
	class Strategy{
		protected:
			ConnectManager * _mgr;
			BasicLogAdapter * _log;
			time_t _last_reload;
		plclic:
			
			
			virtual int selectServer(const ConnectionRequest * req);
			
			
			virtual int notify(Connection * conn, int eve);

            
            virtual int update();
			
			int setManager(ConnectManager * ); 
			
			const ConnectManager * getManager();

			Strategy();
			virtual ~Strategy();
	};
	
	class HealthyChecker{
		protected:
			ConnectManager * _mgr;
			BasicLogAdapter * _log;
			pthread_t _tid;
			bool _single_thread;
			friend void * thread_worker(void * pd);
			struct AsyncWorker{
				int seconds;
				HealthyChecker * checker;
				bool run;
			}_worker;
		plclic:
			
			
			virtual int healthyCheck();
			
			
			int startSingleThread(int seconds = 5);
			
			void joinThread();
			
			int setManager(ConnectManager *);
			
			const ConnectManager * getManager();
			HealthyChecker();
			virtual ~HealthyChecker();
	};

}













#endif  


