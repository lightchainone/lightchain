#include <fcntl.h>
#include <dlfcn.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <Lsc/var/implement.h>
#include <Lsc/ResourcePool.h>
#include <Lsc/var/VPathEvaluator.h>
#include <Lsc/var/VScriptEvaluator.h>
#include <Lsc/var/VTplEvaluator.h>
#include <Lsc/var/JsonSerializer.h>
#include <Lsc/vs/math.h>
#include <Lsc/vs/function.h>

namespace Lsc{  namespace vs{ 
    
    Lsc::var::IVar& vpath( Lsc::var::IVar& args, Lsc::ResourcePool& ){
        Lsc::var::VPathEvaluator evaluator;
        return evaluator.eval( args[0].c_str(), args[1] );
    }

    
    Lsc::var::IVar& vscript( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        Lsc::var::VScriptEvaluator evaluator(rp);
        return evaluator.eval( args[0].c_str(), args[1] );
    }

    
    Lsc::var::IVar& vtpl( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        Lsc::var::VTplEvaluator evaluator(rp);
        Lsc::AutoBuffer  buf;
        if ( args.size() > 2 && args[2].to_bool() == true ){
            evaluator.set_debug_mode(true);
        }
        evaluator.eval( args[0].to_string().c_str(), args[1], buf );
        return rp.create<Lsc::var::String>(buf.c_str());
    }
  
    
    Lsc::var::IVar& call_time( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        long long times = args.size() > 2 ? args[2].to_int64() : 1;
        struct timeval tvb, tve;
        Lsc::var::IVar& func = args[0];
        Lsc::var::IVar& func_args = args[1];
        gettimeofday( &tvb, NULL );
        while( times -- ){
            func(func_args, rp);
        }
        gettimeofday( &tve, NULL );

        return rp.create<Lsc::var::Int64>( (tve.tv_sec - tvb.tv_sec)*1000000 + (tve.tv_usec - tvb.tv_usec) );
    }

    static void my_dlclose(void * handler){
        dlclose(handler);
    }
    
    Lsc::var::IVar& import( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        Lsc::var::IVar& symbol_arg = args.get(0);
        Lsc::var::IVar& alias_arg = args.get(1);
        Lsc::var::IVar& file_arg = args.get(2);
        const char * so_file = file_arg.is_null() ? NULL : file_arg.c_str();
        void * handler = dlopen(so_file, RTLD_NOW);
        if (!handler) {
            throw Lsc::DynamicLinkingException()<<BSL_EARG<<"dlopen("
                <<( so_file == NULL ? "NULL" : so_file )
                    <<", RTLD_NOW) return NULL! message["<<dlerror()<<"]";
        }
        dlerror();  
        if ( so_file == NULL ){
            
            dlclose(handler);
        }else{
            
            rp.attach( handler, my_dlclose ); 
        }
        dlerror();  
        Lsc::var::Function::function_type func = \
            (Lsc::var::Function::function_type)(dlsym(handler, symbol_arg.c_str()));
        if (!func) {
            throw Lsc::DynamicLinkingException()<<BSL_EARG<<"failed to load symbol["
                <<symbol_arg.c_str()<<"] from file["<<(so_file == NULL ? "NULL" : so_file )
                <<"]! message["<<dlerror()<<"]";
        }
        return rp.create<Lsc::var::Function>( func, alias_arg.is_null() ? symbol_arg.c_str() : alias_arg.c_str() );
    }
    
    inline FILE* rp_fopen( Lsc::ResourcePool& rp, const char * file_name, const char * mode ){
        FILE* file = fopen(file_name, mode);
        if ( !file  ){
            throw Lsc::FileNotFoundException()<<BSL_EARG<<"open file["<<file_name
                <<"] with mode["<<mode<<"] failed!";
        }
        rp.attach( file, &Lsc::Lsc_fclose );    
        return file;
    }
    
    
    Lsc::var::IVar& interpret( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        size_t argc = args.size();
        FILE * test_file     = stdin;
        FILE * macro_file    = stderr;
        if ( argc > 1 && !args[1].is_null() ){
            
            test_file = rp_fopen(rp,args[1].c_str(), "r"); 
        }
        if ( argc > 2 && !args[2].is_null() ){
            
            macro_file = rp_fopen(rp,args[2].c_str(), "a"); 
        }

        Lsc::var::VScriptEvaluator vse(rp);
        Lsc::AutoBuffer str_buf;
        Lsc::var::JsonSerializer serializer;
        Lsc::var::IVar& data = args[0];
        char *line_ptr = NULL;
        size_t line_size = 0;
        while(true){
            try{
                if ( test_file == stdin ){
                    fprintf(stderr, ">>>");
                }

                if ( -1 == getline( &line_ptr, &line_size, test_file ) ){
                    
                    free( line_ptr );
                    break;
                }

                if ( line_ptr[0] == '\0' || line_ptr[0] == '\n' || line_ptr[0] == '#' ){
                    if ( test_file != stdin ){
                        fputs( line_ptr, stdout );
                        fputs( line_ptr, macro_file );
                    }
                    continue;
                }

                if ( line_ptr[0] == '>' ){
                    fputs( line_ptr, macro_file );
                    continue;
                }

                if ( test_file != stdin ){
                    
                    fputs( line_ptr, stdout );
                }else if ( macro_file != stderr ){
                    
                    fputs( line_ptr, macro_file );
                }

                Lsc::var::IVar& res = vse.eval(line_ptr, data);
                if ( res.is_array() || res.is_dict() ){
                    
                    
                    str_buf.clear();
                    serializer.serialize( res, str_buf );
                    if ( test_file == stdin && macro_file != stderr ){
                        
                        fputc( '>', macro_file );
                        fputs( str_buf.c_str(), macro_file );
                        fputc( '\n', macro_file );
                    }
                    if ( test_file != stdin ){
                        fputc( '>', stdout );
                    }
                    fputs( str_buf.c_str(), stdout );
                    fputc( '\n', stdout );

                }else if ( res.is_string() ){
                    
                    
                    if ( test_file == stdin && macro_file != stderr ){
                        
                        fputc( '>', macro_file );
                        fputs( res.c_str(), macro_file );
                        fputc( '\n', macro_file );
                    }
                    if ( test_file != stdin ){
                        fputc( '>', stdout );
                    }
                    fputs( res.c_str(), stdout );
                    fputc( '\n', stdout );
                }else if ( !res.is_null() ){
                    
                    
                    const Lsc::var::IVar::string_type& str_res = res.to_string();
                    if ( test_file == stdin && macro_file != stderr ){
                        fputc( '>', macro_file );
                        fputs( str_res.c_str(), macro_file );
                        fputc( '\n', macro_file );
                    }
                    if ( test_file != stdin ){
                        fputc( '>', stdout );
                    }
                    fputs( str_res.c_str(), stdout );
                    fputc( '\n', stdout );
                }
            }catch(Lsc::Exception& e ){
                if ( test_file == stdin && macro_file != stderr ){
                    fputc( '>', macro_file );
                    fputs( e.name(), macro_file );
                    fputc( '\n', macro_file );
                }
                if ( test_file != stdin ){
                    fputc( '>', stdout );
                }
                fputs( e.name(), stdout );
                fputc( '\n', stdout );
                if ( test_file == stdin ){
                    fprintf( stderr, "Oops! an exception '%s' was thrown.\nwhat: %s\nfrom: %s:%d:%s\nstack:\n%s\n",
                            e.name(), e.what(), e.file(), int(e.line()), e.function(), e.stack()
                           );
                }
            }
            if ( test_file != stdin ){
                
                fputs( line_ptr, macro_file );
            }
        }

        if ( test_file == stdin ){
            fputs( "\nBye!\n", stderr );
        }
        return Lsc::var::Null::null;
    }


    extern Lsc::var::IVar& import_vs1( Lsc::var::IVar& args, Lsc::ResourcePool& rp );

    extern Lsc::var::IVar& import_vs2( Lsc::var::IVar& args, Lsc::ResourcePool& rp );
    
    
    Lsc::var::IVar& import_vs( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        Lsc::var::IVar& dest = args.get(0);
        
        typedef Lsc::var::Function& (Lsc::ResourcePool::*FuncCreater)(
            const Lsc::var::Function::function_type& , const Lsc::var::Function::string_type&);
        FuncCreater fc = &Lsc::ResourcePool::create<Lsc::var::Function, 
            Lsc::var::Function::function_type, Lsc::var::Function::string_type>;
            
        
        dest["vpath"]     = (rp.*fc)( &Lsc::vs::vpath, "Lsc::vs::vpath");
        dest["vscript"]   = (rp.*fc)( &Lsc::vs::vscript, "Lsc::vs::vscript");
        dest["vtpl"]      = (rp.*fc)( &Lsc::vs::vtpl, "Lsc::vs::vtpl");

        
        import_math( args, rp );
        import_vs1(args, rp);
        import_vs2(args, rp);
        
        return Lsc::var::Null::null;
    }
} } 


