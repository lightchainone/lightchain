#include "mc_pack_bits/mc_pack2.h"
#include "mc_pack_bits/mc_pack1.h"
#include <Lsc/ResourcePool.h>

class ResourcePool_allocator {
    Lsc::ResourcePool * _pool;
plclic:
    ResourcePool_allocator(Lsc::ResourcePool * pool) : _pool(pool) {
    }
    inline void * malloc(u_int size) {
        if(_pool != 0) {
            try {
                return _pool->create_raw(size);
            } catch(...) {
                return 0;
            }
        } else {
            return 0;
        }
    }
    template<typename T>
    inline T * malloc() {
        if(_pool != 0) {
            try {
                return (T*)_pool->create_raw(sizeof(T));
            } catch(...) {
                return 0;
            }
        } else {
            return 0;
        }
    }
    template<typename T>
    inline void free( T * ) {
        return;
    }
};

using namespace mcpack;
mc_pack_t * mc_pack_open_w_rp(int version, char * buf, int size, Lsc::ResourcePool * _pool) {
    Mcpack * pack;
    if(0 == buf || 0 == _pool) {
        return (mc_pack_t *)(long)MC_PE_BAD_PARAM;
    }
    int result;
    try {
        ResourcePool_allocator * rp_alloc = &_pool->create<ResourcePool_allocator>(_pool);
        if(version == 1) {
            result = Protocol_v1<ResourcePool_allocator>::create
                     (buf, size, rp_alloc, (Protocol_v1<ResourcePool_allocator> **)&pack);
        } else if(version == 2) {
            result = Protocol_v2<ResourcePool_allocator>::create
                     (buf, size, rp_alloc, (Protocol_v2<ResourcePool_allocator> **)&pack);
        } else {
            return (mc_pack_t *)(long)MC_PE_BAD_PARAM;
        }
        if(result != 0) {
            return (mc_pack_t *)(long)result;
        } else {
            return (mc_pack_t *)pack;
        }
    } catch(...) {
        return (mc_pack_t *)(long)MC_PE_NO_TEMP_SPACE;
    }
}
mc_pack_t * mc_pack_open_rw_rp(char * buf, int size, Lsc::ResourcePool * _pool) {
    if(0 == buf || size < (int)sizeof(int) || NULL == _pool) {
        return (mc_pack_t *)(long)MC_PE_BAD_PARAM;
    }
    int tag = *(int*)buf;
    Mcpack * pack;
    int result;
    try {
        ResourcePool_allocator * rp_alloc = &_pool->create<ResourcePool_allocator>(_pool);
        if(tag != *(int *)"PCK") {
            result =  Protocol_v2<ResourcePool_allocator>::reopen
                      (buf, size, rp_alloc, (Protocol_v2<ResourcePool_allocator> **)&pack);
        } else {
            result =  Protocol_v1<ResourcePool_allocator>::reopen
                      (buf, size, rp_alloc, (Protocol_v1<ResourcePool_allocator> **)&pack);
        }
        if(result != 0) {
            return (mc_pack_t *)(long)result;
        } else {
            return (mc_pack_t *)pack;
        }
        return 0;
    } catch(...) {
        return (mc_pack_t *)(long)MC_PE_NO_TEMP_SPACE;
    }
}

mc_pack_t * mc_pack_open_r_rp(const char * buf, int size, Lsc::ResourcePool * _pool) {
    if(0 == buf || size < (int)sizeof(int) || NULL == _pool) {
        return (mc_pack_t *)(long)MC_PE_BAD_PARAM;
    }
    int tag = *(int*)buf;
    Mcpack * pack;
    int result;
    try {
        ResourcePool_allocator * rp_alloc = &_pool->create<ResourcePool_allocator>(_pool);
        if(tag != *(int *)"PCK") {
            result =  Protocol_v2<ResourcePool_allocator>::read
                      (buf, size, rp_alloc, (Protocol_v2<ResourcePool_allocator> **)&pack);
        } else {
            result =  Protocol_v1<ResourcePool_allocator>::read
                      (buf, size, rp_alloc, (Protocol_v1<ResourcePool_allocator> **)&pack);
        }
        if(result != 0) {
            return (mc_pack_t *)(long)result;
        } else {
            return (mc_pack_t *)pack;
        }
        return 0;
    } catch(...) {
        return (mc_pack_t *)(long)MC_PE_NO_TEMP_SPACE;
    }
}
