#include <algorithm>
#include "zcacheapi.h"
#include "zcachedata.h"
#include "commwithms.h"
#include "commwithcs.h"
#include "BufHandle.h"
#include "util.h"
#include "asyncremoteinvoker.h"
#include "asynccond.h"
#include "log.h"
#include "atomic.h"
using namespace std;

#define  ZCACHE_ASYNC_RETRY_TIME    300     


static const char*op_codes[]={
    "",
    "ZCACHE_INSERT",
    "ZCACHE_DELETE",
    "ZCACHE_UPDATE",
    "ZCACHE_SEARCH"
};

const char *zcache_strerror(zcache_err_t err)
{
    return get_err_code(err);
}

inline int err_ret(zcache_err_t err)
{
    return(-1*err);
}

const char *zcache_strop(zcache_op_t optype)
{
    if (optype>ZCACHE_OP_NULL_BEGIN&&optype<ZCACHE_OP_NULL_END)
    {
        return op_codes[optype];
    }
    return "";
}

int zcache_status(zcache_t *zcache, unsigned int idx, unsigned int node_id, zcache_status_t *zcache_status)
{
    int ret = 0;

    if (zcache == NULL ||
            zcache_status == NULL ||
            idx >= zcache->redundant_count ||
            node_id >= zcache->zcaches[idx]->product->get_node_num()) {
        WARNING_LOG("params error");
        return err_ret(ZCACHE_ERR_PARAM);
    }
    return zcache->zcaches[idx]->product->GetNodeStatus(node_id, zcache_status);
}

int zcache_get_node_id(zcache_t *zcache, unsigned int idx, zcache_item_t *key)
{
    if (zcache == NULL || key == NULL || zcache->redundant_count <= idx) {
        WARNING_LOG("params error");
        return err_ret(ZCACHE_ERR_PARAM);
    }

    zcache_node_t node;
    return zcache->zcaches[idx]->product->GetNode(key->item, key->len, &node);
}


static void zcache_op_cb(async_remote_invoker_arg_t *arg);


static void zcache_op_async_cb(async_remote_invoker_arg_t *arg);


static int bufHandle_append_item(BufHandle *handle,char *buf,int len)
{
    if (bufHandle_append(handle,buf,len)<0)
    {
        int buflen=(len>BUFHANDLE_LEN)?len:BUFHANDLE_LEN;
        BufHandle *newhandle=bufHandle_new(buflen);
        if (newhandle==NULL)
        {
            WARNING_LOG("bufHandle_new error");
            return -1;
        }
        handle->_next=newhandle;
        return bufHandle_append(newhandle,buf,len);
    }
    return len;
}


static int bufHandle_append_cache(BufHandle *handle,
                                  char *product_name,
                                  zcache_op_t optype,
                                  zcache_item_t *key, zcache_item_t *slckey,
                                  zcache_item_t *value,
                                  unsigned int delay_time_ms)
{
    if (handle==NULL || key==NULL )
    {
        WARNING_LOG("param null,[optype:%d]",optype);
        return err_ret(ZCACHE_ERR_PARAM);
    }
    if (key->item==NULL || key->len > ZCACHE_MAX_KEY_LEN)
    {
        WARNING_LOG("key error,[len:%d,maxlen:%u]",key->len,ZCACHE_MAX_KEY_LEN);
        return err_ret(ZCACHE_ERR_PARAM);
    }
    if (slckey!=NULL && slckey->len>ZCACHE_MAX_KEY_LEN)
    {
        WARNING_LOG("slckey error,[len:%d,maxlen:%u]",
                    slckey->len,ZCACHE_MAX_KEY_LEN);
        return err_ret(ZCACHE_ERR_PARAM);
    }
    if ((optype==ZCACHE_INSERT || optype==ZCACHE_UPDATE)
        && (value==NULL || value->len>ZCACHE_MAX_VALUE_LEN || value->len==0 || value->item==NULL))
    {
        WARNING_LOG("value error,[op:%u]",optype);
        return err_ret(ZCACHE_ERR_PARAM);
    }

    cmd_head_t reqhead;
    zcache_cmd_lib_req_t libreq;
    unsigned int reqcmdlen = sizeof(zcache_cmd_lib_req_t);
    zcache_cmdtype_t cmdtype;

    switch (optype)
    {
    case ZCACHE_INSERT:
        cmdtype= ZCACHE_CMD_INSERT;
        libreq.value_len = value->len;
        break;
    case ZCACHE_UPDATE:
        cmdtype= ZCACHE_CMD_UPDATE;
        libreq.value_len = value->len;
        break;
    case ZCACHE_DELETE:
        cmdtype= ZCACHE_CMD_DELETE;
        libreq.value_len = 0;
        break;
    case ZCACHE_SEARCH:
        cmdtype= ZCACHE_CMD_SEARCH;
        libreq.value_len = 0;
        break;
    default:
        WARNING_LOG("error optype,[optype:%d]",optype);
        return err_ret(ZCACHE_ERR_PARAM);
    }

    
    snprintf(libreq.product,sizeof(libreq.product),"%s",product_name);
    libreq.gen_time = time_get_cur();
    
    libreq.delay_time = (unsigned long long)delay_time_ms;
    libreq.delay_time = libreq.delay_time*1000;
    libreq.key_len = key->len;
    libreq.slckey_len = (slckey==NULL||slckey->item==NULL)?0:slckey->len;
    reqcmdlen+=libreq.key_len+libreq.slckey_len+libreq.value_len;

    cmd_head_set(&reqhead,cmdtype,0,reqcmdlen);
    while (handle->_next!=NULL)
    {
        handle=handle->_next;
    }
    if (bufHandle_append_item(handle,(char*)&reqhead,sizeof(reqhead))<0)
    {
        return err_ret(ZCACHE_ERR_MEM);
    }
    while (handle->_next!=NULL)
    {
        handle=handle->_next;
    }
    if (bufHandle_append_item(handle,(char*)&libreq,sizeof(libreq))<0)
    {
        return err_ret(ZCACHE_ERR_MEM);
    }
    while (handle->_next!=NULL)
    {
        handle=handle->_next;
    }
    if (bufHandle_append_item(handle,key->item,key->len)<0)
    {
        return err_ret(ZCACHE_ERR_MEM);
    }
    while (handle->_next!=NULL)
    {
        handle=handle->_next;
    }
    if (libreq.slckey_len>0)
    {
        if (bufHandle_append_item(handle,slckey->item,slckey->len)<0)
        {
            return err_ret(ZCACHE_ERR_MEM);
        }
    }
    while (handle->_next!=NULL)
    {
        handle=handle->_next;
    }
    if (libreq.value_len>0)
    {
        if (bufHandle_append_item(handle,value->item,value->len)<0)
        {
            return err_ret(ZCACHE_ERR_MEM);
        }
    }
    return reqcmdlen+sizeof(cmd_head_t);
}

static int zcache_op_once(zcache_t *cache,
                          zcache_op_t optype,
                          zcache_item_t *key, zcache_item_t *slckey,
                          zcache_item_t *value,
                          unsigned int delay_time_ms,
                          zcache_err_t *err_out,
                          unsigned int logid, unsigned int timeout_ms,
                          unsigned int *left_time = NULL);

static single_zcache_t *_zcache_create(const char *service_addrs,
                                       const char *product_name,
                                       unsigned int max_fd_num,
                                       unsigned int max_fd_timeout_ms,
                                       unsigned int cscheck_time_us
                                      )
{
    if (service_addrs==NULL || product_name==NULL || product_name[0]=='\0' || max_fd_num==0 || max_fd_timeout_ms==0)
    {
        WARNING_LOG("param error");
        return NULL;
    }

    single_zcache_t *cache = zcachedata_create(cscheck_time_us);
    if (cache==NULL)
    {
        WARNING_LOG("new single_zcache_t error");
        goto err_exit;
    }

    if (cache->product->SetName(product_name) < 0)
    {
        goto err_exit;
    }
    if (zcache_load_msaddrs(cache,service_addrs)<0)
    {
        goto err_exit;
    }

    return cache;
err_exit:
    if (cache != NULL)
    {
        zcachedata_destroy(cache);
    }
    return NULL;
}



static void  _zcache_destroy(single_zcache_t *cache)
{
    if (cache != NULL )
    {
        zcachedata_destroy(cache);
    }
}

zcache_t * zcache_create(const char *service_addrs, const char *product_name, unsigned int max_fd_num,
                         unsigned max_fd_timeout_ms, unsigned cscheck_time_us)
{
    const char *tmp = service_addrs;
    int service_addrs_len = 0;
    int server_count = 0;
    if (service_addrs==NULL 
            || product_name==NULL 
            || product_name[0]=='\0' 
            || max_fd_num==0 
            || max_fd_timeout_ms==0)
    {
        WARNING_LOG("param error");
        return NULL;
    }

    if (strlen(product_name) >= (int)ZCACHE_MAX_PRODUCT_NAME_LEN)
    {
        WARNING_LOG("product_name too long [ZCACHE_MAX_PRODUCT_NAME_LEN:%d]",  ZCACHE_MAX_PRODUCT_NAME_LEN-1);
        return NULL;
    }

    if (cscheck_time_us < ZCACHEAPI_MIN_CSCHECK_TIME) {
        NOTICE_LOG("cscheck_time_us[%u] is smaller ther MIN CSCHECK TIME[%u], cache check will not perform",
                cscheck_time_us, ZCACHEAPI_MIN_CSCHECK_TIME);
    }

    service_addrs_len =  strlen(service_addrs);
    while (service_addrs[service_addrs_len - 1] == ';'
           ||service_addrs[service_addrs_len - 1] == ','
           ||isspace(service_addrs[service_addrs_len - 1]))
    {
        service_addrs_len --;
    }

    if (service_addrs_len < 9 || !isdigit(service_addrs[service_addrs_len-1]))
    {
        WARNING_LOG("service_addrs format error [addr:%s]", service_addrs);
        return NULL;
    }
    server_count = count(service_addrs, service_addrs+service_addrs_len, ';') + 1;

    zcache_t *zc = NULL;
    AsyncRemoteMgr *mgr=NULL;

    if ((zc = (zcache_t*)calloc(1, sizeof(zcache_t))) == NULL)
    {
        WARNING_LOG("fail to calloc zcache_t");
        goto err_exit;
    }
    if ((zc->zcaches = (single_zcache_t**)calloc(server_count, sizeof(void*)))
        == NULL)
    {
        WARNING_LOG("fail to calloc single_zcaches");
        goto err_exit;
    }
    zc->redundant_count = 0;

    if ((mgr = NEW AsyncRemoteMgr()) == NULL)
    {
        WARNING_LOG("fail to new AsyncRemoteMgr");
        goto err_exit;
    }
    if (mgr->Init(max_fd_timeout_ms,max_fd_num)<0)
    {
        WARNING_LOG("AsyncRemoteMgr::Init error");
        goto err_exit;
    }
    zc->mgr = mgr;

    for (int i = 0; i < server_count && tmp != NULL && tmp < service_addrs + service_addrs_len; i ++)
    {
        char cur_server_addr[64] ={0};
        const char *cur_addr_idx = tmp;
        tmp = strchr(cur_addr_idx, ';');
        if (tmp != NULL)
        {
            memcpy(cur_server_addr, cur_addr_idx, tmp-cur_addr_idx);
            tmp ++;
        }
        else
        {
            memcpy(cur_server_addr, cur_addr_idx, service_addrs + service_addrs_len - cur_addr_idx);
        }
        zc->zcaches[i] = _zcache_create(cur_server_addr, product_name, max_fd_num, max_fd_timeout_ms, cscheck_time_us);
        if (zc->zcaches[i] == NULL)
        {
            WARNING_LOG("fail to create [addr:%s]",  cur_server_addr);
            goto err_exit;
        }
        zc->zcaches[i]->mgr = mgr;
        zc->redundant_count ++;
    }
    if (zc->redundant_count == 0)
    {
        WARNING_LOG("there is no metaserver addr in server addrs [addr:%s]",  service_addrs);
        goto err_exit;
    }
    DEBUG_LOG("create zcache_t with [redundances:%u]",  zc->redundant_count);
    return zc;
err_exit:
    if (zc != NULL)
    {
        for (int i = 0; i < server_count && zc->zcaches[i] != NULL; i ++)
        {
            _zcache_destroy(zc->zcaches[i]);
            zc->zcaches[i] = NULL;
        }
        if (zc->zcaches != NULL)
        {
            UL_SAFEFREE(zc->zcaches);
            UL_SAFEFREE(zc);
        }
    }
    if (mgr != NULL)
    {
        UL_SAFEDELETE(mgr);
    }
    return NULL;
}

void zcache_destroy(zcache_t *cache)
{
    if (cache == NULL)
    {
        return;
    }
    DEBUG_LOG("zcache_destroy [addr:%p]", cache);
    
    if (cache->zcaches != NULL)
    {
        for (unsigned int i = 0; i < cache->redundant_count; i ++)
        {
            if (cache->zcaches[i] != NULL)
            {
                _zcache_destroy(cache->zcaches[i]);
                cache->zcaches[i] = NULL;
            }
        }
        UL_SAFEFREE(cache->zcaches);
    }
    if (cache->mgr != NULL)
    {
        UL_SAFEDELETE(cache->mgr);
    }
    UL_SAFEFREE(cache);
    return;
}

static int _zcache_auth(single_zcache_t *cache,const char *token,bool isblock,zcache_err_t *err_out)
{
    zcache_err_t zcerr;
    int ret=0;
    if (err_out == NULL)
    {
        err_out=&zcerr;
    }

    *err_out=ZCACHE_OK;

    if (cache== NULL)
    {
        *err_out = ZCACHE_ERR_PARAM;
        return -1;
    }
    else if (cache->product == NULL)
    {
        *err_out = ZCACHE_ERR_LIB;
        return -1;
    }
    else if (cache->product->SetToken(token)<0)
    {
        *err_out = ZCACHE_ERR_PARAM;
        return -1;
    }
    if (isblock)
    {
        if (cache->is_block_fetched==false)
        {
            ret=get_all_meta(cache,err_out);
            if (ret<0)
            {
                return ret;
            }
            cache->is_block_fetched=true;
        }
        else
        {
            ret=auth_with_meta(cache,err_out);
            if (ret<0)
            {
                return ret;
            }
        }
    }

    if (cache->update_thread==0)
    {
        ret=pthread_create(&cache->update_thread,NULL,block_update_routine,cache);
        if (ret<0)
        {
            *err_out = ZCACHE_ERR_LIB;
            goto out;
        }

        
        if (cache->cscheck_time_us >= ZCACHEAPI_MIN_CSCHECK_TIME) {
            ret = pthread_create(&cache->cscheck_thread, NULL, cs_check, cache);
            if (ret < 0)
            {
                *err_out = ZCACHE_ERR_LIB;
                goto out;
            }
        }
    }

out:
    return ret;
}

int zcache_auth(zcache_t *cache,const char *token,bool isblock,zcache_err_t *err_out)
{
    single_zcache_t *szc = NULL;
    zcache_err_t zcerr;
    int ret=0;
    if (err_out == NULL)
    {
        err_out=&zcerr;
    }

    *err_out=ZCACHE_OK;

    if (cache== NULL || cache->redundant_count == 0)
    {
        *err_out = ZCACHE_ERR_PARAM;
        return -1;
    }
    for (unsigned int i = 0; i < cache->redundant_count; i++)
    {
        if (cache->zcaches[i] == NULL
            || cache->zcaches[i]->product == NULL)
        {
            *err_out == ZCACHE_ERR_PARAM;
            return -1;
        }
    }

    for (unsigned int i = 0; i < cache->redundant_count; i ++)
    {
        if (_zcache_auth(cache->zcaches[i], token, isblock, err_out) != 0)
        {
            WARNING_LOG("_zcache_auth error");
            return -1;
        }
    }
    return 0;
}

int zcache_set_local_cache(zcache_t *cache,
                           unsigned int max_local_cache_num,
                           unsigned int min_busy_count)
{
    if (cache)
    {
        for (unsigned int i = 0; i < cache->redundant_count; i ++)
        {
            if (cache->zcaches[i]->localcache->Init(max_local_cache_num, min_busy_count) != 0)
            {
                return -1;
            }
        }
        return 0;
    }
    else
    {
        return -1;
    }
}

void zcache_set_param(zcache_t *cache,
                      unsigned int connect_timeout_ms,
                      unsigned int ms_read_timeout_ms,
                      unsigned int ms_write_timeout_ms,
                      unsigned int ms_refetch_time_ms,
                      unsigned int cscheck_time_us)
{
    if (cache)
    {
        for (unsigned int i = 0; i < cache->redundant_count;i ++)
        {
            cache->zcaches[i]->connect_timeout_ms = connect_timeout_ms;
            cache->zcaches[i]->ms_read_timeout_ms = ms_read_timeout_ms;
            cache->zcaches[i]->ms_write_timeout_ms = ms_write_timeout_ms;
            cache->zcaches[i]->ms_refetch_time_ms = ms_refetch_time_ms;
            if (cache->zcaches[i]->cscheck_time_us >= ZCACHEAPI_MIN_CSCHECK_TIME) {
                if (cscheck_time_us < ZCACHEAPI_MIN_CSCHECK_TIME) {
                    WARNING_LOG("cscheck_time_us[%u] is smaller ther [%u], won't take effect",
                            cscheck_time_us, ZCACHEAPI_MIN_CSCHECK_TIME);
                } else {
                    cache->zcaches[i]->cscheck_time_us = cscheck_time_us;
                }
            }
        }
        cache->mgr->SetConnTimeout(connect_timeout_ms);
    }
}

int zcache_search(zcache_t *cache,
                  zcache_item_t *key, zcache_item_t *slckey,
                  zcache_item_t *value_out, zcache_err_t *err_out,
                  unsigned int logid,
                  unsigned int timeout_ms,
                  unsigned int *left_time_ms)
{
    return zcache_op_once(cache,
                          ZCACHE_SEARCH,
                          key, slckey,
                          value_out,
                          0,
                          err_out,
                          logid,
                          timeout_ms,
                          left_time_ms);
}



int zcache_insert(zcache_t *cache,
                  zcache_item_t *key, zcache_item_t *slckey,
                  zcache_item_t *value,
                  unsigned int delay_time_ms,
                  zcache_err_t *err_out,
                  unsigned int logid,
                  unsigned int timeout_ms)
{
    return zcache_op_once(cache,
                          ZCACHE_INSERT,
                          key, slckey,
                          value,
                          delay_time_ms,
                          err_out,
                          logid,
                          timeout_ms);
}

int zcache_update(zcache_t *cache,
                  zcache_item_t *key, zcache_item_t *slckey,
                  zcache_item_t *value,
                  unsigned int delay_time_ms,
                  zcache_err_t *err_out,
                  unsigned int logid,
                  unsigned int timeout_ms)
{
    return zcache_op_once(cache,
                          ZCACHE_UPDATE,
                          key, slckey,
                          value,
                          delay_time_ms,
                          err_out,
                          logid,
                          timeout_ms);
}



int zcache_delete(zcache_t *cache,
                  zcache_item_t *key, zcache_item_t *slckey,
                  unsigned int delay_time_ms,
                  zcache_err_t *err_out,
                  unsigned int logid,
                  unsigned int timeout_ms)
{
    return zcache_op_once(cache,ZCACHE_DELETE,key,slckey,NULL,delay_time_ms,
                          err_out,logid,timeout_ms);
}

typedef struct _zcache_node_item_t
{
    unsigned int msgbodylen;                                            
    unsigned int cmdnum;                                                
    int indx;                                                           
    bool isfinished;                                                    
    bool isnodebusy;                                                    
    async_remote_invoker_arg_t arg;
    DLINK head;
    DLINK link;
}zcache_node_item_t;


int _zcache_op(single_zcache_t *cache, unsigned int count,
               zcache_op_t **optypes,
               zcache_item_t **keys, zcache_item_t **slckeys,
               zcache_item_t **values,
               unsigned int **delay_time_mss,
               zcache_err_t **errs,
               unsigned int logid, unsigned int timeout_ms,
               unsigned int **left_time_mss)
{
    zlog_set_thlogid(logid);
    if (cache==NULL || count==0 || optypes==NULL
        || keys==NULL || slckeys==NULL || values==NULL ||errs==NULL
        || timeout_ms==0 || delay_time_mss == NULL )
    {
        WARNING_LOG("param error");
        return err_ret(ZCACHE_ERR_PARAM);
    }
    else if (count > ZCACHE_MAX_OP_NUM)
    {
        WARNING_LOG("too large count,[count:%u]",count);
        return err_ret(ZCACHE_ERR_PARAM);
    }
    else
    {
        for (unsigned int i=0;i<count;i++)
        {
            if (*(optypes[i])<=ZCACHE_OP_NULL_BEGIN||*(optypes[i])>=ZCACHE_OP_NULL_END)
            {
                WARNING_LOG("error,[i:%u,op:%u]",i,*(optypes[i]));
                return err_ret(ZCACHE_ERR_PARAM);
            }
            else if (keys[i]->item==NULL||keys[i]->len==0||keys[i]->len>ZCACHE_MAX_KEY_LEN)
            {
                WARNING_LOG("key error,[i:%u,keylen:%u]",i,keys[i]->len);
                return err_ret(ZCACHE_ERR_PARAM);
            }
            else if ((*(optypes[i])==ZCACHE_INSERT||*(optypes[i])==ZCACHE_UPDATE)
                     &&(values[i]->item==NULL||values[i]->len==0||values[i]->len>ZCACHE_MAX_VALUE_LEN))
            {
                WARNING_LOG("value error,[i:%u,optype:%s,valuelen:%u]",i,zcache_strop(*(optypes[i])),values[i]->len);
                return err_ret(ZCACHE_ERR_PARAM);
            }
        }
    }

    
    
    
    

    DEBUG_LOG("start,[count:%u]",count);
    AsyncCond *cond=NULL;
    zcache_node_item_t *nodes=NULL;
    zcache_node_item_t *item=NULL;
    zcache_node_item_t *item2=NULL;
    DLINK *itemlink=NULL;
    DLINK *itemhead=NULL;
    zcache_head_t *msghead=NULL;
    zcache_node_t node;
    int ret=0;
    DLINK head;
    int listcount=0;

    DLINK_INITIALIZE(&head);

    nodes=NEW zcache_node_item_t[count];
    if (nodes==NULL)
    {
        WARNING_LOG("new zcache_node_item_t error,[count:%u]",count);
        ret = err_ret(ZCACHE_ERR_MEM);
        goto err_exit;
    }
    memset(nodes,0,sizeof(zcache_node_item_t)*count);

    cond=NEW AsyncCond;
    if (cond==NULL)
    {
        WARNING_LOG("new AsyncCond error");
        ret = err_ret(ZCACHE_ERR_MEM);
        goto err_exit;
    }

    
    for (unsigned int i=0;i<count;i++)
    {
        *(errs[i]) = ZCACHE_ERR_LIB;
        if (cache->product->GetNode(keys[i]->item,keys[i]->len,&node)<0)
        {
            
            *(errs[i])=ZCACHE_ERR_BLOCK_NOT_EXIST;
        }
        else
        {
            nodes[i].indx=i;
            nodes[i].arg.arg=cond;
            nodes[i].arg.logid=logid;
            nodes[i].arg.status=ZCACHE_ERR_LIB;
            nodes[i].isfinished=false;

            itemhead=head._next;
            while (itemhead!=&head)
            {
                item=CONTAINING_RECORD(itemhead,zcache_node_item_t,head);
                if (item->arg.ip[0]==node.ip[0]
                    &&item->arg.ip[1]==node.ip[1]
                    &&item->arg.port==node.port)
                {
                    
                    DLINK_INSERT_PREV(&item->link,&nodes[i].link);
                    
                    break;
                }
                itemhead=itemhead->_next;
            }
            if (itemhead==&head)
            {
                
                DLINK_INSERT_NEXT(&head,&nodes[i].head);
                DLINK_INITIALIZE(&nodes[i].link);
                nodes[i].arg.timeout_ms=timeout_ms;
                nodes[i].arg.ip[0]=node.ip[0];
                nodes[i].arg.ip[1]=node.ip[1];
                nodes[i].arg.port=node.port;
                listcount++;
            }
        }
    }

    
    itemhead=head._next;
    while (itemhead!=&head)
    {
        item=CONTAINING_RECORD(itemhead,zcache_node_item_t,head);
        
        item->isnodebusy=cache->localcache->IsNodeBusy(item->arg.ip,item->arg.port);
        itemlink=&item->link;
        do
        {
            
            item2=CONTAINING_RECORD(itemlink,zcache_node_item_t,link);
            if (item->isnodebusy
                &&(*(optypes[item2->indx])==ZCACHE_SEARCH||*(optypes[item2->indx])==ZCACHE_INSERT))
            {
                
                if (*(optypes[item2->indx])==ZCACHE_SEARCH)
                {
                    if (cache->localcache->Get(keys[item2->indx],slckeys[item2->indx],values[item2->indx])==ZCACHE_OK)
                    {
                        *(errs[item2->indx])=ZCACHE_OK;
                        item2->isfinished=true;
                        if (left_time_mss != NULL)
                        {
                            *(left_time_mss[item2->indx]) = ZCACHEAPI_LOCAL_TIMEOUT_MS;
                        }
                        DEBUG_LOG("Get value from locale cache [key_idx:%d]", item2->indx);
                    }
                    else
                    {
                        
                    }
                }
                else
                {
                    *(errs[item2->indx])=cache->localcache->Add(keys[item2->indx],slckeys[item2->indx],values[item2->indx]);
                    item2->isfinished=true;
                    DEBUG_LOG("insert to locale cache [key_idx:%d]", item2->indx);
                }

            }
            if (!item2->isfinished)
            {
                ret=bufHandle_append_cache(&item->arg.req_handle,
                                           cache->product->GetName(),
                                           *(optypes[item2->indx]),keys[item2->indx],slckeys[item2->indx],
                                           values[item2->indx],*(delay_time_mss[item2->indx]));
                if (ret<0)
                {
                    WARNING_LOG("bufHandle_append_cache error");
                    goto err_exit;
                }
                item->msgbodylen+=ret;
                item->cmdnum++;
            }
            itemlink=itemlink->_next;
        }while (itemlink!=&item->link);

        if (item->cmdnum>0)
        {
            
            item->arg.req_handle.pBuf=NEW char[sizeof(zcache_head_t)];
            item->arg.res_handle.pBuf=NEW char[sizeof(zcache_head_t)];
            if (item->arg.req_handle.pBuf==NULL||item->arg.res_handle.pBuf==NULL)
            {
                WARNING_LOG("new pBuf error");
                ret = ZCACHE_ERR_MEM;
                goto err_exit;
            }
            item->arg.req_handle.nBufLen=sizeof(zcache_head_t);
            item->arg.req_handle.nDataLen=sizeof(zcache_head_t);
            item->arg.res_handle.nBufLen=sizeof(zcache_head_t);
            msghead=(zcache_head_t*)(item->arg.req_handle.pBuf);
            lnhead_set(&(msghead->msghead),logid,ZCACHEAPI_MODULE,item->msgbodylen+sizeof(zcache_head_t)-sizeof(lnhead_t));
            msghead->cmd_num=item->cmdnum;
            DEBUG_LOG("invoker,[ip0:%u,ip1:%u,bodylen:%u]",
                      item->arg.ip[0],item->arg.ip[1],item->msgbodylen+(unsigned int)(sizeof(zcache_head_t)-sizeof(lnhead_t)));
            item->arg.timeout_ms = timeout_ms;
            ret=cache->mgr->AsyncRemoteInvoker(&item->arg,zcache_op_cb);
            if (ret==0)
            {
                cond->AddWait(1);
            }
            else
            {
                ret = 0;
                WARNING_LOG("AsyncRemoteInvoker error");
                break;
            }
        }
        itemhead=itemhead->_next;
    }

    
    cond->Wait();
    DEBUG_LOG("cond signaled");
    
    itemhead=head._next;
    
    while (itemhead!=&head)
    {
        item=CONTAINING_RECORD(itemhead,zcache_node_item_t,head);
        itemlink=&item->link;
        if (item->arg.status!=ZCACHE_OK)
        {
            do
            {
                item2=CONTAINING_RECORD(itemlink,zcache_node_item_t,link);
                if (!item2->isfinished)
                {
                    *(errs[item2->indx])=(zcache_err_t)item->arg.status;
                    cache->product->SetError(keys[item2->indx]->item,keys[item2->indx]->len,false);
                }
                itemlink=itemlink->_next;
            }while (itemlink!=&item->link);
        }
        else
        {
            if (item->arg.res_handle._next==NULL||item->arg.res_handle._next->pBuf==NULL)
            {
                WARNING_LOG("res_handle NULL");
                ret = ZCACHE_ERR_LIB;
                goto err_exit;
            }
            else if (item->arg.res_handle.nDataLen!=sizeof(zcache_head_t))
            {
                ret = ZCACHE_ERR_CACHESERVER;
                WARNING_LOG("res_handle.nDataLen error,[nDataLen:%d,nBuflen:%d]",
                            item->arg.res_handle.nDataLen,item->arg.res_handle.nBufLen);
                goto err_exit;
            }
            zcache_head_t *reshead=(zcache_head_t*)(item->arg.req_handle.pBuf);
            if (reshead->cmd_num!=item->cmdnum)
            {
                ret = ZCACHE_ERR_CACHESERVER;
                WARNING_LOG("cmdnum error,[reshead->cmd_num:%u,item->cmdnum:%u]",
                            reshead->cmd_num,item->cmdnum);
                goto err_exit;
            }
            cmd_head_t *cmdindx=NULL;
            int leftlen=item->arg.res_handle._next->nDataLen;
            
            do
            {
                if (leftlen<(int)sizeof(cmd_head_t))
                {
                    ret = ZCACHE_ERR_CACHESERVER;
                    WARNING_LOG("leftlen error,[leftlen:%d]",leftlen);
                    goto err_exit;
                }
                cmdindx=(cmd_head_t*)(item->arg.res_handle._next->pBuf+item->arg.res_handle._next->nDataLen-leftlen);
                item2=CONTAINING_RECORD(itemlink,zcache_node_item_t,link);
                *(errs[item2->indx])=(zcache_err_t)cmdindx->cmd_ret;
                if (cmdindx->cmd_ret==ZCACHE_ERR_BUSY)
                {
                    cache->localcache->AddNode(item->arg.ip,item->arg.port);
                }
                else if (cmdindx->cmd_ret==ZCACHE_ERR_BLOCK_NOT_EXIST)
                {
                    cache->product->SetError(keys[item2->indx]->item,keys[item2->indx]->len,true);
                }
                else if (item->isnodebusy&&cmdindx->cmd_ret==ZCACHE_OK)
                {
                    cache->localcache->RemoveNode(item->arg.ip,item->arg.port);
                }
                leftlen-=sizeof(cmd_head_t);
                
                if (*(optypes[item2->indx])==ZCACHE_SEARCH&&cmdindx->cmd_ret==ZCACHE_OK)
                {
                    if ((int)cmdindx->body_len>leftlen)
                    {
                        ret = ZCACHE_ERR_CACHESERVER;
                        WARNING_LOG("error,[body_len:%u,leftlen:%d]",cmdindx->body_len,leftlen);
                        goto err_exit;
                    }
                    else if (cmdindx->body_len==0)
                    {
                        
                        WARNING_LOG("search with no data");
                        *(errs[item2->indx])=ZCACHE_ERR_CACHESERVER;
                    }
                    else
                    {
                        zcache_cmd_lib_search_res_t *tres = (zcache_cmd_lib_search_res_t*)(cmdindx+1);
                        if (cmdindx->body_len - sizeof(zcache_cmd_lib_search_res_t)>values[item2->indx]->len)
                        {
                            *(errs[item2->indx])=ZCACHE_ERR_BUF_NOT_ENOUGH;
                            values[item2->indx]->len=cmdindx->body_len - sizeof(zcache_cmd_lib_search_res_t);
                        }
                        else
                        {
                            memcpy(values[item2->indx]->item,cmdindx->cmd_body + sizeof(zcache_cmd_lib_search_res_t),
                                   cmdindx->body_len - sizeof(zcache_cmd_lib_search_res_t));
                            values[item2->indx]->len=cmdindx->body_len - sizeof(zcache_cmd_lib_search_res_t);
                        }
                        if (left_time_mss != NULL)
                        {
                            if (tres->left_time > 0 && tres->left_time < 1000)
                            {
                                *(left_time_mss[item2->indx]) = 1;
                            }
                            else if (tres->left_time >= 1000)
                            {
                                *(left_time_mss[item2->indx]) = tres->left_time/1000;
                            }
                            else
                            {
                                *(left_time_mss[item2->indx]) = 0;
                            }
                        }
                    }
                }
                leftlen-=cmdindx->body_len;
                itemlink=itemlink->_next;
            }while (itemlink!=&item->link);
        }
        itemhead=itemhead->_next;
    }
    
    delete cond;
    if (nodes)
    {
        for (unsigned int i=0;i<count;i++)
        {
            bufHandle_free(nodes[i].arg.req_handle._next);
            delete[] nodes[i].arg.req_handle.pBuf;
            bufHandle_free(nodes[i].arg.res_handle._next);
            delete[] nodes[i].arg.res_handle.pBuf;
        }
    }
    delete[] nodes;
    for (unsigned int i=0;i<count;i++)
    {
        TRACE_LOG("[op:%s,err:%s]",zcache_strop(*(optypes[i])),zcache_strerror(*(errs[i])));
    }
    return 0;

err_exit:
    if (cond)
    {
        cond->Wait();
    }
    delete cond;
    if (nodes)
    {
        for (unsigned int i=0;i<count;i++)
        {
            bufHandle_free(nodes[i].arg.req_handle._next);
            delete[] nodes[i].arg.req_handle.pBuf;
            bufHandle_free(nodes[i].arg.res_handle._next);
            delete[] nodes[i].arg.res_handle.pBuf;
        }
    }
    delete[] nodes;
    return ret;
}

typedef struct _zcache_op_async_redundance_arg_t
{
    zcache_t *cache;                                                    
    zcache_op_t optype;                                                 
    zcache_item_t *key;                                                 
    zcache_item_t *slckey;                                              
    zcache_item_t *value;                                               
    unsigned int delay_time_ms;                                         
    unsigned int logid;                                                 
    unsigned int timeout_ms;                                            
    volatile int err_out;                                               
    volatile int tried_zcache_num;                                      
    unsigned int started_num;                                           
    unsigned long long begin_time;                                      
    pthread_mutex_t    started_mutex;                                   
    zcache_op_cb_t cb;                                                  
    void *arg;                                                          
    bool         is_single;                                             
    bool         is_delayed;                                            
}zcache_op_async_redundance_arg_t;

typedef struct _zcache_op_async_arg_t
{
    zcache_err_t cur_err;                                               
    int          cur_idx:16;                                            
    int          beg_idx:16;                                            
    zcache_op_async_redundance_arg_t *rd_arg;                           
}zcache_op_async_arg_t;

static int _zcache_op_async(zcache_op_async_arg_t *oparg);

void zcache_fill_cb(zcache_t *cache,
                    zcache_err_t err,
                    unsigned int logid,
                    zcache_op_t optype,
                    zcache_item_t *key, zcache_item_t *slckey,
                    zcache_item_t *value,
                    void *arg, unsigned int left_time_ms)
{
    zlog_set_thlogid(logid);
    if (err != ZCACHE_OK)
    {
        WARNING_LOG("zcache_fill error");
    }
    UL_SAFEFREE(key);
    UL_SAFEFREE(slckey);
    UL_SAFEFREE(value);
}

static void zcache_fill(zcache_t *cache, zcache_item_t *key, zcache_item_t *slckey,
                        zcache_item_t *value, unsigned int delay_time_ms, unsigned int logid,
                        unsigned int timeout_ms, int begin_idx, int searched_idx)
{
    zlog_set_thlogid(logid);
    if (cache == NULL )
    {
        WARNING_LOG("cache is NULL");
        return;
    }
    else if (key == NULL || key->item == NULL || key->len == 0)
    {
        WARNING_LOG("key error");
        return;
    }
    else if (value == NULL || value->item == NULL || value->len == 0)
    {
        WARNING_LOG("value error");
        return;
    }
    while (searched_idx != begin_idx)
    {
        zcacheapi_node_t *mtsv_node = NULL;
        searched_idx = (searched_idx - 1 >= 0)?(searched_idx - 1):(cache->redundant_count - 1);

        zcache_op_async_arg_t *oparg = NULL;
        zcache_item_t *tkey = NULL;
        zcache_item_t *tslckey = NULL;
        zcache_item_t *tvalue = NULL;
        tkey = (zcache_item_t*)calloc(1,  sizeof(zcache_item_t) + key->len);
        if (slckey != NULL && slckey->item != NULL && slckey->len != 0)
        {
            tslckey = (zcache_item_t*)calloc(1,  sizeof(zcache_item_t) + slckey->len);
            if (tslckey == NULL)
            {
                WARNING_LOG("malloc error");
                goto err_exit;
            }
        }
        tvalue = (zcache_item_t*)calloc(1,  sizeof(zcache_item_t) + value->len);
        oparg  = (zcache_op_async_arg_t*)calloc(1, sizeof(zcache_op_async_arg_t)+sizeof(zcache_op_async_redundance_arg_t));
        if (tkey == NULL || tvalue == NULL || oparg == NULL)
        {
            WARNING_LOG("malloc error");
            goto err_exit;
        }
        if (key != NULL && key->item != NULL && key->len > 0)
        {
            tkey->len = key->len;
            tkey->item = (char*)(tkey + 1);
            memcpy(tkey->item, key->item, key->len);
        }
        if (slckey != NULL && slckey->item != NULL  && slckey->len > 0)
        {
            tslckey->len = slckey->len;
            tslckey->item = (char*)(tslckey + 1);
            memcpy(tslckey->item, slckey->item, slckey->len);
        }
        if (value != NULL && value->item != NULL && value->len > 0)
        {
            tvalue->len = value->len;
            tvalue->item = (char*)(tvalue + 1);
            memcpy(tvalue->item, value->item, value->len);
        }

        oparg->rd_arg = (zcache_op_async_redundance_arg_t*)(oparg+1);
        oparg->cur_err = ZCACHE_ERR_END;
        oparg->beg_idx = searched_idx;
        oparg->cur_idx = searched_idx;
        oparg->rd_arg->arg = NULL;
        oparg->rd_arg->begin_time = time_get_cur();
        oparg->rd_arg->cache = cache;
        oparg->rd_arg->cb = zcache_fill_cb;
        oparg->rd_arg->delay_time_ms = delay_time_ms;
        oparg->rd_arg->err_out = ZCACHE_ERR_END;
        oparg->rd_arg->is_single = true;
        oparg->rd_arg->key = tkey;
        oparg->rd_arg->logid = logid;
        oparg->rd_arg->optype = ZCACHE_INSERT;
        oparg->rd_arg->slckey = tslckey;
        oparg->rd_arg->timeout_ms = timeout_ms;
        oparg->rd_arg->tried_zcache_num = 0;
        oparg->rd_arg->value = tvalue;
        oparg->rd_arg->started_num = 1;
        oparg->rd_arg->is_delayed = false;
        pthread_mutex_init(&(oparg->rd_arg->started_mutex), NULL);
        if (_zcache_op_async(oparg) < 0)
        {
            WARNING_LOG("_zcache_op_async error");
            goto err_exit;
        }
        mtsv_node = CONTAINING_RECORD(cache->zcaches[searched_idx]->mslist_head._next, zcacheapi_node_t,link);
        DEBUG_LOG("try to fill zcache [mtsv_addr:%s, mtsv_port:%u]", mtsv_node->ip, mtsv_node->port);
        continue;
err_exit:
        UL_SAFEFREE(tkey);
        UL_SAFEFREE(tslckey);
        UL_SAFEFREE(tvalue);
        if (oparg != NULL)
        {
            if (oparg->rd_arg != NULL)
            {
                pthread_mutex_destroy(&(oparg->rd_arg->started_mutex));
            }
            UL_SAFEFREE(oparg);
        }
        break;
    }
}

int zcache_op(zcache_t *cache, unsigned int count,
              zcache_op_t *optypes,
              zcache_item_t *keys, zcache_item_t *slckeys,
              zcache_item_t *values,
              unsigned int* delay_time_mss,
              zcache_err_t *errs,
              unsigned int logid, unsigned int timeout_ms,
              unsigned int *left_time_mss)
{
    int ret = 0;
    unsigned int org_timeoutms = timeout_ms;
    unsigned int left_net_timeoutms = timeout_ms;
    zlog_set_thlogid(logid);
    if (cache==NULL || count==0 || optypes==NULL
        || keys==NULL || slckeys==NULL || values==NULL ||errs==NULL
        || timeout_ms==0 || delay_time_mss == NULL )
    {
        WARNING_LOG("param error");
        return err_ret(ZCACHE_ERR_PARAM);
    }
    else if (count > ZCACHE_MAX_OP_NUM)
    {
        WARNING_LOG("too large count,[count:%u]",count);
        return err_ret(ZCACHE_ERR_PARAM);
    }
    else
    {
        unsigned int value_length_sum = 0;
        for (unsigned int i=0;i<count;i++)
        {
            if (optypes[i]<=ZCACHE_OP_NULL_BEGIN||optypes[i]>=ZCACHE_OP_NULL_END)
            {
                WARNING_LOG("error,[i:%u,op:%u]",i,optypes[i]);
                return err_ret(ZCACHE_ERR_PARAM);
            }
            else if (keys[i].item==NULL||keys[i].len==0||keys[i].len>ZCACHE_MAX_KEY_LEN)
            {
                WARNING_LOG("key error,[i:%u,keylen:%u]",i,keys[i].len);
                return err_ret(ZCACHE_ERR_PARAM);
            }
            else if ((optypes[i]==ZCACHE_INSERT||optypes[i]==ZCACHE_UPDATE)
                     &&(values[i].item==NULL||values[i].len==0||values[i].len>ZCACHE_MAX_VALUE_LEN))
            {
                WARNING_LOG("value error,[i:%u,optype:%s,valuelen:%u]",i,zcache_strop(optypes[i]),values[i].len);
                return err_ret(ZCACHE_ERR_PARAM);
            }
            else if (optypes[i] == ZCACHE_SEARCH
                     && (values[i].item == NULL  || values[i].len == 0))
            {
                WARNING_LOG("value error,[i:%u,optype:%s,valuelen:%u, valueitem:%p]",i,zcache_strop(optypes[i]),
                            values[i].len, values[i].item);
                return err_ret(ZCACHE_ERR_PARAM);
            }
            if (optypes[i] == ZCACHE_INSERT || optypes[i] == ZCACHE_UPDATE)
            {
                if (values[i].item != NULL)
                {
                    value_length_sum += values[i].len;
                }
            }
        }
        if (value_length_sum > ZCACHE_MAX_VALUE_LEN)
        {
            WARNING_LOG("the length of the value is too large [value_length_sum:%d, all_max_length:%d]",
                        value_length_sum, ZCACHE_MAX_VALUE_LEN);
            return err_ret(ZCACHE_ERR_PARAM);
        }
    }
    char *tmp = NULL;
    char *idx = NULL;
    unsigned int calloc_size = 0;
    zcache_op_t   **locale_optypes              = NULL;
    zcache_item_t **locale_keys                 = NULL;
    zcache_item_t **locale_slckeys              = NULL;
    zcache_item_t **locale_values               = NULL;
    unsigned int  **locale_delay_time_mss       = NULL;
    zcache_err_t  **locale_errs                 = NULL;
    unsigned int  **locale_left_time_mss        = NULL;
    unsigned char *success_count                = NULL;
    int begin_index = 0;
    int cur_index = 0;
    int left_count = 0;
    unsigned int tried_zcache_num = 0;
    if (left_time_mss == NULL)
    {
        calloc_size = sizeof(void*)*7 + sizeof(unsigned int)+ sizeof(char);
    }
    else
    {
        calloc_size = sizeof(void*)*7 + sizeof(char);
    }
    tmp = (char*)calloc(count, calloc_size);
    if (tmp == NULL)
    {
        WARNING_LOG("calloc error");
        ret = err_ret(ZCACHE_ERR_MEM);
        goto err_exit;
    }

    idx  = tmp;
    if (left_time_mss == NULL)
    {
        left_time_mss = (unsigned int*)idx;
        idx += sizeof(unsigned int)*count;
    }

    locale_optypes = (zcache_op_t**)idx;
    idx += sizeof(void*)*count;
    locale_keys = (zcache_item_t**)idx;
    idx += sizeof(void*)*count;
    locale_slckeys = (zcache_item_t**)idx;
    idx += sizeof(void*)*count;
    locale_values = (zcache_item_t**)idx;
    idx += sizeof(void*)*count;
    locale_delay_time_mss = (unsigned int **)idx;
    idx += sizeof(void*)*count;
    locale_errs = (zcache_err_t **)idx;
    idx += sizeof(void*)*count;
    locale_left_time_mss = (unsigned int **)idx;
    idx += sizeof(void*)*count;
    success_count = (unsigned char*)idx;
    idx += sizeof(char)*count;

    for (unsigned int i = 0; i < count; i ++)
    {
        errs[i] = ZCACHE_ERR_END;
        locale_optypes[i]           = optypes + i;
        locale_keys[i]              = keys + i;
        locale_slckeys[i]           = slckeys + i;
        locale_values[i]            = values + i;
        locale_delay_time_mss[i]    = delay_time_mss + i;
        locale_errs[i]              = errs + i;
        locale_left_time_mss[i]     = left_time_mss + i;
    }
    left_count = count;
    begin_index = random()%(cache->redundant_count);
    cur_index = begin_index;
    while (left_count > 0 && tried_zcache_num < cache->redundant_count && left_net_timeoutms > 0)
    {
        unsigned long long begin_time = time_get_cur();
        long long int used_time = 0;
        int used_locale_slot = 0;
        timeout_ms = org_timeoutms/(cache->redundant_count);
        if (timeout_ms == 0)
        {
            timeout_ms = org_timeoutms;
        }
        zcacheapi_node_t *mtsv_node = NULL;
        if ((ret = _zcache_op(cache->zcaches[cur_index], left_count, locale_optypes, locale_keys, locale_slckeys,
                              locale_values, locale_delay_time_mss, locale_errs, logid, timeout_ms,
                              locale_left_time_mss)) < 0)
        {
            WARNING_LOG("_zcache_op error");
            goto err_exit;
        }
        mtsv_node = CONTAINING_RECORD(cache->zcaches[cur_index]->mslist_head._next, zcacheapi_node_t,link);
        DEBUG_LOG("_zcache_op on [mtsv_ip:%s, mtsv_port:%u, left_count:%d]", mtsv_node->ip, mtsv_node->port, left_count);

        for (int i = 0; i < left_count; i ++)
        {
            switch (*(locale_optypes[i]))
            {
            case ZCACHE_SEARCH:
                if (*(locale_errs[i]) != ZCACHE_OK && *(locale_errs[i]) != ZCACHE_ERR_BUF_NOT_ENOUGH)
                {
                    locale_optypes[used_locale_slot]           =    locale_optypes[i]         ;
                    locale_keys[used_locale_slot]              =    locale_keys[i]            ;
                    locale_slckeys[used_locale_slot]           =    locale_slckeys[i]         ;
                    locale_values[used_locale_slot]            =    locale_values[i]          ;
                    locale_delay_time_mss[used_locale_slot]    =    locale_delay_time_mss[i]  ;
                    locale_errs[used_locale_slot]              =    locale_errs[i]            ;
                    locale_left_time_mss[used_locale_slot]     =    locale_left_time_mss[i]   ;
                    used_locale_slot ++;
                }
                else if (*(locale_errs[i]) == ZCACHE_OK && cur_index != begin_index)
                {
                    
                    if (*(locale_left_time_mss[i]) > 1 || *(locale_left_time_mss[i]) == 0)
                    {
                        zcache_fill(cache, locale_keys[i], locale_slckeys[i], locale_values[i],
                                    *(locale_left_time_mss[i]), logid, org_timeoutms, begin_index,
                                    cur_index);
                    }
                }
                else
                {
                    
                }
                break;
            case ZCACHE_UPDATE:
            case ZCACHE_DELETE:
            case ZCACHE_INSERT:
                if (*locale_errs[i] == ZCACHE_OK)
                {
                    success_count[locale_errs[i] - errs] ++;
                }
                locale_errs[used_locale_slot] =  locale_errs[i];
                locale_optypes[used_locale_slot]           =    locale_optypes[i]         ;
                locale_keys[used_locale_slot]              =    locale_keys[i]            ;
                locale_slckeys[used_locale_slot]           =    locale_slckeys[i]         ;
                locale_values[used_locale_slot]            =    locale_values[i]          ;
                locale_delay_time_mss[used_locale_slot]    =    locale_delay_time_mss[i]  ;
                locale_left_time_mss[used_locale_slot]     =    locale_left_time_mss[i]   ;
                used_locale_slot ++;
                break;
            default:
                FATAL_LOG("unexpect optype");
                ret  = err_ret(ZCACHE_ERR_PARAM);
                goto err_exit;
            }
        }

        tried_zcache_num ++;
        cur_index = (cur_index + 1)%(cache->redundant_count);
        left_count = used_locale_slot;
        DEBUG_LOG("zcache_op one round [begin_idx:%d,cur_idx:%d, zc_cout:%d, left_count:%d, multop_count:%d]",  begin_index,
                  cur_index, cache->redundant_count, left_count, count);
        used_time = (time_get_cur() - begin_time)/1000;
        if (left_net_timeoutms < used_time && left_count > 0 && tried_zcache_num < cache->redundant_count)
        {
            for (int i = 0; i < left_count; i ++)
            {
                switch (*(locale_optypes[i]))
                {
                case ZCACHE_SEARCH:
                    if (*(locale_errs[i]) == ZCACHE_ERR_NOT_EXIST
                        || *(locale_errs[i]) == ZCACHE_ERR_END
                        || *(locale_errs[i]) == ZCACHE_ERR_BLOCK_NOT_EXIST)
                    {
                        *(locale_errs[i]) = ZCACHE_ERR_TIMEOUT;
                    }
                    break;
                case ZCACHE_INSERT:
                case ZCACHE_UPDATE:
                case ZCACHE_DELETE:
                    
                    break;
                default:
                    FATAL_LOG("unexpect optype");
                    ret  = err_ret(ZCACHE_ERR_PARAM);
                    goto err_exit;
                }
            }
            left_net_timeoutms = 0;
        }
        else if (left_net_timeoutms < used_time)
        {
            left_net_timeoutms = 0;
        }
        else
        {
            left_net_timeoutms -= used_time;
        }
    }

    for (unsigned int i = 0;i < count; i ++)
    {
        if (optypes[i] == ZCACHE_UPDATE || optypes[i] == ZCACHE_DELETE || optypes[i] == ZCACHE_INSERT)
        {
            if (success_count[i] > 0 && success_count[i] < cache->redundant_count)
            {
                errs[i] = ZCACHE_ERR_PART_SUC;
            }
            else if (success_count[i] == 0)
            {
                
            }
            else
            {
                errs[i] = ZCACHE_OK;
            }
        }
        else
        {
            
        }
    }

err_exit:
    UL_SAFEFREE(tmp);
    return ret;
}



static int _zcache_op_async(zcache_op_async_arg_t *oparg)
{
    zcache_err_t err=ZCACHE_OK;
    int ret=0;
    bool isdelayed=false;
    async_remote_invoker_arg_t *asyncarg=NULL;
    zcache_node_t node;
    bool is_node_busy;
    zcache_head_t *msghead=NULL;
    zcache_op_async_redundance_arg_t *rd_arg = oparg->rd_arg;
    if (rd_arg->cache->zcaches[oparg->cur_idx]->product->GetNode(rd_arg->key->item,rd_arg->key->len,&node)<0)
    {
        isdelayed=true;
    }

    if (!isdelayed && rd_arg->is_single)
    {
        
        
        is_node_busy=rd_arg->cache->zcaches[oparg->cur_idx]->localcache->IsNodeBusy(node.ip,node.port);
        if (is_node_busy&&(rd_arg->optype==ZCACHE_SEARCH||rd_arg->optype==ZCACHE_INSERT))
        {
            
            if (rd_arg->optype==ZCACHE_SEARCH)
            {
                err=rd_arg->cache->zcaches[oparg->cur_idx]->localcache->Get(rd_arg->key,rd_arg->slckey,rd_arg->value);
            }
            else
            {
                err=rd_arg->cache->zcaches[oparg->cur_idx]->localcache->Add(rd_arg->key,rd_arg->slckey,rd_arg->value);
            }
            
            rd_arg->cb(rd_arg->cache,err,rd_arg->logid,rd_arg->optype,rd_arg->key, rd_arg->slckey,
                       rd_arg->value, rd_arg->arg, ZCACHEAPI_LOCAL_TIMEOUT_MS);
            return 0;
        }
    }

    
    asyncarg=NEW async_remote_invoker_arg_t;
    if (asyncarg==NULL)
    {
        WARNING_LOG("new async_remote_invoker_arg_t error");
        goto err_exit;
    }
    memset(asyncarg,0,sizeof(async_remote_invoker_arg_t));
    asyncarg->arg=oparg;
    asyncarg->timeout_ms = rd_arg->timeout_ms;
    asyncarg->logid = rd_arg->logid;
    asyncarg->status = ZCACHE_OK;
    if (!isdelayed)
    {
        asyncarg->ip[0]=node.ip[0];
        asyncarg->ip[1]=node.ip[1];
        asyncarg->port=node.port;

        ret=bufHandle_append_cache(&asyncarg->req_handle,
                                   rd_arg->cache->zcaches[oparg->cur_idx]->product->GetName(),
                                   rd_arg->optype,rd_arg->key,rd_arg->slckey,rd_arg->value,
                                   rd_arg->delay_time_ms);
        if (ret<0)
        {
            WARNING_LOG("bufHandle_append_cache error");
            goto err_exit;
        }
        
        asyncarg->req_handle.pBuf=NEW char[sizeof(zcache_head_t)];
        asyncarg->res_handle.pBuf=NEW char[sizeof(zcache_head_t)];
        if (asyncarg->req_handle.pBuf==NULL||asyncarg->res_handle.pBuf==NULL)
        {
            WARNING_LOG("new pBuf error");
            goto err_exit;
        }
        asyncarg->req_handle.nBufLen=sizeof(zcache_head_t);
        asyncarg->res_handle.nBufLen=sizeof(zcache_head_t);
        msghead=(zcache_head_t*)asyncarg->req_handle.pBuf;
        lnhead_set(&(msghead->msghead),rd_arg->logid,ZCACHEAPI_MODULE,ret+sizeof(zcache_head_t)-sizeof(lnhead_t));
        msghead->cmd_num=1;
        asyncarg->req_handle.nDataLen = sizeof(zcache_head_t);

        
        is_node_busy=rd_arg->cache->zcaches[oparg->cur_idx]->localcache->IsNodeBusy(node.ip,node.port);
        if (is_node_busy&&(rd_arg->optype==ZCACHE_SEARCH||rd_arg->optype==ZCACHE_INSERT))
        {
            
            if (rd_arg->optype==ZCACHE_SEARCH)
            {
                err=rd_arg->cache->zcaches[oparg->cur_idx]->localcache->Get(rd_arg->key,rd_arg->slckey,rd_arg->value);
            }
            else
            {
                err=rd_arg->cache->zcaches[oparg->cur_idx]->localcache->Add(rd_arg->key,rd_arg->slckey,rd_arg->value);
            }
            
            if (rd_arg->is_single)
            {                                                           
                rd_arg->cb(rd_arg->cache,err,rd_arg->logid,rd_arg->optype,rd_arg->key, rd_arg->slckey,
                           rd_arg->value,rd_arg->arg, ZCACHEAPI_LOCAL_TIMEOUT_MS);
                goto err_exit;
            }
            else
            {
                asyncarg->ip[0] = 1;
                asyncarg->ip[1] = 1;
                asyncarg->port = 1;
                asyncarg->status = ZCACHE_ERR_BLOCK_NOT_EXIST;
                if (rd_arg->cache->mgr->AsyncTimerInvoker(asyncarg, zcache_op_async_cb, 1) < 0)
                {
                    WARNING_LOG("AsyncTimer error");
                    goto err_exit;
                }
            }
        }
        
        else
        {
            if (rd_arg->optype == ZCACHE_SEARCH)
            {
                asyncarg->timeout_ms = rd_arg->timeout_ms/(rd_arg->cache->redundant_count);
                if (asyncarg->timeout_ms == 0)
                {
                    asyncarg->timeout_ms = rd_arg->timeout_ms;
                }
            }
            if (rd_arg->cache->mgr->AsyncRemoteInvoker(asyncarg,zcache_op_async_cb)<0)
            {
                WARNING_LOG("AsyncRemoteInvoker error");
                goto err_exit;
            }
        }
    }
    else
    {
        if (rd_arg->is_delayed == false)
        {
            if(rd_arg->cache->mgr->AsyncTimerInvoker(asyncarg,zcache_op_async_cb,ZCACHE_ASYNC_RETRY_TIME) < 0)
            {
                WARNING_LOG("AsyncTimerInvoker error");
                goto err_exit;
            }
        }
        else if (rd_arg->is_delayed)
        {
            zcache_err_t err = ZCACHE_ERR_BLOCK_NOT_EXIST;
            if (rd_arg->is_single)
            {
                rd_arg->cb(rd_arg->cache,err,rd_arg->logid,rd_arg->optype,rd_arg->key, rd_arg->slckey,
                           rd_arg->value,rd_arg->arg, ZCACHEAPI_LOCAL_TIMEOUT_MS);
                goto err_exit;
            }
            else
            {
                asyncarg->ip[0] = 1;
                asyncarg->ip[1] = 1;
                asyncarg->port = 1;
                asyncarg->status = ZCACHE_ERR_BLOCK_NOT_EXIST;
                if (rd_arg->cache->mgr->AsyncTimerInvoker(asyncarg, zcache_op_async_cb, 1) < 0)
                {
                    WARNING_LOG("AsyncTimerInvoker error");
                    goto err_exit;
                }
            }
        }
    }
    return 0;


err_exit:
    if (asyncarg)
    {
        bufHandle_free(asyncarg->req_handle._next);
        delete[] asyncarg->req_handle.pBuf;
        bufHandle_free(asyncarg->res_handle._next);
        delete[] asyncarg->res_handle.pBuf;
        delete asyncarg;
    }
    return -1;
}

int zcache_op_async(zcache_t *cache,
                    zcache_op_t optype,
                    zcache_item_t *key, zcache_item_t *slckey,
                    zcache_item_t *value,
                    unsigned int delay_time_ms,
                    unsigned int logid,
                    unsigned int timeout_ms,
                    void *arg,
                    zcache_op_cb_t cb)
{
    int ret = 0;
    zlog_set_thlogid(logid);
    if (cache==NULL || key==NULL || key->item==NULL||key->len==0||key->len>ZCACHE_MAX_KEY_LEN|| timeout_ms==0
        || (slckey != NULL && slckey->len > ZCACHE_MAX_KEY_LEN ))
    {
        WARNING_LOG("param error");
        return -1;
    }
    else if (optype!=ZCACHE_DELETE
             &&(value == NULL || value->item==NULL||value->len==0||value->len>ZCACHE_MAX_VALUE_LEN))
    {
        WARNING_LOG("value error");
        return -1;
    }
    else if (optype<=ZCACHE_OP_NULL_BEGIN&&optype>=ZCACHE_OP_NULL_END)
    {
        WARNING_LOG("bad optype");
        return -1;
    }
    zcache_op_async_arg_t *oparg  = NULL;
    zcache_op_async_redundance_arg_t *rd_arg = NULL;
    bool is_destroy = false;
    oparg = (zcache_op_async_arg_t*)calloc(1, sizeof(zcache_op_async_arg_t)*cache->redundant_count
                                           + sizeof(zcache_op_async_redundance_arg_t));
    if (oparg == NULL)
    {
        WARNING_LOG("malloc error");
        ret = err_ret(ZCACHE_ERR_MEM);
        is_destroy = true;
        goto label_exit;
    }
    rd_arg = (zcache_op_async_redundance_arg_t*)(oparg + cache->redundant_count);
    rd_arg->arg = arg;
    pthread_mutex_init(&(rd_arg->started_mutex), NULL);
    rd_arg->begin_time = time_get_cur();
    rd_arg->cache = cache;
    rd_arg->cb = cb;
    rd_arg->started_num = 0;
    rd_arg->delay_time_ms = delay_time_ms;
    rd_arg->err_out = ZCACHE_ERR_END;
    rd_arg->is_single = false;
    rd_arg->key = key;
    rd_arg->logid = logid;
    rd_arg->optype = optype;
    rd_arg->slckey = slckey;
    rd_arg->timeout_ms = timeout_ms;
    rd_arg->tried_zcache_num = 0;
    rd_arg->value = value;
    rd_arg->is_single = (cache->redundant_count == 1);
    rd_arg->is_delayed = false;
    for (unsigned int i = 0; i < cache->redundant_count; i ++)
    {
        oparg[i].cur_err = ZCACHE_ERR_END;
        oparg[i].rd_arg = rd_arg;
    }
    if (cache->redundant_count == 1)
    {
        oparg->beg_idx  = 0;
        oparg->cur_idx = 0;
        rd_arg->started_num = 1;
        if ((ret = _zcache_op_async(oparg)) < 0)
        {
            rd_arg->started_num = 0;
            WARNING_LOG("_zcache_op_async error");
            is_destroy = true;
            goto label_exit;
        }
    }
    else
    {
        int first_idx = random()%(cache->redundant_count);
        unsigned int tried_num = 0;
        zcache_node_t  first_node;
        switch (optype)
        {
        case ZCACHE_SEARCH:
            while (tried_num < cache->redundant_count)
            {
                if (cache->zcaches[first_idx]->product->GetNode(key->item, key->len, &first_node) < 0
                    || cache->zcaches[first_idx]->localcache->IsNodeBusy(first_node.ip, first_node.port))
                {
                    first_idx = (first_idx+1)%(cache->redundant_count);
                    tried_num ++;
                }
                else
                {
                    break;
                }
            }
            oparg->beg_idx = first_idx;
            oparg->cur_idx = first_idx;
            if ((ret = _zcache_op_async(oparg)) < 0)
            {
                WARNING_LOG("_zcache_op_async error");
                is_destroy = true;
                goto label_exit;
            }
            rd_arg->started_num = 1;
            break;
        case ZCACHE_INSERT:
        case ZCACHE_UPDATE:
        case ZCACHE_DELETE:
            pthread_mutex_lock(&(rd_arg->started_mutex));
            for (unsigned int i = 0;i < cache->redundant_count; i ++)
            {
                oparg[i].cur_err = ZCACHE_ERR_END;
                oparg[i].beg_idx = i;
                oparg[i].cur_idx = i;
                if ((ret = _zcache_op_async(oparg+i)) < 0)
                {
                    WARNING_LOG("_zcache_op_async error");
                    break;
                }
                rd_arg->started_num ++;
            }
            DEBUG_LOG("[started_num:%u, redundant_count:%u, optype:%s]", rd_arg->started_num,
                      cache->redundant_count, zcache_strop(optype));
            if (rd_arg->started_num == 0)
            {
                is_destroy = true;
            }
            else
            {
                
                ret = err_ret(ZCACHE_OK);
                is_destroy = false;
            }
            pthread_mutex_unlock(&(rd_arg->started_mutex));
            break;
        default:
            WARNING_LOG("bad param unknown optype [optype:%s]",  zcache_strop(optype));
            ret = err_ret(ZCACHE_ERR_PARAM);
            is_destroy = true;
        }
    }

label_exit:
    if (is_destroy && oparg != NULL)
    {
        pthread_mutex_destroy(&(rd_arg->started_mutex));
        UL_SAFEFREE(oparg);
    }
    return ret;
}


int zcache_op_once(zcache_t *cache,
                   zcache_op_t optype,
                   zcache_item_t *key, zcache_item_t *slckey,
                   zcache_item_t *value,
                   unsigned int delay_time_ms,
                   zcache_err_t *err_out,
                   unsigned int logid, unsigned int timeout_ms,
                   unsigned int *left_time)
{
    int ret = 0;
    zcache_err_t zcerr;
    zcache_item_t zcslckey;
    zcache_item_t zcvalue;
    memset(&zcslckey,0,sizeof(zcslckey));
    memset(&zcvalue,0,sizeof(zcvalue));

    if (err_out==NULL)
    {
        err_out=&zcerr;
    }
    if (slckey==NULL)
    {
        slckey=&zcslckey;
    }
    if (value==NULL)
    {
        value=&zcvalue;
    }
    if ((ret = zcache_op(cache,1,&optype,key,slckey,value,&delay_time_ms,err_out,logid,timeout_ms, left_time))<0
        || (*err_out!=ZCACHE_OK && *err_out != ZCACHE_ERR_PART_SUC))
    {
        if (err_out != NULL && ret < 0)
        {
            *err_out = (zcache_err_t)(-1*ret);
        }
        return -1;
    }
    else if (*err_out == ZCACHE_ERR_PART_SUC)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

static void zcache_op_cb(async_remote_invoker_arg_t *arg)
{
    if (arg!=NULL&&arg->arg!=NULL)
    {
        DEBUG_LOG("cond finish");
        ((AsyncCond*)(arg->arg))->Finish();
    }
}

static bool  multzc_async_is_timeout(zcache_op_async_arg_t *oparg)
{
    unsigned long long now = time_get_cur();
    if ((now - oparg->rd_arg->begin_time)/1000 > oparg->rd_arg->timeout_ms)
    {
        return true;
    }
    else
    {
        return false;
    }
}


static int multzc_async_search_cb(async_remote_invoker_arg_t *arg, zcache_op_async_arg_t *oparg, cmd_head_t *chead)
{
    int ret = 0;
    zlog_set_thlogid(arg->logid);
    zcache_op_async_redundance_arg_t *rd_arg  = oparg->rd_arg;
    int tried_zcache_num = AtomicInc(rd_arg->tried_zcache_num);
    if (arg->status != ZCACHE_OK || chead->cmd_ret != ZCACHE_OK)
    {
        arg->status = (arg->status == ZCACHE_OK)?(chead->cmd_ret):(arg->status);
        WARNING_LOG("zcache_op_async no ok [arg->status:%s]", zcache_strerror((zcache_err_t)arg->status));
        if (multzc_async_is_timeout(oparg) || rd_arg->cache->redundant_count <= (unsigned)tried_zcache_num)
        {
            
            if (rd_arg->cache->redundant_count > (unsigned)tried_zcache_num)
            {
                rd_arg->err_out = ZCACHE_ERR_TIMEOUT;
            }
            else
            {
                rd_arg->err_out = (zcache_err_t)((arg->status == ZCACHE_OK)?
                                                 (chead->cmd_ret):(arg->status));
            }
            rd_arg->cb(rd_arg->cache,(zcache_err_t)rd_arg->err_out,arg->logid,rd_arg->optype,
                       rd_arg->key, rd_arg->slckey, rd_arg->value,
                       rd_arg->arg, 0);
            WARNING_LOG("timeout or cover all zcaches [tried_num:%d, all_zc_num:%u]", tried_zcache_num,
                        rd_arg->cache->redundant_count);
            ret  = -1;
        }
        else
        {
            
            oparg->cur_idx = (oparg->cur_idx + 1)%(rd_arg->cache->redundant_count);
            if (_zcache_op_async(oparg) < 0)
            {
                
                WARNING_LOG("_zcache_op_async error");
                oparg->cur_err = ZCACHE_ERR_LIB;
                rd_arg->cb(rd_arg->cache,(zcache_err_t)oparg->cur_err,arg->logid,rd_arg->optype,
                           rd_arg->key, rd_arg->slckey, rd_arg->value,
                           rd_arg->arg, 0);
                ret = -1;
            }
            else
            {
                DEBUG_LOG("try next zcache [tried_num:%d, all_zc_num:%u]", rd_arg->tried_zcache_num,
                          rd_arg->cache->redundant_count);
                ret = 0;
            }
        }
    }
    else
    {
        
        zcache_cmd_lib_search_res_t *tres = (zcache_cmd_lib_search_res_t*)(arg->res_handle._next->pBuf
                                                                           + sizeof(cmd_head_t));
        if (rd_arg->value->len < chead->body_len - sizeof(zcache_cmd_lib_search_res_t))
        {
            rd_arg->value->len = chead->body_len - sizeof(zcache_cmd_lib_search_res_t);
            rd_arg->cb(rd_arg->cache,ZCACHE_ERR_BUF_NOT_ENOUGH,arg->logid,rd_arg->optype,
                       rd_arg->key, rd_arg->slckey,
                       rd_arg->value,
                       rd_arg->arg,
                       tres->left_time/1000);
        }
        else
        {
            rd_arg->value->len = chead->body_len - sizeof(zcache_cmd_lib_search_res_t);
            memcpy(rd_arg->value->item, (void*)(tres+1), rd_arg->value->len);
            
            if (tres->left_time >= 2000 || tres->left_time == 0)
            {
                
                zcache_fill(rd_arg->cache, rd_arg->key, rd_arg->slckey, rd_arg->value, tres->left_time/1000,
                            rd_arg->logid, rd_arg->timeout_ms, oparg->beg_idx, oparg->cur_idx);
            }
            rd_arg->cb(rd_arg->cache,(zcache_err_t)(chead->cmd_ret),arg->logid,rd_arg->optype,
                       rd_arg->key, rd_arg->slckey, rd_arg->value, rd_arg->arg,
                       tres->left_time/1000);
        }
        ret = -1;
    }

label_exit:
    return ret;
}



static int multzc_async_tryall_cb(async_remote_invoker_arg_t *arg, zcache_op_async_arg_t *oparg, cmd_head_t *chead)
{
    int ret = 0;
    int cur_err = 0;
    int tried_num = 0;
    zlog_set_thlogid(arg->logid);
    zcache_op_async_redundance_arg_t *rd_arg = oparg->rd_arg;
    zcache_op_async_arg_t *first_oparg = ((zcache_op_async_arg_t *)rd_arg) - rd_arg->cache->redundant_count;
    
    pthread_mutex_lock(&(rd_arg->started_mutex));
    oparg->cur_err =(zcache_err_t)((arg->status == ZCACHE_OK)?(chead->cmd_ret):(arg->status));
    pthread_mutex_unlock(&(rd_arg->started_mutex));
    tried_num = AtomicInc(rd_arg->tried_zcache_num);

    if ((unsigned int)tried_num == rd_arg->started_num )
    {
        unsigned int success_count = 0;
        for (unsigned int i = 0; i < rd_arg->started_num; i ++)
        {
            if (first_oparg[i].cur_err == ZCACHE_OK)
            {
                success_count ++;
            }
        }
        if (success_count < rd_arg->cache->redundant_count && success_count > 0)
        {
            rd_arg->cb(rd_arg->cache,ZCACHE_ERR_PART_SUC,arg->logid,rd_arg->optype,
                       rd_arg->key, rd_arg->slckey, rd_arg->value, rd_arg->arg, 0);
            DEBUG_LOG("all finished and someone fail [tried_num:%d, started_num:%u, total_num:%u, succ_count:%u]",
                      tried_num,  rd_arg->started_num,  rd_arg->cache->redundant_count, success_count);
        }
        else if (success_count == 0)
        {
            rd_arg->cb(rd_arg->cache,oparg->cur_err,arg->logid,rd_arg->optype,
                       rd_arg->key, rd_arg->slckey, rd_arg->value, rd_arg->arg, 0);
            DEBUG_LOG("all finished and all fail [tried_num:%d, started_num:%u, total_num:%u, succ_count:%u]",
                      tried_num,  rd_arg->started_num,  rd_arg->cache->redundant_count, success_count);
        }
        else
        {
            rd_arg->cb(rd_arg->cache,ZCACHE_OK,arg->logid,rd_arg->optype,
                       rd_arg->key, rd_arg->slckey, rd_arg->value, rd_arg->arg, 0);
            DEBUG_LOG("all finished and all success [tried_num:%d, started_num:%u, total_num:%u, succ_count:%u]",
                      tried_num,  rd_arg->started_num,  rd_arg->cache->redundant_count, success_count);
        }
        ret = -1;
    }

    return ret;
}

static void zcache_op_async_cb(async_remote_invoker_arg_t *arg)
{
    if (arg==NULL||arg->arg==NULL)
    {
        FATAL_LOG("arg error");
        return;
    }
    zcache_op_async_arg_t *oparg=(zcache_op_async_arg_t*)arg->arg;
    zcache_op_async_redundance_arg_t *rd_arg = oparg->rd_arg;
    single_zcache_t *cache = rd_arg->cache->zcaches[oparg->cur_idx];
    if (arg->ip[0]==0&&arg->ip[1]==0&&arg->port==0 && rd_arg->is_single)
    {
        
        
        if (arg->timeout_ms>ZCACHE_ASYNC_RETRY_TIME)
        {
            rd_arg->timeout_ms -=  ZCACHE_ASYNC_RETRY_TIME;
            rd_arg->begin_time = time_get_cur();
            if (_zcache_op_async(oparg) < 0)
            {
                rd_arg->cb(rd_arg->cache,ZCACHE_ERR_LIB,arg->logid,rd_arg->optype,
                           rd_arg->key, rd_arg->slckey, rd_arg->value, rd_arg->arg, 0);
                goto label_exit;
            }
        }
        else
        {
            rd_arg->cb(rd_arg->cache,ZCACHE_ERR_TIMEOUT,arg->logid,rd_arg->optype,
                       rd_arg->key, rd_arg->slckey, rd_arg->value, rd_arg->arg, 0);
            goto label_exit;
        }
    }
    else if (rd_arg->is_single)
    {
        zcache_head_t *zhead = (zcache_head_t*)arg->res_handle.pBuf;
        if (arg->status != ZCACHE_OK)
        {
            if (arg->status == ZCACHE_ERR_BLOCK_NOT_EXIST) {
                cache->product->SetError(rd_arg->key->item, rd_arg->key->len, true);
            } else if (arg->status == ZCACHE_ERR_TIMEOUT 
                    || arg->status == ZCACHE_ERR_NET) {
                cache->product->SetError(rd_arg->key->item, rd_arg->key->len, false);
            }
            rd_arg->cb(rd_arg->cache,(zcache_err_t)arg->status,arg->logid,rd_arg->optype,
                       rd_arg->key, rd_arg->slckey, rd_arg->value, rd_arg->arg, 0);
        }
        else if (arg->res_handle.nDataLen != sizeof(zcache_head_t)
                 || zhead->cmd_num != 1
                 || arg->res_handle._next == NULL
                 || zhead->msghead.body_len != sizeof(unsigned) + arg->res_handle._next->nDataLen
                 || (unsigned)arg->res_handle._next->nDataLen < sizeof(cmd_head_t)
                 || (unsigned)arg->res_handle._next->nDataLen < sizeof(cmd_head_t) +
                 ((cmd_head_t*)(arg->res_handle._next->pBuf))->body_len
                )
        {
            rd_arg->cb(rd_arg->cache,ZCACHE_ERR_CACHESERVER,arg->logid,rd_arg->optype,
                       rd_arg->key, rd_arg->slckey, rd_arg->value, rd_arg->arg, 0);
        }
        else if (rd_arg->optype == ZCACHE_SEARCH)
        {
            cmd_head_t *chead = (cmd_head_t*)(arg->res_handle._next->pBuf);
            zcache_cmd_lib_search_res_t *tres = (zcache_cmd_lib_search_res_t*)(arg->res_handle._next->pBuf
                                                                               + sizeof(cmd_head_t));

            rd_arg->value->len = chead->body_len - sizeof(zcache_cmd_lib_search_res_t);
            if (rd_arg->value->len < chead->body_len - sizeof(zcache_cmd_lib_search_res_t))
            {
                rd_arg->cb(rd_arg->cache,ZCACHE_ERR_BUF_NOT_ENOUGH,arg->logid,rd_arg->optype,
                           rd_arg->key, rd_arg->slckey, rd_arg->value, rd_arg->arg, tres->left_time/1000);
            }
            else
            {
                if (chead->cmd_ret == ZCACHE_OK)
                {
                    memcpy(rd_arg->value->item, (void*)(tres+1), rd_arg->value->len);
                } else if (chead->cmd_ret == ZCACHE_ERR_BLOCK_NOT_EXIST) {
                    cache->product->SetError(rd_arg->key->item, rd_arg->key->len, true);
                }
                rd_arg->cb(rd_arg->cache,(zcache_err_t)(chead->cmd_ret),arg->logid,rd_arg->optype,
                           rd_arg->key, rd_arg->slckey, rd_arg->value, rd_arg->arg, tres->left_time/1000);
            }
        }
        else
        {
            cmd_head_t *chead = (cmd_head_t*)(arg->res_handle._next->pBuf);

            if (chead->cmd_ret != ZCACHE_OK) {
                if (chead->cmd_ret == ZCACHE_ERR_BLOCK_NOT_EXIST) {
                    cache->product->SetError(rd_arg->key->item, rd_arg->key->len, true);
                }
            }

            rd_arg->cb(rd_arg->cache,(zcache_err_t)chead->cmd_ret,arg->logid,rd_arg->optype,
                       rd_arg->key, rd_arg->slckey, rd_arg->value, rd_arg->arg, 0);
        }
        goto label_exit;
    }
    else
    {
        cmd_head_t *chead = NULL;
        zcache_head_t *zhead = (zcache_head_t*)arg->res_handle.pBuf;
        if (arg->ip[0]==0&&arg->ip[1]==0&&arg->port==0)
        {
            rd_arg->is_delayed = true;
            if (arg->timeout_ms>ZCACHE_ASYNC_RETRY_TIME)
            {
                rd_arg->timeout_ms -=  ZCACHE_ASYNC_RETRY_TIME;
                rd_arg->begin_time = time_get_cur();
                if (_zcache_op_async(oparg) < 0)
                {
                    arg->status = ZCACHE_ERR_LIB;
                }
                else
                {
                    
                    if (arg)
                    {
                        bufHandle_free(arg->req_handle._next);
                        delete[] arg->req_handle.pBuf;
                        bufHandle_free(arg->res_handle._next);
                        delete[] arg->res_handle.pBuf;
                        delete arg;
                    }
                    return;
                }
            }
            else
            {
                arg->status = ZCACHE_ERR_TIMEOUT;
            }
        }
        else if (arg->status != ZCACHE_OK)
        {
            arg->status = arg->status;

            if (arg->status == ZCACHE_ERR_BLOCK_NOT_EXIST) {
                cache->product->SetError(rd_arg->key->item, rd_arg->key->len, true);
            } else if ((arg->status == ZCACHE_ERR_TIMEOUT)
                      || (arg->status == ZCACHE_ERR_NET)) {
                cache->product->SetError(rd_arg->key->item, rd_arg->key->len, false);
            }
        }
        else if (arg->res_handle.nDataLen != sizeof(zcache_head_t)
                 || zhead->cmd_num != 1
                 || arg->res_handle._next == NULL
                 || zhead->msghead.body_len != sizeof(unsigned) + arg->res_handle._next->nDataLen
                 || (unsigned)arg->res_handle._next->nDataLen < sizeof(cmd_head_t)
                 || (unsigned)arg->res_handle._next->nDataLen < sizeof(cmd_head_t) +
                 ((cmd_head_t*)(arg->res_handle._next->pBuf))->body_len
                )
        {
            arg->status = ZCACHE_ERR_CACHESERVER;
        }
        else
        {
            chead = (cmd_head_t*)(arg->res_handle._next->pBuf);
            if (chead->cmd_ret != ZCACHE_OK)
            {
                arg->status = chead->cmd_ret;
                if (chead->cmd_ret == ZCACHE_ERR_BLOCK_NOT_EXIST) {
                    cache->product->SetError(rd_arg->key->item, rd_arg->key->len, true);
                }
            }
        }
        switch (rd_arg->optype)
        {
        case ZCACHE_SEARCH:
            if (multzc_async_search_cb(arg, oparg, chead) < 0)
            {
                goto label_exit;
            }
            break;
        case ZCACHE_INSERT:
        case ZCACHE_DELETE:
        case ZCACHE_UPDATE:
            if (multzc_async_tryall_cb(arg, oparg, chead) < 0)
            {
                goto label_exit;
            }
            break;
        default:
            FATAL_LOG("bad optype");
            goto label_exit;
        }
    }

    if (arg)
    {
        bufHandle_free(arg->req_handle._next);
        delete[] arg->req_handle.pBuf;
        bufHandle_free(arg->res_handle._next);
        delete[] arg->res_handle.pBuf;
        delete arg;
    }
    return;
label_exit:
    if (arg)
    {
        bufHandle_free(arg->req_handle._next);
        delete[] arg->req_handle.pBuf;
        bufHandle_free(arg->res_handle._next);
        delete[] arg->res_handle.pBuf;
        delete arg;
    }
    if (oparg != NULL)
    {
        pthread_mutex_destroy(&(rd_arg->started_mutex));
        if (rd_arg->is_single)
        {
            oparg = (zcache_op_async_arg_t*)(((char*)rd_arg) - sizeof(zcache_op_async_arg_t));
        }
        else
        {
            oparg = (zcache_op_async_arg_t*)(((char*)rd_arg) -
                                             sizeof(zcache_op_async_arg_t)*rd_arg->cache->redundant_count);
        }
        UL_SAFEFREE(oparg);
    }
}
