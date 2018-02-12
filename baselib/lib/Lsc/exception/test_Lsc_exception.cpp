
#include "Lsc_exception.h"
#include <cassert>

#include<typeinfo>
#include<iostream>
#define see(x) do{  \
    std::cerr<<__FILE__<<":"<<__LINE__<<": " <<(#x)<<" = "<<(x)<<" ("<<typeid(x).name()<<")"<<std::endl;\
}while(0)


int main(){
    int line;
    Lsc::Exception::set_stack_trace_level(10);

    Lsc::AutoBuffer stack;
    //Lsc::Exception::stack_trace( stack );
    //see( stack.c_str() );

    try{
        throw Lsc::UnknownException()<<123<<"abc"<<true<<3.14159<<Lsc::EXCEPTION_LEVEL_FATAL<<BSL_EARG;

    }catch(Lsc::UnknownException& e){
        see(e.name());
        see(e.what());
        see(e.file());
        see(e.line());
        see(e.function());
        see(e.level());
        see(e.level_str());
        see(e.stack());
    }

    try{
        try{
            throw Lsc::NullPointerException()<<123;
        }catch( Lsc::Exception& e ){
            if ( false ){
                e.pushf("%s");
            }
            size_t sz=0;
            e<<456<<sz;
            see( e.what() );
            throw;
        }
    }catch(Lsc::NullPointerException& e){
        see( e.what() );
    }

    class WrongException: plclic Lsc::Exception {};  //wrong inheritance
    try{
        throw WrongException()<<BSL_EARG;
    }catch( Lsc::Exception& e ){
        see( e.what() );
        see( e.level_str() );
    }
    try{
        try{
            try{
                throw Lsc::NullPointerException()<<"haha";
            }catch(Lsc::NullPointerException& e){
                assert( 0 == strcmp( e.what(), "haha" ) );
                throw Lsc::StdException(e);
            }
        }catch(std::exception& e){
            see( e.what() );
            assert( 0 == strcmp( e.what(), "haha" ) );
            throw;
        }
    }catch(Lsc::StdException& e){
        see(e.what());
    }
    try{
        try{
            std::bad_alloc ba;
            see( ba.what());
            throw ba;
        }catch(std::bad_alloc& e){
            see( e.what() );
            throw;
        }
    }catch(std::bad_alloc& e){
        see( e.what() );
    }

/*
    std::bad_alloc ba;
    Lsc::StdException se(ba);
    see(se.name());
    see(se.what());
    */
    return 0;
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
