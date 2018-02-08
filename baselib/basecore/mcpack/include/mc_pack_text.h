

#ifndef  __MC_PACK_TEXT_H_
#define  __MC_PACK_TEXT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "mc_pack_c.h"

    
    int mc_pack_text2pack(const char *txt, mc_pack_t *pack);

    
    int mc_pack_pack2text(const mc_pack_t *pack, char *txt_buf, u_int buf_size, int json);



#ifdef __cplusplus
}
#endif

#endif  


