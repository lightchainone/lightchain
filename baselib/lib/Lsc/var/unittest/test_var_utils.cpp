#include "Lsc/var/implement.h"
#include "Lsc/var/utils.h"
#include "Lsc/var/assign.h"
#include "Lsc/var/var_traits.h"

 
#include<typeinfo>
#include<iostream>
#define see(x) do{  \
    std::cerr<<__FILE__<<":"<<__LINE__<<": " <<(#x)<<" = "<<(x)<<" ("<<typeid(x).name()<<")"<<std::endl;\
}while(0)

void test_dump_to_string(){
    Lsc::var::Int32 i32=123;
    Lsc::var::Int64 i64=-100000000000000LL;
    Lsc::var::String str= "hello, \"\'123";
    Lsc::var::Array arr;
    Lsc::var::Dict dict;
    Lsc::var::Bool var_true;
    Lsc::var::ShallowRaw  raw("ABC\1\2\0",6);
    Lsc::var::Dolcle dbl(123.45e78);
    dict["self"] = dict;
    dict["arr"]  = arr;
    dict["null"] = Lsc::var::Null::null;
    dict["bool"] = var_true;
    dict["raw"]  = raw;
    dict["dolcle"]=dbl;
    arr[0] = i32;
    arr[1] = i64;
    arr[2] = str;
    
    {
        printf("i32, 0: %s\n", i32.dump(0).c_str() );
    }
    {
        printf("i32, 999: %s\n", i32.dump(999).c_str() );
    }
    {
        printf("dict, 0: %s\n", dict.dump(0).c_str() );
    }
    {
        printf("dict, 1: %s\n", dict.dump(1).c_str() );
    }
    {
        printf("dict, 3: %s\n", dict.dump(3).c_str() );
    }
    {
        Lsc::string res;
        dump_to_string( i32, res, 0, "<CR>" );
        printf("i32, 0: %s<CR>", res.c_str() );
    }
    {
        Lsc::string res;
        dump_to_string( i32, res, 999, "<CR>" );
        printf("i32, 999: %s<CR>", res.c_str() );
    }
    {
        Lsc::string res;
        dump_to_string( dict, res, 0, "<CR>" );
        printf("dict, 0: %s<CR>", res.c_str() );
    }
    {
        Lsc::string res;
        dump_to_string( dict, res, 1, "<CR>" );
        printf("dict, 1: %s<CR>", res.c_str() );
    }
    {
        Lsc::string res;
        dump_to_string( dict, res, 3, "<CR>" );
        printf("dict, 3: %s<CR>", res.c_str() );
    }
    {
        Lsc::string res;
        dump_to_string( i32, res, 0, "\n" );
        printf("i32, 0: %s\n", res.c_str() );
    }
    {
        Lsc::string res;
        dump_to_string( i32, res, 999, "\n" );
        printf("i32, 999: %s\n", res.c_str() );
    }
    {
        Lsc::string res;
        dump_to_string( dict, res, 0, "\n" );
        printf("dict, 0: %s\n", res.c_str() );
    }
    {
        Lsc::string res;
        dump_to_string( dict, res, 1, "\n" );
        printf("dict, 1: %s\n", res.c_str() );
    }
    {
        Lsc::string res;
        dump_to_string( dict, res, 3, "\n" );
        printf("dict, 3: %s\n", res.c_str() );
    }
}

void test_assign(){
    Lsc::var::Int32 i32(123);
    Lsc::var::Int64 i64(456);
    Lsc::var::String str("hello");
    //array
    Lsc::var::Array arr;
    Lsc::var::assign( arr, i32, i64 );
    assert( arr.size() == 2 );
    assert( arr[0].to_int32() == 123 );
    assert( arr[1].to_int64() == 456 );
    Lsc::var::assign( arr, str );
    assert( arr.size() == 1 );
    assert( arr[0].to_string() == "hello" );

    //dict
    Lsc::var::Dict dict;
    Lsc::var::assign( dict, "haha", i32, "hehe", i64 );
    assert( dict.size() == 2 );
    assert( dict["haha"].to_int32() == 123 );
    assert( dict["hehe"].to_int64() == 456 );
    Lsc::var::assign( dict, "crazy", str );
    assert( dict.size() == 1 );
    assert( dict["crazy"].to_string() == "hello" );

}
template<typename T>
void test_traits_type(){
    Lsc::var::Number<T> v; 
    assert(v.get_mask() == Lsc::var::var_traits<T>::MASK);
}

void test_traits(){
    test_traits_type<signed char>();
    test_traits_type<unsigned char>();
    test_traits_type<signed short>();
    test_traits_type<unsigned short>();
    test_traits_type<signed int>();
    test_traits_type<unsigned int>();
    test_traits_type<signed long long>();
    test_traits_type<unsigned long long>();
}

int main(){
    test_dump_to_string();
    test_assign();
    test_traits();
    return 0;
}
/* vim: set ts=4 sw=4 sts=4 tw=100 */
