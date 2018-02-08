#ifndef  __BSL_VAR_BMLPARSER_H_
#define  __BSL_VAR_BMLPARSER_H_
#include <vector>
#include <deque>

namespace Lsc{ namespace bml{
    
    inline size_t str_to_size_t(const char *s, const char **end){
        size_t res = 0;
        if ( NULL != s ){
            for( ; '0' <= *s && *s <= '9'; ++ s ){
                res = (res << 3) + (res << 1) + *s - '0';   
            }
        }
        if ( NULL != end ){
            *end = s;
        }
        return res;
    }

    class Parser{
    protected:
        
        struct stack_node_t{
            
            size_t index;               
            
            
            size_t struct_index;        
            size_t struct_count;        
            size_t struct_static_count; 
            const char *struct_bml;     

            bool struct_is_dict;        
            bool vary_struct_size;      
        };

        typedef std::vector<stack_node_t> stack_t;

        enum modifier_base_t{
            MODIFIER_LEN       = 1,
            MODIFIER_NAME      = 2,
            MODIFIER_ENQUEUE   = 4,
            MODIFIER_DEQUEUE   = 8,
            MODIFIER_DEREFERENCE=16,
            MODIFIER_ARRAY     = 32,
            MODIFIER_DICT      = 64,
        };

        struct modifier_t{
            size_t mask;                
            size_t dynamic_len;         
            size_t static_len;          
            const char *name;           
            size_t name_len;            
            size_t enqueue_id;         
            size_t dequeue_id;         
            size_t dereference_id;     
        };
            
        
        
        Parser()
            :_stack_size(0), _throw_on_dequeue_error(true){}

        
        virtual ~Parser(){}

        
        virtual void parse( const char *bml ); 

        
        void set_throw_on_dequeue_error( bool value ){
            _throw_on_dequeue_error = value;
        }

        
        bool get_throw_on_dequeue_error() const {
            return _throw_on_dequeue_error;
        }

        
        virtual void on_headword_lparen() = 0;
        
        virtual void on_headword_rparen() = 0;
        
        virtual void on_headword_pad() = 0;
        
        virtual void on_headword_align() = 0;
        
        virtual void on_headword_u() = 0;
        
        virtual void on_headword_b() = 0;
        
        virtual void on_headword_B() = 0;
        
        virtual void on_headword_c() = 0;
        
        virtual void on_headword_C() = 0;
        
        virtual void on_headword_t() = 0;
        
        virtual void on_headword_T() = 0;
        
        virtual void on_headword_h() = 0;
        
        virtual void on_headword_H() = 0;
        
        virtual void on_headword_i() = 0;
        
        virtual void on_headword_I() = 0;
        
        virtual void on_headword_l() = 0;
        
        virtual void on_headword_L() = 0;
        
        virtual void on_headword_f() = 0;
        
        virtual void on_headword_d() = 0;
        
        virtual void on_headword_v() = 0;
        
        virtual void on_headword_V() = 0;
        
        virtual void on_headword_s() = 0;
        
        virtual void on_headword_S() = 0;
        
        virtual void on_headword_r() = 0;
        
        virtual void on_headword_R() = 0;

        
        virtual void on_headword_m() = 0;

        
        virtual void on_headword_M() = 0;

    
        
        virtual void on_headword_n() = 0;
        
        virtual void on_headword_N() = 0;
        
        virtual size_t on_unknown_modifier(const char *bml) = 0;
        
        virtual size_t on_unknown_headword(const char *bml) = 0;

        
        void push_len( size_t queue_idx, size_t len ){
            if ( queue_idx >= _len_queue_arr.size() ){
                _len_queue_arr.resize( queue_idx + 1 );
            }
            _len_queue_arr[queue_idx].queue.push_back(len);
        }

        
        size_t pop_len( size_t queue_idx ){
            if ( queue_idx >= _len_queue_arr.size() ){
                return size_t(-1);
            }
            
            len_queue_arr_node_t& node = _len_queue_arr[queue_idx];
            if ( node.queue.empty() ){
                return node.last;
            }

            node.last = node.queue.front();
            node.queue.pop_front();
            return node.last;
        }

        
        bool exist_len_queue( size_t queue_idx ){
            if ( queue_idx >= _len_queue_arr.size() ){
                return false;
            }
            len_queue_arr_node_t& node = _len_queue_arr[queue_idx];
            return node.last != size_t(-1) || !node.queue.empty();

        }

        
        size_t len_queue_size( size_t queue_idx ){
            if ( queue_idx >= _len_queue_arr.size() ){
                return 0;
            }
            len_queue_arr_node_t& node = _len_queue_arr[queue_idx];
            return node.queue.size();
        }

        
        void clear_len_queues(){
            _len_queue_arr.clear();
        }

        
        void push_bml( size_t queue_idx, const char *bml ){
            if ( queue_idx >= _bml_queue_arr.size() ){
                _bml_queue_arr.resize( queue_idx + 1 );
            }
            _bml_queue_arr[queue_idx].queue.push_back(bml);
        }

        
        const char * pop_bml( size_t queue_idx ){
            if ( queue_idx >= _bml_queue_arr.size() ){
                return NULL;
            }
            
            bml_queue_arr_node_t& node = _bml_queue_arr[queue_idx];
            if ( node.queue.empty() ){
                return node.last;
            }

            node.last = node.queue.front();
            node.queue.pop_front();
            return node.last;
        }

        
        bool exist_bml_queue( size_t queue_idx ){
            if ( queue_idx >= _bml_queue_arr.size() ){
                return false;
            }
            bml_queue_arr_node_t& node = _bml_queue_arr[queue_idx];
            return node.last != NULL || !node.queue.empty();
        }

        
        size_t bml_queue_size( size_t queue_idx ){
            if ( queue_idx >= _bml_queue_arr.size() ){
                return 0;
            }
            bml_queue_arr_node_t& node = _bml_queue_arr[queue_idx];
            return node.queue.size();
        }

        void clear_bml_queues(){
            _bml_queue_arr.clear();
        }

        
        stack_t         _stack;
        stack_node_t    _frame;
        modifier_t      _modifier;
        size_t          _stack_size;

    private:

        struct len_queue_arr_node_t{
            len_queue_arr_node_t()
                :queue(), last(size_t(-1)) {}
            std::deque<size_t>  queue;
            size_t              last;
        };
        struct bml_queue_arr_node_t{
            bml_queue_arr_node_t()
                :queue(), last(NULL) {}
            std::deque<const char *>    queue;
            const char *                last;
        };

        std::deque<len_queue_arr_node_t>    _len_queue_arr;
        std::deque<bml_queue_arr_node_t>    _bml_queue_arr;

        bool            _throw_on_dequeue_error;
    };

} } 


#endif  


