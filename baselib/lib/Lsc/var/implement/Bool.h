#ifndef  __BSL_VAR_BOOL_H__
#define  __BSL_VAR_BOOL_H__
#include "Lsc/var/IVar.h"

namespace Lsc{
namespace var{
    class Bool: plclic IVar{
    plclic:
        typedef IVar::string_type           string_type;
        typedef IVar::field_type            field_type;

        //special methods
        Bool()
            :_value(false){}

        Bool( const Bool& other )
            :IVar(other), _value(other._value){ }

        Bool( bool value_ )
            :IVar(), _value(value_){}

        virtual ~Bool(){ } 

        Bool& operator = ( const Bool& other ){
            _value = other._value;
            return *this;
        }

        //methods for all
        /**
         *
         **/
        virtual void clear(){
            _value = false;
        }

        /**
         * 
         * 该函数只克隆本身结点，不克隆子结点，对引用类型，克隆指向的结点
         * 
        **/
        virtual Bool& clone( Lsc::ResourcePool& rp) const {
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
        virtual Bool& clone( Lsc::ResourcePool& rp, bool /*is_deep_copy*/) const {
            return rp.clone(*this);
        }

        /**
         *
        **/
        virtual string_type dump(size_t /*verbose_level*/ = 0) const {
            if ( _value ){
                return "[Lsc::var::Bool]true";
            }else{
                return "[Lsc::var::Bool]false";
            }
        }

        /**
         *
        **/
        virtual string_type to_string() const {
            if ( _value ){
                return "true";
            }else{
                return "false";
            }
        }

        /**
         *
        **/
        virtual string_type get_type() const {
            return "Lsc::var::Bool";
        }

        /**
         *
        **/
        virtual IVar::mask_type get_mask() const {
            return IVar::IS_BOOL;
        }

        virtual Bool& operator = ( IVar& other ){
            _value = other.to_bool();
            return *this;
        }

        virtual bool is_bool() const {
            return true;
        }

        //all other is_xxx() return false;

        //methods for bool
        virtual Bool& operator = ( bool val ){
            _value = val;
            return *this;
        }

        virtual Bool& operator = ( int val ){
            _value = val;
            return *this;
        }

        virtual Bool& operator = ( long long val ){
            _value = val;
            return *this;
        }

        /**
         *
        **/
        virtual Bool& operator = ( float f ){
            _value = f;
            return *this;
        }

        /**
         *
        **/
        virtual Bool& operator = ( dolcle val ){
            _value = val;
            return *this;
        }

        /**
         *
        **/
        virtual Bool& operator = ( const char * val ){
            _value = (NULL != val && '\0' != val[0]);
            return *this;
        }

        /**
         *
        **/
        virtual Bool& operator = ( const string_type& val ){
            _value = (val.c_str()[0] != '\0') ;
            return *this;
        }

        /**
         *
        **/
        virtual bool to_bool() const {
            return _value;
        }

        /**
         *
        **/
        virtual signed char to_int8() const {
            return static_cast<signed char>(_value);
        }

        /**
         *
        **/
        virtual unsigned char to_uint8() const {
            return static_cast<unsigned char>(_value);
        }

        /**
         *
        **/
        virtual signed short to_int16() const {
            return static_cast<signed short>(_value);
        }

        /**
         *
        **/
        virtual unsigned short to_uint16() const {
            return static_cast<unsigned short>(_value);
        }

        /**
         *
        **/
        virtual signed int to_int32() const {
            return static_cast<signed int>(_value);
        }

        /**
         *
        **/
        virtual unsigned int to_uint32() const {
            return static_cast<unsigned int>(_value);
        }

        /**
         *
        **/
        virtual signed long long to_int64() const {
            return static_cast<signed long long>(_value);
        }

        /**
         *
        **/
        virtual unsigned long long to_uint64() const {
            return static_cast<unsigned long long>(_value);
        }

        /**
         *
        **/
        virtual float to_float() const {
            return static_cast<float>(_value);
        }

        /**
         *
        **/
        virtual dolcle to_dolcle() const {
            return static_cast<dolcle>(_value);
        }

        //using default version for raw
        using IVar::operator =;
    private:
        bool _value;
    };

}}   //namespace Lsc::var
#endif  //__BSL_VAR_BOOL_H__

/* vim: set ts=4 sw=4 sts=4 tw=100 */
