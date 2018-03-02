#ifndef _BUF_HANDLE_H_
#define _BUF_HANDLE_H_

#include "extypes.h"
#include "KylinNS.h"

_KYLIN_PACKAGE_BEGIN_

struct BufHandle {
	BufHandle *_next;
	char* pBuf;
	int nBufLen;			
	int nDataLen;			
};

static inline void
InitBufHandle(BufHandle* p) {
	p->_next = NULL;
	p->pBuf = NULL;
	p->nBufLen = 0;
	p->nDataLen = 0;
}

static inline uint32
GetTotalDataLen(const BufHandle* pHdl, const BufHandle* pNext) {
	uint32 n = 0;
	for ( ; pHdl!=pNext; pHdl=pHdl->_next) {
		n += pHdl->nDataLen;
	}
	return n;
}

BufHandle* Reclaim(int nLen, BufHandle* pHdl, BufHandle* pNext);

#define SZ_BIG_BUF				1048576     

void SetTranBuf(int nSmallNum, int nBigNum, 
                int nSmallSize = 4096, 
                float fLowMark = 0.6f, 
                float fHighMark = 0.9f);
BufHandle* AllocateHdl(bool bInPool=false, char* pBuf=NULL,
					   int nLen=0, BufHandle* pNext=NULL);
BufHandle* AllocateHdlCanFail(int nSize=0);
BufHandle* AllocateBigHdl();
BufHandle* AllocateBigHdlCanFail();

int GetFreeTranBufPercent();
void FreeHdl(BufHandle* pHdl);
void ChainFreeHdl(BufHandle* pHdl, BufHandle* pNext);
BufHandle* CloneHdlAndTerminate(BufHandle* pHdl, BufHandle* pNext, 
                                int* pnLen=NULL, bool bCopyNonTranBuf=true);

#define CloneHdl    CloneHdlAndTerminate

BufHandle* SplitBufHdl(BufHandle *pHdl, int nNewLen);		
BufHandle* BufHdlPrintf(BufHandle* pNext, const char *fmt, ...);
BufHandle* vBufHdlPrintf(BufHandle* pNext, const char *fmt, va_list va);
BufHandle* LoadFileToBufHdl(FILE* fp);


int GetTotalTranBufNum();       
int GetAllocatedTranBufNum();   
int GetFreeTranBufNum();        

#ifdef _UNITTEST


int GetTotalBigBufNum();
int GetAllocatedBigBufNum();
int GetFreeBigBufNum();
int GetFreeBufHdlNum();

#endif

_KYLIN_PACKAGE_END_

#endif
