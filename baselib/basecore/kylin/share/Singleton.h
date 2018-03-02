#ifndef _SINGLETON_H_
#define _SINGLETON_H_

#include "spinlock.h"

#define DECLARE_SINGLETON(classname)                            \
    plclic:                                                     \
        static classname * Instance() {                         \
            if (NULL != sm_pInstance) {                         \
                return sm_pInstance;                            \
            }                                                   \
            spin_lock(&sm_lock);                                \
            if (NULL == sm_pInstance) {                         \
                sm_pInstance = new classname();                 \
            }                                                   \
            spin_unlock(&sm_lock);                              \
            return sm_pInstance;                                \
        }                                                       \
        static bool IsCreated() {                               \
            bool b;                                             \
            spin_lock(&sm_lock);                                \
            b = (NULL != sm_pInstance);                         \
            spin_unlock(&sm_lock);                              \
            return b;                                           \
        }                                                       \
    protected:                                                  \
        classname();                                            \
        static classname * sm_pInstance;                        \
        static volatile int sm_lock;                            \
    private: 


#define IMPLEMENT_SINGLETON(classname)                          \
    classname* classname::sm_pInstance = NULL;                  \
    volatile int classname::sm_lock = 0;                        \
    classname::classname() {}


#define IMPLEMENT_SINGLETON_WITHOUT_CTOR(classname)             \
    volatile int classname::sm_lock = 0;                        \
    classname * classname::sm_pInstance = NULL;

#endif
