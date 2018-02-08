#include "Lsc/check_cast.h"

#include<typeinfo>
#include<iostream>
#define see(x) do{  \
    std::cerr<<__FILE__<<":"<<__LINE__<<": " <<(#x)<<" = "<<(x)<<" ("<<typeid(x).name()<<")"<<std::endl;\
}while(0)

void check_cast_numbers(){
    try{
        Lsc::check_cast<int>(1000000000000LL); 
    }catch(Lsc::Exception& ae){
        see(ae.name());
    }
        
    try{
       Lsc::check_cast<unsigned int>(-1); 
    }catch(Lsc::Exception& be){
        see(be.name());
    }

    int c = Lsc::check_cast<int>(3.14); 
    see(c);

    float d = Lsc::check_cast<float>(10); 
    see(d);
}

void check_cast_cstrings(){
    int a = Lsc::check_cast<int>("123"); 
    see(a);

    int b = Lsc::check_cast<int>("-1e10"); 
    see(b);

    try{
        Lsc::check_cast<unsigned int>("-123"); 
    }catch(Lsc::Exception& ce){
        see(ce.name());
    }

    try{
        Lsc::check_cast<short>(32768); 
    }catch(Lsc::Exception& de){
        see(de.name());
    }
    
    int e = Lsc::check_cast<int>(NULL); 
    see(e);
    
    try{
        Lsc::check_cast<int>(static_cast<const char*>(NULL)); 
    }catch(Lsc::Exception& fe){
        see(fe.name());
    }
}

int main(){
    check_cast_numbers();
    check_cast_cstrings();
    return 0;
}


