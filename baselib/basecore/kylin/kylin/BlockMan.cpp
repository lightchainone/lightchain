
#define _XOPEN_SOURCE 500
#include "stdafx.h"
#include <alloca.h>
#include "Kylin.h"
#include "BlockMan.h"
#include "CycleTimer.h"
#include "support.h"
#include "utils.h"
#include "zlib.h"

#define IO_ERROR_RETRY_TIMES            3
#define ENABLE_BLOCK_CHECKSUM           1

char* g_pRootDir = "/home";
extern uint64 g_nDiskStats[];
extern uint64 g_nDiskCosts[];

static inline void
BlockIdToPath(uint64 id, char buf[])
{
    int n = (int)(uint8)id;
    sprintf(buf, "%s/disk%x/%llx", g_pRootDir, n, id);
}

static inline int
BlockIdToDiskNo(uint64 id) 
{
    return (uint32)(uint8)id;
}

static inline void
SetChksum(void* pBuf, int nLen)
{
    uint32 checksum = adler32(0, Z_NULL, 0);
    checksum = adler32(checksum, (Bytef*)pBuf+4, nLen-4);
    *(uint32*)pBuf = checksum;
}

static inline bool
CheckChksum(void* pBuf, int nLen)
{
    uint32 checksum = adler32(0, Z_NULL, 0);
    checksum = adler32(checksum, (Bytef*)pBuf+4, nLen-4);
    return (checksum == *(uint32*)pBuf);
}

#define NotifyClient(err, req)	{                                           \
        req->async.nErrCode = err;                                          \
        g_pExecMan->QueueExec((AsyncContext*)req, true);                    \
    }


static void
ReadBlockOp(Job* pJob)
{
    uint64 cost;
    int fd, len;
    char buf[1024];
    BlockRequest* pReq = (BlockRequest*)pJob;
    int retry_times = -1;

    BlockIdToPath(pReq->id, buf);
    do {
        fd = open(buf, O_RDONLY|O_DIRECT, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP);
        TRACE6("read %llx:%x, len=%x\n", pReq->id, pReq->off, pReq->len);
        if (-1 != fd) {
            cost = rdtsc();
            len = pread(fd, pReq->buf, pReq->len, pReq->off);
            cost = rdtsc() - cost;
#if PROFILING
            TRACE6("read(%d, %p, %d)=%d, #%d cost: %.2fms\n", fd,
                pReq->buf, pReq->len, len, (uint32)(uint8)pReq->id,
                timer.Elapsedms());
#endif
            if (0 < len) {
                
                int which = (BlockIdToDiskNo(pReq->id)<<1);
                g_nDiskStats[which] += len;
                g_nDiskCosts[which] += cost;
                pReq->len = len;
#if ENABLE_BLOCK_CHECKSUM
                if (!pReq->bChecksum || CheckChksum(pReq->buf, len)) {
                    NotifyClient(0, pReq);
                }
                else {
                    TRACE0("Error: block %llx is corrupted!\n", pReq->id);
                    NotifyClient(APFE_CORRUPTED, pReq);
                }
#else
                NotifyClient(0, pReq);
#endif
                close(fd);
                break;
            }
            else {
                PERROR("pread");
                if (EIO==errno && GetCpuFreq()>=cost) {
                    
                    if (IO_ERROR_RETRY_TIMES > ++retry_times) {
                        TRACE0("Retry #%d in reading %s\n", retry_times, buf);
                        Sleep(100);
                    }
                    else {
                        NotifyClient(EIO, pReq);
                        TRACE0("Fatal: I/O error in reading %s.\n", buf);
                    }
                    close(fd);
                }
                else {
                    NotifyClient(errno?errno:EIO, pReq);
                    TRACE0("Fatal: I/O error in reading %s. Cost: %s.\n", buf, TimeInterval2Str((uint32)(cost/GetCpuFreq())));
                    close(fd);
                    break;
                }
            }
        }
        else {
            NotifyClient(errno, pReq);
            TRACE0("Error in r_open %s: %s\n", buf, strerror(errno));
            break;
        }
    } while (IO_ERROR_RETRY_TIMES > retry_times);
}

static void
WriteBlockOp(Job* pJob)
{
    uint64 cost;
    int fd, len;
    char buf[1024];
    BlockRequest* pReq = (BlockRequest*)pJob;
    int retry_times = -1;

    BlockIdToPath(pReq->id, buf);
    do {
        fd = open(buf, O_CREAT|O_TRUNC|O_WRONLY|O_DIRECT, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP);
        TRACE6("write %llx:%x, len=%x\n", pReq->id, pReq->off, pReq->len);
        if (-1 != fd) {
            cost = rdtsc();
#if ENABLE_BLOCK_CHECKSUM
            if (pReq->bChecksum) {
                SetChksum(pReq->buf, pReq->len);
            }
#endif
            len = pwrite(fd, pReq->buf, pReq->len, pReq->off);
            cost = rdtsc() - cost;
#if PROFILING
            TRACE6("write(%d, %p, %d)=%d, #%d cost: %.2fms\n", fd,
                pReq->buf, pReq->len, len, (uint32)(uint8)pReq->id,
                timer.Elapsedms());
#endif
            if (0 < len) {
                
                int which = (BlockIdToDiskNo(pReq->id)<<1) + 1;
                g_nDiskStats[which] += len;
                g_nDiskCosts[which] += cost;
                pReq->len = len;
                NotifyClient(0, pReq);
                close(fd);
                break;
            }
            else {
                PERROR("pwrite");
                if (EIO==errno && GetCpuFreq()>=cost) {  
                    
                    if (IO_ERROR_RETRY_TIMES > ++retry_times) {
                        TRACE0("Retry #%d in writing %s\n", retry_times, buf);
                        Sleep(100);
                    }
                    else {
                        NotifyClient(EIO, pReq);
                        TRACE0("Fatal: I/O error in writing %s\n", buf);
                    }
                    close(fd);
                }
                else {
                    NotifyClient(errno?errno:EIO, pReq);
                    TRACE0("Fatal: I/O error in writing %s.\n", buf);
                    close(fd);
                    break;
                }
            }
        }
        else {
            NotifyClient(errno, pReq);
            TRACE0("Error in w_open %s: %s\n", buf, strerror(errno));
            break;
        }
    } while (IO_ERROR_RETRY_TIMES > retry_times);
}

static void
DeleteBlockOp(Job* pJob)
{
    char buf[256];
    BlockRequest* pReq = CONTAINING_RECORD(pJob, BlockRequest, job);
    BlockIdToPath(pReq->id, buf);

    if (0 == unlink(buf)) {
        NotifyClient(0, pReq);
    }
    else {
        NotifyClient(errno, pReq);
        TRACE1("Error in unlink %s: %s\n", buf, strerror(errno));
    }
}

#define QUEUE_TASK(a, b, f)                                         \
    int n = BlockIdToDiskNo(id);                                    \
    pReq->async.pClient = pClient;                                  \
    pClient->AddRef();                                              \
    pReq->async.nAction = a;                                        \
    pReq->buf = pBuf;                                               \
    pReq->len = nSize;                                              \
    pReq->off = nOff;                                               \
    pReq->id = id;                                                  \
    pReq->bChecksum = b;                                            \
    pReq->job.fProc = f;                                            \
    g_pDiskMan->_QueueJob(&pReq->job, n)
 

void CBlockManV::ReadBlock(uint64 id, char* pBuf, int nSize, int nOff, 
                           bool bChecksum, CAsyncClient* pClient, BlockRequest* pReq)
{
    ASSERT(nSize % DIRECT_IO_ALIGNMENT == 0);
    ASSERT(nOff % DIRECT_IO_ALIGNMENT == 0);
    QUEUE_TASK(AA_READ_BLOCK, bChecksum, ReadBlockOp);
}

void CBlockManV::WriteBlock(uint64 id, char* pBuf, int nSize, int nOff, 
                            bool bChecksum, CAsyncClient* pClient, BlockRequest* pReq)
{
    ASSERT(nSize % DIRECT_IO_ALIGNMENT == 0);
    ASSERT(nOff % DIRECT_IO_ALIGNMENT == 0);
    QUEUE_TASK(AA_WRITE_BLOCK, bChecksum, WriteBlockOp);
}



static int 
DoMod(int fd, int off, int flag, char* pMod)
{
    int nOffInBlock = flag & 0xFFF;
    int len = flag >> 12;
    int nToRead = PACK_512((nOffInBlock+len+7)>>3);
    char* pBuf = ALIGN_512_ALLOCA(nToRead);

    if (0 != off)
        lseek(fd, off, SEEK_SET);
    if (0 >= read(fd, pBuf, nToRead)) {
        PERROR("read");
        return 0;
    }

    
    if ((nOffInBlock != ((nOffInBlock>>3)<<3))
        || (len != ((len>>3)<<3))
    ) {
        ASSERT((nOffInBlock % 8) + len <= 64);
        SetBits(pBuf, pMod, nOffInBlock, len);
    }
    else {
        ASSERT((len % 8) == 0);
        memcpy(pBuf+nOffInBlock, pMod, len>>3);
    }

    lseek(fd, off, SEEK_SET);
    if (nToRead != write(fd, pBuf, nToRead)) {
        PERROR("write");
        return 0;
    }
    return nToRead;
}

static void
ModifyBlockOp(Job* pJob)
{
    int fd, len;
    char buf[256];
    BlockRequest* pReq = (BlockRequest*)pJob;
    
    BlockIdToPath(pReq->id, buf);
    fd = open(buf, O_RDWR|O_DIRECT, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP);
    TRACE6("mod %llx:%x\n", pReq->id, pReq->off);
    if (fd != -1) {
        uint64 cost = rdtsc();
        len = DoMod(fd, pReq->off, pReq->len, (char*)pReq->buf);
        cost = rdtsc() - cost;
#if PROFILING
        TRACE6("Mod(%d, %p, %d)=%d, #%d cost: %.2fms\n", fd,
            pReq->buf, pReq->len, len, (uint32)(uint8)pReq->id,
            timer.Elapsedms());                                               
#endif
        if (len > 0) {                                                      
            
            int which = (BlockIdToDiskNo(pReq->id)<<1);
            g_nDiskStats[which] += len;
            g_nDiskCosts[which] += cost;
            which ++;
            g_nDiskStats[which] += len;
            g_nDiskCosts[which] += cost;
            pReq->len = len;
            NotifyClient(0, pReq);
        }
        else {                                                              
            NotifyClient(errno, pReq);                                      
        }                                                                   
        close(fd);                                                          
    }
    else {                                                                  
        TRACE0("Error in open %s: %s\n", buf, strerror(errno));             
        NotifyClient(errno, pReq);
    }
}

void CBlockManV::ModifyBlock(uint64 id, char* pBuf, int nSizeInBits, 
                             uint64 nOffInBits, CAsyncClient* pClient, 
                             BlockRequest* pReq)
{
    ASSERT(nSizeInBits <= MAX_MODIFY_LEN_IN_BITS);
    int nOff = MAKE_MOD_BLOCK_OFF(nOffInBits);
    int nSize = MAKE_MOD_BLOCK_FLAG(nOffInBits, nSizeInBits);
    ASSERT(nOff % DIRECT_IO_ALIGNMENT == 0);

    QUEUE_TASK(AA_MODIFY_BLOCK, false, ModifyBlockOp);
}

static void
FuncBlockOp(Job* pJob)
{
    int fd, rlen, wlen, ret;
    char buf[256];
    BlockRequest* pReq = (BlockRequest*)pJob;

    BlockIdToPath(pReq->id, buf);
    fd = open(buf, O_RDWR|O_DIRECT, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP);
    TRACE6("func %llx:%x\n", pReq->id, pReq->off);
    if (fd != -1) {
        ret = pReq->func(fd, pReq, &rlen, &wlen);
        g_nDiskStats[(BlockIdToDiskNo(pReq->id)<<1)] += rlen;
        g_nDiskStats[(BlockIdToDiskNo(pReq->id)<<1)+1] += wlen;
        NotifyClient(ret, pReq);
        close(fd);                                                          
    }
    else {                                                                  
        TRACE0("Error in open %s: %s\n", buf, strerror(errno));             
        NotifyClient(errno, pReq);
    }
}

void CBlockManV::FuncBlock(uint64 id, int nOff, BLOCK_FUNC f, 
                           CAsyncClient* pClient, BlockRequest* pReq)
{
    int n = BlockIdToDiskNo(id);
    pReq->async.pClient = pClient;
	pClient->AddRef();
    pReq->async.nAction = AA_FUNC_BLOCK;
    pReq->func = f;
    pReq->off = nOff;
    pReq->id = id;
    pReq->job.fProc = FuncBlockOp;
    g_pDiskMan->_QueueJob(&pReq->job, n);
}

#define QUEUE_TASK2(a, f)                                       \
    int n = BlockIdToDiskNo(pReq->id);                          \
    pReq->async.pClient = pClient;                              \
    pClient->AddRef();                                          \
    pReq->async.nAction = a;                                    \
    pReq->job.fProc = f;                                        \
    g_pDiskMan->_QueueJob(&pReq->job, n)


void CBlockManV::ReadBlock(CAsyncClient* pClient, BlockRequest* pReq)
{
    ASSERT(pReq->len % DIRECT_IO_ALIGNMENT == 0);
    ASSERT(pReq->off % DIRECT_IO_ALIGNMENT == 0);
    QUEUE_TASK2(AA_READ_BLOCK, ReadBlockOp);
}

void CBlockManV::WriteBlock(CAsyncClient* pClient, BlockRequest* pReq)
{
    ASSERT(pReq->len % DIRECT_IO_ALIGNMENT == 0);
    ASSERT(pReq->off % DIRECT_IO_ALIGNMENT == 0);
    QUEUE_TASK2(AA_WRITE_BLOCK, WriteBlockOp);
}

void CBlockManV::DeleteBlock(CAsyncClient* pClient, BlockRequest* pReq)
{
    QUEUE_TASK2(AA_DELETE_BLOCK, DeleteBlockOp);
}

void CBlockManV::ModifyBlock(CAsyncClient* pClient, BlockRequest* pReq)
{
    ASSERT(pReq->off % DIRECT_IO_ALIGNMENT == 0);
    QUEUE_TASK2(AA_MODIFY_BLOCK, ModifyBlockOp);
}

