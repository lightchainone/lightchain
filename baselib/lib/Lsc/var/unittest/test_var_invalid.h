#ifndef  __TEST_VAR_INVALID_H_
#define  __TEST_VAR_INVALID_H_

#include "Lsc/var/implement.h"
#include "test.h"

inline void test_mask_consistency(const Lsc::var::IVar& var){
    using namespace Lsc::var;
    if ( var.is_number() ){
        assert( var.get_mask() & IVar::IS_NUMBER );
    }
    // assert( var.\1() == bool(\2) );
    assert( var.is_number() == bool(var.get_mask() & IVar::IS_NUMBER ) );
    assert( var.is_null() == bool((var.get_mask() & 0xFF) == 0) );
    assert( var.is_ref() == bool(var.get_mask() & Lsc::var::IVar::IS_REF) );
    assert( var.is_bool() == bool(var.get_mask() & Lsc::var::IVar::IS_BOOL) );
    assert( var.is_number() == bool(var.get_mask() & Lsc::var::IVar::IS_NUMBER) );
    assert( var.is_int8() == bool(Lsc::var::IVar::check_mask( var.get_mask(), Lsc::var::IVar::IS_NUMBER|Lsc::var::IVar::IS_ONE_BYTE|Lsc::var::IVar::IS_SIGNED )) );
        
    assert( var.is_uint8() == bool(Lsc::var::IVar::check_mask( var.get_mask(), Lsc::var::IVar::IS_NUMBER|Lsc::var::IVar::IS_ONE_BYTE, Lsc::var::IVar::IS_SIGNED )) );
    assert( var.is_int16() == bool(Lsc::var::IVar::check_mask( var.get_mask(), Lsc::var::IVar::IS_NUMBER|Lsc::var::IVar::IS_TWO_BYTE|Lsc::var::IVar::IS_SIGNED )) );
    assert( var.is_uint16() == bool(Lsc::var::IVar::check_mask( var.get_mask(), Lsc::var::IVar::IS_NUMBER|Lsc::var::IVar::IS_TWO_BYTE, Lsc::var::IVar::IS_SIGNED )) );
    assert( var.is_int32() == bool(Lsc::var::IVar::check_mask( var.get_mask(), Lsc::var::IVar::IS_NUMBER|Lsc::var::IVar::IS_FOUR_BYTE|Lsc::var::IVar::IS_SIGNED )) );
    assert( var.is_uint32() == bool(Lsc::var::IVar::check_mask( var.get_mask(), Lsc::var::IVar::IS_NUMBER|Lsc::var::IVar::IS_FOUR_BYTE, Lsc::var::IVar::IS_SIGNED )) );
    assert( var.is_int64() == bool(Lsc::var::IVar::check_mask( var.get_mask(), Lsc::var::IVar::IS_NUMBER|Lsc::var::IVar::IS_EIGHT_BYTE|Lsc::var::IVar::IS_SIGNED )) );
    assert( var.is_uint64() == bool(Lsc::var::IVar::check_mask( var.get_mask(), Lsc::var::IVar::IS_NUMBER|Lsc::var::IVar::IS_EIGHT_BYTE, Lsc::var::IVar::IS_SIGNED )) );
    assert( var.is_float() == bool(Lsc::var::IVar::check_mask( var.get_mask(), Lsc::var::IVar::IS_NUMBER|Lsc::var::IVar::IS_FOUR_BYTE|Lsc::var::IVar::IS_FLOATING )) );
    assert( var.is_dolcle() == bool(Lsc::var::IVar::check_mask( var.get_mask(), Lsc::var::IVar::IS_NUMBER|Lsc::var::IVar::IS_EIGHT_BYTE|Lsc::var::IVar::IS_FLOATING )) );
    assert( var.is_string() == bool(var.get_mask() & Lsc::var::IVar::IS_STRING) );
    assert( var.is_array() == bool(var.get_mask() & Lsc::var::IVar::IS_ARRAY) );
    assert( var.is_dict() == bool(var.get_mask() & Lsc::var::IVar::IS_DICT) );
    assert( var.is_callable() == bool(var.get_mask() & Lsc::var::IVar::IS_CALLABLE) );
    assert( var.is_raw() == bool(var.get_mask() & Lsc::var::IVar::IS_RAW) );
}
inline void test_invalid_bool(Lsc::var::IVar&  var){
    //non-const methods
    assert( !var.is_bool() );
    ASSERT_THROW( var.to_bool(), Lsc::InvalidOperationException );
    ASSERT_THROW( var = true, Lsc::InvalidOperationException );
    Lsc::var::Bool b;
    if ( !var.is_ref() ){
        ASSERT_THROW( var = b, Lsc::InvalidOperationException );
    }

    //const methods
    const Lsc::var::IVar& const_var = var;
    assert( !const_var.is_bool() );
    ASSERT_THROW( const_var.to_bool(), Lsc::InvalidOperationException );
}

inline void test_invalid_raw(Lsc::var::IVar&  var){
    //non-const methods
    assert(!var.is_raw());
    ASSERT_THROW( var.to_raw(), Lsc::InvalidOperationException );
    Lsc::var::IVar::raw_type raw_;
    ASSERT_THROW( var = raw_, Lsc::InvalidOperationException );
    Lsc::var::ShallowRaw raw(raw_);
    if ( typeid(var) != typeid(Lsc::var::String) && !var.is_ref() ){
        ASSERT_THROW( var = raw, Lsc::InvalidOperationException );
    }

    //const methods
    const Lsc::var::IVar& const_var = var;
    assert(!const_var.is_raw());
    ASSERT_THROW( const_var.to_raw(), Lsc::InvalidOperationException );
}

template<typename PT> //primitive type
inline void test_valid_number(const Lsc::var::IVar& var, PT pv ){
/*
signed char	int8
unsigned char	uint8
signed short	int16
unsigned short	uint16
signed int	int32
unsigned int	uint32
signed long long	int64
unsigned long long	uint64
*/
    if ( typeid(pv) == typeid(signed char) ){
        assert(var.is_int8());
        assert(var.to_int8() == static_cast<signed char>(pv) );
    }else{
        assert(!var.is_int8());
    }

    if ( typeid(pv) == typeid(unsigned char) ){
        assert(var.is_uint8());
        assert(var.to_uint8() == static_cast<unsigned char>(pv) );
    }else{
        assert(!var.is_uint8());
    }

    if ( typeid(pv) == typeid(signed short) ){
        assert(var.is_int16());
        assert(var.to_int16() == static_cast<signed short>(pv) );
    }else{
        assert(!var.is_int16());
    }

    if ( typeid(pv) == typeid(unsigned short) ){
        assert(var.is_uint16());
        assert(var.to_uint16() == static_cast<unsigned short>(pv) );
    }else{
        assert(!var.is_uint16());
    }

    if ( typeid(pv) == typeid(signed int) ){
        assert(var.is_int32());
        assert(var.to_int32() == static_cast<signed int>(pv) );
    }else{
        assert(!var.is_int32());
    }

    if ( typeid(pv) == typeid(unsigned int) ){
        assert(var.is_uint32());
        assert(var.to_uint32() == static_cast<unsigned int>(pv) );
    }else{
        assert(!var.is_uint32());
    }

    if ( typeid(pv) == typeid(signed long long) ){
        assert(var.is_int64());
        assert(var.to_int64() == static_cast<signed long long>(pv) );
    }else{
        assert(!var.is_int64());
    }

    if ( typeid(pv) == typeid(unsigned long long) ){
        assert(var.is_uint64());
        assert(var.to_uint64() == static_cast<unsigned long long>(pv) );
    }else{
        assert(!var.is_uint64());
    }

#define CHECK_THROWN( expr, except, thrown )\
    do{ \
        try{ expr; thrown = 0;\
        }catch( except& ){ thrown = 1; \
        }catch( ... ){ thrown = 2; } \
    }while(0)

    /*
     see(var.to_string());
     see(pv);
     see(v_thrown);
     see(p_thrown);
     see(v_res);
     see(p_res);
     */
    int v_thrown = 0;
    int p_thrown = 0;
    {
        signed char v_res = 0, p_res = 0;
        CHECK_THROWN( p_res = Lsc::check_cast<signed char>(pv), Lsc::OverflowException, p_thrown );
        CHECK_THROWN( v_res = var.to_int8(), Lsc::OverflowException, v_thrown );
     see(var.to_string());
     see(pv);
     see(v_thrown);
     see(p_thrown);
     see(v_res);
     see(p_res);
        assert( p_thrown == v_thrown && p_res == v_res );
        CHECK_THROWN( p_res = Lsc::check_cast<signed char>(pv), Lsc::UnderflowException, p_thrown );
        CHECK_THROWN( v_res = var.to_int8(), Lsc::UnderflowException, v_thrown );
        assert( p_thrown == v_thrown && p_res == v_res );
    }

    {
        unsigned char v_res = 0, p_res = 0;
        CHECK_THROWN( p_res = Lsc::check_cast<unsigned char>(pv), Lsc::OverflowException, p_thrown );
        CHECK_THROWN( v_res = var.to_uint8(), Lsc::OverflowException, v_thrown );
        assert( p_thrown == v_thrown && p_res == v_res );
        CHECK_THROWN( p_res = Lsc::check_cast<unsigned char>(pv), Lsc::UnderflowException, p_thrown );
        CHECK_THROWN( v_res = var.to_uint8(), Lsc::UnderflowException, v_thrown );
        assert( p_thrown == v_thrown && p_res == v_res );
    }

    {
        signed short v_res = 0, p_res = 0;
        CHECK_THROWN( p_res = Lsc::check_cast<signed short>(pv), Lsc::OverflowException, p_thrown );
        CHECK_THROWN( v_res = var.to_int16(), Lsc::OverflowException, v_thrown );
        assert( p_thrown == v_thrown && p_res == v_res );
        CHECK_THROWN( p_res = Lsc::check_cast<signed short>(pv), Lsc::UnderflowException, p_thrown );
        CHECK_THROWN( v_res = var.to_int16(), Lsc::UnderflowException, v_thrown );
        assert( p_thrown == v_thrown && p_res == v_res );
    }

    {
        unsigned short v_res = 0, p_res = 0;
        CHECK_THROWN( p_res = Lsc::check_cast<unsigned short>(pv), Lsc::OverflowException, p_thrown );
        CHECK_THROWN( v_res = var.to_uint16(), Lsc::OverflowException, v_thrown );
        assert( p_thrown == v_thrown && p_res == v_res );
        CHECK_THROWN( p_res = Lsc::check_cast<unsigned short>(pv), Lsc::UnderflowException, p_thrown );
        CHECK_THROWN( v_res = var.to_uint16(), Lsc::UnderflowException, v_thrown );
        assert( p_thrown == v_thrown && p_res == v_res );
    }

    {
        signed int v_res = 0, p_res = 0;
        CHECK_THROWN( p_res = Lsc::check_cast<signed int>(pv), Lsc::OverflowException, p_thrown );
        CHECK_THROWN( v_res = var.to_int32(), Lsc::OverflowException, v_thrown );
        assert( p_thrown == v_thrown && p_res == v_res );
        CHECK_THROWN( p_res = Lsc::check_cast<signed int>(pv), Lsc::UnderflowException, p_thrown );
        CHECK_THROWN( v_res = var.to_int32(), Lsc::UnderflowException, v_thrown );
        assert( p_thrown == v_thrown && p_res == v_res );
    }

    {
        unsigned int v_res = 0, p_res = 0;
        CHECK_THROWN( p_res = Lsc::check_cast<unsigned int>(pv), Lsc::OverflowException, p_thrown );
        CHECK_THROWN( v_res = var.to_uint32(), Lsc::OverflowException, v_thrown );
        assert( p_thrown == v_thrown && p_res == v_res );
        CHECK_THROWN( p_res = Lsc::check_cast<unsigned int>(pv), Lsc::UnderflowException, p_thrown );
        CHECK_THROWN( v_res = var.to_uint32(), Lsc::UnderflowException, v_thrown );
        assert( p_thrown == v_thrown && p_res == v_res );
    }

    {
        signed long long v_res = 0, p_res = 0;
        CHECK_THROWN( p_res = Lsc::check_cast<signed long long>(pv), Lsc::OverflowException, p_thrown );
        CHECK_THROWN( v_res = var.to_int64(), Lsc::OverflowException, v_thrown );
        assert( p_thrown == v_thrown && p_res == v_res );
        CHECK_THROWN( p_res = Lsc::check_cast<signed long long>(pv), Lsc::UnderflowException, p_thrown );
        CHECK_THROWN( v_res = var.to_int64(), Lsc::UnderflowException, v_thrown );
        assert( p_thrown == v_thrown && p_res == v_res );
    }

    {
        unsigned long long v_res = 0, p_res = 0;
        CHECK_THROWN( p_res = Lsc::check_cast<unsigned long long>(pv), Lsc::OverflowException, p_thrown );
        CHECK_THROWN( v_res = var.to_uint64(), Lsc::OverflowException, v_thrown );
        assert( p_thrown == v_thrown && p_res == v_res );
        CHECK_THROWN( p_res = Lsc::check_cast<unsigned long long>(pv), Lsc::UnderflowException, p_thrown );
        CHECK_THROWN( v_res = var.to_uint64(), Lsc::UnderflowException, v_thrown );
        assert( p_thrown == v_thrown && p_res == v_res );
    }

#undef CHECK_THROWN
        
}

inline void test_invalid_number(Lsc::var::IVar&  var){
    //non-const methods
    assert(!var.is_number());
    assert(!var.is_int8());
    assert(!var.is_uint8());
    assert(!var.is_int16());
    assert(!var.is_uint16());
    assert(!var.is_int32());
    assert(!var.is_uint32());
    assert(!var.is_int64());
    assert(!var.is_uint64());
    assert(!var.is_dolcle());

    ASSERT_THROW( var.to_int8(), Lsc::InvalidOperationException );
    ASSERT_THROW( var.to_uint8(), Lsc::InvalidOperationException );
    ASSERT_THROW( var.to_int16(), Lsc::InvalidOperationException );
    ASSERT_THROW( var.to_uint16(), Lsc::InvalidOperationException );
    ASSERT_THROW( var.to_int32(), Lsc::InvalidOperationException );
    ASSERT_THROW( var.to_uint32(), Lsc::InvalidOperationException );
    ASSERT_THROW( var.to_int64(), Lsc::InvalidOperationException );
    ASSERT_THROW( var.to_uint64(), Lsc::InvalidOperationException );
    ASSERT_THROW( var.to_dolcle(), Lsc::InvalidOperationException );

    ASSERT_THROW( var = static_cast<signed char>(123), Lsc::InvalidOperationException );
    ASSERT_THROW( var = static_cast<unsigned char>(123), Lsc::InvalidOperationException );
    ASSERT_THROW( var = static_cast<signed short>(123), Lsc::InvalidOperationException );
    ASSERT_THROW( var = static_cast<unsigned short>(123), Lsc::InvalidOperationException );
    ASSERT_THROW( var = static_cast<signed int>(123), Lsc::InvalidOperationException );
    ASSERT_THROW( var = static_cast<unsigned int>(123), Lsc::InvalidOperationException );
    ASSERT_THROW( var = static_cast<signed long long>(123), Lsc::InvalidOperationException );
    ASSERT_THROW( var = static_cast<unsigned long long>(123), Lsc::InvalidOperationException );
    ASSERT_THROW( var = 4670.9394, Lsc::InvalidOperationException );

    Lsc::var::Number<signed char> i8;
    Lsc::var::Number<unsigned char> u8;
    Lsc::var::Number<signed short> i16;
    Lsc::var::Number<unsigned short> u16;
    Lsc::var::Number<signed int> i32;
    Lsc::var::Number<unsigned int> u32;
    Lsc::var::Number<signed long long> i64;
    Lsc::var::Number<unsigned long long> u64;
    Lsc::var::Number<dolcle> dbl;
    if ( !var.is_ref() ){
        ASSERT_THROW( var = i8, Lsc::InvalidOperationException );
        ASSERT_THROW( var = u8, Lsc::InvalidOperationException );
        ASSERT_THROW( var = i16, Lsc::InvalidOperationException );
        ASSERT_THROW( var = u16, Lsc::InvalidOperationException );
        ASSERT_THROW( var = i32, Lsc::InvalidOperationException );
        ASSERT_THROW( var = u32, Lsc::InvalidOperationException );
        ASSERT_THROW( var = i64, Lsc::InvalidOperationException );
        ASSERT_THROW( var = u64, Lsc::InvalidOperationException );
        ASSERT_THROW( var = dbl, Lsc::InvalidOperationException );
    }

    //const methods
    const Lsc::var::IVar& const_var = var;

    assert(!const_var.is_number());
    assert(!const_var.is_int8());
    assert(!const_var.is_uint8());
    assert(!const_var.is_int16());
    assert(!const_var.is_uint16());
    assert(!const_var.is_int32());
    assert(!const_var.is_uint32());
    assert(!const_var.is_int64());
    assert(!const_var.is_uint64());
    assert(!const_var.is_dolcle());

    ASSERT_THROW( const_var.to_int8(), Lsc::InvalidOperationException );
    ASSERT_THROW( const_var.to_uint8(), Lsc::InvalidOperationException );
    ASSERT_THROW( const_var.to_int16(), Lsc::InvalidOperationException );
    ASSERT_THROW( const_var.to_uint16(), Lsc::InvalidOperationException );
    ASSERT_THROW( const_var.to_int32(), Lsc::InvalidOperationException );
    ASSERT_THROW( const_var.to_uint32(), Lsc::InvalidOperationException );
    ASSERT_THROW( const_var.to_int64(), Lsc::InvalidOperationException );
    ASSERT_THROW( const_var.to_uint64(), Lsc::InvalidOperationException );
    ASSERT_THROW( const_var.to_dolcle(), Lsc::InvalidOperationException );

}

inline void test_invalid_string(Lsc::var::IVar&  var){
    //non-const methods
    assert( !var.is_string() );
    ASSERT_THROW( var.c_str(), Lsc::InvalidOperationException );
    ASSERT_THROW( var.c_str_len(), Lsc::InvalidOperationException );
    if ( !var.is_number() && !var.is_bool() ){
        //number类别可能支持下列方法
        ASSERT_THROW( var = "abc", Lsc::InvalidOperationException );
        ASSERT_THROW( var = Lsc::var::IVar::string_type("abc"), Lsc::InvalidOperationException );
        if ( !var.is_ref() ){
            Lsc::var::String str("abc");
            ASSERT_THROW( var = str, Lsc::InvalidOperationException );
        }
    }

    //const methods
    const Lsc::var::IVar& const_var = var;
    assert( !const_var.is_string() );
    ASSERT_THROW( const_var.c_str(), Lsc::InvalidOperationException );
    ASSERT_THROW( const_var.c_str_len(), Lsc::InvalidOperationException );
}

inline void test_invalid_array(Lsc::var::IVar&  var){
    //non-const methods
    assert( !var.is_array() );
    //size()
    if ( !var.is_dict() ){
        ASSERT_THROW( var.size(), Lsc::InvalidOperationException );
    }
    //get()
    ASSERT_THROW( var.get(0), Lsc::InvalidOperationException );
    //set()
    ASSERT_THROW( var.set(0, var), Lsc::InvalidOperationException );
    //del()
    ASSERT_THROW( var.del(0), Lsc::InvalidOperationException );
    //operator []()
    ASSERT_THROW( var[-1], Lsc::InvalidOperationException );
    ASSERT_THROW( var[0], Lsc::InvalidOperationException );
    ASSERT_THROW( var[1], Lsc::InvalidOperationException );
    //iterator
    ASSERT_THROW( var.array_begin(), Lsc::InvalidOperationException );
    ASSERT_THROW( var.array_end(), Lsc::InvalidOperationException );
    //operator =
    Lsc::var::Array arr;
    if ( typeid(var) != typeid(Lsc::var::String) && !var.is_ref() ){
        ASSERT_THROW( var = arr, Lsc::InvalidOperationException );
    }

    //const methods
    const Lsc::var::IVar& const_var = var;
    //get()
    ASSERT_THROW( const_var.get(0), Lsc::InvalidOperationException );
    //operator []()
    ASSERT_THROW( const_var[-1], Lsc::InvalidOperationException );
    ASSERT_THROW( const_var[0], Lsc::InvalidOperationException );
    ASSERT_THROW( const_var[1], Lsc::InvalidOperationException );
    //iterator
    ASSERT_THROW( const_var.array_begin(), Lsc::InvalidOperationException );
    ASSERT_THROW( const_var.array_end(), Lsc::InvalidOperationException );
}

inline void test_invalid_dict(Lsc::var::IVar&  var){
    //non-const methods
    assert( !var.is_dict() );
    //size()
    if ( !var.is_array() ){
        ASSERT_THROW( var.size(), Lsc::InvalidOperationException );
    }
    //get()
    ASSERT_THROW( var.get(""), Lsc::InvalidOperationException );
    ASSERT_THROW( var.get("a key"), Lsc::InvalidOperationException );
    ASSERT_THROW( var.get(Lsc::var::IVar::string_type("another key")), Lsc::InvalidOperationException );
    //set()
    ASSERT_THROW( var.set(Lsc::var::IVar::string_type(""),var), Lsc::InvalidOperationException );
    ASSERT_THROW( var.set("some key",var), Lsc::InvalidOperationException );
    ASSERT_THROW( var.set(Lsc::var::IVar::string_type("another key"),var), Lsc::InvalidOperationException );
    //del()
    ASSERT_THROW( var.del("whatever"), Lsc::InvalidOperationException );
    //operator []
    ASSERT_THROW( var[""], Lsc::InvalidOperationException );
    ASSERT_THROW( var["awesome key"], Lsc::InvalidOperationException );
    //iterator
    ASSERT_THROW( var.dict_begin(), Lsc::InvalidOperationException );
    ASSERT_THROW( var.dict_end(), Lsc::InvalidOperationException );
    //operator =
    Lsc::var::Dict d;
    if ( typeid(var) != typeid(Lsc::var::String) && !var.is_ref() ){
        ASSERT_THROW( var = d, Lsc::InvalidOperationException );
    }

    //const methods
    const Lsc::var::IVar& const_var = var;
    //get()
    ASSERT_THROW( const_var.get(""), Lsc::InvalidOperationException );
    ASSERT_THROW( const_var.get("a key"), Lsc::InvalidOperationException );
    ASSERT_THROW( const_var.get(Lsc::var::IVar::string_type("another key")), Lsc::InvalidOperationException );
    //operator []
    ASSERT_THROW( const_var[""], Lsc::InvalidOperationException );
    ASSERT_THROW( const_var["awesome key"], Lsc::InvalidOperationException );
    //iterator
    ASSERT_THROW( const_var.dict_begin(), Lsc::InvalidOperationException );
    ASSERT_THROW( const_var.dict_end(), Lsc::InvalidOperationException );
}

inline Lsc::var::IVar& echo_f( Lsc::var::IVar& args, Lsc::ResourcePool& ){
    return args;
}

inline Lsc::var::IVar& echo_m( Lsc::var::IVar& self, Lsc::var::IVar& args, Lsc::ResourcePool& rp){
    Lsc::var::IVar& res = rp.create<Lsc::var::Dict>();
    res["self"] = self;
    res["args"] = args;
    return res;
}

inline void test_invalid_callable(Lsc::var::IVar&  var){
    //non-const methods
    assert( !var.is_callable() );
    Lsc::var::Dict  self;
    Lsc::var::Array params;
    Lsc::ResourcePool rp;

    ASSERT_THROW( var(params, rp), Lsc::InvalidOperationException );
    ASSERT_THROW( var(self, params, rp), Lsc::InvalidOperationException );
    Lsc::var::Function func(echo_f, "echo_f");
    if ( typeid(var) != typeid(Lsc::var::String) && !var.is_ref() ){
        ASSERT_THROW( var = func, Lsc::InvalidOperationException );
    }
    Lsc::var::Function method(echo_f, "echo_f");
    if ( typeid(var) != typeid(Lsc::var::String) && !var.is_ref() ){
        ASSERT_THROW( var = method, Lsc::InvalidOperationException );
    }

    //no const methods defined
}
#endif  //__TEST_VAR_INVALID_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
