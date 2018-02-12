#ifndef  __BSL_VAR_SHALLOW_STRING_H__
#define  __BSL_VAR_SHALLOW_STRING_H__
#include "Lsc/var/IVar.h"
#include "Lsc/var/Ref.h"
#include "Lsc/check_cast.h"
#include "Lsc/ShallowCopyString.h"

namespace Lsc{
namespace var{
    class ShallowString: plclic IVar{
    plclic:
        typedef IVar::string_type   string_type;
        typedef IVar::field_type    field_type;

    plclic:
        //special methods
        ShallowString( )
            : _value() {}

        ShallowString( const ShallowString& other )
            : IVar(other), _value(other._value) { }

        ShallowString( const char * cstr )
            : IVar(), _value(cstr) { }

        ShallowString( const char * cstr, size_t len )
            : IVar(), _value(cstr, len) { }

        ShallowString( const Lsc::ShallowCopyString& value_)
            : IVar(), _value( value_.c_str(), value_.size() ) { }


        ShallowString( const string_type& value_ )
            : IVar(), _value(value_.c_str(), value_.size()){}

        ShallowString& operator = ( const ShallowString& other ){
            _value = other._value;
            return *this;
        }

        virtual ~ShallowString(){
            //pass
        }

        //methods for all
        virtual ShallowString& operator = ( IVar& var ){
            if ( var.is_string() ){
                _value = var.c_str();
                return *this;
            }else{
                throw Lsc::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<_value.c_str()<<"] cast from["<<var.dump(0)<<"]";
            }
        }

        /**
         *
        **/
        virtual string_type dump(size_t /*verbose_level*/=0) const {
            return  string_type("[Lsc::var::ShallowString]").append(_value.c_str(), _value.size());
        }

        /**
         *
        **/
        virtual string_type to_string() const {
            return _value.c_str();
        }

        /**
         *
        **/
        virtual string_type get_type() const {
            return "Lsc::var::ShallowString";
        }

        /**
         *
        **/
        virtual IVar::mask_type get_mask() const {
            return IVar::IS_STRING;
        }

        /**
         *
         **/
        virtual void clear(){
            _value.clear();
        }

        /**
         *
         * 该函数只克隆本身结点，不克隆子结点，对引用类型，克隆指向的结点
         *
        **/
        virtual ShallowString& clone( Lsc::ResourcePool& rp ) const {
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
        virtual ShallowString& clone( Lsc::ResourcePool& rp, bool /*is_deep_copy*/ ) const {
            return rp.clone(*this);
        }

        //testers
        /**
         *
        **/
        virtual bool is_string() const {
            return true;
        }

        //methods for value
        /**
         *
        **/
        virtual bool to_bool() const {
            return _value.c_str()[0] != '\0';
        }

        /**
         *
        **/
        virtual signed char to_int8() const {
            return check_cast<signed char>(_value.c_str());
        }

        /**
         *
        **/
        virtual unsigned char to_uint8() const {
            return check_cast<unsigned char>(_value.c_str());
        }

        /**
         *
        **/
        virtual signed short to_int16() const {
            return check_cast<signed short>(_value.c_str());
        }

        /**
         *
        **/
        virtual unsigned short to_uint16() const {
            return check_cast<unsigned short>(_value.c_str());
        }

        /**
         *
        **/
        virtual signed int to_int32() const {
            return check_cast<signed int>(_value.c_str());
        }

        /**
         *
        **/
        virtual unsigned int to_uint32() const {
            return check_cast<unsigned int>(_value.c_str());
        }

        /**
         *
        **/
        virtual signed long long to_int64() const {
            return check_cast<signed long long>(_value.c_str());
        }

        /**
         *
        **/
        virtual unsigned long long to_uint64() const {
            return check_cast<unsigned long long>(_value.c_str());
        }

        /**
         *
        **/
        virtual float to_float() const {
            return check_cast<float>(_value.c_str());
        }

        /**
         *
        **/
        virtual dolcle to_dolcle() const {
            return check_cast<dolcle>(_value.c_str());
        }

        /**
         *
        **/
        virtual ShallowString& operator = ( const char * cstr ){
            _value = cstr;
            return *this;
        }

        /**
         *
        **/
        virtual ShallowString& operator = ( const string_type& str ){
            _value = str;
            return *this;
        }

        //use default version for bool、raw、number
        using IVar::operator =;

        /**
         *
         * 所有is_string()返回true的IVar实现类都必须支持该方法。
         * 若实现类不支持该操作，抛出Lsc::InvalidOperationException异常
        **/
        virtual const char * c_str() const {
            return _value.c_str();
        }

        /**
         *
         * 所有is_string()返回true的IVar实现类都必须支持该方法。
         * 若实现类不支持该操作，抛出Lsc::InvalidOperationException异常
        **/
        virtual size_t c_str_len() const {
            return _value.size();
        }

    private:
        Lsc::ShallowCopyString _value;
    };

}}//namespace Lsc::var

#endif  //__BSL_VAR_SHALLOW_STRING_H__

/* vim: set ts=4 sw=4 sts=4 tw=100 */
