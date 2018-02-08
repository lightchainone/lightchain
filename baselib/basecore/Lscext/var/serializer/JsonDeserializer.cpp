
#include "JsonDeserializer.h"
#include "smart_deserialize_number.h"
#include "Lsc/var/implement.h"
#include <stdint.h>
#include "uconv.h"

extern uint16_t gUnicodeToGBKTable[]; 

namespace Lsc{
namespace var{

    IVar& JsonDeserializer::deserialize_value(){

        skip_white();
        switch( _s_token_map[key_t(*_cur)] ){ 
            case OBJECT_BEGIN:
                return deserialize_object();

            case ARRAY_BEGIN:
                return deserialize_array();

            case STRING_LITERAL:
                deserialize_string();
                return _rp.create<var::String>( _str_buf );

            case NUMBER_LITERAL:
                return smart_deserialize_number( _rp, _cur, &_cur );

            case TRUE_LITERAL:
                if ( *(_cur+1) == 'r' && *(_cur+2) == 'u' && *(_cur+3) == 'e' ){    
                    _cur += 4;
                    return _rp.create<var::Bool>(true);
                }else{
                    throw ParseErrorException().push(BSL_EARG);
                }

            case FALSE_LITERAL:
                if ( *(_cur+1) == 'a' && *(_cur+2) == 'l' && *(_cur+3) == 's' && *(_cur+4) == 'e' ){ 
                    _cur += 5;
                    return _rp.create<var::Bool>(false);
                }else{
                    throw ParseErrorException().push(BSL_EARG);
                }

            case NULL_LITERAL:
                if ( *(_cur+1) == 'u' && *(_cur+2) == 'l' && *(_cur+3) == 'l' ){    
                    _cur += 4;
                    return _rp.create<var::Null>();
                }else{
                    throw ParseErrorException().push(BSL_EARG);
                }

            case UNHEX_LITERAL:
                if ( _cur[1] == 'n' && _cur[2] == 'h' && _cur[3] == 'e' && _cur[4] == 'x' ){    
                    return deserialize_unhex();
                }else{
                    throw ParseErrorException().push(BSL_EARG);
                }

            default:
                throw ParseErrorException().push(BSL_EARG);
        }

    }
    IVar& JsonDeserializer::deserialize_object(){
        Dict& object = _rp.create<Dict>(var::Dict::allocator_type(&_rp.get_mempool()));
        ++ _cur; 
        skip_white();
        if ( _s_token_map[key_t(*_cur)] == STRING_LITERAL ){
            while(true){
                deserialize_string();
                IVar& value = object[_str_buf];
                skip_white();
                if ( _s_token_map[key_t(*_cur)] != COLON ){
                    throw ParseErrorException().push(BSL_EARG);
                }else{
                    ++ _cur; 
                }
                value = deserialize_value();
                skip_white();

                if ( _s_token_map[key_t(*_cur)] == OBJECT_END ){
                    break;
                }else if ( _s_token_map[key_t(*_cur)] != COMMA ){
                    throw ParseErrorException().push(BSL_EARG);
                }
                ++ _cur; 
                skip_white();
                if ( _s_token_map[key_t(*_cur)] != STRING_LITERAL ){
                    throw ParseErrorException().push(BSL_EARG);
                }
            }
        }
        if ( _s_token_map[key_t(*_cur)] != OBJECT_END ){
            throw ParseErrorException().push(BSL_EARG);
        }
        ++ _cur; 
        return object;

    }
    IVar& JsonDeserializer::deserialize_array(){
        Array& array = _rp.create<Array>(var::Array::allocator_type(&_rp.get_mempool()));
        int array_size  = 0;
        ++ _cur; 
        skip_white();
        if ( _s_token_map[key_t(*_cur)] != ARRAY_END ){
            while(true){
                array[array_size++] = deserialize_value();
                skip_white();

                if ( _s_token_map[key_t(*_cur)] == ARRAY_END ){
                    break;
                }else if ( _s_token_map[key_t(*_cur)] != COMMA ){
                    throw ParseErrorException().push(BSL_EARG);
                }
                ++ _cur; 
            }
        }
        if ( _s_token_map[key_t(*_cur)] != ARRAY_END ){
            throw ParseErrorException().push(BSL_EARG);
        }
        ++ _cur; 
        return array;

    }

    void JsonDeserializer::deserialize_string(){
        char delimiter = *_cur;
        const char *last_pos = _cur + 1;
        ++ _cur; 
        _str_buf.clear();   
        while(*_cur && *_cur != delimiter){ 
            if ( *_cur == '\\' ){   
                _str_buf.append( last_pos, _cur );
                if ( *(_cur+1) == 'u' ){    
                    deserialize_unicode();
                }else{
                    ++ _cur;
                    char tmp = _s_unescape_map[key_t(*_cur)];
                    if ( !tmp ){    
                        _str_buf.push_back('\\');
                        _str_buf.push_back(*_cur);
                    }else{
                        _str_buf.push_back(tmp);
                    }
                    ++ _cur;
                }
                last_pos = _cur;

            }else{
                ++ _cur;
            }
        }
        if ( *_cur == delimiter ){
            
            _str_buf.append( last_pos, _cur );
            ++ _cur; 
        }else{
            throw Lsc::ParseErrorException()<<BSL_EARG<<"expect:"<<delimiter<<" get:\\0";
        }
    }

    IVar& JsonDeserializer::deserialize_unhex(){
        
        _cur += 5;  

        
        skip_white();
        if ( _s_token_map[key_t(*_cur)] != LPAREN ){
            throw Lsc::ParseErrorException()<<BSL_EARG<<"expect:( get:"<<*_cur;
        }
        ++ _cur;

        
        skip_white();
        if ( _s_token_map[key_t(*_cur)] != STRING_LITERAL ){
            throw Lsc::ParseErrorException()<<BSL_EARG<<"expect:' or \" get:"<<*_cur;
        }
        deserialize_string(); 
        if ( _str_buf.size() & 1 ){ 
            throw Lsc::ParseErrorException()<<BSL_EARG<<"length of the string can't be odd! str["<<_str_buf<<"] length["<<_str_buf.size()<<"]";
        }

        
        skip_white();
        if ( _s_token_map[key_t(*_cur)] != RPAREN ){
            throw Lsc::ParseErrorException()<<BSL_EARG<<"expect:) get:"<<*_cur;
        }
        ++ _cur;
        
        
        
        size_t raw_len = _str_buf.size() / 2;
        char * raw = static_cast<char*>(_rp.create_raw( raw_len ));
        const char * str = _str_buf.c_str();
        const char * end = _str_buf.c_str() + _str_buf.size();
        while( str != end ){
            *raw    = _s_hex_map[key_t(*str++)] << 4;
            *raw++ += _s_hex_map[key_t(*str++)];
        }
        return _rp.create<Lsc::var::ShallowRaw>( raw - raw_len, raw_len );

    }

    void JsonDeserializer::deserialize_unicode(){
        
        if ( !(*(_cur+2) && *(_cur+3) && *(_cur+4) && *(_cur+5))){
            throw ParseErrorException().push(BSL_EARG);
        }
        if ( _unescape_chinese ){
            unsigned short unicode = _s_hex_map[key_t(*(_cur+2))] << 12 
                | _s_hex_map[key_t(*(_cur+3))] << 8
                | _s_hex_map[key_t(*(_cur+4))] << 4
                | _s_hex_map[key_t(*(_cur+5))];
            switch (_encoding){
                case GBK: 
                    _str_buf.push_back( gUnicodeToGBKTable[unicode] >> 8 );
                    _str_buf.push_back( gUnicodeToGBKTable[unicode] & 0xff );
                    break ;
                default:
                    _str_buf.append( _cur, 6 );  
            }
        }else{  
            _str_buf.append( _cur, 6 );  
        }
        _cur += 6;
    }

    bool JsonDeserializer::_s_init_unescape_map(){
        _s_unescape_map[key_t('\"')] = '\"';
        _s_unescape_map[key_t('\'')] = '\"';
        _s_unescape_map[key_t('\\')] = '\\';
        _s_unescape_map[key_t('/' )] = '/';
        _s_unescape_map[key_t('b')] = '\b';
        _s_unescape_map[key_t('f')] = '\f';
        _s_unescape_map[key_t('n')] = '\n';
        _s_unescape_map[key_t('r')] = '\r';
        _s_unescape_map[key_t('t')] = '\t';
        return true;
    }

    bool JsonDeserializer::_s_init_token_map(){
        _s_token_map[key_t('\0')] = END;  

        _s_token_map[key_t(' ')]  = WHITE_SPACE;
        _s_token_map[key_t('\t')] = WHITE_SPACE;
        _s_token_map[key_t('\r')] = WHITE_SPACE;
        _s_token_map[key_t('\n')] = WHITE_SPACE;

        _s_token_map[key_t('{')]  = OBJECT_BEGIN; 
        _s_token_map[key_t('}')]  = OBJECT_END; 
        _s_token_map[key_t('[')]  = ARRAY_BEGIN; 
        _s_token_map[key_t(']')]  = ARRAY_END; 
        _s_token_map[key_t('(')]  = LPAREN; 
        _s_token_map[key_t(')')]  = RPAREN; 
        _s_token_map[key_t('\"')]  = STRING_LITERAL; 
        _s_token_map[key_t('\'')]  = STRING_LITERAL; 
        _s_token_map[key_t('t')]  = TRUE_LITERAL; 
        _s_token_map[key_t('f')]  = FALSE_LITERAL; 
        _s_token_map[key_t('n')]  = NULL_LITERAL; 
        _s_token_map[key_t('u')]  = UNHEX_LITERAL; 

        _s_token_map[key_t(',')]  = COMMA;
        _s_token_map[key_t(':')]  = COLON;

        _s_token_map[key_t('+')]  = NUMBER_LITERAL;  
        _s_token_map[key_t('-')]  = NUMBER_LITERAL;
        for( char c = '0'; c <= '9'; ++ c ){
            _s_token_map[key_t(c)] = NUMBER_LITERAL;
        }
        return true;
    }
    bool JsonDeserializer::_s_init_hex_map(){
        for( char c = '0'; c <= '9'; ++ c ){
            _s_hex_map[key_t(c)] = c - '0';
        }
        for( char c = 'A'; c <= 'F'; ++ c ){
            _s_hex_map[key_t(c)] = 10 + c - 'A';
        }
        for( char c = 'a'; c <= 'f'; ++ c ){
            _s_hex_map[key_t(c)] = 10 + c - 'a';
        }
        return true;
    }
    char                        JsonDeserializer::_s_unescape_map[0x100];
    JsonDeserializer::token_t   JsonDeserializer::_s_token_map[0x100];
    unsigned short              JsonDeserializer::_s_hex_map[0x100];
    bool                        JsonDeserializer::_s_static_initer = (JsonDeserializer::_s_init_unescape_map(), JsonDeserializer::_s_init_token_map(), JsonDeserializer::_s_init_hex_map() );
}}

