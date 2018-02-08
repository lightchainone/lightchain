#include "Lsc/var/var_traits.h"
#include "JsonSerializer.h"
#include "uconv.h"

#include<typeinfo>
#include<iostream>
using namespace std;
#define see(x) do{  \
    cerr<<__FILE__<<":"<<__LINE__<<": " <<(#x)<<" = "<<(x)<<" ("<<typeid(x).name()<<")"<<endl;\
}while(0)

namespace Lsc{
namespace var{
    void JsonSerializer::serialize( const IVar& var, Lsc::AutoBuffer& buf){
        IVar::mask_type mask = var.get_mask();
        if ( mask & IVar::IS_NUMBER ){
            (this->*_s_slctype_map[(mask>>8)&0xFF])( var, buf );
        }else{
            (this->*_s_type_map[mask&0xFF])( var, buf );
        }
    }

    void JsonSerializer::serialize_array( const IVar& var, Lsc::AutoBuffer& buf){
        IVar::array_const_iterator iter = var.array_begin();
        IVar::array_const_iterator end  = var.array_end();
        buf.push('[');
        for(bool first = true ; iter != end; ++iter ){
            if ( first ){
                first = false;
            }else{
                buf.push(',');
            }
            const IVar& v = iter->value();
            IVar::mask_type mask = v.get_mask();
            if ( mask & IVar::IS_NUMBER ){
                (this->*_s_slctype_map[(mask>>8)&0xFF])( v, buf );
            }else{
                (this->*_s_type_map[mask&0xFF])( v, buf );
            }
        }
        buf.push(']');
    }
    void JsonSerializer::serialize_dict( const IVar& var, Lsc::AutoBuffer& buf){
        IVar::dict_const_iterator iter = var.dict_begin();
        IVar::dict_const_iterator end  = var.dict_end();
        buf.push('{');
        for(bool first = true ; iter != end; ++iter ){
            if ( first ){
                first = false;
            }else{
                buf.push(',');
            }
            serialize_cstring( iter->key().c_str(), buf );
            buf.push(':');
            const IVar& v = iter->value();
            IVar::mask_type mask = v.get_mask();
            if ( mask & IVar::IS_NUMBER ){
                (this->*_s_slctype_map[(mask>>8)&0xFF])( v, buf );
            }else{
                (this->*_s_type_map[mask&0xFF])( v, buf );
            }
        }
        buf.push('}');
    }

    void JsonSerializer::serialize_cstring( const char * str, Lsc::AutoBuffer& buf, bool quote ){
        if ( quote ){
            buf.push('\"');
        }
        switch( _encoding ){
            case ANY:
                serialize_any_cstring( str, buf );
                break;
            case GBK:
                serialize_gbk_cstring( str, buf );
                break;
            default:
                
                serialize_any_cstring( str, buf );
        }
        if ( quote ){
            buf.push('\"');
        }
    }

    void JsonSerializer::serialize_any_cstring( const char * str, Lsc::AutoBuffer& buf ){
        for(; *str; ++ str ){
            if ( IS_ASCII(*str) ){
                
                if ( _s_escape_map[int(*str)] ){
                    buf.push('\\');
                    buf.push(_s_escape_map[int(*str)]);
                }else{
                    buf.push(*str);
                }
            }else{
                buf.push(*str);
            }
        }
    }

    void JsonSerializer::serialize_gbk_cstring( const char * str, Lsc::AutoBuffer& buf ){
        while( *str ){
            if ( IS_ASCII(*str) ){
                
                if ( _s_escape_map[int(*str)] ){
                    buf.push('\\');
                    buf.push(_s_escape_map[int(*str)]);
                }else{
                    buf.push(*str);
                }

                str += 1;
            }else if ( LEGAL_GBK_FIRST_BYTE(*str) ){
                unsigned short unicode = UCS2_NO_MAPPING;
                if ( LEGAL_GBK_2BYTE_SECOND_BYTE(*(str+1)) ){
                    
                    if ( _escape_chinese ){
                        gbk_to_unicode( str, 2, &unicode, 1, UCONV_INVCHAR_REPLACE );
                        buf.pushf("\\u%04hx",unicode);
                    }else{
                        buf.push( str, 2 );
                    }
                    str += 2;
                }else if ( LEGAL_GBK_4BYTE_SECOND_BYTE(*(str+1))
                        && LEGAL_GBK_4BYTE_THIRD_BYTE(*(str+2))
                        && LEGAL_GBK_4BYTE_FORTH_BYTE(*(str+3))){
                    if ( _escape_chinese ){
                        gbk_to_unicode( str, 4, &unicode, 1, UCONV_INVCHAR_REPLACE );
                        buf.pushf("\\u%04hx",unicode);
                    }else{
                        buf.push( str, 4 );
                    }
                    str += 4;
                }else{
                    
                    unicode = UCS2_NO_MAPPING;
                    buf.pushf("\\u%04hx",unicode);
                    str += 1; 
                }

            }else{
                
                str += 1; 
            }
        }
    }

    void JsonSerializer::serialize_raw( const IVar& var, Lsc::AutoBuffer& buf){
        static const char hex_map[] = { '0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f' };
        raw_t raw = var.to_raw(); 
        buf.reserve( buf.size() + raw.length * 2 + 9 ); 
        buf.push( "unhex(\"", 7 ); 
        const unsigned char* data = static_cast<const unsigned char*>(raw.data);
        const unsigned char* end  = data + raw.length;
        for( ; data != end; ++ data ){
            buf.push( hex_map[(*data) >> 4] );
            buf.push( hex_map[(*data) & 15] );
        }
        buf.push( "\")", 2 ); 
    }

    void JsonSerializer::serialize_unknown( const IVar& , Lsc::AutoBuffer&  ){
        
        return;
    }

    void JsonSerializer::_s_set_type_handler( handler_t handler, IVar::mask_type set_mask, IVar::mask_type unset_mask ){
        for( IVar::mask_type i = 0; i < 256; ++ i ){
            if ( IVar::check_mask( i, set_mask, unset_mask ) ){
                _s_type_map[i] = handler;
            }
        }
    }

    void JsonSerializer::_s_set_slctype_handler( handler_t handler, IVar::mask_type set_mask, IVar::mask_type unset_mask ){
        for( IVar::mask_type i = 0; i < 256; ++ i ){
            if ( IVar::check_mask( (i << 8)|IVar::IS_NUMBER, set_mask, unset_mask ) ){
                _s_slctype_map[i] = handler;
            }
        }
    }

    bool JsonSerializer::_s_init_type_map(){
        
        _s_set_type_handler( &JsonSerializer::serialize_unknown,    IVar::NONE_MASK, IVar::NONE_MASK );
        _s_set_type_handler( &JsonSerializer::serialize_null,       IVar::NONE_MASK, IVar::ALL_MASK );
        _s_set_type_handler( &JsonSerializer::serialize_bool,       IVar::IS_BOOL );
        
        _s_set_type_handler( &JsonSerializer::serialize_string,     IVar::IS_STRING );
        _s_set_type_handler( &JsonSerializer::serialize_raw,        IVar::IS_RAW );
        _s_set_type_handler( &JsonSerializer::serialize_array,      IVar::IS_ARRAY );
        _s_set_type_handler( &JsonSerializer::serialize_dict,       IVar::IS_DICT );
        _s_set_type_handler( &JsonSerializer::serialize_callable,   IVar::IS_CALLABLE );
        
        return true;
    }

    bool JsonSerializer::_s_init_slctype_map(){
        
        _s_set_slctype_handler( &JsonSerializer::serialize_unknown, IVar::NONE_MASK, IVar::NONE_MASK );
        _s_set_slctype_handler( &JsonSerializer::_serialize<signed char>, var_traits<signed char>::MASK, var_traits<signed char>::ANTI_MASK );
        _s_set_slctype_handler( &JsonSerializer::_serialize<unsigned char>, var_traits<unsigned char>::MASK, var_traits<unsigned char>::ANTI_MASK );
        _s_set_slctype_handler( &JsonSerializer::_serialize<signed short>, var_traits<signed short>::MASK, var_traits<signed short>::ANTI_MASK );
        _s_set_slctype_handler( &JsonSerializer::_serialize<unsigned short>, var_traits<unsigned short>::MASK, var_traits<unsigned short>::ANTI_MASK );
        _s_set_slctype_handler( &JsonSerializer::_serialize<signed int>, var_traits<signed int>::MASK, var_traits<signed int>::ANTI_MASK );
        _s_set_slctype_handler( &JsonSerializer::_serialize<unsigned int>, var_traits<unsigned int>::MASK, var_traits<unsigned int>::ANTI_MASK );
        _s_set_slctype_handler( &JsonSerializer::_serialize<signed long long>, var_traits<signed long long>::MASK, var_traits<signed long long>::ANTI_MASK );
        _s_set_slctype_handler( &JsonSerializer::_serialize<unsigned long long>, var_traits<unsigned long long>::MASK, var_traits<unsigned long long>::ANTI_MASK );
        _s_set_slctype_handler( &JsonSerializer::_serialize_float, var_traits<float>::MASK, var_traits<float>::ANTI_MASK );
        _s_set_slctype_handler( &JsonSerializer::_serialize_dolcle, var_traits<dolcle>::MASK, var_traits<dolcle>::ANTI_MASK );
        return true;
    }

    bool JsonSerializer::_s_init_escape_map(){
        _s_escape_map[int('\"')] = '"';
        _s_escape_map[int('\\')] = '\\';
        _s_escape_map[int('/' )] = '/';
        _s_escape_map[int('\b')] = 'b';
        _s_escape_map[int('\f')] = 'f';
        _s_escape_map[int('\n')] = 'n';
        _s_escape_map[int('\r')] = 'r';
        _s_escape_map[int('\t')] = 't';
        return true;
    }

    const char JsonSerializer::DEFAULT_DOLCLE_FORMATER[MAX_FORMATER_LEN+1] = "%g";
    char JsonSerializer::_s_escape_map[];
    JsonSerializer::handler_t JsonSerializer::_s_type_map[];
    JsonSerializer::handler_t JsonSerializer::_s_slctype_map[];
    bool JsonSerializer::_s_initer = 
        JsonSerializer::_s_init_escape_map()
        && JsonSerializer::_s_init_type_map()
        && JsonSerializer::_s_init_slctype_map();
}}

