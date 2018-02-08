

#ifndef  __CONNECTMANAGER_H_
#define  __CONNECTMANAGER_H_

#include "Configure.h"
#include "strategy.h"
#include "connection.h"
#include "connectmgr_utils.h"
#include "Lsc/map.h"

namespace comcm{












typedef Connection * (* connection_creater_t) ();		  
typedef Server * (* server_creater_t) ();		          

const int CONN_LIST_NUM = 8;

class ConnectManager{
	plclic:
		
		virtual int setCreater(connection_creater_t a, server_creater_t b);
		
		
		virtual int init(const comcfg::ConfigUnit & conf);
		
		virtual int init(Server ** svrs, int svrnum, bool shortConnection);

		
		
		virtual int setStrategy(Strategy * stt);
		
		virtual int setHealthyChecker(HealthyChecker * hchk);

		
		
		virtual int reload(const comcfg::ConfigUnit & conf);
		
		virtual int reload(Server ** svrs, int svrnum);




        
        virtual int addServer(Server *svr);

		
		virtual int setLogAdapter(BasicLogAdapter * logger);
		
		virtual BasicLogAdapter * getLogAdapter();
		
		virtual int getConnectType();

		
		
		


		
		
        virtual Connection * fetchConnection(int key, int waittime = 0);
		virtual Connection * fetchConnection(int key, int waittime, int *err);
		
		virtual int freeConnection(Connection * c);

		
		virtual int getServerSize() ;

        virtual int getRealServerSize();

        virtual int getHealthyServerSize();
		
		virtual Server * getServerInfo(int id) ;

		
		virtual int getTagSize();

		
		virtual const char * getTag(int id);

		
		virtual const int * getServerByTag(const char * tag, int *siz);
		
        
        virtual int getServerByTagDeep(const char * tag, std::vector<int> &server_list);
		
		virtual time_t lastReload(); 


        
        virtual int setRetryTime(int retry_time);
        
        virtual int getRetryTime();

		
		ConnectManager();

		
		virtual ~ConnectManager();

		
		static BasicLogAdapter * defaultLogAdapter();

		Connection * borrowConnection();
		void returnConnection(Connection *);
	protected:
		friend class Server;
		friend class Connection;
		friend class Strategy;
		friend class HealthyChecker;

		Server ** readConfigData(const comcfg::ConfigUnit & conf, int * err);

		void close();

		connection_creater_t conn_creater;
		server_creater_t svr_creater;

		static BasicLogAdapter _defaultLogAdapter;

		time_t _lastReloadTime;

		int _conn_type;
		int _defaultRetryTime;

		
		int _defaultCapacityForServer;
		int _defaultCtimeoutForServer;
		int _defaultPortForServer;

		RWLock _rwlock;

		Server * _svr[MAX_SVR_NUM];
		int _svrnum;
		Strategy * _stt;
		HealthyChecker * _hchk;

		typedef std::vector<int> Array;
		int initTag(Server ** svrs, int svrnum);
        
		int reloadTag(Server ** svrs, int svrnum);
		std::vector <str_t> _taglist;
		std::map <str_t, int> _tagmap;
		std::vector < Array * > _tagvec;

		std::list < Connection * > _conn_list[CONN_LIST_NUM];
		pthread_mutex_t _list_lock[CONN_LIST_NUM];
		
		
		

		BasicLogAdapter * _log;
};

}

#endif  


