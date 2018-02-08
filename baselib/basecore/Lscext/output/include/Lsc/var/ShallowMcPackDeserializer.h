

#ifndef  __SHALLOWMCPACKDESERIALIZER_H_
#define  __SHALLOWMCPACKDESERIALIZER_H_

#include "Lsc/var/McPackDeserializer.h"

namespace Lsc{
namespace var{
	
    
    class ShallowMcPackDeserializer: plclic McPackDeserializer{
    plclic:
		
		
        ShallowMcPackDeserializer(ResourcePool& rp)
        :McPackDeserializer(rp) { 
			set_copy_option( McPackDeserializer::SHALLOW );
		}
	};

}}   


#endif  


