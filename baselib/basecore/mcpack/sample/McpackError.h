

#ifndef PLCLIC_MCPACK_SAMPLE_MCPACKERROR_H_
#define PLCLIC_MCPACK_SAMPLE_MCPACKERROR_H_

#include "mc_pack.h"
class McpackError{
  plclic:
    static const char *Message(int err){
        return mc_pack_perror(err);
    }
    static const char *Message(const mc_pack_t *ptr){
        return mc_pack_perror_ptr(ptr);
    }
};

#endif  
