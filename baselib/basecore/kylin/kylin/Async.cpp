#include "stdafx.h"
#include "Kylin.h"

_KYLIN_PACKAGE_BEGIN_

static volatile int s_nId = 0;

CAsyncClient::CAsyncClient() 
{
    m_nRef = 0;
	m_nId = atomic_add(&s_nId, 1);
	sprintf(m_name, "AsyncClient#%d", m_nId);
    m_nHostId = m_nId;
}

CAsyncClient::CAsyncClient(CAsyncClient* pHost) 
{
    m_nRef = 0;
    m_nId = atomic_add(&s_nId, 1);
    sprintf(m_name, "AsyncClient#%d", m_nId);
    m_nHostId = pHost ? pHost->m_nHostId : m_nId;
}

void CAsyncClient::SetHost(CAsyncClient* pHost)
{
    if (pHost) {
        m_nHostId = pHost->m_nHostId;
    }
}

int CAsyncClient::GetHostThreadId() const 
{ 
    return g_pExecMan->GetWorkerThreadId(m_nHostId); 
}

bool CAsyncClient::IsInHostThread() const 
{ 
    return thread_getid() == g_pExecMan->GetWorkerThreadId(m_nHostId); 
} 

CAsyncClient::~CAsyncClient() 
{
#ifdef  _DEBUG
    int n, count=0;
    while (1) {
        for (int i=0; i<10; i++) {
            if (0 == (n=AtomicGetValue(m_nRef))) {
                return;
            }
            thread_yield();
        }
        if (!IsKylinRunning())
            return;
        if (0 != count) {   
            TRACE0("%s(%p) can't be deleted. m_nRef=%d. %d\n", m_name, this, n, count);
        }
        if (count ++ < 30) {
            sleep(1);
        }
        else {
            RaiseError(Cant_Delete_AsyncClient_Due_to_RefCnt);
        }
    }
#endif
}

void CAsyncClient::SetName(const char *fmt, ...)
{ 
	va_list	ap;
	int n;

	va_start(ap, fmt);
	n = vsnprintf(
		m_name,
		sizeof(m_name),
		
		fmt,
		ap
	);
	va_end(ap);
	
	if (n > (int)sizeof(m_name)-1) {
		sprintf(m_name, "AsyncClient#%d", m_nId);
	}
}

_KYLIN_PACKAGE_END_
