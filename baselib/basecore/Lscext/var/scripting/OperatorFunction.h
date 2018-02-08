#ifndef  __BSL_VAR_OPERATORFUNCTION_H__
#define  __BSL_VAR_OPERATORFUNCTION_H__

#include <Lsc/var/IVar.h>
#include <Lsc/ResourcePool.h>

namespace Lsc{ namespace var{

    
    Lsc::var::IVar* assign_clone( Lsc::var::IVar** args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar* assign_ref( Lsc::var::IVar** args, Lsc::ResourcePool&  );

    
    Lsc::var::IVar* logic_not ( Lsc::var::IVar** args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar* logic_and ( Lsc::var::IVar** args, Lsc::ResourcePool&  );

    
    Lsc::var::IVar* logic_or( Lsc::var::IVar** args, Lsc::ResourcePool&  );

    
    Lsc::var::IVar* bool_eq( Lsc::var::IVar** args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar* bool_ne( Lsc::var::IVar** args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar* str_eq( Lsc::var::IVar** args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar* str_ne( Lsc::var::IVar** args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar* raw_eq( Lsc::var::IVar** args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar* raw_ne( Lsc::var::IVar** args, Lsc::ResourcePool& rp );
    
    
    Lsc::var::IVar* raw_same( Lsc::var::IVar** args, Lsc::ResourcePool& rp );
   
    
    Lsc::var::IVar* raw_same( Lsc::var::IVar** args, Lsc::ResourcePool& rp ); 
    
}} 
#endif  

