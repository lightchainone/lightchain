

#include "lleo.h"
#include "lleo_list.h"




static void reset_lleo_handler( lleo_t *lleo_handler );


static int translate_lleo_error( int zookeeper_error );


static struct ACL* get_ACL_from_ACL_vector( const struct ACL_vector* acl_vec, const char* ip );


static int prepare_lleo_entity_path_prefix( const char *path, char *entity_path,
        const int entity_path_len );


static int get_lleo_tag_no_from_realpath( const char *realpath, int *tag_no );



static int get_parent_path_from_realpath( const char *realpath, char *tmppath );


static int lleo_get_ip_port(char *ip_port_list, const char *lleo_svr_list);

static int lleo_get_acl(struct ACL *acl, Lsc::var::Dict& acl_item, Lsc::ResourcePool &rp);

static int makeup_lleo_entity_path_by_tag_no( const char *path, const int tag_no,
        char *entity_path, const int entity_path_len );


static int makeup_lleo_entity_path_by_name( const char *path, const char *entity_name,
        char *entity_path, const int entity_path_len );



static int get_all_lleo_data_by_path( zhandle_t *zh, const char *path,
        Lsc::var::IVar& res_data,  int buf_size , Lsc::ResourcePool& rp);


static int set_lleo_acl_to_children( zhandle_t *zh, const char *path,
        const struct ACL_vector *acl_vec, const int buf_size );


static int handle_abstract_change(zhandle_t *zzh, const char *path, lleo_t* hdlr);


static int handle_entity_change(zhandle_t *zzh, const char *path, lleo_t* hdlr);


static int handle_children_event(zhandle_t *zzh, const char *path, lleo_t* hdlr);


static void default_zoo_watcher(zhandle_t *zzh, int type, int state, const char *path,
        void* lleo_handler);


void* check_zoo_handler(void *hdlr);



int refresh_watcher(lleo_t *lleo_handler);



int refresh_register(lleo_t *lleo_handler);


static int update_lleo_callback_table(lleo_t *lleo_handler,
        const char* path, void *cb, void *ctx);



static int delete_lleo_callback_table(lleo_t *lleo_handler, const char* path);


static int lleo_get(lleo_t *lleo_handler);


static int lleo_put(lleo_t *lleo_handler);

static int lleo_get_ref_cnt(lleo_t *lleo_handler);

static int lleo_set_ref_cnt(lleo_t *lleo_handler, int num);


static int default_max_retry_time = 0;
static int default_retry_interval = 1;



    void 
reset_lleo_handler( lleo_t * lleo_handler)
{
    if (NULL == lleo_handler){
        return;
    }

    lleo_handler->lleo_svr_list[0] = 0;
    lleo_handler->is_finish           = 0;
    lleo_handler->delay_watch_time    = 0;

    lleo_handler->magic_num      = GALILEO_MAGICNUM;
    lleo_handler->zh             = NULL;
    lleo_handler->pack_buff_size = GALILEO_DEFAULT_BUF_SIZE;
    lleo_handler->cb_table       = NULL;
    lleo_handler->watch_res_list = NULL;
    lleo_handler->regi_res_list  = NULL;
    lleo_handler->acl_vec.count  = 0;

    return;
}


    int 
translate_lleo_error( int zookeeper_error )
{
    int grc = GALILEO_E_OK;

    switch(zookeeper_error){
        case ZOK:
            grc = GALILEO_E_OK;
            break;
        case ZSYSTEMERROR:
            grc = GALILEO_E_SYSERROR;
            break;
        case ZRUNTIMEINCONSISTENCY:
            grc = GALILEO_E_RUNTIMEINCONSISTENCY;
            break;
        case ZDATAINCONSISTENCY:
            grc = GALILEO_E_DATAINCONSISTENCY;
            break;
        case ZCONNECTIONLOSS:
            grc = GALILEO_E_CONNLOSS;
            break;
        case ZMARSHALLINGERROR:
            grc = GALILEO_E_MARSHALLINGERROR;
            break;
        case ZUNIMPLEMENTED:
            grc = GALILEO_E_UNIMPLEMENTED;
            break;
        case ZOPERATIONTIMEOUT:
            grc = GALILEO_E_OPERATIONTIMEOUT;
            break;
        case ZBADARGUMENTS:
            grc = GALILEO_E_BADPARAM;
            break;
        case ZINVALIDSTATE:
            grc = GALILEO_E_INVALIDSTATE;
            break;
        case ZAPIERROR:
            grc = GALILEO_E_APIERROR;
            break;
        case ZNONODE:
            grc = GALILEO_E_NONODE;
            break;
        case ZNOAUTH:
            grc = GALILEO_E_NOAUTH;
            break;
        case ZBADVERSION:
            grc = GALILEO_E_BADVERSION;
            break;
        case ZNOCHILDRENFOREPHEMERALS:
            grc = GALILEO_E_CHILDFOREPHEM;
            break;
        case ZNODEEXISTS:
            grc = GALILEO_E_NODEEXISTS;
            break;
        case ZNOTEMPTY:
            grc = GALILEO_E_NOTEMPTY;
            break;
        case ZSESSIONEXPIRED:
            grc = GALILEO_E_SESSIONEXPIRED;
            break;
        case ZINVALIDCALLBACK:
            grc = GALILEO_E_INVALIDCALLBACK;
            break;
        case ZINVALIDACL:
            grc = GALILEO_E_INVALIDACL;
            break;
        case ZAUTHFAILED:
            grc = GALILEO_E_AUTHFAILED;
            break;
        case ZCLOSING:
            grc = GALILEO_E_CLOSING;
            break;
        case ZNOTHING:
            grc = GALILEO_E_NOTHING;
            break;
        default:
            grc = GALILEO_E_UNKNOWN;
            break;
    }

    return grc;
}


    struct ACL*
get_ACL_from_ACL_vector( const struct ACL_vector* acl_vec, const char* ip )
{

    if (NULL == acl_vec || NULL == ip){
        ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to get a valid acl_vec or ip",
                __FILE__, __LINE__);
        return NULL;
    }

    for(int i = 0; i < acl_vec->count; i++){
        struct ACL *pACL = &(acl_vec->data[i]);
        if (0 == strcmp(pACL->id.id, ip)){
            return pACL;
        }
    }
    return NULL;
}

    int
prepare_lleo_entity_path_prefix( const char *path, char *entity_path,
        const int entity_path_len)
{
    int  grc = GALILEO_E_OK;
    int  path_len = 0;
    char *pch = NULL;
    char *pch_end = NULL;
    char *pchen = NULL;

    GALILEO_POINTER_CHECK(NULL == path, "Failed to get a valid path");
    GALILEO_POINTER_CHECK(NULL == entity_path, "Failed to get a valid entity_path");

    snprintf(entity_path, entity_path_len, "%s", path);
    entity_path[entity_path_len-1] = '\0';
    path_len = strlen(entity_path);
    pchen = entity_path + (path_len - 1);

    
    if ('/' != *pchen){
        if (path_len == entity_path_len - 1){
            ul_writelog(UL_LOG_WARNING, "[%s:%d] The length of path is too long, only have:%d",
                    __FILE__, __LINE__, entity_path_len);
            return GALILEO_E_BADPARAM;
        }
        else{
            *(++pchen) = '/';
            *(++pchen) = '\0';
            path_len++;
        }
    }

    
    pch_end = pchen - 1; 
    pch = pch_end - 1;
    while ('/' != *pch && entity_path != pch){
        pch--;
    }

    
    if (entity_path == pch && '/' != *pch){
        ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to get a valid path:%s",
                __FILE__, __LINE__, path);
        return GALILEO_E_BADPARAM;
    }

    
    if (entity_path_len < path_len + (int)(pch_end - pch)){
        ul_writelog(UL_LOG_WARNING, "[%s:%d] Buffer length is not enough, need:%d, have:%d",
                __FILE__, __LINE__, path_len + (int)(pch_end - pch), entity_path_len);
        return GALILEO_E_BADPARAM;
    }

    ++pch;
    strncpy(pchen, pch, pch_end - pch);

    
    *(pchen + (pch_end - pch)) = '\0';

#ifdef GALILEO_DEBUG
    ul_writelog(UL_LOG_DEBUG, "input path:%s, output entity_path_len:%s", path, entity_path);
#endif

    return grc;
}


    int
get_lleo_tag_no_from_realpath( const char *realpath, int *tag_no )
{
    int i = 0;
    int grc = GALILEO_E_OK;
    const char *pch = realpath;
    const char *pch_end = realpath;

    GALILEO_POINTER_CHECK((NULL == realpath || 0 == strlen(pch)), "Failed to get a valid realpath");
    GALILEO_POINTER_CHECK(NULL == tag_no, "Failed to get a valid tag_no pointer");

    pch_end = pch_end + (strlen(pch) - 1);
    
    while (isdigit(*pch_end) && realpath != pch_end &&
            i < GALILEO_ENTITY_TAG_LEN)
    {
        i++;
        pch_end--;
    }

    if (realpath != pch_end){
        *tag_no = atoi(++pch_end);
    }
    else{
        grc = GALILEO_E_BADPARAM;
    }

#ifdef GALILEO_DEBUG
    ul_writelog(UL_LOG_DEBUG, "input realpath:%s, output tag_no:%d", realpath, *tag_no);
#endif

    return grc;
}


    int 
get_parent_path_from_realpath( const char *realpath, char *tmppath )
{
    int grc = GALILEO_E_OK;
    const char *pch = realpath;
    const char *pch_end = realpath;

    GALILEO_POINTER_CHECK((NULL == realpath || 0 == strlen(pch)),
            "Failed to get a valid realpath");

    pch_end = pch_end + (strlen(pch) - 1);
    while (realpath != pch_end && *pch_end != '/')
    {
        pch_end--;
    }

    strncpy(tmppath, realpath, pch_end-realpath);
    tmppath[pch_end-realpath] = 0;

    return grc; 
}


    int 
makeup_lleo_entity_path_by_tag_no( const char *path, const int tag_no,
        char *entity_path, const int entity_path_len )
{
    int  i = 0;
    int  grc = GALILEO_E_OK;
    int  path_len = 0;
    char *pch = NULL;

    GALILEO_POINTER_CHECK(NULL == path, "Failed to get a valid path");
    GALILEO_POINTER_CHECK(NULL == entity_path, "Failed to get a valid entity_path");

    grc = prepare_lleo_entity_path_prefix(path, entity_path, entity_path_len);
    if (ZOK != grc){
        return grc;
    }

    path_len = strlen(entity_path);
    if (entity_path_len < path_len + GALILEO_ENTITY_TAG_LEN + 1){
        ul_writelog(UL_LOG_WARNING, "[%s:%d] insufficient sapce for entity path, need:%d, have:%d",
                __FILE__, __LINE__, path_len + GALILEO_ENTITY_TAG_LEN + 1, entity_path_len);
        return GALILEO_E_BADPARAM;
    }

    pch = entity_path + strlen(entity_path);
    snprintf(pch, GALILEO_ENTITY_TAG_LEN + 1, "%10d", tag_no);
    for (i = 0; i < GALILEO_ENTITY_TAG_LEN; i++){
        
        *(pch+i) = (' '==*(pch+i))?('0'):(*(pch+i));
    }

#ifdef GALILEO_DEBUG
    ul_writelog(UL_LOG_DEBUG, "input realpath:%s, tag_no:%d, output entity_path:%s",
            path, tag_no, entity_path);
#endif

    return grc;
}


    int 
makeup_lleo_entity_path_by_name( const char *path, const char *entity_name,
        char *entity_path, const int entity_path_len )
{
    int grc = GALILEO_E_OK;
    int path_len = 0;
    char *pch_end = NULL;

    GALILEO_POINTER_CHECK(NULL == path, "Failed to get a valid path");
    GALILEO_POINTER_CHECK(NULL == entity_name, "Failed to get valid entity name");
    GALILEO_POINTER_CHECK(NULL == entity_path, "Failed to get valid entity path");

    snprintf(entity_path, entity_path_len, "%s", path);
    entity_path[entity_path_len - 1] = '\0';
    path_len = strlen(entity_path);
    pch_end = entity_path + (path_len - 1);

    
    if ('/' != *pch_end){
        if (path_len == entity_path_len - 1){
            ul_writelog(UL_LOG_WARNING, "[%s:%d] The length of path is too long, only have:%d",
                __FILE__, __LINE__, entity_path_len);
            return GALILEO_E_BADPARAM;
        }
        else{
            *(++pch_end) = '/';
            *(++pch_end) = '\0';
            path_len++;
        }
    }

    if ((int)strlen(entity_name) > entity_path_len-path_len-1){
        ul_writelog(UL_LOG_WARNING, "[%s:%d] Buffer length is not enough, need:%d, have:%d",
            __FILE__, __LINE__, (int)strlen(entity_name), entity_path_len-path_len-1);
        return GALILEO_E_BADPARAM;
    }
    snprintf(pch_end, entity_path_len-path_len-1, "%s", entity_name);

#ifdef GALILEO_DEBUG
    ul_writelog(UL_LOG_DEBUG, "input path:%s, entity_name:%s, output entity_path:%s",
        path, entity_name, entity_path);
#endif

    return grc;
}


    int 
get_all_lleo_data_by_path( zhandle_t *zh, const char *path,
        Lsc::var::IVar& res_data, int buf_size, Lsc::ResourcePool& rp)
{
    int zrc = ZOK;
    int grc = GALILEO_E_OK;
    int retry_time = 0;
    bool retry = true;
    int live_threshold = 0;

    GALILEO_POINTER_CHECK(NULL == zh, "Failed to get a valid zookeeper handler");
    GALILEO_POINTER_CHECK(NULL == path, "Failed to get a valid path");
    GALILEO_POINTER_CHECK(!res_data.is_dict(), "Failed to get a valid data dict");

    while (retry) {
        retry = false;
        ++retry_time;
        
        Lsc::ResourcePool tmpRp;
        char *buff = NULL;
        try{
            buff = (char*)tmpRp.create_raw(buf_size);
        }
        catch(...){
            ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to create buff, buf_size:%d",
                    __FILE__, __LINE__, buf_size);
            return GALILEO_E_APIERROR;
        }

        
        struct Stat stat;
        int buff_len = buf_size;
        Lsc::var::Array& acl_array = rp.create<Lsc::var::Array>();

        zrc = zoo_get(zh, path, 0, buff, &buff_len , &stat);
        GALILEO_ZERROR_CHECK(zrc, "Failed to get data", path);
        GALILEO_INT_CHECK(!buff_len, "Failed to get abstract data", buff_len);

        struct ACL_vector acl_vec = {0, NULL};
        zrc = zoo_get_acl(zh, path, &acl_vec, &stat);
        GALILEO_ZERROR_CHECK(zrc, "Failed to get acl", path);

        try{
            for (int i = 0; i < acl_vec.count; i++){
                struct ACL *acl = &(acl_vec.data[i]);
                
                Lsc::var::Dict& acl_item = rp.create<Lsc::var::Dict>();
                grc = lleo_get_acl(acl, acl_item, rp);
                if (GALILEO_E_OK != grc) {
                    ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to get acl", __FILE__, __LINE__);
                    break;
                }
                acl_array.set(i, acl_item);
            }
        }
        catch(...){
            ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to create acl", __FILE__, __LINE__);
            deallocate_ACL_vector(&acl_vec);
            return GALILEO_E_APIERROR;
        }
        deallocate_ACL_vector(&acl_vec);

        try{
            Lsc::var::JsonDeserializer jd(rp);
            res_data[GALILEO_ABSTRACT] = jd.deserialize(buff);
            res_data[GALILEO_ABSTRACT][GALILEO_ACL_NAME] = acl_array;
            if (!res_data[GALILEO_ABSTRACT][GALILEO_THRESHOLD].is_null()) {
                live_threshold = res_data[GALILEO_ABSTRACT][GALILEO_THRESHOLD].to_int32();
            }
        }
        catch(...){
            
            try{
                Lsc::var::CompackDeserializer mpd(rp);
                res_data[GALILEO_ABSTRACT] = mpd.deserialize(buff, buff_len);
                res_data[GALILEO_ABSTRACT][GALILEO_ACL_NAME] = acl_array;
            }
            catch(...){
                ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to deserialize node data, path:%s",
                        __FILE__, __LINE__, path);
                return GALILEO_E_MARSHALLINGERROR;
            }
        }

        
        Lsc::var::Dict& enti_dict = rp.create<Lsc::var::Dict>();
        res_data[GALILEO_ENTITY] = enti_dict;

        struct String_vector str_vec = {0, NULL};
        zrc = zoo_get_children(zh, path, 0, &str_vec);
        if (ZOK != zrc){
            deallocate_String_vector(&str_vec);
            GALILEO_ZERROR_CHECK(zrc, "Failed to get children", path);
        }

        char tmppath[GALILEO_DEFAULT_PATH_LEN];
        int fail_num = 0;
        for (int i = 0; i < str_vec.count; i++){
            tmppath[0] = '\0';

            grc = makeup_lleo_entity_path_by_name(path, str_vec.data[i],
                    tmppath, GALILEO_DEFAULT_PATH_LEN);
            if (GALILEO_E_OK != grc){
                deallocate_String_vector(&str_vec);
                GALILEO_INT_CHECK(GALILEO_E_OK != grc,
                        "Failed to make up entity path by name", grc);
            }

            buff_len = buf_size;
            zrc = zoo_get(zh, tmppath, 0, buff, &buff_len, &stat);
            if (ZOK != zrc || !buff_len){
                ++fail_num;

                
                continue;
            }
            
            try{
                Lsc::var::JsonDeserializer jd(rp);
                enti_dict[str_vec.data[i]] = jd.deserialize(buff);
            }
            catch(...){
                
                try{
                    Lsc::var::CompackDeserializer mpd(rp);
                    enti_dict[str_vec.data[i]] = mpd.deserialize(buff, buff_len);
                }
                catch(...){
                    ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to deserialize node data,"\
                            " path:%s", __FILE__, __LINE__, tmppath);
                    break;
                }
            }
        }

        int total_num = str_vec.count;
        deallocate_String_vector(&str_vec);

		if (fail_num > 0 || total_num == 0) {
            if (retry_time < default_max_retry_time) {
                retry = true;
                sleep(retry_time * default_retry_interval);
                continue;
            } else if (total_num < live_threshold + fail_num) {
                ul_writelog(UL_LOG_FATAL, "[%s:%d] Failed to get node data,"\
                        " path:%s, suc: %d, fail: %d, retry: %d", 
                        __FILE__, __LINE__, path, total_num-fail_num, fail_num, retry_time);
            } else {
                ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to get node data,"\
                        " path:%s, suc: %d, fail: %d, retry: %d", 
                        __FILE__, __LINE__, path, total_num-fail_num, fail_num, retry_time);
            }
        }
    }
    return grc;
}


    int 
set_lleo_acl_to_children( zhandle_t *zzh, const char *path,
        const struct ACL_vector *acl_vec, const int buf_size)
{
    int zrc = ZOK;
    int grc = GALILEO_E_OK;
    Lsc::ResourcePool rp;

    GALILEO_POINTER_CHECK(NULL == zzh, "Failed to get a valid zookeeper handler");
    GALILEO_POINTER_CHECK(NULL == path, "Failed to get a valid path");
    GALILEO_POINTER_CHECK(NULL == acl_vec, "Failed to get a valid acl struct");

    
    char* buff = NULL;
    try{
        buff = (char*)rp.create_raw(buf_size);
    }
    catch(...){
        ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to create buff:", __FILE__, __LINE__);
        return GALILEO_E_APIERROR;
    }

    struct String_vector str_vec = {0, NULL};
    zrc = zoo_get_children(zzh, path, 0, &str_vec);
    if (ZOK != zrc){
        deallocate_String_vector(&str_vec);
        GALILEO_ZERROR_CHECK(zrc, "Failed to get children", path);
    }

    for (int i = 0; i < str_vec.count; i++){
        char tmppath[GALILEO_DEFAULT_PATH_LEN];
        tmppath[0] = '\0';

        grc = makeup_lleo_entity_path_by_name(path, str_vec.data[i],
                tmppath, GALILEO_DEFAULT_PATH_LEN);
        if (GALILEO_E_OK != grc)break;

        struct Stat stat;
        int buff_len = buf_size;
        zrc = zoo_get(zzh, tmppath, 0, buff, &buff_len, &stat);
        if (ZOK != zrc || !buff_len){
            deallocate_String_vector(&str_vec);
            GALILEO_ZERROR_CHECK(zrc, "Failed to get data", tmppath);
        }

        if (0 != stat.ephemeralOwner){
            
            const char *ip = NULL;
            try{
                Lsc::var::JsonDeserializer jd(rp);
                Lsc::var::IVar& json_var = jd.deserialize(buff);

                if(!json_var.is_dict() || json_var.get(GALILEO_ENTITY_IP).is_null()){
                    ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to get a valid dict",
                            __FILE__, __LINE__);
                    zrc = zoo_set_acl(zzh, tmppath, -1, acl_vec);
                    ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to set acl to path:%s, rc%d, "\
                            "err:%s", __FILE__, __LINE__, tmppath, zrc, lleo_err2str(zrc));
                }
                ip = json_var.get(GALILEO_ENTITY_IP).c_str();
            }
            catch(...){
                
                ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to deserialize node data, path:%s"
                        , __FILE__, __LINE__, tmppath);
                zrc = zoo_set_acl(zzh, tmppath, -1, acl_vec);
                ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to set acl to path:%s, rc%d, "\
                        "err:%s", __FILE__, __LINE__, tmppath, zrc, lleo_err2str(zrc));
                continue;
            }

            struct ACL_vector en_acl_vec = {0, NULL};
            char myscheme[GALILEO_ACL_SCHEME_LEN];
            char myid[GALILEO_ACL_ID_LEN];

            if (NULL == get_ACL_from_ACL_vector(acl_vec, ip)){
                en_acl_vec.count = acl_vec->count + 1;
                try{
                    struct ACL *pACL = (struct ACL*)rp.create_raw(
                            en_acl_vec.count*sizeof(struct ACL));
                    memcpy(pACL, acl_vec->data, acl_vec->count * sizeof(struct ACL));
                    en_acl_vec.data = pACL;
                }
                catch(...){
                    ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to create buff",
                            __FILE__, __LINE__);
                    continue;
                }

                en_acl_vec.data[acl_vec->count].perms = ZOO_PERM_ALL;
                en_acl_vec.data[acl_vec->count].id.scheme = myscheme;
                snprintf(myscheme, GALILEO_ACL_SCHEME_LEN, "%s", "ip");
                en_acl_vec.data[acl_vec->count].id.id = myid;
                snprintf(myid, GALILEO_ACL_ID_LEN, "%s", ip);
            }
            else{
                en_acl_vec.count = acl_vec->count;
                try{
                    struct ACL *pACL = (struct ACL*)rp.create_raw(
                            en_acl_vec.count*sizeof(struct ACL));
                    memcpy(pACL, acl_vec->data, acl_vec->count * sizeof(struct ACL));
                    en_acl_vec.data = pACL;
                }
                catch(...){
                    ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to create buff",
                            __FILE__, __LINE__);
                    continue;
                }
            }

            
            zrc = zoo_set_acl(zzh, tmppath, -1, &en_acl_vec);
            ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to set acl to path:%s, rc%d, "\
                    "err:%s", __FILE__, __LINE__, tmppath, zrc, lleo_err2str(zrc));
        }
    }

    deallocate_String_vector(&str_vec);
    return grc;
}


    int 
handle_abstract_change(zhandle_t *zzh, const char *path, lleo_t* hdlr)
{
    int grc = GALILEO_E_OK;
    int gtype = GALILEO_ABSTRACT_CHANGED_EVENT;
    char tmppath[GALILEO_DEFAULT_PATH_LEN];
    snprintf(tmppath, GALILEO_DEFAULT_PATH_LEN, "%s", path);

    
    pthread_rwlock_rdlock(&(hdlr->cb_rwlock));
    cb_item_t *cb_item = (cb_item_t*)myhashtable_search(hdlr->cb_table, tmppath);
    if (NULL == cb_item){
    	pthread_rwlock_unlock(&(hdlr->cb_rwlock));
        ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to find callback which is"\
                " associated with the path:%s", __FILE__, __LINE__, tmppath);
        return GALILEO_E_BADPARAM;
    }

    lleo_proc_callback custom_callback = (lleo_proc_callback)(cb_item->cb_func);
	void *cb_ctx = cb_item->cb_ctx;
    pthread_rwlock_unlock(&(hdlr->cb_rwlock));
    

    struct Stat stat;
    Lsc::ResourcePool rp;
    int buff_len = hdlr->pack_buff_size;
    Lsc::var::Array& acl_array = rp.create<Lsc::var::Array>();
    char *buff = NULL;
    try{
        buff = (char*)rp.create_raw(buff_len);
    }
    catch(...){
        ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to create buff", __FILE__, __LINE__);
        return GALILEO_E_APIERROR;
    }
    int zrc = zoo_wget(zzh, path, default_zoo_watcher, hdlr, buff, &buff_len, &stat);
    GALILEO_ZERROR_CHECK(zrc, "Failed to get data", path);
    GALILEO_INT_CHECK(!buff_len, "Failed to get data", buff_len);

    struct ACL_vector acl_vec = {0, NULL};
    zrc = zoo_get_acl(zzh, path, &acl_vec, &stat);
    GALILEO_ZERROR_CHECK(zrc, "Failed to get acl", path);

    try{
        for (int i = 0; i < acl_vec.count; i++){
            struct ACL *acl = &(acl_vec.data[i]);
            
            Lsc::var::Dict& acl_item = rp.create<Lsc::var::Dict>();
            grc = lleo_get_acl(acl, acl_item, rp);
            if (GALILEO_E_OK != grc) {
                ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to get acl", __FILE__, __LINE__);
                break;
            }
            acl_array.set(i, acl_item);
        }
    }
    catch(...){
        ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to create acl", __FILE__, __LINE__);
        deallocate_ACL_vector(&acl_vec);
        return GALILEO_E_APIERROR;
    }

    try{
        Lsc::var::JsonDeserializer jd(rp);
        Lsc::var::IVar& json_var = jd.deserialize(buff);
        
        json_var[GALILEO_ACL_NAME] = acl_array;
        custom_callback(gtype, path, json_var, cb_ctx);
    }
    catch(...){
        
        try{
            Lsc::var::CompackDeserializer mpd(rp);
            Lsc::var::IVar& mcpack_var = mpd.deserialize(buff, buff_len);
            
            mcpack_var[GALILEO_ACL_NAME] = acl_array;
            custom_callback(gtype, path, mcpack_var, cb_ctx);
        }
        catch(...){
            ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to deserialize node data, path:%s"
                    , __FILE__, __LINE__, path);
            deallocate_ACL_vector(&acl_vec);
            return GALILEO_E_BADPARAM;
        }
    }

    deallocate_ACL_vector(&acl_vec);
    return GALILEO_E_OK;
}

Lsc::var::IVar * lleo_get_acl_from_data(Lsc::var::IVar& data)
{
    return &data.get(GALILEO_ACL_NAME);
}

    int
handle_entity_change(zhandle_t *zzh, const char *path, lleo_t* hdlr){

    int grc = GALILEO_E_OK;
    int gtype = GALILEO_ENTITY_CHANGED_EVENT;
    char tmppath[GALILEO_DEFAULT_PATH_LEN];
    snprintf(tmppath, GALILEO_DEFAULT_PATH_LEN, "%s", path);

    
    pthread_rwlock_rdlock(&(hdlr->cb_rwlock));
    cb_item_t *cb_item = (cb_item_t*)myhashtable_search(hdlr->cb_table, tmppath);
    if (NULL == cb_item){
    	pthread_rwlock_unlock(&(hdlr->cb_rwlock));
        ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to find callback which"\
                " is associated with the path:%s", __FILE__, __LINE__, tmppath);
        return GALILEO_E_BADPARAM;
    }

    lleo_proc_callback custom_callback = (lleo_proc_callback)(cb_item->cb_func);
	void *cb_ctx = cb_item->cb_ctx;
    pthread_rwlock_unlock(&(hdlr->cb_rwlock));
    

    struct Stat stat;
    Lsc::ResourcePool rp;
    int buff_len = hdlr->pack_buff_size;
    char *buff = NULL;
    try{
        buff = (char*)rp.create_raw(buff_len);
    }
    catch(...){
        ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to create buff" , __FILE__, __LINE__);
        return GALILEO_E_APIERROR;
    }
    int zrc = zoo_wget(zzh, path, default_zoo_watcher, hdlr, buff, &buff_len, &stat);
    GALILEO_ZERROR_CHECK(zrc, "Failed to get data", path);
    GALILEO_INT_CHECK(!buff_len, "Failed to get data", buff_len);

    try{
        Lsc::var::JsonDeserializer jd(rp);
        Lsc::var::IVar& json_var = jd.deserialize(buff);
        
        custom_callback(gtype, path, json_var, cb_ctx);
    }
    catch(...){
        try{
            Lsc::var::CompackDeserializer mpd(rp);
            Lsc::var::IVar& mcpack_var = mpd.deserialize(buff, buff_len);
            
            custom_callback(gtype, path, mcpack_var, cb_ctx);
        }
        catch(...){
            ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to deserialize node data, path:%s"
                    , __FILE__, __LINE__, path);
            return GALILEO_E_BADPARAM;
        }
    }

    return GALILEO_E_OK;
}


    int
handle_children_event(zhandle_t *zzh, const char *path, lleo_t* hdlr)
{
    int grc = GALILEO_E_OK;
    
    struct String_vector str_vec = {0, NULL};
    int zrc = zoo_wget_children(zzh, path, default_zoo_watcher, (void*)hdlr, &str_vec);
    GALILEO_ZERROR_CHECK(zrc, "Failed to get data", path);

    int gtype = GALILEO_RESOURCE_UPDATE_EVENT;
    
    char tmppath[GALILEO_DEFAULT_PATH_LEN];
    snprintf(tmppath, GALILEO_DEFAULT_PATH_LEN, "%s", path);

    pthread_rwlock_rdlock(&(hdlr->cb_rwlock));
    cb_item_t* cb_item = (cb_item_t*)myhashtable_search(hdlr->cb_table, tmppath);
    if (NULL == cb_item){
    	pthread_rwlock_unlock(&(hdlr->cb_rwlock));
        ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to find callback which is associated"\
                "with the path [%s]", __FILE__, __LINE__, tmppath);
        deallocate_String_vector(&str_vec);
        return GALILEO_E_BADPARAM;
    }

    lleo_proc_callback custom_callback = (lleo_proc_callback)(cb_item->cb_func);
	void *cb_ctx = cb_item->cb_ctx;
    pthread_rwlock_unlock(&(hdlr->cb_rwlock));

    
    Lsc::ResourcePool rp;
    Lsc::var::Dict& dict_var = rp.create<Lsc::var::Dict>();
    grc = get_all_lleo_data_by_path(zzh, path, dict_var, hdlr->pack_buff_size, rp);
    if (GALILEO_E_OK != grc){
        deallocate_String_vector(&str_vec);
        return GALILEO_E_APIERROR;
    }

    
    custom_callback(gtype, path, dict_var, cb_ctx);

    
    for (int i = 0; i < str_vec.count; i++){
        tmppath[0] = '\0';
        grc = makeup_lleo_entity_path_by_name(path, str_vec.data[i],
                tmppath, GALILEO_DEFAULT_PATH_LEN);
        if (GALILEO_E_OK != grc){
            deallocate_String_vector(&str_vec);
            GALILEO_INT_CHECK(GALILEO_E_OK != grc,
                    "Failed to make up entity path by name", grc);
        }

#ifdef GALILEO_DEBUG
        ul_writelog(UL_LOG_DEBUG, "NO.[%d], path [%s]", i, tmppath);
#endif
        grc = update_lleo_callback_table(hdlr, tmppath,
                (void *)custom_callback, cb_ctx);
        if (GALILEO_E_OK != grc){
            deallocate_String_vector(&str_vec);
            GALILEO_INT_CHECK(GALILEO_E_OK != grc,
                    "Failed to update lleo callback table", grc);
        }

        struct Stat stat;
        int buff_len = hdlr->pack_buff_size;
        try{
            char* buff = (char*)rp.create_raw(buff_len);
            zrc = zoo_wget(hdlr->zh, tmppath, default_zoo_watcher, (void*)hdlr,
                    buff, &buff_len, &stat);
            if (ZOK != zrc || !buff_len){
                deallocate_String_vector(&str_vec);
                GALILEO_ZERROR_CHECK(zrc, "Failed to get data", tmppath);
            }
        }
        catch(...){
            ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to create buff",
                    __FILE__, __LINE__);
            grc = GALILEO_E_APIERROR;
            break;
        }
    }

    deallocate_String_vector(&str_vec);
    return grc;
}


    void 
default_zoo_watcher(zhandle_t *zzh, int type, int state, const char *path,
        void* lleo_handler)
{
    int zrc = ZOK;
    int grc = GALILEO_E_OK;
    lleo_t *hdlr = (lleo_t*)lleo_handler;
    Lsc::ResourcePool rp;
    ul_logstat_t admin_log_stat = {UL_LOG_NOTICE, 0, 0};
    ul_openlog_r("watcher_callback", &admin_log_stat);

    if (NULL == zzh || NULL == path || NULL == lleo_handler){
        ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to get some watcher parameters",
                __FILE__, __LINE__);
        ul_closelog_r(0);
        return;
    }

    if (lleo_get(hdlr) <= 0) {
        ul_writelog(UL_LOG_WARNING, "zookeeper is reinitializing...");
        ul_closelog_r(0);
        return;
    }
#ifdef GALILEO_DEBUG
    ul_writelog(UL_LOG_DEBUG, "watch type:[%d], state:[%d], path[%s]", type, state, path);
#endif

    
    if (ZOO_NOTWATCHING_EVENT == type){
        
        ul_closelog_r(0);
        lleo_put(hdlr);
        return;
    }
    else if (ZOO_SESSION_EVENT == type){
        if(ZOO_CONNECTED_STATE == state){
            
        }
        else if(ZOO_EXPIRED_SESSION_STATE == state){
            
            ul_writelog(UL_LOG_WARNING, "[%s:%d] the session expired, reconnect...",
                    __FILE__, __LINE__);
            
            lleo_put(hdlr);
            
            pthread_mutex_lock(&(hdlr->zh_lock));
            pthread_cond_signal(&(hdlr->zh_cond));
            pthread_mutex_unlock(&(hdlr->zh_lock));
        }
        else if(0 == state){
            
            ul_writelog(UL_LOG_WARNING, "[%s:%d] the connection with zook servers is"\
                    "temporarily unavailable", __FILE__, __LINE__);
        }
        ul_closelog_r(0);
        lleo_put(hdlr);
        return;
    }

    
    struct Stat stat;
    int buff_len = hdlr->pack_buff_size;
    char* buff = NULL;
    try{
        buff = (char*)rp.create_raw(hdlr->pack_buff_size);
    }
    catch(...){
        ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to create buff", __FILE__, __LINE__);
        ul_closelog_r(0);
        lleo_put(hdlr);
        return;
    }
    zrc = zoo_get(hdlr->zh, path, 0, buff, &buff_len, &stat);
  
    
    if (ZOO_CREATED_EVENT == type){
        
        ul_writelog(UL_LOG_WARNING, "[%s:%d] maybe get a error event: type[%d]",
            __FILE__, __LINE__, type);
    }
    else if (ZOO_DELETED_EVENT == type){
        
    }
    else if (ZOO_CHANGED_EVENT == type){
        

        if (0 == stat.ephemeralOwner){
            grc = handle_abstract_change(zzh, path, hdlr);
        }
        else{
            grc = handle_entity_change(zzh, path, hdlr);
        }
    }
    else if (ZOO_CHILD_EVENT == type){
        grc = handle_children_event(zzh, path, hdlr);
    }

    ul_closelog_r(0);
    lleo_put(hdlr);
    return;
}


    void*
check_zoo_handler(void *hldr)
{
    int ret = 0;
    int grc = GALILEO_E_OK;
    int retry_time = 0;

    if (NULL == hldr){
        ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to get a valid lleo_handler",
                __FILE__, __LINE__);
        return NULL;
    }
    
    lleo_t *lleo_handler = (lleo_t*)hldr;
    ul_logstat_t admin_log_stat = {UL_LOG_NOTICE, 0, 0};
    ul_openlog_r("check_zoo_handler", &admin_log_stat);

    while(!(lleo_handler->is_finish)){
        pthread_mutex_lock(&(lleo_handler->zh_lock));
        
        
        if (lleo_handler->zh) {
            pthread_cond_wait(&(lleo_handler->zh_cond), &(lleo_handler->zh_lock));
        }

        if (!lleo_handler->is_finish){
            
            ul_writelog(UL_LOG_WARNING, "[%s:%d] zookeeper session expired, reconnect...",
                    __FILE__, __LINE__);

            while (lleo_get_ref_cnt(lleo_handler) != 0) sleep(1);
            lleo_handler->zh = zookeeper_init(lleo_handler->lleo_svr_list,
                    default_zoo_watcher, lleo_handler->sesn_expi_time, 0,
                    (void*)lleo_handler, 0);
            if (NULL == lleo_handler->zh){
                ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to re-initialize zookeeper handler,"
                    " Retry in %d seconds...", __FILE__, __LINE__, GALILEO_DEFAULT_REWORK_TIME);
                pthread_mutex_unlock(&(lleo_handler->zh_lock));
                lleo_put(lleo_handler);
                sleep(GALILEO_DEFAULT_REWORK_TIME);
                continue;
            }
            lleo_set_ref_cnt(lleo_handler, 1);


            
            retry_time = 0;
            while (ZOO_CONNECTED_STATE != zoo_state(lleo_handler->zh) && 
                    ++retry_time < MAX_RETRY_TIMES) {
                sleep(1);
            }

            if (retry_time >= MAX_RETRY_TIMES) {
                lleo_put(lleo_handler);
                ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to re-initialize zookeeper handler,"
                    " Retry in %d seconds...", __FILE__, __LINE__, GALILEO_DEFAULT_REWORK_TIME);
                pthread_mutex_unlock(&(lleo_handler->zh_lock));
                sleep(GALILEO_DEFAULT_REWORK_TIME);
                continue;
            }

            ul_writelog(UL_LOG_WARNING, "[%s:%d] Success to re-initialize zookeeper handler",
                    __FILE__, __LINE__);
            
            int count = 0;
            for (; count < lleo_handler->acl_vec.count; ++count) {
                struct Id *id = &lleo_handler->acl_vec.data[count].id;
                ret = zoo_add_auth(lleo_handler->zh, id->scheme, id->id, strlen(id->id), NULL, NULL);
                if (ZOK != ret){
                    grc = translate_lleo_error(ret);
                    ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to add auth[%s], rc:%d, error:%s,"
                        " Retry in %d seconds...", __FILE__, __LINE__, lleo_handler->lleo_svr_list,
                        grc, lleo_err2str(grc), GALILEO_DEFAULT_REWORK_TIME);
                    break;
                }
            }

            if (count != lleo_handler->acl_vec.count){
                lleo_put(lleo_handler);
                pthread_mutex_unlock(&(lleo_handler->zh_lock));
                sleep(GALILEO_DEFAULT_REWORK_TIME);
                continue;
            }

            ret = refresh_register(lleo_handler);
            if (0 != ret){
                ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to refresh zookeeper register,"
                    " Retry in %d seconds...", __FILE__, __LINE__, GALILEO_DEFAULT_REWORK_TIME);
                lleo_put(lleo_handler);
                pthread_mutex_unlock(&(lleo_handler->zh_lock));
                sleep(GALILEO_DEFAULT_REWORK_TIME);
                continue;
            }

            sleep(lleo_handler->delay_watch_time);
            ret = refresh_watcher(lleo_handler);
            if (0 != ret){
                ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to refresh zookeeper watcher,"
                    "Retry in %d seconds...", __FILE__, __LINE__, GALILEO_DEFAULT_REWORK_TIME);
                lleo_put(lleo_handler);
                pthread_mutex_unlock(&(lleo_handler->zh_lock));
                sleep(GALILEO_DEFAULT_REWORK_TIME);
                continue;
            }
        }
        pthread_mutex_unlock(&(lleo_handler->zh_lock));
    }

    ul_closelog_r(0);
    return NULL;
}


    int 
refresh_watcher(lleo_t *lleo_handler)
{
    int zrc = 0;
    int grc = GALILEO_E_OK;
    struct Stat stat;
    struct String_vector str_vec = {0, NULL};
    Lsc::ResourcePool rp;

    GALILEO_POINTER_CHECK(NULL == lleo_handler, "Failed to get a valid lleo_handler");

    char* buff = NULL;
    try{
        buff = (char*)rp.create_raw(lleo_handler->pack_buff_size);
    }
    catch(...){
        ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to create buff", __FILE__, __LINE__);
        return GALILEO_E_APIERROR; 
    }

    lleo_list_node* node = lleo_handler->watch_res_list->next;
    
    while(node != NULL){
        
        while (1){
            int buff_len = lleo_handler->pack_buff_size;
            zrc = zoo_wget(lleo_handler->zh, node->path, default_zoo_watcher,
                    (void*)lleo_handler, buff, &buff_len, &stat);

            if (0 == zrc){
                
                default_zoo_watcher(lleo_handler->zh, ZOO_CHANGED_EVENT,
                        zoo_state(lleo_handler->zh), node->path, lleo_handler);
                break;
            }
            else if (ZNONODE != zrc && ZINVALIDSTATE != zrc && ZCONNECTIONLOSS != zrc && ZSESSIONEXPIRED != zrc){
                grc = translate_lleo_error(zrc);
                ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to refresh data watch for path:%s,"\
                        " err:%s", __FILE__, __LINE__, node->path, lleo_err2str(grc));
                sleep(GALILEO_DEFAULT_REWORK_TIME);
            }
            else{
                
                grc = translate_lleo_error(zrc);
                return grc;
            }
        }

        
        while(1){
            zrc = zoo_wget_children(lleo_handler->zh, node->path, default_zoo_watcher,
                    (void*)lleo_handler, &str_vec);

            if (0 == zrc){
                
                default_zoo_watcher(lleo_handler->zh, ZOO_CHILD_EVENT,
                        zoo_state(lleo_handler->zh), node->path, lleo_handler);
                break;
            }
            else if (ZNONODE != zrc && ZSESSIONEXPIRED != zrc && ZINVALIDSTATE != zrc){
                grc = translate_lleo_error(zrc);
                ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to refresh child watch for path:%s,"\
                        " err:%s", __FILE__, __LINE__, node->path, lleo_err2str(grc));
                sleep(GALILEO_DEFAULT_REWORK_TIME);
            }
            else{
                
                grc = translate_lleo_error(zrc);
                deallocate_String_vector(&str_vec);
                return grc;
            }
        }

        
        for (int i = 0; i < str_vec.count; i++){
            char tmppath[GALILEO_DEFAULT_PATH_LEN];
            tmppath[0] = '\0';
            grc = makeup_lleo_entity_path_by_name(node->path, str_vec.data[i],
                    tmppath, GALILEO_DEFAULT_PATH_LEN);
            if (GALILEO_E_OK != grc){
                ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to make up path:%s, node:%s",
                        __FILE__, __LINE__, node->path, str_vec.data[i]);
                continue;
            }

            while(1){
                int buff_len = lleo_handler->pack_buff_size;
                zrc = zoo_wget(lleo_handler->zh, tmppath, default_zoo_watcher,
                        (void*)lleo_handler, buff, &buff_len, &stat);

                if (0 == zrc || !buff_len){
                    
                    default_zoo_watcher(lleo_handler->zh, ZOO_CHANGED_EVENT,
                            zoo_state(lleo_handler->zh), tmppath, lleo_handler);
                    break;
                }
                else if (ZNONODE != zrc && ZSESSIONEXPIRED != zrc){
                    grc = translate_lleo_error(zrc);
                    ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to refresh data watch for"\
                            " path:%s, err:%s", __FILE__, __LINE__, node->path,
                            lleo_err2str(grc));
                    sleep(GALILEO_DEFAULT_REWORK_TIME);
                }
                else{
                    grc = translate_lleo_error(zrc);
                    
                    deallocate_String_vector(&str_vec);
                    return grc;
                }
            }
        }

        ul_writelog(UL_LOG_NOTICE, "[%s:%d] Refresh watch for resource:%s successfully",
                __FILE__, __LINE__, node->path);

        node = node->next;
    }

    deallocate_String_vector(&str_vec);
    return grc;
}


    int
refresh_register(lleo_t *lleo_handler)
{
    int zrc = ZOK;
    int grc = GALILEO_E_OK;
    Lsc::ResourcePool rp;

    GALILEO_POINTER_CHECK(NULL == lleo_handler, "Failed to get a valid lleo_handler");

    char* node_buff = NULL;
    try{
        node_buff = (char*)rp.create_raw(lleo_handler->pack_buff_size);
    }
    catch(...){
        ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to create buff", __FILE__, __LINE__);
        return GALILEO_E_APIERROR;
    }

    lleo_list_node* node = lleo_handler->regi_res_list->next;
    while(node != NULL){
        
        struct ACL_vector acl_vec = {0, NULL};
        char tmppath[GALILEO_DEFAULT_PATH_LEN];
        grc = get_parent_path_from_realpath(node->path, tmppath);

        while(1){
            struct Stat stat;
            zrc = zoo_get_acl(lleo_handler->zh, tmppath, &acl_vec, &stat);
            if (0 != zrc && ZNONODE != zrc && ZINVALIDSTATE != zrc){
                grc = translate_lleo_error(zrc);
                ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to refresh child watch for"\
                        " path[%s], err[%s]", __FILE__, __LINE__,
                        node->path, lleo_err2str(grc));
                sleep(GALILEO_DEFAULT_REWORK_TIME);
            }
            else{
                
                break;
            }
        }

        
        char myid[GALILEO_ACL_ID_LEN];
        char myscheme[GALILEO_ACL_SCHEME_LEN];
        struct ACL_vector en_acl_vec = {0, NULL};
        struct ACL* pACL = NULL;

        if (0 == get_local_conn_ip(myid, GALILEO_ACL_ID_LEN)
                && NULL == get_ACL_from_ACL_vector(&acl_vec, myid)){
            en_acl_vec.count = acl_vec.count + 1;
            try{
                pACL = (struct ACL*)rp.create_raw(en_acl_vec.count*sizeof(struct ACL));
                memcpy(pACL, acl_vec.data, acl_vec.count * sizeof(struct ACL));
                en_acl_vec.data = pACL;
            }
            catch(...){
                ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to create buff",
                        __FILE__, __LINE__);
                continue;
            }

            en_acl_vec.data[acl_vec.count].perms = ZOO_PERM_ALL;
            en_acl_vec.data[acl_vec.count].id.scheme = myscheme;
            snprintf(myscheme, GALILEO_ACL_SCHEME_LEN, "%s", "ip");
            en_acl_vec.data[acl_vec.count].id.id = myid;
        }
        else{
            en_acl_vec.count = acl_vec.count;
            try{
                pACL = (struct ACL*)rp.create_raw(en_acl_vec.count*sizeof(struct ACL));
                memcpy(pACL, acl_vec.data, acl_vec.count * sizeof(struct ACL));
                en_acl_vec.data = pACL;
            }
            catch(...){
                ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to create buff",
                        __FILE__, __LINE__);
                continue;
            }
        }

        
        snprintf(node_buff, lleo_handler->pack_buff_size, "%s", node->data);
        int buff_len = node->data_len;

        zrc = zoo_create(lleo_handler->zh, node->path, node_buff, buff_len,
                &en_acl_vec, ZOO_EPHEMERAL, tmppath, GALILEO_DEFAULT_PATH_LEN);
        if (ZOK != zrc){
            grc = translate_lleo_error(zrc);
            ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to refresh child watch for path[%s],"\
                    " err[%s]", __FILE__, __LINE__, node->path, lleo_err2str(grc));
        }
        else{
            ul_writelog(UL_LOG_NOTICE, "[%s:%d] Refresh register for resource[%s] successfully",
                    __FILE__, __LINE__, node->path);
        }
        deallocate_ACL_vector(&acl_vec);
        node = node->next;
    }

    return grc;
}


    int 
update_lleo_callback_table(lleo_t *lleo_handler,
        const char* path, void *cb, void *ctx)
{
    int grc = GALILEO_E_OK;
    int res = 0;
    cb_item_t *cb_item = NULL;
    char tmppath[GALILEO_DEFAULT_PATH_LEN];
    GALILEO_POINTER_CHECK(NULL == lleo_handler, "Failed to get a valid lleo_handler");
    GALILEO_POINTER_CHECK(NULL == path, "Failed to get a valid path");
    GALILEO_POINTER_CHECK(NULL == cb, "Failed to get a valid cb")

    
    pthread_rwlock_wrlock(&(lleo_handler->cb_rwlock));
    cb_item = (cb_item_t*)myhashtable_search(lleo_handler->cb_table, (void*)path);

    if (NULL == cb_item){
    	pthread_rwlock_unlock(&(lleo_handler->cb_rwlock));
        
        cb_item = (cb_item_t*)calloc(1, sizeof(cb_item_t));
        cb_item->cb_func = (void*)cb;
        cb_item->cb_ctx = ctx;

        pthread_rwlock_wrlock(&(lleo_handler->cb_rwlock));
        res = myhashtable_insert(lleo_handler->cb_table, strdup(path), (void*)cb_item);
        pthread_rwlock_unlock(&(lleo_handler->cb_rwlock));

        if (!res){
            ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to insert callback function",
                    __FILE__, __LINE__);
            return GALILEO_E_APIERROR;
        }
    }
    else{
        
        cb_item->cb_func = (void*)cb;
        cb_item->cb_ctx  = ctx;

        snprintf(tmppath, GALILEO_DEFAULT_PATH_LEN, "%s", path);
        myhashtable_remove(lleo_handler->cb_table, tmppath);
        res = myhashtable_insert(lleo_handler->cb_table, strdup(path), (void*)cb_item);
        pthread_rwlock_unlock(&(lleo_handler->cb_rwlock));

        if (!res){
            ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to insert callback function",
                    __FILE__, __LINE__);
            return GALILEO_E_APIERROR;
        }
    }

#ifdef GALILEO_DEBUG
    ul_writelog(UL_LOG_DEBUG, "Hashtable entry count:%u",
            myhashtable_count(lleo_handler->cb_table));
#endif

    return grc;
}


    int 
delete_lleo_callback_table(lleo_t *lleo_handler, const char* path)
{
    int grc = GALILEO_E_OK;
    cb_item_t *cb_item = NULL;
    char tmppath[GALILEO_DEFAULT_PATH_LEN];

    GALILEO_POINTER_CHECK(NULL == lleo_handler, "Failed to get a valid lleo_handler");
    GALILEO_POINTER_CHECK(NULL == path, "Failed to get a valid path");

    
    pthread_rwlock_wrlock(&(lleo_handler->cb_rwlock));
    cb_item = (cb_item_t*)myhashtable_search(lleo_handler->cb_table, (void*)path);

    if (NULL != cb_item){
        snprintf(tmppath, GALILEO_DEFAULT_PATH_LEN, "%s", path);

        
        myhashtable_remove_prefix(lleo_handler->cb_table, tmppath, 1);
    }
    pthread_rwlock_unlock(&(lleo_handler->cb_rwlock));

#ifdef GALILEO_DEBUG
    ul_writelog(UL_LOG_DEBUG, "Hashtable entry count:%u",
            myhashtable_count(lleo_handler->cb_table));
#endif

    return grc;
}


int lleo_get(lleo_t *lleo_handler)
{
    int ref_cnt = 0;

    pthread_mutex_lock(&lleo_handler->ref_lock);
    if (lleo_handler->ref_cnt > 0) {
        ++lleo_handler->ref_cnt;
    }
    ref_cnt = lleo_handler->ref_cnt;
    pthread_mutex_unlock(&lleo_handler->ref_lock);

    ul_writelog(UL_LOG_DEBUG, "after inc ref cnt [%d]", ref_cnt);

    return ref_cnt;
}


int lleo_put(lleo_t *lleo_handler)
{
    zhandle_t *zh = lleo_handler->zh;
    int ref_cnt = 0;

    pthread_mutex_lock(&lleo_handler->ref_lock);
    ref_cnt = --lleo_handler->ref_cnt;
    if (lleo_handler->ref_cnt <= 0) {
        lleo_handler->zh = NULL;
    }
    pthread_mutex_unlock(&lleo_handler->ref_lock);

    if (ref_cnt == 0) {
        zookeeper_close(zh);
        if (lleo_handler->is_finish) {
            lleo_list_destroy(lleo_handler->watch_res_list);
            lleo_list_destroy(lleo_handler->regi_res_list);

            myhashtable_destroy(lleo_handler->cb_table, 1);
            
            if (lleo_handler->acl_vec.count) {
                free(lleo_handler->acl_vec.data[0].id.scheme);
                free(lleo_handler->acl_vec.data[0].id.id);
                for (int i = 0; i < lleo_handler->acl_vec.count; ++i) {
                    lleo_handler->acl_vec.data[i].id.scheme = NULL;
                    lleo_handler->acl_vec.data[i].id.id = NULL;
                }
                deallocate_ACL_vector(&lleo_handler->acl_vec);
            }
            GALILEO_SAFE_FREE(lleo_handler);
        }
    }

    ul_writelog(UL_LOG_DEBUG, "after dec ref cnt [%d]", ref_cnt);
    return ref_cnt;
}


int lleo_get_ref_cnt(lleo_t *lleo_handler)
{
    int ref_cnt = 0;
    pthread_mutex_lock(&lleo_handler->ref_lock);
    ref_cnt = lleo_handler->ref_cnt;
    pthread_mutex_unlock(&lleo_handler->ref_lock);
    return ref_cnt;
}

int lleo_set_ref_cnt(lleo_t *lleo_handler, int num)
{
    pthread_mutex_lock(&lleo_handler->ref_lock);
    lleo_handler->ref_cnt = num;
    pthread_mutex_unlock(&lleo_handler->ref_lock);

    return num;
}




    lleo_t*
lleo_init( const char *lleo_svr_list, const int sesn_expi_time, int delay_watch_time )
{
    int grc = GALILEO_E_OK;
    lleo_t *lleo_handler = 0;

    if (NULL == lleo_svr_list){
        ul_writelog(UL_LOG_WARNING, "[%s:%d] param lleo_svr_list[NULL] error",
                __FILE__, __LINE__);
        return NULL;
    }

    if (delay_watch_time < 0) {
        ul_writelog(UL_LOG_WARNING, "[%s:%d] param delay_watch_time[%d] error",
                __FILE__, __LINE__, delay_watch_time);
        return NULL;
    }

    lleo_handler = (lleo_t*)calloc(1, sizeof(lleo_t));
    if (NULL == lleo_handler){
        ul_writelog(UL_LOG_WARNING, "[%s:%d] Out of memory when creating lleo handler",
                __FILE__, __LINE__);
        return NULL;
    }

    
    reset_lleo_handler(lleo_handler);
    lleo_handler->sesn_expi_time = sesn_expi_time;
    lleo_handler->delay_watch_time = delay_watch_time;

    
    snprintf(lleo_handler->lleo_svr_list, GALILEO_DEFAULT_PATH_LEN,
            "%s", lleo_svr_list);

    lleo_handler->zh = zookeeper_init(lleo_svr_list, default_zoo_watcher,
            sesn_expi_time, 0, (void*)lleo_handler, 0);
    if (NULL == lleo_handler->zh){
        ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to initialize lleo handler",
                __FILE__, __LINE__);
        grc = GALILEO_E_APIERROR;
        GALILEO_SAFE_FREE(lleo_handler);
        return NULL;
    }

    lleo_set_ref_cnt(lleo_handler, 1);

    
    if (ZOO_CONNECTED_STATE != zoo_state(lleo_handler->zh)) sleep(1);

    
    lleo_handler->cb_table = create_myhashtable(GALILEO_MAX_CALLBACK_NUM,
            string_hash_djb2, string_equal);
    if (NULL == lleo_handler->cb_table){
        ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to initialize callback table",
                __FILE__, __LINE__);
        grc = GALILEO_E_APIERROR;
        lleo_put(lleo_handler);
        GALILEO_SAFE_FREE(lleo_handler);
        return NULL;
    }

    
    if (0 != pthread_rwlock_init(&(lleo_handler->cb_rwlock), NULL)
            || 0 != pthread_mutex_init(&(lleo_handler->zh_lock), NULL)
            || 0 != pthread_mutex_init(&(lleo_handler->ref_lock), NULL)
            || 0 != pthread_cond_init(&(lleo_handler->zh_cond), NULL)
            || 0 != pthread_create(&(lleo_handler->zh_tid), NULL,
                check_zoo_handler, (void*)lleo_handler)){
        ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to initialize rwlock, mutex or cond",
                __FILE__, __LINE__);
        grc = GALILEO_E_APIERROR;
        myhashtable_destroy(lleo_handler->cb_table, 1);
        lleo_put(lleo_handler);
        GALILEO_SAFE_FREE(lleo_handler);
        return NULL;
    }

    
    lleo_handler->watch_res_list = lleo_list_create_head();
    if (NULL == lleo_handler->watch_res_list){
        ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to initialize watch_res_list",
                __FILE__, __LINE__);
        grc = GALILEO_E_APIERROR;
        myhashtable_destroy(lleo_handler->cb_table, 1);
        lleo_put(lleo_handler);
        GALILEO_SAFE_FREE(lleo_handler);
        return NULL;
    }

    
    lleo_handler->regi_res_list = lleo_list_create_head();
    if (NULL == lleo_handler->regi_res_list){
        ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to initializer regi_res_list",
                __FILE__, __LINE__);
        grc = GALILEO_E_APIERROR;
        myhashtable_destroy(lleo_handler->cb_table, 1);
        lleo_put(lleo_handler);
        GALILEO_SAFE_FREE(lleo_handler);
        return NULL;
    }

    return lleo_handler;
}


int lleo_add_auth(lleo_t *lleo_handler, struct ACL_vector *acl_vec)
{
    int grc = GALILEO_E_OK;
    int zrc = ZOK;
    int count = 0;
    struct Id *id = NULL;
    struct ACL_vector *acl = &lleo_handler->acl_vec;
    char *id_buf = NULL;
    char *sch_buf = NULL;

    Lsc::ResourcePool rp;

    if (!lleo_handler) {
        ul_writelog(UL_LOG_WARNING, "[%s:%d] lleo_handler not init",
            __FILE__, __LINE__);
        grc = GALILEO_E_BADPARAM;
        goto err_out;
    }

    if (!acl_vec) {
        ul_writelog(UL_LOG_WARNING, "[%s:%d] acl vec is NULL",
            __FILE__, __LINE__);
        grc = GALILEO_E_BADPARAM;
        goto err_out;
    }

    if (lleo_get(lleo_handler) <= 0) {
        ul_writelog(UL_LOG_DEBUG, "zookeeper is in reinitializing...");
        return GALILEO_E_REINIT;
    }

    zrc = allocate_ACL_vector(acl, acl_vec->count);
    GALILEO_ZERROR_CHECK_PUT(lleo_handler, zrc, "Failed to add acl for lleo", lleo_handler->lleo_svr_list);
    if (!acl->data) {
        ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to allocate ACL vector to %s",
            __FILE__, __LINE__, lleo_handler->lleo_svr_list);
        lleo_put(lleo_handler);
        return GALILEO_E_SYSERROR;
    }

    id_buf = (char *)malloc(acl_vec->count * GALILEO_ACL_ID_LEN);
    if (!id_buf) {
        ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to create buff",
                __FILE__, __LINE__);
        lleo_put(lleo_handler);
        return GALILEO_E_SYSERROR;
    }

    sch_buf = (char *)malloc(acl_vec->count * GALILEO_ACL_SCHEME_LEN);
    if (!sch_buf) {
        free(id_buf);
        ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to create buff",
                __FILE__, __LINE__);
        lleo_put(lleo_handler);
        return GALILEO_E_SYSERROR;
    }

    while(count < acl_vec->count) {
        id = &acl_vec->data[count].id;
        int id_len = strlen(id->id);
        int sch_len = strlen(id->scheme);
        acl->data[count].id.id = id_buf + count * GALILEO_ACL_ID_LEN;
        strncpy(acl->data[count].id.id, id->id, id_len+1);
        acl->data[count].id.scheme = sch_buf + count * GALILEO_ACL_SCHEME_LEN;
        strncpy(acl->data[count].id.scheme, id->scheme, sch_len+1);

        if (0 == strcmp(id->scheme, "digest")) {
            zrc = zoo_add_auth(lleo_handler->zh, id->scheme, id->id, strlen(id->id), NULL, NULL);
            if (zrc) {
                ul_writelog(UL_LOG_WARNING, "[%s:%d] zoo add auth scheme[%s], id[%s]",
                    __FILE__, __LINE__, id->scheme, id->id);
                grc = GALILEO_E_AUTHFAILED;
                lleo_put(lleo_handler);
                goto err_out;
            }
        }
        ++count;
    }

    lleo_put(lleo_handler);
    return grc;

err_out:
    GALILEO_ZERROR_CHECK(zrc, "Failed to add acl for lleo", lleo_handler->lleo_svr_list);
    return grc;
}


    int 
lleo_get_ip_port(char *ip_port_list, const char *lleo_svr_list)
{
    char g_svr_list[GALILEO_DEFAULT_PATH_LEN];
    char *p_ip = g_svr_list;
    char *p_port = NULL, *p_end = NULL;
    struct addrinfo hints, *res;
    int size = GALILEO_DEFAULT_PATH_LEN;

    snprintf(g_svr_list, GALILEO_DEFAULT_PATH_LEN,  "%s", lleo_svr_list);
    bzero(&hints, sizeof(hints));

    
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_IP;
    hints.ai_flags = AI_CANONNAME;

    while (p_ip != NULL && *p_ip != '\0') {
        if ((p_port = strchr(p_ip, ':'))) {
            *p_port++ = '\0';
            if ((p_end = strchr(p_port, ','))) {
                *p_end++ = '\0';
            }
        } else {
            ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to get ip port from %s",
                __FILE__, __LINE__, lleo_svr_list);
            return GALILEO_E_BADPARAM;
        }

        if (getaddrinfo(p_ip, p_port, &hints, &res) < 0) {
            ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to call getaddrinfo from %s:%s",
                __FILE__, __LINE__, p_ip, p_port);
            return GALILEO_E_BADPARAM;
        }

        struct addrinfo *p = res;
        int len = 0;
        while (p) {
            char ip[16] = {'\0'};
            int port = 0;
            struct sockaddr_in *sinp = (struct sockaddr_in *)p->ai_addr;
            if (inet_ntop(sinp->sin_family, &sinp->sin_addr, ip, sizeof(ip))) {
                port = ntohs(sinp->sin_port);
                len = snprintf(ip_port_list, size, "%s:%d,", ip, port);
                
                if (len >= size) {
                    if (size < GALILEO_DEFAULT_PATH_LEN) {
                        *--ip_port_list = '\0';
                    }
                    ul_writelog(UL_LOG_WARNING, "[%s:%d] ip_port_list SPACE NOT ENOUGH %s, %s, %d",
                        __FILE__, __LINE__, ip_port_list, ip, port);
                    freeaddrinfo(res);
                    return GALILEO_E_BADPARAM;
                }
                ip_port_list += len;
                size -= len;
            }
            p = p->ai_next;
        }
        freeaddrinfo(res);
        p_ip = p_end;
    }

    if (size < GALILEO_DEFAULT_PATH_LEN) {
        *--ip_port_list = '\0';
    }
    return GALILEO_E_OK;
}



    int 
lleo_set_buf_size( lleo_t *lleo_handler, int buf_size )
{
    int grc = GALILEO_E_OK;

    GALILEO_POINTER_CHECK(NULL == lleo_handler, "Failed to get a valid lleo_handler");
    GALILEO_INT_CHECK(buf_size <= 0, "Failed to get a valid buf_size", buf_size);
    
    lleo_handler->pack_buff_size = buf_size;
    
    return grc;
}



    int 
lleo_set_log( const char* log_path, int log_level )
{
    int grc = GALILEO_E_OK;
    int zoo_log_level = ZOO_LOG_LEVEL_DEBUG;
    FILE *log_file = NULL;

    GALILEO_POINTER_CHECK(NULL == log_path, "Failed to get a valid log_path");

    log_file = fopen(log_path, "a");
    if (NULL == log_file){
        ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to create a valid log file",
                __FILE__, __LINE__);
        return GALILEO_E_SYSERROR;
    }
    zoo_set_log_stream(log_file);

    switch(log_level){
        case 16:
            zoo_log_level = ZOO_LOG_LEVEL_DEBUG;
            break;
        case 8:
        case 4:
            zoo_log_level = ZOO_LOG_LEVEL_INFO;
            break;
        case 2:
            zoo_log_level = ZOO_LOG_LEVEL_WARN;
            break;
        case 1:
            zoo_log_level = ZOO_LOG_LEVEL_ERROR;
            break;
        default:
            break;
    }
    zoo_set_debug_level((ZooLogLevel)log_level);

    return grc;
}


void lleo_close_log(void)
{
    FILE *log_file = getLogStream();
    GALILEO_SAFE_FCLOSE(log_file);
}


    void 
lleo_destroy( lleo_t *lleo_handler )
{
    int grc = GALILEO_E_OK;
    int zrc = ZOK;

    if (NULL == lleo_handler){
         ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to get a valid lleo_handler",
                __FILE__, __LINE__);
        return;
    }

    if (GALILEO_MAGICNUM == lleo_handler->magic_num){
        lleo_handler->is_finish = 1;
        pthread_mutex_lock(&(lleo_handler->zh_lock));
        pthread_cond_signal(&(lleo_handler->zh_cond));
        pthread_mutex_unlock(&(lleo_handler->zh_lock));
        pthread_join(lleo_handler->zh_tid, NULL);

        if (lleo_handler->zh) {
            zrc = lleo_put(lleo_handler);
            grc = translate_lleo_error(zrc);
        }
    }

    
    return;
}



    int 
lleo_register_resource( lleo_t *lleo_handler,
        const char *path, Lsc::var::IVar& node_data, int *tag_no, int flag )
{
    int zrc = ZOK;
    int grc = GALILEO_E_OK;
    Lsc::ResourcePool rp;

    GALILEO_POINTER_CHECK(NULL == lleo_handler, "Failed to get a valid lleo_handler");
    GALILEO_POINTER_CHECK(((NULL == path) || 0 == strncmp(path, "/", 2)),
            "Failed to get a valid resource path");
    GALILEO_POINTER_CHECK(!node_data.is_dict(), "Failed to get a valid dict");
    GALILEO_POINTER_CHECK(NULL == tag_no, "Failed to get a valid tag_no pointer");

    if (lleo_get(lleo_handler) <= 0) {
        ul_writelog(UL_LOG_WARNING, "zookeeper is under reinitializing...");
        return GALILEO_E_CONNLOSS;
    }
    
    struct Stat stat;
    struct ACL_vector acl_vec = {0, NULL};
    zrc = zoo_get_acl(lleo_handler->zh, path, &acl_vec, &stat);
    GALILEO_ZERROR_CHECK_PUT(lleo_handler, zrc, "Failed to get ACL for node", path);

    
    char myscheme[GALILEO_ACL_SCHEME_LEN];
    char myid[GALILEO_ACL_ID_LEN];
    struct ACL_vector en_acl_vec = {0, NULL};
    if (0 == get_local_conn_ip(myid, GALILEO_ACL_ID_LEN)
            && NULL == get_ACL_from_ACL_vector(&acl_vec, myid)){
        en_acl_vec.count = acl_vec.count + 1;
        try{
            struct ACL* pACL = (struct ACL*)rp.create_raw(
                    en_acl_vec.count * sizeof(struct ACL));
            memcpy(pACL, acl_vec.data, acl_vec.count * sizeof(struct ACL));
            en_acl_vec.data = pACL;
        }
        catch(...){
            ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to create buff",
                    __FILE__, __LINE__);
            lleo_put(lleo_handler);
            return GALILEO_E_APIERROR;
        }

        en_acl_vec.data[acl_vec.count].perms = ZOO_PERM_ALL;
        en_acl_vec.data[acl_vec.count].id.scheme = myscheme;
        snprintf(myscheme, GALILEO_ACL_SCHEME_LEN, "%s", "ip");
        en_acl_vec.data[acl_vec.count].id.id = myid;
    }
    else{
        en_acl_vec.count = acl_vec.count;
        try{
            struct ACL* pACL = (struct ACL*)rp.create_raw(
                    en_acl_vec.count * sizeof(struct ACL));
            memcpy(pACL, acl_vec.data, acl_vec.count * sizeof(struct ACL));
            en_acl_vec.data = pACL;
        }
        catch(...){
            ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to create buff",
                    __FILE__, __LINE__);
            lleo_put(lleo_handler);
            return GALILEO_E_APIERROR;
        }
    }

   
    char entitypath[GALILEO_DEFAULT_PATH_LEN];
    grc = prepare_lleo_entity_path_prefix(path, entitypath, GALILEO_DEFAULT_PATH_LEN);
    if (GALILEO_E_OK != grc){
        deallocate_ACL_vector(&acl_vec);
        lleo_put(lleo_handler);
        GALILEO_INT_CHECK(GALILEO_E_OK != grc, "Failed to prepare entity path prefix", grc);
    }

#ifdef GALILEO_DEBUG
    ul_writelog(UL_LOG_DEBUG, "path: %s, entity_path_prefix: %s", path, entitypath);
#endif

    
    Lsc::var::JsonSerializer js;
    Lsc::AutoBuffer buf;
    js.serialize(node_data, buf);
    const char *node_buff = buf.c_str();
    char realpath[GALILEO_DEFAULT_PATH_LEN];

    if (flag){
        zrc = zoo_create(lleo_handler->zh, entitypath, node_buff,
            strlen(node_buff)+1, &en_acl_vec, ZOO_SEQUENCE,
            realpath, GALILEO_DEFAULT_PATH_LEN);
    }
    else{
        zrc = zoo_create(lleo_handler->zh, entitypath, node_buff,
            strlen(node_buff)+1, &en_acl_vec, ZOO_EPHEMERAL|ZOO_SEQUENCE,
            realpath, GALILEO_DEFAULT_PATH_LEN);
    }

    if (ZOK != zrc){
        deallocate_ACL_vector(&acl_vec);
        GALILEO_ZERROR_CHECK_PUT(lleo_handler, zrc, "Failed to create node", entitypath);
    }

    
    lleo_put(lleo_handler);
    
    grc = get_lleo_tag_no_from_realpath(realpath, tag_no);
    if (GALILEO_E_OK != grc){
        deallocate_ACL_vector(&acl_vec);
        GALILEO_INT_CHECK(GALILEO_E_OK != grc,
                "Failed to get lleo tag no from realpath", grc);
    }

    
    pthread_mutex_lock(&(lleo_handler->zh_lock));
    lleo_list_insert_node(lleo_handler->regi_res_list, realpath, node_buff,
            strlen(node_buff)+1);
    pthread_mutex_unlock(&(lleo_handler->zh_lock));

    deallocate_ACL_vector(&acl_vec);
    return grc;
}



    int 
lleo_query_resource( const lleo_t* g_handler,
        const char *path, Lsc::var::IVar& res_data, Lsc::ResourcePool& rp )
{
    int grc = GALILEO_E_OK;
    GALILEO_POINTER_CHECK(NULL == g_handler, "Failed to get valid lleo_handler");
    GALILEO_POINTER_CHECK(NULL == path, "Failed to get a valid path");

    lleo_t *lleo_handler = const_cast<lleo_t *> (g_handler);
    if (lleo_get(lleo_handler) <= 0) {
        ul_writelog(UL_LOG_WARNING, "zookeeper is in reinitializing...");
        return GALILEO_E_REINIT;
    }
    grc = get_all_lleo_data_by_path(lleo_handler->zh, path, res_data,
            lleo_handler->pack_buff_size, rp);
    lleo_put(lleo_handler);

    return grc;
}



    int 
lleo_watch_resource( lleo_t* lleo_handler, const char *path,
        lleo_proc_callback cb, void *cb_ctx )
{
    int zrc = ZOK;
    int grc = GALILEO_E_OK;
    int retry_time = 0;
    bool retry = true;
    int live_threshold = 0;
    Lsc::ResourcePool rp;

    GALILEO_POINTER_CHECK(NULL == lleo_handler, "Failed to get a valid lleo_handler");
    GALILEO_POINTER_CHECK(NULL == path, "Failed to get a valid path");
    GALILEO_POINTER_CHECK(NULL == cb, "Failed to get a valid callback function");

    if (lleo_get(lleo_handler) <= 0) {
        ul_writelog(UL_LOG_WARNING, "zookeeper is in reinitializing...");
        return GALILEO_E_REINIT;
    }

    while (retry) {
        
        char* buff = NULL;
        ++retry_time;
        retry = false;
        try{
            buff = (char*)rp.create_raw(lleo_handler->pack_buff_size);
        }
        catch(...){
            ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to create buff, buf_size:%d",
                __FILE__, __LINE__, lleo_handler->pack_buff_size);
            lleo_put(lleo_handler);
            return GALILEO_E_APIERROR;
        }

        grc = update_lleo_callback_table(lleo_handler, path, (void*)cb, cb_ctx);
        GALILEO_INT_CHECK_PUT(lleo_handler, GALILEO_E_OK != grc, "Failed to add cb to table", grc);

        
        struct Stat stat;
        int buff_len = lleo_handler->pack_buff_size;
        zrc = zoo_wget(lleo_handler->zh, path, default_zoo_watcher, (void*)lleo_handler,
                buff, &buff_len, &stat);
        GALILEO_ZERROR_CHECK_PUT(lleo_handler, zrc, "Failed to get data", path);
        GALILEO_INT_CHECK_PUT(lleo_handler, 0 == buff_len, "Failed to get data", buff_len);

        
        pthread_mutex_lock(&(lleo_handler->zh_lock));
        zrc = lleo_list_insert_node(lleo_handler->watch_res_list, path, "Watch", 6);
        pthread_mutex_unlock(&(lleo_handler->zh_lock));
        GALILEO_ZERROR_CHECK_PUT(lleo_handler, zrc, "Failed to insert watch list", path);

        
        if (0 != stat.ephemeralOwner){
            ul_writelog(UL_LOG_WARNING, "[%s:%d] Want regular node, get ephemeral node",
                __FILE__, __LINE__);
            lleo_put(lleo_handler);
            return GALILEO_E_BADPARAM;
        }

        try {
            Lsc::var::JsonDeserializer jd(rp);
            Lsc::var::IVar &res_data = rp.create<Lsc::var::Dict>();
            res_data[GALILEO_ABSTRACT] = jd.deserialize(buff);
            if (!res_data[GALILEO_ABSTRACT][GALILEO_THRESHOLD].is_null()) {
                live_threshold = res_data[GALILEO_ABSTRACT][GALILEO_THRESHOLD].to_int32();
            }
 
        } catch (...) {
            ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to deserialize node data, path:%s",
                    __FILE__, __LINE__, path);
            lleo_put(lleo_handler);
            return GALILEO_E_MARSHALLINGERROR;
        }

        
        struct String_vector str_vec = {0, NULL};
        zrc = zoo_wget_children(lleo_handler->zh, path, default_zoo_watcher,
                (void*)lleo_handler, &str_vec);
        GALILEO_ZERROR_CHECK_PUT(lleo_handler, zrc, "Failed to get children", path);

        int fail_num = 0;
        for (int i = 0; i < str_vec.count; i++){
            char tmppath[GALILEO_DEFAULT_PATH_LEN];
            tmppath[0] = '\0';
            grc = makeup_lleo_entity_path_by_name(path, str_vec.data[i],
                    tmppath, GALILEO_DEFAULT_PATH_LEN);
            if (GALILEO_E_OK != grc){
                deallocate_String_vector(&str_vec);
                GALILEO_INT_CHECK_PUT(lleo_handler, GALILEO_E_OK != grc,
                        "Failed make up entity path by name", grc);
            }

    #ifdef GALILEO_DEBUG
            ul_writelog(UL_LOG_DEBUG, "NO.[%d], path [%s]", i, tmppath);
    #endif
            grc = update_lleo_callback_table(lleo_handler, tmppath, (void*)cb, cb_ctx);
            if (GALILEO_E_OK != grc){
                deallocate_String_vector(&str_vec);
                GALILEO_INT_CHECK_PUT(lleo_handler, GALILEO_E_OK != grc, "Failed to add cb to table", grc);
            }

            buff_len = lleo_handler->pack_buff_size;
            zrc = zoo_wget(lleo_handler->zh, tmppath, default_zoo_watcher,
                    (void*)lleo_handler, buff, &buff_len, &stat);
            if (ZOK != zrc || !buff_len){
                ++fail_num;
                
                continue;
            }
        }

        int total_num = str_vec.count;
        deallocate_String_vector(&str_vec);

        if (fail_num > 0 || total_num == 0) {
            if (retry_time < default_max_retry_time) {
                retry = true;
                sleep(retry_time * default_retry_interval);
                continue;
            } else if (total_num < live_threshold + fail_num) {
                ul_writelog(UL_LOG_FATAL, "[%s:%d] Failed to set node watch,"\
                        " path:%s, suc: %d, fail: %d, retry: %d", 
                        __FILE__, __LINE__, path, total_num-fail_num, fail_num, retry_time);
            } else {
                ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to set node watch,"\
                        " path:%s, suc: %d, fail: %d, retry: %d",
                        __FILE__, __LINE__, path, total_num-fail_num, fail_num, retry_time);
            }
        }
    }

    lleo_put(lleo_handler);
    return grc;
}



    int 
lleo_delete_watch(lleo_t* lleo_handler, const char *path)
{
    int grc = GALILEO_E_OK;

    GALILEO_POINTER_CHECK(NULL == lleo_handler, "Failed to get a valid lleo_handler");
    GALILEO_POINTER_CHECK(NULL == path, "Failed to get a valid path");

    if (lleo_get(lleo_handler) <= 0) {
        ul_writelog(UL_LOG_WARNING, "zookeeper is in reinitializing...");
        return GALILEO_E_REINIT;
    }

    grc = delete_lleo_callback_table(lleo_handler, path);

    pthread_mutex_lock(&(lleo_handler->zh_lock));
    grc = lleo_list_delete_node(lleo_handler->watch_res_list, path);
    pthread_mutex_unlock(&(lleo_handler->zh_lock));

    lleo_put(lleo_handler);
    return grc;
}



    int 
lleo_update_resource( lleo_t *lleo_handler, const char *path,
        Lsc::var::IVar& node_data, const int tag_no )
{
    int zrc = ZOK;
    int grc = GALILEO_E_OK;
    Lsc::var::JsonSerializer js;
    Lsc::AutoBuffer buf;

    GALILEO_POINTER_CHECK(NULL == lleo_handler, "Failed to get a valid lleo_handler");
    GALILEO_POINTER_CHECK(NULL == path, "Failed to get a valid path");
    GALILEO_POINTER_CHECK(!node_data.is_dict(), "Failed to get a valid dict");

    if (lleo_get(lleo_handler) <= 0) {
        ul_writelog(UL_LOG_WARNING, "zookeeper is in reinitializing...");
        return GALILEO_E_REINIT;
    }
    char tmppath[GALILEO_DEFAULT_PATH_LEN];
    grc = makeup_lleo_entity_path_by_tag_no(path, tag_no, tmppath, GALILEO_DEFAULT_PATH_LEN);
    GALILEO_INT_CHECK_PUT(lleo_handler, GALILEO_E_OK != grc, "Failed to make up lleo"\
            " entity path by tag_no", grc);

#ifdef GALILEO_DEBUG
    ul_writelog(UL_LOG_DEBUG, "set the data for the path [%s]", tmppath);
#endif

    js.serialize(node_data, buf);
    const char* node_buff = buf.c_str();
    zrc = zoo_set(lleo_handler->zh, tmppath, node_buff, strlen(node_buff)+1, -1);
    GALILEO_ZERROR_CHECK_PUT(lleo_handler, zrc, "Failed to set data", path);

    
    pthread_mutex_lock(&(lleo_handler->zh_lock));
    lleo_list_update_node(lleo_handler->regi_res_list, tmppath, node_buff,
            strlen(node_buff)+1);
    pthread_mutex_unlock(&(lleo_handler->zh_lock));

    lleo_put(lleo_handler);
    return grc;
}



    int 
lleo_delete_resource( lleo_t *lleo_handler, const char *path,
        const int tag_no )
{
    int zrc = ZOK;
    int grc = GALILEO_E_OK;

    GALILEO_POINTER_CHECK(NULL == lleo_handler,
            "Failed to get a valid lleo_handler");
    GALILEO_POINTER_CHECK(NULL == path, "Failed to get a valid path");

    if (lleo_get(lleo_handler) <= 0) {
        ul_writelog(UL_LOG_WARNING, "zookeeper is in reinitializing...");
        return GALILEO_E_REINIT;
    }
    char tmppath[GALILEO_DEFAULT_PATH_LEN];
    grc = makeup_lleo_entity_path_by_tag_no(path, tag_no, tmppath, GALILEO_DEFAULT_PATH_LEN);
    GALILEO_INT_CHECK_PUT(lleo_handler, GALILEO_E_OK != grc, "Failed to make up entity path by tag_no", grc);

    zrc = zoo_delete(lleo_handler->zh, tmppath, -1);
    GALILEO_ZERROR_CHECK_PUT(lleo_handler, zrc, "Failed to delete entity", path);

    
    pthread_mutex_lock(&(lleo_handler->zh_lock));
    lleo_list_delete_node(lleo_handler->regi_res_list, tmppath);
    pthread_mutex_unlock(&(lleo_handler->zh_lock));

    lleo_put(lleo_handler);
    return grc;
}




    int 
lleo_admin_create_abstract(const lleo_t* lleo_handler,
        const char *path, Lsc::var::IVar& node_data, Lsc::var::IVar& node_acl )
{
    int zrc = ZOK;
    int grc = GALILEO_E_OK;
    Lsc::ResourcePool rp;

    GALILEO_POINTER_CHECK(NULL == lleo_handler, "Failed to get a valid lleo_handler");
    GALILEO_POINTER_CHECK(NULL == path, "Failed to get a valid path");
    GALILEO_POINTER_CHECK(!node_data.is_dict(), "Failed to get a valid dict");
    GALILEO_POINTER_CHECK(!node_acl.is_dict(), "Failed to get a valid dict");

    
    if ('/' == path[strlen(path)]){
        ul_writelog(UL_LOG_WARNING, "[%s:%d] the path cannot be end with a '/'",
                __FILE__, __LINE__);
        return GALILEO_E_BADPARAM;
    }

    struct ACL_vector acl_vec = {0, NULL};
    struct ACL_vector pare_acl_vec = {0, NULL};
    struct ACL *pACL = NULL;
    char *acl_scheme = NULL;
    char *acl_id     = NULL;
    if (node_acl.size() > 0){
        
        if (node_acl.get(GALILEO_ACL).is_null()
            || !node_acl.get(GALILEO_ACL).is_array()){
            ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to get a valid acl array",
                    __FILE__, __LINE__);
            return GALILEO_E_BADPARAM;
        }
        Lsc::var::IVar& acl_array = node_acl.get(GALILEO_ACL);
        size_t item_count = acl_array.size();

        
        acl_vec.count = item_count + 1;
        try{
            pACL = (struct ACL*)rp.create_raw(acl_vec.count*sizeof(struct ACL));
            acl_scheme = (char*)rp.create_raw(acl_vec.count*GALILEO_ACL_SCHEME_LEN);
            acl_id = (char*)rp.create_raw(acl_vec.count*GALILEO_ACL_ID_LEN);
        }
        catch(...){
            ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to create buff",
                    __FILE__, __LINE__);
            return GALILEO_E_APIERROR;
        }
        acl_vec.data = pACL;

        for (size_t i = 0; i < item_count; i++){
            Lsc::var::IVar& acl_dict = acl_array.get(i);
            
            unsigned int perms = 0;
            if (acl_dict.get(GALILEO_ACL_PERMS).is_null()){
                ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to get a valid acl dict",
                        __FILE__, __LINE__);
                return GALILEO_E_BADPARAM;
            }
            perms = acl_dict.get(GALILEO_ACL_PERMS).to_int32();

            if (perms & ZOO_PERM_WRITE){
                perms |= ZOO_PERM_DELETE;
                perms |= ZOO_PERM_ADMIN;
            }
            pACL[i].perms = (int)perms;
            pACL[i].id.scheme = acl_scheme+i*GALILEO_ACL_SCHEME_LEN;
            pACL[i].id.id = acl_id + i*GALILEO_ACL_ID_LEN;
            grc = lleo_set_acl(acl_dict, &pACL[i]);
            if (GALILEO_E_OK != grc) {
                ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to get a valid acl",
                    __FILE__, __LINE__);
                return GALILEO_E_BADPARAM;
            }
        }

        
        acl_vec.data[item_count].perms = ZOO_PERM_ALL;
        acl_vec.data[item_count].id.scheme = acl_scheme+item_count*GALILEO_ACL_SCHEME_LEN;
        snprintf(acl_vec.data[item_count].id.scheme, GALILEO_ACL_SCHEME_LEN, "%s", "ip");
        acl_vec.data[item_count].id.id = acl_id+item_count*GALILEO_ACL_ID_LEN;
        
        acl_vec.count = item_count;
        if (0 == get_local_conn_ip(acl_vec.data[item_count].id.id, GALILEO_ACL_ID_LEN)
                && NULL == get_ACL_from_ACL_vector(&acl_vec, acl_vec.data[item_count].id.id)){
            acl_vec.count = item_count + 1;
        }
        else{
            acl_vec.count = item_count;
        }
    }
    else{
        
        const char* ch = strrchr(path, '/');
        GALILEO_POINTER_CHECK(NULL == ch, "Failed to get a '/'");

        char parepath[GALILEO_DEFAULT_PATH_LEN];
        strncpy(parepath, path, ch-path);
        parepath[ch-path] = '\0';
        
        if (parepath[0] == '\0'){
            parepath[0] = '/';
            parepath[1] = '\0';
        }

        struct Stat stat;
        zrc = zoo_get_acl(lleo_handler->zh, parepath, &pare_acl_vec, &stat);
        GALILEO_ZERROR_CHECK(zrc, "Failed to get ACL for node", parepath);

        char* myid = (char*)rp.create_raw(GALILEO_ACL_ID_LEN);
        char* myscheme = (char*)rp.create_raw(GALILEO_ACL_SCHEME_LEN);

        if (0 == get_local_conn_ip(myid, GALILEO_ACL_ID_LEN)
                && NULL == get_ACL_from_ACL_vector(&pare_acl_vec, myid)){

            acl_vec.count = pare_acl_vec.count + 1;
            try{
                pACL = (struct ACL*)rp.create_raw(acl_vec.count*sizeof(struct ACL));
            }
            catch(...){
                deallocate_ACL_vector(&pare_acl_vec);
                ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to create buff",
                        __FILE__, __LINE__);
                return GALILEO_E_APIERROR;
            }

            memcpy(pACL, pare_acl_vec.data, pare_acl_vec.count * sizeof(struct ACL));
            acl_vec.data = pACL;

            acl_vec.data[pare_acl_vec.count].perms = ZOO_PERM_ALL;
            acl_vec.data[pare_acl_vec.count].id.scheme = myscheme;
            snprintf(myscheme, GALILEO_ACL_SCHEME_LEN, "%s", "ip");
            acl_vec.data[pare_acl_vec.count].id.id = myid;
        }
        else{
            acl_vec.count = pare_acl_vec.count;
            try{
                pACL = (struct ACL*)rp.create_raw(acl_vec.count*sizeof(struct ACL));
            }
            catch(...){
                deallocate_ACL_vector(&pare_acl_vec);
                ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to create buff",
                        __FILE__, __LINE__);
                return GALILEO_E_APIERROR;
            }

            memcpy(pACL, pare_acl_vec.data, pare_acl_vec.count * sizeof(struct ACL));
            acl_vec.data = pACL;
        }
    }
    
    Lsc::var::JsonSerializer js;
    Lsc::AutoBuffer buf;
    js.serialize(node_data, buf);
    const char* node_buff = buf.c_str();
    char realpath[GALILEO_DEFAULT_PATH_LEN];

    zrc = zoo_create(lleo_handler->zh, path, node_buff, strlen(node_buff)+1,
            &acl_vec, 0, realpath, GALILEO_DEFAULT_PATH_LEN);
    deallocate_ACL_vector(&pare_acl_vec);
    GALILEO_ZERROR_CHECK(zrc, "Failed to create abstract node", path);

    return grc;
}

    int 
lleo_set_acl(Lsc::var::IVar& acl_dict, struct ACL *pACL)
{
    const char *id = NULL;

    if (!acl_dict.get(GALILEO_ACL_IP).is_null()){
        id = acl_dict.get(GALILEO_ACL_IP).c_str();
        
        if (0 == strcmp(id, "*.*.*.*")){
            snprintf(pACL->id.scheme, GALILEO_ACL_SCHEME_LEN, "%s", "world");
            snprintf(pACL->id.id, GALILEO_ACL_ID_LEN, "%s", "anyone");
        }
        else{
            snprintf(pACL->id.scheme, GALILEO_ACL_SCHEME_LEN, "%s", "ip");
            snprintf(pACL->id.id, GALILEO_ACL_ID_LEN, "%s", id);
        }

        
        if (acl_dict.get(GALILEO_ACL_MASK).is_null()){
            ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to get a valid acl mask",
                    __FILE__, __LINE__);
            return GALILEO_E_BADPARAM;
        }

    } else if (!acl_dict.get(GALILEO_ACL_DIGEST).is_null()){
        char sha1_buf[SHA_DIGEST_LENGTH] = {0};
        char base64_buf[GALILEO_ACL_ID_LEN] = {0};
        int used_len = 0;

        id = acl_dict.get(GALILEO_ACL_DIGEST).c_str();

        snprintf(pACL->id.scheme, GALILEO_ACL_SCHEME_LEN, "%s", "digest");
        used_len = snprintf(pACL->id.id, GALILEO_ACL_ID_LEN, "%s", id);
        if (0 > used_len) {
            ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to snprintf %s, err[%s]",
                    __FILE__, __LINE__, id, strerror(errno));
            return GALILEO_E_SYSERROR;
        } else if (GALILEO_ACL_ID_LEN <= used_len) {
            ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to snprintf %s, err[%s]",
                    __FILE__, __LINE__, id, "id is too long");
            return GALILEO_E_SYSERROR;
        }
        
        if (!sha1Encode(id, strlen(id), sha1_buf, sizeof(sha1_buf))) {
            ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to sha1 %s",
                    __FILE__, __LINE__, id);
            return GALILEO_E_BADPARAM;
        }

        if (!base64Encode(sha1_buf, sizeof(sha1_buf), base64_buf, sizeof(base64_buf))) {
            ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to encypt %s",
                    __FILE__, __LINE__, id);
            return GALILEO_E_BADPARAM;
        }

        char *p = strchr(pACL->id.id, ':');
        if (!p) {
            ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to get a sha1 encrypt",
                    __FILE__, __LINE__);
            return GALILEO_E_BADPARAM;
        }
        used_len = p - pACL->id.id;
        used_len += snprintf(++p, GALILEO_ACL_ID_LEN - used_len, "%s", (char *)base64_buf);
        if (GALILEO_ACL_ID_LEN <= used_len) {
            ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to snprintf encrypt %s, err[%s]",
                    __FILE__, __LINE__, id, "id is too long");
            return GALILEO_E_SYSERROR;
        }
    } else if (!acl_dict.get(GALILEO_ACL_AUTH).is_null()){
        char sha1_buf[SHA_DIGEST_LENGTH] = {0};
        char base64_buf[GALILEO_ACL_ID_LEN] = {0};
        int used_len = 0;

        id = acl_dict.get(GALILEO_ACL_AUTH).c_str();

        snprintf(pACL->id.scheme, GALILEO_ACL_SCHEME_LEN, "%s", "digest");
        used_len = snprintf(pACL->id.id, GALILEO_ACL_ID_LEN, "%s", id);
        if (0 > used_len) {
            ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to snprintf %s, err[%s]",
                    __FILE__, __LINE__, id, strerror(errno));
            return GALILEO_E_SYSERROR;
        } else if (GALILEO_ACL_ID_LEN <= used_len) {
            ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to snprintf %s, err[%s]",
                    __FILE__, __LINE__, id, "id is too long");
            return GALILEO_E_SYSERROR;
        }
        
        if (!sha1Encode(id, strlen(id), sha1_buf, sizeof(sha1_buf))) {
            ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to sha1 %s",
                    __FILE__, __LINE__, id);
            return GALILEO_E_BADPARAM;
        }

        if (!base64Encode(sha1_buf, sizeof(sha1_buf), base64_buf, sizeof(base64_buf))) {
            ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to encypt %s",
                    __FILE__, __LINE__, id);
            return GALILEO_E_BADPARAM;
        }

        char *p = strchr(pACL->id.id, ':');
        if (!p) {
            ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to get a sha1 encrypt",
                    __FILE__, __LINE__);
            return GALILEO_E_BADPARAM;
        }
        used_len = p - pACL->id.id;
        used_len += snprintf(++p, GALILEO_ACL_ID_LEN - used_len, "%s", (char *)base64_buf);
        if (GALILEO_ACL_ID_LEN <= used_len) {
            ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to snprintf encrypt %s, err[%s]",
                    __FILE__, __LINE__, id, "id is too long");
            return GALILEO_E_SYSERROR;
        }
    } else {
        ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to get a valid acl rule",
            __FILE__, __LINE__);
        return GALILEO_E_BADPARAM;
    }

    pACL->perms = acl_dict.get(GALILEO_ACL_PERMS).to_int32();
    if (pACL->perms & ZOO_PERM_WRITE) {
        pACL->perms |= ZOO_PERM_ADMIN | ZOO_PERM_DELETE;
    }

    return GALILEO_E_OK;
}


int 
lleo_admin_get_abstract(const lleo_t* lleo_handler, const char *path,
         Lsc::var::IVar& node_data, Lsc::var::IVar& node_acl, Lsc::ResourcePool& rp)
{
    int zrc = ZOK;
    int grc = GALILEO_E_OK;
    Lsc::ResourcePool tmpRp;

    GALILEO_POINTER_CHECK(NULL == lleo_handler, "Failed to get a valid lleo_handler");
    GALILEO_POINTER_CHECK(NULL == path, "Failed to get a valid path");
    GALILEO_POINTER_CHECK(!node_data.is_dict(), "Failed to get a valid node dict");
    GALILEO_POINTER_CHECK(!node_acl.is_dict(), "Failed to get a valid acl dict");

    
    struct Stat stat;
    int buff_len = lleo_handler->pack_buff_size;
    char* node_buff = NULL;
    try{
        node_buff = (char*)tmpRp.create_raw(buff_len);
    }
    catch(...){
        ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to create buff, buf_size:%d",
                __FILE__, __LINE__, buff_len);
        return GALILEO_E_APIERROR;
    }

    zrc = zoo_get(lleo_handler->zh, path, 0, node_buff, &buff_len, &stat);
    GALILEO_ZERROR_CHECK(zrc, "Failed to get abstract data", path);
    GALILEO_INT_CHECK(!buff_len, "Failed to get abstract data", buff_len);
    try{
        Lsc::var::JsonDeserializer jd(rp);
        node_data = jd.deserialize(node_buff);
    }
    catch(...){
        
        try{
            Lsc::var::CompackDeserializer mpd(rp);
            node_data = mpd.deserialize(node_buff, buff_len);
        }
        catch(...){
            ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to deserialize node data, path:%s",
                    __FILE__, __LINE__, path);
            return GALILEO_E_MARSHALLINGERROR;
        }
    }

    
    struct ACL_vector acl_vec = {0, NULL};
    zrc = zoo_get_acl(lleo_handler->zh, path, &acl_vec, &stat);
    GALILEO_ZERROR_CHECK(zrc, "Failed to get abstract acl", path);

    try{
        Lsc::var::Array& acl_array = rp.create<Lsc::var::Array>();
        node_acl[GALILEO_ACL] = acl_array;
        for (int i = 0; i < acl_vec.count; i++){
            struct ACL *acl = &(acl_vec.data[i]);
            
            Lsc::var::Dict& acl_item = rp.create<Lsc::var::Dict>();
            grc = lleo_get_acl(acl, acl_item, rp);
            if (GALILEO_E_OK != grc) {
                ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to get acl", __FILE__, __LINE__);
                break;
            }
            acl_array.set(i, acl_item);
        }
    }
    catch(...){
        ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to create buff", __FILE__, __LINE__);
        deallocate_ACL_vector(&acl_vec);
        return GALILEO_E_APIERROR;
    }

    deallocate_ACL_vector(&acl_vec);
    return grc;
}

int lleo_get_acl(struct ACL *acl, Lsc::var::Dict& acl_item, Lsc::ResourcePool &rp)
{
    int mask = 0;
    unsigned int perms = 0;
    int grc = GALILEO_E_OK;
    char *id = NULL;
    char *scheme = NULL;

    try {
        id = (char*)rp.create_raw(GALILEO_ACL_ID_LEN);
        scheme = (char*)rp.create_raw(GALILEO_ACL_SCHEME_LEN);
    }
    catch(...) {
        ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to create buf",
            __FILE__, __LINE__);
        return GALILEO_E_SYSERROR;
    }

    
    if (acl->perms & ZOO_PERM_WRITE){
        
        if (!((acl->perms&ZOO_PERM_DELETE) && (acl->perms&ZOO_PERM_ADMIN))){
            ul_writelog(UL_LOG_WARNING, "[%s:%d] Error perms found [%d]",
                    __FILE__, __LINE__, acl->perms);
        }
        perms |= GALILEO_PERM_WRITE;
    }

    if (acl->perms & ZOO_PERM_READ){
        perms |= GALILEO_PERM_READ;
    }

    if (acl->perms & ZOO_PERM_CREATE){
        perms |= GALILEO_PERM_CREATE;
    }

    
    if (NULL == strchr(acl->id.id, '/')){
        
        strncpy(id, acl->id.id, GALILEO_ACL_ID_LEN);
        strncpy(scheme, acl->id.scheme, GALILEO_ACL_SCHEME_LEN);
        id[GALILEO_ACL_ID_LEN-1] = '\0';
        scheme[GALILEO_ACL_SCHEME_LEN-1] = '\0';
        if (0 == strcmp(id, "anyone")){
            snprintf(id, GALILEO_ACL_ID_LEN, "%s", "*.*.*.*");
        }
    }
    else{
        
        char *pch = NULL;
        pch = strchr(acl->id.id, '/');
        mask = atoi(++pch);
        strncpy(id, acl->id.id, pch-acl->id.id);
        strncpy(scheme, acl->id.scheme, GALILEO_ACL_SCHEME_LEN);
        id[pch-acl->id.id] = '\0';
    }

    acl_item[GALILEO_ACL_PERMS] = rp.create<Lsc::var::Int32>(perms);
    acl_item[GALILEO_ACL_MASK] = rp.create<Lsc::var::Int32>(mask);
    if (strcmp(scheme, "digest") == 0) {

        acl_item[GALILEO_ACL_AUTH] = rp.create<Lsc::var::String>(id);
    } else if (strcmp(scheme, "ip") == 0) {
        acl_item[GALILEO_ACL_IP] = rp.create<Lsc::var::String>(id);
    } else if (strcmp(scheme, "world") == 0) {
        acl_item[GALILEO_ACL_IP] = rp.create<Lsc::var::String>(id);
    } else {
        acl_item[GALILEO_ACL_SCHEME] = rp.create<Lsc::var::String>(scheme);
        acl_item[GALILEO_ACL_ID] = rp.create<Lsc::var::String>(id);
    }

    return grc;
}


    int 
lleo_admin_update_abstract( const lleo_t* g_handler,
        const char *path, Lsc::var::IVar& node_data, Lsc::var::IVar& node_acl )
{
    int zrc = ZOK;
    int grc = GALILEO_E_OK;
    Lsc::ResourcePool rp;

    GALILEO_POINTER_CHECK(NULL == g_handler, "Failed to get a valid lleo_handler");
    GALILEO_POINTER_CHECK(NULL == path, "Failed to get a valid path");
    GALILEO_POINTER_CHECK((!node_data.is_dict() || !node_acl.is_dict()),
                        "Failed to get a valid data dict or a valid acl dict");

    lleo_t *lleo_handler = const_cast<lleo_t *> (g_handler);
    if (lleo_get(lleo_handler) <= 0) {
        ul_writelog(UL_LOG_WARNING, "zookeeper is in reinitializing...");
        return GALILEO_E_REINIT;
    }
    
    if (node_data.size() > 0){
        Lsc::var::JsonSerializer js;
        Lsc::AutoBuffer buf;
        js.serialize(node_data, buf);
        const char* node_buff = buf.c_str();

        zrc = zoo_set(lleo_handler->zh, path, node_buff, strlen(node_buff)+1, -1);
        GALILEO_ZERROR_CHECK_PUT(lleo_handler, zrc, "Failed to set data", path);
    }

    struct ACL_vector acl_vec = {0, NULL};
    if (node_acl.size() > 0){
        
        if (node_acl.get(GALILEO_ACL).is_null()
            || !node_acl.get(GALILEO_ACL).is_array()){
            ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to get a valid acl array", 
                    __FILE__, __LINE__);
            lleo_put(lleo_handler);
            return GALILEO_E_BADPARAM;
        }
        Lsc::var::IVar& acl_array = node_acl.get(GALILEO_ACL);
        size_t item_count = acl_array.size();

        acl_vec.count = item_count + 1;
        struct ACL *pACL = NULL;
        char *acl_scheme = NULL;
        char *acl_id = NULL;
        try{
            pACL = (struct ACL*)rp.create_raw(acl_vec.count * sizeof(struct ACL));
            acl_scheme = (char*)rp.create_raw(acl_vec.count * GALILEO_ACL_SCHEME_LEN);
            acl_id = (char*)rp.create_raw(acl_vec.count * GALILEO_ACL_ID_LEN);
        }
        catch(...){
            ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed create buff", __FILE__, __LINE__);
            lleo_put(lleo_handler);
            return GALILEO_E_APIERROR;
        }
        acl_vec.data = pACL;

        for (int i = 0; i < (int)item_count; i++){
            Lsc::var::IVar& acl_dict = acl_array.get(i);

            
            unsigned int perms = 0;
            if (acl_dict.get(GALILEO_ACL_PERMS).is_null()){
                ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to get a valid acl dict",
                        __FILE__, __LINE__);
                lleo_put(lleo_handler);
                return GALILEO_E_BADPARAM;
            }
            perms = acl_dict.get(GALILEO_ACL_PERMS).to_int32();

            if (perms & ZOO_PERM_WRITE){
                perms |= ZOO_PERM_DELETE;
                perms |= ZOO_PERM_ADMIN;
            }
            pACL[i].perms = (int)perms;
            pACL[i].id.scheme = acl_scheme+i*GALILEO_ACL_SCHEME_LEN;

            
            pACL[i].id.id = acl_id + i * GALILEO_ACL_ID_LEN;
            grc = lleo_set_acl(acl_dict, &pACL[i]);
            if (GALILEO_E_OK != grc) {
                ul_writelog(UL_LOG_WARNING, "[%s:%d] Failed to get a valid acl ip",
                    __FILE__, __LINE__);
                lleo_put(lleo_handler);
                return GALILEO_E_BADPARAM;
            }
        }

        
        acl_vec.data[item_count].perms = ZOO_PERM_ALL;
        acl_vec.data[item_count].id.scheme = acl_scheme+item_count*GALILEO_ACL_SCHEME_LEN;
        snprintf(acl_vec.data[item_count].id.scheme, GALILEO_ACL_SCHEME_LEN, "%s", "ip");
        acl_vec.data[item_count].id.id = acl_id+item_count*GALILEO_ACL_ID_LEN;
        
        acl_vec.count = item_count;
        if (0 == get_local_conn_ip(acl_vec.data[item_count].id.id, GALILEO_ACL_ID_LEN)
                && NULL == get_ACL_from_ACL_vector(&acl_vec, acl_vec.data[item_count].id.id)){
            acl_vec.count = item_count + 1;
        }
        else{
            acl_vec.count = item_count;
        }

        
        zrc = zoo_set_acl(lleo_handler->zh, path, -1, &acl_vec);
        GALILEO_ZERROR_CHECK_PUT(lleo_handler, zrc, "Failed to set acl", path);

        
        grc = set_lleo_acl_to_children(lleo_handler->zh, path,
                &acl_vec, lleo_handler->pack_buff_size);
    }

    lleo_put(lleo_handler);
    return grc;
}



int lleo_admin_delete_abstract(const lleo_t* g_handler, const char *path)
{
    int zrc = ZOK;
    int grc = GALILEO_E_OK;

    GALILEO_POINTER_CHECK(NULL == g_handler, "Failed to get a valid lleo_handler");
    GALILEO_POINTER_CHECK(NULL == path, "Failed to get a valid path");
    lleo_t *lleo_handler = const_cast<lleo_t *> (g_handler);
    if (lleo_get(lleo_handler) <= 0) {
        ul_writelog(UL_LOG_WARNING, "zookeeper is in reinitializing...");
        return GALILEO_E_REINIT;
    }
    zrc = zoo_delete(lleo_handler->zh, path, -1);
    GALILEO_ZERROR_CHECK_PUT(lleo_handler, zrc, "Failed to delete node", path);

    lleo_put(lleo_handler);
    return grc;
}



const char* lleo_err2str(const int grc)
{
    switch(grc){
    case GALILEO_E_OK:
        return "ALL OK";
    case GALILEO_E_SYSERROR:
        return "SYSTEM ERROR";
    case GALILEO_E_RUNTIMEINCONSISTENCY:
        return "RUNTIME INCONSISTENCY";
    case GALILEO_E_DATAINCONSISTENCY:
        return "DATA INCONSISTENCY";
    case GALILEO_E_CONNLOSS:
        return "CONNECTION LOSS";
    case GALILEO_E_MARSHALLINGERROR:
        return "MARSHALLING ERROR";
    case GALILEO_E_UNIMPLEMENTED:
        return "UNIMPLEMENTED";
    case GALILEO_E_OPERATIONTIMEOUT:
        return "OPERATION TIMEOUT";
    case GALILEO_E_BADPARAM:
        return "BAD PARAMETER";
    case GALILEO_E_INVALIDSTATE:
        return "INVALID STATE";
    case GALILEO_E_APIERROR:
        return "API ERROR";
    case GALILEO_E_NONODE:
        return "NO NODE";
    case GALILEO_E_NOAUTH:
        return "NO AUTH";
    case GALILEO_E_BADVERSION:
        return "BAD VERSION";
    case GALILEO_E_CHILDFOREPHEM:
        return "NO CHILD FOR EPHEMERAL NODE";
    case GALILEO_E_NODEEXISTS:
        return "NODE EXISTS";
    case GALILEO_E_NOTEMPTY:
        return "NODE NOT EMPTY";
    case GALILEO_E_SESSIONEXPIRED:
        return "SESSION EXPIRED";
    case GALILEO_E_INVALIDCALLBACK:
        return "INVALID CALLBACK";
    case GALILEO_E_INVALIDACL:
        return "INVALID ACL";
    case GALILEO_E_AUTHFAILED:
        return "AUTH FAILED";
    case GALILEO_E_CLOSING:
        return "CLOSING";
    case GALILEO_E_NOTHING:
        return "NOTHING";
    case GALILEO_E_UNKNOWN:
        return "UNKNOW ERROR";
    default:
        return "UNKNOW ERROR";
    }
    return "UNKNOW ERROR";
}


void lleo_set_getchildren_retry_time( int new_retry_time, int new_retry_interval)
{
	if(new_retry_time < 0) {
		ul_writelog(UL_LOG_WARNING, "parameter new_retry_time : %d invalid", new_retry_time);
		return;
	}
	if(new_retry_interval < 0) {
		ul_writelog(UL_LOG_WARNING, "parameter new_retry_time : %d invalid", new_retry_interval);
		return;
	}
	
	
		
		
	
	default_max_retry_time	=	new_retry_time;
	default_retry_interval	=	new_retry_interval;
	return;
}

