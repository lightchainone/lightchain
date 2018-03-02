
#include "stdafx.h"
#include "Trace.h"
#include "SyncObj.h"

int g_nKLLogLevel = 0;
static FILE* s_fpLog = stderr;

void 
Trace(const char *fmt, ...)
{
	static CMutex s_lock;
	va_list	ap;
    if (s_fpLog) {
		s_lock.Lock();
        fprintf(s_fpLog, "[%d]", thread_getid());
	    va_start(ap, fmt);
	    vfprintf(s_fpLog, fmt, ap);
	    va_end(ap);
		s_lock.Unlock();
		fflush(s_fpLog);
	}
}


TRACE_FUNC g_fTrace = Trace;

void KLSetLog(FILE* fpLog, int nLogLevel, TRACE_FUNC f)
{
	s_fpLog = fpLog;
	g_nKLLogLevel = nLogLevel;
	if (NULL != f)
		g_fTrace = f;
}
