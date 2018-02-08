#include "log.h"
#include "lc_log.h"

int  zlog_open(const char *path,const char *proc_name,int level,int filelen, bool is_log_split )
{
    int spec = 0;
    if (is_log_split)
    {
        spec = UL_LOGSIZESPLIT;
    }
    return lc_log_init(path,proc_name,filelen,level,0,spec);
}


int zlog_open_r(const char *thread_name)
{
    return lc_log_initthread(thread_name);
}


int zlog_close()
{
    lc_log_close();
    return 0;
}


int zlog_close_r()
{
    lc_log_closethread();
    return 0;
}

