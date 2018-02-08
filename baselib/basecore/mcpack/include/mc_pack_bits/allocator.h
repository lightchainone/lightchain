#ifndef __ALLOCATOR__
#define __ALLOCATOR__

#include "typelist.h"
#include <string.h>
#ifdef __ALLOCATOR_DEBUG__
#include <stdlib.h>
#include <stdio.h>
#endif


class reverse_allocator {
    size_t buf_size;
    char * free_space;
    void * freelist[10];
plclic:
    reverse_allocator(char * buffer, size_t size) {
        buf_size = size;
        free_space = buffer + size;
        for(size_t i = 0; i< sizeof(freelist)/sizeof(freelist[0]); i++) {
            freelist[i] = 0;
        }
        
    }
    inline void * malloc(u_int size) {
        size = (size + 7 ) & ( -1 << 3);
#ifdef __ALLOCATOR_DEBUG__
        printf("malloc %d, used %d\n", size, buf_size);
#endif
        if(size <= buf_size) {
            buf_size -= size;
            free_space -= size;
            return free_space;
        } else
            return 0;
    }
    template<typename T>
    inline T * malloc() {
        const unsigned int index = (sizeof(T) + 7) /8;
        if(index < sizeof(freelist)/sizeof(freelist[0]) && freelist[index] != 0) {
            T * tmp = (T *)freelist[index];
            freelist[index] = *(T * *)freelist[index];
            return tmp;
        } else
            return (T *)malloc(sizeof(T));
    }
    template<typename T>
    inline void free( T * ptr) {
#ifdef __ALLOCATOR_DEBUG__
        printf("free %d, used %d\n", (int)sizeof(T), buf_size);
#endif
        const unsigned int index = (sizeof(T) + 7) /8;
        if(index < sizeof(freelist)/sizeof(freelist[0])) {
            T * tmp = (T *)freelist[index];
            freelist[index] = (T *)ptr;
            *(T * * )ptr = tmp;
        }
    }
};

template <u_int seg_size>
class mempool_allocator {
    size_t buf_size;
    char * free_space;
    void * memlist;
plclic:
    template <typename T>
    inline T *malloc() {
        return (T *)malloc(sizeof(T));
    }
    inline void * malloc(u_int size) {
        size = (size + 7 ) & ( -1 << 3);
        if(size < seg_size - sizeof(void *)) {
            if(size > buf_size) {
                void *tmp = ::malloc(seg_size);
                if(tmp) {
                    *(void **)tmp = memlist;
                    memlist = tmp;
                    free_space = (char *)tmp;
                    buf_size = seg_size - sizeof(void *);
                } else
                    return 0;
            }
            buf_size -= size;
            free_space += size;
            return free_space;
        } else {
            void *tmp = ::malloc(size + sizeof(void *));
            if(tmp) {
                *(void **)tmp = memlist;
                memlist = tmp;
                return ((char *)tmp + sizeof(void *));
            } else
                return 0;

        }
    }
    template<typename T>
    inline void free( T * ptr) {
    }
    mempool_allocator() {
        buf_size = 0;
        memlist = 0;
        free_space = 0;
    }
    ~mempool_allocator() {
        while(memlist) {
            void * tmp = *(void **)memlist;
            free(memlist);
            memlist = tmp;
        }
    }
};

template<typename types>
class typed_allocator {
    size_t buf_size;
    char * free_space;
    void * freelist[types::length];
plclic:
    typed_allocator(int size):buf_size(size) {
        free_space = (char *)this + sizeof(*this);
        for(int i = 0; i< sizeof(freelist)/sizeof(freelist[0]); i++) {
            freelist[i] = 0;
        }
        memset(free_space, 0, buf_size - sizeof(*this));
    }

    inline void * malloc(int size) {
        size = (size + 7 ) & ( -1 << 3);
        if((size_t)(free_space - (char *)this + size) < buf_size) {
            void * tmp = free_space;
            free_space += size;
            return tmp;
        } else
            return 0;
    }
    template<typename T>
    T * malloc() {
        if(freelist[index_of<types, T>::value] != 0) {
            T * tmp = (T *)freelist[index_of<types, T>::value];
            freelist[index_of<types, T>::value] = *(T * *)freelist[index_of<types, T>::value];
            return tmp;
        } else
            return (T *)malloc(sizeof(T));
    }
    template<typename T>
    void free( T * ptr) {

        T * tmp = (T *)freelist[index_of<types, T>::value];
        freelist[index_of<types, T>::value] = (T *)ptr;
        *(T * * )ptr = tmp;
    }
};
#endif
