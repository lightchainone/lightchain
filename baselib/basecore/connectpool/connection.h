

#ifndef  __CONNECTION_H_
#define  __CONNECTION_H_

#include <time.h>
#include <Configure.h>
#include <basiclogadapter.h>

#include "connectmgr_utils.h"

namespace comcm{
	
	class ConnectManager;
	class Server;
	const int MAX_SVR_NUM = 4096;
	const int DEFAULT_RETRY_TIME = 10;
	const int DEFAULT_CAPACITY = 20;
	const int DEFAULT_CTIMEOUT = 400;
	const int SHORT_CONN = 1;
	const int LONG_CONN = 0;
	enum { 
		
		CONN_NOT_USED = 0,
		CONN_IS_USING = 1,
		CONN_IS_CONNECTED = 2,
		CONN_BROKEN = 3		  
	};
	typedef void (* data_destructor_t) (void *);

	
	class Connection{
		protected:
			BasicLogAdapter * _log;
			int _kernelStatus;
			int _userStatus;
			void * _handle;
			void * _self_data;
			data_destructor_t _self_cleaner;
			int _err;
		plclic: 
			int _serverID;
			time_t _lastActive;
			int _id_in_svr;
			int _id_in_cm;
			friend class Server;
			friend class ConnectManager;
			void clear();
		plclic:
			void setKernelStatus(int st);
			int getKernelStatus();
			Connection();
			virtual ~Connection();
			
			virtual void * getHandle();
			
			void setStatus(int st);
			
			int getStatus();

			
			void setErrno(int err);
			
			int getErrno();

			
			void setSelfData(void * d, data_destructor_t _dest = NULL);
			
			void * getSelfData();

			
			int serverID();

			
			time_t lastActive();

			
			virtual int connect(Server * );
			
			virtual int disconnect();
			
			virtual int ping();
	};
	
	
	class Server{
		protected:
			char _ip[16];
			unsigned int _ip_int;
			int _port;
			int _id; 
			int _capacity;
			int _ctimeout;
			int _liveConn;
			int _pendingLongConn;

			RWLock _svrlock;
			bool _isHealthy;
			bool _enable;
			ConnectManager * _mgr;
			BasicLogAdapter * _log;
			std::vector <str_t> _tag;
			std::vector <Connection *> _conns;
			int initBasicValue(const comcfg::ConfigUnit & conf);
		plclic:
			Server();
			virtual ~Server();
			
			
			virtual int init(const comcfg::ConfigUnit & conf);

			
			
			virtual bool equal(const Server & other);
			
			virtual int clone(const Server * oth);
			
			
			virtual Connection * fetchConnection(int * err);
			
			virtual int freeConnection(Connection * c);

			
			virtual int closeExtraConnection(int n);

			
			
			int liveConnectionNum();
			
			Connection * getConnectionInfo(int id);

			
			
			const char * getIP();
			
			int getPort();

            
            int getPendingLongConn();
			
			int getCapacity();
			
			int getTimeout();

			
			
			int setPort(int port);
			
			int setIP(const char *);
			
			int setCapacity(int cap);
			
			int setTimeout(int tmout);
#if 1
			
			int setTag(const char *); 
			
			int removeTag(const char *);
			
			bool hasTag(const char *);
			
			int getTagSize();
			
			const char * getTag(int i);
#endif
			
			bool isHealthy();
			
			int setHealthy(bool );

			
			int disable();
			
			int enable();
			
			bool isEnable();

			
			int setID(int id);
			
			int getID();

			
			int setManager(ConnectManager *);

			
			
			
			
	};

}














#endif  


