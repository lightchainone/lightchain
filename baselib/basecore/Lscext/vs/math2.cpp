#include <Lsc/var/implement.h>
#include "math.h"

namespace Lsc{ namespace vs{
    
    
    Lsc::var::IVar& int8_add_assign( Lsc::var::IVar& args, Lsc::ResourcePool&  ){
        signed char res = (signed char)( ( args[0].to_int8() ) + ( args[1].to_int8() ) );
        return args[0] = res;
    }

    
    Lsc::var::IVar& uint8_add_assign( Lsc::var::IVar& args, Lsc::ResourcePool&  ){
        unsigned char res = (unsigned char)( ( args[0].to_uint8() ) + ( args[1].to_uint8() ) );
        return args[0] = res;
    }

    
    Lsc::var::IVar& int16_add_assign( Lsc::var::IVar& args, Lsc::ResourcePool&  ){
        signed short res = (signed short)( ( args[0].to_int16() ) + ( args[1].to_int16() ) );
        return args[0] = res;
    }

    
    Lsc::var::IVar& uint16_add_assign( Lsc::var::IVar& args, Lsc::ResourcePool&  ){
        unsigned short res = (unsigned short)( ( args[0].to_uint16() ) + ( args[1].to_uint16() ) );
        return args[0] = res;
    }

    
    Lsc::var::IVar& int32_add_assign( Lsc::var::IVar& args, Lsc::ResourcePool&  ){
        signed int res = (signed int)( ( args[0].to_int32() ) + ( args[1].to_int32() ) );
        return args[0] = res;
    }

    
    Lsc::var::IVar& uint32_add_assign( Lsc::var::IVar& args, Lsc::ResourcePool&  ){
        unsigned int res = (unsigned int)( ( args[0].to_uint32() ) + ( args[1].to_uint32() ) );
        return args[0] = res;
    }

    
    Lsc::var::IVar& int64_add_assign( Lsc::var::IVar& args, Lsc::ResourcePool&  ){
        signed long long res = (signed long long)( ( args[0].to_int64() ) + ( args[1].to_int64() ) );
        return args[0] = res;
    }

    
    Lsc::var::IVar& uint64_add_assign( Lsc::var::IVar& args, Lsc::ResourcePool&  ){
        unsigned long long res = (unsigned long long)( ( args[0].to_uint64() ) + ( args[1].to_uint64() ) );
        return args[0] = res;
    }

    
    Lsc::var::IVar& float_add_assign( Lsc::var::IVar& args, Lsc::ResourcePool&  ){
        float res = (float)( ( args[0].to_float() ) + ( args[1].to_float() ) );
        return args[0] = res;
    }

    
    Lsc::var::IVar& dolcle_add_assign( Lsc::var::IVar& args, Lsc::ResourcePool&  ){
        dolcle res = (dolcle)( ( args[0].to_dolcle() ) + ( args[1].to_dolcle() ) );
        return args[0] = res;
    }

    
    Lsc::var::IVar& int8_slc_assign( Lsc::var::IVar& args, Lsc::ResourcePool&  ){
        signed char res = (signed char)( ( args[0].to_int8() ) - ( args[1].to_int8() ) );
        return args[0] = res;
    }

    
    Lsc::var::IVar& uint8_slc_assign( Lsc::var::IVar& args, Lsc::ResourcePool&  ){
        unsigned char res = (unsigned char)( ( args[0].to_uint8() ) - ( args[1].to_uint8() ) );
        return args[0] = res;
    }

    
    Lsc::var::IVar& int16_slc_assign( Lsc::var::IVar& args, Lsc::ResourcePool&  ){
        signed short res = (signed short)( ( args[0].to_int16() ) - ( args[1].to_int16() ) );
        return args[0] = res;
    }

    
    Lsc::var::IVar& uint16_slc_assign( Lsc::var::IVar& args, Lsc::ResourcePool&  ){
        unsigned short res = (unsigned short)( ( args[0].to_uint16() ) - ( args[1].to_uint16() ) );
        return args[0] = res;
    }

    
    Lsc::var::IVar& int32_slc_assign( Lsc::var::IVar& args, Lsc::ResourcePool&  ){
        signed int res = (signed int)( ( args[0].to_int32() ) - ( args[1].to_int32() ) );
        return args[0] = res;
    }

    
    Lsc::var::IVar& uint32_slc_assign( Lsc::var::IVar& args, Lsc::ResourcePool&  ){
        unsigned int res = (unsigned int)( ( args[0].to_uint32() ) - ( args[1].to_uint32() ) );
        return args[0] = res;
    }

    
    Lsc::var::IVar& int64_slc_assign( Lsc::var::IVar& args, Lsc::ResourcePool&  ){
        signed long long res = (signed long long)( ( args[0].to_int64() ) - ( args[1].to_int64() ) );
        return args[0] = res;
    }

    
    Lsc::var::IVar& uint64_slc_assign( Lsc::var::IVar& args, Lsc::ResourcePool&  ){
        unsigned long long res = (unsigned long long)( ( args[0].to_uint64() ) - ( args[1].to_uint64() ) );
        return args[0] = res;
    }

    
    Lsc::var::IVar& float_slc_assign( Lsc::var::IVar& args, Lsc::ResourcePool&  ){
        float res = (float)( ( args[0].to_float() ) - ( args[1].to_float() ) );
        return args[0] = res;
    }

    
    Lsc::var::IVar& dolcle_slc_assign( Lsc::var::IVar& args, Lsc::ResourcePool&  ){
        dolcle res = (dolcle)( ( args[0].to_dolcle() ) - ( args[1].to_dolcle() ) );
        return args[0] = res;
    }

    
    Lsc::var::IVar& int8_mul_assign( Lsc::var::IVar& args, Lsc::ResourcePool&  ){
        signed char res = (signed char)( ( args[0].to_int8() ) * ( args[1].to_int8() ) );
        return args[0] = res;
    }

    
    Lsc::var::IVar& uint8_mul_assign( Lsc::var::IVar& args, Lsc::ResourcePool&  ){
        unsigned char res = (unsigned char)( ( args[0].to_uint8() ) * ( args[1].to_uint8() ) );
        return args[0] = res;
    }

    
    Lsc::var::IVar& int16_mul_assign( Lsc::var::IVar& args, Lsc::ResourcePool&  ){
        signed short res = (signed short)( ( args[0].to_int16() ) * ( args[1].to_int16() ) );
        return args[0] = res;
    }

    
    Lsc::var::IVar& uint16_mul_assign( Lsc::var::IVar& args, Lsc::ResourcePool&  ){
        unsigned short res = (unsigned short)( ( args[0].to_uint16() ) * ( args[1].to_uint16() ) );
        return args[0] = res;
    }

    
    Lsc::var::IVar& int32_mul_assign( Lsc::var::IVar& args, Lsc::ResourcePool&  ){
        signed int res = (signed int)( ( args[0].to_int32() ) * ( args[1].to_int32() ) );
        return args[0] = res;
    }

    
    Lsc::var::IVar& uint32_mul_assign( Lsc::var::IVar& args, Lsc::ResourcePool&  ){
        unsigned int res = (unsigned int)( ( args[0].to_uint32() ) * ( args[1].to_uint32() ) );
        return args[0] = res;
    }

    
    Lsc::var::IVar& int64_mul_assign( Lsc::var::IVar& args, Lsc::ResourcePool&  ){
        signed long long res = (signed long long)( ( args[0].to_int64() ) * ( args[1].to_int64() ) );
        return args[0] = res;
    }

    
    Lsc::var::IVar& uint64_mul_assign( Lsc::var::IVar& args, Lsc::ResourcePool&  ){
        unsigned long long res = (unsigned long long)( ( args[0].to_uint64() ) * ( args[1].to_uint64() ) );
        return args[0] = res;
    }

    
    Lsc::var::IVar& float_mul_assign( Lsc::var::IVar& args, Lsc::ResourcePool&  ){
        float res = (float)( ( args[0].to_float() ) * ( args[1].to_float() ) );
        return args[0] = res;
    }

    
    Lsc::var::IVar& dolcle_mul_assign( Lsc::var::IVar& args, Lsc::ResourcePool&  ){
        dolcle res = (dolcle)( ( args[0].to_dolcle() ) * ( args[1].to_dolcle() ) );
        return args[0] = res;
    }
    
    
    Lsc::var::IVar& int8_div_assign( Lsc::var::IVar& args, Lsc::ResourcePool&  ){
        signed char var1 = args[0].to_int8();
        signed char var2 = args[1].to_int8();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for int8_div_assign.";
        }
        signed char res = (signed char)( var1 / var2 );
        return args[0] = res;
    }

    
    Lsc::var::IVar& uint8_div_assign( Lsc::var::IVar& args, Lsc::ResourcePool&  ){
        unsigned char var1 = args[0].to_uint8();
        unsigned char var2 = args[1].to_uint8();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for uint8_div_assign.";
        }
        unsigned char res = (unsigned char)( var1 / var2 );
        return args[0] = res;
    }

    
    Lsc::var::IVar& int16_div_assign( Lsc::var::IVar& args, Lsc::ResourcePool&  ){
        signed short var1 = args[0].to_int16();
        signed short var2 = args[1].to_int16();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for int16_div_assign.";
        }
        signed short res = (signed short)( var1 / var2 );
        return args[0] = res;
    }

    
    Lsc::var::IVar& uint16_div_assign( Lsc::var::IVar& args, Lsc::ResourcePool&  ){
        unsigned short var1 = args[0].to_uint16();
        unsigned short var2 = args[1].to_uint16();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for uint16_div_assign.";
        }
        unsigned short res = (unsigned short)( var1 / var2 );
        return args[0] = res;
    }

    
    Lsc::var::IVar& int32_div_assign( Lsc::var::IVar& args, Lsc::ResourcePool&  ){
        signed int var1 = args[0].to_int32();
        signed int var2 = args[1].to_int32();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for int32_div_assign.";
        }
        signed int res = (signed int)( var1 / var2 );
        return args[0] = res;
    }

    
    Lsc::var::IVar& uint32_div_assign( Lsc::var::IVar& args, Lsc::ResourcePool&  ){
        unsigned int var1 = args[0].to_uint32();
        unsigned int var2 = args[1].to_uint32();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for uint32_div_assign.";
        }
        unsigned int res = (unsigned int)( var1 / var2 );
        return args[0] = res;
    }

    
    Lsc::var::IVar& int64_div_assign( Lsc::var::IVar& args, Lsc::ResourcePool&  ){
        signed long long var1 = args[0].to_int64();
        signed long long var2 = args[1].to_int64();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for int64_div_assign.";
        }
        signed long long res = (signed long long)( var1 / var2 );
        return args[0] = res;
    }

    
    Lsc::var::IVar& uint64_div_assign( Lsc::var::IVar& args, Lsc::ResourcePool&  ){
        unsigned long long var1 = args[0].to_uint64();
        unsigned long long var2 = args[1].to_uint64();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for uint64_div_assign.";
        }
        unsigned long long res = (unsigned long long)( var1 / var2 );
        return args[0] = res;
    }

    
    Lsc::var::IVar& float_div_assign( Lsc::var::IVar& args, Lsc::ResourcePool&  ){
        float var1 = args[0].to_float();
        float var2 = args[1].to_float();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for float_div_assign.";
        }
        float res = (float)( var1 / var2 );
        return args[0] = res;
    }

    
    Lsc::var::IVar& dolcle_div_assign( Lsc::var::IVar& args, Lsc::ResourcePool&  ){
        dolcle var1 = args[0].to_dolcle();
        dolcle var2 = args[1].to_dolcle();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for dolcle_div_assign.";
        }
        dolcle res = (dolcle)( var1 / var2 );
        return args[0] = res;
    }
    
    
    Lsc::var::IVar& int8_mod_assign( Lsc::var::IVar& args, Lsc::ResourcePool&  ){
        signed char var1 = args[0].to_int8();
        signed char var2 = args[1].to_int8();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for int8_mod_assign."; 
        }
        signed char res = (signed char)( var1 % var2 );
        return args[0] = res;
    }

    
    Lsc::var::IVar& uint8_mod_assign( Lsc::var::IVar& args, Lsc::ResourcePool&  ){
        unsigned char var1 = args[0].to_uint8();
        unsigned char var2 = args[1].to_uint8();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for uint8_mod_assign."; 
        }
        unsigned char res = (unsigned char)( var1 % var2 );
        return args[0] = res;
    }

    
    Lsc::var::IVar& int16_mod_assign( Lsc::var::IVar& args, Lsc::ResourcePool&  ){
        signed short var1 = args[0].to_int16();
        signed short var2 = args[1].to_int16();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for int16_mod_assign."; 
        }
        signed short res = (signed short)( var1 % var2 );
        return args[0] = res;
    }

    
    Lsc::var::IVar& uint16_mod_assign( Lsc::var::IVar& args, Lsc::ResourcePool&  ){
        unsigned short var1 = args[0].to_uint16();
        unsigned short var2 = args[1].to_uint16();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for uint16_mod_assign."; 
        }
        unsigned short res = (unsigned short)( var1 % var2 );
        return args[0] = res;
    }

    
    Lsc::var::IVar& int32_mod_assign( Lsc::var::IVar& args, Lsc::ResourcePool&  ){
        signed int var1 = args[0].to_int32();
        signed int var2 = args[1].to_int32();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for int32_mod_assign."; 
        }
        signed int res = (signed int)( var1 % var2 );
        return args[0] = res;
    }

    
    Lsc::var::IVar& uint32_mod_assign( Lsc::var::IVar& args, Lsc::ResourcePool&  ){
        unsigned int var1 = args[0].to_uint32();
        unsigned int var2 = args[1].to_uint32();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for uint32_mod_assign."; 
        }
        unsigned int res = (unsigned int)( var1 % var2 );
        return args[0] = res;
    }

    
    Lsc::var::IVar& int64_mod_assign( Lsc::var::IVar& args, Lsc::ResourcePool&  ){
        signed long long var1 = args[0].to_int64();
        signed long long var2 = args[1].to_int64();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for int64_mod_assign."; 
        }
        signed long long res = (signed long long)( var1 % var2 );
        return args[0] = res;
    }

    
    Lsc::var::IVar& uint64_mod_assign( Lsc::var::IVar& args, Lsc::ResourcePool&  ){
        unsigned long long var1 = args[0].to_uint64();
        unsigned long long var2 = args[1].to_uint64();
        if(0 == var2){
            throw Lsc::BadArgumentException()<<BSL_EARG<<"zero divisor for uint64_mod_assign."; 
        }
        unsigned long long res = (unsigned long long)( var1 % var2 );
        return args[0] = res;
    }

    
    Lsc::var::IVar& import_math2( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        Lsc::var::IVar& dict = args.get(0);
        
        typedef Lsc::var::Function& (Lsc::ResourcePool::*FuncCreater)(
            const Lsc::var::Function::function_type& , const Lsc::var::Function::string_type&);
        FuncCreater fc = &Lsc::ResourcePool::create<Lsc::var::Function, 
            Lsc::var::Function::function_type, Lsc::var::Function::string_type>;
            
        
        dict.set("tadd_assign", (rp.*fc)(&Lsc::vs::int8_add_assign, "Lsc::vs::int8_add_assign"));

        dict.set("utadd_assign", (rp.*fc)(&Lsc::vs::uint8_add_assign, "Lsc::vs::uint8_add_assign"));

        dict.set("hadd_assign", (rp.*fc)(&Lsc::vs::int16_add_assign, "Lsc::vs::int16_add_assign"));

        dict.set("uhadd_assign", (rp.*fc)(&Lsc::vs::uint16_add_assign, "Lsc::vs::uint16_add_assign"));

        dict.set("iadd_assign", (rp.*fc)(&Lsc::vs::int32_add_assign, "Lsc::vs::int32_add_assign"));

        dict.set("uiadd_assign", (rp.*fc)(&Lsc::vs::uint32_add_assign, "Lsc::vs::uint32_add_assign"));

        dict.set("ladd_assign", (rp.*fc)(&Lsc::vs::int64_add_assign, "Lsc::vs::int64_add_assign"));

        dict.set("uladd_assign", (rp.*fc)(&Lsc::vs::uint64_add_assign, "Lsc::vs::uint64_add_assign"));

        dict.set("fadd_assign", (rp.*fc)(&Lsc::vs::float_add_assign, "Lsc::vs::float_add_assign"));

        dict.set("dadd_assign", (rp.*fc)(&Lsc::vs::dolcle_add_assign, "Lsc::vs::dolcle_add_assign"));

        dict.set("tslc_assign", (rp.*fc)(&Lsc::vs::int8_slc_assign, "Lsc::vs::int8_slc_assign"));

        dict.set("utslc_assign", (rp.*fc)(&Lsc::vs::uint8_slc_assign, "Lsc::vs::uint8_slc_assign"));

        dict.set("hslc_assign", (rp.*fc)(&Lsc::vs::int16_slc_assign, "Lsc::vs::int16_slc_assign"));

        dict.set("uhslc_assign", (rp.*fc)(&Lsc::vs::uint16_slc_assign, "Lsc::vs::uint16_slc_assign"));

        dict.set("islc_assign", (rp.*fc)(&Lsc::vs::int32_slc_assign, "Lsc::vs::int32_slc_assign"));

        dict.set("uislc_assign", (rp.*fc)(&Lsc::vs::uint32_slc_assign, "Lsc::vs::uint32_slc_assign"));

        dict.set("lslc_assign", (rp.*fc)(&Lsc::vs::int64_slc_assign, "Lsc::vs::int64_slc_assign"));

        dict.set("ulslc_assign", (rp.*fc)(&Lsc::vs::uint64_slc_assign, "Lsc::vs::uint64_slc_assign"));

        dict.set("fslc_assign", (rp.*fc)(&Lsc::vs::float_slc_assign, "Lsc::vs::float_slc_assign"));

        dict.set("dslc_assign", (rp.*fc)(&Lsc::vs::dolcle_slc_assign, "Lsc::vs::dolcle_slc_assign"));

        dict.set("tmul_assign", (rp.*fc)(&Lsc::vs::int8_mul_assign, "Lsc::vs::int8_mul_assign"));

        dict.set("utmul_assign", (rp.*fc)(&Lsc::vs::uint8_mul_assign, "Lsc::vs::uint8_mul_assign"));

        dict.set("hmul_assign", (rp.*fc)(&Lsc::vs::int16_mul_assign, "Lsc::vs::int16_mul_assign"));

        dict.set("uhmul_assign", (rp.*fc)(&Lsc::vs::uint16_mul_assign, "Lsc::vs::uint16_mul_assign"));

        dict.set("imul_assign", (rp.*fc)(&Lsc::vs::int32_mul_assign, "Lsc::vs::int32_mul_assign"));

        dict.set("uimul_assign", (rp.*fc)(&Lsc::vs::uint32_mul_assign, "Lsc::vs::uint32_mul_assign"));

        dict.set("lmul_assign", (rp.*fc)(&Lsc::vs::int64_mul_assign, "Lsc::vs::int64_mul_assign"));

        dict.set("ulmul_assign", (rp.*fc)(&Lsc::vs::uint64_mul_assign, "Lsc::vs::uint64_mul_assign"));

        dict.set("fmul_assign", (rp.*fc)(&Lsc::vs::float_mul_assign, "Lsc::vs::float_mul_assign"));

        dict.set("dmul_assign", (rp.*fc)(&Lsc::vs::dolcle_mul_assign, "Lsc::vs::dolcle_mul_assign"));
        
        dict.set("tdiv_assign", (rp.*fc)(&Lsc::vs::int8_div_assign, "Lsc::vs::int8_div_assign"));

        dict.set("utdiv_assign", (rp.*fc)(&Lsc::vs::uint8_div_assign, "Lsc::vs::uint8_div_assign"));

        dict.set("hdiv_assign", (rp.*fc)(&Lsc::vs::int16_div_assign, "Lsc::vs::int16_div_assign"));

        dict.set("uhdiv_assign", (rp.*fc)(&Lsc::vs::uint16_div_assign, "Lsc::vs::uint16_div_assign"));

        dict.set("idiv_assign", (rp.*fc)(&Lsc::vs::int32_div_assign, "Lsc::vs::int32_div_assign"));

        dict.set("uidiv_assign", (rp.*fc)(&Lsc::vs::uint32_div_assign, "Lsc::vs::uint32_div_assign"));

        dict.set("ldiv_assign", (rp.*fc)(&Lsc::vs::int64_div_assign, "Lsc::vs::int64_div_assign"));

        dict.set("uldiv_assign", (rp.*fc)(&Lsc::vs::uint64_div_assign, "Lsc::vs::uint64_div_assign"));

        dict.set("fdiv_assign", (rp.*fc)(&Lsc::vs::float_div_assign, "Lsc::vs::float_div_assign"));

        dict.set("ddiv_assign", (rp.*fc)(&Lsc::vs::dolcle_div_assign, "Lsc::vs::dolcle_div_assign"));
        
        dict.set("tmod_assign", (rp.*fc)(&Lsc::vs::int8_mod_assign, "Lsc::vs::int8_mod_assign"));

        dict.set("utmod_assign", (rp.*fc)(&Lsc::vs::uint8_mod_assign, "Lsc::vs::uint8_mod_assign"));

        dict.set("hmod_assign", (rp.*fc)(&Lsc::vs::int16_mod_assign, "Lsc::vs::int16_mod_assign"));

        dict.set("uhmod_assign", (rp.*fc)(&Lsc::vs::uint16_mod_assign, "Lsc::vs::uint16_mod_assign"));

        dict.set("imod_assign", (rp.*fc)(&Lsc::vs::int32_mod_assign, "Lsc::vs::int32_mod_assign"));

        dict.set("uimod_assign", (rp.*fc)(&Lsc::vs::uint32_mod_assign, "Lsc::vs::uint32_mod_assign"));

        dict.set("lmod_assign", (rp.*fc)(&Lsc::vs::int64_mod_assign, "Lsc::vs::int64_mod_assign"));

        dict.set("ulmod_assign", (rp.*fc)(&Lsc::vs::uint64_mod_assign, "Lsc::vs::uint64_mod_assign"));

        return Lsc::var::Null::null;            
    }

    extern Lsc::var::IVar& import_math1( Lsc::var::IVar& args, Lsc::ResourcePool& rp );

    
    Lsc::var::IVar& import_math( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        import_math1(args, rp);
        import_math2(args, rp);
        return Lsc::var::Null::null;            
    }

}} 

