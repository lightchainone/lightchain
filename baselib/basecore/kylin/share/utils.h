
#ifndef _UTILS_H_
#define _UTILS_H_

#include "windef.h"

#define MAKE_PTR(cast, ptr, add)        ((cast)((long)(ptr) + (long)(add)))
#define PTR_DIST(ptr1, ptr2)            ((unsigned long)ptr1 - (unsigned long)ptr2)
#define MAKE_UINT64(high, low)          (((uint64)(high))<<32 | (uint64)(low))

#ifndef MAX
	#define MAX(a, b)                   ((a)>(b) ? (a) : (b))
#endif
#ifndef MIN
	#define MIN(a, b)                   ((a)<(b) ? (a) : (b))
#endif

#define PACK_N(p, n)                    ((((p)+(n)-1)/(n))*(n))

#define ALIGN_512_ALLOCA(n)             ((char*)(((unsigned long)alloca(n + 512) >> 9 ) << 9) + 512)
#define PACK_512(p)                     ((((p)+511) >> 9) << 9)

#define IS_BLANK(c)                     ((c)==' ' || (c)=='\t' || (c)=='\r' || (c)=='\n')
#define IS_DIGIT(c)                     ((c)>='0' && (c)<='9')
#define IS_HEX_DIGIT(c)                 (((c)>='0' && (c)<='9') || ((c)>='A' && (c)<='F') || ((c)>='a' && (c)<='f'))
#define IS_UPPER(c)                     ((c)>='A' && (c)<='Z')
#define IS_LOWER(c)                     ((c)>='a' && (c)<='z')
#define IS_ALPHA(c)                     (IS_UPPER(c) || IS_LOWER(c))
#define TO_LOWER(c)                     (IS_UPPER(c) ? ((c)-'A'+'a') : (c))

#define SAFE_DELETE(p)                  if (p) { delete (p); (p) = NULL; }
#define SAFE_FREE(p)                    if (p) { free(p); (p) = NULL; }

class CNocaseCmp
{
plclic:
    bool operator()(const char* x, const char* y) const {
        return _stricmp(x, y) < 0;
    }
};
class CCaseCmp
{
plclic:
    bool operator()(const char* x, const char* y) const {
        return strcmp(x, y) < 0;
    }
};

inline int 
StrDirect2Int(const char* pStr, int len=(int)sizeof(int))
{
    int n = 0;
    if (pStr) {
        char* p = (char*)&n + 3;
        if ((int)sizeof(int) < len)
            len = (int)sizeof(int);
        for (int j=0; j<len; j++) {
            if (0 == pStr[j])
                break;
            *p-- = pStr[j];
        }
    }
    return n;
}

int PrintNow(char* pBuf, int nLen);
int PrintTime(time_t t, char* pBuf, int nLen);
int PrintSize(uint64 s, char* pBuf, int nLen, int nBase=1024);
char* PrintSize(uint64 s, int nBase=1024);      
char* TimeInterval2Str(uint32 seconds);
char* Time2Str(time_t t);
char* Now2Str();

char* xfgets(char *s, int size, FILE *stream, char* leading, char* ending=NULL);
FILE* xfopen(const char* mode, const char* fmt, ...);


int SplitBuf(uint8* pBuf, int nLen, uint8 c, uint8* pElements[], int n);

int SplitString(char* pString, char c, char* pElements[], int n);

int SplitString(char* pString, const char* s, char* pElements[], int n, bool bSkipBlank=false);

int SplitStringByToken(char* pString, const char* token, char* pElements[], int n);



void SetBits(char* pOld, const char* pMod, int nStart, int nLen);
uint64 GetBits(const char* pData, int nStart, int nLen);

void SetBytes(char* pOld, const char* pMod, int nStart, int nLen);

bool ParseNum(char* pStr, uint64* pNum);
uint32 CountPercent(uint32 up, uint32 down);

char* StrToLower(char* pTo, const char* pFrom, OUT int* pnLen=NULL);

static inline 
int Log_2(uint32 nNum) {
    int i;
    for (i=0; i<32; i++) {
        if (nNum == (1UL << i))
            return i;
    }
    return -1;
}

bool RenameFile(char* pOld, char* pNew);

#endif
