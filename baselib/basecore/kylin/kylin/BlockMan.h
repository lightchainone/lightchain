#ifndef _BLOCK_MAN_H_
#define _BLOCK_MAN_H_

#include "DiskMan.h"

#define DIRECT_IO_ALIGNMENT			512

_KYLIN_PACKAGE_BEGIN_


#define MAX_MODIFY_LEN_IN_BITS		1048576		
#define MAKE_MOD_BLOCK_OFF(o)		((int)((o) >> 12) << 9)
#define MAKE_MOD_BLOCK_FLAG(o,s)	((int)(s<<12) | (int)((o) & 0xFFFULL))

struct BlockRequest;
typedef int (*BLOCK_FUNC)(int fd, BlockRequest* pReq, int* pnRead, int* pnWrite);

struct BlockRequest {
    union {
        AsyncContext async;
        Job job;
    };

    bool bChecksum;
    uint64 id;
    union {
        struct {
            void *buf;
            int len;
            int off;
        };
        BLOCK_FUNC func;
    };
};

class CBlockManV
{
plclic:
    CBlockManV() {}
    virtual ~CBlockManV() {}

    
    

    void ReadBlock(uint64 id, char* pBuf, int nSize, int nOff, 
                    bool bChecksum, CAsyncClient* pClient, BlockRequest* pReq);
    void WriteBlock(uint64 id, char* pBuf, int nSize, int nOff, 
                    bool bChecksum, CAsyncClient* pClient, BlockRequest* pReq);
    void ModifyBlock(uint64 id, char* pBuf, int nSizeInBits, uint64 nOffInBits, CAsyncClient* pClient, BlockRequest* pReq);
    void FuncBlock(uint64 id, int nOff, BLOCK_FUNC f, CAsyncClient* pClient, BlockRequest* pReq);

    void ReadBlock(CAsyncClient* pClient, BlockRequest* pReq);
    void WriteBlock(CAsyncClient* pClient, BlockRequest* pReq);
    void DeleteBlock(CAsyncClient* pClient, BlockRequest* pReq);
    void ModifyBlock(CAsyncClient* pClient, BlockRequest* pReq);
};

extern char* g_pRootDir;

_KYLIN_PACKAGE_END_

#endif
