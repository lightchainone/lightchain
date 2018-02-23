
#ifndef  __BSL_TEST_EXCEPTION_H_
#define  __BSL_TEST_EXCEPTION_H_
#include <cxxtest/TestSuite.h>
#include "Lsc/exception/Lsc_exception.h"
#include "Lsc_test_exception_prepare.h_"

class test_exception : plclic CxxTest::TestSuite {
plclic:


    void test_UnknownException_info_a(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_DEFAULT).push(true);
        }catch( Lsc::UnknownException& ex ){
            const char * what = "true";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_b(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_DEFAULT).push(true);
        }catch( Lsc::Exception& ex ){
            const char * what = "true";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_c(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_SILENT).push(true);
        }catch( Lsc::UnknownException& ex ){
            const char * what = "true";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_d(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_SILENT).push(true);
        }catch( Lsc::Exception& ex ){
            const char * what = "true";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_e(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_DEFAULT).push(false);
        }catch( Lsc::UnknownException& ex ){
            const char * what = "false";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_f(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_DEFAULT).push(false);
        }catch( Lsc::Exception& ex ){
            const char * what = "false";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_g(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_SILENT).push(false);
        }catch( Lsc::UnknownException& ex ){
            const char * what = "false";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_h(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_SILENT).push(false);
        }catch( Lsc::Exception& ex ){
            const char * what = "false";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_i(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_DEFAULT).push(true).push(false).push(true);
        }catch( Lsc::UnknownException& ex ){
            const char * what = "truefalsetrue";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_j(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_DEFAULT).push(true).push(false).push(true);
        }catch( Lsc::Exception& ex ){
            const char * what = "truefalsetrue";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_ba(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_SILENT).push(true).push(false).push(true);
        }catch( Lsc::UnknownException& ex ){
            const char * what = "truefalsetrue";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_bb(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_SILENT).push(true).push(false).push(true);
        }catch( Lsc::Exception& ex ){
            const char * what = "truefalsetrue";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_bc(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_DEFAULT).push(123);
        }catch( Lsc::UnknownException& ex ){
            const char * what = "123";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_bd(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_DEFAULT).push(123);
        }catch( Lsc::Exception& ex ){
            const char * what = "123";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_be(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_SILENT).push(123);
        }catch( Lsc::UnknownException& ex ){
            const char * what = "123";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_bf(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_SILENT).push(123);
        }catch( Lsc::Exception& ex ){
            const char * what = "123";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_bg(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_DEFAULT).push(123).push(-456).push(+789).push(0);
        }catch( Lsc::UnknownException& ex ){
            const char * what = "123-4567890";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_bh(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_DEFAULT).push(123).push(-456).push(+789).push(0);
        }catch( Lsc::Exception& ex ){
            const char * what = "123-4567890";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_bi(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_SILENT).push(123).push(-456).push(+789).push(0);
        }catch( Lsc::UnknownException& ex ){
            const char * what = "123-4567890";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_bj(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_SILENT).push(123).push(-456).push(+789).push(0);
        }catch( Lsc::Exception& ex ){
            const char * what = "123-4567890";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_ca(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_DEFAULT).push(123.456).push(-1.2e+100).push(3.14e-50);
        }catch( Lsc::UnknownException& ex ){
            const char * what = "123.456-1.2e+1003.14e-50";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_cb(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_DEFAULT).push(123.456).push(-1.2e+100).push(3.14e-50);
        }catch( Lsc::Exception& ex ){
            const char * what = "123.456-1.2e+1003.14e-50";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_cc(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_SILENT).push(123.456).push(-1.2e+100).push(3.14e-50);
        }catch( Lsc::UnknownException& ex ){
            const char * what = "123.456-1.2e+1003.14e-50";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_cd(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_SILENT).push(123.456).push(-1.2e+100).push(3.14e-50);
        }catch( Lsc::Exception& ex ){
            const char * what = "123.456-1.2e+1003.14e-50";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_ce(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_DEFAULT).push('a').push(char(0)).push('b').push(4,'x');
        }catch( Lsc::UnknownException& ex ){
            const char * what = "abxxxx";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_cf(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_DEFAULT).push('a').push(char(0)).push('b').push(4,'x');
        }catch( Lsc::Exception& ex ){
            const char * what = "abxxxx";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_cg(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_SILENT).push('a').push(char(0)).push('b').push(4,'x');
        }catch( Lsc::UnknownException& ex ){
            const char * what = "abxxxx";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_ch(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_SILENT).push('a').push(char(0)).push('b').push(4,'x');
        }catch( Lsc::Exception& ex ){
            const char * what = "abxxxx";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_ci(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_DEFAULT).push("abc");
        }catch( Lsc::UnknownException& ex ){
            const char * what = "abc";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_cj(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_DEFAULT).push("abc");
        }catch( Lsc::Exception& ex ){
            const char * what = "abc";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_da(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_SILENT).push("abc");
        }catch( Lsc::UnknownException& ex ){
            const char * what = "abc";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_db(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_SILENT).push("abc");
        }catch( Lsc::Exception& ex ){
            const char * what = "abc";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_dc(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_DEFAULT).push("");
        }catch( Lsc::UnknownException& ex ){
            const char * what = "";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_dd(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_DEFAULT).push("");
        }catch( Lsc::Exception& ex ){
            const char * what = "";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_de(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_SILENT).push("");
        }catch( Lsc::UnknownException& ex ){
            const char * what = "";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_df(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_SILENT).push("");
        }catch( Lsc::Exception& ex ){
            const char * what = "";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_dg(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_DEFAULT).push("abc").push("").push("def");
        }catch( Lsc::UnknownException& ex ){
            const char * what = "abcdef";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_dh(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_DEFAULT).push("abc").push("").push("def");
        }catch( Lsc::Exception& ex ){
            const char * what = "abcdef";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_di(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_SILENT).push("abc").push("").push("def");
        }catch( Lsc::UnknownException& ex ){
            const char * what = "abcdef";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_dj(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_SILENT).push("abc").push("").push("def");
        }catch( Lsc::Exception& ex ){
            const char * what = "abcdef";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_ea(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_DEFAULT).pushf("%d%s",123,"abc").push(true);
        }catch( Lsc::UnknownException& ex ){
            const char * what = "123abctrue";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_eb(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_DEFAULT).pushf("%d%s",123,"abc").push(true);
        }catch( Lsc::Exception& ex ){
            const char * what = "123abctrue";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_ec(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_SILENT).pushf("%d%s",123,"abc").push(true);
        }catch( Lsc::UnknownException& ex ){
            const char * what = "123abctrue";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_ed(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_SILENT).pushf("%d%s",123,"abc").push(true);
        }catch( Lsc::Exception& ex ){
            const char * what = "123abctrue";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_ee(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_DEFAULT<<true;
        }catch( Lsc::UnknownException& ex ){
            const char * what = "true";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_ef(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_DEFAULT<<true;
        }catch( Lsc::Exception& ex ){
            const char * what = "true";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_eg(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_SILENT<<true;
        }catch( Lsc::UnknownException& ex ){
            const char * what = "true";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_eh(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_SILENT<<true;
        }catch( Lsc::Exception& ex ){
            const char * what = "true";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_ei(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_DEFAULT<<false;
        }catch( Lsc::UnknownException& ex ){
            const char * what = "false";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_ej(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_DEFAULT<<false;
        }catch( Lsc::Exception& ex ){
            const char * what = "false";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_fa(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_SILENT<<false;
        }catch( Lsc::UnknownException& ex ){
            const char * what = "false";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_fb(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_SILENT<<false;
        }catch( Lsc::Exception& ex ){
            const char * what = "false";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_fc(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_DEFAULT<<true<<false<<true;
        }catch( Lsc::UnknownException& ex ){
            const char * what = "truefalsetrue";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_fd(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_DEFAULT<<true<<false<<true;
        }catch( Lsc::Exception& ex ){
            const char * what = "truefalsetrue";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_fe(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_SILENT<<true<<false<<true;
        }catch( Lsc::UnknownException& ex ){
            const char * what = "truefalsetrue";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_ff(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_SILENT<<true<<false<<true;
        }catch( Lsc::Exception& ex ){
            const char * what = "truefalsetrue";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_fg(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_DEFAULT<<123;
        }catch( Lsc::UnknownException& ex ){
            const char * what = "123";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_fh(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_DEFAULT<<123;
        }catch( Lsc::Exception& ex ){
            const char * what = "123";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_fi(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_SILENT<<123;
        }catch( Lsc::UnknownException& ex ){
            const char * what = "123";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_fj(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_SILENT<<123;
        }catch( Lsc::Exception& ex ){
            const char * what = "123";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_ga(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_DEFAULT<<123<<-456<<+789<<0;
        }catch( Lsc::UnknownException& ex ){
            const char * what = "123-4567890";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_gb(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_DEFAULT<<123<<-456<<+789<<0;
        }catch( Lsc::Exception& ex ){
            const char * what = "123-4567890";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_gc(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_SILENT<<123<<-456<<+789<<0;
        }catch( Lsc::UnknownException& ex ){
            const char * what = "123-4567890";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_gd(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_SILENT<<123<<-456<<+789<<0;
        }catch( Lsc::Exception& ex ){
            const char * what = "123-4567890";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_ge(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_DEFAULT<<123.456<<-1.2e+100<<3.14e-50;
        }catch( Lsc::UnknownException& ex ){
            const char * what = "123.456-1.2e+1003.14e-50";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_gf(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_DEFAULT<<123.456<<-1.2e+100<<3.14e-50;
        }catch( Lsc::Exception& ex ){
            const char * what = "123.456-1.2e+1003.14e-50";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_gg(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_SILENT<<123.456<<-1.2e+100<<3.14e-50;
        }catch( Lsc::UnknownException& ex ){
            const char * what = "123.456-1.2e+1003.14e-50";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_gh(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_SILENT<<123.456<<-1.2e+100<<3.14e-50;
        }catch( Lsc::Exception& ex ){
            const char * what = "123.456-1.2e+1003.14e-50";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_gi(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_DEFAULT<<'a'<<char(0)<<'b';
        }catch( Lsc::UnknownException& ex ){
            const char * what = "ab";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_gj(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_DEFAULT<<'a'<<char(0)<<'b';
        }catch( Lsc::Exception& ex ){
            const char * what = "ab";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_ha(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_SILENT<<'a'<<char(0)<<'b';
        }catch( Lsc::UnknownException& ex ){
            const char * what = "ab";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_hb(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_SILENT<<'a'<<char(0)<<'b';
        }catch( Lsc::Exception& ex ){
            const char * what = "ab";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_hc(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_DEFAULT<<"abc";
        }catch( Lsc::UnknownException& ex ){
            const char * what = "abc";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_hd(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_DEFAULT<<"abc";
        }catch( Lsc::Exception& ex ){
            const char * what = "abc";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_he(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_SILENT<<"abc";
        }catch( Lsc::UnknownException& ex ){
            const char * what = "abc";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_hf(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_SILENT<<"abc";
        }catch( Lsc::Exception& ex ){
            const char * what = "abc";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_hg(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_DEFAULT<<"";
        }catch( Lsc::UnknownException& ex ){
            const char * what = "";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_hh(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_DEFAULT<<"";
        }catch( Lsc::Exception& ex ){
            const char * what = "";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_hi(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_SILENT<<"";
        }catch( Lsc::UnknownException& ex ){
            const char * what = "";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_hj(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_SILENT<<"";
        }catch( Lsc::Exception& ex ){
            const char * what = "";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_ia(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_DEFAULT<<"abc"<<""<<"def";
        }catch( Lsc::UnknownException& ex ){
            const char * what = "abcdef";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_ib(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_DEFAULT<<"abc"<<""<<"def";
        }catch( Lsc::Exception& ex ){
            const char * what = "abcdef";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_ic(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_SILENT<<"abc"<<""<<"def";
        }catch( Lsc::UnknownException& ex ){
            const char * what = "abcdef";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_info_id(){
        int line;
        try{
            line = __LINE__, throw Lsc::UnknownException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_SILENT<<"abc"<<""<<"def";
        }catch( Lsc::Exception& ex ){
            const char * what = "abcdef";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_ie(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_DEFAULT).push(true);
        }catch( Lsc::BadCastException& ex ){
            const char * what = "true";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_if(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_DEFAULT).push(true);
        }catch( Lsc::Exception& ex ){
            const char * what = "true";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_ig(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_SILENT).push(true);
        }catch( Lsc::BadCastException& ex ){
            const char * what = "true";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_ih(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_SILENT).push(true);
        }catch( Lsc::Exception& ex ){
            const char * what = "true";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_ii(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_DEFAULT).push(false);
        }catch( Lsc::BadCastException& ex ){
            const char * what = "false";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_ij(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_DEFAULT).push(false);
        }catch( Lsc::Exception& ex ){
            const char * what = "false";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_ja(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_SILENT).push(false);
        }catch( Lsc::BadCastException& ex ){
            const char * what = "false";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_jb(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_SILENT).push(false);
        }catch( Lsc::Exception& ex ){
            const char * what = "false";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_jc(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_DEFAULT).push(true).push(false).push(true);
        }catch( Lsc::BadCastException& ex ){
            const char * what = "truefalsetrue";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_jd(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_DEFAULT).push(true).push(false).push(true);
        }catch( Lsc::Exception& ex ){
            const char * what = "truefalsetrue";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_je(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_SILENT).push(true).push(false).push(true);
        }catch( Lsc::BadCastException& ex ){
            const char * what = "truefalsetrue";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_jf(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_SILENT).push(true).push(false).push(true);
        }catch( Lsc::Exception& ex ){
            const char * what = "truefalsetrue";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_jg(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_DEFAULT).push(123);
        }catch( Lsc::BadCastException& ex ){
            const char * what = "123";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_jh(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_DEFAULT).push(123);
        }catch( Lsc::Exception& ex ){
            const char * what = "123";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_ji(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_SILENT).push(123);
        }catch( Lsc::BadCastException& ex ){
            const char * what = "123";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_jj(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_SILENT).push(123);
        }catch( Lsc::Exception& ex ){
            const char * what = "123";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_baa(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_DEFAULT).push(123).push(-456).push(+789).push(0);
        }catch( Lsc::BadCastException& ex ){
            const char * what = "123-4567890";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_bab(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_DEFAULT).push(123).push(-456).push(+789).push(0);
        }catch( Lsc::Exception& ex ){
            const char * what = "123-4567890";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_bac(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_SILENT).push(123).push(-456).push(+789).push(0);
        }catch( Lsc::BadCastException& ex ){
            const char * what = "123-4567890";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_bad(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_SILENT).push(123).push(-456).push(+789).push(0);
        }catch( Lsc::Exception& ex ){
            const char * what = "123-4567890";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_bae(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_DEFAULT).push(123.456).push(-1.2e+100).push(3.14e-50);
        }catch( Lsc::BadCastException& ex ){
            const char * what = "123.456-1.2e+1003.14e-50";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_baf(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_DEFAULT).push(123.456).push(-1.2e+100).push(3.14e-50);
        }catch( Lsc::Exception& ex ){
            const char * what = "123.456-1.2e+1003.14e-50";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_bag(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_SILENT).push(123.456).push(-1.2e+100).push(3.14e-50);
        }catch( Lsc::BadCastException& ex ){
            const char * what = "123.456-1.2e+1003.14e-50";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_bah(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_SILENT).push(123.456).push(-1.2e+100).push(3.14e-50);
        }catch( Lsc::Exception& ex ){
            const char * what = "123.456-1.2e+1003.14e-50";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_bai(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_DEFAULT).push('a').push(char(0)).push('b').push(4,'x');
        }catch( Lsc::BadCastException& ex ){
            const char * what = "abxxxx";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_baj(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_DEFAULT).push('a').push(char(0)).push('b').push(4,'x');
        }catch( Lsc::Exception& ex ){
            const char * what = "abxxxx";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_bba(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_SILENT).push('a').push(char(0)).push('b').push(4,'x');
        }catch( Lsc::BadCastException& ex ){
            const char * what = "abxxxx";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_bbb(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_SILENT).push('a').push(char(0)).push('b').push(4,'x');
        }catch( Lsc::Exception& ex ){
            const char * what = "abxxxx";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_bbc(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_DEFAULT).push("abc");
        }catch( Lsc::BadCastException& ex ){
            const char * what = "abc";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_bbd(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_DEFAULT).push("abc");
        }catch( Lsc::Exception& ex ){
            const char * what = "abc";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_bbe(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_SILENT).push("abc");
        }catch( Lsc::BadCastException& ex ){
            const char * what = "abc";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_bbf(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_SILENT).push("abc");
        }catch( Lsc::Exception& ex ){
            const char * what = "abc";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_bbg(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_DEFAULT).push("");
        }catch( Lsc::BadCastException& ex ){
            const char * what = "";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_bbh(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_DEFAULT).push("");
        }catch( Lsc::Exception& ex ){
            const char * what = "";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_bbi(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_SILENT).push("");
        }catch( Lsc::BadCastException& ex ){
            const char * what = "";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_bbj(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_SILENT).push("");
        }catch( Lsc::Exception& ex ){
            const char * what = "";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_bca(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_DEFAULT).push("abc").push("").push("def");
        }catch( Lsc::BadCastException& ex ){
            const char * what = "abcdef";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_bcb(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_DEFAULT).push("abc").push("").push("def");
        }catch( Lsc::Exception& ex ){
            const char * what = "abcdef";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_bcc(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_SILENT).push("abc").push("").push("def");
        }catch( Lsc::BadCastException& ex ){
            const char * what = "abcdef";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_bcd(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_SILENT).push("abc").push("").push("def");
        }catch( Lsc::Exception& ex ){
            const char * what = "abcdef";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_bce(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_DEFAULT).pushf("%d%s",123,"abc").push(true);
        }catch( Lsc::BadCastException& ex ){
            const char * what = "123abctrue";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_bcf(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_DEFAULT).pushf("%d%s",123,"abc").push(true);
        }catch( Lsc::Exception& ex ){
            const char * what = "123abctrue";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_bcg(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_SILENT).pushf("%d%s",123,"abc").push(true);
        }catch( Lsc::BadCastException& ex ){
            const char * what = "123abctrue";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_bch(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException().push(BSL_EARG).push(Lsc::EXCEPTION_LEVEL_SILENT).pushf("%d%s",123,"abc").push(true);
        }catch( Lsc::Exception& ex ){
            const char * what = "123abctrue";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_bci(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_DEFAULT<<true;
        }catch( Lsc::BadCastException& ex ){
            const char * what = "true";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_bcj(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_DEFAULT<<true;
        }catch( Lsc::Exception& ex ){
            const char * what = "true";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_bda(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_SILENT<<true;
        }catch( Lsc::BadCastException& ex ){
            const char * what = "true";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_bdb(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_SILENT<<true;
        }catch( Lsc::Exception& ex ){
            const char * what = "true";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_bdc(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_DEFAULT<<false;
        }catch( Lsc::BadCastException& ex ){
            const char * what = "false";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_bdd(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_DEFAULT<<false;
        }catch( Lsc::Exception& ex ){
            const char * what = "false";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_bde(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_SILENT<<false;
        }catch( Lsc::BadCastException& ex ){
            const char * what = "false";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_bdf(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_SILENT<<false;
        }catch( Lsc::Exception& ex ){
            const char * what = "false";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_bdg(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_DEFAULT<<true<<false<<true;
        }catch( Lsc::BadCastException& ex ){
            const char * what = "truefalsetrue";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_bdh(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_DEFAULT<<true<<false<<true;
        }catch( Lsc::Exception& ex ){
            const char * what = "truefalsetrue";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_bdi(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_SILENT<<true<<false<<true;
        }catch( Lsc::BadCastException& ex ){
            const char * what = "truefalsetrue";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_bdj(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_SILENT<<true<<false<<true;
        }catch( Lsc::Exception& ex ){
            const char * what = "truefalsetrue";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_bea(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_DEFAULT<<123;
        }catch( Lsc::BadCastException& ex ){
            const char * what = "123";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_beb(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_DEFAULT<<123;
        }catch( Lsc::Exception& ex ){
            const char * what = "123";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_bec(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_SILENT<<123;
        }catch( Lsc::BadCastException& ex ){
            const char * what = "123";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_bed(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_SILENT<<123;
        }catch( Lsc::Exception& ex ){
            const char * what = "123";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_bee(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_DEFAULT<<123<<-456<<+789<<0;
        }catch( Lsc::BadCastException& ex ){
            const char * what = "123-4567890";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_bef(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_DEFAULT<<123<<-456<<+789<<0;
        }catch( Lsc::Exception& ex ){
            const char * what = "123-4567890";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_beg(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_SILENT<<123<<-456<<+789<<0;
        }catch( Lsc::BadCastException& ex ){
            const char * what = "123-4567890";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_beh(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_SILENT<<123<<-456<<+789<<0;
        }catch( Lsc::Exception& ex ){
            const char * what = "123-4567890";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_bei(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_DEFAULT<<123.456<<-1.2e+100<<3.14e-50;
        }catch( Lsc::BadCastException& ex ){
            const char * what = "123.456-1.2e+1003.14e-50";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_bej(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_DEFAULT<<123.456<<-1.2e+100<<3.14e-50;
        }catch( Lsc::Exception& ex ){
            const char * what = "123.456-1.2e+1003.14e-50";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_bfa(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_SILENT<<123.456<<-1.2e+100<<3.14e-50;
        }catch( Lsc::BadCastException& ex ){
            const char * what = "123.456-1.2e+1003.14e-50";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_bfb(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_SILENT<<123.456<<-1.2e+100<<3.14e-50;
        }catch( Lsc::Exception& ex ){
            const char * what = "123.456-1.2e+1003.14e-50";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_bfc(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_DEFAULT<<'a'<<char(0)<<'b';
        }catch( Lsc::BadCastException& ex ){
            const char * what = "ab";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_bfd(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_DEFAULT<<'a'<<char(0)<<'b';
        }catch( Lsc::Exception& ex ){
            const char * what = "ab";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_bfe(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_SILENT<<'a'<<char(0)<<'b';
        }catch( Lsc::BadCastException& ex ){
            const char * what = "ab";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_bff(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_SILENT<<'a'<<char(0)<<'b';
        }catch( Lsc::Exception& ex ){
            const char * what = "ab";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_bfg(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_DEFAULT<<"abc";
        }catch( Lsc::BadCastException& ex ){
            const char * what = "abc";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_bfh(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_DEFAULT<<"abc";
        }catch( Lsc::Exception& ex ){
            const char * what = "abc";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_bfi(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_SILENT<<"abc";
        }catch( Lsc::BadCastException& ex ){
            const char * what = "abc";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_bfj(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_SILENT<<"abc";
        }catch( Lsc::Exception& ex ){
            const char * what = "abc";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_bga(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_DEFAULT<<"";
        }catch( Lsc::BadCastException& ex ){
            const char * what = "";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_bgb(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_DEFAULT<<"";
        }catch( Lsc::Exception& ex ){
            const char * what = "";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_bgc(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_SILENT<<"";
        }catch( Lsc::BadCastException& ex ){
            const char * what = "";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_bgd(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_SILENT<<"";
        }catch( Lsc::Exception& ex ){
            const char * what = "";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_bge(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_DEFAULT<<"abc"<<""<<"def";
        }catch( Lsc::BadCastException& ex ){
            const char * what = "abcdef";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_bgf(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_DEFAULT<<"abc"<<""<<"def";
        }catch( Lsc::Exception& ex ){
            const char * what = "abcdef";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_DEFAULT;
            const char * level_str  = "EXCEPTION_LEVEL_DEBUG";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_bgg(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_SILENT<<"abc"<<""<<"def";
        }catch( Lsc::BadCastException& ex ){
            const char * what = "abcdef";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_BadCastException_info_bgh(){
        int line;
        try{
            line = __LINE__, throw Lsc::BadCastException()<<BSL_EARG<<Lsc::EXCEPTION_LEVEL_SILENT<<"abc"<<""<<"def";
        }catch( Lsc::Exception& ex ){
            const char * what = "abcdef";
            Lsc::exception_level_t level = Lsc::EXCEPTION_LEVEL_SILENT;
            const char * level_str  = "EXCEPTION_LEVEL_SILENT";

            TS_ASSERT( 0 == strcmp( ex.what(), what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), __func__ ) );

            TS_ASSERT( 0 == strcmp( ex.file(), __FILE__ ) );
            TS_ASSERT( 0 == strcmp( ex.function(), __PRETTY_FUNCTION__ ) );
            TS_ASSERT( ex.line() == line );

            TS_ASSERT( ex.level() == level );
            TS_ASSERT( 0 == strcmp( ex.level_str(), level_str ) );

        }
    }


    void test_UnknownException_depth_bgi(){
        try{
            throw_func_push<Lsc::UnknownException>(1);
        }catch( Lsc::UnknownException& ex ){

            TS_ASSERT( strlen( ex.what() ) == 1000000 || 0 == strcmp( ex.what(), throw_func_what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), "throw_func_push" ) );

            TS_ASSERT( 0 == strcmp( ex.file(), "Lsc_test_exception_prepare.h_" ) ); //test BSL_EARG

            TS_ASSERT( ex.level() == throw_func_level );

        }
    }


    void test_UnknownException_depth_bgj(){
        try{
            throw_func_push<Lsc::UnknownException>(1);
        }catch( Lsc::Exception& ex ){

            TS_ASSERT( strlen( ex.what() ) == 1000000 || 0 == strcmp( ex.what(), throw_func_what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), "throw_func_push" ) );

            TS_ASSERT( 0 == strcmp( ex.file(), "Lsc_test_exception_prepare.h_" ) ); //test BSL_EARG

            TS_ASSERT( ex.level() == throw_func_level );

        }
    }


    void test_UnknownException_depth_bha(){
        try{
            throw_func_push<Lsc::UnknownException>(3);
        }catch( Lsc::UnknownException& ex ){

            TS_ASSERT( strlen( ex.what() ) == 1000000 || 0 == strcmp( ex.what(), throw_func_what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), "throw_func_push" ) );

            TS_ASSERT( 0 == strcmp( ex.file(), "Lsc_test_exception_prepare.h_" ) ); //test BSL_EARG

            TS_ASSERT( ex.level() == throw_func_level );

        }
    }


    void test_UnknownException_depth_bhb(){
        try{
            throw_func_push<Lsc::UnknownException>(3);
        }catch( Lsc::Exception& ex ){

            TS_ASSERT( strlen( ex.what() ) == 1000000 || 0 == strcmp( ex.what(), throw_func_what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), "throw_func_push" ) );

            TS_ASSERT( 0 == strcmp( ex.file(), "Lsc_test_exception_prepare.h_" ) ); //test BSL_EARG

            TS_ASSERT( ex.level() == throw_func_level );

        }
    }


    void test_UnknownException_depth_bhc(){
        try{
            throw_func_push<Lsc::UnknownException>(10000);
        }catch( Lsc::UnknownException& ex ){

            TS_ASSERT( strlen( ex.what() ) == 1000000 || 0 == strcmp( ex.what(), throw_func_what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), "throw_func_push" ) );

            TS_ASSERT( 0 == strcmp( ex.file(), "Lsc_test_exception_prepare.h_" ) ); //test BSL_EARG

            TS_ASSERT( ex.level() == throw_func_level );

        }
    }


    void test_UnknownException_depth_bhd(){
        try{
            throw_func_push<Lsc::UnknownException>(10000);
        }catch( Lsc::Exception& ex ){

            TS_ASSERT( strlen( ex.what() ) == 1000000 || 0 == strcmp( ex.what(), throw_func_what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), "throw_func_push" ) );

            TS_ASSERT( 0 == strcmp( ex.file(), "Lsc_test_exception_prepare.h_" ) ); //test BSL_EARG

            TS_ASSERT( ex.level() == throw_func_level );

        }
    }


    void test_UnknownException_depth_bhe(){
        try{
            throw_func_shift<Lsc::UnknownException>(1);
        }catch( Lsc::UnknownException& ex ){

            TS_ASSERT( strlen( ex.what() ) == 1000000 || 0 == strcmp( ex.what(), throw_func_what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), "throw_func_shift" ) );

            TS_ASSERT( 0 == strcmp( ex.file(), "Lsc_test_exception_prepare.h_" ) ); //test BSL_EARG

            TS_ASSERT( ex.level() == throw_func_level );

        }
    }


    void test_UnknownException_depth_bhf(){
        try{
            throw_func_shift<Lsc::UnknownException>(1);
        }catch( Lsc::Exception& ex ){

            TS_ASSERT( strlen( ex.what() ) == 1000000 || 0 == strcmp( ex.what(), throw_func_what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), "throw_func_shift" ) );

            TS_ASSERT( 0 == strcmp( ex.file(), "Lsc_test_exception_prepare.h_" ) ); //test BSL_EARG

            TS_ASSERT( ex.level() == throw_func_level );

        }
    }


    void test_UnknownException_depth_bhg(){
        try{
            throw_func_shift<Lsc::UnknownException>(3);
        }catch( Lsc::UnknownException& ex ){

            TS_ASSERT( strlen( ex.what() ) == 1000000 || 0 == strcmp( ex.what(), throw_func_what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), "throw_func_shift" ) );

            TS_ASSERT( 0 == strcmp( ex.file(), "Lsc_test_exception_prepare.h_" ) ); //test BSL_EARG

            TS_ASSERT( ex.level() == throw_func_level );

        }
    }


    void test_UnknownException_depth_bhh(){
        try{
            throw_func_shift<Lsc::UnknownException>(3);
        }catch( Lsc::Exception& ex ){

            TS_ASSERT( strlen( ex.what() ) == 1000000 || 0 == strcmp( ex.what(), throw_func_what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), "throw_func_shift" ) );

            TS_ASSERT( 0 == strcmp( ex.file(), "Lsc_test_exception_prepare.h_" ) ); //test BSL_EARG

            TS_ASSERT( ex.level() == throw_func_level );

        }
    }


    void test_UnknownException_depth_bhi(){
        try{
            throw_func_shift<Lsc::UnknownException>(10000);
        }catch( Lsc::UnknownException& ex ){

            TS_ASSERT( strlen( ex.what() ) == 1000000 || 0 == strcmp( ex.what(), throw_func_what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), "throw_func_shift" ) );

            TS_ASSERT( 0 == strcmp( ex.file(), "Lsc_test_exception_prepare.h_" ) ); //test BSL_EARG

            TS_ASSERT( ex.level() == throw_func_level );

        }
    }


    void test_UnknownException_depth_bhj(){
        try{
            throw_func_shift<Lsc::UnknownException>(10000);
        }catch( Lsc::Exception& ex ){

            TS_ASSERT( strlen( ex.what() ) == 1000000 || 0 == strcmp( ex.what(), throw_func_what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "UnknownException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::UnknownException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), "throw_func_shift" ) );

            TS_ASSERT( 0 == strcmp( ex.file(), "Lsc_test_exception_prepare.h_" ) ); //test BSL_EARG

            TS_ASSERT( ex.level() == throw_func_level );

        }
    }


    void test_BadCastException_depth_bia(){
        try{
            throw_func_push<Lsc::BadCastException>(1);
        }catch( Lsc::BadCastException& ex ){

            TS_ASSERT( strlen( ex.what() ) == 1000000 || 0 == strcmp( ex.what(), throw_func_what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), "throw_func_push" ) );

            TS_ASSERT( 0 == strcmp( ex.file(), "Lsc_test_exception_prepare.h_" ) ); //test BSL_EARG

            TS_ASSERT( ex.level() == throw_func_level );

        }
    }


    void test_BadCastException_depth_bib(){
        try{
            throw_func_push<Lsc::BadCastException>(1);
        }catch( Lsc::Exception& ex ){

            TS_ASSERT( strlen( ex.what() ) == 1000000 || 0 == strcmp( ex.what(), throw_func_what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), "throw_func_push" ) );

            TS_ASSERT( 0 == strcmp( ex.file(), "Lsc_test_exception_prepare.h_" ) ); //test BSL_EARG

            TS_ASSERT( ex.level() == throw_func_level );

        }
    }


    void test_BadCastException_depth_bic(){
        try{
            throw_func_push<Lsc::BadCastException>(3);
        }catch( Lsc::BadCastException& ex ){

            TS_ASSERT( strlen( ex.what() ) == 1000000 || 0 == strcmp( ex.what(), throw_func_what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), "throw_func_push" ) );

            TS_ASSERT( 0 == strcmp( ex.file(), "Lsc_test_exception_prepare.h_" ) ); //test BSL_EARG

            TS_ASSERT( ex.level() == throw_func_level );

        }
    }


    void test_BadCastException_depth_bid(){
        try{
            throw_func_push<Lsc::BadCastException>(3);
        }catch( Lsc::Exception& ex ){

            TS_ASSERT( strlen( ex.what() ) == 1000000 || 0 == strcmp( ex.what(), throw_func_what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), "throw_func_push" ) );

            TS_ASSERT( 0 == strcmp( ex.file(), "Lsc_test_exception_prepare.h_" ) ); //test BSL_EARG

            TS_ASSERT( ex.level() == throw_func_level );

        }
    }


    void test_BadCastException_depth_bie(){
        try{
            throw_func_push<Lsc::BadCastException>(10000);
        }catch( Lsc::BadCastException& ex ){

            TS_ASSERT( strlen( ex.what() ) == 1000000 || 0 == strcmp( ex.what(), throw_func_what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), "throw_func_push" ) );

            TS_ASSERT( 0 == strcmp( ex.file(), "Lsc_test_exception_prepare.h_" ) ); //test BSL_EARG

            TS_ASSERT( ex.level() == throw_func_level );

        }
    }


    void test_BadCastException_depth_bif(){
        try{
            throw_func_push<Lsc::BadCastException>(10000);
        }catch( Lsc::Exception& ex ){

            TS_ASSERT( strlen( ex.what() ) == 1000000 || 0 == strcmp( ex.what(), throw_func_what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), "throw_func_push" ) );

            TS_ASSERT( 0 == strcmp( ex.file(), "Lsc_test_exception_prepare.h_" ) ); //test BSL_EARG

            TS_ASSERT( ex.level() == throw_func_level );

        }
    }


    void test_BadCastException_depth_big(){
        try{
            throw_func_shift<Lsc::BadCastException>(1);
        }catch( Lsc::BadCastException& ex ){

            TS_ASSERT( strlen( ex.what() ) == 1000000 || 0 == strcmp( ex.what(), throw_func_what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), "throw_func_shift" ) );

            TS_ASSERT( 0 == strcmp( ex.file(), "Lsc_test_exception_prepare.h_" ) ); //test BSL_EARG

            TS_ASSERT( ex.level() == throw_func_level );

        }
    }


    void test_BadCastException_depth_bih(){
        try{
            throw_func_shift<Lsc::BadCastException>(1);
        }catch( Lsc::Exception& ex ){

            TS_ASSERT( strlen( ex.what() ) == 1000000 || 0 == strcmp( ex.what(), throw_func_what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), "throw_func_shift" ) );

            TS_ASSERT( 0 == strcmp( ex.file(), "Lsc_test_exception_prepare.h_" ) ); //test BSL_EARG

            TS_ASSERT( ex.level() == throw_func_level );

        }
    }


    void test_BadCastException_depth_bii(){
        try{
            throw_func_shift<Lsc::BadCastException>(3);
        }catch( Lsc::BadCastException& ex ){

            TS_ASSERT( strlen( ex.what() ) == 1000000 || 0 == strcmp( ex.what(), throw_func_what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), "throw_func_shift" ) );

            TS_ASSERT( 0 == strcmp( ex.file(), "Lsc_test_exception_prepare.h_" ) ); //test BSL_EARG

            TS_ASSERT( ex.level() == throw_func_level );

        }
    }


    void test_BadCastException_depth_bij(){
        try{
            throw_func_shift<Lsc::BadCastException>(3);
        }catch( Lsc::Exception& ex ){

            TS_ASSERT( strlen( ex.what() ) == 1000000 || 0 == strcmp( ex.what(), throw_func_what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), "throw_func_shift" ) );

            TS_ASSERT( 0 == strcmp( ex.file(), "Lsc_test_exception_prepare.h_" ) ); //test BSL_EARG

            TS_ASSERT( ex.level() == throw_func_level );

        }
    }


    void test_BadCastException_depth_bja(){
        try{
            throw_func_shift<Lsc::BadCastException>(10000);
        }catch( Lsc::BadCastException& ex ){

            TS_ASSERT( strlen( ex.what() ) == 1000000 || 0 == strcmp( ex.what(), throw_func_what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), "throw_func_shift" ) );

            TS_ASSERT( 0 == strcmp( ex.file(), "Lsc_test_exception_prepare.h_" ) ); //test BSL_EARG

            TS_ASSERT( ex.level() == throw_func_level );

        }
    }


    void test_BadCastException_depth_bjb(){
        try{
            throw_func_shift<Lsc::BadCastException>(10000);
        }catch( Lsc::Exception& ex ){

            TS_ASSERT( strlen( ex.what() ) == 1000000 || 0 == strcmp( ex.what(), throw_func_what ) );
#if __GNUC__ <= 2
            TS_ASSERT( 0 != strstr( ex.name(), "BadCastException" ) );
#else
            TS_ASSERT( 0 == strcmp( ex.name(), "Lsc::BadCastException" ) );
#endif
            TS_ASSERT( 0 != strstr( ex.stack(), "throw_func_shift" ) );

            TS_ASSERT( 0 == strcmp( ex.file(), "Lsc_test_exception_prepare.h_" ) ); //test BSL_EARG

            TS_ASSERT( ex.level() == throw_func_level );

        }
    }


    void test_UnknownException_e2no_bjc(){
        {
            int errno = 123;
            BSL_E2NO( errno, call_nothrow() );
            assert( errno == 0 );
        }
        {
            int errno = 123;
            BSL_E2NO( errno, call_throw<Lsc::UnknownException>() );
            assert( errno == -1 );
        }
        try{
            call_throw<Lsc::UnknownException>();
        }catch(Lsc::Exception& e){
            see(e.what());
        }
    
        size_t old_level = Lsc::Exception::set_stack_trace_level(1);
        printf("Lsc::Exception::set_stack_trace_level(1)\n");
        {
            int errno = 123;
            BSL_E2NO( errno, call_throw<Lsc::UnknownException>() );
            assert( errno == -1 );
        }
        assert( 1 == Lsc::Exception::set_stack_trace_level(0) );
        printf("Lsc::Exception::set_stack_trace_level(0)\n");
        {
            int errno = 123;
            BSL_E2NO( errno, call_throw<Lsc::UnknownException>() );
            assert( errno == -1 );
        }
        assert( Lsc::Exception::set_stack_trace_level(old_level) == 0 );
        printf("Lsc::Exception::set_stack_trace_level(%d)\n", int(old_level));
        {
            int errno = 123;
            BSL_E2NO( errno, throw std::out_of_range("this is message") );
            assert( errno == -1 );
        }
        {
            int errno = 123;
            BSL_E2NO( errno, call_throw_int(456) );
            assert( errno == -1 );
        }
        {
            int errno = 123;
            BSL_E2NO( errno, errno = 789 );
            assert( errno == 0 );
        }
    }


    void test_BadCastException_e2no_bjd(){
        {
            int errno = 123;
            BSL_E2NO( errno, call_nothrow() );
            assert( errno == 0 );
        }
        {
            int errno = 123;
            BSL_E2NO( errno, call_throw<Lsc::BadCastException>() );
            assert( errno == -1 );
        }
        try{
            call_throw<Lsc::BadCastException>();
        }catch(Lsc::Exception& e){
            see(e.what());
        }
    
        size_t old_level = Lsc::Exception::set_stack_trace_level(1);
        printf("Lsc::Exception::set_stack_trace_level(1)\n");
        {
            int errno = 123;
            BSL_E2NO( errno, call_throw<Lsc::BadCastException>() );
            assert( errno == -1 );
        }
        assert( 1 == Lsc::Exception::set_stack_trace_level(0) );
        printf("Lsc::Exception::set_stack_trace_level(0)\n");
        {
            int errno = 123;
            BSL_E2NO( errno, call_throw<Lsc::BadCastException>() );
            assert( errno == -1 );
        }
        assert( Lsc::Exception::set_stack_trace_level(old_level) == 0 );
        printf("Lsc::Exception::set_stack_trace_level(%d)\n", int(old_level));
        {
            int errno = 123;
            BSL_E2NO( errno, throw std::out_of_range("this is message") );
            assert( errno == -1 );
        }
        {
            int errno = 123;
            BSL_E2NO( errno, call_throw_int(456) );
            assert( errno == -1 );
        }
        {
            int errno = 123;
            BSL_E2NO( errno, errno = 789 );
            assert( errno == 0 );
        }
    }


};
#endif  //__BSL_TEST_EXCEPTION_H_

