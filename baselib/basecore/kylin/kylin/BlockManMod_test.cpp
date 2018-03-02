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

static CCond s_RCond, s_WCond, s_MCond;

static char* s_pWriteData = NULL;
static char* s_pReadData = NULL;
static char* s_pModData = NULL;

static bool s_bStartKylin = true; 
static bool s_bStopKylin = false;



static void PrintWriteAckReq(BlockRequest* pReq)
{
	ASSERT(AA_WRITE_BLOCK == pReq->async.nAction);
	printf("write  req[id = %llx, disk = %d, len = %-4d, off = %-5d] ack.\n", pReq->id, ((uint8)pReq->id), pReq->len, pReq->off);
	s_WCond.Signal();
}



static void PrintModAckReq(BlockRequest* pReq)
{
	ASSERT(AA_MODIFY_BLOCK == pReq->async.nAction);
	printf("modify req[id = %llx, disk = %d, len = %-4d, off = %-5d] ack.\n", pReq->id, ((uint8)pReq->id), pReq->len, pReq->off);
	s_MCond.Signal();
}



static void PrintReadAckReq(BlockRequest* pReq)
{
	ASSERT(AA_READ_BLOCK == pReq->async.nAction);
	printf("read   req[id = %llx, disk = %d, len = %-4d, off = %-5d] ack.\n", pReq->id, ((uint8)pReq->id), pReq->len, pReq->off);
	s_RCond.Signal();
}


static CBlockTestClient s_BlockWriter("BlockWriter", PrintWriteAckReq);
static CBlockTestClient s_BlockModder("BlockModder", PrintModAckReq);
static CBlockTestClient s_BlockReader("BlockReader", PrintReadAckReq);


class BlockManTest {
plclic:

	void SetUp() {
		SetNewRootDirForTest(NULL, s_NewRootDir, s_UserInputDataDir); 
		if(s_bStartKylin) {
			ASSERT(APFE_OK == InitKylin(1, 0, NR_DISKS)); 
		}
		ASSERT(true == IsKylinRunning());
	}

	void TearDown() {
		RemoveRootDir(s_NewRootDir, s_UserInputDataDir); 
		ASSERT(true == IsKylinRunning());
		if(s_bStopKylin) {
			while(APFE_OK != StopKylin()) 
				Sleep(100);
		}
	}
};



TEST_F(BlockManTest, Case1_test_ModInBytes)
{
	
	printf("test modify data in bytes(nSizeInBits%%8 == 0 && nOffInBits%%8 == 0)\n\n");

	
	ASSERT(true == IsKylinRunning());
	s_bStartKylin = false;
    
	
	
	int nWBufSize = 2048;
	uint64 datablkId;
	BlockRequest WReq; 

	s_pWriteData = (char*)malloc(nWBufSize);
	ASSERT(NULL != s_pWriteData);
	memset(s_pWriteData, 0, nWBufSize);
	s_WCond.Init(1);

	
	datablkId = MakeDataBlockId(1, 1, 1);

	ZeroMemory(&WReq, sizeof(WReq));
	s_BlkMan.WriteBlock(datablkId, s_pWriteData, nWBufSize, 0, true, &s_BlockWriter, &WReq);
	s_WCond.Wait();

	
	int nMBufSize = 16;
	char MBuf[nMBufSize];
	s_pModData = MBuf;
	memset(MBuf, 0xFF, nMBufSize);

	int nSizeInBits = nMBufSize * 8;
	int nOffInBits = 1024 * 8;
	BlockRequest MReq;

	
	s_MCond.Init(1);
	ZeroMemory(&MReq, sizeof(MReq));
	s_BlkMan.ModifyBlock(datablkId, MBuf, nSizeInBits, nOffInBits, &s_BlockModder, &MReq);
	s_MCond.Wait();

	
	int nRBufSize = nWBufSize;
	s_pReadData = (char*)malloc(nRBufSize);
	ASSERT(NULL != s_pReadData);
	BlockRequest RReq;
	
	
	s_RCond.Init(1);
	ZeroMemory(&RReq, sizeof(RReq));
	s_BlkMan.ReadBlock(datablkId, s_pReadData, nRBufSize, 0, true, &s_BlockReader, &RReq);
	s_RCond.Wait();

	
	ASSERT(((char)0x00 == *(s_pReadData+1023))&&
		   (0 == strncmp(s_pModData, s_pReadData+1024, 16))&&
	       ((char)0x00 == *(s_pReadData+1040)));

	
	free(s_pWriteData);
	free(s_pReadData);
	s_pReadData = NULL;
	s_pWriteData = NULL;
	s_pModData = NULL;
}




TEST_F(BlockManTest, Case2_test_ModInBits1)
{
	printf("test modify data in bits(nSizeInBits%%8 != 0 && nOffInBits%%8 != 0)\n\n");

	ASSERT(true == IsKylinRunning());

	
	int nWBufSize = 2048;
	uint64 datablkId;
	BlockRequest WReq; 

	s_pWriteData = (char*)malloc(nWBufSize);
	ASSERT(NULL != s_pWriteData);
	memset(s_pWriteData, 0, nWBufSize);
	
	s_WCond.Init(1);

	
	datablkId = MakeDataBlockId(1, 1, 2);
	ZeroMemory(&WReq, sizeof(WReq));
	s_BlkMan.WriteBlock(datablkId, s_pWriteData, nWBufSize, 0, true, &s_BlockWriter, &WReq);
	s_WCond.Wait();

	
	int nMBufSize = 5;
	char MBuf[nMBufSize];
	s_pModData = MBuf;
	memset(MBuf, 0xFF, nMBufSize);

	int nSizeInBits = 35;
	int nOffInBits = 1024 * 8 + 4;
	BlockRequest MReq;

	
	s_MCond.Init(1);
	ZeroMemory(&MReq, sizeof(MReq));
	s_BlkMan.ModifyBlock(datablkId, MBuf, nSizeInBits, nOffInBits, &s_BlockModder, &MReq);
	s_MCond.Wait();

	
	int nRBufSize = nWBufSize;
	s_pReadData = (char*)malloc(nRBufSize);
	ASSERT(NULL != s_pReadData);
	BlockRequest RReq;
	
	
	s_RCond.Init(1);
	ZeroMemory(&RReq, sizeof(RReq));
	s_BlkMan.ReadBlock(datablkId, s_pReadData, nRBufSize, 0, true, &s_BlockReader, &RReq);
	s_RCond.Wait();

	
	ASSERT(((char)0x00 == *(s_pReadData+1023))&&
	       ((char)0xF0 == *(s_pReadData+1024))&&
	       ((char)0xFF == *(s_pReadData+1025))&&
	       ((char)0xFF == *(s_pReadData+1026))&&
	       ((char)0xFF == *(s_pReadData+1027))&&
	       ((char)0x7F == *(s_pReadData+1028)));

	
	free(s_pWriteData);
	free(s_pReadData);
	s_pReadData = NULL;
	s_pWriteData = NULL;
	s_pModData = NULL;
}




TEST_F(BlockManTest, Case3_test_ModInBits2)
{
	printf("test modify data in bits(nSizeInBits%%8 == 0 && nOffInBits%%8 != 0)\n\n");

	ASSERT(true == IsKylinRunning());

	
	int nWBufSize = 2048;
	uint64 datablkId;
	BlockRequest WReq; 

	s_pWriteData = (char*)malloc(nWBufSize);
	ASSERT(NULL != s_pWriteData);
	memset(s_pWriteData, 0, nWBufSize);
	
	s_WCond.Init(1);

	
	datablkId = MakeDataBlockId(1, 1, 3);
	ZeroMemory(&WReq, sizeof(WReq));
	s_BlkMan.WriteBlock(datablkId, s_pWriteData, nWBufSize, 0, true, &s_BlockWriter, &WReq);
	s_WCond.Wait();

	
	int nMBufSize = 5;
	char MBuf[nMBufSize];
	s_pModData = MBuf;
	memset(MBuf, 0xFF, nMBufSize);

	int nSizeInBits = nMBufSize * 8;
	int nOffInBits = 1024 * 8 + 3;
	BlockRequest MReq;

	
	s_MCond.Init(1);
	ZeroMemory(&MReq, sizeof(MReq));
	s_BlkMan.ModifyBlock(datablkId, MBuf, nSizeInBits, nOffInBits, &s_BlockModder, &MReq);
	s_MCond.Wait();

	
	int nRBufSize = nWBufSize;
	s_pReadData = (char*)malloc(nRBufSize);
	ASSERT(NULL != s_pReadData);
	BlockRequest RReq;
	
	
	s_RCond.Init(1);
	ZeroMemory(&RReq, sizeof(RReq));
	s_BlkMan.ReadBlock(datablkId, s_pReadData, nRBufSize, 0, true, &s_BlockReader, &RReq);
	s_RCond.Wait();

	
	ASSERT(((char)0x00 == *(s_pReadData+1023))&&
	       ((char)0xF8 == *(s_pReadData+1024))&&
	       ((char)0xFF == *(s_pReadData+1025))&&
	       ((char)0xFF == *(s_pReadData+1026))&&
	       ((char)0xFF == *(s_pReadData+1027))&&
	       ((char)0xFF == *(s_pReadData+1028))&&
	       ((char)0x07 == *(s_pReadData+1029)));

	
	free(s_pWriteData);
	free(s_pReadData);
	s_pReadData = NULL;
	s_pWriteData = NULL;
	s_pModData = NULL;
}




TEST_F(BlockManTest, Case4_test_ModInBits3)
{
	printf("test modify data in bits(nSizeInBits%%8 != 0 && nOffInBits%%8 == 0)\n\n");

	ASSERT(true == IsKylinRunning());

	
	int nWBufSize = 2048;
	uint64 datablkId;
	BlockRequest WReq; 

	s_pWriteData = (char*)malloc(nWBufSize);
	ASSERT(NULL != s_pWriteData);
	memset(s_pWriteData, 0, nWBufSize);
	
	s_WCond.Init(1);

	
	datablkId = MakeDataBlockId(1, 1, 4);
	ZeroMemory(&WReq, sizeof(WReq));
	s_BlkMan.WriteBlock(datablkId, s_pWriteData, nWBufSize, 0, true, &s_BlockWriter, &WReq);
	s_WCond.Wait();

	
	int nMBufSize = 5;
	char MBuf[nMBufSize];
	s_pModData = MBuf;
	memset(MBuf, 0xFF, nMBufSize);

	int nSizeInBits = 35;
	int nOffInBits = 1024 * 8;
	BlockRequest MReq;

	
	s_MCond.Init(1);
	ZeroMemory(&MReq, sizeof(MReq));
	s_BlkMan.ModifyBlock(datablkId, MBuf, nSizeInBits, nOffInBits, &s_BlockModder, &MReq);
	s_MCond.Wait();

	
	int nRBufSize = nWBufSize;
	s_pReadData = (char*)malloc(nRBufSize);
	ASSERT(NULL != s_pReadData);
	BlockRequest RReq;
	
	
	s_RCond.Init(1);
	ZeroMemory(&RReq, sizeof(RReq));
	s_BlkMan.ReadBlock(datablkId, s_pReadData, nRBufSize, 0, true, &s_BlockReader, &RReq);
	s_RCond.Wait();

	
	ASSERT(((char)0x00 == *(s_pReadData+1023))&&
	       ((char)0xFF == *(s_pReadData+1024))&&
	       ((char)0xFF == *(s_pReadData+1025))&&
	       ((char)0xFF == *(s_pReadData+1026))&&
	       ((char)0xFF == *(s_pReadData+1027))&&
	       ((char)0x07 == *(s_pReadData+1028)));

	
	free(s_pWriteData);
	free(s_pReadData);
	s_pReadData = NULL;
	s_pWriteData = NULL;
	s_pModData = NULL;

	
	s_bStopKylin = true;
}

#undef NR_DISKS
