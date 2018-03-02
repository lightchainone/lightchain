
#ifndef _CALLBACK_LOCK_H_
#define _CALLBACK_LOCK_H_

#include "CompQ.h"
#include "atomic.h"


template <class TReq>
class TTokenLock
{
plclic:
    typedef void (*TOKEN_CALLBACK)(TReq*, ptr);

    TTokenLock(int nMaxToken, bool bVerifyOwner=false) {
        m_nCurToken = 0;
        m_nMaxToken = nMaxToken;
        m_bVerifyOwner = bVerifyOwner;
        m_fAcquired = m_fReleased = NULL;
    }
    ~TTokenLock() {}
    void SetCallback(TOKEN_CALLBACK fAcquired, TOKEN_CALLBACK fReleased, ptr pCtx) {
        ASSERT(NULL != fAcquired);
        m_fAcquired = fAcquired;
        m_fReleased = fReleased;
        m_pCtx = pCtx;
    }
    void SetMaxToken(int nMaxToken) {
        m_nMaxToken = nMaxToken;
    }

    bool Acquire(int nPriority, TReq* pReq) {
        ASSERT(NULL != pReq);
        ASSERT(!pReq->bOwner);
        ASSERT(NULL != m_fAcquired);
        
        int n = (0 == nPriority) ? 0 : 1;
        if (m_nCurToken < m_nMaxToken) {
            m_nCurToken ++;
            pReq->bOwner = true;
            pReq->nPriority = n;
            m_fAcquired(pReq, m_pCtx);
            return true;
        }
        pReq->bOwner = false;
        pReq->nPriority = n;
        m_PendingQ[n].push_back(pReq);
        return false;
    }

    
    bool Release(TReq* pReq) {
        ASSERT(!m_bVerifyOwner || NULL!=pReq);
        if (m_bVerifyOwner && !pReq->bOwner) {
            return false;
        }
        if (pReq) {
            ASSERT(0==pReq->nPriority || 1==pReq->nPriority);
            pReq->bOwner = false;
            if (m_fReleased) {
                m_fReleased(pReq, m_pCtx);
            }
        }

        ASSERT(0 < m_nCurToken);
        if (NULL == (pReq=m_PendingQ[0].pop_front())) {
            pReq = m_PendingQ[1].pop_front();
        }
        if (NULL != pReq) {
            pReq->bOwner = true;
            m_fAcquired(pReq, m_pCtx);
        }
        else {
            m_nCurToken --;
        }
        return true;
    }
    
    
    
    bool Cancel(TReq* pReq) {
        ASSERT(NULL != pReq);
        if (pReq->bOwner) {
            return false;
        }
        ASSERT(0==pReq->nPriority || 1==pReq->nPriority);
        m_PendingQ[pReq->nPriority].remove(pReq);
        return true;
    }

    typedef bool (*ENUMERATOR)(TReq* pReq, ptr);
    bool EnumPendingQ(ENUMERATOR f, ptr p) {
        if (m_PendingQ[0].Enumerate(f, p)) {
            return m_PendingQ[1].Enumerate(f, p);
        }
        return false;
    }

    int GetPendingCount(int nPriority) const {
        return m_PendingQ[nPriority].size();
    }

    int GetAvailableToken() const {
        return m_nMaxToken - m_nCurToken;
    }

private:
    bool m_bVerifyOwner;                    
    int m_nMaxToken;                        
    int m_nCurToken;                        
    int m_nMaxPriority;                     
    TLinkedList<TReq> m_PendingQ[2];        
    TOKEN_CALLBACK m_fAcquired;             
    TOKEN_CALLBACK m_fReleased;             
    ptr m_pCtx;                             
};


template <typename TReq>
class TRWLock
{
plclic:
    typedef void (*LOCK_CALLBACK)(TReq*, ptr);
    typedef void (*WRITE_DELEGATE)(ptr);

    TRWLock(bool bVerifyOwner=false, WRITE_DELEGATE fProc=NULL, 
            ptr pDelegateCtx=NULL, bool bTriggered=false
    ) {
        m_pWritingReq = NULL;
        m_fWriteProc = fProc;
        m_pWriteCtx = pDelegateCtx;
        m_bWriteNotify = m_fWriteProc ? (bTriggered?1:0) : 0;
        m_bVerifyOwner = bVerifyOwner;
        m_nReading = 0;
    }
    ~TRWLock() {}
    void SetCallback(LOCK_CALLBACK fRLockAcquired, LOCK_CALLBACK fRLockReleased,
                    LOCK_CALLBACK fWLockAcquired, LOCK_CALLBACK fWLockReleased, ptr pCtx
    ) {
        ASSERT(NULL != fRLockAcquired);
        ASSERT(NULL!=fWLockAcquired || NULL!=m_fWriteProc);

        m_fRLockAcquired = fRLockAcquired;
        m_fRLockReleased = fRLockReleased;
        m_fWLockAcquired = fWLockAcquired;
        m_fWLockReleased = fWLockReleased;
        m_pCtx = pCtx;
    }

    void TriggerDelegate() {
        AtomicSetValue(m_bWriteNotify, m_fWriteProc ? 1 : 0);
    }

    
    bool ReadLock(TReq* pReq) {
        ASSERT(NULL != pReq);
        ASSERT(!pReq->bOwner);

        pReq->bRead = true;
        
        if (0 == m_nReading) {
            if (1==AtomicSetValueIf(m_bWriteNotify, 0, 1)) {
                m_fWriteProc(m_pWriteCtx);
            }
        }
        if (0==AtomicGetValue(m_bWriteNotify) && NULL==m_pWritingReq) {
            m_nReading ++;
            pReq->bOwner = true;
            m_fRLockAcquired(pReq, m_pCtx);
            return true;
        }
        pReq->bOwner = false;
        m_PendingQ.push_back(pReq);
        return false;
    }

    
    bool WriteLock(TReq* pReq) {
        ASSERT(NULL != pReq);
        ASSERT(!pReq->bOwner);
        ASSERT(NULL == m_fWriteProc);
        ASSERT(NULL != m_fWLockAcquired);

        pReq->bRead = false;
        if (NULL == m_pWritingReq) {
            m_pWritingReq = pReq;

            if (0 == m_nReading) {
                pReq->bOwner = true;
                m_fWLockAcquired(pReq, m_pCtx);
                return true;
            }
        }
        pReq->bOwner = false;
        m_PendingQ.push_back(pReq);
        return false;
    }

    
    
    bool ReadUnlock(TReq* pReq) {
        ASSERT(!m_bVerifyOwner || NULL!=pReq);
        if (m_bVerifyOwner && !pReq->bOwner) {
            return false;
        }
        if (pReq) {
            pReq->bOwner = false;
            if (m_fRLockReleased) {
                m_fRLockReleased(pReq, m_pCtx);
            }
        }
        m_nReading --;
        ASSERT(0 <= m_nReading);
        if (0 == m_nReading) {
            m_PendingQ.Enumerate(EnumCallback, this);
        }
        return true;
    }

    
    
    bool WriteUnlock(TReq* pReq) {
        ASSERT(!m_bVerifyOwner || NULL!=pReq);
        if (m_bVerifyOwner && pReq!=m_pWritingReq) {
            return false;
        }
        if (pReq) {
            pReq->bOwner = false;
            if (m_fWLockReleased) {
                m_fWLockReleased(pReq, m_pCtx);
            }
        }
        m_pWritingReq = NULL;
        m_PendingQ.Enumerate(EnumCallback, this);
        return true;
    }

    
    bool Cancel(TReq* pReq) {
        ASSERT(NULL != pReq);
        if (pReq->bOwner) {
            return false;
        }
        m_PendingQ.remove(pReq);
        return true;
    }

    typedef bool (*ENUMERATOR)(TReq* pReq, ptr);
    bool EnumPendingQ(ENUMERATOR f, ptr p) {
        return m_PendingQ.Enumerate(f, p);
    }

    int GetNumberOfReading() const {
        return m_nReading;
    }

private:
    TLinkedList<TReq> m_PendingQ;       
    TReq* m_pWritingReq;                
    WRITE_DELEGATE m_fWriteProc;        
    volatile int m_bWriteNotify;        
    bool m_bVerifyOwner;                
    int m_nReading;                     
    LOCK_CALLBACK m_fRLockAcquired;     
    LOCK_CALLBACK m_fRLockReleased;     
    LOCK_CALLBACK m_fWLockAcquired;     
    LOCK_CALLBACK m_fWLockReleased;     
    ptr m_pWriteCtx;                    
    ptr m_pCtx;                         

    bool ProcReq(TReq* pReq) {
        if (pReq->bRead) {
            ASSERT(NULL == m_pWritingReq);
            m_PendingQ.remove(pReq);
            
            if (0 == m_nReading) {
                if (1==AtomicSetValueIf(m_bWriteNotify, 0, 1)) {
                    m_fWriteProc(m_pWriteCtx);
                }
            }
            
            m_nReading ++;
            pReq->bOwner = true;
            m_fRLockAcquired(pReq, m_pCtx);
            return true;
        }

        ASSERT(NULL == m_fWriteProc);
        ASSERT(NULL==m_pWritingReq || m_pWritingReq==pReq);
        m_pWritingReq = pReq;
        if (0 == m_nReading) {
            m_PendingQ.remove(pReq);
            pReq->bOwner = true;
            m_fWLockAcquired(pReq, m_pCtx);
        }
        return false;
    }

    static bool EnumCallback(TReq* pReq, ptr pData) {
        return ((TRWLock<TReq>*)pData)->ProcReq(pReq);
    }
};

#endif
