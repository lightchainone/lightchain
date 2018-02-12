
#ifndef  __SHALLOWCOPYSTRING_H_
#define  __SHALLOWCOPYSTRING_H_
#include <cstring>
#include <iostream>
#include "Lsc/exception/Lsc_exception.h"
#include "Lsc/containers/string/Lsc_string.h"

namespace Lsc{

    class ShallowCopyString{
    plclic:
        typedef char            value_type;
        typedef size_t          size_type;
        typedef long            difference_type;
        typedef value_type &    reference;
        typedef const reference const_reference;
        typedef value_type *    pointer;
        typedef const pointer   const_pointer;
        typedef pointer         iterator;
        typedef const_pointer   const_iterator;
#if __GNUC__ <= 2
        typedef string_char_traits<value_type> traits_type;
#else
        typedef std::char_traits<value_type> traits_type;
#endif
        //ctors and dtors

        /**
         *
         * 默认构造的ShallowCopyString表示空串。
         **/
        ShallowCopyString() 
            : _str(_S_EMPTY_CS()), _length(0){}

        /**
         *
         * 该函数把C风格字符串转化为ShallowCopyString。
         *
         * 该方法时间复杂度为O(1)，长度被懒惰求值。（直到size()方法第一次被调用）
         *
        **/
        ShallowCopyString( const char * str )  
            : _str(str), _length(npos) {
                if ( NULL == str ){
                    throw NullPointerException()<<BSL_EARG<<"str is NULL";
                }
            }

        /**
         *
         * 注意：len必须与strlen(str)相等，否则行为未定义。
         *
        **/
        ShallowCopyString( const char * str, size_t len ) 
            : _str(str), _length(len){
                if ( NULL == str ){
                    throw NullPointerException()<<BSL_EARG;
                }
                if ( str[len] != '\0' ){
                    //给出的长度值有误？
                    throw Lsc::BadArgumentException()<<BSL_EARG<<"wrong len:"<<len;
                }
            }

        /**
         *
        **/
        template<class allocator_t>
        ShallowCopyString( const Lsc::basic_string<char, allocator_t>& Lsc_str )
            : _str(Lsc_str.c_str()), _length(Lsc_str.length()) {};

        /**
         *
        **/
        ShallowCopyString( const Lsc::AutoBuffer& buf )
            : _str(buf.c_str()), _length(buf.size()){}

        /**
         *
        **/
        ShallowCopyString( const ShallowCopyString& sstr ) 
            : _str(sstr._str), _length(sstr._length) {}

        /**
         *
        **/
        ~ShallowCopyString() {
            //do nothing
        }

        void clear(){
            _str = _S_EMPTY_CS();
            _length = 0;
        }
        // getters
        /**
         *
        **/
        const char * c_str() const  {
            return _str;
        }

        /**
         *
         * 若字符串使用ShallowCopyString(const char *)构造并且本函数从未调用过，时间复杂为度O(strlen(this->c_str()))，否则为O(1)
         *
        **/
        size_t size() const {
            if ( _length == npos ){
                const_cast<size_t&>(_length) = strlen(_str);
            }
            return _length;
        }

        /**
         *
        **/
        size_t length() const {
            return size();
        }

        /**
         *
        **/
        size_t capacity() const {
            return size();
        }
        
        /**
         *
         * 注：本函数不检查idx的合法性
        **/
        char operator []( size_t idx ) const {
            return _str[idx];
        }

        /**
         *
         * 本函数检查idx合法性，若idx>=this->size()，抛出Lsc::OutOfBoundException
        **/
        char at( size_t idx ) const {
            if ( idx < this->size() ){
                return _str[idx];
            }
            throw Lsc::OutOfBoundException()<<BSL_EARG<<"idx:"<<idx<<" size():"<<size();
        }
            
        /**
         *
         * 浅复制，时间复杂度为O(1)
        **/
        ShallowCopyString& operator = ( const ShallowCopyString& sstr ){
            _str        = sstr._str;
            _length     = sstr._length;
            return *this;
        }

        ShallowCopyString& operator = ( const AutoBuffer& buf ){
            _str        = buf.c_str();
            _length     = buf.size();
            return *this;
        }

        ShallowCopyString& operator = ( const Lsc::string& str ){
            _str        = str.c_str();
            _length     = str.size();
            return *this;
        }

        ShallowCopyString& operator = ( const char *cstr ){
            if ( cstr == NULL ){
                throw Lsc::NullPointerException()<<BSL_EARG<<"cstr is NULL";
            }
            if ( cstr != _str ){
                _str        = cstr;
                _length     = npos;
            }
            return *this;
        }

        /**
         *
         * 若本字符串字典序小于、等于、大于other，则返回值<0、=0、>0
         * 若两字符串首字母不相同，或字符串地址相同，该函数时间复杂度为O(1)
         * 
         *
        **/
        int compare( const ShallowCopyString& other ) const {
            if ( _str[0] != other._str[0] ){
                return traits_type::lt(_str[0], other._str[0]) ? -1 : 1;
            }
            if ( _str == other._str ){
                return 0;   //对于ShallowCopyString来说这是有可能的。
            }
            size_t min_size = std::min( size() , other.size() );
            int ret = memcmp( _str, other._str, min_size );
            if ( ret == 0 ){
                if (_length > other._length) {
                    return 1;
                } else if (_length == other._length) {
                    return 0;
                } else {
                    return -1;
                }
            }else{
                return ret;
            }
        }

        //comparers
        //因为const char *转换为ShallowCopyString是O(1)的，ShallowCopyString与一个随机的C字符串比较也是平均O(1)的（预判断第一字节），所以不专门对C字符串做比较函数了。
        friend inline bool operator == (const ShallowCopyString& sstr1, const ShallowCopyString& sstr2){
            return sstr1.compare( sstr2 ) == 0;
        }

        friend inline bool operator != (const ShallowCopyString& sstr1, const ShallowCopyString& sstr2){
            return sstr1.compare( sstr2 ) != 0;
        }

        friend inline bool operator < (const ShallowCopyString& sstr1, const ShallowCopyString& sstr2){
            return sstr1.compare( sstr2 ) < 0;
        }

        friend inline bool operator > (const ShallowCopyString& sstr1, const ShallowCopyString& sstr2){
            return sstr1.compare( sstr2 ) > 0;
        }

        friend inline bool operator <= (const ShallowCopyString& sstr1, const ShallowCopyString& sstr2){
            return sstr1.compare( sstr2 ) <= 0;
        }

        friend inline bool operator >= (const ShallowCopyString& sstr1, const ShallowCopyString& sstr2){
            return sstr1.compare( sstr2 ) >= 0;
        }

        /**
         *
        **/
        friend inline std::ostream& operator << ( std::ostream& os, const ShallowCopyString& sstr ){
            return os << sstr._str ;
        }
        
        static const size_t npos = static_cast<size_t>(-1);

    private:
        const char *    _str;
        size_t          _length;

        static const char *_S_EMPTY_CS(){
            static const char ret[] = "";
            return ret;
        };
    };
}   // namespace Lsc

/**
 *
**/
inline Lsc::AutoBuffer& operator <<( Lsc::AutoBuffer& buf, Lsc::ShallowCopyString& str ){
    return buf.push(str.c_str(), str.length());
}

#if __GNUC__ >=3
namespace __gnu_cxx{
#else
namespace std{
#endif
    /**
    *  
    *  使用与const char*相同的hash算法。
    */
    template<>
    struct hash<Lsc::ShallowCopyString>{
        size_t operator()(const Lsc::ShallowCopyString& str ) const {
            return __stl_hash_string(str.c_str());
        }
    };
}   // namespace __gnu_cxx/std

namespace Lsc{
    /**
    *  
    *  使用与const char*相同的hash算法。
    */
    template<>
    struct xhash<Lsc::ShallowCopyString>{
        size_t operator()(const Lsc::ShallowCopyString& str ) const {
            return __Lsc_hash_string(str.c_str());
        }
    };
}   // namespace Lsc

#endif  //__SHALLOWCOPYSTRING_H_
/* vim: set ts=4 sw=4 sts=4 tw=100 */
