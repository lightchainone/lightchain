#include <stdio.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <Lsc/var/IVar.h>
#include <Lsc/var/VScriptEvaluator.h>
#include <Lsc/var/implement.h>
#include <Lsc/var/BmlDeserializer.h>
#include <Lsc/vs/function.h>


namespace {

    extern "C" {
        Lsc::var::IVar& echo( Lsc::var::IVar& args, Lsc::ResourcePool& ){
            return args;
        }

        Lsc::var::IVar& int32_add( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
            size_t i = 0, n = args.size();
            int res = 0;
            for(; i < n; ++ i ){
                res += args[i].to_int32();
            }
            return rp.create<Lsc::var::Int32>(res);
        }
    }


    
    Lsc::var::IVar& _echo( Lsc::var::IVar& self, Lsc::var::IVar&args, Lsc::ResourcePool& rp ){
        Lsc::var::IVar &res = rp.create<Lsc::var::Dict>();
        res["self"] = self;
        res["args"] = args;
        return res;
    }

    
    Lsc::var::IVar& base_name(Lsc::var::IVar& args, Lsc::ResourcePool& rp){
        Lsc::var::IVar & names = args[0].is_array() ? static_cast<Lsc::var::IVar&>(args[0]) : args; 
        Lsc::var::IVar::string_type res;
        for( int i = 0, size = names.size(); i < size; ++ i ){
            const Lsc::var::IVar::string_type& str = names[i].to_string();
            const char *cstr = str.c_str(), *pos = strchr(cstr, '@');
            if ( i != 0 ){
                res.append("¡¢");
            }
            if(pos){
                res.append(cstr, pos - cstr);
            }else{
                res.append(str);
            }
        }
        return rp.create<Lsc::var::String>(res);
    }

    
    Lsc::var::IVar& name_link(Lsc::var::IVar& args, Lsc::ResourcePool& rp){
        Lsc::var::IVar & names = args[0].is_array() ? static_cast<Lsc::var::IVar&>(args[0]) : args; 

        Lsc::var::IVar::string_type res;
        for( int i = 0, size = names.size(); i < size; ++ i ){
            const Lsc::var::IVar::string_type& str = names[i].to_string();
            const char *cstr = str.c_str(), *pos = strchr(cstr, '@');
            if ( i != 0 ){
                res.append("¡¢");
            }
            if ( pos ){
                res.append("<a href='mailto:");
                res.append(str);
                res.append("'>");
                res.append(cstr, pos - cstr);
                res.append("</a>");
            }else{
                res.append(str);
            }
        }
        return rp.create<Lsc::var::String>(res);
    }

    
    Lsc::var::IVar& format_time( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        char time_str[30];
        const char *format = "%Y-%m-%dT%TZ";
        struct tm time_struct;
        time_t time_(args[0].to_int32());
        localtime_r( &time_, &time_struct );
        strftime( time_str, sizeof(time_str), format, &time_struct );
        return rp.create<Lsc::var::String>(time_str);
    }

    
    Lsc::var::IVar& load_func( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        Lsc::var::IVar& dest = args[0];
        dest["base_name"]       = rp.create<Lsc::var::Function>( &base_name,                  "base_name");
        dest["name_link"]       = rp.create<Lsc::var::Function>( &name_link,                  "name_link");
        dest["format_time"]     = rp.create<Lsc::var::Function>( &format_time,                "format_time");
        Lsc::vs::import_vs( args, rp );
        return Lsc::var::Null::null;
    }

    

    const char * HELP_MESSAGE = 
        "VScript Interactive Interpreter 1.1\n"
        "\n"
        "Usage:\n"
        "   vscript                               Interactive Mode\n"
        "   vscript [-o|--output] [VSCRIPT FILE]  Macro Mode, default:'macro.vs'\n"
        "   vscript [-a|--append] [VSCRIPT FILE]  Macro Append Mode, default:'macro.vs'\n"
        "   vscript [VSCRIPT FILE]                Test Mode\n"
        "   vscript [-h|--help]                   display this message\n"
        "\n"
        "Interactive Mode:\n"
        "   In this mode, you can type VScripts line by line.\n"
        "   Return values of the script are printed to STDOUT.\n"
        "   Auxiliary infomation is printed to STDERR.\n"
        "\n"
        "Macro Mode:\n"
        "   This mode is similar to Interactive Mode, except that STDIN is \n"
        "   copied to the VScript file specified by the cmdline;\n"
        "   STDOUT is also copied to the same file WITH A LEADING '>'.\n"
        "   You can use this mode to generate test file which will be used in Test Mode.\n"
        "\n"
        "Macro Append Mode:\n"
        "   This mode is similar to Macro Mode, except that the VScript\n"
        "   file will be appended instead of overwritten if already exist.\n"
        "\n"
        "Test Mode:\n"
        "   In this mode, test file specified by the cmdline is read line by line and printed to STDERR.\n"
        "   Lines begin with '#' is also copied to STDOUT;\n"
        "   Lines biegin with neither '#' nor '>' will be evaluated,\n"
        "   return values are printed to STDOUT WITH A LEADING '>'.\n"
        "   You can use this mode to test the file automaticly. If return values of all vscript\n" 
        "   expressions remain unchanged, STDOUT should be the same as the VSCRIPT FILE.\n"
        "\n"
        ;

} 

int main(int argc, char** argv){

    Lsc::Exception::set_line_delimiter("\n");
    Lsc::ResourcePool rp;
    Lsc::var::Bool TRUE(true), FALSE(false);
    Lsc::var::IVar& data = rp.create<Lsc::var::Dict>();
    data["load_func"] = rp.create<Lsc::var::Function>(&load_func, Lsc::var::IVar::string_type("load_func") );
    data["null"] = Lsc::var::Null::null;
    data["true"] = TRUE;
    data["false"]= FALSE;

    Lsc::var::Array args;
    args[0] = data;
    load_func( args, rp );  

    if ( argc > 1 ){
        if ( strcmp( "-h", argv[1]) == 0 || strcmp( "--help", argv[1] ) == 0 ){
            fputs( HELP_MESSAGE , stderr);
            return 0;
        } else if ( strcmp( "-o", argv[1]) == 0 || strcmp( "--output", argv[1]) == 0){
            
            const char * filename = "macro.vs";
            if ( argc > 2 ){
                filename = argv[2];
            }
            
            FILE *fp = fopen( filename, "w" );  
            if ( fp ){
                fclose(fp); 
            }
            
            Lsc::var::ShallowString macro_file = filename;
            args[2] = macro_file;
            Lsc::vs::interpret( args, rp );

        } else if ( strcmp( "-a", argv[1]) == 0 || strcmp( "--append", argv[1]) == 0){
            
            const char * filename = "macro.vs";
            if ( argc > 2 ){
                filename = argv[2];
            }
            
            Lsc::var::ShallowString macro_file = filename;
            args[2] = macro_file;
            Lsc::vs::interpret( args, rp );
            
        }else{
            
            Lsc::var::ShallowString test_file = argv[1];
            args[1] = test_file;
            Lsc::vs::interpret( args, rp );
        }
    }else{
        
        Lsc::vs::interpret( args, rp );
    }
    return 0;
}


