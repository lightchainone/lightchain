#ifndef _ASYNC_H_
#define _ASYNC_H_

#include "atomic.h"
#include "MemoryOp.h"
#include "KylinNS.h"

_KYLIN_PACKAGE_BEGIN_

enum AsyncAction {
    AA_LISTEN                   = 1,
    AA_CONNECT,
    AA_READ,
    AA_WRITE,
    AA_BROKEN,
    AA_OPEN,
    AA_READ_BLOCK,
    AA_WRITE_BLOCK,
    AA_DELETE_BLOCK,
    AA_MODIFY_BLOCK,
    AA_FUNC_BLOCK,
    AA_MESSAGE,
    AA_TIMER,
    AA_END                      = 0xff,
    AA_USER_BEGIN
};

class CAsyncClient;
struct AsyncContext : Job {
    APF_ERROR nErrCode;
    int nAction;
    CAsyncClient *pClient;
};

static inline
void InitAsyncContext(AsyncContext* pCtx)
{
    ZeroMemory(pCtx, sizeof(AsyncContext));
    DLINK_INITIALIZE(&pCtx->link);
}

static inline AsyncContext*
NewAsyncContext()
{
    AsyncContext* pCtx = new AsyncContext;
    InitAsyncContext(pCtx);
    return pCtx;
}

class CAsyncClient
{
protected:
    int m_nId, m_nHostId;
    volatile int m_nRef;
    char m_name[64];

    CAsyncClient(CAsyncClient* pHost);
    CAsyncClient();
    virtual ~CAsyncClient();

plclic:
    int GetId() const { return m_nId; }
    int GetAsyncId() const { return m_nHostId; }
    
    int GetHostThreadId() const;
    bool IsInHostThread() const;

    
    void SetHost(CAsyncClient* pHost);

#if 0
    virtual int AddRef() {
        int n = AtomicInc(m_nRef);
        TRACE0("%s(%p) AddRef %d\n", m_name, this, n);
        return n;
    }
    virtual int Release() {
        int n = AtomicDec(m_nRef);
        ASSERT(n >= 0);
        TRACE0("%s(%p) Release %d\n", m_name, this, n);
        return n;
    }
    virtual int GetRef() {
        int n = AtomicGetValue(m_nRef);
        TRACE0("%s(%p) GetRef: %d\n", m_name, this, n);
        return n;
    }
#else
    virtual int AddRef() {
        return AtomicInc(m_nRef);
    }
    virtual int Release() {
        return AtomicDec(m_nRef);
    }
    virtual int GetRef() {
        return AtomicGetValue(m_nRef);
    }
#endif
    virtual const char* GetName() const { return m_name; }
    void SetName(const char *fmt, ...);
    virtual void OnCompletion(AsyncContext* pCtx) = 0;
};

_KYLIN_PACKAGE_END_

#endif
