#include <exception>
#include <Lsc/var/implement.h>
#include "OperatorFunction.h"

namespace Lsc{ namespace var{
    
   
    Lsc::var::IVar* assign_clone( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        return &( *args[0] = args[1]->clone( rp ) );
    }

    
    Lsc::var::IVar* assign_ref( Lsc::var::IVar** args, Lsc::ResourcePool&  ){
        return &( *args[0] = *args[1] );
    }

    
    Lsc::var::IVar* logic_not ( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        return &( rp.create<Lsc::var::Bool>( !args[0]->to_bool() ) );
    }

    
    Lsc::var::IVar* logic_and ( Lsc::var::IVar** args, Lsc::ResourcePool&  ){
        return ( args[0]->to_bool() ? args[1] : args[0] );
    }

    
    Lsc::var::IVar* logic_or( Lsc::var::IVar** args, Lsc::ResourcePool&  ){
        return ( args[0]->to_bool() ? args[0] : args[1] );
    }

    
    Lsc::var::IVar* bool_eq( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        return &( rp.create<Lsc::var::Bool>( args[0]->to_bool() == args[1]->to_bool() ) );
    }

    
    Lsc::var::IVar* bool_ne( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        return &( rp.create<Lsc::var::Bool>( args[0]->to_bool() != args[1]->to_bool() ) );
    }

    
    Lsc::var::IVar* str_eq( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        Lsc::var::IVar& args0 = *args[0];
        Lsc::var::IVar& args1 = *args[1];
        if ( args0.is_string() ){
            Lsc::ShallowCopyString sstr0( args0.c_str(), args0.c_str_len() );
            if ( args1.is_string() ){
                Lsc::ShallowCopyString sstr1( args1.c_str(), args1.c_str_len() );
                return &( rp.create<Lsc::var::Bool>( sstr0 == sstr1 ) );
            }else{
                return &( rp.create<Lsc::var::Bool>( sstr0 == args1.to_string() ) );
            }
        }else{
            if ( args1.is_string() ){
                Lsc::ShallowCopyString sstr1( args1.c_str(), args1.c_str_len() );
                return &( rp.create<Lsc::var::Bool>( args0.to_string() == sstr1 ) );
            }else{
                return &( rp.create<Lsc::var::Bool>( args0.to_string() == args1.to_string() ) );
            }
        }
    }

    
    Lsc::var::IVar* str_ne( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        return &( rp.create<Lsc::var::Bool>( !str_eq(args, rp)->to_bool() ) );
    }

    
    Lsc::var::IVar* raw_eq( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        const Lsc::var::raw_t& raw0 = args[0]->to_raw();
        const Lsc::var::raw_t& raw1 = args[1]->to_raw();
        if ( raw0.data == raw1.data && raw0.length == raw1.length ){
            return &( rp.create<Lsc::var::Bool>( true ) );
        }else if ( raw0.length == raw1.length && 0 == memcmp( raw0.data, raw1.data, raw0.length ) ){
            return &( rp.create<Lsc::var::Bool>( true ) );
        }else{
            return &( rp.create<Lsc::var::Bool>( false ) );
        }
    }

    
    Lsc::var::IVar* raw_ne( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        return &( rp.create<Lsc::var::Bool>( !raw_eq( args, rp )->to_bool() ) ); 
    }

    
    Lsc::var::IVar* same( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        return &( rp.create<Lsc::var::Bool>(
                    &Lsc::var::Ref( *args[0] ).ref() == &Lsc::var::Ref( *args[1] ).ref() ) );
    }

    
    Lsc::var::IVar* raw_same( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        const Lsc::var::raw_t& raw0 = args[0]->to_raw();
        const Lsc::var::raw_t& raw1 = args[1]->to_raw();
        return &( rp.create<Lsc::var::Bool>( raw0.data == raw1.data && raw0.length == raw1.length ) );
    }

}} 


