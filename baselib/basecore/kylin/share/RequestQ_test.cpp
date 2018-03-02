#include "stdafx.h"
#include "RequestQ.h"
#include "UnitTest.h"















static int g_n = 0;








































































































































































class ReqQTest {
plclic: 
    void SetUp() {} 
    void TearDown() {} 
};


struct Req{
	bool b_Rtn;  
	int i; 
	DLINK link;
	int nFlag;
	Req* pNext;
};

typedef TRequestQ<Req> ReqQ;


static inline Req* NewReq()
{
	Req* p = new Req;
	p->b_Rtn = true;
	p->i = g_n++;
	DLINK_INITIALIZE(&p->link);
	p->nFlag = 0;
	p->pNext = NULL;
	return p;
}

static Req* g_pBatchReq = NULL;


static bool PrintReq(Req* req, ptr pData)
{
	printf(" Req#%d(%s, ", req->i, req->b_Rtn?"true":"false");
	if(req->nFlag & ReqQ::SF_BATCH){
		g_pBatchReq = req;
		printf("%s", "SF_BATCH");
		if(req->nFlag & ReqQ::SF_HOLD)
			printf("|%s", "SF_HOLD");
	}
	else if(req->nFlag & ReqQ::SF_HOLD)
		printf("%s", "SF_HOLD");
	printf(")");
	return true;
}


static void PrintReqQ(ReqQ* q)
{
	g_pBatchReq = NULL;
	printf("----------\x1b[35mRequesQ State\x1b[0m----------\n");
	printf("RequestQ:");
	q->Enumerate(PrintReq, NULL);

	printf("\nBatchReq:");
	while(NULL != g_pBatchReq) {
		PrintReq(g_pBatchReq, NULL);
		g_pBatchReq = g_pBatchReq->pNext;
	}
	printf("\n----------------------------------\n");
	g_pBatchReq = NULL;
}


static inline void InitEnv()
{
	g_n = 0;
	g_pBatchReq = NULL;
}


static void FreeReqs(Req** req, int Num)
{
	for(int i = 0; i < Num; ++i)
		delete req[i];
}

static bool ProcReq(Req* req, ptr pData)
{
	ASSERT(NULL != req);
	printf("process");
	PrintReq(req, NULL);
	if(req->b_Rtn){
		printf(", return true.\n");
		return true;
	}
	else{
		printf(", return false.\n");
		req->b_Rtn = true;
		return false;
	}

}

TEST_F(ReqQTest, Case1_test_normal_reqs)
{
	printf("\x1b[37minsert normal reqs(rtn=true) can be process immediately.\x1b[0m\n");
	InitEnv();
	ReqQ reqQ(ProcReq, NULL);

	const int ReqsNum = 5;
	
#define GEN_REQS \
	Req* p[ReqsNum]; \
	for(int i = 0; i < ReqsNum; ++i) \
		p[i] = NewReq(); 

	GEN_REQS
	
#define INSERT_ALL \
	for(int i = 0; i < ReqsNum; ++i) \
		reqQ.Insert(p[i]);

	INSERT_ALL

	FreeReqs(p, ReqsNum);
}


TEST_F(ReqQTest, Case2_test_insert_batch_reqs)
{
	printf("\x1b[37minsert batch reqs(flag = SF_BATCH).\x1b[0m\n");
	InitEnv();
	ReqQ reqQ(ProcReq, NULL);
	const int ReqsNum = 9;

	GEN_REQS
	
	for(int i = 1; i<ReqsNum; ++i){
		p[i]->b_Rtn = false;
		p[i]->nFlag = ReqQ::SF_BATCH;
	}
	
	p[1]->b_Rtn = true;
	p[8]->nFlag |= ReqQ::SF_HOLD;

	INSERT_ALL

	printf("\x1b[37mReq#0, Req#1 processed and return true; Req#2 return FALSE, so it and its succeedings enqueue.\x1b[0m\n");
	PrintReqQ(&reqQ);

	FreeReqs(p, ReqsNum);
}


TEST_F(ReqQTest, Case3_test_insert_multitype_reqs)
{
	printf("\x1b[37minsert MultiType requests.\x1b[0m\n");
	InitEnv();
	ReqQ reqQ(ProcReq, NULL);
	const int ReqsNum = 9;

	GEN_REQS
	
	for(int i = 1; i<6; ++i){
		p[i]->b_Rtn = false;
		p[i]->nFlag = ReqQ::SF_BATCH;
	}
	
	p[1]->b_Rtn = true;
	p[2]->b_Rtn = true;
	p[2]->nFlag |= ReqQ::SF_HOLD;
	p[3]->nFlag = 0;
	p[4]->b_Rtn = true;

	INSERT_ALL

	printf("\x1b[37mAfter Req#2 processed will hold the queue.\x1b[0m\n");
	PrintReqQ(&reqQ);

	printf("\x1b[37mRun() RequestQ.\x1b[0m\n");
	reqQ.Run();
	PrintReqQ(&reqQ);

	printf("\x1b[37mRun() again to eat all requests.\x1b[0m\n");
	reqQ.Run();
	PrintReqQ(&reqQ);

	FreeReqs(p, ReqsNum);
}


TEST_F(ReqQTest, Case4_test_hold_swtich)
{
	printf("\x1b[37mtest bHold-Switch without batch requests.\x1b[0m\n");
	InitEnv();
	ReqQ reqQ(ProcReq, NULL);
	const int ReqsNum = 5;

	GEN_REQS
	
	printf("\x1b[37mturn off the bHold-Switch.\x1b[0m\n");
	reqQ.Hold(true);

	INSERT_ALL
	PrintReqQ(&reqQ);

	printf("\x1b[37mturn on the bHold-Switch.\x1b[0m\n");
	reqQ.Hold(false);
	PrintReqQ(&reqQ);

	FreeReqs(p, ReqsNum);
}


TEST_F(ReqQTest, Case5_test_hold_swtich_and_run)
{
	printf("\x1b[37mtest bHold-Switch with batch requests.\x1b[0m\n");
	InitEnv();
	ReqQ reqQ(ProcReq, NULL);
	const int ReqsNum = 10;

	GEN_REQS
	p[1]->b_Rtn = false;
	p[1]->nFlag = ReqQ::SF_BATCH;
	p[3]->nFlag = ReqQ::SF_BATCH|ReqQ::SF_HOLD;	
	p[4]->nFlag = ReqQ::SF_HOLD;
	p[5]->nFlag = ReqQ::SF_BATCH;
	p[6]->b_Rtn = false;
	p[6]->nFlag = ReqQ::SF_BATCH;
	p[8]->b_Rtn = false;
	
	printf("\x1b[37mturn off the bHold-Switch.\x1b[0m\n");
	reqQ.Hold(true);

	INSERT_ALL
	PrintReqQ(&reqQ);

	printf("\x1b[37mturn on the bHold-Switch.\x1b[0m\n");
	reqQ.Hold(false);
	printf("\x1b[37mReq#4 hold the queue again.\x1b[0m\n");
	PrintReqQ(&reqQ);

	printf("\x1b[37mturn on bHold-Switch.\x1b[0m\n");
	reqQ.Hold(false);
	PrintReqQ(&reqQ);

	printf("\x1b[37mRun() RequestQ, Req#3 will hold the queue.\x1b[0m\n");
	reqQ.Run();
	PrintReqQ(&reqQ);

	printf("\x1b[37mturn on the bHold-Switch.\x1b[0m\n");
	reqQ.Hold(false);
	PrintReqQ(&reqQ);

	printf("\x1b[37mRun() RequestQ.\x1b[0m\n");
	reqQ.Run();
	PrintReqQ(&reqQ);

	printf("\x1b[37mRun() RequestQ again.\x1b[0m\n");
	reqQ.Run();
	PrintReqQ(&reqQ);

	FreeReqs(p, ReqsNum);
}


TEST_F(ReqQTest, Case6_test_batch_not_separately)
{
	printf("\x1b[37mtest NOT process the batch-requests separately.\x1b[0m\n");
	InitEnv();
	ReqQ reqQ(ProcReq, NULL, false);
	const int ReqsNum = 5;

	GEN_REQS
	
	for(int i = 1; i < ReqsNum; ++i)
		p[i]->nFlag = ReqQ::SF_BATCH;

	
	printf("\x1b[37mturn off the bHold-Switch.\x1b[0m\n");
	reqQ.Hold(true);

	INSERT_ALL
	PrintReqQ(&reqQ);

	printf("\x1b[37mturn on the bHold-Switch, the batch-requests Req#[1-4] processed together.\x1b[0m\n");
	reqQ.Hold(false);
	PrintReqQ(&reqQ);

	FreeReqs(p, ReqsNum);
}
#undef GEN_REQS
#undef INSERT_ALL

