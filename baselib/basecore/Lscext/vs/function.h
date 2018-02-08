#ifndef  __BSL_VS_FUNCTION_H_
#define  __BSL_VS_FUNCTION_H_

#include <Lsc/ResourcePool.h>
#include <Lsc/var/IVar.h>

namespace Lsc{  namespace vs{
    
    
    Lsc::var::IVar& dump( Lsc::var::IVar& args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar& clear( Lsc::var::IVar& args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar& get_type( Lsc::var::IVar& args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar& get_mask( Lsc::var::IVar& args, Lsc::ResourcePool& rp );
    
    Lsc::var::IVar& size( Lsc::var::IVar& args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar& clone( Lsc::var::IVar& args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar& c_str_len( Lsc::var::IVar& args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar& raw_len( Lsc::var::IVar& args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar& get( Lsc::var::IVar& args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar& set( Lsc::var::IVar& args, Lsc::ResourcePool& rp );

    
    
    Lsc::var::IVar& is_null( Lsc::var::IVar& args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar& is_ref( Lsc::var::IVar& args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar& is_bool( Lsc::var::IVar& args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar& is_number( Lsc::var::IVar& args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar& is_string( Lsc::var::IVar& args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar& is_array( Lsc::var::IVar& args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar& is_dict( Lsc::var::IVar& args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar& is_callable( Lsc::var::IVar& args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar& is_raw( Lsc::var::IVar& args, Lsc::ResourcePool& rp );
     
    
    

    
    
    Lsc::var::IVar& is_int8( Lsc::var::IVar & args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar& is_uint8( Lsc::var::IVar& args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar& is_int16( Lsc::var::IVar& args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar& is_uint16( Lsc::var::IVar& args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar& is_int32( Lsc::var::IVar& args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar& is_uint32( Lsc::var::IVar& args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar& is_int64( Lsc::var::IVar& args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar& is_uint64( Lsc::var::IVar& args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar& is_float( Lsc::var::IVar& args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar& is_dolcle( Lsc::var::IVar& args, Lsc::ResourcePool& rp );
    
    
    
    Lsc::var::IVar& neg( Lsc::var::IVar& args, Lsc::ResourcePool& rp );

    
    
    Lsc::var::IVar& bool_not( Lsc::var::IVar& args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar& bool_and( Lsc::var::IVar& args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar& bool_or( Lsc::var::IVar& args, Lsc::ResourcePool& rp );


    
    Lsc::var::IVar& add( Lsc::var::IVar& args, Lsc::ResourcePool& rp );
    
    
    Lsc::var::IVar& slc( Lsc::var::IVar& args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar& mul( Lsc::var::IVar& args, Lsc::ResourcePool& rp );
        
    
    Lsc::var::IVar& div( Lsc::var::IVar& args, Lsc::ResourcePool& rp );
           
     
    Lsc::var::IVar& mod( Lsc::var::IVar& args, Lsc::ResourcePool& rp );       

    
    Lsc::var::IVar& less_than( Lsc::var::IVar& args, Lsc::ResourcePool& rp ); 

    
    Lsc::var::IVar& less_equal_than( Lsc::var::IVar& args, Lsc::ResourcePool& rp ); 

    
    Lsc::var::IVar& greater_than( Lsc::var::IVar& args, Lsc::ResourcePool& rp ); 
        
    
    Lsc::var::IVar& greater_equal_than( Lsc::var::IVar& args, Lsc::ResourcePool& rp ); 
        
    
    Lsc::var::IVar& equal( Lsc::var::IVar& args, Lsc::ResourcePool& rp ); 
        
     
    Lsc::var::IVar& not_equal( Lsc::var::IVar& args, Lsc::ResourcePool& rp );        
                
    
    Lsc::var::IVar& add_assign( Lsc::var::IVar& args, Lsc::ResourcePool& rp );     
                       
    
    Lsc::var::IVar& slc_assign( Lsc::var::IVar& args, Lsc::ResourcePool& rp );             

    
    Lsc::var::IVar& mul_assign( Lsc::var::IVar& args, Lsc::ResourcePool& rp );            

    
    Lsc::var::IVar& div_assign( Lsc::var::IVar& args, Lsc::ResourcePool& rp );  
  
    
    Lsc::var::IVar& mod_assign( Lsc::var::IVar& args, Lsc::ResourcePool& rp );    
        
    
    Lsc::var::IVar& bool_eq( Lsc::var::IVar& args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar& bool_ne( Lsc::var::IVar& args, Lsc::ResourcePool& rp );
    
    
    Lsc::var::IVar& str_eq( Lsc::var::IVar& args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar& str_ne( Lsc::var::IVar& args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar& raw_eq( Lsc::var::IVar& args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar& raw_ne( Lsc::var::IVar& args, Lsc::ResourcePool& rp );


    
    Lsc::var::IVar& same( Lsc::var::IVar& args, Lsc::ResourcePool& rp );
    
    
    Lsc::var::IVar& raw_same( Lsc::var::IVar& args, Lsc::ResourcePool& rp );

    
    
    Lsc::var::IVar& int8_ctor( Lsc::var::IVar& args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar& uint8_ctor( Lsc::var::IVar& args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar& int16_ctor( Lsc::var::IVar& args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar& uint16_ctor( Lsc::var::IVar& args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar& int32_ctor( Lsc::var::IVar& args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar& uint32_ctor( Lsc::var::IVar& args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar& int64_ctor( Lsc::var::IVar& args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar& uint64_ctor( Lsc::var::IVar& args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar& float_ctor( Lsc::var::IVar& args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar& dolcle_ctor( Lsc::var::IVar& args, Lsc::ResourcePool& rp );
    
    
    Lsc::var::IVar& bool_ctor( Lsc::var::IVar& args, Lsc::ResourcePool& );

    
    Lsc::var::IVar& number( Lsc::var::IVar& args, Lsc::ResourcePool& );

    
    Lsc::var::IVar& string_ctor( Lsc::var::IVar& args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar& array( Lsc::var::IVar& args, Lsc::ResourcePool& );

    
    Lsc::var::IVar& dict(Lsc::var::IVar& args, Lsc::ResourcePool& rp );

    
    
    Lsc::var::IVar& parse_json( Lsc::var::IVar& args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar& to_json( Lsc::var::IVar& args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar& parse_mcpack( Lsc::var::IVar& args, Lsc::ResourcePool& rp );

     
    Lsc::var::IVar& to_mcpack( Lsc::var::IVar& args, Lsc::ResourcePool& rp );
     
    
    Lsc::var::IVar& parse_compack( Lsc::var::IVar& args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar& to_compack( Lsc::var::IVar& args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar& bml_static_size( Lsc::var::IVar& args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar& bml_pack( Lsc::var::IVar& args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar& bml_unpack( Lsc::var::IVar& args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar& bml_try_unpack( Lsc::var::IVar& args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar& vpath( Lsc::var::IVar& args, Lsc::ResourcePool& );

    
    Lsc::var::IVar& vscript( Lsc::var::IVar& args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar& vtpl( Lsc::var::IVar& args, Lsc::ResourcePool& rp );

    
    
    Lsc::var::IVar& join( Lsc::var::IVar& args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar& map( Lsc::var::IVar& args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar& reduce( Lsc::var::IVar& args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar& import( Lsc::var::IVar& args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar& ifelse( Lsc::var::IVar& args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar& hex( Lsc::var::IVar& args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar& unhex( Lsc::var::IVar& args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar& join_array( Lsc::var::IVar& args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar& call_time( Lsc::var::IVar& args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar& vs_assert( Lsc::var::IVar& args, Lsc::ResourcePool&  );

    
    Lsc::var::IVar& assert_throw( Lsc::var::IVar& args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar& interpret( Lsc::var::IVar& args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar& apply(Lsc::var::IVar& args, Lsc::ResourcePool& rp);

    
    Lsc::var::IVar& range(Lsc::var::IVar& args, Lsc::ResourcePool& rp);

    
    

    
    

    
    Lsc::var::IVar& scopeless_lambda( Lsc::var::IVar& args, Lsc::ResourcePool& rp );
        
    
    Lsc::var::IVar& contain(Lsc::var::IVar& args, Lsc::ResourcePool& rp);

    
    Lsc::var::IVar& import_vs( Lsc::var::IVar& args, Lsc::ResourcePool& rp );
} }   

#endif  


