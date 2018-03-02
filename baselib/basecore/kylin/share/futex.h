
#ifndef _FUTEX_H_
#define _FUTEX_H_

#include "extypes.h"
#include "support.h"
#include "atomic.h"
#include "Trace.h"

struct futex {
	volatile int lock;
	volatile int count;
};

#define LARGE_ENOUGH_NEGATIVE			-0x7fffffff

#ifdef	__cplusplus
extern "C" {
#endif

static inline void
futex_init(struct futex* pf, int count)
{
	pf->lock = 0;
	pf->count = count;
}


static inline int 
futex_sema_down(struct futex* pf, struct timespec* timeout, bool interruptable)
{
	int n = atomic_add(&pf->count, -1);
	if (n <= 0) {
retry:
        if (0 == sys_futex(&pf->lock, FUTEX_WAIT, 0, timeout)) {
			return 0;
        }

		switch (errno) {
		case ETIMEDOUT: 
			atomic_add(&pf->count, 1); 
			return ETIMEDOUT;
		case EINTR: 
			if (!interruptable)
				goto retry;
			atomic_add(&pf->count, 1); 
			return EINTR;
		default:
			RaiseError(IMPOSSIBLE__Can_not_lock_in_futex_sema_down);
		}
	}
	return 0;
}


static inline int
futex_sema_up(struct futex* pf)
{
	int retry;
    int n = atomic_add(&pf->count, 1);
	if (n < 0) {
        retry = 10;
		while (1 != (n=sys_futex(&pf->lock, FUTEX_WAKE, 1, NULL))) {
			
            if (retry --) { 
                nop(); 
            } 
            else { 
                retry = 10; 
                thread_yield();
            }
		}
		return n;
	}
	return 0;
}


static inline int 
futex_cond_wait(struct futex* pf, struct timespec* timeout, bool interruptable)
{

    while (0 != AtomicGetValue(pf->count)) {
        sys_futex(&pf->lock, FUTEX_WAIT, 0, timeout);
		
        switch (errno) {
		case ETIMEDOUT: 
			return ETIMEDOUT;
		case EINTR: 
            if (interruptable) {
			    return EINTR;
            }
        default:
            break;
		}
	}
	return 0;






































}


static inline int
futex_cond_signal(struct futex* pf)
{
	int n = atomic_add(&pf->count, -1);
	if (1 == n) {
        pf->lock = 1;
        mfence_c();
		return sys_futex(&pf->lock, FUTEX_WAKE, 65535, NULL);	
	}
	return 0;
}

static inline int
futex_cond_revoke(struct futex* )
{
	
	return 0;
}


static inline int
futex_event_wait(struct futex* pf, struct timespec* timeout, bool interruptable)
{
    int n = atomic_add(&pf->count, 1);
    if (0 <= n) {    
retry:
        if (0 == sys_futex(&pf->lock, FUTEX_WAIT, 0, timeout))
            return 0;

        switch (errno) {
        case ETIMEDOUT: 
            atomic_add(&pf->count, -1); 
            return ETIMEDOUT;
        case EINTR: 
            if (!interruptable)
                goto retry;
            atomic_add(&pf->count, -1); 
            return EINTR;
        default:
            RaiseError(IMPOSSIBLE__Can_not_lock_in_futex_sema_down);
        }
    }
    else {  
        AtomicSetValue(pf->count, LARGE_ENOUGH_NEGATIVE);
    }
    return 0;
}



static inline int
futex_event_signal(struct futex* pf, bool reset)
{
    int m, n, retry;

    n = AtomicSetValue(pf->count, reset ? 0 : LARGE_ENOUGH_NEGATIVE);
    if (0 < n) {
        retry = 10;
        m = n;
        do {
            n -= sys_futex(&pf->lock, FUTEX_WAKE, n, NULL);
            if (0 == n)
                return m;
            if (retry --) { 
                nop(); 
            } 
            else { 
                retry = 10; 
                thread_yield();
            }
        } while (1);
    }
    return 0;
}

static inline void
futex_event_reset(struct futex* pf) 
{
    int n, retry = 10;
    do {
        n = AtomicSetValueIf(pf->count, 0, LARGE_ENOUGH_NEGATIVE);
        if (0<=n || LARGE_ENOUGH_NEGATIVE==n) {
            return;
        }
        if (retry --) { 
            nop(); 
        } 
        else { 
            retry = 10; 
            thread_yield();
        }
    } while (1);
}

#ifdef	__cplusplus
}
#endif

#endif
