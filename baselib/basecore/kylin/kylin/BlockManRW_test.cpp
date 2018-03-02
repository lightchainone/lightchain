#include "stdafx.h"
#include "Trace.h"
#include "Kylin.h"
#include "Async.h"
#include "BlockMan.h"
#include "UnitTest.h"
#include "../test/KylinUtils4Test.h"

#define NR_DISKS 6

static char s_NewRootDir[PATH_MAX+1];
static char s_UserInputDataDir[PATH_MAX+1]; 

static CBlockManV s_BlkMan;
static CCond s_RCond, s_WCond;
static char* s_pWriteData = NULL;
static char* s_pReadData = NULL;

static inline bool VerifyReadData(BlockRequest* pReq)
{
	
	if(0 == strncmp((char*)pReq->buf, s_pWriteData+((char*)pReq->buf - s_pReadData), 1024))
		return true;
	else
		return false;
}

class CBlockManTester : plclic CAsyncClient
{
plclic:
	CBlockManTester() {
		CAsyncClient::SetName("BlockManTester");
	}

	CBlockManTester(const char* name) {
		CAsyncClient::SetName(name);
	}

	~CBlockManTester() {
		while(0 != GetRef())
			Sleep(100);
	}

	char* GetName() { return m_name; }

private:

	void OnCompletion(AsyncContext* pContext)
	{
		BlockRequest* pReq = (BlockRequest*)pContext;

		switch(pReq->async.nAction) {
			case AA_READ_BLOCK:
				printf("read  req[id = %llx, disk = %d, len = %d, off = %-5d, data = \'%c\'] ack.\n", pReq->id, ((uint8)pReq->id), pReq->len, pReq->off, *(char*)pReq->buf);
				ASSERT(true == VerifyReadData(pReq));
				s_RCond.Signal();
				break;
			case AA_WRITE_BLOCK:
				printf("write req[id = %llx, disk = %d, len = %d, off = %-5d, data = \'%c\'] ack.\n", pReq->id, ((uint8)pReq->id), pReq->len, pReq->off, *(char*)pReq->buf);
				s_WCond.Signal();
				break;
		}
	}
};


class BlockManTest {
plclic:
	void SetUp() {
		SetNewRootDirForTest(NULL, s_NewRootDir, s_UserInputDataDir); 
		ASSERT(APFE_OK == InitKylin(1, 0, NR_DISKS));
		ASSERT(true == IsKylinRunning());
	}

	void TearDown() {
		RemoveRootDir(s_NewRootDir, s_UserInputDataDir); 
		ASSERT(true == IsKylinRunning());
		while(APFE_OK != StopKylin())
			Sleep(100);
		}
};


TEST_F(BlockManTest, Case1_test_ReadWrite)
{
	printf("---------- test BlockMan Read&&Write ----------\n");

    
	
	int nReq = NR_DISKS * 2;
	int nBufSize = 1024 * nReq;	
	uint64 logblkId[NR_DISKS];
	uint64 datablkId[NR_DISKS];
	BlockRequest Reqs[nReq]; 

	CBlockManTester *tester = new CBlockManTester("BlkManTester");

	s_pReadData = (char*)malloc(nBufSize);
	s_pWriteData = (char*)malloc(nBufSize);
	ASSERT(NULL != s_pReadData);
	ASSERT(NULL != s_pWriteData);

	for(int i = 0; i < nReq; ++i){
		memset(s_pWriteData+1024*i, '0'+i, 1024);
		memset(s_pReadData+1024*i, 0, 1024);
	}
	
	s_WCond.Init(nReq);

	
	for(int i = 0; i < NR_DISKS; ++i) {
		logblkId[i] = MakeLogBlockId(1, i, i, i);
		datablkId[i] = MakeDataBlockId(1, i, i);
		ZeroMemory(&Reqs[i], sizeof(BlockRequest));
		ZeroMemory(&Reqs[6+i], sizeof(BlockRequest));
		s_BlkMan.WriteBlock(logblkId[i], s_pWriteData+i*1024, 1024, i*1024, true, tester, &Reqs[i]);
		s_BlkMan.WriteBlock(datablkId[i], s_pWriteData+(i+6)*1024, 1024, (i+6)*1024, true, tester, &Reqs[6+i]);
	}

	s_WCond.Wait();

	
	s_RCond.Init(nReq);

	for(int i = 0; i < NR_DISKS; ++i) {
		ZeroMemory(&Reqs[i], sizeof(BlockRequest));
		ZeroMemory(&Reqs[6+i], sizeof(BlockRequest));
		s_BlkMan.ReadBlock(logblkId[i], s_pReadData+i*1024, 1024, i*1024, true, tester, &Reqs[i]);
		s_BlkMan.ReadBlock(datablkId[i], s_pReadData+(i+6)*1024, 1024, (i+6)*1024, true, tester, &Reqs[6+i]);
	}

	s_RCond.Wait();

	
	delete tester;
	free(s_pReadData);
	free(s_pWriteData);
	s_pReadData = NULL;
	s_pWriteData = NULL;
}

#undef NR_DISKS
