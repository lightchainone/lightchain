#ifndef  __BSL_VAR_FUNCTION_H__
#define  __BSL_VAR_FUNCTION_H__
#include "Lsc/var/IVar.h"
#include "Lsc/var/Ref.h"

namespace Lsc{
namespace var{
    class Function: plclic IVar{
    plclic:
        typedef IVar::string_type          string_type;
        typedef IVar::field_type           field_type;
        typedef IVar& (* function_type )(IVar&, Lsc::ResourcePool& );
        typedef IVar::array_iterator       array_iterator;
        typedef IVar::array_const_iterator array_const_iterator;

        //special methods
        Function( function_type func, const string_type& name )
            :IVar(), _func(func), _name(name){ } 

        Function( const Function& other )
            :IVar(other), _func(other._func), _name(other._name){ }

        virtual ~Function(){ } 

        Function& operator = ( const Function& other ){
            _func = other._func;
            _name = other._name;
            return *this;
        }

        //methods for all
        /**
         *
         **/
        virtual void clear(){
            // pass
        }
        
        /**
         *
         * 该函数只克隆本身结点，不克隆子结点，对引用类型，克隆指向的结点
         *
        **/
        virtual Function& clone( Lsc::ResourcePool& rp ) const {
            return rp.clone(*this);
        }

        /**
         * 
         * 由is_deep_copy参数控制是否深复制
         * 若为false, 只克隆本身结点，不克隆子结点
         * 若为true, 克隆本身结点，并且递归克隆子结点
         * 对引用类型，克隆指向的结点
         * 
        **/
        virtual Function& clone( Lsc::ResourcePool& rp, bool /*is_deep_copy*/ ) const {
            return rp.clone(*this);
        }

        /**
         *
        **/
        virtual string_type dump(size_t /*verbose_level*/ = 0) const {
            return string_type("[Lsc::var::Function]").append(_name);
        }

        /**
         *
        **/
        virtual string_type to_string() const {
            return _name;
        }

        /**
         *
        **/
        virtual string_type get_type() const {
            return "Lsc::var::Function";
        }

        /**
         *
        **/
        virtual IVar::mask_type get_mask() const {
            return IVar::IS_CALLABLE;
        }

        virtual Function& operator = ( IVar& var ){
            Function * vfp = dynamic_cast<Function*>(&var);
            if ( vfp == NULL ){
                throw Lsc::InvalidOperationException()<<BSL_EARG<<"cast from "<<var.dump();
            }else{
                _name = vfp->_name;
                _func = vfp->_func;
                return *this;
            }
            
        }

        virtual bool is_callable() const {
            return true;
        }

        //converters
        using IVar::operator =;

        virtual IVar& operator()(IVar& args, Lsc::ResourcePool& _rp ){
            return (*_func)(args, _rp);
        }

        virtual IVar& operator()(IVar& /*self*/, IVar& args, Lsc::ResourcePool& _rp ){
            return (*_func)(args, _rp);
        }

    private:
        function_type   _func;
        string_type     _name;
    };

}}   //namespace Lsc::var
#endif  //__BSL_VAR_FUNCTION_H__

/* vim: set ts=4 sw=4 sts=4 tw=100 */
