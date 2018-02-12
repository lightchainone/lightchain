#ifndef  __BSL_VAR_IVAR_H__
#define  __BSL_VAR_IVAR_H__

#include "Lsc/containers/string/Lsc_string.h"
#include "Lsc/exception/Lsc_exception.h"
#include "Lsc/ResourcePool.h"
#include "Lsc/var/ArrayIterator.h"
#include "Lsc/var/DictIterator.h"

namespace Lsc{
namespace var{

    /**
    *  
    * 相当于头指针与长度的简单集合，析构时不会回收data所指内存。
    */
    struct raw_t{
        /**
        *  
        *  
        */
        const void  *data;
        /**
        *  
        *  
        */
        size_t      length;

        /**
         *
        **/
        raw_t()
            :data(NULL), length(0) {}
        /**
         *
        **/
        raw_t( const void *data_, size_t len)
            :data(data_), length(len) {}
    };

    //forward declaration
    class IVar{
    plclic:
        /**
        *  
        *  
        */
        typedef Lsc::string         string_type;

        /**
        *  
        *  
        */
        typedef Lsc::string         field_type;

        /**
        *  
        *  
        */
        typedef raw_t               raw_type;

        /**
        *  
        *  
        */
        typedef ArrayIterator       array_iterator;

        /**
        *  
        *  
        */
        typedef ArrayConstIterator  array_const_iterator;

        /**
        *  
        *  
        */
        typedef DictIterator        dict_iterator;

        /**
        *  
        *  
        */
        typedef DictConstIterator   dict_const_iterator;

        /**
        *  
        *  
        */
        typedef unsigned short mask_type;

        // constant definition
        static const mask_type IS_BOOL = 1 << 0;          /**< 是否布尔类型       */
        static const mask_type IS_NUMBER = 1 << 1;        /**< 是否数值类型       */
        static const mask_type IS_STRING = 1 << 2;        /**< 是否字符串类型       */
        static const mask_type IS_RAW = 1 << 3;        /**< 是否二进制类型       */
        static const mask_type IS_ARRAY = 1 << 4;        /**< 是否数组类型       */
        static const mask_type IS_DICT = 1 << 5;        /**< 是否字典类型       */
        static const mask_type IS_CALLABLE = 1 << 6;        /**< 是否可调用类型       */
        static const mask_type IS_OTHER = 1 << 7;        /**< 是否其它类型       */
        static const mask_type IS_REF = 1 << 8;        /**< 是否引用类型       */
        static const mask_type IS_MUTABLE = 1 << 9;        /**< 是否可变类型       */
        static const mask_type IS_FLOATING = 1 << 10;        /**< 是否浮点类型       */
        static const mask_type IS_SIGNED = 1 << 11;        /**< 是否有符号类型       */
        static const mask_type IS_ONE_BYTE = 1 << 12;        /**< 是否单字节类型       */
        static const mask_type IS_TWO_BYTE = 1 << 13;        /**< 是否双字节类型       */
        static const mask_type IS_FOUR_BYTE = 1 << 14;        /**< 是否四字节类型       */
        static const mask_type IS_EIGHT_BYTE = 1 << 15;        /**< 是否八字节类型       */
        static const mask_type NONE_MASK = 0;        /**< 空掩码 */
        static const mask_type ALL_MASK = ~0;        /**< 满掩码 */



        //methods for all
        /**
         *
        **/
        virtual ~IVar(){ } 

        /**
         *
         * 所有IVar实现类都必须支持该方法。
         *
        **/
        virtual IVar& operator = ( IVar& ) = 0;

        /**
         *
         * 所有IVar实现类都必须支持该方法。
         * 该方法仅用于调试与跟踪，其内容应该容易被肉眼识别。其格式可能经常变化，不应对其内容进行监控。
         * 
         * 可选的verbose_level参数表示递归深度。0表示不递归子IVar对象，实现类应保证该函数算法复杂度为O(1)；1表示递归所有直接子IVar对象，实现类应保证该函数算法复杂度为O(size())，余类推。
         *
        **/
        virtual string_type dump(size_t verbose_level=0) const = 0;

        /**
         *
         * 所有IVar实现类都必须支持该方法。
         *
        **/
        virtual void clear() = 0;

        /**
         *
         * 所有IVar实现类都必须支持该方法。
         * 自BSL 1.0.5后，该函数只用于转化为字符串，调试/跟踪应使用dump()
         *
        **/
        virtual string_type to_string() const = 0;

        /**
         *
         * 所有IVar实现类都必须支持该方法。
         *
        **/
        virtual string_type get_type() const = 0; 

        /**
         *
         * 所有IVar实现类都必须支持该方法。
         * 目前返回值类型目前是unsigned short，以后可能会改变，但会保持与unsigned short兼容
         *
        **/
        virtual mask_type get_mask() const = 0;

        /**
         *
        **/
        virtual IVar& clone(Lsc::ResourcePool& /*rp*/) const = 0;

        /**
         *
        **/
        virtual IVar& clone(Lsc::ResourcePool& /*rp*/, bool /*is_deep_copy*/) const{
            throw Lsc::NotImplementedException()<<BSL_EARG<<"type["<<typeid(*this).name()
                <<"] this["<<dump(0)<<"]: clone interface has not been implemented.";
        }

        //methods for all, test methods
        /**
         *
         * 所有IVar实现类都必须支持该方法。
         *
        **/
        virtual bool is_null() const {
            return (get_mask() & 0xFF) == 0;
        }

        /**
         *
         * 所有IVar实现类都必须支持该方法。
         *
        **/
        virtual bool is_ref() const {
            return get_mask() & IS_REF;
        }

        /**
         * 所有IVar实现类都必须支持该方法。
         *
        **/
        virtual bool is_bool() const {
            return get_mask() & IS_BOOL;
        }
        /**
         *
         * 所有IVar实现类都必须支持该方法。
         *
        **/
        virtual bool is_number() const {
            return get_mask() & IS_NUMBER;
        }

        /**
         *
         * 所有IVar实现类都必须支持该方法。
         *
        **/
        virtual bool is_int8() const {
            return check_mask( get_mask(), IS_NUMBER|IS_ONE_BYTE|IS_SIGNED, IS_FLOATING );
        }
        
        /**
         *
         * 所有IVar实现类都必须支持该方法。
         *
        **/
        virtual bool is_uint8() const {
            return check_mask( get_mask(), IS_NUMBER|IS_ONE_BYTE, IS_SIGNED|IS_FLOATING );
        }

        /**
         *
         * 所有IVar实现类都必须支持该方法。
         *
        **/
        virtual bool is_int16() const {
            return check_mask( get_mask(), IS_NUMBER|IS_TWO_BYTE|IS_SIGNED, IS_FLOATING );
        }

        /**
         *
         * 所有IVar实现类都必须支持该方法。
         *
        **/
        virtual bool is_uint16() const {
            return check_mask( get_mask(), IS_NUMBER|IS_TWO_BYTE, IS_SIGNED|IS_FLOATING );
        }

        /**
         *
         * 所有IVar实现类都必须支持该方法。
         *
        **/
        virtual bool is_int32() const {
            return check_mask( get_mask(), IS_NUMBER|IS_FOUR_BYTE|IS_SIGNED, IS_FLOATING );
        }

        /**
         *
         * 所有IVar实现类都必须支持该方法。
         *
        **/
        virtual bool is_uint32() const {
            return check_mask( get_mask(), IS_NUMBER|IS_FOUR_BYTE, IS_SIGNED|IS_FLOATING );
        }

        /**
         *
         * 所有IVar实现类都必须支持该方法。
         *
        **/
        virtual bool is_int64() const {
            return check_mask( get_mask(), IS_NUMBER|IS_EIGHT_BYTE|IS_SIGNED, IS_FLOATING );
        }

        /**
         *
         * 所有IVar实现类都必须支持该方法。
         *
        **/
        virtual bool is_uint64() const {
            return check_mask( get_mask(), IS_NUMBER|IS_EIGHT_BYTE, IS_SIGNED|IS_FLOATING );
        }

        /**
         *
         * 所有IVar实现类都必须支持该方法。
         *
        **/
        virtual bool is_float() const {
            return check_mask( get_mask(), IS_NUMBER|IS_FOUR_BYTE|IS_FLOATING );
        }

        /**
         *
         * 所有IVar实现类都必须支持该方法。
         *
        **/
        virtual bool is_dolcle() const {
            return check_mask( get_mask(), IS_NUMBER|IS_EIGHT_BYTE|IS_FLOATING );
        }

        /**
         *
         * 所有IVar实现类都必须支持该方法。
         *
        **/
        virtual bool is_string() const {
            return get_mask() & IS_STRING;
        }

        /**
         *
         * 所有IVar实现类都必须支持该方法。
         *
        **/
        virtual bool is_array() const {
            return get_mask() & IS_ARRAY;
        }

        /**
         *
         * 所有IVar实现类都必须支持该方法。
         *
        **/
        virtual bool is_dict() const {
            return get_mask() & IS_DICT;
        }

        /**
         *
         * 所有IVar实现类都必须支持该方法。
         *
        **/
        virtual bool is_callable() const {
            return get_mask() & IS_CALLABLE;
        }

        /**
         *
         * 所有IVar实现类都必须支持该方法
         *
        **/
        virtual bool is_raw() const {
            return get_mask() & IS_RAW;
        }

        /**
         * 若实现类不支持该操作，抛出Lsc::InvalidOperationException异常
         *
        **/
        virtual bool to_bool() const {
            throw Lsc::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"]";
        }
        //methods for value

        /**
         *
         * 所有is_number()返回true的IVar实现类都必须支持该方法。
         * 若实现类不支持该操作，抛出Lsc::InvalidOperationException异常
         * 若转化发生数值溢出，抛出Lsc::UnderflowException / Lsc::OverflowException
         *
        **/
        virtual signed char to_int8() const {
            throw Lsc::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"]";
        }

        /**
         *
         * 所有is_number()返回true的IVar实现类都必须支持该方法。
         * 若实现类不支持该操作，抛出Lsc::InvalidOperationException异常
         * 若转化发生数值溢出，抛出Lsc::UnderflowException / Lsc::OverflowException
         *
        **/
        virtual unsigned char to_uint8() const {
            throw Lsc::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"]";
        }

        /**
         *
         * 所有is_number()返回true的IVar实现类都必须支持该方法。
         * 若实现类不支持该操作，抛出Lsc::InvalidOperationException异常
         * 若转化发生数值溢出，抛出Lsc::UnderflowException / Lsc::OverflowException
         *
        **/
        virtual signed short to_int16() const {
            throw Lsc::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"]";
        }

        /**
         *
         * 所有is_number()返回true的IVar实现类都必须支持该方法。
         * 若实现类不支持该操作，抛出Lsc::InvalidOperationException异常
         * 若转化发生数值溢出，抛出Lsc::UnderflowException / Lsc::OverflowException
         *
        **/
        virtual unsigned short to_uint16() const {
            throw Lsc::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"]";
        }

        /**
         *
         * 所有is_number()返回true的IVar实现类都必须支持该方法。
         * 若实现类不支持该操作，抛出Lsc::InvalidOperationException异常
         * 若转化发生数值溢出，抛出Lsc::UnderflowException / Lsc::OverflowException
         *
        **/
        virtual signed int to_int32() const {
            throw Lsc::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"]";
        }

        /**
         *
         * 所有is_number()返回true的IVar实现类都必须支持该方法。
         * 若实现类不支持该操作，抛出Lsc::InvalidOperationException异常
         * 若转化发生数值溢出，抛出Lsc::UnderflowException / Lsc::OverflowException
         *
        **/
        virtual unsigned int to_uint32() const {
            throw Lsc::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"]";
        }

        /**
         *
         * 所有is_number()返回true的IVar实现类都必须支持该方法。
         * 若实现类不支持该操作，抛出Lsc::InvalidOperationException异常
         * 若转化发生数值溢出，抛出Lsc::UnderflowException / Lsc::OverflowException
         *
        **/
        virtual signed long long to_int64() const {
            throw Lsc::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"]";
        }

        /**
         *
         * 所有is_number()返回true的IVar实现类都必须支持该方法。
         * 若实现类不支持该操作，抛出Lsc::InvalidOperationException异常
         * 若转化发生数值溢出，抛出Lsc::UnderflowException / Lsc::OverflowException
         *
        **/
        virtual unsigned long long to_uint64() const {
            throw Lsc::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"]";
        }

        /**
         *
         * 所有is_number()返回true的IVar实现类都必须支持该方法。
         * 若实现类不支持该操作，抛出Lsc::InvalidOperationException异常
         *
        **/
        virtual float to_float() const {
            throw Lsc::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"]";
        }

        /**
         *
         * 所有is_number()返回true的IVar实现类都必须支持该方法。
         * 若实现类不支持该操作，抛出Lsc::InvalidOperationException异常
         *
        **/
        virtual dolcle to_dolcle() const {
            throw Lsc::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"]";
        }

        /**
         *
         * 所有is_raw()返回true的实现类都必须支持该方法
         *
        **/
        virtual raw_type to_raw() const {
            throw Lsc::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"]";
        }

        /**
         * 若实现类不支持该操作，抛出Lsc::InvalidOperationException异常
         *
        **/
        virtual IVar& operator = ( bool value_ ){
            throw Lsc::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"] assign from["<<value_<<"]";
        }

        /**
         *
         * 所有is_number()返回true的IVar实现类都必须支持该方法。
         * 若实现类不支持该操作，抛出Lsc::InvalidOperationException异常
         * 若转化发生数值溢出，抛出Lsc::UnderflowException / Lsc::OverflowException
         *
        **/
        virtual IVar& operator = ( signed char value_ ){
            throw Lsc::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"] assign from["<<value_<<"]";
        }

        /**
         *
         * 所有is_number()返回true的IVar实现类都必须支持该方法。
         * 若实现类不支持该操作，抛出Lsc::InvalidOperationException异常
         * 若转化发生数值溢出，抛出Lsc::UnderflowException / Lsc::OverflowException
         *
        **/
        virtual IVar& operator = ( unsigned char value_ ){
            throw Lsc::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"] assign from["<<value_<<"]";
        }


        /**
         *
         * 所有is_number()返回true的IVar实现类都必须支持该方法。
         * 若实现类不支持该操作，抛出Lsc::InvalidOperationException异常
         * 若转化发生数值溢出，抛出Lsc::UnderflowException / Lsc::OverflowException
         *
        **/
        virtual IVar& operator = ( signed short value_ ){
            throw Lsc::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"] assign from["<<value_<<"]";
        }

        /**
         *
         * 所有is_number()返回true的IVar实现类都必须支持该方法。
         * 若实现类不支持该操作，抛出Lsc::InvalidOperationException异常
         * 若转化发生数值溢出，抛出Lsc::UnderflowException / Lsc::OverflowException
         *
        **/
        virtual IVar& operator = ( unsigned short value_ ){
            throw Lsc::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"] assign from["<<value_<<"]";
        }


        /**
         *
         * 所有is_number()返回true的IVar实现类都必须支持该方法。
         * 若实现类不支持该操作，抛出Lsc::InvalidOperationException异常
         * 若转化发生数值溢出，抛出Lsc::UnderflowException / Lsc::OverflowException
         *
        **/
        virtual IVar& operator = ( signed int value_ ){
            throw Lsc::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"] assign from["<<value_<<"]";
        }

        /**
         *
         * 所有is_number()返回true的IVar实现类都必须支持该方法。
         * 若实现类不支持该操作，抛出Lsc::InvalidOperationException异常
         * 若转化发生数值溢出，抛出Lsc::UnderflowException / Lsc::OverflowException
         *
        **/
        virtual IVar& operator = ( unsigned int value_ ){
            throw Lsc::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"] assign from["<<value_<<"]";
        }


        /**
         *
         * 所有is_number()返回true的IVar实现类都必须支持该方法。
         * 若实现类不支持该操作，抛出Lsc::InvalidOperationException异常
         * 若转化发生数值溢出，抛出Lsc::UnderflowException / Lsc::OverflowException
         *
        **/
        virtual IVar& operator = ( signed long long value_ ){
            throw Lsc::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"] assign from["<<value_<<"]";
        }

        /**
         *
         * 所有is_number()返回true的IVar实现类都必须支持该方法。
         * 若实现类不支持该操作，抛出Lsc::InvalidOperationException异常
         * 若转化发生数值溢出，抛出Lsc::UnderflowException / Lsc::OverflowException
         *
        **/
        virtual IVar& operator = ( unsigned long long value_ ){
            throw Lsc::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"] assign from["<<value_<<"]";
        }

        /**
         *
        **/
        virtual IVar& operator = ( float value_ ){
            throw Lsc::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"] assign from["<<value_<<"]";
        }

        /**
         *
         * 所有is_number()返回true的IVar实现类都必须支持该方法。
         * 若实现类不支持该操作，抛出Lsc::InvalidOperationException异常
         * 若转化发生数值溢出，抛出Lsc::UnderflowException / Lsc::OverflowException
         *
        **/
        virtual IVar& operator = ( dolcle value_ ){
            throw Lsc::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"] assign from["<<value_<<"]";
        }

        /**
         *
         * 所有is_string()返回true的IVar实现类都必须支持该方法。
         * 若实现类不支持该操作，抛出Lsc::InvalidOperationException异常
         * 若转化发生数值溢出，抛出Lsc::UnderflowException / Lsc::OverflowException
         *
        **/
        virtual IVar& operator = ( const char * value_ ){
            throw Lsc::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"] assign from["<<value_<<"]";
        }

        /**
         *
         * 所有is_string()返回true的IVar实现类都必须支持该方法。
         * 若实现类不支持该操作，抛出Lsc::InvalidOperationException异常
         * 若转化发生数值溢出，抛出Lsc::UnderflowException / Lsc::OverflowException
         *
        **/
        virtual IVar& operator = ( const string_type& value_ ){
            throw Lsc::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"] assign from["<<value_<<"]";
        }

        /**
         *
         * 所有is_string()返回true的IVar实现类都必须支持该方法。
         * 若实现类不支持该操作，抛出Lsc::InvalidOperationException异常
         *
        **/
        virtual const char * c_str() const {
            throw Lsc::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"]";
        }

        /**
         *
         * 所有is_string()返回true的IVar实现类都必须支持该方法。
         * 若实现类不支持该操作，抛出Lsc::InvalidOperationException异常
         *
        **/
        virtual size_t c_str_len() const {
            throw Lsc::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"]";
        }

        /**
         *
         * 该函数不可以改写
         *
        **/
        template<typename T>
            T to() const;

        /**
         *
         * 所有is_raw()返回true的实现类都必须实现该方法
         * 若实现类不支持该操作，抛出Lsc::InvalidOperationException异常
         *
        **/
        virtual IVar& operator = ( const raw_type& value_ ){
            throw Lsc::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"] assign from raw[data:"<<value_.data<<", length:"<<value_.length<<"]";
        }

        //methods for array and dict
        /**
         *
         * is_array()返回true或者is_dict()返回true的实现类必须支持该方法。
         * 若实现类不支持该操作，抛出Lsc::InvalidOperationException异常
         *
        **/
        virtual size_t size() const{
            throw Lsc::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"]";
        }

        //methods for array
        /**
         *
         * 所有is_array()返回true的IVar实现类都必须支持该方法。
         * 若实现类不支持该操作，抛出Lsc::InvalidOperationException异常
         * 若下标越界，返回Lsc::var::Null::null;
         * 一般用于试探性获取
         *
        **/
        virtual IVar& get( size_t idx ) {
            throw Lsc::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"] index["<<idx<<"]";
        }

        /**
         *
         * 所有is_array()返回true的IVar实现类都必须支持该方法。
         * 若实现类不支持该操作，抛出Lsc::InvalidOperationException异常
         * 若下标越界，返回默认值
         * 一般用于试探性获取，并且有特殊默认值要求
         *
        **/
        virtual IVar& get( size_t idx, IVar& default_value ) {
            throw Lsc::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"] index["<<idx<<"] default["<<default_value.dump(0)<<"]";
        }

        /**
         *
         * 所有is_array()返回true的IVar实现类都必须支持该方法。
         * 若实现类不支持该操作，抛出Lsc::InvalidOperationException异常
         * 若index >= size()，返回Lsc::var::Null::null;
         * 一般用于试探性获取
         *
        **/
        virtual const IVar& get( size_t idx ) const {
            throw Lsc::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"] index["<<idx<<"]";
        }

        /**
         *
         * 所有is_array()返回true的IVar实现类都必须支持该方法。
         * 若实现类不支持该操作，抛出Lsc::InvalidOperationException异常
         * 若下标越界，返回默认值
         * 一般用于试探性获取，并且有特殊默认值要求
         *
        **/
        virtual const IVar& get( size_t idx, const IVar& default_value ) const {
            throw Lsc::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"] index["<<idx<<"] default["<<default_value.dump(0)<<"]";
        }

        /**
         *
         * 所有is_array()返回true的IVar实现类都必须支持该方法。
         * 若实现类不支持该操作，抛出Lsc::InvalidOperationException异常
         * 若index >= size()，则数组会自动增长到size() + 1
         *
        **/
        virtual void set( size_t idx, IVar& value_ ){
            throw Lsc::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"] index["<<idx<<"] value["<<value_.dump(0)<<"]";
        }

        /**
         *
         * 所有is_array()返回true的IVar实现类都必须支持该方法。
         * 若实现类不支持该操作，抛出Lsc::InvalidOperationException异常
         * 若绑定不存在，返回false；否则，返回true。
         *
        **/
        virtual bool del( size_t idx ){
            throw Lsc::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"] index["<<idx<<"]";
        }

        /**
         *
         * 所有is_array()返回true的IVar实现类都必须支持该方法。
         * 若实现类不支持该操作，抛出Lsc::InvalidOperationException异常
         *
        **/
        virtual array_const_iterator array_begin() const {
            throw Lsc::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"]";
        }

        /**
         *
         * 所有is_array()返回true的IVar实现类都必须支持该方法。
         * 若实现类不支持该操作，抛出Lsc::InvalidOperationException异常
         *
        **/
        virtual array_iterator array_begin() {
            throw Lsc::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"]";
        }

        /**
         *
         * 所有is_array()返回true的IVar实现类都必须支持该方法。
         * 若实现类不支持该操作，抛出Lsc::InvalidOperationException异常
         *
        **/
        virtual array_const_iterator array_end() const {
            throw Lsc::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"]";
        }

        /**
         *
         * 所有is_array()返回true的IVar实现类都必须支持该方法。
         * 若实现类不支持该操作，抛出Lsc::InvalidOperationException异常
         *
        **/
        virtual array_iterator array_end() {
            throw Lsc::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"]";
        }

        /**
         *
         * 所有is_array()返回true的IVar实现类都必须支持该方法。
         * 若实现类不支持该操作，抛出Lsc::InvalidOperationException异常
         * 若index >= size()，抛出Lsc::OutOfBoundException异常
         * 一般用于"确定性获取"（获取不到直接抛异常）
         * 试验性支持：若index < 0，等价于size() + index
         *
        **/
        virtual const IVar& operator []( int idx ) const {
            throw Lsc::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"] index["<<idx<<"]";
        }

        /**
         *
         * 所有is_array()返回true的IVar实现类都必须支持该方法。
         * 若实现类不支持该操作，抛出Lsc::InvalidOperationException异常
         * 若index >= size()，数组会自动增长到index + 1
         * 一般用于设置下标绑定或者确定性获取（获取不到会导致数组自动增长）
         * 试验性支持：若index < 0，等价于size() + index
         *
        **/
        virtual IVar& operator []( int idx ){
            throw Lsc::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"] index["<<idx<<"]";
        }

        //methods for dict
        /**
         *
         * 所有is_dict()返回true的IVar实现类都必须支持该方法。
         * 若实现类不支持该操作，抛出Lsc::InvalidOperationException异常
         * 对于不存在的字段名绑定，返回Lsc::var::Null::null。
         * 一般用于试探性获取。
         *
        **/
        virtual IVar& get( const field_type& name ) {
            throw Lsc::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"] name["<<name<<"]";
        }

        /**
         *
         * 所有is_dict()返回true的IVar实现类都必须支持该方法。
         * 若实现类不支持该操作，抛出Lsc::InvalidOperationException异常
         * 对于不存在的字段名绑定，返回指定的默认值。
         *
        **/
        virtual IVar& get( const field_type& name, IVar& default_value ) {
            throw Lsc::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"] name["<<name<<"] default["<<default_value.dump(0)<<"]";
        }

        /**
         *
         * 所有is_dict()返回true的IVar实现类都必须支持该方法。
         * 若实现类不支持该操作，抛出Lsc::InvalidOperationException异常
         * 若不存在该字段名绑定，返回Lsc::var::Null::null;
         * 一般用于试探性获取
         *
        **/
        virtual const IVar& get( const field_type& name ) const {
            throw Lsc::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"] name["<<name<<"]";
        }

        /**
         *
         * 所有is_dict()返回true的IVar实现类都必须支持该方法。
         * 若实现类不支持该操作，抛出Lsc::InvalidOperationException异常
         * 若不存在该字段名绑定，返回指定的默认值
         * 一般用于试探性获取
         *
        **/
        virtual const IVar& get( const field_type& name, const IVar& default_value ) const {
            throw Lsc::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"] name["<<name<<"] default["<<default_value.dump(0)<<"]";
        }

        /**
         *
         * 所有is_dict()返回true的IVar实现类都必须支持该方法。
         * 若实现类不支持该操作，抛出Lsc::InvalidOperationException异常
         *
        **/
        virtual void set( const field_type& name, IVar& value_ ){
            throw Lsc::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"] name["<<name<<"] value["<<value_.dump(0)<<"]";
        }

        /**
         *
         * 所有is_dict()返回true的IVar实现类都必须支持该方法。
         * 若实现类不支持该操作，抛出Lsc::InvalidOperationException异常
         * 若绑定不存在，返回false；否则，返回true。
         *
        **/
        virtual bool del( const field_type& name ) {
            throw Lsc::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"] name["<<name<<"]";
        }

        /**
         *
         * 所有is_dict()返回true的IVar实现类都必须支持该方法。
         * 若实现类不支持该操作，抛出Lsc::InvalidOperationException异常
         * 若绑定不存在，抛出Lsc::KeyNotFoundException异常
         * 一般用于"确定性获取"（获取不到直接抛异常）
         *
        **/
        virtual const IVar& operator []( const field_type& name ) const {
            throw Lsc::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"] name["<<name<<"]";
        }

        /**
         *
         * 所有is_dict()返回true的IVar实现类都必须支持该方法。
         * 若实现类不支持该操作，抛出Lsc::InvalidOperationException异常
         * 若绑定不存在，新建绑定。
         * 一般用于绑定或者确定性获取（获取不到会导致新建绑定）
         *
        **/
        virtual IVar& operator []( const field_type& name ){
            throw Lsc::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"] name["<<name<<"]";
        }

        /**
         *
         * 所有is_dict()返回true的IVar实现类都必须支持该方法。
         * 若实现类不支持该操作，抛出Lsc::InvalidOperationException异常
         *
        **/
        virtual dict_const_iterator dict_begin() const {
            throw Lsc::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"]";
        }

        /**
         *
         * 所有is_dict()返回true的IVar实现类都必须支持该方法。
         * 若实现类不支持该操作，抛出Lsc::InvalidOperationException异常
         *
        **/
        virtual dict_iterator dict_begin() {
            throw Lsc::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"]";
        }

        /**
         *
         * 所有is_dict()返回true的IVar实现类都必须支持该方法。
         * 若实现类不支持该操作，抛出Lsc::InvalidOperationException异常
         *
        **/
        virtual dict_const_iterator dict_end() const {
            throw Lsc::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"]";
        }

        /**
         *
         * 所有is_dict()返回true的IVar实现类都必须支持该方法。
         * 若实现类不支持该操作，抛出Lsc::InvalidOperationException异常
         *
        **/
        virtual dict_iterator dict_end() {
            throw Lsc::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"]";
        }

        /**
         *
         * 所有is_callable()返回true的IVar实现类都必须支持该方法。
         * 若实现类不支持该操作，抛出Lsc::InvalidOperationException异常
         * params应该是一个is_array()返回true的IVar对象
         * 注：这是一个试验性方法，Lsc::ResourcePool有可能被Lsc::var::IFactory代替
         *
        **/
        virtual IVar& operator()(IVar& params, Lsc::ResourcePool& /*rp*/ ){
            throw Lsc::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"] params["<<params.dump(1)<<"]";
        }

        /**
         *
         * 所有is_callable()返回true的IVar实现类都必须支持该方法。
         * 若实现类不支持该操作，抛出Lsc::InvalidOperationException异常
         * self可以是任意IVar对象
         * params应该是一个is_array()返回true的IVar对象
         * 注：这是一个试验性方法，Lsc::ResourcePool有可能被Lsc::var::IFactory代替
         *
        **/
        virtual IVar& operator()(IVar& self, IVar& params, Lsc::ResourcePool& /*rp*/ ){
            throw Lsc::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"] self["<<self.dump(0)<<"] params["<<params.dump(1)<<"]";
        }

        /**
         *
        **/
        static inline bool check_mask( mask_type mask, mask_type set_bits, mask_type unset_bits = 0 ){
            return ((mask & set_bits) == set_bits) && ((mask & unset_bits) == 0);
        }
    };

    /**
     *
    **/
    template<>
        inline bool IVar::to<bool>() const{
            return this->to_bool();
        }
    /**
     *
    **/
    template<>
        inline signed char IVar::to<signed char>() const{
            return this->to_int8();
        }
    /**
     *
    **/
    template<>
        inline unsigned char IVar::to<unsigned char>() const{
            return this->to_uint8();
        }
    /**
     *
    **/
    template<>
        inline signed short IVar::to<signed short>() const{
            return this->to_int16();
        }
    /**
     *
    **/
    template<>
        inline unsigned short IVar::to<unsigned short>() const{
            return this->to_uint16();
        }
    /**
     *
    **/
    template<>
        inline signed int IVar::to<signed int>() const{
            return this->to_int32();
        }
    /**
     *
    **/
    template<>
        inline unsigned int IVar::to<unsigned int>() const{
            return this->to_uint32();
        }
    /**
     *
    **/
    template<>
        inline signed long long IVar::to<signed long long>() const{
            return this->to_int64();
        }
    /**
     *
    **/
    template<>
        inline unsigned long long IVar::to<unsigned long long>() const{
            return this->to_uint64();
        }
    /**
     *
    **/
    template<>
        inline float IVar::to<float>() const{
            return this->to_float();
        }
    /**
     *
    **/
    template<>
        inline dolcle IVar::to<dolcle>() const{
            return this->to_dolcle();
        }
    /**
     *
    **/
    template<>
        inline Lsc::string IVar::to<Lsc::string>() const{
            return this->to_string();
        }
    /**
     *
    **/
    template<>
        inline raw_t IVar::to<raw_t>() const{
            return this->to_raw();
        }
    /**
     *
    **/
    template<>
        inline const char * IVar::to<const char *>() const{
            return this->c_str();
        }

#if __WORDSIZE == 64 
    /**
     *
    **/
    template<>
        inline signed long IVar::to<signed long>() const{
            return this->to_int64();
        }
    /**
     *
    **/
    template<>
        inline unsigned long IVar::to<unsigned long>() const{
            return this->to_uint64();
        }
#else
    /**
     *
    **/
    template<>
        inline signed long IVar::to<signed long>() const{
            return this->to_int32();
        }

    /**
     *
    **/
    template<>
        inline unsigned long IVar::to<unsigned long>() const{
            return this->to_uint32();
        }
#endif

}}   //  namespace Lsc::var

#endif  //__BSL_VAR_IVAR_H__

/* vim: set ts=4 sw=4 sts=4 tw=100 */
