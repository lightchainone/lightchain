


#ifndef  __LCCLIENT_CONNECTSERVER_H_
#define  __LCCLIENT_CONNECTSERVER_H_

#include <vector>

#include <bsl/containers/string/bsl_string.h>
#include <connection.h>
#include <bsl/var/implement.h>
#include <bsl/ResourcePool.h>
#include <socketpool.h>
#include <Configure.h>
#include <bsl/var/IVar.h>

#include "lcclient_struct.h"
#include "lcclient_connection.h"

namespace lc
{
	namespace {
		typedef std::list<lcclient_talkwith_t*> list_std;
		typedef std::vector<char *> str_vec;
	}

	class LcClientConnectManager;

	
	class LcClientServer : plclic comcm::Server {
		plclic:
			
			LcClientServer();
			
			
			~LcClientServer();

			
			int init(const comcfg::ConfigUnit & conf);

			
			int fetchConnectionAsync(lcclient_talkwith_t *basetalk, lc::IEvent * event, 
									 lc::NetReactor *reactor);

			
			int freeConnectionAsync(LcClientConnection *conn, int tag = 0);

			
			bool equal(const comcm::Server & other);          

			
			int clone(const comcm::Server * oth );  

			
			int connectAll();

			
			LcClientConnectManager *getManager();

			
			int getIPCount();

			
			const char * getIP(int num=0);

			
			int getReadTimeout();

			
			int setReadTimeout(int readTimeout);

			
			int getWriteTimeout();

			
			int setWriteTimeout(int writeTimeout);

			
			int getReserve1() {return _conf_info.reserve1;}
			
			
			void setReserve1(int reserve1) {_conf_info.reserve1 = reserve1;}
		
			
			int getReserve2() {return _conf_info.reserve2;}
	
			
			void setReserve2(int reserve2) {_conf_info.reserve2 = reserve2;}
			
			
			int getReserve3() {return _conf_info.reserve3;}			
			
			void setReserve3(int reserve3) {_conf_info.reserve3 = reserve3;}

			
			long long getReserve4() {return _conf_info.reserve4;}

			
			void setReserve4(long long reserve4) {_conf_info.reserve4 = reserve4;}
			
			int setServerArg(const server_conf_info_t &serverArg);

			
			int getServerArg(server_conf_info_t & servConf);

			
			const char *getServerConfAll();

			
			int isLinger(){return _linger;}

			
			const char *getRange();

			
			const char *getVersion();

			
			int getMaster();

			
			bsl::var::IVar & getConf(bsl::ResourcePool *rp, int *errNo);

			static void common_error_callback(IEvent *ev, void *talk);

            bool isFull()
            {
                comcm::AutoRead autoread(&_svrlock);
                if (_liveConn >= _capacity && _pendingLongConn <= 0) {
                    return true;
                } else {
                    return false;
                }
            }

            int getPoolUsage();

            int getPoolUsingNum();

            int getPoolMaxNum();
		protected:
			
			int copySaveBuffer(const char* resBuf, int len);

			
			static void lc_server_callback(lc::IEvent *e, void *param);

			
			static void lc_server_connect_callback(lc::IEvent *e, void *param);
		
			
			void clearIpVec();

			
			static void postConn(lcclient_nshead_atalk_t *currTalk, LcClientServer *server,
								 lc::SockEvent *sockEvent);
            
            int postEvent(IEvent *event, NetReactor *reactor, lcclient_talkwith_t *basetalk,
                    LcClientConnection *conn);

            void notify_strategy(LcClientConnection *conn);

		protected:
			str_vec _ip_vec;		  	  

			server_conf_info_t _conf_info;            
			int _master;                              
			char *_confSave;                          
			int _oldConfSaveLen;		  			  
			int _confSaveLen;                         
			int _retryTime;                           
			int _linger;                               
		
			bsl::string _range;                       
			bsl::string _version;                     

			list_std _waitingTalkList;                
			int _maxWaitingNum;                       
	};
}


#endif  


