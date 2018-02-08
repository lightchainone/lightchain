
#include<deque>
#include<Lsc/exception/Lsc_exception.h>
#include "static_size.h"
#include "Parser.h"

namespace Lsc{ namespace bml {

    
    struct node_t{
        size_t bits;
        size_t len;
    };

    int static_size( const char * bml ){
        static const void * labels[256] = {
            
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&modifier_dict,
             &&modifier_enqueue,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&headword_lparen,
             &&headword_rparen,
             &&modifier_dereference,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
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
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&modifier_array,
             &&unexpected,
             &&headword_B,
             &&headword_C,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&headword_H,
             &&headword_I,
             &&unexpected,
             &&unexpected,
             &&headword_L,
             &&unexpected,
             &&headword_N,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&headword_R,
             &&headword_S,
             &&headword_T,
             &&unexpected,
             &&headword_V,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&modifier_name,
             &&unexpected,
             &&unexpected,
             &&modifier_dequeue,
             &&headword_pad,
             &&unexpected,
             &&unexpected,
             &&headword_b,
             &&headword_c,
             &&headword_d,
             &&unexpected,
             &&headword_f,
             &&unexpected,
             &&headword_h,
             &&headword_i,
             &&unexpected,
             &&unexpected,
             &&headword_l,
             &&unexpected,
             &&headword_n,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&headword_r,
             &&headword_s,
             &&headword_t,
             &&unexpected,
             &&headword_v,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&headword_align,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
             &&unexpected,
        };
        if ( !bml ){
            throw Lsc::NullPointerException()<<BSL_EARG<<"bml is NULL!";
        }
        const int RES_VARY = -1;
        const char * cur = bml;
        size_t bits = 0;    
        size_t len = 1;    
        std::deque<node_t> stack;
        size_t stack_size = 0;

        while(true){
            goto *labels[static_cast<unsigned char>(*cur)];

modifier_array:         
modifier_dict:          
modifier_enqueue:       
            
            ++ cur;
            while( '0' <= *cur && *cur <= '9' ){
                ++cur;
            }
            continue;
modifier_dequeue:       
modifier_dereference:   
            return RES_VARY;
modifier_len:           
            len = str_to_size_t( cur, &cur );
            continue;
modifier_name:     
            while( *cur && *cur != ']' ){
                ++ cur;
            }
            if ( !cur ){
                throw Lsc::ParseErrorException()<<BSL_EARG<<"unexpected EOF! expect: ]";
            }
            ++ cur; 
            continue;


headword_b:             
            bits += len;
            len = 1;
            ++ cur;
            if ( stack_size ){
                continue;
            }else{
                break;
            }
headword_B:             
headword_c:             
headword_C:             
headword_t:             
headword_T:             
            len <<= 3;  
            goto headword_common;
headword_h:             
headword_H:             
            len <<= 4;  
            goto headword_common;
headword_i:             
headword_I:             
            len <<= 5;  
            goto headword_common;
headword_l:             
headword_L:             
            len <<= 6;  
            goto headword_common;
headword_f:             
            len <<= 5;  
            goto headword_common;
headword_d:             
            len <<= 6;  
            goto headword_common;
headword_pad:           
            len <<= 3;  
            goto headword_common;
headword_common:
            bits += len;
            len  = 1;   
            bits = (bits+7) & -8;   
            ++ cur; 
            if ( stack_size ){
                continue;   
            }else{
                break;      
            }

headword_n:
headword_N:
            ++ cur; 
            if ( stack_size ){
                continue;   
            }else{
                break;      
            }

headword_align:         
            bits = (bits + (len<< 3) -1) & (-len << 3);    
            len = 1;
            ++ cur; 
            if ( stack_size ){
                continue;   
            }else{
                break;      
            }

headword_lparen:      
            bits = (bits+7) & -8;   
            node_t node;
            node.bits   = bits;
            node.len    = len;  
            stack.push_back(node);  
            ++ stack_size;
            bits        = 0;
            len         = 1;
            ++ cur;
            continue;

headword_rparen:     
            bits = (bits+7) & -8;   
            if ( stack_size == 0 ){
                throw Lsc::ParseErrorException()<<BSL_EARG<<"unexpected ')' token!";
            }
            bits = stack.back().bits + stack.back().len * bits;
            stack.pop_back();
            -- stack_size;
            len  = 1;
            ++ cur;
            if ( stack_size ){
                continue;   
            }else{
                break;      
            }



headword_v:             
headword_V:             
headword_s:             
headword_S:             
headword_r:             
headword_R:             
            return RES_VARY;
unexpected:
            throw Lsc::ParseErrorException()<<BSL_EARG<<"unexpected '"<<*cur<<"'!";
        }   

        bits = (bits+7) & -8;   
        return bits >> 3;       
    }
} } 

