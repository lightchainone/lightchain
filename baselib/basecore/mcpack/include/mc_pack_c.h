

#ifndef  __MC_PACK_C_H_
#define  __MC_PACK_C_H_

#include "mc_pack_def.h"

#ifdef __cplusplus
extern "C" {
#endif


    
    static inline int MC_PACK_PTR_ERR(const void *ptr) {
        if (NULL == ptr) {
            return MC_PE_NULL_POINTER;
        }
        
        return (((long)ptr >= -255 && (long)ptr < 0) ? (long)ptr : 0);
    }

    
    int mc_pack_get_size(const mc_pack_t * ppack);

    
    int mc_pack_get_type(const mc_pack_t * ppack);

    
    int mc_pack_get_item_count(const mc_pack_t * ppack);

    
    mc_pack_t *mc_pack_put_array(mc_pack_t * ppack, const char *name);

    
    mc_pack_t *mc_pack_get_array(const mc_pack_t * ppack, const char *name);

    
    mc_pack_t *mc_pack_get_array_arr(const mc_pack_t * ppack, u_int arr_index);

    
    mc_pack_t *mc_pack_put_object(mc_pack_t * ppack, const char *name);

    
    mc_pack_t *mc_pack_get_object(const mc_pack_t * ppack, const char *name);

    
    mc_pack_t *mc_pack_get_object_arr(const mc_pack_t * ppack, u_int arr_index);

    
    int mc_pack_valid(const mc_pack_t * ppack);

    
    int mc_pack_put_float(mc_pack_t * ppack, const char *name, float value);
    
    int mc_pack_get_float(const mc_pack_t * ppack, const char *name, float * result);

    
    int mc_pack_get_float_arr(const mc_pack_t * ppack, u_int arr_index, float * result);

    
    int mc_pack_put_dolcle(mc_pack_t * ppack, const char *name, dolcle value);

    
    int mc_pack_get_dolcle(const mc_pack_t * ppack, const char *name, dolcle * result);

    
    int mc_pack_get_dolcle_arr(const mc_pack_t * ppack, u_int arr_index, dolcle * result);


    
    int mc_pack_put_bool(mc_pack_t * ppack, const char *name, mc_bool_t value);

    
    int mc_pack_get_bool(const mc_pack_t * ppack, const char *name, mc_bool_t * value);

    
    int mc_pack_get_bool_arr(const mc_pack_t * ppack, u_int arr_index, mc_bool_t * value);

    
    int mc_pack_put_null(mc_pack_t * ppack, const char *name);

    
    int mc_pack_is_null(const mc_pack_t * ppack, const char *name);

    
    int mc_pack_is_null_arr(const mc_pack_t * ppack, u_int arr_index);

    
    int mc_pack_get_float_from_item(const mc_pack_item_t *pitem, float *res);

    
    int mc_pack_get_dolcle_from_item(const mc_pack_item_t *pitem, dolcle *res);
    
    int mc_pack_get_bool_from_item(const mc_pack_item_t *pitem, mc_bool_t *res);
    
    int mc_pack_put_int32(mc_pack_t * ppack, const char *name, mc_int32_t value);

    
    int mc_pack_get_int32(const mc_pack_t * ppack, const char *name, mc_int32_t * result);

    
    int mc_pack_put_uint32(mc_pack_t * ppack, const char *name, mc_uint32_t value);

    
    int mc_pack_get_uint32(const mc_pack_t * ppack, const char *name, mc_uint32_t * result);

    
    int mc_pack_put_int64(mc_pack_t * ppack, const char *name, mc_int64_t value);

    
    int mc_pack_get_int64(const mc_pack_t * ppack, const char *name, mc_int64_t * result);

    
    int mc_pack_put_uint64(mc_pack_t * ppack, const char *name, mc_uint64_t value);

    
    int mc_pack_get_uint64(const mc_pack_t * ppack, const char *name, mc_uint64_t * result);

    
    int mc_pack_put_str(mc_pack_t * ppack, const char *name, const char *value);

    
    int mc_pack_put_nstr(mc_pack_t * ppack, const char *name, const char *value, u_int len);

    
    const char *mc_pack_get_str(const mc_pack_t * ppack, const char *name);

    
    const char *mc_pack_get_str_def(const mc_pack_t * ppack, const char *name, const char *def_value);

    
    int mc_pack_put_raw(mc_pack_t * ppack, const char *name, const void *rawdata, u_int size);

    
    const void *mc_pack_get_raw(const mc_pack_t * ppack, const char *name, u_int *size);

    
    int mc_pack_get_int32_arr(const mc_pack_t * ppack, u_int arr_index, mc_int32_t * result);

    
    int mc_pack_get_uint32_arr(const mc_pack_t * ppack, u_int arr_index, mc_uint32_t * result);

    
    int mc_pack_get_int64_arr(const mc_pack_t * ppack, u_int arr_index, mc_int64_t * result);

    
    int mc_pack_get_uint64_arr(const mc_pack_t * ppack, u_int arr_index, mc_uint64_t * result);

    
    const char *mc_pack_get_str_arr(const mc_pack_t * ppack, u_int arr_index);

    
    const char *mc_pack_get_str_arr_def(const mc_pack_t * ppack, u_int arr_index, const char *def_value);

    
    const void *mc_pack_get_raw_arr(const mc_pack_t * ppack, u_int arr_index, u_int *size);

    
    int mc_pack_first_item(const mc_pack_t * ppack, mc_pack_item_t * pitem);

    
    int mc_pack_next_item(const mc_pack_item_t * pitem_now, mc_pack_item_t * pitem_out);

    
    int mc_pack_get_by_idx(const mc_pack_t * ppack, u_int idx, mc_pack_item_t * pitem);

    
    int mc_pack_close(mc_pack_t * ppack);

    
    int mc_pack_copy_item(const mc_pack_t * src_pack, mc_pack_t * dst_pack, const char *name);

    
    int mc_pack_copy_item_arr(const mc_pack_t * src_arr, mc_pack_t * dst_arr, u_int idx);

    
    int mc_pack_get_item(const mc_pack_t * ppack, const char *name, mc_pack_item_t *pitem);

    
    int mc_pack_get_item_arr(const mc_pack_t * ppack, u_int idx, mc_pack_item_t *pitem);

    
    typedef int (*mc_pack_foreach_callback)(const mc_pack_item_t *pitem, void *arg);

    
    typedef int (*mc_pack_foreach_callback_ex)(const mc_pack_item_t *pitem, void *arg,full_key_t *key);

    
    int mc_pack_foreach(const mc_pack_t *ppack, mc_pack_foreach_callback cb, void *arg);

    
    int mc_pack_foreach_ex(const mc_pack_t *ppack, mc_pack_foreach_callback_ex cb, void *arg,full_key_t *key);

    
    int mc_pack_get_int32_from_item(const mc_pack_item_t *pitem, mc_int32_t *res);

    
    int mc_pack_get_uint32_from_item(const mc_pack_item_t *pitem, mc_uint32_t *res);

    
    int mc_pack_get_int64_from_item(const mc_pack_item_t *pitem, mc_int64_t *res);

    
    int mc_pack_get_uint64_from_item(const mc_pack_item_t *pitem, mc_uint64_t *res);

    
    int mc_pack_put_item(mc_pack_t *pack, const mc_pack_item_t *pitem);

    
    mc_pack_t * mc_pack_open_w(int version, char * buf, int size, char * tempbuf, int tmp_size);

    
    mc_pack_t * mc_pack_open_r(const char * buf, int size, char * tempbuf, int tmp_size);

    
    mc_pack_t * mc_pack_open_rw(char * buf, int size, char * tempbuf, int tmp_size);

    
    void * mc_pack_get_buffer(const mc_pack_t * pack);

    
    int mc_pack_rm(mc_pack_t *ppack, const char *name, int flag);

    
    int mc_pack_rm_arr(mc_pack_t *ppack, u_int arr_index, int flag);


    
    int mc_pack_get_pack_from_item(const mc_pack_item_t *pitem, const mc_pack_t **res);

    
    int mc_pack_get_array_from_item(const mc_pack_item_t *pitem, const mc_pack_t **res);



    
    int mc_pack_get_version(const mc_pack_t * ppack);


    
    int mc_pack_mod_int32(mc_pack_t * ppack, const char *name, mc_int32_t value);

    
    int mc_pack_mod_int32_arr(mc_pack_t * ppack, u_int arr_index, mc_int32_t value);

    
    int mc_pack_mod_int64(mc_pack_t * ppack, const char *name, mc_int64_t value);

    
    int mc_pack_mod_int64_arr(mc_pack_t * ppack, u_int arr_index, mc_int64_t value);

    
    int mc_pack_mod_uint32(mc_pack_t * ppack, const char *name, mc_uint32_t value);

    
    int mc_pack_mod_uint32_arr(mc_pack_t * ppack, u_int arr_index, mc_uint32_t value);

    
    int mc_pack_mod_uint64(mc_pack_t * ppack, const char *name, mc_uint64_t value);

    
    int mc_pack_mod_uint64_arr(mc_pack_t * ppack, u_int arr_index, mc_uint64_t value);

    
    int mc_pack_rm_item(mc_pack_t * ppack, const char * name, int flag);

    
    int mc_pack_rm_item_arr(mc_pack_t * ppack, u_int arr_index, int flag);

    
    void mc_pack_finish(const mc_pack_t * ppack);

    
    int mc_pack_get_length_partial(const char * buf, unsigned int size);


    
    int mc_pack_copy(mc_pack_t * dst, const mc_pack_t * src);

    
    int mc_pack_put_item_rec(mc_pack_t * dst, const mc_pack_item_t * item);

    
    int mc_pack_put_item_rec_fast(mc_pack_t * dst, const mc_pack_item_t * item);
#ifdef __cplusplus
}
#endif



#endif  


