#ifndef  __MC_PACK_RP_H_
#define  __MC_PACK_RP_H_

#include "mc_pack_def.h"
#include <Lsc/ResourcePool.h>

mc_pack_t * mc_pack_open_w_rp(int version, char * buf, int size, Lsc::ResourcePool * _pool);

mc_pack_t * mc_pack_open_rw_rp(char * buf, int size, Lsc::ResourcePool * _pool);

mc_pack_t * mc_pack_open_r_rp(const char * buf, int size, Lsc::ResourcePool * _pool);

#endif
