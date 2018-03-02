#ifndef _NETWORK_MAN_H_
#define _NETWORK_MAN_H_

#include "KylinNS.h"
#include "Singleton.h"
#include "network.h"
#include "BufHandle.h"
#include "EPoller.h"
#include "Stl.h"

_KYLIN_PACKAGE_BEGIN_

#define MAX_NR_NETWORK_WORKERS          8

enum SocketState {
    SS_VOID = 0,
    SS_LISTENING_0,
    SS_LISTENING,
    SS_CONNECTING_0,
    SS_CONNECTING,
    SS_CONNECTED_0,
    SS_CONNECTED,
    SS_ERROR,
    SS_SHUTDOWN,
};

enum SocketFlag {
    SF_DIRECT_CALLBACK  = 0x1,
    SF_PERMANENT        = 0x2,
    SF_UDP              = 0x4,
    SF_DONT_EXHAUST     = 0x8,
};


struct NetRequest {
    union {
        AsyncContext async;
        DLINK link;
    };

    union {
        BufHandle hdl;
        struct {
            BufHandle *pHdl;    
            void* buf;
            int len;            
            int request;        
        };
    };
    int xfered;
    uint32 ip;                  
    uint16 port;                
};

class CEPoller;
typedef TLockedQueue<NetRequest, TLinkedList<NetRequest> > TranQueue;
struct NetContext {
    SOCKET s;
    SocketState state;

    DLINK link;                 
    CLockedInt tWrite, tRead;
    TranQueue qRead, qWrite;
    NetRequest *pReadReq, *pWriteReq;
    BufHandle wHdl;
    volatile int nDelayRead, nEnabled;
    uint32 nTimeout;
    uint64 tTimeoutCycle;

    CAsyncClient *pClient;
    CEPoller* pPoller;
    uint32 flag;

    union {
        struct {                    
            uint32 ip;
            uint16 port;
        };
        uint32 backlog;             
    };
};


typedef TLockedQueue<NetContext, TStlList<NetContext*> > NetContexts;
class CNetworkMan
{
DECLARE_SINGLETON(CNetworkMan)
plclic:
    ~CNetworkMan();
    
    APF_ERROR Start(int nWorkers, THREAD_INIT_PROC fInit);
    void Stop();

    typedef bool (*NET_CTX_ENUMERATOR)(NetContext* pCtx, ptr);  
    void Enumerate(NET_CTX_ENUMERATOR f, ptr p);
    
    APF_ERROR Associate(NetContext* pCtx);
    APF_ERROR Deassociate(NetContext* pCtx);
    
    void Listen(NetContext* pCtx, int backlog);
    void Connect(NetContext* pCtx, uint32 ip, uint16 port, void *pWriteBuf, 
                int nWriteCount, uint32 timeout, NetRequest* pReq);
    void Monitor(NetContext* pCtx);
    bool Delay(NetContext* pCtx, uint32 nMilliseconds);

    void Read(NetContext* pCtx, void* pBuf, int len, int request, NetRequest* pReq);
    void Write(NetContext* pCtx, void* pBuf, int count, NetRequest* pReq);
    
    void Read(NetContext* pCtx, NetRequest* pReq);
    void Write(NetContext* pCtx, NetRequest* pReq);
    
    bool IsStarted() { return 1==AtomicGetValue(m_bStarted); }
    void PrintState();
    void GetXfered(uint64* pnRead, uint64* pnWrite);
private:
    CThreadPool m_ThreadPool;
    volatile int m_bStarted;
    NetContexts m_NetContexts;
};

#define g_pNetworkMan       CNetworkMan::Instance()
extern void InitNetContext(NetContext* pCtx, SOCKET s, CAsyncClient* pClient, uint32 flag);

_KYLIN_PACKAGE_END_
#endif
