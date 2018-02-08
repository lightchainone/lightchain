#ifndef _ASYNCREMOTEINVOKER_H_
#define _ASYNCREMOTEINVOKER_H_
#include <sys/uio.h>
#include <stdio.h>
#include <stdlib.h>
#include "windef.h"
#include "hash.h"
#include "lnhead.h"
#include "hashtable.h"
#include "SyncObj.h"
#include "Kylin.h"
#include "Singleton.h"
#include "BufHandle.h"
#include "NetworkMan.h"
#include "link.h"
#include "KylinNS.h"
#include "Async.h"
#include "ThreadPool.h"
#include "EPoller.h"
#include "Socket.h"
#include "ExecMan.h"

#define KYLIN_NETWORK_THREAD_NUM 1






typedef struct  _async_remote_invoker_arg_t
{
    BufHandle req_handle;
    BufHandle res_handle;
    unsigned int  ip[2];
    unsigned int  port;
    unsigned int  status;
    unsigned int  timeout_ms;
    unsigned int  logid;
    void *    arg;
}async_remote_invoker_arg_t;

typedef void (*async_remote_cb_t)(async_remote_invoker_arg_t *arg);


typedef struct _async_server_t
{
    DLINK    server_list;                                            
    DLINK    client_list;                                            
    unsigned int ip;                                                 
    unsigned int port;                                               
} async_server_t;


#define AA_TIMEOUT  10000       
#define AA_DELAY    10001       
#define AA_NEW_TASK 10002       
#define AA_RELEASE  10003       

class AsyncRemoteMgr;
class AsyncRemoteClient:plclic CSocketV
{
    friend class AsyncRemoteMgr;
    const char *GetProcessState();
protected:
    AsyncRemoteClient(AsyncRemoteMgr *mgr);
    void OnCompletion(AsyncContext* pContext);

    
    bool OnConnected(APF_ERROR nErrCode);

    void OnWritten(APF_ERROR nErrCode, NetRequest* pReq);
    void OnRead(APF_ERROR nErrCode, NetRequest* pReq);
    void OnBroken(APF_ERROR nErrCode);
    void OnTimeout();
    int  Release();
    void OnClear();

    enum
    {
        CONNECTED = 1, SENDED , RECVED_NSHEAD, RECVED_BUF, IDLE, CLOSED
    };


plclic:
    ~AsyncRemoteClient();


    
    int BeginJob(async_remote_invoker_arg_t *arg,async_remote_cb_t cb,unsigned int connect_timeout_ms, bool re_alloc = true);


    unsigned int *GetIpPort()
    {
        return _con_ip_port;
    }

    void Shutdown();

    int Delay(int action,unsigned int nMilliseconds, AsyncContext* pCtx);

    DLINK* GetHashLink();

    DLINK* GetListLink();

    DLINK* GetFreeLink();

    static AsyncRemoteClient *GetThisByList(DLINK* link);

    static AsyncRemoteClient *GetThisByHash(DLINK* link);
    static AsyncRemoteClient *GetThisByFree(DLINK* link);

private:
    void _finish(NetRequest *nreq);
    
    int _delay(int action,unsigned int nMilliseconds, AsyncContext* pCtx);
    int _cancel();

	pthread_mutex_t _mutex;
    unsigned int _ip_port[3];                                        
    unsigned int _con_ip_port[2];                                    
    BufHandle  *_req_handle;                                         
    BufHandle  *_res_handle;                                         
    BufHandle  *_cur_res_handle;                                     

    int   _unreaded_len;                                             
    async_remote_invoker_arg_t *_arg;                                
    async_remote_cb_t   _cb;                                         
    short    _process_state;                                         
    unsigned long long _begin_time;
    unsigned long long _process_time[2];
    unsigned int _process_byte[2];

    NetRequest _delay_req;                                           
    bool _is_time_put;                                               
    bool       _re_alloc;                                            
    int    _tried_idx;                                               
    char _tried_ip[128];                                             
    DLINK _hashlink;
    DLINK _listlink;
    DLINK _freelink;                                                 
    AsyncRemoteMgr *_mgr;                                            
};


class AsyncServerItemOp:plclic HashItemOperation<async_server_t, char>
{
plclic:
    unsigned int GetHashOfKey(const char *key) const
    {
        return hash(key, 2*sizeof(unsigned int), 0);
    }

    bool IsEqual(const async_server_t *svr, const char *key) const
    {
        if (svr == NULL || key == NULL)
        {
            return false;
        }
        unsigned int *ip = (unsigned int*)key;
        unsigned int *port = (unsigned int*)((char*)key + sizeof(unsigned int));
        return(svr->ip == *ip   && svr->port == *port);
    }

    DLINK* GetList(async_server_t *svr)
    {
        if (svr == NULL)
        {
            return NULL;
        }
        else
        {
            return &(svr->server_list);
        }
    }

    async_server_t *GetItem(DLINK *list)
    {
        if (list == NULL)
        {
            return NULL;
        }
        else
        {
            return CONTAINING_RECORD(list, async_server_t,server_list);
        }
    }

    unsigned int GetHash(const async_server_t *svr) const
    {
        unsigned int ip_port[2];
        ip_port[0] = svr->ip;
        ip_port[1] = svr->port;
        return hash(ip_port,2*sizeof(unsigned int));
    }
};

class AsyncRemoteMgr
{
    
plclic:
    friend class  AsyncRemoteClient;

    AsyncRemoteMgr();
    ~AsyncRemoteMgr();

    
    int Init(int time_to_close_ms,int max_fdnum,unsigned int connect_timeout_ms=150);

    void SetConnTimeout(unsigned int connect_timeout_ms);

    
    int AsyncRemoteInvoker(async_remote_invoker_arg_t *arg, async_remote_cb_t cb, bool re_alloc = true);

    
    int AsyncExec(async_remote_invoker_arg_t *arg,
                  async_remote_cb_t cb,
                  bool re_alloc);


    
    int AsyncTimerInvoker(async_remote_invoker_arg_t *arg,
                          async_remote_cb_t cb,unsigned int timeoutms);

    
    

    
    int Stop();

    
    void Clear();

protected:
    
    
    
    void ReleaseAsyncClient(AsyncRemoteClient *client,bool isok);

private:
    HashTable<AsyncServerItemOp> _svr_hashtable;                     
    DLINK       _free_svr_list;                                      

    DLINK       _free_client_list;                                   
    DLINK       _used_client_list;                                   
    DLINK       _all_client_list;                                    
    AsyncRemoteClient *_delay_client;

    async_server_t  *_free_svr_array;                                

	pthread_mutex_t _mutex;

    int     _released_count;                                         
    int     _time_to_close;
    int     _max_fdnum;
    unsigned int    _connect_timeout_ms;
    volatile int    _used_async_client_count;                        
    int             _max_used_async_client_count;                    
    volatile int    _asyncclient_ref_num;                            
    bool            _stopped;                                        
    AsyncRemoteClient* _getClient(async_remote_invoker_arg_t *arg);

    int             _GetRefNum();
    void            _RefNumDec();
    void            _RefNumInc();
};

#endif
