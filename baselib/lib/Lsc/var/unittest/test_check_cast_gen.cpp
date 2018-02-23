
#include <cassert>
#include "Lsc/check_cast.h"

#include<typeinfo>
#include<iostream>
#define see(x) do{      std::cerr<<__FILE__<<":"<<__LINE__<<": " <<(#x)<<" = "<<(x)<<" ("<<typeid(x).name()<<")"<<std::endl;}while(0)

#define ASSERT_THROW( expr, except )        do{                                         bool has_thrown = false;                try{                                        expr;                               }catch( except& ){                          has_thrown = true;                  }                                       assert( has_thrown );               }while(0)


int main(){

#if __WORDSIZE == 32
		//int -> int

    assert( -2147483648LL == Lsc::check_cast<int>( static_cast<int>(-2147483648LL) ) ) ;
    assert( -2147483648LL + 1 == Lsc::check_cast<int>( static_cast<int>(-2147483648LL + 1) ) ) ;


    assert( 2147483647LL - 1== Lsc::check_cast<int>( static_cast<int>(2147483647LL - 1) ) ) ;
    assert( 2147483647LL == Lsc::check_cast<int>( static_cast<int>(2147483647LL) ) ) ;

		//unsigned char -> int
		//unsigned long long -> int

    assert( 2147483647LL - 1== Lsc::check_cast<int>( static_cast<unsigned long long>(2147483647LL - 1) ) ) ;
    assert( 2147483647LL == Lsc::check_cast<int>( static_cast<unsigned long long>(2147483647LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<int>( static_cast<unsigned long long>(2147483647LL + 1) ), Lsc::OverflowException );

		//char -> int
		//unsigned long -> int

    assert( 2147483647LL - 1== Lsc::check_cast<int>( static_cast<unsigned long>(2147483647LL - 1) ) ) ;
    assert( 2147483647LL == Lsc::check_cast<int>( static_cast<unsigned long>(2147483647LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<int>( static_cast<unsigned long>(2147483647LL + 1) ), Lsc::OverflowException );

		//size_t -> int

    assert( 2147483647LL - 1== Lsc::check_cast<int>( static_cast<size_t>(2147483647LL - 1) ) ) ;
    assert( 2147483647LL == Lsc::check_cast<int>( static_cast<size_t>(2147483647LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<int>( static_cast<size_t>(2147483647LL + 1) ), Lsc::OverflowException );

		//unsigned int -> int

    assert( 2147483647LL - 1== Lsc::check_cast<int>( static_cast<unsigned int>(2147483647LL - 1) ) ) ;
    assert( 2147483647LL == Lsc::check_cast<int>( static_cast<unsigned int>(2147483647LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<int>( static_cast<unsigned int>(2147483647LL + 1) ), Lsc::OverflowException );

		//short -> int
		//long -> int

    assert( -2147483648LL == Lsc::check_cast<int>( static_cast<long>(-2147483648LL) ) ) ;
    assert( -2147483648LL + 1 == Lsc::check_cast<int>( static_cast<long>(-2147483648LL + 1) ) ) ;


    assert( 2147483647LL - 1== Lsc::check_cast<int>( static_cast<long>(2147483647LL - 1) ) ) ;
    assert( 2147483647LL == Lsc::check_cast<int>( static_cast<long>(2147483647LL) ) ) ;

		//unsigned short -> int
		//long long -> int

    assert( -2147483648LL == Lsc::check_cast<int>( static_cast<long long>(-2147483648LL) ) ) ;
    assert( -2147483648LL + 1 == Lsc::check_cast<int>( static_cast<long long>(-2147483648LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<int>( static_cast<long long>(-2147483648LL - 1) ), Lsc::UnderflowException );


    assert( 2147483647LL - 1== Lsc::check_cast<int>( static_cast<long long>(2147483647LL - 1) ) ) ;
    assert( 2147483647LL == Lsc::check_cast<int>( static_cast<long long>(2147483647LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<int>( static_cast<long long>(2147483647LL + 1) ), Lsc::OverflowException );

		//signed char -> int
		//int -> unsigned char

    assert( 0LL == Lsc::check_cast<unsigned char>( static_cast<int>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned char>( static_cast<int>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned char>( static_cast<int>(0LL - 1) ), Lsc::UnderflowException );


    assert( 255LL - 1== Lsc::check_cast<unsigned char>( static_cast<int>(255LL - 1) ) ) ;
    assert( 255LL == Lsc::check_cast<unsigned char>( static_cast<int>(255LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned char>( static_cast<int>(255LL + 1) ), Lsc::OverflowException );

		//unsigned char -> unsigned char

    assert( 0LL == Lsc::check_cast<unsigned char>( static_cast<unsigned char>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned char>( static_cast<unsigned char>(0LL + 1) ) ) ;


    assert( 255LL - 1== Lsc::check_cast<unsigned char>( static_cast<unsigned char>(255LL - 1) ) ) ;
    assert( 255LL == Lsc::check_cast<unsigned char>( static_cast<unsigned char>(255LL) ) ) ;

		//unsigned long long -> unsigned char

    assert( 0LL == Lsc::check_cast<unsigned char>( static_cast<unsigned long long>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned char>( static_cast<unsigned long long>(0LL + 1) ) ) ;


    assert( 255LL - 1== Lsc::check_cast<unsigned char>( static_cast<unsigned long long>(255LL - 1) ) ) ;
    assert( 255LL == Lsc::check_cast<unsigned char>( static_cast<unsigned long long>(255LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned char>( static_cast<unsigned long long>(255LL + 1) ), Lsc::OverflowException );

		//char -> unsigned char

    assert( 0LL == Lsc::check_cast<unsigned char>( static_cast<char>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned char>( static_cast<char>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned char>( static_cast<char>(0LL - 1) ), Lsc::UnderflowException );

		//unsigned long -> unsigned char

    assert( 0LL == Lsc::check_cast<unsigned char>( static_cast<unsigned long>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned char>( static_cast<unsigned long>(0LL + 1) ) ) ;


    assert( 255LL - 1== Lsc::check_cast<unsigned char>( static_cast<unsigned long>(255LL - 1) ) ) ;
    assert( 255LL == Lsc::check_cast<unsigned char>( static_cast<unsigned long>(255LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned char>( static_cast<unsigned long>(255LL + 1) ), Lsc::OverflowException );

		//size_t -> unsigned char

    assert( 0LL == Lsc::check_cast<unsigned char>( static_cast<size_t>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned char>( static_cast<size_t>(0LL + 1) ) ) ;


    assert( 255LL - 1== Lsc::check_cast<unsigned char>( static_cast<size_t>(255LL - 1) ) ) ;
    assert( 255LL == Lsc::check_cast<unsigned char>( static_cast<size_t>(255LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned char>( static_cast<size_t>(255LL + 1) ), Lsc::OverflowException );

		//unsigned int -> unsigned char

    assert( 0LL == Lsc::check_cast<unsigned char>( static_cast<unsigned int>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned char>( static_cast<unsigned int>(0LL + 1) ) ) ;


    assert( 255LL - 1== Lsc::check_cast<unsigned char>( static_cast<unsigned int>(255LL - 1) ) ) ;
    assert( 255LL == Lsc::check_cast<unsigned char>( static_cast<unsigned int>(255LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned char>( static_cast<unsigned int>(255LL + 1) ), Lsc::OverflowException );

		//short -> unsigned char

    assert( 0LL == Lsc::check_cast<unsigned char>( static_cast<short>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned char>( static_cast<short>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned char>( static_cast<short>(0LL - 1) ), Lsc::UnderflowException );


    assert( 255LL - 1== Lsc::check_cast<unsigned char>( static_cast<short>(255LL - 1) ) ) ;
    assert( 255LL == Lsc::check_cast<unsigned char>( static_cast<short>(255LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned char>( static_cast<short>(255LL + 1) ), Lsc::OverflowException );

		//long -> unsigned char

    assert( 0LL == Lsc::check_cast<unsigned char>( static_cast<long>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned char>( static_cast<long>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned char>( static_cast<long>(0LL - 1) ), Lsc::UnderflowException );


    assert( 255LL - 1== Lsc::check_cast<unsigned char>( static_cast<long>(255LL - 1) ) ) ;
    assert( 255LL == Lsc::check_cast<unsigned char>( static_cast<long>(255LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned char>( static_cast<long>(255LL + 1) ), Lsc::OverflowException );

		//unsigned short -> unsigned char

    assert( 0LL == Lsc::check_cast<unsigned char>( static_cast<unsigned short>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned char>( static_cast<unsigned short>(0LL + 1) ) ) ;


    assert( 255LL - 1== Lsc::check_cast<unsigned char>( static_cast<unsigned short>(255LL - 1) ) ) ;
    assert( 255LL == Lsc::check_cast<unsigned char>( static_cast<unsigned short>(255LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned char>( static_cast<unsigned short>(255LL + 1) ), Lsc::OverflowException );

		//long long -> unsigned char

    assert( 0LL == Lsc::check_cast<unsigned char>( static_cast<long long>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned char>( static_cast<long long>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned char>( static_cast<long long>(0LL - 1) ), Lsc::UnderflowException );


    assert( 255LL - 1== Lsc::check_cast<unsigned char>( static_cast<long long>(255LL - 1) ) ) ;
    assert( 255LL == Lsc::check_cast<unsigned char>( static_cast<long long>(255LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned char>( static_cast<long long>(255LL + 1) ), Lsc::OverflowException );

		//signed char -> unsigned char

    assert( 0LL == Lsc::check_cast<unsigned char>( static_cast<signed char>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned char>( static_cast<signed char>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned char>( static_cast<signed char>(0LL - 1) ), Lsc::UnderflowException );

		//int -> unsigned long long

    assert( 0LL == Lsc::check_cast<unsigned long long>( static_cast<int>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned long long>( static_cast<int>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned long long>( static_cast<int>(0LL - 1) ), Lsc::UnderflowException );

		//unsigned char -> unsigned long long

    assert( 0LL == Lsc::check_cast<unsigned long long>( static_cast<unsigned char>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned long long>( static_cast<unsigned char>(0LL + 1) ) ) ;

		//unsigned long long -> unsigned long long

    assert( 0LL == Lsc::check_cast<unsigned long long>( static_cast<unsigned long long>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned long long>( static_cast<unsigned long long>(0LL + 1) ) ) ;


    assert( 18446744073709551615ULL - 1== Lsc::check_cast<unsigned long long>( static_cast<unsigned long long>(18446744073709551615ULL - 1) ) ) ;
    assert( 18446744073709551615ULL == Lsc::check_cast<unsigned long long>( static_cast<unsigned long long>(18446744073709551615ULL) ) ) ;

		//char -> unsigned long long

    assert( 0LL == Lsc::check_cast<unsigned long long>( static_cast<char>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned long long>( static_cast<char>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned long long>( static_cast<char>(0LL - 1) ), Lsc::UnderflowException );

		//unsigned long -> unsigned long long

    assert( 0LL == Lsc::check_cast<unsigned long long>( static_cast<unsigned long>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned long long>( static_cast<unsigned long>(0LL + 1) ) ) ;

		//size_t -> unsigned long long

    assert( 0LL == Lsc::check_cast<unsigned long long>( static_cast<size_t>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned long long>( static_cast<size_t>(0LL + 1) ) ) ;

		//unsigned int -> unsigned long long

    assert( 0LL == Lsc::check_cast<unsigned long long>( static_cast<unsigned int>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned long long>( static_cast<unsigned int>(0LL + 1) ) ) ;

		//short -> unsigned long long

    assert( 0LL == Lsc::check_cast<unsigned long long>( static_cast<short>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned long long>( static_cast<short>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned long long>( static_cast<short>(0LL - 1) ), Lsc::UnderflowException );

		//long -> unsigned long long

    assert( 0LL == Lsc::check_cast<unsigned long long>( static_cast<long>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned long long>( static_cast<long>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned long long>( static_cast<long>(0LL - 1) ), Lsc::UnderflowException );

		//unsigned short -> unsigned long long

    assert( 0LL == Lsc::check_cast<unsigned long long>( static_cast<unsigned short>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned long long>( static_cast<unsigned short>(0LL + 1) ) ) ;

		//long long -> unsigned long long

    assert( 0LL == Lsc::check_cast<unsigned long long>( static_cast<long long>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned long long>( static_cast<long long>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned long long>( static_cast<long long>(0LL - 1) ), Lsc::UnderflowException );

		//signed char -> unsigned long long

    assert( 0LL == Lsc::check_cast<unsigned long long>( static_cast<signed char>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned long long>( static_cast<signed char>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned long long>( static_cast<signed char>(0LL - 1) ), Lsc::UnderflowException );

		//int -> char

    assert( -128LL == Lsc::check_cast<char>( static_cast<int>(-128LL) ) ) ;
    assert( -128LL + 1 == Lsc::check_cast<char>( static_cast<int>(-128LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<char>( static_cast<int>(-128LL - 1) ), Lsc::UnderflowException );


    assert( 127LL - 1== Lsc::check_cast<char>( static_cast<int>(127LL - 1) ) ) ;
    assert( 127LL == Lsc::check_cast<char>( static_cast<int>(127LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<char>( static_cast<int>(127LL + 1) ), Lsc::OverflowException );

		//unsigned char -> char

    assert( 127LL - 1== Lsc::check_cast<char>( static_cast<unsigned char>(127LL - 1) ) ) ;
    assert( 127LL == Lsc::check_cast<char>( static_cast<unsigned char>(127LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<char>( static_cast<unsigned char>(127LL + 1) ), Lsc::OverflowException );

		//unsigned long long -> char

    assert( 127LL - 1== Lsc::check_cast<char>( static_cast<unsigned long long>(127LL - 1) ) ) ;
    assert( 127LL == Lsc::check_cast<char>( static_cast<unsigned long long>(127LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<char>( static_cast<unsigned long long>(127LL + 1) ), Lsc::OverflowException );

		//char -> char

    assert( -128LL == Lsc::check_cast<char>( static_cast<char>(-128LL) ) ) ;
    assert( -128LL + 1 == Lsc::check_cast<char>( static_cast<char>(-128LL + 1) ) ) ;


    assert( 127LL - 1== Lsc::check_cast<char>( static_cast<char>(127LL - 1) ) ) ;
    assert( 127LL == Lsc::check_cast<char>( static_cast<char>(127LL) ) ) ;

		//unsigned long -> char

    assert( 127LL - 1== Lsc::check_cast<char>( static_cast<unsigned long>(127LL - 1) ) ) ;
    assert( 127LL == Lsc::check_cast<char>( static_cast<unsigned long>(127LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<char>( static_cast<unsigned long>(127LL + 1) ), Lsc::OverflowException );

		//size_t -> char

    assert( 127LL - 1== Lsc::check_cast<char>( static_cast<size_t>(127LL - 1) ) ) ;
    assert( 127LL == Lsc::check_cast<char>( static_cast<size_t>(127LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<char>( static_cast<size_t>(127LL + 1) ), Lsc::OverflowException );

		//unsigned int -> char

    assert( 127LL - 1== Lsc::check_cast<char>( static_cast<unsigned int>(127LL - 1) ) ) ;
    assert( 127LL == Lsc::check_cast<char>( static_cast<unsigned int>(127LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<char>( static_cast<unsigned int>(127LL + 1) ), Lsc::OverflowException );

		//short -> char

    assert( -128LL == Lsc::check_cast<char>( static_cast<short>(-128LL) ) ) ;
    assert( -128LL + 1 == Lsc::check_cast<char>( static_cast<short>(-128LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<char>( static_cast<short>(-128LL - 1) ), Lsc::UnderflowException );


    assert( 127LL - 1== Lsc::check_cast<char>( static_cast<short>(127LL - 1) ) ) ;
    assert( 127LL == Lsc::check_cast<char>( static_cast<short>(127LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<char>( static_cast<short>(127LL + 1) ), Lsc::OverflowException );

		//long -> char

    assert( -128LL == Lsc::check_cast<char>( static_cast<long>(-128LL) ) ) ;
    assert( -128LL + 1 == Lsc::check_cast<char>( static_cast<long>(-128LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<char>( static_cast<long>(-128LL - 1) ), Lsc::UnderflowException );


    assert( 127LL - 1== Lsc::check_cast<char>( static_cast<long>(127LL - 1) ) ) ;
    assert( 127LL == Lsc::check_cast<char>( static_cast<long>(127LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<char>( static_cast<long>(127LL + 1) ), Lsc::OverflowException );

		//unsigned short -> char

    assert( 127LL - 1== Lsc::check_cast<char>( static_cast<unsigned short>(127LL - 1) ) ) ;
    assert( 127LL == Lsc::check_cast<char>( static_cast<unsigned short>(127LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<char>( static_cast<unsigned short>(127LL + 1) ), Lsc::OverflowException );

		//long long -> char

    assert( -128LL == Lsc::check_cast<char>( static_cast<long long>(-128LL) ) ) ;
    assert( -128LL + 1 == Lsc::check_cast<char>( static_cast<long long>(-128LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<char>( static_cast<long long>(-128LL - 1) ), Lsc::UnderflowException );


    assert( 127LL - 1== Lsc::check_cast<char>( static_cast<long long>(127LL - 1) ) ) ;
    assert( 127LL == Lsc::check_cast<char>( static_cast<long long>(127LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<char>( static_cast<long long>(127LL + 1) ), Lsc::OverflowException );

		//signed char -> char

    assert( -128LL == Lsc::check_cast<char>( static_cast<signed char>(-128LL) ) ) ;
    assert( -128LL + 1 == Lsc::check_cast<char>( static_cast<signed char>(-128LL + 1) ) ) ;


    assert( 127LL - 1== Lsc::check_cast<char>( static_cast<signed char>(127LL - 1) ) ) ;
    assert( 127LL == Lsc::check_cast<char>( static_cast<signed char>(127LL) ) ) ;

		//int -> unsigned long

    assert( 0LL == Lsc::check_cast<unsigned long>( static_cast<int>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned long>( static_cast<int>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned long>( static_cast<int>(0LL - 1) ), Lsc::UnderflowException );

		//unsigned char -> unsigned long

    assert( 0LL == Lsc::check_cast<unsigned long>( static_cast<unsigned char>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned long>( static_cast<unsigned char>(0LL + 1) ) ) ;

		//unsigned long long -> unsigned long

    assert( 0LL == Lsc::check_cast<unsigned long>( static_cast<unsigned long long>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned long>( static_cast<unsigned long long>(0LL + 1) ) ) ;


    assert( 4294967295LL - 1== Lsc::check_cast<unsigned long>( static_cast<unsigned long long>(4294967295LL - 1) ) ) ;
    assert( 4294967295LL == Lsc::check_cast<unsigned long>( static_cast<unsigned long long>(4294967295LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned long>( static_cast<unsigned long long>(4294967295LL + 1) ), Lsc::OverflowException );

		//char -> unsigned long

    assert( 0LL == Lsc::check_cast<unsigned long>( static_cast<char>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned long>( static_cast<char>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned long>( static_cast<char>(0LL - 1) ), Lsc::UnderflowException );

		//unsigned long -> unsigned long

    assert( 0LL == Lsc::check_cast<unsigned long>( static_cast<unsigned long>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned long>( static_cast<unsigned long>(0LL + 1) ) ) ;


    assert( 4294967295LL - 1== Lsc::check_cast<unsigned long>( static_cast<unsigned long>(4294967295LL - 1) ) ) ;
    assert( 4294967295LL == Lsc::check_cast<unsigned long>( static_cast<unsigned long>(4294967295LL) ) ) ;

		//size_t -> unsigned long

    assert( 0LL == Lsc::check_cast<unsigned long>( static_cast<size_t>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned long>( static_cast<size_t>(0LL + 1) ) ) ;


    assert( 4294967295LL - 1== Lsc::check_cast<unsigned long>( static_cast<size_t>(4294967295LL - 1) ) ) ;
    assert( 4294967295LL == Lsc::check_cast<unsigned long>( static_cast<size_t>(4294967295LL) ) ) ;

		//unsigned int -> unsigned long

    assert( 0LL == Lsc::check_cast<unsigned long>( static_cast<unsigned int>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned long>( static_cast<unsigned int>(0LL + 1) ) ) ;


    assert( 4294967295LL - 1== Lsc::check_cast<unsigned long>( static_cast<unsigned int>(4294967295LL - 1) ) ) ;
    assert( 4294967295LL == Lsc::check_cast<unsigned long>( static_cast<unsigned int>(4294967295LL) ) ) ;

		//short -> unsigned long

    assert( 0LL == Lsc::check_cast<unsigned long>( static_cast<short>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned long>( static_cast<short>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned long>( static_cast<short>(0LL - 1) ), Lsc::UnderflowException );

		//long -> unsigned long

    assert( 0LL == Lsc::check_cast<unsigned long>( static_cast<long>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned long>( static_cast<long>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned long>( static_cast<long>(0LL - 1) ), Lsc::UnderflowException );

		//unsigned short -> unsigned long

    assert( 0LL == Lsc::check_cast<unsigned long>( static_cast<unsigned short>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned long>( static_cast<unsigned short>(0LL + 1) ) ) ;

		//long long -> unsigned long

    assert( 0LL == Lsc::check_cast<unsigned long>( static_cast<long long>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned long>( static_cast<long long>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned long>( static_cast<long long>(0LL - 1) ), Lsc::UnderflowException );


    assert( 4294967295LL - 1== Lsc::check_cast<unsigned long>( static_cast<long long>(4294967295LL - 1) ) ) ;
    assert( 4294967295LL == Lsc::check_cast<unsigned long>( static_cast<long long>(4294967295LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned long>( static_cast<long long>(4294967295LL + 1) ), Lsc::OverflowException );

		//signed char -> unsigned long

    assert( 0LL == Lsc::check_cast<unsigned long>( static_cast<signed char>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned long>( static_cast<signed char>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned long>( static_cast<signed char>(0LL - 1) ), Lsc::UnderflowException );

		//int -> size_t

    assert( 0LL == Lsc::check_cast<size_t>( static_cast<int>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<size_t>( static_cast<int>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<size_t>( static_cast<int>(0LL - 1) ), Lsc::UnderflowException );

		//unsigned char -> size_t

    assert( 0LL == Lsc::check_cast<size_t>( static_cast<unsigned char>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<size_t>( static_cast<unsigned char>(0LL + 1) ) ) ;

		//unsigned long long -> size_t

    assert( 0LL == Lsc::check_cast<size_t>( static_cast<unsigned long long>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<size_t>( static_cast<unsigned long long>(0LL + 1) ) ) ;


    assert( 4294967295LL - 1== Lsc::check_cast<size_t>( static_cast<unsigned long long>(4294967295LL - 1) ) ) ;
    assert( 4294967295LL == Lsc::check_cast<size_t>( static_cast<unsigned long long>(4294967295LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<size_t>( static_cast<unsigned long long>(4294967295LL + 1) ), Lsc::OverflowException );

		//char -> size_t

    assert( 0LL == Lsc::check_cast<size_t>( static_cast<char>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<size_t>( static_cast<char>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<size_t>( static_cast<char>(0LL - 1) ), Lsc::UnderflowException );

		//unsigned long -> size_t

    assert( 0LL == Lsc::check_cast<size_t>( static_cast<unsigned long>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<size_t>( static_cast<unsigned long>(0LL + 1) ) ) ;


    assert( 4294967295LL - 1== Lsc::check_cast<size_t>( static_cast<unsigned long>(4294967295LL - 1) ) ) ;
    assert( 4294967295LL == Lsc::check_cast<size_t>( static_cast<unsigned long>(4294967295LL) ) ) ;

		//size_t -> size_t

    assert( 0LL == Lsc::check_cast<size_t>( static_cast<size_t>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<size_t>( static_cast<size_t>(0LL + 1) ) ) ;


    assert( 4294967295LL - 1== Lsc::check_cast<size_t>( static_cast<size_t>(4294967295LL - 1) ) ) ;
    assert( 4294967295LL == Lsc::check_cast<size_t>( static_cast<size_t>(4294967295LL) ) ) ;

		//unsigned int -> size_t

    assert( 0LL == Lsc::check_cast<size_t>( static_cast<unsigned int>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<size_t>( static_cast<unsigned int>(0LL + 1) ) ) ;


    assert( 4294967295LL - 1== Lsc::check_cast<size_t>( static_cast<unsigned int>(4294967295LL - 1) ) ) ;
    assert( 4294967295LL == Lsc::check_cast<size_t>( static_cast<unsigned int>(4294967295LL) ) ) ;

		//short -> size_t

    assert( 0LL == Lsc::check_cast<size_t>( static_cast<short>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<size_t>( static_cast<short>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<size_t>( static_cast<short>(0LL - 1) ), Lsc::UnderflowException );

		//long -> size_t

    assert( 0LL == Lsc::check_cast<size_t>( static_cast<long>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<size_t>( static_cast<long>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<size_t>( static_cast<long>(0LL - 1) ), Lsc::UnderflowException );

		//unsigned short -> size_t

    assert( 0LL == Lsc::check_cast<size_t>( static_cast<unsigned short>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<size_t>( static_cast<unsigned short>(0LL + 1) ) ) ;

		//long long -> size_t

    assert( 0LL == Lsc::check_cast<size_t>( static_cast<long long>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<size_t>( static_cast<long long>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<size_t>( static_cast<long long>(0LL - 1) ), Lsc::UnderflowException );


    assert( 4294967295LL - 1== Lsc::check_cast<size_t>( static_cast<long long>(4294967295LL - 1) ) ) ;
    assert( 4294967295LL == Lsc::check_cast<size_t>( static_cast<long long>(4294967295LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<size_t>( static_cast<long long>(4294967295LL + 1) ), Lsc::OverflowException );

		//signed char -> size_t

    assert( 0LL == Lsc::check_cast<size_t>( static_cast<signed char>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<size_t>( static_cast<signed char>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<size_t>( static_cast<signed char>(0LL - 1) ), Lsc::UnderflowException );

		//int -> unsigned int

    assert( 0LL == Lsc::check_cast<unsigned int>( static_cast<int>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned int>( static_cast<int>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned int>( static_cast<int>(0LL - 1) ), Lsc::UnderflowException );

		//unsigned char -> unsigned int

    assert( 0LL == Lsc::check_cast<unsigned int>( static_cast<unsigned char>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned int>( static_cast<unsigned char>(0LL + 1) ) ) ;

		//unsigned long long -> unsigned int

    assert( 0LL == Lsc::check_cast<unsigned int>( static_cast<unsigned long long>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned int>( static_cast<unsigned long long>(0LL + 1) ) ) ;


    assert( 4294967295LL - 1== Lsc::check_cast<unsigned int>( static_cast<unsigned long long>(4294967295LL - 1) ) ) ;
    assert( 4294967295LL == Lsc::check_cast<unsigned int>( static_cast<unsigned long long>(4294967295LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned int>( static_cast<unsigned long long>(4294967295LL + 1) ), Lsc::OverflowException );

		//char -> unsigned int

    assert( 0LL == Lsc::check_cast<unsigned int>( static_cast<char>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned int>( static_cast<char>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned int>( static_cast<char>(0LL - 1) ), Lsc::UnderflowException );

		//unsigned long -> unsigned int

    assert( 0LL == Lsc::check_cast<unsigned int>( static_cast<unsigned long>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned int>( static_cast<unsigned long>(0LL + 1) ) ) ;


    assert( 4294967295LL - 1== Lsc::check_cast<unsigned int>( static_cast<unsigned long>(4294967295LL - 1) ) ) ;
    assert( 4294967295LL == Lsc::check_cast<unsigned int>( static_cast<unsigned long>(4294967295LL) ) ) ;

		//size_t -> unsigned int

    assert( 0LL == Lsc::check_cast<unsigned int>( static_cast<size_t>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned int>( static_cast<size_t>(0LL + 1) ) ) ;


    assert( 4294967295LL - 1== Lsc::check_cast<unsigned int>( static_cast<size_t>(4294967295LL - 1) ) ) ;
    assert( 4294967295LL == Lsc::check_cast<unsigned int>( static_cast<size_t>(4294967295LL) ) ) ;

		//unsigned int -> unsigned int

    assert( 0LL == Lsc::check_cast<unsigned int>( static_cast<unsigned int>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned int>( static_cast<unsigned int>(0LL + 1) ) ) ;


    assert( 4294967295LL - 1== Lsc::check_cast<unsigned int>( static_cast<unsigned int>(4294967295LL - 1) ) ) ;
    assert( 4294967295LL == Lsc::check_cast<unsigned int>( static_cast<unsigned int>(4294967295LL) ) ) ;

		//short -> unsigned int

    assert( 0LL == Lsc::check_cast<unsigned int>( static_cast<short>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned int>( static_cast<short>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned int>( static_cast<short>(0LL - 1) ), Lsc::UnderflowException );

		//long -> unsigned int

    assert( 0LL == Lsc::check_cast<unsigned int>( static_cast<long>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned int>( static_cast<long>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned int>( static_cast<long>(0LL - 1) ), Lsc::UnderflowException );

		//unsigned short -> unsigned int

    assert( 0LL == Lsc::check_cast<unsigned int>( static_cast<unsigned short>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned int>( static_cast<unsigned short>(0LL + 1) ) ) ;

		//long long -> unsigned int

    assert( 0LL == Lsc::check_cast<unsigned int>( static_cast<long long>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned int>( static_cast<long long>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned int>( static_cast<long long>(0LL - 1) ), Lsc::UnderflowException );


    assert( 4294967295LL - 1== Lsc::check_cast<unsigned int>( static_cast<long long>(4294967295LL - 1) ) ) ;
    assert( 4294967295LL == Lsc::check_cast<unsigned int>( static_cast<long long>(4294967295LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned int>( static_cast<long long>(4294967295LL + 1) ), Lsc::OverflowException );

		//signed char -> unsigned int

    assert( 0LL == Lsc::check_cast<unsigned int>( static_cast<signed char>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned int>( static_cast<signed char>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned int>( static_cast<signed char>(0LL - 1) ), Lsc::UnderflowException );

		//int -> short

    assert( -32768LL == Lsc::check_cast<short>( static_cast<int>(-32768LL) ) ) ;
    assert( -32768LL + 1 == Lsc::check_cast<short>( static_cast<int>(-32768LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<short>( static_cast<int>(-32768LL - 1) ), Lsc::UnderflowException );


    assert( 32767LL - 1== Lsc::check_cast<short>( static_cast<int>(32767LL - 1) ) ) ;
    assert( 32767LL == Lsc::check_cast<short>( static_cast<int>(32767LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<short>( static_cast<int>(32767LL + 1) ), Lsc::OverflowException );

		//unsigned char -> short
		//unsigned long long -> short

    assert( 32767LL - 1== Lsc::check_cast<short>( static_cast<unsigned long long>(32767LL - 1) ) ) ;
    assert( 32767LL == Lsc::check_cast<short>( static_cast<unsigned long long>(32767LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<short>( static_cast<unsigned long long>(32767LL + 1) ), Lsc::OverflowException );

		//char -> short
		//unsigned long -> short

    assert( 32767LL - 1== Lsc::check_cast<short>( static_cast<unsigned long>(32767LL - 1) ) ) ;
    assert( 32767LL == Lsc::check_cast<short>( static_cast<unsigned long>(32767LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<short>( static_cast<unsigned long>(32767LL + 1) ), Lsc::OverflowException );

		//size_t -> short

    assert( 32767LL - 1== Lsc::check_cast<short>( static_cast<size_t>(32767LL - 1) ) ) ;
    assert( 32767LL == Lsc::check_cast<short>( static_cast<size_t>(32767LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<short>( static_cast<size_t>(32767LL + 1) ), Lsc::OverflowException );

		//unsigned int -> short

    assert( 32767LL - 1== Lsc::check_cast<short>( static_cast<unsigned int>(32767LL - 1) ) ) ;
    assert( 32767LL == Lsc::check_cast<short>( static_cast<unsigned int>(32767LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<short>( static_cast<unsigned int>(32767LL + 1) ), Lsc::OverflowException );

		//short -> short

    assert( -32768LL == Lsc::check_cast<short>( static_cast<short>(-32768LL) ) ) ;
    assert( -32768LL + 1 == Lsc::check_cast<short>( static_cast<short>(-32768LL + 1) ) ) ;


    assert( 32767LL - 1== Lsc::check_cast<short>( static_cast<short>(32767LL - 1) ) ) ;
    assert( 32767LL == Lsc::check_cast<short>( static_cast<short>(32767LL) ) ) ;

		//long -> short

    assert( -32768LL == Lsc::check_cast<short>( static_cast<long>(-32768LL) ) ) ;
    assert( -32768LL + 1 == Lsc::check_cast<short>( static_cast<long>(-32768LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<short>( static_cast<long>(-32768LL - 1) ), Lsc::UnderflowException );


    assert( 32767LL - 1== Lsc::check_cast<short>( static_cast<long>(32767LL - 1) ) ) ;
    assert( 32767LL == Lsc::check_cast<short>( static_cast<long>(32767LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<short>( static_cast<long>(32767LL + 1) ), Lsc::OverflowException );

		//unsigned short -> short

    assert( 32767LL - 1== Lsc::check_cast<short>( static_cast<unsigned short>(32767LL - 1) ) ) ;
    assert( 32767LL == Lsc::check_cast<short>( static_cast<unsigned short>(32767LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<short>( static_cast<unsigned short>(32767LL + 1) ), Lsc::OverflowException );

		//long long -> short

    assert( -32768LL == Lsc::check_cast<short>( static_cast<long long>(-32768LL) ) ) ;
    assert( -32768LL + 1 == Lsc::check_cast<short>( static_cast<long long>(-32768LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<short>( static_cast<long long>(-32768LL - 1) ), Lsc::UnderflowException );


    assert( 32767LL - 1== Lsc::check_cast<short>( static_cast<long long>(32767LL - 1) ) ) ;
    assert( 32767LL == Lsc::check_cast<short>( static_cast<long long>(32767LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<short>( static_cast<long long>(32767LL + 1) ), Lsc::OverflowException );

		//signed char -> short
		//int -> long

    assert( -2147483648LL == Lsc::check_cast<long>( static_cast<int>(-2147483648LL) ) ) ;
    assert( -2147483648LL + 1 == Lsc::check_cast<long>( static_cast<int>(-2147483648LL + 1) ) ) ;


    assert( 2147483647LL - 1== Lsc::check_cast<long>( static_cast<int>(2147483647LL - 1) ) ) ;
    assert( 2147483647LL == Lsc::check_cast<long>( static_cast<int>(2147483647LL) ) ) ;

		//unsigned char -> long
		//unsigned long long -> long

    assert( 2147483647LL - 1== Lsc::check_cast<long>( static_cast<unsigned long long>(2147483647LL - 1) ) ) ;
    assert( 2147483647LL == Lsc::check_cast<long>( static_cast<unsigned long long>(2147483647LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<long>( static_cast<unsigned long long>(2147483647LL + 1) ), Lsc::OverflowException );

		//char -> long
		//unsigned long -> long

    assert( 2147483647LL - 1== Lsc::check_cast<long>( static_cast<unsigned long>(2147483647LL - 1) ) ) ;
    assert( 2147483647LL == Lsc::check_cast<long>( static_cast<unsigned long>(2147483647LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<long>( static_cast<unsigned long>(2147483647LL + 1) ), Lsc::OverflowException );

		//size_t -> long

    assert( 2147483647LL - 1== Lsc::check_cast<long>( static_cast<size_t>(2147483647LL - 1) ) ) ;
    assert( 2147483647LL == Lsc::check_cast<long>( static_cast<size_t>(2147483647LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<long>( static_cast<size_t>(2147483647LL + 1) ), Lsc::OverflowException );

		//unsigned int -> long

    assert( 2147483647LL - 1== Lsc::check_cast<long>( static_cast<unsigned int>(2147483647LL - 1) ) ) ;
    assert( 2147483647LL == Lsc::check_cast<long>( static_cast<unsigned int>(2147483647LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<long>( static_cast<unsigned int>(2147483647LL + 1) ), Lsc::OverflowException );

		//short -> long
		//long -> long

    assert( -2147483648LL == Lsc::check_cast<long>( static_cast<long>(-2147483648LL) ) ) ;
    assert( -2147483648LL + 1 == Lsc::check_cast<long>( static_cast<long>(-2147483648LL + 1) ) ) ;


    assert( 2147483647LL - 1== Lsc::check_cast<long>( static_cast<long>(2147483647LL - 1) ) ) ;
    assert( 2147483647LL == Lsc::check_cast<long>( static_cast<long>(2147483647LL) ) ) ;

		//unsigned short -> long
		//long long -> long

    assert( -2147483648LL == Lsc::check_cast<long>( static_cast<long long>(-2147483648LL) ) ) ;
    assert( -2147483648LL + 1 == Lsc::check_cast<long>( static_cast<long long>(-2147483648LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<long>( static_cast<long long>(-2147483648LL - 1) ), Lsc::UnderflowException );


    assert( 2147483647LL - 1== Lsc::check_cast<long>( static_cast<long long>(2147483647LL - 1) ) ) ;
    assert( 2147483647LL == Lsc::check_cast<long>( static_cast<long long>(2147483647LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<long>( static_cast<long long>(2147483647LL + 1) ), Lsc::OverflowException );

		//signed char -> long
		//int -> unsigned short

    assert( 0LL == Lsc::check_cast<unsigned short>( static_cast<int>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned short>( static_cast<int>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned short>( static_cast<int>(0LL - 1) ), Lsc::UnderflowException );


    assert( 65535LL - 1== Lsc::check_cast<unsigned short>( static_cast<int>(65535LL - 1) ) ) ;
    assert( 65535LL == Lsc::check_cast<unsigned short>( static_cast<int>(65535LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned short>( static_cast<int>(65535LL + 1) ), Lsc::OverflowException );

		//unsigned char -> unsigned short

    assert( 0LL == Lsc::check_cast<unsigned short>( static_cast<unsigned char>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned short>( static_cast<unsigned char>(0LL + 1) ) ) ;

		//unsigned long long -> unsigned short

    assert( 0LL == Lsc::check_cast<unsigned short>( static_cast<unsigned long long>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned short>( static_cast<unsigned long long>(0LL + 1) ) ) ;


    assert( 65535LL - 1== Lsc::check_cast<unsigned short>( static_cast<unsigned long long>(65535LL - 1) ) ) ;
    assert( 65535LL == Lsc::check_cast<unsigned short>( static_cast<unsigned long long>(65535LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned short>( static_cast<unsigned long long>(65535LL + 1) ), Lsc::OverflowException );

		//char -> unsigned short

    assert( 0LL == Lsc::check_cast<unsigned short>( static_cast<char>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned short>( static_cast<char>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned short>( static_cast<char>(0LL - 1) ), Lsc::UnderflowException );

		//unsigned long -> unsigned short

    assert( 0LL == Lsc::check_cast<unsigned short>( static_cast<unsigned long>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned short>( static_cast<unsigned long>(0LL + 1) ) ) ;


    assert( 65535LL - 1== Lsc::check_cast<unsigned short>( static_cast<unsigned long>(65535LL - 1) ) ) ;
    assert( 65535LL == Lsc::check_cast<unsigned short>( static_cast<unsigned long>(65535LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned short>( static_cast<unsigned long>(65535LL + 1) ), Lsc::OverflowException );

		//size_t -> unsigned short

    assert( 0LL == Lsc::check_cast<unsigned short>( static_cast<size_t>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned short>( static_cast<size_t>(0LL + 1) ) ) ;


    assert( 65535LL - 1== Lsc::check_cast<unsigned short>( static_cast<size_t>(65535LL - 1) ) ) ;
    assert( 65535LL == Lsc::check_cast<unsigned short>( static_cast<size_t>(65535LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned short>( static_cast<size_t>(65535LL + 1) ), Lsc::OverflowException );

		//unsigned int -> unsigned short

    assert( 0LL == Lsc::check_cast<unsigned short>( static_cast<unsigned int>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned short>( static_cast<unsigned int>(0LL + 1) ) ) ;


    assert( 65535LL - 1== Lsc::check_cast<unsigned short>( static_cast<unsigned int>(65535LL - 1) ) ) ;
    assert( 65535LL == Lsc::check_cast<unsigned short>( static_cast<unsigned int>(65535LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned short>( static_cast<unsigned int>(65535LL + 1) ), Lsc::OverflowException );

		//short -> unsigned short

    assert( 0LL == Lsc::check_cast<unsigned short>( static_cast<short>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned short>( static_cast<short>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned short>( static_cast<short>(0LL - 1) ), Lsc::UnderflowException );

		//long -> unsigned short

    assert( 0LL == Lsc::check_cast<unsigned short>( static_cast<long>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned short>( static_cast<long>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned short>( static_cast<long>(0LL - 1) ), Lsc::UnderflowException );


    assert( 65535LL - 1== Lsc::check_cast<unsigned short>( static_cast<long>(65535LL - 1) ) ) ;
    assert( 65535LL == Lsc::check_cast<unsigned short>( static_cast<long>(65535LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned short>( static_cast<long>(65535LL + 1) ), Lsc::OverflowException );

		//unsigned short -> unsigned short

    assert( 0LL == Lsc::check_cast<unsigned short>( static_cast<unsigned short>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned short>( static_cast<unsigned short>(0LL + 1) ) ) ;


    assert( 65535LL - 1== Lsc::check_cast<unsigned short>( static_cast<unsigned short>(65535LL - 1) ) ) ;
    assert( 65535LL == Lsc::check_cast<unsigned short>( static_cast<unsigned short>(65535LL) ) ) ;

		//long long -> unsigned short

    assert( 0LL == Lsc::check_cast<unsigned short>( static_cast<long long>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned short>( static_cast<long long>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned short>( static_cast<long long>(0LL - 1) ), Lsc::UnderflowException );


    assert( 65535LL - 1== Lsc::check_cast<unsigned short>( static_cast<long long>(65535LL - 1) ) ) ;
    assert( 65535LL == Lsc::check_cast<unsigned short>( static_cast<long long>(65535LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned short>( static_cast<long long>(65535LL + 1) ), Lsc::OverflowException );

		//signed char -> unsigned short

    assert( 0LL == Lsc::check_cast<unsigned short>( static_cast<signed char>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned short>( static_cast<signed char>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned short>( static_cast<signed char>(0LL - 1) ), Lsc::UnderflowException );

		//int -> long long
		//unsigned char -> long long
		//unsigned long long -> long long

    assert( 9223372036854775807ULL - 1== Lsc::check_cast<long long>( static_cast<unsigned long long>(9223372036854775807ULL - 1) ) ) ;
    assert( 9223372036854775807ULL == Lsc::check_cast<long long>( static_cast<unsigned long long>(9223372036854775807ULL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<long long>( static_cast<unsigned long long>(9223372036854775807ULL + 1) ), Lsc::OverflowException );

		//char -> long long
		//unsigned long -> long long
		//size_t -> long long
		//unsigned int -> long long
		//short -> long long
		//long -> long long
		//unsigned short -> long long
		//long long -> long long

    assert( -9223372036854775808.0L == Lsc::check_cast<long long>( static_cast<long long>(-9223372036854775808.0L) ) ) ;
    assert( -9223372036854775808.0L + 1 == Lsc::check_cast<long long>( static_cast<long long>(-9223372036854775808.0L + 1) ) ) ;


    assert( 9223372036854775807ULL - 1== Lsc::check_cast<long long>( static_cast<long long>(9223372036854775807ULL - 1) ) ) ;
    assert( 9223372036854775807ULL == Lsc::check_cast<long long>( static_cast<long long>(9223372036854775807ULL) ) ) ;

		//signed char -> long long
		//int -> signed char

    assert( -128LL == Lsc::check_cast<signed char>( static_cast<int>(-128LL) ) ) ;
    assert( -128LL + 1 == Lsc::check_cast<signed char>( static_cast<int>(-128LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<signed char>( static_cast<int>(-128LL - 1) ), Lsc::UnderflowException );


    assert( 127LL - 1== Lsc::check_cast<signed char>( static_cast<int>(127LL - 1) ) ) ;
    assert( 127LL == Lsc::check_cast<signed char>( static_cast<int>(127LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<signed char>( static_cast<int>(127LL + 1) ), Lsc::OverflowException );

		//unsigned char -> signed char

    assert( 127LL - 1== Lsc::check_cast<signed char>( static_cast<unsigned char>(127LL - 1) ) ) ;
    assert( 127LL == Lsc::check_cast<signed char>( static_cast<unsigned char>(127LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<signed char>( static_cast<unsigned char>(127LL + 1) ), Lsc::OverflowException );

		//unsigned long long -> signed char

    assert( 127LL - 1== Lsc::check_cast<signed char>( static_cast<unsigned long long>(127LL - 1) ) ) ;
    assert( 127LL == Lsc::check_cast<signed char>( static_cast<unsigned long long>(127LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<signed char>( static_cast<unsigned long long>(127LL + 1) ), Lsc::OverflowException );

		//char -> signed char

    assert( -128LL == Lsc::check_cast<signed char>( static_cast<char>(-128LL) ) ) ;
    assert( -128LL + 1 == Lsc::check_cast<signed char>( static_cast<char>(-128LL + 1) ) ) ;


    assert( 127LL - 1== Lsc::check_cast<signed char>( static_cast<char>(127LL - 1) ) ) ;
    assert( 127LL == Lsc::check_cast<signed char>( static_cast<char>(127LL) ) ) ;

		//unsigned long -> signed char

    assert( 127LL - 1== Lsc::check_cast<signed char>( static_cast<unsigned long>(127LL - 1) ) ) ;
    assert( 127LL == Lsc::check_cast<signed char>( static_cast<unsigned long>(127LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<signed char>( static_cast<unsigned long>(127LL + 1) ), Lsc::OverflowException );

		//size_t -> signed char

    assert( 127LL - 1== Lsc::check_cast<signed char>( static_cast<size_t>(127LL - 1) ) ) ;
    assert( 127LL == Lsc::check_cast<signed char>( static_cast<size_t>(127LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<signed char>( static_cast<size_t>(127LL + 1) ), Lsc::OverflowException );

		//unsigned int -> signed char

    assert( 127LL - 1== Lsc::check_cast<signed char>( static_cast<unsigned int>(127LL - 1) ) ) ;
    assert( 127LL == Lsc::check_cast<signed char>( static_cast<unsigned int>(127LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<signed char>( static_cast<unsigned int>(127LL + 1) ), Lsc::OverflowException );

		//short -> signed char

    assert( -128LL == Lsc::check_cast<signed char>( static_cast<short>(-128LL) ) ) ;
    assert( -128LL + 1 == Lsc::check_cast<signed char>( static_cast<short>(-128LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<signed char>( static_cast<short>(-128LL - 1) ), Lsc::UnderflowException );


    assert( 127LL - 1== Lsc::check_cast<signed char>( static_cast<short>(127LL - 1) ) ) ;
    assert( 127LL == Lsc::check_cast<signed char>( static_cast<short>(127LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<signed char>( static_cast<short>(127LL + 1) ), Lsc::OverflowException );

		//long -> signed char

    assert( -128LL == Lsc::check_cast<signed char>( static_cast<long>(-128LL) ) ) ;
    assert( -128LL + 1 == Lsc::check_cast<signed char>( static_cast<long>(-128LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<signed char>( static_cast<long>(-128LL - 1) ), Lsc::UnderflowException );


    assert( 127LL - 1== Lsc::check_cast<signed char>( static_cast<long>(127LL - 1) ) ) ;
    assert( 127LL == Lsc::check_cast<signed char>( static_cast<long>(127LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<signed char>( static_cast<long>(127LL + 1) ), Lsc::OverflowException );

		//unsigned short -> signed char

    assert( 127LL - 1== Lsc::check_cast<signed char>( static_cast<unsigned short>(127LL - 1) ) ) ;
    assert( 127LL == Lsc::check_cast<signed char>( static_cast<unsigned short>(127LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<signed char>( static_cast<unsigned short>(127LL + 1) ), Lsc::OverflowException );

		//long long -> signed char

    assert( -128LL == Lsc::check_cast<signed char>( static_cast<long long>(-128LL) ) ) ;
    assert( -128LL + 1 == Lsc::check_cast<signed char>( static_cast<long long>(-128LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<signed char>( static_cast<long long>(-128LL - 1) ), Lsc::UnderflowException );


    assert( 127LL - 1== Lsc::check_cast<signed char>( static_cast<long long>(127LL - 1) ) ) ;
    assert( 127LL == Lsc::check_cast<signed char>( static_cast<long long>(127LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<signed char>( static_cast<long long>(127LL + 1) ), Lsc::OverflowException );

		//signed char -> signed char

    assert( -128LL == Lsc::check_cast<signed char>( static_cast<signed char>(-128LL) ) ) ;
    assert( -128LL + 1 == Lsc::check_cast<signed char>( static_cast<signed char>(-128LL + 1) ) ) ;


    assert( 127LL - 1== Lsc::check_cast<signed char>( static_cast<signed char>(127LL - 1) ) ) ;
    assert( 127LL == Lsc::check_cast<signed char>( static_cast<signed char>(127LL) ) ) ;

		//float -> int

    {
        //avoid floating-point number errors
        int value1 = static_cast<int>( static_cast<float>( -2147483648LL ) );
        int value2 = Lsc::check_cast<int>( static_cast<float>(-2147483648LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        int value1 = static_cast<int>( static_cast<float>( -2147483648LL + 2147483.647 ) );
        int value2 = Lsc::check_cast<int>( static_cast<float>(-2147483648LL + 2147483.647) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<int>( static_cast<float>(-2147483648LL - 2147483.647) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<int>( static_cast<float>(2147483647LL + 2147483.647) ), Lsc::OverflowException );

		//dolcle -> int

    {
        //avoid floating-point number errors
        int value1 = static_cast<int>( static_cast<dolcle>( -2147483648LL ) );
        int value2 = Lsc::check_cast<int>( static_cast<dolcle>(-2147483648LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        int value1 = static_cast<int>( static_cast<dolcle>( -2147483648LL + 2147483.647 ) );
        int value2 = Lsc::check_cast<int>( static_cast<dolcle>(-2147483648LL + 2147483.647) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<int>( static_cast<dolcle>(-2147483648LL - 2147483.647) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<int>( static_cast<dolcle>(2147483647LL + 2147483.647) ), Lsc::OverflowException );

		//long dolcle -> int

    {
        //avoid floating-point number errors
        int value1 = static_cast<int>( static_cast<long dolcle>( -2147483648LL ) );
        int value2 = Lsc::check_cast<int>( static_cast<long dolcle>(-2147483648LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        int value1 = static_cast<int>( static_cast<long dolcle>( -2147483648LL + 2147483.647 ) );
        int value2 = Lsc::check_cast<int>( static_cast<long dolcle>(-2147483648LL + 2147483.647) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<int>( static_cast<long dolcle>(-2147483648LL - 2147483.647) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<int>( static_cast<long dolcle>(2147483647LL + 2147483.647) ), Lsc::OverflowException );

		//float -> unsigned char

    {
        //avoid floating-point number errors
        unsigned char value1 = static_cast<unsigned char>( static_cast<float>( 0LL ) );
        unsigned char value2 = Lsc::check_cast<unsigned char>( static_cast<float>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        unsigned char value1 = static_cast<unsigned char>( static_cast<float>( 0LL + 0.255 ) );
        unsigned char value2 = Lsc::check_cast<unsigned char>( static_cast<float>(0LL + 0.255) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<unsigned char>( static_cast<float>(0LL - 0.255) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<unsigned char>( static_cast<float>(255LL + 0.255) ), Lsc::OverflowException );

		//dolcle -> unsigned char

    {
        //avoid floating-point number errors
        unsigned char value1 = static_cast<unsigned char>( static_cast<dolcle>( 0LL ) );
        unsigned char value2 = Lsc::check_cast<unsigned char>( static_cast<dolcle>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        unsigned char value1 = static_cast<unsigned char>( static_cast<dolcle>( 0LL + 0.255 ) );
        unsigned char value2 = Lsc::check_cast<unsigned char>( static_cast<dolcle>(0LL + 0.255) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<unsigned char>( static_cast<dolcle>(0LL - 0.255) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<unsigned char>( static_cast<dolcle>(255LL + 0.255) ), Lsc::OverflowException );

		//long dolcle -> unsigned char

    {
        //avoid floating-point number errors
        unsigned char value1 = static_cast<unsigned char>( static_cast<long dolcle>( 0LL ) );
        unsigned char value2 = Lsc::check_cast<unsigned char>( static_cast<long dolcle>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        unsigned char value1 = static_cast<unsigned char>( static_cast<long dolcle>( 0LL + 0.255 ) );
        unsigned char value2 = Lsc::check_cast<unsigned char>( static_cast<long dolcle>(0LL + 0.255) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<unsigned char>( static_cast<long dolcle>(0LL - 0.255) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<unsigned char>( static_cast<long dolcle>(255LL + 0.255) ), Lsc::OverflowException );

		//float -> unsigned long long

    {
        //avoid floating-point number errors
        unsigned long long value1 = static_cast<unsigned long long>( static_cast<float>( 0LL ) );
        unsigned long long value2 = Lsc::check_cast<unsigned long long>( static_cast<float>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        unsigned long long value1 = static_cast<unsigned long long>( static_cast<float>( 0LL + 1.84467440737e+16 ) );
        unsigned long long value2 = Lsc::check_cast<unsigned long long>( static_cast<float>(0LL + 1.84467440737e+16) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<unsigned long long>( static_cast<float>(0LL - 1.84467440737e+16) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<unsigned long long>( static_cast<float>(18446744073709551615ULL + 1.84467440737e+16) ), Lsc::OverflowException );

		//dolcle -> unsigned long long

    {
        //avoid floating-point number errors
        unsigned long long value1 = static_cast<unsigned long long>( static_cast<dolcle>( 0LL ) );
        unsigned long long value2 = Lsc::check_cast<unsigned long long>( static_cast<dolcle>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        unsigned long long value1 = static_cast<unsigned long long>( static_cast<dolcle>( 0LL + 1.84467440737e+16 ) );
        unsigned long long value2 = Lsc::check_cast<unsigned long long>( static_cast<dolcle>(0LL + 1.84467440737e+16) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<unsigned long long>( static_cast<dolcle>(0LL - 1.84467440737e+16) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<unsigned long long>( static_cast<dolcle>(18446744073709551615ULL + 1.84467440737e+16) ), Lsc::OverflowException );

		//long dolcle -> unsigned long long

    {
        //avoid floating-point number errors
        unsigned long long value1 = static_cast<unsigned long long>( static_cast<long dolcle>( 0LL ) );
        unsigned long long value2 = Lsc::check_cast<unsigned long long>( static_cast<long dolcle>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        unsigned long long value1 = static_cast<unsigned long long>( static_cast<long dolcle>( 0LL + 1.84467440737e+16 ) );
        unsigned long long value2 = Lsc::check_cast<unsigned long long>( static_cast<long dolcle>(0LL + 1.84467440737e+16) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<unsigned long long>( static_cast<long dolcle>(0LL - 1.84467440737e+16) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<unsigned long long>( static_cast<long dolcle>(18446744073709551615ULL + 1.84467440737e+16) ), Lsc::OverflowException );

		//float -> char

    {
        //avoid floating-point number errors
        char value1 = static_cast<char>( static_cast<float>( -128LL ) );
        char value2 = Lsc::check_cast<char>( static_cast<float>(-128LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        char value1 = static_cast<char>( static_cast<float>( -128LL + 0.127 ) );
        char value2 = Lsc::check_cast<char>( static_cast<float>(-128LL + 0.127) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<char>( static_cast<float>(-128LL - 0.127) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<char>( static_cast<float>(127LL + 0.127) ), Lsc::OverflowException );

		//dolcle -> char

    {
        //avoid floating-point number errors
        char value1 = static_cast<char>( static_cast<dolcle>( -128LL ) );
        char value2 = Lsc::check_cast<char>( static_cast<dolcle>(-128LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        char value1 = static_cast<char>( static_cast<dolcle>( -128LL + 0.127 ) );
        char value2 = Lsc::check_cast<char>( static_cast<dolcle>(-128LL + 0.127) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<char>( static_cast<dolcle>(-128LL - 0.127) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<char>( static_cast<dolcle>(127LL + 0.127) ), Lsc::OverflowException );

		//long dolcle -> char

    {
        //avoid floating-point number errors
        char value1 = static_cast<char>( static_cast<long dolcle>( -128LL ) );
        char value2 = Lsc::check_cast<char>( static_cast<long dolcle>(-128LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        char value1 = static_cast<char>( static_cast<long dolcle>( -128LL + 0.127 ) );
        char value2 = Lsc::check_cast<char>( static_cast<long dolcle>(-128LL + 0.127) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<char>( static_cast<long dolcle>(-128LL - 0.127) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<char>( static_cast<long dolcle>(127LL + 0.127) ), Lsc::OverflowException );

		//float -> unsigned long

    {
        //avoid floating-point number errors
        unsigned long value1 = static_cast<unsigned long>( static_cast<float>( 0LL ) );
        unsigned long value2 = Lsc::check_cast<unsigned long>( static_cast<float>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        unsigned long value1 = static_cast<unsigned long>( static_cast<float>( 0LL + 4294967.295 ) );
        unsigned long value2 = Lsc::check_cast<unsigned long>( static_cast<float>(0LL + 4294967.295) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<unsigned long>( static_cast<float>(0LL - 4294967.295) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<unsigned long>( static_cast<float>(4294967295LL + 4294967.295) ), Lsc::OverflowException );

		//dolcle -> unsigned long

    {
        //avoid floating-point number errors
        unsigned long value1 = static_cast<unsigned long>( static_cast<dolcle>( 0LL ) );
        unsigned long value2 = Lsc::check_cast<unsigned long>( static_cast<dolcle>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        unsigned long value1 = static_cast<unsigned long>( static_cast<dolcle>( 0LL + 4294967.295 ) );
        unsigned long value2 = Lsc::check_cast<unsigned long>( static_cast<dolcle>(0LL + 4294967.295) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<unsigned long>( static_cast<dolcle>(0LL - 4294967.295) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<unsigned long>( static_cast<dolcle>(4294967295LL + 4294967.295) ), Lsc::OverflowException );

		//long dolcle -> unsigned long

    {
        //avoid floating-point number errors
        unsigned long value1 = static_cast<unsigned long>( static_cast<long dolcle>( 0LL ) );
        unsigned long value2 = Lsc::check_cast<unsigned long>( static_cast<long dolcle>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        unsigned long value1 = static_cast<unsigned long>( static_cast<long dolcle>( 0LL + 4294967.295 ) );
        unsigned long value2 = Lsc::check_cast<unsigned long>( static_cast<long dolcle>(0LL + 4294967.295) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<unsigned long>( static_cast<long dolcle>(0LL - 4294967.295) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<unsigned long>( static_cast<long dolcle>(4294967295LL + 4294967.295) ), Lsc::OverflowException );

		//float -> size_t

    {
        //avoid floating-point number errors
        size_t value1 = static_cast<size_t>( static_cast<float>( 0LL ) );
        size_t value2 = Lsc::check_cast<size_t>( static_cast<float>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        size_t value1 = static_cast<size_t>( static_cast<float>( 0LL + 4294967.295 ) );
        size_t value2 = Lsc::check_cast<size_t>( static_cast<float>(0LL + 4294967.295) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<size_t>( static_cast<float>(0LL - 4294967.295) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<size_t>( static_cast<float>(4294967295LL + 4294967.295) ), Lsc::OverflowException );

		//dolcle -> size_t

    {
        //avoid floating-point number errors
        size_t value1 = static_cast<size_t>( static_cast<dolcle>( 0LL ) );
        size_t value2 = Lsc::check_cast<size_t>( static_cast<dolcle>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        size_t value1 = static_cast<size_t>( static_cast<dolcle>( 0LL + 4294967.295 ) );
        size_t value2 = Lsc::check_cast<size_t>( static_cast<dolcle>(0LL + 4294967.295) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<size_t>( static_cast<dolcle>(0LL - 4294967.295) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<size_t>( static_cast<dolcle>(4294967295LL + 4294967.295) ), Lsc::OverflowException );

		//long dolcle -> size_t

    {
        //avoid floating-point number errors
        size_t value1 = static_cast<size_t>( static_cast<long dolcle>( 0LL ) );
        size_t value2 = Lsc::check_cast<size_t>( static_cast<long dolcle>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        size_t value1 = static_cast<size_t>( static_cast<long dolcle>( 0LL + 4294967.295 ) );
        size_t value2 = Lsc::check_cast<size_t>( static_cast<long dolcle>(0LL + 4294967.295) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<size_t>( static_cast<long dolcle>(0LL - 4294967.295) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<size_t>( static_cast<long dolcle>(4294967295LL + 4294967.295) ), Lsc::OverflowException );

		//float -> unsigned int

    {
        //avoid floating-point number errors
        unsigned int value1 = static_cast<unsigned int>( static_cast<float>( 0LL ) );
        unsigned int value2 = Lsc::check_cast<unsigned int>( static_cast<float>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        unsigned int value1 = static_cast<unsigned int>( static_cast<float>( 0LL + 4294967.295 ) );
        unsigned int value2 = Lsc::check_cast<unsigned int>( static_cast<float>(0LL + 4294967.295) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<unsigned int>( static_cast<float>(0LL - 4294967.295) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<unsigned int>( static_cast<float>(4294967295LL + 4294967.295) ), Lsc::OverflowException );

		//dolcle -> unsigned int

    {
        //avoid floating-point number errors
        unsigned int value1 = static_cast<unsigned int>( static_cast<dolcle>( 0LL ) );
        unsigned int value2 = Lsc::check_cast<unsigned int>( static_cast<dolcle>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        unsigned int value1 = static_cast<unsigned int>( static_cast<dolcle>( 0LL + 4294967.295 ) );
        unsigned int value2 = Lsc::check_cast<unsigned int>( static_cast<dolcle>(0LL + 4294967.295) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<unsigned int>( static_cast<dolcle>(0LL - 4294967.295) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<unsigned int>( static_cast<dolcle>(4294967295LL + 4294967.295) ), Lsc::OverflowException );

		//long dolcle -> unsigned int

    {
        //avoid floating-point number errors
        unsigned int value1 = static_cast<unsigned int>( static_cast<long dolcle>( 0LL ) );
        unsigned int value2 = Lsc::check_cast<unsigned int>( static_cast<long dolcle>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        unsigned int value1 = static_cast<unsigned int>( static_cast<long dolcle>( 0LL + 4294967.295 ) );
        unsigned int value2 = Lsc::check_cast<unsigned int>( static_cast<long dolcle>(0LL + 4294967.295) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<unsigned int>( static_cast<long dolcle>(0LL - 4294967.295) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<unsigned int>( static_cast<long dolcle>(4294967295LL + 4294967.295) ), Lsc::OverflowException );

		//float -> short

    {
        //avoid floating-point number errors
        short value1 = static_cast<short>( static_cast<float>( -32768LL ) );
        short value2 = Lsc::check_cast<short>( static_cast<float>(-32768LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        short value1 = static_cast<short>( static_cast<float>( -32768LL + 32.767 ) );
        short value2 = Lsc::check_cast<short>( static_cast<float>(-32768LL + 32.767) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<short>( static_cast<float>(-32768LL - 32.767) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<short>( static_cast<float>(32767LL + 32.767) ), Lsc::OverflowException );

		//dolcle -> short

    {
        //avoid floating-point number errors
        short value1 = static_cast<short>( static_cast<dolcle>( -32768LL ) );
        short value2 = Lsc::check_cast<short>( static_cast<dolcle>(-32768LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        short value1 = static_cast<short>( static_cast<dolcle>( -32768LL + 32.767 ) );
        short value2 = Lsc::check_cast<short>( static_cast<dolcle>(-32768LL + 32.767) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<short>( static_cast<dolcle>(-32768LL - 32.767) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<short>( static_cast<dolcle>(32767LL + 32.767) ), Lsc::OverflowException );

		//long dolcle -> short

    {
        //avoid floating-point number errors
        short value1 = static_cast<short>( static_cast<long dolcle>( -32768LL ) );
        short value2 = Lsc::check_cast<short>( static_cast<long dolcle>(-32768LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        short value1 = static_cast<short>( static_cast<long dolcle>( -32768LL + 32.767 ) );
        short value2 = Lsc::check_cast<short>( static_cast<long dolcle>(-32768LL + 32.767) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<short>( static_cast<long dolcle>(-32768LL - 32.767) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<short>( static_cast<long dolcle>(32767LL + 32.767) ), Lsc::OverflowException );

		//float -> long

    {
        //avoid floating-point number errors
        long value1 = static_cast<long>( static_cast<float>( -2147483648LL ) );
        long value2 = Lsc::check_cast<long>( static_cast<float>(-2147483648LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long value1 = static_cast<long>( static_cast<float>( -2147483648LL + 2147483.647 ) );
        long value2 = Lsc::check_cast<long>( static_cast<float>(-2147483648LL + 2147483.647) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<long>( static_cast<float>(-2147483648LL - 2147483.647) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<long>( static_cast<float>(2147483647LL + 2147483.647) ), Lsc::OverflowException );

		//dolcle -> long

    {
        //avoid floating-point number errors
        long value1 = static_cast<long>( static_cast<dolcle>( -2147483648LL ) );
        long value2 = Lsc::check_cast<long>( static_cast<dolcle>(-2147483648LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long value1 = static_cast<long>( static_cast<dolcle>( -2147483648LL + 2147483.647 ) );
        long value2 = Lsc::check_cast<long>( static_cast<dolcle>(-2147483648LL + 2147483.647) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<long>( static_cast<dolcle>(-2147483648LL - 2147483.647) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<long>( static_cast<dolcle>(2147483647LL + 2147483.647) ), Lsc::OverflowException );

		//long dolcle -> long

    {
        //avoid floating-point number errors
        long value1 = static_cast<long>( static_cast<long dolcle>( -2147483648LL ) );
        long value2 = Lsc::check_cast<long>( static_cast<long dolcle>(-2147483648LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long value1 = static_cast<long>( static_cast<long dolcle>( -2147483648LL + 2147483.647 ) );
        long value2 = Lsc::check_cast<long>( static_cast<long dolcle>(-2147483648LL + 2147483.647) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<long>( static_cast<long dolcle>(-2147483648LL - 2147483.647) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<long>( static_cast<long dolcle>(2147483647LL + 2147483.647) ), Lsc::OverflowException );

		//float -> unsigned short

    {
        //avoid floating-point number errors
        unsigned short value1 = static_cast<unsigned short>( static_cast<float>( 0LL ) );
        unsigned short value2 = Lsc::check_cast<unsigned short>( static_cast<float>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        unsigned short value1 = static_cast<unsigned short>( static_cast<float>( 0LL + 65.535 ) );
        unsigned short value2 = Lsc::check_cast<unsigned short>( static_cast<float>(0LL + 65.535) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<unsigned short>( static_cast<float>(0LL - 65.535) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<unsigned short>( static_cast<float>(65535LL + 65.535) ), Lsc::OverflowException );

		//dolcle -> unsigned short

    {
        //avoid floating-point number errors
        unsigned short value1 = static_cast<unsigned short>( static_cast<dolcle>( 0LL ) );
        unsigned short value2 = Lsc::check_cast<unsigned short>( static_cast<dolcle>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        unsigned short value1 = static_cast<unsigned short>( static_cast<dolcle>( 0LL + 65.535 ) );
        unsigned short value2 = Lsc::check_cast<unsigned short>( static_cast<dolcle>(0LL + 65.535) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<unsigned short>( static_cast<dolcle>(0LL - 65.535) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<unsigned short>( static_cast<dolcle>(65535LL + 65.535) ), Lsc::OverflowException );

		//long dolcle -> unsigned short

    {
        //avoid floating-point number errors
        unsigned short value1 = static_cast<unsigned short>( static_cast<long dolcle>( 0LL ) );
        unsigned short value2 = Lsc::check_cast<unsigned short>( static_cast<long dolcle>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        unsigned short value1 = static_cast<unsigned short>( static_cast<long dolcle>( 0LL + 65.535 ) );
        unsigned short value2 = Lsc::check_cast<unsigned short>( static_cast<long dolcle>(0LL + 65.535) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<unsigned short>( static_cast<long dolcle>(0LL - 65.535) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<unsigned short>( static_cast<long dolcle>(65535LL + 65.535) ), Lsc::OverflowException );

		//float -> long long

    {
        //avoid floating-point number errors
        long long value1 = static_cast<long long>( static_cast<float>( -9223372036854775808.0L ) );
        long long value2 = Lsc::check_cast<long long>( static_cast<float>(-9223372036854775808.0L ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long long value1 = static_cast<long long>( static_cast<float>( -9223372036854775808.0L + 9.22337203685e+15 ) );
        long long value2 = Lsc::check_cast<long long>( static_cast<float>(-9223372036854775808.0L + 9.22337203685e+15) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<long long>( static_cast<float>(-9223372036854775808.0L - 9.22337203685e+15) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<long long>( static_cast<float>(9223372036854775807ULL + 9.22337203685e+15) ), Lsc::OverflowException );

		//dolcle -> long long

    {
        //avoid floating-point number errors
        long long value1 = static_cast<long long>( static_cast<dolcle>( -9223372036854775808.0L ) );
        long long value2 = Lsc::check_cast<long long>( static_cast<dolcle>(-9223372036854775808.0L ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long long value1 = static_cast<long long>( static_cast<dolcle>( -9223372036854775808.0L + 9.22337203685e+15 ) );
        long long value2 = Lsc::check_cast<long long>( static_cast<dolcle>(-9223372036854775808.0L + 9.22337203685e+15) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<long long>( static_cast<dolcle>(-9223372036854775808.0L - 9.22337203685e+15) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<long long>( static_cast<dolcle>(9223372036854775807ULL + 9.22337203685e+15) ), Lsc::OverflowException );

		//long dolcle -> long long

    {
        //avoid floating-point number errors
        long long value1 = static_cast<long long>( static_cast<long dolcle>( -9223372036854775808.0L ) );
        long long value2 = Lsc::check_cast<long long>( static_cast<long dolcle>(-9223372036854775808.0L ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long long value1 = static_cast<long long>( static_cast<long dolcle>( -9223372036854775808.0L + 9.22337203685e+15 ) );
        long long value2 = Lsc::check_cast<long long>( static_cast<long dolcle>(-9223372036854775808.0L + 9.22337203685e+15) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<long long>( static_cast<long dolcle>(-9223372036854775808.0L - 9.22337203685e+15) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<long long>( static_cast<long dolcle>(9223372036854775807ULL + 9.22337203685e+15) ), Lsc::OverflowException );

		//float -> signed char

    {
        //avoid floating-point number errors
        signed char value1 = static_cast<signed char>( static_cast<float>( -128LL ) );
        signed char value2 = Lsc::check_cast<signed char>( static_cast<float>(-128LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        signed char value1 = static_cast<signed char>( static_cast<float>( -128LL + 0.127 ) );
        signed char value2 = Lsc::check_cast<signed char>( static_cast<float>(-128LL + 0.127) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<signed char>( static_cast<float>(-128LL - 0.127) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<signed char>( static_cast<float>(127LL + 0.127) ), Lsc::OverflowException );

		//dolcle -> signed char

    {
        //avoid floating-point number errors
        signed char value1 = static_cast<signed char>( static_cast<dolcle>( -128LL ) );
        signed char value2 = Lsc::check_cast<signed char>( static_cast<dolcle>(-128LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        signed char value1 = static_cast<signed char>( static_cast<dolcle>( -128LL + 0.127 ) );
        signed char value2 = Lsc::check_cast<signed char>( static_cast<dolcle>(-128LL + 0.127) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<signed char>( static_cast<dolcle>(-128LL - 0.127) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<signed char>( static_cast<dolcle>(127LL + 0.127) ), Lsc::OverflowException );

		//long dolcle -> signed char

    {
        //avoid floating-point number errors
        signed char value1 = static_cast<signed char>( static_cast<long dolcle>( -128LL ) );
        signed char value2 = Lsc::check_cast<signed char>( static_cast<long dolcle>(-128LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        signed char value1 = static_cast<signed char>( static_cast<long dolcle>( -128LL + 0.127 ) );
        signed char value2 = Lsc::check_cast<signed char>( static_cast<long dolcle>(-128LL + 0.127) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<signed char>( static_cast<long dolcle>(-128LL - 0.127) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<signed char>( static_cast<long dolcle>(127LL + 0.127) ), Lsc::OverflowException );


    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<char>( 127LL - 1 ) );
        float value2 = Lsc::check_cast<float>( static_cast<char>(127LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<char>( 127LL ) );
        float value2 = Lsc::check_cast<float>( static_cast<char>(127LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<unsigned char>( 127LL - 1 ) );
        float value2 = Lsc::check_cast<float>( static_cast<unsigned char>(127LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<unsigned char>( 127LL ) );
        float value2 = Lsc::check_cast<float>( static_cast<unsigned char>(127LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<short>( 127LL - 1 ) );
        float value2 = Lsc::check_cast<float>( static_cast<short>(127LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<short>( 127LL ) );
        float value2 = Lsc::check_cast<float>( static_cast<short>(127LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<unsigned short>( 127LL - 1 ) );
        float value2 = Lsc::check_cast<float>( static_cast<unsigned short>(127LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<unsigned short>( 127LL ) );
        float value2 = Lsc::check_cast<float>( static_cast<unsigned short>(127LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<int>( 127LL - 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<int>(127LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<int>( 127LL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<int>(127LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<unsigned char>( 127LL - 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<unsigned char>(127LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<unsigned char>( 127LL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<unsigned char>(127LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<char>( 127LL - 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<char>(127LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<char>( 127LL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<char>(127LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<unsigned long>( 127LL - 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<unsigned long>(127LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<unsigned long>( 127LL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<unsigned long>(127LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<size_t>( 127LL - 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<size_t>(127LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<size_t>( 127LL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<size_t>(127LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<unsigned int>( 127LL - 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<unsigned int>(127LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<unsigned int>( 127LL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<unsigned int>(127LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<short>( 127LL - 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<short>(127LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<short>( 127LL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<short>(127LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<long>( 127LL - 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<long>(127LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<long>( 127LL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<long>(127LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<unsigned short>( 127LL - 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<unsigned short>(127LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<unsigned short>( 127LL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<unsigned short>(127LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<signed char>( 127LL - 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<signed char>(127LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<signed char>( 127LL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<signed char>(127LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<int>( 127LL - 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<int>(127LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<int>( 127LL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<int>(127LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<unsigned char>( 127LL - 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<unsigned char>(127LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<unsigned char>( 127LL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<unsigned char>(127LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<char>( 127LL - 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<char>(127LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<char>( 127LL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<char>(127LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<unsigned long>( 127LL - 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<unsigned long>(127LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<unsigned long>( 127LL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<unsigned long>(127LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<size_t>( 127LL - 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<size_t>(127LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<size_t>( 127LL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<size_t>(127LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<unsigned int>( 127LL - 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<unsigned int>(127LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<unsigned int>( 127LL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<unsigned int>(127LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<short>( 127LL - 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<short>(127LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<short>( 127LL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<short>(127LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<long>( 127LL - 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<long>(127LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<long>( 127LL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<long>(127LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<unsigned short>( 127LL - 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<unsigned short>(127LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<unsigned short>( 127LL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<unsigned short>(127LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<long long>( 127LL - 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<long long>(127LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<long long>( 127LL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<long long>(127LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<signed char>( 127LL - 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<signed char>(127LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<signed char>( 127LL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<signed char>(127LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<int>( -2147483648LL ) );
        float value2 = Lsc::check_cast<float>( static_cast<int>(-2147483648LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<int>( -2147483648LL + 1 ) );
        float value2 = Lsc::check_cast<float>( static_cast<int>(-2147483648LL + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<int>( 2147483647LL - 1 ) );
        float value2 = Lsc::check_cast<float>( static_cast<int>(2147483647LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<int>( 2147483647LL ) );
        float value2 = Lsc::check_cast<float>( static_cast<int>(2147483647LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<int>( -2147483648LL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<int>(-2147483648LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<int>( -2147483648LL + 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<int>(-2147483648LL + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<int>( 2147483647LL - 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<int>(2147483647LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<int>( 2147483647LL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<int>(2147483647LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<int>( -2147483648LL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<int>(-2147483648LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<int>( -2147483648LL + 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<int>(-2147483648LL + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<int>( 2147483647LL - 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<int>(2147483647LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<int>( 2147483647LL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<int>(2147483647LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<unsigned char>( 0LL ) );
        float value2 = Lsc::check_cast<float>( static_cast<unsigned char>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<unsigned char>( 0LL + 1 ) );
        float value2 = Lsc::check_cast<float>( static_cast<unsigned char>(0LL + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<unsigned char>( 255LL - 1 ) );
        float value2 = Lsc::check_cast<float>( static_cast<unsigned char>(255LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<unsigned char>( 255LL ) );
        float value2 = Lsc::check_cast<float>( static_cast<unsigned char>(255LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<unsigned char>( 0LL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<unsigned char>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<unsigned char>( 0LL + 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<unsigned char>(0LL + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<unsigned char>( 255LL - 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<unsigned char>(255LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<unsigned char>( 255LL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<unsigned char>(255LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<unsigned char>( 0LL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<unsigned char>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<unsigned char>( 0LL + 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<unsigned char>(0LL + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<unsigned char>( 255LL - 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<unsigned char>(255LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<unsigned char>( 255LL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<unsigned char>(255LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<unsigned long long>( 0LL ) );
        float value2 = Lsc::check_cast<float>( static_cast<unsigned long long>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<unsigned long long>( 0LL + 1 ) );
        float value2 = Lsc::check_cast<float>( static_cast<unsigned long long>(0LL + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<unsigned long long>( 18446744073709551615ULL - 1 ) );
        float value2 = Lsc::check_cast<float>( static_cast<unsigned long long>(18446744073709551615ULL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<unsigned long long>( 18446744073709551615ULL ) );
        float value2 = Lsc::check_cast<float>( static_cast<unsigned long long>(18446744073709551615ULL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<unsigned long long>( 0LL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<unsigned long long>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<unsigned long long>( 0LL + 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<unsigned long long>(0LL + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<unsigned long long>( 18446744073709551615ULL - 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<unsigned long long>(18446744073709551615ULL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<unsigned long long>( 18446744073709551615ULL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<unsigned long long>(18446744073709551615ULL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<unsigned long long>( 0LL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<unsigned long long>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<unsigned long long>( 0LL + 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<unsigned long long>(0LL + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<unsigned long long>( 18446744073709551615ULL - 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<unsigned long long>(18446744073709551615ULL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<unsigned long long>( 18446744073709551615ULL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<unsigned long long>(18446744073709551615ULL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<char>( -128LL ) );
        float value2 = Lsc::check_cast<float>( static_cast<char>(-128LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<char>( -128LL + 1 ) );
        float value2 = Lsc::check_cast<float>( static_cast<char>(-128LL + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<char>( 127LL - 1 ) );
        float value2 = Lsc::check_cast<float>( static_cast<char>(127LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<char>( 127LL ) );
        float value2 = Lsc::check_cast<float>( static_cast<char>(127LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<char>( -128LL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<char>(-128LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<char>( -128LL + 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<char>(-128LL + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<char>( 127LL - 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<char>(127LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<char>( 127LL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<char>(127LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<char>( -128LL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<char>(-128LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<char>( -128LL + 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<char>(-128LL + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<char>( 127LL - 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<char>(127LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<char>( 127LL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<char>(127LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<unsigned long>( 0LL ) );
        float value2 = Lsc::check_cast<float>( static_cast<unsigned long>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<unsigned long>( 0LL + 1 ) );
        float value2 = Lsc::check_cast<float>( static_cast<unsigned long>(0LL + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<unsigned long>( 4294967295LL - 1 ) );
        float value2 = Lsc::check_cast<float>( static_cast<unsigned long>(4294967295LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<unsigned long>( 4294967295LL ) );
        float value2 = Lsc::check_cast<float>( static_cast<unsigned long>(4294967295LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<unsigned long>( 0LL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<unsigned long>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<unsigned long>( 0LL + 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<unsigned long>(0LL + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<unsigned long>( 4294967295LL - 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<unsigned long>(4294967295LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<unsigned long>( 4294967295LL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<unsigned long>(4294967295LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<unsigned long>( 0LL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<unsigned long>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<unsigned long>( 0LL + 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<unsigned long>(0LL + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<unsigned long>( 4294967295LL - 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<unsigned long>(4294967295LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<unsigned long>( 4294967295LL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<unsigned long>(4294967295LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<size_t>( 0LL ) );
        float value2 = Lsc::check_cast<float>( static_cast<size_t>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<size_t>( 0LL + 1 ) );
        float value2 = Lsc::check_cast<float>( static_cast<size_t>(0LL + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<size_t>( 4294967295LL - 1 ) );
        float value2 = Lsc::check_cast<float>( static_cast<size_t>(4294967295LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<size_t>( 4294967295LL ) );
        float value2 = Lsc::check_cast<float>( static_cast<size_t>(4294967295LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<size_t>( 0LL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<size_t>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<size_t>( 0LL + 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<size_t>(0LL + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<size_t>( 4294967295LL - 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<size_t>(4294967295LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<size_t>( 4294967295LL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<size_t>(4294967295LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<size_t>( 0LL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<size_t>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<size_t>( 0LL + 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<size_t>(0LL + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<size_t>( 4294967295LL - 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<size_t>(4294967295LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<size_t>( 4294967295LL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<size_t>(4294967295LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<unsigned int>( 0LL ) );
        float value2 = Lsc::check_cast<float>( static_cast<unsigned int>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<unsigned int>( 0LL + 1 ) );
        float value2 = Lsc::check_cast<float>( static_cast<unsigned int>(0LL + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<unsigned int>( 4294967295LL - 1 ) );
        float value2 = Lsc::check_cast<float>( static_cast<unsigned int>(4294967295LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<unsigned int>( 4294967295LL ) );
        float value2 = Lsc::check_cast<float>( static_cast<unsigned int>(4294967295LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<unsigned int>( 0LL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<unsigned int>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<unsigned int>( 0LL + 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<unsigned int>(0LL + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<unsigned int>( 4294967295LL - 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<unsigned int>(4294967295LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<unsigned int>( 4294967295LL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<unsigned int>(4294967295LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<unsigned int>( 0LL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<unsigned int>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<unsigned int>( 0LL + 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<unsigned int>(0LL + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<unsigned int>( 4294967295LL - 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<unsigned int>(4294967295LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<unsigned int>( 4294967295LL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<unsigned int>(4294967295LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<short>( -32768LL ) );
        float value2 = Lsc::check_cast<float>( static_cast<short>(-32768LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<short>( -32768LL + 1 ) );
        float value2 = Lsc::check_cast<float>( static_cast<short>(-32768LL + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<short>( 32767LL - 1 ) );
        float value2 = Lsc::check_cast<float>( static_cast<short>(32767LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<short>( 32767LL ) );
        float value2 = Lsc::check_cast<float>( static_cast<short>(32767LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<short>( -32768LL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<short>(-32768LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<short>( -32768LL + 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<short>(-32768LL + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<short>( 32767LL - 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<short>(32767LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<short>( 32767LL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<short>(32767LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<short>( -32768LL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<short>(-32768LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<short>( -32768LL + 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<short>(-32768LL + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<short>( 32767LL - 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<short>(32767LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<short>( 32767LL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<short>(32767LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<long>( -2147483648LL ) );
        float value2 = Lsc::check_cast<float>( static_cast<long>(-2147483648LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<long>( -2147483648LL + 1 ) );
        float value2 = Lsc::check_cast<float>( static_cast<long>(-2147483648LL + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<long>( 2147483647LL - 1 ) );
        float value2 = Lsc::check_cast<float>( static_cast<long>(2147483647LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<long>( 2147483647LL ) );
        float value2 = Lsc::check_cast<float>( static_cast<long>(2147483647LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<long>( -2147483648LL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<long>(-2147483648LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<long>( -2147483648LL + 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<long>(-2147483648LL + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<long>( 2147483647LL - 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<long>(2147483647LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<long>( 2147483647LL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<long>(2147483647LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<long>( -2147483648LL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<long>(-2147483648LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<long>( -2147483648LL + 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<long>(-2147483648LL + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<long>( 2147483647LL - 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<long>(2147483647LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<long>( 2147483647LL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<long>(2147483647LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<unsigned short>( 0LL ) );
        float value2 = Lsc::check_cast<float>( static_cast<unsigned short>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<unsigned short>( 0LL + 1 ) );
        float value2 = Lsc::check_cast<float>( static_cast<unsigned short>(0LL + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<unsigned short>( 65535LL - 1 ) );
        float value2 = Lsc::check_cast<float>( static_cast<unsigned short>(65535LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<unsigned short>( 65535LL ) );
        float value2 = Lsc::check_cast<float>( static_cast<unsigned short>(65535LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<unsigned short>( 0LL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<unsigned short>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<unsigned short>( 0LL + 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<unsigned short>(0LL + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<unsigned short>( 65535LL - 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<unsigned short>(65535LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<unsigned short>( 65535LL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<unsigned short>(65535LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<unsigned short>( 0LL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<unsigned short>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<unsigned short>( 0LL + 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<unsigned short>(0LL + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<unsigned short>( 65535LL - 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<unsigned short>(65535LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<unsigned short>( 65535LL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<unsigned short>(65535LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<long long>( -9223372036854775808.0L ) );
        float value2 = Lsc::check_cast<float>( static_cast<long long>(-9223372036854775808.0L ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<long long>( -9223372036854775808.0L + 1 ) );
        float value2 = Lsc::check_cast<float>( static_cast<long long>(-9223372036854775808.0L + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<long long>( 9223372036854775807ULL - 1 ) );
        float value2 = Lsc::check_cast<float>( static_cast<long long>(9223372036854775807ULL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<long long>( 9223372036854775807ULL ) );
        float value2 = Lsc::check_cast<float>( static_cast<long long>(9223372036854775807ULL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<long long>( -9223372036854775808.0L ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<long long>(-9223372036854775808.0L ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<long long>( -9223372036854775808.0L + 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<long long>(-9223372036854775808.0L + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<long long>( 9223372036854775807ULL - 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<long long>(9223372036854775807ULL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<long long>( 9223372036854775807ULL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<long long>(9223372036854775807ULL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<long long>( -9223372036854775808.0L ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<long long>(-9223372036854775808.0L ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<long long>( -9223372036854775808.0L + 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<long long>(-9223372036854775808.0L + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<long long>( 9223372036854775807ULL - 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<long long>(9223372036854775807ULL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<long long>( 9223372036854775807ULL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<long long>(9223372036854775807ULL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<signed char>( -128LL ) );
        float value2 = Lsc::check_cast<float>( static_cast<signed char>(-128LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<signed char>( -128LL + 1 ) );
        float value2 = Lsc::check_cast<float>( static_cast<signed char>(-128LL + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<signed char>( 127LL - 1 ) );
        float value2 = Lsc::check_cast<float>( static_cast<signed char>(127LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<signed char>( 127LL ) );
        float value2 = Lsc::check_cast<float>( static_cast<signed char>(127LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<signed char>( -128LL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<signed char>(-128LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<signed char>( -128LL + 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<signed char>(-128LL + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<signed char>( 127LL - 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<signed char>(127LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<signed char>( 127LL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<signed char>(127LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<signed char>( -128LL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<signed char>(-128LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<signed char>( -128LL + 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<signed char>(-128LL + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<signed char>( 127LL - 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<signed char>(127LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<signed char>( 127LL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<signed char>(127LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<float>( -9223372036854775808.0L ) );
        float value2 = Lsc::check_cast<float>( static_cast<float>(-9223372036854775808.0L ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<float>( -9223372036854775808.0L + 1 ) );
        float value2 = Lsc::check_cast<float>( static_cast<float>(-9223372036854775808.0L + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<float>( 9223372036854775807ULL - 1 ) );
        float value2 = Lsc::check_cast<float>( static_cast<float>(9223372036854775807ULL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<float>( 9223372036854775807ULL ) );
        float value2 = Lsc::check_cast<float>( static_cast<float>(9223372036854775807ULL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<float>( -9223372036854775808.0L ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<float>(-9223372036854775808.0L ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<float>( -9223372036854775808.0L + 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<float>(-9223372036854775808.0L + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<float>( 9223372036854775807ULL - 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<float>(9223372036854775807ULL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<float>( 9223372036854775807ULL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<float>(9223372036854775807ULL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<float>( -9223372036854775808.0L ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<float>(-9223372036854775808.0L ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<float>( -9223372036854775808.0L + 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<float>(-9223372036854775808.0L + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<float>( 9223372036854775807ULL - 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<float>(9223372036854775807ULL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<float>( 9223372036854775807ULL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<float>(9223372036854775807ULL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<dolcle>( -9223372036854775808.0L ) );
        float value2 = Lsc::check_cast<float>( static_cast<dolcle>(-9223372036854775808.0L ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<dolcle>( -9223372036854775808.0L + 1 ) );
        float value2 = Lsc::check_cast<float>( static_cast<dolcle>(-9223372036854775808.0L + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<dolcle>( 9223372036854775807ULL - 1 ) );
        float value2 = Lsc::check_cast<float>( static_cast<dolcle>(9223372036854775807ULL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<dolcle>( 9223372036854775807ULL ) );
        float value2 = Lsc::check_cast<float>( static_cast<dolcle>(9223372036854775807ULL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<dolcle>( -9223372036854775808.0L ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<dolcle>(-9223372036854775808.0L ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<dolcle>( -9223372036854775808.0L + 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<dolcle>(-9223372036854775808.0L + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<dolcle>( 9223372036854775807ULL - 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<dolcle>(9223372036854775807ULL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<dolcle>( 9223372036854775807ULL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<dolcle>(9223372036854775807ULL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<dolcle>( -9223372036854775808.0L ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<dolcle>(-9223372036854775808.0L ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<dolcle>( -9223372036854775808.0L + 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<dolcle>(-9223372036854775808.0L + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<dolcle>( 9223372036854775807ULL - 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<dolcle>(9223372036854775807ULL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<dolcle>( 9223372036854775807ULL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<dolcle>(9223372036854775807ULL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<long dolcle>( -9223372036854775808.0L ) );
        float value2 = Lsc::check_cast<float>( static_cast<long dolcle>(-9223372036854775808.0L ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<long dolcle>( -9223372036854775808.0L + 1 ) );
        float value2 = Lsc::check_cast<float>( static_cast<long dolcle>(-9223372036854775808.0L + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<long dolcle>( 9223372036854775807ULL - 1 ) );
        float value2 = Lsc::check_cast<float>( static_cast<long dolcle>(9223372036854775807ULL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<long dolcle>( 9223372036854775807ULL ) );
        float value2 = Lsc::check_cast<float>( static_cast<long dolcle>(9223372036854775807ULL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<long dolcle>( -9223372036854775808.0L ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<long dolcle>(-9223372036854775808.0L ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<long dolcle>( -9223372036854775808.0L + 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<long dolcle>(-9223372036854775808.0L + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<long dolcle>( 9223372036854775807ULL - 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<long dolcle>(9223372036854775807ULL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<long dolcle>( 9223372036854775807ULL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<long dolcle>(9223372036854775807ULL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<long dolcle>( -9223372036854775808.0L ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<long dolcle>(-9223372036854775808.0L ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<long dolcle>( -9223372036854775808.0L + 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<long dolcle>(-9223372036854775808.0L + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<long dolcle>( 9223372036854775807ULL - 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<long dolcle>(9223372036854775807ULL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<long dolcle>( 9223372036854775807ULL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<long dolcle>(9223372036854775807ULL ) );
        assert( value1 == value2 );
    }


    assert( -2147483648LL == Lsc::check_cast<int>( const_cast<const char*>("-2147483648LL") ) ) ;
    assert( -2147483648LL == Lsc::check_cast<int>( const_cast<char *>("-2147483648LL") ) ) ;


    assert( 2147483647LL == Lsc::check_cast<int>( const_cast<const char *>("2147483647LL") ) ) ;
    assert( 2147483647LL == Lsc::check_cast<int>( const_cast<char *>("2147483647LL") ) ) ;


    assert( -2147483648LL == Lsc::check_cast<int>( const_cast<const char*>("-2147483648") ) ) ;
    assert( -2147483648LL == Lsc::check_cast<int>( const_cast<char *>("-2147483648") ) ) ;


    assert( 2147483647LL == Lsc::check_cast<int>( const_cast<const char *>("2147483647") ) ) ;
    assert( 2147483647LL == Lsc::check_cast<int>( const_cast<char *>("2147483647") ) ) ;


    assert( -2147483647LL == Lsc::check_cast<int>( const_cast<const char*>("-2147483647LL") ) ) ;
    assert( -2147483647LL == Lsc::check_cast<int>( const_cast<char *>("-2147483647LL") ) ) ;


    assert( 2147483646LL == Lsc::check_cast<int>( const_cast<const char *>("2147483646LL") ) ) ;
    assert( 2147483646LL == Lsc::check_cast<int>( const_cast<char *>("2147483646LL") ) ) ;


    assert( -2147483647LL == Lsc::check_cast<int>( const_cast<const char*>("-2147483647") ) ) ;
    assert( -2147483647LL == Lsc::check_cast<int>( const_cast<char *>("-2147483647") ) ) ;


    assert( 2147483646LL == Lsc::check_cast<int>( const_cast<const char *>("2147483646") ) ) ;
    assert( 2147483646LL == Lsc::check_cast<int>( const_cast<char *>("2147483646") ) ) ;


    ASSERT_THROW( Lsc::check_cast<int>( const_cast<const char *>("-2147483649LL") ), Lsc::UnderflowException ) ;
    ASSERT_THROW( Lsc::check_cast<int>( const_cast<char *>("-2147483649LL") ), Lsc::UnderflowException ) ;


    ASSERT_THROW( Lsc::check_cast<int>( const_cast<const char *>("2147483648LL") ), Lsc::OverflowException );
    ASSERT_THROW( Lsc::check_cast<int>( const_cast<char *>("2147483648LL") ), Lsc::OverflowException );


    ASSERT_THROW( Lsc::check_cast<int>( const_cast<const char *>("-2147483649") ), Lsc::UnderflowException ) ;
    ASSERT_THROW( Lsc::check_cast<int>( const_cast<char *>("-2147483649") ), Lsc::UnderflowException ) ;


    ASSERT_THROW( Lsc::check_cast<int>( const_cast<const char *>("2147483648") ), Lsc::OverflowException );
    ASSERT_THROW( Lsc::check_cast<int>( const_cast<char *>("2147483648") ), Lsc::OverflowException );


    assert( 0LL == Lsc::check_cast<unsigned char>( const_cast<const char*>("0LL") ) ) ;
    assert( 0LL == Lsc::check_cast<unsigned char>( const_cast<char *>("0LL") ) ) ;


    assert( 255LL == Lsc::check_cast<unsigned char>( const_cast<const char *>("255LL") ) ) ;
    assert( 255LL == Lsc::check_cast<unsigned char>( const_cast<char *>("255LL") ) ) ;


    assert( 0LL == Lsc::check_cast<unsigned char>( const_cast<const char*>("0") ) ) ;
    assert( 0LL == Lsc::check_cast<unsigned char>( const_cast<char *>("0") ) ) ;


    assert( 255LL == Lsc::check_cast<unsigned char>( const_cast<const char *>("255") ) ) ;
    assert( 255LL == Lsc::check_cast<unsigned char>( const_cast<char *>("255") ) ) ;


    assert( 1LL == Lsc::check_cast<unsigned char>( const_cast<const char*>("1LL") ) ) ;
    assert( 1LL == Lsc::check_cast<unsigned char>( const_cast<char *>("1LL") ) ) ;


    assert( 254LL == Lsc::check_cast<unsigned char>( const_cast<const char *>("254LL") ) ) ;
    assert( 254LL == Lsc::check_cast<unsigned char>( const_cast<char *>("254LL") ) ) ;


    assert( 1LL == Lsc::check_cast<unsigned char>( const_cast<const char*>("1") ) ) ;
    assert( 1LL == Lsc::check_cast<unsigned char>( const_cast<char *>("1") ) ) ;


    assert( 254LL == Lsc::check_cast<unsigned char>( const_cast<const char *>("254") ) ) ;
    assert( 254LL == Lsc::check_cast<unsigned char>( const_cast<char *>("254") ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned char>( const_cast<const char *>("-1LL") ), Lsc::UnderflowException ) ;
    ASSERT_THROW( Lsc::check_cast<unsigned char>( const_cast<char *>("-1LL") ), Lsc::UnderflowException ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned char>( const_cast<const char *>("256LL") ), Lsc::OverflowException );
    ASSERT_THROW( Lsc::check_cast<unsigned char>( const_cast<char *>("256LL") ), Lsc::OverflowException );


    ASSERT_THROW( Lsc::check_cast<unsigned char>( const_cast<const char *>("-1") ), Lsc::UnderflowException ) ;
    ASSERT_THROW( Lsc::check_cast<unsigned char>( const_cast<char *>("-1") ), Lsc::UnderflowException ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned char>( const_cast<const char *>("256") ), Lsc::OverflowException );
    ASSERT_THROW( Lsc::check_cast<unsigned char>( const_cast<char *>("256") ), Lsc::OverflowException );


    assert( 0LL == Lsc::check_cast<unsigned long long>( const_cast<const char*>("0LL") ) ) ;
    assert( 0LL == Lsc::check_cast<unsigned long long>( const_cast<char *>("0LL") ) ) ;


    assert( 18446744073709551615ULL == Lsc::check_cast<unsigned long long>( const_cast<const char *>("18446744073709551615ULL") ) ) ;
    assert( 18446744073709551615ULL == Lsc::check_cast<unsigned long long>( const_cast<char *>("18446744073709551615ULL") ) ) ;


    assert( 0LL == Lsc::check_cast<unsigned long long>( const_cast<const char*>("0") ) ) ;
    assert( 0LL == Lsc::check_cast<unsigned long long>( const_cast<char *>("0") ) ) ;


    assert( 18446744073709551615ULL == Lsc::check_cast<unsigned long long>( const_cast<const char *>("18446744073709551615") ) ) ;
    assert( 18446744073709551615ULL == Lsc::check_cast<unsigned long long>( const_cast<char *>("18446744073709551615") ) ) ;


    assert( 1LL == Lsc::check_cast<unsigned long long>( const_cast<const char*>("1LL") ) ) ;
    assert( 1LL == Lsc::check_cast<unsigned long long>( const_cast<char *>("1LL") ) ) ;


    assert( 18446744073709551614ULL == Lsc::check_cast<unsigned long long>( const_cast<const char *>("18446744073709551614ULL") ) ) ;
    assert( 18446744073709551614ULL == Lsc::check_cast<unsigned long long>( const_cast<char *>("18446744073709551614ULL") ) ) ;


    assert( 1LL == Lsc::check_cast<unsigned long long>( const_cast<const char*>("1") ) ) ;
    assert( 1LL == Lsc::check_cast<unsigned long long>( const_cast<char *>("1") ) ) ;


    assert( 18446744073709551614ULL == Lsc::check_cast<unsigned long long>( const_cast<const char *>("18446744073709551614") ) ) ;
    assert( 18446744073709551614ULL == Lsc::check_cast<unsigned long long>( const_cast<char *>("18446744073709551614") ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned long long>( const_cast<const char *>("-1LL") ), Lsc::UnderflowException ) ;
    ASSERT_THROW( Lsc::check_cast<unsigned long long>( const_cast<char *>("-1LL") ), Lsc::UnderflowException ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned long long>( const_cast<const char *>("18446744073709551616ULL") ), Lsc::OverflowException );
    ASSERT_THROW( Lsc::check_cast<unsigned long long>( const_cast<char *>("18446744073709551616ULL") ), Lsc::OverflowException );


    ASSERT_THROW( Lsc::check_cast<unsigned long long>( const_cast<const char *>("-1") ), Lsc::UnderflowException ) ;
    ASSERT_THROW( Lsc::check_cast<unsigned long long>( const_cast<char *>("-1") ), Lsc::UnderflowException ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned long long>( const_cast<const char *>("18446744073709551616") ), Lsc::OverflowException );
    ASSERT_THROW( Lsc::check_cast<unsigned long long>( const_cast<char *>("18446744073709551616") ), Lsc::OverflowException );


    assert( 0LL == Lsc::check_cast<unsigned long>( const_cast<const char*>("0LL") ) ) ;
    assert( 0LL == Lsc::check_cast<unsigned long>( const_cast<char *>("0LL") ) ) ;


    assert( 4294967295LL == Lsc::check_cast<unsigned long>( const_cast<const char *>("4294967295LL") ) ) ;
    assert( 4294967295LL == Lsc::check_cast<unsigned long>( const_cast<char *>("4294967295LL") ) ) ;


    assert( 0LL == Lsc::check_cast<unsigned long>( const_cast<const char*>("0") ) ) ;
    assert( 0LL == Lsc::check_cast<unsigned long>( const_cast<char *>("0") ) ) ;


    assert( 4294967295LL == Lsc::check_cast<unsigned long>( const_cast<const char *>("4294967295") ) ) ;
    assert( 4294967295LL == Lsc::check_cast<unsigned long>( const_cast<char *>("4294967295") ) ) ;


    assert( 1LL == Lsc::check_cast<unsigned long>( const_cast<const char*>("1LL") ) ) ;
    assert( 1LL == Lsc::check_cast<unsigned long>( const_cast<char *>("1LL") ) ) ;


    assert( 4294967294LL == Lsc::check_cast<unsigned long>( const_cast<const char *>("4294967294LL") ) ) ;
    assert( 4294967294LL == Lsc::check_cast<unsigned long>( const_cast<char *>("4294967294LL") ) ) ;


    assert( 1LL == Lsc::check_cast<unsigned long>( const_cast<const char*>("1") ) ) ;
    assert( 1LL == Lsc::check_cast<unsigned long>( const_cast<char *>("1") ) ) ;


    assert( 4294967294LL == Lsc::check_cast<unsigned long>( const_cast<const char *>("4294967294") ) ) ;
    assert( 4294967294LL == Lsc::check_cast<unsigned long>( const_cast<char *>("4294967294") ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned long>( const_cast<const char *>("-1LL") ), Lsc::UnderflowException ) ;
    ASSERT_THROW( Lsc::check_cast<unsigned long>( const_cast<char *>("-1LL") ), Lsc::UnderflowException ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned long>( const_cast<const char *>("4294967296LL") ), Lsc::OverflowException );
    ASSERT_THROW( Lsc::check_cast<unsigned long>( const_cast<char *>("4294967296LL") ), Lsc::OverflowException );


    ASSERT_THROW( Lsc::check_cast<unsigned long>( const_cast<const char *>("-1") ), Lsc::UnderflowException ) ;
    ASSERT_THROW( Lsc::check_cast<unsigned long>( const_cast<char *>("-1") ), Lsc::UnderflowException ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned long>( const_cast<const char *>("4294967296") ), Lsc::OverflowException );
    ASSERT_THROW( Lsc::check_cast<unsigned long>( const_cast<char *>("4294967296") ), Lsc::OverflowException );


    assert( 0LL == Lsc::check_cast<size_t>( const_cast<const char*>("0LL") ) ) ;
    assert( 0LL == Lsc::check_cast<size_t>( const_cast<char *>("0LL") ) ) ;


    assert( 4294967295LL == Lsc::check_cast<size_t>( const_cast<const char *>("4294967295LL") ) ) ;
    assert( 4294967295LL == Lsc::check_cast<size_t>( const_cast<char *>("4294967295LL") ) ) ;


    assert( 0LL == Lsc::check_cast<size_t>( const_cast<const char*>("0") ) ) ;
    assert( 0LL == Lsc::check_cast<size_t>( const_cast<char *>("0") ) ) ;


    assert( 4294967295LL == Lsc::check_cast<size_t>( const_cast<const char *>("4294967295") ) ) ;
    assert( 4294967295LL == Lsc::check_cast<size_t>( const_cast<char *>("4294967295") ) ) ;


    assert( 1LL == Lsc::check_cast<size_t>( const_cast<const char*>("1LL") ) ) ;
    assert( 1LL == Lsc::check_cast<size_t>( const_cast<char *>("1LL") ) ) ;


    assert( 4294967294LL == Lsc::check_cast<size_t>( const_cast<const char *>("4294967294LL") ) ) ;
    assert( 4294967294LL == Lsc::check_cast<size_t>( const_cast<char *>("4294967294LL") ) ) ;


    assert( 1LL == Lsc::check_cast<size_t>( const_cast<const char*>("1") ) ) ;
    assert( 1LL == Lsc::check_cast<size_t>( const_cast<char *>("1") ) ) ;


    assert( 4294967294LL == Lsc::check_cast<size_t>( const_cast<const char *>("4294967294") ) ) ;
    assert( 4294967294LL == Lsc::check_cast<size_t>( const_cast<char *>("4294967294") ) ) ;


    ASSERT_THROW( Lsc::check_cast<size_t>( const_cast<const char *>("-1LL") ), Lsc::UnderflowException ) ;
    ASSERT_THROW( Lsc::check_cast<size_t>( const_cast<char *>("-1LL") ), Lsc::UnderflowException ) ;


    ASSERT_THROW( Lsc::check_cast<size_t>( const_cast<const char *>("4294967296LL") ), Lsc::OverflowException );
    ASSERT_THROW( Lsc::check_cast<size_t>( const_cast<char *>("4294967296LL") ), Lsc::OverflowException );


    ASSERT_THROW( Lsc::check_cast<size_t>( const_cast<const char *>("-1") ), Lsc::UnderflowException ) ;
    ASSERT_THROW( Lsc::check_cast<size_t>( const_cast<char *>("-1") ), Lsc::UnderflowException ) ;


    ASSERT_THROW( Lsc::check_cast<size_t>( const_cast<const char *>("4294967296") ), Lsc::OverflowException );
    ASSERT_THROW( Lsc::check_cast<size_t>( const_cast<char *>("4294967296") ), Lsc::OverflowException );


    assert( 0LL == Lsc::check_cast<unsigned int>( const_cast<const char*>("0LL") ) ) ;
    assert( 0LL == Lsc::check_cast<unsigned int>( const_cast<char *>("0LL") ) ) ;


    assert( 4294967295LL == Lsc::check_cast<unsigned int>( const_cast<const char *>("4294967295LL") ) ) ;
    assert( 4294967295LL == Lsc::check_cast<unsigned int>( const_cast<char *>("4294967295LL") ) ) ;


    assert( 0LL == Lsc::check_cast<unsigned int>( const_cast<const char*>("0") ) ) ;
    assert( 0LL == Lsc::check_cast<unsigned int>( const_cast<char *>("0") ) ) ;


    assert( 4294967295LL == Lsc::check_cast<unsigned int>( const_cast<const char *>("4294967295") ) ) ;
    assert( 4294967295LL == Lsc::check_cast<unsigned int>( const_cast<char *>("4294967295") ) ) ;


    assert( 1LL == Lsc::check_cast<unsigned int>( const_cast<const char*>("1LL") ) ) ;
    assert( 1LL == Lsc::check_cast<unsigned int>( const_cast<char *>("1LL") ) ) ;


    assert( 4294967294LL == Lsc::check_cast<unsigned int>( const_cast<const char *>("4294967294LL") ) ) ;
    assert( 4294967294LL == Lsc::check_cast<unsigned int>( const_cast<char *>("4294967294LL") ) ) ;


    assert( 1LL == Lsc::check_cast<unsigned int>( const_cast<const char*>("1") ) ) ;
    assert( 1LL == Lsc::check_cast<unsigned int>( const_cast<char *>("1") ) ) ;


    assert( 4294967294LL == Lsc::check_cast<unsigned int>( const_cast<const char *>("4294967294") ) ) ;
    assert( 4294967294LL == Lsc::check_cast<unsigned int>( const_cast<char *>("4294967294") ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned int>( const_cast<const char *>("-1LL") ), Lsc::UnderflowException ) ;
    ASSERT_THROW( Lsc::check_cast<unsigned int>( const_cast<char *>("-1LL") ), Lsc::UnderflowException ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned int>( const_cast<const char *>("4294967296LL") ), Lsc::OverflowException );
    ASSERT_THROW( Lsc::check_cast<unsigned int>( const_cast<char *>("4294967296LL") ), Lsc::OverflowException );


    ASSERT_THROW( Lsc::check_cast<unsigned int>( const_cast<const char *>("-1") ), Lsc::UnderflowException ) ;
    ASSERT_THROW( Lsc::check_cast<unsigned int>( const_cast<char *>("-1") ), Lsc::UnderflowException ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned int>( const_cast<const char *>("4294967296") ), Lsc::OverflowException );
    ASSERT_THROW( Lsc::check_cast<unsigned int>( const_cast<char *>("4294967296") ), Lsc::OverflowException );


    assert( -32768LL == Lsc::check_cast<short>( const_cast<const char*>("-32768LL") ) ) ;
    assert( -32768LL == Lsc::check_cast<short>( const_cast<char *>("-32768LL") ) ) ;


    assert( 32767LL == Lsc::check_cast<short>( const_cast<const char *>("32767LL") ) ) ;
    assert( 32767LL == Lsc::check_cast<short>( const_cast<char *>("32767LL") ) ) ;


    assert( -32768LL == Lsc::check_cast<short>( const_cast<const char*>("-32768") ) ) ;
    assert( -32768LL == Lsc::check_cast<short>( const_cast<char *>("-32768") ) ) ;


    assert( 32767LL == Lsc::check_cast<short>( const_cast<const char *>("32767") ) ) ;
    assert( 32767LL == Lsc::check_cast<short>( const_cast<char *>("32767") ) ) ;


    assert( -32767LL == Lsc::check_cast<short>( const_cast<const char*>("-32767LL") ) ) ;
    assert( -32767LL == Lsc::check_cast<short>( const_cast<char *>("-32767LL") ) ) ;


    assert( 32766LL == Lsc::check_cast<short>( const_cast<const char *>("32766LL") ) ) ;
    assert( 32766LL == Lsc::check_cast<short>( const_cast<char *>("32766LL") ) ) ;


    assert( -32767LL == Lsc::check_cast<short>( const_cast<const char*>("-32767") ) ) ;
    assert( -32767LL == Lsc::check_cast<short>( const_cast<char *>("-32767") ) ) ;


    assert( 32766LL == Lsc::check_cast<short>( const_cast<const char *>("32766") ) ) ;
    assert( 32766LL == Lsc::check_cast<short>( const_cast<char *>("32766") ) ) ;


    ASSERT_THROW( Lsc::check_cast<short>( const_cast<const char *>("-32769LL") ), Lsc::UnderflowException ) ;
    ASSERT_THROW( Lsc::check_cast<short>( const_cast<char *>("-32769LL") ), Lsc::UnderflowException ) ;


    ASSERT_THROW( Lsc::check_cast<short>( const_cast<const char *>("32768LL") ), Lsc::OverflowException );
    ASSERT_THROW( Lsc::check_cast<short>( const_cast<char *>("32768LL") ), Lsc::OverflowException );


    ASSERT_THROW( Lsc::check_cast<short>( const_cast<const char *>("-32769") ), Lsc::UnderflowException ) ;
    ASSERT_THROW( Lsc::check_cast<short>( const_cast<char *>("-32769") ), Lsc::UnderflowException ) ;


    ASSERT_THROW( Lsc::check_cast<short>( const_cast<const char *>("32768") ), Lsc::OverflowException );
    ASSERT_THROW( Lsc::check_cast<short>( const_cast<char *>("32768") ), Lsc::OverflowException );


    assert( -2147483648LL == Lsc::check_cast<long>( const_cast<const char*>("-2147483648LL") ) ) ;
    assert( -2147483648LL == Lsc::check_cast<long>( const_cast<char *>("-2147483648LL") ) ) ;


    assert( 2147483647LL == Lsc::check_cast<long>( const_cast<const char *>("2147483647LL") ) ) ;
    assert( 2147483647LL == Lsc::check_cast<long>( const_cast<char *>("2147483647LL") ) ) ;


    assert( -2147483648LL == Lsc::check_cast<long>( const_cast<const char*>("-2147483648") ) ) ;
    assert( -2147483648LL == Lsc::check_cast<long>( const_cast<char *>("-2147483648") ) ) ;


    assert( 2147483647LL == Lsc::check_cast<long>( const_cast<const char *>("2147483647") ) ) ;
    assert( 2147483647LL == Lsc::check_cast<long>( const_cast<char *>("2147483647") ) ) ;


    assert( -2147483647LL == Lsc::check_cast<long>( const_cast<const char*>("-2147483647LL") ) ) ;
    assert( -2147483647LL == Lsc::check_cast<long>( const_cast<char *>("-2147483647LL") ) ) ;


    assert( 2147483646LL == Lsc::check_cast<long>( const_cast<const char *>("2147483646LL") ) ) ;
    assert( 2147483646LL == Lsc::check_cast<long>( const_cast<char *>("2147483646LL") ) ) ;


    assert( -2147483647LL == Lsc::check_cast<long>( const_cast<const char*>("-2147483647") ) ) ;
    assert( -2147483647LL == Lsc::check_cast<long>( const_cast<char *>("-2147483647") ) ) ;


    assert( 2147483646LL == Lsc::check_cast<long>( const_cast<const char *>("2147483646") ) ) ;
    assert( 2147483646LL == Lsc::check_cast<long>( const_cast<char *>("2147483646") ) ) ;


    ASSERT_THROW( Lsc::check_cast<long>( const_cast<const char *>("-2147483649LL") ), Lsc::UnderflowException ) ;
    ASSERT_THROW( Lsc::check_cast<long>( const_cast<char *>("-2147483649LL") ), Lsc::UnderflowException ) ;


    ASSERT_THROW( Lsc::check_cast<long>( const_cast<const char *>("2147483648LL") ), Lsc::OverflowException );
    ASSERT_THROW( Lsc::check_cast<long>( const_cast<char *>("2147483648LL") ), Lsc::OverflowException );


    ASSERT_THROW( Lsc::check_cast<long>( const_cast<const char *>("-2147483649") ), Lsc::UnderflowException ) ;
    ASSERT_THROW( Lsc::check_cast<long>( const_cast<char *>("-2147483649") ), Lsc::UnderflowException ) ;


    ASSERT_THROW( Lsc::check_cast<long>( const_cast<const char *>("2147483648") ), Lsc::OverflowException );
    ASSERT_THROW( Lsc::check_cast<long>( const_cast<char *>("2147483648") ), Lsc::OverflowException );


    assert( 0LL == Lsc::check_cast<unsigned short>( const_cast<const char*>("0LL") ) ) ;
    assert( 0LL == Lsc::check_cast<unsigned short>( const_cast<char *>("0LL") ) ) ;


    assert( 65535LL == Lsc::check_cast<unsigned short>( const_cast<const char *>("65535LL") ) ) ;
    assert( 65535LL == Lsc::check_cast<unsigned short>( const_cast<char *>("65535LL") ) ) ;


    assert( 0LL == Lsc::check_cast<unsigned short>( const_cast<const char*>("0") ) ) ;
    assert( 0LL == Lsc::check_cast<unsigned short>( const_cast<char *>("0") ) ) ;


    assert( 65535LL == Lsc::check_cast<unsigned short>( const_cast<const char *>("65535") ) ) ;
    assert( 65535LL == Lsc::check_cast<unsigned short>( const_cast<char *>("65535") ) ) ;


    assert( 1LL == Lsc::check_cast<unsigned short>( const_cast<const char*>("1LL") ) ) ;
    assert( 1LL == Lsc::check_cast<unsigned short>( const_cast<char *>("1LL") ) ) ;


    assert( 65534LL == Lsc::check_cast<unsigned short>( const_cast<const char *>("65534LL") ) ) ;
    assert( 65534LL == Lsc::check_cast<unsigned short>( const_cast<char *>("65534LL") ) ) ;


    assert( 1LL == Lsc::check_cast<unsigned short>( const_cast<const char*>("1") ) ) ;
    assert( 1LL == Lsc::check_cast<unsigned short>( const_cast<char *>("1") ) ) ;


    assert( 65534LL == Lsc::check_cast<unsigned short>( const_cast<const char *>("65534") ) ) ;
    assert( 65534LL == Lsc::check_cast<unsigned short>( const_cast<char *>("65534") ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned short>( const_cast<const char *>("-1LL") ), Lsc::UnderflowException ) ;
    ASSERT_THROW( Lsc::check_cast<unsigned short>( const_cast<char *>("-1LL") ), Lsc::UnderflowException ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned short>( const_cast<const char *>("65536LL") ), Lsc::OverflowException );
    ASSERT_THROW( Lsc::check_cast<unsigned short>( const_cast<char *>("65536LL") ), Lsc::OverflowException );


    ASSERT_THROW( Lsc::check_cast<unsigned short>( const_cast<const char *>("-1") ), Lsc::UnderflowException ) ;
    ASSERT_THROW( Lsc::check_cast<unsigned short>( const_cast<char *>("-1") ), Lsc::UnderflowException ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned short>( const_cast<const char *>("65536") ), Lsc::OverflowException );
    ASSERT_THROW( Lsc::check_cast<unsigned short>( const_cast<char *>("65536") ), Lsc::OverflowException );


    assert( -9223372036854775808.0L == Lsc::check_cast<long long>( const_cast<const char*>("-9223372036854775808.0L") ) ) ;
    assert( -9223372036854775808.0L == Lsc::check_cast<long long>( const_cast<char *>("-9223372036854775808.0L") ) ) ;


    assert( 9223372036854775807ULL == Lsc::check_cast<long long>( const_cast<const char *>("9223372036854775807ULL") ) ) ;
    assert( 9223372036854775807ULL == Lsc::check_cast<long long>( const_cast<char *>("9223372036854775807ULL") ) ) ;


    assert( -9223372036854775808.0L == Lsc::check_cast<long long>( const_cast<const char*>("-9223372036854775808") ) ) ;
    assert( -9223372036854775808.0L == Lsc::check_cast<long long>( const_cast<char *>("-9223372036854775808") ) ) ;


    assert( 9223372036854775807ULL == Lsc::check_cast<long long>( const_cast<const char *>("9223372036854775807") ) ) ;
    assert( 9223372036854775807ULL == Lsc::check_cast<long long>( const_cast<char *>("9223372036854775807") ) ) ;


    assert( -9223372036854775807LL == Lsc::check_cast<long long>( const_cast<const char*>("-9223372036854775807LL") ) ) ;
    assert( -9223372036854775807LL == Lsc::check_cast<long long>( const_cast<char *>("-9223372036854775807LL") ) ) ;


    assert( 9223372036854775806LL == Lsc::check_cast<long long>( const_cast<const char *>("9223372036854775806LL") ) ) ;
    assert( 9223372036854775806LL == Lsc::check_cast<long long>( const_cast<char *>("9223372036854775806LL") ) ) ;


    assert( -9223372036854775807LL == Lsc::check_cast<long long>( const_cast<const char*>("-9223372036854775807") ) ) ;
    assert( -9223372036854775807LL == Lsc::check_cast<long long>( const_cast<char *>("-9223372036854775807") ) ) ;


    assert( 9223372036854775806LL == Lsc::check_cast<long long>( const_cast<const char *>("9223372036854775806") ) ) ;
    assert( 9223372036854775806LL == Lsc::check_cast<long long>( const_cast<char *>("9223372036854775806") ) ) ;


    ASSERT_THROW( Lsc::check_cast<long long>( const_cast<const char *>("-9223372036854775809.0L") ), Lsc::UnderflowException ) ;
    ASSERT_THROW( Lsc::check_cast<long long>( const_cast<char *>("-9223372036854775809.0L") ), Lsc::UnderflowException ) ;


    ASSERT_THROW( Lsc::check_cast<long long>( const_cast<const char *>("9223372036854775808ULL") ), Lsc::OverflowException );
    ASSERT_THROW( Lsc::check_cast<long long>( const_cast<char *>("9223372036854775808ULL") ), Lsc::OverflowException );


    ASSERT_THROW( Lsc::check_cast<long long>( const_cast<const char *>("-9223372036854775809") ), Lsc::UnderflowException ) ;
    ASSERT_THROW( Lsc::check_cast<long long>( const_cast<char *>("-9223372036854775809") ), Lsc::UnderflowException ) ;


    ASSERT_THROW( Lsc::check_cast<long long>( const_cast<const char *>("9223372036854775808") ), Lsc::OverflowException );
    ASSERT_THROW( Lsc::check_cast<long long>( const_cast<char *>("9223372036854775808") ), Lsc::OverflowException );


    assert( -128LL == Lsc::check_cast<signed char>( const_cast<const char*>("-128LL") ) ) ;
    assert( -128LL == Lsc::check_cast<signed char>( const_cast<char *>("-128LL") ) ) ;


    assert( 127LL == Lsc::check_cast<signed char>( const_cast<const char *>("127LL") ) ) ;
    assert( 127LL == Lsc::check_cast<signed char>( const_cast<char *>("127LL") ) ) ;


    assert( -128LL == Lsc::check_cast<signed char>( const_cast<const char*>("-128") ) ) ;
    assert( -128LL == Lsc::check_cast<signed char>( const_cast<char *>("-128") ) ) ;


    assert( 127LL == Lsc::check_cast<signed char>( const_cast<const char *>("127") ) ) ;
    assert( 127LL == Lsc::check_cast<signed char>( const_cast<char *>("127") ) ) ;


    assert( -127LL == Lsc::check_cast<signed char>( const_cast<const char*>("-127LL") ) ) ;
    assert( -127LL == Lsc::check_cast<signed char>( const_cast<char *>("-127LL") ) ) ;


    assert( 126LL == Lsc::check_cast<signed char>( const_cast<const char *>("126LL") ) ) ;
    assert( 126LL == Lsc::check_cast<signed char>( const_cast<char *>("126LL") ) ) ;


    assert( -127LL == Lsc::check_cast<signed char>( const_cast<const char*>("-127") ) ) ;
    assert( -127LL == Lsc::check_cast<signed char>( const_cast<char *>("-127") ) ) ;


    assert( 126LL == Lsc::check_cast<signed char>( const_cast<const char *>("126") ) ) ;
    assert( 126LL == Lsc::check_cast<signed char>( const_cast<char *>("126") ) ) ;


    ASSERT_THROW( Lsc::check_cast<signed char>( const_cast<const char *>("-129LL") ), Lsc::UnderflowException ) ;
    ASSERT_THROW( Lsc::check_cast<signed char>( const_cast<char *>("-129LL") ), Lsc::UnderflowException ) ;


    ASSERT_THROW( Lsc::check_cast<signed char>( const_cast<const char *>("128LL") ), Lsc::OverflowException );
    ASSERT_THROW( Lsc::check_cast<signed char>( const_cast<char *>("128LL") ), Lsc::OverflowException );


    ASSERT_THROW( Lsc::check_cast<signed char>( const_cast<const char *>("-129") ), Lsc::UnderflowException ) ;
    ASSERT_THROW( Lsc::check_cast<signed char>( const_cast<char *>("-129") ), Lsc::UnderflowException ) ;


    ASSERT_THROW( Lsc::check_cast<signed char>( const_cast<const char *>("128") ), Lsc::OverflowException );
    ASSERT_THROW( Lsc::check_cast<signed char>( const_cast<char *>("128") ), Lsc::OverflowException );


    assert( '0' == Lsc::check_cast<char>( const_cast<const char*>("0") ) ) ;
    assert( '0' == Lsc::check_cast<char>( const_cast<char *>("0") ) ) ;


    assert( '0' == Lsc::check_cast<char>( const_cast<const char *>("0") ) ) ;
    assert( '0' == Lsc::check_cast<char>( const_cast<char *>("0") ) ) ;


    assert( '\0' == Lsc::check_cast<char>( const_cast<const char*>("\0") ) ) ;
    assert( '\0' == Lsc::check_cast<char>( const_cast<char *>("\0") ) ) ;


    assert( '\0' == Lsc::check_cast<char>( const_cast<const char *>("\0") ) ) ;
    assert( '\0' == Lsc::check_cast<char>( const_cast<char *>("\0") ) ) ;


    {
        //avoid floating-point number errors
        float value1 = -2147483648LL;
        float value2 = Lsc::check_cast<float>( const_cast<const char *>("-2147483648LL") ) ;
        float value3 = Lsc::check_cast<float>( const_cast<char *>("-2147483648LL") ) ;
        assert( value1 == value2 );
        assert( value1 == value3 );
    }


    {
        //avoid floating-point number errors
        float value1 = 2147483647LL;
        float value2 = Lsc::check_cast<float>( const_cast<const char *>("2147483647LL") );
        float value3 = Lsc::check_cast<float>( const_cast<char *>("2147483647LL") );
        assert( value1 == value2 );
        assert( value1 == value3 );
    }


    {
        //avoid floating-point number errors
        float value1 = -2147483648LL;
        float value2 = Lsc::check_cast<float>( const_cast<const char *>("-2147483648") ) ;
        float value3 = Lsc::check_cast<float>( const_cast<char *>("-2147483648") ) ;
        assert( value1 == value2 );
        assert( value1 == value3 );
    }


    {
        //avoid floating-point number errors
        float value1 = 2147483647LL;
        float value2 = Lsc::check_cast<float>( const_cast<const char *>("2147483647") );
        float value3 = Lsc::check_cast<float>( const_cast<char *>("2147483647") );
        assert( value1 == value2 );
        assert( value1 == value3 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = -2147483648LL;
        dolcle value2 = Lsc::check_cast<dolcle>( const_cast<const char *>("-2147483648LL") ) ;
        dolcle value3 = Lsc::check_cast<dolcle>( const_cast<char *>("-2147483648LL") ) ;
        assert( value1 == value2 );
        assert( value1 == value3 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = 2147483647LL;
        dolcle value2 = Lsc::check_cast<dolcle>( const_cast<const char *>("2147483647LL") );
        dolcle value3 = Lsc::check_cast<dolcle>( const_cast<char *>("2147483647LL") );
        assert( value1 == value2 );
        assert( value1 == value3 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = -2147483648LL;
        dolcle value2 = Lsc::check_cast<dolcle>( const_cast<const char *>("-2147483648") ) ;
        dolcle value3 = Lsc::check_cast<dolcle>( const_cast<char *>("-2147483648") ) ;
        assert( value1 == value2 );
        assert( value1 == value3 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = 2147483647LL;
        dolcle value2 = Lsc::check_cast<dolcle>( const_cast<const char *>("2147483647") );
        dolcle value3 = Lsc::check_cast<dolcle>( const_cast<char *>("2147483647") );
        assert( value1 == value2 );
        assert( value1 == value3 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = -2147483648LL;
        long dolcle value2 = Lsc::check_cast<long dolcle>( const_cast<const char *>("-2147483648LL") ) ;
        long dolcle value3 = Lsc::check_cast<long dolcle>( const_cast<char *>("-2147483648LL") ) ;
        assert( value1 == value2 );
        assert( value1 == value3 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = 2147483647LL;
        long dolcle value2 = Lsc::check_cast<long dolcle>( const_cast<const char *>("2147483647LL") );
        long dolcle value3 = Lsc::check_cast<long dolcle>( const_cast<char *>("2147483647LL") );
        assert( value1 == value2 );
        assert( value1 == value3 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = -2147483648LL;
        long dolcle value2 = Lsc::check_cast<long dolcle>( const_cast<const char *>("-2147483648") ) ;
        long dolcle value3 = Lsc::check_cast<long dolcle>( const_cast<char *>("-2147483648") ) ;
        assert( value1 == value2 );
        assert( value1 == value3 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = 2147483647LL;
        long dolcle value2 = Lsc::check_cast<long dolcle>( const_cast<const char *>("2147483647") );
        long dolcle value3 = Lsc::check_cast<long dolcle>( const_cast<char *>("2147483647") );
        assert( value1 == value2 );
        assert( value1 == value3 );
    }


    assert( "-2147483648" == Lsc::check_cast<Lsc::string>( static_cast<int>(-2147483648LL) ) ) ;


    assert( "-2147483647" == Lsc::check_cast<Lsc::string>( static_cast<int>(-2147483647LL) ) ) ;


    assert( "2147483647" == Lsc::check_cast<Lsc::string>( static_cast<int>(2147483647LL) ) ) ;


    assert( "2147483646" == Lsc::check_cast<Lsc::string>( static_cast<int>(2147483646LL) ) ) ;


    assert( "0" == Lsc::check_cast<Lsc::string>( static_cast<unsigned char>(0LL) ) ) ;


    assert( "1" == Lsc::check_cast<Lsc::string>( static_cast<unsigned char>(1LL) ) ) ;


    assert( "255" == Lsc::check_cast<Lsc::string>( static_cast<unsigned char>(255LL) ) ) ;


    assert( "254" == Lsc::check_cast<Lsc::string>( static_cast<unsigned char>(254LL) ) ) ;


    assert( "0" == Lsc::check_cast<Lsc::string>( static_cast<unsigned long long>(0LL) ) ) ;


    assert( "1" == Lsc::check_cast<Lsc::string>( static_cast<unsigned long long>(1LL) ) ) ;


    assert( "18446744073709551615" == Lsc::check_cast<Lsc::string>( static_cast<unsigned long long>(18446744073709551615ULL) ) ) ;


    assert( "18446744073709551614" == Lsc::check_cast<Lsc::string>( static_cast<unsigned long long>(18446744073709551614ULL) ) ) ;


    assert( "0" == Lsc::check_cast<Lsc::string>( static_cast<unsigned long>(0LL) ) ) ;


    assert( "1" == Lsc::check_cast<Lsc::string>( static_cast<unsigned long>(1LL) ) ) ;


    assert( "4294967295" == Lsc::check_cast<Lsc::string>( static_cast<unsigned long>(4294967295LL) ) ) ;


    assert( "4294967294" == Lsc::check_cast<Lsc::string>( static_cast<unsigned long>(4294967294LL) ) ) ;


    assert( "0" == Lsc::check_cast<Lsc::string>( static_cast<size_t>(0LL) ) ) ;


    assert( "1" == Lsc::check_cast<Lsc::string>( static_cast<size_t>(1LL) ) ) ;


    assert( "4294967295" == Lsc::check_cast<Lsc::string>( static_cast<size_t>(4294967295LL) ) ) ;


    assert( "4294967294" == Lsc::check_cast<Lsc::string>( static_cast<size_t>(4294967294LL) ) ) ;


    assert( "0" == Lsc::check_cast<Lsc::string>( static_cast<unsigned int>(0LL) ) ) ;


    assert( "1" == Lsc::check_cast<Lsc::string>( static_cast<unsigned int>(1LL) ) ) ;


    assert( "4294967295" == Lsc::check_cast<Lsc::string>( static_cast<unsigned int>(4294967295LL) ) ) ;


    assert( "4294967294" == Lsc::check_cast<Lsc::string>( static_cast<unsigned int>(4294967294LL) ) ) ;


    assert( "-32768" == Lsc::check_cast<Lsc::string>( static_cast<short>(-32768LL) ) ) ;


    assert( "-32767" == Lsc::check_cast<Lsc::string>( static_cast<short>(-32767LL) ) ) ;


    assert( "32767" == Lsc::check_cast<Lsc::string>( static_cast<short>(32767LL) ) ) ;


    assert( "32766" == Lsc::check_cast<Lsc::string>( static_cast<short>(32766LL) ) ) ;


    assert( "-2147483648" == Lsc::check_cast<Lsc::string>( static_cast<long>(-2147483648LL) ) ) ;


    assert( "-2147483647" == Lsc::check_cast<Lsc::string>( static_cast<long>(-2147483647LL) ) ) ;


    assert( "2147483647" == Lsc::check_cast<Lsc::string>( static_cast<long>(2147483647LL) ) ) ;


    assert( "2147483646" == Lsc::check_cast<Lsc::string>( static_cast<long>(2147483646LL) ) ) ;


    assert( "0" == Lsc::check_cast<Lsc::string>( static_cast<unsigned short>(0LL) ) ) ;


    assert( "1" == Lsc::check_cast<Lsc::string>( static_cast<unsigned short>(1LL) ) ) ;


    assert( "65535" == Lsc::check_cast<Lsc::string>( static_cast<unsigned short>(65535LL) ) ) ;


    assert( "65534" == Lsc::check_cast<Lsc::string>( static_cast<unsigned short>(65534LL) ) ) ;


    assert( "-9223372036854775808" == Lsc::check_cast<Lsc::string>( static_cast<long long>(-9223372036854775808.0L) ) ) ;


    assert( "-9223372036854775807" == Lsc::check_cast<Lsc::string>( static_cast<long long>(-9223372036854775807LL) ) ) ;


    assert( "9223372036854775807" == Lsc::check_cast<Lsc::string>( static_cast<long long>(9223372036854775807ULL) ) ) ;


    assert( "9223372036854775806" == Lsc::check_cast<Lsc::string>( static_cast<long long>(9223372036854775806LL) ) ) ;


    assert( "-128" == Lsc::check_cast<Lsc::string>( static_cast<signed char>(-128LL) ) ) ;


    assert( "-127" == Lsc::check_cast<Lsc::string>( static_cast<signed char>(-127LL) ) ) ;


    assert( "127" == Lsc::check_cast<Lsc::string>( static_cast<signed char>(127LL) ) ) ;


    assert( "126" == Lsc::check_cast<Lsc::string>( static_cast<signed char>(126LL) ) ) ;


    assert( "-1234.5" == Lsc::check_cast<Lsc::string>( static_cast<float>(-1234.5) ) ) ;


    assert( "1.234e+12" == Lsc::check_cast<Lsc::string>( static_cast<float>(1.234e+12) ) ) ;


    assert( "-1234.5" == Lsc::check_cast<Lsc::string>( static_cast<dolcle>(-1234.5) ) ) ;


    assert( "1.234e+12" == Lsc::check_cast<Lsc::string>( static_cast<dolcle>(1.234e+12) ) ) ;


    assert( "-1234.5" == Lsc::check_cast<Lsc::string>( static_cast<long dolcle>(-1234.5) ) ) ;


    assert( "1.234e+12" == Lsc::check_cast<Lsc::string>( static_cast<long dolcle>(1.234e+12) ) ) ;


    assert( "A" == Lsc::check_cast<Lsc::string>( static_cast<char>('A') ) ) ;

#else
		//int -> int

    assert( -2147483648LL == Lsc::check_cast<int>( static_cast<int>(-2147483648LL) ) ) ;
    assert( -2147483648LL + 1 == Lsc::check_cast<int>( static_cast<int>(-2147483648LL + 1) ) ) ;


    assert( 2147483647LL - 1== Lsc::check_cast<int>( static_cast<int>(2147483647LL - 1) ) ) ;
    assert( 2147483647LL == Lsc::check_cast<int>( static_cast<int>(2147483647LL) ) ) ;

		//unsigned char -> int
		//unsigned long long -> int

    assert( 2147483647LL - 1== Lsc::check_cast<int>( static_cast<unsigned long long>(2147483647LL - 1) ) ) ;
    assert( 2147483647LL == Lsc::check_cast<int>( static_cast<unsigned long long>(2147483647LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<int>( static_cast<unsigned long long>(2147483647LL + 1) ), Lsc::OverflowException );

		//char -> int
		//unsigned long -> int

    assert( 2147483647LL - 1== Lsc::check_cast<int>( static_cast<unsigned long>(2147483647LL - 1) ) ) ;
    assert( 2147483647LL == Lsc::check_cast<int>( static_cast<unsigned long>(2147483647LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<int>( static_cast<unsigned long>(2147483647LL + 1) ), Lsc::OverflowException );

		//size_t -> int

    assert( 2147483647LL - 1== Lsc::check_cast<int>( static_cast<size_t>(2147483647LL - 1) ) ) ;
    assert( 2147483647LL == Lsc::check_cast<int>( static_cast<size_t>(2147483647LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<int>( static_cast<size_t>(2147483647LL + 1) ), Lsc::OverflowException );

		//unsigned int -> int

    assert( 2147483647LL - 1== Lsc::check_cast<int>( static_cast<unsigned int>(2147483647LL - 1) ) ) ;
    assert( 2147483647LL == Lsc::check_cast<int>( static_cast<unsigned int>(2147483647LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<int>( static_cast<unsigned int>(2147483647LL + 1) ), Lsc::OverflowException );

		//short -> int
		//long -> int

    assert( -2147483648LL == Lsc::check_cast<int>( static_cast<long>(-2147483648LL) ) ) ;
    assert( -2147483648LL + 1 == Lsc::check_cast<int>( static_cast<long>(-2147483648LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<int>( static_cast<long>(-2147483648LL - 1) ), Lsc::UnderflowException );


    assert( 2147483647LL - 1== Lsc::check_cast<int>( static_cast<long>(2147483647LL - 1) ) ) ;
    assert( 2147483647LL == Lsc::check_cast<int>( static_cast<long>(2147483647LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<int>( static_cast<long>(2147483647LL + 1) ), Lsc::OverflowException );

		//unsigned short -> int
		//long long -> int

    assert( -2147483648LL == Lsc::check_cast<int>( static_cast<long long>(-2147483648LL) ) ) ;
    assert( -2147483648LL + 1 == Lsc::check_cast<int>( static_cast<long long>(-2147483648LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<int>( static_cast<long long>(-2147483648LL - 1) ), Lsc::UnderflowException );


    assert( 2147483647LL - 1== Lsc::check_cast<int>( static_cast<long long>(2147483647LL - 1) ) ) ;
    assert( 2147483647LL == Lsc::check_cast<int>( static_cast<long long>(2147483647LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<int>( static_cast<long long>(2147483647LL + 1) ), Lsc::OverflowException );

		//signed char -> int
		//int -> unsigned char

    assert( 0LL == Lsc::check_cast<unsigned char>( static_cast<int>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned char>( static_cast<int>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned char>( static_cast<int>(0LL - 1) ), Lsc::UnderflowException );


    assert( 255LL - 1== Lsc::check_cast<unsigned char>( static_cast<int>(255LL - 1) ) ) ;
    assert( 255LL == Lsc::check_cast<unsigned char>( static_cast<int>(255LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned char>( static_cast<int>(255LL + 1) ), Lsc::OverflowException );

		//unsigned char -> unsigned char

    assert( 0LL == Lsc::check_cast<unsigned char>( static_cast<unsigned char>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned char>( static_cast<unsigned char>(0LL + 1) ) ) ;


    assert( 255LL - 1== Lsc::check_cast<unsigned char>( static_cast<unsigned char>(255LL - 1) ) ) ;
    assert( 255LL == Lsc::check_cast<unsigned char>( static_cast<unsigned char>(255LL) ) ) ;

		//unsigned long long -> unsigned char

    assert( 0LL == Lsc::check_cast<unsigned char>( static_cast<unsigned long long>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned char>( static_cast<unsigned long long>(0LL + 1) ) ) ;


    assert( 255LL - 1== Lsc::check_cast<unsigned char>( static_cast<unsigned long long>(255LL - 1) ) ) ;
    assert( 255LL == Lsc::check_cast<unsigned char>( static_cast<unsigned long long>(255LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned char>( static_cast<unsigned long long>(255LL + 1) ), Lsc::OverflowException );

		//char -> unsigned char

    assert( 0LL == Lsc::check_cast<unsigned char>( static_cast<char>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned char>( static_cast<char>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned char>( static_cast<char>(0LL - 1) ), Lsc::UnderflowException );

		//unsigned long -> unsigned char

    assert( 0LL == Lsc::check_cast<unsigned char>( static_cast<unsigned long>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned char>( static_cast<unsigned long>(0LL + 1) ) ) ;


    assert( 255LL - 1== Lsc::check_cast<unsigned char>( static_cast<unsigned long>(255LL - 1) ) ) ;
    assert( 255LL == Lsc::check_cast<unsigned char>( static_cast<unsigned long>(255LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned char>( static_cast<unsigned long>(255LL + 1) ), Lsc::OverflowException );

		//size_t -> unsigned char

    assert( 0LL == Lsc::check_cast<unsigned char>( static_cast<size_t>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned char>( static_cast<size_t>(0LL + 1) ) ) ;


    assert( 255LL - 1== Lsc::check_cast<unsigned char>( static_cast<size_t>(255LL - 1) ) ) ;
    assert( 255LL == Lsc::check_cast<unsigned char>( static_cast<size_t>(255LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned char>( static_cast<size_t>(255LL + 1) ), Lsc::OverflowException );

		//unsigned int -> unsigned char

    assert( 0LL == Lsc::check_cast<unsigned char>( static_cast<unsigned int>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned char>( static_cast<unsigned int>(0LL + 1) ) ) ;


    assert( 255LL - 1== Lsc::check_cast<unsigned char>( static_cast<unsigned int>(255LL - 1) ) ) ;
    assert( 255LL == Lsc::check_cast<unsigned char>( static_cast<unsigned int>(255LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned char>( static_cast<unsigned int>(255LL + 1) ), Lsc::OverflowException );

		//short -> unsigned char

    assert( 0LL == Lsc::check_cast<unsigned char>( static_cast<short>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned char>( static_cast<short>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned char>( static_cast<short>(0LL - 1) ), Lsc::UnderflowException );


    assert( 255LL - 1== Lsc::check_cast<unsigned char>( static_cast<short>(255LL - 1) ) ) ;
    assert( 255LL == Lsc::check_cast<unsigned char>( static_cast<short>(255LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned char>( static_cast<short>(255LL + 1) ), Lsc::OverflowException );

		//long -> unsigned char

    assert( 0LL == Lsc::check_cast<unsigned char>( static_cast<long>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned char>( static_cast<long>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned char>( static_cast<long>(0LL - 1) ), Lsc::UnderflowException );


    assert( 255LL - 1== Lsc::check_cast<unsigned char>( static_cast<long>(255LL - 1) ) ) ;
    assert( 255LL == Lsc::check_cast<unsigned char>( static_cast<long>(255LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned char>( static_cast<long>(255LL + 1) ), Lsc::OverflowException );

		//unsigned short -> unsigned char

    assert( 0LL == Lsc::check_cast<unsigned char>( static_cast<unsigned short>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned char>( static_cast<unsigned short>(0LL + 1) ) ) ;


    assert( 255LL - 1== Lsc::check_cast<unsigned char>( static_cast<unsigned short>(255LL - 1) ) ) ;
    assert( 255LL == Lsc::check_cast<unsigned char>( static_cast<unsigned short>(255LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned char>( static_cast<unsigned short>(255LL + 1) ), Lsc::OverflowException );

		//long long -> unsigned char

    assert( 0LL == Lsc::check_cast<unsigned char>( static_cast<long long>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned char>( static_cast<long long>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned char>( static_cast<long long>(0LL - 1) ), Lsc::UnderflowException );


    assert( 255LL - 1== Lsc::check_cast<unsigned char>( static_cast<long long>(255LL - 1) ) ) ;
    assert( 255LL == Lsc::check_cast<unsigned char>( static_cast<long long>(255LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned char>( static_cast<long long>(255LL + 1) ), Lsc::OverflowException );

		//signed char -> unsigned char

    assert( 0LL == Lsc::check_cast<unsigned char>( static_cast<signed char>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned char>( static_cast<signed char>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned char>( static_cast<signed char>(0LL - 1) ), Lsc::UnderflowException );

		//int -> unsigned long long

    assert( 0LL == Lsc::check_cast<unsigned long long>( static_cast<int>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned long long>( static_cast<int>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned long long>( static_cast<int>(0LL - 1) ), Lsc::UnderflowException );

		//unsigned char -> unsigned long long

    assert( 0LL == Lsc::check_cast<unsigned long long>( static_cast<unsigned char>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned long long>( static_cast<unsigned char>(0LL + 1) ) ) ;

		//unsigned long long -> unsigned long long

    assert( 0LL == Lsc::check_cast<unsigned long long>( static_cast<unsigned long long>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned long long>( static_cast<unsigned long long>(0LL + 1) ) ) ;


    assert( 18446744073709551615ULL - 1== Lsc::check_cast<unsigned long long>( static_cast<unsigned long long>(18446744073709551615ULL - 1) ) ) ;
    assert( 18446744073709551615ULL == Lsc::check_cast<unsigned long long>( static_cast<unsigned long long>(18446744073709551615ULL) ) ) ;

		//char -> unsigned long long

    assert( 0LL == Lsc::check_cast<unsigned long long>( static_cast<char>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned long long>( static_cast<char>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned long long>( static_cast<char>(0LL - 1) ), Lsc::UnderflowException );

		//unsigned long -> unsigned long long

    assert( 0LL == Lsc::check_cast<unsigned long long>( static_cast<unsigned long>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned long long>( static_cast<unsigned long>(0LL + 1) ) ) ;


    assert( 18446744073709551615ULL - 1== Lsc::check_cast<unsigned long long>( static_cast<unsigned long>(18446744073709551615ULL - 1) ) ) ;
    assert( 18446744073709551615ULL == Lsc::check_cast<unsigned long long>( static_cast<unsigned long>(18446744073709551615ULL) ) ) ;

		//size_t -> unsigned long long

    assert( 0LL == Lsc::check_cast<unsigned long long>( static_cast<size_t>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned long long>( static_cast<size_t>(0LL + 1) ) ) ;


    assert( 18446744073709551615ULL - 1== Lsc::check_cast<unsigned long long>( static_cast<size_t>(18446744073709551615ULL - 1) ) ) ;
    assert( 18446744073709551615ULL == Lsc::check_cast<unsigned long long>( static_cast<size_t>(18446744073709551615ULL) ) ) ;

		//unsigned int -> unsigned long long

    assert( 0LL == Lsc::check_cast<unsigned long long>( static_cast<unsigned int>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned long long>( static_cast<unsigned int>(0LL + 1) ) ) ;

		//short -> unsigned long long

    assert( 0LL == Lsc::check_cast<unsigned long long>( static_cast<short>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned long long>( static_cast<short>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned long long>( static_cast<short>(0LL - 1) ), Lsc::UnderflowException );

		//long -> unsigned long long

    assert( 0LL == Lsc::check_cast<unsigned long long>( static_cast<long>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned long long>( static_cast<long>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned long long>( static_cast<long>(0LL - 1) ), Lsc::UnderflowException );

		//unsigned short -> unsigned long long

    assert( 0LL == Lsc::check_cast<unsigned long long>( static_cast<unsigned short>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned long long>( static_cast<unsigned short>(0LL + 1) ) ) ;

		//long long -> unsigned long long

    assert( 0LL == Lsc::check_cast<unsigned long long>( static_cast<long long>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned long long>( static_cast<long long>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned long long>( static_cast<long long>(0LL - 1) ), Lsc::UnderflowException );

		//signed char -> unsigned long long

    assert( 0LL == Lsc::check_cast<unsigned long long>( static_cast<signed char>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned long long>( static_cast<signed char>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned long long>( static_cast<signed char>(0LL - 1) ), Lsc::UnderflowException );

		//int -> char

    assert( -128LL == Lsc::check_cast<char>( static_cast<int>(-128LL) ) ) ;
    assert( -128LL + 1 == Lsc::check_cast<char>( static_cast<int>(-128LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<char>( static_cast<int>(-128LL - 1) ), Lsc::UnderflowException );


    assert( 127LL - 1== Lsc::check_cast<char>( static_cast<int>(127LL - 1) ) ) ;
    assert( 127LL == Lsc::check_cast<char>( static_cast<int>(127LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<char>( static_cast<int>(127LL + 1) ), Lsc::OverflowException );

		//unsigned char -> char

    assert( 127LL - 1== Lsc::check_cast<char>( static_cast<unsigned char>(127LL - 1) ) ) ;
    assert( 127LL == Lsc::check_cast<char>( static_cast<unsigned char>(127LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<char>( static_cast<unsigned char>(127LL + 1) ), Lsc::OverflowException );

		//unsigned long long -> char

    assert( 127LL - 1== Lsc::check_cast<char>( static_cast<unsigned long long>(127LL - 1) ) ) ;
    assert( 127LL == Lsc::check_cast<char>( static_cast<unsigned long long>(127LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<char>( static_cast<unsigned long long>(127LL + 1) ), Lsc::OverflowException );

		//char -> char

    assert( -128LL == Lsc::check_cast<char>( static_cast<char>(-128LL) ) ) ;
    assert( -128LL + 1 == Lsc::check_cast<char>( static_cast<char>(-128LL + 1) ) ) ;


    assert( 127LL - 1== Lsc::check_cast<char>( static_cast<char>(127LL - 1) ) ) ;
    assert( 127LL == Lsc::check_cast<char>( static_cast<char>(127LL) ) ) ;

		//unsigned long -> char

    assert( 127LL - 1== Lsc::check_cast<char>( static_cast<unsigned long>(127LL - 1) ) ) ;
    assert( 127LL == Lsc::check_cast<char>( static_cast<unsigned long>(127LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<char>( static_cast<unsigned long>(127LL + 1) ), Lsc::OverflowException );

		//size_t -> char

    assert( 127LL - 1== Lsc::check_cast<char>( static_cast<size_t>(127LL - 1) ) ) ;
    assert( 127LL == Lsc::check_cast<char>( static_cast<size_t>(127LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<char>( static_cast<size_t>(127LL + 1) ), Lsc::OverflowException );

		//unsigned int -> char

    assert( 127LL - 1== Lsc::check_cast<char>( static_cast<unsigned int>(127LL - 1) ) ) ;
    assert( 127LL == Lsc::check_cast<char>( static_cast<unsigned int>(127LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<char>( static_cast<unsigned int>(127LL + 1) ), Lsc::OverflowException );

		//short -> char

    assert( -128LL == Lsc::check_cast<char>( static_cast<short>(-128LL) ) ) ;
    assert( -128LL + 1 == Lsc::check_cast<char>( static_cast<short>(-128LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<char>( static_cast<short>(-128LL - 1) ), Lsc::UnderflowException );


    assert( 127LL - 1== Lsc::check_cast<char>( static_cast<short>(127LL - 1) ) ) ;
    assert( 127LL == Lsc::check_cast<char>( static_cast<short>(127LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<char>( static_cast<short>(127LL + 1) ), Lsc::OverflowException );

		//long -> char

    assert( -128LL == Lsc::check_cast<char>( static_cast<long>(-128LL) ) ) ;
    assert( -128LL + 1 == Lsc::check_cast<char>( static_cast<long>(-128LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<char>( static_cast<long>(-128LL - 1) ), Lsc::UnderflowException );


    assert( 127LL - 1== Lsc::check_cast<char>( static_cast<long>(127LL - 1) ) ) ;
    assert( 127LL == Lsc::check_cast<char>( static_cast<long>(127LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<char>( static_cast<long>(127LL + 1) ), Lsc::OverflowException );

		//unsigned short -> char

    assert( 127LL - 1== Lsc::check_cast<char>( static_cast<unsigned short>(127LL - 1) ) ) ;
    assert( 127LL == Lsc::check_cast<char>( static_cast<unsigned short>(127LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<char>( static_cast<unsigned short>(127LL + 1) ), Lsc::OverflowException );

		//long long -> char

    assert( -128LL == Lsc::check_cast<char>( static_cast<long long>(-128LL) ) ) ;
    assert( -128LL + 1 == Lsc::check_cast<char>( static_cast<long long>(-128LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<char>( static_cast<long long>(-128LL - 1) ), Lsc::UnderflowException );


    assert( 127LL - 1== Lsc::check_cast<char>( static_cast<long long>(127LL - 1) ) ) ;
    assert( 127LL == Lsc::check_cast<char>( static_cast<long long>(127LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<char>( static_cast<long long>(127LL + 1) ), Lsc::OverflowException );

		//signed char -> char

    assert( -128LL == Lsc::check_cast<char>( static_cast<signed char>(-128LL) ) ) ;
    assert( -128LL + 1 == Lsc::check_cast<char>( static_cast<signed char>(-128LL + 1) ) ) ;


    assert( 127LL - 1== Lsc::check_cast<char>( static_cast<signed char>(127LL - 1) ) ) ;
    assert( 127LL == Lsc::check_cast<char>( static_cast<signed char>(127LL) ) ) ;

		//int -> unsigned long

    assert( 0LL == Lsc::check_cast<unsigned long>( static_cast<int>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned long>( static_cast<int>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned long>( static_cast<int>(0LL - 1) ), Lsc::UnderflowException );

		//unsigned char -> unsigned long

    assert( 0LL == Lsc::check_cast<unsigned long>( static_cast<unsigned char>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned long>( static_cast<unsigned char>(0LL + 1) ) ) ;

		//unsigned long long -> unsigned long

    assert( 0LL == Lsc::check_cast<unsigned long>( static_cast<unsigned long long>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned long>( static_cast<unsigned long long>(0LL + 1) ) ) ;


    assert( 18446744073709551615ULL - 1== Lsc::check_cast<unsigned long>( static_cast<unsigned long long>(18446744073709551615ULL - 1) ) ) ;
    assert( 18446744073709551615ULL == Lsc::check_cast<unsigned long>( static_cast<unsigned long long>(18446744073709551615ULL) ) ) ;

		//char -> unsigned long

    assert( 0LL == Lsc::check_cast<unsigned long>( static_cast<char>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned long>( static_cast<char>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned long>( static_cast<char>(0LL - 1) ), Lsc::UnderflowException );

		//unsigned long -> unsigned long

    assert( 0LL == Lsc::check_cast<unsigned long>( static_cast<unsigned long>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned long>( static_cast<unsigned long>(0LL + 1) ) ) ;


    assert( 18446744073709551615ULL - 1== Lsc::check_cast<unsigned long>( static_cast<unsigned long>(18446744073709551615ULL - 1) ) ) ;
    assert( 18446744073709551615ULL == Lsc::check_cast<unsigned long>( static_cast<unsigned long>(18446744073709551615ULL) ) ) ;

		//size_t -> unsigned long

    assert( 0LL == Lsc::check_cast<unsigned long>( static_cast<size_t>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned long>( static_cast<size_t>(0LL + 1) ) ) ;


    assert( 18446744073709551615ULL - 1== Lsc::check_cast<unsigned long>( static_cast<size_t>(18446744073709551615ULL - 1) ) ) ;
    assert( 18446744073709551615ULL == Lsc::check_cast<unsigned long>( static_cast<size_t>(18446744073709551615ULL) ) ) ;

		//unsigned int -> unsigned long

    assert( 0LL == Lsc::check_cast<unsigned long>( static_cast<unsigned int>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned long>( static_cast<unsigned int>(0LL + 1) ) ) ;

		//short -> unsigned long

    assert( 0LL == Lsc::check_cast<unsigned long>( static_cast<short>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned long>( static_cast<short>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned long>( static_cast<short>(0LL - 1) ), Lsc::UnderflowException );

		//long -> unsigned long

    assert( 0LL == Lsc::check_cast<unsigned long>( static_cast<long>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned long>( static_cast<long>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned long>( static_cast<long>(0LL - 1) ), Lsc::UnderflowException );

		//unsigned short -> unsigned long

    assert( 0LL == Lsc::check_cast<unsigned long>( static_cast<unsigned short>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned long>( static_cast<unsigned short>(0LL + 1) ) ) ;

		//long long -> unsigned long

    assert( 0LL == Lsc::check_cast<unsigned long>( static_cast<long long>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned long>( static_cast<long long>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned long>( static_cast<long long>(0LL - 1) ), Lsc::UnderflowException );

		//signed char -> unsigned long

    assert( 0LL == Lsc::check_cast<unsigned long>( static_cast<signed char>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned long>( static_cast<signed char>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned long>( static_cast<signed char>(0LL - 1) ), Lsc::UnderflowException );

		//int -> size_t

    assert( 0LL == Lsc::check_cast<size_t>( static_cast<int>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<size_t>( static_cast<int>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<size_t>( static_cast<int>(0LL - 1) ), Lsc::UnderflowException );

		//unsigned char -> size_t

    assert( 0LL == Lsc::check_cast<size_t>( static_cast<unsigned char>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<size_t>( static_cast<unsigned char>(0LL + 1) ) ) ;

		//unsigned long long -> size_t

    assert( 0LL == Lsc::check_cast<size_t>( static_cast<unsigned long long>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<size_t>( static_cast<unsigned long long>(0LL + 1) ) ) ;


    assert( 18446744073709551615ULL - 1== Lsc::check_cast<size_t>( static_cast<unsigned long long>(18446744073709551615ULL - 1) ) ) ;
    assert( 18446744073709551615ULL == Lsc::check_cast<size_t>( static_cast<unsigned long long>(18446744073709551615ULL) ) ) ;

		//char -> size_t

    assert( 0LL == Lsc::check_cast<size_t>( static_cast<char>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<size_t>( static_cast<char>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<size_t>( static_cast<char>(0LL - 1) ), Lsc::UnderflowException );

		//unsigned long -> size_t

    assert( 0LL == Lsc::check_cast<size_t>( static_cast<unsigned long>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<size_t>( static_cast<unsigned long>(0LL + 1) ) ) ;


    assert( 18446744073709551615ULL - 1== Lsc::check_cast<size_t>( static_cast<unsigned long>(18446744073709551615ULL - 1) ) ) ;
    assert( 18446744073709551615ULL == Lsc::check_cast<size_t>( static_cast<unsigned long>(18446744073709551615ULL) ) ) ;

		//size_t -> size_t

    assert( 0LL == Lsc::check_cast<size_t>( static_cast<size_t>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<size_t>( static_cast<size_t>(0LL + 1) ) ) ;


    assert( 18446744073709551615ULL - 1== Lsc::check_cast<size_t>( static_cast<size_t>(18446744073709551615ULL - 1) ) ) ;
    assert( 18446744073709551615ULL == Lsc::check_cast<size_t>( static_cast<size_t>(18446744073709551615ULL) ) ) ;

		//unsigned int -> size_t

    assert( 0LL == Lsc::check_cast<size_t>( static_cast<unsigned int>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<size_t>( static_cast<unsigned int>(0LL + 1) ) ) ;

		//short -> size_t

    assert( 0LL == Lsc::check_cast<size_t>( static_cast<short>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<size_t>( static_cast<short>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<size_t>( static_cast<short>(0LL - 1) ), Lsc::UnderflowException );

		//long -> size_t

    assert( 0LL == Lsc::check_cast<size_t>( static_cast<long>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<size_t>( static_cast<long>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<size_t>( static_cast<long>(0LL - 1) ), Lsc::UnderflowException );

		//unsigned short -> size_t

    assert( 0LL == Lsc::check_cast<size_t>( static_cast<unsigned short>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<size_t>( static_cast<unsigned short>(0LL + 1) ) ) ;

		//long long -> size_t

    assert( 0LL == Lsc::check_cast<size_t>( static_cast<long long>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<size_t>( static_cast<long long>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<size_t>( static_cast<long long>(0LL - 1) ), Lsc::UnderflowException );

		//signed char -> size_t

    assert( 0LL == Lsc::check_cast<size_t>( static_cast<signed char>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<size_t>( static_cast<signed char>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<size_t>( static_cast<signed char>(0LL - 1) ), Lsc::UnderflowException );

		//int -> unsigned int

    assert( 0LL == Lsc::check_cast<unsigned int>( static_cast<int>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned int>( static_cast<int>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned int>( static_cast<int>(0LL - 1) ), Lsc::UnderflowException );

		//unsigned char -> unsigned int

    assert( 0LL == Lsc::check_cast<unsigned int>( static_cast<unsigned char>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned int>( static_cast<unsigned char>(0LL + 1) ) ) ;

		//unsigned long long -> unsigned int

    assert( 0LL == Lsc::check_cast<unsigned int>( static_cast<unsigned long long>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned int>( static_cast<unsigned long long>(0LL + 1) ) ) ;


    assert( 4294967295LL - 1== Lsc::check_cast<unsigned int>( static_cast<unsigned long long>(4294967295LL - 1) ) ) ;
    assert( 4294967295LL == Lsc::check_cast<unsigned int>( static_cast<unsigned long long>(4294967295LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned int>( static_cast<unsigned long long>(4294967295LL + 1) ), Lsc::OverflowException );

		//char -> unsigned int

    assert( 0LL == Lsc::check_cast<unsigned int>( static_cast<char>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned int>( static_cast<char>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned int>( static_cast<char>(0LL - 1) ), Lsc::UnderflowException );

		//unsigned long -> unsigned int

    assert( 0LL == Lsc::check_cast<unsigned int>( static_cast<unsigned long>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned int>( static_cast<unsigned long>(0LL + 1) ) ) ;


    assert( 4294967295LL - 1== Lsc::check_cast<unsigned int>( static_cast<unsigned long>(4294967295LL - 1) ) ) ;
    assert( 4294967295LL == Lsc::check_cast<unsigned int>( static_cast<unsigned long>(4294967295LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned int>( static_cast<unsigned long>(4294967295LL + 1) ), Lsc::OverflowException );

		//size_t -> unsigned int

    assert( 0LL == Lsc::check_cast<unsigned int>( static_cast<size_t>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned int>( static_cast<size_t>(0LL + 1) ) ) ;


    assert( 4294967295LL - 1== Lsc::check_cast<unsigned int>( static_cast<size_t>(4294967295LL - 1) ) ) ;
    assert( 4294967295LL == Lsc::check_cast<unsigned int>( static_cast<size_t>(4294967295LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned int>( static_cast<size_t>(4294967295LL + 1) ), Lsc::OverflowException );

		//unsigned int -> unsigned int

    assert( 0LL == Lsc::check_cast<unsigned int>( static_cast<unsigned int>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned int>( static_cast<unsigned int>(0LL + 1) ) ) ;


    assert( 4294967295LL - 1== Lsc::check_cast<unsigned int>( static_cast<unsigned int>(4294967295LL - 1) ) ) ;
    assert( 4294967295LL == Lsc::check_cast<unsigned int>( static_cast<unsigned int>(4294967295LL) ) ) ;

		//short -> unsigned int

    assert( 0LL == Lsc::check_cast<unsigned int>( static_cast<short>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned int>( static_cast<short>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned int>( static_cast<short>(0LL - 1) ), Lsc::UnderflowException );

		//long -> unsigned int

    assert( 0LL == Lsc::check_cast<unsigned int>( static_cast<long>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned int>( static_cast<long>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned int>( static_cast<long>(0LL - 1) ), Lsc::UnderflowException );


    assert( 4294967295LL - 1== Lsc::check_cast<unsigned int>( static_cast<long>(4294967295LL - 1) ) ) ;
    assert( 4294967295LL == Lsc::check_cast<unsigned int>( static_cast<long>(4294967295LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned int>( static_cast<long>(4294967295LL + 1) ), Lsc::OverflowException );

		//unsigned short -> unsigned int

    assert( 0LL == Lsc::check_cast<unsigned int>( static_cast<unsigned short>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned int>( static_cast<unsigned short>(0LL + 1) ) ) ;

		//long long -> unsigned int

    assert( 0LL == Lsc::check_cast<unsigned int>( static_cast<long long>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned int>( static_cast<long long>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned int>( static_cast<long long>(0LL - 1) ), Lsc::UnderflowException );


    assert( 4294967295LL - 1== Lsc::check_cast<unsigned int>( static_cast<long long>(4294967295LL - 1) ) ) ;
    assert( 4294967295LL == Lsc::check_cast<unsigned int>( static_cast<long long>(4294967295LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned int>( static_cast<long long>(4294967295LL + 1) ), Lsc::OverflowException );

		//signed char -> unsigned int

    assert( 0LL == Lsc::check_cast<unsigned int>( static_cast<signed char>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned int>( static_cast<signed char>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned int>( static_cast<signed char>(0LL - 1) ), Lsc::UnderflowException );

		//int -> short

    assert( -32768LL == Lsc::check_cast<short>( static_cast<int>(-32768LL) ) ) ;
    assert( -32768LL + 1 == Lsc::check_cast<short>( static_cast<int>(-32768LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<short>( static_cast<int>(-32768LL - 1) ), Lsc::UnderflowException );


    assert( 32767LL - 1== Lsc::check_cast<short>( static_cast<int>(32767LL - 1) ) ) ;
    assert( 32767LL == Lsc::check_cast<short>( static_cast<int>(32767LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<short>( static_cast<int>(32767LL + 1) ), Lsc::OverflowException );

		//unsigned char -> short
		//unsigned long long -> short

    assert( 32767LL - 1== Lsc::check_cast<short>( static_cast<unsigned long long>(32767LL - 1) ) ) ;
    assert( 32767LL == Lsc::check_cast<short>( static_cast<unsigned long long>(32767LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<short>( static_cast<unsigned long long>(32767LL + 1) ), Lsc::OverflowException );

		//char -> short
		//unsigned long -> short

    assert( 32767LL - 1== Lsc::check_cast<short>( static_cast<unsigned long>(32767LL - 1) ) ) ;
    assert( 32767LL == Lsc::check_cast<short>( static_cast<unsigned long>(32767LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<short>( static_cast<unsigned long>(32767LL + 1) ), Lsc::OverflowException );

		//size_t -> short

    assert( 32767LL - 1== Lsc::check_cast<short>( static_cast<size_t>(32767LL - 1) ) ) ;
    assert( 32767LL == Lsc::check_cast<short>( static_cast<size_t>(32767LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<short>( static_cast<size_t>(32767LL + 1) ), Lsc::OverflowException );

		//unsigned int -> short

    assert( 32767LL - 1== Lsc::check_cast<short>( static_cast<unsigned int>(32767LL - 1) ) ) ;
    assert( 32767LL == Lsc::check_cast<short>( static_cast<unsigned int>(32767LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<short>( static_cast<unsigned int>(32767LL + 1) ), Lsc::OverflowException );

		//short -> short

    assert( -32768LL == Lsc::check_cast<short>( static_cast<short>(-32768LL) ) ) ;
    assert( -32768LL + 1 == Lsc::check_cast<short>( static_cast<short>(-32768LL + 1) ) ) ;


    assert( 32767LL - 1== Lsc::check_cast<short>( static_cast<short>(32767LL - 1) ) ) ;
    assert( 32767LL == Lsc::check_cast<short>( static_cast<short>(32767LL) ) ) ;

		//long -> short

    assert( -32768LL == Lsc::check_cast<short>( static_cast<long>(-32768LL) ) ) ;
    assert( -32768LL + 1 == Lsc::check_cast<short>( static_cast<long>(-32768LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<short>( static_cast<long>(-32768LL - 1) ), Lsc::UnderflowException );


    assert( 32767LL - 1== Lsc::check_cast<short>( static_cast<long>(32767LL - 1) ) ) ;
    assert( 32767LL == Lsc::check_cast<short>( static_cast<long>(32767LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<short>( static_cast<long>(32767LL + 1) ), Lsc::OverflowException );

		//unsigned short -> short

    assert( 32767LL - 1== Lsc::check_cast<short>( static_cast<unsigned short>(32767LL - 1) ) ) ;
    assert( 32767LL == Lsc::check_cast<short>( static_cast<unsigned short>(32767LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<short>( static_cast<unsigned short>(32767LL + 1) ), Lsc::OverflowException );

		//long long -> short

    assert( -32768LL == Lsc::check_cast<short>( static_cast<long long>(-32768LL) ) ) ;
    assert( -32768LL + 1 == Lsc::check_cast<short>( static_cast<long long>(-32768LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<short>( static_cast<long long>(-32768LL - 1) ), Lsc::UnderflowException );


    assert( 32767LL - 1== Lsc::check_cast<short>( static_cast<long long>(32767LL - 1) ) ) ;
    assert( 32767LL == Lsc::check_cast<short>( static_cast<long long>(32767LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<short>( static_cast<long long>(32767LL + 1) ), Lsc::OverflowException );

		//signed char -> short
		//int -> long
		//unsigned char -> long
		//unsigned long long -> long

    assert( 9223372036854775807ULL - 1== Lsc::check_cast<long>( static_cast<unsigned long long>(9223372036854775807ULL - 1) ) ) ;
    assert( 9223372036854775807ULL == Lsc::check_cast<long>( static_cast<unsigned long long>(9223372036854775807ULL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<long>( static_cast<unsigned long long>(9223372036854775807ULL + 1) ), Lsc::OverflowException );

		//char -> long
		//unsigned long -> long

    assert( 9223372036854775807ULL - 1== Lsc::check_cast<long>( static_cast<unsigned long>(9223372036854775807ULL - 1) ) ) ;
    assert( 9223372036854775807ULL == Lsc::check_cast<long>( static_cast<unsigned long>(9223372036854775807ULL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<long>( static_cast<unsigned long>(9223372036854775807ULL + 1) ), Lsc::OverflowException );

		//size_t -> long

    assert( 9223372036854775807ULL - 1== Lsc::check_cast<long>( static_cast<size_t>(9223372036854775807ULL - 1) ) ) ;
    assert( 9223372036854775807ULL == Lsc::check_cast<long>( static_cast<size_t>(9223372036854775807ULL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<long>( static_cast<size_t>(9223372036854775807ULL + 1) ), Lsc::OverflowException );

		//unsigned int -> long
		//short -> long
		//long -> long

    assert( -9223372036854775808.0L == Lsc::check_cast<long>( static_cast<long>(-9223372036854775808.0L) ) ) ;
    assert( -9223372036854775808.0L + 1 == Lsc::check_cast<long>( static_cast<long>(-9223372036854775808.0L + 1) ) ) ;


    assert( 9223372036854775807ULL - 1== Lsc::check_cast<long>( static_cast<long>(9223372036854775807ULL - 1) ) ) ;
    assert( 9223372036854775807ULL == Lsc::check_cast<long>( static_cast<long>(9223372036854775807ULL) ) ) ;

		//unsigned short -> long
		//long long -> long

    assert( -9223372036854775808.0L == Lsc::check_cast<long>( static_cast<long long>(-9223372036854775808.0L) ) ) ;
    assert( -9223372036854775808.0L + 1 == Lsc::check_cast<long>( static_cast<long long>(-9223372036854775808.0L + 1) ) ) ;


    assert( 9223372036854775807ULL - 1== Lsc::check_cast<long>( static_cast<long long>(9223372036854775807ULL - 1) ) ) ;
    assert( 9223372036854775807ULL == Lsc::check_cast<long>( static_cast<long long>(9223372036854775807ULL) ) ) ;

		//signed char -> long
		//int -> unsigned short

    assert( 0LL == Lsc::check_cast<unsigned short>( static_cast<int>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned short>( static_cast<int>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned short>( static_cast<int>(0LL - 1) ), Lsc::UnderflowException );


    assert( 65535LL - 1== Lsc::check_cast<unsigned short>( static_cast<int>(65535LL - 1) ) ) ;
    assert( 65535LL == Lsc::check_cast<unsigned short>( static_cast<int>(65535LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned short>( static_cast<int>(65535LL + 1) ), Lsc::OverflowException );

		//unsigned char -> unsigned short

    assert( 0LL == Lsc::check_cast<unsigned short>( static_cast<unsigned char>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned short>( static_cast<unsigned char>(0LL + 1) ) ) ;

		//unsigned long long -> unsigned short

    assert( 0LL == Lsc::check_cast<unsigned short>( static_cast<unsigned long long>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned short>( static_cast<unsigned long long>(0LL + 1) ) ) ;


    assert( 65535LL - 1== Lsc::check_cast<unsigned short>( static_cast<unsigned long long>(65535LL - 1) ) ) ;
    assert( 65535LL == Lsc::check_cast<unsigned short>( static_cast<unsigned long long>(65535LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned short>( static_cast<unsigned long long>(65535LL + 1) ), Lsc::OverflowException );

		//char -> unsigned short

    assert( 0LL == Lsc::check_cast<unsigned short>( static_cast<char>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned short>( static_cast<char>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned short>( static_cast<char>(0LL - 1) ), Lsc::UnderflowException );

		//unsigned long -> unsigned short

    assert( 0LL == Lsc::check_cast<unsigned short>( static_cast<unsigned long>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned short>( static_cast<unsigned long>(0LL + 1) ) ) ;


    assert( 65535LL - 1== Lsc::check_cast<unsigned short>( static_cast<unsigned long>(65535LL - 1) ) ) ;
    assert( 65535LL == Lsc::check_cast<unsigned short>( static_cast<unsigned long>(65535LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned short>( static_cast<unsigned long>(65535LL + 1) ), Lsc::OverflowException );

		//size_t -> unsigned short

    assert( 0LL == Lsc::check_cast<unsigned short>( static_cast<size_t>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned short>( static_cast<size_t>(0LL + 1) ) ) ;


    assert( 65535LL - 1== Lsc::check_cast<unsigned short>( static_cast<size_t>(65535LL - 1) ) ) ;
    assert( 65535LL == Lsc::check_cast<unsigned short>( static_cast<size_t>(65535LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned short>( static_cast<size_t>(65535LL + 1) ), Lsc::OverflowException );

		//unsigned int -> unsigned short

    assert( 0LL == Lsc::check_cast<unsigned short>( static_cast<unsigned int>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned short>( static_cast<unsigned int>(0LL + 1) ) ) ;


    assert( 65535LL - 1== Lsc::check_cast<unsigned short>( static_cast<unsigned int>(65535LL - 1) ) ) ;
    assert( 65535LL == Lsc::check_cast<unsigned short>( static_cast<unsigned int>(65535LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned short>( static_cast<unsigned int>(65535LL + 1) ), Lsc::OverflowException );

		//short -> unsigned short

    assert( 0LL == Lsc::check_cast<unsigned short>( static_cast<short>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned short>( static_cast<short>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned short>( static_cast<short>(0LL - 1) ), Lsc::UnderflowException );

		//long -> unsigned short

    assert( 0LL == Lsc::check_cast<unsigned short>( static_cast<long>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned short>( static_cast<long>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned short>( static_cast<long>(0LL - 1) ), Lsc::UnderflowException );


    assert( 65535LL - 1== Lsc::check_cast<unsigned short>( static_cast<long>(65535LL - 1) ) ) ;
    assert( 65535LL == Lsc::check_cast<unsigned short>( static_cast<long>(65535LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned short>( static_cast<long>(65535LL + 1) ), Lsc::OverflowException );

		//unsigned short -> unsigned short

    assert( 0LL == Lsc::check_cast<unsigned short>( static_cast<unsigned short>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned short>( static_cast<unsigned short>(0LL + 1) ) ) ;


    assert( 65535LL - 1== Lsc::check_cast<unsigned short>( static_cast<unsigned short>(65535LL - 1) ) ) ;
    assert( 65535LL == Lsc::check_cast<unsigned short>( static_cast<unsigned short>(65535LL) ) ) ;

		//long long -> unsigned short

    assert( 0LL == Lsc::check_cast<unsigned short>( static_cast<long long>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned short>( static_cast<long long>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned short>( static_cast<long long>(0LL - 1) ), Lsc::UnderflowException );


    assert( 65535LL - 1== Lsc::check_cast<unsigned short>( static_cast<long long>(65535LL - 1) ) ) ;
    assert( 65535LL == Lsc::check_cast<unsigned short>( static_cast<long long>(65535LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned short>( static_cast<long long>(65535LL + 1) ), Lsc::OverflowException );

		//signed char -> unsigned short

    assert( 0LL == Lsc::check_cast<unsigned short>( static_cast<signed char>(0LL) ) ) ;
    assert( 0LL + 1 == Lsc::check_cast<unsigned short>( static_cast<signed char>(0LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned short>( static_cast<signed char>(0LL - 1) ), Lsc::UnderflowException );

		//int -> long long
		//unsigned char -> long long
		//unsigned long long -> long long

    assert( 9223372036854775807ULL - 1== Lsc::check_cast<long long>( static_cast<unsigned long long>(9223372036854775807ULL - 1) ) ) ;
    assert( 9223372036854775807ULL == Lsc::check_cast<long long>( static_cast<unsigned long long>(9223372036854775807ULL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<long long>( static_cast<unsigned long long>(9223372036854775807ULL + 1) ), Lsc::OverflowException );

		//char -> long long
		//unsigned long -> long long

    assert( 9223372036854775807ULL - 1== Lsc::check_cast<long long>( static_cast<unsigned long>(9223372036854775807ULL - 1) ) ) ;
    assert( 9223372036854775807ULL == Lsc::check_cast<long long>( static_cast<unsigned long>(9223372036854775807ULL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<long long>( static_cast<unsigned long>(9223372036854775807ULL + 1) ), Lsc::OverflowException );

		//size_t -> long long

    assert( 9223372036854775807ULL - 1== Lsc::check_cast<long long>( static_cast<size_t>(9223372036854775807ULL - 1) ) ) ;
    assert( 9223372036854775807ULL == Lsc::check_cast<long long>( static_cast<size_t>(9223372036854775807ULL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<long long>( static_cast<size_t>(9223372036854775807ULL + 1) ), Lsc::OverflowException );

		//unsigned int -> long long
		//short -> long long
		//long -> long long

    assert( -9223372036854775808.0L == Lsc::check_cast<long long>( static_cast<long>(-9223372036854775808.0L) ) ) ;
    assert( -9223372036854775808.0L + 1 == Lsc::check_cast<long long>( static_cast<long>(-9223372036854775808.0L + 1) ) ) ;


    assert( 9223372036854775807ULL - 1== Lsc::check_cast<long long>( static_cast<long>(9223372036854775807ULL - 1) ) ) ;
    assert( 9223372036854775807ULL == Lsc::check_cast<long long>( static_cast<long>(9223372036854775807ULL) ) ) ;

		//unsigned short -> long long
		//long long -> long long

    assert( -9223372036854775808.0L == Lsc::check_cast<long long>( static_cast<long long>(-9223372036854775808.0L) ) ) ;
    assert( -9223372036854775808.0L + 1 == Lsc::check_cast<long long>( static_cast<long long>(-9223372036854775808.0L + 1) ) ) ;


    assert( 9223372036854775807ULL - 1== Lsc::check_cast<long long>( static_cast<long long>(9223372036854775807ULL - 1) ) ) ;
    assert( 9223372036854775807ULL == Lsc::check_cast<long long>( static_cast<long long>(9223372036854775807ULL) ) ) ;

		//signed char -> long long
		//int -> signed char

    assert( -128LL == Lsc::check_cast<signed char>( static_cast<int>(-128LL) ) ) ;
    assert( -128LL + 1 == Lsc::check_cast<signed char>( static_cast<int>(-128LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<signed char>( static_cast<int>(-128LL - 1) ), Lsc::UnderflowException );


    assert( 127LL - 1== Lsc::check_cast<signed char>( static_cast<int>(127LL - 1) ) ) ;
    assert( 127LL == Lsc::check_cast<signed char>( static_cast<int>(127LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<signed char>( static_cast<int>(127LL + 1) ), Lsc::OverflowException );

		//unsigned char -> signed char

    assert( 127LL - 1== Lsc::check_cast<signed char>( static_cast<unsigned char>(127LL - 1) ) ) ;
    assert( 127LL == Lsc::check_cast<signed char>( static_cast<unsigned char>(127LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<signed char>( static_cast<unsigned char>(127LL + 1) ), Lsc::OverflowException );

		//unsigned long long -> signed char

    assert( 127LL - 1== Lsc::check_cast<signed char>( static_cast<unsigned long long>(127LL - 1) ) ) ;
    assert( 127LL == Lsc::check_cast<signed char>( static_cast<unsigned long long>(127LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<signed char>( static_cast<unsigned long long>(127LL + 1) ), Lsc::OverflowException );

		//char -> signed char

    assert( -128LL == Lsc::check_cast<signed char>( static_cast<char>(-128LL) ) ) ;
    assert( -128LL + 1 == Lsc::check_cast<signed char>( static_cast<char>(-128LL + 1) ) ) ;


    assert( 127LL - 1== Lsc::check_cast<signed char>( static_cast<char>(127LL - 1) ) ) ;
    assert( 127LL == Lsc::check_cast<signed char>( static_cast<char>(127LL) ) ) ;

		//unsigned long -> signed char

    assert( 127LL - 1== Lsc::check_cast<signed char>( static_cast<unsigned long>(127LL - 1) ) ) ;
    assert( 127LL == Lsc::check_cast<signed char>( static_cast<unsigned long>(127LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<signed char>( static_cast<unsigned long>(127LL + 1) ), Lsc::OverflowException );

		//size_t -> signed char

    assert( 127LL - 1== Lsc::check_cast<signed char>( static_cast<size_t>(127LL - 1) ) ) ;
    assert( 127LL == Lsc::check_cast<signed char>( static_cast<size_t>(127LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<signed char>( static_cast<size_t>(127LL + 1) ), Lsc::OverflowException );

		//unsigned int -> signed char

    assert( 127LL - 1== Lsc::check_cast<signed char>( static_cast<unsigned int>(127LL - 1) ) ) ;
    assert( 127LL == Lsc::check_cast<signed char>( static_cast<unsigned int>(127LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<signed char>( static_cast<unsigned int>(127LL + 1) ), Lsc::OverflowException );

		//short -> signed char

    assert( -128LL == Lsc::check_cast<signed char>( static_cast<short>(-128LL) ) ) ;
    assert( -128LL + 1 == Lsc::check_cast<signed char>( static_cast<short>(-128LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<signed char>( static_cast<short>(-128LL - 1) ), Lsc::UnderflowException );


    assert( 127LL - 1== Lsc::check_cast<signed char>( static_cast<short>(127LL - 1) ) ) ;
    assert( 127LL == Lsc::check_cast<signed char>( static_cast<short>(127LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<signed char>( static_cast<short>(127LL + 1) ), Lsc::OverflowException );

		//long -> signed char

    assert( -128LL == Lsc::check_cast<signed char>( static_cast<long>(-128LL) ) ) ;
    assert( -128LL + 1 == Lsc::check_cast<signed char>( static_cast<long>(-128LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<signed char>( static_cast<long>(-128LL - 1) ), Lsc::UnderflowException );


    assert( 127LL - 1== Lsc::check_cast<signed char>( static_cast<long>(127LL - 1) ) ) ;
    assert( 127LL == Lsc::check_cast<signed char>( static_cast<long>(127LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<signed char>( static_cast<long>(127LL + 1) ), Lsc::OverflowException );

		//unsigned short -> signed char

    assert( 127LL - 1== Lsc::check_cast<signed char>( static_cast<unsigned short>(127LL - 1) ) ) ;
    assert( 127LL == Lsc::check_cast<signed char>( static_cast<unsigned short>(127LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<signed char>( static_cast<unsigned short>(127LL + 1) ), Lsc::OverflowException );

		//long long -> signed char

    assert( -128LL == Lsc::check_cast<signed char>( static_cast<long long>(-128LL) ) ) ;
    assert( -128LL + 1 == Lsc::check_cast<signed char>( static_cast<long long>(-128LL + 1) ) ) ;


    ASSERT_THROW( Lsc::check_cast<signed char>( static_cast<long long>(-128LL - 1) ), Lsc::UnderflowException );


    assert( 127LL - 1== Lsc::check_cast<signed char>( static_cast<long long>(127LL - 1) ) ) ;
    assert( 127LL == Lsc::check_cast<signed char>( static_cast<long long>(127LL) ) ) ;


    ASSERT_THROW( Lsc::check_cast<signed char>( static_cast<long long>(127LL + 1) ), Lsc::OverflowException );

		//signed char -> signed char

    assert( -128LL == Lsc::check_cast<signed char>( static_cast<signed char>(-128LL) ) ) ;
    assert( -128LL + 1 == Lsc::check_cast<signed char>( static_cast<signed char>(-128LL + 1) ) ) ;


    assert( 127LL - 1== Lsc::check_cast<signed char>( static_cast<signed char>(127LL - 1) ) ) ;
    assert( 127LL == Lsc::check_cast<signed char>( static_cast<signed char>(127LL) ) ) ;

		//float -> int

    {
        //avoid floating-point number errors
        int value1 = static_cast<int>( static_cast<float>( -2147483648LL ) );
        int value2 = Lsc::check_cast<int>( static_cast<float>(-2147483648LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        int value1 = static_cast<int>( static_cast<float>( -2147483648LL + 2147483.647 ) );
        int value2 = Lsc::check_cast<int>( static_cast<float>(-2147483648LL + 2147483.647) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<int>( static_cast<float>(-2147483648LL - 2147483.647) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<int>( static_cast<float>(2147483647LL + 2147483.647) ), Lsc::OverflowException );

		//dolcle -> int

    {
        //avoid floating-point number errors
        int value1 = static_cast<int>( static_cast<dolcle>( -2147483648LL ) );
        int value2 = Lsc::check_cast<int>( static_cast<dolcle>(-2147483648LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        int value1 = static_cast<int>( static_cast<dolcle>( -2147483648LL + 2147483.647 ) );
        int value2 = Lsc::check_cast<int>( static_cast<dolcle>(-2147483648LL + 2147483.647) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<int>( static_cast<dolcle>(-2147483648LL - 2147483.647) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<int>( static_cast<dolcle>(2147483647LL + 2147483.647) ), Lsc::OverflowException );

		//long dolcle -> int

    {
        //avoid floating-point number errors
        int value1 = static_cast<int>( static_cast<long dolcle>( -2147483648LL ) );
        int value2 = Lsc::check_cast<int>( static_cast<long dolcle>(-2147483648LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        int value1 = static_cast<int>( static_cast<long dolcle>( -2147483648LL + 2147483.647 ) );
        int value2 = Lsc::check_cast<int>( static_cast<long dolcle>(-2147483648LL + 2147483.647) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<int>( static_cast<long dolcle>(-2147483648LL - 2147483.647) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<int>( static_cast<long dolcle>(2147483647LL + 2147483.647) ), Lsc::OverflowException );

		//float -> unsigned char

    {
        //avoid floating-point number errors
        unsigned char value1 = static_cast<unsigned char>( static_cast<float>( 0LL ) );
        unsigned char value2 = Lsc::check_cast<unsigned char>( static_cast<float>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        unsigned char value1 = static_cast<unsigned char>( static_cast<float>( 0LL + 0.255 ) );
        unsigned char value2 = Lsc::check_cast<unsigned char>( static_cast<float>(0LL + 0.255) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<unsigned char>( static_cast<float>(0LL - 0.255) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<unsigned char>( static_cast<float>(255LL + 0.255) ), Lsc::OverflowException );

		//dolcle -> unsigned char

    {
        //avoid floating-point number errors
        unsigned char value1 = static_cast<unsigned char>( static_cast<dolcle>( 0LL ) );
        unsigned char value2 = Lsc::check_cast<unsigned char>( static_cast<dolcle>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        unsigned char value1 = static_cast<unsigned char>( static_cast<dolcle>( 0LL + 0.255 ) );
        unsigned char value2 = Lsc::check_cast<unsigned char>( static_cast<dolcle>(0LL + 0.255) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<unsigned char>( static_cast<dolcle>(0LL - 0.255) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<unsigned char>( static_cast<dolcle>(255LL + 0.255) ), Lsc::OverflowException );

		//long dolcle -> unsigned char

    {
        //avoid floating-point number errors
        unsigned char value1 = static_cast<unsigned char>( static_cast<long dolcle>( 0LL ) );
        unsigned char value2 = Lsc::check_cast<unsigned char>( static_cast<long dolcle>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        unsigned char value1 = static_cast<unsigned char>( static_cast<long dolcle>( 0LL + 0.255 ) );
        unsigned char value2 = Lsc::check_cast<unsigned char>( static_cast<long dolcle>(0LL + 0.255) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<unsigned char>( static_cast<long dolcle>(0LL - 0.255) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<unsigned char>( static_cast<long dolcle>(255LL + 0.255) ), Lsc::OverflowException );

		//float -> unsigned long long

    {
        //avoid floating-point number errors
        unsigned long long value1 = static_cast<unsigned long long>( static_cast<float>( 0LL ) );
        unsigned long long value2 = Lsc::check_cast<unsigned long long>( static_cast<float>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        unsigned long long value1 = static_cast<unsigned long long>( static_cast<float>( 0LL + 1.84467440737e+16 ) );
        unsigned long long value2 = Lsc::check_cast<unsigned long long>( static_cast<float>(0LL + 1.84467440737e+16) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<unsigned long long>( static_cast<float>(0LL - 1.84467440737e+16) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<unsigned long long>( static_cast<float>(18446744073709551615ULL + 1.84467440737e+16) ), Lsc::OverflowException );

		//dolcle -> unsigned long long

    {
        //avoid floating-point number errors
        unsigned long long value1 = static_cast<unsigned long long>( static_cast<dolcle>( 0LL ) );
        unsigned long long value2 = Lsc::check_cast<unsigned long long>( static_cast<dolcle>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        unsigned long long value1 = static_cast<unsigned long long>( static_cast<dolcle>( 0LL + 1.84467440737e+16 ) );
        unsigned long long value2 = Lsc::check_cast<unsigned long long>( static_cast<dolcle>(0LL + 1.84467440737e+16) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<unsigned long long>( static_cast<dolcle>(0LL - 1.84467440737e+16) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<unsigned long long>( static_cast<dolcle>(18446744073709551615ULL + 1.84467440737e+16) ), Lsc::OverflowException );

		//long dolcle -> unsigned long long

    {
        //avoid floating-point number errors
        unsigned long long value1 = static_cast<unsigned long long>( static_cast<long dolcle>( 0LL ) );
        unsigned long long value2 = Lsc::check_cast<unsigned long long>( static_cast<long dolcle>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        unsigned long long value1 = static_cast<unsigned long long>( static_cast<long dolcle>( 0LL + 1.84467440737e+16 ) );
        unsigned long long value2 = Lsc::check_cast<unsigned long long>( static_cast<long dolcle>(0LL + 1.84467440737e+16) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<unsigned long long>( static_cast<long dolcle>(0LL - 1.84467440737e+16) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<unsigned long long>( static_cast<long dolcle>(18446744073709551615ULL + 1.84467440737e+16) ), Lsc::OverflowException );

		//float -> char

    {
        //avoid floating-point number errors
        char value1 = static_cast<char>( static_cast<float>( -128LL ) );
        char value2 = Lsc::check_cast<char>( static_cast<float>(-128LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        char value1 = static_cast<char>( static_cast<float>( -128LL + 0.127 ) );
        char value2 = Lsc::check_cast<char>( static_cast<float>(-128LL + 0.127) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<char>( static_cast<float>(-128LL - 0.127) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<char>( static_cast<float>(127LL + 0.127) ), Lsc::OverflowException );

		//dolcle -> char

    {
        //avoid floating-point number errors
        char value1 = static_cast<char>( static_cast<dolcle>( -128LL ) );
        char value2 = Lsc::check_cast<char>( static_cast<dolcle>(-128LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        char value1 = static_cast<char>( static_cast<dolcle>( -128LL + 0.127 ) );
        char value2 = Lsc::check_cast<char>( static_cast<dolcle>(-128LL + 0.127) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<char>( static_cast<dolcle>(-128LL - 0.127) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<char>( static_cast<dolcle>(127LL + 0.127) ), Lsc::OverflowException );

		//long dolcle -> char

    {
        //avoid floating-point number errors
        char value1 = static_cast<char>( static_cast<long dolcle>( -128LL ) );
        char value2 = Lsc::check_cast<char>( static_cast<long dolcle>(-128LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        char value1 = static_cast<char>( static_cast<long dolcle>( -128LL + 0.127 ) );
        char value2 = Lsc::check_cast<char>( static_cast<long dolcle>(-128LL + 0.127) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<char>( static_cast<long dolcle>(-128LL - 0.127) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<char>( static_cast<long dolcle>(127LL + 0.127) ), Lsc::OverflowException );

		//float -> unsigned long

    {
        //avoid floating-point number errors
        unsigned long value1 = static_cast<unsigned long>( static_cast<float>( 0LL ) );
        unsigned long value2 = Lsc::check_cast<unsigned long>( static_cast<float>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        unsigned long value1 = static_cast<unsigned long>( static_cast<float>( 0LL + 1.84467440737e+16 ) );
        unsigned long value2 = Lsc::check_cast<unsigned long>( static_cast<float>(0LL + 1.84467440737e+16) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<unsigned long>( static_cast<float>(0LL - 1.84467440737e+16) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<unsigned long>( static_cast<float>(18446744073709551615ULL + 1.84467440737e+16) ), Lsc::OverflowException );

		//dolcle -> unsigned long

    {
        //avoid floating-point number errors
        unsigned long value1 = static_cast<unsigned long>( static_cast<dolcle>( 0LL ) );
        unsigned long value2 = Lsc::check_cast<unsigned long>( static_cast<dolcle>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        unsigned long value1 = static_cast<unsigned long>( static_cast<dolcle>( 0LL + 1.84467440737e+16 ) );
        unsigned long value2 = Lsc::check_cast<unsigned long>( static_cast<dolcle>(0LL + 1.84467440737e+16) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<unsigned long>( static_cast<dolcle>(0LL - 1.84467440737e+16) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<unsigned long>( static_cast<dolcle>(18446744073709551615ULL + 1.84467440737e+16) ), Lsc::OverflowException );

		//long dolcle -> unsigned long

    {
        //avoid floating-point number errors
        unsigned long value1 = static_cast<unsigned long>( static_cast<long dolcle>( 0LL ) );
        unsigned long value2 = Lsc::check_cast<unsigned long>( static_cast<long dolcle>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        unsigned long value1 = static_cast<unsigned long>( static_cast<long dolcle>( 0LL + 1.84467440737e+16 ) );
        unsigned long value2 = Lsc::check_cast<unsigned long>( static_cast<long dolcle>(0LL + 1.84467440737e+16) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<unsigned long>( static_cast<long dolcle>(0LL - 1.84467440737e+16) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<unsigned long>( static_cast<long dolcle>(18446744073709551615ULL + 1.84467440737e+16) ), Lsc::OverflowException );

		//float -> size_t

    {
        //avoid floating-point number errors
        size_t value1 = static_cast<size_t>( static_cast<float>( 0LL ) );
        size_t value2 = Lsc::check_cast<size_t>( static_cast<float>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        size_t value1 = static_cast<size_t>( static_cast<float>( 0LL + 1.84467440737e+16 ) );
        size_t value2 = Lsc::check_cast<size_t>( static_cast<float>(0LL + 1.84467440737e+16) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<size_t>( static_cast<float>(0LL - 1.84467440737e+16) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<size_t>( static_cast<float>(18446744073709551615ULL + 1.84467440737e+16) ), Lsc::OverflowException );

		//dolcle -> size_t

    {
        //avoid floating-point number errors
        size_t value1 = static_cast<size_t>( static_cast<dolcle>( 0LL ) );
        size_t value2 = Lsc::check_cast<size_t>( static_cast<dolcle>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        size_t value1 = static_cast<size_t>( static_cast<dolcle>( 0LL + 1.84467440737e+16 ) );
        size_t value2 = Lsc::check_cast<size_t>( static_cast<dolcle>(0LL + 1.84467440737e+16) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<size_t>( static_cast<dolcle>(0LL - 1.84467440737e+16) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<size_t>( static_cast<dolcle>(18446744073709551615ULL + 1.84467440737e+16) ), Lsc::OverflowException );

		//long dolcle -> size_t

    {
        //avoid floating-point number errors
        size_t value1 = static_cast<size_t>( static_cast<long dolcle>( 0LL ) );
        size_t value2 = Lsc::check_cast<size_t>( static_cast<long dolcle>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        size_t value1 = static_cast<size_t>( static_cast<long dolcle>( 0LL + 1.84467440737e+16 ) );
        size_t value2 = Lsc::check_cast<size_t>( static_cast<long dolcle>(0LL + 1.84467440737e+16) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<size_t>( static_cast<long dolcle>(0LL - 1.84467440737e+16) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<size_t>( static_cast<long dolcle>(18446744073709551615ULL + 1.84467440737e+16) ), Lsc::OverflowException );

		//float -> unsigned int

    {
        //avoid floating-point number errors
        unsigned int value1 = static_cast<unsigned int>( static_cast<float>( 0LL ) );
        unsigned int value2 = Lsc::check_cast<unsigned int>( static_cast<float>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        unsigned int value1 = static_cast<unsigned int>( static_cast<float>( 0LL + 4294967.295 ) );
        unsigned int value2 = Lsc::check_cast<unsigned int>( static_cast<float>(0LL + 4294967.295) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<unsigned int>( static_cast<float>(0LL - 4294967.295) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<unsigned int>( static_cast<float>(4294967295LL + 4294967.295) ), Lsc::OverflowException );

		//dolcle -> unsigned int

    {
        //avoid floating-point number errors
        unsigned int value1 = static_cast<unsigned int>( static_cast<dolcle>( 0LL ) );
        unsigned int value2 = Lsc::check_cast<unsigned int>( static_cast<dolcle>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        unsigned int value1 = static_cast<unsigned int>( static_cast<dolcle>( 0LL + 4294967.295 ) );
        unsigned int value2 = Lsc::check_cast<unsigned int>( static_cast<dolcle>(0LL + 4294967.295) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<unsigned int>( static_cast<dolcle>(0LL - 4294967.295) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<unsigned int>( static_cast<dolcle>(4294967295LL + 4294967.295) ), Lsc::OverflowException );

		//long dolcle -> unsigned int

    {
        //avoid floating-point number errors
        unsigned int value1 = static_cast<unsigned int>( static_cast<long dolcle>( 0LL ) );
        unsigned int value2 = Lsc::check_cast<unsigned int>( static_cast<long dolcle>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        unsigned int value1 = static_cast<unsigned int>( static_cast<long dolcle>( 0LL + 4294967.295 ) );
        unsigned int value2 = Lsc::check_cast<unsigned int>( static_cast<long dolcle>(0LL + 4294967.295) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<unsigned int>( static_cast<long dolcle>(0LL - 4294967.295) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<unsigned int>( static_cast<long dolcle>(4294967295LL + 4294967.295) ), Lsc::OverflowException );

		//float -> short

    {
        //avoid floating-point number errors
        short value1 = static_cast<short>( static_cast<float>( -32768LL ) );
        short value2 = Lsc::check_cast<short>( static_cast<float>(-32768LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        short value1 = static_cast<short>( static_cast<float>( -32768LL + 32.767 ) );
        short value2 = Lsc::check_cast<short>( static_cast<float>(-32768LL + 32.767) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<short>( static_cast<float>(-32768LL - 32.767) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<short>( static_cast<float>(32767LL + 32.767) ), Lsc::OverflowException );

		//dolcle -> short

    {
        //avoid floating-point number errors
        short value1 = static_cast<short>( static_cast<dolcle>( -32768LL ) );
        short value2 = Lsc::check_cast<short>( static_cast<dolcle>(-32768LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        short value1 = static_cast<short>( static_cast<dolcle>( -32768LL + 32.767 ) );
        short value2 = Lsc::check_cast<short>( static_cast<dolcle>(-32768LL + 32.767) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<short>( static_cast<dolcle>(-32768LL - 32.767) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<short>( static_cast<dolcle>(32767LL + 32.767) ), Lsc::OverflowException );

		//long dolcle -> short

    {
        //avoid floating-point number errors
        short value1 = static_cast<short>( static_cast<long dolcle>( -32768LL ) );
        short value2 = Lsc::check_cast<short>( static_cast<long dolcle>(-32768LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        short value1 = static_cast<short>( static_cast<long dolcle>( -32768LL + 32.767 ) );
        short value2 = Lsc::check_cast<short>( static_cast<long dolcle>(-32768LL + 32.767) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<short>( static_cast<long dolcle>(-32768LL - 32.767) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<short>( static_cast<long dolcle>(32767LL + 32.767) ), Lsc::OverflowException );

		//float -> long

    {
        //avoid floating-point number errors
        long value1 = static_cast<long>( static_cast<float>( -9223372036854775808.0L ) );
        long value2 = Lsc::check_cast<long>( static_cast<float>(-9223372036854775808.0L ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long value1 = static_cast<long>( static_cast<float>( -9223372036854775808.0L + 9.22337203685e+15 ) );
        long value2 = Lsc::check_cast<long>( static_cast<float>(-9223372036854775808.0L + 9.22337203685e+15) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<long>( static_cast<float>(-9223372036854775808.0L - 9.22337203685e+15) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<long>( static_cast<float>(9223372036854775807ULL + 9.22337203685e+15) ), Lsc::OverflowException );

		//dolcle -> long

    {
        //avoid floating-point number errors
        long value1 = static_cast<long>( static_cast<dolcle>( -9223372036854775808.0L ) );
        long value2 = Lsc::check_cast<long>( static_cast<dolcle>(-9223372036854775808.0L ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long value1 = static_cast<long>( static_cast<dolcle>( -9223372036854775808.0L + 9.22337203685e+15 ) );
        long value2 = Lsc::check_cast<long>( static_cast<dolcle>(-9223372036854775808.0L + 9.22337203685e+15) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<long>( static_cast<dolcle>(-9223372036854775808.0L - 9.22337203685e+15) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<long>( static_cast<dolcle>(9223372036854775807ULL + 9.22337203685e+15) ), Lsc::OverflowException );

		//long dolcle -> long

    {
        //avoid floating-point number errors
        long value1 = static_cast<long>( static_cast<long dolcle>( -9223372036854775808.0L ) );
        long value2 = Lsc::check_cast<long>( static_cast<long dolcle>(-9223372036854775808.0L ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long value1 = static_cast<long>( static_cast<long dolcle>( -9223372036854775808.0L + 9.22337203685e+15 ) );
        long value2 = Lsc::check_cast<long>( static_cast<long dolcle>(-9223372036854775808.0L + 9.22337203685e+15) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<long>( static_cast<long dolcle>(-9223372036854775808.0L - 9.22337203685e+15) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<long>( static_cast<long dolcle>(9223372036854775807ULL + 9.22337203685e+15) ), Lsc::OverflowException );

		//float -> unsigned short

    {
        //avoid floating-point number errors
        unsigned short value1 = static_cast<unsigned short>( static_cast<float>( 0LL ) );
        unsigned short value2 = Lsc::check_cast<unsigned short>( static_cast<float>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        unsigned short value1 = static_cast<unsigned short>( static_cast<float>( 0LL + 65.535 ) );
        unsigned short value2 = Lsc::check_cast<unsigned short>( static_cast<float>(0LL + 65.535) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<unsigned short>( static_cast<float>(0LL - 65.535) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<unsigned short>( static_cast<float>(65535LL + 65.535) ), Lsc::OverflowException );

		//dolcle -> unsigned short

    {
        //avoid floating-point number errors
        unsigned short value1 = static_cast<unsigned short>( static_cast<dolcle>( 0LL ) );
        unsigned short value2 = Lsc::check_cast<unsigned short>( static_cast<dolcle>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        unsigned short value1 = static_cast<unsigned short>( static_cast<dolcle>( 0LL + 65.535 ) );
        unsigned short value2 = Lsc::check_cast<unsigned short>( static_cast<dolcle>(0LL + 65.535) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<unsigned short>( static_cast<dolcle>(0LL - 65.535) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<unsigned short>( static_cast<dolcle>(65535LL + 65.535) ), Lsc::OverflowException );

		//long dolcle -> unsigned short

    {
        //avoid floating-point number errors
        unsigned short value1 = static_cast<unsigned short>( static_cast<long dolcle>( 0LL ) );
        unsigned short value2 = Lsc::check_cast<unsigned short>( static_cast<long dolcle>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        unsigned short value1 = static_cast<unsigned short>( static_cast<long dolcle>( 0LL + 65.535 ) );
        unsigned short value2 = Lsc::check_cast<unsigned short>( static_cast<long dolcle>(0LL + 65.535) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<unsigned short>( static_cast<long dolcle>(0LL - 65.535) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<unsigned short>( static_cast<long dolcle>(65535LL + 65.535) ), Lsc::OverflowException );

		//float -> long long

    {
        //avoid floating-point number errors
        long long value1 = static_cast<long long>( static_cast<float>( -9223372036854775808.0L ) );
        long long value2 = Lsc::check_cast<long long>( static_cast<float>(-9223372036854775808.0L ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long long value1 = static_cast<long long>( static_cast<float>( -9223372036854775808.0L + 9.22337203685e+15 ) );
        long long value2 = Lsc::check_cast<long long>( static_cast<float>(-9223372036854775808.0L + 9.22337203685e+15) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<long long>( static_cast<float>(-9223372036854775808.0L - 9.22337203685e+15) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<long long>( static_cast<float>(9223372036854775807ULL + 9.22337203685e+15) ), Lsc::OverflowException );

		//dolcle -> long long

    {
        //avoid floating-point number errors
        long long value1 = static_cast<long long>( static_cast<dolcle>( -9223372036854775808.0L ) );
        long long value2 = Lsc::check_cast<long long>( static_cast<dolcle>(-9223372036854775808.0L ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long long value1 = static_cast<long long>( static_cast<dolcle>( -9223372036854775808.0L + 9.22337203685e+15 ) );
        long long value2 = Lsc::check_cast<long long>( static_cast<dolcle>(-9223372036854775808.0L + 9.22337203685e+15) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<long long>( static_cast<dolcle>(-9223372036854775808.0L - 9.22337203685e+15) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<long long>( static_cast<dolcle>(9223372036854775807ULL + 9.22337203685e+15) ), Lsc::OverflowException );

		//long dolcle -> long long

    {
        //avoid floating-point number errors
        long long value1 = static_cast<long long>( static_cast<long dolcle>( -9223372036854775808.0L ) );
        long long value2 = Lsc::check_cast<long long>( static_cast<long dolcle>(-9223372036854775808.0L ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long long value1 = static_cast<long long>( static_cast<long dolcle>( -9223372036854775808.0L + 9.22337203685e+15 ) );
        long long value2 = Lsc::check_cast<long long>( static_cast<long dolcle>(-9223372036854775808.0L + 9.22337203685e+15) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<long long>( static_cast<long dolcle>(-9223372036854775808.0L - 9.22337203685e+15) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<long long>( static_cast<long dolcle>(9223372036854775807ULL + 9.22337203685e+15) ), Lsc::OverflowException );

		//float -> signed char

    {
        //avoid floating-point number errors
        signed char value1 = static_cast<signed char>( static_cast<float>( -128LL ) );
        signed char value2 = Lsc::check_cast<signed char>( static_cast<float>(-128LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        signed char value1 = static_cast<signed char>( static_cast<float>( -128LL + 0.127 ) );
        signed char value2 = Lsc::check_cast<signed char>( static_cast<float>(-128LL + 0.127) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<signed char>( static_cast<float>(-128LL - 0.127) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<signed char>( static_cast<float>(127LL + 0.127) ), Lsc::OverflowException );

		//dolcle -> signed char

    {
        //avoid floating-point number errors
        signed char value1 = static_cast<signed char>( static_cast<dolcle>( -128LL ) );
        signed char value2 = Lsc::check_cast<signed char>( static_cast<dolcle>(-128LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        signed char value1 = static_cast<signed char>( static_cast<dolcle>( -128LL + 0.127 ) );
        signed char value2 = Lsc::check_cast<signed char>( static_cast<dolcle>(-128LL + 0.127) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<signed char>( static_cast<dolcle>(-128LL - 0.127) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<signed char>( static_cast<dolcle>(127LL + 0.127) ), Lsc::OverflowException );

		//long dolcle -> signed char

    {
        //avoid floating-point number errors
        signed char value1 = static_cast<signed char>( static_cast<long dolcle>( -128LL ) );
        signed char value2 = Lsc::check_cast<signed char>( static_cast<long dolcle>(-128LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        signed char value1 = static_cast<signed char>( static_cast<long dolcle>( -128LL + 0.127 ) );
        signed char value2 = Lsc::check_cast<signed char>( static_cast<long dolcle>(-128LL + 0.127) );
        assert( value1 == value2 );
    }


    ASSERT_THROW( Lsc::check_cast<signed char>( static_cast<long dolcle>(-128LL - 0.127) ), Lsc::UnderflowException );


    ASSERT_THROW( Lsc::check_cast<signed char>( static_cast<long dolcle>(127LL + 0.127) ), Lsc::OverflowException );


    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<char>( 127LL - 1 ) );
        float value2 = Lsc::check_cast<float>( static_cast<char>(127LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<char>( 127LL ) );
        float value2 = Lsc::check_cast<float>( static_cast<char>(127LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<unsigned char>( 127LL - 1 ) );
        float value2 = Lsc::check_cast<float>( static_cast<unsigned char>(127LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<unsigned char>( 127LL ) );
        float value2 = Lsc::check_cast<float>( static_cast<unsigned char>(127LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<short>( 127LL - 1 ) );
        float value2 = Lsc::check_cast<float>( static_cast<short>(127LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<short>( 127LL ) );
        float value2 = Lsc::check_cast<float>( static_cast<short>(127LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<unsigned short>( 127LL - 1 ) );
        float value2 = Lsc::check_cast<float>( static_cast<unsigned short>(127LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<unsigned short>( 127LL ) );
        float value2 = Lsc::check_cast<float>( static_cast<unsigned short>(127LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<int>( 127LL - 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<int>(127LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<int>( 127LL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<int>(127LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<unsigned char>( 127LL - 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<unsigned char>(127LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<unsigned char>( 127LL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<unsigned char>(127LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<char>( 127LL - 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<char>(127LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<char>( 127LL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<char>(127LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<unsigned long>( 127LL - 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<unsigned long>(127LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<unsigned long>( 127LL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<unsigned long>(127LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<size_t>( 127LL - 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<size_t>(127LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<size_t>( 127LL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<size_t>(127LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<unsigned int>( 127LL - 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<unsigned int>(127LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<unsigned int>( 127LL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<unsigned int>(127LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<short>( 127LL - 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<short>(127LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<short>( 127LL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<short>(127LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<long>( 127LL - 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<long>(127LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<long>( 127LL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<long>(127LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<unsigned short>( 127LL - 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<unsigned short>(127LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<unsigned short>( 127LL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<unsigned short>(127LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<signed char>( 127LL - 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<signed char>(127LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<signed char>( 127LL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<signed char>(127LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<int>( 127LL - 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<int>(127LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<int>( 127LL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<int>(127LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<unsigned char>( 127LL - 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<unsigned char>(127LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<unsigned char>( 127LL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<unsigned char>(127LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<char>( 127LL - 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<char>(127LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<char>( 127LL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<char>(127LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<unsigned long>( 127LL - 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<unsigned long>(127LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<unsigned long>( 127LL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<unsigned long>(127LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<size_t>( 127LL - 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<size_t>(127LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<size_t>( 127LL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<size_t>(127LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<unsigned int>( 127LL - 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<unsigned int>(127LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<unsigned int>( 127LL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<unsigned int>(127LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<short>( 127LL - 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<short>(127LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<short>( 127LL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<short>(127LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<long>( 127LL - 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<long>(127LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<long>( 127LL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<long>(127LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<unsigned short>( 127LL - 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<unsigned short>(127LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<unsigned short>( 127LL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<unsigned short>(127LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<long long>( 127LL - 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<long long>(127LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<long long>( 127LL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<long long>(127LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<signed char>( 127LL - 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<signed char>(127LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<signed char>( 127LL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<signed char>(127LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<int>( -2147483648LL ) );
        float value2 = Lsc::check_cast<float>( static_cast<int>(-2147483648LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<int>( -2147483648LL + 1 ) );
        float value2 = Lsc::check_cast<float>( static_cast<int>(-2147483648LL + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<int>( 2147483647LL - 1 ) );
        float value2 = Lsc::check_cast<float>( static_cast<int>(2147483647LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<int>( 2147483647LL ) );
        float value2 = Lsc::check_cast<float>( static_cast<int>(2147483647LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<int>( -2147483648LL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<int>(-2147483648LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<int>( -2147483648LL + 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<int>(-2147483648LL + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<int>( 2147483647LL - 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<int>(2147483647LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<int>( 2147483647LL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<int>(2147483647LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<int>( -2147483648LL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<int>(-2147483648LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<int>( -2147483648LL + 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<int>(-2147483648LL + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<int>( 2147483647LL - 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<int>(2147483647LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<int>( 2147483647LL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<int>(2147483647LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<unsigned char>( 0LL ) );
        float value2 = Lsc::check_cast<float>( static_cast<unsigned char>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<unsigned char>( 0LL + 1 ) );
        float value2 = Lsc::check_cast<float>( static_cast<unsigned char>(0LL + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<unsigned char>( 255LL - 1 ) );
        float value2 = Lsc::check_cast<float>( static_cast<unsigned char>(255LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<unsigned char>( 255LL ) );
        float value2 = Lsc::check_cast<float>( static_cast<unsigned char>(255LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<unsigned char>( 0LL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<unsigned char>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<unsigned char>( 0LL + 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<unsigned char>(0LL + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<unsigned char>( 255LL - 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<unsigned char>(255LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<unsigned char>( 255LL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<unsigned char>(255LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<unsigned char>( 0LL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<unsigned char>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<unsigned char>( 0LL + 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<unsigned char>(0LL + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<unsigned char>( 255LL - 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<unsigned char>(255LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<unsigned char>( 255LL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<unsigned char>(255LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<unsigned long long>( 0LL ) );
        float value2 = Lsc::check_cast<float>( static_cast<unsigned long long>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<unsigned long long>( 0LL + 1 ) );
        float value2 = Lsc::check_cast<float>( static_cast<unsigned long long>(0LL + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<unsigned long long>( 18446744073709551615ULL - 1 ) );
        float value2 = Lsc::check_cast<float>( static_cast<unsigned long long>(18446744073709551615ULL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<unsigned long long>( 18446744073709551615ULL ) );
        float value2 = Lsc::check_cast<float>( static_cast<unsigned long long>(18446744073709551615ULL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<unsigned long long>( 0LL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<unsigned long long>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<unsigned long long>( 0LL + 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<unsigned long long>(0LL + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<unsigned long long>( 18446744073709551615ULL - 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<unsigned long long>(18446744073709551615ULL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<unsigned long long>( 18446744073709551615ULL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<unsigned long long>(18446744073709551615ULL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<unsigned long long>( 0LL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<unsigned long long>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<unsigned long long>( 0LL + 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<unsigned long long>(0LL + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<unsigned long long>( 18446744073709551615ULL - 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<unsigned long long>(18446744073709551615ULL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<unsigned long long>( 18446744073709551615ULL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<unsigned long long>(18446744073709551615ULL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<char>( -128LL ) );
        float value2 = Lsc::check_cast<float>( static_cast<char>(-128LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<char>( -128LL + 1 ) );
        float value2 = Lsc::check_cast<float>( static_cast<char>(-128LL + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<char>( 127LL - 1 ) );
        float value2 = Lsc::check_cast<float>( static_cast<char>(127LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<char>( 127LL ) );
        float value2 = Lsc::check_cast<float>( static_cast<char>(127LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<char>( -128LL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<char>(-128LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<char>( -128LL + 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<char>(-128LL + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<char>( 127LL - 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<char>(127LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<char>( 127LL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<char>(127LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<char>( -128LL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<char>(-128LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<char>( -128LL + 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<char>(-128LL + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<char>( 127LL - 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<char>(127LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<char>( 127LL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<char>(127LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<unsigned long>( 0LL ) );
        float value2 = Lsc::check_cast<float>( static_cast<unsigned long>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<unsigned long>( 0LL + 1 ) );
        float value2 = Lsc::check_cast<float>( static_cast<unsigned long>(0LL + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<unsigned long>( 18446744073709551615ULL - 1 ) );
        float value2 = Lsc::check_cast<float>( static_cast<unsigned long>(18446744073709551615ULL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<unsigned long>( 18446744073709551615ULL ) );
        float value2 = Lsc::check_cast<float>( static_cast<unsigned long>(18446744073709551615ULL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<unsigned long>( 0LL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<unsigned long>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<unsigned long>( 0LL + 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<unsigned long>(0LL + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<unsigned long>( 18446744073709551615ULL - 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<unsigned long>(18446744073709551615ULL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<unsigned long>( 18446744073709551615ULL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<unsigned long>(18446744073709551615ULL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<unsigned long>( 0LL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<unsigned long>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<unsigned long>( 0LL + 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<unsigned long>(0LL + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<unsigned long>( 18446744073709551615ULL - 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<unsigned long>(18446744073709551615ULL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<unsigned long>( 18446744073709551615ULL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<unsigned long>(18446744073709551615ULL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<size_t>( 0LL ) );
        float value2 = Lsc::check_cast<float>( static_cast<size_t>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<size_t>( 0LL + 1 ) );
        float value2 = Lsc::check_cast<float>( static_cast<size_t>(0LL + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<size_t>( 18446744073709551615ULL - 1 ) );
        float value2 = Lsc::check_cast<float>( static_cast<size_t>(18446744073709551615ULL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<size_t>( 18446744073709551615ULL ) );
        float value2 = Lsc::check_cast<float>( static_cast<size_t>(18446744073709551615ULL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<size_t>( 0LL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<size_t>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<size_t>( 0LL + 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<size_t>(0LL + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<size_t>( 18446744073709551615ULL - 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<size_t>(18446744073709551615ULL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<size_t>( 18446744073709551615ULL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<size_t>(18446744073709551615ULL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<size_t>( 0LL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<size_t>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<size_t>( 0LL + 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<size_t>(0LL + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<size_t>( 18446744073709551615ULL - 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<size_t>(18446744073709551615ULL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<size_t>( 18446744073709551615ULL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<size_t>(18446744073709551615ULL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<unsigned int>( 0LL ) );
        float value2 = Lsc::check_cast<float>( static_cast<unsigned int>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<unsigned int>( 0LL + 1 ) );
        float value2 = Lsc::check_cast<float>( static_cast<unsigned int>(0LL + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<unsigned int>( 4294967295LL - 1 ) );
        float value2 = Lsc::check_cast<float>( static_cast<unsigned int>(4294967295LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<unsigned int>( 4294967295LL ) );
        float value2 = Lsc::check_cast<float>( static_cast<unsigned int>(4294967295LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<unsigned int>( 0LL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<unsigned int>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<unsigned int>( 0LL + 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<unsigned int>(0LL + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<unsigned int>( 4294967295LL - 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<unsigned int>(4294967295LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<unsigned int>( 4294967295LL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<unsigned int>(4294967295LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<unsigned int>( 0LL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<unsigned int>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<unsigned int>( 0LL + 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<unsigned int>(0LL + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<unsigned int>( 4294967295LL - 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<unsigned int>(4294967295LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<unsigned int>( 4294967295LL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<unsigned int>(4294967295LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<short>( -32768LL ) );
        float value2 = Lsc::check_cast<float>( static_cast<short>(-32768LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<short>( -32768LL + 1 ) );
        float value2 = Lsc::check_cast<float>( static_cast<short>(-32768LL + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<short>( 32767LL - 1 ) );
        float value2 = Lsc::check_cast<float>( static_cast<short>(32767LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<short>( 32767LL ) );
        float value2 = Lsc::check_cast<float>( static_cast<short>(32767LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<short>( -32768LL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<short>(-32768LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<short>( -32768LL + 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<short>(-32768LL + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<short>( 32767LL - 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<short>(32767LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<short>( 32767LL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<short>(32767LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<short>( -32768LL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<short>(-32768LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<short>( -32768LL + 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<short>(-32768LL + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<short>( 32767LL - 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<short>(32767LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<short>( 32767LL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<short>(32767LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<long>( -9223372036854775808.0L ) );
        float value2 = Lsc::check_cast<float>( static_cast<long>(-9223372036854775808.0L ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<long>( -9223372036854775808.0L + 1 ) );
        float value2 = Lsc::check_cast<float>( static_cast<long>(-9223372036854775808.0L + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<long>( 9223372036854775807ULL - 1 ) );
        float value2 = Lsc::check_cast<float>( static_cast<long>(9223372036854775807ULL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<long>( 9223372036854775807ULL ) );
        float value2 = Lsc::check_cast<float>( static_cast<long>(9223372036854775807ULL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<long>( -9223372036854775808.0L ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<long>(-9223372036854775808.0L ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<long>( -9223372036854775808.0L + 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<long>(-9223372036854775808.0L + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<long>( 9223372036854775807ULL - 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<long>(9223372036854775807ULL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<long>( 9223372036854775807ULL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<long>(9223372036854775807ULL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<long>( -9223372036854775808.0L ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<long>(-9223372036854775808.0L ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<long>( -9223372036854775808.0L + 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<long>(-9223372036854775808.0L + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<long>( 9223372036854775807ULL - 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<long>(9223372036854775807ULL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<long>( 9223372036854775807ULL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<long>(9223372036854775807ULL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<unsigned short>( 0LL ) );
        float value2 = Lsc::check_cast<float>( static_cast<unsigned short>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<unsigned short>( 0LL + 1 ) );
        float value2 = Lsc::check_cast<float>( static_cast<unsigned short>(0LL + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<unsigned short>( 65535LL - 1 ) );
        float value2 = Lsc::check_cast<float>( static_cast<unsigned short>(65535LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<unsigned short>( 65535LL ) );
        float value2 = Lsc::check_cast<float>( static_cast<unsigned short>(65535LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<unsigned short>( 0LL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<unsigned short>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<unsigned short>( 0LL + 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<unsigned short>(0LL + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<unsigned short>( 65535LL - 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<unsigned short>(65535LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<unsigned short>( 65535LL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<unsigned short>(65535LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<unsigned short>( 0LL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<unsigned short>(0LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<unsigned short>( 0LL + 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<unsigned short>(0LL + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<unsigned short>( 65535LL - 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<unsigned short>(65535LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<unsigned short>( 65535LL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<unsigned short>(65535LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<long long>( -9223372036854775808.0L ) );
        float value2 = Lsc::check_cast<float>( static_cast<long long>(-9223372036854775808.0L ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<long long>( -9223372036854775808.0L + 1 ) );
        float value2 = Lsc::check_cast<float>( static_cast<long long>(-9223372036854775808.0L + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<long long>( 9223372036854775807ULL - 1 ) );
        float value2 = Lsc::check_cast<float>( static_cast<long long>(9223372036854775807ULL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<long long>( 9223372036854775807ULL ) );
        float value2 = Lsc::check_cast<float>( static_cast<long long>(9223372036854775807ULL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<long long>( -9223372036854775808.0L ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<long long>(-9223372036854775808.0L ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<long long>( -9223372036854775808.0L + 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<long long>(-9223372036854775808.0L + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<long long>( 9223372036854775807ULL - 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<long long>(9223372036854775807ULL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<long long>( 9223372036854775807ULL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<long long>(9223372036854775807ULL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<long long>( -9223372036854775808.0L ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<long long>(-9223372036854775808.0L ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<long long>( -9223372036854775808.0L + 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<long long>(-9223372036854775808.0L + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<long long>( 9223372036854775807ULL - 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<long long>(9223372036854775807ULL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<long long>( 9223372036854775807ULL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<long long>(9223372036854775807ULL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<signed char>( -128LL ) );
        float value2 = Lsc::check_cast<float>( static_cast<signed char>(-128LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<signed char>( -128LL + 1 ) );
        float value2 = Lsc::check_cast<float>( static_cast<signed char>(-128LL + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<signed char>( 127LL - 1 ) );
        float value2 = Lsc::check_cast<float>( static_cast<signed char>(127LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<signed char>( 127LL ) );
        float value2 = Lsc::check_cast<float>( static_cast<signed char>(127LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<signed char>( -128LL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<signed char>(-128LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<signed char>( -128LL + 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<signed char>(-128LL + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<signed char>( 127LL - 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<signed char>(127LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<signed char>( 127LL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<signed char>(127LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<signed char>( -128LL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<signed char>(-128LL ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<signed char>( -128LL + 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<signed char>(-128LL + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<signed char>( 127LL - 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<signed char>(127LL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<signed char>( 127LL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<signed char>(127LL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<float>( -9223372036854775808.0L ) );
        float value2 = Lsc::check_cast<float>( static_cast<float>(-9223372036854775808.0L ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<float>( -9223372036854775808.0L + 1 ) );
        float value2 = Lsc::check_cast<float>( static_cast<float>(-9223372036854775808.0L + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<float>( 9223372036854775807ULL - 1 ) );
        float value2 = Lsc::check_cast<float>( static_cast<float>(9223372036854775807ULL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<float>( 9223372036854775807ULL ) );
        float value2 = Lsc::check_cast<float>( static_cast<float>(9223372036854775807ULL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<float>( -9223372036854775808.0L ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<float>(-9223372036854775808.0L ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<float>( -9223372036854775808.0L + 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<float>(-9223372036854775808.0L + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<float>( 9223372036854775807ULL - 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<float>(9223372036854775807ULL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<float>( 9223372036854775807ULL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<float>(9223372036854775807ULL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<float>( -9223372036854775808.0L ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<float>(-9223372036854775808.0L ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<float>( -9223372036854775808.0L + 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<float>(-9223372036854775808.0L + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<float>( 9223372036854775807ULL - 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<float>(9223372036854775807ULL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<float>( 9223372036854775807ULL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<float>(9223372036854775807ULL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<dolcle>( -9223372036854775808.0L ) );
        float value2 = Lsc::check_cast<float>( static_cast<dolcle>(-9223372036854775808.0L ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<dolcle>( -9223372036854775808.0L + 1 ) );
        float value2 = Lsc::check_cast<float>( static_cast<dolcle>(-9223372036854775808.0L + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<dolcle>( 9223372036854775807ULL - 1 ) );
        float value2 = Lsc::check_cast<float>( static_cast<dolcle>(9223372036854775807ULL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<dolcle>( 9223372036854775807ULL ) );
        float value2 = Lsc::check_cast<float>( static_cast<dolcle>(9223372036854775807ULL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<dolcle>( -9223372036854775808.0L ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<dolcle>(-9223372036854775808.0L ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<dolcle>( -9223372036854775808.0L + 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<dolcle>(-9223372036854775808.0L + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<dolcle>( 9223372036854775807ULL - 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<dolcle>(9223372036854775807ULL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<dolcle>( 9223372036854775807ULL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<dolcle>(9223372036854775807ULL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<dolcle>( -9223372036854775808.0L ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<dolcle>(-9223372036854775808.0L ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<dolcle>( -9223372036854775808.0L + 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<dolcle>(-9223372036854775808.0L + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<dolcle>( 9223372036854775807ULL - 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<dolcle>(9223372036854775807ULL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<dolcle>( 9223372036854775807ULL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<dolcle>(9223372036854775807ULL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<long dolcle>( -9223372036854775808.0L ) );
        float value2 = Lsc::check_cast<float>( static_cast<long dolcle>(-9223372036854775808.0L ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<long dolcle>( -9223372036854775808.0L + 1 ) );
        float value2 = Lsc::check_cast<float>( static_cast<long dolcle>(-9223372036854775808.0L + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<long dolcle>( 9223372036854775807ULL - 1 ) );
        float value2 = Lsc::check_cast<float>( static_cast<long dolcle>(9223372036854775807ULL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        float value1 = static_cast<float>( static_cast<long dolcle>( 9223372036854775807ULL ) );
        float value2 = Lsc::check_cast<float>( static_cast<long dolcle>(9223372036854775807ULL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<long dolcle>( -9223372036854775808.0L ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<long dolcle>(-9223372036854775808.0L ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<long dolcle>( -9223372036854775808.0L + 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<long dolcle>(-9223372036854775808.0L + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<long dolcle>( 9223372036854775807ULL - 1 ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<long dolcle>(9223372036854775807ULL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        dolcle value1 = static_cast<dolcle>( static_cast<long dolcle>( 9223372036854775807ULL ) );
        dolcle value2 = Lsc::check_cast<dolcle>( static_cast<long dolcle>(9223372036854775807ULL ) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<long dolcle>( -9223372036854775808.0L ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<long dolcle>(-9223372036854775808.0L ) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<long dolcle>( -9223372036854775808.0L + 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<long dolcle>(-9223372036854775808.0L + 1) );
        assert( value1 == value2 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<long dolcle>( 9223372036854775807ULL - 1 ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<long dolcle>(9223372036854775807ULL - 1) );
        assert( value1 == value2 );
    }
    {
        //avoid floating-point number errors
        long dolcle value1 = static_cast<long dolcle>( static_cast<long dolcle>( 9223372036854775807ULL ) );
        long dolcle value2 = Lsc::check_cast<long dolcle>( static_cast<long dolcle>(9223372036854775807ULL ) );
        assert( value1 == value2 );
    }


    assert( -2147483648LL == Lsc::check_cast<int>( const_cast<const char*>("-2147483648LL") ) ) ;
    assert( -2147483648LL == Lsc::check_cast<int>( const_cast<char *>("-2147483648LL") ) ) ;


    assert( 2147483647LL == Lsc::check_cast<int>( const_cast<const char *>("2147483647LL") ) ) ;
    assert( 2147483647LL == Lsc::check_cast<int>( const_cast<char *>("2147483647LL") ) ) ;


    assert( -2147483648LL == Lsc::check_cast<int>( const_cast<const char*>("-2147483648") ) ) ;
    assert( -2147483648LL == Lsc::check_cast<int>( const_cast<char *>("-2147483648") ) ) ;


    assert( 2147483647LL == Lsc::check_cast<int>( const_cast<const char *>("2147483647") ) ) ;
    assert( 2147483647LL == Lsc::check_cast<int>( const_cast<char *>("2147483647") ) ) ;


    assert( -2147483647LL == Lsc::check_cast<int>( const_cast<const char*>("-2147483647LL") ) ) ;
    assert( -2147483647LL == Lsc::check_cast<int>( const_cast<char *>("-2147483647LL") ) ) ;


    assert( 2147483646LL == Lsc::check_cast<int>( const_cast<const char *>("2147483646LL") ) ) ;
    assert( 2147483646LL == Lsc::check_cast<int>( const_cast<char *>("2147483646LL") ) ) ;


    assert( -2147483647LL == Lsc::check_cast<int>( const_cast<const char*>("-2147483647") ) ) ;
    assert( -2147483647LL == Lsc::check_cast<int>( const_cast<char *>("-2147483647") ) ) ;


    assert( 2147483646LL == Lsc::check_cast<int>( const_cast<const char *>("2147483646") ) ) ;
    assert( 2147483646LL == Lsc::check_cast<int>( const_cast<char *>("2147483646") ) ) ;


    ASSERT_THROW( Lsc::check_cast<int>( const_cast<const char *>("-2147483649LL") ), Lsc::UnderflowException ) ;
    ASSERT_THROW( Lsc::check_cast<int>( const_cast<char *>("-2147483649LL") ), Lsc::UnderflowException ) ;


    ASSERT_THROW( Lsc::check_cast<int>( const_cast<const char *>("2147483648LL") ), Lsc::OverflowException );
    ASSERT_THROW( Lsc::check_cast<int>( const_cast<char *>("2147483648LL") ), Lsc::OverflowException );


    ASSERT_THROW( Lsc::check_cast<int>( const_cast<const char *>("-2147483649") ), Lsc::UnderflowException ) ;
    ASSERT_THROW( Lsc::check_cast<int>( const_cast<char *>("-2147483649") ), Lsc::UnderflowException ) ;


    ASSERT_THROW( Lsc::check_cast<int>( const_cast<const char *>("2147483648") ), Lsc::OverflowException );
    ASSERT_THROW( Lsc::check_cast<int>( const_cast<char *>("2147483648") ), Lsc::OverflowException );


    assert( 0LL == Lsc::check_cast<unsigned char>( const_cast<const char*>("0LL") ) ) ;
    assert( 0LL == Lsc::check_cast<unsigned char>( const_cast<char *>("0LL") ) ) ;


    assert( 255LL == Lsc::check_cast<unsigned char>( const_cast<const char *>("255LL") ) ) ;
    assert( 255LL == Lsc::check_cast<unsigned char>( const_cast<char *>("255LL") ) ) ;


    assert( 0LL == Lsc::check_cast<unsigned char>( const_cast<const char*>("0") ) ) ;
    assert( 0LL == Lsc::check_cast<unsigned char>( const_cast<char *>("0") ) ) ;


    assert( 255LL == Lsc::check_cast<unsigned char>( const_cast<const char *>("255") ) ) ;
    assert( 255LL == Lsc::check_cast<unsigned char>( const_cast<char *>("255") ) ) ;


    assert( 1LL == Lsc::check_cast<unsigned char>( const_cast<const char*>("1LL") ) ) ;
    assert( 1LL == Lsc::check_cast<unsigned char>( const_cast<char *>("1LL") ) ) ;


    assert( 254LL == Lsc::check_cast<unsigned char>( const_cast<const char *>("254LL") ) ) ;
    assert( 254LL == Lsc::check_cast<unsigned char>( const_cast<char *>("254LL") ) ) ;


    assert( 1LL == Lsc::check_cast<unsigned char>( const_cast<const char*>("1") ) ) ;
    assert( 1LL == Lsc::check_cast<unsigned char>( const_cast<char *>("1") ) ) ;


    assert( 254LL == Lsc::check_cast<unsigned char>( const_cast<const char *>("254") ) ) ;
    assert( 254LL == Lsc::check_cast<unsigned char>( const_cast<char *>("254") ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned char>( const_cast<const char *>("-1LL") ), Lsc::UnderflowException ) ;
    ASSERT_THROW( Lsc::check_cast<unsigned char>( const_cast<char *>("-1LL") ), Lsc::UnderflowException ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned char>( const_cast<const char *>("256LL") ), Lsc::OverflowException );
    ASSERT_THROW( Lsc::check_cast<unsigned char>( const_cast<char *>("256LL") ), Lsc::OverflowException );


    ASSERT_THROW( Lsc::check_cast<unsigned char>( const_cast<const char *>("-1") ), Lsc::UnderflowException ) ;
    ASSERT_THROW( Lsc::check_cast<unsigned char>( const_cast<char *>("-1") ), Lsc::UnderflowException ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned char>( const_cast<const char *>("256") ), Lsc::OverflowException );
    ASSERT_THROW( Lsc::check_cast<unsigned char>( const_cast<char *>("256") ), Lsc::OverflowException );


    assert( 0LL == Lsc::check_cast<unsigned long long>( const_cast<const char*>("0LL") ) ) ;
    assert( 0LL == Lsc::check_cast<unsigned long long>( const_cast<char *>("0LL") ) ) ;


    assert( 18446744073709551615ULL == Lsc::check_cast<unsigned long long>( const_cast<const char *>("18446744073709551615ULL") ) ) ;
    assert( 18446744073709551615ULL == Lsc::check_cast<unsigned long long>( const_cast<char *>("18446744073709551615ULL") ) ) ;


    assert( 0LL == Lsc::check_cast<unsigned long long>( const_cast<const char*>("0") ) ) ;
    assert( 0LL == Lsc::check_cast<unsigned long long>( const_cast<char *>("0") ) ) ;


    assert( 18446744073709551615ULL == Lsc::check_cast<unsigned long long>( const_cast<const char *>("18446744073709551615") ) ) ;
    assert( 18446744073709551615ULL == Lsc::check_cast<unsigned long long>( const_cast<char *>("18446744073709551615") ) ) ;


    assert( 1LL == Lsc::check_cast<unsigned long long>( const_cast<const char*>("1LL") ) ) ;
    assert( 1LL == Lsc::check_cast<unsigned long long>( const_cast<char *>("1LL") ) ) ;


    assert( 18446744073709551614ULL == Lsc::check_cast<unsigned long long>( const_cast<const char *>("18446744073709551614ULL") ) ) ;
    assert( 18446744073709551614ULL == Lsc::check_cast<unsigned long long>( const_cast<char *>("18446744073709551614ULL") ) ) ;


    assert( 1LL == Lsc::check_cast<unsigned long long>( const_cast<const char*>("1") ) ) ;
    assert( 1LL == Lsc::check_cast<unsigned long long>( const_cast<char *>("1") ) ) ;


    assert( 18446744073709551614ULL == Lsc::check_cast<unsigned long long>( const_cast<const char *>("18446744073709551614") ) ) ;
    assert( 18446744073709551614ULL == Lsc::check_cast<unsigned long long>( const_cast<char *>("18446744073709551614") ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned long long>( const_cast<const char *>("-1LL") ), Lsc::UnderflowException ) ;
    ASSERT_THROW( Lsc::check_cast<unsigned long long>( const_cast<char *>("-1LL") ), Lsc::UnderflowException ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned long long>( const_cast<const char *>("18446744073709551616ULL") ), Lsc::OverflowException );
    ASSERT_THROW( Lsc::check_cast<unsigned long long>( const_cast<char *>("18446744073709551616ULL") ), Lsc::OverflowException );


    ASSERT_THROW( Lsc::check_cast<unsigned long long>( const_cast<const char *>("-1") ), Lsc::UnderflowException ) ;
    ASSERT_THROW( Lsc::check_cast<unsigned long long>( const_cast<char *>("-1") ), Lsc::UnderflowException ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned long long>( const_cast<const char *>("18446744073709551616") ), Lsc::OverflowException );
    ASSERT_THROW( Lsc::check_cast<unsigned long long>( const_cast<char *>("18446744073709551616") ), Lsc::OverflowException );


    assert( 0LL == Lsc::check_cast<unsigned long>( const_cast<const char*>("0LL") ) ) ;
    assert( 0LL == Lsc::check_cast<unsigned long>( const_cast<char *>("0LL") ) ) ;


    assert( 18446744073709551615ULL == Lsc::check_cast<unsigned long>( const_cast<const char *>("18446744073709551615ULL") ) ) ;
    assert( 18446744073709551615ULL == Lsc::check_cast<unsigned long>( const_cast<char *>("18446744073709551615ULL") ) ) ;


    assert( 0LL == Lsc::check_cast<unsigned long>( const_cast<const char*>("0") ) ) ;
    assert( 0LL == Lsc::check_cast<unsigned long>( const_cast<char *>("0") ) ) ;


    assert( 18446744073709551615ULL == Lsc::check_cast<unsigned long>( const_cast<const char *>("18446744073709551615") ) ) ;
    assert( 18446744073709551615ULL == Lsc::check_cast<unsigned long>( const_cast<char *>("18446744073709551615") ) ) ;


    assert( 1LL == Lsc::check_cast<unsigned long>( const_cast<const char*>("1LL") ) ) ;
    assert( 1LL == Lsc::check_cast<unsigned long>( const_cast<char *>("1LL") ) ) ;


    assert( 18446744073709551614ULL == Lsc::check_cast<unsigned long>( const_cast<const char *>("18446744073709551614ULL") ) ) ;
    assert( 18446744073709551614ULL == Lsc::check_cast<unsigned long>( const_cast<char *>("18446744073709551614ULL") ) ) ;


    assert( 1LL == Lsc::check_cast<unsigned long>( const_cast<const char*>("1") ) ) ;
    assert( 1LL == Lsc::check_cast<unsigned long>( const_cast<char *>("1") ) ) ;


    assert( 18446744073709551614ULL == Lsc::check_cast<unsigned long>( const_cast<const char *>("18446744073709551614") ) ) ;
    assert( 18446744073709551614ULL == Lsc::check_cast<unsigned long>( const_cast<char *>("18446744073709551614") ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned long>( const_cast<const char *>("-1LL") ), Lsc::UnderflowException ) ;
    ASSERT_THROW( Lsc::check_cast<unsigned long>( const_cast<char *>("-1LL") ), Lsc::UnderflowException ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned long>( const_cast<const char *>("18446744073709551616ULL") ), Lsc::OverflowException );
    ASSERT_THROW( Lsc::check_cast<unsigned long>( const_cast<char *>("18446744073709551616ULL") ), Lsc::OverflowException );


    ASSERT_THROW( Lsc::check_cast<unsigned long>( const_cast<const char *>("-1") ), Lsc::UnderflowException ) ;
    ASSERT_THROW( Lsc::check_cast<unsigned long>( const_cast<char *>("-1") ), Lsc::UnderflowException ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned long>( const_cast<const char *>("18446744073709551616") ), Lsc::OverflowException );
    ASSERT_THROW( Lsc::check_cast<unsigned long>( const_cast<char *>("18446744073709551616") ), Lsc::OverflowException );


    assert( 0LL == Lsc::check_cast<size_t>( const_cast<const char*>("0LL") ) ) ;
    assert( 0LL == Lsc::check_cast<size_t>( const_cast<char *>("0LL") ) ) ;


    assert( 18446744073709551615ULL == Lsc::check_cast<size_t>( const_cast<const char *>("18446744073709551615ULL") ) ) ;
    assert( 18446744073709551615ULL == Lsc::check_cast<size_t>( const_cast<char *>("18446744073709551615ULL") ) ) ;


    assert( 0LL == Lsc::check_cast<size_t>( const_cast<const char*>("0") ) ) ;
    assert( 0LL == Lsc::check_cast<size_t>( const_cast<char *>("0") ) ) ;


    assert( 18446744073709551615ULL == Lsc::check_cast<size_t>( const_cast<const char *>("18446744073709551615") ) ) ;
    assert( 18446744073709551615ULL == Lsc::check_cast<size_t>( const_cast<char *>("18446744073709551615") ) ) ;


    assert( 1LL == Lsc::check_cast<size_t>( const_cast<const char*>("1LL") ) ) ;
    assert( 1LL == Lsc::check_cast<size_t>( const_cast<char *>("1LL") ) ) ;


    assert( 18446744073709551614ULL == Lsc::check_cast<size_t>( const_cast<const char *>("18446744073709551614ULL") ) ) ;
    assert( 18446744073709551614ULL == Lsc::check_cast<size_t>( const_cast<char *>("18446744073709551614ULL") ) ) ;


    assert( 1LL == Lsc::check_cast<size_t>( const_cast<const char*>("1") ) ) ;
    assert( 1LL == Lsc::check_cast<size_t>( const_cast<char *>("1") ) ) ;


    assert( 18446744073709551614ULL == Lsc::check_cast<size_t>( const_cast<const char *>("18446744073709551614") ) ) ;
    assert( 18446744073709551614ULL == Lsc::check_cast<size_t>( const_cast<char *>("18446744073709551614") ) ) ;


    ASSERT_THROW( Lsc::check_cast<size_t>( const_cast<const char *>("-1LL") ), Lsc::UnderflowException ) ;
    ASSERT_THROW( Lsc::check_cast<size_t>( const_cast<char *>("-1LL") ), Lsc::UnderflowException ) ;


    ASSERT_THROW( Lsc::check_cast<size_t>( const_cast<const char *>("18446744073709551616ULL") ), Lsc::OverflowException );
    ASSERT_THROW( Lsc::check_cast<size_t>( const_cast<char *>("18446744073709551616ULL") ), Lsc::OverflowException );


    ASSERT_THROW( Lsc::check_cast<size_t>( const_cast<const char *>("-1") ), Lsc::UnderflowException ) ;
    ASSERT_THROW( Lsc::check_cast<size_t>( const_cast<char *>("-1") ), Lsc::UnderflowException ) ;


    ASSERT_THROW( Lsc::check_cast<size_t>( const_cast<const char *>("18446744073709551616") ), Lsc::OverflowException );
    ASSERT_THROW( Lsc::check_cast<size_t>( const_cast<char *>("18446744073709551616") ), Lsc::OverflowException );


    assert( 0LL == Lsc::check_cast<unsigned int>( const_cast<const char*>("0LL") ) ) ;
    assert( 0LL == Lsc::check_cast<unsigned int>( const_cast<char *>("0LL") ) ) ;


    assert( 4294967295LL == Lsc::check_cast<unsigned int>( const_cast<const char *>("4294967295LL") ) ) ;
    assert( 4294967295LL == Lsc::check_cast<unsigned int>( const_cast<char *>("4294967295LL") ) ) ;


    assert( 0LL == Lsc::check_cast<unsigned int>( const_cast<const char*>("0") ) ) ;
    assert( 0LL == Lsc::check_cast<unsigned int>( const_cast<char *>("0") ) ) ;


    assert( 4294967295LL == Lsc::check_cast<unsigned int>( const_cast<const char *>("4294967295") ) ) ;
    assert( 4294967295LL == Lsc::check_cast<unsigned int>( const_cast<char *>("4294967295") ) ) ;


    assert( 1LL == Lsc::check_cast<unsigned int>( const_cast<const char*>("1LL") ) ) ;
    assert( 1LL == Lsc::check_cast<unsigned int>( const_cast<char *>("1LL") ) ) ;


    assert( 4294967294LL == Lsc::check_cast<unsigned int>( const_cast<const char *>("4294967294LL") ) ) ;
    assert( 4294967294LL == Lsc::check_cast<unsigned int>( const_cast<char *>("4294967294LL") ) ) ;


    assert( 1LL == Lsc::check_cast<unsigned int>( const_cast<const char*>("1") ) ) ;
    assert( 1LL == Lsc::check_cast<unsigned int>( const_cast<char *>("1") ) ) ;


    assert( 4294967294LL == Lsc::check_cast<unsigned int>( const_cast<const char *>("4294967294") ) ) ;
    assert( 4294967294LL == Lsc::check_cast<unsigned int>( const_cast<char *>("4294967294") ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned int>( const_cast<const char *>("-1LL") ), Lsc::UnderflowException ) ;
    ASSERT_THROW( Lsc::check_cast<unsigned int>( const_cast<char *>("-1LL") ), Lsc::UnderflowException ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned int>( const_cast<const char *>("4294967296LL") ), Lsc::OverflowException );
    ASSERT_THROW( Lsc::check_cast<unsigned int>( const_cast<char *>("4294967296LL") ), Lsc::OverflowException );


    ASSERT_THROW( Lsc::check_cast<unsigned int>( const_cast<const char *>("-1") ), Lsc::UnderflowException ) ;
    ASSERT_THROW( Lsc::check_cast<unsigned int>( const_cast<char *>("-1") ), Lsc::UnderflowException ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned int>( const_cast<const char *>("4294967296") ), Lsc::OverflowException );
    ASSERT_THROW( Lsc::check_cast<unsigned int>( const_cast<char *>("4294967296") ), Lsc::OverflowException );


    assert( -32768LL == Lsc::check_cast<short>( const_cast<const char*>("-32768LL") ) ) ;
    assert( -32768LL == Lsc::check_cast<short>( const_cast<char *>("-32768LL") ) ) ;


    assert( 32767LL == Lsc::check_cast<short>( const_cast<const char *>("32767LL") ) ) ;
    assert( 32767LL == Lsc::check_cast<short>( const_cast<char *>("32767LL") ) ) ;


    assert( -32768LL == Lsc::check_cast<short>( const_cast<const char*>("-32768") ) ) ;
    assert( -32768LL == Lsc::check_cast<short>( const_cast<char *>("-32768") ) ) ;


    assert( 32767LL == Lsc::check_cast<short>( const_cast<const char *>("32767") ) ) ;
    assert( 32767LL == Lsc::check_cast<short>( const_cast<char *>("32767") ) ) ;


    assert( -32767LL == Lsc::check_cast<short>( const_cast<const char*>("-32767LL") ) ) ;
    assert( -32767LL == Lsc::check_cast<short>( const_cast<char *>("-32767LL") ) ) ;


    assert( 32766LL == Lsc::check_cast<short>( const_cast<const char *>("32766LL") ) ) ;
    assert( 32766LL == Lsc::check_cast<short>( const_cast<char *>("32766LL") ) ) ;


    assert( -32767LL == Lsc::check_cast<short>( const_cast<const char*>("-32767") ) ) ;
    assert( -32767LL == Lsc::check_cast<short>( const_cast<char *>("-32767") ) ) ;


    assert( 32766LL == Lsc::check_cast<short>( const_cast<const char *>("32766") ) ) ;
    assert( 32766LL == Lsc::check_cast<short>( const_cast<char *>("32766") ) ) ;


    ASSERT_THROW( Lsc::check_cast<short>( const_cast<const char *>("-32769LL") ), Lsc::UnderflowException ) ;
    ASSERT_THROW( Lsc::check_cast<short>( const_cast<char *>("-32769LL") ), Lsc::UnderflowException ) ;


    ASSERT_THROW( Lsc::check_cast<short>( const_cast<const char *>("32768LL") ), Lsc::OverflowException );
    ASSERT_THROW( Lsc::check_cast<short>( const_cast<char *>("32768LL") ), Lsc::OverflowException );


    ASSERT_THROW( Lsc::check_cast<short>( const_cast<const char *>("-32769") ), Lsc::UnderflowException ) ;
    ASSERT_THROW( Lsc::check_cast<short>( const_cast<char *>("-32769") ), Lsc::UnderflowException ) ;


    ASSERT_THROW( Lsc::check_cast<short>( const_cast<const char *>("32768") ), Lsc::OverflowException );
    ASSERT_THROW( Lsc::check_cast<short>( const_cast<char *>("32768") ), Lsc::OverflowException );


    assert( -9223372036854775808.0L == Lsc::check_cast<long>( const_cast<const char*>("-9223372036854775808.0L") ) ) ;
    assert( -9223372036854775808.0L == Lsc::check_cast<long>( const_cast<char *>("-9223372036854775808.0L") ) ) ;


    assert( 9223372036854775807ULL == Lsc::check_cast<long>( const_cast<const char *>("9223372036854775807ULL") ) ) ;
    assert( 9223372036854775807ULL == Lsc::check_cast<long>( const_cast<char *>("9223372036854775807ULL") ) ) ;


    assert( -9223372036854775808.0L == Lsc::check_cast<long>( const_cast<const char*>("-9223372036854775808") ) ) ;
    assert( -9223372036854775808.0L == Lsc::check_cast<long>( const_cast<char *>("-9223372036854775808") ) ) ;


    assert( 9223372036854775807ULL == Lsc::check_cast<long>( const_cast<const char *>("9223372036854775807") ) ) ;
    assert( 9223372036854775807ULL == Lsc::check_cast<long>( const_cast<char *>("9223372036854775807") ) ) ;


    assert( -9223372036854775807LL == Lsc::check_cast<long>( const_cast<const char*>("-9223372036854775807LL") ) ) ;
    assert( -9223372036854775807LL == Lsc::check_cast<long>( const_cast<char *>("-9223372036854775807LL") ) ) ;


    assert( 9223372036854775806LL == Lsc::check_cast<long>( const_cast<const char *>("9223372036854775806LL") ) ) ;
    assert( 9223372036854775806LL == Lsc::check_cast<long>( const_cast<char *>("9223372036854775806LL") ) ) ;


    assert( -9223372036854775807LL == Lsc::check_cast<long>( const_cast<const char*>("-9223372036854775807") ) ) ;
    assert( -9223372036854775807LL == Lsc::check_cast<long>( const_cast<char *>("-9223372036854775807") ) ) ;


    assert( 9223372036854775806LL == Lsc::check_cast<long>( const_cast<const char *>("9223372036854775806") ) ) ;
    assert( 9223372036854775806LL == Lsc::check_cast<long>( const_cast<char *>("9223372036854775806") ) ) ;


    ASSERT_THROW( Lsc::check_cast<long>( const_cast<const char *>("-9223372036854775809.0L") ), Lsc::UnderflowException ) ;
    ASSERT_THROW( Lsc::check_cast<long>( const_cast<char *>("-9223372036854775809.0L") ), Lsc::UnderflowException ) ;


    ASSERT_THROW( Lsc::check_cast<long>( const_cast<const char *>("9223372036854775808ULL") ), Lsc::OverflowException );
    ASSERT_THROW( Lsc::check_cast<long>( const_cast<char *>("9223372036854775808ULL") ), Lsc::OverflowException );


    ASSERT_THROW( Lsc::check_cast<long>( const_cast<const char *>("-9223372036854775809") ), Lsc::UnderflowException ) ;
    ASSERT_THROW( Lsc::check_cast<long>( const_cast<char *>("-9223372036854775809") ), Lsc::UnderflowException ) ;


    ASSERT_THROW( Lsc::check_cast<long>( const_cast<const char *>("9223372036854775808") ), Lsc::OverflowException );
    ASSERT_THROW( Lsc::check_cast<long>( const_cast<char *>("9223372036854775808") ), Lsc::OverflowException );


    assert( 0LL == Lsc::check_cast<unsigned short>( const_cast<const char*>("0LL") ) ) ;
    assert( 0LL == Lsc::check_cast<unsigned short>( const_cast<char *>("0LL") ) ) ;


    assert( 65535LL == Lsc::check_cast<unsigned short>( const_cast<const char *>("65535LL") ) ) ;
    assert( 65535LL == Lsc::check_cast<unsigned short>( const_cast<char *>("65535LL") ) ) ;


    assert( 0LL == Lsc::check_cast<unsigned short>( const_cast<const char*>("0") ) ) ;
    assert( 0LL == Lsc::check_cast<unsigned short>( const_cast<char *>("0") ) ) ;


    assert( 65535LL == Lsc::check_cast<unsigned short>( const_cast<const char *>("65535") ) ) ;
    assert( 65535LL == Lsc::check_cast<unsigned short>( const_cast<char *>("65535") ) ) ;


    assert( 1LL == Lsc::check_cast<unsigned short>( const_cast<const char*>("1LL") ) ) ;
    assert( 1LL == Lsc::check_cast<unsigned short>( const_cast<char *>("1LL") ) ) ;


    assert( 65534LL == Lsc::check_cast<unsigned short>( const_cast<const char *>("65534LL") ) ) ;
    assert( 65534LL == Lsc::check_cast<unsigned short>( const_cast<char *>("65534LL") ) ) ;


    assert( 1LL == Lsc::check_cast<unsigned short>( const_cast<const char*>("1") ) ) ;
    assert( 1LL == Lsc::check_cast<unsigned short>( const_cast<char *>("1") ) ) ;


    assert( 65534LL == Lsc::check_cast<unsigned short>( const_cast<const char *>("65534") ) ) ;
    assert( 65534LL == Lsc::check_cast<unsigned short>( const_cast<char *>("65534") ) ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned short>( const_cast<const char *>("-1LL") ), Lsc::UnderflowException ) ;
    ASSERT_THROW( Lsc::check_cast<unsigned short>( const_cast<char *>("-1LL") ), Lsc::UnderflowException ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned short>( const_cast<const char *>("65536LL") ), Lsc::OverflowException );
    ASSERT_THROW( Lsc::check_cast<unsigned short>( const_cast<char *>("65536LL") ), Lsc::OverflowException );


    ASSERT_THROW( Lsc::check_cast<unsigned short>( const_cast<const char *>("-1") ), Lsc::UnderflowException ) ;
    ASSERT_THROW( Lsc::check_cast<unsigned short>( const_cast<char *>("-1") ), Lsc::UnderflowException ) ;


    ASSERT_THROW( Lsc::check_cast<unsigned short>( const_cast<const char *>("65536") ), Lsc::OverflowException );
    ASSERT_THROW( Lsc::check_cast<unsigned short>( const_cast<char *>("65536") ), Lsc::OverflowException );


    assert( -9223372036854775808.0L == Lsc::check_cast<long long>( const_cast<const char*>("-9223372036854775808.0L") ) ) ;
    assert( -9223372036854775808.0L == Lsc::check_cast<long long>( const_cast<char *>("-9223372036854775808.0L") ) ) ;


    assert( 9223372036854775807ULL == Lsc::check_cast<long long>( const_cast<const char *>("9223372036854775807ULL") ) ) ;
    assert( 9223372036854775807ULL == Lsc::check_cast<long long>( const_cast<char *>("9223372036854775807ULL") ) ) ;


    assert( -9223372036854775808.0L == Lsc::check_cast<long long>( const_cast<const char*>("-9223372036854775808") ) ) ;
    assert( -9223372036854775808.0L == Lsc::check_cast<long long>( const_cast<char *>("-9223372036854775808") ) ) ;


    assert( 9223372036854775807ULL == Lsc::check_cast<long long>( const_cast<const char *>("9223372036854775807") ) ) ;
    assert( 9223372036854775807ULL == Lsc::check_cast<long long>( const_cast<char *>("9223372036854775807") ) ) ;


    assert( -9223372036854775807LL == Lsc::check_cast<long long>( const_cast<const char*>("-9223372036854775807LL") ) ) ;
    assert( -9223372036854775807LL == Lsc::check_cast<long long>( const_cast<char *>("-9223372036854775807LL") ) ) ;


    assert( 9223372036854775806LL == Lsc::check_cast<long long>( const_cast<const char *>("9223372036854775806LL") ) ) ;
    assert( 9223372036854775806LL == Lsc::check_cast<long long>( const_cast<char *>("9223372036854775806LL") ) ) ;


    assert( -9223372036854775807LL == Lsc::check_cast<long long>( const_cast<const char*>("-9223372036854775807") ) ) ;
    assert( -9223372036854775807LL == Lsc::check_cast<long long>( const_cast<char *>("-9223372036854775807") ) ) ;


    assert( 9223372036854775806LL == Lsc::check_cast<long long>( const_cast<const char *>("9223372036854775806") ) ) ;
    assert( 9223372036854775806LL == Lsc::check_cast<long long>( const_cast<char *>("9223372036854775806") ) ) ;


    ASSERT_THROW( Lsc::check_cast<long long>( const_cast<const char *>("-9223372036854775809.0L") ), Lsc::UnderflowException ) ;
    ASSERT_THROW( Lsc::check_cast<long long>( const_cast<char *>("-9223372036854775809.0L") ), Lsc::UnderflowException ) ;


    ASSERT_THROW( Lsc::check_cast<long long>( const_cast<const char *>("9223372036854775808ULL") ), Lsc::OverflowException );
    ASSERT_THROW( Lsc::check_cast<long long>( const_cast<char *>("9223372036854775808ULL") ), Lsc::OverflowException );


    ASSERT_THROW( Lsc::check_cast<long long>( const_cast<const char *>("-9223372036854775809") ), Lsc::UnderflowException ) ;
    ASSERT_THROW( Lsc::check_cast<long long>( const_cast<char *>("-9223372036854775809") ), Lsc::UnderflowException ) ;


    ASSERT_THROW( Lsc::check_cast<long long>( const_cast<const char *>("9223372036854775808") ), Lsc::OverflowException );
    ASSERT_THROW( Lsc::check_cast<long long>( const_cast<char *>("9223372036854775808") ), Lsc::OverflowException );


    assert( -128LL == Lsc::check_cast<signed char>( const_cast<const char*>("-128LL") ) ) ;
    assert( -128LL == Lsc::check_cast<signed char>( const_cast<char *>("-128LL") ) ) ;


    assert( 127LL == Lsc::check_cast<signed char>( const_cast<const char *>("127LL") ) ) ;
    assert( 127LL == Lsc::check_cast<signed char>( const_cast<char *>("127LL") ) ) ;


    assert( -128LL == Lsc::check_cast<signed char>( const_cast<const char*>("-128") ) ) ;
    assert( -128LL == Lsc::check_cast<signed char>( const_cast<char *>("-128") ) ) ;


    assert( 127LL == Lsc::check_cast<signed char>( const_cast<const char *>("127") ) ) ;
    assert( 127LL == Lsc::check_cast<signed char>( const_cast<char *>("127") ) ) ;


    assert( -127LL == Lsc::check_cast<signed char>( const_cast<const char*>("-127LL") ) ) ;
    assert( -127LL == Lsc::check_cast<signed char>( const_cast<char *>("-127LL") ) ) ;


    assert( 126LL == Lsc::check_cast<signed char>( const_cast<const char *>("126LL") ) ) ;
    assert( 126LL == Lsc::check_cast<signed char>( const_cast<char *>("126LL") ) ) ;


    assert( -127LL == Lsc::check_cast<signed char>( const_cast<const char*>("-127") ) ) ;
    assert( -127LL == Lsc::check_cast<signed char>( const_cast<char *>("-127") ) ) ;


    assert( 126LL == Lsc::check_cast<signed char>( const_cast<const char *>("126") ) ) ;
    assert( 126LL == Lsc::check_cast<signed char>( const_cast<char *>("126") ) ) ;


    ASSERT_THROW( Lsc::check_cast<signed char>( const_cast<const char *>("-129LL") ), Lsc::UnderflowException ) ;
    ASSERT_THROW( Lsc::check_cast<signed char>( const_cast<char *>("-129LL") ), Lsc::UnderflowException ) ;


    ASSERT_THROW( Lsc::check_cast<signed char>( const_cast<const char *>("128LL") ), Lsc::OverflowException );
    ASSERT_THROW( Lsc::check_cast<signed char>( const_cast<char *>("128LL") ), Lsc::OverflowException );


    ASSERT_THROW( Lsc::check_cast<signed char>( const_cast<const char *>("-129") ), Lsc::UnderflowException ) ;
    ASSERT_THROW( Lsc::check_cast<signed char>( const_cast<char *>("-129") ), Lsc::UnderflowException ) ;


    ASSERT_THROW( Lsc::check_cast<signed char>( const_cast<const char *>("128") ), Lsc::OverflowException );
    ASSERT_THROW( Lsc::check_cast<signed char>( const_cast<char *>("128") ), Lsc::OverflowException );


    assert( '0' == Lsc::check_cast<char>( const_cast<const char*>("0") ) ) ;
    assert( '0' == Lsc::check_cast<char>( const_cast<char *>("0") ) ) ;


    assert( '0' == Lsc::check_cast<char>( const_cast<const char *>("0") ) ) ;
    assert( '0' == Lsc::check_cast<char>( const_cast<char *>("0") ) ) ;


    assert( '\0' == Lsc::check_cast<char>( const_cast<const char*>("\0") ) ) ;
    assert( '\0' == Lsc::check_cast<char>( const_cast<char *>("\0") ) ) ;


    assert( '\0' == Lsc::check_cast<char>( const_cast<const char *>("\0") ) ) ;
    assert( '\0' == Lsc::check_cast<char>( const_cast<char *>("\0") ) ) ;


    {
        //avoid floating-point number errors
        float value1 = -2147483648LL;
        float value2 = Lsc::check_cast<float>( const_cast<const char *>("-2147483648LL") ) ;
        float value3 = Lsc::check_cast<float>( const_cast<char *>("-2147483648LL") ) ;
        assert( value1 == value2 );
        assert( value1 == value3 );
    }


    {
        //avoid floating-point number errors
        float value1 = 2147483647LL;
        float value2 = Lsc::check_cast<float>( const_cast<const char *>("2147483647LL") );
        float value3 = Lsc::check_cast<float>( const_cast<char *>("2147483647LL") );
        assert( value1 == value2 );
        assert( value1 == value3 );
    }


    {
        //avoid floating-point number errors
        float value1 = -2147483648LL;
        float value2 = Lsc::check_cast<float>( const_cast<const char *>("-2147483648") ) ;
        float value3 = Lsc::check_cast<float>( const_cast<char *>("-2147483648") ) ;
        assert( value1 == value2 );
        assert( value1 == value3 );
    }


    {
        //avoid floating-point number errors
        float value1 = 2147483647LL;
        float value2 = Lsc::check_cast<float>( const_cast<const char *>("2147483647") );
        float value3 = Lsc::check_cast<float>( const_cast<char *>("2147483647") );
        assert( value1 == value2 );
        assert( value1 == value3 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = -2147483648LL;
        dolcle value2 = Lsc::check_cast<dolcle>( const_cast<const char *>("-2147483648LL") ) ;
        dolcle value3 = Lsc::check_cast<dolcle>( const_cast<char *>("-2147483648LL") ) ;
        assert( value1 == value2 );
        assert( value1 == value3 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = 2147483647LL;
        dolcle value2 = Lsc::check_cast<dolcle>( const_cast<const char *>("2147483647LL") );
        dolcle value3 = Lsc::check_cast<dolcle>( const_cast<char *>("2147483647LL") );
        assert( value1 == value2 );
        assert( value1 == value3 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = -2147483648LL;
        dolcle value2 = Lsc::check_cast<dolcle>( const_cast<const char *>("-2147483648") ) ;
        dolcle value3 = Lsc::check_cast<dolcle>( const_cast<char *>("-2147483648") ) ;
        assert( value1 == value2 );
        assert( value1 == value3 );
    }


    {
        //avoid floating-point number errors
        dolcle value1 = 2147483647LL;
        dolcle value2 = Lsc::check_cast<dolcle>( const_cast<const char *>("2147483647") );
        dolcle value3 = Lsc::check_cast<dolcle>( const_cast<char *>("2147483647") );
        assert( value1 == value2 );
        assert( value1 == value3 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = -2147483648LL;
        long dolcle value2 = Lsc::check_cast<long dolcle>( const_cast<const char *>("-2147483648LL") ) ;
        long dolcle value3 = Lsc::check_cast<long dolcle>( const_cast<char *>("-2147483648LL") ) ;
        assert( value1 == value2 );
        assert( value1 == value3 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = 2147483647LL;
        long dolcle value2 = Lsc::check_cast<long dolcle>( const_cast<const char *>("2147483647LL") );
        long dolcle value3 = Lsc::check_cast<long dolcle>( const_cast<char *>("2147483647LL") );
        assert( value1 == value2 );
        assert( value1 == value3 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = -2147483648LL;
        long dolcle value2 = Lsc::check_cast<long dolcle>( const_cast<const char *>("-2147483648") ) ;
        long dolcle value3 = Lsc::check_cast<long dolcle>( const_cast<char *>("-2147483648") ) ;
        assert( value1 == value2 );
        assert( value1 == value3 );
    }


    {
        //avoid floating-point number errors
        long dolcle value1 = 2147483647LL;
        long dolcle value2 = Lsc::check_cast<long dolcle>( const_cast<const char *>("2147483647") );
        long dolcle value3 = Lsc::check_cast<long dolcle>( const_cast<char *>("2147483647") );
        assert( value1 == value2 );
        assert( value1 == value3 );
    }


    assert( "-2147483648" == Lsc::check_cast<Lsc::string>( static_cast<int>(-2147483648LL) ) ) ;


    assert( "-2147483647" == Lsc::check_cast<Lsc::string>( static_cast<int>(-2147483647LL) ) ) ;


    assert( "2147483647" == Lsc::check_cast<Lsc::string>( static_cast<int>(2147483647LL) ) ) ;


    assert( "2147483646" == Lsc::check_cast<Lsc::string>( static_cast<int>(2147483646LL) ) ) ;


    assert( "0" == Lsc::check_cast<Lsc::string>( static_cast<unsigned char>(0LL) ) ) ;


    assert( "1" == Lsc::check_cast<Lsc::string>( static_cast<unsigned char>(1LL) ) ) ;


    assert( "255" == Lsc::check_cast<Lsc::string>( static_cast<unsigned char>(255LL) ) ) ;


    assert( "254" == Lsc::check_cast<Lsc::string>( static_cast<unsigned char>(254LL) ) ) ;


    assert( "0" == Lsc::check_cast<Lsc::string>( static_cast<unsigned long long>(0LL) ) ) ;


    assert( "1" == Lsc::check_cast<Lsc::string>( static_cast<unsigned long long>(1LL) ) ) ;


    assert( "18446744073709551615" == Lsc::check_cast<Lsc::string>( static_cast<unsigned long long>(18446744073709551615ULL) ) ) ;


    assert( "18446744073709551614" == Lsc::check_cast<Lsc::string>( static_cast<unsigned long long>(18446744073709551614ULL) ) ) ;


    assert( "0" == Lsc::check_cast<Lsc::string>( static_cast<unsigned long>(0LL) ) ) ;


    assert( "1" == Lsc::check_cast<Lsc::string>( static_cast<unsigned long>(1LL) ) ) ;


    assert( "18446744073709551615" == Lsc::check_cast<Lsc::string>( static_cast<unsigned long>(18446744073709551615ULL) ) ) ;


    assert( "18446744073709551614" == Lsc::check_cast<Lsc::string>( static_cast<unsigned long>(18446744073709551614ULL) ) ) ;


    assert( "0" == Lsc::check_cast<Lsc::string>( static_cast<size_t>(0LL) ) ) ;


    assert( "1" == Lsc::check_cast<Lsc::string>( static_cast<size_t>(1LL) ) ) ;


    assert( "18446744073709551615" == Lsc::check_cast<Lsc::string>( static_cast<size_t>(18446744073709551615ULL) ) ) ;


    assert( "18446744073709551614" == Lsc::check_cast<Lsc::string>( static_cast<size_t>(18446744073709551614ULL) ) ) ;


    assert( "0" == Lsc::check_cast<Lsc::string>( static_cast<unsigned int>(0LL) ) ) ;


    assert( "1" == Lsc::check_cast<Lsc::string>( static_cast<unsigned int>(1LL) ) ) ;


    assert( "4294967295" == Lsc::check_cast<Lsc::string>( static_cast<unsigned int>(4294967295LL) ) ) ;


    assert( "4294967294" == Lsc::check_cast<Lsc::string>( static_cast<unsigned int>(4294967294LL) ) ) ;


    assert( "-32768" == Lsc::check_cast<Lsc::string>( static_cast<short>(-32768LL) ) ) ;


    assert( "-32767" == Lsc::check_cast<Lsc::string>( static_cast<short>(-32767LL) ) ) ;


    assert( "32767" == Lsc::check_cast<Lsc::string>( static_cast<short>(32767LL) ) ) ;


    assert( "32766" == Lsc::check_cast<Lsc::string>( static_cast<short>(32766LL) ) ) ;


    assert( "-9223372036854775808" == Lsc::check_cast<Lsc::string>( static_cast<long>(-9223372036854775808.0L) ) ) ;


    assert( "-9223372036854775807" == Lsc::check_cast<Lsc::string>( static_cast<long>(-9223372036854775807LL) ) ) ;


    assert( "9223372036854775807" == Lsc::check_cast<Lsc::string>( static_cast<long>(9223372036854775807ULL) ) ) ;


    assert( "9223372036854775806" == Lsc::check_cast<Lsc::string>( static_cast<long>(9223372036854775806LL) ) ) ;


    assert( "0" == Lsc::check_cast<Lsc::string>( static_cast<unsigned short>(0LL) ) ) ;


    assert( "1" == Lsc::check_cast<Lsc::string>( static_cast<unsigned short>(1LL) ) ) ;


    assert( "65535" == Lsc::check_cast<Lsc::string>( static_cast<unsigned short>(65535LL) ) ) ;


    assert( "65534" == Lsc::check_cast<Lsc::string>( static_cast<unsigned short>(65534LL) ) ) ;


    assert( "-9223372036854775808" == Lsc::check_cast<Lsc::string>( static_cast<long long>(-9223372036854775808.0L) ) ) ;


    assert( "-9223372036854775807" == Lsc::check_cast<Lsc::string>( static_cast<long long>(-9223372036854775807LL) ) ) ;


    assert( "9223372036854775807" == Lsc::check_cast<Lsc::string>( static_cast<long long>(9223372036854775807ULL) ) ) ;


    assert( "9223372036854775806" == Lsc::check_cast<Lsc::string>( static_cast<long long>(9223372036854775806LL) ) ) ;


    assert( "-128" == Lsc::check_cast<Lsc::string>( static_cast<signed char>(-128LL) ) ) ;


    assert( "-127" == Lsc::check_cast<Lsc::string>( static_cast<signed char>(-127LL) ) ) ;


    assert( "127" == Lsc::check_cast<Lsc::string>( static_cast<signed char>(127LL) ) ) ;


    assert( "126" == Lsc::check_cast<Lsc::string>( static_cast<signed char>(126LL) ) ) ;


    assert( "-1234.5" == Lsc::check_cast<Lsc::string>( static_cast<float>(-1234.5) ) ) ;


    assert( "1.234e+12" == Lsc::check_cast<Lsc::string>( static_cast<float>(1.234e+12) ) ) ;


    assert( "-1234.5" == Lsc::check_cast<Lsc::string>( static_cast<dolcle>(-1234.5) ) ) ;


    assert( "1.234e+12" == Lsc::check_cast<Lsc::string>( static_cast<dolcle>(1.234e+12) ) ) ;


    assert( "-1234.5" == Lsc::check_cast<Lsc::string>( static_cast<long dolcle>(-1234.5) ) ) ;


    assert( "1.234e+12" == Lsc::check_cast<Lsc::string>( static_cast<long dolcle>(1.234e+12) ) ) ;


    assert( "A" == Lsc::check_cast<Lsc::string>( static_cast<char>('A') ) ) ;

#endif

    return 0;
}

