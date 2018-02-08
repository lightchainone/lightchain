#ifndef  __BSL_VAR_SHALLOW_BML_DESERIALIZER_H_
#define  __BSL_VAR_SHALLOW_BML_DESERIALIZER_H_
#include <deque>
#include <Lsc/var/IVar.h>
#include <Lsc/var/Null.h>
#include <Lsc/var/IBinaryDeserializer.h>
#include <Lsc/bml/Parser.h>

namespace Lsc{ namespace var {

    class BmlDeserializer: protected Lsc::bml::Parser, plclic IBinaryDeserializer {
    plclic:
        
        
        enum copy_option_t{
            AUTO,          
            SHALLOW,       
            DEEP,          
        };

        
        BmlDeserializer( const Lsc::string& bml, Lsc::ResourcePool& rp )
            :_bml(bml), _rp(rp), _copy_opt(AUTO) { }

        
        void set_copy_option( copy_option_t opt ){
            _copy_opt = opt;
        }

        copy_option_t get_copy_option() const {
            return _copy_opt;
        }

        virtual ~BmlDeserializer(){}

        
        virtual size_t try_deserialize( const void * buf, size_t max_size );

        
        Lsc::var::IVar& get_result() const {
            if ( _var_frame.pvar ){
                return *_var_frame.pvar;
            }else{
                return Lsc::var::Null::null;
            }
        }

		
		virtual Lsc::var::IVar& deserialize(const void* buf, size_t max_size){
            size_t res = try_deserialize( buf, max_size );
            if ( res > max_size ){
                throw Lsc::OutOfBoundException()<<BSL_EARG<<"buffer too small! max_size["<<max_size<<"] expect[>="<<res<<"]";
            }
            return get_result();
        }
    
    protected:
        virtual void on_headword_lparen();
        virtual void on_headword_rparen();
        virtual void on_headword_pad();
        virtual void on_headword_align();
        virtual void on_headword_u();
        virtual void on_headword_b();
        virtual void on_headword_B();
        virtual void on_headword_c();
        virtual void on_headword_C();
        virtual void on_headword_t();
        virtual void on_headword_T();
        virtual void on_headword_h();
        virtual void on_headword_H();
        virtual void on_headword_i();
        virtual void on_headword_I();
        virtual void on_headword_l();
        virtual void on_headword_L();
        virtual void on_headword_f();
        virtual void on_headword_d();
        virtual void on_headword_v();
        virtual void on_headword_V();
        virtual void on_headword_s();
        virtual void on_headword_S();
        virtual void on_headword_r();
        virtual void on_headword_R();
        virtual void on_headword_m(){
            return _deserialize_mcpack();
        }
        virtual void on_headword_M(){
            return _deserialize_mcpack();
        }
        virtual void on_headword_n();
        virtual void on_headword_N();
        virtual size_t on_unknown_modifier(const char *bml);
        virtual size_t on_unknown_headword(const char *bml);

    private:

        
        struct var_stack_node_t{
            Lsc::var::IVar* pvar;
            Lsc::var::IVar* ploop_var;
            const char * struct_begin;
            const char * struct_name;
            size_t       struct_name_len;
        };

        typedef std::vector<var_stack_node_t> var_stack_t;

        
        template<typename VarT>
            void _deserialize_string();

        void _unrefill_bit_buf();

        unsigned long long _deserialize_bits(size_t len);

        long long _deserialize_vint();

        unsigned long long _deserialize_uvint();

        void _deserialize_char(bool as_string);

        void _deserialize_raw(bool shallow_copy);

        bool _need_deserialization();

        void _set_var(Lsc::var::IVar& var );

        template<typename T>
            void _enqueue( size_t len );

        void _dry_enqueue( size_t len );

        template<typename T, typename VarT>
            void _deserialize_headword_number();

        void _deserialize_mcpack();

        
        Lsc::string         _bml;             
        Lsc::ResourcePool&  _rp;              
        var_stack_t         _var_stack;       
        var_stack_node_t    _var_frame;       
        const char *        _buf_begin;       
        const char *        _buf_cur;         
        const char *        _buf_end;         
        unsigned long long  _bit_buf;         
        size_t              _bit_idx;         
        size_t              _bit_max;         
        copy_option_t       _copy_opt;        

    };
} }   
#endif  


