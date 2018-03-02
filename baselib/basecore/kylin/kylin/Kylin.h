#ifndef _KYLIN_H_
#define _KYLIN_H_

#include "KylinNS.h"

enum ThreadType {
    TT_DISK,
    TT_NETWORK,
    TT_EXEC,
    TT_PROXY,
};
const char* ThreadType2Str(int type);

enum APFError {
    
    APFE_OK                     =  0,
    APFE_NOTIFY                 = -1,
    APFE_SYS_ERROR              = -2,
    APFE_NET_ERROR              = -3,
    APFE_DISK_ERROR             = -4,
    APFE_CONNECT_ERROR          = -5,
    APFE_ALREADY_CREATED        = -6,
    APFE_NOT_CREATED            = -7,
    APFE_NO_MEMORY              = -8,
    APFE_NO_WORKER              = -9,
    APFE_AFTER_BROKEN           = -10,
    APFE_ALREADY_FREED          = -11,
    APFE_ALREADY_EXECED         = -12,
    APFE_CORRUPTED              = -13,
    APFE_ALREADY_STOPPED        = -14,
    APFE_NO_RETURN              = -15,
    APFE_CANCELED               = -16,
    APFE_TIMEDOUT               = -17,
    APFE_INVALID_ARGS           = -18,
    APFE_END                    = -256,
};

typedef int APF_ERROR;

#include "ThreadPool.h"
#include "Async.h"
#include "ExecMan.h"
#include "DiskMan.h"
#include "NetworkMan.h"


APF_ERROR InitKylin(int nExecWorkers, int nNetWorkers, int nDiskWorkers, 
                    THREAD_INIT_PROC f=NULL, uint32 nTimerPrecision=1000);

APF_ERROR StopKylin(bool bWait=false);
bool IsKylinRunning();
const char* APFError2Str(int err);

#endif
