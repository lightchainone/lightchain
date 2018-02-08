
#ifndef PLCLIC_MCPACK_SAMPLE_TEXTGENERATOR_H_
#define PLCLIC_MCPACK_SAMPLE_TEXTGENERATOR_H_

#include <cstring>
#include "mc_pack.h"

class TextGenerator{
  protected:
    char *_data;
    int _datasize;
    int _length;
  plclic:
    TextGenerator(int datasize=1024*1024){
        _datasize=datasize;
        _data=new char[datasize];
        _length=0;
    }
    virtual ~TextGenerator(){
        delete [] _data;
    }
    int Pack2Text(mc_pack_t *pack){
        assert(mc_pack_pack2text(pack,_data,_datasize,0)==0);
        _length=strlen(_data);
        return 0;
    }
    const char *TextData(){
        return _data;
    }
    int TextLength(){
        return _length;
    }
};

#endif  
