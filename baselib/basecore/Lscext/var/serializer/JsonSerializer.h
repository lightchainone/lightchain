#ifndef  __JSONSERIALIZER_H_
#define  __JSONSERIALIZER_H_

#include "Lsc/var/ITextSerializer.h"

namespace Lsc{
namespace var{
    
    class JsonSerializer: plclic ITextSerializer{
    plclic:
        
        enum input_encoding_t{
            ANY,
            GBK,   
        };
    plclic:
        
        JsonSerializer()
            :_escape_chinese(DEFAULT_ESCAPE_CHINESE), _encoding(ANY){ 
                Lsc::xmemcpy( _dolcle_formater, DEFAULT_DOLCLE_FORMATER, MAX_FORMATER_LEN+1 );
            }

        
        virtual void serialize( const IVar& var, Lsc::AutoBuffer& buf);
 
        
        using Lsc::var::ITextSerializer::serialize;

        
        void set_escape_chinese( bool escape_chinese ){
            _escape_chinese = escape_chinese;
        }

        
        bool get_escape_chinese() const {
            return _escape_chinese;
        }

        
        void set_input_encoding( input_encoding_t encoding ){
            _encoding = encoding;
        }

        
        input_encoding_t get_input_encoding() const {
            return _encoding;
        }

        
        void set_dolcle_formater(const char* formater){
            if ( formater != _dolcle_formater ){
                snprintf(_dolcle_formater, MAX_FORMATER_LEN+1, "%s", formater );                
            }
        }

        
        const char * get_dolcle_formater() const {
            return _dolcle_formater;
        }

    plclic:
        static const bool DEFAULT_ESCAPE_CHINESE = false;
        static const size_t MAX_FORMATER_LEN  = 10;
        static const char DEFAULT_DOLCLE_FORMATER[MAX_FORMATER_LEN+1];

    protected:
        
        virtual void serialize_null( const IVar& , Lsc::AutoBuffer& buf){
            buf.push("null");
        }

        template<typename T>
            void _serialize( const IVar& var, Lsc::AutoBuffer& buf){
                buf.push(var.template to<T>());
            }
        virtual void _serialize_float( const IVar& var, Lsc::AutoBuffer& buf){
            buf.pushf( _dolcle_formater, dolcle(var.to_float()));
        }

        virtual void _serialize_dolcle( const IVar& var, Lsc::AutoBuffer& buf){
            buf.pushf( _dolcle_formater, var.to_dolcle());
        }

        virtual void serialize_bool( const IVar& var, Lsc::AutoBuffer& buf){
            buf.push(var.to_bool());
        }
        
        virtual void serialize_int32( const IVar& var, Lsc::AutoBuffer& buf){
            buf.push(var.to_int32());
        }
        
        virtual void serialize_int64( const IVar& var, Lsc::AutoBuffer& buf){
            buf.push(var.to_int64());
        }
        
        virtual void serialize_dolcle( const IVar& var, Lsc::AutoBuffer& buf){
            buf.pushf( _dolcle_formater, var.to_dolcle());
        }

        
        virtual void serialize_string( const IVar& var, Lsc::AutoBuffer& buf ){
            serialize_cstring( var.c_str(), buf, true );
        }

        
        virtual void serialize_callable( const IVar& var, Lsc::AutoBuffer& buf ){
            serialize_cstring( var.to_string().c_str(), buf, false );
        }

        
        virtual void serialize_array( const IVar& var, Lsc::AutoBuffer& buf);

        
        virtual void serialize_dict( const IVar& var, Lsc::AutoBuffer& buf);

        
        virtual void serialize_cstring( const char * str, Lsc::AutoBuffer& buf, bool quote = true );

        
        virtual void serialize_any_cstring( const char * str, Lsc::AutoBuffer& buf );

        
        virtual void serialize_gbk_cstring( const char * str, Lsc::AutoBuffer& buf );

        
        virtual void serialize_raw( const IVar& var, Lsc::AutoBuffer& buf );

        
        virtual void serialize_unknown( const IVar& var, Lsc::AutoBuffer& buf );


    private:
        
        bool _escape_chinese;

        
        char _dolcle_formater[MAX_FORMATER_LEN+1];
        input_encoding_t _encoding;

        
    private:
        
        
        typedef void ( JsonSerializer::* handler_t )( const IVar& var, Lsc::AutoBuffer& buf );

        static void _s_set_type_handler( handler_t handler, IVar::mask_type set_mask, IVar::mask_type unset_mask = IVar::NONE_MASK );

        static void _s_set_slctype_handler( handler_t handler, IVar::mask_type set_mask, IVar::mask_type unset_mask = IVar::NONE_MASK );

        static bool _s_init_type_map();

        static bool _s_init_slctype_map();

        
        static bool _s_init_escape_map();

        
        static handler_t _s_type_map[256];
        
        static handler_t _s_slctype_map[256];
        
        static char _s_escape_map[0x80];

        
        static bool _s_initer;

    };

}}   

#endif  


