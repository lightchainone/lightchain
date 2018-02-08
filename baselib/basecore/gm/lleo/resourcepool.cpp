
#include "resourcepool.h"

class ErrorRsrcControl {
    plclic:
        ErrorRsrcControl() {
            RsrcPool :: get_instance();
        }
        ~ErrorRsrcControl() {
            RsrcPool *er = RsrcPool :: get_instance();
            sleep(1);
            delete er;
        }
};

static ErrorRsrcControl err_rsrc_control;


RsrcPool* RsrcPool::m_instance        = NULL;
const char* RsrcPool::m_gali_conf_dir = NULL;
const char* RsrcPool::m_gali_zoo_log  = NULL;
int RsrcPool::m_gali_buff_size        = 0;
int RsrcPool::m_gali_sesn_time        = 0;
int RsrcPool::m_empt_req_flg          = 0;
int RsrcPool::m_is_running            = 0;
int RsrcPool::m_gali_num              = 0;
int RsrcPool::m_init_from_conf[MAX_GALILEO_CLUSTER_NUM] = {'\0'};
int RsrcPool::m_gali_delay_watch_time[MAX_GALILEO_CLUSTER_NUM] = {'\0'};
lleo_t *RsrcPool::m_gali_hdlr[MAX_GALILEO_CLUSTER_NUM] = {'\0'};
const char *RsrcPool::m_gali_hosts[MAX_GALILEO_CLUSTER_NUM] = {'\0'};
pthread_mutex_t RsrcPool::m_run_mutex;
int RsrcPool::m_req_res_num           = 0;
int RsrcPool::m_reg_res_num           = 0;

pthread_mutex_t RsrcPool::m_ref_mutex;
pthread_cond_t  RsrcPool::m_ref_cond;
int             RsrcPool::m_ref_count = 0;

Lsc::list<req_resource_t*>* RsrcPool::m_req_res_list = NULL;
Lsc::list<req_resource_t*>* RsrcPool::m_req_res_lleo_list = NULL;
Lsc::list<req_resource_t*>* RsrcPool::m_req_res_conf_list = NULL;
pthread_rwlock_t RsrcPool::m_req_res_rwlock;
Lsc::list<reg_resource_t*>* RsrcPool::m_reg_res_list = NULL;
pthread_mutex_t RsrcPool::m_reg_res_mutex;


RsrcPool* RsrcPool :: get_instance()
{
    if (NULL == m_instance){
        m_instance = new RsrcPool();
    }
    return m_instance;
}


RsrcPool :: RsrcPool()
{
    m_gali_buff_size    = 0;
    m_is_running        = 0;
    m_req_res_num       = 0;
    m_reg_res_num       = 0;
    m_ref_count         = 0;
    m_req_table_by_name = NULL;

    m_req_res_list = new Lsc::list<req_resource_t*>();
    m_req_res_lleo_list = new Lsc::list<req_resource_t*>();
    m_req_res_conf_list = new Lsc::list<req_resource_t*>();
    m_reg_res_list = new Lsc::list<reg_resource_t*>();

    bzero(m_init_from_conf, MAX_GALILEO_CLUSTER_NUM*sizeof(int));
    bzero(m_gali_hosts, MAX_GALILEO_CLUSTER_NUM*sizeof(char *));
    bzero(m_gali_delay_watch_time, MAX_GALILEO_CLUSTER_NUM*sizeof(int));
    bzero(m_gali_hdlr, MAX_GALILEO_CLUSTER_NUM*sizeof(lleo_t *));
    bzero(m_acl_vec, MAX_GALILEO_CLUSTER_NUM*sizeof(struct ACL_vector));
}


RsrcPool :: ~RsrcPool()
{
    pthread_mutex_lock(&m_run_mutex);
    m_is_running = 0;
    pthread_mutex_unlock(&m_run_mutex);

    while(m_ref_count > 0){
        pthread_mutex_lock(&m_ref_mutex);
        pthread_cond_wait(&m_ref_cond, &m_ref_mutex);
        pthread_mutex_unlock(&m_ref_mutex);
    }

    destroy_registed_resource();
    int i = 0;
    while (i < m_gali_num) {
        if (m_gali_hdlr[i]) {
            lleo_destroy(m_gali_hdlr[i]);
        }

        if (m_acl_vec[i].data) {
            for (int j = 0; j < m_acl_vec[i].count; j++){
                if (!m_acl_vec[i].data[j].id.id){
                    free(m_acl_vec[i].data[j].id.id);
                    m_acl_vec[i].data[j].id.id = NULL;
                }
                if (!m_acl_vec[i].data[j].id.scheme){
                    free(m_acl_vec[i].data[j].id.scheme);
                    m_acl_vec[i].data[j].id.scheme = NULL;
                }
            }
            deallocate_ACL_vector(&m_acl_vec[i]);
        }
        ++i;
    }

    lleo_close_log();

    if (0xfb726031 == m_magic_num){
        
        int ret = pthread_join(m_update_thread, &m_thread_result);
        if (0 != ret){
            ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to join update thread, msg[%m]",
                    __FILE__, __LINE__);
        }
    }

    Lsc::list<req_resource_t*>::iterator iter1;
    for(iter1 = m_req_res_list->begin(); iter1 != m_req_res_list->end();){
        req_resource_t *req_res = *iter1;
        pthread_rwlock_destroy(&(req_res->res_rwlock));
        ++iter1;
        delete req_res;
    }
    for(iter1 = m_req_res_lleo_list->begin(); iter1 != m_req_res_lleo_list->end(); ++iter1){
        req_resource_t *req_res = *iter1;
        pthread_rwlock_destroy(&(req_res->res_rwlock));
        delete req_res;
    }
    for(iter1 = m_req_res_conf_list->begin(); iter1 != m_req_res_conf_list->end(); ++iter1){
        req_resource_t *req_res = *iter1;
        pthread_rwlock_destroy(&(req_res->res_rwlock));
        delete req_res;
    }

    Lsc::list<reg_resource_t*>::iterator iter2 = m_reg_res_list->begin();
    for(; iter2 != m_reg_res_list->end();){
        reg_resource_t *reg_res = *iter2;
        ++iter2;
        delete reg_res;
    }

    m_req_res_list->destroy();
    m_req_res_lleo_list->destroy();
    m_req_res_conf_list->destroy();
    m_reg_res_list->destroy();

    myhashtable_destroy(m_req_table_by_name, 0);
    pthread_rwlock_destroy(&m_req_res_rwlock);
    pthread_mutex_destroy(&m_reg_res_mutex);
    pthread_mutex_destroy(&m_run_mutex);
    pthread_mutex_destroy(&m_ref_mutex);
    pthread_cond_destroy(&m_ref_cond);

    delete m_req_res_list;
    delete m_req_res_lleo_list;
    delete m_req_res_conf_list;
    delete m_reg_res_list;
}


    int
RsrcPool :: set_thread_log(const char *thread_info)
{
    if(NULL == thread_info)
        return -1;

    ul_logstat_t _log_stat;
    _log_stat.events    = 4; 
    _log_stat.to_syslog = 0;
    _log_stat.spec      = 0;

    if (ul_openlog_r(thread_info, &_log_stat) < 0)
    {
        fprintf(stderr, "open log in thread failed, "
                "error[%m], file[%s] line[%d]\n", __FILE__, __LINE__);
        return -1; 
    }
    return 0;
}


    void
RsrcPool :: close_thread_log()
{
    ul_closelog_r(0);
}



int
RsrcPool::parse_raw_address(char* raw_address, char** cluster, char** path)
{
    if (NULL == raw_address || NULL == cluster || NULL == path){
        ul_writelog(UL_LOG_TRACE, "[%s:%d] fail to get valid params",
            __FILE__, __LINE__);
        return -1;
    }

    char *first = strchr(raw_address, ':');
    if (NULL == first || *(++first) == '\0'){
        ul_writelog(UL_LOG_WARNING, "[%s:%d] fail to get a valid raw_address",
            __FILE__, __LINE__);
        return -2;
    }

    char *second = strchr(first, ':');
    if (NULL == second || *(++second) == '\0'){
        ul_writelog(UL_LOG_WARNING, "[%s:%d] fail to get a valid raw_address",
            __FILE__, __LINE__);
        return -3;
    }

    char *more = strchr(second, ':');
    while ((more)) {
        second = more + 1;
        more = strchr(second, ':');
    }

    if (second == '\0') {
        ul_writelog(UL_LOG_WARNING, "[%s:%d] fail to get a valid raw_address",
            __FILE__, __LINE__);
        return -4;
    }

    *cluster = raw_address;
    *path = second;

    return 0;
}


void*
RsrcPool :: update_request_resource(void* )
{
    Lsc::ResourcePool rp;
    set_thread_log("resource_watcher");
    if(!incr_refer_count()){
        return NULL;
    }

    int rc = 0;
    char path[GALILEO_DEFAULT_PATH_LEN];
    char* json_txt = (char*)rp.create_raw(m_gali_buff_size*10);

    while(m_is_running){
        sleep(RESOURCE_UPDATE_INTERVAL);

        pthread_rwlock_rdlock(&m_req_res_rwlock);
        
        Lsc::list<req_resource_t*>::iterator iter = m_req_res_conf_list->begin();
        for(; iter != m_req_res_conf_list->end(); ++iter){
            req_resource_t *req_res = *iter;
            
            if (req_res->init_from_conf == 0) {
                continue;
            }
            snprintf(path, GALILEO_DEFAULT_PATH_LEN, "%s", m_gali_conf_dir);
            int start = strlen(path);
            if ('/' != path[start-1]){
                path[start] = '/';
                start += 1;
            }
            snprintf(path+start, GALILEO_DEFAULT_PATH_LEN-start, "%s", req_res->res_name);

            
            struct stat file_stat;
            if (0 > stat(path, &file_stat)){
                ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to get file stat for file,"
                    " name[%s]", __FILE__, __LINE__, req_res->res_name);
                continue;
            }

            if (file_stat.st_mtime > req_res->res_lastupdateat){
                
                ul_writelog(UL_LOG_WARNING, "[%s:%d] Update resource [%s] from configure"
                    " file...", __FILE__, __LINE__, req_res->res_name);

                rc = load_respack(m_gali_conf_dir, req_res->res_name, json_txt);
                if (0 != rc){
                    ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to load resource configure"
                        " from file, dir[%s], name[%s]", __FILE__, __LINE__,
                        m_gali_conf_dir, req_res->res_name);
                    continue;
                }

                try {
                    Lsc::var::JsonDeserializer jd(rp);
                    Lsc::var::IVar& json_var = jd.deserialize(json_txt);

                    rc = update_resource_pool(req_res, json_var);
                    if (0 != rc){
                        ul_writelog(UL_LOG_FATAL, "[%s:%d] Failed to update resource"
                            " name[%s], addr[%s]", __FILE__, __LINE__,
                            req_res->res_name, req_res->res_addr);
                        continue;
                    }
                }
                catch(...){
                    ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to deserialize json text, "\
                            "resource name[%s], addr[%s]", __FILE__, __LINE__,
                            req_res->res_name, req_res->res_addr);
                }
            }
            else{
                
                ul_writelog(UL_LOG_DEBUG, "Configure for resource [%s] has no change",
                        req_res->res_name);
            }
        }
        pthread_rwlock_unlock(&m_req_res_rwlock);
    }

    close_thread_log();

    decr_refer_count();
    return NULL;
}



    int
RsrcPool :: resource_watcher(int watch_type, const char *realpath,
        Lsc::var::IVar& res_data, void* gali_hdlr)
{
    int rc = 0;
    if (NULL == realpath || !res_data.is_dict()){
        ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to get a valid resource path or a"\
            " valid res_data dict", __FILE__, __LINE__);
        return -1;
    }
    ul_writelog(UL_LOG_DEBUG, "Resource Event, type[%d], addr[%s]", watch_type, realpath);

    if(!incr_refer_count()){
        return rc;
    }

    Lsc::ResourcePool rp;
    Lsc::list<req_resource_t*>::iterator iter;
    Lsc::var::Dict& query_data = rp.create<Lsc::var::Dict>();
    int is_res_found = 0;

    char parepath[GALILEO_DEFAULT_PATH_LEN];
    snprintf(parepath, GALILEO_DEFAULT_PATH_LEN, "%s", realpath);
    char *p = strrchr(parepath, '/');

    switch(watch_type){
        case GALILEO_ABSTRACT_CHANGED_EVENT:
            
            rc = lleo_query_resource((lleo_t *)gali_hdlr, realpath, query_data, rp);
            if (0 != rc){
                ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to query resource, path[%s]",
                        __FILE__, __LINE__, realpath);
                decr_refer_count();
                return -2;
            }

            pthread_rwlock_rdlock(&m_req_res_rwlock);
            
            iter = m_req_res_lleo_list->begin();
            for(; iter != m_req_res_lleo_list->end(); ++iter){
                req_resource_t *req_res = *iter;
                if (0 == strcmp(req_res->res_addr, realpath)){
                    is_res_found = 1;
                    rc = update_resource_pool(req_res, query_data);
                    if (0 != rc){
                        ul_writelog(UL_LOG_FATAL, "[%s:%d] Failed to update resource"
                            " name[%s], addr[%s]", __FILE__, __LINE__,
                            req_res->res_name, req_res->res_addr);
                    }
                    ul_writelog(UL_LOG_WARNING, "[%s:%d] Finish update for resource addr[%s]",
                        __FILE__, __LINE__, realpath);
                }
            }
            pthread_rwlock_unlock(&m_req_res_rwlock);

            if (0 == is_res_found){
                
                if (NULL != p){
                    *p = '\0';
                    rc = lleo_query_resource((lleo_t*)gali_hdlr, parepath, query_data, rp); 
                    if (0 != rc){ 
                        ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to query resource, path[%s]",
                            __FILE__, __LINE__, parepath);
                        decr_refer_count();
                        return -2;
                    }

                    pthread_rwlock_rdlock(&m_req_res_rwlock);
                    
                    iter = m_req_res_lleo_list->begin();
                    for(; iter != m_req_res_lleo_list->end(); ++iter){
                        req_resource_t *req_res = *iter;
                        if (0 == strcmp(req_res->res_addr, parepath)){
                            rc = update_resource_pool(req_res, query_data);
                            if (0 != rc){
                                ul_writelog(UL_LOG_FATAL, "[%s:%d] Failed to update resource"
                                    " name[%s], addr[%s]", __FILE__, __LINE__,
                                    req_res->res_name, req_res->res_addr);
                            }
                            ul_writelog(UL_LOG_WARNING, "[%s:%d] Finish update for resource addr[%s]",
                                __FILE__, __LINE__, parepath);
                        }
                    }
                    pthread_rwlock_unlock(&m_req_res_rwlock);
                }
            }

            break;
        case GALILEO_ENTITY_CHANGED_EVENT:
            if (NULL != p){
                *p = '\0';
                rc = lleo_query_resource((lleo_t*)gali_hdlr, parepath, query_data, rp); 
                if (0 != rc){ 
                    ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to query resource, path[%s]",
                        __FILE__, __LINE__, parepath);
                    decr_refer_count();
                    return -2;
                }

                pthread_rwlock_rdlock(&m_req_res_rwlock);
                
                iter = m_req_res_lleo_list->begin();
                for(; iter != m_req_res_lleo_list->end(); ++iter){
                    req_resource_t *req_res = *iter;
                    if (0 == strcmp(req_res->res_addr, parepath)){
                        rc = update_resource_pool(req_res, query_data);
                        if (0 != rc){
                            ul_writelog(UL_LOG_FATAL, "[%s:%d] Failed to update resource"
                                " name[%s], addr[%s]", __FILE__, __LINE__,
                                req_res->res_name, req_res->res_addr);
                        }
                    }
                }
                pthread_rwlock_unlock(&m_req_res_rwlock);
            }
            break;
        case GALILEO_RESOURCE_UPDATE_EVENT:
            pthread_rwlock_rdlock(&m_req_res_rwlock);
            
            iter = m_req_res_lleo_list->begin();
            for(; iter != m_req_res_lleo_list->end(); ++iter){
                req_resource_t *req_res = *iter;
                if (0 == strcmp(req_res->res_addr, realpath)){
                    rc = update_resource_pool(req_res, res_data);
                    if (0 != rc){
                        ul_writelog(UL_LOG_FATAL, "[%s:%d] Failed to update resource"
                            " name[%s], addr[%s]", __FILE__, __LINE__,
                            req_res->res_name, req_res->res_addr);
                    }
                }
            }
            pthread_rwlock_unlock(&m_req_res_rwlock);
            ul_writelog(UL_LOG_WARNING, "[%s:%d] Finish update for resource addr[%s]",
                    __FILE__, __LINE__, realpath);
            break;
        default:
            ul_writelog(UL_LOG_WARNING, "[%s:%d] Get a invalid event [%d], path[%s]",
                    __FILE__, __LINE__, watch_type, realpath);
            break;
    }

    decr_refer_count();
    return rc;
}


int
RsrcPool :: init(const char *conf_dir, const char *conf_file,
        const int empt_req_flg, int *reg_tags, const int reg_tags_cnt)
{
    m_magic_num = 0;
    if (NULL == reg_tags && reg_tags_cnt > 0){
        ul_writelog(UL_LOG_WARNING, "[%s:%d] No space for reg_tags, cnt[%d]",
             __FILE__, __LINE__, reg_tags_cnt);
        return -1;
    }

    if (NULL != reg_tags && reg_tags_cnt <= 0){
        ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to get a valid reg_tags_cnt[%d]",
             __FILE__, __LINE__, reg_tags_cnt);
        return -1;
    }

    
    if (0 != m_req_res_list->create() || 0 != m_reg_res_list->create()){
        ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to initilize the Lsc_deque",
            __FILE__, __LINE__);
        return -1;
    }

    
    if (0 != m_req_res_lleo_list->create() || 0 != m_req_res_conf_list->create()){
        ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to initilize the req_res_lleo_list or reg_res_conf_list",
            __FILE__, __LINE__);
        return -1;
    }

    if (0 != pthread_rwlock_init(&m_req_res_rwlock, NULL)
            || 0 != pthread_mutex_init(&m_reg_res_mutex, NULL)){
        ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to initilize the pthread_mutex, msg[%m]",
             __FILE__, __LINE__);
        return -1;
    }

    if (0 != pthread_mutex_init(&m_ref_mutex, NULL)
            || 0 != pthread_cond_init(&m_ref_cond, NULL)
            || 0 != pthread_mutex_init(&m_run_mutex, NULL)){
        ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to initilize the pthread_mutex, msg[%m]",
                __FILE__, __LINE__);
        return -1;
    }

    
    int ret = m_gali_conf.load(conf_dir, conf_file);
    if (0 != ret){
        ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to load lleo config: dir[%s]"
            " file[%s] ret[%d]", __FILE__, __LINE__, conf_dir, conf_file, ret);
        return -1;
    }

    
    ret = check_config();
    if (0 != ret){
        ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to check lleo config",
             __FILE__, __LINE__);
        return -1;
    }

    m_empt_req_flg = empt_req_flg;
    
    m_gali_zoo_log = (NULL == m_gali_zoo_log)?GALILEO_DEFAULT_ZOO_LOG:m_gali_zoo_log;
    if (0 != lleo_set_log(m_gali_zoo_log, ZOO_LOG_LEVEL_INFO)){
        ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to initilize the zookeeper client log",
             __FILE__, __LINE__);
    }

    for (int i = 0; i < m_gali_num; ++i) {
        
        m_gali_hdlr[i] = lleo_init(m_gali_hosts[i], m_gali_sesn_time,
            m_gali_delay_watch_time[i]);

        if (NULL == m_gali_hdlr[i]){
            
            ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to initilize the lleo handler",
              __FILE__, __LINE__);
            m_init_from_conf[i] = 1;
            continue;
        }

        
        if (ZOO_CONNECTED_STATE != zoo_state(m_gali_hdlr[i]->zh))
            sleep(GALILEO_INIT_TIMELIMIT);

        if (ZOO_CONNECTED_STATE == zoo_state(m_gali_hdlr[i]->zh)){
            
            lleo_set_buf_size(m_gali_hdlr[i], m_gali_buff_size);
            ul_writelog(UL_LOG_WARNING, "[%s:%d] Connected to the resource server, "\
                "start update resource...", __FILE__, __LINE__);
        }
        else{
            
            m_init_from_conf[i] = 1;
            ul_writelog(UL_LOG_WARNING, "[%s:%d] Cannot connect to resource server %s, "\
                "svr_idx:%d, load configure for resource...", __FILE__, __LINE__,
                m_gali_hosts[i], i);
        }

        
        if (0 == m_init_from_conf[i]) {
            if (m_acl_vec[i].count > 0) {
                ret = lleo_add_auth(m_gali_hdlr[i], &m_acl_vec[i]);
                if (0 != ret) {
                    ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to add lleo auth to %s",
                            __FILE__, __LINE__, m_gali_hosts[i]);
                }
            }
        }
    }

    
    ret = request_resource_init();
    if (0 > ret){
        
        ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to initialize request resource",
                __FILE__, __LINE__);
        return -1;
    }

    
    
    

    ret = request_resource_init_from_conf();
    if (0 != ret){
        ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to initilize resource pool from configure",
            __FILE__, __LINE__);
        return -1;
    }

    if (!m_req_res_list->empty()) {
        
        ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to initilize resource pool",
            __FILE__, __LINE__);
        return -1;
    }

    ret = register_resource_init(reg_tags, reg_tags_cnt);
    if (0 != ret){
        
        return -1;
    }

    
    pthread_mutex_lock(&m_run_mutex);
    m_is_running = 1;
    pthread_mutex_unlock(&m_run_mutex);

    if (!m_req_res_conf_list->empty()) {
        
        ret = pthread_create(&m_update_thread, NULL, update_request_resource, (void*)0);
        if (0 != ret){
            ul_writelog(UL_LOG_WARNING, "[%s:%d] resource update thread failed to start, msg[%m]",
                 __FILE__, __LINE__);
            return -1;
        }

        m_magic_num = 0xfb726031;
        return 1;
    }

    return 0;
}


    int
RsrcPool :: check_config()
{
    
    int err_no = 0;
    const char *name = NULL;
    const char *addr = NULL;
    const char *data = NULL;

    
    m_gali_num = m_gali_conf["GALILEO_CLUSTER"]["NUM"].to_int32(&err_no);
    if (0 != err_no || m_gali_num > MAX_GALILEO_CLUSTER_NUM){
        ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to load lleo cluster num, err[%d],"
            " num[%d], max_num[%d]", __FILE__, __LINE__, err_no,
            m_gali_num, MAX_GALILEO_CLUSTER_NUM);
        return -1;
    }
    else {
        ul_writelog(UL_LOG_TRACE, "GALILEO_CLUSTER NUM: %d", m_gali_num);
    }

    for (int i = 0; i < m_gali_num; ++i) {
        const comcfg::ConfigUnit &ClusterUnit = m_gali_conf["GALILEO_CLUSTER"]["CLUSTER"];
        m_gali_hosts[i] = ClusterUnit[i]["ADDR"].to_cstr(&err_no);
        if (0 != err_no){
            ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to load a valid lleo cluster,"
                " err[%d]", __FILE__, __LINE__, err_no);
            return -1;
        }
        else{
            ul_writelog(UL_LOG_TRACE, "GALILEO CLUSTER ADDR: %s", m_gali_hosts[i]);
        }

        
        int acl_size = ClusterUnit[i]["ACL"].size();
        if (acl_size > 0) {
            err_no = allocate_ACL_vector(&m_acl_vec[i], acl_size);
            if (err_no || !m_acl_vec[i].data) {
                ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to allocate_ACL_vector, err[%d]",
                    __FILE__, __LINE__, err_no);
                return -1;
            }
            else {
                ul_writelog(UL_LOG_TRACE, "allocate_ACL_vecotr: %d", i);
            }
            const comcfg::ConfigUnit &AclUnit = ClusterUnit[i]["ACL"];
            for (unsigned int j = 0; j < AclUnit.size(); ++j) {
                
                const char *tmp = AclUnit[j]["SCHEME"].to_cstr(&err_no);
                if (0 != err_no) {
                    ul_writelog(UL_LOG_WARNING, "[%s:%d] config file SCHEME err[%d]",
                        __FILE__, __LINE__, err_no);
                    return -1;
                }
                char *scheme = strdup(tmp);
                if (!scheme) {
                    ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to strdup, err[%d]",
                        __FILE__, __LINE__, err_no);
                    return -1;
                }

                tmp = AclUnit[j]["ID"].to_cstr(&err_no);
                if (0 != err_no) {
                    ul_writelog(UL_LOG_WARNING, "[%s:%d] config file ID err[%d]",
                        __FILE__, __LINE__, err_no);
                    return -1;
                }

                char *id = strdup(tmp);
                if (!id) {
                    ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to strdup, err[%d]",
                        __FILE__, __LINE__, err_no);
                    return -1;
                }

                m_acl_vec[i].data[j].id.id = id;
                m_acl_vec[i].data[j].id.scheme = scheme;
            }
        }

        
        m_gali_delay_watch_time[i] = ClusterUnit[i]["DELAY"].to_int32(&err_no);
        if (0 != err_no){
            ul_writelog(UL_LOG_WARNING, "GALILEO_DELAY_WATCH_TIME: %d (Default)",
                    GALILEO_DEFAULT_DELAY_WATCH_TIME);
            m_gali_delay_watch_time[i] = GALILEO_DEFAULT_DELAY_WATCH_TIME;
        }
        else{
            ul_writelog(UL_LOG_TRACE, "GALILEO_DELAY_WATCH_TIME: %d",
                m_gali_delay_watch_time[i]);
        }
    }

    
    m_gali_buff_size = m_gali_conf["GALILEO_DATA_PACK_SIZE"].to_int32(&err_no);
    if (0 != err_no){
        ul_writelog(UL_LOG_WARNING, "GALILEO_DATA_PACK_SIZE: %d (Default)",
            GALILEO_DEFAULT_BUF_SIZE);
        m_gali_buff_size = GALILEO_DEFAULT_BUF_SIZE;
    }
    else{
        ul_writelog(UL_LOG_TRACE, "GALILEO_DATA_PACK_SIZE: %d", m_gali_buff_size);
    }

    
    m_gali_sesn_time = m_gali_conf["GALILEO_SESSION_EXPIRE_TIME"].to_int32(&err_no);
    if (0 != err_no){
        ul_writelog(UL_LOG_WARNING, "GALILEO_SESSION_EXPIRE_TIME: %d (Default)",
                GALILEO_DEFAULT_SESN_EXPIRE_TIME);
        m_gali_sesn_time = GALILEO_DEFAULT_SESN_EXPIRE_TIME;
    }
    else{
        ul_writelog(UL_LOG_TRACE, "GALILEO_SESSION_EXPIRE_TIME: %d", m_gali_sesn_time);
    }

    
    m_gali_conf_dir = m_gali_conf["GALILEO_CONF_DIR"].to_cstr(&err_no);
    if (0 != err_no){
        ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to load a lleo conf directory,"
            " err[%d]", __FILE__, __LINE__, err_no);
        return -1;
    }
    else{
        ul_writelog(UL_LOG_TRACE, "GALILEO_CONF_DIR: %s", m_gali_conf_dir);
    }

    
    m_gali_zoo_log = m_gali_conf["GALILEO_ZOO_LOG"].to_cstr(&err_no);
    if (0 != err_no){
        ul_writelog(UL_LOG_WARNING, "GALILEO_ZOO_LOG: %s (Default)",
            GALILEO_DEFAULT_ZOO_LOG);
        m_gali_zoo_log = NULL;
    }
    else{
        ul_writelog(UL_LOG_TRACE, "GALILEO_ZOO_LOG: %s", m_gali_zoo_log);
    }

    
    m_req_res_num = m_gali_conf["REQUEST_RESOURCE"]["NUM"].to_int32(&err_no);
    if (0 != err_no){
        ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to load a request resource number,"
            " err[%d]", __FILE__, __LINE__, err_no);
        return -1;
    }
    else{
        ul_writelog(UL_LOG_TRACE, "REQUEST_RESOURCE NUM: %d", m_req_res_num);
    }

    for (int i = 0; i < m_req_res_num; i++){
        req_resource_t *req_res = new req_resource_t(m_gali_buff_size);

        
        int rc = pthread_rwlock_init(&(req_res->res_rwlock), NULL);
        if (0 != rc){
            ul_writelog(UL_LOG_FATAL, "[%s:%d] Failed to initilize the read write lock, msg[%m],"
                " I can not continue", __FILE__, __LINE__);
            return -1;;
        }

        if (0 != m_req_res_list->push_back(req_res)){
            ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to add req_res structure", 
                    __FILE__, __LINE__);
            return -1;
        }

        name = m_gali_conf["REQUEST_RESOURCE"]["RES_ARR"][i]["NAME"].to_cstr(&err_no);
        if (0 != err_no){
            ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to get the name of %dth req_res, err[%d]",
                 __FILE__, __LINE__, i, err_no);
            return -1;
        }
        else{
            snprintf(req_res->res_name, GALILEO_DEFAULT_PATH_LEN, "%s", name);
            ul_writelog(UL_LOG_TRACE, "REQUEST_RESOURCE %d NAME: %s", i, req_res->res_name);
        }

        
        
        
        
        
        
        
        
        
        

        addr = m_gali_conf["REQUEST_RESOURCE"]["RES_ARR"][i]["ADDR"].to_cstr(&err_no);
        if (0 != err_no){
            ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to get the address of %dth req_res,"
                " err[%d]", __FILE__, __LINE__, i, err_no);
            return -1;
        }
        else{
            char raw_address[GALILEO_DEFAULT_PATH_LEN];
            snprintf(raw_address, GALILEO_DEFAULT_PATH_LEN, "%s", addr);
            char *cluster = NULL;
            char *path = NULL;
            err_no = parse_raw_address(raw_address, &cluster, &path);
            if (0 != err_no){
                ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to get the address of %dth"
                    " req_res, err[%d]", __FILE__, __LINE__, i, err_no);
                return -1;
            }

            snprintf(req_res->res_cluster, GALILEO_DEFAULT_PATH_LEN, "%s", cluster);
            req_res->res_cluster[path-cluster-1] = '\0';
            ul_writelog(UL_LOG_TRACE, "REQUEST_RESOURCE %d CLUSTER: %s", i,
                req_res->res_cluster);

            snprintf(req_res->res_addr, GALILEO_DEFAULT_PATH_LEN, "%s", path);
            ul_writelog(UL_LOG_TRACE, "REQUEST_RESOURCE %d ADDR: %s", i, req_res->res_addr);
        }

        req_res->res_capacity
            = m_gali_conf["REQUEST_RESOURCE"]["RES_ARR"][i]["CAPACITY"].to_int32(&err_no);
        if (0 != err_no){
            ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to get the number of %dth req_res, err[%d]",
                 __FILE__, __LINE__, i, err_no);
            return -1;
        }
        else{
            ul_writelog(UL_LOG_TRACE, "REQUEST_RESOURCE %d CAPACITY: %d", i, req_res->res_capacity);
        }
    }

    
    m_reg_res_num = m_gali_conf["REGISTER_RESOURCE"]["NUM"].to_int32(&err_no);
    if (0 != err_no){
        ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to load a response resource number, err[%d]",
             __FILE__, __LINE__, err_no);
        return -1;
    }
    else{
        ul_writelog(UL_LOG_TRACE, "REGISTER_RESOURCE NUM: %d", m_reg_res_num);
    }

    for (int i = 0; i < m_reg_res_num; i++){
        reg_resource_t *reg_res = new reg_resource_t(m_gali_buff_size);

        if (0 != m_reg_res_list->push_back(reg_res)){
            ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to add reg_res structure",
                    __FILE__, __LINE__);
            return -1;
        }

        addr = m_gali_conf["REGISTER_RESOURCE"]["RES_ARR"][i]["ADDR"].to_cstr(&err_no);
        if (0 != err_no){
            ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to get the address of %dth reg_res, err[%d]",
                 __FILE__, __LINE__, i, err_no);
            return -1;
        }
        else{
            char raw_address[GALILEO_DEFAULT_PATH_LEN];
            snprintf(raw_address, GALILEO_DEFAULT_PATH_LEN, "%s", addr);
            char *cluster = NULL;
            char *path = NULL;
            err_no = parse_raw_address(raw_address, &cluster, &path);
            if (0 != err_no){
                ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to get the address of %dth"
                    " reg_res, err[%d]", __FILE__, __LINE__, i, err_no);
                return -1;
            }

            snprintf(reg_res->res_cluster, GALILEO_DEFAULT_PATH_LEN, "%s", cluster);
            reg_res->res_cluster[path-cluster-1] = '\0';
            ul_writelog(UL_LOG_TRACE, "REQUEST_RESOURCE %d CLUSTER: %s", i,
                reg_res->res_cluster);

            snprintf(reg_res->res_addr, GALILEO_DEFAULT_PATH_LEN, "%s", path);
            ul_writelog(UL_LOG_TRACE, "REGISTER_RESOURCE %d ADDR: %s", i, reg_res->res_addr);
        }

        data = m_gali_conf["REGISTER_RESOURCE"]["RES_ARR"][i]["DATA"].to_cstr(&err_no);
        if (0 != err_no){
            ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to get the data of %dth reg_res, err[%d]",
                 __FILE__, __LINE__, i, err_no);
            return -1;
        }
        else{
            snprintf(reg_res->res_data, m_gali_buff_size, "%s", data);
            ul_writelog(UL_LOG_TRACE, "REGISTER_RESOURCE %d DATA: %s", i, reg_res->res_data);
        }

    }
    return 0;
}



    int
RsrcPool :: request_resource_init()
{
    int rc = 0;
    int grc = GALILEO_E_OK;
    void *value = NULL;

    
    m_req_table_by_name = create_myhashtable(MAX_RESOURCE_NUM, string_hash_djb2, string_equal);
    if (NULL == m_req_table_by_name){
        ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to create myhashtable for request reource "\
            "table(by name)", __FILE__, __LINE__);
        return -1;
    }

    if (0 >= m_req_res_num){
        
        return 0;
    }

    
    pthread_rwlock_wrlock(&m_req_res_rwlock);
    Lsc::list<req_resource_t*>::iterator iter = m_req_res_list->begin();
    Lsc::list<req_resource_t*>::iterator iter_tmp;
    for (; iter != m_req_res_list->end(); ++iter){
        req_resource_t *req_res = *iter;
        
        value = myhashtable_search(m_req_table_by_name, (void*)req_res->res_name);

        if (NULL != value){
            
            delete req_res;
            req_res = (req_resource_t*)value;
            iter_tmp = iter;
            --iter;
            m_req_res_list->erase(iter_tmp);
            

            ul_writelog(UL_LOG_WARNING,"[%s:%d] Dupliated res_name[%s] found, use the first"\
                " configure", __FILE__, __LINE__, req_res->res_name);
            continue;
        }

        Lsc::ResourcePool rp;
        Lsc::var::Dict& res_data = rp.create<Lsc::var::Dict>();

        
        int i = get_handle_by_name(req_res->res_cluster);
        if (i < 0) {
            ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to get cluster, cluster[%s]",
                __FILE__, __LINE__, req_res->res_cluster);
            continue;
        }

        grc = lleo_query_resource(m_gali_hdlr[i], req_res->res_addr, res_data, rp);
        if (GALILEO_E_OK != grc){
            ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to get resource, address[%s], err[%d], "\
                "msg[%s], init form conf", __FILE__, __LINE__, req_res->res_addr,
                grc, lleo_err2str(grc));

            continue;
        }

        
        rc = update_resource_pool(req_res, res_data);
        if (0 != rc){
            rc = 0;
            ul_writelog(UL_LOG_FATAL, "[%s:%d] Failed to update resource name[%s], addr[%s],"
                " init from conf", __FILE__, __LINE__, req_res->res_name, req_res->res_addr);
            continue;
        }

        
        grc = lleo_watch_resource(m_gali_hdlr[i], req_res->res_addr,
            resource_watcher, m_gali_hdlr[i]);
        if (GALILEO_E_OK != grc){
            ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to watch resource, address[%s],"
                " err[%d], msg[%s], init from conf", __FILE__, __LINE__,
                req_res->res_addr, grc, lleo_err2str(grc));

            continue;
        }

        iter_tmp = iter;
        --iter;
        m_req_res_list->erase(iter_tmp);
        m_req_res_lleo_list->push_back(req_res);
        ul_writelog(UL_LOG_NOTICE,"[%s:%d] init res"
                " from lleo, name[%s]", __FILE__, __LINE__, req_res->res_name);

        
        int trc = myhashtable_insert(m_req_table_by_name, strdup(req_res->res_name), (void*)req_res);
        if(!trc){
            ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to insert name table, I can not continue",
                    __FILE__, __LINE__);
            rc = -3;
            break;
        }

    }
    pthread_rwlock_unlock(&m_req_res_rwlock);

    return rc;
}



    int
RsrcPool :: request_resource_init_from_conf()
{
    int rc = 0;

    
    if (!m_req_table_by_name) {
        m_req_table_by_name = create_myhashtable(MAX_RESOURCE_NUM,
            string_hash_djb2, string_equal);
        if (NULL == m_req_table_by_name){
            ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to create myhashtable for"
                " request reource table(by name)", __FILE__, __LINE__);
            return -1;
        }
    }

    if (0 >= m_req_res_num){
        
        return rc;
    }

    Lsc::ResourcePool rp;
    void* value = NULL;
    char* json_txt = (char*)rp.create_raw(m_gali_buff_size*10);

    
    pthread_rwlock_wrlock(&m_req_res_rwlock);
    Lsc::list<req_resource_t*>::iterator iter = m_req_res_list->begin();
    Lsc::list<req_resource_t*>::iterator iter_tmp;
    while (iter != m_req_res_list->end()){
        req_resource_t *req_res = *iter;
        value = myhashtable_search(m_req_table_by_name, (void*)req_res->res_name);
        if (NULL == value){
            rc = myhashtable_insert(m_req_table_by_name, strdup(req_res->res_name),
                (void*)req_res);
            if(!rc){
                ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to insert name table",
                    __FILE__, __LINE__);
                break;
            }
        }
        else{
            
            delete req_res;
            req_res = (req_resource_t*)value;
            iter_tmp = iter;
            ++iter;
            m_req_res_list->erase(iter_tmp);
            

            ul_writelog(UL_LOG_WARNING,"[%s:%d] Dupliated res_name[%s] found,"
                " use the first configure", __FILE__, __LINE__, req_res->res_name);
            continue;
        }

        rc = load_respack(m_gali_conf_dir, req_res->res_name, json_txt);
        if (0 != rc){
            ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to load resource configure"
                " from file, name[%s]", __FILE__, __LINE__, req_res->res_name);
            break;
        }

        try{
            Lsc::var::JsonDeserializer jd(rp);
            Lsc::var::IVar& json_var = jd.deserialize(json_txt);

            rc = update_resource_pool(req_res, json_var);
            if (0 != rc){
                ul_writelog(UL_LOG_FATAL, "[%s:%d] Failed to update resource name[%s],"
                    " addr[%s]", __FILE__, __LINE__, req_res->res_name, req_res->res_addr);
                break;
            }
        }
        catch(...){
            ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to deserialize json text, "\
                    "resource name[%s], addr[%s]", __FILE__, __LINE__, 
                    req_res->res_name, req_res->res_addr);
            break;
        }
        
        req_res->init_from_conf = 1;
        iter_tmp = iter;
        ++iter;
        m_req_res_list->erase(iter_tmp);
        m_req_res_conf_list->push_back(req_res);
        ul_writelog(UL_LOG_NOTICE,"[%s:%d] init res"
                " from file, name[%s]", __FILE__, __LINE__, req_res->res_name);
    }

    pthread_rwlock_unlock(&m_req_res_rwlock);

    return rc;
}


    int
RsrcPool :: update_resource_pool(req_resource_t *res_item, Lsc::var::IVar& res_data)
{
    if (NULL == res_item){
        ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to get a valid resource item",
                 __FILE__, __LINE__);
        return -1;
    }

    if (!res_data.is_dict()){
        ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to get a valid resource dict",
            __FILE__, __LINE__);
        return -1;
    }

    
    if(res_data.get(GALILEO_ABSTRACT).is_null()){
        ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to get a valid abstract item",
                __FILE__, __LINE__);
        return -1;
    }
    Lsc::var::IVar& abst = res_data.get(GALILEO_ABSTRACT);

    if (!abst.is_dict()) {
        ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to get a valid resource dict",
            __FILE__, __LINE__);
        return -1;
    }

    if(abst.get(GALILEO_ABSTRACT_SERVICES).is_null()){
        ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to get a valid abstract services item",
                __FILE__, __LINE__);
        return -1;
    }
    Lsc::var::IVar& abst_slc = abst.get(GALILEO_ABSTRACT_SERVICES);

    if (!abst_slc.is_dict()) {
        ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to get a valid resource dict",
            __FILE__, __LINE__);
        return -1;
    }

    if(abst_slc.get(res_item->res_name).is_null()){
        ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to get a valid [%s] item",
                __FILE__, __LINE__, res_item->res_name);
        return -1;
    }
    Lsc::var::IVar& service = abst_slc.get(res_item->res_name);

    if(res_data.get(GALILEO_ENTITY).is_null()){
        ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to get a valid entity item",
                __FILE__, __LINE__);
        return -1;
    }
    Lsc::var::IVar& enti = res_data.get(GALILEO_ENTITY);

    int rc = update_socket_pool(res_item, service, enti);
    if (0 != rc){
        ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to update socket pool, name[%s]",
                __FILE__, __LINE__, res_item->res_name);
        return -1;
    }

    
    
    pthread_rwlock_wrlock(&(res_item->res_rwlock));
    try{
        Lsc::var::JsonSerializer js;
        Lsc::AutoBuffer buf;
        js.serialize(res_data, buf);
        res_item->res_data_len = strlen(buf.c_str()) + 1;
        memcpy(res_item->res_data, buf.c_str(), res_item->res_data_len);
        dump_respack(m_gali_conf_dir, res_item->res_name, buf.c_str());
    }
    catch(...){
        ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to serialize resource data, name[%s]",
                __FILE__, __LINE__, res_item->res_name);

        pthread_rwlock_unlock(&(res_item->res_rwlock));
        return -1;
    }

    res_item->res_lastupdateat = time(NULL);
    pthread_rwlock_unlock(&(res_item->res_rwlock));

    return 0;
}



    int
RsrcPool :: dump_respack(const char *dir, const char *name, const char *txt)
{
    if (NULL == dir || NULL == name || NULL == txt){
        ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to get a valid dir, name or txt",
            __FILE__, __LINE__);
        return -1;
    }

    char path[GALILEO_DEFAULT_PATH_LEN];
    snprintf(path, GALILEO_DEFAULT_PATH_LEN, "%s", dir);
    int start = strlen(dir);
    if ('/' != dir[start-1]){
        path[start] = '/';
        start += 1;
    }
    snprintf(path+start, GALILEO_DEFAULT_PATH_LEN-start, "%s", name);

    FILE *file = fopen(path, "w");
    if (NULL == file){
        ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to open dump file [%s]",
             __FILE__, __LINE__, path);
        return -1;
    }
    fprintf(file, "%s", txt);
    fclose(file);

    ul_writelog(UL_LOG_DEBUG, "Dump resource conf successfully, dir[%s], name[%s]", dir, name);
    return 0;
}


    int
RsrcPool :: load_respack(const char *dir, const char *name, char *txt)
{
    if (NULL == dir || NULL == name || NULL == txt){
        ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to get a valid dir, name or txt",
             __FILE__, __LINE__);
        return -1;
    }

    char path[GALILEO_DEFAULT_PATH_LEN];
    snprintf(path, GALILEO_DEFAULT_PATH_LEN, "%s", dir);
    int start = strlen(dir);
    if ('/' != dir[start-1]){
        path[start] = '/';
        start += 1;
    }
    snprintf(path+start, GALILEO_DEFAULT_PATH_LEN-start, "%s", name);

    FILE *file = fopen(path, "r");
    if (NULL == file){
        ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to open file [%s]", __FILE__, __LINE__, path);
        return -1;
    }
    
    int data_len = fread(txt, 1, m_gali_buff_size, file);
    data_len = (data_len >= m_gali_buff_size)? m_gali_buff_size-1:data_len;
    data_len = (data_len < 0)? 0:data_len;
    txt[data_len] = '\0';
    fclose(file);

    ul_writelog(UL_LOG_DEBUG, "Load resource conf successfully, dir[%s], name[%s]", dir, name);
    return 0;
}



    int
RsrcPool :: has_range_file_for_resource(const char *name, const char *dir, char *range_file)
{
    if (NULL == dir || NULL == name){
        ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to get a valid dir or name",
             __FILE__, __LINE__);
        return -1;
    }

    snprintf(range_file, GALILEO_DEFAULT_PATH_LEN, "%s", dir);
    int start = strlen(dir);
    if ('/' != dir[start-1]){
        range_file[start] = '/';
        start += 1;
    }
    snprintf(range_file+start, GALILEO_DEFAULT_PATH_LEN-start, "%s.range", name);

    struct stat fstat;
    if (0 == stat(range_file, &fstat)){
        ul_writelog(UL_LOG_WARNING,"[%s:%d] Find a range file[%s] for resource[%s]",
             __FILE__, __LINE__, range_file, name);
        return 1;
    }

    return 0;
}


    int
RsrcPool :: incr_refer_count()
{
    int rc = 1;
    pthread_mutex_lock(&m_run_mutex);
    if (0 == m_is_running){
        rc = 0;
    }
    else{
        pthread_mutex_lock(&m_ref_mutex);
        m_ref_count++;
        pthread_cond_signal(&m_ref_cond);
        pthread_mutex_unlock(&m_ref_mutex);
    }
    pthread_mutex_unlock(&m_run_mutex);
    return rc;
}


    int
RsrcPool :: decr_refer_count()
{
    pthread_mutex_lock(&m_ref_mutex);
    m_ref_count--;
    pthread_cond_signal(&m_ref_cond);
    pthread_mutex_unlock(&m_ref_mutex);
    return 0;
}



    int
RsrcPool :: get_service_by_name(Lsc::var::IVar& services, const char *name,
                                Lsc::var::IVar& service)
{
    if (!services.is_dict()){
        ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to get a valid services dict",
             __FILE__, __LINE__);
        return -1;
    }

    if (NULL == name){
        ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to get a valid name", __FILE__, __LINE__);
        return -1;
    }

    if(services.get(name).is_null()){
        return -2;
    }
    
    service = services.get(name);
    return 0;
}



    int
RsrcPool :: update_socket_pool(req_resource_t *res_item, Lsc::var::IVar &service,
        Lsc::var::IVar &entities)
{
    int rc        = 0;

    if (NULL == res_item){
        ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to get a valid res_item",
             __FILE__, __LINE__);
        return -1;
    }

    if (!service.is_dict() || !entities.is_dict()){
        ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to get a valid service dict or a entities dict",
            __FILE__, __LINE__);
        return -1;
    }

    int port = 0;
    int conn_type = 0;
    Lsc::var::Int32 conn_type_default = 0;
    Lsc::var::Int32 ctimeout_default = GALILEO_DEFAULT_CTIMEOUT;
    Lsc::var::Int32 rtimeout_default = GALILEO_DEFAULT_RTIMEOUT;
    Lsc::var::Int32 wtimeout_default = GALILEO_DEFAULT_WTIMEOUT;

    if(service.get(GALILEO_ABSTRACT_SVR_PORT).is_null()){
        ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to get a valid port",
                __FILE__, __LINE__);
        return -1;
    }
    Lsc::var::IVar& port_var = service.get(GALILEO_ABSTRACT_SVR_PORT);
    port = port_var.to_int32();

    if(service.get(GALILEO_ABSTRACT_SVR_CONNTYPE).is_null()){
        ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to get a valid connect type, use default[%d]",
                __FILE__, __LINE__, conn_type_default.to_int32());
    }
    Lsc::var::IVar& conn_type_var= service.get(GALILEO_ABSTRACT_SVR_CONNTYPE, conn_type_default);
    conn_type = conn_type_var.to_int32();
    
    if(service.get(GALILEO_ABSTRACT_SVR_CTIMEOUT).is_null()){
        ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to get a valid connect timeout, use default[%d]",
                __FILE__, __LINE__, GALILEO_DEFAULT_CTIMEOUT);
    }
    Lsc::var::IVar& ctimeout_var = service.get(GALILEO_ABSTRACT_SVR_CTIMEOUT, ctimeout_default);
    res_item->res_ctimeout = ctimeout_var.to_int32();

    if(service.get(GALILEO_ABSTRACT_SVR_RTIMEOUT).is_null()){
        ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to get a valid read timeout, use default[%d]",
                __FILE__, __LINE__, GALILEO_DEFAULT_RTIMEOUT);
    }
    Lsc::var::IVar& rtimeout_var = service.get(GALILEO_ABSTRACT_SVR_RTIMEOUT, rtimeout_default);
    res_item->res_rtimeout = rtimeout_var.to_int32();

    if(service.get(GALILEO_ABSTRACT_SVR_WTIMEOUT).is_null()){
        ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to get a valid write timeout, use default[%d]",
                __FILE__, __LINE__, GALILEO_DEFAULT_WTIMEOUT);
    }
    Lsc::var::IVar& wtimeout_var = service.get(GALILEO_ABSTRACT_SVR_WTIMEOUT, wtimeout_default);
    res_item->res_wtimeout = wtimeout_var.to_int32();

    pthread_rwlock_wrlock(&(res_item->res_rwlock)); 
    res_item->res_count = entities.size();
    size_t old_res_count = res_item->res_count;
    
    if (0 >= res_item->res_count){
        
       if (0 == m_empt_req_flg){
            
            ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to get a valid res_count[%d] for res_name[%s], "\
                "no change has been made", __FILE__, __LINE__, (int)res_item->res_count, res_item->res_name);
            res_item->res_count = old_res_count;
        }
        else{
            
            ul_writelog(UL_LOG_WARNING,"[%s:%d] Empty request resource found for res_name[%s]",
                 __FILE__, __LINE__, res_item->res_name);
            res_item->res_count = 0;
       }
        pthread_rwlock_unlock(&(res_item->res_rwlock));
        return ((0 == m_empt_req_flg)?-1:0);
    }

    
    comcm::Server *svr = new comcm::Server[res_item->res_count];
    comcm::Server **psvr = new comcm::Server*[res_item->res_count];

    int svr_idx = 0;
    int svr_per_capacity = int(res_item->res_capacity/res_item->res_count) + 1;
    const char *ip = NULL;
    char range_file[GALILEO_DEFAULT_PATH_LEN];
    int need_check = has_range_file_for_resource(res_item->res_name, m_gali_conf_dir, range_file);

    Lsc::var::IVar::dict_iterator iter = entities.dict_begin();
    for (; iter != entities.dict_end(); ++iter)
    {
        Lsc::var::IVar& item = iter->value();
        if (1 == need_check){
            comcfg::Configure config;
            rc = config.loadIVar(item);
            if (0 != 0){
                ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to load IVar to Configure,"\
                        " res_name[%s]", __FILE__, __LINE__, res_item->res_name);
            }
            else{
                rc = config.check_once(range_file);
                if (0 != rc){
                    ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to check once for resource,"\
                            " node[%s]", __FILE__, __LINE__, iter->key().c_str());
                    
                    goto EXIT;
                }
            }
        }

        if (item.get(GALILEO_ENTITY_IP).is_null()){
            ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to get ip from pack, idx[%d]",
                    __FILE__, __LINE__, svr_idx);
            rc = -1;
            goto EXIT;
        }

        ip = item.get(GALILEO_ENTITY_IP).c_str();
        ul_writelog(UL_LOG_DEBUG, "svr_idx:[%d], ip[%s], port[%d], ctimeout[%d], capacity[%d]",
                svr_idx, ip, port, res_item->res_ctimeout, svr_per_capacity);

        psvr[svr_idx] = &(svr[svr_idx]);
        rc = set_server_info(&(svr[svr_idx++]), ip, port, res_item->res_ctimeout, svr_per_capacity);
        if (0 != rc){
            ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to set server info, err[%d]",
                    __FILE__, __LINE__, rc);
            goto EXIT;
        }
    }

    
    if (NULL == res_item->res_sockpool){
        
        res_item->res_sockpool = new SocketPool();
        
        conn_type = (conn_type == 0)?1:0;
        res_item->res_sockpool->init(psvr, res_item->res_count, conn_type);
        ul_writelog(UL_LOG_WARNING,"[%s:%d] init socketpool for name[%s], count[%d], shortconn[%d]",
                 __FILE__, __LINE__, res_item->res_name, (int)res_item->res_count, conn_type);
    }
    else{
        
        res_item->res_sockpool->reload(psvr, res_item->res_count);
        ul_writelog(UL_LOG_WARNING,"[%s:%d] update socketpool for name[%s], count[%d]",
                 __FILE__, __LINE__, res_item->res_name, (int)res_item->res_count);
    }

EXIT:
    delete [] svr;
    delete [] psvr;
    if (0 != rc){
        
        res_item->res_count = old_res_count;
    }
    pthread_rwlock_unlock(&(res_item->res_rwlock));
    return rc;
}


    int
RsrcPool :: set_server_info(comcm::Server *svr, const char *ip,
        const int port, const int ctimeout, const int capacity)
{
    if (NULL == svr || NULL == ip){
        ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to get a valid Server pointer or"
            " a ip string", __FILE__, __LINE__);
        return -1;
    }

    svr->setIP(ip);
    svr->setPort(port);
    svr->setTimeout(ctimeout);
    svr->setCapacity(capacity);

    return 0;
}



    int
RsrcPool :: register_resource_init(int *reg_tags, const int reg_tags_cnt)
{
    int reg_res_idx = 0;
    int rc = 0;
    int grc = GALILEO_E_OK;
    if (0 >= m_reg_res_num){
        
        return rc;
    }

    if (NULL != reg_tags){
        if (m_reg_res_num > reg_tags_cnt){
            ul_writelog(UL_LOG_WARNING,"[%s:%d] No enough array space for register tag,"
                " need[%d], have[%d]", __FILE__, __LINE__, m_reg_res_num, reg_tags_cnt);
            return -1;
        }
    }

    
    pthread_mutex_lock(&m_reg_res_mutex);
    Lsc::list<reg_resource_t*>::iterator iter = m_reg_res_list->begin();
    for (; iter != m_reg_res_list->end(); ++iter){
        reg_resource_t *reg_res = *iter;
        Lsc::ResourcePool rp;
        Lsc::var::JsonDeserializer jd(rp);
        Lsc::var::Dict& res_var = rp.create<Lsc::var::Dict>();

        try{
            res_var = jd.deserialize(reg_res->res_data);
            if (!res_var.is_dict()){
                ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to deserialize res_data, path[%s]",
                    __FILE__, __LINE__, reg_res->res_addr);
                rc = -1;
                break;
            }

            
            if (res_var.get(GALILEO_ENTITY_IP).is_null()){
                char* tmpip = new char[MAX_IP_STR_LEN];
                rc = get_local_conn_ip(tmpip, MAX_IP_STR_LEN);
                if (0 == rc){
                    Lsc::var::String& ip_str = rp.create<Lsc::var::String>(tmpip);
                    res_var.set(GALILEO_ENTITY_IP, ip_str);
                }
                else{
                    ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to get a valid host ip", 
                            __FILE__, __LINE__);
                }
                delete [] tmpip;
            }

            
            if (res_var.get(GALILEO_ENTITY_HOSTNAME).is_null()){
                char* tmphostname = new char[MAX_HOSTNAME_STR_LEN];
                rc = gethostname(tmphostname, MAX_HOSTNAME_STR_LEN);
                if (0 == rc){
                    Lsc::var::String& hostname_str = rp.create<Lsc::var::String>(tmphostname);
                    res_var.set(GALILEO_ENTITY_HOSTNAME, hostname_str);
                }
                else{
                    ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to deserialize json data",
                            __FILE__, __LINE__);
                }
                delete [] tmphostname;
            }

        }
        catch(...){
            ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to deserialize res_data, path[%s]",
                    __FILE__, __LINE__, reg_res->res_addr);

            rc = -1;
            break;
        }

        int i = get_handle_by_name(reg_res->res_cluster);
        if (i < 0) {
            ul_writelog(UL_LOG_WARNING,"[%s:%d] Can not find lleo[%s]",
                     __FILE__, __LINE__, reg_res->res_cluster);
            rc = -1;
            break;
        }

        grc = lleo_register_resource(m_gali_hdlr[i], reg_res->res_addr,
            res_var, &(reg_res->res_tag));
        if (0 != grc){
            ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to register new resource"
                " at addr[%s], err[%d], msg[%s]", __FILE__, __LINE__,
                reg_res->res_addr, grc, lleo_err2str(grc));
            rc = -1;
            break;
        }
        ul_writelog(UL_LOG_DEBUG, "add register resource err[%d]", grc);
        if (NULL != reg_tags)
            reg_tags[reg_res_idx++] = reg_res->res_tag;
    }

    pthread_mutex_unlock(&m_reg_res_mutex);
    return rc;
}


    int
RsrcPool :: get_timeout(const char* res_name, int *rtimeout, int *wtimeout)
{
    if (NULL == res_name || NULL == rtimeout || NULL == wtimeout){
        ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to get a valid res_name,"
            " rtimeout or a wtimeout", __FILE__, __LINE__);
        return -1;
    }

    if(!incr_refer_count()){
        return -1;
    }

    void *value = NULL;
    pthread_rwlock_rdlock(&m_req_res_rwlock);
    value = myhashtable_search(m_req_table_by_name, (void*)res_name);
    if (NULL == value){
        ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to get resource of name[%s]",
             __FILE__, __LINE__, res_name);
        pthread_rwlock_unlock(&m_req_res_rwlock);
        decr_refer_count();
        return -2;
    }

    req_resource_t *req_res = (req_resource_t*)value;
    pthread_rwlock_rdlock(&(req_res->res_rwlock));

    *rtimeout = req_res->res_rtimeout;
    *wtimeout = req_res->res_wtimeout;

    pthread_rwlock_unlock(&(req_res->res_rwlock));
    pthread_rwlock_unlock(&m_req_res_rwlock);

    decr_refer_count();
    return 0;
}


    int
RsrcPool :: fetch_socket(const char* res_name, int key, int waittime)
{
    if (NULL == res_name){
        ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to get a valid res_name",
            __FILE__, __LINE__);
        return -1;
    }

    if(!incr_refer_count()){
        return -1;
    }

    void *value = NULL;
    pthread_rwlock_rdlock(&m_req_res_rwlock);
    value = myhashtable_search(m_req_table_by_name, (void*)res_name);
    if (NULL == value){
        ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to get resource of name[%s]",
            __FILE__, __LINE__, res_name);
        pthread_rwlock_unlock(&m_req_res_rwlock);
        decr_refer_count();
        return -2;
    }

    req_resource_t *req_res = (req_resource_t*)value;
    pthread_rwlock_rdlock(&(req_res->res_rwlock));
    if (0 >= req_res->res_count){
        ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to get resource of name[%s], count[%d]",
                 __FILE__, __LINE__, res_name, (int)req_res->res_count);
        pthread_rwlock_unlock(&(req_res->res_rwlock));
        pthread_rwlock_unlock(&m_req_res_rwlock);
        decr_refer_count();
        return -3;
    }

    int fd = req_res->res_sockpool->FetchSocket(key, waittime);
    pthread_rwlock_unlock(&(req_res->res_rwlock));
    pthread_rwlock_unlock(&m_req_res_rwlock);

    decr_refer_count();
    return (fd >= 0)?fd:-4;
}


    int
RsrcPool :: free_socket(const char* res_name, int sock, bool errclose)
{
    if (NULL == res_name){
        ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to get a valid res_name",
            __FILE__, __LINE__);
        return -1;
    }

    if(!incr_refer_count()){
        return -1;
    }

    void *value = NULL;
    pthread_rwlock_rdlock(&m_req_res_rwlock);
    value = myhashtable_search(m_req_table_by_name, (void*)res_name);
    if (NULL == value){
        ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to get resource of name[%s]",
            __FILE__, __LINE__, res_name);
        pthread_rwlock_unlock(&m_req_res_rwlock);
        decr_refer_count();
        return -2;
    }

    req_resource_t *req_res = (req_resource_t*)value;
    pthread_rwlock_rdlock(&(req_res->res_rwlock));
    if (0 >= req_res->res_count){
        ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to get resource of name[%s], count[%d]",
                 __FILE__, __LINE__, res_name, (int)req_res->res_count);
        pthread_rwlock_unlock(&(req_res->res_rwlock));
        pthread_rwlock_unlock(&m_req_res_rwlock);
        decr_refer_count();
        return -3;
    }

    int rc = req_res->res_sockpool->FreeSocket(sock, errclose);
    pthread_rwlock_unlock(&(req_res->res_rwlock));
    pthread_rwlock_unlock(&m_req_res_rwlock);
    
    decr_refer_count();
    return (0 == rc)?0:-4;
}


    int
RsrcPool :: get_handle_by_name(const char *gali_name)
{
    if (NULL == gali_name){
        ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to get a valid"
            " lleo name", __FILE__, __LINE__);
        return -1;
    }

    for (int i = 0; i < m_gali_num; ++i) {
        if (!m_init_from_conf[i] && !strcmp(m_gali_hosts[i], gali_name)) {
            if (m_gali_hdlr[i]) {
                return i;
            }
            else {
                return -1;
            }
        }
    }

    return -1;
}


    int
RsrcPool :: add_request_resource(const char *res_name, const char *res_addr,
    const int capacity)
{
    int rc = 0;
    if (NULL == res_name || NULL == res_addr){
        ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to get a valid res_name or"
            " a valid res_addr", __FILE__, __LINE__);
        return -1;
    }

    if(!incr_refer_count()){
        return -1;
    }

    
    char raw_address[GALILEO_DEFAULT_PATH_LEN];
    snprintf(raw_address, GALILEO_DEFAULT_PATH_LEN, "%s", res_addr);
    char *cluster = NULL;
    char *path = NULL;
    rc = parse_raw_address(raw_address, &cluster, &path);
    if (0 != rc){
        decr_refer_count();
        ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to get a valid res_addr",
             __FILE__, __LINE__);
        return -1;
    }
    raw_address[path-cluster-1] = '\0';

    
    void *value = NULL;
    req_resource_t *req_res = NULL;
    pthread_rwlock_wrlock(&m_req_res_rwlock);
    value = myhashtable_search(m_req_table_by_name, (void*)res_name);
    if(NULL != value){
        req_res = (req_resource_t*)value;
        if (0 != strcmp(req_res->res_addr, path)){
            ul_writelog(UL_LOG_WARNING,"[%s:%d] duplicated res_name[%s],"
                " with different res_addr[%s][%s]", __FILE__, __LINE__,
                res_name, req_res->res_addr, path);
            pthread_rwlock_unlock(&m_req_res_rwlock);
            decr_refer_count();
            return -1;
        }
    }
    else{
        req_res = new req_resource_t(m_gali_buff_size);
        snprintf(req_res->res_name, GALILEO_DEFAULT_PATH_LEN, "%s", res_name);
        snprintf(req_res->res_addr, GALILEO_DEFAULT_PATH_LEN, "%s", path);
        snprintf(req_res->res_cluster, GALILEO_DEFAULT_PATH_LEN, "%s", cluster);
        req_res->res_capacity = capacity;

        
        rc = pthread_rwlock_init(&(req_res->res_rwlock), NULL);
        if (0 != rc){
            ul_writelog(UL_LOG_FATAL, "[%s:%d] Failed to initilize the read"
                " write lock, msg[%m]", __FILE__, __LINE__);
            delete req_res;
            pthread_rwlock_unlock(&m_req_res_rwlock);
            decr_refer_count();
            return -1;
        }
    }

    int grc = GALILEO_E_OK;
    int i = get_handle_by_name(req_res->res_cluster);

    
    if (i >= 0 && NULL != m_gali_hdlr[i]){
        Lsc::ResourcePool rp;
        Lsc::var::Dict& res_var = rp.create<Lsc::var::Dict>();
        grc = lleo_query_resource(m_gali_hdlr[i], req_res->res_addr, res_var, rp);
        if (GALILEO_E_OK != grc){
            ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to get resource, address[%s],"
                " err[%d], msg[%s]", __FILE__, __LINE__, req_res->res_addr,
                grc, lleo_err2str(grc));
            rc = -2;
            goto EXIT;
        }

        rc = update_resource_pool(req_res, res_var);
        if (0 != rc){
            ul_writelog(UL_LOG_FATAL, "[%s:%d] Failed to update resource name[%s], address[%s]",
                    __FILE__, __LINE__, req_res->res_name, req_res->res_addr);
            
            rc = -3;
            goto EXIT;
        }
        req_res->init_from_conf = 0;
    }
    else{

        char *json_txt = new char[m_gali_buff_size * 10];
        rc = load_respack(m_gali_conf_dir, req_res->res_name, json_txt);
        if (0 != rc){
            ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to load resource configure"
                " form file, name[%s]", __FILE__, __LINE__, req_res->res_name);

            delete [] json_txt;
            rc = -2;
            goto EXIT;
        }

        try{
            Lsc::ResourcePool rp;
            Lsc::var::JsonDeserializer jd(rp);
            Lsc::var::IVar& json_var = jd.deserialize(json_txt);

            rc = update_resource_pool(req_res, json_var);
            if (0 != rc){
                ul_writelog(UL_LOG_FATAL, "[%s:%d] Failed to update resource name[%s],"
                    " address[%s]", __FILE__, __LINE__,
                    req_res->res_name, req_res->res_addr);

                delete [] json_txt;
                
                rc = -3;
                goto EXIT;
            }
        }
        catch(...){
            ul_writelog(UL_LOG_FATAL, "[%s:%d] Failed to deserialize json text, name[%s],"
                " address[%s]", __FILE__, __LINE__, req_res->res_name, req_res->res_addr);

            delete [] json_txt;
            rc = -3;
            goto EXIT;
        }

        req_res->init_from_conf = 1;
        delete [] json_txt;
    }

    if (NULL == value){
        
        Lsc::list<req_resource_t*> *list;
        if (req_res->init_from_conf == 0) {
            list = m_req_res_lleo_list;
        } else {
            list = m_req_res_conf_list;
        }

        if (0 != list->push_back(req_res)){
            ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to push new req_res", __FILE__, __LINE__);
            rc = -4;
            goto EXIT;
        }

        m_req_res_num = m_req_res_conf_list->size() + m_req_res_lleo_list->size();

        int rc = myhashtable_insert(m_req_table_by_name, strdup(res_name), (void*)req_res);
        if (!rc){
            ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to insert name table, name[%s]",
                 __FILE__, __LINE__, res_name);
            list->pop_back();
            rc = -4;
            goto EXIT;
        }
        
        if (i >= 0 && NULL != m_gali_hdlr[i]){
            grc = lleo_watch_resource(m_gali_hdlr[i], req_res->res_addr,
                    this->resource_watcher, m_gali_hdlr[i]);
            if (GALILEO_E_OK != grc){
                ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to get resource, address[%s],"
                    " err[%d], msg[%s]", __FILE__, __LINE__,
                    req_res->res_addr, grc, lleo_err2str(grc));
                list->pop_back();
                char tmp_name[GALILEO_DEFAULT_PATH_LEN];
                snprintf(tmp_name, GALILEO_DEFAULT_PATH_LEN, "%s", req_res->res_name);
                myhashtable_remove(m_req_table_by_name, tmp_name);
                rc = -5;
                goto EXIT;
            }
        }
    }

EXIT:
    pthread_rwlock_unlock(&m_req_res_rwlock);
    if (0 != rc && NULL == value)
        delete req_res;
    decr_refer_count();
    return rc;
}


    int
RsrcPool :: remove_request_resource(const char *res_name, const char *res_addr)
{
    int rc = 0;
    if (NULL == res_name || NULL == res_addr){
        ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to get a valid res_name or a res_addr",
             __FILE__, __LINE__);
        return -1;
    }

    if(!incr_refer_count()){
        return -1;
    }

    
    char raw_address[GALILEO_DEFAULT_PATH_LEN];
    snprintf(raw_address, GALILEO_DEFAULT_PATH_LEN, "%s", res_addr);
    char *cluster = NULL;
    char *path = NULL;
    rc = parse_raw_address(raw_address, &cluster, &path);
    if (0 != rc){
        decr_refer_count();
        ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to get a valid res_addr",
             __FILE__, __LINE__);
        return -1;
    }
    raw_address[path-cluster-1] = '\0';

    
    void *value = NULL;
    req_resource_t *req_res = NULL;
    pthread_rwlock_wrlock(&m_req_res_rwlock);
    value = myhashtable_search(m_req_table_by_name, (void*)res_name);
    
    req_res = (req_resource_t*)value;
    if(NULL != value && 0 == strcmp(req_res->res_addr, path)){
        
        char tmp_name[GALILEO_DEFAULT_PATH_LEN];
        snprintf(tmp_name, GALILEO_DEFAULT_PATH_LEN, "%s", res_name);
        myhashtable_remove(m_req_table_by_name, tmp_name);

        
        Lsc::list<req_resource_t*> *list;
        Lsc::list<req_resource_t*>::iterator iter;
        if (req_res->init_from_conf) {
            list = m_req_res_conf_list;
        } else {
            list = m_req_res_lleo_list;
        }

        for (iter = list->begin(); iter != list->end(); ++iter){
            req_resource_t * req_res = *iter;
            if ((0 == strcmp(req_res->res_name, res_name))
                    && (0 == strcmp(req_res->res_cluster, cluster))){
                list->erase(iter);
                delete req_res;
                break;
            }
        }

        
        int is_addr_dup = 0;
        iter = list->begin();
        for (; iter != list->end(); ++iter){
            req_resource_t * req_res = *iter;
            if (0 == strcmp(req_res->res_addr, path)){
                is_addr_dup = 1;
                break;
            }
        }

        if (0 == is_addr_dup) {
            int i = get_handle_by_name(req_res->res_cluster);
            if (i < 0) {
                ul_writelog(UL_LOG_WARNING,"[%s:%d] Can not find lleo[%s]",
                        __FILE__, __LINE__, req_res->res_cluster);
                pthread_rwlock_unlock(&m_req_res_rwlock);
                decr_refer_count();
                return 0;
            }
            lleo_delete_watch(m_gali_hdlr[i], path);
        }
    }
    else{
        ul_writelog(UL_LOG_WARNING,"[%s:%d] Intend to remove a request resource which not existed, "\
                "name[%s], addr[%s]", __FILE__, __LINE__, res_name, res_addr);
        pthread_rwlock_unlock(&m_req_res_rwlock);
        decr_refer_count();
        return -2;
    }
    pthread_rwlock_unlock(&m_req_res_rwlock);

    decr_refer_count();
    return 0;
}



    int
RsrcPool :: add_register_resource(const char *res_addr, const char *res_data)
{
    if (NULL == res_addr || NULL == res_data || NULL == m_gali_hdlr){
        ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to get a res_addr or a valid res_data",
             __FILE__, __LINE__);
        return -1;
    }

    int rc = 0;
    int grc = GALILEO_E_OK;

    if(!incr_refer_count()){
        return -1;
    }

    
    char raw_address[GALILEO_DEFAULT_PATH_LEN];
    snprintf(raw_address, GALILEO_DEFAULT_PATH_LEN, "%s", res_addr);
    char *cluster = NULL;
    char *path = NULL;
    rc = parse_raw_address(raw_address, &cluster, &path);
    if (0 != rc){
        ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to get a valid res_addr",
             __FILE__, __LINE__);
        decr_refer_count();
        return -1;
    }
    raw_address[path-cluster-1] = '\0';


    reg_resource_t *reg_res = new reg_resource_t(m_gali_buff_size);
    Lsc::ResourcePool rp;
    Lsc::var::JsonDeserializer jd(rp);
    Lsc::var::Dict& json_var = rp.create<Lsc::var::Dict>();

    try{
        json_var = jd.deserialize(res_data);

        if (!json_var.is_dict()){
            ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to get a valid dict",
                __FILE__, __LINE__);
            delete reg_res;
            decr_refer_count();
            return -3;
        }

        
        if (json_var.get(GALILEO_ENTITY_IP).is_null()){
            char* tmpip = new char[MAX_IP_STR_LEN];
            rc = get_local_conn_ip(tmpip, MAX_IP_STR_LEN);
            if (0 == rc){
                Lsc::var::String& ip_str = rp.create<Lsc::var::String>(tmpip);
                json_var.set(GALILEO_ENTITY_IP, ip_str);
            }
            else{
                ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to get a valid host ip", 
                        __FILE__, __LINE__);
            }
            delete [] tmpip;
        }

        
        if (json_var.get(GALILEO_ENTITY_HOSTNAME).is_null()){
            char* tmphostname = new char[MAX_HOSTNAME_STR_LEN];
            rc = gethostname(tmphostname, MAX_HOSTNAME_STR_LEN);
            if (0 == rc){
                Lsc::var::String& hostname_str = rp.create<Lsc::var::String>(tmphostname);
                json_var.set(GALILEO_ENTITY_HOSTNAME, hostname_str);
            }
            else{
                ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to get a valid host name",
                        __FILE__, __LINE__);
            }
            delete [] tmphostname;
        }
    }
    catch(...){
        ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to get a valid dict", __FILE__, __LINE__);
        delete reg_res;
        decr_refer_count();
        return -4;
    }

    rc = 0;
    pthread_mutex_lock(&(m_reg_res_mutex));
    snprintf(reg_res->res_cluster, GALILEO_DEFAULT_PATH_LEN, "%s", cluster);
    int i = get_handle_by_name(reg_res->res_cluster);
    if (i < 0) {
        ul_writelog(UL_LOG_WARNING,"[%s:%d] Can not find lleo[%s]",
                __FILE__, __LINE__, reg_res->res_cluster);
        delete reg_res;
        rc = -5;
        goto out;
    }

    grc = lleo_register_resource(m_gali_hdlr[i], path, json_var, &(reg_res->res_tag));
    if (0 != grc){
        ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to register new resource at addr[%s], err[%d], msg[%s]",
             __FILE__, __LINE__, res_addr, grc, lleo_err2str(grc));
        delete reg_res;
        rc = -5;
    }
    else{
        if (0 != m_reg_res_list->push_back(reg_res)){
            ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to add reg_res structure", __FILE__, __LINE__);
            delete reg_res;
            rc = -5;
        }
        else{
            snprintf(reg_res->res_addr, GALILEO_DEFAULT_PATH_LEN, "%s", path);
            snprintf(reg_res->res_data, m_gali_buff_size, "%s", res_data);
            m_reg_res_num = m_reg_res_list->size();
        }
    }

out:
    pthread_mutex_unlock(&(m_reg_res_mutex));

    decr_refer_count();
    return (0 == rc)?reg_res->res_tag:rc;
}



    int
RsrcPool :: remove_register_resource(const char *res_addr, const int res_tag)
{
    int rc = 0;
    if (NULL == res_addr || 0 >= res_tag || NULL == m_gali_hdlr){
        ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to get a valid res_addr, a valid res_tag",
             __FILE__, __LINE__);
        return -1;
    }

    if(!incr_refer_count()){
        return -1;
    }

    
    char raw_address[GALILEO_DEFAULT_PATH_LEN];
    snprintf(raw_address, GALILEO_DEFAULT_PATH_LEN, "%s", res_addr);
    char *cluster = NULL;
    char *path = NULL;
    rc = parse_raw_address(raw_address, &cluster, &path);
    if (0 != rc){
        ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to get a valid res_addr",
             __FILE__, __LINE__);
        decr_refer_count();
        return -1;
    }
    raw_address[path-cluster-1] = '\0';

    pthread_mutex_lock(&m_reg_res_mutex);
    Lsc::list<reg_resource_t*>::iterator iter = m_reg_res_list->begin();
    Lsc::list<reg_resource_t*>::iterator iter_tmp = iter;
    for(; iter != m_reg_res_list->end(); ++iter){
        reg_resource_t *reg_res = *iter;
        if ((0 == strcmp(reg_res->res_addr, path))
                && (reg_res->res_tag == res_tag)
                && (0 == strcmp(reg_res->res_cluster, cluster))){

            int i = get_handle_by_name(reg_res->res_cluster);
            if (i < 0) {
                ul_writelog(UL_LOG_WARNING,"[%s:%d] Can not find lleo[%s]",
                        __FILE__, __LINE__, reg_res->res_cluster);
                iter_tmp = iter;
                iter++;
                m_reg_res_list->erase(iter_tmp);
                delete reg_res;
                rc = -5;
                continue;
            }

            int grc = lleo_delete_resource(m_gali_hdlr[i], path, res_tag);
            if (0 != grc){
                ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to delete resource from"
                    " resource server, err[%d], msg[%s]", __FILE__, __LINE__,
                    grc, lleo_err2str(grc));
                rc = -2;
                break;
            }
            delete reg_res;
            m_reg_res_list->erase(iter);
            break;
        }
    }
    pthread_mutex_unlock(&m_reg_res_mutex);

    decr_refer_count();
    return rc;
}


    int
RsrcPool :: destroy_registed_resource()
{
    int rc = 0;

    if(!incr_refer_count()){
        return -1;
    }

    

    pthread_mutex_lock(&m_reg_res_mutex);
    Lsc::list<reg_resource_t*>::iterator iter = m_reg_res_list->begin();
    Lsc::list<reg_resource_t*>::iterator iter_tmp = iter;
    for(; iter != m_reg_res_list->end();){
        reg_resource_t *reg_res = *iter;

        int i = get_handle_by_name(reg_res->res_cluster);
        if (i < 0) {
            ul_writelog(UL_LOG_WARNING,"[%s:%d] Can not find lleo[%s]",
                    __FILE__, __LINE__, reg_res->res_cluster);
            iter_tmp = iter;
            iter++;
            m_reg_res_list->erase(iter_tmp);
            delete reg_res;
            rc = -1;
            continue;
        }

        int grc = lleo_delete_resource(m_gali_hdlr[i], reg_res->res_addr, reg_res->res_tag);
        
        if (0 != grc){
            ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to delete resource from"
                " resource server, err[%d], msg[%s]", __FILE__, __LINE__,
                grc, lleo_err2str(grc));
            rc = -2;
        }
        iter_tmp = iter;
        iter++;
        m_reg_res_list->erase(iter_tmp);
        delete reg_res;
    }
    pthread_mutex_unlock(&m_reg_res_mutex);

    decr_refer_count();
    return rc;
}




    int 
RsrcPool :: get_request_resource_data_json(const char* res_name, char* data_buff, int buff_len)
{
    int rc = 0;
    if (NULL == res_name || NULL == data_buff){
        ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to get a valid res_name, data_buff",
             __FILE__, __LINE__);
        return -1;
    }

    if(!incr_refer_count()){
        return -1;
    }

    void *value = NULL;
    req_resource_t *req_res = NULL;
    pthread_rwlock_rdlock(&m_req_res_rwlock);
    value = myhashtable_search(m_req_table_by_name, (void*)res_name);
    req_res = (req_resource_t*)value;
    if (NULL != value){
        if (buff_len < req_res->res_data_len){
            ul_writelog(UL_LOG_WARNING,"[%s:%d] buff_len[%d] is not enough, want[%d]",
                 __FILE__, __LINE__, buff_len, req_res->res_data_len);
            rc = -1;
        }
        else{
            pthread_rwlock_rdlock(&(req_res->res_rwlock));
            memcpy(data_buff, req_res->res_data, req_res->res_data_len);
            pthread_rwlock_unlock(&(req_res->res_rwlock));
        }
    }
    else{
        
        ul_writelog(UL_LOG_WARNING,"[%s:%d] No data for the resource name[%s]",
                __FILE__, __LINE__, res_name);
        rc = -2;
    }
    pthread_rwlock_unlock(&m_req_res_rwlock);

    decr_refer_count();
    return rc;
}


    int
RsrcPool :: get_request_resource_data(const char* res_name, Lsc::var::IVar& data_var,
        Lsc::ResourcePool& rp)
{
    int rc = 0;
    if (NULL == res_name){
        ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to get a valid res_name",
                __FILE__, __LINE__);
        return -1;
    }

    if (!incr_refer_count()){
        return -1;
    }

    void *value = NULL;
    req_resource_t *req_res = NULL;
    pthread_rwlock_rdlock(&m_req_res_rwlock);
    value = myhashtable_search(m_req_table_by_name, (void*)res_name);
    req_res = (req_resource_t*)value;
    if (NULL != value){
        pthread_rwlock_rdlock(&(req_res->res_rwlock));
        try{
            Lsc::var::JsonDeserializer jd(rp);
            data_var = jd.deserialize(req_res->res_data);
        }
        catch(...){
            
            ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to deserialize json data, name[%s]",
                    __FILE__, __LINE__, res_name);

            rc = -3;
        }
        pthread_rwlock_unlock(&(req_res->res_rwlock));
    }
    else{
        
        ul_writelog(UL_LOG_WARNING,"[%s:%d] No data for the resource name[%s]",
                __FILE__, __LINE__, res_name);
        rc = -2;
    }
    pthread_rwlock_unlock(&m_req_res_rwlock);

    decr_refer_count();
    return rc;
}


    int
RsrcPool :: get_request_resource_data_buff(const char* res_name,
        char* data_buff, int buff_len, int mcpack_version)
{
    int rc = 0;
    if (NULL == res_name || NULL == data_buff){
        ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to get a valid res_name, data_buff",
                __FILE__, __LINE__);
        return -1;
    }

    if(!incr_refer_count()){
        return -1;
    }

    void *value = NULL;
    Lsc::ResourcePool rp;
    req_resource_t *req_res = NULL;
    pthread_rwlock_rdlock(&m_req_res_rwlock);
    value = myhashtable_search(m_req_table_by_name, (void*)res_name);
    req_res = (req_resource_t*)value;
    if (NULL != value){
        pthread_rwlock_rdlock(&(req_res->res_rwlock));
        try{
            Lsc::var::JsonDeserializer jd(rp);
            Lsc::var::IVar& data_var = jd.deserialize(req_res->res_data);

            Lsc::var::CompackSerializer ms;
            ms.set_opt(1, &mcpack_version, sizeof(mcpack_version));
            ms.serialize(data_var, data_buff, buff_len);
        }
        catch(...){
            
            ul_writelog(UL_LOG_WARNING,"[%s:%d] Failed to deserialize or serialize data, name[%s]",
                    __FILE__, __LINE__, res_name);

            rc = -3;
        }
        pthread_rwlock_unlock(&(req_res->res_rwlock));
    }
    else{
        
        ul_writelog(UL_LOG_WARNING,"[%s:%d] No data for the resource name[%s]",
                __FILE__, __LINE__, res_name);
        rc = -2;
    }

    pthread_rwlock_unlock(&m_req_res_rwlock);

    decr_refer_count();
    return rc;
}



void
RsrcPool :: show_request_resource()
{
    if(!incr_refer_count()){
        return;
    }

    pthread_rwlock_rdlock(&m_req_res_rwlock);
    Lsc::list<req_resource_t*>::iterator iter;
    
    for (iter = m_req_res_lleo_list->begin(); iter != m_req_res_lleo_list->end(); ++iter){
        req_resource_t *req_res = *iter;
        pthread_rwlock_rdlock(&(req_res->res_rwlock));
        fprintf(stderr, "request_res: name:%s, addr:%s, count:%d, capacity:%d, rtimeout:%d, "\
            "wtimeout:%d\n", req_res->res_name, req_res->res_addr, (int)req_res->res_count,
            (int)req_res->res_capacity, (int)req_res->res_rtimeout, (int)req_res->res_wtimeout);

        ul_writelog(UL_LOG_DEBUG, "request_res: name:%s, addr:%s, count:%d, capacity:%d, "\
            "rtimeout:%d, wtimeout:%d\n", req_res->res_name, req_res->res_addr,
            (int)req_res->res_count, req_res->res_capacity, req_res->res_rtimeout, req_res->res_wtimeout);
        pthread_rwlock_unlock(&(req_res->res_rwlock));
    }
    
    for (iter = m_req_res_conf_list->begin(); iter != m_req_res_conf_list->end(); ++iter){
        req_resource_t *req_res = *iter;
        pthread_rwlock_rdlock(&(req_res->res_rwlock));
        fprintf(stderr, "request_res: name:%s, addr:%s, count:%d, capacity:%d, rtimeout:%d, "\
            "wtimeout:%d\n", req_res->res_name, req_res->res_addr, (int)req_res->res_count,
            (int)req_res->res_capacity, (int)req_res->res_rtimeout, (int)req_res->res_wtimeout);

        ul_writelog(UL_LOG_DEBUG, "request_res: name:%s, addr:%s, count:%d, capacity:%d, "\
            "rtimeout:%d, wtimeout:%d\n", req_res->res_name, req_res->res_addr,
            (int)req_res->res_count, req_res->res_capacity, req_res->res_rtimeout, req_res->res_wtimeout);
        pthread_rwlock_unlock(&(req_res->res_rwlock));
    }
    pthread_rwlock_unlock(&m_req_res_rwlock);
    
    decr_refer_count();
}


void
RsrcPool :: show_register_resource()
{
    if(!incr_refer_count()){
        return;
    }

    pthread_mutex_lock(&m_reg_res_mutex);
    Lsc::list<reg_resource_t*>::iterator iter = m_reg_res_list->begin();
    for (; iter != m_reg_res_list->end(); ++iter){
        reg_resource_t *reg_res = *iter;
        fprintf(stderr, "register_res: res_addr:%s, res_data:%s, res_tag:%d\n", reg_res->res_addr,
            reg_res->res_data, reg_res->res_tag);

        ul_writelog(UL_LOG_DEBUG, "register_res: res_addr:%s, res_data:%s, res_tag:%d\n", reg_res->res_addr,
            reg_res->res_data, reg_res->res_tag);
    }
    pthread_mutex_unlock(&m_reg_res_mutex);

    decr_refer_count();
}




