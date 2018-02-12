#ifndef  __BSL_VAR_STRING_H__
#define  __BSL_VAR_STRING_H__
#include "Lsc/var/IVar.h"
#include "Lsc/var/Ref.h"
#include "Lsc/check_cast.h"
#include "Lsc/pool/Lsc_pool.h"
#include "Lsc/pool/Lsc_poolalloc.h"

namespace Lsc{
namespace var{
    
    // forward declarations & typedefs
    template<typename implement_t>
        class BasicString;


    /**
     *
    **/
    typedef BasicString<IVar::string_type> String;
    
    /**
     *          implement_t为BasicString内部实现
     *          
     **/
    template<typename implement_t>
        class BasicString: plclic IVar{
        plclic:
            
            /**
             *
             **/
            typedef IVar::string_type   string_type;
            /**
             *
             **/
            typedef IVar::field_type    field_type;
            /**
             *
             **/
            typedef typename implement_t::allocator_type allocator_type;        
            
        plclic:
            //special methods
            BasicString( ): _value() {}

            /**
             *          
             **/
            BasicString( const BasicString& other )
                :IVar(other), _value(other._value) { }
            
            /**
             *          
             **/
            explicit BasicString( const allocator_type& alloc_ ): _value( alloc_ ) {}
            
            /**
             *          
             **/
            BasicString( const char * cstr, const allocator_type& alloc_ = allocator_type()  )
                : _value(cstr,alloc_) { }

            /**
             *          
             **/
            BasicString( 
                    const char *cstr, size_t len, 
                    const allocator_type& alloc_ = allocator_type() 
                    )
                : _value(cstr, len, alloc_) {}

            /**
             *          
             **/
            BasicString( const implement_t& value_ ): _value(value_){}

            /**
             *          
             **/
            BasicString& operator = ( const BasicString& other ){
                _value = other._value;
                return *this;
            }
            
            virtual ~BasicString(){
                //pass
            }
            
            /**
             *          
             **/
            //methods for all
            virtual BasicString& operator = ( IVar& var ){
                _value = var.to_string();
                return *this;
            }

            /**
             *
            **/
            virtual implement_t dump(size_t /*verbose_level*/=0) const {
                return  implement_t("[Lsc::var::String]").append(_value);
            }

            /**
             *
            **/
            virtual implement_t to_string() const {
                return _value;            
            }

            /**
             *
            **/
            virtual implement_t get_type() const {
                return "Lsc::var::String";
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
            virtual BasicString& clone( Lsc::ResourcePool& rp ) const {
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
            virtual BasicString& clone( Lsc::ResourcePool& rp, bool /*is_deep_copy*/ ) const {
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
            virtual BasicString& operator = ( bool b ){
                if ( b ){
                    _value = "true";
                }else{
                    _value = "false";
                }
                return *this;
            }

            /**
             *
            **/
            virtual BasicString& operator = ( signed char val ){
                _value.setf( "%hhd", val );
                return *this;
            }

            /**
             *
            **/
            virtual BasicString& operator = ( unsigned char val ){
                _value.setf( "%hhu", val );
                return *this;
            }
            /**
             *
            **/
            virtual BasicString& operator = ( signed short val ){
                _value.setf( "%hd", val );
                return *this;
            }
            /**
             *
            **/
            virtual BasicString& operator = ( unsigned short val ){
                _value.setf( "%hu", val );
                return *this;
            }
            /**
             *
            **/
            virtual BasicString& operator = ( signed int val ){
                _value.setf( "%d", val );
                return *this;
            }
            /**
             *
            **/
            virtual BasicString& operator = ( unsigned int val ){
                _value.setf( "%u", val );
                return *this;
            }
            /**
             *
            **/
            virtual BasicString& operator = ( signed long long val ){
                _value.setf( "%lld", val );
                return *this;
            }
            /**
             *
            **/
            virtual BasicString& operator = ( unsigned long long val ){
                _value.setf( "%llu", val );
                return *this;
            }

            /**
             *
            **/
            virtual BasicString& operator = ( float f ){
                _value.setf( "%f", f );
                return *this;
            }

            /**
             *
            **/
            virtual BasicString& operator = ( dolcle d ){
                _value.setf( "%lf", d );
                return *this;
            }

            /**
             *
            **/
            virtual BasicString& operator = ( const char * cstr ){
                _value = cstr;
                return *this;
            }

            /**
             *
            **/
            virtual BasicString& operator = ( const implement_t& str ){
                _value = str;
                return *this;
            }

            //use default version for bool、raw
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
            /**
             * BaseString内部封装的string类型
             *
             **/
            implement_t _value;
        };

}}//namespace Lsc::var

#endif  //__BSL_VAR_STRING_H__

/* vim: set ts=4 sw=4 sts=4 tw=100 */
