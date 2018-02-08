#include <Lsc/var/implement.h>
#include "math.h"

namespace Lsc{ namespace vs{
    
    
    Lsc::var::IVar& int8_equal( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].to_int8() == args[1].to_int8() );
    }

    
    Lsc::var::IVar& uint8_equal( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].to_uint8() == args[1].to_uint8() );
    }

    
    Lsc::var::IVar& int16_equal( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].to_int16() == args[1].to_int16() );
    }

    
    Lsc::var::IVar& uint16_equal( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].to_uint16() == args[1].to_uint16() );
    }

    
    Lsc::var::IVar& int32_equal( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].to_int32() == args[1].to_int32() );
    }

    
    Lsc::var::IVar& uint32_equal( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].to_uint32() == args[1].to_uint32() );
    }

    
    Lsc::var::IVar& int64_equal( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].to_int64() == args[1].to_int64() );
    }

    
    Lsc::var::IVar& uint64_equal( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].to_uint64() == args[1].to_uint64() );
    }

    
    Lsc::var::IVar& float_equal( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].to_float() == args[1].to_float() );
    }

    
    Lsc::var::IVar& dolcle_equal( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].to_dolcle() == args[1].to_dolcle() );
    }

    
    Lsc::var::IVar& int8_not_equal( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].to_int8() != args[1].to_int8() );
    }

    
    Lsc::var::IVar& uint8_not_equal( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].to_uint8() != args[1].to_uint8() );
    }

    
    Lsc::var::IVar& int16_not_equal( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].to_int16() != args[1].to_int16() );
    }

    
    Lsc::var::IVar& uint16_not_equal( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].to_uint16() != args[1].to_uint16() );
    }

    
    Lsc::var::IVar& int32_not_equal( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].to_int32() != args[1].to_int32() );
    }

    
    Lsc::var::IVar& uint32_not_equal( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].to_uint32() != args[1].to_uint32() );
    }

    
    Lsc::var::IVar& int64_not_equal( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].to_int64() != args[1].to_int64() );
    }

    
    Lsc::var::IVar& uint64_not_equal( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].to_uint64() != args[1].to_uint64() );
    }

    
    Lsc::var::IVar& float_not_equal( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].to_float() != args[1].to_float() );
    }

    
    Lsc::var::IVar& dolcle_not_equal( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].to_dolcle() != args[1].to_dolcle() );
    }

    
    Lsc::var::IVar& int8_less( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].to_int8() < args[1].to_int8() );
    }

    
    Lsc::var::IVar& uint8_less( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].to_uint8() < args[1].to_uint8() );
    }

    
    Lsc::var::IVar& int16_less( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].to_int16() < args[1].to_int16() );
    }

    
    Lsc::var::IVar& uint16_less( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].to_uint16() < args[1].to_uint16() );
    }

    
    Lsc::var::IVar& int32_less( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].to_int32() < args[1].to_int32() );
    }

    
    Lsc::var::IVar& uint32_less( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].to_uint32() < args[1].to_uint32() );
    }

    
    Lsc::var::IVar& int64_less( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].to_int64() < args[1].to_int64() );
    }

    
    Lsc::var::IVar& uint64_less( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].to_uint64() < args[1].to_uint64() );
    }

    
    Lsc::var::IVar& float_less( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].to_float() < args[1].to_float() );
    }

    
    Lsc::var::IVar& dolcle_less( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].to_dolcle() < args[1].to_dolcle() );
    }

    
    Lsc::var::IVar& int8_less_equal( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].to_int8() <= args[1].to_int8() );
    }

    
    Lsc::var::IVar& uint8_less_equal( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].to_uint8() <= args[1].to_uint8() );
    }

    
    Lsc::var::IVar& int16_less_equal( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].to_int16() <= args[1].to_int16() );
    }

    
    Lsc::var::IVar& uint16_less_equal( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].to_uint16() <= args[1].to_uint16() );
    }

    
    Lsc::var::IVar& int32_less_equal( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].to_int32() <= args[1].to_int32() );
    }

    
    Lsc::var::IVar& uint32_less_equal( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].to_uint32() <= args[1].to_uint32() );
    }

    
    Lsc::var::IVar& int64_less_equal( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].to_int64() <= args[1].to_int64() );
    }

    
    Lsc::var::IVar& uint64_less_equal( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].to_uint64() <= args[1].to_uint64() );
    }

    
    Lsc::var::IVar& float_less_equal( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].to_float() <= args[1].to_float() );
    }

    
    Lsc::var::IVar& dolcle_less_equal( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].to_dolcle() <= args[1].to_dolcle() );
    }

    
    Lsc::var::IVar& int8_greater( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].to_int8() > args[1].to_int8() );
    }

    
    Lsc::var::IVar& uint8_greater( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].to_uint8() > args[1].to_uint8() );
    }

    
    Lsc::var::IVar& int16_greater( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].to_int16() > args[1].to_int16() );
    }

    
    Lsc::var::IVar& uint16_greater( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].to_uint16() > args[1].to_uint16() );
    }

    
    Lsc::var::IVar& int32_greater( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].to_int32() > args[1].to_int32() );
    }

    
    Lsc::var::IVar& uint32_greater( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].to_uint32() > args[1].to_uint32() );
    }

    
    Lsc::var::IVar& int64_greater( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].to_int64() > args[1].to_int64() );
    }

    
    Lsc::var::IVar& uint64_greater( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].to_uint64() > args[1].to_uint64() );
    }

    
    Lsc::var::IVar& float_greater( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].to_float() > args[1].to_float() );
    }

    
    Lsc::var::IVar& dolcle_greater( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].to_dolcle() > args[1].to_dolcle() );
    }

    
    Lsc::var::IVar& int8_greater_equal( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].to_int8() >= args[1].to_int8() );
    }

    
    Lsc::var::IVar& uint8_greater_equal( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].to_uint8() >= args[1].to_uint8() );
    }

    
    Lsc::var::IVar& int16_greater_equal( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].to_int16() >= args[1].to_int16() );
    }

    
    Lsc::var::IVar& uint16_greater_equal( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].to_uint16() >= args[1].to_uint16() );
    }

    
    Lsc::var::IVar& int32_greater_equal( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].to_int32() >= args[1].to_int32() );
    }

    
    Lsc::var::IVar& uint32_greater_equal( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].to_uint32() >= args[1].to_uint32() );
    }

    
    Lsc::var::IVar& int64_greater_equal( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].to_int64() >= args[1].to_int64() );
    }

    
    Lsc::var::IVar& uint64_greater_equal( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].to_uint64() >= args[1].to_uint64() );
    }

    
    Lsc::var::IVar& float_greater_equal( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].to_float() >= args[1].to_float() );
    }

    
    Lsc::var::IVar& dolcle_greater_equal( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Bool>( args[0].to_dolcle() >= args[1].to_dolcle() );
    }
    
    
    Lsc::var::IVar& int8_add( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        signed char res = (signed char)( ( args[0].to_int8() ) + ( args[1].to_int8() ) );
        return rp.create<Lsc::var::Number<signed char> >( res );
    }

    
    Lsc::var::IVar& uint8_add( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        unsigned char res = (unsigned char)( ( args[0].to_uint8() ) + ( args[1].to_uint8() ) );
        return rp.create<Lsc::var::Number<unsigned char> >( res );
    }

    
    Lsc::var::IVar& int16_add( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        signed short res = (signed short)( ( args[0].to_int16() ) + ( args[1].to_int16() ) );
        return rp.create<Lsc::var::Number<signed short> >( res );
    }

    
    Lsc::var::IVar& uint16_add( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        unsigned short res = (unsigned short)( ( args[0].to_uint16() ) + ( args[1].to_uint16() ) );
        return rp.create<Lsc::var::Number<unsigned short> >( res );
    }

    
    Lsc::var::IVar& int32_add( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        signed int res = (signed int)( ( args[0].to_int32() ) + ( args[1].to_int32() ) );
        return rp.create<Lsc::var::Number<signed int> >( res );
    }

    
    Lsc::var::IVar& uint32_add( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        unsigned int res = (unsigned int)( ( args[0].to_uint32() ) + ( args[1].to_uint32() ) );
        return rp.create<Lsc::var::Number<unsigned int> >( res );
    }

    
    Lsc::var::IVar& int64_add( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        signed long long res = (signed long long)( ( args[0].to_int64() ) + ( args[1].to_int64() ) );
        return rp.create<Lsc::var::Number<signed long long> >( res );
    }

    
    Lsc::var::IVar& uint64_add( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        unsigned long long res = (unsigned long long)( ( args[0].to_uint64() ) + ( args[1].to_uint64() ) );
        return rp.create<Lsc::var::Number<unsigned long long> >( res );
    }

    
    Lsc::var::IVar& float_add( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        float res = (float)( ( args[0].to_float() ) + ( args[1].to_float() ) );
        return rp.create<Lsc::var::Number<float> >( res );
    }

    
    Lsc::var::IVar& dolcle_add( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        dolcle res = (dolcle)( ( args[0].to_dolcle() ) + ( args[1].to_dolcle() ) );
        return rp.create<Lsc::var::Number<dolcle> >( res );
    }

    
    Lsc::var::IVar& int8_slc( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        signed char res = (signed char)( ( args[0].to_int8() ) - ( args[1].to_int8() ) );
        return rp.create<Lsc::var::Number<signed char> >( res );
    }

    
    Lsc::var::IVar& uint8_slc( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        unsigned char res = (unsigned char)( ( args[0].to_uint8() ) - ( args[1].to_uint8() ) );
        return rp.create<Lsc::var::Number<unsigned char> >( res );
    }

    
    Lsc::var::IVar& int16_slc( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        signed short res = (signed short)( ( args[0].to_int16() ) - ( args[1].to_int16() ) );
        return rp.create<Lsc::var::Number<signed short> >( res );
    }

    
    Lsc::var::IVar& uint16_slc( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        unsigned short res = (unsigned short)( ( args[0].to_uint16() ) - ( args[1].to_uint16() ) );
        return rp.create<Lsc::var::Number<unsigned short> >( res );
    }

    
    Lsc::var::IVar& int32_slc( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        signed int res = (signed int)( ( args[0].to_int32() ) - ( args[1].to_int32() ) );
        return rp.create<Lsc::var::Number<signed int> >( res );
    }

    
    Lsc::var::IVar& uint32_slc( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        unsigned int res = (unsigned int)( ( args[0].to_uint32() ) - ( args[1].to_uint32() ) );
        return rp.create<Lsc::var::Number<unsigned int> >( res );
    }

    
    Lsc::var::IVar& int64_slc( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        signed long long res = (signed long long)( ( args[0].to_int64() ) - ( args[1].to_int64() ) );
        return rp.create<Lsc::var::Number<signed long long> >( res );
    }

    
    Lsc::var::IVar& uint64_slc( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        unsigned long long res = (unsigned long long)( ( args[0].to_uint64() ) - ( args[1].to_uint64() ) );
        return rp.create<Lsc::var::Number<unsigned long long> >( res );
    }

    
    Lsc::var::IVar& float_slc( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        float res = (float)( ( args[0].to_float() ) - ( args[1].to_float() ) );
        return rp.create<Lsc::var::Number<float> >( res );
    }

    
    Lsc::var::IVar& dolcle_slc( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        dolcle res = (dolcle)( ( args[0].to_dolcle() ) - ( args[1].to_dolcle() ) );
        return rp.create<Lsc::var::Number<dolcle> >( res );
    }

    
    Lsc::var::IVar& int8_mul( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        signed char res = (signed char)( ( args[0].to_int8() ) * ( args[1].to_int8() ) );
        return rp.create<Lsc::var::Number<signed char> >( res );
    }

    
    Lsc::var::IVar& uint8_mul( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        unsigned char res = (unsigned char)( ( args[0].to_uint8() ) * ( args[1].to_uint8() ) );
        return rp.create<Lsc::var::Number<unsigned char> >( res );
    }

    
    Lsc::var::IVar& int16_mul( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        signed short res = (signed short)( ( args[0].to_int16() ) * ( args[1].to_int16() ) );
        return rp.create<Lsc::var::Number<signed short> >( res );
    }

    
    Lsc::var::IVar& uint16_mul( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        unsigned short res = (unsigned short)( ( args[0].to_uint16() ) * ( args[1].to_uint16() ) );
        return rp.create<Lsc::var::Number<unsigned short> >( res );
    }

    
    Lsc::var::IVar& int32_mul( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        signed int res = (signed int)( ( args[0].to_int32() ) * ( args[1].to_int32() ) );
        return rp.create<Lsc::var::Number<signed int> >( res );
    }

    
    Lsc::var::IVar& uint32_mul( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        unsigned int res = (unsigned int)( ( args[0].to_uint32() ) * ( args[1].to_uint32() ) );
        return rp.create<Lsc::var::Number<unsigned int> >( res );
    }

    
    Lsc::var::IVar& int64_mul( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        signed long long res = (signed long long)( ( args[0].to_int64() ) * ( args[1].to_int64() ) );
        return rp.create<Lsc::var::Number<signed long long> >( res );
    }

    
    Lsc::var::IVar& uint64_mul( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        unsigned long long res = (unsigned long long)( ( args[0].to_uint64() ) * ( args[1].to_uint64() ) );
        return rp.create<Lsc::var::Number<unsigned long long> >( res );
    }

    
    Lsc::var::IVar& float_mul( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        float res = (float)( ( args[0].to_float() ) * ( args[1].to_float() ) );
        return rp.create<Lsc::var::Number<float> >( res );
    }

    
    Lsc::var::IVar& dolcle_mul( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        dolcle res = (dolcle)( ( args[0].to_dolcle() ) * ( args[1].to_dolcle() ) );
        return rp.create<Lsc::var::Number<dolcle> >( res );
    }
    
    
    Lsc::var::IVar& int8_div( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        signed char var1 = args[0].to_int8();
        signed char var2 = args[1].to_int8();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for int8_div."; 
        }
        signed char res = (signed char)( var1 / var2 );
        return rp.create<Lsc::var::Number<signed char> >( res );
    }

    
    Lsc::var::IVar& uint8_div( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        unsigned char var1 = args[0].to_uint8();
        unsigned char var2 = args[1].to_uint8();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for uint8_div."; 
        }
        unsigned char res = (unsigned char)( var1 / var2 );
        return rp.create<Lsc::var::Number<unsigned char> >( res );
    }

    
    Lsc::var::IVar& int16_div( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        signed short var1 = args[0].to_int16();
        signed short var2 = args[1].to_int16();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for int16_div."; 
        }
        signed short res = (signed short)( var1 / var2 );
        return rp.create<Lsc::var::Number<signed short> >( res );
    }

    
    Lsc::var::IVar& uint16_div( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        unsigned short var1 = args[0].to_uint16();
        unsigned short var2 = args[1].to_uint16();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for uint16_div."; 
        }
        unsigned short res = (unsigned short)( var1 / var2 );
        return rp.create<Lsc::var::Number<unsigned short> >( res );
    }

    
    Lsc::var::IVar& int32_div( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        signed int var1 = args[0].to_int32();
        signed int var2 = args[1].to_int32();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for int32_div."; 
        }
        signed int res = (signed int)( var1 / var2 );
        return rp.create<Lsc::var::Number<signed int> >( res );
    }

    
    Lsc::var::IVar& uint32_div( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        unsigned int var1 = args[0].to_uint32();
        unsigned int var2 = args[1].to_uint32();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for uint32_div."; 
        }
        unsigned int res = (unsigned int)( var1 / var2 );
        return rp.create<Lsc::var::Number<unsigned int> >( res );
    }

    
    Lsc::var::IVar& int64_div( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        signed long long var1 = args[0].to_int64();
        signed long long var2 = args[1].to_int64();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for int64_div."; 
        }
        signed long long res = (signed long long)( var1 / var2 );
        return rp.create<Lsc::var::Number<signed long long> >( res );
    }

    
    Lsc::var::IVar& uint64_div( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        unsigned long long var1 = args[0].to_uint64();
        unsigned long long var2 = args[1].to_uint64();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for uint64_div."; 
        }
        unsigned long long res = (unsigned long long)( var1 / var2 );
        return rp.create<Lsc::var::Number<unsigned long long> >( res );
    }

    
    Lsc::var::IVar& float_div( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        float var1 = args[0].to_float();
        float var2 = args[1].to_float();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for float_div."; 
        }
        float res = (float)( var1 / var2 );
        return rp.create<Lsc::var::Number<float> >( res );
    }

    
    Lsc::var::IVar& dolcle_div( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        dolcle var1 = args[0].to_dolcle();
        dolcle var2 = args[1].to_dolcle();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for dolcle_div."; 
        }
        dolcle res = (dolcle)( var1 / var2 );
        return rp.create<Lsc::var::Number<dolcle> >( res );
    }
    
    
    Lsc::var::IVar& int8_mod( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        signed char var1 = args[0].to_int8();
        signed char var2 = args[1].to_int8();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for int8_mod."; 
        }
        signed char res = (signed char)( var1 % var2 );
        return rp.create<Lsc::var::Number<signed char> >( res );
    }

    
    Lsc::var::IVar& uint8_mod( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        unsigned char var1 = args[0].to_uint8();
        unsigned char var2 = args[1].to_uint8();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for uint8_mod."; 
        }
        unsigned char res = (unsigned char)( var1 % var2 );
        return rp.create<Lsc::var::Number<unsigned char> >( res );
    }

    
    Lsc::var::IVar& int16_mod( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        signed short var1 = args[0].to_int16();
        signed short var2 = args[1].to_int16();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for int16_mod."; 
        }
        signed short res = (signed short)( var1 % var2 );
        return rp.create<Lsc::var::Number<signed short> >( res );
    }

    
    Lsc::var::IVar& uint16_mod( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        unsigned short var1 = args[0].to_uint16();
        unsigned short var2 = args[1].to_uint16();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for uint16_mod."; 
        }
        unsigned short res = (unsigned short)( var1 % var2 );
        return rp.create<Lsc::var::Number<unsigned short> >( res );
    }

    
    Lsc::var::IVar& int32_mod( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        signed int var1 = args[0].to_int32();
        signed int var2 = args[1].to_int32();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for int32_mod."; 
        }
        signed int res = (signed int)( var1 % var2 );
        return rp.create<Lsc::var::Number<signed int> >( res );
    }

    
    Lsc::var::IVar& uint32_mod( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        unsigned int var1 = args[0].to_uint32();
        unsigned int var2 = args[1].to_uint32();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for uint32_mod."; 
        }
        unsigned int res = (unsigned int)( var1 % var2 );
        return rp.create<Lsc::var::Number<unsigned int> >( res );
    }

    
    Lsc::var::IVar& int64_mod( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        signed long long var1 = args[0].to_int64();
        signed long long var2 = args[1].to_int64();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for int64_mod."; 
        }
        signed long long res = (signed long long)( var1 % var2 );
        return rp.create<Lsc::var::Number<signed long long> >( res );
    }

    
    Lsc::var::IVar& uint64_mod( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        unsigned long long var1 = args[0].to_uint64();
        unsigned long long var2 = args[1].to_uint64();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for uint64_mod."; 
        }
        unsigned long long res = (unsigned long long)( var1 % var2 );
        return rp.create<Lsc::var::Number<unsigned long long> >( res );
    }

    
    Lsc::var::IVar& import_math1( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        Lsc::var::IVar& dict = args.get(0);
        
        typedef Lsc::var::Function& (Lsc::ResourcePool::*FuncCreater)(
            const Lsc::var::Function::function_type& , const Lsc::var::Function::string_type&);
        FuncCreater fc = &Lsc::ResourcePool::create<Lsc::var::Function, 
            Lsc::var::Function::function_type, Lsc::var::Function::string_type>;
            
        
        dict.set("teq", (rp.*fc)(&Lsc::vs::int8_equal, "Lsc::vs::int8_equal"));

        dict.set("uteq", (rp.*fc)(&Lsc::vs::uint8_equal, "Lsc::vs::uint8_equal"));

        dict.set("heq", (rp.*fc)(&Lsc::vs::int16_equal, "Lsc::vs::int16_equal"));

        dict.set("uheq", (rp.*fc)(&Lsc::vs::uint16_equal, "Lsc::vs::uint16_equal"));

        dict.set("ieq", (rp.*fc)(&Lsc::vs::int32_equal, "Lsc::vs::int32_equal"));

        dict.set("uieq", (rp.*fc)(&Lsc::vs::uint32_equal, "Lsc::vs::uint32_equal"));

        dict.set("leq", (rp.*fc)(&Lsc::vs::int64_equal, "Lsc::vs::int64_equal"));

        dict.set("uleq", (rp.*fc)(&Lsc::vs::uint64_equal, "Lsc::vs::uint64_equal"));

        dict.set("feq", (rp.*fc)(&Lsc::vs::float_equal, "Lsc::vs::float_equal"));

        dict.set("deq", (rp.*fc)(&Lsc::vs::dolcle_equal, "Lsc::vs::dolcle_equal"));

        dict.set("tne", (rp.*fc)(&Lsc::vs::int8_not_equal, "Lsc::vs::int8_not_equal"));

        dict.set("utne", (rp.*fc)(&Lsc::vs::uint8_not_equal, "Lsc::vs::uint8_not_equal"));

        dict.set("hne", (rp.*fc)(&Lsc::vs::int16_not_equal, "Lsc::vs::int16_not_equal"));

        dict.set("uhne", (rp.*fc)(&Lsc::vs::uint16_not_equal, "Lsc::vs::uint16_not_equal"));

        dict.set("ine", (rp.*fc)(&Lsc::vs::int32_not_equal, "Lsc::vs::int32_not_equal"));

        dict.set("uine", (rp.*fc)(&Lsc::vs::uint32_not_equal, "Lsc::vs::uint32_not_equal"));

        dict.set("lne", (rp.*fc)(&Lsc::vs::int64_not_equal, "Lsc::vs::int64_not_equal"));

        dict.set("ulne", (rp.*fc)(&Lsc::vs::uint64_not_equal, "Lsc::vs::uint64_not_equal"));

        dict.set("fne", (rp.*fc)(&Lsc::vs::float_not_equal, "Lsc::vs::float_not_equal"));

        dict.set("dne", (rp.*fc)(&Lsc::vs::dolcle_not_equal, "Lsc::vs::dolcle_not_equal"));

        dict.set("tlt", (rp.*fc)(&Lsc::vs::int8_less, "Lsc::vs::int8_less"));

        dict.set("utlt", (rp.*fc)(&Lsc::vs::uint8_less, "Lsc::vs::uint8_less"));

        dict.set("hlt", (rp.*fc)(&Lsc::vs::int16_less, "Lsc::vs::int16_less"));

        dict.set("uhlt", (rp.*fc)(&Lsc::vs::uint16_less, "Lsc::vs::uint16_less"));

        dict.set("ilt", (rp.*fc)(&Lsc::vs::int32_less, "Lsc::vs::int32_less"));

        dict.set("uilt", (rp.*fc)(&Lsc::vs::uint32_less, "Lsc::vs::uint32_less"));

        dict.set("llt", (rp.*fc)(&Lsc::vs::int64_less, "Lsc::vs::int64_less"));

        dict.set("ullt", (rp.*fc)(&Lsc::vs::uint64_less, "Lsc::vs::uint64_less"));

        dict.set("flt", (rp.*fc)(&Lsc::vs::float_less, "Lsc::vs::float_less"));

        dict.set("dlt", (rp.*fc)(&Lsc::vs::dolcle_less, "Lsc::vs::dolcle_less"));

        dict.set("tle", (rp.*fc)(&Lsc::vs::int8_less_equal, "Lsc::vs::int8_less_equal"));

        dict.set("utle", (rp.*fc)(&Lsc::vs::uint8_less_equal, "Lsc::vs::uint8_less_equal"));

        dict.set("hle", (rp.*fc)(&Lsc::vs::int16_less_equal, "Lsc::vs::int16_less_equal"));

        dict.set("uhle", (rp.*fc)(&Lsc::vs::uint16_less_equal, "Lsc::vs::uint16_less_equal"));

        dict.set("ile", (rp.*fc)(&Lsc::vs::int32_less_equal, "Lsc::vs::int32_less_equal"));

        dict.set("uile", (rp.*fc)(&Lsc::vs::uint32_less_equal, "Lsc::vs::uint32_less_equal"));

        dict.set("lle", (rp.*fc)(&Lsc::vs::int64_less_equal, "Lsc::vs::int64_less_equal"));

        dict.set("ulle", (rp.*fc)(&Lsc::vs::uint64_less_equal, "Lsc::vs::uint64_less_equal"));

        dict.set("fle", (rp.*fc)(&Lsc::vs::float_less_equal, "Lsc::vs::float_less_equal"));

        dict.set("dle", (rp.*fc)(&Lsc::vs::dolcle_less_equal, "Lsc::vs::dolcle_less_equal"));

        dict.set("tgt", (rp.*fc)(&Lsc::vs::int8_greater, "Lsc::vs::int8_greater"));

        dict.set("utgt", (rp.*fc)(&Lsc::vs::uint8_greater, "Lsc::vs::uint8_greater"));

        dict.set("hgt", (rp.*fc)(&Lsc::vs::int16_greater, "Lsc::vs::int16_greater"));

        dict.set("uhgt", (rp.*fc)(&Lsc::vs::uint16_greater, "Lsc::vs::uint16_greater"));

        dict.set("igt", (rp.*fc)(&Lsc::vs::int32_greater, "Lsc::vs::int32_greater"));

        dict.set("uigt", (rp.*fc)(&Lsc::vs::uint32_greater, "Lsc::vs::uint32_greater"));

        dict.set("lgt", (rp.*fc)(&Lsc::vs::int64_greater, "Lsc::vs::int64_greater"));

        dict.set("ulgt", (rp.*fc)(&Lsc::vs::uint64_greater, "Lsc::vs::uint64_greater"));

        dict.set("fgt", (rp.*fc)(&Lsc::vs::float_greater, "Lsc::vs::float_greater"));

        dict.set("dgt", (rp.*fc)(&Lsc::vs::dolcle_greater, "Lsc::vs::dolcle_greater"));

        dict.set("tge", (rp.*fc)(&Lsc::vs::int8_greater_equal, "Lsc::vs::int8_greater_equal"));

        dict.set("utge", (rp.*fc)(&Lsc::vs::uint8_greater_equal, "Lsc::vs::uint8_greater_equal"));

        dict.set("hge", (rp.*fc)(&Lsc::vs::int16_greater_equal, "Lsc::vs::int16_greater_equal"));

        dict.set("uhge", (rp.*fc)(&Lsc::vs::uint16_greater_equal, "Lsc::vs::uint16_greater_equal"));

        dict.set("ige", (rp.*fc)(&Lsc::vs::int32_greater_equal, "Lsc::vs::int32_greater_equal"));

        dict.set("uige", (rp.*fc)(&Lsc::vs::uint32_greater_equal, "Lsc::vs::uint32_greater_equal"));

        dict.set("lge", (rp.*fc)(&Lsc::vs::int64_greater_equal, "Lsc::vs::int64_greater_equal"));

        dict.set("ulge", (rp.*fc)(&Lsc::vs::uint64_greater_equal, "Lsc::vs::uint64_greater_equal"));

        dict.set("fge", (rp.*fc)(&Lsc::vs::float_greater_equal, "Lsc::vs::float_greater_equal"));

        dict.set("dge", (rp.*fc)(&Lsc::vs::dolcle_greater_equal, "Lsc::vs::dolcle_greater_equal"));
        
        dict.set("tadd", (rp.*fc)(&Lsc::vs::int8_add, "Lsc::vs::int8_add"));

        dict.set("utadd", (rp.*fc)(&Lsc::vs::uint8_add, "Lsc::vs::uint8_add"));

        dict.set("hadd", (rp.*fc)(&Lsc::vs::int16_add, "Lsc::vs::int16_add"));

        dict.set("uhadd", (rp.*fc)(&Lsc::vs::uint16_add, "Lsc::vs::uint16_add"));

        dict.set("iadd", (rp.*fc)(&Lsc::vs::int32_add, "Lsc::vs::int32_add"));

        dict.set("uiadd", (rp.*fc)(&Lsc::vs::uint32_add, "Lsc::vs::uint32_add"));

        dict.set("ladd", (rp.*fc)(&Lsc::vs::int64_add, "Lsc::vs::int64_add"));

        dict.set("uladd", (rp.*fc)(&Lsc::vs::uint64_add, "Lsc::vs::uint64_add"));

        dict.set("fadd", (rp.*fc)(&Lsc::vs::float_add, "Lsc::vs::float_add"));

        dict.set("dadd", (rp.*fc)(&Lsc::vs::dolcle_add, "Lsc::vs::dolcle_add"));

        dict.set("tslc", (rp.*fc)(&Lsc::vs::int8_slc, "Lsc::vs::int8_slc"));

        dict.set("utslc", (rp.*fc)(&Lsc::vs::uint8_slc, "Lsc::vs::uint8_slc"));

        dict.set("hslc", (rp.*fc)(&Lsc::vs::int16_slc, "Lsc::vs::int16_slc"));

        dict.set("uhslc", (rp.*fc)(&Lsc::vs::uint16_slc, "Lsc::vs::uint16_slc"));

        dict.set("islc", (rp.*fc)(&Lsc::vs::int32_slc, "Lsc::vs::int32_slc"));

        dict.set("uislc", (rp.*fc)(&Lsc::vs::uint32_slc, "Lsc::vs::uint32_slc"));

        dict.set("lslc", (rp.*fc)(&Lsc::vs::int64_slc, "Lsc::vs::int64_slc"));

        dict.set("ulslc", (rp.*fc)(&Lsc::vs::uint64_slc, "Lsc::vs::uint64_slc"));

        dict.set("fslc", (rp.*fc)(&Lsc::vs::float_slc, "Lsc::vs::float_slc"));

        dict.set("dslc", (rp.*fc)(&Lsc::vs::dolcle_slc, "Lsc::vs::dolcle_slc"));

        dict.set("tmul", (rp.*fc)(&Lsc::vs::int8_mul, "Lsc::vs::int8_mul"));

        dict.set("utmul", (rp.*fc)(&Lsc::vs::uint8_mul, "Lsc::vs::uint8_mul"));

        dict.set("hmul", (rp.*fc)(&Lsc::vs::int16_mul, "Lsc::vs::int16_mul"));

        dict.set("uhmul", (rp.*fc)(&Lsc::vs::uint16_mul, "Lsc::vs::uint16_mul"));

        dict.set("imul", (rp.*fc)(&Lsc::vs::int32_mul, "Lsc::vs::int32_mul"));

        dict.set("uimul", (rp.*fc)(&Lsc::vs::uint32_mul, "Lsc::vs::uint32_mul"));

        dict.set("lmul", (rp.*fc)(&Lsc::vs::int64_mul, "Lsc::vs::int64_mul"));

        dict.set("ulmul", (rp.*fc)(&Lsc::vs::uint64_mul, "Lsc::vs::uint64_mul"));

        dict.set("fmul", (rp.*fc)(&Lsc::vs::float_mul, "Lsc::vs::float_mul"));

        dict.set("dmul", (rp.*fc)(&Lsc::vs::dolcle_mul, "Lsc::vs::dolcle_mul"));
        
        dict.set("tdiv", (rp.*fc)(&Lsc::vs::int8_div, "Lsc::vs::int8_div"));

        dict.set("utdiv", (rp.*fc)(&Lsc::vs::uint8_div, "Lsc::vs::uint8_div"));

        dict.set("hdiv", (rp.*fc)(&Lsc::vs::int16_div, "Lsc::vs::int16_div"));

        dict.set("uhdiv", (rp.*fc)(&Lsc::vs::uint16_div, "Lsc::vs::uint16_div"));

        dict.set("idiv", (rp.*fc)(&Lsc::vs::int32_div, "Lsc::vs::int32_div"));

        dict.set("uidiv", (rp.*fc)(&Lsc::vs::uint32_div, "Lsc::vs::uint32_div"));

        dict.set("ldiv", (rp.*fc)(&Lsc::vs::int64_div, "Lsc::vs::int64_div"));

        dict.set("uldiv", (rp.*fc)(&Lsc::vs::uint64_div, "Lsc::vs::uint64_div"));

        dict.set("fdiv", (rp.*fc)(&Lsc::vs::float_div, "Lsc::vs::float_div"));

        dict.set("ddiv", (rp.*fc)(&Lsc::vs::dolcle_div, "Lsc::vs::dolcle_div"));
        
        dict.set("tmod", (rp.*fc)(&Lsc::vs::int8_mod, "Lsc::vs::int8_mod"));

        dict.set("utmod", (rp.*fc)(&Lsc::vs::uint8_mod, "Lsc::vs::uint8_mod"));

        dict.set("hmod", (rp.*fc)(&Lsc::vs::int16_mod, "Lsc::vs::int16_mod"));

        dict.set("uhmod", (rp.*fc)(&Lsc::vs::uint16_mod, "Lsc::vs::uint16_mod"));

        dict.set("imod", (rp.*fc)(&Lsc::vs::int32_mod, "Lsc::vs::int32_mod"));

        dict.set("uimod", (rp.*fc)(&Lsc::vs::uint32_mod, "Lsc::vs::uint32_mod"));

        dict.set("lmod", (rp.*fc)(&Lsc::vs::int64_mod, "Lsc::vs::int64_mod"));

        dict.set("ulmod", (rp.*fc)(&Lsc::vs::uint64_mod, "Lsc::vs::uint64_mod"));

        return Lsc::var::Null::null;            
    }

}} 

