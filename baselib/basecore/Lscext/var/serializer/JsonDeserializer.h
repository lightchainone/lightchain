#ifndef  __JSONDESERIALIZER_H_
#define  __JSONDESERIALIZER_H_

#include <algorithm>
#include "Lsc/var/ITextDeserializer.h"
#include "Lsc/ResourcePool.h"



namespace Lsc{
namespace var{

    
    class JsonDeserializer: plclic ITextDeserializer{
    plclic:
        
        enum output_encoding_t{
            GBK,   
        };

        
        typedef Lsc::string string_type;
    plclic:
        
        explicit JsonDeserializer(ResourcePool& rp)
            :_unescape_chinese(DEFAULT_UNESCAPE_CHINESE), 
            _encoding(DEFAULT_UNESCAPE_ENCODING), _rp(rp), 
            _str_buf(_rp.create<string_type>(string_type::allocator_type(&_rp.get_mempool()))),
            _begin(NULL), _cur(NULL), _cur_line(0), _cur_line_begin(NULL) { }

        
        virtual IVar& deserialize( const char * text){
            if ( !text ){
                throw NullPointerException()<<BSL_EARG<<"text can't be NULL";
            }
            _begin = _cur = _cur_line_begin = text;
            _cur_line = 1;
            try{
                return deserialize_value();
            }catch(Lsc::Exception& e){
                
                
                const char * begin = _cur - 30 > _begin ? _cur - 30 : _begin; 
                e.push("{JsonDeserializer line[").push(current_line())
                    .push("] col[").push(current_column());
                e.push("] json[").push( begin, _cur - begin ).pushf("~!~%.20s] }", _cur);
                throw;
            }
        }
 
        
        void set_unescape_chinese( bool unescape_chinese ){
            _unescape_chinese = unescape_chinese;
        }

        
        bool get_unescape_chinese() const {
            return _unescape_chinese;
        }

        
        void set_output_encoding( output_encoding_t encoding ){
            _encoding = encoding;
        }

        
        output_encoding_t get_output_encoding() const {
            return _encoding;
        }

        
        int current_pos() const {
            return _cur - _begin;
        }

        
        int current_line() const {
            return _cur_line;
        }

        
        int current_column() const {
            return _cur - _cur_line_begin + 1;
        }

     plclic:
        
        static const bool DEFAULT_UNESCAPE_CHINESE = false;             
        
        static const output_encoding_t DEFAULT_UNESCAPE_ENCODING = GBK;

    private:
        
        void skip_white(){
            while( _s_token_map[key_t(*_cur)] == WHITE_SPACE ){
                if ( *_cur == '\n' ){
                    ++ _cur_line;
                    _cur_line_begin = _cur;
                }
                ++ _cur;
            }
        }

        
        virtual IVar& deserialize_value();

        
        virtual IVar& deserialize_object();


        
        virtual IVar& deserialize_array();

        
        virtual void deserialize_string();

        
        virtual void deserialize_unicode();

        
        virtual IVar& deserialize_unhex();
    private:
        
        bool                _unescape_chinese;
        
        output_encoding_t   _encoding;
        
        ResourcePool&       _rp;
        
        string_type&        _str_buf;
        
        const char *        _begin;
        
        const char *        _cur;

        
        int                 _cur_line;

        
        const char *        _cur_line_begin;

        
    private:

        
        typedef unsigned char key_t;
        
        enum token_t{
            END,          
            WHITE_SPACE,  
            OBJECT_BEGIN, 
            OBJECT_END,   
            ARRAY_BEGIN,  
            ARRAY_END,    
            LPAREN,       
            RPAREN,       
            NUMBER_LITERAL,       
            STRING_LITERAL,       
            TRUE_LITERAL,         
            FALSE_LITERAL,        
            NULL_LITERAL,         
            UNHEX_LITERAL,        
            COMMA,        
            COLON,        
        };

        
        static bool _s_init_unescape_map();

        
        static bool _s_init_token_map();

        
        static bool _s_init_hex_map();

        
        static char _s_unescape_map[0x100];

        
        static token_t _s_token_map[0x100];

        
        static unsigned short _s_hex_map[0x100];

        
        static bool _s_static_initer;
    };

}}   

#endif  


