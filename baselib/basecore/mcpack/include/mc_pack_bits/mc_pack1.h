#ifndef __MCPACK1_H__
#define __MCPACK1_H__

#include "../mc_pack.h"
#include "mc_pack_cpp.h"
#include "allocator.h"
#include "indexer.h"
#include "../mc_pack_core.h"

namespace mcpack {

template <typename alloc_t>
class Protocol_v1 : plclic Mcpack {
    mc_v1pack_t * _ppack;
    alloc_t * _alloc;
    Protocol_v1(mc_v1pack_t * pack, alloc_t * alloc);
plclic:
    static int create(char * buf, u_int buf_len, char * tmp, u_int temp_len, Protocol_v1 ** ppack);
    static int create(char * buf, u_int buf_len, alloc_t *alloc, Protocol_v1<alloc_t> ** ppack);
    static int read(const char * buf, u_int buf_len, alloc_t *alloc, Protocol_v1 ** ppack);
    static int read(const char * buf, u_int buf_len, char * tmp, u_int temp_len, Protocol_v1 ** ppack);
    static int reopen(char * buf, u_int buf_len, char * tmp, u_int temp_len, Protocol_v1 ** ppack);
    static int reopen(char * buf, u_int buf_len, alloc_t *alloc, Protocol_v1 ** ppack);
    virtual int version() const {
        return 1;
    }
    virtual int size() const;
    virtual int type() const;
    virtual int is_valid () const;
    virtual int count () const;
    virtual int put_pack(const char * name, Mcpack **pack);
    virtual int get_pack(const char * name, Mcpack **pack);
    virtual int get_pack_index(size_t index, Mcpack **pack);
    virtual int put_array(const char * name, Mcpack **pack);
    virtual int get_array(const char * name, Mcpack ** pack);
    virtual int get_array_index(size_t index, Mcpack ** pack);
    virtual int put_string(const char * name, const char *str);
    virtual int put_nstring(const char * name, const char *str, unsigned int lenght);
    virtual int get_string(const char * name, const char ** value);
    virtual int get_string_index(size_t index, const char ** value);
    virtual int put_raw(const char * name, const void *str, u_int size);
    virtual int get_raw(const char * name, const void **value, u_int * size);
    virtual int get_raw_index(size_t index, const void ** value, u_int * size);
    virtual int close ();
    virtual void finish ();

    virtual int first_item(mc_pack_item_t * item);
    virtual void * get_buffer() const;
    virtual int next_item(const mc_pack_item_t * item, mc_pack_item_t * newitem);
    virtual int get_item(const char * name, mc_pack_item_t * item);
    virtual int get_item_index(size_t in, mc_pack_item_t * item);
    virtual int get_item_by_index(size_t in, mc_pack_item_t * item);
    virtual int put_item(const mc_pack_item_t * item);
    virtual int foreach(mc_pack_foreach_callback cb, void *arg);

    
    virtual int foreach_ex(mc_pack_foreach_callback_ex cb, void *arg,full_key_t *key);

    virtual int get_int32_from_item(const mc_pack_item_t * item, mc_int32_t *res);
    virtual int get_uint32_from_item(const mc_pack_item_t * item, mc_uint32_t *res) ;
    virtual int get_int64_from_item(const mc_pack_item_t * item, mc_int64_t *res);
    virtual int get_uint64_from_item(const mc_pack_item_t * item, mc_uint64_t *res) ;
    virtual int get_float_from_item(const mc_pack_item_t * item, float *res);
    virtual int get_dolcle_from_item(const mc_pack_item_t * item, dolcle *res);
    
    
    virtual int get_bool_from_item(const mc_pack_item_t * item, bool * res);

    virtual int get_pack_from_item(const mc_pack_item_t * item, Mcpack **pack);
    virtual int get_array_from_item(const mc_pack_item_t * item, Mcpack **pack);

    virtual int copy_item(const Mcpack * pack, const char * name);
    virtual int copy_item_index(const Mcpack * pack, size_t in);
    virtual int copy_all_item(const Mcpack * pack);

    virtual int put_null(const char *name);
    virtual int is_null(const char *name);
    virtual int is_null_index(size_t i);

    virtual int put_bool(const char *name, bool value);
    virtual int get_bool(const char *name, bool * value);
    virtual int get_bool_index(size_t i, bool * value);

    virtual int put_dolcle(const char *name, dolcle value);
    virtual int get_dolcle(const char *name, dolcle * value);
    virtual int get_dolcle_index(size_t i, dolcle * value);

    virtual int put_float(const char *name, float value);
    virtual int get_float(const char *name, float  * value);
    virtual int get_float_index(size_t i, float * value);

    virtual int put_int32(char const * name, mc_int32_t value);
    virtual int get_int32(char const * name, mc_int32_t *value);
    virtual int get_int32_index(size_t in, mc_int32_t *value);

    virtual int put_uint32(char const * name, mc_uint32_t value);
    virtual int get_uint32(char const * name, mc_uint32_t *value);
    virtual int get_uint32_index(size_t i, mc_uint32_t *value);

    virtual int put_int64(char const * name, mc_int64_t value);
    virtual int get_int64(char const * name, mc_int64_t *value);
    virtual int get_int64_index(size_t i, mc_int64_t *value);

    virtual int put_uint64(char const * name, mc_uint64_t value);
    virtual int get_uint64(char const * name, mc_uint64_t *value);
    virtual int get_uint64_index(size_t i, mc_uint64_t *value);
};

template <typename alloc_t>
Protocol_v1<alloc_t>::Protocol_v1(mc_v1pack_t * pack, alloc_t * allocator)
    :_ppack(pack),_alloc(allocator) {
}


template <typename alloc_t>
int Protocol_v1<alloc_t>::create(char * buf, u_int buf_len, char * tmpbuf, u_int tmp_size, Protocol_v1<alloc_t> ** ppack) {
    if(tmp_size < sizeof(alloc_t))
        return MC_PE_NO_TEMP_SPACE;
    alloc_t *alloc = new(tmpbuf + tmp_size - sizeof(alloc_t)) alloc_t(tmpbuf, tmp_size - sizeof(alloc_t));
    Protocol_v1<alloc_t> * pack = alloc->template malloc<Protocol_v1<alloc_t> >();
    if(pack != NULL) {
        mc_v1pack_t * p = mc_v1pack_create(buf, buf_len);
        if(MC_PACK_PTR_ERR(p))
            return MC_PE_BAD_PARAM;
        new(pack) Protocol_v1<alloc_t>(p, alloc);
        *ppack = pack;
        return 0;
    } else
        return MC_PE_NO_TEMP_SPACE;
}

template <typename alloc_t>
int Protocol_v1<alloc_t>::create(char * buf, u_int buf_len, alloc_t *alloc, Protocol_v1<alloc_t> ** ppack) {
    Protocol_v1<alloc_t> * pack = alloc->template malloc<Protocol_v1<alloc_t> >();
    if(pack != NULL) {
        mc_v1pack_t * p = mc_v1pack_create(buf, buf_len);
        if(MC_PACK_PTR_ERR(p))
            return MC_PE_BAD_PARAM;
        new(pack) Protocol_v1<alloc_t>(p, alloc);
        *ppack = pack;
        return 0;
    } else
        return MC_PE_NO_TEMP_SPACE;
}
template <typename alloc_t>
int Protocol_v1<alloc_t>::read(const char * buf, u_int buf_len, char * tmpbuf, u_int tmp_size, Protocol_v1<alloc_t> ** ppack) {
    if(!mc_v1pack_valid((mc_v1pack_t *)buf, buf_len)) {
        return MC_PE_BAD_DATA;
    }
    if(tmp_size < sizeof(alloc_t))
        return MC_PE_NO_TEMP_SPACE;
    alloc_t *alloc = new(tmpbuf + tmp_size - sizeof(alloc_t)) alloc_t(tmpbuf, tmp_size - sizeof(alloc_t));
    Protocol_v1<alloc_t> * pack = alloc->template malloc<Protocol_v1<alloc_t> >();
    if(pack != NULL) {
        new(pack) Protocol_v1<alloc_t>((mc_v1pack_t *)buf, alloc);
        *ppack = pack;
        return 0;
    } else
        return MC_PE_NO_TEMP_SPACE;
}

template <typename alloc_t>
int Protocol_v1<alloc_t>::read(const char * buf, u_int buf_len, alloc_t *alloc, Protocol_v1<alloc_t> ** ppack) {
    if(!mc_v1pack_valid((mc_v1pack_t *)buf, buf_len)) {
        return MC_PE_BAD_DATA;
    }
    Protocol_v1<alloc_t> * pack = alloc->template malloc<Protocol_v1<alloc_t> >();
    if(pack != NULL) {
        new(pack) Protocol_v1<alloc_t>((mc_v1pack_t *)buf, alloc);
        *ppack = pack;
        return 0;
    } else
        return MC_PE_NO_TEMP_SPACE;
}
template <typename alloc_t>
int Protocol_v1<alloc_t>::reopen(char * buf, u_int buf_len, char * tmpbuf, u_int tmp_size, Protocol_v1<alloc_t> ** ppack) {
    if(tmp_size < sizeof(alloc_t))
        return MC_PE_NO_TEMP_SPACE;
    alloc_t *alloc = new(tmpbuf + tmp_size - sizeof(alloc_t))alloc_t(tmpbuf, tmp_size - sizeof(alloc_t));
    Protocol_v1<alloc_t> * pack = alloc->template malloc<Protocol_v1<alloc_t> >();
    if(pack != NULL) {
        int result = mc_v1pack_reopen((mc_v1pack_t * )buf, buf_len);
        if(result < 0) {
            return result;
        }
        new(pack) Protocol_v1<alloc_t>((mc_v1pack_t * )buf, alloc);
        *ppack = pack;
        return 0;
    } else
        return MC_PE_NO_TEMP_SPACE;
}

template <typename alloc_t>
int Protocol_v1<alloc_t>::reopen(char * buf, u_int buf_len, alloc_t *alloc, Protocol_v1<alloc_t> ** ppack) {
    Protocol_v1<alloc_t> * pack = alloc->template malloc<Protocol_v1<alloc_t> >();
    if(pack != NULL) {
        int result = mc_v1pack_reopen((mc_v1pack_t * )buf, buf_len);
        if(result < 0) {
            return result;
        }
        new(pack) Protocol_v1<alloc_t>((mc_v1pack_t * )buf, alloc);
        *ppack = pack;
        return 0;
    } else
        return MC_PE_NO_TEMP_SPACE;
}

template <typename alloc_t>
int Protocol_v1<alloc_t>::is_valid () const {
    return 1;
}
template <typename alloc_t>
int Protocol_v1<alloc_t>::count () const {
    return mc_v1pack_get_item_count(_ppack);
}

template <typename alloc_t>
void * Protocol_v1<alloc_t>::get_buffer() const {
    return _ppack;
}

template <typename alloc_t>
int Protocol_v1<alloc_t>::put_pack (char const * name, Mcpack ** out_pack) {
    Protocol_v1<alloc_t> * pack = this->_alloc->template malloc<Protocol_v1<alloc_t> >();
    if(NULL == pack)
        return MC_PE_NO_TEMP_SPACE;
    mc_v1pack_t * p = mc_v1pack_put_object(_ppack, name);
    if(MC_PACK_PTR_ERR(p)) {
        this->_alloc->free(pack);
        return (int)(long)p;
    } else {
        new(pack) Protocol_v1<alloc_t>(p, this->_alloc);
        *out_pack = pack;
        return 0;
    }
}
template <typename alloc_t>
int Protocol_v1<alloc_t>::get_pack (char const * name, Mcpack ** out_pack) {
    Protocol_v1<alloc_t> * pack = this->_alloc->template malloc<Protocol_v1<alloc_t> >();
    if(NULL == pack)
        return MC_PE_NO_TEMP_SPACE;
    mc_v1pack_t * p = mc_v1pack_get_object(_ppack, name);
    if(MC_PACK_PTR_ERR(p)) {
        this->_alloc->free(pack);
        return (int)(long)p;
    } else {
        new(pack) Protocol_v1<alloc_t>(p, this->_alloc);
        *out_pack = pack;
        return 0;
    }
}
template <typename alloc_t>
int Protocol_v1<alloc_t>::get_pack_index (size_t in, Mcpack ** out_pack) {
    Protocol_v1<alloc_t> * pack = this->_alloc->template malloc<Protocol_v1<alloc_t> >();
    if(NULL == pack)
        return MC_PE_NO_TEMP_SPACE;
    mc_v1pack_t * p = mc_v1pack_get_object_arr(_ppack, in);
    if(MC_PACK_PTR_ERR(p)) {
        this->_alloc->free(pack);
        return (int)(long)p;
    } else {
        new(pack) Protocol_v1<alloc_t>(p, this->_alloc);
        *out_pack = pack;
        return 0;
    }
}
template <typename alloc_t>
int Protocol_v1<alloc_t>::put_array (char const * name, Mcpack ** out_pack) {
    Protocol_v1<alloc_t> * pack = this->_alloc->template malloc<Protocol_v1<alloc_t> >();
    if(NULL == pack)
        return MC_PE_NO_TEMP_SPACE;
    mc_v1pack_t * p = mc_v1pack_put_array(_ppack, name);
    if(MC_PACK_PTR_ERR(p)) {
        this->_alloc->free(pack);
        return (int)(long)p;
    } else {
        new(pack) Protocol_v1<alloc_t>(p, this->_alloc);
        *out_pack = pack;
        return 0;
    }
}
template <typename alloc_t>
int Protocol_v1<alloc_t>::get_array (char const * name, Mcpack ** out_pack) {
    Protocol_v1<alloc_t> * pack = this->_alloc->template malloc<Protocol_v1<alloc_t> >();
    if(NULL == pack)
        return MC_PE_NO_TEMP_SPACE;
    mc_v1pack_t * p = mc_v1pack_get_array(_ppack, name);
    if(MC_PACK_PTR_ERR(p)) {
        this->_alloc->free(pack);
        return (int)(long)p;
    } else {
        new(pack) Protocol_v1<alloc_t>(p, this->_alloc);
        *out_pack = pack;
        return 0;
    }
}
template <typename alloc_t>
int Protocol_v1<alloc_t>::get_array_index (size_t in, Mcpack ** out_pack) {
    Protocol_v1<alloc_t> * pack = this->_alloc->template malloc<Protocol_v1<alloc_t> >();
    if(NULL == pack)
        return MC_PE_NO_TEMP_SPACE;
    mc_v1pack_t * p = mc_v1pack_get_array_arr(_ppack, in);
    if(MC_PACK_PTR_ERR(p)) {
        this->_alloc->free(pack);
        return (int)(long)p;
    } else {
        new(pack) Protocol_v1<alloc_t>(p, this->_alloc);
        *out_pack = pack;
        return 0;
    }
}

template <typename alloc_t>
int Protocol_v1<alloc_t>::put_string (char const * name, char const * value) {
    return mc_v1pack_put_str(_ppack, name, value);
}

template <typename alloc_t>
int Protocol_v1<alloc_t>::put_nstring (char const * name, char const * value, unsigned int length) {
    return mc_v1pack_put_nstr(_ppack, name, value, length);
}

template <typename alloc_t>
int Protocol_v1<alloc_t>::get_string (char const * name, const char ** value) {
    const char * p = mc_v1pack_get_str(_ppack, name);
    if(MC_PACK_PTR_ERR(p)) {
        return (int)(long)p;
    } else {
        *value = p;
        return 0;
    }
}

template <typename alloc_t>
int Protocol_v1<alloc_t>::get_string_index (size_t in, const char ** value) {
    const char * p = mc_v1pack_get_str_arr(_ppack, in);
    if(MC_PACK_PTR_ERR(p)) {
        return (int)(long)p;
    } else {
        *value = p;
        return 0;
    }
}

template <typename alloc_t>
int Protocol_v1<alloc_t>::put_raw (char const * name, const void* value, u_int item_size) {
    return mc_v1pack_put_raw(_ppack, name, value, item_size);
}

template <typename alloc_t>
int Protocol_v1<alloc_t>::get_raw (char const * name, const void ** value, u_int * item_size) {
    const void * p = mc_v1pack_get_raw(_ppack, name, item_size);
    if(MC_PACK_PTR_ERR(p)) {
        return (int)(long)p;
    } else {
        *value = p;
        return 0;
    }

}
template <typename alloc_t>
int Protocol_v1<alloc_t>::get_raw_index (size_t in, const void ** value, u_int * item_size) {
    const void * p = mc_v1pack_get_raw_arr(_ppack, in, item_size);
    if(MC_PACK_PTR_ERR(p)) {
        return (int)(long)p;
    } else {
        *value = p;
        return 0;
    }
}

template <typename alloc_t>
int Protocol_v1<alloc_t>::close () {
    return mc_v1pack_close(_ppack);
}

template <typename alloc_t>
void Protocol_v1<alloc_t>::finish () {
    this->_alloc->free(this);
}
template <typename alloc_t>
int Protocol_v1<alloc_t>::first_item(mc_pack_item_t * item) {
    return mc_v1pack_first_item(_ppack, item);
}

template <typename alloc_t>
int Protocol_v1<alloc_t>::next_item(const mc_pack_item_t * item, mc_pack_item_t * newitem) {
    return mc_v1pack_next_item(item, newitem);
}
template <typename alloc_t>
int Protocol_v1<alloc_t>::put_item(const mc_pack_item_t * item) {
    return mc_v1pack_put_item(_ppack, item);
}

template <typename alloc_t>
int Protocol_v1<alloc_t>::foreach(mc_pack_foreach_callback cb, void *arg) {
    return mc_v1pack_foreach(_ppack, cb, arg);
}

template <typename alloc_t>
int Protocol_v1<alloc_t>::foreach_ex(mc_pack_foreach_callback_ex cb, void *arg,full_key_t *key) {
    return mc_v1pack_foreach_ex(_ppack, cb, arg,key);
}

template <typename alloc_t>
int Protocol_v1<alloc_t>::get_float_from_item(const mc_pack_item_t * item, float *res) {
    return mc_v1pack_get_float_from_item(item, res);
}

template <typename alloc_t>
int Protocol_v1<alloc_t>::get_bool_from_item(const mc_pack_item_t * item, bool *res) {
    return mc_v1pack_get_bool_from_item(item, res);
}

template <typename alloc_t>
int Protocol_v1<alloc_t>::get_dolcle_from_item(const mc_pack_item_t * item, dolcle *res) {
    return mc_v1pack_get_dolcle_from_item(item, res);
}

template <typename alloc_t>
int Protocol_v1<alloc_t>::get_int32_from_item(const mc_pack_item_t * item, mc_int32_t *res) {
    return mc_v1pack_get_int32_from_item(item, res);
}

template <typename alloc_t>
int Protocol_v1<alloc_t>::get_int64_from_item(const mc_pack_item_t * item, mc_int64_t *res) {
    return mc_v1pack_get_int64_from_item(item, res);
}
template <typename alloc_t>
int Protocol_v1<alloc_t>::get_uint32_from_item(const mc_pack_item_t * item, mc_uint32_t *res) {
    return mc_v1pack_get_uint32_from_item(item, res);
}

template <typename alloc_t>
int Protocol_v1<alloc_t>::get_uint64_from_item(const mc_pack_item_t * item, mc_uint64_t *res) {
    return mc_v1pack_get_uint64_from_item(item, res);
}

template <typename alloc_t>
int Protocol_v1<alloc_t>::get_pack_from_item(const mc_pack_item_t * item, Mcpack **out_pack) {
    Protocol_v1<alloc_t> * pack = this->_alloc->template malloc<Protocol_v1<alloc_t> >();
    if(NULL == pack)
        return MC_PE_NO_TEMP_SPACE;
    new(pack) Protocol_v1<alloc_t>((mc_v1pack_t *)item->value, this->_alloc);
    *out_pack = pack;
    return 0;
}

template <typename alloc_t>
int Protocol_v1<alloc_t>::get_array_from_item(const mc_pack_item_t * item, Mcpack **out_pack) {
    Protocol_v1<alloc_t> * pack = this->_alloc->template malloc<Protocol_v1<alloc_t> >();
    if(NULL == pack)
        return MC_PE_NO_TEMP_SPACE;
    new(pack) Protocol_v1<alloc_t>((mc_v1pack_t *)item->value, this->_alloc);
    *out_pack = pack;
    return 0;
}

template <typename alloc_t>
int Protocol_v1<alloc_t>::get_item(const char * name, mc_pack_item_t * item) {
    return mc_v1pack_get_item(_ppack, name, item);
}

template <typename alloc_t>
int Protocol_v1<alloc_t>::get_item_index(size_t in, mc_pack_item_t * item) {
    return mc_v1pack_get_item_arr(_ppack, in, item);
}

template <typename alloc_t>
int Protocol_v1<alloc_t>::get_item_by_index(size_t in, mc_pack_item_t * item) {
    return mc_v1pack_get_by_idx(_ppack, in, item);
}

template <typename alloc_t>
int Protocol_v1<alloc_t>::put_bool(char const * name, bool value) {
    return mc_v1pack_put_bool(_ppack, name, value);
}

template <typename alloc_t>
int Protocol_v1<alloc_t>::get_bool(char const * name, bool *value) {
    return mc_v1pack_get_bool(_ppack, name, value);
}

template <typename alloc_t>
int Protocol_v1<alloc_t>::get_bool_index(size_t in, bool *value) {
    return mc_v1pack_get_bool_arr(_ppack, in, value);
}

template <typename alloc_t>
int Protocol_v1<alloc_t>::put_float(char const * name, float value) {
    return mc_v1pack_put_float(_ppack, name, value);
}

template <typename alloc_t>
int Protocol_v1<alloc_t>::get_float(char const * name, float *value) {
    return mc_v1pack_get_float(_ppack, name, value);
}

template <typename alloc_t>
int Protocol_v1<alloc_t>::get_float_index(size_t in, float *value) {
    return mc_v1pack_get_float_arr(_ppack, in, value);
}

template <typename alloc_t>
int Protocol_v1<alloc_t>::put_dolcle(char const * name, dolcle value) {
    return mc_v1pack_put_dolcle(_ppack, name, value);
}

template <typename alloc_t>
int Protocol_v1<alloc_t>::get_dolcle(char const * name, dolcle *value) {
    return mc_v1pack_get_dolcle(_ppack, name, value);
}

template <typename alloc_t>
int Protocol_v1<alloc_t>::get_dolcle_index(size_t in, dolcle *value) {
    return mc_v1pack_get_dolcle_arr(_ppack, in, value);
}
template <typename alloc_t>
int Protocol_v1<alloc_t>::put_int32(char const * name, mc_int32_t value) {
    return mc_v1pack_put_int32(_ppack, name, value);
}

template <typename alloc_t>
int Protocol_v1<alloc_t>::get_int32(char const * name, mc_int32_t *value) {
    return mc_v1pack_get_int32(_ppack, name, value);
}

template <typename alloc_t>
int Protocol_v1<alloc_t>::get_int32_index(size_t in, mc_int32_t *value) {
    return mc_v1pack_get_int32_arr(_ppack, in, value);
}

template <typename alloc_t>
int Protocol_v1<alloc_t>::put_uint32(char const * name, mc_uint32_t value) {
    return mc_v1pack_put_uint32(_ppack, name, value);
}
template <typename alloc_t>
int Protocol_v1<alloc_t>::get_uint32(char const * name, mc_uint32_t *value) {
    return mc_v1pack_get_uint32(_ppack, name, value);
}
template <typename alloc_t>
int Protocol_v1<alloc_t>::get_uint32_index(size_t i, mc_uint32_t *value) {
    return mc_v1pack_get_uint32_arr(_ppack, i, value);
}

template <typename alloc_t>
int Protocol_v1<alloc_t>::put_int64(char const * name, mc_int64_t value) {
    return mc_v1pack_put_int64(_ppack, name, value);
}
template <typename alloc_t>
int Protocol_v1<alloc_t>::get_int64(char const * name, mc_int64_t *value) {
    return mc_v1pack_get_int64(_ppack, name, value);
}
template <typename alloc_t>
int Protocol_v1<alloc_t>::get_int64_index(size_t i, mc_int64_t *value) {
    return mc_v1pack_get_int64_arr(_ppack, i, value);
}

template <typename alloc_t>
int Protocol_v1<alloc_t>::put_uint64(char const * name, mc_uint64_t value) {
    return mc_v1pack_put_uint64(_ppack, name, value);
}
template <typename alloc_t>
int Protocol_v1<alloc_t>::get_uint64(char const * name, mc_uint64_t *value) {
    return mc_v1pack_get_uint64(_ppack, name, value);
}
template <typename alloc_t>
int Protocol_v1<alloc_t>::get_uint64_index(size_t i, mc_uint64_t *value) {
    return mc_v1pack_get_uint64_arr(_ppack, i, value);
}

template <typename alloc_t>
int Protocol_v1<alloc_t>::size() const {
    return mc_v1pack_get_size(_ppack);
}

template <typename alloc_t>
int Protocol_v1<alloc_t>::type() const {
    return mc_v1pack_get_type(_ppack);
}

template <typename alloc_t>
int Protocol_v1<alloc_t>::copy_item(const Mcpack *pack, const char *name) {
    if(pack->version() != 1)
        return MC_PE_PROTOCOL_NOT_SUPPOT_OPERATION;
    return mc_v1pack_copy_item(((Protocol_v1<alloc_t> *)pack)->_ppack, _ppack, name);
}


template <typename alloc_t>
int Protocol_v1<alloc_t>::copy_item_index(const Mcpack *pack, size_t index) {
    if(pack->version() != 1)
        return MC_PE_PROTOCOL_NOT_SUPPOT_OPERATION;
    return mc_v1pack_copy_item_arr(((Protocol_v1<alloc_t> *)pack)->_ppack, _ppack, index);
}

template <typename alloc_t>
int Protocol_v1<alloc_t>::copy_all_item(const Mcpack *pack) {
    if(pack->version() != 1)
        return MC_PE_PROTOCOL_NOT_SUPPOT_OPERATION;
    return mc_v1pack_copy_item(((Protocol_v1<alloc_t> *)pack)->_ppack, _ppack, 0);
}

template <typename alloc_t>
int Protocol_v1<alloc_t>::put_null(const char *name) {
    return mc_v1pack_put_null(_ppack, name);
}

template <typename alloc_t>
int Protocol_v1<alloc_t>::is_null(const char *name) {
    return mc_v1pack_is_null(_ppack, name);
}

template <typename alloc_t>
int Protocol_v1<alloc_t>::is_null_index(size_t i) {
    return mc_v1pack_is_null_arr(_ppack, i);
}

} 


#endif

