#include "zcachedata.h"
#include "util.h"
#include "zcacheapi.h"
#include "commwithms.h"
#include "mynet.h"
#include "ul_net.h"

volatile int csc_running = 1;

single_zcache_t* zcachedata_create(unsigned int cscheck_time_us)
{
    single_zcache_t *cache = NEW single_zcache_t;
    if (cache==NULL)
    {
        WARNING_LOG("new single_zcache_t error");
        return NULL;
    }
    memset(cache,0,sizeof(single_zcache_t));

    DLINK_INITIALIZE(&cache->mslist_head);
    cache->ms_num = 0;                                                  
    pthread_mutex_init(&cache->mutex,NULL);

    cache->update_thread = 0;
    cache->cscheck_thread = 0;
    cache->ms_refetch_time_ms = ZCACHEAPI_DEFAULT_REFETCH_TIME;         
    cache->connect_timeout_ms = ZCACHEAPI_DEFAULT_CONNECT_TIMEOUT;      
    cache->ms_read_timeout_ms = ZCACHEAPI_DEFAULT_READ_TIMEOUT;
    cache->ms_write_timeout_ms = ZCACHEAPI_DEFAULT_WRITE_TIMEOUT;
    cache->cscheck_time_us = cscheck_time_us;

    cache->is_block_fetched = false;                                    
    cache->is_running = true;
    cache->reqlen = sizeof(zcache_head_t)+sizeof(cmd_head_t)
                    +sizeof(zcache_cmd_lib_getsomemeta_req_t)+
                    sizeof(int)*ZCACHE_BLOCK_MAX_PERFETCH;
    cache->reslen = sizeof(zcache_head_t)+sizeof(cmd_head_t)
                    +sizeof(zcache_cmd_lib_getsomemeta_res_t)+
                    sizeof(zcacheapi_node_t)*ZCACHE_MAX_NODE_NUM
                    +sizeof(int)*2*ZCACHE_BLOCK_MAX_PERFETCH;

    cache->reqbuf = NEW char[cache->reqlen];
    cache->resbuf = NEW char[cache->reslen];
    cache->product = NEW ProductInfo;
    cache->localcache = NEW LocalCache;

    if (cache->reqbuf==NULL || cache->resbuf == NULL || cache->product==NULL
        || cache->localcache==NULL)
    {
        WARNING_LOG("new error,[reqlen:%u,resbuf:%u]",cache->reqlen,cache->reslen);
        goto err_exit;
    }
    if (cache->localcache->Init(ZCACHEAPI_DEFAULT_LOCAL_CACHE_NUM,ZCACHEAPI_DEFAULT_LOCAL_CACHE_MIN_BUSY_COUNT)<0)
    {
        goto err_exit;
    }
    return cache;
err_exit:
    zcachedata_destroy(cache);
    return NULL;
}

void zcachedata_destroy(single_zcache_t *cache)
{
    if (cache)
    {
        cache->is_running=false;
	csc_running = 0;

        if (cache->update_thread > 0)
        {
            pthread_join(cache->update_thread,NULL);
            cache->update_thread = 0;
        }
        if (cache->cscheck_thread > 0)
        {
            pthread_join(cache->cscheck_thread, NULL);
            cache->cscheck_thread = 0;
        }
        poll(NULL, 0, cache->cscheck_time_us/1000);

        delete cache->product;
        cache->product = NULL;
        delete[] cache->reqbuf;
        cache->reqbuf = NULL;
        delete[] cache->resbuf;
        cache->resbuf = NULL;
        delete cache->localcache;
        cache->localcache=NULL;

        zcacheapi_node_t *node = NULL;
        while (!DLINK_IS_EMPTY(&cache->mslist_head))
        {
            node = CONTAINING_RECORD(cache->mslist_head._next,zcacheapi_node_t,link);
            DLINK_REMOVE(&node->link);
            delete node;
        }
        cache->ms_num = 0;
        pthread_mutex_destroy(&cache->mutex);
        delete cache;
    }
}


int zcache_connect_ms(single_zcache_t *cache)
{
    pthread_mutex_lock(&cache->mutex);
    int connect_time = cache->connect_timeout_ms*1000;
    DLINK *indx = cache->mslist_head._next;
    zcacheapi_node_t *node = NULL;
    int fd = -1;
    unsigned int try_time = 0;
    while (indx != &cache->mslist_head)
    {
        
        unsigned int ms_num =((cache->ms_num == 1)?2:(cache->ms_num));
        errno = 0;
        node = CONTAINING_RECORD(indx,zcacheapi_node_t,link);
        if ((node->err_count == 0)
            ||((node->err_count>0)&&(random()%ms_num != 0)))
        {
            pthread_mutex_unlock(&cache->mutex);
            fd = MyConnectO(node->ip,node->port,&connect_time,ZCACHEAPI_MODULE);
            pthread_mutex_lock(&cache->mutex);
            DEBUG_LOG("try connected [fd:%d, ip:%s,port:%d, err_count:%d, ms_num:%d, err_msg:%s]", fd, node->ip, node->port,
                      node->err_count, cache->ms_num, strerror(errno));
        }
        if (fd < 0)
        {
            node->err_count++;
            
            DLINK_REMOVE(indx);
            DLINK_INSERT_PREV(&cache->mslist_head,indx);
            indx = cache->mslist_head._next;
            try_time++;
            if (try_time == cache->ms_num)
            {
                WARNING_LOG("can't connect,[ip:%s,port:%d,try_time:%u, err_count:%d, ms_num:%d, err_msg:%s]",
                            node->ip,node->port,try_time, node->err_count, cache->ms_num, strerror(errno));
                break;
            }
        }
        else
        {
            node->err_count = 0;
            break;
        }
    }
    pthread_mutex_unlock(&cache->mutex);
    return fd;
}

int zcache_load_msaddrs(single_zcache_t *cache,const char *addrs)
{
    if (cache==NULL || addrs==NULL)
    {
        WARNING_LOG("param null");
        return -1;
    }

    
    const char *begin = addrs;
    const char *bc;
    int port;
    int iplen=0;
    zcacheapi_node_t *node = NULL;
    struct sockaddr_in sockaddr;
    while (begin[0] != '\0')
    {
        char ip_tmp[64] ={0};
        node = NEW zcacheapi_node_t;
        if (node==NULL)
        {
            WARNING_LOG("new zcacheapi_node_t error");
            goto err_exit;
        }
        memset(node,0,sizeof(zcacheapi_node_t));
        DLINK_INSERT_NEXT(&cache->mslist_head,&node->link);
        cache->ms_num++;
        bc = index(begin,':');
        if (bc == NULL)
        {
            WARNING_LOG("format error,no :");
            goto err_exit;
        }
        iplen=bc-begin;
        if (iplen>=(int)sizeof(node->ip))
        {
            WARNING_LOG("too long ip,[iplen:%d]",iplen);
            goto err_exit;
        }
        if (isdigit(*begin))
        {
            strncpy(node->ip,begin,iplen);
        }
        else
        {
            strncpy(ip_tmp,begin,iplen);
            if (ul_gethostipbyname_r(ip_tmp, &sockaddr, BD_ASCII, node->ip) < 0)
            {
                WARNING_LOG("unknown host name [hostname:%s]", ip_tmp);
                goto err_exit;
            }
        }
        if (inet_pton(AF_INET, node->ip, &port) <= 0)
        {
            WARNING_LOG("error [ip:%s]",node->ip);
            goto err_exit;
        }
        begin = bc+1;
        if (begin[0]=='\0')
        {
            WARNING_LOG("no port");
            goto err_exit;
        }
        if (sscanf(begin,"%d",&port) != 1)
        {
            WARNING_LOG("port error");
            goto err_exit;
        }
        else if (port < 0 || port > 65535)
        {
            WARNING_LOG("port error,[port:%d]",port);
            goto err_exit;
        }
        else
        {
            node->port = port;
            NOTICE_LOG("add metaserver,[ip:%s,port:%d]",node->ip,node->port);
            bc=index(begin,',');
            if (bc==NULL)
            {
                break;
            }
            else
            {
                begin = bc+1;
            }
        }
    }
    return 0;
err_exit:
    return -1;
}
