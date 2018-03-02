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

static CCond s_RCond, s_WCond, s_FCond, s_DCond;

static bool s_bStartKylin = true; 
static bool s_bStopKylin = false;

const static int s_nFuncBufLen = 16;
static char FuncReadBuf[s_nFuncBufLen];
static char FuncWriteBuf[s_nFuncBufLen];


static inline void PrintWriteAckReq(BlockRequest* pReq)
{
	ASSERT(AA_WRITE_BLOCK == pReq->async.nAction);
	printf("write  req[id = %llx, disk = %d, len = %-4d, off = %-5d] ack.\n", pReq->id, ((uint8)pReq->id), pReq->len, pReq->off);
	s_WCond.Signal();
}



static inline void PrintFuncAckReq(BlockRequest* pReq)
{
	ASSERT(AA_FUNC_BLOCK == pReq->async.nAction);
	printf("func   req[id = %llx, disk = %d, len = XXXX, off = %-5d] ack.\n", pReq->id, ((uint8)pReq->id), pReq->off);
	s_FCond.Signal();
}



static inline void PrintReadAckReq(BlockRequest* pReq)
{
	ASSERT(AA_READ_BLOCK == pReq->async.nAction);
	printf("read   req[id = %llx, disk = %d, len = %-4d, off = %-5d] ack.\n", pReq->id, ((uint8)pReq->id), pReq->len, pReq->off);
	s_RCond.Signal();
}


static inline void PrintDelAckReq(BlockRequest* pReq)
{
	ASSERT(AA_DELETE_BLOCK == pReq->async.nAction);
	printf("delete req[id = %llx, disk = %d, len = XXXX, off = XXXXX] ack.\n", pReq->id, ((uint8)pReq->id));
	s_DCond.Signal();
}

static CBlockTestClient s_BlockWriter("BlockWriter", PrintWriteAckReq);
static CBlockTestClient s_BlockFuncer("BlockFuncer", PrintFuncAckReq);
static CBlockTestClient s_BlockReader("BlockReader", PrintReadAckReq);
static CBlockTestClient s_BlockDeleter("BlockDeleter", PrintDelAckReq);

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



static int BlockFunc(int fd, BlockRequest* pReq, int* pnRead, int* pnWrite)
{
	int len = 0;
	ASSERT(NULL != pReq);
	ASSERT(NULL != pnRead);
	ASSERT(NULL != pnWrite);

	
	lseek(fd, pReq->off, SEEK_SET);
	len = read(fd, FuncReadBuf, s_nFuncBufLen);	
	if(len > 0)
		*pnRead = len;
	else {
		PERROR("BlockFunc");
		return errno;
	}

	
	lseek(fd, pReq->off, SEEK_SET);
	len = write(fd, FuncWriteBuf, s_nFuncBufLen);
	if(len > 0)
		*pnWrite = len;
	else {
		PERROR("BlockFunc");
		return errno;
	}

	return 0;
}


TEST_F(BlockManTest, Case1_test_Func_and_Del)
{
	printf("---------- test BlockMan Func&&Delete ----------\n\n");

	
	ASSERT(true == IsKylinRunning());
	s_bStartKylin = false;
	
	
	int nWBufSize = 2048;
	uint64 logblkId;
	BlockRequest WReq; 

	char* pWriteData = (char*)malloc(nWBufSize);
	ASSERT(NULL != pWriteData);
	memset(pWriteData, 0, nWBufSize);
	
	s_WCond.Init(1);

	
	logblkId = MakeLogBlockId(1, 1, 1, 1);

	ASSERT(false == IsBlockExist(s_NewRootDir, logblkId));

	ZeroMemory(&WReq, sizeof(WReq));
	s_BlkMan.WriteBlock(logblkId, pWriteData, nWBufSize, 0, true, &s_BlockWriter, &WReq);

	s_WCond.Wait();

	
	ASSERT(true == IsBlockExist(s_NewRootDir, logblkId));

	
	int nOff = 1024;	
	memset(FuncReadBuf, '*', s_nFuncBufLen);
	memset(FuncWriteBuf, '*', s_nFuncBufLen);
	BlockRequest FReq;

	s_FCond.Init(1);
	ZeroMemory(&FReq, sizeof(FReq));
	s_BlkMan.FuncBlock(logblkId, nOff, BlockFunc, &s_BlockFuncer, &FReq);
	s_FCond.Wait();

	
	for(int i = 0; i < s_nFuncBufLen; ++i)
		ASSERT((char)0x00 == FuncReadBuf[i]);
	
	
	int nRBufSize = nWBufSize;
	BlockRequest RReq; 

	char* pReadData = (char*)malloc(nRBufSize);
	ASSERT(NULL != pReadData);
	memset(pReadData, 0, nRBufSize);

	s_RCond.Init(1);
	ZeroMemory(&RReq, sizeof(RReq));
	s_BlkMan.ReadBlock(logblkId, pReadData, nRBufSize, 0, true, &s_BlockReader, &RReq);

	s_RCond.Wait();

	
	ASSERT((char)0x00 == *(pReadData+1023)&&
		   (0 == strncmp(FuncWriteBuf, pReadData+1024, s_nFuncBufLen))&&
		   ((char)0x00 == *(pReadData+40)));

	
	ASSERT(true == IsBlockExist(s_NewRootDir,logblkId));

	BlockRequest DReq;
	ZeroMemory(&DReq, sizeof(DReq));
	DReq.id = logblkId;
	
	s_DCond.Init(1);
	s_BlkMan.DeleteBlock(&s_BlockDeleter, &DReq);
	s_DCond.Wait();

	
	ASSERT(false == IsBlockExist(s_NewRootDir, logblkId));

	
	free(pWriteData);
	free(pReadData);

	
	s_bStopKylin = true;
}

#undef NR_DISKS
