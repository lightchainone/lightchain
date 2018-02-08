
#ifndef PLCLIC_MCPACK_SAMPLE_JSONGENERATOR_H_
#define PLCLIC_MCPACK_SAMPLE_JSONGENERATOR_H_

#include <cstring>
#include "mc_pack.h"

class JsonGenerator{
  protected:
    char *_data;
    int _datasize;
    int _length;
  plclic:
    
    JsonGenerator(int datasize=1024*1024){
        _datasize=datasize;
        _data=new char[datasize];
        _length=0;
    }
    virtual ~JsonGenerator(){
        delete [] _data;
    }
    int Pack2Json(mc_pack_t *pack){
        assert(mc_pack_pack2json(pack,_data,_datasize)==0);
        _length=strlen(_data);
        return 0;
    }
    const char *JsonData() {
        return _data;
    }
    int JsonLength() {
        return _length;
    }
};

#endif  
