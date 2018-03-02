#include "stdafx.h"
#include "SyncObj.h"
#include "BufHandle.h"
#include "UnitTest.h"
#include "../test/KylinUtils4Test.h"

static const int s_nTranBuf = 20; 
static const int s_nBigTranBuf = 20; 
static const int s_nBufSize = 1024; 
static const int s_nBufHdl = 1024;  

static const dolcle s_fTranRatio1 = 0.6;
static const dolcle s_fTranRatio2 = 0.9;
static const dolcle s_fBigRatio1 = 0.9;
static const dolcle s_fBigRatio2 = 0.9;

class BufHdlTest {
plclic:
	void SetUp() {
		SetTranBuf(s_nTranBuf, s_nBigTranBuf, s_nBufSize); 
	}
	void TearDown() {}
};



TEST_F(BufHdlTest, Case1_test_AllocateHdl)
{
	ColorPrintf(TC_Red, "test AllocateHdl(true, NULL, 0, NULL)...\n");
	
	
	BufHandle* pHdl[s_nTranBuf+1];
	BufHandle* pNext = NULL;
	bool bInPool = true;
	char* pBuf = NULL;
	int nLen = 0;

	
	for(int i = 0; i < s_nTranBuf; ++i)
		pHdl[i] = NULL;

	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl));
	
	
	PrintPoolInfo("before AllocateHdl():\n");

	
	pHdl[0] = AllocateHdl(bInPool, pBuf, nLen, pNext);
	
	ASSERT(NULL != pHdl[0]);
	ASSERT(NULL != pHdl[0]->pBuf);
	ASSERT(s_nBufSize == pHdl[0]->nBufLen);
	ASSERT(0 == pHdl[0]->nDataLen);
	ASSERT(NULL == pHdl[0]->_next);

	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-1, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl-1));

	PrintPoolInfo("after AllocateHdl(true, NULL, 0, NULL):\n");

	
	for(int i = 1; i < s_nTranBuf; ++i) {
		pHdl[i] = AllocateHdl(bInPool, pBuf, nLen, pNext);
		ASSERT(NULL != pHdl[i]);
		ASSERT(NULL != pHdl[i]->pBuf);
		ASSERT(s_nBufSize == pHdl[i]->nBufLen);
		ASSERT(0 == pHdl[i]->nDataLen);
		ASSERT(NULL == pHdl[i]->_next);
	}

	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, 0, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl-s_nTranBuf));

	PrintPoolInfo("exhaust BufPool using AllocateHdl(true, NULL, 0, NULL):\n");

	
	
	

	
	for(int i = 0; i < s_nTranBuf; ++i) {
		FreeHdl(pHdl[i]);
	}

	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl));

	PrintPoolInfo("after free all the allocated buffers:\n");
}





static void* AllocProc(void* arg)
{
	ColorTrace(TC_Red, "try to allocate 5 buffers from the exhausted BufPool.\n");
	BufHandle** pHdl = (BufHandle**)arg;
	int i = 0;

	while(i != 5) {
		pHdl[i] = AllocateHdl(true, NULL, 0, NULL);
		ASSERT(NULL != pHdl[i]);
		ASSERT(NULL != pHdl[i]->pBuf);
		ASSERT(s_nBufSize == pHdl[i]->nBufLen);
		ASSERT(0 == pHdl[i]->nDataLen);
		ASSERT(NULL == pHdl[i]->_next);
		ColorTrace(TC_Red, "Buffer%d(%p) allocated.\n", i, pHdl[i]->pBuf);
		++i;
	}

	return NULL;
}


static void* FreeProc(void* arg)
{
	sleep(1); 
	BufHandle** pHdl = (BufHandle**)arg;
	char* pTmp = NULL;

	for(int i = 0; i < 5; ++i){
		pTmp = pHdl[i]->pBuf;	
		FreeHdl(pHdl[i]);
		ColorTrace(TC_Blue, "Buffer(%p) freed.\n", pTmp);
		sleep(1); 
	}

	return NULL;
}


TEST_F(BufHdlTest, Case2_test_AllocateHdl)
{
	ColorPrintf(TC_Red, "Two threads: one thread attempt to allocate from Empty BufPool,\nthe other free some buffers...\n");

	
	BufHandle* pHdl[s_nTranBuf];
	BufHandle* pNext = NULL;
	bool bInPool = true;
	char* pBuf = NULL;
	int nLen = 0;
	
	
	for(int i = 0; i < s_nTranBuf; ++i) {
		pHdl[i] = AllocateHdl(bInPool, pBuf, nLen, pNext);
		ASSERT(NULL != pHdl[i]);
		ASSERT(NULL != pHdl[i]->pBuf);
		ASSERT(s_nBufSize == pHdl[i]->nBufLen);
		ASSERT(0 == pHdl[i]->nDataLen);
		ASSERT(NULL == pHdl[i]->_next);
	}
	
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, 0, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl-s_nTranBuf));

	PrintPoolInfo("exhaust BufPool using AllocateHdl(true,):\n");

	
	BufHandle* pHdl_2[5];
	thread_t tid1, tid2;	
	thread_create(&tid1, NULL, AllocProc, (void*)pHdl_2);
	thread_create(&tid2, NULL, FreeProc, (void*)pHdl);
	thread_join(tid1, NULL);
	thread_join(tid2, NULL);

	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, 0, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl-s_nTranBuf));

	
	for(int i = 5; i < s_nTranBuf; ++i) {
		FreeHdl(pHdl[i]);
	}
	for(int i = 0; i < 5; ++i)
		FreeHdl(pHdl_2[i]);

	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl));

	PrintPoolInfo("after free all the allocated buffers:\n");
}


TEST_F(BufHdlTest, Case3_test_AllocateHdl)
{
	ColorPrintf(TC_Red, "test AllocateHdl(false, pBuf(!=NULL), >0,)...\n");

	
	BufHandle *pHdl_1, *pHdl_2;;
	BufHandle* pNext = NULL;
	bool bInPool = false;
	char pBuf_1[] = "user content";
	int nLen_1 = sizeof(pBuf_1);
	int nLen_2 = s_nBufSize + 1;
	char pBuf_2[nLen_2]; 

	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl));

	PrintPoolInfo("before AllocateHdl():\n");

	
	pHdl_1 = AllocateHdl(bInPool, pBuf_1, nLen_1, pNext);
	ASSERT(NULL != pHdl_1);
	ASSERT(NULL != pHdl_1->pBuf);
	ASSERT(nLen_1 == pHdl_1->nBufLen);
	ASSERT(nLen_1 == pHdl_1->nDataLen);
	ASSERT(NULL == pHdl_1->_next);
	
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl-1));

	PrintPoolInfo("after AllocateHdl(false, pBuf_1,):\n");

	
	pHdl_2 = AllocateHdl(bInPool, pBuf_2, nLen_2, pHdl_1);
	ASSERT(NULL != pHdl_2);
	ASSERT(NULL != pHdl_2->pBuf);
	ASSERT(nLen_2 == pHdl_2->nBufLen);
	ASSERT(nLen_2 == pHdl_2->nDataLen);
	ASSERT(pHdl_1 == pHdl_2->_next);
	
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl-2));
	
	PrintPoolInfo("after AllocateHdl(false, pBuf_2(5120B),):\n");

	
	ChainFreeHdl(pHdl_2, NULL);

	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl));

	PrintPoolInfo("after free the 2 allocated BufHandles:\n");
}



TEST_F(BufHdlTest, Case4_test_AllocateHdl)
{
	ColorPrintf(TC_Red, "test AllocateHdl(true, pBuf(==NULL), >0,), used to preoccupy buffer...\n");

	
	BufHandle *pHdl = NULL;
	BufHandle* pNext = NULL;
	bool bInPool = true;
	char* pBuf = NULL;
	int nLen = 4;;

	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl));

	PrintPoolInfo("before AllocateHdl():\n");

	pHdl = AllocateHdl(bInPool, pBuf, nLen, pNext);
	ASSERT(NULL != pHdl);
	ASSERT(NULL != pHdl->pBuf);
	ASSERT(s_nBufSize == pHdl->nBufLen);
	ASSERT(nLen == pHdl->nDataLen);
	ASSERT(NULL == pHdl->_next);

	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-1, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl-1));

	PrintPoolInfo("after AllocateHdl(true, pBuf(==NULL), >0,):\n");
	
	
	FreeHdl(pHdl);

	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl));

	PrintPoolInfo("after free the allcoated BufHandles:\n");
}



TEST_F(BufHdlTest, Case5_test_AllocateHdl)
{
	ColorPrintf(TC_Red, "test AllocateHdl(true, pBuf(!=NULL), >0,), used to manage user memory with BufHandle...\n");

	
	BufHandle *pHdl_1, *pHdl_2;
	BufHandle* pNext = NULL;
	bool bInPool = true;
	char pBuf_1[] = "user content";
	int nLen_1 = sizeof(pBuf_1);
	int nLen_2 = s_nBufSize*2 + 1; 
	char pBuf_2[nLen_2];

	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl));

	PrintPoolInfo("before AllocateHdl():\n");

	
	pHdl_1 = AllocateHdl(bInPool, pBuf_1, nLen_1, pNext);
	ASSERT(NULL != pHdl_1);
	ASSERT(NULL != pHdl_1->pBuf);
	ASSERT(s_nBufSize == pHdl_1->nBufLen);
	ASSERT(nLen_1 == pHdl_1->nDataLen);
	ASSERT(NULL == pHdl_1->_next);

	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-1, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl-1));

	PrintPoolInfo("after AllocateHdl(true, pBuf_1(!=NULL), 13,):\n");
	
	
	pHdl_2 = AllocateHdl(bInPool, pBuf_2, nLen_2, pHdl_1);
	ASSERT(NULL != pHdl_2);
	ASSERT(NULL != pHdl_2->pBuf);
	ASSERT(s_nBufSize == pHdl_2->nBufLen);
	ASSERT(pHdl_1 == pHdl_2->_next->_next->_next);

	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-4, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl-4));

	PrintPoolInfo("after AllocateHdl(true, pBuf_2(!=NULL), 8193,):\n");

	
	ChainFreeHdl(pHdl_2, NULL);
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl));
	PrintPoolInfo("after free the allcoated BufHandles:\n");
}



TEST_F(BufHdlTest, Case6_test_AllocateHdlCanFail)
{
	ColorPrintf(TC_Red, "test AllocateHdlCanFail(), which can only allocate within BufPool[0, %d%%]...\n", (int)(100*s_fTranRatio1));

	
	BufHandle *pHdl[s_nTranBuf];
	int watermark = (int)((dolcle)s_nTranBuf*s_fTranRatio1);

	PrintPoolInfo("before AllocateHdl():\n");

	
	for(int i = 0; i <= watermark; ++i) {
		pHdl[i] = AllocateHdlCanFail();
		ASSERT(NULL != pHdl[i]);
		ASSERT(NULL != pHdl[i]->pBuf);
		ASSERT(s_nBufSize == pHdl[i]->nBufLen);
		ASSERT(0 == pHdl[i]->nDataLen);
		ASSERT(NULL == pHdl[i]->_next);
	}

	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-watermark-1, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl-watermark-1));

	PrintPoolInfo("request all the buffers below watermark(%d%%) using AllocateHdlCanFail():\n", (int)(100*s_fTranRatio1));
	
	
	pHdl[watermark+1] = AllocateHdlCanFail();
	ASSERT_EQUAL((BufHandle*)NULL, pHdl[watermark+1]);

	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-watermark-1, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl-watermark-1));
	PrintPoolInfo("try to request more buffer failed:\n");

	
	for(int i = 0; i <= watermark; ++i) 
		FreeHdl(pHdl[i]);
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl));
	PrintPoolInfo("after free the allcoated BufHandles:\n");
}



TEST_F(BufHdlTest, Case7_test_AllocateBigHdl)
{
	ColorPrintf(TC_Red, "test AllocateBigHdl()...\n");

	
	BufHandle *pHdl[s_nBigTranBuf];

	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl));
	PrintPoolInfo("before AllocateBigHdl():\n");

	
	pHdl[0] = AllocateBigHdl();
	ASSERT(NULL != pHdl[0]);

	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf, 
				                     s_nBigTranBuf, s_nBigTranBuf, s_nBigTranBuf-1,
									 s_nBufHdl-1));
	PrintPoolInfo("request 1 BigBuffer:\n");

	ColorPrintf(TC_Red, "try to exhaust BigBufPool...\n");

	
	for(int i = 1; i < s_nBigTranBuf; ++i) {
		pHdl[i] = AllocateBigHdl();
		ASSERT(NULL != pHdl[i]);
		ASSERT(NULL != pHdl[i]->pBuf);
		ASSERT(SZ_BIG_BUF == pHdl[i]->nBufLen);
		ASSERT(0 == pHdl[i]->nDataLen);
		ASSERT(NULL == pHdl[i]->_next);
	}

	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf, 
				                     s_nBigTranBuf, s_nBigTranBuf, 0,
									 s_nBufHdl-s_nBigTranBuf));
	PrintPoolInfo("after request all the BigBuffers:\n");

	
	for(int i = 0; i < s_nBigTranBuf; ++i) 
		FreeHdl(pHdl[i]);
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf, 
				                     s_nBigTranBuf, s_nBigTranBuf, s_nBigTranBuf,
									 s_nBufHdl));
	PrintPoolInfo("after free all allocated buffers:\n");
}



TEST_F(BufHdlTest, Case8_test_AllocateBigHdl)
{
	printf("test AllocateBigHdlCanFail(), only can request %d%% BigBuffers...\n", (int)(100*s_fBigRatio1));

	
	BufHandle *pHdl[s_nBigTranBuf];
	int watermark = (int)((dolcle)s_nBigTranBuf*s_fBigRatio1);

	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf, 
				                     s_nBigTranBuf, s_nBigTranBuf, s_nBigTranBuf,
									 s_nBufHdl));
	PrintPoolInfo("before AllocateBigHdlCanFail():\n");

	
	for(int i = 0; i <= watermark; ++i) {
		pHdl[i] = AllocateBigHdlCanFail();
		ASSERT(NULL != pHdl[i]);
		ASSERT(NULL != pHdl[i]->pBuf);
		ASSERT(SZ_BIG_BUF == pHdl[i]->nBufLen);
		ASSERT(0 == pHdl[i]->nDataLen);
		ASSERT(NULL == pHdl[i]->_next);
	}

	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf, 
				                     s_nBigTranBuf, s_nBigTranBuf, s_nBigTranBuf-watermark-1,
									 s_nBufHdl-watermark-1));
	PrintPoolInfo("request all the BigBuffers below watermark(%d%%):\n", (int)(100*s_fBigRatio1));

	
	pHdl[watermark+1] = AllocateBigHdlCanFail();
	ASSERT(NULL == pHdl[watermark+1]);
	PrintPoolInfo("try to request more BigBuffers above watermark but failed:\n");

	
	for(int i = 0; i <= watermark; ++i) 
		FreeHdl(pHdl[i]);
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf, 
				                     s_nBigTranBuf, s_nBigTranBuf, s_nBigTranBuf,
									 s_nBufHdl));
	PrintPoolInfo("after free all allocated buffers:\n");
}




TEST_F(BufHdlTest, Case9_test_AllocateHdl)
{
	ColorPrintf(TC_Red, "test AllocateHdl(false, pBuf(in pool), nLen,)...\n");

	
	BufHandle *pHdl[3];
	BufHandle *pBigHdl[2];

	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf, 
				                     s_nBigTranBuf, s_nBigTranBuf, s_nBigTranBuf,
									 s_nBufHdl));
	PrintPoolInfo("before AllocateHdl():\n");

	
	pHdl[0] = AllocateHdl(true, NULL, 0, NULL);	
	ASSERT(NULL != pHdl[0]);

	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-1, 
				                     s_nBigTranBuf, s_nBigTranBuf, s_nBigTranBuf,
									 s_nBufHdl-1));
	PrintPoolInfo("allocate 1 TranBuf:\n");

	
	pHdl[1] = AllocateHdl(false, pHdl[0]->pBuf, pHdl[0]->nBufLen, pHdl[0]);	
	ASSERT(NULL != pHdl[1]);
	ASSERT(pHdl[0]->pBuf == pHdl[1]->pBuf);
	ASSERT(pHdl[1]->_next == pHdl[0]);

	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-1, 
				                     s_nBigTranBuf, s_nBigTranBuf, s_nBigTranBuf,
									 s_nBufHdl-2));
	PrintPoolInfo("allocate 1 more BufHandle(pHdl[1]) for pHdl[0]->pBuf:\n");

	
	pHdl[2] = AllocateHdl(false, pHdl[0]->pBuf, pHdl[0]->nBufLen, pHdl[1]);	
	ASSERT(NULL != pHdl[2]);
	ASSERT(pHdl[1]->pBuf == pHdl[2]->pBuf);
	ASSERT(pHdl[2]->_next == pHdl[1]);
	ASSERT(pHdl[1]->_next == pHdl[0]);

	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-1, 
				                     s_nBigTranBuf, s_nBigTranBuf, s_nBigTranBuf,
									 s_nBufHdl-3));
	PrintPoolInfo("allocate 1 more BufHandle(pHdl[2]) for pHdl[0]->pBuf:\n");

	for(int i = 0; i < 3; ++i) {
		FreeHdl(pHdl[i]);
		PrintPoolInfo("after free pHdl[%d]:\n", i);
	}

	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf, 
				                     s_nBigTranBuf, s_nBigTranBuf, s_nBigTranBuf,
									 s_nBufHdl));

	
	pBigHdl[0] = AllocateBigHdl();	
	ASSERT(NULL != pBigHdl[0]);

	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf, 
				                     s_nBigTranBuf, s_nBigTranBuf, s_nBigTranBuf-1,
									 s_nBufHdl-1));
	PrintPoolInfo("allocate 1 BigBuf:\n");
	
	
	pBigHdl[1] = AllocateHdl(false, pBigHdl[0]->pBuf, pBigHdl[0]->nBufLen, pBigHdl[0]);
	ASSERT(NULL != pBigHdl[1]);
	ASSERT(pBigHdl[1]->pBuf == pBigHdl[0]->pBuf);
	ASSERT(pBigHdl[1]->_next == pBigHdl[0]);

	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf, 
				                     s_nBigTranBuf, s_nBigTranBuf, s_nBigTranBuf-1,
									 s_nBufHdl-2));
	PrintPoolInfo("allocate 1 more BufHandle(pBigHdl[1]) for pBigHdl[0]->pBuf:\n");

	for(int i = 0; i < 2; ++i) {
		FreeHdl(pBigHdl[i]);
		PrintPoolInfo("after free pBigHdl[%d]:\n", i);
	}

	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf, 
				                     s_nBigTranBuf, s_nBigTranBuf, s_nBigTranBuf,
									 s_nBufHdl));
}



TEST_F(BufHdlTest, Case10_test_FreeHdl)
{


	
	BufHandle *pHdl = NULL;

	
	ColorPrintf(TC_Red, "test FreeHdl(NULL)...\n");
	SHOULD_ABORT(FreeHdl(NULL));	
	
	
	pHdl = AllocateHdl(true, NULL, 0);
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-1, 
				                     s_nBigTranBuf, s_nBigTranBuf, s_nBigTranBuf,
									 s_nBufHdl-1));
	PrintPoolInfo("before FreeHdl(valid-pHdl), pHdl->pBuf is inPool:\n");

	FreeHdl(pHdl);
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf, 
				                     s_nBigTranBuf, s_nBigTranBuf, s_nBigTranBuf,
									 s_nBufHdl));
	PrintPoolInfo("after FreeHdl(valid-pHdl), pHdl->pBuf is inPool:\n");

	
	char pBuf[] = "user content";
	int nLen = sizeof(pBuf);
	pHdl = AllocateHdl(false, pBuf, nLen);
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf, 
				                     s_nBigTranBuf, s_nBigTranBuf, s_nBigTranBuf,
									 s_nBufHdl-1));
	PrintPoolInfo("before FreeHdl(valid-pHdl), pHdl->pBuf is NOT inPool:\n");

	FreeHdl(pHdl);
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf, 
				                     s_nBigTranBuf, s_nBigTranBuf, s_nBigTranBuf,
									 s_nBufHdl));
	PrintPoolInfo("after FreeHdl(valid-pHdl), pHdl->pBuf is NOT inPool:\n");

	
	ColorPrintf(TC_Red, "\ntest dolcle-free the same BufHdl...\n");
	pHdl = AllocateHdl(true, NULL, 0);
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-1, 
				                     s_nBigTranBuf, s_nBigTranBuf, s_nBigTranBuf,
									 s_nBufHdl-1));
	PrintPoolInfo("AllocateHdl() 1 BufHandle pHdl:\n");

	FreeHdl(pHdl);
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf, 
				                     s_nBigTranBuf, s_nBigTranBuf, s_nBigTranBuf,
									 s_nBufHdl));
	PrintPoolInfo("FreeHdl() pHdl 1 time:\n");

	SHOULD_ABORT(FreeHdl(pHdl));

	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf, 
				                     s_nBigTranBuf, s_nBigTranBuf, s_nBigTranBuf,
									 s_nBufHdl));
	PrintPoolInfo("FreeHdl() pHdl again:\n");
}



TEST_F(BufHdlTest, Case11_test_ChainFreeHdl)
{
	
	BufHandle *pHdl[10];
	BufHandle *pNext = NULL;


#define CREATE_CHAIN_HDL                \
	pNext = NULL;                       \
	for(int i = 9; i >= 0; --i) {       \
		if(i == 5) {                    \
			pHdl[5] = AllocateBigHdl(); \
			pHdl[5]->_next = pHdl[6];   \
		}                               \
		else                            \
			pHdl[i] = AllocateHdl(true, NULL, 0, pNext); \
                                        \
		pNext = pHdl[i];                \
	}

	CREATE_CHAIN_HDL
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-9, 
				                     s_nBigTranBuf, s_nBigTranBuf, s_nBigTranBuf-1,
									 s_nBufHdl-10));
	PrintPoolInfo("before ChainFreeHdl(pHdl[0], NULL):\n");

	ChainFreeHdl(pHdl[0], NULL);
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf, 
				                     s_nBigTranBuf, s_nBigTranBuf, s_nBigTranBuf,
									 s_nBufHdl));
	PrintPoolInfo("after ChainFreeHdl(pHdl[0], NULL):\n:");
	
	
	CREATE_CHAIN_HDL
	PrintPoolInfo("before ChainFreeHdl(pHdl[0], pHdl[5]):\n");
	ChainFreeHdl(pHdl[0], pHdl[5]);
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-4, 
				                     s_nBigTranBuf, s_nBigTranBuf, s_nBigTranBuf-1,
									 s_nBufHdl-5));
	PrintPoolInfo("after ChainFreeHdl(pHdl[0], pHdl[5]):\n");
	ChainFreeHdl(pHdl[5], NULL);
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf, 
				                     s_nBigTranBuf, s_nBigTranBuf, s_nBigTranBuf,
									 s_nBufHdl));
	PrintPoolInfo("after ChainFreeHdl(pHdl[5], NULL):\n");

	
	ColorPrintf(TC_Red, "test dolcle-ChainFreeHdl()...\n");
	CREATE_CHAIN_HDL
	ChainFreeHdl(pHdl[0], NULL); 
	SHOULD_ABORT(ChainFreeHdl(pHdl[1], pHdl[4])); 

	
	ColorPrintf(TC_Red, "test ChainFreeHdl(NULL,)...\n");
	ChainFreeHdl(NULL, NULL);	

	CREATE_CHAIN_HDL
	SHOULD_ABORT(ChainFreeHdl(NULL, pHdl[0]));
	ChainFreeHdl(pHdl[0], NULL); 

	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf, 
				                     s_nBigTranBuf, s_nBigTranBuf, s_nBigTranBuf,
									 s_nBufHdl));

#undef CREATE_CHAIN_HDL
}



TEST_F(BufHdlTest, Case12_test_CloneHdl)
{
	ColorPrintf(TC_Red, "test CloneHdlAndTerminate()...\n");

	
	BufHandle *pHdl[4], *pCloneHdl, *pCloneHdl_2;
	int nDataLen_2, nDataLen_3, nCloneLen;
	char pBuf_1[] = "user content 1";
	nDataLen_2 = 256;
	nDataLen_3 = s_nBufSize * 2 + 1; 
	char pBuf_3[nDataLen_3];

#define CREATE_CHAIN_HDL_2 \
	pHdl[0] = AllocateHdl(true, NULL, 0); \
	ASSERT(NULL != pHdl[0]); \
	pHdl[1] = AllocateHdl(false, pBuf_1, sizeof(pBuf_1), NULL); \
	ASSERT(NULL != pHdl[1]); \
	pHdl[2] = AllocateBigHdl(); \
	ASSERT(NULL != pHdl[2]); \
	memset(pHdl[2]->pBuf, '2', nDataLen_2); \
	pHdl[2]->nDataLen = nDataLen_2; \
	pHdl[3] = AllocateHdl(true, pBuf_3, nDataLen_3); \
	for(int i = 0; i < 3; ++i) \
		pHdl[i]->_next = pHdl[i+1];

	
	CREATE_CHAIN_HDL_2
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-4, 
				                     s_nBigTranBuf, s_nBigTranBuf, s_nBigTranBuf-1,
									 s_nBufHdl-6));
	PrintPoolInfo("before CloneHdlAndTerminate(pHdl[0], NULL,):\n");

	
	pCloneHdl = CloneHdlAndTerminate(pHdl[0], NULL, &nCloneLen);
    PrintPoolInfo("after CloneHdlAndTerminate(pHdl[0], NULL,):\n");
	
	ASSERT(NULL != pCloneHdl);
	ASSERT_EQUAL(nCloneLen, 0 + (int)sizeof(pBuf_1) + nDataLen_2 + nDataLen_3);
	ASSERT_EQUAL(true, CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-5, 
				                     s_nBigTranBuf, s_nBigTranBuf, s_nBigTranBuf-1,
									 s_nBufHdl-12));

	
	pCloneHdl_2 = CloneHdlAndTerminate(pHdl[0], pHdl[3], &nCloneLen);
	PrintPoolInfo("after CloneHdlAndTerminate(pHdl[0], pHdl[3],):\n");
    
	ASSERT(NULL != pCloneHdl_2);
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-6, 
				                     s_nBigTranBuf, s_nBigTranBuf, s_nBigTranBuf-1,
									 s_nBufHdl-15));
	ASSERT(nCloneLen == 0 + (int)sizeof(pBuf_1) + nDataLen_2);
	
	
	ChainFreeHdl(pCloneHdl_2, NULL);
	PrintPoolInfo("after ChainFreeHdl(pCloneHdl_2, pHdl[3]):\n");
    ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-5, 
				                     s_nBigTranBuf, s_nBigTranBuf, s_nBigTranBuf-1,
									 s_nBufHdl-12));

	
	ChainFreeHdl(pHdl[0], NULL);
	PrintPoolInfo("after ChainFreeHdl(pHdl[0], NULL):\n");
    ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-5, 
				                     s_nBigTranBuf, s_nBigTranBuf, s_nBigTranBuf-1,
									 s_nBufHdl-6));

	
	ChainFreeHdl(pCloneHdl, NULL);
	PrintPoolInfo("after ChainFreeHdl(pCloneHdl, NULL):\n");
    ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf, 
				                     s_nBigTranBuf, s_nBigTranBuf, s_nBigTranBuf,
									 s_nBufHdl));
}

































































#undef CREATE_CHAIN_HDL_2


TEST_F(BufHdlTest, Case14_test_SplitBufHdl)
{
	ColorPrintf(TC_Red, "test SplitBufHdl()...\n");

	
	BufHandle *pHdl, *pNew;
	int nNewLen, nLen1, nLen2;
	char pBuf1[] = "user content1";
	char pBuf2[] = "user content_2";
	nLen1 = sizeof(pBuf1);
	nLen2 = sizeof(pBuf2);

	
	pHdl = AllocateHdl(true, pBuf2, nLen2, NULL);
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-1, 
				                     s_nBigTranBuf, s_nBigTranBuf, s_nBigTranBuf,
									 s_nBufHdl-1));
	PrintPoolInfo("before SplitBufHdl():\n");

	
	nNewLen = nLen2 + 1;
	SHOULD_ABORT(SplitBufHdl(pHdl, nNewLen));
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-1, 
				                     s_nBigTranBuf, s_nBigTranBuf, s_nBigTranBuf,
									 s_nBufHdl-1));

	
	nNewLen = nLen2 - 1;
	pNew = SplitBufHdl(pHdl, nNewLen);
	
	ASSERT(nNewLen == pHdl->nDataLen);
	ASSERT(pNew == pHdl->_next);
	ASSERT(pNew->nDataLen = 1);
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-1, 
				                     s_nBigTranBuf, s_nBigTranBuf, s_nBigTranBuf,
									 s_nBufHdl-2));
	PrintPoolInfo("after SplitBufHdl(pHdl):\n");
	PrintChainHdl(pHdl, NULL);

	
	FreeHdl(pHdl);
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-1, 
				                     s_nBigTranBuf, s_nBigTranBuf, s_nBigTranBuf,
									 s_nBufHdl-1));
	PrintPoolInfo("after FreeHdl(pHdl):\n");
	FreeHdl(pNew);
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf, 
				                     s_nBigTranBuf, s_nBigTranBuf, s_nBigTranBuf,
									 s_nBufHdl));
	PrintPoolInfo("after FreeHdl(pNew):\n");

	
	pHdl = AllocateHdl(true, pBuf2, nLen2, NULL);
	pHdl = AllocateHdl(true, pBuf1, nLen1, pHdl);
	ColorPrintf(TC_Red, "before Split, PrintChain(pHdl):\n");
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-2, 
				                     s_nBigTranBuf, s_nBigTranBuf, s_nBigTranBuf,
									 s_nBufHdl-2));
	PrintChainHdl(pHdl, NULL);
	PrintPoolInfo("before Split:\n");

	nNewLen = nLen1 - 5;;
	pNew = SplitBufHdl(pHdl, nNewLen); 
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-2, 
				                     s_nBigTranBuf, s_nBigTranBuf, s_nBigTranBuf,
									 s_nBufHdl-3));
	ColorPrintf(TC_Red, "after Split(pHdl), PrintChain(pHdl):\n");
	PrintChainHdl(pHdl, NULL);
	PrintPoolInfo("after Split:\n");

	
	ChainFreeHdl(pHdl, NULL);	
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf, 
				                     s_nBigTranBuf, s_nBigTranBuf, s_nBigTranBuf,
									 s_nBufHdl));
	PrintPoolInfo("after free all allocated BufHandles:\n");
}


#if 0

static void PrintHdlsContent(BufHandle* pHdl, BufHandle* pNext)
{
	ASSERT(pHdl != NULL);
	static char buf[s_nBufSize+1];
	memset((void*)buf, 0, s_nBufSize+1);

	while(pHdl != pNext) {
		memcpy((void*)buf, (void*)pHdl->pBuf, pHdl->nDataLen);
		printf("%s", buf);
		memset((void*)buf, 0, s_nBufSize+1);
		pHdl = pHdl->_next;
	}
	printf("\n");
	fflush(stdout);
}
#endif

static void s_SetBuf(char* buf, int nSize)
{
	char* p = "0123456789";

	for(int i = 0; i < nSize/10; ++i) {
		memcpy((void*)buf, (void*)p, 10);
		buf += 10;
	}

	if(nSize%10)
		memcpy((void*)buf, (void*)p, nSize%10);
}


TEST_F(BufHdlTest, Case15_test_BufHdlPrintf)
{
	ColorPrintf(TC_Red, "test BufHdlPrintf()...\n");

	
	BufHandle *pHdl, *pHdl2, *pHdl3;
	int nExpectLen;

	
	PrintPoolInfo("before test:\n");
	char* pUC = "user content#%d.\n";
	pHdl = BufHdlPrintf(NULL, pUC, 1);		
	
	PrintPoolInfo("after printf(\"user content#%%d.\\n\", 1):\n");
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-1, 
				                     s_nBigTranBuf, s_nBigTranBuf, s_nBigTranBuf,
									 s_nBufHdl-1));
	ASSERT(NULL != pHdl);
	nExpectLen = strlen(pUC) - 1;  
	ASSERT_EQUAL(nExpectLen, pHdl->nDataLen);
	ASSERT(*(pHdl->pBuf+pHdl->nDataLen-1) == '\n');

	
	pHdl2 = BufHdlPrintf(pHdl, pUC, 2);
	
	PrintPoolInfo("continue to printf(\"user content#%%d.\\n\", 2):\n");
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-2, 
				                     s_nBigTranBuf, s_nBigTranBuf, s_nBigTranBuf,
									 s_nBufHdl-2));
	ASSERT(NULL != pHdl2);
	nExpectLen = strlen(pUC) - 1;
	ASSERT_EQUAL(nExpectLen, pHdl2->nDataLen);
	
	ChainFreeHdl(pHdl2, NULL);  
	PrintPoolInfo("after free all the handles:\n");
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf, 
				                     s_nBigTranBuf, s_nBigTranBuf, s_nBigTranBuf,
									 s_nBufHdl));

	 
	char buf[s_nBufSize+2];
	s_SetBuf(buf, s_nBufSize+2);
	memcpy((void*)(buf+s_nBufSize+1-strlen(pUC)), (void*)pUC, strlen(pUC));	
	buf[s_nBufSize+1] = '\0';
	ASSERT(NULL != (pHdl = BufHdlPrintf(NULL, buf, 3)));
	
	PrintPoolInfo("after BufHdlPrintf(nlen == %d):\n", s_nBufSize);
	
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-1, 
				                     s_nBigTranBuf, s_nBigTranBuf, s_nBigTranBuf,
									 s_nBufHdl-1));
	ASSERT(s_nBufSize == pHdl->nDataLen);
	ASSERT(*(pHdl->pBuf+pHdl->nDataLen-1) == '\n');

	
	char buf2[s_nBufSize+1];
	s_SetBuf(buf2, s_nBufSize+1);
	buf2[s_nBufSize] = '\0';
	ASSERT(NULL != (pHdl2 = BufHdlPrintf(pHdl, buf2)));
	
	PrintPoolInfo("after BufHdlPrintf(nlen == %d) again:\n", s_nBufSize);
	
	ASSERT(s_nBufSize == pHdl2->nDataLen);
	
	ASSERT(s_nBufSize%10? '0'+(s_nBufSize-1)%10:'9'== *(pHdl2->pBuf+s_nBufSize-1));
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-2, 
				                     s_nBigTranBuf, s_nBigTranBuf, s_nBigTranBuf,
									 s_nBufHdl-2));

	
	char buf3[s_nBufSize+3];
	s_SetBuf(buf3, s_nBufSize+3);
	memcpy((void*)(buf3+s_nBufSize+2-strlen(pUC)), (void*)pUC, strlen(pUC));	
	buf3[s_nBufSize+2] = '\0';
	ASSERT(NULL != (pHdl3 = BufHdlPrintf(pHdl2, buf3, 4)));
	
	PrintPoolInfo("continue BufHdlPrintf(nLen == %d):\n", s_nBufSize+1);
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-4, 
				                     s_nBigTranBuf, s_nBigTranBuf, s_nBigTranBuf,
									 s_nBufHdl-4));
	
	ASSERT(s_nBufSize == pHdl3->nDataLen);	
	ASSERT(1 == pHdl3->_next->nDataLen);	
	ASSERT('\n' == *(pHdl3->_next->pBuf));
	ChainFreeHdl(pHdl3, NULL);  
	PrintPoolInfo("after free all the handles:\n");

	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf, 
				                     s_nBigTranBuf, s_nBigTranBuf, s_nBigTranBuf,
									 s_nBufHdl));
}


TEST_F(BufHdlTest, Case16_test_Reclaim)
{
	
	BufHandle *pHdl, *pReclaim;

	
#define CREATE_CHAIN_HDL \
	pHdl = AllocateHdl(true, NULL, s_nBufSize-30, NULL); \
	pHdl = AllocateHdl(true, NULL, s_nBufSize-20, pHdl); \
	pHdl = AllocateHdl(true, NULL, s_nBufSize-10, pHdl); 
	
	
	
	
	ColorPrintf(TC_Red, "first try to Reclaim 50 Bytes:\n");
	CREATE_CHAIN_HDL
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-3, 
				                     s_nBigTranBuf, s_nBigTranBuf, s_nBigTranBuf,
									 s_nBufHdl-3));
	PrintPoolInfo("before Reclaim:\n");
	PrintChainHdl(pHdl, NULL);
	
	ASSERT(NULL == Reclaim(50, pHdl, NULL));
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-3, 
				                     s_nBigTranBuf, s_nBigTranBuf, s_nBigTranBuf,
									 s_nBufHdl-3));
	PrintPoolInfo("try Reclaim 50 Bytes failed:\n");
	PrintChainHdl(pHdl, NULL);

	ChainFreeHdl(pHdl, NULL);
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf, 
				                     s_nBigTranBuf, s_nBigTranBuf, s_nBigTranBuf,
									 s_nBufHdl));
	PrintPoolInfo("free all the BufHdls:\n");

	
	ColorPrintf(TC_Red, "try to Reclaim 15 Bytes:\n");
	CREATE_CHAIN_HDL
	PrintPoolInfo("before Reclaim:\n");
	PrintChainHdl(pHdl, NULL);
	
	pReclaim = Reclaim(15, pHdl, NULL);
	
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-3, 
				                     s_nBigTranBuf, s_nBigTranBuf, s_nBigTranBuf,
									 s_nBufHdl-4));
	ASSERT(15 == pReclaim->nBufLen);
	ASSERT(15 == pReclaim->nDataLen);
	PrintPoolInfo("after Reclaim 15 Bytes:\n");
	PrintChainHdl(pHdl, NULL);

	
	ChainFreeHdl(pHdl, NULL);
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-1, 
				                     s_nBigTranBuf, s_nBigTranBuf, s_nBigTranBuf,
									 s_nBufHdl-1));
	PrintPoolInfo("free original BufHdls:\n");

	
	FreeHdl(pReclaim);
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf, 
				                     s_nBigTranBuf, s_nBigTranBuf, s_nBigTranBuf,
									 s_nBufHdl));
	PrintPoolInfo("free reclaimed BufHdl:\n");
	
	
	ColorPrintf(TC_Red, "try to Reclaim 21 Bytes from [pHdl, pHdl->_next):\n");
	CREATE_CHAIN_HDL
	PrintPoolInfo("before Reclaim:\n");
	PrintChainHdl(pHdl, NULL);
	
	pReclaim = Reclaim(21, pHdl, pHdl->_next);
	
	ASSERT(NULL == pReclaim);
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-3, 
				                     s_nBigTranBuf, s_nBigTranBuf, s_nBigTranBuf,
									 s_nBufHdl-3));
	PrintPoolInfo("try to Reclaim failed:\n");
	PrintChainHdl(pHdl, NULL);

	ChainFreeHdl(pHdl, NULL);
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf, 
				                     s_nBigTranBuf, s_nBigTranBuf, s_nBigTranBuf,
									 s_nBufHdl));
	PrintPoolInfo("free BufHdls:\n");

	
	ColorPrintf(TC_Red, "try to Reclaim 20 Bytes from [pHdl->_next, pHdl->_next->_next):\n");
	CREATE_CHAIN_HDL
	PrintPoolInfo("before Reclaim:\n");
	PrintChainHdl(pHdl, NULL);
	
	pReclaim = Reclaim(20, pHdl->_next, pHdl->_next->_next);
	
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-3, 
				                     s_nBigTranBuf, s_nBigTranBuf, s_nBigTranBuf,
									 s_nBufHdl-4));
	ASSERT(20 == pReclaim->nBufLen);
	ASSERT(20 == pReclaim->nDataLen);
	PrintPoolInfo("Reclaim 20 bytes from pHdl->_next:\n");
	PrintChainHdl(pHdl, NULL);

	
	ChainFreeHdl(pHdl, NULL);
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-1, 
				                     s_nBigTranBuf, s_nBigTranBuf, s_nBigTranBuf,
									 s_nBufHdl-1));
	PrintPoolInfo("free original BufHdls:\n");
	FreeHdl(pReclaim);
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf, 
				                     s_nBigTranBuf, s_nBigTranBuf, s_nBigTranBuf,
									 s_nBufHdl));
	PrintPoolInfo("free reclaimed BufHdl:\n");
}
#undef CREATE_CHAIN_HDL 

