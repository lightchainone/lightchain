#include "stdafx.h"
#include "DolcleList.h"
#include "Random.h"
#include "thread.h"
#include "SyncedQ.h"
#include "UnitTest.h"

enum SyncedQAction {
	SQA_PUSH_BACK,
	SQA_PUSH_FRONT,
	SQA_POP_BACK,
	SQA_POP_FRONT,
};

enum TraceColor {
	TC_Red = 31,
	TC_Green,
	TC_Yellow,
	TC_Blue,
	TC_Magenta,
	TC_Cyan,
	TC_White,
};

static void ColorTrace(int color, const char* fmt, ...)
{
	static CMutex s_lock;
	va_list ap;

	s_lock.Lock();
	fprintf(stderr, "\x1b[%dm[%d]", color, thread_getid());

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);

	fprintf(stderr, "\x1b[0m");
	s_lock.Unlock();

	fflush(stderr);

}


struct SyncedQEle
{
	int data;
	DLINK link;		
};


typedef TLinkedList<SyncedQEle> SLList;

static TSyncedQueue<SyncedQEle, SLList> g_SyncedQ;

static int g_n = 1;

static void CleanSyncedQ()
{
	
	SyncedQEle* pTmp = NULL;

	int m = g_SyncedQ.size();

	for(int i = 0; i < m; ++i) {
		pTmp = g_SyncedQ.pop_front();
		delete pTmp;
	}

	ASSERT(0 == g_SyncedQ.size());
}


static void* PushProc(void* arg)
{
	SyncedQAction act = *(SyncedQAction*)arg; 
	delete (SyncedQAction*)arg;
	SyncedQEle* p = new SyncedQEle;
	p->data = g_n++;
	DLINK_INITIALIZE(&p->link);
	
	if(act == SQA_PUSH_BACK){
		ColorTrace(TC_Magenta, "want to push back  %d...\n",p->data);
		g_SyncedQ.push_back(p);
		ColorTrace(TC_Magenta, "push back  %-4d, totally [%-4d]\n",p->data, g_SyncedQ.size());

	}
	else{
		ASSERT(act == SQA_PUSH_FRONT);
		ColorTrace(TC_Magenta, "want to push front  %d...\n",p->data);
		g_SyncedQ.push_front(p);
		ColorTrace(TC_Magenta, "push front %-4d, totally [%-4d]\n",p->data, g_SyncedQ.size());

	}

	return NULL;
}


static void* PopProc(void* arg)
{
	SyncedQAction act = *(SyncedQAction*)arg; 
	delete (SyncedQAction*)arg;
	SyncedQEle* p  = NULL;
	
	if(act == SQA_POP_BACK){
		ColorTrace(TC_White, "pop_back() called...\n");
		p = g_SyncedQ.pop_back();
		ColorTrace(TC_White, "pop back   %-4d, totally [%-4d]\n",p->data, g_SyncedQ.size());

	}
	else{
		ASSERT(act == SQA_POP_FRONT);
		ColorTrace(TC_White, "pop_front() called...\n");
		p = g_SyncedQ.pop_front();
		ColorTrace(TC_White, "pop front  %-4d, totally [%-4d]\n",p->data, g_SyncedQ.size());

	}

	delete p;

	return NULL;
}


class SyncedQTest {
plclic: 
    void SetUp() {} 
    void TearDown() {} 
protected:

};

#define PUSH_BACK(tid) \
	p_Act = new SyncedQAction;\
    *p_Act = SQA_PUSH_BACK;\
	thread_create(&tid, NULL, PushProc, (void*)p_Act); 

#define PUSH_FRONT(tid) \
	p_Act = new SyncedQAction;\
	*p_Act = SQA_PUSH_FRONT;\
	thread_create(&tid, NULL, PushProc, (void*)p_Act); 

#define POP_BACK(tid) \
	p_Act = new SyncedQAction;\
	*p_Act = SQA_POP_BACK; \
	thread_create(&tid, NULL, PopProc, (void*)p_Act); 

#define POP_FRONT(tid) \
	p_Act = new SyncedQAction;\
	*p_Act = SQA_POP_FRONT; \
	thread_create(&tid, NULL, PopProc, (void*)p_Act); 

#define WAIT_THREAD(tid) \
	thread_join(tid, NULL);

TEST_F(SyncedQTest, Case1)
{
	g_SyncedQ.Init();
	thread_t tid1, tid2;
	SyncedQAction* p_Act; 

	TRACE0("push first, then pop.\n");
	
	PUSH_BACK(tid1)
	POP_FRONT(tid2)
	WAIT_THREAD(tid1)
	WAIT_THREAD(tid2)

	
	printf("\n");
	TRACE0("pop first, wait till push.\n");
	POP_BACK(tid2)
	PUSH_FRONT(tid1)
	WAIT_THREAD(tid1)
	WAIT_THREAD(tid2)

	CleanSyncedQ();

}

#if 0
struct SyncedQLog {
	SyncedQAction act; 
	int nChgd; 
	int nQsize; 
};




static inline void SetSynObj(SyncedQAction act, int nChgd)
{
	g_RWLock.WriteLock();

	g_synobj.act = act;
	g_synobj.nChgd = nChgd;

	g_RWLock.WriteUnlock();
}

static inline void GetSynObj(Test_SQSyncedOjb* s)
{
	g_RWLock.ReadLock();

	s->act = g_synobj.act;
	s->nChgd = g_synobj.nChgd;

	g_RWLock.ReadUnlock();

}

#endif

static const int g_ExecCounter = 200;
static CRandom g_Random;
static int g_ratio_PushPop = 3; 


static void* RandomPushProc(void* arg)
{
	SyncedQEle* p = NULL;
	
	for(int i = 0; i < g_ExecCounter * g_ratio_PushPop; ++i){
		
		p = new SyncedQEle;
		p->data = g_n++;
		DLINK_INITIALIZE(&p->link);
		
		if((int(g_Random.Next()*10))%2) {

			g_SyncedQ.push_back(p);
			ColorTrace(TC_Magenta, "push back  %-4d, totally [%-4d]\n",p->data, g_SyncedQ.size());
		}
		else {

			g_SyncedQ.push_front(p);
			ColorTrace(TC_Magenta, "push front %-4d, totally [%-4d]\n",p->data, g_SyncedQ.size());
		}
		Sleep(1); 
	}
	return NULL;
}



static void* RandomPopProc(void* arg)
{
	SyncedQEle* p = NULL;
	
	for(int i = 0; i < g_ExecCounter; ++i){
		
		if((int(g_Random.Next()*10))%2) {

			p = g_SyncedQ.pop_back();
			ColorTrace(TC_White, "pop back   %-4d, totally [%-4d]\n",p->data, g_SyncedQ.size());
		}
		else {

			p = g_SyncedQ.pop_back();
			ColorTrace(TC_White, "pop front  %-4d, totally [%-4d]\n",p->data, g_SyncedQ.size());
		}

		delete p;

		Sleep(2); 
	}
	return NULL;
}


TEST_F(SyncedQTest, Case2_random)
{
	g_SyncedQ.Init();
	g_n = 0;
	thread_t tid1, tid2;

	if(0 != thread_create(&tid1, NULL, RandomPushProc, NULL)){
		PERROR("thread create error.\n");
	}
	if(0 != thread_create(&tid2, NULL, RandomPopProc, NULL)){
		PERROR("thread create error.\n");
	}

	WAIT_THREAD(tid1)
	WAIT_THREAD(tid2)

	
	
	ASSERT(g_ExecCounter*(g_ratio_PushPop-1) == g_SyncedQ.size());

	CleanSyncedQ();

}

#undef PUSH_BACK
#undef PUSH_FRONT
#undef POP_BACK
#undef POP_FRONT
#undef WAIT_THREAD
