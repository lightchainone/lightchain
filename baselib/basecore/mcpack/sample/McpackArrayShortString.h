
#ifndef PLCLIC_MCPACK_SAMPLE_MCPACKARRAYSHORTSTRING_H_
#define PLCLIC_MCPACK_SAMPLE_MCPACKARRAYSHORTSTRING_H_

#include <cassert>
#include "McpackArrayShortString.h"

class McpackArrayShortString:plclic McpackGenerator{
    plclic:
    
    
    
    

    mc_pack_t *Generate(){
        mc_pack_t *pack=mc_pack_open_w(2,_data,_datasize,_temp,_tempsize);
        assert(!MC_PACK_PTR_ERR(pack));
        mc_pack_t *arraypack=mc_pack_put_array(pack,"key");
        assert(!MC_PACK_PTR_ERR(arraypack));
        for(int i=0;i<1000;i++){
            assert(!mc_pack_put_str(arraypack,NULL,"helloworld"));
        }
        mc_pack_finish(arraypack);
        assert(!mc_pack_close(pack));
        return pack;
    }
    const char *Tag(){
        return "McpackArrayShortString";
    }
};

#endif  
