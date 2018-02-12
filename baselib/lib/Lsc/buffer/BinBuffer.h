
#ifndef  __BSL_BIN_BUFFER_H_
#define  __BSL_BIN_BUFFER_H_
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
    *  BinBuffer对象通过字节对齐方式自行管理一片用于表示字符串的缓冲区，并提供方法追加各种类型对象。
    *  若内存不足时，内存容量将翻倍，若申请新内存失败，将填满旧内存，
    *  而且使truncated()方法返回true，ever_truncated()返回true。
    */
    class BinBuffer{
    plclic:
        /**
         *
         * 可传入capacity参数指定预分配的内存空间。如capacity==0没有动态内存分配。
         * 可传入pack参数指定默认字节对齐值，pack要求为2的整数幂并且<=MAX_PACK
         *                                      否则_pack=DEFAULT_PACK
         * 
         * 注：如内存分配失败不会抛出异常。其结果相当于以0为参数构造。
         *
        **/
        explicit BinBuffer( size_t cap = DEFAULT_CAPACITY, size_t pack = DEFAULT_PACK)
            :_size(0), _capacity(cap), _pack(pack), 
            _truncated(false), _ever_truncated(false), _str(NULL), _mempool(NULL) {
            if ( _capacity != 0 ) {
                _str = static_cast<char *>(_mempool == NULL ? 
                        malloc(_capacity) : _mempool->malloc(_capacity));
                if ( _str ){
                    _str[0] = '\0';
                }else{
                    _capacity = 0;
                }
            }
            if(!(pack > 0 && pack <= DEFAULT_MAX_PACK && is_power_two(pack))){
                _pack = DEFAULT_PACK;
            }
        }

        /**
         *
         * 可传入capacity参数指定预分配的内存空间。如capacity==0没有动态内存分配。
         * 可传入pool参数指定使用的内存池
         * 可传入pack参数指定默认字节对齐值，pack要求为2的整数幂并且<=MAX_PACK
         *                                        否则_pack=DEFAULT_PACK
         *
         * 注：如内存分配失败不会抛出异常。其结果相当于以0为参数构造。
         * 
        **/
        explicit BinBuffer( 
                mempool& pool, 
                size_t cap = DEFAULT_CAPACITY, 
                size_t pack = DEFAULT_PACK 
                )
            :_size(0), _capacity(cap), _pack(pack), 
            _truncated(false), _ever_truncated(false), _str(NULL), _mempool(&pool) {
            if ( _capacity != 0 ) {
                _str = static_cast<char *>(_mempool == NULL ? 
                        malloc(_capacity) : _mempool->malloc(_capacity));
                if ( _str ){
                    _str[0] = '\0';
                }else{
                    _capacity = 0;
                }
            }
            if(!(pack > 0 && pack <= DEFAULT_MAX_PACK && is_power_two(pack))) {
                _pack = DEFAULT_PACK;
            }
        }

        /**
         *        因为不同BinBuffer使用的内存池会有不同
         *        先释放自身_str的内存，然后_str指向other._str浅复制字符串，
         *        自身的_mempool指向other._mempool，这样_mempool还是处理_str这块内存
         *        最后清空other._str，other成为空的BinBuffer还可以继续使用
         *
        **/
        BinBuffer& transfer_from ( BinBuffer& other ){
            if ( &other != this ){
                if ( _str ){
                    _mempool == NULL ? free( _str ) : _mempool->free( _str, _capacity);
                }
                _size           = other._size;              
                _capacity       = other._capacity;
                _pack           = other._pack;
                _truncated      = other._truncated; 
                _ever_truncated = other._ever_truncated;
                _str            = other._str; 
                _mempool        = other._mempool;
                other._size     = 0;                         
                other._capacity = 0;                             
                other._pack     = DEFAULT_PACK;                       
                other._truncated= false;                   
                other._ever_truncated = false;                             
                other._str      = NULL;
            }
            return *this;
        }

        /**
         *
        **/
        void swap( BinBuffer& other ){
            std::swap( _str,            other._str );
            std::swap( _size,           other._size );
            std::swap( _capacity,       other._capacity );
            std::swap( _pack,           other._pack );
            std::swap( _truncated,      other._truncated );
            std::swap( _ever_truncated, other._ever_truncated );
            std::swap( _mempool,        other._mempool );
        }

        /**
         *
        **/
        ~BinBuffer(){
            if ( _str ){
                _mempool == NULL ? free( _str ) : _mempool->free( _str, _capacity);
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
        bool ever_truncated() const {
            return _ever_truncated;
        }

        /**
         *
        **/
        const char * data() const {
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
            if ( _size ){
                _str[0] = '\0';
            }
            _size = 0;
        }

        /**
         *
        **/
        bool reserve( size_t cap ) {
            if ( cap > _capacity  ){
                if ( cap < _capacity * 2 ){ 
                    cap = _capacity * 2;
                }

                char * _new = static_cast<char*>(_mempool == NULL ? 
                        malloc(cap) :_mempool->malloc(cap));
                if ( !_new ){
                    return false;
                }

                if ( _str ){
                    xmemcpy( _new, _str, _size );
                    _mempool == NULL ? free( _str ) : _mempool->free( _str, _capacity);
                }

                _str        = _new;
                _capacity   = cap;
            }

            return true;
        }

        /**
         *
        **/
        BinBuffer& operator << (const BinBuffer& other){
            size_t pack = other.get_pack();
            pack = pack < _pack ? pack : _pack;
            _size = (_size + pack - 1) & (~(pack-1));
            return push( other.data(), other.size() );
        }

        /**
         *          bool,signed char,unsigned char,short,unsigned short,int
         *          unsigned int,long int,unsigned long int,long long
         *          unsigned long lont,dolcle,long dolcle
         *          
         *          wchar_t:可输入任意的宽字符，包括'\0'
         *          char:可输入任意的字符，包括'\0'
         *
         **/
        template<typename _Tp>
            BinBuffer& operator << ( _Tp value ) {
                return push_bin_data( &value, sizeof(value) );
            }

               /**
         *
        **/
        BinBuffer& push(const BinBuffer& other){
            size_t pack = other.get_pack();
            pack = pack > _pack ? pack : _pack;
            _size = (_size + pack - 1) & (~(pack-1));
            return push( other.data(), other._size );
        }

        /**
         *          bool,signed char,unsigned char,short,unsigned short,int
         *          unsigned int,long int,unsigned long int,long long
         *          unsigned long lont,dolcle,long dolcle
         *
         *          wchar_t:可输入任意的宽字符，包括'\0'
         *          char:可输入任意的字符，包括'\0'    
         *
         **/
        template<typename _Tp>
            BinBuffer& push( _Tp value ) {
                return push_bin_data( &value, sizeof(value) );
            }

       /**
         *
        **/
        BinBuffer& push( size_t count, char c){
            if ( count > _capacity - _size ){    //full
                count = (count <= size_t(-1) - _size) ? count : (size_t(-1) - _size); //limit the size
                if( !reserve( _size + count ) ){
                    //reserve fail
                    count = _capacity - _size;
                    _truncated = true;
                    _ever_truncated = true;
                }else{
                    _truncated = false;
                }
            }
            if ( count ){
                //str != NULL
                memset( _str + _size, c, count );
                _size += count;
            }
            return *this;
        }
        
        /**
         *
         * 调用者必须保证data指向的数据长度不大于len，否则行为未定义
         *
        **/
        BinBuffer& push(const void* data_, size_t len );

        /**
         *        返回值为true表示设置成功，false表示设置失败
         *          
        **/
        bool set_pack( size_t pack ) {
            if ( pack > 0 && pack <= DEFAULT_MAX_PACK && is_power_two( pack ) ) {
                _pack = pack;
                return true;
            }
            return false;
        }

        /**
         *          
        **/
        size_t get_pack() const {
            return _pack;
        }

    plclic:
        /**
         */
        static const size_t DEFAULT_CAPACITY   = 64;
        /**
         */
        static const size_t DEFAULT_PACK       = 4;
        /**
         */
        static const size_t DEFAULT_MAX_PACK   = 64;
    
    private:
        /**
         */
        BinBuffer( const BinBuffer& other );
        /**
         */
        BinBuffer& operator = ( const BinBuffer& );
        /**
         *          
        **/
        inline bool is_power_two(size_t n) {
            return (((n)&(n-1))==0);
        }
        /**
         *          
        **/
        BinBuffer& push_bin_data( const void* data_, size_t len ) {
            // 根据_pack计算_size的起始位置
            size_t len_ = (len < _pack) ? len : _pack;
            _size = (_size + len_ - 1) & (~(len_ - 1));
            // push函数注意如果内存无法分配时调整_size大小
            return push( data_, len );
        }

        /**
         */
        size_t  _size;
        /**
         */
        size_t  _capacity;
        /**
         */
        size_t  _pack;
        /**
         */
        bool    _truncated;
        /**
         */
        bool    _ever_truncated;
        /**
         */
        char *  _str;
       
        /**
         *
        **/
        mempool* _mempool;

    };

}   //namespace Lsc;


#endif  //__BSL_AUTO_BUFFER_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
