#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <new>
#include <exception>
#include "log.h"
#include "productinfo.h"
#include "hash.h"

#define BLOCK_ERR_CLEAR_INTERVAL 60     
ProductInfo::ProductInfo()
{
    memset(_name,0,sizeof(_name));
    memset(_token,0,sizeof(_token));
    _nodes = NULL;
    _status = NULL;
    _blocks = NULL;
    _max_err_time = 0;
    _max_block_num = 0;
    pthread_rwlock_init(&_mutex,NULL);
    DLINK_INITIALIZE(&_invalid_blocklist);
}

ProductInfo::~ProductInfo()
{
    delete[] _blocks;
    delete _nodes;
    delete _status;
    pthread_rwlock_destroy(&_mutex);
}

int ProductInfo::Init(int max_err_time,unsigned int max_block_num)
{
    if (max_err_time < 0)
    {
        WARNING_LOG("param error,[max_err_time:%d]",max_err_time);
        return -1;
    }
    else if (max_block_num==0)
    {
        WARNING_LOG("param error,[max_block_num==0]");
        return -1;
    }
    pthread_rwlock_wrlock(&_mutex);
    delete _status;
    delete _nodes;
    delete[] _blocks;
    _status = NULL;
    _nodes = NULL;
    _blocks = NULL;

    try
    {
        _status = new std::vector<zcache_status_t>;
        _nodes = new std::vector<zcache_node_t>;
        _blocks = new product_block_t[max_block_num];
    }
    catch (std::exception &e)
    {
        WARNING_LOG("%s,[max_block_num:%u]",
                    e.what(),max_block_num);
        delete _status;
        delete _nodes;
        delete[] _blocks;
        _status = NULL;
        _nodes = NULL;
        _blocks = NULL;
        pthread_rwlock_unlock(&_mutex);
        return -1;
    }
    _max_block_num = max_block_num;
    _max_err_time = max_err_time;
    for (unsigned int i=0;i<max_block_num;i++)
    {
        _blocks[i].blockid = i;
        _blocks[i].nodeid = -1;
        _blocks[i].err_time = 0;
        _blocks[i].last_err_time = 0;
        DLINK_INITIALIZE(&_blocks[i].link);
        DEBUG_LOG("[blockid:%d,pre:%p,next:%p, array_beg:%p, array_end:%p]", i,
                  _blocks[i].link._prev, _blocks[i].link._next, _blocks,
                  _blocks + max_block_num);
    }
    pthread_rwlock_unlock(&_mutex);
    return 0;
}

int ProductInfo::SetName(const char* name)
{
    if (name == NULL
        || strlen(name) == 0 || strlen(name) >= sizeof(_name))
    {
        WARNING_LOG("param error");
        return -1;
    }
    pthread_rwlock_wrlock(&_mutex);
    snprintf(_name,sizeof(_name),"%s",name);
    pthread_rwlock_unlock(&_mutex);
    return 0;
}

char *ProductInfo::GetName()
{
    return _name;
}

int ProductInfo::SetToken(const char* token)
{
    if (token !=NULL && strlen(token) >= sizeof(_token))
    {
        WARNING_LOG("token too long");
        return -1;
    }
    pthread_rwlock_wrlock(&_mutex);
    if (token==NULL||token[0]=='\0')
    {
        memset(_token,0,sizeof(_token));
    }
    else
    {
        snprintf(_token,sizeof(_token),"%s",token);
    }
    pthread_rwlock_unlock(&_mutex);
    return 0;
}

char *ProductInfo::GetToken()
{
    return _token;
}

int ProductInfo::SetError(const char *key,unsigned int len,bool force)
{
    if (key==NULL||len==0)
    {
        WARNING_LOG("param error");
        return -1;
    }
    pthread_rwlock_wrlock(&_mutex);
    unsigned int blockid = hash(key,len);
    if (_nodes == NULL || _blocks == NULL || _status == NULL)
    {
        WARNING_LOG("not init");
        goto err_exit;
    }
    blockid %= _max_block_num;

    DEBUG_LOG("before seterror [blockid:%d,pre:%p,next:%p, array_beg:%p, array_end:%p]",
              blockid, _blocks[blockid].link._prev, _blocks[blockid].link._next, _blocks,
              _blocks + _max_block_num);
    if (_blocks[blockid].link._next == _blocks[blockid].link._prev 
        && _blocks[blockid].link._next == &(_blocks[blockid].link))
    {
        
        if (force)
        {
            DLINK_INSERT_NEXT(&_invalid_blocklist,&_blocks[blockid].link);
            _blocks[blockid].nodeid=-1;
        }
        else
        {
            time_t cur;
            time(&cur);
            if (cur-BLOCK_ERR_CLEAR_INTERVAL>_blocks[blockid].last_err_time)
            {
                _blocks[blockid].err_time=1;
                _blocks[blockid].last_err_time=cur;
            }
            else
            {
                _blocks[blockid].err_time++;
            }
            if (_blocks[blockid].err_time >= _max_err_time)
            {
                DLINK_INSERT_NEXT(&_invalid_blocklist,&_blocks[blockid].link);
                _blocks[blockid].nodeid=-1;
            }
        }
    }
    WARNING_LOG("blockid set error,[blockid:%u]",blockid);
    DEBUG_LOG("after seterror [blockid:%d,pre:%p,next:%p, array_beg:%p, array_end:%p]",
              blockid, _blocks[blockid].link._prev, _blocks[blockid].link._next, _blocks,
              _blocks + _max_block_num);
    pthread_rwlock_unlock(&_mutex);
    return 0;
err_exit:
    pthread_rwlock_unlock(&_mutex);
    return -1;
}


bool ProductInfo::IsBlockOk(unsigned int blockid)
{
    pthread_rwlock_rdlock(&_mutex);
    bool ret = true;
    if (_nodes == NULL || _blocks == NULL || _status == NULL)
    {
        WARNING_LOG("not init");
        ret = false;
    }
    else if (blockid >= _max_block_num)
    {
        WARNING_LOG("blockid too large,[blockid:%u,_max_block_num:%u]",
                    blockid,_max_block_num);
        ret = false;
    }
    else if (_blocks[blockid].err_time > _max_err_time)
    {
        ret = false;
    }
    else
    {
        ret = (_blocks[blockid].nodeid<0)?false:true;
    }
    pthread_rwlock_unlock(&_mutex);
    return ret;
}


int ProductInfo::Update(unsigned int blockid,int nodeid)
{
    int ret = 0;
    pthread_rwlock_wrlock(&_mutex);
    if (_nodes == NULL || _blocks == NULL || _status == NULL)
    {
        WARNING_LOG("not init");
        ret = -1;
    }
    else if (blockid >= _max_block_num)
    {
        WARNING_LOG("blockid too large,[blockid:%u,_max_block_num:%u]",
                    blockid,_max_block_num);
        ret = -1;
    }
    else
    {
        DEBUG_LOG("before update [blockid:%d,pre:%p,next:%p, array_beg:%p, array_end:%p]",
                  blockid, _blocks[blockid].link._prev, _blocks[blockid].link._next, _blocks,
                  _blocks + _max_block_num);
        _blocks[blockid].err_time = 0;
        _blocks[blockid].nodeid = nodeid;
        _blocks[blockid].last_err_time=0;
        DLINK_REMOVE(&_blocks[blockid].link);
        DLINK_INITIALIZE(&_blocks[blockid].link);
        if(nodeid<0)
        {
            DLINK_INSERT_NEXT(&_invalid_blocklist,&_blocks[blockid].link);
        }
        DEBUG_LOG("after update [blockid:%d,pre:%p,next:%p, array_beg:%p, array_end:%p]",
                  blockid, _blocks[blockid].link._prev, _blocks[blockid].link._next, _blocks,
                  _blocks + _max_block_num);
        
    }
    pthread_rwlock_unlock(&_mutex);
    return ret;
}

int ProductInfo::Update(unsigned int blockid,unsigned int ip[2],unsigned short port)
{
    int ret = AddNode(ip,port);
    if (ret >= 0)
    {
        ret = Update(blockid,ret);
    }
    return ret;
}


int ProductInfo::AddNode(unsigned int ip[2],unsigned short port)
{
    int ret = 0;
    pthread_rwlock_wrlock(&_mutex);
    if (_nodes == NULL || _blocks == NULL || _status == NULL)
    {
        WARNING_LOG("not init");
        ret = -1;
    }
    else if (ip[0] == ip[1])
    {
        WARNING_LOG("ip the same,[ip:%u]",ip[0]);
        ret = -1;
    }
    else
    {
        unsigned int lip[2];
        int node_idx = 0;
        zcache_node_t node;
        
        lip[0] = (ip[0]>ip[1])?ip[1]:ip[0];
        lip[1] = (ip[0]<ip[1])?ip[1]:ip[0];
        int count = _nodes->size();
        
        for (node_idx = 0; node_idx <count;node_idx++)
        {
            if ((*_nodes)[node_idx].ip[0] == lip[0] && (*_nodes)[node_idx].ip[1] == lip[1]
                && (*_nodes)[node_idx].port == port)
            {
                ret = node_idx;
                break;
            }
        }

        if (node_idx>=count)
        {
            node.ip[0] = lip[0];
            node.ip[1] = lip[1];
            node.port = port;
            zcache_status_t status;
            pthread_rwlock_init(&status.lock, NULL);
            status.is_valid = false;
            try
            {
                _nodes->push_back(node);
                _status->push_back(status);
                ret = count;
                TRACE_LOG("add node,[idx:%d,ip0:%u,ip1:%u,port:%d]",ret,lip[0],lip[1],port);
            }
            catch (std::exception& e)
            {
                WARNING_LOG("%s,[ip:%u,%u,port:%d]",e.what(),lip[0],lip[1],port);
                ret = -1;
            }
        }
        DEBUG_LOG("Add node [ip0:%u, ip1:%u, port:%u, add_idx:%d, cur_nodes_num:%u]",
                  lip[0],lip[1],port, ret, (unsigned int)_nodes->size());
    }
    pthread_rwlock_unlock(&_mutex);
    return ret;
}


int ProductInfo::get_nodeid(unsigned int ip[2], unsigned int port, unsigned int *nodeid)
{
    int ret = -1;
    pthread_rwlock_rdlock(&_mutex);
    if (_nodes == NULL || _blocks == NULL || _status == NULL) {
        WARNING_LOG("not init");
        ret = -1;
        goto out;
    }

    unsigned int lip[2];
    lip[0] = ip[0]>ip[1]?ip[1]:ip[0];
    lip[1] = ip[0]<ip[1]?ip[1]:ip[0];
    for(unsigned int i = 0; i < _nodes->size(); ++i) {
        
        if ((*_nodes)[i].ip[0] == lip[0] &&
                (*_nodes)[i].ip[1] == lip[1] &&
                (*_nodes)[i].port == port) {
            *nodeid = i;
            ret = 0;
            break;
        }
    }

out:
    pthread_rwlock_unlock(&_mutex);
    return ret;
}

int ProductInfo::GetNode(const char *key,unsigned int len,zcache_node_t *node_out)
{
    int ret = 0;
    pthread_rwlock_rdlock(&_mutex);
    if (_nodes == NULL || _blocks == NULL || _status == NULL)
    {
        WARNING_LOG("not init");
        ret = -1;
    }
    else if (key == NULL || len == 0 || node_out == NULL)
    {
        WARNING_LOG("param error");
        ret = -1;
    }
    else
    {
        unsigned int blockid = hash(key,len);
        blockid %= _max_block_num;
        if (_blocks[blockid].err_time > _max_err_time
            || _blocks[blockid].nodeid < 0
            || (int)_nodes->size()<=_blocks[blockid].nodeid)
        {
            WARNING_LOG("block current cannot use,[blockid:%u,err_time:%d,nodeid:%u]",
                        blockid,_blocks[blockid].err_time,_blocks[blockid].nodeid);
            ret = -1;
        }
        else
        {
            ret = _blocks[blockid].nodeid;
            *node_out = (*_nodes)[_blocks[blockid].nodeid];
        }
    }
    pthread_rwlock_unlock(&_mutex);
    return ret;
}

void ProductInfo::ValidNode(unsigned int nodeid)
{
    if (_status == NULL || _blocks == NULL || _nodes == NULL) {
        WARNING_LOG("not init");
        return;
    }
    if (_status->size() <= nodeid) {
        WARNING_LOG("node id exceeds _status size [nodeid: %u, status_size: %lu]", nodeid, _status->size());
        return;
    }
    (*_status)[nodeid].is_valid = true;
}

void ProductInfo::InvalidNode(unsigned int nodeid)
{
    if (_status->size() <= nodeid) {
        WARNING_LOG("node id exceeds _status size [nodeid: %u, status_size: %lu]", nodeid, _status->size());
        return;
    }
    (*_status)[nodeid].is_valid = false;
}

int ProductInfo::SetNodeStatus(unsigned int nodeid, zcache_status_t *status)
{
    int ret = 0;
    if (nodeid >= _status->size() || status == NULL) {
        return -ZCACHE_ERR_PARAM;
    }
    zcache_status_t *s = &(*_status)[nodeid];
    pthread_rwlock_wrlock(&s->lock);
    if (status->is_valid) {
        s->block_total = status->block_total;
        s->mem_total = status->mem_total;
        s->disk_total = status->disk_total;
        s->disk_avail_size = status->disk_avail_size;
        s->total_query = status->total_query;
        s->succ_query_count = status->succ_query_count;
        s->memused = status->memused;
        s->item_num = status->item_num;
        s->net_read_flowload = status->net_read_flowload;
        s->net_write_flowload = status->net_write_flowload;
        s->disk_read_count = status->disk_read_count;
        s->disk_write_count = status->disk_write_count;
        s->cache_hit_count[0] = status->cache_hit_count[0];
        s->cache_hit_count[1] = status->cache_hit_count[1];
        s->cache_hit_count[2] = status->cache_hit_count[2];
        s->insert_count = status->insert_count;
        s->remove_count = status->remove_count;
        s->update_count = status->update_count;
        s->search_count = status->search_count;
        s->lru_die_out_count = status->lru_die_out_count;
        s->avg_op_load = status->avg_op_load;
        s->node_capacity = status->node_capacity;
        s->is_valid = true;
    } else {
        s->is_valid = false;
    }
    pthread_rwlock_unlock(&s->lock);

    return ret;
}

int ProductInfo::GetNodeStatus(unsigned int nodeid, zcache_status_t *status)
{
    int ret = 0;
    if (nodeid >= _status->size() || status == NULL) {
        ret = -ZCACHE_ERR_PARAM;
        return ret;
    }

    pthread_rwlock_rdlock(&(*_status)[nodeid].lock);
    memcpy(status, &(*_status)[nodeid], sizeof(*status));
    pthread_rwlock_unlock(&(*_status)[nodeid].lock);
    return ret;
}

int ProductInfo::GetInvalidBlocks(int *blocks,int num)
{
    int invalid_num = 0;
    pthread_rwlock_wrlock(&_mutex);
    if (blocks == NULL)
    {
        WARNING_LOG("param error");
        invalid_num = -1;
    }
    else if (invalid_num < num)
    {
        DLINK *link = _invalid_blocklist._next;
        product_block_t* block = NULL;
        while ((link != &_invalid_blocklist) && (invalid_num < num))
        {
            block = CONTAINING_RECORD(link,product_block_t,link);
            blocks[invalid_num] = block->blockid;
            link = link->_next;
            invalid_num++;
        }
        if (link != &_invalid_blocklist)
        {
            
            DLINK_REMOVE(&_invalid_blocklist);
            DLINK_INSERT_PREV(link,&_invalid_blocklist);
        }
    }
    pthread_rwlock_unlock(&_mutex);
    return invalid_num;
}

unsigned int ProductInfo::get_max_block_num()
{
  return this->_max_block_num;
}

int ProductInfo::get_nodeid(unsigned int blockid)
{
  if (blockid >= _max_block_num)
  {
    WARNING_LOG("blockid too large,[blockid:%u,_max_block_num:%u]", blockid,
        _max_block_num);
    return -1;
  }

  int result = 0;
  pthread_rwlock_rdlock(&_mutex);
  result = this->_blocks [blockid].nodeid;
  pthread_rwlock_unlock(&_mutex);
  return result;
}

unsigned int ProductInfo::get_node_num()
{
  unsigned int result=0;
  pthread_rwlock_rdlock(&_mutex);
  result=this->_nodes->size();
  pthread_rwlock_unlock(&_mutex);
  return result;
}

int ProductInfo::get_node(unsigned int nodeid, zcache_node_t *node_out)
{
  pthread_rwlock_rdlock(&_mutex);
  if (this->_nodes->size() <= nodeid)
  {
    WARNING_LOG("nodeid is too large,nodeid:%u]",nodeid);
    pthread_rwlock_unlock(&_mutex);
    return -1;
  }
  
  node_out->ip[0]=(*_nodes) [nodeid].ip[0];
  node_out->ip[1]=(*_nodes) [nodeid].ip[1];
  node_out->port=(*_nodes) [nodeid].port;
  pthread_rwlock_unlock(&_mutex);
  return 0;
}

