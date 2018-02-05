
#ifndef  __LCCLIENTMANAGER_H_
#define  __LCCLIENTMANAGER_H_

#include <pthread.h>

#include <bsl/containers/string/bsl_string.h>
#include <lc/lc2.h>
#include <Configure.h>
#include <bsl/map.h>
#include <vector>

#include "LcMonitor.h"

#include "lcclient_struct.h"
#include "lcclient_connectpool.h"
#include "lcclient_connectmanager.h"
#include "lcclient_utils.h"
#include "lcclient_define.h"
#include "lcclient_http.h"

#include "lc_galileo/galileo_cluster.h"
#include "lc_galileo/galileo_manager.h"

namespace lc {

    void lc_client_convert_ip(char *ip);

    bsl::var::IVar &lc_client_get_pool_usage(bsl::var::IVar &query, bsl::ResourcePool &rp);

    

    class LcClientEventPool;


    
    class LcClientManager {
        typedef bsl::phashmap<long, bsl::string> conn_sev_hashmap;
        typedef std::multimap<bsl::string, bsl::string> path_sev_map;
        
        
        typedef struct _galileo_regres_t {
            
            _galileo_regres_t(int data_buff_size){
                res_addr[0] = 0;
                res_data = new char[data_buff_size];
                res_tag = -1;
            }

            
            ~_galileo_regres_t(){
                if (NULL != res_data){
                    delete []res_data;
                }
            }

            char res_addr[LCCLIENT_PATH_LEN]; 
            char *res_data; 
            int res_tag;    
        }galileo_regres_t;

        typedef bsl::list<galileo_regres_t*> regres_list;
        
        typedef struct _service_info {
            char serviceName[LCCLIENT_SERVERNAME_LEN];        
            char proxyName[LCCLIENT_SERVERNAME_LEN];          
            char simpleName[LCCLIENT_HOSTNAME_STR_LEN];       
            LcClientPool *pool;                               
            int pos;
        } service_info;
      
        typedef std::vector<service_info*> serv_vec;
        plclic:

        typedef struct _cluster_wrap
        {
            GalileoCluster  *cluster;
            LcClientManager *lccm;
            
        } cluster_wrap_t;

            
            int setMonitor(LcMonitor *monitor, int threadnum) {
                if (!monitor || threadnum < 0) {
                    return -1;
                }
                _monitor = monitor;
                _monitor_threadnum = threadnum;
                return 0;
            }

            
            LcMonitor *getMonitor() {
                return _monitor;
            }

            
            
            
            
            LcClientManager(lc::NetReactor *reactor=NULL);
            
            
            virtual ~LcClientManager();   
            
            
            virtual int init(const char *dir=LCCLIENT_DEFAULT_CONFDIR , 
                     const char *file=LCCLIENT_DEFAULT_CONFFILE);
            
            
            virtual int load(const comcfg::ConfigUnit &conf);

            
            int setReactor(lc::NetReactor *reactor);
           
            
            lc::NetReactor *getReactor(){return _netreactor;}

            
            virtual int reload(const comcfg::ConfigUnit & conf);

            
            virtual int resourceReload(const comcfg::ConfigUnit & conf);
            
                       
            virtual int close();

            
            int setConnPool(const char *servname, LcClientPool *pool);


            
            
            
            
            virtual int nshead_talk(const char *servicename, nshead_talkwith_t *talkmsg_vec[],
                            int count, bsl::var::IVar & confVar = bsl::var::Null::null);
            
            
            virtual int nshead_nonblock_talk(const char *servicename,
                                     nshead_talkwith_t *talkmsg_vec[], int count,
                                     bsl::var::IVar & confVar = bsl::var::Null::null,
                                     LcClientEventPool *pool=NULL);
            
            
            virtual int nshead_singletalk(const char *servicename, nshead_talkwith_t *talkmsg, 
                                  bsl::var::IVar &confVar = bsl::var::Null::null);

            
            virtual int nshead_simple_singletalk(const char *servicename, int hash_key,
                                         nshead_t *reqhead, const void *reqbuff, 
                                         nshead_t *reshead, void *resbuff,
                                         u_int maxreslen);
            
            
            virtual int nshead_nonblock_singletalk(const char *service, nshead_talkwith_t *talkmsg, 
                                           bsl::var::IVar & confVar = bsl::var::Null::null,
                                           LcClientEventPool *pool=NULL);
            
            
            virtual int wait(nshead_talkwith_t **return_talkmsg_vec, int num, int ms_timeout=0, 
                     LcClientEventPool *pool=NULL);
            
            
            virtual int wait(nshead_talkwith_t *req_talkmsg, int ms_timeout=0,
                     LcClientEventPool *pool=NULL);
            
            
            virtual int wait(int ms_timeout=0, LcClientEventPool *pool=NULL);
            
            
            virtual int waitAll(LcClientEventPool *pool=NULL);
            
            
            virtual nshead_talkwith_t *fetchFirstReadyReq(LcClientEventPool *pool=NULL);
            
            
            virtual int isNsheadTalkInProcessing(nshead_talkwith_t *talk, LcClientEventPool *pool=NULL);

            
            virtual int cancelAll(LcClientEventPool *pool=NULL);
            
            
            virtual int nshead_async_talk(const char *servicename, nshead_talkwith_t *talkmsg, 
                                  lc::IEvent * event,
                                  bsl::var::IVar &confVar = bsl::var::Null::null);


            
            virtual int common_async_talk(const char *servicename, LcEvent *event,
                                  bsl::var::IVar &confVar = bsl::var::Null::null);

            
            virtual int common_singletalk(const char *servicename, LcEvent *event, 
                                bsl::var::IVar &confVar = bsl::var::Null::null);


            
            virtual int common_nonblock_singletalk(const char *servicename, LcEvent *event, 
                                         bsl::var::IVar &confVar = bsl::var::Null::null,
                                         LcClientEventPool *pool=NULL);
            
            
            virtual int common_talk(const char *servicename, LcEvent *event_vec[],
                            int count, bsl::var::IVar & confVar = bsl::var::Null::null);

            
            virtual int common_nonblock_talk(const char *servicename,
                                   LcEvent *event_vec[], int count,
                                   bsl::var::IVar & confVar = bsl::var::Null::null,
                                   LcClientEventPool *pool=NULL);
            
            
            virtual int common_singletalk_ex(LcEvent *event);
            
            
            virtual int common_nonblock_singletalk_ex(LcEvent *event, LcClientEventPool *pool=NULL);

            
            virtual int common_async_talk_ex(LcEvent *event);

            
            virtual int common_nonblock_talk_ex(LcEvent *event_vec[], int num, LcClientEventPool *pool=NULL);
            
            virtual int wait(LcEvent *req_event, int ms_timeout=0,
                     LcClientEventPool *pool=NULL);

            
            virtual LcEvent *fetchFirstReadyReqForCommon(LcClientEventPool *pool=NULL);

            
            virtual int isCommonTalkInProcessing(LcEvent *event, LcClientEventPool *pool=NULL);

            
            virtual int isTalkInProcessing(lcclient_talkwith_t *talk, LcClientEventPool *pool);

            
            
            
            
            virtual LcClientConnection *fetchConnection(const char *servicename, int *errNo,
                                                default_server_arg_t *serverArg = NULL, 
                                                bsl::var::IVar & info = bsl::var::Null::null);
            
            
            virtual int freeConnection(LcClientConnection* conn);
            
            
            
            
            
            virtual int common_singletalk_ip(LcEvent *event, char *ip, int port, unsigned int ms_timeout);

            
            virtual int common_nonblock_singletalk_ip(LcEvent *event, char *ip, int port, unsigned int ms_timeout, LcClientEventPool *pool=NULL);

            
            virtual int common_async_talk_ip(LcEvent *event, char *ip, int port, unsigned int ms_timeout);
            
            
            
            
            virtual int startHealthyCheck(u_int msSleepTime = 5000);
            
            
            virtual int stopHealthyCheck();
            
            
            virtual int healthyCheckOnce();


            
            
            
            
            virtual int readLock(const char *servicename);

            
            virtual int unlock(const char *servicename);

            
            virtual int getReqAndResBufLen(const char *servicename, int &reqBufLen, int &resBufLen);

            
            virtual int getReqBufLen(const char *servicename);

            
            virtual int getResBufLen(const char *servicename);

            
            virtual int getServiceCount();
            
            
            virtual LcClientPool *getServiceByID(int id);

            
            virtual const LcClientPool *getServiceByName(const char *servicename);

            
            virtual bsl::var::IVar & getConf();

            
            virtual bsl::var::IVar & getServiceConf(const char *servicename, bsl::ResourcePool *rp,
                                            int *errNo);
        
            
            virtual int insertRegisterResource(const char *path, const char *res_data);

            
            virtual int delRegisterResource(const char *path, const int res_tag);

            
            virtual bsl::var::IVar &insertDependingResource(const char *name, const char *addr, bsl::ResourcePool *rp);

            
            virtual int delDependingResource(const char *name, const char *addr);

            
            virtual int getServiceConnType(const char *servicename, bool &conntype);

            
            virtual int getServiceRetryTime(const char *servicename);
 
            
            virtual int getServiceServerNum(const char *servicename);

            
            virtual int getServiceRealServerNum(const char *servicename);

            
            int registerSingleCallbackInfo(const char *name);

            
            int gen_monitor_info_note();

            
            int gen_monitor_service_info(bsl::var::IVar &service_note, LcClientConnectManager *mgr,
                                      const char *item, const char *note);

            
            int gen_monitor_service_info_ex(bsl::var::IVar &service_note,
                                            LcClientConnectManager *mgr,
                                            const char *item, const char *note);

            
            bsl::var::IVar & get_note_var();

            int add_monitor_info_ipport(const char *monitor_item, const char *ip, int port, const
char *service_name);
            
            int add_monitor_info_ipport(const char *monitor_item, int fd, const char *service_name);

            
            int add_monitor_info_ipport_ex(const char *monitor_item, int fd, const char *service_name, int num);

            
            int registerSingleMonitorInfo(const char *name, int tag = 0);

            
            int registerMonitorInfo();
			
            
            virtual int reload_by_galileo_event(bsl::var::IVar & conf, const char *path);


        protected:
            
            virtual int init(const comcfg::ConfigUnit & conf);

            
            virtual int initAfterGalileo(const comcfg::ConfigUnit & conf);

            
            virtual int initResource();

            
            virtual int innerLoad(const comcfg::ConfigUnit &conf);

            
            virtual int init(bsl::var::IVar &conf, const char *servname, bsl::ResourcePool &rp, 
                     bsl::var::IVar &galileoConfVar, const comcfg::ConfigUnit &local_conf, int tag =
0, const char *proxyname = NULL,
                     const char *sname=NULL);
            
            
            virtual int init_reload(bsl::var::IVar &conf, const char *servname, bsl::ResourcePool &rp, 
                    bsl::var::IVar &galileoConfVar, const char *proxyname = NULL,
                    const char *sname=NULL);


            
            static void lc_client_callback(lc::IEvent *e, void *param);

            
            static int lc_client_common_callback(void *talk);
            
            
            static int lc_client_common_callback_ex(void *talk);
            
            static void *start_healthy_check(void *param);

            static void lc_client_callback_ip(lc::IEvent *e, void *param);

            
            virtual int stop_healthy_check();

            
            virtual int healthyCheck();
            
            
            virtual LcClientServer *fetchServer(const char *servicename, int serverID);

            
            virtual bsl::var::IVar & convertMcpackToVar(bsl::var::IVar &conf, bsl::ResourcePool & rp, 
                                                const char *servname, int &errNo);
            
            
            virtual int dump_galileo_conf(const char *txt);

            
            virtual LcClientEventPool *getLocalEventPool(LcClientEventPool *pool=NULL);

            
            virtual LcClientEventPool *getLocalEventPool1(LcClientEventPool *pool=NULL);
        
            
            virtual int postSingleTalk(nshead_talkwith_t* talkmsg, bsl::var::IVar & confVar,
                               LcClientPool *currPool, LcClientEventPool *currEventPool,
                               const char* servicename);

            
            virtual int saveGalileoCopy(bsl::var::IVar & currArary);

            
            virtual LcClientPool *getManagerPool(const char *servicename);

            
            virtual LcClientPool *getManagerAllPool(const char *servicename);

            
            virtual int exitThreadUsingInsidePool();
            
            
            virtual void deleteServiceMap();
            
            
            virtual void deleteConnMap();
            
            
            virtual void deleteRegRes();

            
            
            virtual int addService(const char *serviceName, LcClientPool *pool, const char *proxyname,
                            const char *sname, int tag = 0);
            
            virtual int findService(const char* serviceName);
            
            
            virtual int getService(const char *serviceName, LcClientPool **pool);
            
            
            virtual const char *getProxyName(const char *serviceName);

            
            virtual const char *getSimpleName(const char *serviceName);
           
             
            virtual void clearService();

            
            virtual int getFD(nshead_talkwith_t* talkmsg, bsl::var::IVar & confVar,
                      LcClientPool *currPool, const char* servicename);

            
            virtual int postSingleTalkBlock(nshead_talkwith_t* talkmsg, bsl::var::IVar & confVar,
                                    LcClientPool *currPool, const char* servicename);

            void merge_galileo_local_conf(const comcfg::ConfigUnit &local_conf, const char *type, bsl::ResourcePool &rp, bsl::var::IVar &resultVar);

            int merge_galileo_local_conf2(const comcfg::ConfigUnit &local_conf, const char *type, bsl::ResourcePool &rp, bsl::var::IVar &resultVar);


			
		private:
            
            int _init_with_localconf(const comcfg::ConfigUnit & conf);
            int _init_with_localconf_ext(const comcfg::ConfigUnit & conf);

            
			int _init_with_galileo(const comcfg::ConfigUnit &conf, int conftype);
            

			int _process_request_resource(const comcfg::ConfigUnit&, const comcfg::ConfigUnit&, int, bsl::ResourcePool&, bsl::var::IVar&);
			int _process_register_resource(const comcfg::ConfigUnit& );		
			
        protected:
            lc::NetReactor *_netreactor;          
            int _isOut;                           

            pthread_key_t _poolkey;               
            int _createTag;                       
            comcm::RWLock _service_rwlock;        
            
            serv_vec _service_map;            

            
            conn_sev_hashmap _conn_map;

            int _confType;                        

            
            MLock _depres_lock;                               
            path_sev_map _galileo_servmap;
            char _galileoDir[LCCLIENT_DIR_LEN];               
            char _galileoFile[LCCLIENT_FILE_LEN];             

            bsl::var::Dict _main_conf;           
            bsl::ResourcePool _rpool;            

            int _galileoRun;                     
            char *_galileoConfSave;              
            int _galileoConfSaveLen;             

            pthread_t _healthy_check_t;          
            int _healthy_run;                    
            u_int _healthyCheckTime;             

            int _galileoBufferSize;              
 
            MLock _regres_lock;                  
            regres_list _reg_resource_list;      

            LcMonitor *_monitor;
            bsl::ResourcePool _monitor_rp;
            bsl::var::Ref _monitor_info;
            int _monitor_threadnum;


			bool                          m_new_version;
			std::vector<cluster_wrap_t *> m_cluster_vector;

    };

}

#endif  


