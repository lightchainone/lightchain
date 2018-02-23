
#include <cassert>
#include <iostream>
#include "Lsc/ShallowCopyString.h"

int main(){
    //todo: more tests, even more tests!
    const char *str = "hello world";
    size_t len = strlen(str);
    Lsc::ShallowCopyString ss = str;
    assert( 0 == strcmp(ss.c_str(), str) );
    assert( ss.length() == len );
    assert( ss.size() == len );
    assert( ss.capacity() == len );
    assert( ss == str );
    assert( str == ss );
    assert( ss == Lsc::ShallowCopyString(str) );
    assert( ss == Lsc::ShallowCopyString(ss) );
    
    std::cout<<Lsc::ShallowCopyString("Hello world! I am ShallowCopyString!!!")<<std::endl;

    return 0;
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
