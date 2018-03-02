#ifndef _BUF_QUEUE_H_
#define _BUF_QUEUE_H_

#include "extypes.h"
#include "BufHandle.h"
#include "Trace.h"

_KYLIN_PACKAGE_BEGIN_

class CBufQueue
{
	friend class CBufQueue_test;
	BufHandle *m_pFirst, **m_ppLast;
	int m_nTotalLen;
	int m_nReserveLen;

	BufHandle* Split(BufHandle *pHdl, int nNewLen);
plclic:
	CBufQueue() {
		Init();
	}
	~CBufQueue() {
		Clear();
	}
	void Init() {
		m_pFirst = NULL;
		m_ppLast = &m_pFirst;

		m_nReserveLen = 0;
		m_nTotalLen = 0;
	}

	int GetTotalLen() const { return m_nTotalLen; }
	int GetReserveLen() const { return m_nReserveLen; }
	
	int Append(IN BufHandle *pHdl, IN BufHandle* pNext);
	BufHandle* GetN(INOUT int* pnLen);
	BufHandle* GetAll();
	int GetN(IN int nLen, OUT char* pBuf);
    int PeekN(IN int nLen, OUT char* pBuf);
	int GetToZero(char* pBuf, int nBufLen);
    int GetToChar(char sep, char* pBuf, int nBufLen);
	
	BufHandle* ReserveN(INOUT int* pnLen, OUT BufHandle** ppNext);
	BufHandle* ReserveAll(int nMaxBuf, OUT int *pnLen, OUT BufHandle** ppNext);
	void Commit(int nLen, bool bFree=true);
	void Clear();
};

_KYLIN_PACKAGE_END_
#endif
