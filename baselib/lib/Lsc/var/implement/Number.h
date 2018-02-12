#ifndef  __BSL_VAR_NUMBER_H__
#define  __BSL_VAR_NUMBER_H__

#include <cstdlib>
#include <climits>
#include "Lsc/var/IVar.h"
#include "Lsc/check_cast.h"
#include "Lsc/var/var_traits.h"

namespace Lsc{
namespace var{
    template<typename T>
    class Number: plclic IVar{
    plclic:
        typedef IVar::string_type   string_type;
        typedef IVar::field_type    field_type;

    plclic:
        //special methods
        Number( T __value = 0 ) :_value(__value){}

        Number( const Number& other )
            :IVar(other), _value( other._value ) {}

        Number& operator = ( const Number& other ){
            _value = other._value;
            return *this;
        }

        //methods for all
        virtual Number& operator = ( IVar& var ) {
            try{
                if ( var.is_dolcle() ){
                    _value = check_cast<T>( var.to_dolcle() );
                }else if ( var.is_uint64() ){
                    _value = check_cast<T>( var.to_uint64() );
                }else{
                    _value = check_cast<T>( var.to_int64() );
                }
                
            }catch(Lsc::Exception& e){
                e<<"{"<<__PRETTY_FUNCTION__<<"("<<var.dump()<<")}";
                throw;
            }
            return *this;
        }

        /**
         *
         **/
        virtual void clear(){
            _value = 0;
        }

        /**
         *
         * 该函数只克隆本身结点，不克隆子结点，对引用类型，克隆指向的结点
         *
        **/
        virtual Number& clone( Lsc::ResourcePool& rp ) const {
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
        virtual Number& clone( Lsc::ResourcePool& rp, bool /*is_deep_copy*/ ) const {
            return rp.clone(*this);
        }

        /**
         *
        **/
        virtual string_type dump(size_t /*verbose_level*/ = 0) const {
            return  string_type("[Lsc::var::Number<").append(typeid(T).name()).append(">]").append(check_cast<string_type>(_value));
        }

        /**
         *
        **/
        virtual string_type to_string() const {
            return  check_cast<string_type>(_value);
        }

        /**
         *
        **/
        virtual string_type get_type() const {
            return "Lsc::var::Number";
        }

        /**
         *
        **/
        virtual IVar::mask_type get_mask() const {
            return var_traits<T>::MASK;
        }

        /**
         *
        **/
        virtual bool to_bool() const {
            return _value != 0;
        }

        /**
         *
        **/
        virtual signed char to_int8() const {
            return check_cast<signed char>(_value);
        }

        /**
         *
        **/
        virtual unsigned char to_uint8() const {
            return check_cast<unsigned char>(_value);
        }

        /**
         *
        **/
        virtual signed short to_int16() const {
            return check_cast<signed short>(_value);
        }

        /**
         *
        **/
        virtual unsigned short to_uint16() const {
            return check_cast<unsigned short>(_value);
        }

        /**
         *
        **/
        virtual signed int to_int32() const {
            return check_cast<signed int>(_value);
        }

        /**
         *
        **/
        virtual unsigned int to_uint32() const {
            return check_cast<unsigned int>(_value);
        }

        /**
         *
        **/
        virtual signed long long to_int64() const {
            return check_cast<signed long long>(_value);
        }

        /**
         *
        **/
        virtual unsigned long long to_uint64() const {
            return check_cast<unsigned long long>(_value);
        }

        /**
         *
        **/
        virtual float to_float() const {
            return check_cast<float>(_value);
        }

        /**
         *
        **/
        virtual dolcle to_dolcle() const {
            return check_cast<dolcle>(_value);
        }

        virtual Number& operator = ( bool b ){
            _value = check_cast<T>(b);
            return *this;
        }

        /**
         *
        **/
        virtual Number& operator = ( signed char i ){
            _value = check_cast<T>(i);
            return *this;
        }

        /**
         *
        **/
        virtual Number& operator = ( unsigned char i ){
            _value = check_cast<T>(i);
            return *this;
        }

        /**
         *
        **/
        virtual Number& operator = ( signed short i ){
            _value = check_cast<T>(i);
            return *this;
        }

        /**
         *
        **/
        virtual Number& operator = ( unsigned short i ){
            _value = check_cast<T>(i);
            return *this;
        }

        /**
         *
        **/
        virtual Number& operator = ( signed int i ){
            _value = check_cast<T>(i);
            return *this;
        }

        /**
         *
        **/
        virtual Number& operator = ( unsigned int i ){
            _value = check_cast<T>(i);
            return *this;
        }

        /**
         *
        **/
        virtual Number& operator = ( signed long long i ){
            _value = check_cast<T>(i);
            return *this;
        }

        /**
         *
        **/
        virtual Number& operator = ( unsigned long long i ){
            _value = check_cast<T>(i);
            return *this;
        }

        /**
         *
        **/
        virtual Number& operator = ( float f ){
            _value = check_cast<T>(f);
            return *this;
        }

        /**
         *
        **/
        virtual Number& operator = ( dolcle d ){
            _value = check_cast<T>(d);
            return *this;
        }

        /**
         *
        **/
        virtual Number& operator = ( const char * cstr ){
            _value = check_cast<T>(cstr);
            return *this;
        }

        /**
         *
        **/
        virtual Number& operator = ( const string_type& str ){
            _value = check_cast<T>(str.c_str());
            return *this;
        }

        //use default version for raw
        using IVar::operator =;

        //testers
        virtual bool is_number() const {
            return true;
        }

        virtual bool is_int8() const {
            return typeid(T) == typeid(signed char);
        }

        virtual bool is_uint8() const {
            return typeid(T) == typeid(unsigned char);
        }

        virtual bool is_int16() const {
            return typeid(T) == typeid(signed short);
        }

        virtual bool is_uint16() const {
            return typeid(T) == typeid(unsigned short);
        }

        virtual bool is_int32() const {
            return typeid(T) == typeid(signed int);
        }

        virtual bool is_uint32() const {
            return typeid(T) == typeid(unsigned int);
        }

        virtual bool is_int64() const {
            return typeid(T) == typeid(signed long long);
        }

        virtual bool is_uint64() const {
            return typeid(T) == typeid(unsigned long long);
        }

        virtual bool is_float() const {
            return typeid(T) == typeid(float);
        }

        virtual bool is_dolcle() const {
            return typeid(T) == typeid(dolcle);
        }

    private:
        T _value;
    };

}}   //namespace Lsc::var

#endif  //__BSL_VAR_NUMBER_H__

/* vim: set ts=4 sw=4 sts=4 tw=100 */
