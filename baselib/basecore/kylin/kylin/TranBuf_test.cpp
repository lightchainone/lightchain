#include "stdafx.h"
#include "SyncObj.h"
#include "TranBuf.h"
#include "UnitTest.h"
#include "../test/KylinUtils4Test.h"

class TranBufTest {
plclic:
	void SetUp() {
		
		nBlockSize = 16;
		nAlloc = 4;
		nMin = 1;
		nMax = 10;
		fRatio1 = 0.6;
		fRatio2 = 0.9;
	}
	void TearDown() {}
protected:
	int nBlockSize;
	int nAlloc;
	int nMin;
	int nMax;
	dolcle fRatio1;
	dolcle fRatio2;
};




TEST_F(TranBufTest, Case1_test_Create)
{
	ColorPrintf(TC_Red, "test CTranBufPool::Create()...\n");

	CTranBufPool *pBufPool = new CTranBufPool("tranbuf", BUFPOOL_C1);

	ASSERT(pBufPool->Create(nBlockSize, nAlloc, nMin, nMax, fRatio1, fRatio2));
	
	ASSERT(nAlloc*nMax == pBufPool->GetTotalBufNum());
	ASSERT(nMin*nAlloc == pBufPool->GetAllocatedBufNum());
	ASSERT(nAlloc*nMax == pBufPool->GetFreeBufNum());
	
	PrintTranBufPool(pBufPool, "TranBufPool", "TranBufPool after Create(%d, %d, %d, %d, %.1f, %.1f):\n", nBlockSize, nAlloc, nMin, nMax, fRatio1, fRatio2);
	delete pBufPool;
}



TEST_F(TranBufTest, Case2_test_Create)
{
	ColorPrintf(TC_Red, "test CTranBufPool::Create(nMin=0)...\n");

	nMin = 0; 

	CTranBufPool *pBufPool = new CTranBufPool("tranbuf", BUFPOOL_C1);
	
	ASSERT(pBufPool->Create(nBlockSize, nAlloc, nMin, nMax, fRatio1, fRatio2));
	
	ASSERT(nAlloc*nMax == pBufPool->GetTotalBufNum());
	ASSERT(nMin*nAlloc == pBufPool->GetAllocatedBufNum());
	ASSERT(nAlloc*nMax == pBufPool->GetFreeBufNum());

	PrintTranBufPool(pBufPool, "TranBufPool", "TranBufPool after Create(nMin=0):\n");

	delete pBufPool;
}



TEST_F(TranBufTest, Case3_test_Allocate)
{
	ColorPrintf(TC_Red, "test Allocate() with priority = 0...\n");

	
	const int nPriority = 0;
	char* pBufs[nMax*nAlloc];
	int watermark0 = (int)(nAlloc*nMax*fRatio1);

	for(int i = 0; i < nMax*nAlloc; ++i)
		pBufs[i] = NULL;

	
	CTranBufPool *pBufPool = new CTranBufPool("tranbuf", BUFPOOL_C1);
	ASSERT(pBufPool->Create(nBlockSize, nAlloc, nMin, nMax, fRatio1, fRatio2));
	PrintTranBufPool(pBufPool, "TranBufPool", "on creation:\n");

	
	ASSERT(nAlloc*nMax == pBufPool->GetTotalBufNum());
	ASSERT(nMin*nAlloc == pBufPool->GetAllocatedBufNum());
	ASSERT(nAlloc*nMax == pBufPool->GetFreeBufNum());
	
	
	for(int i = 0; i <= watermark0; ++i) {
		pBufs[i] = pBufPool->Allocate(nPriority);
		ASSERT(NULL != pBufs[i]);
	}
	
	
	ASSERT(nAlloc*nMax == pBufPool->GetTotalBufNum());
	ASSERT(watermark0+nAlloc == pBufPool->GetAllocatedBufNum());
	ASSERT(nAlloc*nMax-watermark0-1 == pBufPool->GetFreeBufNum());

	
	PrintTranBufPool(pBufPool, "TranBufPool", "exhaust free-buffers below watermark#0(%d):\n", watermark0);
	
	
	ColorPrintf(TC_Red, "try allocate buffers above watermark#0(%d) with priority = 0...\n", watermark0);
	pBufs[25] = pBufPool->Allocate(nPriority);
	ASSERT(NULL == pBufs[25]);

	
	ASSERT(nAlloc*nMax == pBufPool->GetTotalBufNum());
	ASSERT(watermark0+nAlloc == pBufPool->GetAllocatedBufNum());
	ASSERT(nAlloc*nMax-watermark0-1 == pBufPool->GetFreeBufNum());

	PrintTranBufPool(pBufPool, "TranBufPool", "try to allocate buffers above watermark#0 FAILED:\n");

	delete pBufPool;
}



TEST_F(TranBufTest, Case4_test_Allocate)
{
	ColorPrintf(TC_Red, "test Allocate() with priority = 1...\n");

	
	const int nPriority = 1;
	char* pBufs[nMax*nAlloc];
	int watermark0 = (int)(nAlloc*nMax*fRatio1);
	int watermark1 = (int)(nAlloc*nMax*fRatio2);

	for(int i = 0; i < nMax*nAlloc; ++i)
		pBufs[i] = NULL;

	
	CTranBufPool *pBufPool = new CTranBufPool("tranbuf", BUFPOOL_C1);
	ASSERT(pBufPool->Create(nBlockSize, nAlloc, nMin, nMax, fRatio1, fRatio2));
	PrintTranBufPool(pBufPool, "TranBufPool", "on creation:\n");

	
	ASSERT(nAlloc*nMax == pBufPool->GetTotalBufNum());
	ASSERT(nMin*nAlloc == pBufPool->GetAllocatedBufNum());
	ASSERT(nAlloc*nMax == pBufPool->GetFreeBufNum());

	
	int i = 0;
	for(; i <= watermark0; ++i) {
		pBufs[i] = pBufPool->Allocate(nPriority);
		ASSERT(NULL != pBufs[i]);
	}

	
	ASSERT(nAlloc*nMax == pBufPool->GetTotalBufNum());
	ASSERT(watermark0+nAlloc == pBufPool->GetAllocatedBufNum());
	ASSERT(nAlloc*nMax-watermark0-1 == pBufPool->GetFreeBufNum());

	PrintTranBufPool(pBufPool, "TranBufPool", "exhaust free-buffers below watermark#0(%d):\n", watermark0);

	
	ColorPrintf(TC_Red, "allocate buffers (watermark#0, watermark#1]...\n");
	for(; i <= watermark1; ++i) {
		pBufs[i] = pBufPool->Allocate(nPriority);
		ASSERT(NULL != pBufs[i]);
	}

	
	ASSERT(nAlloc*nMax == pBufPool->GetTotalBufNum());
	ASSERT(watermark1+nAlloc == pBufPool->GetAllocatedBufNum());
	ASSERT(nAlloc*nMax-watermark1-1 == pBufPool->GetFreeBufNum());

	PrintTranBufPool(pBufPool, "TranBufPool", "exhaust free-buffers (watermark#0, watermark#1]:\n");

	
	ColorPrintf(TC_Red, "try to allocate buffers above watermark#1...\n");
	pBufs[watermark1+1] = pBufPool->Allocate(nPriority);
	
	
	ASSERT(NULL == pBufs[watermark1+1]);
	ASSERT(nAlloc*nMax == pBufPool->GetTotalBufNum());
	ASSERT(watermark1+nAlloc == pBufPool->GetAllocatedBufNum());
	ASSERT(nAlloc*nMax-watermark1-1 == pBufPool->GetFreeBufNum());

	PrintTranBufPool(pBufPool, "TranBufPool", "try to allocate buffers above watermark#1 FAILED:\n");

	delete pBufPool;
}



TEST_F(TranBufTest, Case5_test_Allocate)
{
	ColorPrintf(TC_Red, "test Allocate() with priority = 2...\n");

	
	const int nPriority = 2;
	char* pBufs[nMax*nAlloc];
	int watermark1 = (int)(nAlloc*nMax*fRatio2);

	for(int i = 0; i < nMax*nAlloc; ++i)
		pBufs[i] = NULL;

	
	CTranBufPool *pBufPool = new CTranBufPool("tranbuf", BUFPOOL_C1);
	ASSERT(pBufPool->Create(nBlockSize, nAlloc, nMin, nMax, fRatio1, fRatio2));
	
	
	int i = 0;
	for(; i <= watermark1; ++i) {
		pBufs[i] = pBufPool->Allocate(nPriority);
		ASSERT(NULL != pBufs[i]);
	}

	
	ASSERT(nAlloc*nMax == pBufPool->GetTotalBufNum());
	ASSERT(watermark1+nAlloc == pBufPool->GetAllocatedBufNum());
	ASSERT(nAlloc*nMax-watermark1-1 == pBufPool->GetFreeBufNum());

	PrintTranBufPool(pBufPool, "TranBufPool", "exhaust free-buffers below watermark#1(%d):\n", watermark1);

	
	for(; i <= nMax*nAlloc-1; ++i) {
		pBufs[i] = pBufPool->Allocate(nPriority);
		ASSERT(NULL != pBufs[i]);
	}

	
	ASSERT(nAlloc*nMax == pBufPool->GetTotalBufNum());
	ASSERT(nMax*nAlloc == pBufPool->GetAllocatedBufNum());
	ASSERT(0 == pBufPool->GetFreeBufNum());

	PrintTranBufPool(pBufPool, "TranBufPool", "no free buffer in the pool:\n");

	ColorPrintf(TC_Red, "try allocate buffer from empty-pool...\n");
	char* pTmp = NULL;
	pTmp = pBufPool->Allocate(nPriority);
	
	ASSERT(NULL == pTmp);

	delete pBufPool;
}


TEST_F(TranBufTest, Case6_test_AddRef)
{
	ColorPrintf(TC_Red, "test AddRef()...\n");
	
	char* p1 = NULL;

	
	CTranBufPool *pBufPool = new CTranBufPool("tranbuf", BUFPOOL_C1);
	ASSERT(pBufPool->Create(nBlockSize, nAlloc, nMin, nMax, fRatio1, fRatio2));

	
	ASSERT(-1 == pBufPool->AddRef(NULL));

	
	p1 = pBufPool->Allocate(0);
	
	
	ASSERT(nAlloc*nMax == pBufPool->GetTotalBufNum());
	ASSERT(nAlloc == pBufPool->GetAllocatedBufNum());
	ASSERT(nAlloc*nMax-1 == pBufPool->GetFreeBufNum());
	ASSERT(NULL != p1);
	ASSERT(2 == pBufPool->AddRef(p1));

	
	ASSERT(3 == pBufPool->AddRef(p1));

	
	ASSERT(2 == pBufPool->Free(p1));

	
	ASSERT(1 == pBufPool->Free(p1));

	
	ASSERT(0 == pBufPool->Free(p1));
	ASSERT(nAlloc == pBufPool->GetAllocatedBufNum());
	ASSERT(nAlloc*nMax == pBufPool->GetFreeBufNum());
	
	
	

	delete pBufPool;
}



TEST_F(TranBufTest, Case7_test_Free)
{
	ColorPrintf(TC_Red, "test Free()...\n");

	char* p1 = NULL;

	
	CTranBufPool *pBufPool = new CTranBufPool("tranbuf", BUFPOOL_C1);
	ASSERT(pBufPool->Create(nBlockSize, nAlloc, nMin, nMax, fRatio1, fRatio2));

	
	ASSERT(-1 == pBufPool->Free(NULL));

	
	p1 = pBufPool->Allocate(0);
	ASSERT(NULL != p1);
	ASSERT(nAlloc*nMax == pBufPool->GetTotalBufNum());
	ASSERT(nAlloc == pBufPool->GetAllocatedBufNum());
	ASSERT(nAlloc*nMax-1 == pBufPool->GetFreeBufNum());

	
	ASSERT(0 == pBufPool->Free(p1));
	ASSERT(nAlloc == pBufPool->GetAllocatedBufNum());
	ASSERT(nAlloc*nMax == pBufPool->GetFreeBufNum());
	
	
	

	delete pBufPool;
}

TEST_F(TranBufTest, Case8_Log_2)
{
    int n;

    for (int i=0; i<32; i++) {
        n = Log_2(1 << i);
        ASSERT_EQUAL(n, i);
    }

    n = Log_2(0);
    ASSERT_EQUAL(n, -1);
    for (int i=1; i<32; i++) {
        n = Log_2((1 << i) + 1);
        ASSERT_EQUAL(n, -1);
        n = Log_2((1 << i) + 3);
        ASSERT_EQUAL(n, -1);
    }
}

TEST_F(TranBufTest, Case9_Allocate1)
{
    CTranBufPool *pBufPool = new CTranBufPool("tranbuf", BUFPOOL_C1);
    ASSERT(pBufPool->Create(nBlockSize, nAlloc, nMin, nMax, fRatio1, fRatio2));

    char* p = pBufPool->Allocate(0, 3);
    ASSERT(NULL != p);

    pBufPool->Free(p);
    ASSERT_EQUAL(100, pBufPool->GetFreePercent());
}

TEST_F(TranBufTest, Case10_Allocate2)
{
    CTranBufPool *pBufPool = new CTranBufPool("tranbuf", BUFPOOL_C1);
    ASSERT(pBufPool->Create(nBlockSize, nAlloc, nMin, nMax, fRatio1, fRatio2));

    int n;
    char* p;
    for (int i=0; i<3; i++) {
        p = pBufPool->Allocate(0, 3);
        ASSERT(NULL != p);
        
        n = pBufPool->AddRef(p+1);
        ASSERT_EQUAL(2, n);
        n = pBufPool->AddRef(p+nBlockSize);
        ASSERT_EQUAL(3, n);
        n = pBufPool->AddRef(p+nBlockSize+1);
        ASSERT_EQUAL(4, n);

        n = pBufPool->Free(p);
        ASSERT_EQUAL(3, n);
        n = pBufPool->Free(p);
        ASSERT_EQUAL(2, n);
        n = pBufPool->Free(p);
        ASSERT_EQUAL(1, n);
        n = pBufPool->Free(p);
        ASSERT_EQUAL(0, n);
        ASSERT_EQUAL(100, pBufPool->GetFreePercent());
    }
}

TEST_F(TranBufTest, Case11_Allocate3)
{
    CTranBufPool *pBufPool = new CTranBufPool("tranbuf", BUFPOOL_C1);
    ASSERT(pBufPool->Create(nBlockSize, nAlloc, nMin, nMax, fRatio1, fRatio2));

    int n;
    char* p[3];
    for (int i=0; i<3; i++) {
        p[i] = pBufPool->Allocate(0, 3);
        ASSERT(NULL != p[i]);

        n = pBufPool->AddRef(p[i]+1);
        ASSERT_EQUAL(2, n);
        n = pBufPool->AddRef(p[i]+nBlockSize);
        ASSERT_EQUAL(3, n);
        n = pBufPool->AddRef(p[i]+nBlockSize+1);
        ASSERT_EQUAL(4, n);
    }

    for (int i=0; i<3; i++) {
        n = pBufPool->Free(p[i]);
        ASSERT_EQUAL(3, n);
        n = pBufPool->Free(p[i]);
        ASSERT_EQUAL(2, n);
        n = pBufPool->Free(p[i]);
        ASSERT_EQUAL(1, n);
        n = pBufPool->Free(p[i]);
        ASSERT_EQUAL(0, n);
    }
    ASSERT_EQUAL(100, pBufPool->GetFreePercent());
}


TEST_F(TranBufTest, Case12_Allocate4)
{
    CTranBufPool *pBufPool = new CTranBufPool("tranbuf", BUFPOOL_C1);
    ASSERT(pBufPool->Create(nBlockSize, nAlloc, nMin, nMax, fRatio1, fRatio2));

    int n;
    char* p[6];
    for (int i=0; i<6; i+=3) {
        p[i] = pBufPool->Allocate(0, 3);
        ASSERT(NULL != p[i]);

        n = pBufPool->AddRef(p[i]+1);
        ASSERT_EQUAL(2, n);
        n = pBufPool->AddRef(p[i]+nBlockSize);
        ASSERT_EQUAL(3, n);
        n = pBufPool->AddRef(p[i]+nBlockSize+1);
        ASSERT_EQUAL(4, n);

        p[i+1] = pBufPool->Allocate(0, 1);
        n = pBufPool->AddRef(p[i+1]+1);
        ASSERT_EQUAL(2, n);
        p[i+2] = pBufPool->Allocate(0, 1);
    }

    for (int i=0; i<6; i+=3) {
        n = pBufPool->Free(p[i]);
        ASSERT_EQUAL(3, n);
        n = pBufPool->Free(p[i]);
        ASSERT_EQUAL(2, n);
        n = pBufPool->Free(p[i]);
        ASSERT_EQUAL(1, n);
        n = pBufPool->Free(p[i]);
        ASSERT_EQUAL(0, n);

        n = pBufPool->Free(p[i+1]+1);
        ASSERT_EQUAL(1, n);
        n = pBufPool->Free(p[i+1]);
        ASSERT_EQUAL(0, n);
        n = pBufPool->Free(p[i+2]);
        ASSERT_EQUAL(0, n);
    }
    ASSERT_EQUAL(100, pBufPool->GetFreePercent());
}
