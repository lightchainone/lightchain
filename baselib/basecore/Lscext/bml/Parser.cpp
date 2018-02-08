#include <Lsc/exception/Lsc_exception.h>
#include "Parser.h"

namespace Lsc{ namespace bml{

    void Parser::parse( const char *bml ){
        static const void * labels[256] = {
            
             &&end_of_file,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&modifier_dict,
             &&modifier_enqueue,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&headword_lparen,
             &&headword_rparen,
             &&modifier_dereference,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&modifier_len,
             &&modifier_len,
             &&modifier_len,
             &&modifier_len,
             &&modifier_len,
             &&modifier_len,
             &&modifier_len,
             &&modifier_len,
             &&modifier_len,
             &&modifier_len,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&modifier_array,
             &&unknown_headword,
             &&headword_B,
             &&headword_C,
             &&unknown_headword,
             &&unknown_headword,
             &&unknown_headword,
             &&unknown_headword,
             &&headword_H,
             &&headword_I,
             &&unknown_headword,
             &&unknown_headword,
             &&headword_L,
             &&headword_M,
             &&headword_N,
             &&unknown_headword,
             &&unknown_headword,
             &&unknown_headword,
             &&headword_R,
             &&headword_S,
             &&headword_T,
             &&unknown_headword,
             &&headword_V,
             &&unknown_headword,
             &&unknown_headword,
             &&unknown_headword,
             &&unknown_headword,
             &&modifier_name,
             &&unknown_modifier,
             &&unknown_modifier,
             &&modifier_dequeue,
             &&headword_pad,
             &&unknown_modifier,
             &&unknown_headword,
             &&headword_b,
             &&headword_c,
             &&headword_d,
             &&unknown_headword,
             &&headword_f,
             &&unknown_headword,
             &&headword_h,
             &&headword_i,
             &&unknown_headword,
             &&unknown_headword,
             &&headword_l,
             &&headword_m,
             &&headword_n,
             &&unknown_headword,
             &&unknown_headword,
             &&unknown_headword,
             &&headword_r,
             &&headword_s,
             &&headword_t,
             &&headword_u,
             &&headword_v,
             &&unknown_headword,
             &&unknown_headword,
             &&unknown_headword,
             &&unknown_headword,
             &&unknown_modifier,
             &&headword_align,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
             &&unknown_modifier,
        };

        const char *cur = bml;
        _frame.struct_bml = bml;
        _frame.index = 0;
        _frame.struct_index = 0;
        _frame.struct_count = 0;
        _frame.struct_static_count = 0;
        _frame.struct_is_dict = false;
        _frame.vary_struct_size = false;

        _stack.clear();
        _stack_size = 0;

        _modifier.mask = 0;

        while(true){
            goto *labels[static_cast<unsigned char>(*cur)];
            
modifier_array:         
            ++ cur;
            _modifier.mask |= MODIFIER_ARRAY;
            continue;
modifier_dict:          
            ++ cur;
            _modifier.mask |= MODIFIER_DICT;
            continue;
modifier_enqueue:       
            ++ cur;
            _modifier.enqueue_id = str_to_size_t( cur, &cur );  
            _modifier.mask |= MODIFIER_ENQUEUE;
            continue;

modifier_dequeue:       
            ++ cur;
            _modifier.dequeue_id = str_to_size_t( cur, &cur );  
            _modifier.dynamic_len = pop_len( _modifier.dequeue_id );
            if ( _modifier.dynamic_len == size_t(-1) ){
                if ( _throw_on_dequeue_error ){
                    throw Lsc::OutOfBoundException()<<BSL_EARG<<"queue["<<_modifier.dequeue_id<<"] is empty!";
                }
            }

            _modifier.mask |= MODIFIER_DEQUEUE;
            _frame.vary_struct_size = true;
            continue;

modifier_dereference:
            ++ cur;
            _modifier.dereference_id = str_to_size_t( cur, &cur );  
            _modifier.mask |= MODIFIER_DEREFERENCE;
            continue;

modifier_len:           
            _modifier.static_len = str_to_size_t( cur, &cur );
            _modifier.mask |= MODIFIER_LEN;
            continue;

modifier_name:     
            _modifier.name = cur + 1;
            while( *cur && *cur != ']' ){
                ++ cur;
            }
            if ( '\0' == *cur ){
                throw Lsc::ParseErrorException()<<BSL_EARG<<"unexpected EOF! expect: ]";
            }
            _modifier.name_len = cur - _modifier.name;
            _modifier.mask |= MODIFIER_NAME;
            ++ cur; 
            continue;

            
headword_u:
            on_headword_u();
            
            _modifier.mask = 0;
            ++ _frame.index;
            ++ cur;
            if ( _stack_size ){
                continue;
            }else{
                break;
            }

headword_b:             
            on_headword_b();

            
            _modifier.mask = 0;
            ++ _frame.index;
            ++ cur;
            if ( _stack_size ){
                continue;
            }else{
                break;
            }
headword_c:             
            on_headword_c();

            
            _modifier.mask = 0;
            ++ _frame.index;
            ++ cur;
            if ( _stack_size ){
                continue;
            }else{
                break;
            }

headword_C:             
            on_headword_C();

            
            _modifier.mask = 0;
            ++ _frame.index;
            ++ cur;
            if ( _stack_size ){
                continue;
            }else{
                break;
            }

headword_B:             
            on_headword_B();

            
            _modifier.mask = 0;
            ++ _frame.index;
            ++ cur;
            if ( _stack_size ){
                continue;
            }else{
                break;
            }
headword_t:             
            on_headword_t();

            
            _modifier.mask = 0;
            ++ _frame.index;
            ++ cur;
            if ( _stack_size ){
                continue;
            }else{
                break;
            }
headword_T:             
            on_headword_T();

            
            _modifier.mask = 0;
            ++ _frame.index;
            ++ cur;
            if ( _stack_size ){
                continue;
            }else{
                break;
            }
headword_h:             
            on_headword_h();

            
            _modifier.mask = 0;
            ++ _frame.index;
            ++ cur;
            if ( _stack_size ){
                continue;
            }else{
                break;
            }
headword_H:             
            on_headword_H();

            
            _modifier.mask = 0;
            ++ _frame.index;
            ++ cur;
            if ( _stack_size ){
                continue;
            }else{
                break;
            }
headword_i:             
            on_headword_i();

            
            _modifier.mask = 0;
            ++ _frame.index;
            ++ cur;
            if ( _stack_size ){
                continue;
            }else{
                break;
            }
headword_I:             
            on_headword_I();

            
            _modifier.mask = 0;
            ++ _frame.index;
            ++ cur;
            if ( _stack_size ){
                continue;
            }else{
                break;
            }
headword_l:             
            on_headword_l();

            
            _modifier.mask = 0;
            ++ _frame.index;
            ++ cur;
            if ( _stack_size ){
                continue;
            }else{
                break;
            }
headword_L:             
            on_headword_L();

            
            _modifier.mask = 0;
            ++ _frame.index;
            ++ cur;
            if ( _stack_size ){
                continue;
            }else{
                break;
            }
headword_f:             
            on_headword_f();

            
            _modifier.mask = 0;
            ++ _frame.index;
            ++ cur;
            if ( _stack_size ){
                continue;
            }else{
                break;
            }
headword_d:             
            on_headword_d();

            
            _modifier.mask = 0;
            ++ _frame.index;
            ++ cur;
            if ( _stack_size ){
                continue;
            }else{
                break;
            }
headword_pad:           
            on_headword_pad();

            
            _modifier.mask = 0;
            ++ cur;
            if ( _stack_size ){
                continue;
            }else{
                break;
            }
headword_n:
            on_headword_n();

            
            _modifier.mask = 0;
            ++ _frame.index;
            ++ cur;
            if ( _stack_size ){
                continue;
            }else{
                break;
            }
headword_N:
            on_headword_N();

            
            _modifier.mask = 0;
            ++ _frame.index;
            ++ cur;
            if ( _stack_size ){
                continue;
            }else{
                break;
            }

headword_align:         
            
            on_headword_align();

            
            _modifier.mask = 0;
            ++ cur;
            if ( _stack_size ){
                continue;   
            }else{
                break;      
            }

headword_lparen:      
            on_headword_lparen();

            
            _stack.push_back(_frame);
            ++ _stack_size;

            
            if ( _modifier.mask & MODIFIER_LEN ){
                if ( _modifier.mask & MODIFIER_DEQUEUE ){
                    
                    _frame.struct_count         = _modifier.dynamic_len;
                    _frame.struct_static_count  = _modifier.static_len; 
                    if ( _frame.struct_static_count < _frame.struct_count ){
                        throw Lsc::OutOfBoundException()<<BSL_EARG<<"len["<<_frame.struct_count<<"] < static_len["<<_frame.struct_static_count<<"]";
                    }
                }else{
                    
                    _frame.struct_count         = _modifier.static_len;
                    _frame.struct_static_count  = _modifier.static_len; 
                }
            }else if ( _modifier.mask & MODIFIER_DEQUEUE ){
                
                _frame.struct_count         = _modifier.dynamic_len;
                _frame.struct_static_count  = _modifier.dynamic_len; 
            }else{ 
                _frame.struct_count         = 1;
                _frame.struct_static_count  = 1;
            }

            _frame.struct_bml = cur + 1;
            _frame.struct_index = 0;
            _frame.struct_is_dict = (( _modifier.mask & MODIFIER_DICT ) != 0 );
            _frame.vary_struct_size = false;

            _frame.index = 0;
            _modifier.mask = 0;

            ++ cur;
            continue;

headword_rparen:     
            if ( 0 == _stack_size ){
                throw Lsc::ParseErrorException()<<BSL_EARG<<"unexpected ')'!";
            }
            on_headword_rparen();

            ++ _frame.struct_index;
            if ( _frame.struct_index < _frame.struct_count ){
                
                _frame.index = 0;
                _modifier.mask = 0;
                cur         = _frame.struct_bml;
                continue;

            }else{
                
                stack_node_t& parent_frame = _stack.back();

                
                _frame = parent_frame;
                _stack.pop_back();
                -- _stack_size;

                
                _modifier.mask = 0;
                ++ _frame.index;
                ++ cur;
                if ( _stack_size ){
                    continue;   
                }else{
                    break;      
                }
            }

            
headword_v:             
            _frame.vary_struct_size = true;
            on_headword_v();

            
            _modifier.mask = 0;
            ++ _frame.index;
            ++ cur;
            if ( _stack_size ){
                continue;   
            }else{
                break;      
            }

headword_V:             
            _frame.vary_struct_size = true;
            on_headword_V();

            
            _modifier.mask = 0;
            ++ _frame.index;
            ++ cur;
            if ( _stack_size ){
                continue;   
            }else{
                break;      
            }

headword_s:             
            _frame.vary_struct_size = true;
            on_headword_s();

            
            _modifier.mask = 0;
            ++ _frame.index;
            ++ cur;
            if ( _stack_size ){
                continue;   
            }else{
                break;      
            }

headword_S:             
            _frame.vary_struct_size = true;
            on_headword_S();

            
            _modifier.mask = 0;
            ++ _frame.index;
            ++ cur;
            if ( _stack_size ){
                continue;   
            }else{
                break;      
            }

headword_r:             
            _frame.vary_struct_size = true;
            on_headword_r();

            
            _modifier.mask = 0;
            ++ _frame.index;
            ++ cur;
            if ( _stack_size ){
                continue;   
            }else{
                break;      
            }

headword_R:             
            _frame.vary_struct_size = true;
            on_headword_R();

            
            _modifier.mask = 0;
            ++ _frame.index;
            ++ cur;
            if ( _stack_size ){
                continue;   
            }else{
                break;      
            }

headword_m:             
            _frame.vary_struct_size = true;
            on_headword_m();

            
            _modifier.mask = 0;
            ++ _frame.index;
            ++ cur;
            if ( _stack_size ){
                continue;   
            }else{
                break;      
            }
headword_M:             
            _frame.vary_struct_size = true;
            on_headword_M();

            
            _modifier.mask = 0;
            ++ _frame.index;
            ++ cur;
            if ( _stack_size ){
                continue;   
            }else{
                break;      
            }

end_of_file:
            if ( _stack_size ){
                throw Lsc::ParseErrorException()<<BSL_EARG<<"unexpected EOF! expect: ')'";
            }else{
                break;
            }

unknown_modifier:
            cur += on_unknown_modifier(cur); 
            continue;

unknown_headword:
            cur += on_unknown_headword(cur); 
            _modifier.mask = 0;
            ++ _frame.index;
            if ( _stack_size ){
                continue;   
            }else{
                break;      
            }
        }   

    } 

} } 

