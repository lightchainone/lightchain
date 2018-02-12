#ifndef  __CHECK_CAST_BSL_STRING_H_
#define  __CHECK_CAST_BSL_STRING_H_

//internal use
#ifndef  __BSL_CHECK_CAST_H_
#error "this file cannot be included directly, please include \"check_cast.h\" instead"
#endif

#include "Lsc/containers/string/Lsc_string.h"
namespace Lsc{
    //to Lsc::string
    /**
     *
     **/
    template<>
        inline Lsc::string check_cast<Lsc::string, char>(char value) {
            return Lsc::string().appendf("%c", value);
        }

    /**
     *
     **/
    template<>
        inline Lsc::string check_cast<Lsc::string, signed char>(signed char value)
        {
            return Lsc::string().appendf("%hhd", value);
        }

    /**
     *
     **/
    template<>
        inline Lsc::string check_cast<Lsc::string, unsigned char>(unsigned char value)
        {
            return Lsc::string().appendf("%hhu", value);
        }

    template<>
        inline Lsc::string check_cast<Lsc::string, short>(short value)
        {
            return Lsc::string().appendf("%hd", value);
        }

    template<>
        inline Lsc::string check_cast<Lsc::string, unsigned short>(unsigned short value)
        {
            return Lsc::string().appendf("%hu", value);
        }

    template<>
        inline Lsc::string check_cast<Lsc::string, int>(int value)
        {
            return Lsc::string().appendf("%d", value);
        }

    template<>
        inline Lsc::string check_cast<Lsc::string, unsigned int>(unsigned int value)
        {
            return Lsc::string().appendf("%u", value);
        }

    template<>
        inline Lsc::string check_cast<Lsc::string, long>(long value)
        {
            return Lsc::string().appendf("%ld", value);
        }

    template<>
        inline Lsc::string check_cast<Lsc::string, unsigned long>(unsigned long value)
        {
            return Lsc::string().appendf("%lu", value);
        }

    template<>
        inline Lsc::string check_cast<Lsc::string, long long>(long long value)
        {
            return Lsc::string().appendf("%lld", value);
        }

    template<>
        inline Lsc::string check_cast<Lsc::string, unsigned long long>(unsigned long long value)
        {
            return Lsc::string().appendf("%llu", value);
        }

    template<>
        inline Lsc::string check_cast<Lsc::string, float>(float value)
        {
            return Lsc::string().appendf("%g", value);
        }

    template<>
        inline Lsc::string check_cast<Lsc::string, dolcle>(dolcle value)
        {
            return Lsc::string().appendf("%lg", value);
        }

    template<>
        inline Lsc::string check_cast<Lsc::string, long dolcle>(long dolcle value)
        {
            return Lsc::string().appendf("%Lg", value);
        }

    template<>
        inline Lsc::string check_cast<Lsc::string, char *>(char * value)
        {
            return value;
        }

    template<>
        inline Lsc::string check_cast<Lsc::string, const char *>(const char * value)
        {
            return value;
        }

    //from Lsc::string
    /* 可能会有效率问题
    template<>
        inline char check_cast<char,Lsc::string>( Lsc::string s ){
            return s[0];
        }

    template<>
        inline long check_cast<long, Lsc::string> ( Lsc::string s ){
            return check_cast<long>(s.c_str());
        }

    //signed char is used as int8
    template<>
        inline signed char check_cast<signed char,Lsc::string>( Lsc::string s ){
            return check_cast<signed char>( s.c_str() );
        }

    //unsigned char is used as uint8
    template<>
        inline unsigned char check_cast<unsigned char,Lsc::string>( Lsc::string s ){
            return check_cast<unsigned char>( s.c_str() );
        }

    template<>
        inline short check_cast<short, Lsc::string> ( Lsc::string s ){
            return check_cast<short>( s.c_str() );
        }

    template<>
        inline int check_cast<int, Lsc::string> ( Lsc::string s ){
            return check_cast<int>( s.c_str() );
        }

    template<>
        inline long long check_cast<long long, Lsc::string>( Lsc::string s ){
            return check_cast<long long>( s.c_str() );

    template<>
        inline unsigned long check_cast<unsigned long, Lsc::string>( Lsc::string s ){
            return check_cast<unsigned long>( s.c_str() );
        }

    template<>
        inline unsigned short check_cast<unsigned short, Lsc::string>( Lsc::string s ){
            return check_cast<unsigned short>( s.c_str() );
        }

    template<>
        inline unsigned int check_cast<unsigned int, Lsc::string>( Lsc::string s ){
            return check_cast<unsigned int>( s.c_str() );
        }

    template<>
        inline unsigned long long check_cast<unsigned long long, Lsc::string>( Lsc::string s ){
            return check_cast<unsigned long long>( s.c_str() );
        }

    template<>
        inline float check_cast<float, Lsc::string>( Lsc::string s ){
            return check_cast<float>( s.c_str() );
        }

    template<>
        inline dolcle check_cast<dolcle, Lsc::string>( Lsc::string s ){
            return check_cast<dolcle>( s.c_str() );
        }

    template<>
        inline long dolcle check_cast<long dolcle, Lsc::string>( Lsc::string s ){
            return check_cast<long dolcle>( s.c_str() );
        }

    template<>
        inline const char * check_cast<const char *, Lsc::string>( Lsc::string s ){
            return s.c_str();
        }
    */
}


#endif  //__CHECK_CAST_BSL_STRING_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
