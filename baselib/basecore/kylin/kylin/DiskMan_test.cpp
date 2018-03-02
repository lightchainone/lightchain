#include "stdafx.h"
#include "Trace.h"
#include "Kylin.h"
#include "Async.h"
#include "DiskMan.h"
#include "UnitTest.h"

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif	

#define NR_DISKS 6

static bool s_bStartKylin = true; 
static bool s_bStopKylin = false; 

static char* s_ptestfile1 = "disk_test_file_1";
static char* s_ptestfile2 = "dist_test_file_2";
static inline void CreateTestFile();
static inline void RemoveTestFile();

static uint64 nRead = 0;
static uint64 nWrite = 0;

class CDiskTestClient : plclic CAsyncClient
{
plclic:

	CDiskTestClient(const char* name, CCond* RCond, CCond* WCond) {
		CAsyncClient::SetName(name);
        m_pRCond = RCond;
        m_pWCond = WCond;
	}

	char* GetName() { return m_name; }

private:

	void OnCompletion(AsyncContext* pContext)
	{
        DiskRequest* pReq = (DiskRequest*)pContext;
        switch(pReq->async.nAction) {
            case AA_READ:
				ASSERT(0 == pReq->async.nErrCode);
                printf("read  req[ %s, off=%-5lld, request=%-5d, xfered=%-5d] ack.\n", pReq->async.pClient->GetName(), pReq->off, pReq->request, pReq->xfered);
                m_pRCond->Signal();
                break;

            case AA_WRITE:
				ASSERT(0 == pReq->async.nErrCode);
                printf("write req[ %s, off=%-5lld, request=%-5d, xfered=%-5d] ack.\n", pReq->async.pClient->GetName(), pReq->off, pReq->request, pReq->xfered);
                m_pWCond->Signal();
                break;
        }
	}

private:
    CCond* m_pRCond;
    CCond* m_pWCond;
};


class DiskManTest {
plclic:

	void SetUp() {

		if(s_bStartKylin)
			ASSERT(APFE_OK == InitKylin(1, 0, NR_DISKS)); 

		ASSERT(true == IsKylinRunning());
	}

	void TearDown() {
		
		RemoveTestFile();

		ASSERT(true == IsKylinRunning());

		if(s_bStopKylin) {
			while(APFE_OK != StopKylin()) 
				Sleep(100);
		}
	}
};



TEST_F(DiskManTest, Case1_test_RW)
{
    s_bStartKylin = false;
    CCond RCond_1, WCond_1;
    
    CDiskTestClient Client1("disk_tester#1", &RCond_1, &WCond_1);
    
    DiskContext ctx1;

	
	CreateTestFile();
    
	
    
	ZeroMemory(&ctx1, sizeof(ctx1));
    ASSERT(APFE_OK == g_pDiskMan->Associate(1, s_ptestfile1, O_TRUNC|O_WRONLY|O_DIRECT, (CAsyncClient*)&Client1, &ctx1));

    
    int nBufSize1 = 4096; 
    char buf1[nBufSize1];
    DiskRequest req1;

    memset(buf1, '0', nBufSize1);
	ZeroMemory(&req1, sizeof(req1));
    req1.off = 0;
	
	WCond_1.Init(1);
    g_pDiskMan->Write(&ctx1, (void*)buf1, nBufSize1, &req1);
	WCond_1.Wait();

	
	ASSERT((uint64)nBufSize1 == ctx1.nCurOff);
	
    
    ASSERT(APFE_OK == g_pDiskMan->Deassociate(&ctx1));
    
	
	ZeroMemory(&ctx1, sizeof(ctx1));
    ASSERT(APFE_OK == g_pDiskMan->Associate(1, s_ptestfile1, O_WRONLY|O_DIRECT, (CAsyncClient*)&Client1, &ctx1));
    memset(buf1, '1', nBufSize1);
	ZeroMemory(&req1, sizeof(req1));
    req1.off = nBufSize1;
	WCond_1.Init(1);
    g_pDiskMan->Write(&ctx1, (void*)buf1, nBufSize1, &req1);
	WCond_1.Wait();
	ASSERT((uint64)nBufSize1*2 == ctx1.nCurOff);
    ASSERT(APFE_OK == g_pDiskMan->Deassociate(&ctx1));

	
	ZeroMemory(&ctx1, sizeof(ctx1));
    ASSERT(APFE_OK == g_pDiskMan->Associate(1, s_ptestfile1, O_RDONLY|O_DIRECT, (CAsyncClient*)&Client1, &ctx1));
    memset(buf1, 0, nBufSize1);
	ZeroMemory(&req1, sizeof(req1));
    req1.off = 0;
	RCond_1.Init(1);
    g_pDiskMan->Read(&ctx1, (void*)buf1, nBufSize1, &req1);
	RCond_1.Wait();
	for(int i = 0; i < nBufSize1; ++i)
		ASSERT('0' == buf1[i]);
    ASSERT(APFE_OK == g_pDiskMan->Deassociate(&ctx1));
	
	
	ZeroMemory(&ctx1, sizeof(ctx1));
    ASSERT(APFE_OK == g_pDiskMan->Associate(1, s_ptestfile1, O_RDONLY|O_DIRECT, (CAsyncClient*)&Client1, &ctx1));
    memset(buf1, 0, nBufSize1);
	ZeroMemory(&req1, sizeof(req1));
    req1.off = nBufSize1;
	RCond_1.Init(1);
    g_pDiskMan->Read(&ctx1, (void*)buf1, nBufSize1, &req1);
	RCond_1.Wait();
	for(int i = 0; i < nBufSize1; ++i)
		ASSERT('1' == buf1[i]);
    ASSERT(APFE_OK == g_pDiskMan->Deassociate(&ctx1));

	
	g_pDiskMan->GetXfered(&nRead, &nWrite);
	ASSERT(8192 == nRead);
	ASSERT(8192 == nWrite);
	
}


TEST_F(DiskManTest, Case2_test_RW)
{
    CCond RCond_1, WCond_1;
    CCond RCond_2, WCond_2;
    
    CDiskTestClient Client1("disk_tester#1", &RCond_1, &WCond_1);
    CDiskTestClient Client2("disk_tester#2", &RCond_2, &WCond_2);
    
    DiskContext ctx1, ctx2;

	CreateTestFile();

    
	ZeroMemory(&ctx1, sizeof(ctx1));
	ZeroMemory(&ctx2, sizeof(ctx2));
    ASSERT(APFE_OK == g_pDiskMan->Associate(1, s_ptestfile1, O_WRONLY|O_DIRECT, (CAsyncClient*)&Client1, &ctx1));
    ASSERT(APFE_OK == g_pDiskMan->Associate(2, s_ptestfile2, O_WRONLY|O_DIRECT, (CAsyncClient*)&Client2, &ctx2));

    
    int nBufSize1 = 1024; 
    int nBufSize2 = 1024; 
    char buf1[nBufSize1];
    char buf2[nBufSize2];
    DiskRequest req1, req2;

    memset(buf1, '0', nBufSize1);
    memset(buf2, '1', nBufSize2);
	ZeroMemory(&req1, sizeof(req1));
	ZeroMemory(&req2, sizeof(req2));
    req1.off = 0;
	req2.off = 2048;
	
	WCond_1.Init(1);
	WCond_2.Init(1);
    g_pDiskMan->Write(&ctx1, (void*)buf1, nBufSize1, &req1);
    g_pDiskMan->Write(&ctx2, (void*)buf2, nBufSize2, &req2);
	WCond_1.Wait();
	WCond_2.Wait();

	
	ASSERT(1024 == ctx1.nCurOff);
	ASSERT(3072 == ctx2.nCurOff);

	
    ASSERT(APFE_OK == g_pDiskMan->Deassociate(&ctx1));
    ASSERT(APFE_OK == g_pDiskMan->Deassociate(&ctx2));

	
	ZeroMemory(&ctx1, sizeof(ctx1));
	ZeroMemory(&ctx2, sizeof(ctx2));
    ASSERT(APFE_OK == g_pDiskMan->Associate(1, s_ptestfile1, O_RDONLY|O_DIRECT, (CAsyncClient*)&Client1, &ctx1));
    ASSERT(APFE_OK == g_pDiskMan->Associate(2, s_ptestfile2, O_RDONLY|O_DIRECT, (CAsyncClient*)&Client2, &ctx2));

    memset(buf1, '0', nBufSize1);
    memset(buf2, '1', nBufSize2);
	ZeroMemory(&req1, sizeof(req1));
	ZeroMemory(&req2, sizeof(req2));
    req1.off = 0;
	req2.off = 2048;
	
	RCond_1.Init(1);
	RCond_2.Init(1);
    g_pDiskMan->Read(&ctx1, (void*)buf1, nBufSize1, &req1);
    g_pDiskMan->Read(&ctx2, (void*)buf2, nBufSize2, &req2);
	RCond_1.Wait();
	RCond_2.Wait();
	
	
	for(int i = 0; i < 1024; ++i) {
		ASSERT('0' == buf1[i]);
		ASSERT('1' == buf2[i]);
	}
	
	
    ASSERT(APFE_OK == g_pDiskMan->Deassociate(&ctx1));
    ASSERT(APFE_OK == g_pDiskMan->Deassociate(&ctx2));

	
	g_pDiskMan->GetXfered(&nRead, &nWrite);
	ASSERT(8192+2048 == nRead);
	ASSERT(8192+2048 == nWrite);

	

	
    s_bStopKylin = true;
}


static inline void CreateTestFile()
{
	int fd1 = open(s_ptestfile1, O_CREAT|O_TRUNC|O_RDWR, S_IRWXU|S_IRWXG|S_IRWXO);
	int fd2 = open(s_ptestfile2, O_CREAT|O_TRUNC|O_RDWR, S_IRWXU|S_IRWXG|S_IRWXO);
	ASSERT(-1 != fd1);
	ASSERT(-1 != fd2);
	close(fd1);
	close(fd2);
}


static inline void RemoveTestFile()
{
	unlink(s_ptestfile1);
	unlink(s_ptestfile2);
}
