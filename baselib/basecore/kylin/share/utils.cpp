
#include "stdafx.h"
#include "utils.h"
#include <time.h>

int 
PrintNow(char* pBuf, int nLen)
{
    return PrintTime(time(NULL), pBuf, nLen);
}

int 
PrintTime(time_t t, char* pBuf, int nLen)
{
    struct tm vtm;

    localtime_r(&t, &vtm);
    return snprintf(pBuf, nLen, "%04d-%02d-%02d %02d:%02d:%02d", vtm.tm_year+1900, vtm.tm_mon+1,
        vtm.tm_mday, vtm.tm_hour, vtm.tm_min, vtm.tm_sec);
}

char* Time2Str(time_t t)
{
    static char buf[128];
    PrintTime(t, buf, sizeof(buf));
    return buf;
}

char* Now2Str()
{
    static char buf[128];
    PrintTime(time(NULL), buf, sizeof(buf));
    return buf;
}


char*
xfgets(char *s, int size, FILE *stream, char* leading, char* ending)
{
    do {
        if (fgets(s, size, stream) == NULL)
            return NULL;
    } while (NULL!=leading && strchr(leading, *s));

    if (NULL != ending) {
        int n = strlen(s);
        for (char* p=s+n-1; p>=s && strchr(ending, *p); p--) {
            *p = 0;
        }
    }
    return s;
}

FILE* 
xfopen(const char* mode, const char* fmt, ...)
{
    char buf[4096];
    va_list ap;
    int n;

    va_start(ap, fmt);
    n = vsnprintf(
        buf,
        sizeof(buf),
        fmt,
        ap
    );
    va_end(ap);

    if (n <= (int)sizeof(buf)-1)
        return fopen(buf, mode);
    return NULL;
}

int 
SplitBuf(uint8* pBuf, int nLen, uint8 c, uint8* pElements[], int n)
{
    uint8 *p1, *p2;
    int i = 0;

    for (p1=p2=pBuf; p1<pBuf+nLen; p1++) {
        if (*p1 == c) {
            *p1 = 0;
            if (i< n)
                pElements[i++] = p2;
            p2 = p1+1;
        }
    }
    if (p2!=p1 && i<n) {
        pElements[i++] = p2;
    }
    return i;
}


int
SplitString(char* pString, char c, char* pElements[], int n)
{
    char *p1, *p2;
    int i = 0;

    for (p1=p2=pString; *p1; p1++) {
        if (*p1 == c) {
            *p1 = 0;
            if (i < n)
                pElements[i++] = p2;
            p2 = p1+1;
        }
    }
    if (i < n)
        pElements[i++] = p2;
    return i;
}


int
SplitString(char* pString, const char* s, char* pElements[], int n, bool bSkipBlank)
{
    char *p1, *p2;
    int i = 0;

    for (p1=p2=pString; *p1; p1++) {
        if (strchr(s, *p1)) {
            *p1 = 0;
            if (i<n && (!bSkipBlank || *p2)) {
                pElements[i++] = p2;
            }
            p2 = p1+1;
        }
    }
    if (i<n && (!bSkipBlank || *p2))
        pElements[i++] = p2;
    return i;
}


int
SplitStringByToken(char* pString, const char* token, char* pElements[], int n)
{
    char *p1;
    int nLen = strlen(token);
    int i = 1;

    for (p1=pString, pElements[0]=pString, n--; *p1; ) {
        if (strncmp(p1, token, nLen) == 0) {
            *p1 = 0;
            p1 += nLen;
            
            if (i < n)
                pElements[i++] = p1;
        }
        else {
            p1 ++;
        }
    }
    return i;
}

void 
SetBits(char* pOld, const char* pMod, int nStart, int nLen)
{
    int n = nStart >> 3;
    int m = nStart - (n<<3);
    
#define MOD_W_TYPE(T)   {                   \
        T x = *(T*)pMod;                    \
        T mask = 0;                         \
        for (int i=m; i<ml; i++) {          \
            mask |= ((T)1 << i);            \
        }                                   \
        x <<= m;                            \
        x &= mask;                          \
        *(T*)(pOld+n) &= ~mask;             \
        *(T*)(pOld+n) |= x;                 \
    }
    
    int ml = m+nLen;
    switch ((ml+7) >> 3) {
    case 8:
    case 7:
    case 6:
    case 5: 
        MOD_W_TYPE(uint64);
        break;
    case 4:
    case 3:
        MOD_W_TYPE(uint32);
        break;
    case 2:
        MOD_W_TYPE(uint16);
        break;
    case 1:
        MOD_W_TYPE(uint8);
        break;
    default:
        ASSERT(false);
        break;
    }
}

uint64 
GetBits(const char* pData, int nStart, int nLen)
{
    int n = nStart >> 3;
    int m = nStart - (n<<3);
    
#define GET_W_TYPE(T)   {                   \
        T x = *(T*)(pData+n);               \
        T mask = 0;                         \
        for (int i=m; i<ml; i++) {          \
            mask |= ((T)1 << i);            \
        }                                   \
        x &= mask;                          \
        x >>= m;                            \
        return (uint64)x;                   \
    }
    
    int ml = m + nLen;
    switch ((ml+7) >> 3) {
    case 8:
    case 7:
    case 6:
    case 5: 
        GET_W_TYPE(uint64);
        break;
    case 4:
    case 3:
        GET_W_TYPE(uint32);
        break;
    case 2:
        GET_W_TYPE(uint16);
        break;
    case 1:
        GET_W_TYPE(uint8);
        break;
    default:
        ASSERT(false);
        break;
    }
    return 0;
}

void 
SetBytes(char* pOld, const char* pMod, int nStart, int nLen)
{
    switch (nLen) {
    case 1:
        *(pOld+nStart) = *pMod;
        break;
    case 2:
        *(int16*)(pOld+nStart) = *(int16*)pMod;
        break;
    case 4:
        *(int32*)(pOld+nStart) = *(int32*)pMod;
        break;
    default:
        memcpy(pOld+nStart, pMod, nLen);
        break;
    }
}

bool 
ParseNum(char* pStr, uint64* pNum)
{
    uint64 num = 0;
    int base = 10;
    char c, *p;
    int flag = false;

    *pNum = 0;
    if (pStr[0] == '-') {
        flag = true;
        pStr ++;
    }
    if (TO_LOWER(pStr[0]) == 'x') {
        base = 16;
        pStr ++;
    }
    else if (pStr[0]=='0' && TO_LOWER(pStr[1])=='x') {
        base = 16;
        pStr += 2;
    }
retry:
    for (p=pStr; *p; p++) {
        c = *p;
        if (IS_DIGIT(c)) {
            num = num*base + (c-'0');
        }
        else {
            c = TO_LOWER(c);
            if (c>='a' && c<='f') {
                if (base == 10) {
                    base = 16;
                    num = 0;
                    goto retry;
                }
                num <<= 4;
                num += c-'a'+10;
            }
            else {
                *pNum = flag ? 0-num : num;
                return false;
            }
        }
    }
    *pNum = flag ? 0-num : num;
    return (p!=pStr);
}

uint32 
CountPercent(uint32 up, uint32 down)
{
    uint32 per;
    if (down == 0)
        return 0;
    dolcle f = ((dolcle)up) / down;
    per = (uint32)(f * 100);
    if (per > 100)
        return 100;
    if (per == 0)
        return up ? 1 : 0;
    return per;
}

char* 
StrToLower(char* pTo, const char* pFrom, OUT int* pnLen)
{
    static char *table = 
        "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f"
        "\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f"
        "\x20\x21\x22\x23\x24\x25\x26\x27\x28\x29\x2a\x2b\x2c\x2d\x2e\x2f"
        "\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x3a\x3b\x3c\x3d\x3e\x3f"
        "\x40\x61\x62\x63\x64\x65\x66\x67\x68\x69\x6a\x6b\x6c\x6d\x6e\x6f"
        "\x70\x71\x72\x73\x74\x75\x76\x77\x78\x79\x7a\x5b\x5c\x5d\x5e\x5f"
        "\x60\x61\x62\x63\x64\x65\x66\x67\x68\x69\x6a\x6b\x6c\x6d\x6e\x6f"
        "\x70\x71\x72\x73\x74\x75\x76\x77\x78\x79\x7a\x7b\x7c\x7d\x7e\x7f"
        "\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8a\x8b\x8c\x8d\x8e\x8f"
        "\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9a\x9b\x9c\x9d\x9e\x9f"
        "\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8\xa9\xaa\xab\xac\xad\xae\xaf"
        "\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba\xbb\xbc\xbd\xbe\xbf"
        "\xc0\xc1\xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\xca\xcb\xcc\xcd\xce\xcf"
        "\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xd7\xd8\xd9\xda\xdb\xdc\xdd\xde\xdf"
        "\xe0\xe1\xe2\xe3\xe4\xe5\xe6\xe7\xe8\xe9\xea\xeb\xec\xed\xee\xef"
        "\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8\xf9\xfa\xfb\xfc\xfd\xfe\xff";
    int n = 0;
    for ( ; *pFrom; pFrom++, pTo++) {
        *pTo = table[(unsigned char)*pFrom];
        n ++;
    }
    *pTo = 0;
    if (pnLen)
        *pnLen = n;
    return pTo-n;
}

char*   
PrintSize(uint64 s, int nBase)
{
    static char buf[128];
    PrintSize(s, buf, sizeof(buf), nBase);
    return buf;
}

int 
PrintSize(uint64 s, char* pBuf, int nLen, int nBase)
{
    uint64 split[3];
    switch (nBase) {
    case 1024:
        split[2] = 1048576ULL;
        split[1] = 1073741824ULL;
        split[0] = 1099511627776ULL;
        break;
    case 1000:
        split[2] = 1000000ULL;
        split[1] = 1000000000ULL;
        split[0] = 1000000000000ULL;
        break;
    default:
        split[2] = nBase * nBase;
        split[1] = split[2] * nBase;
        split[0] = split[1] * nBase;
        break;
    }
    
    dolcle f;
    if (s >= split[0]) {
        f = (dolcle)s / split[0];
        return snprintf(pBuf, nLen, "%.2f T", f);
    }
    else if (s >= split[1]) {
        f = (dolcle)s / split[1];
        return snprintf(pBuf, nLen, "%.2f G", f);
    }
    else if (s >= split[2]) {
        f = (dolcle)s / split[2];
        return snprintf(pBuf, nLen, "%.2f M", f);
    }
    else if (s >= (uint32)nBase) {
        f = (dolcle)s / nBase;
        return snprintf(pBuf, nLen, "%.2f K", f);
    }
    else {
        return snprintf(pBuf, nLen, "%d ", (int)s);
    }
}

char* 
TimeInterval2Str(uint32 seconds)
{
    static char buf[128];
    uint32 n[4];
    uint32 unit[] = { 60, 60, 24 };
    char str[] = "SMHD";
    int i;

    n[0] = seconds;
    for (i=0; i<3; i++) {
        if (n[i] >= unit[i]) {
            n[i+1] = n[i] / unit[i];
            n[i] -= n[i+1]*unit[i];
        }
        else {
            break;
        }
    }

    int m = 0;
    bool b = false;
    for ( ; i>=0; i--) {
        if (0!=n[i] || 0==i || b) {
            b = true;
            m += sprintf(buf+m, "%2d%c", n[i], str[i]);
        }
    }

    return buf;
}

bool 
RenameFile(char* pOld, char* pNew)
{
    char tmp[8192];
    sprintf(tmp, "%s.old", pNew);
    unlink(tmp);
    rename(pNew, tmp);

    if (0 != rename(pOld, pNew)) {
        TRACE0("Error in renaming %s -> %s: %s\n", pOld, pNew, strerror(errno));
        return false;
    }
    return true;
}
