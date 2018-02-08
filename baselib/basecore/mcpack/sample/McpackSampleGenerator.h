
#ifndef PLCLIC_MCPACK_SAMPLE_MCPACKSAMPLEGENERATOR_H_
#define PLCLIC_MCPACK_SAMPLE_MCPACKSAMPLEGENERATOR_H_

#include <cassert>
#include "McpackError.h"
#include "McpackGenerator.h"

class McpackSampleGenerator:plclic McpackGenerator{
  plclic:
    mc_pack_t *Generate(){
        
        
        
        
        
        
        mc_pack_t *pack=mc_pack_open_w(2,_data,_datasize,_temp,_tempsize);
        assert(!MC_PACK_PTR_ERR(pack));
        
        
        assert(!mc_pack_put_int32(pack,"key1",123));
        assert(!mc_pack_put_bool(pack,"key2",true));
        assert(!mc_pack_put_str(pack,"key3","value"));
        
        
        mc_pack_t *slcpack=mc_pack_put_object(pack,"key4");
        assert(!MC_PACK_PTR_ERR(slcpack));
        assert(!mc_pack_put_str(slcpack,"slckey1","value"));
        assert(!mc_pack_put_str(slcpack,"slckey2","value"));
        
        mc_pack_finish(slcpack);

        
        slcpack=mc_pack_put_array(pack,"key5");
        assert(!MC_PACK_PTR_ERR(slcpack));
        
        assert(!mc_pack_put_str(slcpack,NULL,"value"));
        assert(!mc_pack_put_str(slcpack,NULL,"value2"));
        mc_pack_finish(slcpack);

        
        assert(!mc_pack_close(pack));
        return pack;
    }
    const char *Tag(){
        return "McpackSampleGenerator";
    }
};

#endif  
