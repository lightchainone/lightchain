

#include "stdafx.h"
#include "Random.h"
#include "DolcleList.h"
#include "thread.h"
#include "SyncedQ.h"
#include "UnitTest.h"

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

class LockedQTest {
plclic: 
    void SetUp() {} 
    void TearDown() {} 
};


struct LkdQEle
{
	int data;
	DLINK link;		
};


typedef TLinkedList<LkdQEle> LinkedList;
static TLockedQueue<LkdQEle, LinkedList> g_LkdQ;

static const int g_ExecCounter = 100;

static CRandom g_Random; 

static int g_n = 1;

static int g_EnumCounter = 0;


bool Enum1(LkdQEle* pNode, ptr pData)
{
	ASSERT(NULL != pNode);
	if(0 == pNode->data%2){
		g_LkdQ.remove(pNode);
		delete pNode;
		g_EnumCounter++;
		return true;
	}
	else
		return true;
}


static void* PushProc(void* arg)
{
	LkdQEle *p = NULL;
	int m = 0;

	for(int i = 0; i < g_ExecCounter; ++i) {
		p = new LkdQEle;
		p->data = g_n++;
		DLINK_INITIALIZE(&p->link);

		if((int(g_Random.Next()*10))%2) {
			m = g_LkdQ.push_back(p);
			ColorTrace(TC_White, "push back  %-4d, totally [%-4d]\n", p->data, m);
		}
		else {
			g_LkdQ.push_front(p);
			ColorTrace(TC_White, "push front %-4d, totally [%-4d]\n", p->data, g_LkdQ.size());
		}

		Sleep(1); 
	}
	return NULL;
}

static void* PopProc(void* arg)
{
	LkdQEle *p = NULL;

	for(int i = 0; i < g_ExecCounter/2; ++i) {

		if((int(g_Random.Next()*10))%2) {
			p = g_LkdQ.pop_front();
			if(p)
				ColorTrace(TC_Yellow, "pop front  %-4d, totally [%-4d]\n", p->data, g_LkdQ.size());
		}
		else {
			p = g_LkdQ.pop_back();
			if(p)
				ColorTrace(TC_Yellow, "pop back   %-4d, totally [%-4d]\n", p->data, g_LkdQ.size());
		}

		if(p) delete p;

		Sleep(2);

	}
	return NULL;
}


static void* GetProc(void* arg)
{
	LkdQEle *p = NULL;

	for(int i = 0; i < g_ExecCounter; ++i) {

		if((int(g_Random.Next()*10))%2) {
			p = g_LkdQ.get_front();
			if(p)
				ColorTrace(TC_Magenta, "get front  %-4d, totally [%-4d]\n", p->data, g_LkdQ.size());
		}
		else {
			p = g_LkdQ.get_back();
			if(p)
				ColorTrace(TC_Magenta, "get back   %-4d, totally [%-4d]\n", p->data, g_LkdQ.size());
		}

		Sleep(1);
	}
	return NULL;
}

static void* EnumProc(void* arg)
{
	for(int i = 0; i < g_ExecCounter/2; ++i) {

		if((int(g_Random.Next()*10))%2) {
			g_EnumCounter = 0;
			g_LkdQ.Enumerate(Enum1, NULL);
			ColorTrace(TC_Cyan, "Enum del   %-4d, totally [%-4d]\n", g_EnumCounter, g_LkdQ.size());
		}

		Sleep(3);
	}
	return NULL;
}



TEST_F(LockedQTest, Case1)
{
	printf("LockedQ test start...\n");
	g_LkdQ.Init();

	thread_t tid[4];
	
	
	thread_create(&tid[0], NULL, PushProc, NULL);
	thread_create(&tid[1], NULL, PopProc, NULL);
	thread_create(&tid[2], NULL, GetProc, NULL);
	thread_create(&tid[3], NULL, EnumProc, NULL);
	thread_join(tid[0], NULL);
	thread_join(tid[1], NULL);
	thread_join(tid[2], NULL);
	thread_join(tid[3], NULL);
	
	LkdQEle* pTmp = NULL;
	while(NULL != (pTmp = g_LkdQ.pop_front()))
		delete pTmp;
	ASSERT(0 == g_LkdQ.size());
}

