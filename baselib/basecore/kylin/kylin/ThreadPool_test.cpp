#include "stdafx.h"
#include "ThreadPool.h"
#include "Kylin.h"
#include "UnitTest.h"
#include "../test/KylinUtils4Test.h"

static const int TT_TEST = TT_PROXY + 1;
static int s_n = 0;

static inline void AccJob(Job* pJob)
{
	ASSERT(DLINK_IS_STANDALONE(&pJob->link));
	s_n++;
}

static inline void InitWorker(int type, int id)
{
	ASSERT(TT_TEST == type);

	ColorTrace(TC_White, "Test worker#%d init...\n", id);	
}

class ThreadPoolTest {
plclic:
	void SetUp() {}
	void TearDown() {}
};

TEST_F(ThreadPoolTest, Case1_test_Start)
{
	printf("test CThreadPool::Start()...\n");

	const int nMaxWorkers = 3;

	
	CThreadPool threadpool(TT_TEST, nMaxWorkers, false);
	threadpool.Start(0, NULL);
	ASSERT(1 == threadpool.GetWorkerCount());

	
	CThreadPool threadpool_2(TT_TEST, nMaxWorkers, false);
	threadpool_2.Start(4, NULL);
	ASSERT(nMaxWorkers == threadpool_2.GetWorkerCount());

	
	
	
	CThreadPool threadpool_3(TT_TEST, nMaxWorkers, false);
	threadpool_3.Start(2, InitWorker);
	ASSERT(2 == threadpool_3.GetWorkerCount());
	
}


TEST_F(ThreadPoolTest, Case2_test_QueueJob)
{
	printf("test CThreadPool::QueueJob()...\n");

	s_n = 0;
	const int nMaxWorkers = 3;
	const int nJobs = 100;

	
	CThreadPool* pPool = new CThreadPool(TT_TEST, nMaxWorkers, false);
	pPool->Start(3, NULL);

	Job* pJob = new Job[nJobs];
	for(int i = 0; i < nJobs; ++i) {
		DLINK_INITIALIZE(&pJob[i].link);
		pJob[i].fProc = AccJob;
		pPool->QueueJob(&pJob[i], -1);
	}

	delete pPool;
	delete pJob;

	ASSERT(100 == s_n);
}



static int s_n1[3];

static inline void SlcJob0(Job* pJob)
{
	ASSERT(DLINK_IS_STANDALONE(&pJob->link));
	s_n1[0]--;
}

static inline void SlcJob1(Job* pJob)
{
	ASSERT(DLINK_IS_STANDALONE(&pJob->link));
	s_n1[1]--;
}

static inline void SlcJob2(Job* pJob)
{
	ASSERT(DLINK_IS_STANDALONE(&pJob->link));
	s_n1[2]--;
}

TEST_F(ThreadPoolTest, Case3_test_QueueJob)
{
	printf("test CThreadPool::QueueJob()...\n");

	s_n1[0] = 10;
	s_n1[1] = 20;
	s_n1[2] = 30;
	const int nMaxWorkers = 3;

	
	CThreadPool* pPool = new CThreadPool(TT_TEST, nMaxWorkers, false);
	pPool->Start(3, NULL);

	Job* pJob0 = new Job[s_n1[0]];
	Job* pJob1 = new Job[s_n1[1]];
	Job* pJob2 = new Job[s_n1[2]];

	for(int i = 0; i < 10; ++i) { 
		DLINK_INITIALIZE(&pJob0[i].link);
		pJob0[i].fProc = SlcJob0;
		pPool->QueueJob(&pJob0[i], 0);
	}

	for(int i = 0; i < 20; ++i) { 
		DLINK_INITIALIZE(&pJob1[i].link);
		pJob1[i].fProc = SlcJob1;
		pPool->QueueJob(&pJob1[i], 1);
	}

	for(int i = 0; i < 30; ++i) { 
		DLINK_INITIALIZE(&pJob2[i].link);
		pJob2[i].fProc = SlcJob2;
		pPool->QueueJob(&pJob2[i], 2);
	}

	
	
	

	delete pPool;
	delete pJob0;
	delete pJob1;
	delete pJob2;

	ASSERT(0 == s_n1[0]);
	ASSERT(0 == s_n1[1]);
	ASSERT(0 == s_n1[2]);
}


TEST_F(ThreadPoolTest, Case4_test_QueueJob)
{
	printf("test CThreadPool::QueueJob() in shareQ...\n");

	s_n = 0;
	const int nMaxWorkers = 3;
	const int nJobs = 100;

	
	CThreadPool* pPool = new CThreadPool(TT_TEST, nMaxWorkers, true);
	pPool->Start(3, NULL);

	Job* pJob = new Job[nJobs];
	for(int i = 0; i < nJobs; ++i) {
		DLINK_INITIALIZE(&pJob[i].link);
		pJob[i].fProc = AccJob;
		pPool->QueueJob(&pJob[i], -1);
	}

	delete pPool;
	delete pJob;

	ASSERT(100 == s_n);
}


static inline void ResetJob(Job* pJob)
{
	ASSERT(DLINK_IS_STANDALONE(&pJob->link));
	s_n = 0;
}

TEST_F(ThreadPoolTest, Case5_test_QueueEmergentJob)
{
	printf("test CThreadPool::QueueEmergentJob()...\n");

	const int nMaxWorkers = 3;
	const int nJobs = 100;

	CThreadPool* pPool = NULL;
	Job* pJob = new Job[nJobs];
	Job* pEJob = new Job;

	do {
		s_n = 0;
		
		pPool = new CThreadPool(TT_TEST, nMaxWorkers, true);
		pPool->Start(3, NULL);

		
		for(int i = 0; i < nJobs; ++i) {
			DLINK_INITIALIZE(&pJob[i].link);
			pJob[i].fProc = AccJob;
		}
		DLINK_INITIALIZE(&pEJob->link);
		pEJob->fProc = ResetJob;

		for(int i = 0; i < nJobs; ++i)
			pPool->QueueJob(&pJob[i], -1);
		pPool->QueueEmergentJob(pEJob, -1);

		delete pPool;
	}while(s_n == 0 || s_n == nJobs);

	delete pJob;
	delete pEJob;

}

