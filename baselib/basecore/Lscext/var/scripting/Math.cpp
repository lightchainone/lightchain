#include <Lsc/var/implement.h>
#include "Math.h"

namespace Lsc{ namespace var{
    
    
    Lsc::var::IVar* int8_neg( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        signed char res = (signed char)( - ( args[0]->to_int8() ) );
        return &( rp.create<Lsc::var::Number<signed char> >( res ) );
    }

    
    Lsc::var::IVar* uint8_neg( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        unsigned char res = (unsigned char)( - ( args[0]->to_uint8() ) );
        return &( rp.create<Lsc::var::Number<unsigned char> >( res ) );
    }

    
    Lsc::var::IVar* int16_neg( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        signed short res = (signed short)( - ( args[0]->to_int16() ) );
        return &( rp.create<Lsc::var::Number<signed short> >( res ) );
    }

    
    Lsc::var::IVar* uint16_neg( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        unsigned short res = (unsigned short)( - ( args[0]->to_uint16() ) );
        return &( rp.create<Lsc::var::Number<unsigned short> >( res ) );
    }

    
    Lsc::var::IVar* int32_neg( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        signed int res = (signed int)( - ( args[0]->to_int32() ) );
        return &( rp.create<Lsc::var::Number<signed int> >( res ) );
    }

    
    Lsc::var::IVar* uint32_neg( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        unsigned int res = (unsigned int)( - ( args[0]->to_uint32() ) );
        return &( rp.create<Lsc::var::Number<unsigned int> >( res ) );
    }

    
    Lsc::var::IVar* int64_neg( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        signed long long res = (signed long long)( - ( args[0]->to_int64() ) );
        return &( rp.create<Lsc::var::Number<signed long long> >( res ) );
    }

    
    Lsc::var::IVar* uint64_neg( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        unsigned long long res = (unsigned long long)( - ( args[0]->to_uint64() ) );
        return &( rp.create<Lsc::var::Number<unsigned long long> >( res ) );
    }

    
    Lsc::var::IVar* float_neg( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        float res = (float)( - ( args[0]->to_float() ) );
        return &( rp.create<Lsc::var::Number<float> >( res ) );
    }

    
    Lsc::var::IVar* dolcle_neg( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        dolcle res = (dolcle)( - ( args[0]->to_dolcle() ) );
        return &( rp.create<Lsc::var::Number<dolcle> >( res ) );
    }

    
    Lsc::var::IVar* int8_posi( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        signed char res = (signed char)( + ( args[0]->to_int8() ) );
        return &( rp.create<Lsc::var::Number<signed char> >( res ) );
    }

    
    Lsc::var::IVar* uint8_posi( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        unsigned char res = (unsigned char)( + ( args[0]->to_uint8() ) );
        return &( rp.create<Lsc::var::Number<unsigned char> >( res ) );
    }

    
    Lsc::var::IVar* int16_posi( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        signed short res = (signed short)( + ( args[0]->to_int16() ) );
        return &( rp.create<Lsc::var::Number<signed short> >( res ) );
    }

    
    Lsc::var::IVar* uint16_posi( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        unsigned short res = (unsigned short)( + ( args[0]->to_uint16() ) );
        return &( rp.create<Lsc::var::Number<unsigned short> >( res ) );
    }

    
    Lsc::var::IVar* int32_posi( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        signed int res = (signed int)( + ( args[0]->to_int32() ) );
        return &( rp.create<Lsc::var::Number<signed int> >( res ) );
    }

    
    Lsc::var::IVar* uint32_posi( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        unsigned int res = (unsigned int)( + ( args[0]->to_uint32() ) );
        return &( rp.create<Lsc::var::Number<unsigned int> >( res ) );
    }

    
    Lsc::var::IVar* int64_posi( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        signed long long res = (signed long long)( + ( args[0]->to_int64() ) );
        return &( rp.create<Lsc::var::Number<signed long long> >( res ) );
    }

    
    Lsc::var::IVar* uint64_posi( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        unsigned long long res = (unsigned long long)( + ( args[0]->to_uint64() ) );
        return &( rp.create<Lsc::var::Number<unsigned long long> >( res ) );
    }

    
    Lsc::var::IVar* float_posi( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        float res = (float)( + ( args[0]->to_float() ) );
        return &( rp.create<Lsc::var::Number<float> >( res ) );
    }

    
    Lsc::var::IVar* dolcle_posi( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        dolcle res = (dolcle)( + ( args[0]->to_dolcle() ) );
        return &( rp.create<Lsc::var::Number<dolcle> >( res ) );
    }
    
    
    Lsc::var::IVar* int8_equal( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        return &( rp.create<Lsc::var::Bool>( args[0]->to_int8() == args[1]->to_int8() ) );
    }

    
    Lsc::var::IVar* uint8_equal( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        return &( rp.create<Lsc::var::Bool>( args[0]->to_uint8() == args[1]->to_uint8() ) );
    }

    
    Lsc::var::IVar* int16_equal( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        return &( rp.create<Lsc::var::Bool>( args[0]->to_int16() == args[1]->to_int16() ) );
    }

    
    Lsc::var::IVar* uint16_equal( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        return &( rp.create<Lsc::var::Bool>( args[0]->to_uint16() == args[1]->to_uint16() ) );
    }

    
    Lsc::var::IVar* int32_equal( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        return &( rp.create<Lsc::var::Bool>( args[0]->to_int32() == args[1]->to_int32() ) );
    }

    
    Lsc::var::IVar* uint32_equal( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        return &( rp.create<Lsc::var::Bool>( args[0]->to_uint32() == args[1]->to_uint32() ) );
    }

    
    Lsc::var::IVar* int64_equal( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        return &( rp.create<Lsc::var::Bool>( args[0]->to_int64() == args[1]->to_int64() ) );
    }

    
    Lsc::var::IVar* uint64_equal( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        return &( rp.create<Lsc::var::Bool>( args[0]->to_uint64() == args[1]->to_uint64() ) );
    }

    
    Lsc::var::IVar* float_equal( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        return &( rp.create<Lsc::var::Bool>( args[0]->to_float() == args[1]->to_float() ) );
    }

    
    Lsc::var::IVar* dolcle_equal( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        return &( rp.create<Lsc::var::Bool>( args[0]->to_dolcle() == args[1]->to_dolcle() ) );
    }

    
    Lsc::var::IVar* int8_not_equal( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        return &( rp.create<Lsc::var::Bool>( args[0]->to_int8() != args[1]->to_int8() ) );
    }

    
    Lsc::var::IVar* uint8_not_equal( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        return &( rp.create<Lsc::var::Bool>( args[0]->to_uint8() != args[1]->to_uint8() ) );
    }

    
    Lsc::var::IVar* int16_not_equal( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        return &( rp.create<Lsc::var::Bool>( args[0]->to_int16() != args[1]->to_int16() ) );
    }

    
    Lsc::var::IVar* uint16_not_equal( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        return &( rp.create<Lsc::var::Bool>( args[0]->to_uint16() != args[1]->to_uint16() ) );
    }

    
    Lsc::var::IVar* int32_not_equal( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        return &( rp.create<Lsc::var::Bool>( args[0]->to_int32() != args[1]->to_int32() ) );
    }

    
    Lsc::var::IVar* uint32_not_equal( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        return &( rp.create<Lsc::var::Bool>( args[0]->to_uint32() != args[1]->to_uint32() ) );
    }

    
    Lsc::var::IVar* int64_not_equal( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        return &( rp.create<Lsc::var::Bool>( args[0]->to_int64() != args[1]->to_int64() ) );
    }

    
    Lsc::var::IVar* uint64_not_equal( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        return &( rp.create<Lsc::var::Bool>( args[0]->to_uint64() != args[1]->to_uint64() ) );
    }

    
    Lsc::var::IVar* float_not_equal( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        return &( rp.create<Lsc::var::Bool>( args[0]->to_float() != args[1]->to_float() ) );
    }

    
    Lsc::var::IVar* dolcle_not_equal( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        return &( rp.create<Lsc::var::Bool>( args[0]->to_dolcle() != args[1]->to_dolcle() ) );
    }

    
    Lsc::var::IVar* int8_less( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        return &( rp.create<Lsc::var::Bool>( args[0]->to_int8() < args[1]->to_int8() ) );
    }

    
    Lsc::var::IVar* uint8_less( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        return &( rp.create<Lsc::var::Bool>( args[0]->to_uint8() < args[1]->to_uint8() ) );
    }

    
    Lsc::var::IVar* int16_less( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        return &( rp.create<Lsc::var::Bool>( args[0]->to_int16() < args[1]->to_int16() ) );
    }

    
    Lsc::var::IVar* uint16_less( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        return &( rp.create<Lsc::var::Bool>( args[0]->to_uint16() < args[1]->to_uint16() ) );
    }

    
    Lsc::var::IVar* int32_less( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        return &( rp.create<Lsc::var::Bool>( args[0]->to_int32() < args[1]->to_int32() ) );
    }

    
    Lsc::var::IVar* uint32_less( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        return &( rp.create<Lsc::var::Bool>( args[0]->to_uint32() < args[1]->to_uint32() ) );
    }

    
    Lsc::var::IVar* int64_less( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        return &( rp.create<Lsc::var::Bool>( args[0]->to_int64() < args[1]->to_int64() ) );
    }

    
    Lsc::var::IVar* uint64_less( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        return &( rp.create<Lsc::var::Bool>( args[0]->to_uint64() < args[1]->to_uint64() ) );
    }

    
    Lsc::var::IVar* float_less( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        return &( rp.create<Lsc::var::Bool>( args[0]->to_float() < args[1]->to_float() ) );
    }

    
    Lsc::var::IVar* dolcle_less( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        return &( rp.create<Lsc::var::Bool>( args[0]->to_dolcle() < args[1]->to_dolcle() ) );
    }

    
    Lsc::var::IVar* int8_less_equal( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        return &( rp.create<Lsc::var::Bool>( args[0]->to_int8() <= args[1]->to_int8() ) );
    }

    
    Lsc::var::IVar* uint8_less_equal( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        return &( rp.create<Lsc::var::Bool>( args[0]->to_uint8() <= args[1]->to_uint8() ) );
    }

    
    Lsc::var::IVar* int16_less_equal( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        return &( rp.create<Lsc::var::Bool>( args[0]->to_int16() <= args[1]->to_int16() ) );
    }

    
    Lsc::var::IVar* uint16_less_equal( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        return &( rp.create<Lsc::var::Bool>( args[0]->to_uint16() <= args[1]->to_uint16() ) );
    }

    
    Lsc::var::IVar* int32_less_equal( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        return &( rp.create<Lsc::var::Bool>( args[0]->to_int32() <= args[1]->to_int32() ) );
    }

    
    Lsc::var::IVar* uint32_less_equal( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        return &( rp.create<Lsc::var::Bool>( args[0]->to_uint32() <= args[1]->to_uint32() ) );
    }

    
    Lsc::var::IVar* int64_less_equal( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        return &( rp.create<Lsc::var::Bool>( args[0]->to_int64() <= args[1]->to_int64() ) );
    }

    
    Lsc::var::IVar* uint64_less_equal( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        return &( rp.create<Lsc::var::Bool>( args[0]->to_uint64() <= args[1]->to_uint64() ) );
    }

    
    Lsc::var::IVar* float_less_equal( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        return &( rp.create<Lsc::var::Bool>( args[0]->to_float() <= args[1]->to_float() ) );
    }

    
    Lsc::var::IVar* dolcle_less_equal( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        return &( rp.create<Lsc::var::Bool>( args[0]->to_dolcle() <= args[1]->to_dolcle() ) );
    }

    
    Lsc::var::IVar* int8_greater( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        return &( rp.create<Lsc::var::Bool>( args[0]->to_int8() > args[1]->to_int8() ) );
    }

    
    Lsc::var::IVar* uint8_greater( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        return &( rp.create<Lsc::var::Bool>( args[0]->to_uint8() > args[1]->to_uint8() ) );
    }

    
    Lsc::var::IVar* int16_greater( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        return &( rp.create<Lsc::var::Bool>( args[0]->to_int16() > args[1]->to_int16() ) );
    }

    
    Lsc::var::IVar* uint16_greater( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        return &( rp.create<Lsc::var::Bool>( args[0]->to_uint16() > args[1]->to_uint16() ) );
    }

    
    Lsc::var::IVar* int32_greater( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        return &( rp.create<Lsc::var::Bool>( args[0]->to_int32() > args[1]->to_int32() ) );
    }

    
    Lsc::var::IVar* uint32_greater( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        return &( rp.create<Lsc::var::Bool>( args[0]->to_uint32() > args[1]->to_uint32() ) );
    }

    
    Lsc::var::IVar* int64_greater( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        return &( rp.create<Lsc::var::Bool>( args[0]->to_int64() > args[1]->to_int64() ) );
    }

    
    Lsc::var::IVar* uint64_greater( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        return &( rp.create<Lsc::var::Bool>( args[0]->to_uint64() > args[1]->to_uint64() ) );
    }

    
    Lsc::var::IVar* float_greater( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        return &( rp.create<Lsc::var::Bool>( args[0]->to_float() > args[1]->to_float() ) );
    }

    
    Lsc::var::IVar* dolcle_greater( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        return &( rp.create<Lsc::var::Bool>( args[0]->to_dolcle() > args[1]->to_dolcle() ) );
    }

    
    Lsc::var::IVar* int8_greater_equal( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        return &( rp.create<Lsc::var::Bool>( args[0]->to_int8() >= args[1]->to_int8() ) );
    }

    
    Lsc::var::IVar* uint8_greater_equal( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        return &( rp.create<Lsc::var::Bool>( args[0]->to_uint8() >= args[1]->to_uint8() ) );
    }

    
    Lsc::var::IVar* int16_greater_equal( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        return &( rp.create<Lsc::var::Bool>( args[0]->to_int16() >= args[1]->to_int16() ) );
    }

    
    Lsc::var::IVar* uint16_greater_equal( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        return &( rp.create<Lsc::var::Bool>( args[0]->to_uint16() >= args[1]->to_uint16() ) );
    }

    
    Lsc::var::IVar* int32_greater_equal( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        return &( rp.create<Lsc::var::Bool>( args[0]->to_int32() >= args[1]->to_int32() ) );
    }

    
    Lsc::var::IVar* uint32_greater_equal( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        return &( rp.create<Lsc::var::Bool>( args[0]->to_uint32() >= args[1]->to_uint32() ) );
    }

    
    Lsc::var::IVar* int64_greater_equal( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        return &( rp.create<Lsc::var::Bool>( args[0]->to_int64() >= args[1]->to_int64() ) );
    }

    
    Lsc::var::IVar* uint64_greater_equal( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        return &( rp.create<Lsc::var::Bool>( args[0]->to_uint64() >= args[1]->to_uint64() ) );
    }

    
    Lsc::var::IVar* float_greater_equal( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        return &( rp.create<Lsc::var::Bool>( args[0]->to_float() >= args[1]->to_float() ) );
    }

    
    Lsc::var::IVar* dolcle_greater_equal( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        return &( rp.create<Lsc::var::Bool>( args[0]->to_dolcle() >= args[1]->to_dolcle() ) );
    }
    
    
    Lsc::var::IVar* int8_add( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        signed char res = (signed char)( ( args[0]->to_int8() ) + ( args[1]->to_int8() ) );
        return &( rp.create<Lsc::var::Number<signed char> >( res ) );
    }

    
    Lsc::var::IVar* uint8_add( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        unsigned char res = (unsigned char)( ( args[0]->to_uint8() ) + ( args[1]->to_uint8() ) );
        return &( rp.create<Lsc::var::Number<unsigned char> >( res ) );
    }

    
    Lsc::var::IVar* int16_add( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        signed short res = (signed short)( ( args[0]->to_int16() ) + ( args[1]->to_int16() ) );
        return &( rp.create<Lsc::var::Number<signed short> >( res ) );
    }

    
    Lsc::var::IVar* uint16_add( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        unsigned short res = (unsigned short)( ( args[0]->to_uint16() ) + ( args[1]->to_uint16() ) );
        return &( rp.create<Lsc::var::Number<unsigned short> >( res ) );
    }

    
    Lsc::var::IVar* int32_add( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        signed int res = (signed int)( ( args[0]->to_int32() ) + ( args[1]->to_int32() ) );
        return &( rp.create<Lsc::var::Number<signed int> >( res ) );
    }

    
    Lsc::var::IVar* uint32_add( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        unsigned int res = (unsigned int)( ( args[0]->to_uint32() ) + ( args[1]->to_uint32() ) );
        return &( rp.create<Lsc::var::Number<unsigned int> >( res ) );
    }

    
    Lsc::var::IVar* int64_add( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        signed long long res = (signed long long)( ( args[0]->to_int64() ) + ( args[1]->to_int64() ) );
        return &( rp.create<Lsc::var::Number<signed long long> >( res ) );
    }

    
    Lsc::var::IVar* uint64_add( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        unsigned long long res = (unsigned long long)( ( args[0]->to_uint64() ) + ( args[1]->to_uint64() ) );
        return &( rp.create<Lsc::var::Number<unsigned long long> >( res ) );
    }

    
    Lsc::var::IVar* float_add( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        float res = (float)( ( args[0]->to_float() ) + ( args[1]->to_float() ) );
        return &( rp.create<Lsc::var::Number<float> >( res ) );
    }

    
    Lsc::var::IVar* dolcle_add( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        dolcle res = (dolcle)( ( args[0]->to_dolcle() ) + ( args[1]->to_dolcle() ) );
        return &( rp.create<Lsc::var::Number<dolcle> >( res ) );
    }

    
    Lsc::var::IVar* int8_slc( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        signed char res = (signed char)( ( args[0]->to_int8() ) - ( args[1]->to_int8() ) );
        return &( rp.create<Lsc::var::Number<signed char> >( res ) );
    }

    
    Lsc::var::IVar* uint8_slc( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        unsigned char res = (unsigned char)( ( args[0]->to_uint8() ) - ( args[1]->to_uint8() ) );
        return &( rp.create<Lsc::var::Number<unsigned char> >( res ) );
    }

    
    Lsc::var::IVar* int16_slc( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        signed short res = (signed short)( ( args[0]->to_int16() ) - ( args[1]->to_int16() ) );
        return &( rp.create<Lsc::var::Number<signed short> >( res ) );
    }

    
    Lsc::var::IVar* uint16_slc( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        unsigned short res = (unsigned short)( ( args[0]->to_uint16() ) - ( args[1]->to_uint16() ) );
        return &( rp.create<Lsc::var::Number<unsigned short> >( res ) );
    }

    
    Lsc::var::IVar* int32_slc( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        signed int res = (signed int)( ( args[0]->to_int32() ) - ( args[1]->to_int32() ) );
        return &( rp.create<Lsc::var::Number<signed int> >( res ) );
    }

    
    Lsc::var::IVar* uint32_slc( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        unsigned int res = (unsigned int)( ( args[0]->to_uint32() ) - ( args[1]->to_uint32() ) );
        return &( rp.create<Lsc::var::Number<unsigned int> >( res ) );
    }

    
    Lsc::var::IVar* int64_slc( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        signed long long res = (signed long long)( ( args[0]->to_int64() ) - ( args[1]->to_int64() ) );
        return &( rp.create<Lsc::var::Number<signed long long> >( res ) );
    }

    
    Lsc::var::IVar* uint64_slc( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        unsigned long long res = (unsigned long long)( ( args[0]->to_uint64() ) - ( args[1]->to_uint64() ) );
        return &( rp.create<Lsc::var::Number<unsigned long long> >( res ) );
    }

    
    Lsc::var::IVar* float_slc( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        float res = (float)( ( args[0]->to_float() ) - ( args[1]->to_float() ) );
        return &( rp.create<Lsc::var::Number<float> >( res ) );
    }

    
    Lsc::var::IVar* dolcle_slc( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        dolcle res = (dolcle)( ( args[0]->to_dolcle() ) - ( args[1]->to_dolcle() ) );
        return &( rp.create<Lsc::var::Number<dolcle> >( res ) );
    }

    
    Lsc::var::IVar* int8_mul( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        signed char res = (signed char)( ( args[0]->to_int8() ) * ( args[1]->to_int8() ) );
        return &( rp.create<Lsc::var::Number<signed char> >( res ) );
    }

    
    Lsc::var::IVar* uint8_mul( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        unsigned char res = (unsigned char)( ( args[0]->to_uint8() ) * ( args[1]->to_uint8() ) );
        return &( rp.create<Lsc::var::Number<unsigned char> >( res ) );
    }

    
    Lsc::var::IVar* int16_mul( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        signed short res = (signed short)( ( args[0]->to_int16() ) * ( args[1]->to_int16() ) );
        return &( rp.create<Lsc::var::Number<signed short> >( res ) );
    }

    
    Lsc::var::IVar* uint16_mul( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        unsigned short res = (unsigned short)( ( args[0]->to_uint16() ) * ( args[1]->to_uint16() ) );
        return &( rp.create<Lsc::var::Number<unsigned short> >( res ) );
    }

    
    Lsc::var::IVar* int32_mul( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        signed int res = (signed int)( ( args[0]->to_int32() ) * ( args[1]->to_int32() ) );
        return &( rp.create<Lsc::var::Number<signed int> >( res ) );
    }

    
    Lsc::var::IVar* uint32_mul( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        unsigned int res = (unsigned int)( ( args[0]->to_uint32() ) * ( args[1]->to_uint32() ) );
        return &( rp.create<Lsc::var::Number<unsigned int> >( res ) );
    }

    
    Lsc::var::IVar* int64_mul( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        signed long long res = (signed long long)( ( args[0]->to_int64() ) * ( args[1]->to_int64() ) );
        return &( rp.create<Lsc::var::Number<signed long long> >( res ) );
    }

    
    Lsc::var::IVar* uint64_mul( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        unsigned long long res = (unsigned long long)( ( args[0]->to_uint64() ) * ( args[1]->to_uint64() ) );
        return &( rp.create<Lsc::var::Number<unsigned long long> >( res ) );
    }

    
    Lsc::var::IVar* float_mul( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        float res = (float)( ( args[0]->to_float() ) * ( args[1]->to_float() ) );
        return &( rp.create<Lsc::var::Number<float> >( res ) );
    }

    
    Lsc::var::IVar* dolcle_mul( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        dolcle res = (dolcle)( ( args[0]->to_dolcle() ) * ( args[1]->to_dolcle() ) );
        return &( rp.create<Lsc::var::Number<dolcle> >( res ) );
    }
    
    
    Lsc::var::IVar* int8_div( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        signed char var1 = args[0]->to_int8();
        signed char var2 = args[1]->to_int8();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for int8_div."; 
        }
        signed char res = (signed char)( var1 / var2 );
        return &( rp.create<Lsc::var::Number<signed char> >( res ) );
    }

    
    Lsc::var::IVar* uint8_div( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        unsigned char var1 = args[0]->to_uint8();
        unsigned char var2 = args[1]->to_uint8();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for uint8_div."; 
        }
        unsigned char res = (unsigned char)( var1 / var2 );
        return &( rp.create<Lsc::var::Number<unsigned char> >( res ) );
    }

    
    Lsc::var::IVar* int16_div( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        signed short var1 = args[0]->to_int16();
        signed short var2 = args[1]->to_int16();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for int16_div."; 
        }
        signed short res = (signed short)( var1 / var2 );
        return &( rp.create<Lsc::var::Number<signed short> >( res ) );
    }

    
    Lsc::var::IVar* uint16_div( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        unsigned short var1 = args[0]->to_uint16();
        unsigned short var2 = args[1]->to_uint16();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for uint16_div."; 
        }
        unsigned short res = (unsigned short)( var1 / var2 );
        return &( rp.create<Lsc::var::Number<unsigned short> >( res ) );
    }

    
    Lsc::var::IVar* int32_div( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        signed int var1 = args[0]->to_int32();
        signed int var2 = args[1]->to_int32();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for int32_div."; 
        }
        signed int res = (signed int)( var1 / var2 );
        return &( rp.create<Lsc::var::Number<signed int> >( res ) );
    }

    
    Lsc::var::IVar* uint32_div( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        unsigned int var1 = args[0]->to_uint32();
        unsigned int var2 = args[1]->to_uint32();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for uint32_div."; 
        }
        unsigned int res = (unsigned int)( var1 / var2 );
        return &( rp.create<Lsc::var::Number<unsigned int> >( res ) );
    }

    
    Lsc::var::IVar* int64_div( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        signed long long var1 = args[0]->to_int64();
        signed long long var2 = args[1]->to_int64();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for int64_div."; 
        }
        signed long long res = (signed long long)( var1 / var2 );
        return &( rp.create<Lsc::var::Number<signed long long> >( res ) );
    }

    
    Lsc::var::IVar* uint64_div( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        unsigned long long var1 = args[0]->to_uint64();
        unsigned long long var2 = args[1]->to_uint64();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for uint64_div."; 
        }
        unsigned long long res = (unsigned long long)( var1 / var2 );
        return &( rp.create<Lsc::var::Number<unsigned long long> >( res ) );
    }

    
    Lsc::var::IVar* float_div( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        float var1 = args[0]->to_float();
        float var2 = args[1]->to_float();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for float_div."; 
        }
        float res = (float)( var1 / var2 );
        return &( rp.create<Lsc::var::Number<float> >( res ) );
    }

    
    Lsc::var::IVar* dolcle_div( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        dolcle var1 = args[0]->to_dolcle();
        dolcle var2 = args[1]->to_dolcle();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for dolcle_div."; 
        }
        dolcle res = (dolcle)( var1 / var2 );
        return &( rp.create<Lsc::var::Number<dolcle> >( res ) );
    }
    
    
    Lsc::var::IVar* int8_mod( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        signed char var1 = args[0]->to_int8();
        signed char var2 = args[1]->to_int8();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for int8_mod."; 
        }
        signed char res = (signed char)( var1 % var2 );
        return &( rp.create<Lsc::var::Number<signed char> >( res ) );
    }

    
    Lsc::var::IVar* uint8_mod( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        unsigned char var1 = args[0]->to_uint8();
        unsigned char var2 = args[1]->to_uint8();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for uint8_mod."; 
        }
        unsigned char res = (unsigned char)( var1 % var2 );
        return &( rp.create<Lsc::var::Number<unsigned char> >( res ) );
    }

    
    Lsc::var::IVar* int16_mod( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        signed short var1 = args[0]->to_int16();
        signed short var2 = args[1]->to_int16();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for int16_mod."; 
        }
        signed short res = (signed short)( var1 % var2 );
        return &( rp.create<Lsc::var::Number<signed short> >( res ) );
    }

    
    Lsc::var::IVar* uint16_mod( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        unsigned short var1 = args[0]->to_uint16();
        unsigned short var2 = args[1]->to_uint16();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for uint16_mod."; 
        }
        unsigned short res = (unsigned short)( var1 % var2 );
        return &( rp.create<Lsc::var::Number<unsigned short> >( res ) );
    }

    
    Lsc::var::IVar* int32_mod( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        signed int var1 = args[0]->to_int32();
        signed int var2 = args[1]->to_int32();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for int32_mod."; 
        }
        signed int res = (signed int)( var1 % var2 );
        return &( rp.create<Lsc::var::Number<signed int> >( res ) );
    }

    
    Lsc::var::IVar* uint32_mod( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        unsigned int var1 = args[0]->to_uint32();
        unsigned int var2 = args[1]->to_uint32();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for uint32_mod."; 
        }
        unsigned int res = (unsigned int)( var1 % var2 );
        return &( rp.create<Lsc::var::Number<unsigned int> >( res ) );
    }

    
    Lsc::var::IVar* int64_mod( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        signed long long var1 = args[0]->to_int64();
        signed long long var2 = args[1]->to_int64();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for int64_mod."; 
        }
        signed long long res = (signed long long)( var1 % var2 );
        return &( rp.create<Lsc::var::Number<signed long long> >( res ) );
    }

    
    Lsc::var::IVar* uint64_mod( Lsc::var::IVar** args, Lsc::ResourcePool& rp ){
        unsigned long long var1 = args[0]->to_uint64();
        unsigned long long var2 = args[1]->to_uint64();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for uint64_mod."; 
        }
        unsigned long long res = (unsigned long long)( var1 % var2 );
        return &( rp.create<Lsc::var::Number<unsigned long long> >( res ) );
    }
    
    
    Lsc::var::IVar* float_mod( Lsc::var::IVar** , Lsc::ResourcePool&  ){
        throw Lsc::BadArgumentException()<<BSL_EARG<<"float arguments for float_mod."; 
    }

    
    Lsc::var::IVar* dolcle_mod( Lsc::var::IVar** , Lsc::ResourcePool&  ){
        throw Lsc::BadArgumentException()<<BSL_EARG<<"float arguments for dolcle_mod."; 
    }
    
    
    Lsc::var::IVar* int8_add_assign( Lsc::var::IVar** args, Lsc::ResourcePool&  ){
        signed char res = (signed char)( ( args[0]->to_int8() ) + ( args[1]->to_int8() ) );
        return &( args[0]->operator =( res ) );
    }

    
    Lsc::var::IVar* uint8_add_assign( Lsc::var::IVar** args, Lsc::ResourcePool&  ){
        unsigned char res = (unsigned char)( ( args[0]->to_uint8() ) + ( args[1]->to_uint8() ) );
        return &( args[0]->operator =( res ) );
    }

    
    Lsc::var::IVar* int16_add_assign( Lsc::var::IVar** args, Lsc::ResourcePool&  ){
        signed short res = (signed short)( ( args[0]->to_int16() ) + ( args[1]->to_int16() ) );
        return &( args[0]->operator =( res ) );
    }

    
    Lsc::var::IVar* uint16_add_assign( Lsc::var::IVar** args, Lsc::ResourcePool&  ){
        unsigned short res = (unsigned short)( ( args[0]->to_uint16() ) + ( args[1]->to_uint16() ) );
        return &( args[0]->operator =( res ) );
    }

    
    Lsc::var::IVar* int32_add_assign( Lsc::var::IVar** args, Lsc::ResourcePool&  ){
        signed int res = (signed int)( ( args[0]->to_int32() ) + ( args[1]->to_int32() ) );
        return &( args[0]->operator =( res ) );
    }

    
    Lsc::var::IVar* uint32_add_assign( Lsc::var::IVar** args, Lsc::ResourcePool&  ){
        unsigned int res = (unsigned int)( ( args[0]->to_uint32() ) + ( args[1]->to_uint32() ) );
        return &( args[0]->operator =( res ) );
    }

    
    Lsc::var::IVar* int64_add_assign( Lsc::var::IVar** args, Lsc::ResourcePool&  ){
        signed long long res = (signed long long)( ( args[0]->to_int64() ) + ( args[1]->to_int64() ) );
        return &( args[0]->operator =( res ) );
    }

    
    Lsc::var::IVar* uint64_add_assign( Lsc::var::IVar** args, Lsc::ResourcePool&  ){
        unsigned long long res = (unsigned long long)( ( args[0]->to_uint64() ) + ( args[1]->to_uint64() ) );
        return &( args[0]->operator =( res ) );
    }

    
    Lsc::var::IVar* float_add_assign( Lsc::var::IVar** args, Lsc::ResourcePool&  ){
        float res = (float)( ( args[0]->to_float() ) + ( args[1]->to_float() ) );
        return &( args[0]->operator =( res ) );
    }

    
    Lsc::var::IVar* dolcle_add_assign( Lsc::var::IVar** args, Lsc::ResourcePool&  ){
        dolcle res = (dolcle)( ( args[0]->to_dolcle() ) + ( args[1]->to_dolcle() ) );
        return &( args[0]->operator =( res ) );
    }

    
    Lsc::var::IVar* int8_slc_assign( Lsc::var::IVar** args, Lsc::ResourcePool&  ){
        signed char res = (signed char)( ( args[0]->to_int8() ) - ( args[1]->to_int8() ) );
        return &( args[0]->operator =( res ) );
    }

    
    Lsc::var::IVar* uint8_slc_assign( Lsc::var::IVar** args, Lsc::ResourcePool&  ){
        unsigned char res = (unsigned char)( ( args[0]->to_uint8() ) - ( args[1]->to_uint8() ) );
        return &( args[0]->operator =( res ) );
    }

    
    Lsc::var::IVar* int16_slc_assign( Lsc::var::IVar** args, Lsc::ResourcePool&  ){
        signed short res = (signed short)( ( args[0]->to_int16() ) - ( args[1]->to_int16() ) );
        return &( args[0]->operator =( res ) );
    }

    
    Lsc::var::IVar* uint16_slc_assign( Lsc::var::IVar** args, Lsc::ResourcePool&  ){
        unsigned short res = (unsigned short)( ( args[0]->to_uint16() ) - ( args[1]->to_uint16() ) );
        return &( args[0]->operator =( res ) );
    }

    
    Lsc::var::IVar* int32_slc_assign( Lsc::var::IVar** args, Lsc::ResourcePool&  ){
        signed int res = (signed int)( ( args[0]->to_int32() ) - ( args[1]->to_int32() ) );
        return &( args[0]->operator =( res ) );
    }

    
    Lsc::var::IVar* uint32_slc_assign( Lsc::var::IVar** args, Lsc::ResourcePool&  ){
        unsigned int res = (unsigned int)( ( args[0]->to_uint32() ) - ( args[1]->to_uint32() ) );
        return &( args[0]->operator =( res ) );
    }

    
    Lsc::var::IVar* int64_slc_assign( Lsc::var::IVar** args, Lsc::ResourcePool&  ){
        signed long long res = (signed long long)( ( args[0]->to_int64() ) - ( args[1]->to_int64() ) );
        return &( args[0]->operator =( res ) );
    }

    
    Lsc::var::IVar* uint64_slc_assign( Lsc::var::IVar** args, Lsc::ResourcePool&  ){
        unsigned long long res = (unsigned long long)( ( args[0]->to_uint64() ) - ( args[1]->to_uint64() ) );
        return &( args[0]->operator =( res ) );
    }

    
    Lsc::var::IVar* float_slc_assign( Lsc::var::IVar** args, Lsc::ResourcePool&  ){
        float res = (float)( ( args[0]->to_float() ) - ( args[1]->to_float() ) );
        return &( args[0]->operator =( res ) );
    }

    
    Lsc::var::IVar* dolcle_slc_assign( Lsc::var::IVar** args, Lsc::ResourcePool&  ){
        dolcle res = (dolcle)( ( args[0]->to_dolcle() ) - ( args[1]->to_dolcle() ) );
        return &( args[0]->operator =( res ) );
    }

    
    Lsc::var::IVar* int8_mul_assign( Lsc::var::IVar** args, Lsc::ResourcePool&  ){
        signed char res = (signed char)( ( args[0]->to_int8() ) * ( args[1]->to_int8() ) );
        return &( args[0]->operator =( res ) );
    }

    
    Lsc::var::IVar* uint8_mul_assign( Lsc::var::IVar** args, Lsc::ResourcePool&  ){
        unsigned char res = (unsigned char)( ( args[0]->to_uint8() ) * ( args[1]->to_uint8() ) );
        return &( args[0]->operator =( res ) );
    }

    
    Lsc::var::IVar* int16_mul_assign( Lsc::var::IVar** args, Lsc::ResourcePool&  ){
        signed short res = (signed short)( ( args[0]->to_int16() ) * ( args[1]->to_int16() ) );
        return &( args[0]->operator =( res ) );
    }

    
    Lsc::var::IVar* uint16_mul_assign( Lsc::var::IVar** args, Lsc::ResourcePool&  ){
        unsigned short res = (unsigned short)( ( args[0]->to_uint16() ) * ( args[1]->to_uint16() ) );
        return &( args[0]->operator =( res ) );
    }

    
    Lsc::var::IVar* int32_mul_assign( Lsc::var::IVar** args, Lsc::ResourcePool&  ){
        signed int res = (signed int)( ( args[0]->to_int32() ) * ( args[1]->to_int32() ) );
        return &( args[0]->operator =( res ) );
    }

    
    Lsc::var::IVar* uint32_mul_assign( Lsc::var::IVar** args, Lsc::ResourcePool&  ){
        unsigned int res = (unsigned int)( ( args[0]->to_uint32() ) * ( args[1]->to_uint32() ) );
        return &( args[0]->operator =( res ) );
    }

    
    Lsc::var::IVar* int64_mul_assign( Lsc::var::IVar** args, Lsc::ResourcePool&  ){
        signed long long res = (signed long long)( ( args[0]->to_int64() ) * ( args[1]->to_int64() ) );
        return &( args[0]->operator =( res ) );
    }

    
    Lsc::var::IVar* uint64_mul_assign( Lsc::var::IVar** args, Lsc::ResourcePool&  ){
        unsigned long long res = (unsigned long long)( ( args[0]->to_uint64() ) * ( args[1]->to_uint64() ) );
        return &( args[0]->operator =( res ) );
    }

    
    Lsc::var::IVar* float_mul_assign( Lsc::var::IVar** args, Lsc::ResourcePool&  ){
        float res = (float)( ( args[0]->to_float() ) * ( args[1]->to_float() ) );
        return &( args[0]->operator =( res ) );
    }

    
    Lsc::var::IVar* dolcle_mul_assign( Lsc::var::IVar** args, Lsc::ResourcePool&  ){
        dolcle res = (dolcle)( ( args[0]->to_dolcle() ) * ( args[1]->to_dolcle() ) );
        return &( args[0]->operator =( res ) );
    }
    
    
    Lsc::var::IVar* int8_div_assign( Lsc::var::IVar** args, Lsc::ResourcePool&  ){
        signed char var1 = args[0]->to_int8();
        signed char var2 = args[1]->to_int8();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for int8_div_assign.";
        }
        signed char res = (signed char)( var1 / var2 );
        return &( args[0]->operator =( res ) );
    }

    
    Lsc::var::IVar* uint8_div_assign( Lsc::var::IVar** args, Lsc::ResourcePool&  ){
        unsigned char var1 = args[0]->to_uint8();
        unsigned char var2 = args[1]->to_uint8();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for uint8_div_assign.";
        }
        unsigned char res = (unsigned char)( var1 / var2 );
        return &( args[0]->operator =( res ) );
    }

    
    Lsc::var::IVar* int16_div_assign( Lsc::var::IVar** args, Lsc::ResourcePool&  ){
        signed short var1 = args[0]->to_int16();
        signed short var2 = args[1]->to_int16();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for int16_div_assign.";
        }
        signed short res = (signed short)( var1 / var2 );
        return &( args[0]->operator =( res ) );
    }

    
    Lsc::var::IVar* uint16_div_assign( Lsc::var::IVar** args, Lsc::ResourcePool&  ){
        unsigned short var1 = args[0]->to_uint16();
        unsigned short var2 = args[1]->to_uint16();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for uint16_div_assign.";
        }
        unsigned short res = (unsigned short)( var1 / var2 );
        return &( args[0]->operator =( res ) );
    }

    
    Lsc::var::IVar* int32_div_assign( Lsc::var::IVar** args, Lsc::ResourcePool&  ){
        signed int var1 = args[0]->to_int32();
        signed int var2 = args[1]->to_int32();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for int32_div_assign.";
        }
        signed int res = (signed int)( var1 / var2 );
        return &( args[0]->operator =( res ) );
    }

    
    Lsc::var::IVar* uint32_div_assign( Lsc::var::IVar** args, Lsc::ResourcePool&  ){
        unsigned int var1 = args[0]->to_uint32();
        unsigned int var2 = args[1]->to_uint32();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for uint32_div_assign.";
        }
        unsigned int res = (unsigned int)( var1 / var2 );
        return &( args[0]->operator =( res ) );
    }

    
    Lsc::var::IVar* int64_div_assign( Lsc::var::IVar** args, Lsc::ResourcePool&  ){
        signed long long var1 = args[0]->to_int64();
        signed long long var2 = args[1]->to_int64();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for int64_div_assign.";
        }
        signed long long res = (signed long long)( var1 / var2 );
        return &( args[0]->operator =( res ) );
    }

    
    Lsc::var::IVar* uint64_div_assign( Lsc::var::IVar** args, Lsc::ResourcePool&  ){
        unsigned long long var1 = args[0]->to_uint64();
        unsigned long long var2 = args[1]->to_uint64();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for uint64_div_assign.";
        }
        unsigned long long res = (unsigned long long)( var1 / var2 );
        return &( args[0]->operator =( res ) );
    }

    
    Lsc::var::IVar* float_div_assign( Lsc::var::IVar** args, Lsc::ResourcePool&  ){
        float var1 = args[0]->to_float();
        float var2 = args[1]->to_float();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for float_div_assign.";
        }
        float res = (float)( var1 / var2 );
        return &( args[0]->operator =( res ) );
    }

    
    Lsc::var::IVar* dolcle_div_assign( Lsc::var::IVar** args, Lsc::ResourcePool&  ){
        dolcle var1 = args[0]->to_dolcle();
        dolcle var2 = args[1]->to_dolcle();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for dolcle_div_assign.";
        }
        dolcle res = (dolcle)( var1 / var2 );
        return &( args[0]->operator =( res ) );
    }
    
    
    Lsc::var::IVar* int8_mod_assign( Lsc::var::IVar** args, Lsc::ResourcePool&  ){
        signed char var1 = args[0]->to_int8();
        signed char var2 = args[1]->to_int8();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for int8_mod_assign."; 
        }
        signed char res = (signed char)( var1 % var2 );
        return &( args[0]->operator =( res ) );
    }

    
    Lsc::var::IVar* uint8_mod_assign( Lsc::var::IVar** args, Lsc::ResourcePool&  ){
        unsigned char var1 = args[0]->to_uint8();
        unsigned char var2 = args[1]->to_uint8();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for uint8_mod_assign."; 
        }
        unsigned char res = (unsigned char)( var1 % var2 );
        return &( args[0]->operator =( res ) );
    }

    
    Lsc::var::IVar* int16_mod_assign( Lsc::var::IVar** args, Lsc::ResourcePool&  ){
        signed short var1 = args[0]->to_int16();
        signed short var2 = args[1]->to_int16();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for int16_mod_assign."; 
        }
        signed short res = (signed short)( var1 % var2 );
        return &( args[0]->operator =( res ) );
    }

    
    Lsc::var::IVar* uint16_mod_assign( Lsc::var::IVar** args, Lsc::ResourcePool&  ){
        unsigned short var1 = args[0]->to_uint16();
        unsigned short var2 = args[1]->to_uint16();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for uint16_mod_assign."; 
        }
        unsigned short res = (unsigned short)( var1 % var2 );
        return &( args[0]->operator =( res ) );
    }

    
    Lsc::var::IVar* int32_mod_assign( Lsc::var::IVar** args, Lsc::ResourcePool&  ){
        signed int var1 = args[0]->to_int32();
        signed int var2 = args[1]->to_int32();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for int32_mod_assign."; 
        }
        signed int res = (signed int)( var1 % var2 );
        return &( args[0]->operator =( res ) );
    }

    
    Lsc::var::IVar* uint32_mod_assign( Lsc::var::IVar** args, Lsc::ResourcePool&  ){
        unsigned int var1 = args[0]->to_uint32();
        unsigned int var2 = args[1]->to_uint32();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for uint32_mod_assign."; 
        }
        unsigned int res = (unsigned int)( var1 % var2 );
        return &( args[0]->operator =( res ) );
    }

    
    Lsc::var::IVar* int64_mod_assign( Lsc::var::IVar** args, Lsc::ResourcePool&  ){
        signed long long var1 = args[0]->to_int64();
        signed long long var2 = args[1]->to_int64();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for int64_mod_assign."; 
        }
        signed long long res = (signed long long)( var1 % var2 );
        return &( args[0]->operator =( res ) );
    }

    
    Lsc::var::IVar* uint64_mod_assign( Lsc::var::IVar** args, Lsc::ResourcePool&  ){
        unsigned long long var1 = args[0]->to_uint64();
        unsigned long long var2 = args[1]->to_uint64();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for uint64_mod_assign."; 
        }
        unsigned long long res = (unsigned long long)( var1 % var2 );
        return &( args[0]->operator =( res ) );
    }
    
    
    Lsc::var::IVar* float_mod_assign( Lsc::var::IVar** , Lsc::ResourcePool&  ){
        throw Lsc::BadArgumentException()<<BSL_EARG<<"float arguments for float_mod_assign."; 
    }

    
    Lsc::var::IVar* dolcle_mod_assign( Lsc::var::IVar** , Lsc::ResourcePool&  ){
        throw Lsc::BadArgumentException()<<BSL_EARG<<"float arguments for dolcle_mod_assign."; 
    }

}} 

