#include "mc_pack_bits/mc_pack_factory.h"
#include "mc_pack_bits/allocator.h"

int mc_pack_factory::create(int version, char * buf, int size, char * tempbuf, int tmp_size, Mcpack ** ppack) {
    int tag = *(int*)buf;
    int result;
    if(tag != *(int *)"PCK") {
        result =  Protocol_v2<reverse_allocator>::reopen(buf, size, tempbuf, tmp_size, (Protocol_v2<reverse_allocator> **)ppack);
    } else {
        result =  Protocol_v1<reverse_allocator>::reopen(buf, size, tempbuf, tmp_size, (Protocol_v1<reverse_allocator> **)ppack);
    }
    return 0;
}

int mc_pack_factory::read(char * buf, int size, char * tempbuf, int tem_size, Mcpack ** ppack) {
    int tag = *(int*)buf;
    if(tag == *(int *)"PCK") {
        return Protocol_v1<reverse_allocator>::read(buf, size, tempbuf, tem_size, (Protocol_v1<reverse_allocator> **)ppack);
    } else {
        return Protocol_v2<reverse_allocator>::read(buf, size, tempbuf, tem_size, (Protocol_v2<reverse_allocator> **)ppack);
    }

}

int mc_pack_factory::reopen(char * buf, int size, char * tempbuf, int tem_size, Mcpack ** ppack) {
    int tag = *(int*)buf;
    if(tag == *(int *)"PCK") {
        return Protocol_v1<reverse_allocator>::reopen(buf, size, tempbuf, tem_size, (Protocol_v1<reverse_allocator> **)ppack);
    } else {
        return Protocol_v2<reverse_allocator>::reopen(buf, size, tempbuf, tem_size, (Protocol_v2<reverse_allocator> **)ppack);
    }

}
















