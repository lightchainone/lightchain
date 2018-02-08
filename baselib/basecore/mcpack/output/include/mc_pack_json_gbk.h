

#ifndef  __MC_PACK_JSON_GBK_H_
#define  __MC_PACK_JSON_GBK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "mc_pack_c.h"

    
    int mc_pack_json2pack_gbk(const char *txt, mc_pack_t *pack);

    
    int mc_pack_pack2json_gbk(const mc_pack_t *pack, char *txt_buf, u_int buf_size);

#ifdef __cplusplus
}
#endif

#endif  


