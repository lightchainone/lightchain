
#ifndef PLCLIC_MCPACK_SAMPLE_MCPACKEMPTY_H_
#define PLCLIC_MCPACK_SAMPLE_MCPACKEMPTY_H_

#include "McpackGenerator.h"

class McpackEmpty:plclic McpackGenerator{
    plclic:
    mc_pack_t *Generate(){
        mc_pack_t *pack=mc_pack_open_w(2,_data,_datasize,_temp,_tempsize);
        assert(!MC_PACK_PTR_ERR(pack));
        mc_pack_close(pack);
        return pack;
    }
    const char *Tag(){
        return "McpackEmpty";
    }
};

#endif  
