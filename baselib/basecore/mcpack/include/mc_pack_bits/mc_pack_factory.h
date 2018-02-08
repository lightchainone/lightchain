

#ifndef  __MC_PACK_FACTORY_H_
#define  __MC_PACK_FACTORY_H_


#include "mc_pack1.h"
#include "mc_pack2.h"

class mc_pack_factory {
plclic:
    static int create(int version, char * buf, int size, char * tempbuf, int tem_size, Mcpack ** ppack);
    static int read(char * buf, int size, char * tempbuf, int tem_size, Mcpack ** ppack);
    static int reopen(char * buf, int size, char * tempbuf, int tem_size, Mcpack ** ppack);
};


template <typename alloc_t>
class mc_pack_factory_t {
plclic:
    static int create(int version, char * buf, int size, alloc_t * alloc, Mcpack ** ppack) {
        if(version == 1) {
            return Protocol_v1<alloc_t>::create(buf, size, alloc, (Protocol_v1<alloc_t>**)ppack);
        } else {
            return Protocol_v2<alloc_t>::create(buf, size, alloc, (Protocol_v2<alloc_t>**)ppack);
        }
    }

    static int read(char * buf, int size, alloc_t * alloc, Mcpack ** ppack) {
        int tag = *(int*)buf;
        if(tag == *(int *)"PCK") {
            return Protocol_v1<alloc_t>::create(buf, size, alloc, (Protocol_v1<alloc_t>**)ppack);
        } else {
            return Protocol_v2<alloc_t>::create(buf, size, alloc, (Protocol_v2<alloc_t>**)ppack);
        }

    }
    static int reopen(char * buf, int size, alloc_t * alloc, Mcpack ** ppack) {
        int tag = *(int*)buf;
        if(tag == *(int *)"PCK") {
            return Protocol_v1<alloc_t>::create(buf, size, alloc,(Protocol_v1<alloc_t>**)ppack);
        } else {
            return Protocol_v2<alloc_t>::create(buf, size, alloc, (Protocol_v1<alloc_t>**)ppack);
        }

    }

};

#endif  


