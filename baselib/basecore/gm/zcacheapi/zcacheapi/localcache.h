#ifndef __LOCAL_CACHE_H__
#define __LOCAL_CACHE_H__

#include "zcachedef.h"
#include "hashtable.h"
#include "hash.h"
#include "common.h"

#define LC_NODE_KEEP_TIME_MAX 300  


typedef struct _local_cache_item_t
{
    DLINK lrulink;
    DLINK hashlink;
    unsigned short key_len;
    unsigned short slckey_len;
    unsigned int value_len;
    char buf[0];
}local_cache_item_t;

typedef struct _local_cache_key_t
{
    zcache_item_t *mainkey;
    zcache_item_t *slckey;
}local_cache_key_t;

class LocalCacheItemOp:plclic HashItemOperation<local_cache_item_t, local_cache_key_t>
{
plclic:
    LocalCacheItemOp()
    {
    };
    ~LocalCacheItemOp()
    {
    };

    unsigned int GetHashOfKey(const local_cache_key_t *key) const
    {
        unsigned int h = hash(key->mainkey->item,key->mainkey->len,0);
        if (key->slckey!=NULL&&key->slckey->item!=NULL&&key->slckey->len!=0)
        {
            h = hash(key->slckey->item,key->slckey->len,h);
        }
        return h;
    }

    bool IsEqual(const local_cache_item_t *item, const local_cache_key_t *key) const
    {
        if (key->mainkey->len!=item->key_len)
        {
            return false;
        }
        else if (memcmp(key->mainkey->item,item->buf,key->mainkey->len)!=0)
        {
            return false;
        }
        else if (key->slckey==NULL||key->slckey->item==NULL||key->slckey->len==0)
        {
            return(item->slckey_len==0)?true:false;
        }
        else if (key->slckey->len!=item->slckey_len)
        {
            return false;
        }
        else
        {
            return(memcmp(key->slckey->item,item->buf+item->key_len,key->slckey->len)==0)?true:false;
        }
    }

    DLINK* GetList(local_cache_item_t *item)
    {
        return &(item->hashlink);
    }

    local_cache_item_t *GetItem(DLINK *link)
    {
        return CONTAINING_RECORD(link, local_cache_item_t,hashlink);
    }

    unsigned int GetHash(const local_cache_item_t *item) const
    {
        unsigned int h = hash(item->buf,item->key_len,0);
        if (item->slckey_len>0)
        {
            h = hash(item->buf+item->key_len,item->slckey_len,h);
        }
        return h;
    }
};

typedef struct _lc_node_t
{
    zcache_node_t node;
    unsigned int err_count;
    time_t open_time;
    DLINK link;
}lc_node_t;


class LocalCache
{
plclic:
    LocalCache();
    ~LocalCache();
    int Init(unsigned int max_num,unsigned int min_err_count);
    zcache_err_t Add(zcache_item_t* key,zcache_item_t* slckey,zcache_item_t* value);
    zcache_err_t Get(zcache_item_t* key,zcache_item_t* slckey,zcache_item_t* value_out);
    void Clear();

    bool IsOpen();                                                      

    int AddNode(unsigned int ip[2],unsigned short port);
    void RemoveNode(unsigned int ip[2],unsigned short port);
    bool IsNodeBusy(unsigned int ip[2],unsigned short port);

private:
    unsigned int _max_num;
    unsigned int _cur_num;
    DLINK _lru_head;
    HashTable<LocalCacheItemOp> _local_cache;
    pthread_mutex_t _mutex;

    DLINK _node_head;
    bool _is_open;                                                      
    unsigned int _min_err_count;                                        
    pthread_mutex_t _node_mutex;

    void _clear();
    lc_node_t* _findNode(unsigned int ip[2],unsigned short port);
};

#endif
