

#include "Lsc/var/Array.h"
#include "Lsc/var/Dict.h"
#include "Lsc/var/String.h"
#include "Lsc/var/Function.h"
#include "Lsc/var/Method.h"
#include "Lsc/var/Int32.h"


#include "Lsc/var/JsonSerializer.h"
#include "Lsc/var/VScriptEvaluator.h"

#include<typeinfo>
#include<iostream>
#define see(x) do{  \
    std::cerr<<__FILE__<<":"<<__LINE__<<": " <<(#x)<<" = "<<(x)<<" ("<<typeid(x).name()<<")"<<std::endl;\
}while(0)

    
    Lsc::var::IVar& array(Lsc::var::IVar& args, Lsc::ResourcePool& ){
        return args;
    }

    
    Lsc::var::IVar& _echo( Lsc::var::IVar& self, Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        Lsc::var::IVar &res = rp.create<Lsc::var::Dict>();
        res["self"] = self;
        res["args"] = args;
        return res;
    }

    
    Lsc::var::IVar& formatTime( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        char time_str[30];
        const char *format = "%Y-%m-%dT%TZ";
        struct tm time_struct;
        time_t time_val(args[0].to_int32());
        localtime_r( &time_val, &time_struct );
        strftime( time_str, sizeof(time_str), format, &time_struct );
        return rp.create<Lsc::var::String>(time_str);
    }

    
    Lsc::var::IVar& size( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Int32>(args[0].size());
    }

    
    Lsc::var::IVar& let( Lsc::var::IVar& args, Lsc::ResourcePool& ){
        if ( args[1].is_number() ){
            args[0][args[1].to_int32()] = args[2];
        }else{
            args[0][args[1].to_string()] = args[2];
        }
        return Lsc::var::Null::null;
    }

    
    Lsc::var::IVar& join( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        Lsc::var::IVar::string_type res;
        for( int i = 0, n = args.size(); i < n; ++ i ){
            res.append( args[i].to_string() );
        }
        return rp.create<Lsc::var::String>(res);
    }

    
    Lsc::var::IVar& vs_map( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        Lsc::var::IVar &func = args[0], &arg_arr = args[1];
        Lsc::var::IVar &res = rp.create<Lsc::var::Array>(), &func_arg = rp.create<Lsc::var::Array>();
        for( int i = 0, n = arg_arr.size() ; i < n; ++ i ){
            func_arg[0] = arg_arr[i];
            res[i] = func( func_arg, rp );
        }
        return res;
    }

    
    Lsc::var::IVar& reduce( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        Lsc::var::IVar &func = args[0], &arg_arr = args[1], &func_arg = rp.create<Lsc::var::Array>();
        func_arg[0] = args[2];
        for( int i = 0, n = arg_arr.size() ; i < n; ++ i ){
            func_arg[1] = arg_arr[i];
            func_arg[0] = func( func_arg, rp );
        }
        return func_arg[0];
    }

    
    Lsc::var::IVar& loadFunc( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        Lsc::var::IVar& dest = args[0];
        dest["formatTime"]  = rp.create<Lsc::var::Function>(&formatTime, Lsc::var::IVar::string_type("formatTime") );
        dest["size"]        = rp.create<Lsc::var::Function>(&size,       Lsc::var::IVar::string_type("size") );
        dest["let"]         = rp.create<Lsc::var::Function>(&let,        Lsc::var::IVar::string_type("let") );
        dest["join"]        = rp.create<Lsc::var::Function>(&join,       Lsc::var::IVar::string_type("join") );
        dest["array"]       = rp.create<Lsc::var::Function>(&array,      Lsc::var::IVar::string_type("array") );
        dest["map"]         = rp.create<Lsc::var::Function>(&vs_map,     Lsc::var::IVar::string_type("map") );
        dest["reduce"]      = rp.create<Lsc::var::Function>(&reduce,     Lsc::var::IVar::string_type("reduce") );

        return Lsc::var::Null::null;
    }


void common_function_use(){
    Lsc::ResourcePool rp;
    Lsc::var::Dict context;
    Lsc::var::Function formatTime_f( &formatTime, Lsc::var::IVar::string_type("formatTime") ); 
    context["formatTime"] = formatTime_f;
    
    context["array"] = rp.create<Lsc::var::Function>( &array, Lsc::var::IVar::string_type("array") );
    Lsc::var::VScriptEvaluator vse(rp);

    Lsc::var::IVar& res1 = vse.eval("formatTime(0)", context);
    see(res1.to_string());   
    
    
    Lsc::var::IVar& res2 = vse.eval("array(123, array(array()), 'hello')", context);
    Lsc::AutoBuffer res2_json;
    Lsc::var::JsonSerializer js;
    js.serialize( res2, res2_json );
    see( res2_json.c_str() ); 

}

void common_method_use(){
    Lsc::ResourcePool rp;
    Lsc::var::Dict context;
    Lsc::var::Dict obj;
    Lsc::var::Method echo(&_echo, Lsc::var::IVar::string_type("echo") );
    obj["echo"] = echo;
    context["obj"] = obj;

    Lsc::var::VScriptEvaluator vse(rp);
    Lsc::var::IVar& res1 = vse.eval("obj.echo(123, 'hello', obj)", context);
    Lsc::AutoBuffer res1_json;
    Lsc::var::JsonSerializer js;
    js.serialize(res1, res1_json);
    see(res1_json.c_str()); 

}

void high_class_function(){
    
    Lsc::ResourcePool rp;
    Lsc::var::Dict context;
    context["loadFunc"] = rp.create<Lsc::var::Function>(&loadFunc, "loadFunc");
    Lsc::var::VScriptEvaluator vse(rp);
    Lsc::var::JsonSerializer js;
    
    
    vse.eval("loadFunc(.)", context);

    
    Lsc::var::IVar& map_res = vse.eval("map( formatTime, array(0,1,2) )", context);
    Lsc::AutoBuffer map_res_json;
    js.serialize( map_res, map_res_json );
    see( map_res_json.c_str() );    

    
    Lsc::var::IVar& res = vse.eval("reduce( join, map( formatTime, array(0,1,2) ), 'result:' )", context);
    Lsc::AutoBuffer res_json;
    js.serialize( res, res_json );
    see( res_json.c_str() );    
    
}

void variable_definition(){
    
    
    Lsc::ResourcePool rp;
    Lsc::var::Dict context;
    context["loadFunc"] = rp.create<Lsc::var::Function>(&loadFunc, "loadFunc");
    Lsc::var::VScriptEvaluator vse(rp);
    Lsc::var::JsonSerializer js;

    
    vse.eval("loadFunc(.)", context);

    
    vse.eval("let(., 'arr', array(123, 'abc') )", context);
    Lsc::AutoBuffer res1_json;
    js.serialize( context["arr"], res1_json );
    see( res1_json.c_str() );   
    
    
    vse.eval("let( arr, 3, 9394 )", context );
    Lsc::AutoBuffer res2_json;
    js.serialize( context["arr"], res2_json );
    see( res2_json.c_str() );   

    
}

void put_functions_into_namepaces(){
    
    
    Lsc::ResourcePool rp;
    Lsc::var::Dict context, vs, lang, parellel,utils;
    Lsc::var::Function 
        let_f( &let, "let" ), array_f( &array, "array" ),
        formatTime_f( &formatTime, "formatTime" ), join_f( &join, "join" ),
        map_f( &vs_map, "map" ), reduce_f( &reduce, "reduce" );
    lang["let"]     = let_f;
    lang["array"]   = array_f;
    utils["formatTime"] = formatTime_f;
    utils["join"]   = join_f;
    parellel["map"] = map_f;
    parellel["reduce"]  = reduce_f;
    
    vs["lang"]      = lang;
    vs["utils"]     = utils;
    vs["parellel"]  = parellel;

    context["vs"]   = vs;
    
    Lsc::var::VScriptEvaluator vse(rp);
    Lsc::var::JsonSerializer js;

    Lsc::AutoBuffer lib_json;
    js.serialize( context, lib_json );
    see( lib_json.c_str() );  
    

    
    vse.eval( "vs.lang.let( ., 'result', \
        vs.parellel.reduce( vs.utils.join, vs.parellel.map(\
            vs.utils.formatTime, vs.lang.array( 0, 1, 2 )\
        ), 'result:' )\
    )", context);

    see(context["result"].to_string()); 
    
}

int main(){
    common_function_use();
    common_method_use();
    high_class_function();
    variable_definition();
    put_functions_into_namepaces();
    return 0;
}

