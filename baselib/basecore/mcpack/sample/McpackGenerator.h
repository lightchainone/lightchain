
#ifndef PLCLIC_MCPACK_SAMPLE_MCPACKGENERATOR_H_
#define PLCLIC_MCPACK_SAMPLE_MCPACKGENERATOR_H_

#include <cassert>
#include "mc_pack.h"

class McpackGenerator{
  protected:
    char *_data;
    char *_temp;
    int _datasize;
    int _tempsize;
    plclic:
    
    McpackGenerator(int datasize=1024*1024,
                    int tempsize=1024*1024){
        _datasize=datasize;
        _tempsize=tempsize;
        _data=new char[datasize];
        _temp=new char[tempsize];       
    }
    virtual ~McpackGenerator(){
        delete [] _data;
        delete [] _temp;
    }
    
    virtual mc_pack_t *Generate()=0;
    
    
    virtual const char *Tag()=0;
    
    
    const char *McpackData() {
        return _data;
    }
    int McpackLength(){
        return mc_pack_get_length_partial(_data,_datasize);
    }
    int Json2Pack(const char *json){
        mc_pack_t *pack=mc_pack_open_w(2,_data,_datasize,_temp,_tempsize);
        assert(!MC_PACK_PTR_ERR(pack));
        assert(!mc_pack_json2pack(json,pack));
        assert(!mc_pack_close(pack));
        return 0;
    }
    int Text2Pack(const char *text){
        mc_pack_t *pack=mc_pack_open_w(2,_data,_datasize,_temp,_tempsize);
        assert(!MC_PACK_PTR_ERR(pack));
        assert(!mc_pack_text2pack(text,pack));
        assert(!mc_pack_close(pack));
        return 0;
    }
};

#endif  
