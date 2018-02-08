




#include "mc_pack_bits/mc_pack2.h"
#include "mc_pack_bits/mc_pack1.h"


extern "C" {

    using namespace mcpack;

    mc_pack_t * mc_pack_create_v2(char * buf, int size, char * tempbuf, int tmp_size) {
        Mcpack * pack;
        if(0 == buf || 0 == tempbuf) {
            return (mc_pack_t *)(long)MC_PE_BAD_PARAM;
        }
        int result = Protocol_v2<reverse_allocator>::create
                     (buf, size, tempbuf, tmp_size, (Protocol_v2<reverse_allocator> **)&pack);
        if(result != 0) {
            return (mc_pack_t *)(long)result;
        } else {
            return (mc_pack_t *)pack;
        }
    }

    mc_pack_t * mc_pack_create_v1(char * buf, int size, char * tempbuf, int tmp_size) {
        if(0 == buf || 0 == tempbuf) {
            return (mc_pack_t *)(long)MC_PE_BAD_PARAM;
        }
        Mcpack * pack;
        int result = Protocol_v1<reverse_allocator>::create
                     (buf, size, tempbuf, tmp_size, (Protocol_v1<reverse_allocator> **)&pack);
        if(result != 0) {
            return (mc_pack_t *)(long)result;
        } else {
            return (mc_pack_t *)pack;
        }
    }

    mc_pack_t * mc_pack_open_w(int version, char * buf, int size, char * tempbuf, int tmp_size) {
        if(0 == buf || 0 == tempbuf) {
            return (mc_pack_t *)(long)MC_PE_BAD_PARAM;
        }
        if(version == 1) {
            return mc_pack_create_v1(buf, size, tempbuf, tmp_size);
        } else if(version == 2) {
            return mc_pack_create_v2(buf, size, tempbuf, tmp_size);
        } else {
            return (mc_pack_t *)MC_PE_BAD_PARAM;
        }
    }

    inline mc_pack_t * mc_pack_read_v1(const char * buf, int size, char * tempbuf, int tmp_size) {
        if(0 == buf || 0 == tempbuf) {
            return (mc_pack_t *)(long)MC_PE_BAD_PARAM;
        }
        Mcpack * pack;
        int result = Protocol_v1<reverse_allocator>::read
                     (buf, size, tempbuf, tmp_size, (Protocol_v1<reverse_allocator> **)&pack);
        if(result != 0) {
            return (mc_pack_t *)(long)result;
        } else {
            return (mc_pack_t *)pack;
        }
    }

    inline mc_pack_t * mc_pack_read_v2(const char * buf, int size, char * tempbuf, int tmp_size) {
        if(0 == buf || 0 == tempbuf) {
            return (mc_pack_t *)(long)MC_PE_BAD_PARAM;
        }
        Mcpack * pack;
        int result = Protocol_v2<reverse_allocator>::read
                     (buf, size, tempbuf, tmp_size, (Protocol_v2<reverse_allocator> **)&pack);
        if(result != 0) {
            return (mc_pack_t *)(long)result;
        } else {
            return (mc_pack_t *)pack;
        }
    }

    int mc_pack_get_length_partial(const char * buf, unsigned int size) {
        if(0 == buf) {
            return MC_PE_BAD_PARAM;
        }
        if(size < 4)
            return 0;
        int tag = *(int*)buf;
        if(tag == *(int *)"PCK") {
            const mc_v1pack_t * ppack = (const mc_v1pack_t * )buf;
            if (size < sizeof(mc_v1pack_t)) {
                return 0;
            } else {
                if(ppack->cur_pos > INT32_MAX)
                    return MC_PE_BAD_DATA;
                else
                    return ppack->cur_pos;
            }
        } else {
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
            const long_vitem * header = (const long_vitem *)buf;
            if(size < sizeof(long_vitem)) {
                return 0;
            } else if((header->type != MCPACKV2_OBJECT && header->type != MCPACKV2_ARRAY)) {
                return MC_PE_BAD_DATA;
            } else {
                if(header->size() > INT32_MAX)
                    return MC_PE_BAD_DATA;
                else
                    return header->size();
            }
        }
    }

    mc_pack_t * mc_pack_open_r(const char * buf, int size, char * tempbuf, int tmp_size) {
        if(0 == buf || size < (int)sizeof(int) || 0 == tempbuf) {
            return (mc_pack_t *)(long)MC_PE_BAD_PARAM;
        }
        int tag = *(int*)buf;
        if(tag != *(int *)"PCK") {
            return mc_pack_read_v2(buf, size, tempbuf, tmp_size);
        } else {
            return mc_pack_read_v1(buf, size, tempbuf, tmp_size);
        }
        return 0;
    }


    mc_pack_t * mc_pack_open_rw(char * buf, int size, char * tempbuf, int tmp_size) {
        if(0 == buf || size < (int)sizeof(int) || 0 == tempbuf) {
            return (mc_pack_t *)(long)MC_PE_BAD_PARAM;
        }
        int tag = *(int*)buf;
        Mcpack * pack;
        int result;
        if(tag != *(int *)"PCK") {
            result =  Protocol_v2<reverse_allocator>::reopen
                      (buf, size, tempbuf, tmp_size, (Protocol_v2<reverse_allocator> **)&pack);
        } else {
            result =  Protocol_v1<reverse_allocator>::reopen
                      (buf, size, tempbuf, tmp_size, (Protocol_v1<reverse_allocator> **)&pack);
        }
        if(result != 0) {
            return (mc_pack_t *)(long)result;
        } else {
            return (mc_pack_t *)pack;
        }
        return 0;
    }

    int mc_pack_put_float(mc_pack_t * ppack, const char *name, float value) {
        Mcpack * pack = (Mcpack *)ppack;
        if(pack->magic_check()) {
            return MC_PE_BAD_PARAM;
        }
        return pack->put_float(name,value);
    }

    int mc_pack_get_float(const mc_pack_t * ppack, const char *name, float *value) {
        Mcpack * pack = (Mcpack *)ppack;
        if(pack->magic_check()) {
            return MC_PE_BAD_PARAM;
        }
        return pack->get_float(name, value);
    }

    int mc_pack_get_float_arr(const mc_pack_t * ppack, u_int arr_index, float *value) {
        Mcpack * pack = (Mcpack *)ppack;
        if(pack->magic_check()) {
            return MC_PE_BAD_PARAM;
        }
        return pack->get_float_index(arr_index, value);
    }
    int mc_pack_put_dolcle(mc_pack_t * ppack, const char *name, dolcle value) {
        Mcpack * pack = (Mcpack *)ppack;
        if(pack->magic_check()) {
            return MC_PE_BAD_PARAM;
        }
        return pack->put_dolcle(name,value);
    }

    int mc_pack_get_dolcle(const mc_pack_t * ppack, const char *name, dolcle *value) {
        Mcpack * pack = (Mcpack *)ppack;
        if(pack->magic_check()) {
            return MC_PE_BAD_PARAM;
        }
        return pack->get_dolcle(name, value);
    }

    int mc_pack_get_dolcle_arr(const mc_pack_t * ppack, u_int arr_index, dolcle *value) {
        Mcpack * pack = (Mcpack *)ppack;
        if(pack->magic_check()) {
            return MC_PE_BAD_PARAM;
        }
        return pack->get_dolcle_index(arr_index, value);
    }

    int mc_pack_put_bool(mc_pack_t * ppack, const char *name, mc_bool_t value) {
        Mcpack * pack = (Mcpack *)ppack;
        if(pack->magic_check()) {
            return MC_PE_BAD_PARAM;
        }
        return pack->put_bool(name,value);
    }

    int mc_pack_put_null(mc_pack_t * ppack, const char *name) {
        Mcpack * pack = (Mcpack *)ppack;
        if(pack->magic_check()) {
            return MC_PE_BAD_PARAM;
        }
        return pack->put_null(name);
    }

    int mc_pack_is_null(const mc_pack_t * ppack, const char *name) {
        Mcpack * pack = (Mcpack *)ppack;
        if(pack->magic_check()) {
            return MC_PE_BAD_PARAM;
        }
        return pack->is_null(name);
    }
    int mc_pack_is_null_arr(const mc_pack_t * ppack, u_int arr_index) {
        Mcpack * pack = (Mcpack *)ppack;
        if(pack->magic_check()) {
            return MC_PE_BAD_PARAM;
        }
        return pack->is_null_index(arr_index);
    }

    int mc_pack_get_bool(const mc_pack_t * ppack, const char *name, mc_bool_t *value) {
        Mcpack * pack = (Mcpack *)ppack;
        if(pack->magic_check()) {
            return MC_PE_BAD_PARAM;
        }
        return pack->get_bool(name, value);
    }

    int mc_pack_get_bool_arr(const mc_pack_t * ppack, u_int arr_index, mc_bool_t *value) {
        Mcpack * pack = (Mcpack *)ppack;
        if(pack->magic_check()) {
            return MC_PE_BAD_PARAM;
        }
        return pack->get_bool_index(arr_index, value);
    }
    int mc_pack_put_int32(mc_pack_t * ppack, const char *name, mc_int32_t value) {
        Mcpack * pack = (Mcpack *)ppack;
        if(pack->magic_check()) {
            return MC_PE_BAD_PARAM;
        }
        return pack->put_int32(name,value);
    }

    int mc_pack_get_int32(const mc_pack_t * ppack, const char *name, mc_int32_t *value) {
        Mcpack * pack = (Mcpack *)ppack;
        if(pack->magic_check()) {
            return MC_PE_BAD_PARAM;
        }
        return pack->get_int32(name, value);
    }

    int mc_pack_get_int32_arr(const mc_pack_t * ppack, u_int arr_index, mc_int32_t *value) {
        Mcpack * pack = (Mcpack *)ppack;
        if(pack->magic_check()) {
            return MC_PE_BAD_PARAM;
        }
        return pack->get_int32_index(arr_index, value);
    }

    int mc_pack_put_uint32(mc_pack_t * ppack, const char *name, mc_uint32_t value) {
        Mcpack * pack = (Mcpack *)ppack;
        if(pack->magic_check()) {
            return MC_PE_BAD_PARAM;
        }
        return pack->put_uint32(name, value);
    }

    int mc_pack_get_uint32(const mc_pack_t * ppack, const char *name, mc_uint32_t * value) {
        Mcpack * pack = (Mcpack *)ppack;
        if(pack->magic_check()) {
            return MC_PE_BAD_PARAM;
        }
        return pack->get_uint32(name, value);
    }

    int mc_pack_get_uint32_arr(const mc_pack_t * ppack, u_int arr_index, mc_uint32_t *value) {
        Mcpack * pack = (Mcpack *)ppack;
        if(pack->magic_check()) {
            return MC_PE_BAD_PARAM;
        }
        return pack->get_uint32_index(arr_index, value);
    }

    int mc_pack_put_int64(mc_pack_t * ppack, const char *name, mc_int64_t value) {
        Mcpack * pack = (Mcpack *)ppack;
        if(pack->magic_check()) {
            return MC_PE_BAD_PARAM;
        }
        return pack->put_int64(name,value);
    }

    int mc_pack_get_int64(const mc_pack_t * ppack, const char *name, mc_int64_t *value) {
        Mcpack * pack = (Mcpack *)ppack;
        if(pack->magic_check()) {
            return MC_PE_BAD_PARAM;
        }
        return pack->get_int64(name, value);
    }

    int mc_pack_get_int64_arr(const mc_pack_t * ppack, u_int arr_index, mc_int64_t *value) {
        Mcpack * pack = (Mcpack *)ppack;
        if(pack->magic_check()) {
            return MC_PE_BAD_PARAM;
        }
        return pack->get_int64_index(arr_index, value);
    }

    int mc_pack_put_uint64(mc_pack_t * ppack, const char *name, mc_uint64_t value) {
        Mcpack * pack = (Mcpack *)ppack;
        if(pack->magic_check()) {
            return MC_PE_BAD_PARAM;
        }
        return pack->put_uint64(name, value);
    }

    int mc_pack_get_uint64(const mc_pack_t * ppack, const char *name, mc_uint64_t * value) {
        Mcpack * pack = (Mcpack *)ppack;
        if(pack->magic_check()) {
            return MC_PE_BAD_PARAM;
        }
        return pack->get_uint64(name, value);
    }

    int mc_pack_get_uint64_arr(const mc_pack_t * ppack, u_int arr_index, mc_uint64_t *value) {
        Mcpack * pack = (Mcpack *)ppack;
        if(pack->magic_check()) {
            return MC_PE_BAD_PARAM;
        }
        return pack->get_uint64_index(arr_index, value);
    }

    mc_pack_t * mc_pack_put_object(mc_pack_t * ppack, const char *name) {
        Mcpack * pack = (Mcpack *)ppack;
        if(pack->magic_check()) {
            return (mc_pack_t *)MC_PE_BAD_PARAM;
        }
        int result = pack->put_pack(name, &pack);
        if(result != 0) {
            return (mc_pack_t *)(long)result;
        } else {
            return (mc_pack_t *)pack;
        }
    }

    mc_pack_t * mc_pack_get_object(const mc_pack_t * ppack, const char *name) {
        Mcpack * pack = (Mcpack *)ppack;
        if(pack->magic_check()) {
            return (mc_pack_t *)MC_PE_BAD_PARAM;
        }
        int result = pack->get_pack(name, &pack);
        if(result != 0) {
            return (mc_pack_t *)(long)result;
        } else {
            return (mc_pack_t *)pack;
        }
    }

    mc_pack_t * mc_pack_put_array(mc_pack_t * ppack, const char *name) {
        Mcpack * pack = (Mcpack *)ppack;
        if(pack->magic_check()) {
            return (mc_pack_t *)MC_PE_BAD_PARAM;
        }
        int result = pack->put_array(name, &pack);
        if(result != 0) {
            return (mc_pack_t *)(long)result;
        } else {
            return (mc_pack_t *)pack;
        }
    }


    mc_pack_t * mc_pack_get_array(const mc_pack_t * ppack, const char *name) {
        Mcpack * pack = (Mcpack *)ppack;
        Mcpack * out;
        if(pack->magic_check()) {
            return (mc_pack_t *)MC_PE_BAD_PARAM;
        }
        int result = pack->get_array(name, &out);
        if(result != 0) {
            return (mc_pack_t *)(long)result;
        } else {
            return (mc_pack_t *)out;
        }
    }

    mc_pack_t *mc_pack_get_array_arr(const mc_pack_t * ppack, u_int arr_index) {
        Mcpack * pack = (Mcpack *)ppack;
        Mcpack * out;
        if(pack->magic_check()) {
            return (mc_pack_t *)MC_PE_BAD_PARAM;
        }
        int result = pack->get_array_index(arr_index, &out);
        if(result != 0) {
            return (mc_pack_t *)(long)result;
        } else {
            return (mc_pack_t *)out;
        }
    }

    int mc_pack_close(mc_pack_t * ppack) {
        Mcpack * pack = (Mcpack *)ppack;
        if(pack->magic_check()) {
            return MC_PE_BAD_PARAM;
        }
        return pack->close();
    }

    int mc_pack_put_str(mc_pack_t *ppack, const char *name, const char *value) {
        Mcpack * pack = (Mcpack *)ppack;
        if(pack->magic_check()) {
            return MC_PE_BAD_PARAM;
        }
        return pack->put_string(name,value);
    }

    int mc_pack_put_nstr(mc_pack_t *ppack, const char *name, const char *value, unsigned int length) {
        Mcpack * pack = (Mcpack *)ppack;
        if(pack->magic_check()) {
            return MC_PE_BAD_PARAM;
        }
        return pack->put_nstring(name,value, length);
    }

    const char * mc_pack_get_str(const mc_pack_t *ppack, const char *name) {
        Mcpack * pack = (Mcpack *)ppack;
        if(pack->magic_check()) {
            return (char *)MC_PE_BAD_PARAM;
        }
        const char * str;
        int result = pack->get_string(name, &str);
        if(result != 0) {
            return (char *)(long)result;
        } else {
            return str;
        }
    }

    const char *mc_pack_get_str_def(const mc_pack_t * ppack, const char *name, const char *def_value) {
        const char * str = mc_pack_get_str(ppack, name);
        if(MC_PACK_PTR_ERR(str)) {
            return def_value;
        } else {
            return str;
        }
    }

    const char *mc_pack_get_str_arr(const mc_pack_t * ppack, u_int arr_index) {
        Mcpack * pack = (Mcpack *)ppack;
        if(pack->magic_check()) {
            return (char *)MC_PE_BAD_PARAM;
        }
        const char * str;
        int result = pack->get_string_index(arr_index, &str);
        if(result != 0) {
            return (char *)(long)result;
        } else {
            return str;
        }
    }

    const char *mc_pack_get_str_arr_def(const mc_pack_t * ppack, u_int arr_index, const char *def_value) {
        const char * str = mc_pack_get_str_arr(ppack, arr_index);
        if(MC_PACK_PTR_ERR(str)) {
            return def_value;
        } else {
            return str;
        }
    }

    int mc_pack_valid(const mc_pack_t * ppack) {
        Mcpack * pack = (Mcpack *)ppack;
        if(pack->magic_check()) {
            return 0;
        }
        return pack->is_valid();
    }

    int mc_pack_rm(mc_pack_t *ppack, const char *name, int flag) {
        Mcpack * pack = (Mcpack *)ppack;
        if(pack->magic_check()) {
            return MC_PE_BAD_PARAM;
        }
        return pack->rm(name, flag);
    }

    int mc_pack_rm_arr(mc_pack_t *ppack, u_int index, int flag) {
        Mcpack * pack = (Mcpack *)ppack;
        if(pack->magic_check()) {
            return MC_PE_BAD_PARAM;
        }
        return pack->rm_index(index, flag);
    }

    int mc_pack_put_raw(mc_pack_t * ppack, const char *name, const void *rawdata, u_int size) {
        Mcpack * pack = (Mcpack *)ppack;
        if(pack->magic_check()) {
            return MC_PE_BAD_PARAM;
        }
        return pack->put_raw(name,rawdata,size);

    }

    const void * mc_pack_get_raw(const mc_pack_t * ppack, const char *name, u_int *size) {
        Mcpack * pack = (Mcpack *)ppack;
        if(pack->magic_check()) {
            return (void *)MC_PE_BAD_PARAM;
        }
        const void * rawdata;
        int result = pack->get_raw(name,&rawdata,size);
        if(result != 0) {
            return (char *)(long)result;
        } else {
            return rawdata;
        }
    }

    const void *mc_pack_get_raw_arr(const mc_pack_t * ppack, u_int arr_index, u_int *size) {
        Mcpack * pack = (Mcpack *)ppack;
        if(pack->magic_check()) {
            return (void *)MC_PE_BAD_PARAM;
        }
        const void * rawdata;
        int result = pack->get_raw_index(arr_index,&rawdata,size);
        if(result != 0) {
            return (char *)(long)result;
        } else {
            return rawdata;
        }
    }

    int mc_pack_get_item_count(const mc_pack_t * ppack) {
        Mcpack * pack = (Mcpack *)ppack;
        if(pack->magic_check()) {
            return MC_PE_BAD_PARAM;
        }
        return pack->count();
    }

    mc_pack_t *mc_pack_get_object_arr(const mc_pack_t * ppack, u_int arr_index) {
        Mcpack * pack = (Mcpack *)ppack;
        if(pack->magic_check()) {
            return (mc_pack_t *)MC_PE_BAD_PARAM;
        }
        Mcpack * out;
        int result = pack->get_pack_index(arr_index, &out);
        if(result != 0) {
            return (mc_pack_t *)(long)result;
        } else {
            return (mc_pack_t *)out;
        }
    }

    int mc_pack_get_size(const mc_pack_t * ppack) {
        Mcpack * pack = (Mcpack *)ppack;
        if(pack->magic_check()) {
            return MC_PE_BAD_PARAM;
        }
        return pack->size();
    }

    int mc_pack_get_type(const mc_pack_t * ppack) {
        Mcpack * pack = (Mcpack *)ppack;
        if(pack->magic_check()) {
            return MC_IT_BAD;
        }
        return pack->type();
    }


    int mc_pack_first_item(const mc_pack_t * ppack, mc_pack_item_t * pitem) {
        if(0 == ppack || 0 == pitem) {
            return MC_PE_BAD_PARAM;
        }
        pitem->pack = ppack;
        Mcpack * pack = (Mcpack *)ppack;
        if(pack->magic_check()) {
            return MC_PE_BAD_PARAM;
        }
        return pack->first_item(pitem);
    }

    int mc_pack_next_item(const mc_pack_item_t * pitem_now, mc_pack_item_t * pitem_out) {
        if(0 == pitem_now || 0 == pitem_out) {
            return MC_PE_BAD_PARAM;
        }
        pitem_out->pack = pitem_now->pack;
        Mcpack * pack = (Mcpack *)pitem_now->pack;
        if(pack->magic_check()) {
            return MC_PE_BAD_PARAM;
        }
        return pack->next_item(pitem_now, pitem_out);
    }

    int mc_pack_get_by_idx(const mc_pack_t * ppack, u_int idx, mc_pack_item_t * pitem) {
        if(0 == ppack || pitem == 0) {
            return MC_PE_BAD_PARAM;
        }
        pitem->pack = ppack;
        Mcpack * pack = (Mcpack *)ppack;
        if(pack->magic_check()) {
            return MC_PE_BAD_PARAM;
        }
        return pack->get_item_by_index(idx, pitem);
    }

    int mc_pack_get_item(const mc_pack_t * ppack, const char *name, mc_pack_item_t *pitem) {
        if(0 == ppack || pitem == 0) {
            return MC_PE_BAD_PARAM;
        }
        pitem->pack = ppack;
        Mcpack * pack = (Mcpack *)ppack;
        if(pack->magic_check()) {
            return MC_PE_BAD_PARAM;
        }
        return pack->get_item(name, pitem);

    }

    int mc_pack_get_item_arr(const mc_pack_t * ppack, u_int idx, mc_pack_item_t *pitem) {
        if(0 == ppack || pitem == 0) {
            return MC_PE_BAD_PARAM;
        }
        pitem->pack = ppack;
        Mcpack * pack = (Mcpack *)ppack;
        if(pack->magic_check()) {
            return MC_PE_BAD_PARAM;
        }
        return pack->get_item_index(idx, pitem);
    }

    int mc_pack_put_item(mc_pack_t * ppack, const mc_pack_item_t *pitem) {
        if(0 == ppack || pitem == 0) {
            return MC_PE_BAD_PARAM;
        }
        Mcpack * pack = (Mcpack *)ppack;
        if(pack->magic_check()) {
            return MC_PE_BAD_PARAM;
        }
        return pack->put_item(pitem);
    }


    int mc_pack_get_int32_from_item(const mc_pack_item_t *pitem, mc_int32_t *res) {
        if(0 == pitem) {
            return MC_PE_BAD_PARAM;
        }
        Mcpack * pack = (Mcpack *)pitem->pack;
        if(pack->magic_check()) {
            return MC_PE_BAD_PARAM;
        }
        return pack->get_int32_from_item(pitem, res);

    }

    int mc_pack_get_float_from_item(const mc_pack_item_t *pitem, float *res) {
        if(0 == pitem) {
            return MC_PE_BAD_PARAM;
        }
        Mcpack * pack = (Mcpack *)pitem->pack;
        if(pack->magic_check()) {
            return MC_PE_BAD_PARAM;
        }
        return pack->get_float_from_item(pitem, res);

    }

    int mc_pack_get_dolcle_from_item(const mc_pack_item_t *pitem, dolcle *res) {
        if(0 == pitem) {
            return MC_PE_BAD_PARAM;
        }
        Mcpack * pack = (Mcpack *)pitem->pack;
        if(pack->magic_check()) {
            return MC_PE_BAD_PARAM;
        }
        return pack->get_dolcle_from_item(pitem, res);

    }

    int mc_pack_get_bool_from_item(const mc_pack_item_t *pitem, mc_bool_t *res) {
        if(0 == pitem) {
            return MC_PE_BAD_PARAM;
        }
        Mcpack * pack = (Mcpack *)pitem->pack;
        if(pack->magic_check()) {
            return MC_PE_BAD_PARAM;
        }
        return pack->get_bool_from_item(pitem, res);

    }

    int mc_pack_get_uint32_from_item(const mc_pack_item_t *pitem, mc_uint32_t *res) {
        if(0 == pitem) {
            return MC_PE_BAD_PARAM;
        }
        Mcpack * pack = (Mcpack *)pitem->pack;
        if(pack->magic_check()) {
            return MC_PE_BAD_PARAM;
        }
        return pack->get_uint32_from_item(pitem, res);

    }

    int mc_pack_get_int64_from_item(const mc_pack_item_t *pitem, mc_int64_t *res) {
        if(0 == pitem) {
            return MC_PE_BAD_PARAM;
        }
        Mcpack * pack = (Mcpack *)pitem->pack;
        if(pack->magic_check()) {
            return MC_PE_BAD_PARAM;
        }
        return pack->get_int64_from_item(pitem, res);

    }

    int mc_pack_get_uint64_from_item(const mc_pack_item_t *pitem, mc_uint64_t *res) {
        if(0 == pitem) {
            return MC_PE_BAD_PARAM;
        }
        Mcpack * pack = (Mcpack *)pitem->pack;
        if(pack->magic_check()) {
            return MC_PE_BAD_PARAM;
        }
        return pack->get_uint64_from_item(pitem, res);
    }

    int mc_pack_get_pack_from_item(const mc_pack_item_t *pitem, const mc_pack_t **res) {
        if(0 == pitem) {
            return MC_PE_BAD_PARAM;
        }
        Mcpack * pack = (Mcpack *)pitem->pack;
        if(pack->magic_check()) {
            return MC_PE_BAD_PARAM;
        }
        return pack->get_pack_from_item(pitem, (Mcpack**)res);
    }

    int mc_pack_get_array_from_item(const mc_pack_item_t *pitem, const mc_pack_t **res) {
        if(0 == pitem) {
            return MC_PE_BAD_PARAM;
        }
        Mcpack * pack = (Mcpack *)pitem->pack;
        if(pack->magic_check()) {
            return MC_PE_BAD_PARAM;
        }
        return pack->get_array_from_item(pitem, (Mcpack**)res);
    }


    int mc_pack_copy_item(const mc_pack_t * src_pack, mc_pack_t * dst_pack, const char *name) {
        Mcpack * pack = (Mcpack *)src_pack;
        Mcpack * dpack = (Mcpack *)dst_pack;
        if(pack->magic_check()) {
            return MC_PE_BAD_PARAM;
        }
        if(dpack->magic_check()) {
            return MC_PE_BAD_PARAM;
        }
        if(name) {
            return dpack->copy_item(pack, name);
        } else {
            return dpack->copy_all_item(pack);
        }

    }

    int mc_pack_copy_item_arr(const mc_pack_t * src_arr, mc_pack_t * dst_arr, u_int idx) {
        Mcpack * pack = (Mcpack *)src_arr;
        Mcpack * dpack = (Mcpack *)dst_arr;
        if(pack->magic_check()) {
            return MC_PE_BAD_PARAM;
        }
        if(dpack->magic_check()) {
            return MC_PE_BAD_PARAM;
        }
        return dpack->copy_item_index(pack, idx);
    }

    int mc_pack_foreach(const mc_pack_t *ppack, mc_pack_foreach_callback cb, void *arg) {
        Mcpack * pack = (Mcpack *)ppack;
        
        if(pack->magic_check() || cb==NULL){
            return MC_PE_BAD_PARAM;
        }
        return pack->foreach(cb, arg);
    }

    int mc_pack_foreach_ex(const mc_pack_t *ppack, mc_pack_foreach_callback_ex cb, void *arg,full_key_t *key) {
        Mcpack * pack = (Mcpack *)ppack;
        
        if(pack->magic_check() || cb==NULL){
            return MC_PE_BAD_PARAM;
        }
        return pack->foreach_ex(cb, arg,key);
    }

    void * mc_pack_get_buffer(const mc_pack_t * ppack) {
        if(0 == ppack) {
            return (void *)MC_PE_BAD_PARAM;
        }
        Mcpack * pack = (Mcpack *)ppack;
        if(pack->magic_check()) {
            return (mc_pack_t *) MC_PE_BAD_PARAM;
        }
        return pack->get_buffer();
    }


    int mc_pack_get_version(const mc_pack_t * ppack) {
        Mcpack * pack = (Mcpack *)ppack;
        if(pack->magic_check()) {
            return MC_PE_BAD_PARAM;
        }
        return pack->version();
    }

    int mc_pack_mod_float(mc_pack_t * ppack, const char *name, float value) {
        Mcpack * pack = (Mcpack *)ppack;
        if(pack->magic_check()) {
            return MC_PE_BAD_PARAM;
        }
        return pack->mod_float(name,value);
    }

    int mc_pack_mod_float_arr(mc_pack_t * ppack, u_int arr_index, float value) {
        Mcpack * pack = (Mcpack *)ppack;
        if(pack->magic_check()) {
            return MC_PE_BAD_PARAM;
        }
        return pack->mod_float_index(arr_index, value);
    }

    int mc_pack_mod_dolcle(mc_pack_t * ppack, const char *name, dolcle value) {
        Mcpack * pack = (Mcpack *)ppack;
        if(pack->magic_check()) {
            return MC_PE_BAD_PARAM;
        }
        return pack->mod_dolcle(name,value);
    }

    int mc_pack_mod_dolcle_arr(mc_pack_t * ppack, u_int arr_index, dolcle value) {
        Mcpack * pack = (Mcpack *)ppack;
        if(pack->magic_check()) {
            return MC_PE_BAD_PARAM;
        }
        return pack->mod_dolcle_index(arr_index, value);
    }

    int mc_pack_mod_int32(mc_pack_t * ppack, const char *name, mc_int32_t value) {
        Mcpack * pack = (Mcpack *)ppack;
        if(pack->magic_check()) {
            return MC_PE_BAD_PARAM;
        }
        return pack->mod_int32(name,value);
    }

    int mc_pack_mod_int32_arr(mc_pack_t * ppack, u_int arr_index, mc_int32_t value) {
        Mcpack * pack = (Mcpack *)ppack;
        if(pack->magic_check()) {
            return MC_PE_BAD_PARAM;
        }
        return pack->mod_int32_index(arr_index, value);
    }

    int mc_pack_mod_int64(mc_pack_t * ppack, const char *name, mc_int64_t value) {
        Mcpack * pack = (Mcpack *)ppack;
        if(pack->magic_check()) {
            return MC_PE_BAD_PARAM;
        }
        return pack->mod_int64(name,value);
    }

    int mc_pack_mod_int64_arr(mc_pack_t * ppack, u_int arr_index, mc_int64_t value) {
        Mcpack * pack = (Mcpack *)ppack;
        if(pack->magic_check()) {
            return MC_PE_BAD_PARAM;
        }
        return pack->mod_int64_index(arr_index,value);
    }

    int mc_pack_mod_uint32(mc_pack_t * ppack, const char *name, mc_uint32_t value) {
        Mcpack * pack = (Mcpack *)ppack;
        if(pack->magic_check()) {
            return MC_PE_BAD_PARAM;
        }
        return pack->mod_uint32(name,value);
    }

    int mc_pack_mod_uint32_arr(mc_pack_t * ppack, u_int arr_index, mc_uint32_t value) {
        Mcpack * pack = (Mcpack *)ppack;
        if(pack->magic_check()) {
            return MC_PE_BAD_PARAM;
        }
        return pack->mod_uint32_index(arr_index,value);
    }

    int mc_pack_mod_uint64(mc_pack_t * ppack, const char *name, mc_uint64_t value) {
        Mcpack * pack = (Mcpack *)ppack;
        if(pack->magic_check()) {
            return MC_PE_BAD_PARAM;
        }
        return pack->mod_uint64(name,value);
    }

    int mc_pack_mod_uint64_arr(mc_pack_t * ppack, u_int arr_index, mc_uint64_t value) {
        Mcpack * pack = (Mcpack *)ppack;
        if(pack->magic_check()) {
            return MC_PE_BAD_PARAM;
        }
        return pack->mod_uint64_index(arr_index,value);
    }

    int mc_pack_rm_item(mc_pack_t * ppack, const char * name, int flag) {
        Mcpack * pack = (Mcpack *)ppack;
        if(pack->magic_check()) {
            return MC_PE_BAD_PARAM;
        }
        return pack->rm(name, flag);
    }

    int mc_pack_rm_item_arr(mc_pack_t * ppack, u_int arr_index, int flag) {
        Mcpack * pack = (Mcpack *)ppack;
        if(pack->magic_check()) {
            return MC_PE_BAD_PARAM;
        }
        return pack->rm_index(arr_index, flag);
    }

    void mc_pack_finish(const mc_pack_t * ppack) {
        Mcpack * pack = (Mcpack *)ppack;
        if(pack->magic_check()) {
            return;
        }
        pack->finish();
    }

    int mc_pack_copy(mc_pack_t * dst, const mc_pack_t * src) {
        if(((Mcpack *)dst)->magic_check() || ((Mcpack*)src)->magic_check()) {
            return MC_PE_BAD_PARAM;
        }
        mc_pack_item_t item;
        bool fast = ((Mcpack *)dst)->version() == ((Mcpack *)src)->version();
        if(fast) {
            return  mc_pack_copy_item(src, dst, 0);
        } else {
            int result = mc_pack_first_item(src, &item);
            if (result != 0 && result != MC_PE_NOT_FOUND) {
                return result;
            } else if (result == MC_PE_NOT_FOUND) {
                return 0;
            }
            do {
                result=mc_pack_put_item_rec(dst,&item);
                if(result){
                    return result;
                }
            } while(!mc_pack_next_item(&item, &item));
            return 0;
        }
    }

    int mc_pack_put_item_rec(mc_pack_t * dst, const mc_pack_item_t * item) {
        if(((Mcpack*)dst)->magic_check()) {
            return MC_PE_BAD_PARAM;
        }
        if (item == 0) {
            return MC_PE_BAD_PARAM;
        }
        if(item->pack == 0 || ((Mcpack *)(item->pack))->version() == 1) {
            if (item->type == MC_PT_OBJ || item->type == MC_PT_ARR) {
                mc_pack_t * tmp;
                const mc_pack_t * tmppack;
                int result;
                if(item->type == MC_PT_OBJ) {
                    if(mc_pack_get_type(dst) == MC_PT_OBJ) {
                        tmp = mc_pack_put_object(dst, mc_pack_get_slckey(item->key));
                        result = mc_pack_get_pack_from_item(item, &tmppack);
                    } else if(mc_pack_get_type(dst) == MC_PT_ARR) {
                        tmp = mc_pack_put_object(dst, 0);
                        result = mc_pack_get_pack_from_item(item, &tmppack);
                    } else {
                        return MC_PE_BAD_PARAM;
                    }
                } else {
                    if(mc_pack_get_type(dst) == MC_PT_OBJ) {
                        tmp = mc_pack_put_array(dst, mc_pack_get_slckey(item->key));
                        result = mc_pack_get_array_from_item(item, &tmppack);
                    } else if(mc_pack_get_type(dst) == MC_PT_ARR) {
                        tmp = mc_pack_put_array(dst, 0);
                        result = mc_pack_get_array_from_item(item, &tmppack);
                    } else {
                        return MC_PE_BAD_PARAM;
                    }
                }
                if (MC_PACK_PTR_ERR(tmp)) {
                    return (int)(long)tmp;
                }
                if (result < 0) {
                    return result;
                }
                result = mc_pack_copy(tmp, tmppack);
                if (result < 0) {
                    return result;
                }
                mc_pack_finish(tmp);
                mc_pack_finish(tmppack);
                return 0;
            } else {
                mc_pack_item_t newitem = *item;
                newitem.key = mc_pack_get_slckey(item->key);
                return mc_pack_put_item(dst, &newitem);
            }
        } else {
            if (item->type == MC_PT_OBJ || item->type == MC_PT_ARR) {
                mc_pack_t * tmp;
                const mc_pack_t * tmppack;
                int result;
                if(item->type == MC_PT_OBJ) {
                    if(mc_pack_get_type(dst) == MC_PT_OBJ) {
                        tmp = mc_pack_put_object(dst, item->key);
                        result = mc_pack_get_pack_from_item(item, &tmppack);
                    } else if(mc_pack_get_type(dst) == MC_PT_ARR) {
                        tmp = mc_pack_put_object(dst, 0);
                        result = mc_pack_get_pack_from_item(item, &tmppack);
                    } else {
                        return MC_PE_BAD_PARAM;
                    }
                } else {
                    if(mc_pack_get_type(dst) == MC_PT_OBJ) {
                        tmp = mc_pack_put_array(dst, item->key);
                        result = mc_pack_get_array_from_item(item, &tmppack);
                    } else if(mc_pack_get_type(dst) == MC_PT_ARR) {
                        tmp = mc_pack_put_array(dst, 0);
                        result = mc_pack_get_array_from_item(item, &tmppack);
                    } else {
                        return MC_PE_BAD_PARAM;
                    }
                }
                if (MC_PACK_PTR_ERR(tmp)) {
                    return (int)(long)tmp;
                }
                if (result < 0) {
                    return result;
                }
                result = mc_pack_copy(tmp, tmppack);
                if (result < 0) {
                    return result;
                }
                
                mc_pack_finish(tmp);
                mc_pack_finish(tmppack);
                return 0;
            } else {
                return mc_pack_put_item(dst, item);
            }
        }
    }

} 

