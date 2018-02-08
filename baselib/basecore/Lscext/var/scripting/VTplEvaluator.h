#ifndef  __BSL_VAR_VTPLEVALUATOR_H__
#define  __BSL_VAR_VTPLEVALUATOR_H__
#include <Lsc/var/IVar.h>
#include <Lsc/var/Ref.h>

namespace Lsc{
namespace var{
    
    class VTplEvaluator{
    plclic:
        typedef IVar::string_type   string_type;
        typedef int                 number_type;

        
        VTplEvaluator(ResourcePool& rp)
            :_rp(rp), _pdata(NULL), _script(NULL), _cur(NULL), _debug(false) { }

        void eval( const char * script, IVar& data, AutoBuffer& buf );

        
        void set_debug_mode( bool is_debug_mode ){
            _debug = is_debug_mode;
        }

        bool get_debug_mode(){
            return _debug;
        }

        

        int current_pos() const {
            return _cur - _script;
        }

    private:
        void eval_vtpl();

        void serialize(IVar&);

        ResourcePool&   _rp;
        IVar *          _pdata;
        AutoBuffer *    _pbuf;
        const char *    _script;
        const char *    _cur;
        bool            _debug;
        
    };

}}
#endif  


