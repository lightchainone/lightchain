

#ifndef  __MC_PACK_JSON_H_
#define  __MC_PACK_JSON_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "mc_pack_c.h"

    
    int mc_pack_json2pack(const char *txt, mc_pack_t *pack);

    
    int mc_pack_pack2json(const mc_pack_t *pack, char *txt_buf, u_int buf_size);


    
    int mc_pack_print(const mc_pack_t * src);

    
    int mc_pack_printbuf(const char * packbuf, unsigned int length);
#ifdef __cplusplus
}
#endif

#endif  


