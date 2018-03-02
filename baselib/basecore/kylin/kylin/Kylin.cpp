#include "stdafx.h"
#include "Kylin.h"
#include <signal.h>

_KYLIN_PACKAGE_BEGIN_

volatile int s_nKylinRunning = 0;

const char* ThreadType2Str(int type)
{
    switch (type) {
    case TT_NETWORK:
        return "Net";
    case TT_DISK:
        return "Disk";
    case TT_EXEC:
        return "Exec";
    case TT_PROXY:
        return "Proxy";
    }
    return "Unknown";
}

static void 
OnPipeBroken(int signum)
{
    TRACE5("OnPipeBroken, ignored!\n");
}

APF_ERROR InitKylin(int nExecWorkers, int nNetWorkers, int nDiskWorkers, 
                    THREAD_INIT_PROC f, uint32 nTimerPrecision)
{
    if (0 == AtomicSetValue(s_nKylinRunning, 1)) {
        signal(SIGPIPE, OnPipeBroken);

        if (0 >= nExecWorkers)
            nExecWorkers = 1;
        g_pExecMan->Start(nExecWorkers, f, nTimerPrecision);
        if (0 >= nNetWorkers)
            nNetWorkers = 1;
        g_pNetworkMan->Start(nNetWorkers, f);
        if (nDiskWorkers)
            g_pDiskMan->Start(nDiskWorkers, f);
        return APFE_OK;
    }
    return APFE_ALREADY_CREATED;
}

APF_ERROR StopKylin(bool bWait)
{
    int n = AtomicSetValue(s_nKylinRunning, 0);
    if (1 == n) {
        g_pExecMan->Stop(bWait);
        if (g_pNetworkMan->IsStarted())
            g_pNetworkMan->Stop();
        if (g_pDiskMan->IsStarted())
            g_pDiskMan->Stop();
        return APFE_OK;
    }
    return APFE_ALREADY_STOPPED;
}

bool IsKylinRunning()
{
    return 1 == AtomicGetValue(s_nKylinRunning);
}

const char* APFError2Str(int e)
{
    const char* err[] = {
        "Okay",
        "... APFE_NOTIFY",
        "System error",
        "Network error",
        "Disk error",
        "Fail to connect dest",
        "Already created",
        "Not created",
        "No enough memory",
        "No worker thread",
        "Can't r/w on a broken connection",
        "Already freed",
        "Already executed",
        "Data is corrupted",
        "... APFE_NO_RETURN",
        "Event has been canceled",
        "Operation time out",
        "Invalid arguments"
    };

    if (e > 0) {
        return strerror(e);
    }
    e = -e;
    if (e >= (int)(sizeof(err)/sizeof(err[0]))) {
        return "Unknown error";
    }
    return err[e];
}

_KYLIN_PACKAGE_END_
