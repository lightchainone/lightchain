#include <Lsc/var/VPathEvaluator.h>

namespace Lsc{
namespace var{
    void VPathEvaluator::eval_path_expr(){
        skip_white();
        const char * last_pos;
        do{
            last_pos = _path;
            eval_step_expr();    
        }while( _path != last_pos );
    }

    void VPathEvaluator::eval_step_expr(){
        bool empty = true;

        while(true){
            skip_white();
            switch ( *_path ){
                case '.':
                    empty = false;
                    ++ _path;
                    if ( eval_identifier() ){
                        
                        _p_var = &_p_var->get(_str); 
                    }else{
                        
                        if ( *_path == '.' ){
                            throw Lsc::ParseErrorException()<<BSL_EARG<<"expect: <identifier>";
                        }
                    }
                    break;

                case '[':
                    empty = false;
                    ++ _path;
                    eval_field_expr();
                    skip_white();
                    if ( *_path != ']' ){
                        throw Lsc::ParseErrorException()<<BSL_EARG<<"expect:]";
                    }
                    ++ _path;
                    break;

                default:
                    if ( empty ){
                        if ( eval_identifier() ){
                            empty = false;
                            _p_var = &_p_var->get(_str); 
                        }else{
                            
                            
                            return;
                        }
                    }else{
                        
                        return;
                    }
            }
        }

    }

    void VPathEvaluator::eval_field_expr(){
        skip_white();
        if ( '0' <= *_path && *_path <= '9' ){
            _p_var = &_p_var->get(eval_number_literal());
        }else if ( *_path == '\'' || *_path == '\"' ){
            eval_string_literal();
            _p_var = &_p_var->get(_str);
        }else{
            
            _has_slc_query = true;
            VPathEvaluator slc_evaluator;
            try{
                const IVar& var = slc_evaluator.eval( _path, *_ori_p_var ); 
                if ( var.is_string() ){
                    _p_var = &_p_var->get(var.to_string());
                }else{
                    _p_var = &_p_var->get(var.to_int32());
                }
            }catch(...){
                
                _path += slc_evaluator.current_pos();
                throw;
            }
            _path += slc_evaluator.current_pos();
        }
    }

    bool VPathEvaluator::eval_identifier(){
        skip_white();
        const char * begin_pos = _path;
        
        if ( *_path != '_' && 
                !( 'a' <= *_path && *_path <= 'z' ) && 
                !( 'A' <= *_path && *_path <= 'Z' ) ){
            return false;
        }

        
        do{ 
            ++_path;
        }while( *_path == '_' || 
                'a' <= *_path && *_path <= 'z' || 
                'A' <= *_path && *_path <= 'Z' || 
                '0' <= *_path && *_path <= '9' );

        _str.clear();
        _str.append(begin_pos, _path - begin_pos );
        return true;
    }

    void VPathEvaluator::eval_string_literal(){
        const char *begin_pos  = _path;  
        do{
            ++ _path;
        }while( *_path && *_path != *begin_pos );

        if ( *_path == *begin_pos ){
            ++ _path;
            _str.clear();
            _str.append( begin_pos + 1, (_path - 1) - (begin_pos+1) );
        }else{
            
            throw Lsc::ParseErrorException()<<BSL_EARG<<"unexpected <EOF>. expect:"<<*begin_pos;
        }
    }

    VPathEvaluator::index_type VPathEvaluator::eval_number_literal(){
        VPathEvaluator::index_type num = 0;
        for( ; '0' <= *_path && *_path <= '9'; ++ _path ){
            num = num * 10 + *_path - '0';
        }
        return num;
    }

}}   


