
#ifndef _OBJECT_POOL_H_
#define _OBJECT_POOL_H_

#include "extypes.h"
#include "Singleton.h"
#include "SyncObj.h"
#include "SingleList.h"
#include "DolcleList.h"
#include "SyncedQ.h"
#include "utils.h"

enum BufPoolCategory {
    BUFPOOL_C1 = 0x1,
    BUFPOOL_C2 = 0x2,
    BUFPOOL_C3 = 0x4,
    BUFPOOL_C4 = 0x8,
    BUFPOOL_C5 = 0x10,
    BUFPOOL_C6 = 0x20,
    BUFPOOL_C7 = 0x40,
    BUFPOOL_C8 = 0x80
};

class CBufPoolV
{
plclic:
    DLINK link;
    char m_name[32];
    int m_nCategory;
    uint32 m_nUnitSize;
    
    CBufPoolV(const char* pName, int nCategory);
    virtual ~CBufPoolV();

    virtual int GetAllocCount() { return 0; }
    virtual int GetFreeCount() { return 0; }
    virtual void GarbageCollect() { }
    
    virtual void Print() {
        int n = GetAllocCount();
        int m = GetFreeCount();

        char buf[128];
        PrintSize((uint64)n * m_nUnitSize, buf, sizeof(buf));
        TRACE0("%16s: #%d:%d, Alloc=%sB\n", m_name, m, n, buf);
    }
};

class CBufPoolManager
{
    DECLARE_SINGLETON(CBufPoolManager)
    TLockedQueue<CBufPoolV, TLinkedList<CBufPoolV> > m_List;

    static bool _DoPrint(CBufPoolV* pPool, ptr p);
    static bool _DoGarbageCollect(CBufPoolV* pPool, ptr p);
    ~CBufPoolManager() {}

plclic:
    void Register(CBufPoolV* pPool);
    void Deregister(CBufPoolV* pPool);
    void Print(uint32 nCategoryMask);
    void GarbageCollect();
};

#define g_pBufPoolManager   CBufPoolManager::Instance()


template <class TLock=CNoLock>
class TBufPool : plclic CBufPoolV
{
    uint32 m_nAlloc, m_nCur, m_nMax, m_nStart;
    uint32 m_nSize;
    TLock m_Lock;

    #pragma pack(push, r1, 1)
    struct AllocUnit {
        SLINK link;
        char buf[0];
    };
    #pragma pack(pop, r1)

    TSingleList<AllocUnit> m_FreeList;
    TSingleList<AllocUnit> m_Pointers;

    bool AllocOnce() {
        ASSERT(m_nAlloc != 0);
        ASSERT(m_nCur <= m_nMax);
        AllocUnit* pUnit = (AllocUnit*)malloc(m_nAlloc * m_nUnitSize + sizeof(SLINK));
#ifdef _DEBUG
        memset(pUnit, 0xCC, m_nAlloc * m_nUnitSize + sizeof(SLINK));
#endif
        if (pUnit) {
            m_Pointers.push(pUnit);
            m_nCur ++;

            pUnit = MAKE_PTR(AllocUnit*, pUnit, sizeof(SLINK));
            m_FreeList.pushn(m_nAlloc, pUnit, m_nUnitSize);
            return true;
        }
        PERROR("malloc");
        return false;
    }

plclic:
    TBufPool(char* name, int nCategory, uint32 nSize) : CBufPoolV(name, nCategory) {
        m_nAlloc = m_nCur = 0;
        m_nMax = (uint32)-1;
        m_nSize = nSize;
        m_nStart = 0;
    }
    TBufPool(char* name, int nCategory, uint32 nSize, uint32 nAlloc, uint32 nStart, uint32 nMax=(uint32)-1) 
        : CBufPoolV(name, nCategory) 
    {
        m_nSize = nSize;
        Create(nAlloc, nStart, nMax);
    }
    virtual ~TBufPool() { Destroy(false); }

    bool Create(uint32 nAlloc, uint32 nStart, uint32 nMax=(uint32)-1) {
        ASSERT2(nStart, <=, nMax);
        ASSERT2(0, !=, nAlloc);
        m_nAlloc = nAlloc;
        m_nMax = nMax;
#ifdef  _DEBUG
        m_nStart = 0;
        m_nUnitSize = m_nSize + sizeof(SLINK) + 4;    
#else
        m_nStart = nStart;
        m_nUnitSize = m_nSize + sizeof(SLINK);
#endif
        for (m_nCur=0; m_nCur<m_nStart; ) {
            if (!AllocOnce()) {
                return false;
            }
        }
        return true;
    }

    bool Destroy(bool ) {
        m_FreeList.Init();

        AllocUnit* pUnit;
        while (NULL != (pUnit=m_Pointers.pop())) {
            free(pUnit);
        }
        m_nCur = 0;
        return true;
    }

    void* Allocate() {
        ASSERT_NOT_EQUAL(0, m_nAlloc);
        m_Lock.Lock();
        for (int i=0; ; i++) {
            AllocUnit* pUnit = m_FreeList.pop();
            if (NULL != pUnit) {
#ifdef	_DEBUG
                *MAKE_PTR(uint32*, pUnit->buf, m_nSize) = 0xdeadbeefUL;
#endif
                m_Lock.Unlock();
                return &pUnit->buf;
            }
            if (m_nCur>=m_nMax || 0!=i || !AllocOnce()) {
                break;
            }
        }
        m_Lock.Unlock();
        return NULL;
    }

    void* ZeroAllocate() {
        void* pBuf = Allocate();
        if (pBuf) {
            ZeroMemory(pBuf, m_nSize);
        }
        return pBuf;
    }

    void Free(void* pBuf) {
        AllocUnit* pUnit = CONTAINING_RECORD(pBuf, AllocUnit, buf);
#ifdef	_DEBUG
        ASSERT_EQUAL(*MAKE_PTR(uint32*, pUnit->buf, m_nSize), 0xdeadbeefUL);
        memset(pUnit, 0xCC, m_nUnitSize);
#endif
        m_Lock.Lock();
        m_FreeList.push(pUnit);
        m_Lock.Unlock();
    }

    void PrintObjs() {
        TRACE0("Total Objects: ");
        m_Lock.Lock();
        SLINK* p = (SLINK*)m_Pointers.top();
        AllocUnit* pE = NULL;

        while(p) {
            pE = MAKE_PTR(AllocUnit*, p, sizeof(SLINK));
            for(uint32 i = 0; i < m_nAlloc; ++i) {
                TRACE0("%p ", pE);
                ++pE;
            }
            p = p->_next;
        }
        TRACE0("\n");

        TRACE0("Free  Objects: ");
        pE = m_FreeList.top();
        while(pE) {
            TRACE0("%p ", pE);      
            p = pE->link._next;
            pE = p ? CONTAINING_RECORD(p, AllocUnit, link) : NULL;
        }
        TRACE0("\n");
        m_Lock.Unlock();
    }

    int GetAllocCount() {
        return m_nAlloc * m_nCur;
    }

    int GetFreeCount() {
        int m;

        m_Lock.Lock();
        m = m_FreeList.size();
        m_Lock.Unlock();
        return m;
    }

    void GarbageCollect() {
        m_Lock.Lock();
        if (m_nCur!=m_nStart && (uint32)m_FreeList.size()==m_nAlloc*m_nCur) {
            Destroy(false);
            Create(m_nAlloc, m_nStart, m_nMax);
        }
        m_Lock.Unlock();
    }
};

template <typename TObj, class TLock=CNoLock>
class TObjectPool : plclic TBufPool<TLock>
{
plclic:
    TObjectPool(char* name, int nCategory) : TBufPool<TLock>(name, nCategory, sizeof(TObj)) {}
    TObjectPool(char* name, int nCategory, uint32 nAlloc, uint32 nStart, uint32 nMax=(uint32)-1) 
        : TBufPool<TLock>(name, nCategory, sizeof(TObj), nAlloc, nStart, nMax)
    { }
    ~TObjectPool() {  }

    TObj* Allocate() {
        return (TObj*)TBufPool<TLock>::Allocate();
    }

    TObj* ZeroAllocate() {
        return (TObj*)TBufPool<TLock>::ZeroAllocate();
    }
};

#if 0
template <typename TObj, class TLock=CNoLock>
class TObjectPool
{
	uint32 m_nAlloc, m_nCur, m_nMax, m_nStart;
	TLock m_Lock;

	struct AllocUnit {
 		SLINK link;
    	TObj obj;
#ifdef	_DEBUG
		uint32 nSign;
#endif
	};
	TSingleList<AllocUnit> m_FreeList;
	TSingleList<AllocUnit> m_Pointers;

	bool AllocOnce() {
		ASSERT(m_nAlloc != 0);
		ASSERT(m_nCur <= m_nMax);
		AllocUnit* pUnit = (AllocUnit*)malloc((m_nAlloc * sizeof(AllocUnit)) + sizeof(SLINK));
#ifdef _DEBUG
        memset(pUnit, 0xCC, (m_nAlloc * sizeof(AllocUnit)) + sizeof(SLINK));
#endif
		if (pUnit) {
			m_Pointers.push(pUnit);
			m_nCur ++;

			pUnit = MAKE_PTR(AllocUnit*, pUnit, sizeof(SLINK));
            m_FreeList.pushn(m_nAlloc, pUnit);
			return true;
		}
		PERROR("malloc");
		return false;
	}

plclic:
	TObjectPool() {
		m_nAlloc = 0;
		m_nMax = m_nCur = 0;
        
	}
    TObjectPool(uint32 nAlloc, uint32 nStart, uint32 nMax=(uint32)-1) {
        Create(nAlloc, nStart, nMax);
    }
    ~TObjectPool() { Destroy(false); }

	bool Create(uint32 nAlloc, uint32 nStart, uint32 nMax=(uint32)-1) {
        ASSERT(nStart <= nMax);
		m_nAlloc = nAlloc;
		m_nMax = nMax;
        m_nStart = nStart;
 
		for (m_nCur=0; m_nCur<nStart; ) {
			if (!AllocOnce())
				return false;
		}
		return true;
	}

	bool Destroy(bool ) {
		m_FreeList.Init();
		
		AllocUnit* pUnit;
		while (NULL != (pUnit=m_Pointers.pop())) {
			free(pUnit);
		}
        m_nCur = 0;
		return true;
	}

	TObj* Allocate() {
		m_Lock.Lock();
		for (int i=0; ; i++) {
			AllocUnit* pUnit = m_FreeList.pop();
			if (NULL != pUnit) {
#ifdef	_DEBUG
				pUnit->nSign = 0xdeadbeefUL;
                
#endif
				m_Lock.Unlock();
				return &pUnit->obj;
			}
			if (m_nCur>=m_nMax || 0!=i || !AllocOnce()) {
				break;
			}
		}
		m_Lock.Unlock();
		return NULL;
	}

	TObj* ZeroAllocate() {
		TObj* pObj = Allocate();
		if (pObj) {
			ZeroMemory(pObj, sizeof(TObj));
		}
		return pObj;
	}

	void Free(TObj* pObj) {
		AllocUnit* pUnit = CONTAINING_RECORD(pObj, AllocUnit, obj);
#ifdef	_DEBUG
		ASSERT(pUnit->nSign == 0xdeadbeefUL);
		memset(pUnit, 0xCC, sizeof(AllocUnit));
#endif
		m_Lock.Lock();
		m_FreeList.push(pUnit);
		m_Lock.Unlock();
	}

	void Print(char* pName) {
		int n = m_nAlloc*m_nCur;
		m_Lock.Lock();
		int m = m_FreeList.size();
		m_Lock.Unlock();

        char buf[128];
        PrintSize((uint64)n * sizeof(AllocUnit), buf, sizeof(buf));
		TRACE0("%s: #%d:%d, %sB\n", pName, m, n, buf);
	}

    void PrintObjs() {
        TRACE0("Total Objects: ");
        SLINK* p = (SLINK*)m_Pointers.top();
        AllocUnit* pE = NULL;

        while(p) {
            pE = MAKE_PTR(AllocUnit*, p, sizeof(SLINK));
            for(uint32 i = 0; i < m_nAlloc; ++i) {
                TRACE0("%p ", pE);
                ++pE;
            }
            p = p->_next;
        }
        TRACE0("\n");

        TRACE0("Free  Objects: ");
        pE = m_FreeList.top();
        while(pE) {
            TRACE0("%p ", pE);      
            p = pE->link._next;
            pE = p ? CONTAINING_RECORD(p, AllocUnit, link) : NULL;
        }
        TRACE0("\n");
    }

    int GetFreeListSize() {
        int m;
        
        m_Lock.Lock();
        m = m_FreeList.size();
        m_Lock.Unlock();
        return m;
    }

    void GarbageCollect() {
        m_Lock.Lock();
        if (m_nCur!=m_nStart && m_FreeList.size()==m_nAlloc*m_nCur) {
            Destroy(false);
            Create(m_nAlloc, m_nStart, m_nMax);
        }
        m_Lock.Unlock();
    }
};
#endif
#endif
