

#ifndef  __MC_PACK_EX_H_
#define  __MC_PACK_EX_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>

#include "mc_pack_c.h"

    
    const char *mc_pack_perror(int err);
    const char *mc_pack_perror_ptr(const mc_pack_t * ptr);

    
    const char *mc_pack_ptype(int type);

    
    int mc_pack_cmp_str(const mc_pack_t *ppack, const char *name, const char *value);

    
    int mc_pack_casecmp_str(const mc_pack_t *ppack, const char *name, const char *value);

    
    mc_pack_t *
    mc_pack_creatf(int version, char *buf, u_int buf_size, char * tmp, u_int tmp_size, const char *fmt, ...);


    
    int mc_pack_putf(mc_pack_t *ppack, const char *fmt, ...) __attribute__((format(printf, 2, 3)));

    
    int mc_pack_vputf(mc_pack_t *ppack, const char *fmt, va_list arg);

    
    int mc_pack_getf(const mc_pack_t *ppack, const char *fmt, ...) __attribute__((format(scanf, 2, 3)));

    
    int mc_pack_vgetf(const mc_pack_t *ppack, const char *fmt, va_list arg);

    
    int mc_pack_pack2str(const mc_pack_t *ppack, char *str, u_int size);

    
    const char *mc_pack_get_slckey(const char *key);

#ifdef __cplusplus
}
#endif

#endif  


