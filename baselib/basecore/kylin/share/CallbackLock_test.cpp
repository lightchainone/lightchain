#include "stdafx.h"
#include "CallbackLock.h"
#include "UnitTest.h"

class LockTest {
  plclic:
    void SetUp() {
    }
    void TearDown() {
    }
};

struct LockReq {
    DLINK link;
    TTokenLock<LockReq>::TOKEN_CALLBACK fCallback;
    bool bOwner;
    bool bRead;
    int n;
    int nPriority;
};

int g_n = -1;

void LockAcquired(LockReq* pReq, ptr) 
{
    g_n = pReq->n;
    printf("%d acquired\n", pReq->n);
}

void LockReleased(LockReq* pReq, ptr)
{
    ASSERT(pReq->n == g_n);
    printf("%d released\n", pReq->n);
}

TEST_F(LockTest, Case1) 
{
    TTokenLock<LockReq> lock(1);
    lock.SetCallback(LockAcquired, LockReleased, NULL);

    LockReq req[10];
    ZeroMemory(req, sizeof(req));
    for (int i=0; i<10; i++) {
        req[i].n = i;
    }

    for (int i=0; i<10; i++) {
        lock.Acquire(0, &req[i]);
    }

    for (int i=0; i<10; i++) {
        lock.Release(&req[i]);
    }
}

#define NR_TOKEN        4
#include <set>
typedef std::set<int> IntSet;
IntSet g_IntSet;
void LockAcquired2(LockReq* pReq, ptr) 
{
    std::pair<IntSet::iterator, bool> pr;
    pr = g_IntSet.insert(pReq->n);
    ASSERT(pr.second);
    ASSERT(g_IntSet.size() <= NR_TOKEN);
    printf("%d acquired\n", pReq->n);
}

void LockReleased2(LockReq* pReq, ptr)
{
    IntSet::iterator it = g_IntSet.find(pReq->n);
    ASSERT(it != g_IntSet.end());
    g_IntSet.erase(it);
    printf("%d released\n", pReq->n);
}

TEST_F(LockTest, Case2) 
{
    TTokenLock<LockReq> lock(NR_TOKEN);
    lock.SetCallback(LockAcquired2, LockReleased2, NULL);

    LockReq req[NR_TOKEN+5];
    ZeroMemory(req, sizeof(req));
    for (int i=0; i<NR_TOKEN+5; i++) {
        req[i].n = i;
    }

    for (int i=0; i<NR_TOKEN+5; i++) {
        lock.Acquire(0, &req[i]);
    }

    ASSERT(g_IntSet.size() == NR_TOKEN);
    for (int i=0; i<NR_TOKEN+5; i++) {
        lock.Release(&req[i]);
    }
}

void LockAcquired3(LockReq* pReq, TTokenLock<LockReq>* pLock) 
{
    
    printf("%d acquired\n", pReq->n);
}

void LockReleased3(LockReq* pReq, ptr)
{
    
    
    
    printf("%d released\n", pReq->n);
}

TEST_F(LockTest, TokenCase) 
{
    TTokenLock<LockReq> lock(12);
    lock.SetCallback((TTokenLock<LockReq>::TOKEN_CALLBACK)LockAcquired3, LockReleased3, &lock);

    LockReq req[150];
    ZeroMemory(req, sizeof(req));
    for (int i=0; i<150; i++) {
        req[i].n = i;
    }

    for (int i=0; i<150; i++) {
        printf("try acquire for %d, pending: %d\n", i, lock.GetPendingCount(0));
        lock.Acquire(0, &req[i]);
    }

    
}


bool g_bRead = true;

void RLockAcquired(LockReq* pReq, ptr) 
{
    ASSERT(g_bRead);
    printf("%d acquired R\n", pReq->n);
}

void RLockReleased(LockReq* pReq, ptr)
{
    ASSERT(g_bRead);
    printf("%d released R\n", pReq->n);
}

void WLockAcquired(LockReq* pReq, ptr) 
{
    ASSERT(g_bRead);
    g_bRead = false;
    printf("%d acquired W\n", pReq->n);
}

void WLockReleased(LockReq* pReq, ptr)
{
    ASSERT(!g_bRead);
    printf("%d released W\n", pReq->n);
    g_bRead = true;
}

TEST_F(LockTest, Case3) 
{
    TRWLock<LockReq> lock;
    lock.SetCallback(RLockAcquired, RLockReleased, WLockAcquired, WLockReleased, NULL);

    LockReq req[12];
    ZeroMemory(req, sizeof(req));
    for (int i=0; i<12; i++) {
        req[i].n = i;
    }

    for (int i=0; i<2; i++) {
        lock.ReadLock(&req[i]);
    }
    lock.WriteLock(&req[10]);
    for (int i=2; i<4; i++) {
        lock.ReadLock(&req[i]);
    }
    lock.WriteLock(&req[11]);
    for (int i=4; i<6; i++) {
        lock.ReadLock(&req[i]);
    }

    for (int i=0; i<2; i++) {
        lock.ReadUnlock(&req[i]);
    }
    lock.WriteUnlock(&req[10]);
    for (int i=2; i<4; i++) {
        lock.ReadUnlock(&req[i]);
    }
    lock.WriteUnlock(&req[11]);
    for (int i=4; i<6; i++) {
        lock.ReadUnlock(&req[i]);
    }
}

void WriteProc(ptr)
{
    printf("write n=%d\n", g_n);
}

TEST_F(LockTest, Case4) 
{
    TRWLock<LockReq> lock(false, WriteProc, NULL, true);
    lock.SetCallback(RLockAcquired, RLockReleased, NULL, NULL, NULL);

    LockReq req[12];
    ZeroMemory(req, sizeof(req));
    for (int i=0; i<12; i++) {
        req[i].n = i;
    }

    g_n = 0;
    for (int i=0; i<2; i++) {
        lock.ReadLock(&req[i]);
    }
    g_n ++;
    lock.TriggerDelegate();
    for (int i=2; i<4; i++) {
        lock.ReadLock(&req[i]);
    }
    g_n ++;
    lock.TriggerDelegate();
    for (int i=4; i<6; i++) {
        lock.ReadLock(&req[i]);
    }
    g_n ++;
    lock.TriggerDelegate();
    for (int i=0; i<6; i++) {
        lock.ReadUnlock(&req[i]);
    }
}

bool g_bFlag = false;
int g_nCase5 = 0;
void OnReadable(LockReq* pReq, ptr p) 
{
    TRWLock<LockReq>* pLock = (TRWLock<LockReq>*)p;
    TRACE0("OnReadable: %p, %d\n", pReq, pReq->n);
    if (g_bFlag) {
        pLock->ReadUnlock(pReq);
    }
    g_nCase5 = pReq->n;
}

void OnWritable(LockReq* pReq, ptr p) 
{
    TRWLock<LockReq>* pLock = (TRWLock<LockReq>*)p;
    TRACE0("OnWritable: %p, %d\n", pReq, pReq->n);
    g_bFlag = true;
    pLock->WriteUnlock(pReq);
}

TEST_F(LockTest, Case5) 
{
    TRWLock<LockReq> lock(true);
    lock.SetCallback(OnReadable, NULL, OnWritable, NULL, &lock);

    LockReq req[2002];
    ZeroMemory(req, sizeof(req));
    for (int i=0; i<2002; i++) {
        req[i].n = i;
    }
    
    for (int i=0; i<10; i++) {
        lock.ReadLock(&req[i]);
    }
    lock.WriteLock(&req[2000]);
    lock.WriteLock(&req[2001]);

    for (int i=1000; i<1020; i++) {
        lock.ReadLock(&req[i]);
    }

    for (int i=0; i<10; i++) {
        lock.ReadUnlock(&req[i]);
    }

    ASSERT_EQUAL(g_nCase5, 1019);
}

void OnWritable2(LockReq* pReq, ptr p) 
{
    TRWLock<LockReq>* pLock = (TRWLock<LockReq>*)p;
    TRACE0("OnWritable: %p, %d\n", pReq, pReq->n);
}

TEST_F(LockTest, Case_Test_multiple_WriteLock) 
{
    TRWLock<LockReq> lock(true);
    lock.SetCallback(OnReadable, NULL, OnWritable2, NULL, &lock);

    LockReq req[2001];
    ZeroMemory(req, sizeof(req));
    for (int i=0; i<2001; i++) {
        req[i].n = i;
    }

    lock.WriteLock(&req[0]);
    lock.WriteLock(&req[1]);

    for (int i=0; i<2; i++) {
        lock.WriteUnlock(&req[i]);
    }
}
