#include "stdafx.h"
#include "SyncObj.h"
#include "BufStream.h"
#include "UnitTest.h"
#include "../test/KylinUtils4Test.h"

static const int s_nTranBuf = 20; 
static const int s_nBigTranBuf = 20; 
static const int s_nBufSize = 1024; 
static const int s_nBufHdl = 1024;  

class CBufReadStream_test : plclic CBufReadStream
{
plclic:
	CBufReadStream_test(BufHandle* pBegin, const BufHandle *pNext):CBufReadStream(pBegin, pNext) {}

	BufHandle* GetCurHdl() const { return m_pCur; }

	const BufHandle* GetGuardHdl() const { return m_pGuard; }

	char* GetBufPointer() const { return m_pBuf; }

	int GetCurDataLen() const { return m_nLen; }

	int GetTotalDataLen() const {
		BufHandle* pTmp = m_pCur;
		int n = 0;

		if(pTmp == m_pGuard || pTmp == NULL)
			return 0;
		else {
			n += m_nLen;
			pTmp = pTmp->_next;
			for(; pTmp != m_pGuard; pTmp = pTmp->_next)
				n += pTmp->nDataLen;
			return n;
		}
	}

	int GetBufHdlNum() const {
		BufHandle* pTmp = m_pCur;
		int n = 0;

		if(pTmp == m_pGuard || pTmp == NULL)
			return 0;
		else {
			for(; pTmp != m_pGuard; pTmp = pTmp->_next, ++n);
			return n;
		}
	}

};

class BufReadStreamTest {
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

bool BufReadStreamTest::m_bInit = false;

static void PrintBufReadStream(CBufReadStream_test *pBRS, const char* p=NULL);



#define VERIFY_BRS_STATE(pBRS, pCur, pGuard, nCurDataLen, nTotalDataLen)			\
    ASSERT(pCur == pBRS->GetCurHdl());												\
    ASSERT(pGuard == pBRS->GetGuardHdl());											\
    ASSERT((-1 == nCurDataLen)? true : (nCurDataLen == pBRS->GetCurDataLen()));		\
    ASSERT((-1 == nTotalDataLen) ? true : (nTotalDataLen == pBRS->GetTotalDataLen()));



TEST_F(BufReadStreamTest, Case1_test_constructor)
{
	
	BufHandle* pHdls[3];

    ColorPrintf(TC_Red, "test constructing CBufReadStream with no data...\n");

	
	CBufReadStream_test* pBRS = new CBufReadStream_test(NULL, NULL);

	
	VERIFY_BRS_STATE(pBRS, NULL, NULL, 0, 0)
	PrintBufReadStream(pBRS, "constructing with(NULL, NULL)");
	
	
	delete pBRS;
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl));

	
	CreateHdlChain(pHdls, 3);
	for(int i = 0; i < 3; ++i)
	PrintHdlChain("handle chain to construct BRS", 0, pHdls[0], NULL);

    ColorPrintf(TC_Red, "test constructing with[pHdls[0], pHdls[2])...\n");

	
	pBRS = new CBufReadStream_test(pHdls[0], pHdls[2]);

	
	VERIFY_BRS_STATE(pBRS, pHdls[0], pHdls[2], 5, 10)
	ASSERT(2 == pBRS->GetBufHdlNum());

	PrintBufReadStream(pBRS, "constructing with[pHdls[0], pHdls[2])");

	
	delete pBRS;
	ChainFreeHdl(pHdls[0], NULL);
}



TEST_F(BufReadStreamTest, Case2_test_Seek)
{
    ColorPrintf(TC_Red, "test Seek()...\n");

	
	BufHandle* pHdls[4];

	
	CreateHdlChain(pHdls, 4);
	for(int i = 0; i < 4; ++i)

	CBufReadStream_test* pBRS = new CBufReadStream_test(pHdls[0], NULL);

	PrintBufReadStream(pBRS, "before Seek()");

	
	ColorPrintf(TC_Magenta, "Seek(0)\n");
	ASSERT(pBRS->Seek(0));
	VERIFY_BRS_STATE(pBRS, pHdls[0], NULL, 5, 20)
	ASSERT(4 == pBRS->GetBufHdlNum());
	PrintBufReadStream(pBRS, "after Seek(0)");

	
	ColorPrintf(TC_Magenta, "go on Seek(5)\n");
	ASSERT(pBRS->Seek(5));
	VERIFY_BRS_STATE(pBRS, pHdls[0], NULL, 0, 15)
	ASSERT(4 == pBRS->GetBufHdlNum());
	PrintBufReadStream(pBRS, "after Seek(5)");

	
	ColorPrintf(TC_Magenta, "go on Seek(6)\n");
	ASSERT(pBRS->Seek(6));
	VERIFY_BRS_STATE(pBRS, pHdls[2], NULL, 4, 9)
	ASSERT(2 == pBRS->GetBufHdlNum());
	PrintBufReadStream(pBRS, "after Seek(6)");

	
	ColorPrintf(TC_Magenta, "go on Seek(10), 10 > BRSTotalDataLen\n");
	ASSERT(false == pBRS->Seek(10));
	VERIFY_BRS_STATE(pBRS, NULL, NULL, -1, 0)
	ASSERT(0 == pBRS->GetBufHdlNum());
	PrintBufReadStream(pBRS, "after Seek(10)");

	
	ASSERT(false == pBRS->Seek(0));

	
	delete pBRS;
	ChainFreeHdl(pHdls[0], NULL);
}



TEST_F(BufReadStreamTest, Case3_test_SeekToZero)
{
    ColorPrintf(TC_Red, "test SeekToZero() when no \'0\'...\n");

	
	BufHandle* pHdls[4];
	int nRtnLen = 0;

	
	CreateHdlChain(pHdls, 4);
	for(int i = 0; i < 4; ++i)

	CBufReadStream_test* pBRS = new CBufReadStream_test(pHdls[0], NULL);
	PrintBufReadStream(pBRS, "before SeekToZero()");

	ColorPrintf(TC_Magenta, "SeekToZero...\n");

	
	ASSERT(false == pBRS->SeekToZero(&nRtnLen));

	
	VERIFY_BRS_STATE(pBRS, NULL, NULL, -1, 0)
		
	PrintBufReadStream(pBRS, "after SeekToZero()");

	
	delete pBRS;
	ChainFreeHdl(pHdls[0], NULL);
}



TEST_F(BufReadStreamTest, Case4_test_SeekToZero)
{
    ColorPrintf(TC_Red, "test SeekToZero() when \'0\' at the beginning...\n");

	
	BufHandle* pHdls[4];
	int nRtnLen = 0;

	
	CreateHdlChain(pHdls, 4);
	for(int i = 0; i < 4; ++i)
	pHdls[0]->pBuf[0] = 0;

	CBufReadStream_test* pBRS = new CBufReadStream_test(pHdls[0], NULL);
	PrintBufReadStream(pBRS, "before SeekToZero()");

	ColorPrintf(TC_Magenta, "SeekToZero...\n");

	
	ASSERT(true == pBRS->SeekToZero(&nRtnLen));

	
	ASSERT(1 == nRtnLen);
	VERIFY_BRS_STATE(pBRS, pHdls[0], NULL, 4, 19)
		
	PrintBufReadStream(pBRS, "after SeekToZero()");

	
	delete pBRS;
	ChainFreeHdl(pHdls[0], NULL);
}



TEST_F(BufReadStreamTest, Case5_test_SeekToZero)
{
    ColorPrintf(TC_Red, "test SeekToZero() when \'0\' at boundary...\n");

	
	BufHandle* pHdls[4];
	int nRtnLen = 0;

	
	CreateHdlChain(pHdls, 4);
	for(int i = 0; i < 4; ++i)
	pHdls[0]->pBuf[4] = 0;

	CBufReadStream_test* pBRS = new CBufReadStream_test(pHdls[0], NULL);
	PrintBufReadStream(pBRS, "before SeekToZero()");

	ColorPrintf(TC_Magenta, "SeekToZero...\n");

	
	ASSERT(true == pBRS->SeekToZero(&nRtnLen));

	
	ASSERT(5 == nRtnLen);
	VERIFY_BRS_STATE(pBRS, pHdls[0], NULL, 0, 15)
		
	PrintBufReadStream(pBRS, "after SeekToZero()");

	
	delete pBRS;
	ChainFreeHdl(pHdls[0], NULL);
}



TEST_F(BufReadStreamTest, Case6_test_SeekToZero)
{
    ColorPrintf(TC_Red, "test SeekToZero() when \'0\' at boundary...\n");

	
	BufHandle* pHdls[4];
	int nRtnLen = 0;

	
	CreateHdlChain(pHdls, 4);
	for(int i = 0; i < 4; ++i)
	pHdls[1]->pBuf[0] = 0;

	CBufReadStream_test* pBRS = new CBufReadStream_test(pHdls[0], NULL);
	PrintBufReadStream(pBRS, "before SeekToZero()");

	ColorPrintf(TC_Magenta, "SeekToZero...\n");

	
	ASSERT(true == pBRS->SeekToZero(&nRtnLen));

	
	ASSERT(6 == nRtnLen);
	VERIFY_BRS_STATE(pBRS, pHdls[1], NULL, 4, 14)
		
	PrintBufReadStream(pBRS, "after SeekToZero()");

	
	delete pBRS;
	ChainFreeHdl(pHdls[0], NULL);
}



TEST_F(BufReadStreamTest, Case7_test_SeekToZero)
{
    ColorPrintf(TC_Red, "test SeekToZero() when \'0\' at the end...\n");

	
	BufHandle* pHdls[4];
	int nRtnLen = 0;

	
	CreateHdlChain(pHdls, 4);
	for(int i = 0; i < 4; ++i)
	pHdls[3]->pBuf[4] = 0;

	CBufReadStream_test* pBRS = new CBufReadStream_test(pHdls[0], NULL);
	PrintBufReadStream(pBRS, "before SeekToZero()");

	ColorPrintf(TC_Magenta, "SeekToZero...\n");

	
	ASSERT(true == pBRS->SeekToZero(&nRtnLen));

	
	ASSERT(20 == nRtnLen);
	VERIFY_BRS_STATE(pBRS, pHdls[3], NULL, 0, 0)
		
	PrintBufReadStream(pBRS, "after SeekToZero()");

	
	delete pBRS;
	ChainFreeHdl(pHdls[0], NULL);
}



TEST_F(BufReadStreamTest, Case8_test_GetCurrentHdl)
{
    ColorPrintf(TC_Red, "test GetCurrentHdl() when m_nLen = 0...\n");

	
	BufHandle* pHdls[4];

	
	CreateHdlChain(pHdls, 4);
	for(int i = 0; i < 4; ++i)
	pHdls[0]->pBuf[4] = 0;

	CBufReadStream_test* pBRS = new CBufReadStream_test(pHdls[0], NULL);
	pBRS->SeekToZero(NULL);
	VERIFY_BRS_STATE(pBRS, pHdls[0], NULL, 0, 15)
	PrintBufReadStream(pBRS, "before GetCurrentHdl()");

	
	ColorPrintf(TC_Magenta, "GetCurrentHdl()...\n");
	ASSERT(pHdls[1] == pBRS->GetCurrentHdl());
	VERIFY_BRS_STATE(pBRS, pHdls[1], NULL, 5, 15)
		
	PrintBufReadStream(pBRS, "after GetCurrentHdl()");

	
	delete pBRS;
	ChainFreeHdl(pHdls[0], NULL);
}



TEST_F(BufReadStreamTest, Case9_test_GetCurrentHdl)
{
    ColorPrintf(TC_Red, "test GetCurrentHdl() and will SplitBuf...\n");

	
	BufHandle* pHdls[4];

	
	CreateHdlChain(pHdls, 4);
	for(int i = 0; i < 4; ++i)

	CBufReadStream_test* pBRS = new CBufReadStream_test(pHdls[0], NULL);
	ASSERT(true == pBRS->Seek(7));
	VERIFY_BRS_STATE(pBRS, pHdls[1], NULL, 3, 13)
	PrintBufReadStream(pBRS, "before GetCurrentHdl()");

	
	ColorPrintf(TC_Magenta, "GetCurrentHdl()...\n");
	ASSERT(pHdls[1]->_next == pBRS->GetCurrentHdl());
	VERIFY_BRS_STATE(pBRS, pHdls[1]->_next, NULL, 3, 13)
	ASSERT(3 == pBRS->GetBufHdlNum());
	ASSERT(true == CheckBufPoolState(s_nTranBuf, s_nTranBuf, s_nTranBuf-4, 
				                     s_nBigTranBuf, 0, s_nBigTranBuf,
									 s_nBufHdl-5));
		
	PrintBufReadStream(pBRS, "after GetCurrentHdl()");

	
	delete pBRS;
	ChainFreeHdl(pHdls[0], NULL);
}



TEST_F(BufReadStreamTest, Case10_test_GetCurrentHdl)
{
    ColorPrintf(TC_Red, "test GetCurrentHdl() when m_pCur = m_pGuard...\n");

	
	BufHandle* pHdls[4];

	
	CreateHdlChain(pHdls, 4);
	for(int i = 0; i < 4; ++i)

	CBufReadStream_test* pBRS = new CBufReadStream_test(pHdls[0], NULL);
	pBRS->SeekToZero(NULL);
	VERIFY_BRS_STATE(pBRS, NULL, NULL, 0, 0)
	PrintBufReadStream(pBRS, "before GetCurrentHdl()");

	
	ASSERT(NULL == pBRS->GetCurrentHdl());
		
	PrintBufReadStream(pBRS, "after GetCurrentHdl()");

	
	delete pBRS;
	ChainFreeHdl(pHdls[0], NULL);
}



TEST_F(BufReadStreamTest, Case11_test_GetBuffer)
{
    ColorPrintf(TC_Red, "test GetBuffer() with nLen = 0...\n");

	
	BufHandle* pHdls[4];
	char buf[64]; 
	int nLen, nRtn;

	
	CreateHdlChain(pHdls, 4);
	for(int i = 0; i < 4; ++i)

	CBufReadStream_test* pBRS = new CBufReadStream_test(pHdls[0], NULL);
	PrintBufReadStream(pBRS, "before GetBuffer()");

	
	nLen = 0;
	nRtn = pBRS->GetBuffer(buf, nLen);
	ASSERT(0 == nRtn);
	VERIFY_BRS_STATE(pBRS, pHdls[0], NULL, 5, 20)
	
	PrintBufContent(buf, nRtn, "the buffer get");
	PrintBufReadStream(pBRS, "after GetBuffer(nLen=0)");

	
	delete pBRS;
	ChainFreeHdl(pHdls[0], NULL);
}



TEST_F(BufReadStreamTest, Case12_test_GetBuffer)
{
    ColorPrintf(TC_Red, "test GetBuffer(nLen > 0) and at boundary...\n");

	
	BufHandle* pHdls[4];
	char buf[64]; 
	int nLen, nRtn;

	
	CreateHdlChain(pHdls, 4);
	for(int i = 0; i < 4; ++i)

	CBufReadStream_test* pBRS = new CBufReadStream_test(pHdls[0], NULL);
	PrintBufReadStream(pBRS, "before GetBuffer()");

	
	nLen = 10;
	nRtn = pBRS->GetBuffer(buf, nLen);
	ASSERT(10 == nRtn);
	VERIFY_BRS_STATE(pBRS, pHdls[1], NULL, 0, 10)
	
	PrintBufContent(buf, nRtn, "the buffer get");
	PrintBufReadStream(pBRS, "after GetBuffer(nLen=10)");

	
	delete pBRS;
	ChainFreeHdl(pHdls[0], NULL);
}



TEST_F(BufReadStreamTest, Case13_test_GetBuffer)
{
    ColorPrintf(TC_Red, "test GetBuffer(nLen > 0)...\n");

	
	BufHandle* pHdls[4];
	char buf[64]; 
	int nLen, nRtn;

	
	CreateHdlChain(pHdls, 4);
	for(int i = 0; i < 4; ++i)

	CBufReadStream_test* pBRS = new CBufReadStream_test(pHdls[0], NULL);
	PrintBufReadStream(pBRS, "before GetBuffer()");

	
	nLen = 11;
	nRtn = pBRS->GetBuffer(buf, nLen);
	ASSERT(11 == nRtn);
	VERIFY_BRS_STATE(pBRS, pHdls[2], NULL, 4, 9)
	
	PrintBufContent(buf, nRtn, "the buffer get");
	PrintBufReadStream(pBRS, "after GetBuffer(nLen=11)");

	
	delete pBRS;
	ChainFreeHdl(pHdls[0], NULL);
}



TEST_F(BufReadStreamTest, Case14_test_GetBuffer)
{
    ColorPrintf(TC_Red, "test GetBuffer(nLen > 0) and nLen = BRSTotalDataLen...\n");

	
	BufHandle* pHdls[4];
	char buf[64]; 
	int nLen, nRtn;

	
	CreateHdlChain(pHdls, 4);
	for(int i = 0; i < 4; ++i)

	CBufReadStream_test* pBRS = new CBufReadStream_test(pHdls[0], NULL);
	PrintBufReadStream(pBRS, "before GetBuffer()");

	
	nLen = 20;
	nRtn = pBRS->GetBuffer(buf, nLen);
	ASSERT(20 == nRtn);
	VERIFY_BRS_STATE(pBRS, pHdls[3], NULL, 0, 0)
	
	PrintBufContent(buf, nRtn, "the buffer get");
	PrintBufReadStream(pBRS, "after GetBuffer(nLen=20)");

	
	delete pBRS;
	ChainFreeHdl(pHdls[0], NULL);
}



TEST_F(BufReadStreamTest, Case15_test_GetBuffer)
{
    ColorPrintf(TC_Red, "test GetBuffer(nLen > 0) and nLen > BRSTotalDataLen...\n");

	
	BufHandle* pHdls[4];
	char buf[64]; 
	int nLen, nRtn;

	
	CreateHdlChain(pHdls, 4);
	for(int i = 0; i < 4; ++i)

	CBufReadStream_test* pBRS = new CBufReadStream_test(pHdls[0], NULL);
	PrintBufReadStream(pBRS, "before GetBuffer()");

	
	nLen = 21;
	nRtn = pBRS->GetBuffer(buf, nLen);
	ASSERT(20 == nRtn);
	VERIFY_BRS_STATE(pBRS, NULL, NULL, -1, 0)
	
	PrintBufContent(buf, nRtn, "the buffer get");
	PrintBufReadStream(pBRS, "after GetBuffer(nLen=21)");

	
	delete pBRS;
	ChainFreeHdl(pHdls[0], NULL);
}



TEST_F(BufReadStreamTest, Case16_test_GetPascalString)
{
    ColorPrintf(TC_Red, "test GetPascalString(char*, int)...\n");

	
	CBufWriteStream *pBWS = new CBufWriteStream();
	BufHandle* pHdl;
	int nBufSize = s_nBufSize*2;
	char buf[nBufSize]; 
	char *p1 = "pascal string 1";
	char p2[s_nBufSize];
	p2[s_nBufSize-1] = 0;
	memcpy(p2, "PASCAL STRING TWO ", 18);
	int nLen = strlen(p1)+strlen(p2)+2+4+4;

	pBWS->PutStringAsPascal(p1);
	pBWS->PutStringAsPascal(p2);

	pHdl = pBWS->GetBegin();
	ASSERT(s_nBufSize == pHdl->nDataLen);

	CBufReadStream_test *pBRS = new CBufReadStream_test(pHdl, NULL);
	
	
	VERIFY_BRS_STATE(pBRS, pHdl, NULL, s_nBufSize, nLen)
	PrintBufReadStream(pBRS, "before GetPascalString()");
	
	
	ASSERT(true == pBRS->GetPascalString(buf, nBufSize));
	ASSERT(0 == strncmp(buf, p1, strlen(p1)));
	PrintBufContent(buf, -1, "the 1st pascal string");

	
	ASSERT(true == pBRS->GetPascalString(buf, nBufSize));
	ASSERT(0 == strncmp(buf, p2, strlen(p2)));
	PrintBufContent(buf, -1, "the 2nd pascal string");

	VERIFY_BRS_STATE(pBRS, pHdl->_next, NULL, 0, 0)

	
	delete pBWS;
	delete pBRS;
	ChainFreeHdl(pHdl, NULL);
}


TEST_F(BufReadStreamTest, Case17_test_GetPascalString)
{
    ColorPrintf(TC_Red, "test GetPascalString(BufHandle*, BufHandle*)...\n");

	
	CBufWriteStream *pBWS = new CBufWriteStream();
	BufHandle* pHdl, *pBegin, *pNext, *pTmp;
	char *p1 = "pascal string 1";
	char p2[s_nBufSize];
	char buf[s_nBufSize];
	int nLen = 0, n;
	int nPreOccupy = s_nBufSize - 4;

	memset(p2, '#', s_nBufSize);
	pBWS->PutBuffer(p2, nPreOccupy);
	p2[s_nBufSize-1] = 0;
	memcpy(p2, "PASCAL STRING TWO ", 18);

	pBWS->PutStringAsPascal(p1);
	pBWS->PutStringAsPascal(p2);
	nLen = nPreOccupy+strlen(p1)+strlen(p2)+2+8;
	

	pHdl = pBWS->GetBegin();
	ASSERT(s_nBufSize == pHdl->nDataLen);

	CBufReadStream_test* pBRS = new CBufReadStream_test(pHdl, NULL);
	
	
	VERIFY_BRS_STATE(pBRS, pHdl, NULL, s_nBufSize, nLen)
	ASSERT(true == pBRS->Seek(nPreOccupy));
	PrintBufReadStream(pBRS, "before GetPascalString()");
	
	
	pTmp = pBRS->GetCurHdl();
	ASSERT(true == pBRS->GetPascalString(&pBegin, &pNext));
	n = GetHdlChainContent(pBegin, pNext, buf, s_nBufSize);
	ASSERT(n == (int)strlen(p1) + 1);
	ASSERT(0 == strncmp(buf, p1, strlen(p1)));
	VERIFY_BRS_STATE(pBRS, pNext, NULL, s_nBufSize-(int)strlen(p1)-1, (int)strlen(p2)+1+4)
	PrintBufContent(buf, -1, "the 1st pascal string");
	PrintBufReadStream(pBRS, "after get 1st pascal string");
	ChainFreeHdl(pBegin, pNext);
	ChainFreeHdl(pTmp, pBegin);

	
	pTmp = pBRS->GetCurHdl();
	ASSERT(true == pBRS->GetPascalString(&pBegin, &pNext));
	n = GetHdlChainContent(pBegin, pNext, buf, s_nBufSize);
	ASSERT(n == (int)strlen(p2) + 1);
	ASSERT(0 == strncmp(buf, p2, (int)strlen(p2)));
	VERIFY_BRS_STATE(pBRS, NULL, NULL, 0, 0)
	PrintBufContent(buf, -1, "the 2nd pascal string");
	PrintBufReadStream(pBRS, "after get 2nd pascal string");
	ChainFreeHdl(pBegin, pNext);
	ChainFreeHdl(pTmp, pBegin);

	
	delete pBWS;
	delete pBRS;
}



TEST_F(BufReadStreamTest, Case18_test_SkipToken)
{
    ColorPrintf(TC_Red, "test SkipToken() when no Token in BRS...\n");

	
	CBufWriteStream *pBWS = new CBufWriteStream();
	char buf[50];
	BufHandle* pHdl;

	pBWS->PutBuffer(buf, 50);
	pHdl = pBWS->GetBegin();

	CBufReadStream_test* pBRS = new CBufReadStream_test(pHdl, NULL);
	VERIFY_BRS_STATE(pBRS, pHdl, NULL, 50, 50)
	PrintBufReadStream(pBRS, "before SkipToken()");

	
	ColorPrintf(TC_Magenta, "SkipToken()...\n");
	ASSERT(false == pBRS->SkipToken());
	VERIFY_BRS_STATE(pBRS, NULL, NULL, 0, 0)
	PrintBufReadStream(pBRS, "after SkipToken()");

	
	delete pBWS;
	delete pBRS;
	ChainFreeHdl(pHdl, NULL);
}



TEST_F(BufReadStreamTest, Case19_test_SkipToken)
{
    ColorPrintf(TC_Red, "test SkipToken()...\n");

	
	CBufWriteStream *pBWS = new CBufWriteStream();
	BufHandle* pHdl;
	char buf[s_nBufSize];
	char *token1 = "token1 ";
	char *token2 = "token2\r";
	char *token3 = "token3\r";

	pBWS->PutBuffer(token1);
	pBWS->PutBuffer(buf, s_nBufSize-(int)strlen(token1)-(int)strlen(token2));
	pBWS->PutBuffer(token2);
	pBWS->PutBuffer(buf, 1);
	pBWS->PutBuffer(token3);
	pHdl = pBWS->GetBegin();

	CBufReadStream_test* pBRS = new CBufReadStream_test(pHdl, NULL);
	VERIFY_BRS_STATE(pBRS, pHdl, NULL, s_nBufSize, -1)
	PrintBufReadStream(pBRS, "before SkipToken()");

	
	ColorPrintf(TC_Magenta, "SkipToken(token1)...\n");
	ASSERT(true == pBRS->SkipToken());
	VERIFY_BRS_STATE(pBRS, pHdl, NULL, s_nBufSize-(int)strlen(token1), -1)
	PrintBufReadStream(pBRS, "after SkipToken(token1)");

	
	ColorPrintf(TC_Magenta, "SkipToken(token2)...\n");
	ASSERT(true == pBRS->SkipToken());
	VERIFY_BRS_STATE(pBRS, pHdl, NULL, 0, -1)
	PrintBufReadStream(pBRS, "after SkipToken(token2)");

	
	ColorPrintf(TC_Magenta, "SkipToken(token3)...\n");
	ASSERT(true == pBRS->SkipToken());
	VERIFY_BRS_STATE(pBRS, pHdl->_next, NULL, 0, 0)
	PrintBufReadStream(pBRS, "after SkipToken(token3)");

	
	delete pBWS;
	delete pBRS;
	ChainFreeHdl(pHdl, NULL);
}



TEST_F(BufReadStreamTest, Case20_test_GetToken)
{
    ColorPrintf(TC_Red, "test GetToken() when no Token in BRS...\n");

	
	CBufWriteStream *pBWS = new CBufWriteStream();
	int nBufSize = 50, nRequest;
	char buf[nBufSize];
	BufHandle* pHdl;

	pBWS->PutBuffer(buf, 40);
	pHdl = pBWS->GetBegin();

	CBufReadStream_test* pBRS = new CBufReadStream_test(pHdl, NULL);
	VERIFY_BRS_STATE(pBRS, pHdl, NULL, 40, 40)
	PrintBufReadStream(pBRS, "before GetToken()");

	
	memset(buf, 0, nBufSize);
	nRequest = nBufSize;
	ASSERT(false == pBRS->GetToken(buf, &nRequest));
	VERIFY_BRS_STATE(pBRS, NULL, NULL, 0, 0)
	ASSERT(40 == nRequest);
	PrintBufContent(buf, -1, "the content get");
	PrintBufReadStream(pBRS, "after SkipToken()");

	
	delete pBWS;
	delete pBRS;
	ChainFreeHdl(pHdl, NULL);
}



TEST_F(BufReadStreamTest, Case21_test_GetToken)
{
    ColorPrintf(TC_Red, "test GetToken()...\n");

	
	CBufWriteStream *pBWS = new CBufWriteStream();
	BufHandle* pHdl;
	int nBufSize = s_nBufSize, nRequest;
	char buf[nBufSize];
	char *token1 = "token1 ";
	char *token2 = "token2\r";
	char *token3 = "token3\r";

	pBWS->PutBuffer(token1);
	pBWS->PutBuffer(buf, s_nBufSize-(int)strlen(token1)-(int)strlen(token2));
	pBWS->PutBuffer(token2);
	pBWS->PutBuffer(buf, 1);
	pBWS->PutBuffer(token3);
	pHdl = pBWS->GetBegin();

	CBufReadStream_test* pBRS = new CBufReadStream_test(pHdl, NULL);
	VERIFY_BRS_STATE(pBRS, pHdl, NULL, s_nBufSize, -1)
	PrintBufReadStream(pBRS, "before GetToken()");

	
	memset(buf, 0, nBufSize);
	nRequest = nBufSize;
	ASSERT(true == pBRS->GetToken(buf, &nRequest));
	ASSERT(nRequest == (int)strlen(token1));
	ASSERT(0 == strncmp(buf, token1, nRequest-1));
	VERIFY_BRS_STATE(pBRS, pHdl, NULL, s_nBufSize-(int)strlen(token1), -1)
	PrintBufContent(buf, -1, "the token get");
	PrintBufReadStream(pBRS, "after GetToken(token1)");

	
	memset(buf, 0, nBufSize);
	nRequest = nBufSize;
	ASSERT(true == pBRS->Seek(s_nBufSize-(int)strlen(token1)-(int)strlen(token2)));
	ASSERT(true == pBRS->GetToken(buf, &nRequest));
	ASSERT(nRequest == (int)strlen(token2));
	ASSERT(0 == strncmp(buf, token2, nRequest-1));
	VERIFY_BRS_STATE(pBRS, pHdl, NULL, 0, -1)
	PrintBufContent(buf, -1, "the token get");
	PrintBufReadStream(pBRS, "after GetToken(token2)");

	
	memset(buf, 0, nBufSize);
	nRequest = nBufSize;
	ASSERT(true == pBRS->Seek(1));
	ASSERT(true == pBRS->GetToken(buf, &nRequest));
	ASSERT(nRequest == (int)strlen(token3));
	ASSERT(0 == strncmp(buf, token3, nRequest-1));
	VERIFY_BRS_STATE(pBRS, pHdl->_next, NULL, 0, 0)
	PrintBufContent(buf, -1, "the token get");
	PrintBufReadStream(pBRS, "after GetToken(token3)");

	
	delete pBWS;
	delete pBRS;
	ChainFreeHdl(pHdl, NULL);
}



TEST_F(BufReadStreamTest, Case22_test_SeekToken)
{
    ColorPrintf(TC_Red, "test GetToken()...\n");

	
	CBufWriteStream *pBWS = new CBufWriteStream();
	BufHandle* pHdl;
	int nBufSize = s_nBufSize;
	char buf[nBufSize];
	char *token1 = "token1 ";
	char *token2 = "token2\r";
	char *token3 = "token3\r";

	pBWS->PutBuffer(token1);
	pBWS->PutBuffer(buf, s_nBufSize-(int)strlen(token1)-(int)strlen(token2));
	pBWS->PutBuffer(token2);
	pBWS->PutBuffer(buf, 1);
	pBWS->PutBuffer(token3);
	pHdl = pBWS->GetBegin();

	CBufReadStream_test* pBRS = new CBufReadStream_test(pHdl, NULL);
	VERIFY_BRS_STATE(pBRS, pHdl, NULL, s_nBufSize, -1)
	PrintBufReadStream(pBRS, "before SeekToken()");

	
	ColorPrintf(TC_Magenta, "SeekToken(token1)...\n");
	ASSERT(true == pBRS->SeekToken(token1));
	VERIFY_BRS_STATE(pBRS, pHdl, NULL, s_nBufSize, -1)
	PrintBufReadStream(pBRS, "after SeekToken(token1)");

	
	ColorPrintf(TC_Magenta, "SeekToken(token2)...\n");
	ASSERT(true == pBRS->SeekToken(token2));
	VERIFY_BRS_STATE(pBRS, pHdl, NULL, (int)strlen(token2), -1)
	PrintBufReadStream(pBRS, "after SeekToken(token2)");

	
	ColorPrintf(TC_Magenta, "SeekToken(token3)...\n");
	ASSERT(true == pBRS->SeekToken(token3));
	VERIFY_BRS_STATE(pBRS, pHdl->_next, NULL, (int)strlen(token3), (int)strlen(token3))
	PrintBufReadStream(pBRS, "after SeekToken(token3)");

	
	delete pBWS;
	delete pBRS;
	ChainFreeHdl(pHdl, NULL);
}



TEST_F(BufReadStreamTest, Case23_test_GetLine)
{
    ColorPrintf(TC_Red, "test GetLine() when no line in BRS...\n");

	
	BufHandle* pHdls[2];
	int nBufSize = 256, nRequest;
	char buf[nBufSize];
	CreateHdlChain(pHdls, 2);
	
	FillBufHdlContent(pHdls[0], '0', 5);
	FillBufHdlContent(pHdls[1], '1', 5);
	CBufReadStream_test* pBRS = new CBufReadStream_test(pHdls[0], NULL);

	VERIFY_BRS_STATE(pBRS, pHdls[0], NULL, 5, 10)

	PrintBufReadStream(pBRS, "before GetLine()");

	
	memset(buf, 0, nBufSize);
	nRequest = nBufSize;
	ColorPrintf(TC_Magenta, "GetLine()...\n");
	ASSERT(false == pBRS->GetLine(buf, &nRequest));
	VERIFY_BRS_STATE(pBRS, NULL, NULL, 0, 0)
	PrintBufReadStream(pBRS, "after GetLine()");

	
	delete pBRS;
	ChainFreeHdl(pHdls[0], NULL);
}



TEST_F(BufReadStreamTest, Case24_test_GetLine)
{
    ColorPrintf(TC_Red, "test GetLine() when buffer is not enough...\n");

	
	BufHandle* pHdls[2];
	int nBufSize = 8, nRequest;
	char buf[nBufSize];
	CreateHdlChain(pHdls, 2);
	
	FillBufHdlContent(pHdls[0], '0', 5);
	FillBufHdlContent(pHdls[1], '1', 5);
	pHdls[1]->pBuf[5] = '\r';
	pHdls[1]->pBuf[6] = '\n';
	pHdls[1]->nDataLen += 2;
	CBufReadStream_test* pBRS = new CBufReadStream_test(pHdls[0], NULL);

	VERIFY_BRS_STATE(pBRS, pHdls[0], NULL, 5, 12)

	PrintBufReadStream(pBRS, "before GetLine()");

	
	memset(buf, 0, nBufSize);
	nRequest = nBufSize;
	ColorPrintf(TC_Magenta, "GetLine(%d)...\n", nRequest);
	ASSERT(false == pBRS->GetLine(buf, &nRequest));
	ASSERT(8 == nRequest);
	VERIFY_BRS_STATE(pBRS, pHdls[1], NULL, 4, 4)
	PrintBufReadStream(pBRS, "after GetLine(8)");
	PrintBufContent(buf, nBufSize, "the buffer get");

	
	delete pBRS;
	ChainFreeHdl(pHdls[0], NULL);
}



TEST_F(BufReadStreamTest, Case25_test_GetLine)
{
    ColorPrintf(TC_Red, "test GetLine()...\n");

	
	BufHandle* pHdls[2];
	int nBufSize = 256, nRequest;
	char buf[nBufSize];
	CreateHdlChain(pHdls, 2);
	
	FillBufHdlContent(pHdls[0], '0', 20);
	FillBufHdlContent(pHdls[1], '1', 20);
	pHdls[0]->pBuf[5] = '\r';
	pHdls[0]->pBuf[9] = '\n';
	pHdls[1]->pBuf[5] = '\n';
	pHdls[1]->pBuf[6] = '\r';
	pHdls[1]->pBuf[7] = '\n';
	CBufReadStream_test* pBRS = new CBufReadStream_test(pHdls[0], NULL);

	VERIFY_BRS_STATE(pBRS, pHdls[0], NULL, 20, 40)

	PrintBufReadStream(pBRS, "before GetLine()");

	
	memset(buf, 0, nBufSize);
	nRequest = nBufSize;
	ColorPrintf(TC_Magenta, "GetLine(%d)...\n", nRequest);
	ASSERT(true == pBRS->GetLine(buf, &nRequest));
	ASSERT(26 == nRequest);
	VERIFY_BRS_STATE(pBRS, pHdls[1], NULL, 12, 12)
	PrintBufReadStream(pBRS, "after GetLine()");
	PrintBufContent(buf, -1, "the buffer get");

	
	delete pBRS;
	ChainFreeHdl(pHdls[0], NULL);
}



TEST_F(BufReadStreamTest, Case26_test_GetUInt32)
{
    ColorPrintf(TC_Red, "test GetUInt32() when no unit32 in BRS...\n");

	
	BufHandle* pHdl = AllocateHdl(true, NULL, 0, NULL);
	uint32 rtn;

	CBufReadStream_test* pBRS = new CBufReadStream_test(pHdl, NULL);

	VERIFY_BRS_STATE(pBRS, pHdl, NULL, 20, 20)

	PrintBufReadStream(pBRS, "before GetUInt32()");

	
	ColorPrintf(TC_Magenta, "GetUInt32()...\n");
	ASSERT(false == pBRS->GetUInt32(&rtn));
	VERIFY_BRS_STATE(pBRS, pHdl, NULL, -1, -1)
	PrintBufReadStream(pBRS, "after GetUInt32()");

	
	delete pBRS;
	ChainFreeHdl(pHdl, NULL);
}



TEST_F(BufReadStreamTest, Case27_test_GetUInt32)
{
    ColorPrintf(TC_Red, "test GetUInt32() when unit32 exists...\n");

	
	BufHandle* pHdl = AllocateHdl(true, NULL, 0, NULL);
	uint32 rtn;

	pHdl->pBuf[0] = '1';
	pHdl->pBuf[1] = '0';
	pHdl->pBuf[2] = '0';
	pHdl->pBuf[3] = ' ';
	CBufReadStream_test* pBRS = new CBufReadStream_test(pHdl, NULL);

	VERIFY_BRS_STATE(pBRS, pHdl, NULL, 20, 20)

	PrintBufReadStream(pBRS, "before GetUInt32()");

	
	ColorPrintf(TC_Magenta, "GetUInt32()...\n");
	ASSERT(true == pBRS->GetUInt32(&rtn));
	ASSERT(rtn == 100);
	printf("the uint32 got is: %u\n", rtn);
	VERIFY_BRS_STATE(pBRS, pHdl, NULL, 16, 16)
	PrintBufReadStream(pBRS, "after GetUInt32()");

	
	delete pBRS;
	ChainFreeHdl(pHdl, NULL);
}



TEST_F(BufReadStreamTest, Case28_test_GetUInt32)
{
    ColorPrintf(TC_Red, "test GetUInt32() when unit32 exists...\n");

	
	BufHandle* pHdls[2];
	uint32 rtn;

	CreateHdlChain(pHdls, 2);
	pHdls[0]->pBuf[4] = '1';
	pHdls[1]->pBuf[0] = '2';
	CBufReadStream_test* pBRS = new CBufReadStream_test(pHdls[0], NULL);

	VERIFY_BRS_STATE(pBRS, pHdls[0], NULL, 5, 10)
	ASSERT(true == pBRS->Seek(4));
	PrintBufReadStream(pBRS, "before GetUInt32()");

	
	ColorPrintf(TC_Magenta, "GetUInt32()...\n");
	ASSERT(false == pBRS->GetUInt32(&rtn));
	ASSERT(rtn == 12);
	printf("the uint32 got is: %u\n", rtn);
	VERIFY_BRS_STATE(pBRS, pHdls[1], NULL, 4, 4)
	PrintBufReadStream(pBRS, "after GetUInt32()");

	
	delete pBRS;
	ChainFreeHdl(pHdls[0], NULL);
}



TEST_F(BufReadStreamTest, Case29_test_EmptyBRS)
{
    ColorPrintf(TC_Red, "test functions in empty BRS...\n");

	
	BufHandle *pHdl, *pNext;
	int nRtn, nRequest;
	int nBufSize = 16;
	char buf[nBufSize];
	char* token = "token ";

	
	CBufReadStream_test* pBRS = new CBufReadStream_test(NULL, NULL);
	VERIFY_BRS_STATE(pBRS, NULL, NULL, 0, 0)

	
	printf("test Seek()...\n");
	ASSERT(false == pBRS->Seek(0));
	ASSERT(false == pBRS->Seek(1));
	VERIFY_BRS_STATE(pBRS, NULL, NULL, 0, 0)

	
	printf("test SeekToZero()...\n");
	ASSERT(false == pBRS->SeekToZero(&nRtn));
	VERIFY_BRS_STATE(pBRS, NULL, NULL, 0, 0)

	
	printf("test GetCurrentHdl()...\n");
	ASSERT(NULL == pBRS->GetCurrentHdl());
	VERIFY_BRS_STATE(pBRS, NULL, NULL, 0, 0)

	
	printf("test GetPascalString()...\n");
	ASSERT(false == pBRS->GetPascalString(buf, nBufSize));
	ASSERT(false == pBRS->GetPascalString(&pHdl, &pNext));
	VERIFY_BRS_STATE(pBRS, NULL, NULL, 0, 0)
	
	
	printf("test GetBuffer()...\n");
	ASSERT(0 == pBRS->GetBuffer(buf, nBufSize));
	VERIFY_BRS_STATE(pBRS, NULL, NULL, 0, 0)

	
	printf("test SkipToken()...\n");
	ASSERT(false == pBRS->SkipToken());
	VERIFY_BRS_STATE(pBRS, NULL, NULL, 0, 0)

	
	printf("test GetToken()...\n");
	nRequest = nBufSize;
	ASSERT(false == pBRS->GetToken(buf, &nRequest));
	ASSERT(0 == nRequest);
	VERIFY_BRS_STATE(pBRS, NULL, NULL, 0, 0)

	
	printf("test SeekToken()...\n");
	ASSERT(false == pBRS->SeekToken(token));
	VERIFY_BRS_STATE(pBRS, NULL, NULL, 0, 0)

	
	printf("test GetLine()...\n");
	nRequest = nBufSize;
	ASSERT(false == pBRS->GetLine(buf, &nRequest));
	ASSERT(0 == nRequest);
	VERIFY_BRS_STATE(pBRS, NULL, NULL, 0, 0)

	
	printf("test GetUInt32()...\n");
	ASSERT(false == pBRS->GetUInt32((uint32*)&nRtn));
	VERIFY_BRS_STATE(pBRS, NULL, NULL, 0, 0)

	
	delete pBRS;
}

#undef VERIFY_BRS_STATE

static void PrintBufReadStream(CBufReadStream_test *pBRS, const char* p)
{
    ASSERT(NULL != pBRS);
    BufHandle* pCur;
	const BufHandle* pGuard;

	printf("--------------------------------------------\n");
    if(p != NULL)
        printf("\x1b[35m%s\x1b[0m\n", p);
	
    pCur = pBRS->GetCurHdl();
	pGuard = pBRS->GetGuardHdl();

    if(0 == pBRS->GetTotalDataLen()) {
        printf("the read stream is empty.\n");
	    printf("--------------------------------------------\n");
        return;
    }

    printf("the BufReadStream State(nTotalDataLen = %d, nBufsNum = %d):\n", pBRS->GetTotalDataLen(), pBRS->GetBufHdlNum());
    int i = 1;
    for(; pCur != pGuard; pCur = pCur->_next, ++i) {
        printf("#%-2d ", i);
		if(i == 1)
			PrintHdlContent(pCur, (pBRS->GetBufPointer() - pCur->pBuf));
		else
			PrintHdlContent(pCur, 0);
    }

	printf("--------------------------------------------\n");
}

