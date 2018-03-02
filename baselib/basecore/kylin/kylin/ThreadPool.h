#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include "extypes.h"
#include "DolcleList.h"
#include "SyncedQ.h"
#include "KylinNS.h"

_KYLIN_PACKAGE_BEGIN_

struct Job;
struct ControlJob;
struct WorkerContext;

typedef void (*JOB_PROC)(Job*);
typedef bool (*CONTROL_JOB_PROC)(ControlJob*, WorkerContext*);	
typedef void (*THREAD_INIT_PROC)(int type, int id);

struct Job {
	DLINK link;
	JOB_PROC fProc;
};
struct ControlJob {
	DLINK link;
	JOB_PROC fZero;				
	CONTROL_JOB_PROC fProc;		
};

typedef TSyncedQueue<Job, TLinkedList<Job> > JobQ;
struct WorkerContext {
    int type;
    int id;
    int thread_id;
	THREAD_INIT_PROC fInit;
	volatile int nJobs;
    bool bDoing;
	JobQ* pJobQ;
};


class CThreadPool
{
	bool m_bShareQ;
	int m_nWorkers, m_nMaxWorkers;
	volatile int m_nJobs;
	thread_t *m_hWorkerThreads;
	WorkerContext *m_pContexts;
	int m_nType;
    THREAD_INIT_PROC m_fInit;
	
	int _AddWorker(int nAdd);
	int _DelWorker(int nDel, bool bFinal);
plclic:
	CThreadPool(int type, int nMaxWorkers, bool bShareQ);
	virtual ~CThreadPool();
	
	void PrintState();
	int GetWorkerCount() const {
		return m_nWorkers;
	}
	
	int GetTotalJobCount() {
		return AtomicGetValue(m_nJobs);
	}
    
    int GetWorkThreadId(int nWhich) const {
        VERIFY_OR_RETURN(nWhich < m_nWorkers && nWhich > -1, 0);
        return m_pContexts[nWhich].thread_id;
    }

	
	int GetCurJobCount(int nWhich) const {
		ASSERT(nWhich<m_nWorkers && nWhich>=-1);

		if (-1 == nWhich) {
			return m_pContexts[nWhich].pJobQ->size();
		}
        if (m_bShareQ) {
			return m_pContexts[0].pJobQ->size();
        }
		int n = 0;
		for (int i=0; i<m_nWorkers; i++) {
			n += m_pContexts[i].pJobQ->size();
		}
		return n;
	}

	
	int Start(int nWorkers, THREAD_INIT_PROC fInit=NULL) {
		ASSERT(m_nWorkers == 0);
		if (nWorkers <= 0) {
			nWorkers = 1;
		}
		else if (nWorkers > m_nMaxWorkers) {
			nWorkers = m_nMaxWorkers;
		}
        m_fInit = fInit;
		_AddWorker(nWorkers);
		return m_nWorkers;
	}

	void Stop() {
		_DelWorker(m_nWorkers, true);
	}
	
	void QueueJob(Job* pJob, int nWhich) {
		ASSERT(m_nWorkers > 0);
		VERIFY_OR_RETURN(nWhich < m_nWorkers && nWhich >= -1, BLANK);
		
		int nJobs = atomic_add(&m_nJobs, 1);
        if (-1 == nWhich) {
			nWhich = nJobs % m_nWorkers;
        }
		atomic_add(&m_pContexts[nWhich].nJobs, 1);
		m_pContexts[nWhich].pJobQ->push_back(pJob);
	}
	
	void QueueEmergentJob(Job* pJob, int nWhich) {
        ASSERT(m_nWorkers > 0);
        VERIFY_OR_RETURN(nWhich < m_nWorkers && nWhich >= -1, BLANK);

        int nJobs = atomic_add(&m_nJobs, 1);
        if (-1 == nWhich) {
            nWhich = nJobs % m_nWorkers;
        }
        atomic_add(&m_pContexts[nWhich].nJobs, 1);
		m_pContexts[nWhich].pJobQ->push_front(pJob);
	}

    bool CancelJob(Job* pJob, int nWhich);
};

_KYLIN_PACKAGE_END_
#endif
