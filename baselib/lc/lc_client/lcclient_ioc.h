
#ifndef  __LCCLIENTIOC_H_
#define  __LCCLIENTIOC_H_

#include <lc/lc2.h>
#include "lcclient_strategy.h"
#include "lcclient_healthycheck.h"

namespace lc {
    typedef lc::IOC<LcClientStrategy> strategy_ioc;                   
    typedef lc::IOC<LcClientHealthyChecker> healthychecker_ioc;       

    
    strategy_ioc *lcclient_get_strategyioc(); 

    
    healthychecker_ioc *lcclient_get_healthycheckerioc();

}
    
#define LCCLIENT_GEN_IOC_CREATE(ClassName)\
    void * ClassName##_ioc_create_func() { \
        return static_cast<void *> (new ClassName); \
    }\
    void ClassName##_ioc_construct_func(void *p) {\
        ::new(p) ClassName;\
    }\
    size_t ClassName##_ioc_sizeof() {\
        return sizeof(ClassName);\
    }

    
#define LCCLIENT_ADD_IOC_CLASS(ioc, ClassName) \
    {\
     ioc->add(#ClassName, ClassName##_ioc_create_func, ClassName##_ioc_construct_func, \
            ClassName##_ioc_sizeof);\
    }

#endif  


