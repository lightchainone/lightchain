#include <Lsc/var/JsonSerializer.h>
#include <Lsc/var/VTplEvaluator.h>
#include <Lsc/var/VScriptEvaluator.h>

namespace Lsc{
namespace var{

    void VTplEvaluator::eval( const char * script, IVar& data, AutoBuffer& buf ){
        if ( script == NULL ){
            throw Lsc::NullPointerException()<<BSL_EARG<<"script can't be NULL!";
        }
        _script = _cur = script;
        _pdata  = &data;
        _pbuf   = &buf;

        try{
            eval_vtpl();
        }catch(Lsc::Exception& e){
            const char * begin = _cur - 30 > _script ? _cur - 30 : _script; 
            e.pushf( "{VTplEvaluator pos[%d] vtpl[", 
                    int(current_pos()) );
            e.push( begin, _cur - begin );
            e.pushf( "~!~%.20s] }", _cur );
            throw;
        }
    }

    void VTplEvaluator::eval_vtpl(){
        const char * pchr = NULL;  
        VScriptEvaluator evaluator(_rp);
        while( pchr = strchr( _cur, '$' ), pchr!=NULL ){
            
            try{
                
                _pbuf->push( _cur, pchr - _cur );

                if ( *(pchr+1) == '$' ){  
                    _pbuf->push('$');
                    _cur = pchr + 2;
                    continue;
                }

                
                
                _cur = pchr + 1;
                try{
                    IVar& res = evaluator.eval(_cur, *_pdata);  
                    
                    serialize(res); 
                }catch(...){
                    
                    _cur += evaluator.current_pos();
                    throw;
                }

                _cur += evaluator.current_pos();
                if ( *_cur == '#' ){  
                    pchr = strchr( _cur, '$' );
                    if ( pchr == NULL ){
                        throw ParseErrorException()<<BSL_EARG<<"expect: $";
                    }else{
                        _cur = pchr + 1;
                        
                    }
                }else if ( *_cur == '$' ){
                    ++ _cur;
                    
                }else{
                    
                    throw ParseErrorException()<<BSL_EARG<<"expect: $";
                }
            }catch(Lsc::Exception& e){
                if ( _debug ){  
                    
                    _pbuf->pushf(
                            "<pre>\nERROR: %s was thrown!\npos[%d] what[%s]\nfile[%s] line[%d] function[%s]\n</pre>",
                            e.name(), int(_cur - _script), e.what(), 
                            e.file(), int(e.line()), e.function()
                            );
                    
                    pchr = strchr( _cur, '$' );
                    if ( pchr == NULL ){
                        
                        
                        
                        return;
                    }else{
                        
                        
                        _cur = pchr + 1;
                    }
                            
                }else{
                    
                    throw;
                }
            }
        }
        _pbuf->push( _cur );  
    }

    void VTplEvaluator::serialize(IVar& res){
        if ( !res.is_null() ){
            if ( res.is_array() || res.is_dict() ){
                JsonSerializer js;
                js.serialize(res, *_pbuf);  
            }else if ( res.is_string() ){
                _pbuf->push( res.c_str(), res.c_str_len() ) ;
            }else{
                const Lsc::var::IVar::string_type& str = res.to_string();
                _pbuf->push( str.c_str(), str.length() );
            }
        }
    }

}}   


