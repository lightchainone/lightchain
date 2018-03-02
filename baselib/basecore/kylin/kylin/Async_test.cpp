#include "stdafx.h"
#include "SyncObj.h"
#include "Kylin.h"
#include "Async.h"
#include "UnitTest.h"
#include "../test/KylinUtils4Test.h"

class AsyncTest {
plclic:
	void SetUp() {}
	void TearDown() {}
};

class CTestClient: plclic CAsyncClient {
plclic:
	void OnCompletion(AsyncContext* pCtx) {}
};


TEST_F(AsyncTest, Case1)
{
	CTestClient client;
	ColorPrintf(TC_Cyan, "client name = \"%s\".\n", client.GetName());
	
	client.SetName("TestClient#%d", 1);
	ColorPrintf(TC_Cyan, "client name = \"%s\".\n", client.GetName());

	
	char Name[64];
	char* p = "TestClient#2";
	memcpy((void*)(Name+64-1-strlen(p)), (void*)p, strlen(p));
	Name[63] = '\0';
	ASSERT(63 == strlen(Name));
	client.SetName(Name);
	ColorPrintf(TC_Cyan, "client name = \"%s\".\n", client.GetName());
	ASSERT(63 == strlen(client.GetName()));
	
	
	char Name2[65];
	char* p2 = "TestClient#3";
	memset((void*)Name2, '#', 65);
	memcpy((void*)(Name2+65-1-strlen(p2)), (void*)p2, strlen(p2));
	Name2[64] = '\0';
	client.SetName(Name2);
	ColorPrintf(TC_Cyan, "client name = \"%s\".\n", client.GetName());
	ASSERT(strlen("AsyncClient#0") == strlen(client.GetName()));
}

