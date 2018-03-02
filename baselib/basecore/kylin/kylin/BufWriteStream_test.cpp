#include "stdafx.h"
#include "SyncObj.h"
#include "BufStream.h"
#include "UnitTest.h"
#include "../test/KylinUtils4Test.h"

static const int s_nTranBuf = 20; 
static const int s_nBigTranBuf = 20; 
static const int s_nBufSize = 1024; 
static const int s_nBufHdl = 1024;  

class CBufWriteStream_test: plclic CBufWriteStream
{
plclic:

	CBufWriteStream_test(BufHandle* pBegin=NULL, BufHandle* pNext=NULL):
		CBufWriteStream(pBegin, pNext) {}

	BufHandle* GetGuardHdl() const { return m_pGuard; }

	BufHandle* GetCurHdl() const { return m_pCur; }

	BufHandle* GetBeginHdl() const { return m_pBegin; }

	int GetFreeLen() const { return m_nLen; }

	int GetBufHdlNum() {
		int n = 0; 
		BufHandle* pTmp = NULL;

		if(NULL == m_pBegin)
			return 0;
		else {
			for(pTmp = m_pBegin; pTmp != m_pGuard; pTmp = pTmp->_next, ++n);
			return n;
		}
	}
	
	int GetDataLen() {
		ASSERT(NULL != m_pBegin);
		int n = 0;

		if(m_pBegin == m_pGuard)
			return n;

		BufHandle* pHdl = m_pBegin;
		for(; pHdl != m_pCur; pHdl = pHdl->_next)
			n += pHdl->nDataLen;
		n += pHdl->nDataLen;

		return n;
	}

};


class BufWriteStreamTest {
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

bool BufWriteStreamTest::m_bInit = false;

static void PrintBufWriteStream(CBufWriteStream_test *pBWS, const char* p=NULL);



#define VERIFY_BWS_STATE(pBWS, pBegin, pGuard, pCur, nFreeLen, nDataLen) \
    ASSERT(pBegin == pBWS->GetBeginHdl());        \
    ASSERT(pGuard == pBWS->GetGuardHdl());     \
    ASSERT(pCur == pBWS->GetCurHdl());         \
    ASSERT(nFreeLen == pBWS->GetFreeLen());    \
    ASSERT(nDataLen == pBWS->GetDataLen());


TEST_F(BufWriteStreamTest, Case1_test_constructor)
{
    ColorPrintf(TC_Red, "test constructing CBufWriteStream with no data...\n");

    
    ColorPrintf(TC_Magenta, "constructed with (NULL, NULL)...\n");
    CBufWriteStream_test* pBWS = new CBufWriteStream_test();
    PrintBufWriteStream(pBWS);
    
    
    VERIFY_BWS_STATE(pBWS, pBWS->GetBeginHdl(), NULL, pBWS->GetCurHdl(), s_nBufSize, 0)

    
    delete pBWS;
}


TEST_F(BufWriteStreamTest, Case2_test_constructor)
{
    ColorPrintf(TC_Red, "test constructing CBufWriteStream with data...\n");

    
    BufHandle* pHdls[3];
    char* pBufs[3] = {"user content 0", "user content 1", "user content 2"};
    int nDataLen = 0;

    
    CreateHdlChain(pHdls, 3);
    for(int i = 0; i < 3; ++i) 
        FillBufHdlContent(pHdls[i], pBufs[i], strlen(pBufs[i]), true); 
    PrintHdlChain("handle chain for constructing BWS", 0, pHdls[0], NULL);

    
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-3, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl-3));

    
    ColorPrintf(TC_Magenta, "constructed with (pHdls[0], NULL)...\n");
    CBufWriteStream_test* pBWS = new CBufWriteStream_test(pHdls[0], NULL);
    PrintBufWriteStream(pBWS);
    
    
    nDataLen = GetTotalDataLen(pHdls[0], NULL);
    VERIFY_BWS_STATE(pBWS, pHdls[0], NULL, pHdls[2], (s_nBufSize-(int)strlen(pBufs[2])-1), nDataLen)

    
    delete pBWS;
}



TEST_F(BufWriteStreamTest, Case3_test_constructor)
{
    ColorPrintf(TC_Red, "test constructing CBufWriteStream with data...\n");

    
    BufHandle* pHdls[3];
    char* pBufs[3] = {"user content 0", "user content 1", "user content 2"};
    int nDataLen = 0;

    
    CreateHdlChain(pHdls, 3);
    for(int i = 0; i < 3; ++i) 
        FillBufHdlContent(pHdls[i], pBufs[i], strlen(pBufs[i]), true); 
    PrintHdlChain("handle chain for constructing BWS", 0, pHdls[0], NULL);

    
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-3, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl-3));

    
    ColorPrintf(TC_Magenta, "constructing with [pHdls[0], pHdls[2])...\n");
    CBufWriteStream_test* pBWS = new CBufWriteStream_test(pHdls[0], pHdls[2]);
    PrintBufWriteStream(pBWS);
    
    
    nDataLen = GetTotalDataLen(pHdls[0], pHdls[2]);
    VERIFY_BWS_STATE(pBWS, pHdls[0], pHdls[2], pHdls[1], (s_nBufSize-(int)strlen(pBufs[1])-1), nDataLen)

    
    delete pBWS;
    ChainFreeHdl(pHdls[2], NULL);
}



TEST_F(BufWriteStreamTest, Case4_test_PutBuffer)
{
    ColorPrintf(TC_Red, "test PutBuffer(char*, int), BWS has no data && WriteLen < BWSFreeLen...\n");

    
    char* pBuf = "test case 4 data";
    int nLen = strlen(pBuf);
    BufHandle* p;

    
    CBufWriteStream_test* pBWS = new CBufWriteStream_test();
    PrintBufWriteStream(pBWS, "before PutBuffer()");
    
    
    p = pBWS->GetBeginHdl();
    VERIFY_BWS_STATE(pBWS, p, NULL, p, s_nBufSize, 0)

    
    ColorPrintf(TC_Magenta, "put buffer(nLen=%d)...\n", nLen);
    pBWS->PutBuffer(pBuf, nLen);
    PrintBufWriteStream(pBWS, "after PutBuffer()");

    
    VERIFY_BWS_STATE(pBWS, p, NULL, p, s_nBufSize-nLen, nLen)

    
    delete pBWS;
}



TEST_F(BufWriteStreamTest, Case5_test_PutBuffer)
{
    ColorPrintf(TC_Red, "test PutBuffer(char*, int), BWS has no data && WriteLen > BWSFreeLen...\n");

    
    int nLen = s_nBufSize+1;
    char pBuf[nLen];
    memcpy(pBuf, "test case 5 data", 16);
    BufHandle* p;

    
    CBufWriteStream_test* pBWS = new CBufWriteStream_test();
    PrintBufWriteStream(pBWS, "before PutBuffer()");
    
    
    p = pBWS->GetBeginHdl();
    VERIFY_BWS_STATE(pBWS, p, NULL, p, s_nBufSize, 0)

    
    ColorPrintf(TC_Magenta, "put buffer(nLen=%d)...\n", nLen);
    pBWS->PutBuffer(pBuf, nLen);
    PrintBufWriteStream(pBWS, "after PutBuffer()");

    
    VERIFY_BWS_STATE(pBWS, p, NULL, p->_next, s_nBufSize-1, s_nBufSize+1)

    
    delete pBWS;
}



TEST_F(BufWriteStreamTest, Case6_test_PutBuffer)
{
    ColorPrintf(TC_Red, "test PutBuffer(char*, int), BWS has no data && WriteLen = s_nBufSize...\n");

    
    int nLen = s_nBufSize;
    char pBuf[nLen];
    memcpy(pBuf, "test case 6 data", 16);
    BufHandle* p;

    
    CBufWriteStream_test* pBWS = new CBufWriteStream_test();
    PrintBufWriteStream(pBWS, "before PutBuffer()");
    
    
    p = pBWS->GetBeginHdl();
    VERIFY_BWS_STATE(pBWS, p, NULL, p, s_nBufSize, 0)

    
    ColorPrintf(TC_Magenta, "put buffer(nLen=%d)...\n", nLen);
    pBWS->PutBuffer(pBuf, nLen);
    PrintBufWriteStream(pBWS, "after PutBuffer()");

    
    VERIFY_BWS_STATE(pBWS, p, NULL, p, 0, s_nBufSize)

    
    ColorPrintf(TC_Magenta, "put the same buffer again...\n");
    pBWS->PutBuffer(pBuf, nLen);
    PrintBufWriteStream(pBWS, "after put the same buffer twice");

    
    VERIFY_BWS_STATE(pBWS, p, NULL, p->_next, 0, s_nBufSize*2)

    
    delete pBWS;
}



TEST_F(BufWriteStreamTest, Case7_test_PutBuffer)
{
    ColorPrintf(TC_Red, "test PutBuffer(char*, int), BWS has data && WriteLen < BWSFreeLen...\n");

    
    BufHandle* pHdl = AllocateHdl(true, NULL, 0, NULL);
    char* pBuf = "test case 7 data";
    int nLen = strlen(pBuf);
    int nPreOccupy = 10;


    
    CBufWriteStream_test* pBWS = new CBufWriteStream_test(pHdl, NULL);
    PrintBufWriteStream(pBWS, "before PutBuffer()");
    
    
    VERIFY_BWS_STATE(pBWS, pHdl, NULL, pHdl, s_nBufSize-nPreOccupy, nPreOccupy)

    
    ColorPrintf(TC_Magenta, "put buffer(nLen=%d)...\n", nLen);
    pBWS->PutBuffer(pBuf, nLen);
    PrintBufWriteStream(pBWS, "after PutBuffer()");

    
    VERIFY_BWS_STATE(pBWS, pHdl, NULL, pHdl, s_nBufSize-nPreOccupy-nLen, nPreOccupy+nLen)

    
    delete pBWS;
}



TEST_F(BufWriteStreamTest, Case8_test_PutBuffer)
{
    ColorPrintf(TC_Red, "test PutBuffer(char*, int), BWS has data && WriteLen > BWSFreeLen...\n");

    
    BufHandle* pHdl = AllocateHdl(true, NULL, 0, NULL);
    char* pBuf = " test case 8 data";
    int nLen = strlen(pBuf);
    int nPreOccupy = s_nBufSize-1;


    
    CBufWriteStream_test* pBWS = new CBufWriteStream_test(pHdl, NULL);
    PrintBufWriteStream(pBWS, "before PutBuffer()");
    
    
    VERIFY_BWS_STATE(pBWS, pHdl, NULL, pHdl, 1, nPreOccupy)

    
    ColorPrintf(TC_Magenta, "put buffer(nLen=%d)...\n", nLen);
    pBWS->PutBuffer(pBuf, nLen);
    PrintBufWriteStream(pBWS, "after PutBuffer()");

    
    VERIFY_BWS_STATE(pBWS, pHdl, NULL, pHdl->_next, s_nBufSize-nLen+1, nPreOccupy+nLen)

    
    delete pBWS;
}



TEST_F(BufWriteStreamTest, Case9_test_PutBuffer)
{
    ColorPrintf(TC_Red, "test PutBuffer(char*, int), BWS has data && WriteLen = BWSFreeLen...\n");

    
    BufHandle* pHdl = AllocateHdl(true, NULL, 0, NULL);
    char* pBuf = " test case 8 data";
    int nLen = strlen(pBuf);
    int nPreOccupy = s_nBufSize-nLen;


    
    CBufWriteStream_test* pBWS = new CBufWriteStream_test(pHdl, NULL);
    PrintBufWriteStream(pBWS, "before PutBuffer()");
    
    
    VERIFY_BWS_STATE(pBWS, pHdl, NULL, pHdl, nLen, nPreOccupy)

    
    ColorPrintf(TC_Magenta, "put buffer(nLen=%d)...\n", nLen);
    pBWS->PutBuffer(pBuf, nLen);
    PrintBufWriteStream(pBWS, "after PutBuffer()");

    
    VERIFY_BWS_STATE(pBWS, pHdl, NULL, pHdl, 0, s_nBufSize)

    
    delete pBWS;
}



TEST_F(BufWriteStreamTest, Case10_test_BackSeek)
{
    ColorPrintf(TC_Red, "test BackSeek(nLen=0) when BWS has data...\n");

    
    char* pBuf = "test case 10 data";
    int nBufLen = strlen(pBuf);
    int nRtn = 1;
    BufHandle* pHdl = AllocateHdl(true, pBuf, nBufLen, NULL);

    
    CBufWriteStream_test* pBWS = new CBufWriteStream_test(pHdl, NULL);
    PrintBufWriteStream(pBWS, "before BackSeek()");
    
    
    VERIFY_BWS_STATE(pBWS, pHdl, NULL, pHdl, s_nBufSize-nBufLen, nBufLen)
    
    
    ColorPrintf(TC_Magenta, "BackSeek(-1) should abort...\n");
    SHOULD_ABORT(pBWS->BackSeek(-1));

    
    ColorPrintf(TC_Magenta, "BackSeek(0) when BWS has data...\n");
    nRtn = pBWS->BackSeek(0);
    PrintBufWriteStream(pBWS, "after BackSeek(0)");

    
    ASSERT(0 == nRtn);
    VERIFY_BWS_STATE(pBWS, pHdl, NULL, pHdl, s_nBufSize-nBufLen, nBufLen)

    
    delete pBWS;
}


TEST_F(BufWriteStreamTest, Case11_test_BackSeek)
{
    ColorPrintf(TC_Red, "test BackSeek(nLen=0) when BWS has no data...\n");

    
    int nRtn = 1;

    
    CBufWriteStream_test* pBWS = new CBufWriteStream_test();
    PrintBufWriteStream(pBWS, "before BackSeek()");
    
    
    BufHandle* pBegin = pBWS->GetBeginHdl();
    VERIFY_BWS_STATE(pBWS, pBegin, NULL, pBegin, s_nBufSize, 0)
    
    
    ColorPrintf(TC_Magenta, "BackSeek(0) when BWS has no data...\n");
    nRtn = pBWS->BackSeek(0);
    PrintBufWriteStream(pBWS, "after BackSeek(0)");

    
    ASSERT(0 == nRtn);
    VERIFY_BWS_STATE(pBWS, pBegin, NULL, pBegin, s_nBufSize, 0)

    
    delete pBWS;
}



TEST_F(BufWriteStreamTest, Case12_test_BackSeek)
{
    ColorPrintf(TC_Red, "test BackSeek(nLen>0) and m_pCur->nDataLen > nLen...\n");

    
    char* pBuf = "test case 12 data";
    int nBufLen = strlen(pBuf);
    int nRtn, nFreeLen, nDataLen;
    BufHandle* pHdl = AllocateHdl(true, pBuf, nBufLen, NULL);

    
    CBufWriteStream_test* pBWS = new CBufWriteStream_test(pHdl, NULL);
    PrintBufWriteStream(pBWS, "before BackSeek()");
    
    
    BufHandle* pBegin = pBWS->GetBeginHdl();
    nFreeLen = s_nBufSize - nBufLen;
    nDataLen = nBufLen;
    VERIFY_BWS_STATE(pBWS, pBegin, NULL, pBegin, nFreeLen, nDataLen)
    
    
    ColorPrintf(TC_Magenta, "BackSeek(1)...\n");
    nRtn = pBWS->BackSeek(1);
    PrintBufWriteStream(pBWS, "after BackSeek(1)");

    
    ASSERT(1 == nRtn);
    nFreeLen += 1;
    nDataLen -= 1;
    VERIFY_BWS_STATE(pBWS, pBegin, NULL, pBegin, nFreeLen, nDataLen)

    
    delete pBWS;
}



TEST_F(BufWriteStreamTest, Case13_test_BackSeek)
{
    ColorPrintf(TC_Red, "test BackSeek(nLen>0) and at boundary, but dose not reach m_pBegin...\n");

    
    int nRtn, nFreeLen, nDataLen;
    BufHandle* pHdls[5];

    
    CreateHdlChain(pHdls, 5);
    for(int i = 0; i < 5; ++i) 

    
    CBufWriteStream_test* pBWS = new CBufWriteStream_test(pHdls[0], NULL);
    PrintBufWriteStream(pBWS, "before BackSeek()");
    
    
    nFreeLen = s_nBufSize - 5;
    nDataLen = 25;
    VERIFY_BWS_STATE(pBWS, pHdls[0], NULL, pHdls[4], nFreeLen, nDataLen)
    
    
    ColorPrintf(TC_Magenta, "BackSeek(15)...\n");
    nRtn = pBWS->BackSeek(15);
    PrintBufWriteStream(pBWS, "after BackSeek(15)");

    
    ASSERT(15 == nRtn);
    nFreeLen = s_nBufSize - 5;
    nDataLen -= 15;
    VERIFY_BWS_STATE(pBWS, pHdls[0], NULL, pHdls[1], nFreeLen, nDataLen)

    
    delete pBWS;
}



TEST_F(BufWriteStreamTest, Case14_test_BackSeek)
{
    ColorPrintf(TC_Red, "test BackSeek(nLen>0) and nLen = AllBWSDataLen...\n");

    
    int nRtn, nFreeLen, nDataLen;
    BufHandle* pHdls[5];

    
    CreateHdlChain(pHdls, 5);
    for(int i = 0; i < 5; ++i) 

    
    CBufWriteStream_test* pBWS = new CBufWriteStream_test(pHdls[0], NULL);
    PrintBufWriteStream(pBWS, "before BackSeek()");
    
    
    nFreeLen = s_nBufSize - 5;
    nDataLen = 25;
    VERIFY_BWS_STATE(pBWS, pHdls[0], NULL, pHdls[4], nFreeLen, nDataLen)
    
    
    ColorPrintf(TC_Magenta, "BackSeek(25)...\n");
    nRtn = pBWS->BackSeek(25);
    PrintBufWriteStream(pBWS, "after BackSeek(25)");

    
    ASSERT(25 == nRtn);
    nFreeLen = s_nBufSize;
    nDataLen -= 25;
    VERIFY_BWS_STATE(pBWS, pHdls[0], NULL, pHdls[0], nFreeLen, nDataLen)

    
    delete pBWS;
}



TEST_F(BufWriteStreamTest, Case15_test_BackSeek)
{
    ColorPrintf(TC_Red, "test BackSeek(nLen>0) and nLen > AllBWSDataLen...\n");

    
    int nRtn, nFreeLen, nDataLen;
    BufHandle* pHdls[5];

    
    CreateHdlChain(pHdls, 5);
    for(int i = 0; i < 5; ++i) 

    
    CBufWriteStream_test* pBWS = new CBufWriteStream_test(pHdls[0], NULL);
    PrintBufWriteStream(pBWS, "before BackSeek()");
    
    
    nFreeLen = s_nBufSize - 5;
    nDataLen = 25;
    VERIFY_BWS_STATE(pBWS, pHdls[0], NULL, pHdls[4], nFreeLen, nDataLen)
    
    
    ColorPrintf(TC_Magenta, "BackSeek(26)...\n");
    nRtn = pBWS->BackSeek(26);
    PrintBufWriteStream(pBWS, "after BackSeek(26)");

    
    ASSERT(25 == nRtn);
    nFreeLen = s_nBufSize;
    nDataLen = 0;
    VERIFY_BWS_STATE(pBWS, pHdls[0], NULL, pHdls[0], nFreeLen, nDataLen)

    
    delete pBWS;
}



TEST_F(BufWriteStreamTest, Case16_test_Write)
{
    ColorPrintf(TC_Red, "test Write(nWriteLen>8192)...\n");

    
	BufHandle* pTmp;
    int nWriteLen, nFreeLen, nDataLen, nRtn, nMore;
	nMore = 16;
	nWriteLen = 8192 + nMore; 
	char* pUC = "user content#%d.\n";
	char pBuf[nWriteLen];

	
	memcpy((void*)(pBuf+nWriteLen-1-strlen(pUC)), (void*)pUC, strlen(pUC));
	pBuf[nWriteLen-1] = '\0';  

    
    CBufWriteStream_test* pBWS = new CBufWriteStream_test();
    PrintBufWriteStream(pBWS, "before Write()");
    
    
    nFreeLen = s_nBufSize;
    nDataLen = 0;
	pTmp = pBWS->GetBeginHdl();
    VERIFY_BWS_STATE(pBWS, pTmp, NULL, pTmp, nFreeLen, nDataLen)
    
    
    ColorPrintf(TC_Magenta, "Write(%d)...\n", strlen(pBuf)-1);
    nRtn = pBWS->Write(pBuf, 1);
    PrintBufWriteStream(pBWS, "after Write:");

    
	ASSERT(nWriteLen-2 == nRtn);  
    nFreeLen = s_nBufSize - ((nMore-2));
    nDataLen = strlen(pBuf)-1;
    VERIFY_BWS_STATE(pBWS, pBWS->GetBeginHdl(), NULL, pBWS->GetCurHdl(), nFreeLen, nDataLen)

    
    delete pBWS;
}



TEST_F(BufWriteStreamTest, Case17_test_BufferCat)
{
    ColorPrintf(TC_Red, "test BufferCat(pHdl, NULL)...\n");

    
	BufHandle *pHdls[4], *pTmp;
    int nFreeLen, nDataLen, nRtn, nCatLen;

	
    CreateHdlChain(pHdls, 4);
    FillBufHdlContent(pHdls[0], '0', 10); 
    FillBufHdlContent(pHdls[1], '1', s_nBufSize); 
    FillBufHdlContent(pHdls[3], '3', 20); 
	nCatLen = GetTotalDataLen(pHdls[0], NULL);
	PrintHdlChain("the handle chain will catenate", 0, pHdls[0], NULL);
		
    
    CBufWriteStream_test* pBWS = new CBufWriteStream_test();
    PrintBufWriteStream(pBWS, "before BufferCat()");
    
    
    nFreeLen = s_nBufSize;
    nDataLen = 0;
	pTmp = pBWS->GetBeginHdl();
    VERIFY_BWS_STATE(pBWS, pTmp, NULL, pTmp, nFreeLen, nDataLen)
    
    
    ColorPrintf(TC_Magenta, "BufferCat(pHdl[0], NULL)...\n");
    nRtn = pBWS->BufferCat(pHdls[0], NULL);
    PrintBufWriteStream(pBWS, "after BufferCat()");

    
	ASSERT(nCatLen == nRtn);
    nFreeLen = s_nBufSize - 20;
    nDataLen = nCatLen;
    VERIFY_BWS_STATE(pBWS, pBWS->GetBeginHdl(), NULL, pHdls[3], nFreeLen, nDataLen)
	ASSERT(5 == pBWS->GetBufHdlNum());

    
    delete pBWS;
}



TEST_F(BufWriteStreamTest, Case18_test_BufferCat)
{
    ColorPrintf(TC_Red, "test BufferCat(pHdl, pNext!=NULL)...\n");

    
	BufHandle *pHdls[4], *pTmp;
    int nFreeLen, nDataLen, nRtn, nCatLen, n;

	
    CreateHdlChain(pHdls, 4);
    FillBufHdlContent(pHdls[0], '0', 10); 
    FillBufHdlContent(pHdls[1], '1', s_nBufSize); 
    FillBufHdlContent(pHdls[3], '3', 20); 
	PrintHdlChain("the handle chain will catenate", 0, pHdls[0], NULL);
	
	nCatLen = GetTotalDataLen(pHdls[0], pHdls[3]);
		
    
    CBufWriteStream_test* pBWS = new CBufWriteStream_test();
	n = 'aaaa'; 
	pBWS->PutBuffer((char*)&n, 4);

    PrintBufWriteStream(pBWS, "before BufferCat()");
    
    
    nFreeLen = s_nBufSize - 4;
    nDataLen = 4;
	pTmp = pBWS->GetBeginHdl();
    VERIFY_BWS_STATE(pBWS, pTmp, NULL, pTmp, nFreeLen, nDataLen)
    
    
    ColorPrintf(TC_Magenta, "BufferCat(pHdl[0], pHdls[3])...\n");
    nRtn = pBWS->BufferCat(pHdls[0], pHdls[3]);
    PrintBufWriteStream(pBWS, "after BufferCat()");

    
	ASSERT(nCatLen == nRtn);
    nFreeLen = s_nBufSize;
    nDataLen = nCatLen + 4;
    VERIFY_BWS_STATE(pBWS, pBWS->GetBeginHdl(), NULL, pHdls[2], nFreeLen, nDataLen)
	ASSERT(4 == pBWS->GetBufHdlNum());

    
    delete pBWS;
	ChainFreeHdl(pHdls[3], NULL);
}



TEST_F(BufWriteStreamTest, Case19_test_GetBegin)
{
    ColorPrintf(TC_Red, "test GetBegin()...\n");

    
	BufHandle *pHdls[4], *pGetBegin, *pTmp;
	int nDataLen, nFreeLen;

	
    CreateHdlChain(pHdls, 4);
	for(int i = 0; i < 4; ++i)
	
    
    CBufWriteStream_test* pBWS = new CBufWriteStream_test();
    PrintBufWriteStream(pBWS, "before GetBegin()");
    
    
    nFreeLen = s_nBufSize;
    nDataLen = 0;
	pTmp = pBWS->GetBeginHdl();
    VERIFY_BWS_STATE(pBWS, pTmp, NULL, pTmp, nFreeLen, nDataLen)
    
    
    ColorPrintf(TC_Magenta, "GetBegin()...\n");
    pGetBegin = pBWS->GetBegin();
    PrintBufWriteStream(pBWS, "after GetBegin()");

    
	ASSERT(NULL == pGetBegin);
    nFreeLen = s_nBufSize;
    nDataLen = 0;
    VERIFY_BWS_STATE(pBWS, pTmp, NULL, pTmp, nFreeLen, nDataLen)

    
    ColorPrintf(TC_Magenta, "delete the old pBWS, construct a new one\n");
    delete pBWS;
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-4, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl-4));

	
	pBWS = new CBufWriteStream_test(pHdls[0], pHdls[3]);	
    PrintBufWriteStream(pBWS, "construct a new pBWS(pHdls[0], pHdls[3])");

    
    ColorPrintf(TC_Magenta, "GetBegin()...\n");
    pGetBegin = pBWS->GetBegin();

	
	ASSERT(pGetBegin == pHdls[0]);
    VERIFY_BWS_STATE(pBWS, pHdls[3], pHdls[3], pHdls[3], 0, 0)

    PrintBufWriteStream(pBWS, "after GetBegin()");

    
    delete pBWS;
	ChainFreeHdl(pGetBegin, NULL);
}



TEST_F(BufWriteStreamTest, Case20_test_Write)
{
    ColorPrintf(TC_Red, "test Write(nWriteLen < 8192)...\n");

    
	BufHandle* pTmp;
    int nWriteLen, nFreeLen, nDataLen, nRtn;
	char* pUC = "user content#%d, input char='%c'.\n";
	nWriteLen = strlen(pUC)-2;

    
    CBufWriteStream_test* pBWS = new CBufWriteStream_test();
    PrintBufWriteStream(pBWS, "before Write()");
    
    
    nFreeLen = s_nBufSize;
    nDataLen = 0;
	pTmp = pBWS->GetBeginHdl();
    VERIFY_BWS_STATE(pBWS, pTmp, NULL, pTmp, nFreeLen, nDataLen)
    
    
    ColorPrintf(TC_Magenta, "Write(%d)...\n", nWriteLen);
    nRtn = pBWS->Write(pUC, 2, 'w');
    PrintBufWriteStream(pBWS, "after Write:");

    
	ASSERT(nWriteLen == nRtn); 
    nFreeLen = s_nBufSize - nWriteLen % s_nBufSize;
    nDataLen = nWriteLen;
	ASSERT('\n' == *(pBWS->GetBeginHdl()->pBuf+nWriteLen-1));
    VERIFY_BWS_STATE(pBWS, pBWS->GetBeginHdl(), NULL, pBWS->GetCurHdl(), nFreeLen, nDataLen)

    
    delete pBWS;
}




TEST_F(BufWriteStreamTest, Case21_test_Write)
{
    ColorPrintf(TC_Red, "test Write(nWriteLen = 8192)...\n");

    
	BufHandle* pTmp;
    int nWriteLen, nFreeLen, nDataLen, nRtn, nBufSize;
	nWriteLen = 8192; 
	nBufSize = nWriteLen+1;
	char pBuf[nBufSize];

	
	pBuf[nBufSize-2] = '#';
	pBuf[nBufSize-1] = '\0';

    
    CBufWriteStream_test* pBWS = new CBufWriteStream_test();
    PrintBufWriteStream(pBWS, "before Write()");
    
    
    nFreeLen = s_nBufSize;
    nDataLen = 0;
	pTmp = pBWS->GetBeginHdl();
    VERIFY_BWS_STATE(pBWS, pTmp, NULL, pTmp, nFreeLen, nDataLen)
    
    
    ColorPrintf(TC_Magenta, "Write(%d)...\n", nWriteLen);
    nRtn = pBWS->Write(pBuf);
    PrintBufWriteStream(pBWS, "after Write:");

    
	ASSERT(nWriteLen == nRtn);
    nFreeLen = 0;
    nDataLen = nWriteLen;
	ASSERT(s_nBufSize == pBWS->GetCurHdl()->nDataLen);
	ASSERT('#' == *(pBWS->GetCurHdl()->pBuf+s_nBufSize-1));
    VERIFY_BWS_STATE(pBWS, pBWS->GetBeginHdl(), NULL, pBWS->GetCurHdl(), nFreeLen, nDataLen)

    
    delete pBWS;
}



TEST_F(BufWriteStreamTest, Case22_test_Write)
{
    ColorPrintf(TC_Red, "test Write(nWriteLen = s_nBufSize)...\n");

    
	BufHandle* pTmp;
    int nWriteLen, nFreeLen, nDataLen, nRtn;
	nWriteLen = s_nBufSize;
	int nBufSize = nWriteLen+1;
	char pBuf[nBufSize];

	
	pBuf[nBufSize-2] = '#';
	pBuf[nBufSize-1] = 0;

    
    CBufWriteStream_test* pBWS = new CBufWriteStream_test();
    PrintBufWriteStream(pBWS, "before Write()");
    
    
    nFreeLen = s_nBufSize;
    nDataLen = 0;
	pTmp = pBWS->GetBeginHdl();
    VERIFY_BWS_STATE(pBWS, pTmp, NULL, pTmp, nFreeLen, nDataLen)
    
    
    ColorPrintf(TC_Magenta, "Write(%d)...\n", nWriteLen);
    nRtn = pBWS->Write(pBuf);
    PrintBufWriteStream(pBWS, "after Write:");

    
	ASSERT(nWriteLen == nRtn); 
    nFreeLen = 0;
    nDataLen = nWriteLen;
	ASSERT(s_nBufSize == pBWS->GetCurHdl()->nDataLen);
	ASSERT('#' == *(pBWS->GetCurHdl()->pBuf+s_nBufSize-1));
    VERIFY_BWS_STATE(pBWS, pBWS->GetBeginHdl(), NULL, pBWS->GetCurHdl(), nFreeLen, nDataLen)

    
    delete pBWS;
}



TEST_F(BufWriteStreamTest, Case23_test_Write)
{
    ColorPrintf(TC_Red, "a bit more complex case to test Write()...\n");

    
	BufHandle* pTmp; 
	int nDataLen, nFreeLen, nRtn;
	int nWriteLen1, nWriteLen2, nBufLen2, nWriteLen3, nBufLen3;
	char* pUC = "user content#%d.\n"; 
	nWriteLen1 = strlen(pUC)-1;	
	nWriteLen2 = 8192 + 10; 
	nBufLen2 = nWriteLen2 + 2;
	nWriteLen3 = 8192*2 - nWriteLen1 - nWriteLen2;
	nBufLen3 = nWriteLen3 + 2;
	char pBuf2[nBufLen2];
	char pBuf3[nBufLen3];

	
	memset(pBuf3, '#', nBufLen3);
	memcpy((void*)(pBuf2+nBufLen2-1-strlen(pUC)), pUC, strlen(pUC));
	memcpy((void*)(pBuf3+nBufLen3-1-strlen(pUC)), pUC, strlen(pUC));
	pBuf2[nBufLen2-1] = '\0';
	pBuf3[nBufLen3-1] = '\0';

    
    CBufWriteStream_test* pBWS = new CBufWriteStream_test();
    PrintBufWriteStream(pBWS, "before Write()");
    
    
    nFreeLen = s_nBufSize;
    nDataLen = 0;
	pTmp = pBWS->GetBeginHdl();
    VERIFY_BWS_STATE(pBWS, pTmp, NULL, pTmp, nFreeLen, nDataLen)
    
    
    ColorPrintf(TC_Magenta, "Write(%d)...\n", nWriteLen1);
    nRtn = pBWS->Write(pUC, 1);
    PrintBufWriteStream(pBWS, "after Write:");

    
	ASSERT(nWriteLen1 == nRtn); 
    nFreeLen = s_nBufSize-nWriteLen1;
    nDataLen = nWriteLen1;
	ASSERT('\n' == *(pBWS->GetCurHdl()->pBuf+nWriteLen1-1));
    VERIFY_BWS_STATE(pBWS, pBWS->GetBeginHdl(), NULL, pBWS->GetCurHdl(), nFreeLen, nDataLen)

    
    ColorPrintf(TC_Magenta, "Write(%d)...\n", nWriteLen2);
    nRtn = pBWS->Write(pBuf2, 2);
    PrintBufWriteStream(pBWS, "after Write:");

    
	ASSERT(nWriteLen2 == nRtn); 
    nFreeLen = s_nBufSize - (nWriteLen1 + nWriteLen2)%s_nBufSize;
    nDataLen = nWriteLen1 + nWriteLen2;
	ASSERT('\n' == *(pBWS->GetCurHdl()->pBuf+s_nBufSize-nFreeLen-1));
    VERIFY_BWS_STATE(pBWS, pBWS->GetBeginHdl(), NULL, pBWS->GetCurHdl(), nFreeLen, nDataLen)

    
    ColorPrintf(TC_Magenta, "Write(%d)...\n", nWriteLen3);
    nRtn = pBWS->Write(pBuf3, 3);
    PrintBufWriteStream(pBWS, "after Write:");

    
	ASSERT(nWriteLen3 == nRtn); 
    nFreeLen = 0;
    nDataLen = nWriteLen1 + nWriteLen2 + nWriteLen3;
	ASSERT('\n' == *(pBWS->GetCurHdl()->pBuf+s_nBufSize-1));
    VERIFY_BWS_STATE(pBWS, pBWS->GetBeginHdl(), NULL, pBWS->GetCurHdl(), nFreeLen, nDataLen)
	
    
    delete pBWS;
}
#undef VERIFY_BWS_STATE

static void PrintBufWriteStream(CBufWriteStream_test *pBWS, const char* p)
{
    ASSERT(NULL != pBWS);
    BufHandle *pBegin, *pGuard;

	printf("--------------------------------------------\n");
    if(p != NULL)
        printf("\x1b[35m%s\x1b[0m\n", p);
	
    pBegin = pBWS->GetBeginHdl();
	pGuard = pBWS->GetGuardHdl();

    if(NULL == pBegin || pBegin == pGuard) {
        printf("the writestream is empty.\n");
	    printf("--------------------------------------------\n");
        return;
    }

    printf("the BufWriteStream State(nFreeLen = %d, nDataLen = %d, nBufsNum = %d):\n", pBWS->GetFreeLen(), pBWS->GetDataLen(), pBWS->GetBufHdlNum());
    int i = 1;
    for(; pBegin != pGuard; pBegin = pBegin->_next, ++i) {
        printf("#%-2d ", i);
		PrintHdlContent(pBegin);
    }

	printf("--------------------------------------------\n");
}

