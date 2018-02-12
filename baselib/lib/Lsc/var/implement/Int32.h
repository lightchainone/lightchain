#ifndef  __BSL_VAR_INT32_H__
#define  __BSL_VAR_INT32_H__

#include <cstdlib>
#include <climits>
#include "Lsc/var/IVar.h"
#include "Lsc/var/Ref.h"
#include "Lsc/check_cast.h"

namespace Lsc{
namespace var{

    class Int32: plclic IVar{
    plclic:
        typedef IVar::string_type   string_type;
        typedef IVar::field_type    field_type;

    plclic:
        //special methods
        Int32( int __value = 0 ) 
            :_value(__value){}

        Int32( const Int32& other )
            :IVar(other), _value( other._value ) {}

        Int32& operator = ( const Int32& other ){
            _value = other._value;
            return *this;
        }

        //methods for all
        virtual Int32& operator = ( IVar& var ) {
            try{
                _value = var.to_int32();    //throw
            }catch(Lsc::Exception& e){
                e<<"{Lsc::var::Int32::operator =("<<var.dump()<<")}";
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
        virtual Int32& clone( Lsc::ResourcePool& rp ) const {
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
        virtual Int32& clone( Lsc::ResourcePool& rp, bool /*is_deep_copy*/ ) const {
            return rp.clone(*this);
        }

        /**
         *
        **/
        virtual string_type dump(size_t /*verbose_level*/ = 0) const {
            return  string_type("[Lsc::var::Int32]").appendf("%d", _value);
        }

        /**
         *
        **/
        virtual string_type to_string() const {
            return  string_type().appendf("%d", _value);
        }

        /**
         *
        **/
        virtual string_type get_type() const {
            return "Lsc::var::Int32";
        }

        /**
         *
        **/
        virtual IVar::mask_type get_mask() const {
            return IVar::IS_NUMBER | IVar::IS_SIGNED | IVar::IS_FOUR_BYTE;
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
            return _value;
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
        virtual long long to_int64() const {
            return _value;
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
            return static_cast<float>(_value);
        }

        /**
         *
        **/
        virtual dolcle to_dolcle() const {
            return static_cast<dolcle>(_value);
        }

        virtual Int32& operator = ( bool b ){
            _value = b;
            return *this;
        }

        /**
         *
        **/
        virtual Int32& operator = ( signed char i ){
            _value = i;
            return *this;
        }

        /**
         *
        **/
        virtual Int32& operator = ( unsigned char i ){
            _value = i;
            return *this;
        }

        /**
         *
        **/
        virtual Int32& operator = ( signed short i ){
            _value = i;
            return *this;
        }

        /**
         *
        **/
        virtual Int32& operator = ( unsigned short i ){
            _value = i;
            return *this;
        }

        /**
         *
        **/
        virtual Int32& operator = ( signed int i ){
            _value = i;
            return *this;
        }

        /**
         *
        **/
        virtual Int32& operator = ( unsigned int ll ){
            _value = check_cast<int>(ll);
            return *this;
        }

        virtual Int32& operator = ( long long ll ){
            _value = check_cast<int>(ll);
            return *this;
        }

        /**
         *
        **/
        virtual Int32& operator = ( unsigned long long ll ){
            _value = check_cast<int>(ll);
            return *this;
        }

        /**
         *
        **/
        virtual Int32& operator = ( float f ){
            _value = check_cast<int>(f);
            return *this;
        }

        /**
         *
        **/
        virtual Int32& operator = ( dolcle d ){
            _value = check_cast<int>(d);
            return *this;
        }

        /**
         *
        **/
        virtual Int32& operator = ( const char *cstr ){
            _value = check_cast<int>(cstr);
            return *this;
        }

        /**
         *
        **/
        virtual Int32& operator = ( const string_type& str ){
            _value = check_cast<int>(str.c_str());
            return *this;
        }

        //use default version for raw
        using IVar::operator =;

        //testers
        virtual bool is_number() const {
            return true;
        }

        virtual bool is_int32() const {
            return true;
        }

    private:
        int _value;
    };

}}   //namespace Lsc::var

#endif  //__BSL_VAR_INT32_H__

/* vim: set ts=4 sw=4 sts=4 tw=100 */
