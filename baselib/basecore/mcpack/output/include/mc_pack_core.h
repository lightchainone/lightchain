#ifndef __MC_PACK_CORE_H
#define __MC_PACK_CORE_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <sys/types.h>
#include "mc_pack_def.h"
#define MC_PACK_VERSION (0x0000)




    

    typedef struct _mc_v1pack_t {
        char tag[4];
        u_int item_count;
        u_int bytes_left;
        u_int cur_pos;
        u_int internal_item_count;
        u_short version;
        u_short reserved;
        char data[0];
    } __attribute__((packed)) mc_v1pack_t;

    
    mc_v1pack_t *mc_v1pack_create(char *buf, u_int buf_size);

    
    int mc_v1pack_get_size(const mc_v1pack_t * ppack);

    
    int mc_v1pack_get_type(const mc_v1pack_t * ppack);

    
    int mc_v1pack_get_item_count(const mc_v1pack_t * ppack);

    
    mc_v1pack_t *mc_v1pack_put_array(mc_v1pack_t * ppack, const char *name);

    
    mc_v1pack_t *mc_v1pack_get_array(const mc_v1pack_t * ppack, const char *name);

    
    mc_v1pack_t *mc_v1pack_get_array_arr(const mc_v1pack_t * ppack, u_int arr_index);

    
    mc_v1pack_t *mc_v1pack_put_object(mc_v1pack_t * ppack, const char *name);

    
    mc_v1pack_t *mc_v1pack_get_object(const mc_v1pack_t * ppack, const char *name);

    
    mc_v1pack_t *mc_v1pack_get_object_arr(const mc_v1pack_t * ppack, u_int arr_index);

    
    int mc_v1pack_valid(const mc_v1pack_t * ppack, u_int buf_size);

    
    int mc_v1pack_put_int32(mc_v1pack_t * ppack, const char *name, mc_int32_t value);

    
    int mc_v1pack_put_float(mc_v1pack_t * ppack, const char *name, float value);

    
    int mc_v1pack_get_float(const mc_v1pack_t * ppack, const char *name, float * result);

    
    int mc_v1pack_get_float_arr(const mc_v1pack_t * ppack, u_int arr_index, float * result);

    
    int mc_v1pack_put_dolcle(mc_v1pack_t * ppack, const char *name, dolcle value);

    
    int mc_v1pack_get_dolcle(const mc_v1pack_t * ppack, const char *name, dolcle * result);

    
    int mc_v1pack_get_dolcle_arr(const mc_v1pack_t * ppack, u_int arr_index, dolcle * result);

    
    int mc_v1pack_put_bool(mc_v1pack_t * ppack, const char *name, mc_bool_t value);

    
    int mc_v1pack_get_bool(const mc_v1pack_t * ppack, const char *name, mc_bool_t * value);

    
    int mc_v1pack_get_bool_arr(const mc_v1pack_t * ppack, u_int arr_index, mc_bool_t * value);

    
    int mc_v1pack_put_null(mc_v1pack_t * ppack, const char *name);

    
    int mc_v1pack_is_null(const mc_v1pack_t * ppack, const char *name);

    
    int mc_v1pack_is_null_arr(const mc_v1pack_t * ppack, u_int arr_index);

    
    int mc_v1pack_get_float_from_item(const mc_pack_item_t *pitem, float *res);

    
    int mc_v1pack_get_dolcle_from_item(const mc_pack_item_t *pitem, dolcle *res);

    int mc_v1pack_get_bool_from_item(const mc_pack_item_t *pitem, mc_bool_t *res);
    
    
    int mc_v1pack_get_int32(const mc_v1pack_t * ppack, const char *name, mc_int32_t * result);

    
    int mc_v1pack_put_uint32(mc_v1pack_t * ppack, const char *name, mc_uint32_t value);

    
    int mc_v1pack_get_uint32(const mc_v1pack_t * ppack, const char *name, mc_uint32_t * result);

    
    int mc_v1pack_put_int64(mc_v1pack_t * ppack, const char *name, mc_int64_t value);

    
    int mc_v1pack_get_int64(const mc_v1pack_t * ppack, const char *name, mc_int64_t * result);

    
    int mc_v1pack_put_uint64(mc_v1pack_t * ppack, const char *name, mc_uint64_t value);

    
    int mc_v1pack_get_uint64(const mc_v1pack_t * ppack, const char *name, mc_uint64_t * result);

    
    int mc_v1pack_put_str(mc_v1pack_t * ppack, const char *name, const char *value);

    
    int mc_v1pack_put_nstr(mc_v1pack_t * ppack, const char *name, const char *value, u_int len);

    
    const char *mc_v1pack_get_str(const mc_v1pack_t * ppack, const char *name);

    
    const char *mc_v1pack_get_str_def(const mc_v1pack_t * ppack, const char *name, const char *def_value);

    
    int mc_v1pack_put_raw(mc_v1pack_t * ppack, const char *name, const void *rawdata, u_int size);

    
    const void *mc_v1pack_get_raw(const mc_v1pack_t * ppack, const char *name, u_int *size);

    
    int mc_v1pack_get_int32_arr(const mc_v1pack_t * ppack, u_int arr_index, mc_int32_t * result);

    
    int mc_v1pack_get_uint32_arr(const mc_v1pack_t * ppack, u_int arr_index, mc_uint32_t * result);

    
    int mc_v1pack_get_int64_arr(const mc_v1pack_t * ppack, u_int arr_index, mc_int64_t * result);

    
    int mc_v1pack_get_uint64_arr(const mc_v1pack_t * ppack, u_int arr_index, mc_uint64_t * result);

    
    const char *mc_v1pack_get_str_arr(const mc_v1pack_t * ppack, u_int arr_index);

    
    const char *mc_v1pack_get_str_arr_def(const mc_v1pack_t * ppack, u_int arr_index, const char *def_value);

    
    const void *mc_v1pack_get_raw_arr(const mc_v1pack_t * ppack, u_int arr_index, u_int *size);

    
    int mc_v1pack_first_item(const mc_v1pack_t * ppack, mc_pack_item_t * pitem);

    
    int mc_v1pack_next_item(const mc_pack_item_t * pitem_now, mc_pack_item_t * pitem_out);

    
    int mc_v1pack_get_by_idx(const mc_v1pack_t * ppack, u_int idx, mc_pack_item_t * pitem);

    
    int mc_v1pack_close(mc_v1pack_t * ppack);

    
    int mc_v1pack_reopen(mc_v1pack_t * ppack, u_int buf_size);

    
    int mc_v1pack_copy_item(const mc_v1pack_t * src_pack, mc_v1pack_t * dst_pack, const char *name);

    
    int mc_v1pack_copy_item_arr(const mc_v1pack_t * src_arr, mc_v1pack_t * dst_arr, u_int idx);

    
    int mc_v1pack_get_item(const mc_v1pack_t * ppack, const char *name, mc_pack_item_t *pitem);

    
    int mc_v1pack_get_item_arr(const mc_v1pack_t * ppack, u_int idx, mc_pack_item_t *pitem);

    
    int mc_v1pack_reset(mc_v1pack_t *ppack);

    
    typedef int (*mc_v1pack_foreach_callback)(const mc_pack_item_t *pitem, void *arg);
    typedef int (*mc_v1pack_foreach_callback_ex)(const mc_pack_item_t *pitem, void *arg,full_key_t *key);

    
    int mc_v1pack_foreach(const mc_v1pack_t *ppack, mc_v1pack_foreach_callback cb, void *arg);
    int mc_v1pack_foreach_ex(const mc_v1pack_t *ppack, mc_v1pack_foreach_callback_ex cb, void *arg,full_key_t *key);

    
    int mc_v1pack_get_int32_from_item(const mc_pack_item_t *pitem, mc_int32_t *res);

    
    int mc_v1pack_get_uint32_from_item(const mc_pack_item_t *pitem, mc_uint32_t *res);

    
    int mc_v1pack_get_int64_from_item(const mc_pack_item_t *pitem, mc_int64_t *res);

    
    int mc_v1pack_get_uint64_from_item(const mc_pack_item_t *pitem, mc_uint64_t *res);

    
    int mc_v1pack_put_item(mc_v1pack_t *pack, const mc_pack_item_t *pitem);

#ifdef __cplusplus
}
#endif

#endif	


