
#ifndef  __RSRCPOOL_H_
#define  __RSRCPOOL_H_

#include <sys/socket.h>
#include <netinet/tcp.h>
#include <time.h>

#include <ul_log.h>
#include <Lsc/map.h>
#include <Lsc/containers/list/Lsc_list.h>
#include <Lsc/var/CompackSerializer.h>
#include <Lsc/var/implement.h>
#include <Lsc/ResourcePool.h>
#include <Configure.h>
#include <connectmgr.h>
#include <socketpool.h>
#include "lleo.h"


const int MAX_RESOURCE_NUM         = 1024;
const int MAX_IP_STR_LEN           = 32;
const int MAX_HOSTNAME_STR_LEN     = 128;
const int RESOURCE_UPDATE_INTERVAL = 10;
const int GALILEO_INIT_TIMELIMIT   = 5;    
const int RSRCPOOL_MAGIC_NUM       = 0xfb726031;
const int MAX_GALILEO_CLUSTER_NUM  = 10;
const int GALILEO_DEFAULT_DELAY_WATCH_TIME  = 10;


typedef struct _req_resource_t{
    _req_resource_t(int data_buff_size){
        res_count    = 0;
        res_capacity = 0;
        res_name[0]  = 0;
        res_cluster[0]  = 0;
        res_addr[0]  = 0;
        res_data = new char[data_buff_size]; 
        res_data_len = 0; 

        res_rtimeout = GALILEO_DEFAULT_RTIMEOUT;
        res_wtimeout = GALILEO_DEFAULT_WTIMEOUT;
        res_lastupdateat = 0;

        res_sockpool = NULL;
        init_from_conf = 0;
    }

    ~_req_resource_t(){
        if (NULL != res_data){
            delete [] res_data;
        }
        if (NULL != res_sockpool){
            delete res_sockpool;
        }
    }

    size_t  res_count;    
    int  res_capacity;    
    char res_name[GALILEO_DEFAULT_PATH_LEN]; 
    char res_addr[GALILEO_DEFAULT_PATH_LEN]; 
    char res_cluster[GALILEO_DEFAULT_PATH_LEN]; 
    char *res_data;       
    int res_data_len;     
    int res_ctimeout;     
    int res_rtimeout;     
    int res_wtimeout;     
    time_t res_lastupdateat; 
    int init_from_conf;     

    SocketPool *res_sockpool;
    pthread_rwlock_t res_rwlock; 
}req_resource_t;



typedef struct _reg_resource_t{
    _reg_resource_t(int data_buff_size){
        res_addr[0] = 0;
        res_cluster[0] = 0;
        res_data    = new char[data_buff_size];
        res_tag     = -1;
    }
    ~_reg_resource_t(){
        if (NULL != res_data){
            delete [] res_data;
        }
    }

    char res_addr[GALILEO_DEFAULT_PATH_LEN]; 
    char res_cluster[GALILEO_DEFAULT_PATH_LEN]; 
    char *res_data; 
    int res_tag;    
}reg_resource_t;


class RsrcPool{
    plclic:
        
        static RsrcPool* get_instance();

        static RsrcPool* m_instance; 

        
        ~RsrcPool();

    protected:
        
        RsrcPool();

        
        static int set_thread_log(const char *thread_info);

        
        static void close_thread_log();

        
        int check_config();

        
        int request_resource_init();

        
        int parse_raw_address(char* raw_address, char** cluster, char** path);

        
        static int get_handle_by_name(const char *name);

        
        int request_resource_init_from_conf();

        
        int register_resource_init(int *reg_tags,const int reg_tags_cnt);

        

        static int update_resource_pool(req_resource_t *res_item, Lsc::var::IVar& res_data);

        
        static int dump_respack(const char *dir, const char *name, const char *json_txt);

        
        static int load_respack(const char *dir, const char *name, char *json_txt);

        
        static int get_service_by_name(Lsc::var::IVar& services, const char *name,
                                Lsc::var::IVar& service);

        
        static int update_socket_pool(req_resource_t *res_item, Lsc::var::IVar& service,
                               Lsc::var::IVar& enti_var);

        
        static int set_server_info(comcm::Server *svr, const char *ip, const int port,
                            const int ctimeout, const int capacity);

        
        static void* update_request_resource(void* argv);

        
        static int resource_watcher(int watch_type, const char *realpath,
                                    Lsc::var::IVar& res_data, void *ctx);

        
        static int has_range_file_for_resource(const char* res_name, const char* dir,
                char *range_file);

        
        static int incr_refer_count();

        
        static int decr_refer_count();

        unsigned int      m_magic_num;          
        static int        m_gali_num;           
        
        static lleo_t  *m_gali_hdlr[MAX_GALILEO_CLUSTER_NUM];
        comcfg::Configure m_gali_conf;          
        
        static const char *m_gali_hosts[MAX_GALILEO_CLUSTER_NUM];
        
        static int        m_gali_delay_watch_time[MAX_GALILEO_CLUSTER_NUM];
        static int        m_init_from_conf[MAX_GALILEO_CLUSTER_NUM];
        struct ACL_vector m_acl_vec[MAX_GALILEO_CLUSTER_NUM];

        static const char *m_gali_conf_dir;     
        static const char *m_gali_zoo_log ;     
        static int        m_gali_buff_size;     
        static int        m_gali_sesn_time;     
        
        static int        m_empt_req_flg;       
        static int        m_is_running;         
        static pthread_mutex_t m_run_mutex;     

        static int        m_req_res_num;        
        static int        m_reg_res_num;        

        static pthread_mutex_t m_ref_mutex;     
        static pthread_cond_t  m_ref_cond;      
        static int             m_ref_count;     

        static Lsc::list<req_resource_t*>* m_req_res_list;
        static Lsc::list<req_resource_t*>* m_req_res_lleo_list;
        static Lsc::list<req_resource_t*>* m_req_res_conf_list;
        
        static pthread_rwlock_t m_req_res_rwlock;

        static Lsc::list<reg_resource_t*>* m_reg_res_list; 
        static pthread_mutex_t m_reg_res_mutex; 

        myhashtable         *m_req_table_by_name; 

        pthread_t         m_update_thread;
        void              *m_thread_result;

    plclic:
        
        int init(const char *conf_dir = "./conf/", const char *conf_file = "lleo.conf",
                const int empt_req_flg = 0, int *reg_tags = NULL, const int reg_tags_cnt = 0);

        
        int destroy_registed_resource();


        
        int get_timeout(const char* res_name, int *rtimeout, int *wtimeout);

        
        int fetch_socket(const char* res_name, int key = -1, int waittime = 0);

        
        int free_socket(const char* res_name, int sock, bool errclose);

        
        int add_request_resource(const char* res_name,
                const char* res_addr, const int capacity);

        
        int add_register_resource(const char* res_addr, const char* data_pack);

        
        int remove_request_resource(const char* res_name, const char* res_addr);

        
        int remove_register_resource(const char *res_addr, const int res_tag);

         
        int get_request_resource_data_json(const char* res_name, char* data_buff, int buff_len);

         
        int get_request_resource_data(const char* res_name, Lsc::var::IVar& data_var,
                Lsc::ResourcePool& rp);

        
        int get_request_resource_data_buff(const char* res_name,
                char* data_buff, int buff_len, int mcpack_version = 2);

        
        void show_request_resource();

        
        void show_register_resource();
};


#endif

