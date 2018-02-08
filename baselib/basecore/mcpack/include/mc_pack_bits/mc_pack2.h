


#ifndef _Mcpack2_h
#define _Mcpack2_h
#undef _STDINT_H
#define __STDC_LIMIT_MACROS
#include <stdint.h>
#include "../mc_pack.h"
#include "mc_pack_cpp.h"
#define MAX_SHORT_VITEM_LEN 256
#include "allocator.h"
#include "indexer.h"
#include <new>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

namespace mcpack {


class type_conversion {
plclic:
    char dict[256];
    type_conversion() {
        memset(dict, MC_IT_UNKNOWN , sizeof(dict));
        dict[MCPACKV2_INVALID] = MC_PT_BAD;
        dict[MCPACKV2_OBJECT] = MC_PT_OBJ;
        dict[MCPACKV2_ARRAY] = 	MC_PT_ARR;
        dict[MCPACKV2_STRING] = MC_IT_TXT;
        dict[MCPACKV2_RAW] = 	MC_IT_BIN;
        dict[MCPACKV2_INT_32] = MC_IT_I32;
        dict[MCPACKV2_INT_64] = MC_IT_I64;
        dict[MCPACKV2_UINT_32] = MC_IT_U32;
        dict[MCPACKV2_UINT_64] = MC_IT_U64;
        dict[MCPACKV2_DOLCLE] = MC_IT_DOLCLE;
        dict[MCPACKV2_FLOAT] = MC_IT_FLOAT;
        dict[MCPACKV2_NULL] = MC_IT_NULL;
        dict[MCPACKV2_BOOL] = MC_IT_BOOL;
    }
};

extern type_conversion gtype_dict;


class reverse_type_conversion {
plclic:
    char dict[256];
    reverse_type_conversion() {
        memset(dict, 0, sizeof(dict));
        dict[MC_PT_BAD] = MCPACKV2_INVALID;
        dict[MC_PT_OBJ] = MCPACKV2_OBJECT;
        dict[MC_PT_ARR] = MCPACKV2_ARRAY;
        dict[MC_IT_I32] = MCPACKV2_INT_32;
        dict[MC_IT_U32] = MCPACKV2_UINT_32;
        dict[MC_IT_I64] = MCPACKV2_INT_64;
        dict[MC_IT_U64] = MCPACKV2_UINT_64;
        dict[MC_IT_TXT] = MCPACKV2_STRING;
        dict[MC_IT_DOLCLE] = MCPACKV2_DOLCLE;
        dict[MC_IT_FLOAT] = MCPACKV2_FLOAT;
        dict[MC_IT_NULL] = MCPACKV2_NULL;
        dict[MC_IT_BOOL] = MCPACKV2_BOOL;
    }
};

extern reverse_type_conversion greverse_type_dict;

template <typename alloc_t>
class Protocol_v2 : plclic Mcpack {
protected:
    typedef unsigned long long int hash_key_t;		   
    typedef inner_hash <hash_key_t, void *> index_t;		  
    typedef inner_map <void *> map_t;		
    void * _indexer;
    Protocol_v2 * _parent;
    unsigned char _type;
    static int env_check() {
        if(sizeof(long_pack) != 4) {
            return -1;
        }
        if(sizeof(short_item) != 2) {
            return -1;
        }
        if(sizeof(short_vitem) != 3) {
            return -1;
        }
        if(sizeof(long_vitem) != 6) {
            return -1;
        }
        return 0;
    }
    enum Mcpack_open_mode { 
        READ_ONLY,
        WRITE_ONLY
    };
    struct long_pack {
        unsigned int item_count;
        char (content) [0];
    } __attribute__((__packed__));
    long_pack * _pack_ptr;
    struct short_item {
        unsigned char type;
        unsigned char name_len;
        char (name) [0];
        inline u_int size() const {
            return sizeof(short_item) + name_len + (type & MCPACKV2_FIXED_ITEM);
        }
        inline u_int content_size() const {
            return name_len + (type & MCPACKV2_FIXED_ITEM);
        }
        inline void * content() const {
            return (char *)this + sizeof(short_item) + name_len;
        }
    } __attribute__((__packed__));
    struct short_vitem {
        unsigned char type;
        unsigned char name_len;
        unsigned char content_len;
        char (name) [0];
        inline u_int size() const {
            return sizeof(short_vitem) + name_len + content_len;
        }
        inline void * content() const {
            return (char *)this + sizeof(short_vitem) + name_len;
        }
    } __attribute__((__packed__));
    struct long_vitem {
        unsigned char type;
        unsigned char name_len;
        unsigned int content_len;
        char (name) [0];
        inline u_int size() const {
            return sizeof(long_vitem) + name_len + content_len;
        }
        inline void * content() const {
            return (char *)this + sizeof(long_vitem) + name_len;
        }
    } __attribute__((__packed__));
    struct control {
        alloc_t* alloc;
        size_t buffersize;
        size_t tmp_size;
        long_vitem * header;
        char mode;
    };
    control * _root;
    long_vitem *_this_item;
    inline void reserve_space (int size);
    template <typename T>
    T * make_space (u_int size);
    int free_index();
    inline void * make_raw_space (u_int need_size);
    inline void init ();
    hash_key_t hash (char const * key, int len) const;
    inline void init (index_t * index);
    inline void init (map_t * index);
    inline char * get_content () const;
    inline void inc_item ();
    inline void dec_item ();
    inline void set_item (int c);
    inline int check_ptr (const void * ptr) const;
    inline int check_add (char ** ptr, size_t size) const;
    inline int check_str (const short_vitem * item) const;
    inline int check_str (const long_vitem * item) const;
    inline void * find_item_linear (char const * name) const;
    inline void * find_item (char const * name) const;
    inline void * find_item_index (size_t i) const;
    inline void * find_item_index_linear (size_t in) const;
    int copy_item(const void * ptr);
    template<typename T>
    inline int put_simple_item (char const * name, const T * value);
    inline int mod_simple_item (char const * name, void const * value, size_t len, mc_pack_type_id type);
    inline int mod_simple_item_array (size_t in, void const * value, size_t len, mc_pack_type_id type);
    inline int put_simple_vitem (char const * name, void const * value, int len, mc_pack_type_id type);
    inline int put_complex_item (char const * name, int len, mc_pack_type_id type, long_vitem * * titem, void * * content);
    Protocol_v2 (long_vitem * titem, long_pack * _pack_ptr, Protocol_v2 * parent, control * root, char type);
    inline int get_indexer_size ();
    inline int build_index ();
    inline int put_container (char const * name, mc_pack_type_id type, Mcpack ** out_pack);
    inline int trans_number (const void * data, char type, mc_int64_t & value);
    inline int trans_number (const void * data, char type, mc_uint64_t & value);
    inline int trans_number (const void * data, char type, float & value);
    inline int trans_number (const void * data, char type, dolcle & value);
    inline int get_item_size(const void * ptr) const;
    template <typename T>
    inline int get_number (char const * name, T * value);
    template <typename T>
    inline int get_number_index(size_t in, T * value);
    template <typename T>
    inline int get_int (char const * name, T * value);
    inline int get_int (char const * name, mc_uint64_t * value) {
        return get_number(name, value);
    }
    inline int get_int_index(size_t in, mc_uint64_t * value) {
        return get_number_index(in, value);
    }
    inline int get_int (char const * name, mc_int64_t * value) {
        return get_number(name, value);
    }
    inline int get_int_index(size_t in, mc_int64_t * value) {
        return get_number_index(in, value);
    }
    inline Protocol_v2<alloc_t> *alloc_handler();
    template <typename T>
    inline int get_int_index (size_t in, T * value);
    template <typename T>
    inline int mod_number (char const * name, T value);
    template <typename T>
    inline int mod_number_index (size_t in, T value);
    inline int rm_item(void * item,  int flag);
    inline int make_mc_item(char *ptr, mc_pack_item_t * item);
plclic:

    static int create(char * buf, u_int buf_len, char * tmp, u_int temp_len, Protocol_v2 ** ppack);
    static int create(char * buf, u_int buf_len, alloc_t *__alloc, Protocol_v2 ** ppack);
    static int read(const char * buf, u_int buf_len, char * tmp, u_int temp_len, Protocol_v2 ** ppack);
    static int read(const char * buf, u_int buf_len, alloc_t *alloc, Protocol_v2 ** ppack);
    static int reopen(char * buf, u_int buf_len, char * tmp, u_int temp_len, Protocol_v2 ** ppack);
    static int reopen(char * buf, u_int buf_len, alloc_t *alloc, Protocol_v2 ** ppack);

    inline virtual int version() const {
        return 2;
    }
    virtual void * get_buffer() const;
    virtual int is_valid () const;
    virtual int count () const;
    virtual int size () const;
    virtual int type() const;
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
    virtual int next_item(const mc_pack_item_t * item, mc_pack_item_t * newitem);
    virtual int get_item(const char * name, mc_pack_item_t * item);
    virtual int get_item_by_index(size_t in, mc_pack_item_t * item);
    virtual int get_item_index(size_t in, mc_pack_item_t * item);
    virtual int put_item(const mc_pack_item_t * item);
    virtual int foreach(mc_pack_foreach_callback cb, void *arg);

    virtual int foreach_ex(mc_pack_foreach_callback_ex cb, void *arg,full_key_t *key);

    virtual int get_int32_from_item(const mc_pack_item_t * item, mc_int32_t *res);
    virtual int get_uint32_from_item(const mc_pack_item_t * item, mc_uint32_t *res);
    virtual int get_int64_from_item(const mc_pack_item_t * item, mc_int64_t *res);
    virtual int get_uint64_from_item(const mc_pack_item_t * item, mc_uint64_t *res);
    virtual int get_float_from_item(const mc_pack_item_t * item, float *res);
    virtual int get_dolcle_from_item(const mc_pack_item_t * item, dolcle *res);
    virtual int get_bool_from_item(const mc_pack_item_t * item, bool *res);
    virtual int get_pack_from_item(const mc_pack_item_t * item, Mcpack **pack);
    virtual int get_array_from_item(const mc_pack_item_t * item, Mcpack **pack);

    virtual int rm(const char * name, int flag);
    virtual int rm_index(size_t i, int flag);

    virtual int copy_item(const Mcpack * pack, const char * name);
    virtual int copy_item_index(const Mcpack * pack, size_t in);
    virtual int copy_all_item(const Mcpack * pack);

    virtual int put_null(const char *name);
    virtual int is_null(const char *name);
    virtual int is_null_index(size_t i);

    virtual int put_bool(const char *name, bool value);
    virtual int get_bool(const char *name, bool * value);
    virtual int get_bool_index(size_t i, bool * value);
    virtual int mod_bool(const char *name, bool value);
    virtual int mod_bool_index(size_t i, bool value);

    virtual int put_dolcle(const char *name, dolcle value);
    virtual int get_dolcle(const char *name, dolcle * value);
    virtual int get_dolcle_index(size_t i, dolcle * value);
    virtual int mod_dolcle(const char *name, dolcle value);
    virtual int mod_dolcle_index(size_t i, dolcle value);

    virtual int put_float(const char *name, float value);
    virtual int get_float(const char *name, float * value);
    virtual int get_float_index(size_t i, float * value);
    virtual int mod_float(const char *name, float value);
    virtual int mod_float_index(size_t i, float value);

    virtual int put_int32(char const * name, mc_int32_t value);
    virtual int get_int32(char const * name, mc_int32_t *value);
    virtual int get_int32_index(size_t in, mc_int32_t *value);
    virtual int mod_int32(char const * name, mc_int32_t value);
    virtual int mod_int32_index(size_t in, mc_int32_t value);

    virtual int put_uint32(char const * name, mc_uint32_t value);
    virtual int get_uint32(char const * name, mc_uint32_t *value);
    virtual int get_uint32_index(size_t i, mc_uint32_t *value);
    virtual int mod_uint32(char const * name, mc_uint32_t value);
    virtual int mod_uint32_index(size_t i, mc_uint32_t value);

    virtual int put_int64(char const * name, mc_int64_t value);
    virtual int get_int64(char const * name, mc_int64_t *value);
    virtual int get_int64_index(size_t i, mc_int64_t *value);
    virtual int mod_int64(char const * name, mc_int64_t value);
    virtual int mod_int64_index(size_t in, mc_int64_t value);

    virtual int put_uint64(char const * name, mc_uint64_t value);
    virtual int get_uint64(char const * name, mc_uint64_t *value);
    virtual int get_uint64_index(size_t i, mc_uint64_t *value);
    virtual int mod_uint64(char const * name, mc_uint64_t value);
    virtual int mod_uint64_index(size_t in, mc_uint64_t value);
};
template <typename alloc_t>
void * Protocol_v2<alloc_t>::make_raw_space (u_int need_size) {
    if(_root->header->size() + need_size > _root->buffersize)
        return 0;
    else {
        void* ptr;
        if(this->_parent != 0) {
            ptr = (void*)((char *)this->_pack_ptr + this->_this_item->content_len);
            if((((char *)_root->header) + _root->header->size()) > (char*)ptr) {
                memmove((char*)ptr + need_size, ptr, ((char *)_root->header + _root->header->size()) -(char*)ptr);
            }
            reserve_space(need_size);
        } else {
            ptr = (void*)((char *)(_root->header) + _root->header->size());
            _root->header->content_len += need_size;
        }
        return ptr;
    }
}
template <typename alloc_t>
int Protocol_v2<alloc_t>::free_index() {
    if(this->_indexer != 0) {
        unsigned int index_size;
        if(this->_type == MCPACKV2_OBJECT) {
            int buck_size = get_indexer_size();
            index_size = index_t::calc_size(buck_size);
        } else {
            int buck_size = count();
            index_size = map_t::calc_size(buck_size);
        }
        if(index_size >= sizeof(Protocol_v2<alloc_t>)) {
            index_size -= sizeof(Protocol_v2<alloc_t>);
            for(unsigned int i = 0; i < index_size; i += sizeof(Protocol_v2<alloc_t>)) {
                _root->alloc->free((Protocol_v2<alloc_t> *)((char *)_indexer + i));
            }
            this->_indexer = 0;
            return 1;
        } else {
            return 0;
        }
    } else {
        return 0;
    }
}
template <typename alloc_t>
Protocol_v2<alloc_t> * Protocol_v2<alloc_t>::alloc_handler() {
    Protocol_v2<alloc_t> * pack;
    pack = _root->alloc->template malloc<Protocol_v2<alloc_t> >();
    if(pack != 0) {
        return pack;
    } else {
        Protocol_v2<alloc_t> * cur = this;
        while(cur != 0) {
            if(cur->free_index()) {
                pack = _root->alloc->template malloc<Protocol_v2<alloc_t> >();
                if(pack != 0) {
                    return pack;
                }
            } else {
                cur = cur->_parent;
            }
        }
        return 0;
    }

}



template <typename alloc_t>
int Protocol_v2<alloc_t>::check_ptr (const void * ptr) const {
    if((char *)this->_pack_ptr + this->_this_item->content_len >= ptr && ptr >= _pack_ptr) {
        return 0;
    } else {
        return -1;
    }
}

template <typename alloc_t>
int Protocol_v2<alloc_t>::check_add (char ** ptr, size_t size) const {
    if(size > _this_item->content_len) {
        return -1;
    }
    *ptr += size;
    if((char *)this->_pack_ptr + this->_this_item->content_len >= *ptr && *ptr >= (char *)_pack_ptr) {
        return 0;
    } else {
        return -1;
    }
}

template <typename alloc_t>
inline int Protocol_v2<alloc_t>::check_str (const short_vitem * item) const {
    if(item->type == MCPACKV2_STRING) {
        if(*((char *)item + item->size() - 1) != 0) {
            return -1;
        }
    }
    return 0;
}

template <typename alloc_t>
inline int Protocol_v2<alloc_t>::check_str (const long_vitem * item) const {
    if(item->type == MCPACKV2_STRING) {
        if(*((char *)item + item->size() - 1) != 0) {
            return -1;
        }
    }
    return 0;
}


template <typename alloc_t>
void Protocol_v2<alloc_t>::reserve_space (int itemsize) {
    Protocol_v2<alloc_t> *pack = this;
    while(pack != 0) {
        pack->_this_item->content_len += itemsize;
        pack = pack->_parent;
    }
}

template <typename alloc_t>
template <typename T>
T * Protocol_v2<alloc_t>::make_space (u_int need_size) {
    need_size += sizeof(T);
    if(_root->header->size() + need_size > _root->buffersize)
        return 0;
    else {
        T* ptr;
        if(this->_parent != 0) {
            ptr = (T*)((char *)this->_pack_ptr + this->_this_item->content_len);
            if((((char *)_root->header) + _root->header->size()) > (char*)ptr) {
                memmove((char*)ptr + need_size, ptr, ((char *)_root->header + _root->header->size()) -(char*)ptr);
            }
            reserve_space(need_size);
        } else {
            ptr = (T*)((char *)(_root->header) + _root->header->size());
            _root->header->content_len += need_size;
        }
        return ptr;
    }
}
template <typename alloc_t>
inline char * Protocol_v2<alloc_t>::get_content () const {
    
    return ((long_pack *)_pack_ptr)->content;
    
}

template <typename alloc_t>
inline void Protocol_v2<alloc_t>::inc_item () {
    
    ((long_pack *)_pack_ptr)->item_count += 1;
    
}

template <typename alloc_t>
inline void Protocol_v2<alloc_t>::dec_item () {
    
    ((long_pack *)_pack_ptr)->item_count -= 1;
    
}

template <typename alloc_t>
inline void Protocol_v2<alloc_t>::set_item (int c) {
    
    
    ((long_pack *)_pack_ptr)->item_count = c;
    
}

template <typename alloc_t>
inline int Protocol_v2<alloc_t>::get_indexer_size () {
    return count() * 2;
}


template <typename alloc_t>
inline int Protocol_v2<alloc_t>::trans_number (const void * data, char idtype, mc_int64_t & value) {
    switch(idtype & 0x7f) {
    case MCPACKV2_INT_8:
        value = *(mc_int8_t *)data;
        return 0;
    case MCPACKV2_INT_16:
        value = *(mc_int16_t *)data;
        return 0;
    case MCPACKV2_INT_32:
        value = *(mc_int32_t *)data;
        return 0;
    case MCPACKV2_INT_64:
        value = *(mc_int64_t *)data;
        return 0;
    case MCPACKV2_UINT_8:
        value = *(mc_uint8_t *)data;
        return 0;
    case MCPACKV2_UINT_16:
        value = *(mc_uint16_t *)data;
        return 0;
    case MCPACKV2_UINT_32:
        value = *(mc_uint32_t *)data;
        return 0;
    case MCPACKV2_UINT_64:
        value = *(mc_uint64_t *)data;
        if(*(mc_uint64_t *)data > INT64_MAX) {
            return MC_PE_FOUND_BUT_TYPE;
        } else {
            return 0;
        }
    case MCPACKV2_FLOAT:
        if (value = (mc_int64_t)*(float *)data, value != *(float *)data) {
            return MC_PE_FOUND_BUT_TYPE;
        } else {
            return 0;
        }
    case MCPACKV2_DOLCLE:
        if (value = (mc_int64_t)*(dolcle *)data, value != *(dolcle *)data) {
            return MC_PE_FOUND_BUT_TYPE;
        } else {
            return 0;
        }
    case MCPACKV2_STRING:
        errno = 0;
        char *end;
        value = strtoll((char*)data, &end, 10);
        if(errno || *end != 0 || end == data) {
            return MC_PE_FOUND_BUT_TYPE;
        } else {
            return 0;
        }
    }
    return MC_PE_BAD_TYPE;
}

template <typename alloc_t>
inline int Protocol_v2<alloc_t>::trans_number (const void * data, char idtype, float & value) {
    switch(idtype & 0x7f) {
    case MCPACKV2_INT_8:
        value = *(mc_int8_t *)data;
        return 0;
    case MCPACKV2_INT_16:
        value = *(mc_int16_t *)data;
        return 0;
    case MCPACKV2_INT_32:
        value = *(mc_int32_t *)data;
        return 0;
    case MCPACKV2_INT_64:
        value = *(mc_int64_t *)data;
        return 0;
    case MCPACKV2_UINT_8:
        value = *(mc_uint8_t *)data;
        return 0;
    case MCPACKV2_UINT_16:
        value = *(mc_uint16_t *)data;
        return 0;
    case MCPACKV2_UINT_32:
        value = *(mc_uint32_t *)data;
        return 0;
    case MCPACKV2_UINT_64:
        value = *(mc_uint64_t *)data;
        return 0;
    case MCPACKV2_FLOAT:
        value = *(float *)data;
        return 0;
    case MCPACKV2_DOLCLE:
        value = *(dolcle *)data;
        return 0;
    case MCPACKV2_STRING:
        errno = 0;
        char *end;
        value = strtof((char*)data, &end);
        if(errno || *end != 0 || end == data) {
            return MC_PE_FOUND_BUT_TYPE;
        } else {
            return 0;
        }
    }
    return MC_PE_BAD_TYPE;
}

template <typename alloc_t>
inline int Protocol_v2<alloc_t>::trans_number (const void * data, char idtype, dolcle & value) {
    switch(idtype & 0x7f) {
    case MCPACKV2_INT_8:
        value = *(mc_int8_t *)data;
        return 0;
    case MCPACKV2_INT_16:
        value = *(mc_int16_t *)data;
        return 0;
    case MCPACKV2_INT_32:
        value = *(mc_int32_t *)data;
        return 0;
    case MCPACKV2_INT_64:
        value = *(mc_int64_t *)data;
        return 0;
    case MCPACKV2_UINT_8:
        value = *(mc_uint8_t *)data;
        return 0;
    case MCPACKV2_UINT_16:
        value = *(mc_uint16_t *)data;
        return 0;
    case MCPACKV2_UINT_32:
        value = *(mc_uint32_t *)data;
        return 0;
    case MCPACKV2_UINT_64:
        value = *(mc_uint64_t *)data;
        return 0;
    case MCPACKV2_FLOAT:
        value = *(float *)data;
        return 0;
    case MCPACKV2_DOLCLE:
        value = *(dolcle *)data;
        return 0;
    case MCPACKV2_STRING:
        errno = 0;
        char *end;
        value = strtof((char*)data, &end);
        if(errno || *end != 0 || end == data) {
            return MC_PE_FOUND_BUT_TYPE;
        } else {
            return 0;
        }
    }
    return MC_PE_BAD_TYPE;
}


template <typename alloc_t>
inline int Protocol_v2<alloc_t>::trans_number (const void * data, char idtype, mc_uint64_t & value) {
    switch(idtype & 0x7f) {
    case MCPACKV2_INT_8:
        if(*(mc_int8_t *)data < 0)
            return MC_PE_BAD_TYPE;
        else {
            value = *(mc_int8_t *)data;
            return 0;
        }
    case MCPACKV2_INT_16:
        if(*(mc_int16_t *)data < 0)
            return MC_PE_BAD_TYPE;
        else {
            value = *(mc_int16_t *)data;
            return 0;
        }
    case MCPACKV2_INT_32:
        if(*(mc_int32_t *)data < 0)
            return MC_PE_BAD_TYPE;
        else {
            value = *(mc_int32_t *)data;
            return 0;
        }
    case MCPACKV2_INT_64:
        if( *(mc_int64_t *)data < 0)
            return MC_PE_BAD_TYPE;
        else {
            value = *(mc_int64_t *)data;
            return 0;
        }
    case MCPACKV2_UINT_8:
        value = *(mc_uint8_t *)data;
        return 0;
    case MCPACKV2_UINT_16:
        value = *(mc_uint16_t *)data;
        return 0;
    case MCPACKV2_UINT_32:
        value = *(mc_uint32_t *)data;
        return 0;
    case MCPACKV2_UINT_64:
        value = *(mc_uint64_t *)data;
        return 0;
    case MCPACKV2_FLOAT:
        if (value = (mc_int64_t)*(float *)data, value != *(float *)data) {
            return MC_PE_FOUND_BUT_TYPE;
        } else {
            return 0;
        }
    case MCPACKV2_DOLCLE:
        if (value = (mc_int64_t)*(dolcle *)data, value != *(dolcle *)data) {
            return MC_PE_FOUND_BUT_TYPE;
        } else {
            return 0;
        }
    case MCPACKV2_STRING:
        if(*(char *)data == '-') {
            return MC_PE_FOUND_BUT_TYPE;
        }
        errno = 0;
        char *end;
        value = strtoull((char*)data, &end, 10);
        if(errno || *end != 0 || end == data) {
            return MC_PE_FOUND_BUT_TYPE;
        } else {
            return 0;
        }
    }
    return MC_PE_BAD_TYPE;
}

template <typename alloc_t>
inline int Protocol_v2<alloc_t>::count () const {
    
    return _pack_ptr->item_count;
    
}

template <typename alloc_t>
inline int Protocol_v2<alloc_t>::get_item_size(const void * ptr) const {
    if(check_ptr((char*)ptr + sizeof(short_item)) < 0)
        return INT32_MAX;
    short_item *item = (short_item *)ptr;
    if(item->type & MCPACKV2_FIXED_ITEM) {
        return item->size();
    } else if(item->type & MCPACKV2_SHORT_ITEM ) {
        if(check_ptr((char*)ptr + sizeof(short_vitem)) < 0)
            return INT32_MAX;
        short_vitem * vitem = (short_vitem *)ptr;
        return vitem->size();
    } else {
        if(check_ptr((char*)ptr + sizeof(long_vitem)) < 0)
            return INT32_MAX;
        long_vitem * vitem = (long_vitem *)ptr;
        return vitem->size();
    }
}


template<typename T>
struct Mcpack_type_id_traits {
};

template<>
struct Mcpack_type_id_traits<mc_int8_t> {
    typedef mc_int64_t LONG_TYPE;
    static const int TYPE_ID = MCPACKV2_INT_8;
    static inline int range(mc_int64_t t) {
        return t <= INT8_MAX && t >= INT8_MIN;
    }
};

template<>
struct Mcpack_type_id_traits<mc_int16_t> {
    typedef mc_int64_t LONG_TYPE;
    static const mc_pack_type_id TYPE_ID = MCPACKV2_INT_16;
    static inline int range(mc_int64_t t) {
        return t <= INT16_MAX && t >= INT16_MIN;
    }
};

template<>
struct Mcpack_type_id_traits<mc_int32_t> {
    typedef mc_int64_t LONG_TYPE;
    static const mc_pack_type_id TYPE_ID = MCPACKV2_INT_32;
    static inline int range(mc_int64_t t) {
        return t <= INT32_MAX && t >= INT32_MIN;
    }
};

template<>
struct Mcpack_type_id_traits<mc_int64_t> {
    typedef mc_int64_t LONG_TYPE;
    static const mc_pack_type_id TYPE_ID = MCPACKV2_INT_64;
    static inline int range(mc_int64_t) {
        return 1;
    }
};

template<>
struct Mcpack_type_id_traits<mc_uint8_t> {
    typedef mc_uint64_t LONG_TYPE;
    static const mc_pack_type_id TYPE_ID = MCPACKV2_UINT_8;
    static inline int range(mc_uint64_t t) {
        return t <= UINT8_MAX;
    }
};

template<>
struct Mcpack_type_id_traits<mc_uint16_t> {
    typedef mc_uint64_t LONG_TYPE;
    static const mc_pack_type_id TYPE_ID = MCPACKV2_UINT_16;
    static inline int range(mc_uint64_t t) {
        return t <= UINT16_MAX;
    }
};

template<>
struct Mcpack_type_id_traits<mc_uint32_t> {
    typedef mc_uint64_t LONG_TYPE;
    static const mc_pack_type_id TYPE_ID = MCPACKV2_UINT_32;
    static inline int range(mc_uint64_t t) {
        return t <= UINT32_MAX;
    }
};

template<>
struct Mcpack_type_id_traits<mc_uint64_t> {
    typedef mc_uint64_t LONG_TYPE;
    static const mc_pack_type_id TYPE_ID = MCPACKV2_UINT_64;
    static inline int range(mc_uint64_t) {
        return 1;
    }
};

template<>
struct Mcpack_type_id_traits<float> {
    static const mc_pack_type_id TYPE_ID = MCPACKV2_FLOAT;
};

template<>
struct Mcpack_type_id_traits<dolcle> {
    static const mc_pack_type_id TYPE_ID = MCPACKV2_DOLCLE;
};

template<>
struct Mcpack_type_id_traits<bool> {
    static const mc_pack_type_id TYPE_ID = MCPACKV2_BOOL;
};

struct null_type {
    null_type():value(0) {
    }
    char value;
};
template<>
struct Mcpack_type_id_traits<null_type> {
    static const mc_pack_type_id TYPE_ID = MCPACKV2_NULL;
};

template <typename alloc_t>
template <typename T>
int Protocol_v2<alloc_t>::get_int (char const * name, T * value) {
    typename Mcpack_type_id_traits<T>::LONG_TYPE v;
    int result = get_int(name, &v);
    if(result != 0) {
        return result;
    } else {
        if(Mcpack_type_id_traits<T>::range(v)) {
            *value = (int)v;
            return 0;

        } else {
            return MC_PE_FOUND_BUT_TYPE;
        }
    }
}

template <typename alloc_t>
template <typename T>
int Protocol_v2<alloc_t>::get_int_index (size_t in, T * value) {
    if(this->_type != MCPACKV2_ARRAY)
        return MC_PE_WRONG_PACK_TYPE;
    typename Mcpack_type_id_traits<T>::LONG_TYPE v;
    int result = get_int_index(in, &v);
    if(result != 0) {
        return result;
    } else {
        if(Mcpack_type_id_traits<T>::range(v)) {
            *value = (int)v;
            return 0;
        } else {
            return MC_PE_FOUND_BUT_TYPE;
        }
    }
}

template <typename alloc_t>
template <typename T>
int Protocol_v2<alloc_t>::mod_number (char const * name, T value) {
    return mod_simple_item(name, &value, sizeof(T), Mcpack_type_id_traits<T>::TYPE_ID);
}

template <typename alloc_t>
template <typename T>
int Protocol_v2<alloc_t>::mod_number_index(size_t in, T value) {
    return mod_simple_item_array(in, &value, sizeof(T), Mcpack_type_id_traits<T>::TYPE_ID);
}

template <typename alloc_t>
template<typename T>
int Protocol_v2<alloc_t>::put_simple_item (char const * name, const T * value) {
    if(!((this->_type == MCPACKV2_OBJECT && name != 0) || name == 0)) {
        return MC_PE_BAD_TYPE;
    }
    if(this->_root->mode != WRITE_ONLY)
        return MC_PE_WRONG_OPEN_MODE;
    int name_len;
    if(0 == name) {
        name_len = 0;
    } else {
        name_len = strlen(name) + 1;
    }
    if(name_len > 255) {
        return MC_PE_NAME_TOO_LONG;
    }
    short_item *item = make_space<short_item>(name_len + sizeof(T));
    if(item == 0) {
        return MC_PE_NO_SPACE;
    }
    inc_item();
    item->type = Mcpack_type_id_traits<T>::TYPE_ID;;
    item->name_len = name_len;
    memcpy(item->name, name, name_len);
    *(T*)(item->name + name_len) = *value;
    return 0;
}


template <typename alloc_t>
template <typename T>
int Protocol_v2<alloc_t>::get_number (char const * name, T * value) {
    if(this->_type != MCPACKV2_OBJECT)
        return MC_PE_WRONG_PACK_TYPE;
    short_item *item = (short_item *)find_item(name);
    if(item != 0) {
        if(item->type & MCPACKV2_FIXED_ITEM) {
            void *data = (char *)item->content();
            return trans_number(data, item->type, *value);
        } else if(item->type == (MCPACKV2_SHORT_ITEM | MCPACKV2_STRING)) {
            void *data = (char *)((short_vitem*)item)->content();
            return trans_number(data, MCPACKV2_STRING, *value);
        } else if(item->type == MCPACKV2_STRING) {
            void *data = (char *)((long_vitem*)item)->content();
            return trans_number(data, MCPACKV2_STRING, *value);
        } else {
            return MC_PE_BAD_TYPE;
        }
    }
    return MC_PE_NOT_FOUND;

}


template <typename alloc_t>
template <typename T>
int Protocol_v2<alloc_t>::get_number_index(size_t in, T * value) {
    if(this->_type != MCPACKV2_ARRAY)
        return MC_PE_WRONG_PACK_TYPE;
    short_item *item = (short_item *)find_item_index(in);
    if(item != 0) {
        if(item->type & MCPACKV2_FIXED_ITEM) {
            void *data = (char *)item->content();
            return trans_number(data, item->type, *value);
        } else if(item->type == (MCPACKV2_SHORT_ITEM | MCPACKV2_STRING)) {
            void *data = (char *)((short_vitem*)item)->content();
            return trans_number(data, MCPACKV2_STRING, *value);
        } else if(item->type == MCPACKV2_STRING) {
            void *data = (char *)((long_vitem*)item)->content();
            return trans_number(data, MCPACKV2_STRING, *value);
        } else {
            return MC_PE_BAD_TYPE;
        }
    }
    return MC_PE_NOT_FOUND;
}

template <typename alloc_t>
int Protocol_v2<alloc_t>::create(char * __buf, u_int __size, char * __tempbuf, u_int __tmp_size, Protocol_v2<alloc_t> ** __ppack) {
    if(env_check()) {
        return MC_PE_SYSTEM_ERROR;
    }
    if((char *)(((long)(__tempbuf + __tmp_size)) & -8L) - __tempbuf < (int)(sizeof(alloc_t) + sizeof(control))) {
        return MC_PE_NO_TEMP_SPACE;
    }
    if(__size < sizeof(long_vitem) + sizeof(long_pack)) {
        return MC_PE_NO_SPACE;
    }
    long_vitem * header = (long_vitem *)__buf;
    header->type = MCPACKV2_OBJECT;
    header->name_len = 0;
    header->content_len = 0;
    control * con = (typename Protocol_v2<alloc_t>::control *)((long)(__tempbuf + __tmp_size - sizeof(control)) & -8L);
    con->header = header;
    con->buffersize = __size;
    con->tmp_size = __tmp_size;
    con->mode = WRITE_ONLY;
    alloc_t *alloc = new((char *)con - sizeof(alloc_t)) alloc_t(__tempbuf, (char *)con - __tempbuf - sizeof(alloc_t));
    con->alloc = alloc;
    Protocol_v2<alloc_t> * pack = alloc->template malloc<Protocol_v2<alloc_t> >();
    if(pack != NULL) {
        new(pack) Protocol_v2<alloc_t>(header, (typename Protocol_v2<alloc_t>::long_pack *)(__buf + sizeof(long_vitem)), NULL, con, MCPACKV2_OBJECT);
    } else {
        return MC_PE_NO_TEMP_SPACE;
    }
    pack->init();
    *__ppack = pack;
    return 0;

}

template <typename alloc_t>
int Protocol_v2<alloc_t>::create(char * __buf, u_int __size, alloc_t *alloc, Protocol_v2<alloc_t> ** __ppack) {
    if(env_check()) {
        return MC_PE_SYSTEM_ERROR;
    }
    if(__size < sizeof(long_vitem) + sizeof(long_pack)) {
        return MC_PE_NO_SPACE;
    }
    long_vitem * header = (long_vitem *)__buf;
    header->type = MCPACKV2_OBJECT;
    header->name_len = 0;
    header->content_len = 0;
    control * con = alloc->template malloc<typename Protocol_v2<alloc_t>::control>();
    if(con==NULL){
        return MC_PE_NO_TEMP_SPACE;
    }
    con->header = header;
    con->buffersize = __size;
    con->mode = WRITE_ONLY;
    con->alloc = alloc;
    Protocol_v2<alloc_t> * pack = alloc->template malloc<Protocol_v2<alloc_t> >();
    if(pack != NULL) {
        new(pack) Protocol_v2<alloc_t>(header, (typename Protocol_v2<alloc_t>::long_pack *)(__buf + sizeof(long_vitem)), NULL, con, MCPACKV2_OBJECT);
    } else {
        return MC_PE_NO_TEMP_SPACE;
    }
    pack->init();
    *__ppack = pack;
    return 0;
}

template <typename alloc_t>
int Protocol_v2<alloc_t>::read(const char * __buf, u_int __size, char * __tempbuf, u_int __tmp_size, Protocol_v2<alloc_t> ** __ppack) {
    if(env_check()) {
        return MC_PE_SYSTEM_ERROR;
    }
    if((char *)(((long)(__tempbuf + __tmp_size)) & -8L) - __tempbuf < (int)(sizeof(alloc_t) + sizeof(control))) {
        return MC_PE_NO_TEMP_SPACE;
    }
    long_vitem * header = (long_vitem *)__buf;
    if(__size < sizeof(long_vitem) || (header->type != MCPACKV2_OBJECT && header->type != MCPACKV2_ARRAY) || header->size() > __size) {
        return MC_PE_BAD_DATA;
    }
    control * con = (typename Protocol_v2<alloc_t>::control *)((long)(__tempbuf + __tmp_size - sizeof(control)) & -8L);
    con->header = header;
    con->buffersize = __size;
    con->tmp_size = __tmp_size;
    alloc_t *alloc = new((char *)con - sizeof(alloc_t)) alloc_t(__tempbuf, (char *)con - __tempbuf - sizeof(alloc_t));
    con->alloc = alloc;
    con->mode = READ_ONLY;
    Protocol_v2<alloc_t> * pack = alloc->template malloc<Protocol_v2<alloc_t> >();
    if(pack != NULL) {
        new(pack) Protocol_v2<alloc_t>(header, (typename Protocol_v2<alloc_t>::long_pack *)header->content(), NULL, con, header->type);
    } else {
        return MC_PE_NO_TEMP_SPACE;
    }
    pack->build_index();
    *__ppack = pack;
    return 0;
}

template <typename alloc_t>
int Protocol_v2<alloc_t>::read(const char * __buf, u_int __size, alloc_t * alloc, Protocol_v2<alloc_t> ** __ppack) {
    if(env_check()) {
        return MC_PE_SYSTEM_ERROR;
    }
    long_vitem * header = (long_vitem *)__buf;
    if(__size < sizeof(long_vitem) || (header->type != MCPACKV2_OBJECT && header->type != MCPACKV2_ARRAY) || header->size() > __size) {
        return MC_PE_BAD_DATA;
    }
    control * con = alloc->template malloc<typename Protocol_v2<alloc_t>::control>();
    if(con==NULL){
        return MC_PE_NO_TEMP_SPACE;
    }
    con->header = header;
    con->alloc = alloc;
    con->mode = READ_ONLY;
    Protocol_v2<alloc_t> * pack = alloc->template malloc<Protocol_v2<alloc_t> >();
    if(pack != NULL) {
        new(pack) Protocol_v2<alloc_t>(header, (typename Protocol_v2<alloc_t>::long_pack *)header->content(), NULL, con, header->type);
    } else {
        return MC_PE_NO_TEMP_SPACE;
    }
    pack->build_index();
    *__ppack = pack;
    return 0;
}

template <typename alloc_t>
int Protocol_v2<alloc_t>::reopen(char * __buf, u_int __size, char * __tempbuf, u_int __tmp_size, Protocol_v2<alloc_t> ** __ppack) {
    if(env_check()) {
        return MC_PE_SYSTEM_ERROR;
    }
    if((char *)(((long)(__tempbuf + __tmp_size)) & -8L) - __tempbuf < (int)(sizeof(alloc_t) + sizeof(control))) {
        return MC_PE_NO_TEMP_SPACE;
    }
    long_vitem * header = (long_vitem *)__buf;
    if(__size < sizeof(long_vitem) || (header->type != MCPACKV2_OBJECT && header->type != MCPACKV2_ARRAY)|| header->size() > __size) {
        return MC_PE_BAD_DATA;
    }
    control * con = (typename Protocol_v2<alloc_t>::control *)((long)(__tempbuf + __tmp_size - sizeof(control)) & -8L);
    con->header = header;
    con->tmp_size = __tmp_size;
    alloc_t *alloc = new((char *)con - sizeof(alloc_t)) alloc_t(__tempbuf, (char *)con - __tempbuf - sizeof(alloc_t));
    con->alloc = alloc;
    con->mode = WRITE_ONLY;
    con->buffersize = __size;
    Protocol_v2<alloc_t> * pack = alloc->template malloc<Protocol_v2<alloc_t> >();
    if(pack != NULL) {
        new(pack) Protocol_v2<alloc_t>(header, (typename Protocol_v2<alloc_t>::long_pack *)header->content(), NULL, con, header->type);
    } else {
        return MC_PE_NO_TEMP_SPACE;
    }
    *__ppack = pack;
    return 0;
}

template <typename alloc_t>
int Protocol_v2<alloc_t>::reopen(char * __buf, u_int __size, alloc_t * alloc, Protocol_v2<alloc_t> ** __ppack) {
    if(env_check()) {
        return MC_PE_SYSTEM_ERROR;
    }
    long_vitem * header = (long_vitem *)__buf;
    if(__size < sizeof(long_vitem) || (header->type != MCPACKV2_OBJECT && header->type != MCPACKV2_ARRAY)|| header->size() > __size) {
        return MC_PE_BAD_DATA;
    }
    control * con = alloc->template malloc<typename Protocol_v2<alloc_t>::control>();
    if(con==NULL){
        return MC_PE_NO_TEMP_SPACE;
    }
    con->header = header;
    con->alloc = alloc;
    con->mode = WRITE_ONLY;
    con->buffersize = __size;
    Protocol_v2<alloc_t> * pack = alloc->template malloc<Protocol_v2<alloc_t> >();
    if(pack != NULL) {
        new(pack) Protocol_v2<alloc_t>(header, (typename Protocol_v2<alloc_t>::long_pack *)header->content(), NULL, con, header->type);
    } else {
        return MC_PE_NO_TEMP_SPACE;
    }
    *__ppack = pack;
    return 0;
}


template <typename alloc_t>
void Protocol_v2<alloc_t>::init () {
    long_vitem * vitem = (long_vitem *)this->_this_item;
    vitem->content_len = sizeof(long_pack);
    set_item(0);
}


template <typename alloc_t>
typename Protocol_v2<alloc_t>::hash_key_t Protocol_v2<alloc_t>::hash (char const * key, int len) const {
    const char *end = key + len;
    hash_key_t hashkey;
    for (hashkey = 0; key < end; key++) {
        hashkey *= 16777619;
        hashkey ^= *key;
    }
    return hashkey;
}

template <typename alloc_t>
void Protocol_v2<alloc_t>::init (index_t * index) {
    int cou = Protocol_v2<alloc_t>::count();
    this->_indexer = index;
    char * ptr = get_content();
    for(int i = 0; i < cou; i++) {
        if(check_ptr(ptr + sizeof(short_item)) < 0)
            return;
        short_item * item = (short_item *)ptr;
        if(!(item->type & 0x70)) {
            i--;
            if(check_add(&ptr, get_item_size(ptr)) < 0) {
                return;
            }
            continue;
        }
        if(item->type & MCPACKV2_FIXED_ITEM) {
            if(check_add(&ptr, item->size()) < 0) {
                return;
            }
            index->set(hash(item->name,item->name_len), item);
        } else if(item->type & MCPACKV2_SHORT_ITEM ) {
            if(check_ptr(ptr + sizeof(short_vitem)) < 0) {
                return;
            }
            short_vitem * vitem = (short_vitem *)ptr;
            if(check_add(&ptr, vitem->size()) < 0) {
                return;
            }
            if(check_str(vitem) < 0) {
                return;
            }
            index->set(hash(vitem->name,vitem->name_len), item);
        } else {
            if(check_ptr(ptr + sizeof(long_vitem)) < 0)
                return;
            long_vitem * vitem = (long_vitem *)ptr;
            if(check_add(&ptr, vitem->size()) < 0) {
                return;
            }
            if(check_str(vitem) < 0) {
                return;
            }
            index->set(hash(vitem->name,vitem->name_len), item);
        }
    }
}

template <typename alloc_t>
void Protocol_v2<alloc_t>::init (map_t * index) {
    int cou = Protocol_v2<alloc_t>::count();
    this->_indexer = index;
    char * ptr = get_content();
    for(int i = 0; i < cou; i++) {
        short_item * item = (short_item *)ptr;
        if(check_add(&ptr, get_item_size(ptr)) < 0)
            return;
        if(item->type & 0x70) {
            index->set(i, item);
        } else {
            i--;
        }
    }
}

template <typename alloc_t>
void * Protocol_v2<alloc_t>::find_item_linear (char const * name) const {
    int cou = Protocol_v2<alloc_t>::count();
    char * ptr = get_content();
    for(int i = 0; i < cou; i++) {
        if(check_ptr(ptr + sizeof(short_item)) < 0) {
            return 0;
        }
        short_item * item = (short_item *)ptr;
        if(!(item->type & 0x70)) {
            i--;
            if(check_add(&ptr, get_item_size(ptr)) < 0) {
                return 0;
            }
            continue;
        }
        if(item->type & MCPACKV2_FIXED_ITEM) {
            if(check_add(&ptr, item->size()) < 0) {
                return 0;
            }
            if(strncmp(name, item->name, item->name_len) == 0) {
                return item;
            }
        } else if(item->type & MCPACKV2_SHORT_ITEM) {
            short_vitem * vitem = (short_vitem *)ptr;
            if(check_ptr(ptr + sizeof(short_vitem)) < 0) {
                return 0;
            }
            if(check_add(&ptr, vitem->size()) < 0) {
                return 0;
            }
            if(strncmp(name, vitem->name, vitem->name_len) == 0) {
                if(check_str(vitem) < 0) {
                    return 0;
                }
                return item;
            }
        } else {
            long_vitem * vitem = (long_vitem *)ptr;
            if(check_ptr(ptr + sizeof(long_vitem)) < 0) {
                return 0;
            }
            if(check_add(&ptr, vitem->size()) < 0) {
                return 0;
            }
            if(strncmp(name, vitem->name, vitem->name_len) == 0) {
                if(check_str(vitem) < 0) {
                    return 0;
                }
                return item;
            }
        }
    }
    return 0;
}

template <typename alloc_t>
void * Protocol_v2<alloc_t>::find_item (char const * name) const {
    if(name == 0)
        return 0;
    int name_len = strlen(name) + 1;
    if(_indexer != 0) {
        short_item *item;
        item = (short_item *)((index_t*)_indexer)->get(hash(name, name_len));
        if(item == 0)
            return 0;
        else if(item == (short_item *)-1) {
            return find_item_linear(name);
        } else {
            return item;
        }
    } else {
        return find_item_linear(name);
    }
}

template <typename alloc_t>
void * Protocol_v2<alloc_t>::find_item_index (size_t i) const {
    if(_indexer != 0) {
        short_item * item = (short_item *)((map_t*)_indexer)->get(i);
        if(item == 0)
            return 0;
        else {
            return item;
        }
    } else {
        return find_item_index_linear(i);
    }
}

template <typename alloc_t>
void * Protocol_v2<alloc_t>::find_item_index_linear (size_t in) const {
    size_t cou = Protocol_v2<alloc_t>::count();
    if(in >= cou)
        return 0;
    char * ptr = get_content();
    short_item *item = 0;
    for(size_t i = 0; i <= in; i++) {
        item = (short_item *)ptr;
        if(check_add(&ptr, get_item_size(ptr)) < 0) {
            return 0;
        }
        if(!(item->type & 0x70)) {
            i--;
        }
    }
    if(item->type & MCPACKV2_SHORT_ITEM) {
        if(check_str((short_vitem *)item) < 0) {
            return 0;
        }
    } else if(item->type & MCPACKV2_SHORT_ITEM) {
        if(check_str((long_vitem *)item) < 0) {
            return 0;
        }
    }
    return item;
}


template <typename alloc_t>
int Protocol_v2<alloc_t>::mod_simple_item (char const * name, void const * value, size_t len, mc_pack_type_id idtype) {
    if(this->_type != MCPACKV2_OBJECT) {
        return MC_PE_WRONG_PACK_TYPE;
    }
    if(this->_root->mode != WRITE_ONLY) {
        return MC_PE_WRONG_OPEN_MODE;
    }
    if(0 == name) {
        return MC_PE_BAD_NAME;
    }
    short_item *item = (short_item *)find_item(name);
    if(item != 0) {
        if(item->type != idtype)
            return MC_PE_FOUND_BUT_TYPE;
        memcpy(item->name + item->name_len, value, len);
        return 0;
    } else {
        return MC_PE_NOT_FOUND;
    }
}


template <typename alloc_t>
int Protocol_v2<alloc_t>::mod_simple_item_array (size_t in, void const * value, size_t len, mc_pack_type_id idtype) {
    if(this->_type != MCPACKV2_ARRAY) {
        return MC_PE_WRONG_PACK_TYPE;
    }
    if(this->_root->mode != WRITE_ONLY) {
        return MC_PE_WRONG_OPEN_MODE;
    }
    short_item *item = (short_item *)find_item_index(in);
    if(item != 0) {
        if(item->type != idtype)
            return MC_PE_FOUND_BUT_TYPE;
        memcpy(item->content(), value, len);
        return 0;
    } else {
        return MC_PE_NOT_FOUND;
    }
}

template <typename alloc_t>
int Protocol_v2<alloc_t>::put_simple_vitem (char const * name, void const * value, int len, mc_pack_type_id idtype) {
    if(!((this->_type == MCPACKV2_OBJECT && name != 0) || name == 0)) {
        return MC_PE_BAD_TYPE;
    }
    if(this->_root->mode != WRITE_ONLY)
        return MC_PE_WRONG_OPEN_MODE;
    int name_len;
    if(0 == name) {
        name_len = 0;
    } else {
        name_len = strlen(name) + 1;
    }
    if(name_len > 255) {
        return MC_PE_NAME_TOO_LONG;
    }
    if(len < MAX_SHORT_VITEM_LEN) {
        short_vitem *item = make_space<short_vitem>(name_len + len);
        if(item == 0) {
            return MC_PE_NO_SPACE;
        }
        inc_item();
        item->type = MCPACKV2_SHORT_ITEM | idtype;
        item->name_len = name_len;
        item->content_len = len;
        memcpy(item->name, name, name_len);
        memcpy(item->name + name_len, value, len);
        if(idtype == MCPACKV2_STRING) {
            *(item->name + name_len + len - 1) = 0;
        }
    } else {
        long_vitem *item = make_space<long_vitem>( name_len + len);
        if(item == 0) {
            return MC_PE_NO_SPACE;
        }
        inc_item();
        item->type = idtype;
        item->name_len = name_len;
        item->content_len = len;
        memcpy(item->name, name, name_len);;
        memcpy(item->name + name_len, value, len);
        if(idtype == MCPACKV2_STRING) {
            *(item->name + name_len + len) = 0;
        }
    }
    return 0;
}

template <typename alloc_t>
int Protocol_v2<alloc_t>::put_complex_item (char const * name, int len, mc_pack_type_id idtype, long_vitem * * titem, void * * content) {
    if(this->_root->mode != WRITE_ONLY)
        return MC_PE_WRONG_OPEN_MODE;
    int name_len;
    if(0 == name) {
        name_len = 0;
    } else {
        name_len = strlen(name) + 1;
        if(name_len > 255) {
            return MC_PE_NAME_TOO_LONG;
        }
    }
    long_vitem *item = make_space<long_vitem>( name_len + len);
    if(item == 0) {
        return MC_PE_NO_SPACE;
    }
    inc_item();
    *titem = item;
    item->type = idtype;
    item->name_len = name_len;
    memcpy(item->name, name, name_len);;
    *content = item->name + name_len;
    return 0;
}
template <typename alloc_t>
Protocol_v2<alloc_t>::Protocol_v2(long_vitem * ttitem, long_pack * tpack_ptr, Protocol_v2<alloc_t> * tparent,
                                  typename Protocol_v2<alloc_t>::control * troot, char idtype) {
    this->_pack_ptr = tpack_ptr;
    this->_parent = tparent;
    this->_root = troot;
    this->_indexer = 0;
    this->_this_item = ttitem;
    this->_type = idtype;
}

template <typename alloc_t>
int Protocol_v2<alloc_t>::build_index () {
    if(_root->mode != READ_ONLY)
        return 0;
    if(count() < 4)
        return 0;
    if(this->_type == MCPACKV2_OBJECT) {
        uint64_t buck_size = get_indexer_size();
        uint64_t index_size = index_t::calc_size(buck_size);
        if(index_size > UINT32_MAX)
            return MC_PE_NO_TEMP_SPACE;
        index_t * index = (index_t *)_root->alloc->malloc(index_size);
        if(index != NULL) {
            new(index) index_t(buck_size);
            init(index);
            return 0;
        } else {
            return MC_PE_NO_TEMP_SPACE;            
        }
    } else {
        uint64_t buck_size = count();
        uint64_t index_size = map_t::calc_size(buck_size);
        if(index_size > UINT32_MAX)
            return MC_PE_NO_TEMP_SPACE;
        map_t * index = (map_t *)_root->alloc->malloc(index_size);
        if(index != NULL) {
            new(index) map_t(buck_size);
            init(index);
            return 0;
        } else {
            return MC_PE_NO_TEMP_SPACE;
        }
    }
}

template <typename alloc_t>
int Protocol_v2<alloc_t>::put_container (char const * name, mc_pack_type_id idtype, Mcpack ** out_pack) {
    if(this->_type == MCPACKV2_ARRAY) {
        if(name != 0) {
            return MC_PE_BAD_TYPE;
        }
    } else {
        if(name == 0) {
            return MC_PE_BAD_NAME;
        } else if(find_item(name) != 0) {
            return MC_PE_ALREADY_EXIST;
        }
    }
    if(this->_root->mode != WRITE_ONLY)
        return MC_PE_WRONG_OPEN_MODE;
    Protocol_v2<alloc_t> * newpack;
    newpack = alloc_handler();
    if(newpack == NULL)
        return MC_PE_NO_TEMP_SPACE;
    long_pack * pack = 0;
    long_vitem * titem;
    int result = put_complex_item(name, sizeof(long_pack), idtype, &titem, (void **)&pack);
    if( result != 0) {
        _root->alloc->free(newpack);
        return result;
    }
    new(newpack) Protocol_v2<alloc_t>(titem, pack, this, this->_root, idtype);
    *out_pack = newpack;
    newpack->init();
    return 0;
}
template <typename alloc_t>
int Protocol_v2<alloc_t>::is_valid () const {
    int cou = Protocol_v2<alloc_t>::count();
    char * ptr = get_content();
    for(int i = 0; i < cou; i++) {
        if(check_add(&ptr, get_item_size(ptr)) < 0)
            return 0;
    }
    return 1;
}
template <typename alloc_t>
void * Protocol_v2<alloc_t>::get_buffer() const {
    return _this_item;
}

template <typename alloc_t>
int Protocol_v2<alloc_t>::put_pack (char const * name, Mcpack ** out_pack) {
    return put_container(name, MCPACKV2_OBJECT, out_pack);
}
template <typename alloc_t>
int Protocol_v2<alloc_t>::get_pack (char const * name, Mcpack ** out_pack) {
    if(this->_type != MCPACKV2_OBJECT)
        return MC_PE_WRONG_PACK_TYPE;
    Protocol_v2<alloc_t> * pack;
    pack = alloc_handler();
    if(pack == NULL)
        return MC_PE_NO_TEMP_SPACE;
    long_vitem * titem = (long_vitem *)find_item(name);
    if(titem != 0) {
        if(titem->type == MCPACKV2_NULL) {
            _root->alloc->free(pack);
            return MC_PE_NULL_VALUE;
        } else if(titem->content_len < sizeof(long_pack)) {
            _root->alloc->free(pack);
            return MC_PE_BAD_DATA;
        } else if(titem->type != MCPACKV2_OBJECT) {
            _root->alloc->free(pack);
            return MC_PE_FOUND_BUT_TYPE;
        } else {
            new(pack) Protocol_v2<alloc_t>(titem, (long_pack *)(titem->name + titem->name_len), this, this->_root, MCPACKV2_OBJECT);
            *out_pack = pack;
            
            
            pack->build_index();
            return 0;
        }
    } else {
        _root->alloc->free(pack);
        return MC_PE_NOT_FOUND;
    }
}
template <typename alloc_t>
int Protocol_v2<alloc_t>::get_pack_index (size_t in, Mcpack ** out_pack) {
    if(this->_type != MCPACKV2_ARRAY)
        return MC_PE_WRONG_PACK_TYPE;
    Protocol_v2<alloc_t> * pack;
    pack = alloc_handler();
    if(pack == NULL)
        return MC_PE_NO_TEMP_SPACE;
    long_vitem * titem = (long_vitem *)find_item_index(in);
    if(titem != 0) {
        if(titem->type == MCPACKV2_NULL) {
            _root->alloc->free(pack);
            return MC_PE_NULL_VALUE;
        } else if(titem->content_len < sizeof(long_pack)) {
            _root->alloc->free(pack);
            return MC_PE_BAD_DATA;
        } else if(titem->type != MCPACKV2_OBJECT) {
            _root->alloc->free(pack);
            return MC_PE_FOUND_BUT_TYPE;
        } else {
            new(pack) Protocol_v2<alloc_t>(titem, (long_pack *)(titem->content()), this, this->_root, MCPACKV2_OBJECT);
            *out_pack = pack;
            
            pack->build_index();
            return 0;
        }
    } else {
        _root->alloc->free(pack);
        return MC_PE_NOT_FOUND;
    }
}
template <typename alloc_t>
int Protocol_v2<alloc_t>::put_array (char const * name, Mcpack ** out_pack) {
    return put_container(name, MCPACKV2_ARRAY, out_pack);
}
template <typename alloc_t>
int Protocol_v2<alloc_t>::get_array (char const * name, Mcpack ** out_pack) {
    if(this->_type != MCPACKV2_OBJECT)
        return MC_PE_WRONG_PACK_TYPE;
    Protocol_v2<alloc_t> * pack;
    pack = alloc_handler();
    if(pack == NULL)
        return MC_PE_NO_TEMP_SPACE;
    long_vitem * titem = (long_vitem *)find_item(name);
    if(titem != 0) {
        if(titem->type == MCPACKV2_NULL) {
            _root->alloc->free(pack);
            return MC_PE_NULL_VALUE;
        } else if(titem->content_len < sizeof(long_pack)) {
            _root->alloc->free(pack);
            return MC_PE_BAD_DATA;
        } else if(titem->type != MCPACKV2_ARRAY) {
            _root->alloc->free(pack);
            return MC_PE_FOUND_BUT_TYPE;
        } else {
            new(pack) Protocol_v2<alloc_t>(titem, (long_pack *)titem->content(), this, this->_root, MCPACKV2_ARRAY);
            *out_pack = pack;
            
            pack->build_index();
            return 0;
        }
    } else {
        _root->alloc->free(pack);
        return MC_PE_NOT_FOUND;
    }
}
template <typename alloc_t>
int Protocol_v2<alloc_t>::get_array_index (size_t in, Mcpack ** out_pack) {
    if(this->_type != MCPACKV2_ARRAY)
        return MC_PE_WRONG_PACK_TYPE;
    Protocol_v2<alloc_t> * pack;
    pack = alloc_handler();
    if(pack == NULL)
        return MC_PE_NO_TEMP_SPACE;
    long_vitem * titem = (long_vitem *)find_item_index(in);
    if(titem != 0) {
        if(titem->type == MCPACKV2_NULL) {
            _root->alloc->free(pack);
            return MC_PE_NULL_VALUE;
        } else if(titem->content_len < sizeof(long_pack)) {
            _root->alloc->free(pack);
            return MC_PE_BAD_DATA;
        } else if(titem->type != MCPACKV2_ARRAY) {
            _root->alloc->free(pack);
            return MC_PE_FOUND_BUT_TYPE;
        } else {
            new(pack) Protocol_v2<alloc_t>(titem, (long_pack *)(titem->content()), this, this->_root, MCPACKV2_ARRAY);
            *out_pack = pack;
            
            pack->build_index();
            return 0;
        }
    } else {
        _root->alloc->free(pack);
        return MC_PE_NOT_FOUND;
    }
}
template <typename alloc_t>
int Protocol_v2<alloc_t>::put_string (char const * name, char const * value) {
    if(value == 0)
        return MC_PE_BAD_PARAM;
    return put_simple_vitem(name, value, strlen(value) + 1, MCPACKV2_STRING);
}

template <typename alloc_t>
int Protocol_v2<alloc_t>::put_nstring (char const * name, char const * value, unsigned int length) {
    if(value == 0)
        return MC_PE_BAD_PARAM;
    unsigned int s = strlen(value);
    if(s > length)
        s = length;
    return put_simple_vitem(name, value, s + 1, MCPACKV2_STRING);
}

template <typename alloc_t>
int Protocol_v2<alloc_t>::get_string (char const * name, const char ** value) {
    if(this->_type != MCPACKV2_OBJECT)
        return MC_PE_WRONG_PACK_TYPE;
    short_vitem *item = (short_vitem *)find_item(name);
    if(item != 0) {
        if(item->type == MCPACKV2_NULL) {
            return MC_PE_NULL_VALUE;
        } else if(item->type == (MCPACKV2_STRING | MCPACKV2_SHORT_ITEM)) {
            *value = (char *)item->content();
            return 0;

        } else if(item->type == MCPACKV2_STRING) {
            *value = (char *)((long_vitem*)item)->content();
            return 0;
        } else {
            return MC_PE_BAD_TYPE;
        }
    } else {
        return MC_PE_NOT_FOUND;
    }
}
template <typename alloc_t>
int Protocol_v2<alloc_t>::get_string_index (size_t in, const char ** value) {
    if(this->_type != MCPACKV2_ARRAY)
        return MC_PE_WRONG_PACK_TYPE;
    short_vitem *item = (short_vitem *)find_item_index(in);
    if(item != 0) {
        if(item->type == MCPACKV2_NULL) {
            return MC_PE_NULL_VALUE;
        } else if(item->type == (MCPACKV2_STRING | MCPACKV2_SHORT_ITEM)) {
            *value = (char*)item->content();
            return 0;

        } else if(item->type == MCPACKV2_STRING) {
            *value = (char*)((long_vitem *)item)->content();
            return 0;
        } else {
            return MC_PE_BAD_TYPE;
        }
    } else {
        return MC_PE_NOT_FOUND;
    }
}

template <typename alloc_t>
int Protocol_v2<alloc_t>::put_raw (char const * name, const void * value, u_int itemsize) {
    if(value == 0)
        return MC_PE_BAD_PARAM;
    return put_simple_vitem(name, (const char *)value, itemsize, MCPACKV2_RAW);
}

template <typename alloc_t>
int Protocol_v2<alloc_t>::get_raw (char const * name, const void ** value, u_int * itemsize) {
    if(this->_type != MCPACKV2_OBJECT)
        return MC_PE_WRONG_PACK_TYPE;
    short_vitem *item = (short_vitem *)find_item(name);
    if(item != 0) {
        if(item->type == (MCPACKV2_RAW | MCPACKV2_SHORT_ITEM)) {
            *value = (char *)item + sizeof(short_vitem) + item->name_len;
            *itemsize = item->content_len;
            return 0;

        } else if(item->type == MCPACKV2_RAW) {
            *value = (char *)item + sizeof(long_vitem) + item->name_len;
            *itemsize = ((long_vitem *)item)->content_len;
            return 0;
        } else {
            return MC_PE_BAD_TYPE;
        }
    } else {
        return MC_PE_NOT_FOUND;
    }
}

template <typename alloc_t>
int Protocol_v2<alloc_t>::get_raw_index (size_t in, const void ** value, u_int * itemsize) {
    if(this->_type != MCPACKV2_ARRAY)
        return MC_PE_WRONG_PACK_TYPE;
    short_vitem *item = (short_vitem *)find_item_index(in);
    if(item != 0) {
        if(item->type == (MCPACKV2_RAW | MCPACKV2_SHORT_ITEM)) {
            *value = item->content();
            *itemsize = ((short_vitem *)item)->content_len;
            return 0;

        } else if(item->type == MCPACKV2_RAW) {
            *value = ((long_vitem *)item)->content();
            *itemsize = ((long_vitem *)item)->content_len;
            return 0;
        } else {
            return MC_PE_BAD_TYPE;
        }
    } else {
        return MC_PE_NOT_FOUND;
    }
}

template <typename alloc_t>
int Protocol_v2<alloc_t>::rm_item(void * __titem, int flag) {
    if(0 == __titem) {
        return MC_PE_BAD_PARAM;
    }
    short_item *item = (short_item *)__titem;
    if(0 == item)
        return MC_PE_NOT_FOUND;
    dec_item ();
    if(flag) {
        item->type &= 0x8f;
    } else {
        int item_size = get_item_size(item);
        this->reserve_space(-item_size);
        memmove(item , (char*)item + item_size, (char *)_root->header + _root->header->size() - (char*)item);
    }
    return 0;
}

template <typename alloc_t>
int Protocol_v2<alloc_t>::rm(const char * name, int flag) {
    if(this->_type != MCPACKV2_OBJECT)
        return MC_PE_WRONG_PACK_TYPE;
    return rm_item(find_item_linear(name), flag);
}

template <typename alloc_t>
int Protocol_v2<alloc_t>::rm_index(size_t i, int flag) {
    if(this->_type != MCPACKV2_ARRAY)
        return MC_PE_WRONG_PACK_TYPE;
    return rm_item(find_item_index_linear(i), flag);
}

template <typename alloc_t>
int Protocol_v2<alloc_t>::close () {
    return 0;
}

template <typename alloc_t>
void Protocol_v2<alloc_t>::finish () {
    this->free_index();
    _root->alloc->free(this);
}

template <typename alloc_t>
int Protocol_v2<alloc_t>::make_mc_item(char * ptr, mc_pack_item_t * __item) {
    short_item *item = (short_item *)ptr;
    __item->internal = ptr;
    if(item->type & MCPACKV2_FIXED_ITEM) {
        if(check_add(&ptr, item->size()) < 0)
            return MC_PE_BAD_DATA;
        if(item->name_len > 0) {
            if(strnlen(item->name, item->name_len) + 1 != item->name_len) {
                return MC_PE_BAD_DATA;
            }
            __item->key = item->name;
        } else {
            __item->key = (const char *)&item->name_len;
        }
        __item->value = (const char *)item->content();
        __item->type = gtype_dict.dict[item->type & 0x7f];
        __item->value_size = item->type & MCPACKV2_FIXED_ITEM;
        return 0;
    } else if(item->type & MCPACKV2_SHORT_ITEM) {
        short_vitem * vitem = (short_vitem *)ptr;
        if(check_add(&ptr, vitem->size()) < 0)
            return MC_PE_BAD_DATA;
        if(vitem->name_len > 0) {
            if(strnlen(vitem->name, vitem->name_len) + 1 != vitem->name_len) {
                return MC_PE_BAD_DATA;
            }
            __item->key = vitem->name;
        } else {
            __item->key = (const char *)&vitem->name_len;
        }
        __item->value = (const char *)vitem->content();
        __item->type = gtype_dict.dict[vitem->type & 0x7f];
        __item->value_size = vitem->content_len;
        return 0;
    } else {
        long_vitem * vitem = (long_vitem *)ptr;
        if(check_add(&ptr, vitem->size()) < 0) {
            return MC_PE_BAD_DATA;
        }
        if(vitem->name_len > 0) {
            if(strnlen(vitem->name, vitem->name_len) + 1 != vitem->name_len) {
                return MC_PE_BAD_DATA;
            }
            __item->key = vitem->name;
        } else {
            __item->key = (const char *)&vitem->name_len;
        }
        __item->value = (const char *)vitem->content();
        __item->type = gtype_dict.dict[vitem->type & 0x7f];
        if(vitem->type == MCPACKV2_OBJECT || vitem->type == MCPACKV2_ARRAY) {
            if(vitem->content_len < sizeof(long_pack)) {
                return MC_PE_BAD_DATA;
            }
            __item->value_size = ((long_pack *)vitem->content())->item_count;
        } else {
            __item->value_size = vitem->content_len;
        }
        return 0;
    }
}
template <typename alloc_t>
int Protocol_v2<alloc_t>::first_item(mc_pack_item_t * __item) {
    if(0 == __item) {
        return MC_PE_BAD_PARAM;
    }
    char * ptr = get_content();
    while(true) {
        if(check_ptr(ptr + sizeof(short_item)) < 0)
            return -1;
        short_item *item = (short_item *)ptr;
        if(!(item->type & 0x70)) {
            if(check_add(&ptr, get_item_size(ptr)) < 0) {
                return MC_PE_NOT_FOUND;
            }
            if(item->type & MCPACKV2_SHORT_ITEM) {
                if(check_str((short_vitem *)item) < 0) {
                    return MC_PE_NOT_FOUND;
                }
            } else if(item->type & MCPACKV2_SHORT_ITEM) {
                if(check_str((long_vitem *)item) < 0) {
                    return MC_PE_NOT_FOUND;
                }
            }
        } else {

            break;
        }
    }
    return make_mc_item(ptr, __item);
}

template <typename alloc_t>
int Protocol_v2<alloc_t>::next_item(const mc_pack_item_t * __itemold, mc_pack_item_t * __item) {
    if(0 == __itemold || 0 == __item) {
        return MC_PE_BAD_PARAM;
    }
    char *ptr = __itemold->internal;
    if(check_ptr(ptr) < 0) {
        return MC_PE_BAD_PARAM;
    }
    if(check_add(&ptr, get_item_size(__itemold->internal)) < 0) {
        return MC_PE_BAD_PARAM;
    }
    while(true) {
        if(check_ptr(ptr + sizeof(short_item)) < 0)
            return -1;
        short_item *item = (short_item *)ptr;
        if(!(item->type & 0x70)) {
            if(check_add(&ptr, get_item_size(ptr)) < 0) {
                return MC_PE_NOT_FOUND;
            }
            if(item->type & MCPACKV2_SHORT_ITEM) {
                if(check_str((short_vitem *)item) < 0) {
                    return MC_PE_NOT_FOUND;
                }
            } else if(item->type & MCPACKV2_SHORT_ITEM) {
                if(check_str((long_vitem *)item) < 0) {
                    return MC_PE_NOT_FOUND;
                }
            }
        } else {

            break;
        }
    }
    return make_mc_item(ptr, __item);
}

template <typename alloc_t>
int Protocol_v2<alloc_t>::get_item(const char * name, mc_pack_item_t * __item) {
    if(0 == __item) {
        return MC_PE_BAD_PARAM;
    }
    if(this->_type != MCPACKV2_OBJECT)
        return MC_PE_WRONG_PACK_TYPE;
    char *ptr = (char *)find_item(name);
    if(ptr) {
        return make_mc_item(ptr, __item);
    } else {
        return MC_PE_NOT_FOUND;
    }
}

template <typename alloc_t>
int Protocol_v2<alloc_t>::get_item_index(size_t in, mc_pack_item_t * __item) {
    if(0 == __item) {
        return MC_PE_BAD_PARAM;
    }
    if(this->_type != MCPACKV2_ARRAY)
        return MC_PE_WRONG_PACK_TYPE;
    char *ptr = (char *)find_item_index(in);
    if(ptr) {
        return make_mc_item(ptr, __item);
    } else {
        return MC_PE_NOT_FOUND;
    }
}

template <typename alloc_t>
int Protocol_v2<alloc_t>::get_item_by_index(size_t in, mc_pack_item_t * __item) {
    if(0 == __item) {
        return MC_PE_BAD_PARAM;
    }
    unsigned int cou = Protocol_v2<alloc_t>::count();
    if(in >= cou) {
        return MC_PE_NOT_FOUND;
    }
    char * ptr = get_content();
    for(size_t i = 0; i < in; i++) {
        if(check_ptr(ptr + sizeof(short_item)) < 0) {
            return MC_PE_NOT_FOUND;
        }
        short_item * item = (short_item *)ptr;
        if(!(item->type & 0x70)) {
            i--;
            if(check_add(&ptr, get_item_size(ptr)) < 0) {
                return MC_PE_NOT_FOUND;
            }
            continue;
        }
        if(item->type & MCPACKV2_FIXED_ITEM) {
            if(check_add(&ptr, item->size()) < 0) {
                return MC_PE_NOT_FOUND;
            }
        } else if(item->type & MCPACKV2_SHORT_ITEM) {
            short_vitem * vitem = (short_vitem *)ptr;
            if(check_ptr(ptr + sizeof(short_vitem)) < 0) {
                return MC_PE_NOT_FOUND;
            }
            if(check_add(&ptr, vitem->size()) < 0) {
                return MC_PE_NOT_FOUND;
            }
        } else {
            long_vitem * vitem = (long_vitem *)ptr;
            if(check_ptr(ptr + sizeof(long_vitem)) < 0) {
                return MC_PE_NOT_FOUND;
            }
            if(check_add(&ptr, vitem->size()) < 0) {
                return MC_PE_NOT_FOUND;
            }
        }
    }
    return make_mc_item(ptr, __item);
}

template <typename alloc_t>
int Protocol_v2<alloc_t>::get_int32_from_item(const mc_pack_item_t * mcitem, mc_int32_t *value) {
    if(0 == mcitem || 0 == value) {
        return MC_PE_BAD_PARAM;
    }
    mc_int64_t v;
    if(mcitem != 0) {
        if(mcitem->type > 256) {
            return MC_PE_BAD_DATA;
        }
        char oldtype = *mcitem->internal;
        if(check_ptr(mcitem->value + mcitem->value_size) < 0) {
            return MC_PE_BAD_DATA;
        }
        int result =  trans_number(mcitem->value, oldtype, v);
        if(result < 0)
            return result;
        else {
            if(Mcpack_type_id_traits<mc_int32_t>::range(v)) {
                *value = (int)v;
                return 0;
            } else {
                return MC_PE_FOUND_BUT_TYPE;
            }
        }
    } else {
        return MC_PE_BAD_PARAM;
    }
}

template <typename alloc_t>
int Protocol_v2<alloc_t>::get_int64_from_item(const mc_pack_item_t * mcitem, mc_int64_t *value) {
    if(0 == mcitem || 0 == value) {
        return MC_PE_BAD_PARAM;
    }
    mc_int64_t v;
    if(mcitem != 0) {
        if(mcitem->type > 256) {
            return MC_PE_BAD_DATA;
        }
        char oldtype = *mcitem->internal;
        if(check_ptr(mcitem->value + mcitem->value_size) < 0) {
            return MC_PE_BAD_DATA;
        }
        int result =  trans_number(mcitem->value, oldtype, v);
        if(result < 0)
            return result;
        else {
            *value = v;
            return 0;
        }
    } else {
        return MC_PE_BAD_PARAM;
    }
}

template <typename alloc_t>
int Protocol_v2<alloc_t>::get_uint32_from_item(const mc_pack_item_t * mcitem, mc_uint32_t *value) {
    if(0 == mcitem || 0 == value) {
        return MC_PE_BAD_PARAM;
    }
    mc_uint64_t v;
    if(mcitem != 0) {
        if(mcitem->type > 256) {
            return MC_PE_BAD_DATA;
        }
        char oldtype = *mcitem->internal;
        if(check_ptr(mcitem->value + mcitem->value_size) < 0) {
            return MC_PE_BAD_DATA;
        }
        int result =  trans_number(mcitem->value, oldtype, v);
        if(result < 0)
            return result;
        else {
            if(Mcpack_type_id_traits<mc_uint32_t>::range(v)) {
                *value = (int)v;
                return 0;
            } else {
                return MC_PE_FOUND_BUT_TYPE;
            }
        }
    } else {
        return MC_PE_BAD_PARAM;
    }
}

template <typename alloc_t>
int Protocol_v2<alloc_t>::get_uint64_from_item(const mc_pack_item_t * mcitem, mc_uint64_t *value) {
    if(0 == mcitem || 0 == value) {
        return MC_PE_BAD_PARAM;
    }
    mc_uint64_t v;
    if(mcitem != 0) {
        if(mcitem->type > 256) {
            return MC_PE_BAD_DATA;
        }
        char oldtype = *mcitem->internal;
        if(check_ptr(mcitem->value + mcitem->value_size) < 0) {
            return MC_PE_BAD_DATA;
        }
        int result =  trans_number(mcitem->value, oldtype, v);
        if(result < 0)
            return result;
        else {
            *value = v;
            return 0;
        }
    } else {
        return MC_PE_BAD_PARAM;
    }
}
template <typename alloc_t>
int Protocol_v2<alloc_t>::get_float_from_item(const mc_pack_item_t * mcitem, float *value) {
    if(0 == mcitem || 0 == value) {
        return MC_PE_BAD_PARAM;
    }
    if(mcitem != 0) {
        if(mcitem->type > 256) {
            return MC_PE_BAD_DATA;
        }
        char oldtype = *mcitem->internal;
        if(check_ptr(mcitem->value + mcitem->value_size) < 0) {
            return MC_PE_BAD_DATA;
        }
        return trans_number(mcitem->value, oldtype, *value);
    } else {
        return MC_PE_BAD_PARAM;
    }
}
template <typename alloc_t>
int Protocol_v2<alloc_t>::get_dolcle_from_item(const mc_pack_item_t * mcitem, dolcle *value) {
    if(0 == mcitem || 0 == value) {
        return MC_PE_BAD_PARAM;
    }
    if(mcitem != 0) {
        if(mcitem->type > 256) {
            return MC_PE_BAD_DATA;
        }
        char oldtype = *mcitem->internal;
        if(check_ptr(mcitem->value + mcitem->value_size) < 0) {
            return MC_PE_BAD_DATA;
        }
        return trans_number(mcitem->value, oldtype, *value);
    } else {
        return MC_PE_BAD_PARAM;
    }
}
template <typename alloc_t>
int Protocol_v2<alloc_t>::get_bool_from_item(const mc_pack_item_t * mcitem, bool *value) {
    if(0 == mcitem || 0 == value) {
        return MC_PE_BAD_PARAM;
    }
    if(mcitem != 0) {
        if(mcitem->type > 256) {
            return MC_PE_BAD_DATA;
        }
        char oldtype = greverse_type_dict.dict[mcitem->type];
        if(oldtype != MCPACKV2_BOOL) {
            return MC_PE_BAD_TYPE;
        }
        if(check_ptr(mcitem->value + mcitem->value_size) < 0) {
            return MC_PE_BAD_DATA;
        } else {
            *value = *(bool *)mcitem->value;
            return 0;
        }
    } else {
        return MC_PE_BAD_PARAM;
    }
}
template <typename alloc_t>
int Protocol_v2<alloc_t>::get_pack_from_item(const mc_pack_item_t * item, Mcpack **out_pack) {
    if(0 == item || 0 == out_pack) {
        return MC_PE_BAD_PARAM;
    }
    if(greverse_type_dict.dict[item->type] != MCPACKV2_OBJECT)
        return MC_PE_WRONG_PACK_TYPE;
    Protocol_v2<alloc_t> * pack;
    pack = alloc_handler();
    if(pack == NULL)
        return MC_PE_NO_TEMP_SPACE;
    long_vitem * titem = (long_vitem *)item->internal;
    if(titem != 0) {
        if(titem->type == MCPACKV2_NULL) {
            _root->alloc->free(pack);
            return MC_PE_NULL_VALUE;
        } else if(titem->type != MCPACKV2_OBJECT) {
            _root->alloc->free(pack);
            return MC_PE_FOUND_BUT_TYPE;
        } else {
            new(pack) Protocol_v2<alloc_t>(titem, (long_pack *)titem->content(), this, this->_root, MCPACKV2_OBJECT);
            *out_pack = pack;
            return 0;
        }
    } else {
        _root->alloc->free(pack);
        return MC_PE_NOT_FOUND;
    }
}

template <typename alloc_t>
int Protocol_v2<alloc_t>::get_array_from_item(const mc_pack_item_t * item, Mcpack **out_pack) {
    if(0 == item || 0 == out_pack) {
        return MC_PE_BAD_PARAM;
    }
    if(greverse_type_dict.dict[item->type] != MCPACKV2_ARRAY)
        return MC_PE_WRONG_PACK_TYPE;
    Protocol_v2<alloc_t> * pack;
    pack = alloc_handler();
    if(pack == NULL)
        return MC_PE_NO_TEMP_SPACE;
    long_vitem * titem = (long_vitem *)item->internal;
    if(titem != 0) {
        if(titem->type == MCPACKV2_NULL) {
            _root->alloc->free(pack);
            return MC_PE_NULL_VALUE;
        } else if(titem->type != MCPACKV2_ARRAY) {
            _root->alloc->free(pack);
            return MC_PE_FOUND_BUT_TYPE;
        } else {
            new(pack) Protocol_v2<alloc_t>(titem, (long_pack *)titem->content(), this, this->_root, MCPACKV2_ARRAY);
            *out_pack = pack;
            return 0;
        }
    } else {
        _root->alloc->free(pack);
        return MC_PE_NOT_FOUND;
    }
}
template <typename alloc_t>
int Protocol_v2<alloc_t>::copy_item(const void* ptr) {
    const short_item *item = (const short_item *)ptr;
    if(item != 0) {
        if(item->type & MCPACKV2_FIXED_ITEM) {
            short_item *newitem = make_space<short_item>(item->content_size());
            if(newitem == 0) {
                return MC_PE_NO_SPACE;
            }
            memcpy(newitem, item, item->size());
        } else if(item->type & MCPACKV2_SHORT_ITEM) {
            short_vitem * newitem = (short_vitem *) make_raw_space(((short_vitem *)item)->size());
            if(newitem == 0) {
                return MC_PE_NO_SPACE;
            }
            memcpy(newitem, item, ((short_vitem*)item)->size());
        } else {
            long_vitem *newitem = (long_vitem *) make_raw_space(((long_vitem*)item)->size());
            if(newitem == 0) {
                return MC_PE_NO_SPACE;
            }
            memcpy(newitem, item, ((long_vitem *)item)->size());
        }
        inc_item();
        return 0;
    } else {
        return MC_PE_NOT_FOUND;
    }
}

template <typename alloc_t>
int Protocol_v2<alloc_t>::put_item(const mc_pack_item_t * item) {
    if(0 == item) {
        return MC_PE_BAD_PARAM;
    }
    const char * name = 0;
    if(this->_type == MCPACKV2_OBJECT) {
        name = item->key;
    }
    switch(item->type) {
    case MC_PT_OBJ:
        Mcpack * p1;
        return put_pack(name, &p1);
    case MC_PT_ARR:
        Mcpack * p2;
        return put_array(name, &p2);
    case MC_IT_BIN:
        return put_raw(name, item->value, item->value_size);
    case MC_IT_TXT:
        return put_string(name, item->value);
    case MC_IT_I32:
        return put_int32(name, *(mc_int32_t *)item->value);
    case MC_IT_U32:
        return put_uint32(name, *(mc_uint32_t *)item->value);
    case MC_IT_I64:
        return put_int64(name, *(mc_int64_t *)item->value);
    case MC_IT_U64:
        return put_uint64(name, *(mc_uint64_t *)item->value);
    case MC_IT_FLOAT:
        return put_float(name, *(float *)item->value);
    case MC_IT_DOLCLE:
        return put_dolcle(name, *(dolcle *)item->value);
    case MC_IT_BOOL:
        return put_bool(name, *(bool *)item->value);
    case MC_IT_NULL:
        return put_null(name);
    case MC_IT_UNKNOWN:
        return copy_item(item->internal);
    default:
        return MC_PE_BAD_TYPE;
    }
}

template <typename alloc_t>
int Protocol_v2<alloc_t>::foreach(mc_pack_foreach_callback cb, void *arg) {
    mc_pack_item_t item;
    mc_pack_item_t new_item;
    
    if(count()==0){
        return 0;
    }
    item.pack = (mc_pack_t*)this;
    int result = first_item(&item);
    if(result < 0)
        return result;
    int ret;
    int retcnt = 0;
    while(MC_PE_SUCCESS == result) {
        ret = cb(&item, arg);
        if(ret < 0) {
            return ret;
        } else {
            retcnt += ret;
        }
        if(item.type == MC_PT_OBJ) {
            Mcpack * p;
            result=get_pack_from_item(&item,&p);
            if(result < 0) {
                return result;
            } else {
                ret=p->foreach(cb, arg);
                if(ret<0){
                    return ret;
                }
            }
        } else if(item.type == MC_PT_ARR) {
            Mcpack * p;
            result=get_array_from_item(&item,&p);
            if(result  < 0) {
                return result;
            } else {
                ret=p->foreach(cb, arg);
                if(ret<0){
                    return ret;
                }
            }
        }
        result = next_item(&item, &new_item);
        item = new_item;
        item.pack = (mc_pack_t*)this;
    }
    return retcnt;
}


template <typename alloc_t>
int Protocol_v2<alloc_t>::foreach_ex(mc_pack_foreach_callback_ex cb, void *arg,full_key_t *key) {
    mc_pack_item_t item;
    mc_pack_item_t new_item;
    
    if(count()==0){
        return 0;
    }
    item.pack = (mc_pack_t*)this;
    int result = first_item(&item);
    if(result < 0)
        return result;
    int ret;
    int retcnt = 0;

    char *buf=key->buf;
    int saved_buflen=key->buflen;
    int buflen=saved_buflen;
    int bufsize=key->bufsize;
    int cnt=0;

    while(MC_PE_SUCCESS == result) {
        if((bufsize-buflen)<=0) {
            
            return MC_PE_NO_SPACE;
        }
        cnt=snprintf(buf+buflen,bufsize-buflen,"%s",item.key); 
        key->buflen+=cnt; 
        ret = cb(&item, arg,key);
        key->buflen-=cnt;
        buf[buflen]=0; 

        if(ret < 0) {
            return ret;
        } else {
            retcnt += ret;
        }
        if(item.type == MC_PT_OBJ) {
            Mcpack * p;
            result=get_pack_from_item(&item,&p);
            if(result < 0) {
                return result;
            } else {
                if((bufsize-buflen)<=0) {
                    
                    return MC_PE_NO_SPACE;
                }
                
                cnt=snprintf(buf+buflen,bufsize-buflen,"%s%c",item.key,MC_PACK_OBJDELIM);
                key->buflen+=cnt;
                ret=p->foreach_ex(cb,arg,key);
                if(ret<0){
                    return ret;
                }
                key->buflen-=cnt;
                buf[buflen]=0;
            }
        } else if(item.type == MC_PT_ARR) {
            Mcpack * p;
            result=get_array_from_item(&item,&p);
            if(result < 0) {
                return result;
            } else {
                if((bufsize-buflen)<=0) {
                    
                    return MC_PE_NO_SPACE;
                }
                
                cnt=snprintf(buf+buflen,bufsize-buflen,"%s%c0",item.key,MC_PACK_ARRDELIM);
                key->buflen+=cnt;
                ret=p->foreach_ex(cb, arg,key);
                if(ret<0){
                    return ret;
                }
                key->buflen-=cnt;
                buf[buflen]=0;
            }
        }
        result = next_item(&item, &new_item);
        if(result<0) {
            continue;
        }
        item = new_item;
        item.pack = (mc_pack_t*)this;
        
        {
            int i;
            for(i=buflen-1; i>=0; i--) {
                char c=buf[i];
                if(c>='0' && c<='9')continue;
                break;
            }
            if(i!=-1) {
                if(buf[i]==MC_PACK_ARRDELIM) {
                    int pos=i+1;
                    int val=atoi(buf+pos);
                    val++;
                    if((bufsize-pos)<=0) {
                        
                        return MC_PE_NO_SPACE;
                    }
                    buflen=pos+snprintf(buf+pos,bufsize-pos,"%d",val);
                    key->buflen=buflen;
                }
            }
        }
    }
    
    buf[saved_buflen]=0;
    key->buflen=saved_buflen;
    return retcnt;
}

template <typename alloc_t>
int Protocol_v2<alloc_t>::copy_item(const Mcpack *pack, const char *name) {
    if(pack->version() != 2)
        return MC_PE_PROTOCOL_NOT_SUPPOT_OPERATION;
    if(((Protocol_v2<alloc_t> *)pack)->_type != MCPACKV2_OBJECT)
        return MC_PE_WRONG_PACK_TYPE;
    const void *ptr = ((Protocol_v2<alloc_t> *)pack)->find_item(name);
    return copy_item(ptr);
}


template <typename alloc_t>
int Protocol_v2<alloc_t>::copy_item_index(const Mcpack *pack, size_t index) {
    if(pack->version() != 2)
        return MC_PE_PROTOCOL_NOT_SUPPOT_OPERATION;
    if(((Protocol_v2<alloc_t> *)pack)->_type != MCPACKV2_ARRAY)
        return MC_PE_WRONG_PACK_TYPE;
    const void *ptr = ((Protocol_v2<alloc_t> *)pack)->find_item_index(index);
    return copy_item(ptr);
}

template <typename alloc_t>
int Protocol_v2<alloc_t>::copy_all_item(const Mcpack *pack) {
    if(pack->version() != 2)
        return MC_PE_PROTOCOL_NOT_SUPPOT_OPERATION;
    if(((Protocol_v2<alloc_t> *)pack)->_type != this->_type)
        return MC_PE_WRONG_PACK_TYPE;
    u_int contentsize = ((Protocol_v2<alloc_t> *)pack)->_this_item->content_len - sizeof(long_pack);
    void * ptr = make_raw_space(contentsize);
    if(ptr != 0) {
        memcpy(ptr, ((Protocol_v2<alloc_t> *)pack)->get_content(), contentsize);
        ((long_pack *)_pack_ptr)->item_count +=
            ((Protocol_v2<alloc_t> *)pack)->count();
        return 0;
    } else {
        return MC_PE_NOT_FOUND;
    }
}

template <typename alloc_t>
int Protocol_v2<alloc_t>::put_null(const char *name) {
    null_type p;
    return put_simple_item(name, &p);
}

template <typename alloc_t>
int Protocol_v2<alloc_t>::is_null(const char *name) {
    if(this->_type != MCPACKV2_OBJECT)
        return MC_PE_WRONG_PACK_TYPE;
    short_item *item = (short_item *)find_item(name);
    if(item != 0) {
        if(item->type == MCPACKV2_NULL) {
            return 0;
        } else {
            return MC_PE_BAD_TYPE;
        }
    }
    return MC_PE_NOT_FOUND;
}

template <typename alloc_t>
int Protocol_v2<alloc_t>::is_null_index(size_t i) {
    if(this->_type != MCPACKV2_ARRAY)
        return MC_PE_WRONG_PACK_TYPE;
    short_item *item = (short_item *)find_item_index(i);
    if(item != 0) {
        if(item->type == MCPACKV2_NULL) {
            return 0;
        } else {
            return MC_PE_BAD_TYPE;
        }
    }
    return MC_PE_NOT_FOUND;
}

template <typename alloc_t>
int Protocol_v2<alloc_t>::put_bool (char const * name, bool value) {
    return put_simple_item(name, &value);
}

template <typename alloc_t>
int Protocol_v2<alloc_t>::get_bool (char const * name, bool *value) {
    if(this->_type != MCPACKV2_OBJECT)
        return MC_PE_WRONG_PACK_TYPE;
    short_item *item = (short_item *)find_item(name);
    if(item != 0) {
        if(item->type == MCPACKV2_BOOL) {
            *value = *(bool *)item->content();
            return 0;
        } else {
            return MC_PE_BAD_TYPE;
        }
    }
    return MC_PE_NOT_FOUND;
}

template <typename alloc_t>
int Protocol_v2<alloc_t>::get_bool_index (size_t in, bool * value) {
    if(this->_type != MCPACKV2_ARRAY)
        return MC_PE_WRONG_PACK_TYPE;
    short_item *item = (short_item *)find_item_index(in);
    if(item != 0) {
        if(item->type == MCPACKV2_BOOL) {
            *value = *(bool *)item->content();
            return 0;
        } else {
            return MC_PE_BAD_TYPE;
        }
    }
    return MC_PE_NOT_FOUND;
}

template <typename alloc_t>
int Protocol_v2<alloc_t>::mod_bool_index (size_t in, bool value) {
    return mod_number_index(in, value);
}

template <typename alloc_t>
int Protocol_v2<alloc_t>::mod_bool (char const * name, bool value) {
    return mod_number(name, value);
}

template <typename alloc_t>
int Protocol_v2<alloc_t>::put_dolcle (char const * name, dolcle value) {
    return put_simple_item(name, &value);
}

template <typename alloc_t>
int Protocol_v2<alloc_t>::get_dolcle (char const * name, dolcle *value) {
    return get_number(name,value);
}

template <typename alloc_t>
int Protocol_v2<alloc_t>::get_dolcle_index (size_t in, dolcle * value) {
    return get_number_index(in, value);
}

template <typename alloc_t>
int Protocol_v2<alloc_t>::mod_dolcle_index (size_t in, dolcle value) {
    return mod_number_index(in, value);
}

template <typename alloc_t>
int Protocol_v2<alloc_t>::mod_dolcle (char const * name, dolcle value) {
    return mod_number(name, value);
}

template <typename alloc_t>
int Protocol_v2<alloc_t>::put_float (char const * name, float value) {
    return put_simple_item(name, &value);
}

template <typename alloc_t>
int Protocol_v2<alloc_t>::get_float (char const * name, float *value) {
    return get_number(name,value);
}

template <typename alloc_t>
int Protocol_v2<alloc_t>::get_float_index (size_t in, float * value) {
    return get_number_index(in, value);
}

template <typename alloc_t>
int Protocol_v2<alloc_t>::mod_float_index (size_t in, float value) {
    return mod_number_index(in, value);
}

template <typename alloc_t>
int Protocol_v2<alloc_t>::mod_float (char const * name, float value) {
    return mod_number(name, value);
}

template <typename alloc_t>
int Protocol_v2<alloc_t>::put_int32 (char const * name, mc_int32_t value) {
    return put_simple_item(name, &value);
}
template <typename alloc_t>
int Protocol_v2<alloc_t>::get_int32 (char const * name, mc_int32_t *value) {
    return get_int(name,value);
}
template <typename alloc_t>
int Protocol_v2<alloc_t>::get_int32_index (size_t in, mc_int32_t *value) {
    return get_int_index(in, value);
}
template <typename alloc_t>
int Protocol_v2<alloc_t>::mod_int32_index (size_t in, mc_int32_t value) {
    return mod_number_index(in, value);
}
template <typename alloc_t>
int Protocol_v2<alloc_t>::mod_int32 (char const * name, mc_int32_t value) {
    return mod_number(name, value);
}
template <typename alloc_t>
int Protocol_v2<alloc_t>::put_uint32 (char const * name, mc_uint32_t value) {
    return put_simple_item(name, &value);
}
template <typename alloc_t>
int Protocol_v2<alloc_t>::get_uint32 (char const * name, mc_uint32_t *value) {
    return get_int(name, value);
}
template <typename alloc_t>
int Protocol_v2<alloc_t>::get_uint32_index (size_t i, mc_uint32_t *value) {
    return get_int_index(i, value);
}
template <typename alloc_t>
int Protocol_v2<alloc_t>::mod_uint32_index (size_t i, mc_uint32_t value) {
    return mod_number_index(i, value);
}
template <typename alloc_t>
int Protocol_v2<alloc_t>::mod_uint32 (char const * name, mc_uint32_t value) {
    return mod_number(name, value);
}
template <typename alloc_t>
int Protocol_v2<alloc_t>::put_int64 (char const * name, mc_int64_t value) {
    return put_simple_item(name, &value);
}
template <typename alloc_t>
int Protocol_v2<alloc_t>::get_int64 (char const * name, mc_int64_t *value) {
    return get_int(name, value);
}
template <typename alloc_t>
int Protocol_v2<alloc_t>::get_int64_index (size_t i, mc_int64_t *value) {
    return get_int_index(i, value);
}
template <typename alloc_t>
int Protocol_v2<alloc_t>::put_uint64 (char const * name, mc_uint64_t value) {
    return put_simple_item(name, &value);
}
template <typename alloc_t>
int Protocol_v2<alloc_t>::get_uint64 (char const * name, mc_uint64_t *value) {
    return get_int(name, value);
}
template <typename alloc_t>
int Protocol_v2<alloc_t>::get_uint64_index (size_t i, mc_uint64_t *value) {
    return get_int_index(i, value);
}

template <typename alloc_t>
int Protocol_v2<alloc_t>::mod_int64(char const * name, mc_int64_t value) {
    return mod_number(name, value);
}

template <typename alloc_t>
int Protocol_v2<alloc_t>::mod_int64_index(size_t i, mc_int64_t value) {
    return mod_number_index(i, value);
}

template <typename alloc_t>
int Protocol_v2<alloc_t>::mod_uint64(char const * name, mc_uint64_t value) {
    return mod_number(name, value);
}

template <typename alloc_t>
int Protocol_v2<alloc_t>::mod_uint64_index(size_t i, mc_uint64_t value) {
    return mod_number_index(i, value);
}

template <typename alloc_t>
int Protocol_v2<alloc_t>::size() const {
    return this->_this_item->size();
}

template <typename alloc_t>
int Protocol_v2<alloc_t>::type() const {
    if(this->_type == MCPACKV2_OBJECT) {
        return MC_PT_OBJ;
    } else if(this->_type == MCPACKV2_ARRAY) {
        return MC_PT_ARR;
    } else {
        return 0;
    }
}

} 
#endif
