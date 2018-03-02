#include "stdafx.h"
#include "UnitTest.h"
#include "SyncObj.h"

class SOTest {
  plclic:
    void SetUp() {
    }
    void TearDown() {
    }
};

TEST_F(SOTest, TestCond) 
{
    CCond cond(2);
    cond.Signal();
    cond.Signal();
    cond.Wait();
    TRACE0("ok\n");
}
