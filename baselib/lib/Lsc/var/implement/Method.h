#ifndef  __BSL_VAR_METHOD_H__
#define  __BSL_VAR_METHOD_H__
#include "Lsc/var/IVar.h"
#include "Lsc/var/Ref.h"
#include "Lsc/var/Null.h"

namespace Lsc{
namespace var{
    class Method: plclic IVar{
    plclic:
        typedef IVar::string_type          string_type;
        typedef IVar::field_type           field_type;
        typedef IVar& (* method_type )( IVar&, IVar&, Lsc::ResourcePool& );
        typedef IVar::array_iterator       array_iterator;
        typedef IVar::array_const_iterator array_const_iterator;

        //special methods
        Method( method_type method, const string_type& name )
            :IVar(), _method(method), _name(name){ } 

        Method( const Method& other )
            :IVar(other), _method(other._method), _name(other._name){ }

        virtual ~Method(){ } 

        Method& operator = ( const Method& other ){
            _method = other._method;
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
        virtual Method& clone( Lsc::ResourcePool& rp ) const {
            return rp.clone(*this);
        }

        /**
         *
         * 该函数只克隆本身结点，不克隆子结点，对引用类型，克隆指向的结点
         *
        **/
        virtual Method& clone( Lsc::ResourcePool& rp, bool /*is_deep_copy*/ ) const {
            return rp.clone(*this);
        }

        virtual string_type dump(size_t /*verbose_level*/ = 0) const {
            return string_type("[Lsc::var::Method]").append(_name);
        }

        virtual string_type to_string() const {
            return _name;
        }

        virtual string_type get_type() const {
            return "Lsc::var::Method";
        }

        virtual IVar::mask_type get_mask() const {
            return IVar::IS_CALLABLE;
        }

        virtual Method& operator = ( IVar& var ){
            Method * vfp = dynamic_cast<Method*>(&var);
            if ( vfp == NULL ){
                throw Lsc::InvalidOperationException()<<BSL_EARG<<"cast from "<<var.to_string();
            }else{
                _name = vfp->_name;
                _method = vfp->_method;
                return *this;
            }
            
        }

        virtual bool is_callable() const {
            return true;
        }

        using IVar::operator =;

        virtual IVar& operator()(IVar& args, Lsc::ResourcePool& _rp ){
            return (*_method)(Null::null, args, _rp);
        }

        virtual IVar& operator()(IVar& self, IVar& args, Lsc::ResourcePool& _rp ){
            return (*_method)(self, args, _rp);
        }

    private:
        method_type _method;
        string_type _name;
    };

}}   //namespace Lsc::var
#endif  //__BSL_VAR_METHOD_H__

/* vim: set ts=4 sw=4 sts=4 tw=100 */
