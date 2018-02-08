#ifndef  __BSL_VAR_VPATHEVALUATOR_H__
#define  __BSL_VAR_VPATHEVALUATOR_H__
#include <Lsc/var/IVar.h>
#include <Lsc/var/Ref.h>

namespace Lsc{
namespace var{
    
    class VPathEvaluator{
    plclic:
        typedef IVar::field_type field_type;
        typedef size_t           index_type;

        
        VPathEvaluator()
            : _p_var(NULL), _ori_p_var(NULL), _path(NULL), _ori_path(NULL), _has_slc_query(false) {}

        IVar& eval( const char * path,  IVar& var ){
            return const_cast<IVar&>(eval( path, const_cast<const IVar&>(var) ));
        }
        
        const IVar& eval( const char * path,  const IVar& data ){
            if ( !path ){
                throw Lsc::NullPointerException()<<BSL_EARG<<"path can't be NULL";
            }
            _p_var = _ori_p_var = &data;
            _path  = _ori_path = path;
            _has_slc_query = false;
            try{
                eval_path_expr();
            }catch( Lsc::Exception& e ){
                const char * begin = _path - 30 > _ori_path ? _path - 30 : _ori_path; 
                e.pushf( "{VPathEvaluator pos[%d] &data[%p] &current_var[%p] vpath[", 
                        int(current_pos()), &data, _p_var );
                e.push( begin, _path - begin );
                e.pushf( "~!~%.20s] }", _path );
                throw;
            }
            return *_p_var;
        }

        
        const IVar& current_var() const {
            if ( !_p_var ){
                throw Lsc::NullPointerException()<<BSL_EARG<<"not binding to any IVar object";
            }
            return *_p_var;
        }

        
        size_t current_pos() const {
            return _path - _ori_path;
        }

        
        bool has_slc_query() const {
            return _has_slc_query;
        }

    private:
        void eval_path_expr();

        void eval_step_expr();

        void eval_field_expr();

        bool eval_identifier();

        void eval_string_literal();

        index_type eval_number_literal();

        void skip_white(){
            while( *_path == ' ' || *_path == '\t' ){
                ++ _path;
            }
        }

        const IVar * _p_var;
        const IVar * _ori_p_var;
        const char * _path;
        const char * _ori_path;
        field_type   _str;
        bool         _has_slc_query;
    };

}}
#endif  


