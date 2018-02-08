#include "localcache.h"
#include "util.h"

LocalCache::LocalCache()
{
    _max_num=0;
    _cur_num=0;
    DLINK_INITIALIZE(&_lru_head);
    pthread_mutex_init(&_mutex,NULL);

    pthread_mutex_init(&_node_mutex,NULL);
    DLINK_INITIALIZE(&_node_head);
    _is_open=false;                                                     
    _min_err_count=0;                                                   
}

LocalCache::~LocalCache()
{
    Clear();
    pthread_mutex_destroy(&_mutex);
}

int LocalCache::Init(unsigned int max_num,unsigned int min_err_count)
{
    Clear();
    if (max_num > 0 && _local_cache.Init(max_num)<0)
    {
        return -1;
    }
    _max_num=max_num;
    _min_err_count=min_err_count;
    return 0;
}

zcache_err_t LocalCache::Add(zcache_item_t* key,zcache_item_t* slckey,zcache_item_t* value)
{
    if (_max_num==0)
    {
        WARNING_LOG("_max_num==0");
        return ZCACHE_ERR_LIB;
    }
    local_cache_item_t *item=NULL;
    char *buf=NULL;
    if (key==NULL||key->len==0||key->item==NULL||value==NULL)
    {
        
        pthread_mutex_lock(&_mutex);
        if (_cur_num>0)
        {
            _cur_num--;
            item=CONTAINING_RECORD(_lru_head._prev,local_cache_item_t,lrulink);
            DLINK_REMOVE(&item->hashlink);
            DLINK_REMOVE(&item->lrulink);
            delete[] (char*)item;
        }
        pthread_mutex_unlock(&_mutex);
        return ZCACHE_OK;
    }
    if (slckey != NULL && slckey->item == NULL && slckey->len != 0)
    {
        
        slckey->len = 0;
    }
    local_cache_key_t  hashkey;
    hashkey.mainkey = key;
    hashkey.slckey = slckey;
    unsigned int  hashvalue = 0;
    hashvalue = hash(key->item, key->len, hashvalue);
    if (slckey != NULL && slckey->item != NULL && slckey->len != 0)
    {
        hashvalue = hash(slckey->item, slckey->len, hashvalue);
    }

    pthread_mutex_lock(&_mutex);
    if (_local_cache.Find(&hashkey, hashvalue) != NULL)
    {
        pthread_mutex_unlock(&_mutex);
        return ZCACHE_OK;
    }
    if (_cur_num==_max_num)
    {
        _cur_num--;
        item=CONTAINING_RECORD(_lru_head._prev,local_cache_item_t,lrulink);
        DLINK_REMOVE(&item->hashlink);
        DLINK_REMOVE(&item->lrulink);
        delete[] ((char*)item);
        DEBUG_LOG("locale cache lru die out");
    }
    unsigned int len=sizeof(local_cache_item_t);
    len+=key->len;
    if (slckey!=NULL&&slckey->item!=NULL&&slckey->len>0)
    {
        len+=slckey->len;
    }
    if (value!=NULL&&value->item!=NULL&&value->len>0)
    {
        len+=value->len;
    }
    buf=NEW char[len];
    if (buf!=NULL)
    {
        item=(local_cache_item_t*)buf;
        item->key_len=key->len;
        memcpy(item->buf,key->item,key->len);
        item->slckey_len=0;
        item->value_len=0;
        if (slckey&&slckey->item!=NULL&&slckey->len!=0)
        {
            item->slckey_len=slckey->len;
            memcpy(item->buf+item->key_len,slckey->item,slckey->len);
        }
        if (value&&value->item!=NULL&&value->len!=0)
        {
            item->value_len=value->len;
            memcpy(item->buf+item->key_len+item->slckey_len,value->item,value->len);
        }
        _local_cache.Put(item, hashvalue);
        DLINK_INSERT_NEXT(&_lru_head,&item->lrulink);
        _cur_num++;
        pthread_mutex_unlock(&_mutex);
        DEBUG_LOG("add locale cache");
        return ZCACHE_OK;
    }
    else
    {
        WARNING_LOG("new buf error");
        pthread_mutex_unlock(&_mutex);
        return ZCACHE_ERR_LIB;
    }
}

zcache_err_t LocalCache::Get(zcache_item_t* key,zcache_item_t* slckey,zcache_item_t* value_out)
{
    if (key==NULL||key->item==NULL||key->len==0||value_out==NULL||value_out->item==NULL||value_out->len==0)
    {
        return ZCACHE_ERR_PARAM;
    }
    else if (_max_num==0)
    {
        WARNING_LOG("_max_num==0");
        return ZCACHE_ERR_LIB;
    }
    local_cache_key_t lkey;
    local_cache_item_t* item=NULL;
    zcache_err_t ret=ZCACHE_OK;
    lkey.mainkey=key;
    lkey.slckey=slckey;
    pthread_mutex_lock(&_mutex);
    item=_local_cache.Find(&lkey);
    if (item==NULL)
    {
        ret=ZCACHE_ERR_NOT_EXIST;
    }
    else if (item->value_len>value_out->len)
    {
        ret=ZCACHE_ERR_BUF_NOT_ENOUGH;
        value_out->len=item->value_len;
    }
    else
    {
        value_out->len=item->value_len;
        memcpy(value_out->item,item->buf+item->key_len+item->slckey_len,item->value_len);

        DLINK_REMOVE(&item->lrulink);
        DLINK_INSERT_NEXT(&_lru_head,&item->lrulink);
    }
    pthread_mutex_unlock(&_mutex);
    DEBUG_LOG("get locale cache");
    return ret;
}

void LocalCache::Clear()
{
    if (_max_num==0)
    {
        return;
    }
    pthread_mutex_lock(&_mutex);
    lc_node_t *node=NULL;
    while (!DLINK_IS_EMPTY(&_node_head))
    {
        node=CONTAINING_RECORD(_node_head._next,lc_node_t,link);
        DLINK_REMOVE(&node->link);
        delete node;
    }
    _clear();
    pthread_mutex_unlock(&_mutex);
}

bool LocalCache::IsOpen()                                               
{
    return _is_open;
}

int LocalCache::AddNode(unsigned int ip[2],unsigned short port)
{
    if (_max_num==0)
    {
        return 0;
    }

    int ret=0;
    time_t t;
    time(&t);
    pthread_mutex_lock(&_node_mutex);
    lc_node_t *node=_findNode(ip,port);
    if (node)
    {
        node->err_count++;
        node->open_time=t;
        if (node->err_count>=_min_err_count)
        {
            
            _is_open=true;
        }
    }
    else
    {
        
        node=NEW lc_node_t;
        if (node==NULL)
        {
            WARNING_LOG("new zcacheapi_node_t error");
            ret=-1;
        }
        else
        {
            node->node.ip[0]=(ip[0]>ip[1])?ip[1]:ip[0];
            node->node.ip[1]=(ip[0]>ip[1])?ip[0]:ip[1];;
            node->node.port=port;
            node->err_count=1;
            node->open_time=t;
            DLINK_INSERT_NEXT(&_node_head,&node->link);
        }
    }
    pthread_mutex_unlock(&_node_mutex);
    return ret;
}

void LocalCache::RemoveNode(unsigned int ip[2],unsigned short port)
{
    if (_max_num==0)
    {
        return;
    }

    pthread_mutex_lock(&_node_mutex);
    lc_node_t *node=_findNode(ip,port);
    if (node!=NULL)
    {
        node->err_count--;
        if (node->err_count==0)
        {
            
            DLINK_REMOVE(&node->link);
            delete node;
        }
    }

    if (DLINK_IS_EMPTY(&_node_head))
    {
        _clear();
    }
    pthread_mutex_unlock(&_node_mutex);
}

bool LocalCache::IsNodeBusy(unsigned int ip[2],unsigned short port)
{
    if (!_is_open)
    {
        return false;
    }
    bool ret=false;
    pthread_mutex_lock(&_node_mutex);
    lc_node_t *node=NULL;
    node=_findNode(ip,port);
    if (node&& node->err_count>=_min_err_count)
    {
        ret=true;
    }
    pthread_mutex_unlock(&_node_mutex);
    return ret;
}

void LocalCache::_clear()
{
    local_cache_item_t* item=NULL;
    while (!DLINK_IS_EMPTY(&_lru_head))
    {
        item=CONTAINING_RECORD(_lru_head._next,local_cache_item_t,lrulink);
        DLINK_REMOVE(&item->hashlink);
        DLINK_REMOVE(&item->lrulink);
        delete[] (char*)item;
    }
    _cur_num=0;
    _is_open=false;
}

lc_node_t* LocalCache::_findNode(unsigned int ip[2],unsigned short port)
{
    unsigned int thisip[2];
    thisip[0]=(ip[0]>ip[1])?ip[1]:ip[0];
    thisip[1]=(ip[0]>ip[1])?ip[0]:ip[1];
    time_t t;
    time(&t);
    lc_node_t *node=NULL;
    DLINK *indx=_node_head._next;
    
    while (indx!=&_node_head)
    {
        node=CONTAINING_RECORD(indx,lc_node_t,link);
        if (node->node.ip[0]==thisip[0]
            &&node->node.ip[1]==thisip[1]
            &&node->node.port==port)
        {
            
            return node;
        }
        indx=indx->_next;
        
        if (t-node->open_time>LC_NODE_KEEP_TIME_MAX)
        {
            DLINK_REMOVE(&node->link);
            delete node;
        }
    }
    return NULL;
}
