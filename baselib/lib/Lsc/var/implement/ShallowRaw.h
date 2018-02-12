#ifndef  __BSL_VAR_SHALLOW_RAW_H__
#define  __BSL_VAR_SHALLOW_RAW_H__
#include "Lsc/var/IVar.h"

namespace Lsc{
namespace var{
    /**
    *  
    *  
    */
    class ShallowRaw: plclic IVar{
    plclic:
        typedef IVar::string_type           string_type;
        typedef IVar::field_type            field_type;
        typedef IVar::raw_type              raw_type;

        //special methods
        ShallowRaw()
            :_value(){}

        ShallowRaw( const ShallowRaw& other )
            :IVar(other), _value(other._value){ }

        explicit ShallowRaw( const raw_type& value_ )
            :IVar(), _value(value_){}

        ShallowRaw( const void *data_, size_t size_ )
            :IVar(), _value(data_, size_) {}

        virtual ~ShallowRaw(){ } 

        ShallowRaw& operator = ( const ShallowRaw& other ){
            if ( this != &other ){
                _value = other._value;
            }
            return *this;
        }

        //methods for all
        /**
         *
         **/
        virtual void clear(){
            _value.data = NULL;
            _value.length = 0;
        }

        /**
         *
         * 该函数只克隆本身结点，不克隆子结点，对引用类型，克隆指向的结点
         *
        **/
        virtual ShallowRaw& clone( Lsc::ResourcePool& rp ) const {
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
        virtual ShallowRaw& clone( Lsc::ResourcePool& rp, bool /*is_deep_copy*/ ) const {
            return rp.clone(*this);
        }

        /**
         *
        **/
        virtual string_type dump(size_t verbose_level = 0) const {
            string_type res;
            res.appendf("[Lsc::var::ShallowRaw] data[%p] length[%zd]", _value.data, _value.length);
            if ( _value.data != NULL && verbose_level > 0 ){
                res.append(" value[");
                const char * bytes = static_cast<const char*>(_value.data);
                for( size_t i = 0; i < _value.length; ++ i){
                    res.appendf("\\x%02hhx", bytes[i]);
                }
                res.append("]");
            }
            return res;
        }

        /**
         *
        **/
        virtual string_type to_string() const {
            return dump(0);
        }

        /**
         *
        **/
        virtual string_type get_type() const {
            return "Lsc::var::ShallowRaw";
        }

        /**
         *
        **/
        virtual IVar::mask_type get_mask() const {
            return IVar::IS_RAW;
        }

        virtual ShallowRaw& operator = ( IVar& other ){
            if ( this != &other ){
                _value = other.to_raw();
            }
            return *this;
        }

        virtual bool is_raw() const {
            return true;
        }

        //all other is_xxx() return false;

        //methods for raw
        virtual ShallowRaw& operator = ( const raw_type& val ){
            _value = val;
            return *this;
        }

        virtual raw_type to_raw() const {
            return _value;
        }

        using IVar::operator =;
    private:
        raw_type _value;
    };

}}   //namespace Lsc::var
#endif  //__BSL_VAR_SHALLOW_RAW_H__

/* vim: set ts=4 sw=4 sts=4 tw=100 */
