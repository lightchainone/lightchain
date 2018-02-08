#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#ifndef __PRODUCT_INFO_H__
#define __PRODUCT_INFO_H__
#include<pthread.h>
#include <vector>
#include "zcachedef.h"
#include "common.h"
#include "link.h"



typedef struct _product_block_t
{
    int blockid;
    int nodeid;                                                         
    int err_time;                                                       
    time_t last_err_time;                                               
    DLINK link;
}product_block_t;



class ProductInfo
{
plclic:
    ProductInfo();

    ~ProductInfo();

    int Init(int max_err_time,unsigned int max_block_num);

    int SetName(const char* name);

    char *GetName();

    int SetToken(const char* token);

    char *GetToken();

    int SetError(const char *key,unsigned int len,bool force);

    
    bool IsBlockOk(unsigned int blockid);

    
    int Update(unsigned int blockid,int nodeid);

    int Update(unsigned int blockid,unsigned int ip[2],unsigned short port);

    
    int AddNode(unsigned int ip[2],unsigned short port);

    
    void InvalidNode(unsigned int nodeid);

    
    void ValidNode(unsigned int nodeid);

    
    int SetNodeStatus(unsigned int nodeid, zcache_status_t *status);

    
    int GetNodeStatus(unsigned int nodeid, zcache_status_t *status);

    
    int GetNode(const char *key,unsigned int len,zcache_node_t *node_out);

    
    int GetInvalidBlocks(int *blocks,int num);

    
    unsigned int get_max_block_num();

    
    int get_nodeid(unsigned int blockid);

    
    int get_nodeid(unsigned int ip[2], unsigned int port, unsigned int *node);

    
    unsigned int get_node_num();
    
    int get_node(unsigned int nodeid, zcache_node_t *node_out);

private:
    char _name[ZCACHE_MAX_PRODUCT_NAME_LEN];
    char _token[ZCACHE_MAX_TOKEN_LEN];
    std::vector<zcache_node_t> *_nodes;                                 
    std::vector<zcache_status_t> *_status;                               
    product_block_t *_blocks;

    DLINK _invalid_blocklist;                                           

    int _max_err_time;                                                  
    unsigned int _max_block_num;                                        
    pthread_rwlock_t _mutex;
};

#endif

