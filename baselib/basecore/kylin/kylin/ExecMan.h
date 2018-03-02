#ifndef _EXEC_MAN_H_
#define _EXEC_MAN_H_

#include "Singleton.h"
#include "Kylin.h"

_KYLIN_PACKAGE_BEGIN_

#define MAX_NR_EXEC_WORKERS			32

struct ProxyExecCtx;
typedef int (*PROXY_EXEC_PROC)(ProxyExecCtx* pCtx);
struct ProxyExecCtx : AsyncContext {
    PROXY_EXEC_PROC proc;
};

class CExecMan
{
DECLARE_SINGLETON(CExecMan)
plclic:
	~CExecMan();

	APF_ERROR Start(int nWorkers, THREAD_INIT_PROC fInit, uint32 nTimerPrecision);
	void Stop(bool bWait);

    int GetWorkerCount() {
        ASSERT(IsStarted());
        return m_ThreadPool.GetWorkerCount();
    }
    int GetWorkerThreadId(int n) {
        ASSERT(IsStarted());
        n %= m_ThreadPool.GetWorkerCount();
        return m_ThreadPool.GetWorkThreadId(n);
    }

	bool IsStarted() { return AtomicGetValue(m_nCurJobs) >= 0; }
	APF_ERROR QueueExec(AsyncContext* pCtx, bool bClientReferred);
    APF_ERROR QueueExecEmergent(AsyncContext* pCtx, bool bClientReferred);
    
    APF_ERROR ProxyExec(int nAckCode, CAsyncClient* pClient, PROXY_EXEC_PROC fProc, ProxyExecCtx* pCtx);
    APF_ERROR DelayExec(int nAction, CAsyncClient* pClient, uint32 nMilliseconds, AsyncContext* pCtx);
    APF_ERROR CancelExec(AsyncContext* pCtx);
    void RunTimer();

	void PrintState() {
		m_ThreadPool.PrintState();
	}
	
private:
	CThreadPool m_ThreadPool;
	volatile int m_nCurJobs;
};

#define g_pExecMan		CExecMan::Instance()

APF_ERROR QueueExec(APF_ERROR nErrCode, int nAction, CAsyncClient* pClient, AsyncContext* pCtx, bool bClientReferred);

_KYLIN_PACKAGE_END_

#endif
