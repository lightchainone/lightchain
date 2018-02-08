
#ifndef PLCLIC_MCPACK_SAMPLE_MCPACKOBJECTLARGEINT32_H_
#define PLCLIC_MCPACK_SAMPLE_MCPACKOBJECTLARGEINT32_H_

#include <cassert>
#include "McpackGenerator.h"

class McpackObjectLargeInt32:plclic McpackGenerator{
    plclic:
    
    
    
    
    
    mc_pack_t *Generate(){
        mc_pack_t *pack=mc_pack_open_w(2,_data,_datasize,_temp,_tempsize);
        assert(!MC_PACK_PTR_ERR(pack));
        
        for(int i=0;i<1000;i++){
            char objkey[10];
            snprintf(objkey,sizeof(objkey),"%d",i);
            assert(mc_pack_put_int32(pack,objkey,12345678)==0);
        }
        assert(!mc_pack_close(pack));
        return pack;
    }
    
    const char *Tag(){
        return "McpackObjectLargeInt32";
    }
    
};


#endif  
