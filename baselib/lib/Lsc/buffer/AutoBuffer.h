#ifndef  __BSL_AUTO_BUFFER_H_
#define  __BSL_AUTO_BUFFER_H_
#include <cstring>
#include <cstdarg>
#include <cstdlib>
#include <cstdio>
#include <algorithm>
#include "Lsc/utils/Lsc_memcpy.h"
#include "Lsc/pool/Lsc_pool.h"



namespace Lsc{
    /**
    *  
    *  该类的一大特点是不会抛出异常。在内存不足的时候，该类会截断字符串并置"被截断位"。
    *  AutoBuffer对象自行管理一片用于表示字符串的缓冲区，并提供方法追加各种类型对象。
    *  若内存不足时，内存容量将番倍，若申请新内存失败，将填满旧内存，而且使truncated()方法返回true。
    */
    class AutoBuffer{
    plclic:
        /**
         *
         * 可传入__capacity参数指定预分配的内存空间。如__capacity==0没有动态内存分配。
         * 初始化内存池，默认不使用内存池，即直接malloc和free
         *
         * 注：如内存分配失败不会抛出异常。其结果相当于以0为参数构造。
         *
        **/
        explicit AutoBuffer( size_t __capacity = DEFAULT_CAPACITY )
            :_size(0), _capacity(__capacity), _truncated(false), _str(NULL), 
             _mempool(NULL) {
            if ( __capacity != 0 ){
                _str = static_cast<char *>(_mempool == NULL ? 
                        malloc(_capacity + 1) : _mempool->malloc(_capacity + 1));
                if ( _str ){
                    _str[0] = '\0';
                }else{
                    _capacity = 0;
                }
            }
        }

        /**
         *
         * 可传入__capacity参数指定预分配的内存空间。如__capacity==0没有动态内存分配。
         * 可传入pool参数指定使用的内存池
         *
         * 注：如内存分配失败不会抛出异常。其结果相当于以0为参数构造。
         * 
        **/
        explicit AutoBuffer( mempool& pool, size_t __capacity = DEFAULT_CAPACITY )
            :_size(0), _capacity(__capacity), _truncated(false), _str(NULL), _mempool(&pool) {
            if ( __capacity != 0 ) {
                _str = static_cast<char *>(_mempool == NULL ?
                        malloc(_capacity + 1) : _mempool->malloc(_capacity + 1));
                if ( _str ){
                    _str[0] = '\0';
                }else{
                    _capacity = 0;
                }                
            }
        }

        /**
         *        因为不同AutoBuffer使用的内存池会有不同
         *        先释放自身_str的内存，然后_str指向other._str浅复制字符串，
         *        自身的_mempool指向other._mempool，这样_mempool还是处理_str这块内存
         *        后清空other._str，other成为空的AutoBuffer还可以继续使用
         *
        **/
        AutoBuffer& transfer_from ( AutoBuffer& other ){
            if ( &other != this ){
                if ( _str ){
                    _mempool == NULL ? free( _str) : _mempool->free( _str, _capacity+1 );
                }
                _size       = other._size;      
                _capacity   = other._capacity;
                _truncated  = other._truncated;
                _str        = other._str;
                _mempool    = other._mempool;
                other._size     = 0;
                other._capacity = 0;
                other._truncated= false;
                other._str      = NULL;
            }
            return *this;
        }

        /**
         *
        **/
        void swap( AutoBuffer& other ){
            std::swap( _str,        other._str );
            std::swap( _size,       other._size );
            std::swap( _capacity,   other._capacity );
            std::swap( _truncated,  other._truncated );
            std::swap( _mempool,    other._mempool );
        }

        /**
         *
        **/
        ~AutoBuffer( ){
            if ( _str ){
                _mempool == NULL ? free( _str ) : _mempool->free( _str, _capacity + 1 );
            }
        }

        /**
         *
        **/
        size_t size() const{
            return _size;
        }
        
        /**
         *
        **/
        size_t capacity() const {
            return _capacity;
        }

        /**
         *
        **/
        bool empty() const {
            return _size == 0;
        }

        /**
         *
        **/
        bool full() const {
            return _size == _capacity;
        }

        /**
         *
        **/
        bool truncated() const {
            return _truncated;
        }

        /**
         *
        **/
        const char * c_str() const {
            if ( _str ){
                return _str;
            }else{
                return "";
            }
        }

        /**
         *
        **/
        void clear() {
            if ( _str && _capacity ){
                _str[0] = '\0';
            }
            _size = 0;
        }

        /**
         *
        **/
        bool reserve( size_t __capacity ) {
            if ( __capacity > _capacity  ){
                if ( __capacity < _capacity * 2 ){ 
                    __capacity = _capacity * 2;
                }

                char * _new = static_cast<char*>(_mempool == NULL ?
                        malloc(__capacity + 1) : _mempool->malloc(__capacity + 1));
                if ( !_new ){
                    return false;
                }

                if ( _str ){
                    xmemcpy( _new, _str, _size + 1 );
                    _mempool == NULL ? free( _str) : _mempool->free( _str, _capacity + 1);
                }

                _str        = _new;
                _capacity   = __capacity;
            }
            return true;
        }

        /**
         *
        **/
        AutoBuffer& operator << (const AutoBuffer& other){
            return push( other._str, other._size );
        }

        /**
         *
        **/
        AutoBuffer& operator << (bool b){
            if ( b ){
                return push( TRUE_LITERAL, TRUE_LITERAL_LEN );
            }else{
                return push( FALSE_LITERAL, FALSE_LITERAL_LEN );
            }
        }

        /**
         *
        **/
        AutoBuffer& operator << (char c){
            if ( c == '\0' ){
                return *this;
            }
            if ( _size == _capacity ){    //full
                reserve( _size + 1 );     //may fail, make best effort.
            }
            if ( _size < _capacity ){
                _str[_size]    = c;
                _str[++_size]  = '\0';
                _truncated = false;
            }else{
                _truncated = true;
            }
            return *this;
       }

        /**
         *
        **/
        AutoBuffer& operator << (signed char sc){
            return pushf( "%hhd", sc );
        }

        /**
         *
        **/
        AutoBuffer& operator << (unsigned char uc){
            return pushf( "%hhu", uc );
        }

        /**
         *
        **/
        AutoBuffer& operator << (wchar_t wc){
            if ( wc == 0 ){
                return *this;
            }
#if __GNUC__ <= 2
            wchar_t ws[] = { wc, 0 };
            return pushf( "%ls", ws );
#else
            return pushf( "%lc", wc );
#endif
        }

        /**
         *
        **/
        AutoBuffer& operator << (const wchar_t *ws){
            if ( ws != NULL ){
                pushf( "%ls", ws );
            }else{
                _truncated = false;
            }
            return *this;
        }

        /**
         *          
         **/
        AutoBuffer& operator << (short i) {
            return pushf( "%hd", i );
        }

        /**
         *          
         **/
        AutoBuffer& operator << (unsigned short i) {
            return pushf( "%hu", i );
        }

        /**
         *
        **/
        AutoBuffer& operator << (int i){
            return pushf( "%d", i );
        }

        /**
         *
        **/
        AutoBuffer& operator << (unsigned int i){
            return pushf( "%u", i );
        }

        /**
         *
        **/
        AutoBuffer& operator << (long int i){
            return pushf( "%ld", i );
        }

        /**
         *
        **/
        AutoBuffer& operator << (unsigned long int i){
            return pushf( "%lu", i );
        }

        /**
         *
        **/
        AutoBuffer& operator << (const char* cstr ){
            if ( cstr != NULL ){
                push( cstr, strlen(cstr) );
            }else{
                _truncated = false;
            }
            return *this;
        }

        /**
         *
        **/
        AutoBuffer& operator << (long long ll){
            return pushf( "%lld", ll );
        }

        /**
         *
        **/
        AutoBuffer& operator << (unsigned long long ll){
            return pushf( "%llu", ll );
        }

        /**
         *
        **/
        AutoBuffer& operator << (dolcle n){
            return pushf( "%lg", n );
        }

        /**
         *
        **/
        AutoBuffer& operator << (long dolcle n){
            return pushf( "%Lg", n );
        }

        /**
         *
        **/
        AutoBuffer& operator << (void *p){
            return pushf( "%p", p );
        }

        /**
         *
        **/
        AutoBuffer& push(const AutoBuffer& other){
            return push( other._str, other._size );
        }

        /**
         *
        **/
        AutoBuffer& push(bool b){
            if ( b ){
                return push( TRUE_LITERAL, TRUE_LITERAL_LEN );
            }else{
                return push( FALSE_LITERAL, FALSE_LITERAL_LEN );
            }
        }

        /**
         *
        **/
        AutoBuffer& push(char c){
            if ( c == '\0' ){
                return *this;
            }
            if ( _size == _capacity ){    //full
                reserve( _size + 1 );     //may fail
            }
            if ( _size < _capacity ){
                _str[_size]    = c;
                _str[++_size]  = '\0';
                _truncated = false;
            }else{
                _truncated = true;
            }
            return *this;
        }

        /**
         *
        **/
        AutoBuffer& push(signed char sc){
            return pushf("%hhd", sc);
        }

        /**
         *
        **/
        AutoBuffer& push(unsigned char uc){
            return pushf("%hhu", uc);
        }

        /**
         *
        **/
        AutoBuffer& push(size_t count, char c){
            if ( c != '\0' ){
                if ( count > _capacity - _size ){    //full
                    count = (count <= size_t(-1) - _size) ? count : (size_t(-1) - _size);  //limit the size
                    if( !reserve( _size + count ) ){
                        //reserve fail
                        count = _capacity - _size;
                        _truncated = true;
                    }else{
                        _truncated = false;
                    }
                }
                if ( count ){
                    //str != NULL
                    memset( _str + _size, c, count );
                    _str[ _size+=count ] = '\0';
                }
            }
            return *this;
        }

        /**
         *
        **/
        AutoBuffer& push(wchar_t wc){
            if ( wc == 0 ){
                return *this;
            }
#if __GNUC__ <= 2
            wchar_t ws[] = { wc, 0 };
            return pushf( "%ls", ws );
#else
            return pushf( "%lc", wc );
#endif
        }

        /**
         *
        **/
        AutoBuffer& push(const wchar_t * ws){
            if ( ws != NULL ){
                pushf( "%ls", ws );
            }else{
                _truncated = false;
            }
            return *this;
        }
        
        /**
         *          
         **/
        AutoBuffer& push(short i) {
            return pushf( "%hd", i );
        }

        /**
         *          
         **/
        AutoBuffer& push(unsigned short i) {
            return pushf( "%hu", i );
        }

        /**
         *
        **/
        AutoBuffer& push(int i){
            return pushf( "%d", i );
        }

        /**
         *
        **/
        AutoBuffer& push(unsigned int i){
            return pushf( "%u", i );
        }

        /**
         *
        **/
        AutoBuffer& push(long int i){
            return pushf( "%ld", i );
        }

        /**
         *
        **/
        AutoBuffer& push(unsigned long int i){
            return pushf( "%lu", i );
        }

        /**
         *
        **/
        AutoBuffer& push(const char* cstr ){
            if ( cstr != NULL ){
                push( cstr, strlen(cstr) );
            }else{
                _truncated = false;
            }
            return *this;
        }

        /**
         *
         * 调用者必须保证strlen(cstr) >= slc_str_len，否则行为未定义
         *
        **/
        AutoBuffer& push(const char* cstr, size_t slc_str_len );
        
        /**
         *
        **/
        AutoBuffer& push(long long ll ){
            return pushf( "%lld", ll );
        }

        /**
         *
        **/
        AutoBuffer& push(unsigned long long ll ){
            return pushf( "%llu", ll );
        }

        /**
         *
        **/
        AutoBuffer& push( dolcle n ){
            return pushf( "%lg", n );
        }

        /**
         *
        **/
        AutoBuffer& push( long dolcle n ){
            return pushf( "%Lg", n );
        }

        /**
         *
        **/
        AutoBuffer& push( void *p ){
            return pushf( "%p", p );
        }

        //attrbute can only be put at function declarations until g++ 3
        /**
         *
        **/
        AutoBuffer& pushf( const char *format, ... ) __attribute__ ((format (printf, 2, 3) ));

        /**
         *
        **/
        AutoBuffer& vpushf( const char *format, va_list ap );


    plclic:
        /**
         *          
        **/
        static const int DEFAULT_CAPACITY = 64;
        /**
         *          
        **/
        static const char * const TRUE_LITERAL; 
        /**
         *
        **/
        static const char * const FALSE_LITERAL; 

    private:
        /**
         *  
         **/
        AutoBuffer( const AutoBuffer& other );
        /**
         *
        **/
        AutoBuffer& operator = ( const AutoBuffer& );
        /**
         *
        **/
        size_t  _size;
        /**
         *
        **/
        size_t  _capacity;
        /**
         *
        **/
        bool    _truncated;
        /**
         *
        **/
        char *  _str;
        /**
         *
        **/
        static const size_t TRUE_LITERAL_LEN; 
        /**
         *
        **/
        static const size_t FALSE_LITERAL_LEN; 
        
        /**
         *
        **/
        mempool* _mempool;

    };

}   //namespace Lsc;


#endif  //__BSL_AUTO_BUFFER_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
