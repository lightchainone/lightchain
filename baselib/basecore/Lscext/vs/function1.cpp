#include <Lsc/var/implement.h>
#include <Lsc/ResourcePool.h>
#include <Lsc/var/VScriptOperator.h>
#include <Lsc/var/smart_deserialize_number.h>

namespace Lsc{  namespace vs{ 

    
    
    Lsc::var::IVar& is_null( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>(args[0].is_null());
    }

    
    Lsc::var::IVar& is_ref( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>(Lsc::var::Ref(args[0]).ref().is_ref());
    }

    
    Lsc::var::IVar& is_bool( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>(args[0].is_bool());
    }

    
    Lsc::var::IVar& is_number( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>(args[0].is_number());
    }

    
    Lsc::var::IVar& is_string( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>(args[0].is_string());
    }

    
    Lsc::var::IVar& is_array( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>(args[0].is_array());
    }

    
    Lsc::var::IVar& is_dict( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>(args[0].is_dict());
    }

    
    Lsc::var::IVar& is_callable( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>(args[0].is_callable());
    }

    
    Lsc::var::IVar& is_raw( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>(args[0].is_raw());
    }

    
    
    Lsc::var::IVar& is_int8( Lsc::var::IVar &args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].is_int8() );
    }

    
    Lsc::var::IVar& is_uint8( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].is_uint8() );
    }

    
    Lsc::var::IVar& is_int16( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].is_int16() );
    }

    
    Lsc::var::IVar& is_uint16( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].is_uint16() );
    }

    
    Lsc::var::IVar& is_int32( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].is_int32() );
    }

    
    Lsc::var::IVar& is_uint32( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].is_uint32() );
    }

    
    Lsc::var::IVar& is_int64( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].is_int64() );
    }

    
    Lsc::var::IVar& is_uint64( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].is_uint64() );
    }

    
    Lsc::var::IVar& is_float( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].is_float() );
    }

    
    Lsc::var::IVar& is_dolcle( Lsc::var::IVar &args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].is_dolcle() );
    }

    
    
    Lsc::var::IVar& neg( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return *Lsc::var::VScriptOperator::exec_operator(
            Lsc::var::VScriptOperator::OP_NEGATIVE, 
            &args[0], rp); 
    }
 
    
    
    Lsc::var::IVar& bool_not( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>(!args[0].to_bool() );
    }

    
    Lsc::var::IVar& bool_and( Lsc::var::IVar& args, Lsc::ResourcePool&  ){
        size_t i = 0;
        size_t n = args.size();
        if ( n == 0 ){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"args.size() can't be 0!";
        }
        -- n;
        for( ; i < n; ++ i ){
            if ( !args[i].to_bool() ){
                return args[i];
            }
        }
        return args[i];
    }

    
    Lsc::var::IVar& bool_or( Lsc::var::IVar& args, Lsc::ResourcePool&  ){
        size_t i = 0;
        size_t n = args.size();
        if ( n == 0 ){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"args.size() can't be 0!";
        }
        -- n;
        for( ; i < n; ++ i ){
            if ( args[i].to_bool() ){
                return args[i];
            }
        }
        return args[i];
    }

    
    Lsc::var::IVar& add( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return *Lsc::var::VScriptOperator::exec_operator(
            Lsc::var::VScriptOperator::OP_ADD, 
            &args[0], &args[1], rp); 
    }
    
    
    Lsc::var::IVar& slc( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return *Lsc::var::VScriptOperator::exec_operator(
            Lsc::var::VScriptOperator::OP_SLC, 
            &args[0], &args[1], rp); 
    }

    
    Lsc::var::IVar& mul( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return *Lsc::var::VScriptOperator::exec_operator(
            Lsc::var::VScriptOperator::OP_MUL, 
            &args[0], &args[1], rp); 
    }
        
    
    Lsc::var::IVar& div( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return *Lsc::var::VScriptOperator::exec_operator(
            Lsc::var::VScriptOperator::OP_DIV, 
            &args[0], &args[1], rp); 
    }
           
     
    Lsc::var::IVar& mod( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return *Lsc::var::VScriptOperator::exec_operator(
            Lsc::var::VScriptOperator::OP_MOD,
            &args[0], &args[1], rp); 
    }       

    
    Lsc::var::IVar& less_than( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return *Lsc::var::VScriptOperator::exec_operator(
            Lsc::var::VScriptOperator::OP_LESS_THAN, 
            &args[0], &args[1], rp); 
    }         

    
    Lsc::var::IVar& less_equal_than( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return *Lsc::var::VScriptOperator::exec_operator(
            Lsc::var::VScriptOperator::OP_LESS_EQUAL_THAN, 
            &args[0], &args[1], rp); 
    }   

    
    Lsc::var::IVar& greater_than( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return *Lsc::var::VScriptOperator::exec_operator(
            Lsc::var::VScriptOperator::OP_GREATER_THAN, 
            &args[0], &args[1], rp); 
    }  
        
    
    Lsc::var::IVar& greater_equal_than( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return *Lsc::var::VScriptOperator::exec_operator(
            Lsc::var::VScriptOperator::OP_GREATER_EQUAL_THAN, 
            &args[0], &args[1], rp); 
    }   
        
    
    Lsc::var::IVar& equal( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return *Lsc::var::VScriptOperator::exec_operator(
            Lsc::var::VScriptOperator::OP_EQUAL, 
            &args[0], &args[1], rp); 
    }  
        
     
    Lsc::var::IVar& not_equal( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return *Lsc::var::VScriptOperator::exec_operator(
            Lsc::var::VScriptOperator::OP_NOT_EQUAL, 
            &args[0], &args[1], rp); 
    }          
       
    
    Lsc::var::IVar& add_assign( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return *Lsc::var::VScriptOperator::exec_operator(
            Lsc::var::VScriptOperator::OP_ADD_ASSIGN, 
            &args[0], &args[1], rp); 
    }       
                       
    
    Lsc::var::IVar& slc_assign( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return *Lsc::var::VScriptOperator::exec_operator(
            Lsc::var::VScriptOperator::OP_SLC_ASSIGN, 
            &args[0], &args[1], rp); 
    }             

    
    Lsc::var::IVar& mul_assign( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return *Lsc::var::VScriptOperator::exec_operator(
            Lsc::var::VScriptOperator::OP_MUL_ASSIGN, 
            &args[0], &args[1], rp); 
    }            

    
    Lsc::var::IVar& div_assign( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return *Lsc::var::VScriptOperator::exec_operator(
            Lsc::var::VScriptOperator::OP_DIV_ASSIGN, 
            &args[0], &args[1], rp); 
    }  
  
    
    Lsc::var::IVar& mod_assign( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return *Lsc::var::VScriptOperator::exec_operator(
            Lsc::var::VScriptOperator::OP_MOD_ASSIGN, 
            &args[0], &args[1], rp); 
    }    
        
        
    
    Lsc::var::IVar& bool_eq( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].to_bool() == args[1].to_bool() );
    }
    
    
    Lsc::var::IVar& bool_ne( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].to_bool() != args[1].to_bool() );
    }
    
    
    Lsc::var::IVar& str_eq( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        Lsc::var::IVar& args1 = args[1];
        Lsc::var::IVar& args0 = args[0];
        if ( args0.is_string() ){
            Lsc::ShallowCopyString sstr0( args0.c_str(), args0.c_str_len() );
            if ( args1.is_string() ){
                Lsc::ShallowCopyString sstr1( args1.c_str(), args1.c_str_len() );
                return rp.create<Lsc::var::Bool>( sstr0 == sstr1 );
            }else{
                return rp.create<Lsc::var::Bool>( sstr0 == args1.to_string() );
            }
        }else{
            if ( args1.is_string() ){
                Lsc::ShallowCopyString sstr1( args1.c_str(), args1.c_str_len() );
                return rp.create<Lsc::var::Bool>( args0.to_string() == sstr1 );
            }else{
                return rp.create<Lsc::var::Bool>( args0.to_string() == args1.to_string() );
            }
        }
    }

    
    Lsc::var::IVar& str_ne( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>(!str_eq( args, rp ).to_bool());
    }

    
    Lsc::var::IVar& raw_eq( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        Lsc::var::raw_t raw0 = args[0].to_raw();
        Lsc::var::raw_t raw1 = args[1].to_raw();
        if ( raw0.data == raw1.data && raw0.length == raw1.length ){
            return rp.create<Lsc::var::Bool>(true);
        }else if ( raw0.length == raw1.length 
            && 0 == memcmp( raw0.data, raw1.data, raw0.length ) ){
            return rp.create<Lsc::var::Bool>(true);
        }else{
            return rp.create<Lsc::var::Bool>(false);
        }

    }

    
    Lsc::var::IVar& raw_ne( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>(!raw_eq( args, rp ).to_bool());
    }

    
    Lsc::var::IVar& same( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( 
            &Lsc::var::Ref(args[0]).ref() == &Lsc::var::Ref(args[1]).ref() );
    }
    
    
    Lsc::var::IVar& raw_same( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        Lsc::var::raw_t raw0 = args[0].to_raw();
        Lsc::var::raw_t raw1 = args[1].to_raw();
        return rp.create<Lsc::var::Bool>( 
            raw0.data == raw1.data && raw0.length == raw1.length );
    }
    
    
    
    Lsc::var::IVar& int8_ctor( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Number<signed char> >( args[0].to_int8() );
    }

    
    Lsc::var::IVar& uint8_ctor( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Number<unsigned char> >( args[0].to_uint8() );
    }

    
    Lsc::var::IVar& int16_ctor( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Number<signed short> >( args[0].to_int16() );
    }

    
    Lsc::var::IVar& uint16_ctor( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Number<unsigned short> >( args[0].to_uint16() );
    }

    
    Lsc::var::IVar& int32_ctor( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Number<signed int> >( args[0].to_int32() );
    }

    
    Lsc::var::IVar& uint32_ctor( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Number<unsigned int> >( args[0].to_uint32() );
    }

    
    Lsc::var::IVar& int64_ctor( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Number<signed long long> >( args[0].to_int64() );
    }

    
    Lsc::var::IVar& uint64_ctor( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Number<unsigned long long> >( args[0].to_uint64() );
    }

    
    Lsc::var::IVar& float_ctor( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Number<float> >( args[0].to_float() );
    }

    
    Lsc::var::IVar& dolcle_ctor( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Number<dolcle> >( args[0].to_dolcle() );
    }

    
    Lsc::var::IVar& bool_ctor( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].to_bool() );
    }

    
    Lsc::var::IVar& number( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return Lsc::var::smart_deserialize_number( rp, args[0].c_str() );
    }
    
    Lsc::var::IVar& string_ctor( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        Lsc::var::IVar& arg = args[0];
        if ( arg.is_string() ){
            return rp.create<Lsc::var::String>( arg.c_str(), arg.c_str_len() );
        }else{
            return rp.create<Lsc::var::String>( arg.to_string() );
        }
    }

    
    Lsc::var::IVar& array( Lsc::var::IVar& args, Lsc::ResourcePool& ){
        return args;
    }

    
    Lsc::var::IVar& dict(Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        Lsc::var::IVar& res = rp.create<Lsc::var::Dict>();
        size_t i = 0;
        size_t size_ = args.size();
        if ( size_ & 1 ){    
            throw Lsc::BadArgumentException()<<BSL_EARG<<"args.size() can't be odd!";
        }
        for( ; i < size_; i += 2 ){
            res[ args[i].to_string() ] = args[i+1];
        }
        return res;
    }

    
    Lsc::var::IVar& import_vs1( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        Lsc::var::IVar& dest = args.get(0);
        
        typedef Lsc::var::Function& (Lsc::ResourcePool::*FuncCreater)(
            const Lsc::var::Function::function_type& , const Lsc::var::Function::string_type&);
        FuncCreater fc = &Lsc::ResourcePool::create<Lsc::var::Function, 
            Lsc::var::Function::function_type, Lsc::var::Function::string_type>;
           
        
        dest["is_null"]   = (rp.*fc)( &Lsc::vs::is_null, "Lsc::vs::is_null");
        dest["is_ref"]    = (rp.*fc)( &Lsc::vs::is_ref, "Lsc::vs::is_ref");
        dest["is_bool"]   = (rp.*fc)( &Lsc::vs::is_bool, "Lsc::vs::is_bool");
        dest["is_number"] = (rp.*fc)( &Lsc::vs::is_number, "Lsc::vs::is_number");
        dest["is_string"] = (rp.*fc)( &Lsc::vs::is_string, "Lsc::vs::is_string");
        dest["is_array"]  = (rp.*fc)( &Lsc::vs::is_array, "Lsc::vs::is_array");
        dest["is_dict"]   = (rp.*fc)( &Lsc::vs::is_dict, "Lsc::vs::is_dict");
        dest["is_callable"]     = (rp.*fc)( &Lsc::vs::is_callable,"Lsc::vs::is_callable");
        dest["is_raw"]    = (rp.*fc)( &Lsc::vs::is_raw, "Lsc::vs::is_raw");
        dest["is_int8"]= (rp.*fc)( &Lsc::vs::is_int8, "Lsc::vs::is_int8");
        dest["is_uint8"]    = (rp.*fc)( &Lsc::vs::is_uint8, "Lsc::vs::is_uint8");
        dest["is_int16"]= (rp.*fc)( &Lsc::vs::is_int16, "Lsc::vs::is_int16");
        dest["is_uint16"]= (rp.*fc)( &Lsc::vs::is_uint16, "Lsc::vs::is_uint16");
        dest["is_int32"]= (rp.*fc)( &Lsc::vs::is_int32, "Lsc::vs::is_int32");
        dest["is_uint32"]= (rp.*fc)( &Lsc::vs::is_uint32, "Lsc::vs::is_uint32");
        dest["is_int64"]= (rp.*fc)( &Lsc::vs::is_int64, "Lsc::vs::is_int64");
        dest["is_uint64"]= (rp.*fc)( &Lsc::vs::is_uint64, "Lsc::vs::is_uint64");
        dest["is_float"]= (rp.*fc)( &Lsc::vs::is_float, "Lsc::vs::is_float");
        dest["is_dolcle"]= (rp.*fc)( &Lsc::vs::is_dolcle, "Lsc::vs::is_dolcle");

        
        dest["neg"]       = (rp.*fc)( &Lsc::vs::neg, "Lsc::vs::neg");
        dest["not"]       = (rp.*fc)( &Lsc::vs::bool_not, "Lsc::vs::bool_not");
        dest["and"]       = (rp.*fc)( &Lsc::vs::bool_and, "Lsc::vs::bool_and");
        dest["or"]        = (rp.*fc)( &Lsc::vs::bool_or, "Lsc::vs::bool_or");
        dest["add"]       = (rp.*fc)( &Lsc::vs::add, "Lsc::vs::add");
        dest["slc"]       = (rp.*fc)( &Lsc::vs::slc, "Lsc::vs::slc");
        dest["mul"]       = (rp.*fc)( &Lsc::vs::mul, "Lsc::vs::mul");
        dest["div"]       = (rp.*fc)( &Lsc::vs::div, "Lsc::vs::div");
        dest["mod"]       = (rp.*fc)( &Lsc::vs::mod, "Lsc::vs::mod");
        dest["lt"]        = (rp.*fc)( &Lsc::vs::less_than, "Lsc::vs::less_than");
        dest["le"]        = (rp.*fc)( &Lsc::vs::less_equal_than, "Lsc::vs::less_equal_than");
        dest["gt"]        = (rp.*fc)( &Lsc::vs::greater_than, "Lsc::vs::greater_than");
        dest["ge"]        = (rp.*fc)( &Lsc::vs::greater_equal_than, "Lsc::vs::greater_equal_than");
        dest["eq"]        = (rp.*fc)( &Lsc::vs::equal, "Lsc::vs::equal");
        dest["ne"]        = (rp.*fc)( &Lsc::vs::not_equal, "Lsc::vs::not_equal");
        dest["add_assign"]        = (rp.*fc)( &Lsc::vs::add_assign, "Lsc::vs::add_assign");
        dest["slc_assign"]        = (rp.*fc)( &Lsc::vs::slc_assign, "Lsc::vs::slc_assign");
        dest["mul_assign"]        = (rp.*fc)( &Lsc::vs::mul_assign, "Lsc::vs::mul_assign");
        dest["div_assign"]        = (rp.*fc)( &Lsc::vs::div_assign, "Lsc::vs::div_assign");
        dest["mod_assign"]        = (rp.*fc)( &Lsc::vs::mod_assign, "Lsc::vs::mod_assign");

        
        dest["beq"]       = (rp.*fc)( &Lsc::vs::bool_eq, "Lsc::vs::bool_eq");
        dest["bne"]       = (rp.*fc)( &Lsc::vs::bool_ne, "Lsc::vs::bool_ne");
        dest["seq"]       = (rp.*fc)( &Lsc::vs::str_eq, "Lsc::vs::str_eq");
        dest["sne"]       = (rp.*fc)( &Lsc::vs::str_ne, "Lsc::vs::str_ne");
        dest["raw_eq"]    = (rp.*fc)( &Lsc::vs::raw_eq, "Lsc::vs::raw_eq");
        dest["raw_ne"]    = (rp.*fc)( &Lsc::vs::raw_ne, "Lsc::vs::raw_ne");
    
        dest["same"]      = (rp.*fc)( &Lsc::vs::same, "Lsc::vs::same");
        dest["raw_same"]  = (rp.*fc)( &Lsc::vs::raw_same, "Lsc::vs::raw_same");

        
        dest["int8"]= (rp.*fc)( &Lsc::vs::int8_ctor, "Lsc::vs::int8_ctor");
        dest["uint8"]= (rp.*fc)( &Lsc::vs::uint8_ctor, "Lsc::vs::uint8_ctor");
        dest["int16"]= (rp.*fc)( &Lsc::vs::int16_ctor, "Lsc::vs::int16_ctor");
        dest["uint16"]= (rp.*fc)( &Lsc::vs::uint16_ctor, "Lsc::vs::uint16_ctor");
        dest["int32"]= (rp.*fc)( &Lsc::vs::int32_ctor, "Lsc::vs::int32_ctor");
        dest["uint32"]= (rp.*fc)( &Lsc::vs::uint32_ctor, "Lsc::vs::uint32_ctor");
        dest["int64"]= (rp.*fc)( &Lsc::vs::int64_ctor, "Lsc::vs::int64_ctor");
        dest["uint64"]= (rp.*fc)( &Lsc::vs::uint64_ctor,"Lsc::vs::uint64_ctor");
        dest["float"]= (rp.*fc)( &Lsc::vs::float_ctor, "Lsc::vs::float_ctor");
        dest["dolcle"]= (rp.*fc)( &Lsc::vs::dolcle_ctor, "Lsc::vs::dolcle_ctor");
        dest["bool"]      = (rp.*fc)( &Lsc::vs::bool_ctor, "Lsc::vs::bool_ctor");
        dest["number"]    = (rp.*fc)( &Lsc::vs::number, "Lsc::vs::number");
        dest["string"]    = (rp.*fc)( &Lsc::vs::string_ctor, "Lsc::vs::string_ctor");
        dest["array"]     = (rp.*fc)( &Lsc::vs::array, "Lsc::vs::array");
        dest["dict"]      = (rp.*fc)( &Lsc::vs::dict, "Lsc::vs::dict");
        
        return Lsc::var::Null::null;
    }
} } 


