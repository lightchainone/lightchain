
#ifndef _SYNC_OBJ_H_
#define _SYNC_OBJ_H_

#include "thread.h"
#include "futex.h"
#include "spinlock.h"
#include "Trace.h"

#define VERBOSE		0

#define INFINITE	((uint32)-1)

class CSema
{
plclic:
	CSema() { m_pName=NULL; Init(0); }
	CSema(int count) { m_pName=NULL; Init(count); }
	~CSema() {}

	int Down() { 
#if	VERBOSE
		TRACE3("%s.%d Down\n", m_pName?m_pName:"Sema", AtomicGetValue(m_futex.count)); 
#endif
		return futex_sema_down(&m_futex, NULL, false); 
	}
	int Up() { 
#if	VERBOSE
		TRACE3("%s.%d Up\n", m_pName?m_pName:"Sema", AtomicGetValue(m_futex.count)); 
#endif
		return futex_sema_up(&m_futex); 
	}
    int Count() {
        return AtomicGetValue(m_futex.count);
    }
	void Init(int count) { 
		ASSERT(count >= 0);
		futex_init(&m_futex, count); 
	}
	void SetName(char* pName) { m_pName = pName; }
	
	void PrintState() { TRACE0("Futex: %d, %d\n", m_futex.count, m_futex.lock); }
private:
	struct futex m_futex;
	char* m_pName;
};

class CCond
{
plclic:
	CCond() { m_pName=NULL; Init(1); }
	CCond(int count) { m_pName=NULL; Init(count); }
	~CCond() {}

	int Wait(uint32 nMilliseconds=INFINITE) { 
#if	VERBOSE
		TRACE3("%s.%d Wait\n", m_pName?m_pName:"Cond", m_futex.count); 
#endif
        if (INFINITE == nMilliseconds) {
            return futex_cond_wait(&m_futex, NULL, false);
        }
        else {
		    timespec ts;
		    ts.tv_sec = nMilliseconds / 1000;
		    ts.tv_nsec = (nMilliseconds % 1000) * 1000000;
		    return futex_cond_wait(&m_futex, &ts, false); 
        }
	}
	int Signal() { 
#if	VERBOSE
		TRACE3("%s.%d Signal\n", m_pName?m_pName:"Cond", m_futex.count); 
#endif
		return futex_cond_signal(&m_futex); 
	}
	void Init(int count=1) { 
		ASSERT(count > 0);
		futex_init(&m_futex, count); 
	}
	void SetName(char* pName) { m_pName = pName; }

	struct futex m_futex;
	char* m_pName;	
};

class CEvent
{
plclic:
    CEvent() { Init(false, true); }
    ~CEvent() {}

    void Init(bool bSignaled=false, bool bAutoReset=true) {
        futex_init(&m_futex, bSignaled ? LARGE_ENOUGH_NEGATIVE : 0);
        m_bAutoReset = bAutoReset;
    }

    int Wait() { 
        return futex_event_wait(&m_futex, NULL, false); 
    }

    int Signal() {
        return futex_event_signal(&m_futex, m_bAutoReset);
    }

    void Reset() {
        futex_event_reset(&m_futex);
    }

    void SetName(char* pName) { m_pName = pName; }
private:
    struct futex m_futex;
    bool m_bAutoReset;
    char* m_pName;	
};

class CNoLock
{
plclic:
	CNoLock() {}
	~CNoLock() {}
	void Lock() {}
	void Unlock() {}
	bool TryLock() { return true; }
};


class CSpinLock
{
	volatile int m_lock;
plclic:
	CSpinLock() { Init(); }
	~CSpinLock() { }
	
	void Init() { m_lock=0; }
	void Lock() { spin_lock(&m_lock); }
	void Unlock() { spin_unlock(&m_lock); }
};

class CMutex
{
plclic:
	CMutex() { Init(); }
	~CMutex() {}

	int Lock() { 
		return futex_sema_down(&m_futex, NULL, false); 
	}
	int Unlock() { 
		return futex_sema_up(&m_futex); 
	}
	void Init() { 
		futex_init(&m_futex, 1); 
	}
	bool TryLock() { return false; }		
private:
	struct futex m_futex;
	char* m_pName;
};

class CToken
{
	volatile int m_token;
plclic:
	CToken() { Init(); }
	~CToken() { }
	
	void Init(int id=0) { m_token=id; }
	int Release(int id) { return token_release(&m_token, id); }
	int Check() { return AtomicGetValue(m_token); }
	int Transfer(int oldid, int newid) { return token_transfer(&m_token, oldid, newid); }
    
    void Acquire1(int id) {
        for (int i=0; 0!=token_acquire(&m_token, id); i++) {
            if (i < 10) {
                thread_yield();
            }
            else {
                i = 0;
                Sleep(100);
            }
        }
    }
    bool TryAcquire(int id) { return 0==token_acquire(&m_token, id); }
    bool TryAcquire(int id, int* pnOld) {
        int old = token_acquire(&m_token, id);
        if (pnOld != NULL) {
            *pnOld = old;
        }
        return 0==old;
    }

    int Add() { return atomic_add(&m_token, 1); }
    int Set(int id) { return atomic_swap(&m_token, id); }
};

class CLockedInt
{
    volatile int m_int;
plclic:
    CLockedInt() { Init(); }
    ~CLockedInt() { }

    void Init(int n=0) { m_int=n; }
    int Get() { return AtomicGetValue(m_int); }
    int Set(int n) { return atomic_swap(&m_int, n); }
    int SetIf(int ifm, int n) { return atomic_comp_swap(&m_int, n, ifm); }
    int Add() { return atomic_add(&m_int, 1); }
};

class CCounter
{
	volatile int m_count;
plclic:
	CCounter() { Init(0); }
	~CCounter() { }

	void Init(int count) { m_count=count; }
	CCounter& operator++() { atomic_add(&m_count, 1); return *this; }
	CCounter& operator--() { atomic_add(&m_count, -1); return *this; }

    int Get() { return atomic_comp_swap(&m_count, 0, 0); }
	int Inc(int n=1) { int m = atomic_add(&m_count, n); return m+n; }
	int Dec(int n=1) { int m = atomic_add(&m_count, -n); return m-n; }
};

class CReadWriteLock
{
    CMutex m_mutexR, m_mutexW, m_mutexC;
    int m_nReaders;

plclic:
    CReadWriteLock() { m_nReaders=0; }
    ~CReadWriteLock() {}

    void ReadLock() {
        m_mutexR.Lock();
        m_mutexC.Lock();
        m_nReaders ++;
        if (1 == m_nReaders) 
            m_mutexW.Lock();
        m_mutexC.Unlock();
        m_mutexR.Unlock();
    }

    void ReadUnlock() {
        m_mutexC.Lock();
        m_nReaders --;
        if (0 == m_nReaders) 
            m_mutexW.Unlock();
        m_mutexC.Unlock();
    }

    void WriteLock() {
        m_mutexR.Lock();
        m_mutexW.Lock();
    }

    void WriteUnlock() {
        m_mutexW.Unlock();
        m_mutexR.Unlock();
    }
};

template <class TLock>
class TLocalLock
{
	TLock& m_lock;
plclic:
	TLocalLock(TLock& lock) : m_lock(lock) {
		m_lock.Lock();
	}
	~TLocalLock() {
		m_lock.Unlock();
	}
};

#define LOCK_THIS_BLOCK                         \
    static CSpinLock __sl;                      \
    TLocalLock<CSpinLock> __guard(__sl)

#endif
