#include "stdafx.h"
#include "BufHandle.h"
#include "TranBuf.h"
#include "ObjectPool.h"
#include "spinlock.h"
#include "BufQueue.h"

_KYLIN_PACKAGE_BEGIN_

#define LOCK            spin_lock(&m_lock)
#define UNLOCK          spin_unlock(&m_lock)

int s_nTranBuf = 1024;
int s_nBufSize = 4096;
int s_nBigTranBuf = 64;
float s_fLowMark = 0.6f;
float s_fHighMark = 0.9f;

void SetTranBuf(int nSmallNum, int nBigNum, int nSmallSize, float fLowMark, float fHighMark)
{
    LOCK_THIS_BLOCK;

    s_nTranBuf = nSmallNum;
    s_nBigTranBuf = nBigNum;
    s_nBufSize = nSmallSize;
    s_fLowMark = fLowMark;
    s_fHighMark = fHighMark;
}

class CBufHandlePool : plclic TObjectPool<BufHandle>
{
    volatile int m_lock;
    CTranBufPool m_TranBufPool;
    CTranBufPool m_BigBufPool;

    BufHandle* DoAllocate(CTranBufPool* pPool, int nRetry) {
        BufHandle* pHdl;

        for (int i=0; i<nRetry; i++) {
            LOCK;
            pHdl = TObjectPool<BufHandle>::Allocate();
            pHdl->pBuf = pPool->Allocate(i > 0 ? 2 : 1);
            if (NULL == pHdl->pBuf) {
                TObjectPool<BufHandle>::Free(pHdl);
                pHdl = NULL;
            }
            UNLOCK;
            if (NULL != pHdl)
                return pHdl;
            if (i > 1) {
                TRACE0("No enough memory, sleep %d\n", i+1);
            }
            sleep(1);
        }
        RaiseError(TODO_NO_ENOUGH_MEMORY);
        return NULL;
    }

    BufHandle* DoAllocateCanFail(CTranBufPool* pPool, int nSize) {
        BufHandle* pHdl;
        int nBlockSize = pPool->GetBlockSize();
        ASSERT(0 != nSize);

        LOCK;
        pHdl = TObjectPool<BufHandle>::Allocate();
        if (nSize == nBlockSize) {
            pHdl->pBuf = pPool->Allocate(0);
        }
        else {
            pHdl->pBuf = pPool->Allocate(0, (nSize+nBlockSize-1) / nBlockSize);
        }
        if (NULL == pHdl->pBuf) {
            TObjectPool<BufHandle>::Free(pHdl);
            pHdl = NULL;
        }
        UNLOCK;
        return pHdl;
    }

    BufHandle* _DoAddRef(BufHandle* pHdl, BufHandle* pNext, BufHandle*** pppLast) {
        if (-1!=m_TranBufPool.AddRef(pHdl->pBuf) || -1!=m_BigBufPool.AddRef(pHdl->pBuf)) {
            BufHandle* pTmp = TObjectPool<BufHandle>::Allocate();
            pTmp->_next = pNext;
            pTmp->pBuf = pHdl->pBuf;
            pTmp->nBufLen = pHdl->nDataLen;
            pTmp->nDataLen = pHdl->nDataLen;
            *pppLast = &pTmp->_next;
            return pTmp;
        }
        return NULL;
    }

    void _DoFree(BufHandle* pHdl) {
        if (-1 == m_TranBufPool.Free(pHdl->pBuf))
            m_BigBufPool.Free(pHdl->pBuf);
        TObjectPool<BufHandle>::Free(pHdl);
    }

plclic:
    CBufHandlePool() : TObjectPool<BufHandle>("BufHandle", BUFPOOL_C2),
                       m_TranBufPool("TranBuffer", BUFPOOL_C1),
                       m_BigBufPool("BigBuffer", BUFPOOL_C1)
    {
        m_lock = 0;
        Create(1024, 1);

        int nAlloc = s_nTranBuf;
        int nMax = 1;
        while ((s_nBufSize/1024) * nAlloc > 524288) {   
            nAlloc >>= 1;
            nMax <<= 1;
        }
        m_TranBufPool.Create(s_nBufSize, nAlloc, 1, nMax, s_fLowMark, s_fHighMark);
        m_BigBufPool.Create(SZ_BIG_BUF, s_nBigTranBuf, 0, 10, 0.9, 0.9);
    }
    ~CBufHandlePool() {
    }
    BufHandle* AllocateBig(bool bCanFail) {
        BufHandle* pHdl;

        pHdl = bCanFail 
             ? DoAllocateCanFail(&m_BigBufPool, SZ_BIG_BUF) 
             : DoAllocate(&m_BigBufPool, 60);
        if (pHdl) {
            pHdl->_next = NULL;
            pHdl->nBufLen = SZ_BIG_BUF;
            pHdl->nDataLen = 0;
        }
        return pHdl;
    }

    BufHandle* AllocateCanFail(int nSize) {
        BufHandle* pHdl = DoAllocateCanFail(&m_TranBufPool, nSize);
        if (pHdl) {
            pHdl->_next = NULL;
            pHdl->nBufLen = nSize;
            pHdl->nDataLen = 0;
        }
        return pHdl;
    }

    BufHandle* AllocForBuf(char* pBuf, int nLen, BufHandle* pNext, BufHandle*** pppLast) {
        BufHandle *pFirst, *pHdl, **ppLast;

        pFirst = NULL;
        ppLast = &pFirst;
        while (nLen > 0) {
            pHdl = DoAllocate(&m_TranBufPool, 120);

            pHdl->nBufLen = s_nBufSize;
            pHdl->nDataLen = nLen>s_nBufSize ? s_nBufSize : nLen;
            memcpy(pHdl->pBuf, pBuf, pHdl->nDataLen);
            pBuf += pHdl->nDataLen;
            nLen -= pHdl->nDataLen;

            pHdl->_next = pNext;
            *ppLast = pHdl;
            ppLast = &pHdl->_next;
        }
        if (pppLast) {
            *pppLast = ppLast;
        }
        return pFirst;
    }

    BufHandle* Allocate(bool bInPool=false, char* pBuf=NULL, 
                        int nLen=0, BufHandle* pNext=NULL
    ) {
        BufHandle* pHdl;

        UNLIKELY_IF (false == bInPool) {
            LOCK;
            pHdl = TObjectPool<BufHandle>::Allocate();
            if (-1 == m_TranBufPool.AddRef(pBuf))
                m_BigBufPool.AddRef(pBuf);
            UNLOCK;

            pHdl->_next = pNext;
            pHdl->pBuf = pBuf;
            pHdl->nBufLen = nLen;
            pHdl->nDataLen = nLen;
            return pHdl;
        }
        if (pBuf==NULL || nLen==0) {
            pHdl = DoAllocate(&m_TranBufPool, 120);

            pHdl->_next = pNext;
            pHdl->nBufLen = s_nBufSize;
            pHdl->nDataLen = nLen;
            return pHdl;
        }
        
        return AllocForBuf(pBuf, nLen, pNext, NULL);
    }

    int GetFreePercent() {
        int n;
        LOCK;
        n = m_TranBufPool.GetFreePercent();
        UNLOCK;
        return n;
    }

    void Free(BufHandle* pHdl) {
		ASSERT(NULL != pHdl);
        LOCK;
        _DoFree(pHdl);
        UNLOCK;
    }
    
    
    void ChainFree(BufHandle* pHdl, BufHandle* pNext) {
#ifdef _DEBUG
        if (NULL == pHdl) {
            ASSERT_EQUAL(pNext, (BufHandle*)NULL);
        }
#endif
        BufHandle *pTmp;
        LOCK;
        for ( ; pHdl!=pNext; pHdl=pTmp) {
            ASSERT(NULL != pHdl);
            pTmp = pHdl->_next;
            _DoFree(pHdl);
        }
        UNLOCK;
    }
    
    BufHandle* CloneAndTerminate(BufHandle* pHdl, BufHandle* pNext, 
                                int* pnLen, bool bCopyNonTranBuf
    ) {
        BufHandle *pFirst, *pTmp, **ppLast, **ppLastTmp;
        int nLen = 0;
        
        pFirst = NULL;
        ppLast = &pFirst;
        LOCK;
        for ( ; pHdl!=pNext; pHdl=pHdl->_next) {
            pTmp = _DoAddRef(pHdl, NULL, &ppLastTmp);
            if (NULL == pTmp) {
                if (bCopyNonTranBuf) {
                    UNLOCK;
                    pTmp = AllocForBuf(pHdl->pBuf, pHdl->nDataLen, NULL, &ppLastTmp);
                    LOCK;
                }
                else {
                    pTmp = TObjectPool<BufHandle>::Allocate();
                    pTmp->pBuf = pHdl->pBuf;
                    pTmp->nDataLen = pTmp->nBufLen = pHdl->nDataLen;
                    pTmp->_next = NULL;
                    ppLastTmp = &pTmp->_next;
                }
            }
            nLen += pHdl->nDataLen;
            *ppLast = pTmp;
            ppLast = ppLastTmp;
        }
        UNLOCK;

        if (pnLen) {
            *pnLen = nLen;
        }
        if (nLen) {
            return pFirst;
        }
        ChainFreeHdl(pFirst, NULL);
        return NULL;
    }

    void GarbageCollect() {
        TObjectPool<BufHandle>::GarbageCollect();
        LOCK;
        m_TranBufPool._GarbageCollect();
        m_BigBufPool._GarbageCollect();
        UNLOCK;
    }
    
	CTranBufPool* GetTranBufPool() { return &m_TranBufPool; }
	CTranBufPool* GetBigBufPool() { return &m_BigBufPool; }
};

static CBufHandlePool* s_pBufHandlePool = NULL;
static CBufHandlePool* GetBufHdlPool() 
{
    if (NULL != s_pBufHandlePool) {
        return s_pBufHandlePool;
    }
    else {
        LOCK_THIS_BLOCK;
        if (NULL == s_pBufHandlePool) {
            s_pBufHandlePool = new CBufHandlePool;
        }
        return s_pBufHandlePool;
    }
}

BufHandle* AllocateHdl(bool bInPool, char* pBuf, int nLen, BufHandle* pNext)
{
    return GetBufHdlPool()->Allocate(bInPool, pBuf, nLen, pNext);
}

BufHandle* AllocateHdlCanFail(int nSize)
{
    if (0==nSize || s_nBufSize==nSize) {
        return GetBufHdlPool()->AllocateCanFail(s_nBufSize);
    }

    BufHandle* pHdl;
    for (int i=0; i<2; i++) {
        pHdl = GetBufHdlPool()->AllocateCanFail(nSize);
        if (NULL != pHdl) {
            return pHdl;
        }
        nSize = s_nBufSize;
    }
    return NULL;
}

BufHandle* AllocateBigHdl()
{
    return GetBufHdlPool()->AllocateBig(false);
}

BufHandle* AllocateBigHdlCanFail()
{
    return GetBufHdlPool()->AllocateBig(true);
}

void FreeHdl(BufHandle* pHdl)
{
    GetBufHdlPool()->Free(pHdl);
}

void ChainFreeHdl(BufHandle* pHdl, BufHandle* pNext)
{
    GetBufHdlPool()->ChainFree(pHdl, pNext);
}

int GetFreeTranBufPercent()
{
    return GetBufHdlPool()->GetFreePercent();
}

BufHandle* CloneHdlAndTerminate(BufHandle* pHdl, BufHandle* pNext, int* pnLen, bool bCopyNonTranBuf)
{
    return GetBufHdlPool()->CloneAndTerminate(pHdl, pNext, pnLen, bCopyNonTranBuf);
}

BufHandle* CopyHdl(BufHandle* pHdl)
{
    if (pHdl == NULL)
        return NULL;

    BufHandle* pNext = CopyHdl(pHdl->_next);
    return AllocateHdl(true, pHdl->pBuf, pHdl->nDataLen, pNext);
}

BufHandle* SplitBufHdl(BufHandle *pHdl, int nNewLen)    
{                                                       
    BufHandle *pNext;
    ASSERT(nNewLen < pHdl->nDataLen && nNewLen!=0);

    pNext = AllocateHdl(false, pHdl->pBuf+nNewLen, pHdl->nDataLen-nNewLen);

    pHdl->nDataLen = nNewLen;

    pNext->_next = pHdl->_next;
    pHdl->_next = pNext;
    return pNext;
}

BufHandle* Reclaim(int nLen, BufHandle* pHdl, BufHandle* pNext)
{
    BufHandle* pTmp;

    for (pTmp=pHdl; pTmp!=pNext; pTmp=pTmp->_next) {
        if (pTmp->nBufLen >= pTmp->nDataLen+nLen) {
            pTmp->nBufLen -= nLen;
            pTmp = AllocateHdl(false, pTmp->pBuf+pTmp->nBufLen, nLen);
            return pTmp;
        }
    }
    return NULL;
}

BufHandle* vBufHdlPrintf(BufHandle* pNext, const char *fmt, va_list va)
{
    char* pBuf;
    int n, m, nRetry = 0;

    BufHandle* pHdl = AllocateHdl(true);
    pHdl->_next = pNext;
    pBuf = pHdl->pBuf;
    n = pHdl->nBufLen;

here:
    m = vsnprintf(
        pBuf,
        n,
        fmt,
        va
        );

    ASSERT(m != 0);
    if (m < n) {
        if (nRetry == 0) {
            pHdl->nDataLen = m;
            ASSERT(m <= pHdl->nBufLen);
        }
        else {
            pHdl = AllocateHdl(true, pBuf, m, pNext);
            free(pBuf);
        }
        return pHdl;
    }
    if (nRetry++ == 0) {
        FreeHdl(pHdl);
        pBuf = NULL;
    }
    n = m+1;
    if (pBuf) {
        free(pBuf);
    }
    pBuf = (char*)malloc(n);
    if (pBuf != NULL)
        goto here;
    return pNext;
}

BufHandle* BufHdlPrintf(BufHandle* pNext, const char *fmt, ...)
{
    va_list ap;
    BufHandle* pHdl;

    va_start(ap, fmt);
    pHdl = vBufHdlPrintf(pNext, fmt, ap);
    va_end(ap);

    return pHdl;












































}

BufHandle* LoadFileToBufHdl(FILE* fp)
{
    int n;
    BufHandle* pTmp;
    CBufQueue bq;

    if (fp && !feof(fp)) {
        for ( ; ; ) {
            pTmp = AllocateHdl(true);
            n = (int)fread(pTmp->pBuf, 1, pTmp->nBufLen, fp);
            if (n > 0) {
                pTmp->nDataLen = n;
                bq.Append(pTmp, NULL);
            }
            else {
                FreeHdl(pTmp);
                break;
            }
        }
    }
    return bq.GetAll();
}

int GetTotalTranBufNum()
{
	return GetBufHdlPool()->GetTranBufPool()->GetTotalBufNum();
}

int GetAllocatedTranBufNum()
{
	return GetBufHdlPool()->GetTranBufPool()->GetAllocatedBufNum();
}
	
int GetFreeTranBufNum()
{
	return GetBufHdlPool()->GetTranBufPool()->GetFreeBufNum();
}

#ifdef _UNITTEST

int GetTotalBigBufNum()
{
	return GetBufHdlPool()->GetBigBufPool()->GetTotalBufNum();
}

int GetAllocatedBigBufNum()
{
	return GetBufHdlPool()->GetBigBufPool()->GetAllocatedBufNum();
}

int GetFreeBigBufNum()
{
	return GetBufHdlPool()->GetBigBufPool()->GetFreeBufNum();
}

int GetFreeBufHdlNum() 
{
	return GetBufHdlPool()->GetFreeCount();
}

#endif

_KYLIN_PACKAGE_END_
