#ifndef  __BSL_VAR_BMLSERIALIZER_H_
#define  __BSL_VAR_BMLSERIALIZER_H_
#include <deque>
#include <Lsc/var/IVar.h>
#include <Lsc/var/IBinarySerializer.h>
#include <Lsc/bml/Parser.h>

namespace Lsc{ namespace var {

    class BmlSerializer: protected Lsc::bml::Parser, plclic IBinarySerializer {
    plclic:
        explicit BmlSerializer( const Lsc::string& bml )
            :_bml(bml){ }

        virtual ~BmlSerializer(){}

		virtual size_t serialize(const IVar& var, void* buf, size_t max_size);

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
        virtual void on_headword_m();
        virtual void on_headword_M();
        virtual void on_headword_n();
        virtual void on_headword_N();
        virtual size_t on_unknown_modifier(const char *bml);
        virtual size_t on_unknown_headword(const char *bml);

    private:

        struct var_stack_node_t{
            const Lsc::var::IVar* pvar;
            const Lsc::var::IVar* ploop_var;
            const char * struct_begin;
        };

        typedef std::vector<var_stack_node_t> var_stack_t;

        template<typename T, typename R>
        void _serialize_headword_common( R (Lsc::var::IVar::* pmethod )() const );

        const Lsc::var::IVar* _get_pvar();

        void _serialize_bits();

        void _serialize_char(bool append_trailing_zero);

        void _serialize_vint(long long num);

        void _serialize_uvint(unsigned long long num);

        void _serialize_mcpack(int version);
        var_stack_t         _var_stack;       
        var_stack_node_t    _var_frame;       
        Lsc::string         _bml;             
        char *              _buf_begin;       
        char *              _buf_cur;         
        char *              _buf_end;         
        unsigned long long  _bit_buf;         
        size_t              _bit_idx;         

    };
} }   
#endif  


