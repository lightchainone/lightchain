#include "Lsc/var/implement.h"
#include "Lsc/var/JsonDeserializer.h"
#include "Lsc/var/JsonSerializer.h"

#include<typeinfo>
#include<iostream>
#define see(x) do{  \
    std::cerr<<__FILE__<<":"<<__LINE__<<": " <<(#x)<<" = "<<(x)<<" ("<<typeid(x).name()<<")"<<std::endl;\
}while(0)



Lsc::var::IVar& formatTime( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
    char time_str[30];
    const char *format = "%Y-%m-%dT%TZ";
    struct tm time_struct;
    time_t time(args[0].to_int32());
    localtime_r( &time, &time_struct );
    strftime( time_str, sizeof(time_str), format, &time_struct );
    return rp.create<Lsc::var::String>(time_str);
}


Lsc::var::IVar& join( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
    Lsc::var::IVar::string_type res;
    for( int i = 0, size = args.size(); i < size; ++ i ){
        res.append( args[i].to_string() );
    }
    return rp.create<Lsc::var::String>(res);
}




Lsc::var::IVar& echo_method( Lsc::var::IVar& self, Lsc::var::IVar&args, Lsc::ResourcePool& rp ){
    Lsc::var::IVar &res = rp.create<Lsc::var::Dict>();
    res["self"] = self;
    res["args"] = args;
    return res;
}

void var_function(){
    Lsc::ResourcePool rp;
    Lsc::var::Int32 time1 = 0, time2 = 1231401179;

    Lsc::var::Array args;
    args[0] = time1;
    see( formatTime(args, rp).to_string() );
    args[0] = time2;
    see( formatTime(args, rp).to_string() );

}

void var_method(){
    Lsc::ResourcePool rp;
    Lsc::var::Dict obj;
    Lsc::var::Array args;
    Lsc::var::IVar& res = echo_method( obj, args, rp );
    see( res["self"].is_dict() );   
    see( res["args"].is_array() );
}

int main(){
    var_function();
    var_method();
    return 0;
}


