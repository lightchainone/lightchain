


#ifndef  __LCCLIENT_CONNECTPOOL_H_
#define  __LCCLIENT_CONNECTPOOL_H_

#include <sys/socket.h>
#include <netinet/tcp.h>

#include <connection.h>
#include <connectmgr.h>
#include <Configure.h>
#include <bsl/map.h>
#include <bsl/var/IVar.h>

#include "lcclient_connection.h"
#include "lcclient_connectmanager.h"

namespace lc
{
    class LcClientManager;

	
	class LcClientPool{
		plclic:
			
			LcClientPool();
			
			
			virtual ~LcClientPool();
			
			
			virtual int init(const comcfg::ConfigUnit & conf);

			
			

			
			virtual int setStrategy(LcClientStrategy *stt); 

			
			virtual int setHealthyChecker(LcClientHealthyChecker *hchk);  

			
			virtual int reload(const comcfg::ConfigUnit & conf);

			
			

			
			virtual LcClientConnection *fetchConnection(const default_server_arg_t &serverArg,
												bsl::var::IVar & info, int *errNo, LcClientManager *mgr);

			
			int freeConnection(LcClientConnection* conn);

			
			virtual int fetchConnectionAsync(lcclient_talkwith_t *talkmsg, bsl::var::IVar &confVar, 
						lc::IEvent * event, lc::NetReactor *reactor);

			
			int getReqAndResBufLen(int &reqBufLen, int &resBufLen);

			
			int getReqBufLen();

			
			int getResBufLen();

			
			LcClientConnectManager * getManager();

			

			
			int getServerCount();

            int getRealServerCount();

			
			LcClientServer* getServerByID(int id);

			
			int setReloadTag(int tag);

			
			int getReloadTag();

			
			int setConnType(int connType);

			
			bsl::var::IVar & getConf(bsl::ResourcePool *rp, int *errNo);

			
			int isEnable();

			
			int setEnable(int enable);

			
			int healthyCheck();

			
			void readLock(){_mgr->readLock();}

			
			void unlock(){_mgr->unlock();}

			
			int setServerArg(LcClientServer *server, const nshead_talkwith_t *talk);

			
			int setServerArgAfterConn(LcClientServer *server, int errNo = -1);

			
			bool getConnectType();

            bsl::var::IVar &getPoolUsage(bsl::ResourcePool &rp);

            bsl::var::IVar &getPoolUsingNum(bsl::ResourcePool &rp);

            bsl::var::IVar &getPoolMaxNum(bsl::ResourcePool &rp);

		protected:
			
			int saveConf(const comcfg::ConfigUnit & conf);

		protected:
			LcClientConnectManager _manager;          
			LcClientConnectManager *_mgr;             
			LcClientStrategy *_stt;                   
			LcClientHealthyChecker *_checker;         
			bsl::string _sttName;                     
			bsl::string _checkerName;                 

			LcClientStrategy *_def_stt;               
			LcClientHealthyChecker *_def_checker;     


			comcm::RWLock _poollock;                 
			int _enable;                             

			
			int _reloadTag;                          

			char *_confSave;                         
			int _oldConfSaveLen;		             
			int _confSaveLen;                        
	};

	comcm::Server * lcclient_server_creater();                
	comcm::Connection * lcclient_connection_creater();        
}
















#endif  


