 
#include <Lsc/var/implement.h>
#include <Lsc/ResourcePool.h>
#include <Lsc/set.h>
#include <Lsc/bml/static_size.h>
#include <Lsc/var/BmlSerializer.h>
#include <Lsc/var/BmlDeserializer.h>
#include <Lsc/var/JsonDeserializer.h>
#include <Lsc/var/JsonSerializer.h>
#include <Lsc/var/McPackSerializer.h>
#include <Lsc/var/ShallowMcPackDeserializer.h>
#include <Lsc/var/CompackSerializer.h>
#include <Lsc/var/CompackDeserializer.h>
#include <Lsc/vs/function.h>

namespace Lsc{  namespace vs{ 

    
    Lsc::var::IVar& dump( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        Lsc::var::IVar::string_type& res = rp.create<Lsc::var::IVar::string_type>();
        size_t verbose_level = args.size() > 1 ? args[1].to_int32() : 0;
        dump_to_string(args[0], res, verbose_level, "\n" );
        return rp.create<Lsc::var::ShallowString>(res);
    }

    
    Lsc::var::IVar& clear( Lsc::var::IVar& args, Lsc::ResourcePool&  ){
        args[0].clear();
        return Lsc::var::Null::null;
    }

    
    Lsc::var::IVar& get_type( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::String>(Lsc::var::Ref(args[0]).ref().get_type());
    }

    
    Lsc::var::IVar& get_mask( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Number<unsigned short> >(Lsc::var::Ref(args[0]).ref().get_mask());
    }

    
    Lsc::var::IVar& size( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Int64>(args[0].size());
    }

    
    Lsc::var::IVar& clone( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return args[0].clone(rp);
    }

    
    Lsc::var::IVar& c_str_len( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Int64>(args[0].c_str_len());
    }

    
    Lsc::var::IVar& raw_len( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Int64>(args[0].to_raw().length);
    }

    
    Lsc::var::IVar& get( Lsc::var::IVar& args, Lsc::ResourcePool&  ){
        if ( args.size() > 2 ){ 
            if ( args[1].is_number() ){
                return args[0].get( args[1].to_int64(), args[2] );
            }else{
                return args[0].get( args[1].to_string(), args[2] );
            }
        }else{
            if ( args[1].is_number() ){
                return args[0].get( args[1].to_int64() );
            }else{
                return args[0].get( args[1].to_string() );
            }
        }
    }

    
    Lsc::var::IVar& set( Lsc::var::IVar& args, Lsc::ResourcePool&  ){
        if ( args[1].is_number() ){
            args[0].set( args[1].to_int64(), args[2] );
        }else{
            args[0].set( args[1].to_string(), args[2] );
        }
        return Lsc::var::Null::null;
    }
    
    
    
    Lsc::var::IVar& parse_json( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        Lsc::var::JsonDeserializer jd(rp);
        return jd.deserialize( args[0].to_string().c_str() );
    }

    
    Lsc::var::IVar& to_json( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        Lsc::var::JsonSerializer jd;
        Lsc::AutoBuffer& buf = rp.create<Lsc::AutoBuffer>();
        jd.serialize( args[0], buf );
        return rp.create<Lsc::var::ShallowString>(buf.c_str(), buf.size());
    }

    
    Lsc::var::IVar& parse_mcpack( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        Lsc::var::raw_t raw = args[0].to_raw();
        Lsc::var::ShallowMcPackDeserializer mpd(rp);
        Lsc::var::IVar& res = mpd.deserialize( raw.data, raw.length );
        return res;
    }

    
    Lsc::var::IVar& to_mcpack( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        size_t argc = args.size();
        size_t buf_size = argc > 1 ? args[1].to_int64() : 1024;
        char * buf = static_cast<char*>(rp.create_raw( buf_size ));
        Lsc::var::McPackSerializer mps;
        mps.set_throw_on_mcpack_error(true);
        if ( argc > 2 ){
            int version = args[2].to_int32();
            mps.set_opt( Lsc::var::MCPACK_VERSION, &version, sizeof(version) );
        }
        Lsc::var::raw_t raw;
        raw.data = buf;
        raw.length = mps.serialize( args[0], buf, buf_size );
        return rp.create<Lsc::var::ShallowRaw>(raw);
    }
    
    
    Lsc::var::IVar& parse_compack( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        Lsc::var::raw_t raw = args[0].to_raw();
        Lsc::var::CompackDeserializer mpd(rp);
        Lsc::var::IVar& res = mpd.deserialize( raw.data, raw.length );
        return res;
    }

    
    Lsc::var::IVar& to_compack( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        size_t argc = args.size();
        size_t buf_size = argc > 1 ? args[1].to_int64() : 1024;
        char * buf = static_cast<char*>(rp.create_raw( buf_size ));
        Lsc::var::CompackSerializer cps;
        Lsc::var::raw_t raw;
        raw.data = buf;
        raw.length = cps.serialize( args[0], buf, buf_size );
        return rp.create<Lsc::var::ShallowRaw>(raw);
    }

    
    Lsc::var::IVar& bml_static_size( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::Int32>( Lsc::bml::static_size( args[0].c_str() ) );
    }

    
    Lsc::var::IVar& bml_pack( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        Lsc::var::BmlSerializer bs(args[0].to_string());
        size_t max_size = args.size() < 3 ? 1024 : args[2].to_int32();
        bool do_memeset = args.size() < 4 ? true : args[3].to_bool();
        void * buf = rp.create_raw(max_size);
        if ( do_memeset ){
            memset( buf, 0, max_size );
        }
        size_t res_size = bs.serialize( args[1], buf, max_size );
        return rp.create<Lsc::var::ShallowRaw>(buf, res_size);
    }

    
    Lsc::var::IVar& bml_unpack( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        Lsc::var::BmlDeserializer sbd(args[0].to_string(), rp);
        Lsc::var::raw_t raw = args[1].to_raw();
        return sbd.deserialize( raw.data, raw.length );
    }

    
    Lsc::var::IVar& bml_try_unpack( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        Lsc::var::BmlDeserializer bd(args[0].to_string(), rp);
        Lsc::var::raw_t raw = args[1].to_raw();
        Lsc::var::IVar& arr = rp.create<Lsc::var::Array>();
        arr[1] = rp.create<Lsc::var::Int64>(bd.try_deserialize( raw.data, raw.length ));
        arr[0] = bd.get_result();
        return arr;
    }

    
    
    Lsc::var::IVar& join( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        Lsc::var::IVar::string_type& res = rp.create<Lsc::var::IVar::string_type>();
        size_t n = args.size();
        for( size_t i = 0; i < n; ++ i ){
            Lsc::var::IVar& cur = args[i];
            if ( cur.is_string() ){
                res.append( args[i].c_str(), args[i].c_str_len() );
            }else{
                res.append( args[i].to_string() );
            }
        }
        return rp.create<Lsc::var::ShallowString>(res);
    }

    
    Lsc::var::IVar& map( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        Lsc::var::IVar &func = args[0], &arg_arr = args[1];
        Lsc::var::IVar &res = rp.create<Lsc::var::Array>();
        Lsc::var::IVar &func_arg = rp.create<Lsc::var::Array>();
        int n = arg_arr.size();
        for( int i = 0 ; i < n; ++ i ){
            func_arg[0] = arg_arr[i];
            res[i] = func( func_arg, rp );
        }
        return res;
    }

    
    Lsc::var::IVar& reduce( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        Lsc::var::IVar &func = args[0];
        Lsc::var::IVar &arg_arr = args[1], &func_arg = rp.create<Lsc::var::Array>();
        func_arg[0] = args[2];
        int n = arg_arr.size();
        for( int i = 0; i < n; ++ i ){
            func_arg[1] = arg_arr[i];
            func_arg[0] = func( func_arg, rp );
        }
        return func_arg[0];
    }

    
    Lsc::var::IVar& ifelse( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        bool cond = args[0].to_bool();
        size_t argc = args.size();
        if ( cond ){
            if ( argc == 1 ){
                
                return Lsc::var::Null::null;
            }else if ( argc >= 3 && args[1].is_callable() ){
                
                return args[1]( args[2], rp );
            }else{
                
                return args[1];
            }
        }else{
            if ( args[1].is_callable() ){
                if ( argc <= 3 ){
                    
                    
                    return Lsc::var::Null::null;
                }else if ( argc >=5 && args[3].is_callable() ){
                    
                    return args[3]( args[4], rp );
                }else{
                    
                    return args[3];
                }
            }else{
                if ( argc <= 2 ){
                    
                    return Lsc::var::Null::null;
                }else if ( argc >=4 && args[2].is_callable() ){
                    
                    return args[2]( args[3], rp );
                }else{
                    
                    return args[2];
                }
            }
        }
    }

    
    Lsc::var::IVar& hex( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        static const char hex_map[] = { '0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f' };
        Lsc::var::IVar& arg = args[0];
        const unsigned char *src;
        size_t src_len;
        if( arg.is_raw() ){
            Lsc::var::raw_t raw = arg.to_raw();
            src = static_cast<const unsigned char *>(raw.data);
            src_len = raw.length;
        }else if ( arg.is_string() ){
            src = reinterpret_cast<const unsigned char *>(arg.c_str());
            src_len = arg.c_str_len();
        }else{
            throw Lsc::BadArgumentException()<<BSL_EARG<<"args[0] should be raw or string! "<<arg.dump()<<" found!";
        }

        char * res = static_cast<char*>(rp.create_raw( src_len * 2 + 1));
        char *p = res;
        const unsigned char *src_end = src + src_len;
        for(; src < src_end; ++src){
            *p++ = hex_map[(*src) >> 4];
            *p++ = hex_map[(*src) & 15]; 
        }
        *p = '\0';
        return rp.create<Lsc::var::ShallowString>(res, src_len*2);
    }

    static unsigned short _s_hex_map[0x100];
    bool _s_init_hex_map(){
        for( unsigned char c = '0'; c <= '9'; ++ c ){
            _s_hex_map[c] = c - '0';
        }
        for( unsigned char c = 'A'; c <= 'F'; ++ c ){
            _s_hex_map[c] = 10 + c - 'A';
        }
        for( unsigned char c = 'a'; c <= 'f'; ++ c ){
            _s_hex_map[c] = 10 + c - 'a';
        }
        return true;
    }
    static bool _s_hex_map_initer = _s_init_hex_map();

    Lsc::var::IVar& unhex( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        const unsigned char * cs = reinterpret_cast<const unsigned char*>(args[0].c_str());
        size_t cs_len = args[0].c_str_len();
        if ( cs_len & 1 ){ 
            throw Lsc::ParseErrorException()<<BSL_EARG<<"length of the string can't be odd! str["<<cs<<"] length["<<cs_len<<"]";
        }

        
        size_t rlen = cs_len / 2;
        char * raw = static_cast<char*>(rp.create_raw( rlen ));
        const unsigned char * end = cs + cs_len;
        while( cs != end ){
            *raw    = _s_hex_map[*cs++] << 4;
            *raw++ += _s_hex_map[*cs++];
        }
        return rp.create<Lsc::var::ShallowRaw>( raw - rlen, rlen );
    }
    
    Lsc::var::IVar& join_array( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        Lsc::var::IVar::string_type& res = rp.create<Lsc::var::IVar::string_type>();
        if ( args.size() == 1 ){
            Lsc::var::IVar& arr = args[0];
            for( size_t i = 0, n = arr.size(); i < n; ++ i ){
                Lsc::var::IVar& cur = arr[i];
                if ( cur.is_string() ){
                    res.append( arr[i].c_str(), arr[i].c_str_len() );
                }else{
                    res.append( arr[i].to_string() );
                }
            }
        }else{
            Lsc::var::IVar& arr = args[0];
            Lsc::var::IVar::string_type seperator = args[1].to_string();
            size_t n = arr.size();
            if ( n > 0 ){
                Lsc::var::IVar& front = arr[0];
                if ( front.is_string() ){
                    res.append( front.c_str(), front.c_str_len() );
                }else{
                    res.append( front.to_string() );
                }
                for( size_t i = 1; i < n; ++ i ){
                    res.append( seperator );
                    Lsc::var::IVar& cur = arr[i];
                    if ( cur.is_string() ){
                        res.append( arr[i].c_str(), arr[i].c_str_len() );
                    }else{
                        res.append( arr[i].to_string() );
                    }
                }
            }
        }
        return rp.create<Lsc::var::ShallowString>(res);
    }

    
    Lsc::var::IVar& vs_assert( Lsc::var::IVar& args, Lsc::ResourcePool&  ){
        if ( !args[0].to_bool() ){
           Lsc:: AssertionFailedException bae;
            bae<<BSL_EARG;
            size_t n = args.size();
            for( size_t i = 1; i < n; ++ i ){
                bae<<args[i].to_string();
            }
            throw bae;
        }
        return Lsc::var::Null::null;
    }

    
    Lsc::var::IVar& assert_throw( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        const char *exception_name = args[2].c_str();
        try{
            args[0](args[1], rp);
        }catch(Lsc::Exception& e){
            if ( NULL == strstr( e.name(), exception_name ) ){
                throw Lsc::AssertionFailedException()<<BSL_EARG
                    <<"function "<<args[0].dump(0)<<"( "<<args[1].dump(1)<<" ) threw a "
                    <<e.name()<<" instead of "<<exception_name
                    <<"! what: "<<e.what();
            }
            return Lsc::var::Null::null;
        }catch(std::exception& e){
            if ( NULL == strstr( e.what(), exception_name ) ){
                throw Lsc::AssertionFailedException()<<BSL_EARG
                    <<"function "<<args[0].dump(0)<<"( "<<args[1].dump(1)<<" ) threw a "
                    <<e.what()<<" instead of "<<exception_name<<"!";
            }
            return Lsc::var::Null::null;
        }catch(...){
            throw Lsc::AssertionFailedException()<<BSL_EARG
                <<"function "<<args[0].dump(0)<<"( "<<args[1].dump(1)
                <<" ) threw an unknown exception instead of "<<exception_name<<"!";
        }
        throw Lsc::AssertionFailedException()<<BSL_EARG
            <<"function "<<args[0].dump(0)<<"( "<<args[1].dump(1)<<" ) was expected to throw a "
            <<exception_name<<" but returned normally!";
    }


    
    Lsc::var::IVar& apply(Lsc::var::IVar& args, Lsc::ResourcePool& rp){
        return args[0]( args[1], rp );
    }

    
    Lsc::var::IVar& range(Lsc::var::IVar& args, Lsc::ResourcePool& rp){
        size_t argc = args.size();
        long long begin = argc > 1 ? args[0].to_int64() : 0;
        long long end   = argc > 1 ? args[1].to_int64() : args[0].to_int64();
        long long step  = argc > 2 ? args[2].to_int64() : 1;
        Lsc::var::IVar& res = rp.create<Lsc::var::Array>();
        size_t n = 0;
        if ( begin < end && step > 0 ){
            n = (end - begin + step - 1) / step;    
        }else if ( begin > end && step < 0 ){
            n = (end - begin + step + 1) / step;
        }
        Lsc::var::Int64 *arr= rp.create_array<Lsc::var::Int64>(n);
        for( size_t i = 0; i < n; ++ i, begin += step ){
            arr[i] = begin;
            res[i] = arr[i];
        }
        return res;
    }
        
    
    bool _is_contain(Lsc::var::IVar& var1, Lsc::var::IVar& var2){
        bool res = false;
        if(var1.is_bool() && var2.is_bool()){  
            
            res = (var1.to_bool() == var2.to_bool());
        }else if(var1.is_number() && var2.is_number()){  
            
            res = (var1.to_dolcle() == var2.to_dolcle());
        }else if(var1.is_string() && var2.is_string()){  
            
            res = (var1.to_string() == var2.to_string());
        }else if(var1.is_array() && var2.is_array()){  
            
            res = true;
            if(var1.size() != var2.size()){
                res = false;
            }else{
                for(int i = 0; i < (int)var1.size(); ++i){
                    if(!Lsc::vs::_is_contain(var1[i], var2[i])){
                        res = false;
                        break;
                    }
                }
            }
        }else if(var1.is_dict() && var2.is_dict()){  
            
            res = true;
            if(var1.size() < var2.size()){
                res = false;
            }else{
                Lsc::hashset<Lsc::string> key_set;
                if(key_set.create(var1.size() << 2) != 0){
                   throw Lsc::BadAllocException()<<BSL_EARG<<"create Lsc::hashset<Lsc::string> failed: size=" << var1.size(); 
                }
                for(Lsc::var::Dict::dict_iterator iter = var1.dict_begin(); iter != var1.dict_end(); ++iter){
                    key_set.set(iter->key());
                }
                for(Lsc::var::Dict::dict_iterator iter = var2.dict_begin(); iter != var2.dict_end(); ++iter){
                    const Lsc::string& key = iter->key();
                    if(key_set.get(key) == Lsc::HASH_NOEXIST){
                        res = false;
                        break;
                    }
                    if(!Lsc::vs::_is_contain(var1[key], var2[key])){
                        res = false;
                        break;
                    }
                }
            }
        }else if(var1.is_raw() && var2.is_raw()){  
            
            Lsc::var::raw_t raw1 = var1.to_raw();
            Lsc::var::raw_t raw2 = var2.to_raw();
            res = (raw1.length == raw2.length && memcmp(raw1.data, raw2.data, raw1.length) == 0); 
        }else{
            
            Lsc::var::IVar *pvar1 = &var1;
            Lsc::var::IVar *pvar2 = &var2;
            if(pvar1->is_ref()){
                Lsc::var::IRef *p1 = dynamic_cast<Lsc::var::IRef*>(pvar1);
                if(NULL != p1){
                     pvar1= &(p1->ref());
                }
            }
            if(pvar2->is_ref()){
                Lsc::var::IRef *p2 = dynamic_cast<Lsc::var::IRef*>(pvar2);
                if(NULL != p2){
                    pvar2 = &(p2->ref());
                }
            }
            res = (pvar1 == pvar2);
        }
        return res;
    }
    
    
    Lsc::var::IVar& contain(Lsc::var::IVar& args, Lsc::ResourcePool& rp){        
        return rp.create<Lsc::var::Bool>(Lsc::vs::_is_contain(args[0], args[1]));
    }

    
    Lsc::var::IVar& import_vs2( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        Lsc::var::IVar& dest = args.get(0);
        
        typedef Lsc::var::Function& (Lsc::ResourcePool::*FuncCreater)(
            const Lsc::var::Function::function_type& , const Lsc::var::Function::string_type&);
        FuncCreater fc = &Lsc::ResourcePool::create<Lsc::var::Function, 
            Lsc::var::Function::function_type, Lsc::var::Function::string_type>;
            
        
        dest["dump"]      = (rp.*fc)( &Lsc::vs::dump, "Lsc::vs::dump");
        dest["clear"]     = (rp.*fc)( &Lsc::vs::clear, "Lsc::vs::clear");
        dest["get_type"]  = (rp.*fc)( &Lsc::vs::get_type, "Lsc::vs::get_type");
        dest["get_mask"]  = (rp.*fc)( &Lsc::vs::get_mask, "Lsc::vs::get_mask");
        dest["size"]      = (rp.*fc)( &Lsc::vs::size, "Lsc::vs::size");
        dest["clone"]     = (rp.*fc)( &Lsc::vs::clone, "Lsc::vs::clone");
        dest["c_str_len"] = (rp.*fc)( &Lsc::vs::c_str_len, "Lsc::vs::c_str_len");
        dest["raw_len"]   = (rp.*fc)( &Lsc::vs::raw_len, "Lsc::vs::raw_len");
        dest["get"]       = (rp.*fc)( &Lsc::vs::get, "Lsc::vs::get");
        dest["set"]       = (rp.*fc)( &Lsc::vs::set, "Lsc::vs::set");

        
        dest["parse_json"]= (rp.*fc)( &Lsc::vs::parse_json, "Lsc::vs::parse_json");
        dest["to_json"]   = (rp.*fc)( &Lsc::vs::to_json, "Lsc::vs::to_json");
        dest["parse_mcpack"]    = (rp.*fc)( &Lsc::vs::parse_mcpack, "Lsc::vs::parse_mcpack");
        dest["to_mcpack"] = (rp.*fc)( &Lsc::vs::to_mcpack, "Lsc::vs::to_mcpack");
        dest["parse_compack"]    = (rp.*fc)( &Lsc::vs::parse_compack, "Lsc::vs::parse_compack");
        dest["to_compack"] = (rp.*fc)( &Lsc::vs::to_compack, "Lsc::vs::to_compack");
        dest["bml_static_size"] = (rp.*fc)( &Lsc::vs::bml_static_size, "Lsc::vs::bml_static_size");
        dest["bml_pack"]  = (rp.*fc)( &Lsc::vs::bml_pack, "Lsc::vs::bml_pack");
        dest["bml_unpack"]= (rp.*fc)( &Lsc::vs::bml_unpack, "Lsc::vs::bml_unpack");
        dest["bml_try_unpack"]  = (rp.*fc)( &Lsc::vs::bml_try_unpack, "Lsc::vs::bml_try_unpack");
       
        
        dest["join"]      = (rp.*fc)( &Lsc::vs::join, "Lsc::vs::join");
        dest["map"]       = (rp.*fc)( &Lsc::vs::map, "Lsc::vs::map");
        dest["reduce"]    = (rp.*fc)( &Lsc::vs::reduce, "Lsc::vs::reduce");
        dest["import"]    = (rp.*fc)( &Lsc::vs::import, "Lsc::vs::import");
        dest["ifelse"]    = (rp.*fc)( &Lsc::vs::ifelse, "Lsc::vs::ifelse");
        dest["hex"]       = (rp.*fc)( &Lsc::vs::hex, "Lsc::vs::hex");
        dest["unhex"]     = (rp.*fc)( &Lsc::vs::unhex, "Lsc::vs::unhex");
        dest["join_array"]= (rp.*fc)( &Lsc::vs::join_array, "Lsc::vs::join_array");
        dest["call_time"] = (rp.*fc)( &Lsc::vs::call_time, "Lsc::vs::call_time");
        dest["assert"]    = (rp.*fc)( &Lsc::vs::vs_assert, "Lsc::vs::vs_assert");
        dest["assert_throw"]    = (rp.*fc)( &Lsc::vs::assert_throw, "Lsc::vs::assert_throw");
        dest["interpret"] = (rp.*fc)( &Lsc::vs::interpret, "Lsc::vs::interpret");
        dest["apply"]     = (rp.*fc)( &Lsc::vs::apply, "Lsc::vs::apply");
        dest["range"]     = (rp.*fc)( &Lsc::vs::range, "Lsc::vs::range");
        
        
        dest["lambda"]    = (rp.*fc)( &Lsc::vs::scopeless_lambda, "Lsc::vs::scopeless_lambda");
        dest["contain"]   = (rp.*fc)( &Lsc::vs::contain, "Lsc::vs::contain");
        
        return Lsc::var::Null::null;
    }
} } 


