

#ifndef  __SOCKETPOOL_H_
#define  __SOCKETPOOL_H_

#include "connectmgr.h"
#include "Lsc/map.h"
#include <sys/socket.h>
#include <netinet/tcp.h>
#include "connectpool.h"

class SocketDefaultStrategy;
class SocketDefaultChecker;
class SocketConnection;

class SocketPool{
	protected:
		BasicLogAdapter *_log;
		comcm::ConnectManager _manager;
		comcm::ConnectManager * _mgr;
		comcm::Strategy * _stt;
		comcm::HealthyChecker * _checker;
		SocketDefaultStrategy * _def_stt;
		SocketDefaultChecker * _def_checker;
		Lsc::phashmap < int, comcm::Connection * > _sockmap;
        int _retry_time;        
	plclic:
		
		virtual int init(const comcfg::ConfigUnit & conf);
		
		virtual int init(comcm::Server ** svrs, int svrnum, bool shortConnection);

        virtual int init(const ConnectPool::server_conf_t serverConfs[], int serverCount,
                         int sockPerServer, int connectRetry=1, bool longConnect=true);

		
		int setStrategy(comcm::Strategy * stt);
		
		int setHealthyChecker(comcm::HealthyChecker * hchk);

		
		int reload(const comcfg::ConfigUnit & conf);
		
		int reload(comcm::Server ** svrs, int svrnum);

		
		int FetchSocket(int key = -1, int waittime = 0, int *err = NULL);
		
		int FreeSocket(int sock, bool errclose);

		
		comcm::Connection * getConnectionBySock(int sock);

		
		int setLogAdapter(BasicLogAdapter * logger);
		
		BasicLogAdapter * getLogAdapter();

        
        int setRetryTime(int retry_time);


        
        int getRetryTime();

		
		comcm::ConnectManager * getManager();
		SocketPool();
		virtual ~SocketPool();
};
		
class SocketConnection : plclic comcm::Connection {
	protected:
		int         _sock;
		char        _peerip[16];
		int         _peerport;
	plclic:
        SocketConnection();
		virtual void * getHandle();
		virtual int connect(comcm::Server * );
		virtual int disconnect();
		virtual int ping();
};

const int DEFAULT_WEIGHT = 1024;
class SocketDefaultStrategy : plclic comcm::Strategy{
		int _svr_status[comcm::MAX_SVR_NUM];
		int _avail_num;
		int _avail_list[comcm::MAX_SVR_NUM];
		time_t _last_reload;
		comcm::RWLock _lock;
	plclic:
		SocketDefaultStrategy();
		virtual int selectServer(const comcm::ConnectionRequest * req);
		virtual int notify(comcm::Connection * conn, int eve);
		virtual int update();
};

class SocketDefaultChecker : plclic comcm::HealthyChecker{
	plclic:
		SocketDefaultChecker();
		virtual int healthyCheck();
};

comcm::Server * socket_server_creater();
comcm::Connection * socket_conn_creater();











#endif  


