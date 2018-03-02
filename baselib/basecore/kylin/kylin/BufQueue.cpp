#include "stdafx.h"
#include "BufQueue.h"
#include "Trace.h"
#include "SyncObj.h"

_KYLIN_PACKAGE_BEGIN_

BufHandle* CBufQueue::Split(BufHandle *pHdl, int nNewLen) 
{
	BufHandle *pNext;
	ASSERT(nNewLen < pHdl->nDataLen);
	ASSERT(nNewLen != 0);

	pNext = AllocateHdl(false, pHdl->pBuf+nNewLen, pHdl->nDataLen-nNewLen);
	pHdl->nDataLen = nNewLen;

	pNext->_next = pHdl->_next;
	pHdl->_next = pNext;

	if (m_ppLast == &pHdl->_next) {			
		m_ppLast = &pNext->_next;
		
	}
	return pNext;
}

BufHandle* CBufQueue::ReserveN(INOUT int* pnLen, OUT BufHandle** ppNext)
{
	BufHandle* pNext;
	int nLen = *pnLen;
	ASSERT(nLen != 0);
	ASSERT(m_nReserveLen == 0);

	m_nReserveLen = nLen;
	for (pNext=m_pFirst; pNext!=NULL; pNext=pNext->_next) {
		ASSERT(pNext->nDataLen != 0);

		if (pNext->nDataLen < nLen) {
			nLen -= pNext->nDataLen;
		}
		else {
			if (pNext->nDataLen > nLen) {		
				Split(pNext, nLen);
			}
			pNext = pNext->_next;
			*ppNext = pNext;
			return m_pFirst;
		}
	}
	ASSERT(m_nTotalLen < m_nReserveLen);
	m_nReserveLen = m_nTotalLen;
	
	*pnLen = m_nReserveLen;
	*ppNext = NULL;
	return m_pFirst;
}

BufHandle* CBufQueue::ReserveAll(int nMaxBuf, OUT int *pnLen, OUT BufHandle** ppNext)
{
	BufHandle* pNext;
	int n = 0;
	
	ASSERT(m_nReserveLen == 0);
	for (pNext=m_pFirst; pNext!=NULL && n<nMaxBuf; pNext=pNext->_next, n++) {
		m_nReserveLen += pNext->nDataLen;
	}
	*ppNext = pNext;
	*pnLen = m_nReserveLen;
	return m_pFirst;
}


int CBufQueue::GetToZero(char* pBuf, int nBufLen)
{
    return GetToChar('\0', pBuf, nBufLen);
}

int CBufQueue::GetToChar(char sep, char* pBuf, int nBufLen)
{
    BufHandle *pHdl;
    char *p, *pStop;
    int nLen = 0;

    
    for (pHdl=m_pFirst; pHdl!=NULL; pHdl=pHdl->_next) {
        ASSERT(pHdl->nDataLen != 0);
        p = pHdl->pBuf;
        pStop = pHdl->pBuf + pHdl->nDataLen;
        for ( ; p<pStop; p++) {
            if (sep == *p) {
                p ++;
                nLen += (p - pHdl->pBuf);
                if (nLen <= nBufLen) {
                    if (p != pStop) {
                        Split(pHdl, p-pHdl->pBuf);
                    }
                    memcpy(pBuf, pHdl->pBuf, pHdl->nDataLen);

                    pHdl = pHdl->_next;
                    ChainFreeHdl(m_pFirst, pHdl);
                    m_pFirst = pHdl;
                    m_nTotalLen -= nLen;
                    if (m_pFirst == NULL) {
                        ASSERT(m_nTotalLen == 0);
                        m_ppLast = &m_pFirst;
                    }
                    return nLen;
                }
                else {
                    return -1;
                }
            }
        }
        nLen += pHdl->nDataLen;
        if (nLen >= nBufLen)
            return -1;
        memcpy(pBuf, pHdl->pBuf, pHdl->nDataLen);
        pBuf += pHdl->nDataLen;
    }
    return 0;
}

BufHandle* CBufQueue::GetAll()
{
	BufHandle* pHdl = m_pFirst;

	m_nTotalLen = 0;
	m_nReserveLen = 0;
	m_pFirst = NULL;
	m_ppLast = &m_pFirst;
	return pHdl;
}

BufHandle* CBufQueue::GetN(INOUT int* pnLen)
{
	BufHandle *pHdl, *pNext;
	int nLen = *pnLen;
	ASSERT(nLen > 0);
	ASSERT(m_nReserveLen == 0);



	m_nTotalLen -= nLen;
	for (pNext=m_pFirst; pNext!=NULL; pNext=pNext->_next) {
		ASSERT(pNext->nDataLen != 0);

		if (pNext->nDataLen < nLen) {
			nLen -= pNext->nDataLen;
		}
		else {
			ASSERT(m_nTotalLen >= 0);
			if (pNext->nDataLen > nLen) {		
				Split(pNext, nLen);
			}

			pHdl = m_pFirst;
			m_pFirst = pNext->_next;
			pNext->_next = NULL;				
			if (m_pFirst == NULL) {
				m_ppLast = &m_pFirst;
				
			}
			return pHdl;
		}
	}

	ASSERT(m_nTotalLen <= 0);
	ASSERT(m_nTotalLen == -nLen);
	*pnLen = *pnLen - nLen;
	m_nTotalLen = 0;
	pHdl = m_pFirst;
	m_pFirst = NULL;
	m_ppLast = &m_pFirst;

	return pHdl;
}

int CBufQueue::PeekN(IN int nLen, OUT char* pBuf)
{
    BufHandle *pHdl;
    int n = nLen;
    
    for (pHdl=m_pFirst; pHdl!=NULL; pHdl=pHdl->_next) {
        ASSERT(pHdl->nDataLen != 0);
        if (pHdl->nDataLen < nLen) {
            memcpy(pBuf, pHdl->pBuf, pHdl->nDataLen);
            pBuf += pHdl->nDataLen;
            nLen -= pHdl->nDataLen;
        }
        else {
            memcpy(pBuf, pHdl->pBuf, nLen);
            return n;
        }
    }
    return n-nLen;
}

int CBufQueue::GetN(IN int nLen, OUT char* pBuf)
{
	BufHandle *pHdl, *pNext;
	int n = nLen;
	ASSERT(nLen != 0);
	ASSERT(m_nReserveLen == 0);



	m_nTotalLen -= nLen;
	for (pNext=m_pFirst; pNext!=NULL; pNext=pNext->_next) {
		ASSERT(pNext->nDataLen != 0);

		if (pNext->nDataLen < nLen) {
			memcpy(pBuf, pNext->pBuf, pNext->nDataLen);
			pBuf += pNext->nDataLen;
			nLen -= pNext->nDataLen;
		}
		else {
			ASSERT(m_nTotalLen >= 0);
			if (pNext->nDataLen > nLen) {		
				Split(pNext, nLen);
			}
			memcpy(pBuf, pNext->pBuf, pNext->nDataLen);

			pHdl = m_pFirst;
			m_pFirst = pNext->_next;
			ChainFreeHdl(pHdl, m_pFirst);
			if (m_pFirst == NULL) {
				m_ppLast = &m_pFirst;
				
			}
			return n;
		}
	}

	ASSERT(m_nTotalLen <= 0);
	ASSERT(m_nTotalLen == -nLen);
	m_nTotalLen = 0;
	if(NULL != m_pFirst)
		ChainFreeHdl(m_pFirst, NULL);
	m_pFirst = NULL;
	m_ppLast = &m_pFirst;
	
	return n - nLen;
}

void CBufQueue::Commit(int nLen, bool bFree)
{
	BufHandle* pNext = NULL;
	int n = 0;
	ASSERT(nLen > 0);

	
	
	
	ASSERT(m_nReserveLen >= nLen);
	m_nReserveLen -= nLen;
	m_nTotalLen -= nLen;

	
	for (pNext=m_pFirst; pNext!=NULL; pNext=pNext->_next) {
		n += pNext->nDataLen;
		if (n > nLen) {
			n -= nLen;
			pNext = Split(pNext, pNext->nDataLen-n);
			break;
		}
		else if(n == nLen) {
			pNext = pNext->_next;
			break;
		}
	}
	if (bFree)
		ChainFreeHdl(m_pFirst, pNext);
	m_pFirst = pNext;
	if (m_pFirst == NULL) {
		m_ppLast = &m_pFirst;
		
	}
	
}

int CBufQueue::Append(IN BufHandle *pHdl, IN BufHandle* pNext) 
{
	int n = 0;
	ASSERT(pHdl != pNext);

	
	
	if (!(*m_ppLast == NULL && m_nReserveLen == 0)) {
		TRACE0("*m_ppLast=%p, m_nReserveLen=%d\n", *m_ppLast, m_nReserveLen);
		RaiseError(Error_In_CBufQueue_Append);
	}

	for (*m_ppLast=pHdl; pHdl->_next!=pNext; pHdl=pHdl->_next) {
		ASSERT(pHdl->nDataLen != 0);
		n += pHdl->nDataLen;
	}
	n += pHdl->nDataLen;

	m_ppLast = &pHdl->_next;
	*m_ppLast = NULL;
	
	m_nTotalLen += n;
	return n;
}

void CBufQueue::Clear()
{
	if (&m_pFirst != m_ppLast) {
		ASSERT(m_pFirst != NULL);
		ChainFreeHdl(m_pFirst, *m_ppLast);
	}
	m_pFirst = NULL;
	m_ppLast = &m_pFirst;
	m_nReserveLen = 0;
	m_nTotalLen = 0;
}

_KYLIN_PACKAGE_END_
