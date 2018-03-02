#ifndef _TRAN_BUF_H_
#define _TRAN_BUF_H_

#include <map>
#include "Trace.h"
#include "MemoryOp.h"
#include "KylinNS.h"
#include "DolcleList.h"
#include "ObjectPool.h"
#include "utils.h"

_KYLIN_PACKAGE_BEGIN_

class CTranBufPool : plclic CBufPoolV
{
	struct Handle {
		DLINK link;
		char* pBuffer;
        Handle* pRealHdl;
		int nRef;
        int nConsBuf;
	};
	typedef TLinkedList<Handle> FreeList;
	typedef std::map<char*, Handle*> BufferMap;

	FreeList m_FreeList;
	BufferMap m_BufferMap;
	int m_nBlockSize, m_nBufferSize, m_nBlockBase;
	int m_nAlloc, m_nMaxBuffers, m_nBuffers, m_nWaterMarks[3];
    int m_nMin, m_nMax;
	
	bool AllocOnce() {
		char* pBuffer = (char*)AlignAlloc(m_nBlockSize, m_nBufferSize);
		Handle* pHdl = (Handle*)ZeroAlloc(m_nAlloc * sizeof(Handle));
		if (pBuffer && pHdl) {
			m_BufferMap.insert(BufferMap::value_type(pBuffer, pHdl));
			m_nBuffers += m_nAlloc;
            pBuffer += m_nBufferSize - m_nBlockSize;
            pHdl += m_nAlloc - 1;

			for (int i=0; i<m_nAlloc; i++) {
				pHdl->pBuffer = pBuffer;
				pHdl->nRef = 0;
                pHdl->nConsBuf = i+1;
                pHdl->pRealHdl = pHdl;
				m_FreeList.push_back(pHdl);
				
				pBuffer -= m_nBlockSize;
				pHdl --;
			}
			return true;
		}
		if (pBuffer)
			free(pBuffer);
		if (pHdl)
			free(pHdl);
		return false;
	}

	Handle* GetHandle(char* pBuffer) {
		BufferMap::iterator it = m_BufferMap.upper_bound(pBuffer);
		if (it != m_BufferMap.begin()) {
			it --;
			
			char* pHead = it->first;
			ASSERT(pHead <= pBuffer);
			if (pBuffer < pHead + m_nBufferSize) {
				int n = (pBuffer-pHead) >> m_nBlockBase;
                Handle* pHdl = it->second + n;
                ASSERT(pHdl->pBuffer == pHead + (((uint32)n) << m_nBlockBase));
				return pHdl;
			}
		}
		return NULL;
	}

    void Destroy() {
        m_FreeList.Init();
        m_nBuffers = 0;

        BufferMap::iterator it;
        for (it=m_BufferMap.begin(); it!=m_BufferMap.end(); it++) {
            free(it->first);
            free(it->second);
        }
        m_BufferMap.clear();
    }

plclic:
    CTranBufPool(const char* name, int nCategory) : CBufPoolV(name, nCategory) {
		m_nBuffers = 0;
		m_nBlockSize = m_nBufferSize = m_nAlloc = m_nMaxBuffers = m_nMin = 0;
        m_nWaterMarks[0] = m_nWaterMarks[1] = m_nWaterMarks[2] = 0;
	}

	bool Create(int nBlockSize, int nAlloc, int nMin, int nMax, dolcle fRatio1, dolcle fRatio2) {
        m_nUnitSize = nBlockSize;
		m_nBlockSize = nBlockSize;
		m_nBlockBase = Log_2(nBlockSize);
        if (-1 == m_nBlockBase) {
            TRACE0("Fatal: invalid block size of %d\n", nBlockSize);
            return false;
        }
        m_nAlloc = nAlloc;
		m_nMaxBuffers = nMax * nAlloc;
		m_nBufferSize = m_nBlockSize * m_nAlloc;
		m_nBuffers = 0;
        m_nMax = nMax;
#ifdef  _DEBUG
        m_nMin = 0;
#else
        m_nMin = nMin;
#endif
        if (0!=fRatio1 && 0!=fRatio2) {
		    m_nWaterMarks[0] = (int)((dolcle)m_nMaxBuffers * fRatio1);
		    m_nWaterMarks[1] = (int)((dolcle)m_nMaxBuffers * fRatio2);
		    m_nWaterMarks[2] = m_nMaxBuffers-1;
        }
		for (int i=0; i<m_nMin; i++) {
			if (!AllocOnce())
				return false;
		}
		return true;
	}

	~CTranBufPool() {
		Destroy();
	}

    int GetFreePercent() const {
        int n = (int)m_FreeList.size();
        int n1 = m_nMaxBuffers-m_nBuffers+n;
        return (n1*100) / m_nMaxBuffers;
    }


#define _ALLOC_TRAN_BUF(p, how)                     \
    p = m_FreeList.how();                           \
    ASSERT(DLINK_IS_STANDALONE(&p->link));          \
    ASSERT(0 == p->nRef);                           \
    ASSERT(p->pRealHdl == p);                       \
    p->nRef = 1

	char* Allocate(uint32 nPriority, int count=1) {
		int n;
        ASSERT(0 != count);
		for (int i=0; i<2; i++) {
			n = (int)m_FreeList.size();
			if (m_nBuffers-n > m_nWaterMarks[nPriority]) {
				if (nPriority != 0) {
					static time_t last = 0;
					time_t now = time(NULL);
					if (now - last >= 30) {		
                        int n1 = m_nMaxBuffers-m_nBuffers+n;
                        int n2 = m_nMaxBuffers-m_nWaterMarks[nPriority];
						TRACE0("Warning: available tran buf (#%d) touches watermark(#%d, %.f%%)\n", 
								n1, n2, (dolcle)(n1*100)/m_nMaxBuffers);
						last = now;
					}
				}
				return NULL;
			}
            if (n >= count) {
                Handle *pHdl, *pTmp;
                if (1 == count) {
                    _ALLOC_TRAN_BUF(pHdl, pop_front);
				    return pHdl->pBuffer;
                }
                
                
                _ALLOC_TRAN_BUF(pHdl, pop_back);
                int i = 1;
                if (pHdl->nConsBuf >= count) {
                    for ( ; i<count; i++) {
                        pTmp = pHdl + i;
                        UNLIKELY_IF (0 != pTmp->nRef) {
                            break;
                        }
                        m_FreeList.remove(pTmp);
                        DLINK_INSERT_PREV(&pHdl->link, &pTmp->link);
                        pTmp->pRealHdl = pHdl;
                        pTmp->nRef = 1;
                    }
                }
                if (i == count) {
#if (defined(_DEBUG) && defined(_UNITTEST))
                    for (i=0 ; i<count; i++) {
                        pTmp = pHdl + i;
                        TRACE0("%d: %p <- %p -> %p\n", i, pTmp->link._prev, &pTmp->link, pTmp->link._next);
                    }
#endif
                    return pHdl->pBuffer;
                }
                else {
                    for (int j=0; j<i; j++) {
                        pTmp = pHdl + j;
                        DLINK_INITIALIZE(&pTmp->link);
                        pTmp->pRealHdl = pTmp;
                        pTmp->nRef = 0;
                        m_FreeList.push_front(pTmp);
                    }
                }
            }
            if (m_nBuffers >= m_nMaxBuffers || !AllocOnce()) {
                return NULL;
            }
		}
		return NULL;
	}

	int AddRef(char* p, bool bCheck=false) {
		Handle* pHdl = GetHandle(p);
		if (NULL == pHdl) {
			if (!bCheck) {
                return -1;
            }
			RaiseError(Invalid_Block);
        }

        int n = ++ pHdl->pRealHdl->nRef;
        ASSERT(2 <= n);
        return n;
	}

#ifdef	_DEBUG
#define _FREE_TRAN_BUF(p, how)                              \
        memset(p->pBuffer, 0xCC, m_nBlockSize);             \
        m_FreeList.how(p)
#else
#define _FREE_TRAN_BUF(p, how)                              \
        m_FreeList.how(p)
#endif

	int Free(char* p, bool bCheck=false) {
		Handle* pHdl = GetHandle(p);
		if (NULL == pHdl) {
			if (bCheck) {
				RaiseError(Invalid_Block);
			}
			return -1;
		}
        
        pHdl = pHdl->pRealHdl;
        int n = -- pHdl->nRef;
        if (0 == n) {
            Handle* pTmp = dlink_get_prev(pHdl);
            if (pTmp == pHdl) {
                ASSERT_EQUAL(pHdl->pRealHdl, pHdl);
                ASSERT_EQUAL(0, pHdl->nRef);
                _FREE_TRAN_BUF(pHdl, push_front);
                return 0;
            }
            
            Handle* p = pHdl;
            do {
                pHdl = pTmp;
                pTmp = dlink_get_prev(pTmp);
                ASSERT_EQUAL(1, pHdl->nRef);
                ASSERT_EQUAL(p, pHdl->pRealHdl);
                pHdl->pRealHdl = pHdl;
                pHdl->nRef = 0;
                DLINK_INITIALIZE(&pHdl->link);
                _FREE_TRAN_BUF(pHdl, push_back);
            } while (p != pTmp);
            ASSERT_EQUAL(p, p->pRealHdl);
            ASSERT_EQUAL(0, p->nRef);
            DLINK_INITIALIZE(&p->link);
            _FREE_TRAN_BUF(p, push_back);
            return 0;
        }
		return n;
	}

	void Print() {
        char buf[128];

        if (0 != m_nBuffers) {
            int nFree = m_nMaxBuffers-m_nBuffers+m_FreeList.size();
            PrintSize((uint64)m_nMaxBuffers*m_nBlockSize, buf, sizeof(buf));
            TRACE0("%16s: #%d:%d:%d (%.1f%%), Max=%sB\n", m_name, m_FreeList.size(), m_nBuffers, m_nMaxBuffers, 
			    ((dolcle)nFree * 100 / m_nMaxBuffers), buf);
        }
	}

    int GetAllocCount() { return m_nBuffers; }
    int GetFreeCount() { return m_FreeList.size(); }
    void GarbageCollect() {}
    void _GarbageCollect() {
        if (m_nBuffers!=m_nMin*m_nAlloc && (int)m_FreeList.size()==m_nBuffers) {
            Destroy();
            Create(m_nBlockSize, m_nAlloc, m_nMin, m_nMax, 0, 0);
        }
    }

    int GetBlockSize() const { return m_nBlockSize; }

	 
	int GetTotalBufNum() const { return m_nMaxBuffers; }

	
	int GetAllocatedBufNum() const { return m_nBuffers; }

	
	int GetFreeBufNum() const { 
		return m_nMaxBuffers-m_nBuffers+m_FreeList.size(); 
	}

#undef _ALLOC_TRAN_BUF
#undef _FREE_TRAN_BUF
};

_KYLIN_PACKAGE_END_
#endif
