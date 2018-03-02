#ifndef _DISK_MAN_H_
#define _DISK_MAN_H_

#include <list>
#include "Singleton.h"
#include "ThreadPool.h"
#include "KylinNS.h"

_KYLIN_PACKAGE_BEGIN_

#define MAX_NR_DISKS				128

struct DiskContext {
	int fd;
	int diskno;					
	CAsyncClient *pClient;
	uint64 nCurOff, nRead, nWrite;
	char* pPath;				
	int nFlag;					
};


struct DiskRequest {
	union {
		AsyncContext async;
		Job job;
	};

	void *buf;
	int request;
	int xfered;
	uint64 off;
	DiskContext *pCtx;
};

class CDiskMan
{
DECLARE_SINGLETON(CDiskMan)
plclic:
	~CDiskMan();
	
	APF_ERROR Start(int nDisks, THREAD_INIT_PROC fInit);
	void Stop();
	
	APF_ERROR Associate(int diskno, int fd, CAsyncClient* pClient, DiskContext* pContext);
	APF_ERROR Associate(int diskno, char* pPath, int nFlag, CAsyncClient* pClient, DiskContext* pContext);
	APF_ERROR Deassociate(DiskContext* pContext);
	
	void Read(DiskContext* pContext, void* pBuf, int count, DiskRequest* pReq);
	void Write(DiskContext* pContext, void* pBuf, int count, DiskRequest* pReq);
	
	bool IsStarted() const { return m_bStarted; }
	int GetDiskCount() const { 
		ASSERT(IsStarted()); 
		return m_ThreadPool.GetWorkerCount(); 
	}
	
	void PrintState();

	void _QueueJob(Job* pJob, int nWhich) { 
		ASSERT(IsStarted()); 
		m_ThreadPool.QueueJob(pJob, nWhich); 
	}
	void _QueueEmergentJob(Job* pJob, int nWhich) { 
		ASSERT(IsStarted()); 
		m_ThreadPool.QueueEmergentJob(pJob, nWhich); 
	}

	void GetXfered(uint64* pnRead, uint64* pnWrite);
private:
	CThreadPool m_ThreadPool;
	bool m_bStarted;
};

#define g_pDiskMan		CDiskMan::Instance()

_KYLIN_PACKAGE_END_

#endif
