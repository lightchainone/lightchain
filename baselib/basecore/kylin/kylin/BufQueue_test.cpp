#include "stdafx.h"
#include "SyncObj.h"
#include "BufQueue.h"
#include "UnitTest.h"
#include "../test/KylinUtils4Test.h"

static const int s_nTranBuf = 20; 
static const int s_nBigTranBuf = 20; 
static const int s_nBufSize = 1024; 
static const int s_nBufHdl = 1024;  

class CBufQueue_test : plclic CBufQueue {
plclic:
	BufHandle* GetFirstHdl() const { return m_pFirst; }

	int GetBufNum() const {
		int n = 0;
		BufHandle* pHdl = NULL;
		for(pHdl = m_pFirst; pHdl != NULL; pHdl = pHdl->_next, ++n);
		return n;
	}
};

class BufQTest {
plclic:
	void SetUp() {
		if(!m_bInit) {
			SetTranBuf(s_nTranBuf, s_nBigTranBuf, s_nBufSize);
			m_bInit = true;
		}
	}
	void TearDown() {
		ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf, 
					                     s_nBigTranBuf, 0, s_nBigTranBuf,
										 s_nBufHdl));
	}
protected:
	static bool m_bInit;
};

bool BufQTest::m_bInit = false;

const static int s_nHdlDataLen = 10;
static char s_c = '0';

static void PrintHdlContent2(BufHandle* pHdl);
static void CreateHdlChain(BufHandle** pHdl, int nChainLen, bool bReset);
static void PrintHdlChain(BufHandle* pHdl, BufHandle* pNext);
static void PrintBuf(char* pBuf, int nBufSize);

#define VERIFY_QUEUE_STATE(pFirstHdl, nBufNum, nTotalLen, nReserveLen) \
	ASSERT(pFirstHdl == bufQ.GetFirstHdl());                  \
	ASSERT(nBufNum == bufQ.GetBufNum());                      \
	ASSERT(nTotalLen == bufQ.GetTotalLen());                  \
	ASSERT(nReserveLen == bufQ.GetReserveLen());


TEST_F(BufQTest, Case1_test_Append)
{
	BufHandle *pHdls[5];
	CBufQueue_test bufQ;
	
	
	ColorPrintf(TC_Red, "some handle's DataLen in [pHdl,pNext) is 0\n");
	CreateHdlChain(pHdls, 5, true);
	
	
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-5, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl-5));
	VERIFY_QUEUE_STATE(NULL, 0, 0, 0)

	BufHandle *pTmp = AllocateHdl(true, NULL, 0, NULL);
	ASSERT(NULL != pTmp);
	
	pHdls[2]->_next = pTmp;
	pTmp->_next = pHdls[3];
	
	PrintPoolInfo("now the pool:\n");
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-6, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl-6));
	PrintHdlChain(pHdls[0], NULL);

	
	SHOULD_ABORT(bufQ.Append(pHdls[0], NULL));

	
	ChainFreeHdl(pHdls[0], NULL);	
	PrintPoolInfo("free the allocated handles:\n");
}


TEST_F(BufQTest, Case2_test_Append)
{
	BufHandle *pHdls[5], *pHdls2[3];
	int nTotalLen, nAppendLen;
	CBufQueue_test bufQ;
	
	ColorPrintf(TC_Red, "test normal Append()...\n");

	CreateHdlChain(pHdls, 5, true);
	CreateHdlChain(pHdls2, 3, false);
	pHdls2[1]->nDataLen = 5; 

	
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-8, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl-8));
	VERIFY_QUEUE_STATE(NULL, 0, 0, 0)
	
	
	nAppendLen = GetTotalDataLen(pHdls[0], pHdls[3]);
	ASSERT(nAppendLen == bufQ.Append(pHdls[0], pHdls[3]));

	nTotalLen = nAppendLen;
	ColorPrintf(TC_Magenta, "Append(pHdls[0], pHdls[3])\n");
	PrintHdlChain(pHdls[0], NULL);
	
	ASSERT(bufQ.GetTotalLen() == nTotalLen);
	VERIFY_QUEUE_STATE(pHdls[0], 3, nTotalLen, 0)

	
	nAppendLen = GetTotalDataLen(pHdls[3], NULL);
	ASSERT(nAppendLen == bufQ.Append(pHdls[3], NULL));
	nTotalLen += nAppendLen;

	ColorPrintf(TC_Magenta, "Append(pHdls[3], NULL)\n");
	PrintHdlChain(pHdls[0], NULL);
	
	ASSERT(bufQ.GetTotalLen() == nTotalLen);
	VERIFY_QUEUE_STATE(pHdls[0], 5, nTotalLen, 0)

	
	nAppendLen = GetTotalDataLen(pHdls2[0], NULL);
	ASSERT(nAppendLen == bufQ.Append(pHdls2[0], NULL));
	nTotalLen += nAppendLen;

	ColorPrintf(TC_Magenta, "Append(pHdls2[0], NULL)\n");
	PrintHdlChain(pHdls[0], NULL);
	
	ASSERT(bufQ.GetTotalLen() == nTotalLen);
	VERIFY_QUEUE_STATE(pHdls[0], 8, nTotalLen, 0)

	
	bufQ.Clear();	
	VERIFY_QUEUE_STATE(NULL, 0, 0, 0)
	PrintPoolInfo("free the allocated handles:\n");
}



TEST_F(BufQTest, Case3_test_GetN)
{
	ColorPrintf(TC_Red, "test \"BufHandle* GetN()\" (ReqLen < BufQ_DataLen)...\n");

	BufHandle *pHdls[5], *pGetN, *pGetAll;
	int pnLen;
	CBufQueue_test bufQ;

#define APPEND_ALL(n) \
	CreateHdlChain(pHdls, n, true); \
	bufQ.Append(pHdls[0], NULL);
	
	APPEND_ALL(5)
	
	VERIFY_QUEUE_STATE(pHdls[0], 5, 50, 0)
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-5, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl-5));
	
	ColorPrintf(TC_Magenta, "before GetN():\n");
	PrintHdlChain(pHdls[0], NULL);

	pnLen = 21;
	pGetN = bufQ.GetN(&pnLen);	
	
	
	ASSERT(pnLen == 21);
	VERIFY_QUEUE_STATE(bufQ.GetFirstHdl(), 3, 29, 0)
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-5, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl-6));

	ColorPrintf(TC_Magenta, "the chain get by GetN(%d):\n", pnLen);
	PrintHdlChain(pGetN, NULL);
	ColorPrintf(TC_Magenta, "the chain remain in BufQueue:\n");
	pGetAll = bufQ.GetAll();
	PrintHdlChain(pGetAll, NULL);

	
	ChainFreeHdl(pGetN, NULL);	
	ChainFreeHdl(pGetAll, NULL);	
	VERIFY_QUEUE_STATE(NULL, 0, 0, 0)
	PrintPoolInfo("free the allocated handles:\n");
}



TEST_F(BufQTest, Case4_test_GetN)
{
	ColorPrintf(TC_Red, "test \"BufHandle* GetN()\" (ReqLen = BufQ_DataLen)...\n");

	BufHandle *pHdls[4], *pGetN, *pGetAll;
	int pnLen;
	CBufQueue_test bufQ;

	APPEND_ALL(4)
	
	VERIFY_QUEUE_STATE(pHdls[0], 4, 40, 0)
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-4, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl-4));

	ColorPrintf(TC_Magenta, "before GetN():\n");
	PrintHdlChain(pHdls[0], NULL);

	pnLen = 40;
	pGetN = bufQ.GetN(&pnLen);	
	
	
	ASSERT(pnLen == 40);
	VERIFY_QUEUE_STATE(NULL, 0, 0, 0)
	
	ColorPrintf(TC_Magenta, "the chain get by GetN(%d):\n", pnLen);
	PrintHdlChain(pGetN, NULL);
	ColorPrintf(TC_Magenta, "the chain remain in BufQueue:\n");
	pGetAll = bufQ.GetAll();
	ASSERT(NULL == pGetAll);
	PrintHdlChain(pGetAll, NULL);

	
	ChainFreeHdl(pGetN, NULL);	
	VERIFY_QUEUE_STATE(NULL, 0, 0, 0)
	PrintPoolInfo("free the allocated handles:\n");
}




TEST_F(BufQTest, Case5_test_GetN)
{
	ColorPrintf(TC_Red, "test \"BufHandle* GetN()\" (ReqLen < BufQ_DataLen && ReqLen is at boundary)...\n");

	BufHandle *pHdls[5], *pGetN, *pGetAll;
	int pnLen;
	CBufQueue_test bufQ;

	APPEND_ALL(5)
	
	VERIFY_QUEUE_STATE(pHdls[0], 5, 50, 0)
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-5, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl-5));

	ColorPrintf(TC_Magenta, "before GetN():\n");
	PrintHdlChain(pHdls[0], NULL);

	pnLen = 20;
	pGetN = bufQ.GetN(&pnLen);	
	
	
	ASSERT(pnLen == 20);
	VERIFY_QUEUE_STATE(pHdls[2], 3, 30, 0)

	ColorPrintf(TC_Magenta, "the chain get by GetN(%d):\n", pnLen);
	PrintHdlChain(pGetN, NULL);
	ColorPrintf(TC_Magenta, "the chain remain in BufQueue:\n");
	pGetAll = bufQ.GetAll();
	PrintHdlChain(pGetAll, NULL);

	
	ChainFreeHdl(pGetN, NULL);	
	ChainFreeHdl(pGetAll, NULL);	
	VERIFY_QUEUE_STATE(NULL, 0, 0, 0)
	PrintPoolInfo("free the allocated handles:\n");
}



TEST_F(BufQTest, Case6_test_GetN)
{
	ColorPrintf(TC_Red, "test \"BufHandle* GetN()\" (ReqLen > BufQ_DataLen)...\n");

	BufHandle *pHdls[3], *pGetN, *pGetAll;
	int pnLen;
	CBufQueue_test bufQ;

	APPEND_ALL(3)
	
	VERIFY_QUEUE_STATE(pHdls[0], 3, 30, 0)
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-3, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl-3));

	ColorPrintf(TC_Magenta, "before GetN():\n");
	PrintHdlChain(pHdls[0], NULL);

	pnLen = 40;
	pGetN = bufQ.GetN(&pnLen);	
	
	
	ASSERT(pnLen == 30);
	VERIFY_QUEUE_STATE(NULL, 0, 0, 0)

	ColorPrintf(TC_Magenta, "the chain get by GetN(40):\n");
	PrintHdlChain(pGetN, NULL);
	ColorPrintf(TC_Magenta, "the chain remain in BufQueue:\n");
	pGetAll = bufQ.GetAll();
	ASSERT(NULL == pGetAll);
	PrintHdlChain(pGetAll, NULL);

	
	ChainFreeHdl(pGetN, NULL);	
	VERIFY_QUEUE_STATE(NULL, 0, 0, 0)
	PrintPoolInfo("free the allocated handles:\n");
}



TEST_F(BufQTest, Case7_test_GetN)
{
	ColorPrintf(TC_Red, "test \"int GetN()\" (ReqLen < BufQ_DataLen)...\n");

	BufHandle *pHdls[5], *pGetAll;
	int nRtn, nRequest;
	const int nBufSize = 23;
	char buf[nBufSize];
	CBufQueue_test bufQ;

	memset(buf, 0, nBufSize);

	APPEND_ALL(5)
	
	VERIFY_QUEUE_STATE(pHdls[0], 5, 50, 0)
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-5, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl-5));

	ColorPrintf(TC_Magenta, "before GetN():\n");
	PrintHdlChain(pHdls[0], NULL);

	nRequest = nBufSize;
	nRtn = bufQ.GetN(nRequest, buf);	

	
	ASSERT(27 == bufQ.GetTotalLen());
	VERIFY_QUEUE_STATE(bufQ.GetFirstHdl(), 3, 27, 0)
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-3, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl-3));
	ASSERT(nRtn == 23);

	ColorPrintf(TC_Magenta, "the buffer get by GetN(%d):\n", nRequest);
	PrintBuf(buf, nBufSize);

	ColorPrintf(TC_Magenta, "the chain remain in BufQueue:\n");
	pGetAll = bufQ.GetAll();
	PrintHdlChain(pGetAll, NULL);

	
	ChainFreeHdl(pGetAll, NULL);	
	VERIFY_QUEUE_STATE(NULL, 0, 0, 0)
	PrintPoolInfo("free the allocated handles:\n");
}



TEST_F(BufQTest, Case8_test_GetN)
{
	ColorPrintf(TC_Red, "test \"int GetN()\" (ReqLen = BufQ_DataLen)...\n");

	BufHandle *pHdls[4], *pGetAll;
	int nRtn, nRequest;
	const int nBufSize = 50;
	char buf[nBufSize];
	CBufQueue_test bufQ;

	memset(buf, 0, nBufSize);

	APPEND_ALL(4)
	
	VERIFY_QUEUE_STATE(pHdls[0], 4, 40, 0)
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-4, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl-4));

	ColorPrintf(TC_Magenta, "before GetN():\n");
	PrintHdlChain(pHdls[0], NULL);

	nRequest = 40;
	nRtn = bufQ.GetN(nRequest, buf);	

	
	ASSERT(0 == bufQ.GetTotalLen());
	VERIFY_QUEUE_STATE(NULL, 0, 0, 0)
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl));
	ASSERT(nRtn == 40);

	ColorPrintf(TC_Magenta, "the buffer get by GetN(%d):\n", nRequest);
	PrintBuf(buf, nBufSize);

	ColorPrintf(TC_Magenta, "the chain remain in BufQueue:\n");
	pGetAll = bufQ.GetAll();
	PrintHdlChain(pGetAll, NULL);

	
	VERIFY_QUEUE_STATE(NULL, 0, 0, 0)
	PrintPoolInfo("the bufpool:\n");
}




TEST_F(BufQTest, Case9_test_GetN)
{
	ColorPrintf(TC_Red, "test \"int GetN()\" (ReqLen = BufQ_DataLen && ReqLen is at boundary)...\n");

	BufHandle *pHdls[5], *pGetAll;
	int nRtn, nRequest;
	const int nBufSize = 50;
	char buf[nBufSize];
	CBufQueue_test bufQ;

	memset(buf, 0, nBufSize);

	APPEND_ALL(5)
	
	VERIFY_QUEUE_STATE(pHdls[0], 5, 50, 0)
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-5, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl-5));

	ColorPrintf(TC_Magenta, "before GetN():\n");
	PrintHdlChain(pHdls[0], NULL);

	nRequest = 20;
	nRtn = bufQ.GetN(nRequest, buf);	

	
	ASSERT(30 == bufQ.GetTotalLen());
	ASSERT(nRtn == 20);
	VERIFY_QUEUE_STATE(pHdls[2], 3, 30, 0)
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-3, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl-3));

	ColorPrintf(TC_Magenta, "the buffer get by GetN(%d):\n", nRequest);
	PrintBuf(buf, nBufSize);

	ColorPrintf(TC_Magenta, "the chain remain in BufQueue:\n");
	pGetAll = bufQ.GetAll();
	PrintHdlChain(pGetAll, NULL);

	
	ChainFreeHdl(pGetAll, NULL);
	VERIFY_QUEUE_STATE(NULL, 0, 0, 0)
	PrintPoolInfo("free the allocated handles:\n");
}



TEST_F(BufQTest, Case10_test_GetN)
{
	ColorPrintf(TC_Red, "test \"int GetN()\" (ReqLen > BufQ_DataLen)...\n");

	BufHandle *pHdls[3], *pGetAll;
	int nRtn, nRequest;
	const int nBufSize = 50;
	char buf[nBufSize];
	CBufQueue_test bufQ;

	memset(buf, 0, nBufSize);

	APPEND_ALL(3)
	
	VERIFY_QUEUE_STATE(pHdls[0], 3, 30, 0)
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-3, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl-3));

	ColorPrintf(TC_Magenta, "before GetN():\n");
	PrintHdlChain(pHdls[0], NULL);

	nRequest = 40;
	nRtn = bufQ.GetN(nRequest, buf);	

	
	ASSERT(0 == bufQ.GetTotalLen());
	VERIFY_QUEUE_STATE(NULL, 0, 0, 0)
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl));
	ASSERT(nRtn == 30);

	ColorPrintf(TC_Magenta, "the buffer get by GetN(%d):\n", nRequest);
	PrintBuf(buf, nBufSize);

	ColorPrintf(TC_Magenta, "the chain remain in BufQueue:\n");
	pGetAll = bufQ.GetAll();
	PrintHdlChain(pGetAll, NULL);

	
	VERIFY_QUEUE_STATE(NULL, 0, 0, 0)
	PrintPoolInfo("the bufpool:\n");
}



TEST_F(BufQTest, Case11_test_GetToZero)
{
	ColorPrintf(TC_Red, "test \"int GetToZero()\"(nRtn = 0)...\n");

	BufHandle *pHdls[3], *pGetAll;
	int nRtn;
	const int nBufSize = 50;
	char buf[nBufSize];
	CBufQueue_test bufQ;

	memset(buf, 0, nBufSize);

	APPEND_ALL(3)
	
	VERIFY_QUEUE_STATE(pHdls[0], 3, 30, 0)
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-3, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl-3));

	ColorPrintf(TC_Magenta, "before GetToZero(%d):\n", nBufSize);
	PrintHdlChain(pHdls[0], NULL);

	nRtn = bufQ.GetToZero(buf, nBufSize);	

	
	ASSERT(0 == nRtn);
	ASSERT(30 == bufQ.GetTotalLen());
	VERIFY_QUEUE_STATE(pHdls[0], 3, 30, 0)
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-3, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl-3));

	ColorPrintf(TC_Magenta, "the queue after GetToZero(%d):\n", nBufSize);
	pGetAll = bufQ.GetAll();
	PrintHdlChain(pGetAll, NULL);

	
	ChainFreeHdl(pGetAll, NULL);
	VERIFY_QUEUE_STATE(NULL, 0, 0, 0)
	PrintPoolInfo("release the handles:\n");
}



TEST_F(BufQTest, Case12_test_GetToZero)
{
	ColorPrintf(TC_Red, "test \"int GetToZero()\"(nRtn = -1)...\n");

	BufHandle *pHdls[5], *pGetAll;
	int nRtn;
	const int nBufSize = 20;
	char buf[nBufSize];
	CBufQueue_test bufQ;

	memset(buf, 0, nBufSize);

	
	CreateHdlChain(pHdls, 5, true);
	pHdls[2]->pBuf[1] = 0; 
	bufQ.Append(pHdls[0], NULL);

	
	VERIFY_QUEUE_STATE(pHdls[0], 5, 50, 0)
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-5, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl-5));

	ColorPrintf(TC_Magenta, "before GetToZero(%d):\n", nBufSize);
	PrintHdlChain(pHdls[0], NULL);

	nRtn = bufQ.GetToZero(buf, nBufSize);	

	
	ASSERT(-1 == nRtn);
	ASSERT(50 == bufQ.GetTotalLen());
	VERIFY_QUEUE_STATE(pHdls[0], 5, 50, 0)
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-5, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl-5));

	ColorPrintf(TC_Magenta, "the queue after GetToZero(%d):\n", nBufSize);
	pGetAll = bufQ.GetAll();
	PrintHdlChain(pGetAll, NULL);

	
	ChainFreeHdl(pGetAll, NULL);
	VERIFY_QUEUE_STATE(NULL, 0, 0, 0)
	PrintPoolInfo("release the handles:\n");
}



TEST_F(BufQTest, Case13_test_GetToZero)
{
	ColorPrintf(TC_Red, "test \"int GetToZero()\"(nRtn > 0)...\n");

	BufHandle *pHdls[5], *pGetAll;
	int nRtn;
	const int nBufSize = 30;
	char buf[nBufSize];
	CBufQueue_test bufQ;

	memset(buf, 0, nBufSize);

	
	CreateHdlChain(pHdls, 5, true);
	pHdls[2]->pBuf[1] = 0; 
	bufQ.Append(pHdls[0], NULL);

	
	VERIFY_QUEUE_STATE(pHdls[0], 5, 50, 0)
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-5, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl-5));

	ColorPrintf(TC_Magenta, "before GetToZero(%d):\n", nBufSize);
	PrintHdlChain(pHdls[0], NULL);

	nRtn = bufQ.GetToZero(buf, nBufSize);	

	
	ASSERT(22 == nRtn);
	ASSERT(28 == bufQ.GetTotalLen());
	ASSERT(21 == strlen(buf));
	VERIFY_QUEUE_STATE(bufQ.GetFirstHdl(), 3, 28, 0)
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-3, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl-3));

	ColorPrintf(TC_Magenta, "the buffer got by GetToZero():\n", nBufSize);
	PrintBuf(buf, nBufSize);

	ColorPrintf(TC_Magenta, "the queue after GetToZero(%d):\n", nBufSize);
	pGetAll = bufQ.GetAll();
	PrintHdlChain(pGetAll, NULL);

	
	ChainFreeHdl(pGetAll, NULL);
	VERIFY_QUEUE_STATE(NULL, 0, 0, 0)
	PrintPoolInfo("release the handles:\n");
}



TEST_F(BufQTest, Case14_test_GetToZero)
{
	ColorPrintf(TC_Red, "test \"int GetToZero()\"(nRtn > 0 && found at boundary)...\n");

	BufHandle *pHdls[3], *pGetAll;
	int nRtn;
	const int nBufSize = 30;
	char buf[nBufSize];
	CBufQueue_test bufQ;

	memset(buf, 0, nBufSize);

	
	CreateHdlChain(pHdls, 3, true);
	pHdls[2]->pBuf[9] = 0; 
	bufQ.Append(pHdls[0], NULL);

	
	VERIFY_QUEUE_STATE(pHdls[0], 3, 30, 0)
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-3, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl-3));

	ColorPrintf(TC_Magenta, "before GetToZero(%d):\n", nBufSize);
	PrintHdlChain(pHdls[0], NULL);

	nRtn = bufQ.GetToZero(buf, nBufSize);	

	
	ASSERT(30 == nRtn);
	ASSERT(0 == bufQ.GetTotalLen());
	ASSERT(29 == strlen(buf));
	VERIFY_QUEUE_STATE(NULL, 0, 0, 0)
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl));

	ColorPrintf(TC_Magenta, "the buffer got by GetToZero():\n", nBufSize);
	PrintBuf(buf, nBufSize);

	ColorPrintf(TC_Magenta, "the queue after GetToZero(%d):\n", nBufSize);
	pGetAll = bufQ.GetAll();
	PrintHdlChain(pGetAll, NULL);

	
	VERIFY_QUEUE_STATE(NULL, 0, 0, 0)
	PrintPoolInfo("release the handles:\n");
}



TEST_F(BufQTest, Case15_test_GetToZero)
{
	ColorPrintf(TC_Red, "test \"int GetToZero()\"(nRtn > 0 && found at boundary)...\n");

	BufHandle *pHdls[5], *pGetAll;
	int nRtn;
	const int nBufSize = 30;
	char buf[nBufSize];
	CBufQueue_test bufQ;

	memset(buf, 0, nBufSize);

	
	CreateHdlChain(pHdls, 5, true);
	pHdls[2]->pBuf[9] = 0; 
	bufQ.Append(pHdls[0], NULL);

	
	VERIFY_QUEUE_STATE(pHdls[0], 5, 50, 0)
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-5, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl-5));

	ColorPrintf(TC_Magenta, "before GetToZero(%d):\n", nBufSize);
	PrintHdlChain(pHdls[0], NULL);

	nRtn = bufQ.GetToZero(buf, nBufSize);	

	
	ASSERT(30 == nRtn);
	ASSERT(20 == bufQ.GetTotalLen());
	ASSERT(29 == strlen(buf));
	VERIFY_QUEUE_STATE(pHdls[3], 2, 20, 0)
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-2, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl-2));

	ColorPrintf(TC_Magenta, "the buffer got by GetToZero():\n", nBufSize);
	PrintBuf(buf, nBufSize);

	ColorPrintf(TC_Magenta, "the queue after GetToZero(%d):\n", nBufSize);
	pGetAll = bufQ.GetAll();
	PrintHdlChain(pGetAll, NULL);

	
	ChainFreeHdl(pGetAll, NULL);
	VERIFY_QUEUE_STATE(NULL, 0, 0, 0)
	PrintPoolInfo("release the handles:\n");
}



TEST_F(BufQTest, Case16_test_PeekN)
{
	ColorPrintf(TC_Red, "test \"int PeekN()\"(RequestLen < bufQ_DataLen)...\n");

	BufHandle *pHdls[5], *pGetAll;
	int nRtn, nRequest;
	const int nBufSize = 50;
	char buf[nBufSize];
	CBufQueue_test bufQ;

	memset(buf, 0, nBufSize);

	
	APPEND_ALL(5)

	
	VERIFY_QUEUE_STATE(pHdls[0], 5, 50, 0)
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-5, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl-5));

	nRequest = 26;

	ColorPrintf(TC_Magenta, "before PeekN(%d):\n", nRequest);
	PrintHdlChain(pHdls[0], NULL);

	nRtn = bufQ.PeekN(nRequest, buf);	

	
	ASSERT(26 == nRtn);
	ASSERT(50 == bufQ.GetTotalLen());
	VERIFY_QUEUE_STATE(pHdls[0], 5, 50, 0)

	ColorPrintf(TC_Magenta, "the buffer got by PeekN(%d):\n", nRequest);
	PrintBuf(buf, nBufSize);

	ColorPrintf(TC_Magenta, "the queue after PeekN(%d):\n", nRequest);
	pGetAll = bufQ.GetAll();
	PrintHdlChain(pGetAll, NULL);

	
	ChainFreeHdl(pGetAll, NULL);
	VERIFY_QUEUE_STATE(NULL, 0, 0, 0)
	PrintPoolInfo("release the handles:\n");
}



TEST_F(BufQTest, Case17_test_PeekN)
{
	ColorPrintf(TC_Red, "test \"int PeekN()\"(RequestLen = bufQ_DataLen)...\n");

	BufHandle *pHdls[5], *pGetAll;
	int nRtn, nRequest;
	const int nBufSize = 50;
	char buf[nBufSize];
	CBufQueue_test bufQ;

	memset(buf, 0, nBufSize);

	
	APPEND_ALL(5)

	
	VERIFY_QUEUE_STATE(pHdls[0], 5, 50, 0)
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-5, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl-5));

	nRequest = 50;

	ColorPrintf(TC_Magenta, "before PeekN(%d):\n", nRequest);
	PrintHdlChain(pHdls[0], NULL);

	nRtn = bufQ.PeekN(nRequest, buf);	

	
	ASSERT(50 == nRtn);
	ASSERT(50 == bufQ.GetTotalLen());
	VERIFY_QUEUE_STATE(pHdls[0], 5, 50, 0)
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-5, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl-5));

	ColorPrintf(TC_Magenta, "the buffer got by PeekN(%d):\n", nRequest);
	PrintBuf(buf, nBufSize);

	ColorPrintf(TC_Magenta, "the queue after PeekN(%d):\n", nRequest);
	pGetAll = bufQ.GetAll();
	PrintHdlChain(pGetAll, NULL);

	
	ChainFreeHdl(pGetAll, NULL);
	VERIFY_QUEUE_STATE(NULL, 0, 0, 0)
	PrintPoolInfo("release the handles:\n");
}



TEST_F(BufQTest, Case18_test_PeekN)
{
	ColorPrintf(TC_Red, "test \"int PeekN()\"(RequestLen > bufQ_DataLen)...\n");

	BufHandle *pHdls[3], *pGetAll;
	int nRtn, nRequest;
	const int nBufSize = 50;
	char buf[nBufSize];
	CBufQueue_test bufQ;

	memset(buf, 0, nBufSize);

	
	APPEND_ALL(3)

	
	VERIFY_QUEUE_STATE(pHdls[0], 3, 30, 0)
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-3, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl-3));

	nRequest = 50;

	ColorPrintf(TC_Magenta, "before PeekN(%d):\n", nRequest);
	PrintHdlChain(pHdls[0], NULL);

	nRtn = bufQ.PeekN(nRequest, buf);	

	
	ASSERT(30 == nRtn);
	ASSERT(30 == bufQ.GetTotalLen());
	VERIFY_QUEUE_STATE(pHdls[0], 3, 30, 0)
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-3, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl-3));

	ColorPrintf(TC_Magenta, "the buffer got by PeekN(%d):\n", nRequest);
	PrintBuf(buf, nBufSize);

	ColorPrintf(TC_Magenta, "the queue after PeekN(%d):\n", nRequest);
	pGetAll = bufQ.GetAll();
	PrintHdlChain(pGetAll, NULL);

	
	ChainFreeHdl(pGetAll, NULL);
	VERIFY_QUEUE_STATE(NULL, 0, 0, 0)
	PrintPoolInfo("release the handles:\n");
}



TEST_F(BufQTest, Case19_test_ReserveN)
{
	ColorPrintf(TC_Red, "test \"ReserveN()\"(RequestLen < bufQ_DataLen)...\n");

	BufHandle *pHdls[5], *pRsv, *pRtn, *pGetAll;
	int nRequest;
	CBufQueue_test bufQ;

	
	APPEND_ALL(5)

	
	VERIFY_QUEUE_STATE(pHdls[0], 5, 50, 0)
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-5, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl-5));

	nRequest = 23;

	PrintPoolInfo("BufHdlPool before ReserveN(%d):\n", nRequest);

	ColorPrintf(TC_Magenta, "before ReserveN(%d):\n", nRequest);
	PrintHdlChain(pHdls[0], NULL);

	pRtn = bufQ.ReserveN(&nRequest, &pRsv);	

	
	ASSERT(23 == nRequest);
	ASSERT(23 == GetTotalDataLen(pRtn, pRsv));
	VERIFY_QUEUE_STATE(pRtn, 6, 50, 23)
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-5, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl-6));

	ColorPrintf(TC_Magenta, "the reserved buffers:\n");
	PrintHdlChain(pRtn, pRsv);

	ColorPrintf(TC_Magenta, "the rest buffers:\n");
	PrintHdlChain(pRsv, NULL);

	ColorPrintf(TC_Magenta, "the queue after ReserveN(%d):\n", nRequest);
	pGetAll = bufQ.GetAll();
	PrintHdlChain(pGetAll, NULL);

	PrintPoolInfo("BufHdlPool after ReserveN(%d):\n", nRequest);

	
	ChainFreeHdl(pGetAll, NULL);
	VERIFY_QUEUE_STATE(NULL, 0, 0, 0)
	PrintPoolInfo("release the handles:\n");
}



TEST_F(BufQTest, Case20_test_ReserveN)
{
	ColorPrintf(TC_Red, "test \"ReserveN()\"(RequestLen < bufQ_DataLen && RequestLen is at boundary)...\n");

	BufHandle *pHdls[5], *pRsv, *pRtn, *pGetAll;
	int nRequest;
	CBufQueue_test bufQ;

	
	APPEND_ALL(5)

	
	VERIFY_QUEUE_STATE(pHdls[0], 5, 50, 0)
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-5, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl-5));
	
	nRequest = 20;

	PrintPoolInfo("BufHdlPool before ReserveN(%d):\n", nRequest);

	ColorPrintf(TC_Magenta, "before ReserveN(%d):\n", nRequest);
	PrintHdlChain(pHdls[0], NULL);

	pRtn = bufQ.ReserveN(&nRequest, &pRsv);	

	
	ASSERT(20 == nRequest);
	ASSERT(20 == GetTotalDataLen(pRtn, pRsv));
	VERIFY_QUEUE_STATE(pHdls[0], 5, 50, 20)
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-5, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl-5));

	ColorPrintf(TC_Magenta, "the reserved buffers:\n");
	PrintHdlChain(pRtn, pRsv);

	ColorPrintf(TC_Magenta, "the rest buffers:\n");
	PrintHdlChain(pRsv, NULL);

	ColorPrintf(TC_Magenta, "the queue after ReserveN(%d):\n", nRequest);
	pGetAll = bufQ.GetAll();
	PrintHdlChain(pGetAll, NULL);

	PrintPoolInfo("BufHdlPool after ReserveN(%d):\n", nRequest);

	
	ChainFreeHdl(pGetAll, NULL);
	VERIFY_QUEUE_STATE(NULL, 0, 0, 0)
	PrintPoolInfo("release the handles:\n");
}



TEST_F(BufQTest, Case21_test_ReserveN)
{
	ColorPrintf(TC_Red, "test \"ReserveN()\"(RequestLen = bufQ_DataLen)...\n");

	BufHandle *pHdls[5], *pRsv, *pRtn, *pGetAll;
	int nRequest;
	CBufQueue_test bufQ;

	
	APPEND_ALL(5)

	
	VERIFY_QUEUE_STATE(pHdls[0], 5, 50, 0)
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-5, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl-5));

	nRequest = 50;

	PrintPoolInfo("BufHdlPool before ReserveN(%d):\n", nRequest);

	ColorPrintf(TC_Magenta, "before ReserveN(%d):\n", nRequest);
	PrintHdlChain(pHdls[0], NULL);

	pRtn = bufQ.ReserveN(&nRequest, &pRsv);	

	
	ASSERT(50 == nRequest);
	ASSERT(50 == GetTotalDataLen(pRtn, pRsv));
	VERIFY_QUEUE_STATE(pHdls[0], 5, 50, 50)
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-5, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl-5));

	ColorPrintf(TC_Magenta, "the reserved buffers:\n");
	PrintHdlChain(pRtn, pRsv);

	ColorPrintf(TC_Magenta, "the rest buffers:\n");
	PrintHdlChain(pRsv, NULL);

	ColorPrintf(TC_Magenta, "the queue after ReserveN(%d):\n", nRequest);
	pGetAll = bufQ.GetAll();
	PrintHdlChain(pGetAll, NULL);

	PrintPoolInfo("BufHdlPool after ReserveN(%d):\n", nRequest);

	
	ChainFreeHdl(pGetAll, NULL);
	VERIFY_QUEUE_STATE(NULL, 0, 0, 0)
	PrintPoolInfo("release the handles:\n");
}



TEST_F(BufQTest, Case22_test_ReserveN)
{
	ColorPrintf(TC_Red, "test \"ReserveN()\"(RequestLen < bufQ_DataLen)...\n");

	BufHandle *pHdls[5], *pRsv, *pRtn, *pGetAll;
	int nRequest, nReq;
	CBufQueue_test bufQ;

	
	APPEND_ALL(5)

	
	VERIFY_QUEUE_STATE(pHdls[0], 5, 50, 0)
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-5, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl-5));

	nRequest = 60;
	nReq = nRequest;

	PrintPoolInfo("BufHdlPool before ReserveN(%d):\n", nRequest);

	ColorPrintf(TC_Magenta, "before ReserveN(%d):\n", nRequest);
	PrintHdlChain(pHdls[0], NULL);

	pRtn = bufQ.ReserveN(&nReq, &pRsv);	

	
	ASSERT(50 == nReq);
	ASSERT(50 == GetTotalDataLen(pRtn, pRsv));
	VERIFY_QUEUE_STATE(pHdls[0], 5, 50, 50)
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-5, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl-5));

	ColorPrintf(TC_Magenta, "the reserved buffers:\n");
	PrintHdlChain(pRtn, pRsv);

	ColorPrintf(TC_Magenta, "the rest buffers:\n");
	PrintHdlChain(pRsv, NULL);

	ColorPrintf(TC_Magenta, "the queue after ReserveN(%d):\n", nRequest);
	pGetAll = bufQ.GetAll();
	PrintHdlChain(pGetAll, NULL);

	PrintPoolInfo("BufHdlPool after ReserveN(%d):\n", nRequest);

	
	ChainFreeHdl(pGetAll, NULL);
	VERIFY_QUEUE_STATE(NULL, 0, 0, 0)
	PrintPoolInfo("release the handles:\n");
}



TEST_F(BufQTest, Case23_test_ReserveAll)
{
	ColorPrintf(TC_Red, "test \"ReserveAll()\"(nMaxBuf < bufQ_BufNum)...\n");

	BufHandle *pHdls[5], *pRsv, *pRtn, *pGetAll;
	int nRsvLen, nMaxBuf;
	CBufQueue_test bufQ;

	
	APPEND_ALL(5)

	
	VERIFY_QUEUE_STATE(pHdls[0], 5, 50, 0)
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-5, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl-5));
	
	nMaxBuf = 3;

	PrintPoolInfo("BufHdlPool before ReserveAll(%d):\n", nMaxBuf);

	ColorPrintf(TC_Magenta, "before ReserveAll(%d):\n", nMaxBuf);
	PrintHdlChain(pHdls[0], NULL);

	pRtn = bufQ.ReserveAll(nMaxBuf, &nRsvLen, &pRsv);	

	
	ASSERT(30 == nRsvLen);
	ASSERT(30 == GetTotalDataLen(pRtn, pRsv));
	VERIFY_QUEUE_STATE(pHdls[0], 5, 50, 30)
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-5, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl-5));

	ColorPrintf(TC_Magenta, "the reserved buffers:\n");
	PrintHdlChain(pRtn, pRsv);

	ColorPrintf(TC_Magenta, "the rest buffers:\n");
	PrintHdlChain(pRsv, NULL);

	ColorPrintf(TC_Magenta, "the queue after ReserveAll(%d):\n", nMaxBuf);
	pGetAll = bufQ.GetAll();
	PrintHdlChain(pGetAll, NULL);

	PrintPoolInfo("BufHdlPool after ReserveAll(%d):\n", nMaxBuf);

	
	ChainFreeHdl(pGetAll, NULL);
	VERIFY_QUEUE_STATE(NULL, 0, 0, 0)
	PrintPoolInfo("release the handles:\n");
}



TEST_F(BufQTest, Case24_test_ReserveAll)
{
	ColorPrintf(TC_Red, "test \"ReserveAll()\"(nMaxBuf = bufQ_BufNum)...\n");

	BufHandle *pHdls[5], *pRsv, *pRtn, *pGetAll;
	int nRsvLen, nMaxBuf;
	CBufQueue_test bufQ;

	
	APPEND_ALL(5)

	
	VERIFY_QUEUE_STATE(pHdls[0], 5, 50, 0)
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-5, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl-5));
	
	nMaxBuf = 5;

	PrintPoolInfo("BufHdlPool before ReserveAll(%d):\n", nMaxBuf);

	ColorPrintf(TC_Magenta, "before ReserveAll(%d):\n", nMaxBuf);
	PrintHdlChain(pHdls[0], NULL);

	pRtn = bufQ.ReserveAll(nMaxBuf, &nRsvLen, &pRsv);	

	
	ASSERT(50 == nRsvLen);
	ASSERT(50 == GetTotalDataLen(pRtn, pRsv));
	VERIFY_QUEUE_STATE(pHdls[0], 5, 50, 50)
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-5, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl-5));

	ColorPrintf(TC_Magenta, "the reserved buffers:\n");
	PrintHdlChain(pRtn, pRsv);

	ColorPrintf(TC_Magenta, "the rest buffers:\n");
	PrintHdlChain(pRsv, NULL);

	ColorPrintf(TC_Magenta, "the queue after ReserveAll(%d):\n", nMaxBuf);
	pGetAll = bufQ.GetAll();
	PrintHdlChain(pGetAll, NULL);

	PrintPoolInfo("BufHdlPool after ReserveAll(%d):\n", nMaxBuf);

	
	ChainFreeHdl(pGetAll, NULL);
	VERIFY_QUEUE_STATE(NULL, 0, 0, 0)
	PrintPoolInfo("release the handles:\n");
}



TEST_F(BufQTest, Case25_test_ReserveAll)
{
	ColorPrintf(TC_Red, "test \"ReserveAll()\"(nMaxBuf < bufQ_BufNum)...\n");

	BufHandle *pHdls[5], *pRsv, *pRtn, *pGetAll;
	int nRsvLen, nMaxBuf;
	CBufQueue_test bufQ;

	
	APPEND_ALL(5)

	
	VERIFY_QUEUE_STATE(pHdls[0], 5, 50, 0)
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-5, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl-5));

	nMaxBuf = 8;

	PrintPoolInfo("BufHdlPool before ReserveAll(%d):\n", nMaxBuf);

	ColorPrintf(TC_Magenta, "before ReserveAll(%d):\n", nMaxBuf);
	PrintHdlChain(pHdls[0], NULL);

	pRtn = bufQ.ReserveAll(nMaxBuf, &nRsvLen, &pRsv);	

	
	ASSERT(50 == nRsvLen);
	ASSERT(50 == GetTotalDataLen(pRtn, pRsv));
	VERIFY_QUEUE_STATE(pHdls[0], 5, 50, 50)
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-5, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl-5));

	ColorPrintf(TC_Magenta, "the reserved buffers:\n");
	PrintHdlChain(pRtn, pRsv);

	ColorPrintf(TC_Magenta, "the rest buffers:\n");
	PrintHdlChain(pRsv, NULL);

	ColorPrintf(TC_Magenta, "the queue after ReserveAll(%d):\n", nMaxBuf);
	pGetAll = bufQ.GetAll();
	PrintHdlChain(pGetAll, NULL);

	PrintPoolInfo("BufHdlPool after ReserveAll(%d):\n", nMaxBuf);

	
	ChainFreeHdl(pGetAll, NULL);
	VERIFY_QUEUE_STATE(NULL, 0, 0, 0)
	PrintPoolInfo("release the handles:\n");
}


TEST_F(BufQTest, Case26_test_Commit)
{
	ColorPrintf(TC_Red, "test \"Commit(nCommit, true)\"(nCommit = nReserved)...\n");

	BufHandle *pHdls[5], *pRtn, *pRsv, *pGetAll;
	int nReserved, nCommit;
	CBufQueue_test bufQ;

	
	APPEND_ALL(5)

	
	VERIFY_QUEUE_STATE(pHdls[0], 5, 50, 0)
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-5, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl-5));
	
	nReserved = 30;
	nCommit = nReserved;

	ColorPrintf(TC_Magenta, "before ReserveN(%d):\n", nReserved);
	PrintHdlChain(pHdls[0], NULL);

	pRtn = bufQ.ReserveN(&nReserved, &pRsv);	

	
	ASSERT(30 == nReserved);
	ASSERT(30 == GetTotalDataLen(pRtn, pRsv));
	VERIFY_QUEUE_STATE(pHdls[0], 5, 50, 30)
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-5, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl-5));
	
	ColorPrintf(TC_Magenta, "the reserved buffers:\n");
	PrintHdlChain(pRtn, pRsv);

	ColorPrintf(TC_Magenta, "the rest buffers:\n");
	PrintHdlChain(pRsv, NULL);

	ColorPrintf(TC_Magenta, "the queue after ReserveN(%d):\n", nReserved);
	PrintHdlChain(pRtn, NULL);

	PrintPoolInfo("BufHdlPool after ReserveN(%d):\n", nReserved);

	bufQ.Commit(nCommit, true);

	
	VERIFY_QUEUE_STATE(pHdls[3], 2, 20, 0)
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-2, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl-2));

	pGetAll = bufQ.GetAll();	
	ColorPrintf(TC_Magenta, "the queue after Commit(%d, true):\n", nCommit);
	PrintHdlChain(pGetAll, NULL);
	PrintPoolInfo("BufHdlPool after Commit(%d, true):\n", nCommit);

	
	ChainFreeHdl(pGetAll, NULL);
	VERIFY_QUEUE_STATE(NULL, 0, 0, 0)
	PrintPoolInfo("release the handles:\n");
}



TEST_F(BufQTest, Case27_test_Commit)
{
	ColorPrintf(TC_Red, "test \"Commit(nCommit, false)\"(nCommit = nReserved)...\n");

	BufHandle *pHdls[5], *pRtn, *pRsv, *pGetAll;
	int nReserved, nCommit;
	CBufQueue_test bufQ;

	
	APPEND_ALL(5)

	
	VERIFY_QUEUE_STATE(pHdls[0], 5, 50, 0)
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-5, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl-5));
	
	nReserved = 30;
	nCommit = nReserved;

	ColorPrintf(TC_Magenta, "before ReserveN(%d):\n", nReserved);
	PrintHdlChain(pHdls[0], NULL);

	pRtn = bufQ.ReserveN(&nReserved, &pRsv);	

	
	ASSERT(30 == nReserved);
	ASSERT(30 == GetTotalDataLen(pRtn, pRsv));
	VERIFY_QUEUE_STATE(pHdls[0], 5, 50, 30)
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-5, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl-5));

	ColorPrintf(TC_Magenta, "the reserved buffers:\n");
	PrintHdlChain(pRtn, pRsv);

	ColorPrintf(TC_Magenta, "the rest buffers:\n");
	PrintHdlChain(pRsv, NULL);

	ColorPrintf(TC_Magenta, "the queue after ReserveN(%d):\n", nReserved);
	PrintHdlChain(pRtn, NULL);

	PrintPoolInfo("BufHdlPool after ReserveN(%d):\n", nReserved);

	bufQ.Commit(nCommit, false);

	
	VERIFY_QUEUE_STATE(pHdls[3], 2, 20, 0)
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-5, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl-5));

	pGetAll = bufQ.GetAll();	
	ColorPrintf(TC_Magenta, "the queue after Commit(%d, false):\n", nCommit);
	PrintHdlChain(pGetAll, NULL);

	ColorPrintf(TC_Magenta, "the committed buffers:\n");
	PrintHdlChain(pRtn, pGetAll);

	
	ChainFreeHdl(pRtn, pGetAll);
	ChainFreeHdl(pGetAll, NULL);
	VERIFY_QUEUE_STATE(NULL, 0, 0, 0)
	PrintPoolInfo("release the handles:\n");
}



TEST_F(BufQTest, Case28_test_Commit)
{
	ColorPrintf(TC_Red, "test \"Commit(nCommit, false)\"(nCommit < nReserved)...\n");

	BufHandle *pHdls[5], *pRtn, *pRsv, *pGetAll;
	int nReserved, nCommit;
	CBufQueue_test bufQ;

	
	APPEND_ALL(5)

	
	VERIFY_QUEUE_STATE(pHdls[0], 5, 50, 0)
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-5, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl-5));

	nReserved = 32;
	nCommit = nReserved - 5;

	ColorPrintf(TC_Magenta, "before ReserveN(%d):\n", nReserved);
	PrintHdlChain(pHdls[0], NULL);

	pRtn = bufQ.ReserveN(&nReserved, &pRsv);	

	
	ASSERT(32 == nReserved);
	ASSERT(32 == GetTotalDataLen(pRtn, pRsv));
	VERIFY_QUEUE_STATE(pHdls[0], 6, 50, 32)
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-5, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl-6));

	ColorPrintf(TC_Magenta, "the reserved buffers:\n");
	PrintHdlChain(pRtn, pRsv);

	ColorPrintf(TC_Magenta, "the rest buffers:\n");
	PrintHdlChain(pRsv, NULL);

	ColorPrintf(TC_Magenta, "the queue after ReserveN(%d):\n", nReserved);
	PrintHdlChain(pRtn, NULL);

	PrintPoolInfo("BufHdlPool after ReserveN(%d):\n", nReserved);

	bufQ.Commit(nCommit, false);

	
	VERIFY_QUEUE_STATE(bufQ.GetFirstHdl(), 4, 23, 5)
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-5, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl-7));

	pGetAll = bufQ.GetAll();	
	ColorPrintf(TC_Magenta, "the queue after Commit(%d, false):\n", nCommit);
	PrintHdlChain(pGetAll, NULL);

	ColorPrintf(TC_Magenta, "the committed buffers:\n");
	PrintHdlChain(pRtn, pGetAll);

	
	ChainFreeHdl(pRtn, pGetAll);
	ChainFreeHdl(pGetAll, NULL);
	VERIFY_QUEUE_STATE(NULL, 0, 0, 0)
	PrintPoolInfo("release the handles:\n");
}



TEST_F(BufQTest, Case29_test_Commit)
{
	ColorPrintf(TC_Red, "test \"Commit(nCommit, true)\"(nCommit < nReserved)...\n");

	BufHandle *pHdls[5], *pRtn, *pRsv, *pGetAll;
	int nReserved, nCommit;
	CBufQueue_test bufQ;

	
	APPEND_ALL(5)

	
	VERIFY_QUEUE_STATE(pHdls[0], 5, 50, 0)
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-5, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl-5));

	nReserved = 36;
	nCommit = nReserved - 3;

	ColorPrintf(TC_Magenta, "before ReserveN(%d):\n", nReserved);
	PrintHdlChain(pHdls[0], NULL);

	pRtn = bufQ.ReserveN(&nReserved, &pRsv);	

	
	ASSERT(36 == nReserved);
	ASSERT(36 == GetTotalDataLen(pRtn, pRsv));
	VERIFY_QUEUE_STATE(pHdls[0], 6, 50, 36)
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-5, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl-6));

	ColorPrintf(TC_Magenta, "the reserved buffers:\n");
	PrintHdlChain(pRtn, pRsv);

	ColorPrintf(TC_Magenta, "the rest buffers:\n");
	PrintHdlChain(pRsv, NULL);

	ColorPrintf(TC_Magenta, "the queue after ReserveN(%d):\n", nReserved);
	PrintHdlChain(pRtn, NULL);

	PrintPoolInfo("BufHdlPool after ReserveN(%d):\n", nReserved);

	bufQ.Commit(nCommit, true);

	
	VERIFY_QUEUE_STATE(bufQ.GetFirstHdl(), 3, 17, 3)
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-2, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl-3));

	pGetAll = bufQ.GetAll();	
	ColorPrintf(TC_Magenta, "the queue after Commit(%d, true):\n", nCommit);
	PrintHdlChain(pGetAll, NULL);
	PrintPoolInfo("BufHdlPool after Commit(%d, true):\n", nCommit);

	
	ChainFreeHdl(pGetAll, NULL);
	VERIFY_QUEUE_STATE(NULL, 0, 0, 0)
	PrintPoolInfo("release the handles:\n");
}



TEST_F(BufQTest, Case30_test_EmptyQ)
{
	ColorPrintf(TC_Red, "test operations on EmptyQ...\n");

	BufHandle *pRsv;
	int nReserved, nRequest;
	const int nBufSize = 50;
	char buf[nBufSize];

	CBufQueue_test bufQ;
	
	VERIFY_QUEUE_STATE(NULL, 0, 0, 0)

	
	printf("test GetTotalLen()...\n");
	ASSERT(0 == bufQ.GetTotalLen());
		
	
	printf("test \"BufHandle* GetN()\"...\n");
	nRequest = 10;
	ASSERT(NULL == bufQ.GetN(&nRequest));
	ASSERT(0 == nRequest);

	
	printf("test \"int GetN()\"...\n");
	nRequest = nBufSize;
	ASSERT(0 == bufQ.GetN(nRequest, buf));

	
	printf("test \"BufHandle* GetAll()\"...\n");
	ASSERT(NULL == bufQ.GetAll());
	
	
	printf("test \"int PeekN()\"...\n");
	nRequest = nBufSize;
	ASSERT(0 == bufQ.PeekN(nRequest, buf));

	
	printf("test \"int GetToZero()\"...\n");
	ASSERT(0 == bufQ.GetToZero(buf, nBufSize));

	
	printf("test \"BufHandle* ReserveN()\"...\n");
	nReserved = 10;
	ASSERT(NULL == bufQ.ReserveN(&nReserved, &pRsv));
	ASSERT(NULL == pRsv);

	
	printf("test \"BufHandle* ReserveAll()\"...\n");
	nReserved = 10;
	ASSERT(NULL == bufQ.ReserveAll(1, &nReserved, &pRsv));
	ASSERT(NULL == pRsv);

	
	printf("test \"void Commit()\"...\n");
	SHOULD_ABORT(bufQ.Commit(0, false));
	SHOULD_ABORT(bufQ.Commit(0, true));
	SHOULD_ABORT(bufQ.Commit(1, false));
	SHOULD_ABORT(bufQ.Commit(1, true));

	
	VERIFY_QUEUE_STATE(NULL, 0, 0, 0)
	PrintPoolInfo("release the handles:\n");
}

#undef APPEND_ALL
#undef VERIFY_QUEUE_STATE

static void CreateHdlChain(BufHandle** pHdl, int nChainLen, bool bReset)
{
	BufHandle** pTmp = pHdl; 
	char tmp;

	if(bReset == true)
		s_c = '0';
		
	s_c += nChainLen;
	tmp = s_c-1;
	
	for(int i = nChainLen-1; i >= 0; --i) {
		if(i == nChainLen-1)
			pTmp[i] = AllocateHdl(true, NULL, 0, NULL);
		else {
			pTmp[i] = AllocateHdl(true, NULL, 0, pTmp[i+1]);
		}

		ASSERT(NULL != pTmp[i]);

		memset(pTmp[i]->pBuf, tmp--, s_nHdlDataLen);
		pTmp[i]->nDataLen = s_nHdlDataLen;
	}
}

static void PrintHdlChain(BufHandle* pHdl, BufHandle* pNext)
{
	printf("--------------------------------------------\n");
	if(pHdl == NULL){
		printf("the chain is empty(datalen = 0).\n");
		return;
	}
	BufHandle *pTmp;
	printf("the handles chain(datalen = %d):\n", GetTotalDataLen(pHdl, pNext));
	for(pTmp = pHdl; pTmp != pNext; pTmp = pTmp->_next)
		PrintHdlContent2(pTmp);
}

static void PrintBuf(char* pBuf, int nBufSize)
{
	printf("--------------------------------------------\n");
	if(pBuf == NULL) {
		printf("the buffer is empty(datalen = 0).\n");
		return;
	}

	char* p = pBuf;
	int i = 0;
	for(i = 0; i<nBufSize && *p; i++)
		printf("%c", *p++);

	printf(" (datalen = %d).\n", i);
}

static void PrintHdlContent2(BufHandle* pHdl)
{
	printf("[ BufLen = %-4d, DataLen = %-2d | \"", pHdl->nBufLen, pHdl->nDataLen);
	for(int i = 0; i < pHdl->nDataLen; ++i)
		printf("%c", pHdl->pBuf[i]? pHdl->pBuf[i]:' ');
	for(int i = 0; i < s_nHdlDataLen - pHdl->nDataLen; ++i)
		printf(" ");
	printf("\" ]\n");
}

