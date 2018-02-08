#ifndef  __BSL_VAR_BML_LEN_ADDER_H_
#define  __BSL_VAR_BML_LEN_ADDER_H_
#include <deque>
#include <Lsc/var/IVar.h>
#include <Lsc/var/IBinarySerializer.h>
#include <Lsc/var/BmlParser.h>

namespace Lsc{ namespace var {

    class BmlLenAdder: protected BmlParser, plclic IBinarySerializer {
    plclic:
        explicit BmlLenAdder( const Lsc::string& bml )
            :_bml(bml){ }

        virtual ~BmlLenAdder(){}

		virtual void modify(IVar& var);

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
        virtual void on_headword_n();
        virtual void on_headword_N();
        virtual size_t on_unknown_modifier(const char *bml);
        virtual size_t on_unknown_headword(const char *bml);

    private:

        struct var_stack_node_t{
            Lsc::var::IVar* pvar;
            Lsc::var::IVar* ploop_var;
        };

        typedef std::vector<var_stack_node_t> var_stack_t;

        typedef std::deque<Lsc::var::IVar*>   var_queue_t;
        struct var_queue_arr_node_t{
            var_queue_arr_node_t()
                :queue(), last(NULL) {}
            std::deque<Lsc::var::IVar*> queue;
            Lsc::var::IVar*             last;
        };

        
        void push_var( size_t queue_idx, Lsc::var::IVar* pvar ){
            if ( queue_idx >= _var_queue_arr.size() ){
                _var_queue_arr.resize( queue_idx + 1 );
            }
            _var_queue_arr[queue_idx].queue.push_back(pvar);
        }

        
        size_t pop_var( size_t queue_idx ){
            if ( queue_idx >= _var_queue_arr.size() ){
                return size_t(-1);
            }
            
            var_queue_arr_node_t& node = _var_queue_arr[queue_idx];
            if ( node.queue.empty() ){
                return node.last;
            }

            node.last = node.queue.front();
            node.queue.pop_front();
            return node.last;
        }

        
        bool exist_var_queue( size_t queue_idx ){
            if ( queue_idx >= _var_queue_arr.size() ){
                return false;
            }
            var_queue_arr_node_t& node = _var_queue_arr[queue_idx];
            return node.last != size_t(-1) || !node.queue.empty();

        }

        
        void clear_var_queues(){
            _var_queue_arr.clear();
        }

        var_stack_t         _var_stack;       
        var_stack_node_t    _var_frame;       

        std::deque<var_queue_arr_node_t> _var_queue_arr;
        Lsc::string         _bml;             

    };
} }   
#endif  


