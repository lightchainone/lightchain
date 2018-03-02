
#ifndef _SPINLOCK_H_
#define _SPINLOCK_H_

#include "atomic.h"
#include "thread.h"

#ifdef	__cplusplus
extern "C" {
#endif


#define barrier() asm volatile("": : :"memory")
 
 
#define cpu_relax() asm volatile("pause\n": : :"memory")

static inline void 
spin_lock(volatile int *lock)
{
    while (1)
    {
        int i = 0;
        if (!atomic_swap(lock, EBUSY)) return;
        while (*lock) {
            i++;
            if (i == 4000) {
                i = 0;
                thread_yield();
            }
            cpu_relax();
        }
    }
}

static inline void
spin_unlock(volatile int *lock)
{
    barrier();
    *lock = 0;
}

static inline int
token_acquire(volatile int *token, int id)
{
	return atomic_comp_swap(token, id, 0);
}

static inline int
token_release(volatile int *token, int id)
{
	return atomic_comp_swap(token, 0, id);
}

static inline int
token_transfer(volatile int *token, int oldid, int newid)
{
	return atomic_comp_swap(token, newid, oldid);
}

static inline int
token_set(volatile int *token, int id)
{
    return atomic_comp_swap(token, id, *token);
}

#ifdef	__cplusplus
}
#endif

#endif
