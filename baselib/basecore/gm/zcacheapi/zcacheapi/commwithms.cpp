#include "zcachedata.h"
#include "lnhead.h"
#include "mynet.h"
#include "msgdef.h"
#include "util.h"
#include "commwithms.h"

#define ZCACHE_BLOCK_MAX_ERR_TIME 3     


void *block_update_routine(void *arg)
{
    single_zcache_t *cache = (single_zcache_t *)arg;
    zcache_err_t err = ZCACHE_OK;
    zlog_open_r("commwithms");
    while (1)
    {
        if (!cache->is_running)
        {
            pthread_exit(0);
        }
        if (!cache->is_block_fetched)
        {
            zlog_set_thlogid((unsigned int)random());
            if (get_all_meta(cache,&err)==0
                && err==ZCACHE_OK)
            {
                cache->is_block_fetched=true;
                break;
            }
            struct timeval v;
            time_to_val(1000*cache->ms_refetch_time_ms, v);
            select(0, NULL, NULL, NULL, &v);
        }
        else
        {
            break;
        }
    }
    while (1)
    {
        if (!cache->is_running)
        {
            pthread_exit(0);
        }
        struct timeval v;
        time_to_val(1000*cache->ms_refetch_time_ms, v);
        select(0, NULL, NULL, NULL, &v);
        zlog_set_thlogid((unsigned int)random());
        get_some_meta(cache,cache->reqbuf,cache->reqlen,cache->resbuf,cache->reslen,&err);
    }
}

#ifndef __USE_STLC_METASERVER__


int get_all_meta(single_zcache_t *cache,zcache_err_t *err_out)
{
    int fd = zcache_connect_ms(cache);
    if (fd < 0)
    {
        *err_out = ZCACHE_ERR_NET;
        return -1;
    }

    int ret = 0;
    int resbodylen = sizeof(zcache_cmd_lib_getallmeta_res_t)
                     +sizeof(zcache_node_t)*ZCACHE_MAX_NODE_NUM
                     +sizeof(unsigned short)*ZCACHE_MAX_BLOCK_NUM;
    cmd_head_t *cmd_head = NULL;
    zcache_cmd_lib_getallmeta_req_t *req = NULL;
    zcache_cmd_lib_getallmeta_res_t *res = NULL;
    char reqbuf[sizeof(zcache_head_t)+sizeof(cmd_head_t)+sizeof(zcache_cmd_lib_getallmeta_req_t)];
    char reshead[sizeof(zcache_head_t)+sizeof(cmd_head_t)];

    short *blocks=NULL;
    int nodeindx = 0;

    char *resbody = (char *)malloc(resbodylen);
    if (resbody == NULL)
    {
        WARNING_LOG("malloc resbody error");
        *err_out = ZCACHE_ERR_LIB;
        goto err_exit;
    }

    lnhead_set((lnhead_t*)reqbuf,gen_logid(),ZCACHEAPI_MODULE,
               sizeof(zcache_head_t)-sizeof(lnhead_t)+sizeof(cmd_head_t)+sizeof(zcache_cmd_lib_getallmeta_req_t));
    ((zcache_head_t*)(reqbuf))->cmd_num = 1;
    cmd_head_set(((zcache_head_t*)(reqbuf))->cmds,
                 ZCACHE_CMD_LIB_GETALLBLOCK,0,sizeof(zcache_cmd_lib_getallmeta_req_t));
    req = (zcache_cmd_lib_getallmeta_req_t*)(((zcache_head_t*)(reqbuf))->cmds[0].cmd_body);
    snprintf(req->product,sizeof(req->product),"%s",cache->product->GetName());
    snprintf(req->token,sizeof(req->token),"%s",cache->product->GetToken());

    ret = lnhead_write(fd,(lnhead_t*)reqbuf,cache->ms_write_timeout_ms);
    if (ret < 0)
    {
        *err_out = ZCACHE_ERR_NET;
        goto err_exit;
    }
    ret = lnhead_read(fd,(lnhead_t*)reshead,
                      reshead+sizeof(lnhead_t),sizeof(cmd_head_t)+sizeof(zcache_head_t)-sizeof(lnhead_t),
                      resbody,resbodylen,cache->ms_read_timeout_ms);
    
    close(fd);
    fd = -1;

    if (ret < 0)
    {
        *err_out = ZCACHE_ERR_NET;
        goto err_exit;
    }

    if (((zcache_head_t*)reshead)->cmd_num != 1)
    {
        WARNING_LOG("error,[cmd_num:%u]",((zcache_head_t*)reshead)->cmd_num);
        *err_out = ZCACHE_ERR_METASERVER;
        goto err_exit;
    }

    cmd_head = ((zcache_head_t*)reshead)->cmds;
    if (cmd_head->cmd_ret != ZCACHE_OK)
    {
        WARNING_LOG("error,[cmdret:%s]",get_err_code(zcache_err_t(cmd_head->cmd_ret)));
        *err_out = (zcache_err_t)cmd_head->cmd_ret;
        goto err_exit;
    }
    else if (cmd_head->body_len !=
             ((lnhead_t*)reshead)->body_len+sizeof(lnhead_t)-sizeof(zcache_head_t)-sizeof(cmd_head_t))
    {
        WARNING_LOG("error bodylen,[msgbodylen:%u,cmdbodylen:%u]",
                    ((lnhead_t*)reshead)->body_len,cmd_head->body_len);
        *err_out = ZCACHE_ERR_METASERVER;
        goto err_exit;
    }
    res = (zcache_cmd_lib_getallmeta_res_t*)resbody;
    if (res->block_num*sizeof(unsigned short)+res->node_num*sizeof(zcache_node_t)
        +sizeof(zcache_cmd_lib_getallmeta_res_t)
        != cmd_head->body_len)
    {
        WARNING_LOG("error cmd,[block_num:%u,node_num:%u,body_len:%u]",
                    res->block_num,res->node_num,cmd_head->body_len);
        *err_out = ZCACHE_ERR_METASERVER;
        goto err_exit;
    }
    else if (res->block_num==0 || res->node_num==0)
    {
        WARNING_LOG("error cmd,[block_num:%u,node_num:%u,body_len:%u]",
                    res->block_num,res->node_num,cmd_head->body_len);
        *err_out = ZCACHE_ERR_METASERVER;
        goto err_exit;
    }
    
    if (cache->product->Init(ZCACHE_BLOCK_MAX_ERR_TIME,res->block_num) < 0)
    {
        *err_out = ZCACHE_ERR_LIB;
        goto err_exit;
    }
    
    for (unsigned int i=0;i<res->node_num;i++)
    {
        
        if (res->nodes[i].ip[0] == 0)
            continue;
        nodeindx = cache->product->AddNode(res->nodes[i].ip,res->nodes[i].port);
        if (nodeindx < 0)
        {
            *err_out = ZCACHE_ERR_LIB;
            goto err_exit;
        }
        else
        {
            
            res->nodes[i].ip[0] = (unsigned int)nodeindx;
        }
    }
    
    blocks = (short*)(&res->nodes[res->node_num]);
    for (unsigned int i=0;i<res->block_num;i++)
    {
        if (blocks[i] >= 0)
        {
            if (cache->product->Update(i,(int)res->nodes[blocks[i]].ip[0]) < 0)
            {
                *err_out = ZCACHE_ERR_LIB;
                goto err_exit;
            }
        }
        else
        {
            if (cache->product->Update(i,-1) < 0)
            {
                *err_out = ZCACHE_ERR_LIB;
                goto err_exit;
            }
        }
    }
    NOTICE_LOG("[product:%s,nodenum:%u,blocknum:%u]",cache->product->GetName(),res->node_num,res->block_num);
    if (resbody != NULL)
    {
        free(resbody);
    }
    return 0;

err_exit:
    if (fd >= 0)
    {
        close(fd);
    }
    if (resbody != NULL)
    {
        free(resbody);
    }
    WARNING_LOG("[err:%d]",*err_out);
    return -1;
}

int get_some_meta(single_zcache_t *cache,char *reqbuf,int reqlen,char *resbuf,int reslen,zcache_err_t *err_out)
{
    if (reqlen < int(sizeof(zcache_head_t)+sizeof(cmd_head_t)
                     +sizeof(zcache_cmd_lib_getsomemeta_req_t)+sizeof(int)*ZCACHE_BLOCK_MAX_PERFETCH))
    {
        WARNING_LOG("reqlen too short, [reqlen:%d]",reqlen);
        *err_out = ZCACHE_ERR_PARAM;
        return -1;
    }
    if (reslen < int(sizeof(zcache_head_t)+sizeof(cmd_head_t)
                     +sizeof(zcache_cmd_lib_getsomemeta_res_t)))
    {
        WARNING_LOG("reslen too short, [reqlen:%d]",reslen);
        *err_out = ZCACHE_ERR_PARAM;
        return -1;
    }

    int *blocks = (int*)(reqbuf+sizeof(zcache_head_t)+
                         sizeof(cmd_head_t)+sizeof(zcache_cmd_lib_getsomemeta_req_t));

    int fd=-1;
    int ret=0;

    
    int fetchcount = cache->product->GetInvalidBlocks(blocks,ZCACHE_BLOCK_MAX_PERFETCH);
    if (fetchcount < 0)
    {
        *err_out = ZCACHE_ERR_LIB;
        return -1;
    }
    else if (fetchcount == 0)
    {
        *err_out = ZCACHE_OK;
        return 0;
    }

    zcache_cmd_lib_getsomemeta_req_t *cmdreqbody=(zcache_cmd_lib_getsomemeta_req_t*)
                                                 ((zcache_head_t*)reqbuf)->cmds[0].cmd_body;
    lnhead_set((lnhead_t*)reqbuf,gen_logid(),ZCACHEAPI_MODULE,
               sizeof(zcache_head_t)-sizeof(lnhead_t)
               +sizeof(cmd_head_t)+sizeof(zcache_cmd_lib_getsomemeta_req_t)
               +fetchcount*sizeof(int));
    ((zcache_head_t*)reqbuf)->cmd_num = 1;
    cmd_head_set(((zcache_head_t*)reqbuf)->cmds,
                 ZCACHE_CMD_LIB_GETSOMEBLOCK,0,sizeof(zcache_cmd_lib_getsomemeta_req_t)+fetchcount*sizeof(int));
    snprintf(cmdreqbody->product,sizeof(cmdreqbody->product),"%s",cache->product->GetName());
    snprintf(cmdreqbody->token,sizeof(cmdreqbody->token),"%s",cache->product->GetToken());
    cmdreqbody->block_num=fetchcount;

    
    fd = zcache_connect_ms(cache);
    if (fd < 0)
    {
        *err_out = ZCACHE_ERR_NET;
        goto err_exit;
    }

    ret = lnhead_write(fd,(lnhead_t*)reqbuf,cache->ms_write_timeout_ms);
    if (ret < 0)
    {
        *err_out = ZCACHE_ERR_NET;
        goto err_exit;
    }
    ret = lnhead_read(fd,resbuf,reslen,cache->ms_read_timeout_ms);
    if (ret < 0)
    {
        *err_out = ZCACHE_ERR_NET;
        goto err_exit;
    }
    close(fd);
    fd = -1;

    
    if (((zcache_head_t*)resbuf)->msghead.body_len<
        sizeof(zcache_head_t)-sizeof(lnhead_t)+sizeof(cmd_head_t))
    {
        *err_out = ZCACHE_ERR_METASERVER;
        goto err_exit;
    }
    else if (((zcache_head_t*)resbuf)->cmds[0].cmd_ret != ZCACHE_OK)
    {
        WARNING_LOG("[cmd_ret:%u]",((zcache_head_t*)resbuf)->cmds[0].cmd_ret);
        *err_out = (zcache_err_t)((zcache_head_t*)resbuf)->cmds[0].cmd_ret;
        goto err_exit;
    }
    else if (((zcache_head_t*)resbuf)->cmds[0].body_len < sizeof(zcache_cmd_lib_getsomemeta_res_t))
    {
        WARNING_LOG("[cmd_body_len:%u]",((zcache_head_t*)resbuf)->cmds[0].body_len);
        *err_out = ZCACHE_ERR_METASERVER;
        goto err_exit;
    }
    else if (((zcache_head_t*)resbuf)->msghead.body_len
             !=((zcache_head_t*)resbuf)->cmds[0].body_len+sizeof(cmd_head_t)+sizeof(zcache_head_t)-sizeof(lnhead_t))
    {
        WARNING_LOG("[msgbodylen:%u,cmdbodylen:%u]",
                    ((zcache_head_t*)resbuf)->cmds[0].body_len,((zcache_head_t*)resbuf)->cmds[0].body_len);
        *err_out = ZCACHE_ERR_METASERVER;
        goto err_exit;
    }
    else
    {
        zcache_cmd_lib_getsomemeta_res_t *resbody = (zcache_cmd_lib_getsomemeta_res_t*)
                                                    ((zcache_head_t*)resbuf)->cmds[0].cmd_body;
        if (resbody->node_num*sizeof(zcache_node_t)+resbody->block_num*sizeof(int)*2
            != ((zcache_head_t*)resbuf)->cmds[0].body_len-sizeof(zcache_cmd_lib_getsomemeta_res_t))
        {
            WARNING_LOG("[nodenum:%u,blocknum:%u]",resbody->node_num,resbody->block_num);
            *err_out = ZCACHE_ERR_METASERVER;
            goto err_exit;
        }
        unsigned int blockid;
        int nodeid;
        zcache_node_t *node;
        
        for (unsigned int i=0;i<resbody->block_num;i++)
        {
            
            blockid = *((unsigned int*)((char*)resbody+sizeof(zcache_cmd_lib_getsomemeta_res_t)+
                                        sizeof(zcache_node_t)*resbody->node_num+sizeof(unsigned int)*2*i));
            
            nodeid = *((int*)((char*)resbody+sizeof(zcache_cmd_lib_getsomemeta_res_t)+
                              sizeof(zcache_node_t)*resbody->node_num+sizeof(unsigned int)*(2*i+1)));
            if (nodeid>=0 && nodeid < (int)resbody->node_num)
            {
                node = &resbody->nodes[nodeid];
                if (cache->product->Update(blockid,node->ip,node->port) < 0)
                {
                    *err_out = ZCACHE_ERR_LIB;
                    goto err_exit;
                }
                TRACE_LOG("update,[blockid:%u,ip0:%u,ip1:%u,port:%u]",blockid,node->ip[0],node->ip[1],node->port);
            }
            else
            {
                TRACE_LOG("update fail block invalid,[blockid:%u]",blockid);
            }
        }

        *err_out = ZCACHE_OK;
        NOTICE_LOG("[product:%s,nodenum:%u,blocknum:%u]",cache->product->GetName(),resbody->node_num,resbody->block_num);
    }
    return 0;

err_exit:
    if (fd >= 0)
    {
        close(fd);
    }
    WARNING_LOG("[err:%d]",*err_out);
    return -1;

}


int auth_with_meta(single_zcache_t *cache,zcache_err_t *err_out)
{
    char reqbuf[sizeof(lnhead_t)+zcachemsg_minlen(1)+sizeof(zcache_cmd_lib_getsomemeta_req_t)];
    char resbuf[sizeof(lnhead_t)+zcachemsg_minlen(1)+sizeof(zcache_cmd_lib_getsomemeta_res_t) +
                ZCACHE_MAX_NODE_NUM*sizeof(zcache_node_t)];
    zcache_cmd_lib_getsomemeta_req_t* cmdreqbody=NULL;
    int ret = 0;

    
    int fd = zcache_connect_ms(cache);
    if (fd < 0)
    {
        *err_out = ZCACHE_ERR_NET;
        goto err_exit;
    }
    lnhead_set((lnhead_t*)reqbuf,gen_logid(),ZCACHEAPI_MODULE, zcachemsg_minlen(1)+sizeof(zcache_cmd_lib_getsomemeta_req_t));
    ((zcache_head_t*)reqbuf)->cmd_num = 1;
    cmd_head_set(((zcache_head_t*)reqbuf)->cmds,
                 ZCACHE_CMD_LIB_GETSOMEBLOCK,0,sizeof(zcache_cmd_lib_getsomemeta_req_t));
    cmdreqbody=(zcache_cmd_lib_getsomemeta_req_t*)((zcache_head_t*)reqbuf)->cmds[0].cmd_body;
    snprintf(cmdreqbody->product,sizeof(cmdreqbody->product),"%s",cache->product->GetName());
    snprintf(cmdreqbody->token,sizeof(cmdreqbody->token),"%s",cache->product->GetToken());
    cmdreqbody->block_num=0;

    ret = lnhead_write(fd,(lnhead_t*)reqbuf,cache->ms_write_timeout_ms);
    if (ret < 0)
    {
        *err_out = ZCACHE_ERR_NET;
        goto err_exit;
    }
    ret = lnhead_read(fd,resbuf,sizeof(resbuf),cache->ms_read_timeout_ms);
    if (ret < 0)
    {
        *err_out = ZCACHE_ERR_NET;
        goto err_exit;
    }
    close(fd);
    fd = -1;

    
    if (((zcache_head_t*)resbuf)->msghead.body_len<zcachemsg_minlen(1))
    {
        *err_out = ZCACHE_ERR_METASERVER;
    }
    else
    {
        *err_out = (zcache_err_t)((zcache_head_t*)resbuf)->cmds[0].cmd_ret;
    }

    return(*err_out==ZCACHE_OK)?0:-1;

err_exit:
    if (fd >= 0)
    {
        close(fd);
    }
    WARNING_LOG("[err:%d]",*err_out);
    return -1;
}



#else


    #define TEST_PRODUCT_BLOCK_NUM 10000

int get_all_meta(single_zcache_t *cache,zcache_err_t *err_out)
{
    int resbodylen = sizeof(zcache_cmd_lib_getallmeta_res_t)
                     +sizeof(zcache_node_t)*ZCACHE_MAX_NODE_NUM
                     +sizeof(unsigned short)*ZCACHE_MAX_BLOCK_NUM;
    cmd_head_t *cmd_head = NULL;
    zcache_cmd_lib_getallmeta_res_t *res = NULL;
    char reshead[sizeof(zcache_head_t)+sizeof(cmd_head_t)];

    short *blocks=NULL;
    int nodeindx = 0;

    zcacheapi_node_t *node=CONTAINING_RECORD(cache->mslist_head._next,zcacheapi_node_t,link);

    char *resbody = (char *)malloc(resbodylen);
    if (resbody == NULL)
    {
        WARNING_LOG("malloc resbody error");
        *err_out = ZCACHE_ERR_LIB;
        goto err_exit;
    }
    memset(resbody,0,resbodylen);

    cmd_head = ((zcache_head_t*)reshead)->cmds;

    res = (zcache_cmd_lib_getallmeta_res_t*)resbody;
    res->block_num = TEST_PRODUCT_BLOCK_NUM;
    res->node_num = 1;
    res->nodes[0].ip[0] = inet_addr(node->ip);
    res->nodes[0].port = node->port;

    
    if (cache->product->Init(ZCACHE_BLOCK_MAX_ERR_TIME,res->block_num) < 0)
    {
        *err_out = ZCACHE_ERR_LIB;
        goto err_exit;
    }
    
    for (unsigned int i=0;i<res->node_num;i++)
    {
        nodeindx = cache->product->AddNode(res->nodes[i].ip,res->nodes[i].port);
        if (nodeindx < 0)
        {
            *err_out = ZCACHE_ERR_LIB;
            goto err_exit;
        }
        else
        {
            
            res->nodes[i].ip[0] = (unsigned int)nodeindx;
        }
    }
    
    blocks = (short*)(&res->nodes[res->node_num]);
    for (unsigned int i=0;i<res->block_num;i++)
    {
        if (blocks[i] >= 0)
        {
            if (cache->product->Update(i,(int)res->nodes[blocks[i]].ip[0]) < 0)
            {
                *err_out = ZCACHE_ERR_LIB;
                goto err_exit;
            }
        }
        else
        {
            if (cache->product->Update(i,-1) < 0)
            {
                *err_out = ZCACHE_ERR_LIB;
                goto err_exit;
            }
        }
    }
    NOTICE_LOG("[product:%s,nodenum:%u,blocknum:%u]",cache->product->GetName(),res->node_num,res->block_num);
    if (resbody != NULL)
    {
        free(resbody);
    }
    return 0;

err_exit:
    if (resbody != NULL)
    {
        free(resbody);
    }
    return -1;
}

int get_some_meta(single_zcache_t *cache,char *reqbuf,int reqlen,char *resbuf,int reslen,zcache_err_t *err_out)
{
    if (reqlen < int(sizeof(zcache_head_t)+sizeof(cmd_head_t)
                     +sizeof(zcache_cmd_lib_getsomemeta_req_t)+sizeof(int)*ZCACHE_BLOCK_MAX_PERFETCH))
    {
        WARNING_LOG("reqlen too short, [reqlen:%d]",reqlen);
        *err_out = ZCACHE_ERR_PARAM;
        return -1;
    }
    if (reslen < int(sizeof(zcache_head_t)+sizeof(cmd_head_t)
                     +sizeof(zcache_cmd_lib_getsomemeta_res_t)))
    {
        WARNING_LOG("reslen too short, [reqlen:%d]",reslen);
        *err_out = ZCACHE_ERR_PARAM;
        return -1;
    }

    int *blocks = (int*)(reqbuf+sizeof(zcache_head_t)+
                         sizeof(cmd_head_t)+sizeof(zcache_cmd_lib_getsomemeta_req_t));

    int fd=-1;

    
    int fetchcount = cache->product->GetInvalidBlocks(blocks,ZCACHE_BLOCK_MAX_PERFETCH);
    if (fetchcount < 0)
    {
        *err_out = ZCACHE_ERR_LIB;
        return -1;
    }
    else if (fetchcount == 0)
    {
        *err_out = ZCACHE_OK;
        return 0;
    }

    WARNING_LOG("blocks need refetch,[fetchcount:%d]",fetchcount);
    zcache_cmd_lib_getsomemeta_req_t *cmdreqbody=(zcache_cmd_lib_getsomemeta_req_t*)((zcache_head_t*)reqbuf)->cmds[0].cmd_body;
    lnhead_set((lnhead_t*)reqbuf,gen_logid(),ZCACHEAPI_MODULE,
               sizeof(zcache_head_t)-sizeof(lnhead_t)
               +sizeof(cmd_head_t)+sizeof(zcache_cmd_lib_getsomemeta_req_t)
               +fetchcount*sizeof(int));
    ((zcache_head_t*)reqbuf)->cmd_num = 1;
    cmd_head_set(((zcache_head_t*)reqbuf)->cmds,
                 ZCACHE_CMD_LIB_GETSOMEBLOCK,0,sizeof(zcache_cmd_lib_getsomemeta_req_t)+fetchcount*sizeof(int));
    snprintf(cmdreqbody->product,sizeof(cmdreqbody->product),"%s",cache->product->GetName());
    snprintf(cmdreqbody->token,sizeof(cmdreqbody->token),"%s",cache->product->GetToken());
    cmdreqbody->block_num=fetchcount;

    zcacheapi_node_t *node=CONTAINING_RECORD(cache->mslist_head._next,zcacheapi_node_t,link);
    zcache_cmd_lib_getsomemeta_res_t *resbody = (zcache_cmd_lib_getsomemeta_res_t*)((zcache_head_t*)resbuf)->cmds[0].cmd_body;

    

    resbody->node_num=1;
    resbody->block_num=fetchcount;
    resbody->nodes[0].ip[0] = inet_addr(node->ip);
    resbody->nodes[0].ip[1] = 0;
    resbody->nodes[0].port = node->port;

    
    for (unsigned int i=0;i<resbody->block_num;i++)
    {
        
        if (cache->product->Update(blocks[i],resbody->nodes[0].ip,resbody->nodes[0].port) < 0)
        {
            *err_out = ZCACHE_ERR_LIB;
            goto err_exit;
        }
        NOTICE_LOG("block update,[blockid:%u,ip0:%u,ip1:%u,port:%u]",
                   blocks[i],resbody->nodes[0].ip[0],resbody->nodes[0].ip[1],resbody->nodes[0].port);
    }

    *err_out = ZCACHE_OK;
    NOTICE_LOG("[product:%s,nodenum:%u,blocknum:%u]",cache->product->GetName(),resbody->node_num,resbody->block_num);
    return 0;

err_exit:
    if (fd >= 0)
    {
        close(fd);
    }
    return -1;


}


int auth_with_meta(single_zcache_t *cache,zcache_err_t *err_out)
{
    if (cache&&err_out)
    {
        *err_out=ZCACHE_OK;
    }

    return 0;

}

#endif
