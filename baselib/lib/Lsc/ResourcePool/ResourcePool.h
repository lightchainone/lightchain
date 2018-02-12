
#ifndef  __BSL_RESOURCEPOOL_H_
#define  __BSL_RESOURCEPOOL_H_

#include "Lsc/exception/Lsc_exception.h"
#include "Lsc/pool/Lsc_pool.h"
#include "Lsc/pool/Lsc_poolalloc.h"
#include "Lsc/utils/Lsc_memcpy.h"
#include "Lsc/ResourcePool/Lsc_wrappers.h"

namespace Lsc{
    /**
    *  
    * 用户可以通过资源池托管包括内存、句柄在内的各种资源，还可以建造、克隆各种对象、数组。
    *
    */
    class ResourcePool{
    plclic:
        
        typedef Lsc::pool_allocator<char>   allocator_type;

        /**
        *  
        *  该回调函数以资源指针（如FILE*）/对象地址作为参数，没有返回值
        * ng_wrappers.h有针对常见资源的实现，建议尽量使用ng_wrappers.h的实现
        */
        typedef void( *object_destructor_t )(void * p_object);

        /**
        *  
        *  该回调函数以数组的首址、尾址（＝首址＋数组大小）作为参数，没有返回值。
        */
        typedef void( *array_destructor_t )(void * begin, void * end);

        /**
         *
        **/
        ResourcePool()
            :_syspool(), 
            _mempool(_syspool), 
            _p_attach_object_list(NULL), 
            _p_attach_array_list(NULL), 
            _p_alloc_object_list(NULL), 
            _p_alloc_array_list(NULL){}

        /**
         *
        **/
        explicit ResourcePool( mempool& pool )
            :_mempool(pool), 
            _p_attach_object_list(NULL), 
            _p_attach_array_list(NULL), 
            _p_alloc_object_list(NULL), 
            _p_alloc_array_list(NULL){}

        /**
         *
         * 资源池析构时，会调用destroy_all()方法。
         *
        **/
        ~ResourcePool(){
            reset();
        }

        allocator_type get_allocator() const {
            return allocator_type(&_mempool);
        }

        /**
         *          
         **/

        mempool& get_mempool() const {
            return _mempool;
        }

        /**
         *
         * 清理函数将会在destroy_all()被调用或资源池析构时被自动调用。
         * 
         * 注：
         * 可以的话，尽量使用create_xxx()方法或clone_xxx()方法，以降低出错的可能性
         * 如果已经把资源attach到资源池，无论如何也不要试图自行释放资源！
         * 若destructor == NULL 本函数简单地忽略该请求（既然没事情要干，也没必要做记录了）
         * 若data == NULL   本函数不做特别处理（NULL会被传递给destructor）
         * 如果在创建过程中发生了异常（内存不足，非常罕有但仍有可能），资源会立刻被清理
         *
        **/
        void attach( void * p_object, object_destructor_t destructor ){
            if ( NULL != destructor ){
                try{
                    attach_object_info_t& info = _push_info(_p_attach_object_list); //throw
                    info.p_object   = p_object;
                    info.destructor = destructor;
                }catch(...){
                    destructor( p_object );
                    throw;
                }
            }
        }


        /**
         *
         *
         * 清理函数将会在destroy_all()被调用或资源池析构时被自动调用。
         *
         * 注：
         * 可以的话，尽量使用create_xxx()方法或clone_xxx()方法，以降低出错的可能性
         * 如果已经把资源attach到资源池，无论如何也不要试图自行释放资源！
         * 若destructor == NULL 本函数简单地忽略该请求（既然没事情要干，也没必要做记录了）
         * 若begin == end   本函数不做特别处理（NULL会被传递给destructor）
         * 如果在创建过程中发生了异常（内存不足，非常罕有但仍有可能），资源会立刻被清理
         *
        **/
        void attach_array( void *begin, void *end, array_destructor_t destructor ){ 
            if ( NULL != destructor ){
                try{
                    attach_array_info_t& info = _push_info(_p_attach_array_list); //throw
                    info.begin      = begin;
                    info.end        = end;
                    info.destructor = destructor;
                }catch(...){
                    destructor( begin, end );
                    throw;
                }
            }
        }

        /**
         *
        **/
        template<typename T>
            T& create(){
                return _create_object<T>( DefaultCtor<T>() );
            }

        template<typename T>
            T* createp(){
                try{
                    return &_create_object<T>( DefaultCtor<T>() );
                }catch(...){
                    return NULL;
                }
            }

        /**
         *
         **/
        template<typename T, typename T_arg>
            T& create(const T_arg& arg_){
                return _create_object<T>( OneConstArgCtor<T, T_arg>(arg_) );
            }

        /**
         *
         **/
        template<typename T, typename T_arg>
            T& createn(T_arg& arg_){
                return _create_object<T>( OneArgCtor<T, T_arg>(arg_) );
            }

            /**
             *
             * NOTE：不抛异常可能更符合C程序员的习惯，但
             *
            **/
        template<typename T, typename T_arg>
            T* createp(const T_arg& arg_){
                try{
                    return &_create_object<T>( OneConstArgCtor<T, T_arg>(arg_) );
                }catch(...){
                    return NULL;
                }
            }
        /**
         *
        **/
        template<typename T, typename T_arg1, typename T_arg2>
            T& create( const T_arg1& arg1, const T_arg2& arg2 ){
                return _create_object<T>( TwoConstArgCtor<T,T_arg1,T_arg2>(arg1,arg2) );
            }

        template<typename T, typename T_arg1, typename T_arg2>
            T* createp( const T_arg1& arg1, const T_arg2& arg2 ){
                try{
                    return &_create_object<T>( TwoConstArgCtor<T,T_arg1,T_arg2>(arg1,arg2) );
                }catch(...){
                    return NULL;
                }
            }

        /**
         *
         * 目前构造需要执行两遍构造函数（多参构造与复制构造），将来需求大时可以优化一下
         *
        **/
        template<typename T, typename T_arg1, typename T_arg2, typename T_arg3 >
            T& create( const T_arg1& arg1, const T_arg2& arg2, const T_arg3 arg3 ){
                return _create_object<T>( 
                        ThreeConstArgCtor<T,T_arg1,T_arg2,T_arg3>(arg1, arg2, arg3) 
                        );
            }

        template<typename T, typename T_arg1, typename T_arg2, typename T_arg3 >
            T* createp( const T_arg1& arg1, const T_arg2& arg2, const T_arg3 arg3 ){
                try{
                    return &_create_object<T>( 
                            ThreeConstArgCtor<T,T_arg1,T_arg2,T_arg3>(arg1, arg2, arg3) 
                            );
                }catch(...){
                    return NULL;
                }
            }
        /**
         *
         * 目前构造需要执行两遍构造函数（多参构造与复制构造），将来需求大时可以优化一下
         *
        **/
        template<typename T, typename T_arg1, typename T_arg2, typename T_arg3, typename T_arg4 >
            T& create( 
                    const T_arg1& arg1, 
                    const T_arg2& arg2, 
                    const T_arg3 arg3, 
                    const T_arg4 arg4 
                    ){
                return _create_object<T>( 
                        FourConstArgCtor<T, T_arg1, T_arg2, T_arg3, T_arg4>(
                            arg1, 
                            arg2, 
                            arg3, 
                            arg4
                            ) );
            }

        template<typename T, typename T_arg1, typename T_arg2, typename T_arg3, typename T_arg4 >
            T* createp( 
                    const T_arg1& arg1, 
                    const T_arg2& arg2, 
                    const T_arg3 arg3, 
                    const T_arg4 arg4 
                    ){
                try{
                    return &_create_object<T>( 
                            FourConstArgCtor<T, T_arg1, T_arg2, T_arg3, T_arg4>(
                                arg1, 
                                arg2, 
                                arg3, 
                                arg4
                                ) );
                }catch(...){
                    return NULL;
                }
            }

        /**
         *
        **/
        template<typename T>
            T* create_array( size_t n ){
                T *begin     = NULL;
                //抛异常表示info都分配不了，没什么可回滚的。
                alloc_array_info_t& info = _push_info(_p_alloc_array_list); //throw
                try{
                    begin = static_cast<T*>(_mempool.malloc( n * sizeof(T) )); //throw
                    if ( !begin ){
                        throw Lsc::BadAllocException()
                            <<BSL_EARG<<"_mempool["<<&_mempool<<"] size["<<n * sizeof(T)<<"]";
                    }
                    try{
                        construct_array<T>(begin, begin + n);   //throw (by user code)
                        info.begin      = begin;
                        info.end        = begin + n;
                        info.destructor = _s_destroy_and_deallocate_array<T>;
                        return begin;
                    }catch(...){
                        //回滚为对象数组分配的内存
                        _mempool.free( begin, n * sizeof(T) );
                        throw;
                    }
                }catch(...){
                    //回滚info的分配
                    _pop_info(_p_alloc_array_list);
                    throw;
                }
            }

        /**
         *
         **/
        template<typename T, typename T_arg>
            T* create_array( size_t n, const T_arg& __arg ){
                T *begin     = NULL;
                //抛异常表示info都分配不了，没什么可回滚的。
                alloc_array_info_t& info = _push_info(_p_alloc_array_list); //throw
                try{
                    begin = static_cast<T*>(_mempool.malloc( n * sizeof(T) ));   //throw
                    if ( !begin ){
                        throw Lsc::BadAllocException()
                            <<BSL_EARG<<"_mempool["<<&_mempool<<"] size["<<n * sizeof(T)<<"]";
                    }
                    try{
                        construct_array<T, T_arg>(begin, begin + n, &__arg);   //throw (by user code)
                        info.begin      = begin;
                        info.end        = begin + n;
                        info.destructor = _s_destroy_and_deallocate_array<T>;
                        return begin;
                    }catch(...){
                        //回滚为对象数组分配的内存
                        _mempool.free( begin, n * sizeof(T) );
                        throw;
                    }
                }catch(...){
                    //回滚info的分配
                    _pop_info(_p_alloc_array_list);
                    throw;
                }
            }

        /**
         *
        **/
        void * create_raw( size_t bytes ){ 
            alloc_array_info_t& info = _push_info(_p_alloc_array_list); //throw
            try{
                char * res = static_cast<char*>(_mempool.malloc( bytes )); //throw
                if ( !res ){
                    throw Lsc::BadAllocException()
                        <<BSL_EARG<<"_mempool["<<&_mempool<<"] size["<<bytes<<"]";
                }
                info.begin      = res;
                info.end        = res+bytes;
                info.destructor = _s_deallocate;
                return res;
            }catch(...){
                //若内存分配失败，回滚info的分配
                _pop_info(_p_alloc_array_list);
                throw;
            }
        }

        /**
         *
        **/
        template<typename T>
            T& clone(const T& src){
                return create<T,T>(src);
            }

        /**
         *
         * 注：如果谁敢复制一个void数组还问我为什么编译不过的话，我一定会打他的PP
         *
        **/
        template<typename T>
            T* clone_array(const T* src, size_t n){
                T *begin     = NULL;
                //抛异常表示info都分配不了，没什么可回滚的。
                alloc_array_info_t& info = _push_info(_p_alloc_array_list); //throw
                try{
                    begin = static_cast<T*>(_mempool.malloc(n*sizeof(T)));  //throw
                    if ( !begin ){
                        throw Lsc::BadAllocException()
                            <<BSL_EARG<<"_mempool["<<&_mempool<<"] size["<<n * sizeof(T)<<"]";
                    }
                    try{
                        std::uninitialized_copy( src, src + n, begin );   //throw (by user code )
                        info.begin      = begin;
                        info.end        = begin + n;
                        info.destructor = _s_destroy_and_deallocate_array<T>;
                        return begin;
                    }catch(...){
                        //回滚为对象数组分配的内存
                        _mempool.free(begin, n*sizeof(T));
                        throw;
                    }
                }catch(...){
                    //回滚info的分配
                    _pop_info(_p_alloc_array_list);
                    throw;
                }
            }

        /**
         *
        **/
        void * clone_raw( const void * src, size_t bytes ){
            void * res = create_raw( bytes );
            return xmemcpy( res, src, bytes );
        }

        /**
         *
         **/
        char * clone_cstring(const char * src_str ){
            size_t size = strlen(src_str) + 1;
            void * res  = clone_raw( src_str, size ); //throw
            return static_cast<char *>(res);
        }

        /**
         *
         * 注意：若slc_str_len > strlen(src_str)，本方法行为未定义。
         *
        **/
        char * clone_cstring(const char * src_str, size_t slc_str_len ){
            char * res = static_cast<char*>(clone_raw( src_str, slc_str_len + 1 )); //throw
            res[slc_str_len] = 0;
            return res;
        }

        /**
         *
         * 在目前的实现里，crcprintf(...)相当于crcprintf_hint( 63, ... )
         *
        **/
        const char * crcprintf( const char * format, ... )__attribute__ ((format (printf, 2, 3) ));

        /**
         *
        **/
        const char * crcprintf_hint( 
                size_t hint_capacity, 
                const char * format, 
                ... 
                )__attribute__ ((format (printf, 3, 4) ));

        /**
         *          
         **/
        const char * vcrcprintf( const char * format, va_list ap );

        /**
         *          
         **/
        const char * vcrcprintf_hint( size_t hint_capacity, const char * format, va_list ap );



        /**
         *
        **/
        void reset();

    private:
        static const size_t _S_ITEMS_PER_NODE = 64;

        /**
        *  
        */
        typedef void( *alloc_object_destructor_t )(void * p_object, mempool& pool );

        /**
        *  
        *  该回调函数以数组的首址、尾址（＝首址＋数组大小）作为参数，没有返回值。
        */
        typedef void( *alloc_array_destructor_t )(void * begin, void * end, mempool& pool );

        /**
        *  
        *  
        */
        struct attach_object_info_t{
            void *p_object;
            object_destructor_t destructor;
        };

        /**
        *  
        *  
        */
        struct attach_array_info_t{
            void *begin;
            void *end;
            array_destructor_t destructor;
        };

        /**
        *  
        *  
        */
        struct alloc_object_info_t{
            void *p_object;
            alloc_object_destructor_t destructor;
        };

        /**
        *  
        *  
        */
        struct alloc_array_info_t{
            void *begin;
            void *end;
            alloc_array_destructor_t destructor;
        };

        template<typename info_t>
        struct block_list_node_t{
            block_list_node_t<info_t>  *p_next;
            size_t  current;
            info_t  data[_S_ITEMS_PER_NODE];
        };

        /**
         *
         */
        //构造函数仿函数，相当于lambda表达式
        template<typename T>
            class DefaultCtor{
            plclic:
                void operator ()( T* ptr ) const {
                    new(ptr) T();
                }
            };

        /**
         *
         */
        template<typename T, typename ArgT>
            class OneArgCtor{
            plclic:
                OneArgCtor( ArgT& arg )
                    :_arg(arg){}
                void operator ()( T* ptr ) const {
                    new(ptr) T(_arg);
                }
            private:
                ArgT& _arg;
            };

        /**
         *
         */
        template<typename T, typename ArgT>
            class OneConstArgCtor{
            plclic:
                OneConstArgCtor( const ArgT& arg )
                    :_arg(arg) {}
                void operator ()( T* ptr ) const {
                    new(ptr) T(_arg);
                }
            private:
                const ArgT& _arg;
            };

        /**
         *
         */
        template<typename T, typename Arg1T, typename Arg2T>
            class TwoConstArgCtor{
            plclic:
                TwoConstArgCtor( const Arg1T& arg1, const Arg2T& arg2 )
                    :_arg1(arg1), _arg2(arg2) {}
                void operator ()( T* ptr ) const {
                    new(ptr) T(_arg1, _arg2);
                }
            private:
                const Arg1T& _arg1;
                const Arg2T& _arg2;
            };
        /**
         *
         */
        template<typename T, typename Arg1T, typename Arg2T, typename Arg3T>
            class ThreeConstArgCtor{
            plclic:
                ThreeConstArgCtor( const Arg1T& arg1, const Arg2T& arg2, const Arg3T& arg3 )
                    :_arg1(arg1), _arg2(arg2), _arg3(arg3) {}
                void operator ()( T* ptr ) const {
                    new(ptr) T(_arg1, _arg2, _arg3);
                }
            private:
                const Arg1T& _arg1;
                const Arg2T& _arg2;
                const Arg3T& _arg3;
            };
        /**
         *
         */
        template<typename T, typename Arg1T, typename Arg2T, typename Arg3T, typename Arg4T>
            class FourConstArgCtor{
            plclic:
                FourConstArgCtor( 
                        const Arg1T& arg1, 
                        const Arg2T& arg2, 
                        const Arg3T& arg3, 
                        const Arg4T& arg4 
                        )
                    :_arg1(arg1), _arg2(arg2), _arg3(arg3), _arg4(arg4) {}
                void operator ()( T* ptr ) const {
                    new(ptr) T(_arg1, _arg2, _arg3, _arg4);
                }
            private:
                const Arg1T& _arg1;
                const Arg2T& _arg2;
                const Arg3T& _arg3;
                const Arg4T& _arg4;
            };

    plclic:
        template<typename T, typename CtorT>
            T& _create_object( const CtorT& ctor ){
                T *p_object = NULL;
                //抛异常表示info都分配不了，没什么可回滚的。
                alloc_object_info_t& info = _push_info(_p_alloc_object_list); //throw
                //抛异常表示对象空间分配不了，回滚info的分配。
                try{
                    p_object = static_cast<T*>(_mempool.malloc(sizeof(T)));
                    if ( !p_object ){
                        throw Lsc::BadAllocException()
                            <<BSL_EARG<<"_mempool["<<&_mempool<<"] size["<<sizeof(T)<<"]";
                    }
                    try{
                        ctor(p_object); //throw (by user code)
                        info.p_object   = p_object;
                        info.destructor = _s_destroy_and_deallocate<T>;
                        return *p_object;
                    }catch(...){
                        //回滚对象空间的分配
                        _mempool.free( p_object, sizeof(T) );
                        throw;
                    }
                }catch(...){
                    //回滚对象空间的分配
                    _pop_info(_p_alloc_object_list);
                    throw;
                }
            }

    private:
        template<typename T, typename ArrayCtorT>
            T* _create_array( size_t n, const ArrayCtorT& ctor ){
                T *begin     = NULL;
                //抛异常表示info都分配不了，没什么可回滚的。
                alloc_array_info_t& info = _push_info(_p_alloc_array_list); //throw
                try{
                    begin = static_cast<T*>(_mempool.malloc( n * sizeof(T) )); //throw
                    if ( !begin ){
                        throw Lsc::BadAllocException()
                            <<BSL_EARG<<"_mempool["<<&_mempool<<"] size["<<n * sizeof(T)<<"]";
                    }
                    try{
                        //construct_array<T>(begin, begin + n);   
                        ctor(begin, begin+n);   //throw (by user code)
                        info.begin      = begin;
                        info.end        = begin + n;
                        info.destructor = _s_destroy_and_deallocate_array<T>;
                        return begin;
                    }catch(...){
                        //回滚为对象数组分配的内存
                        _mempool.free( begin, n * sizeof(T) );
                        throw;
                    }
                }catch(...){
                    //回滚info的分配
                    _pop_info(_p_alloc_array_list);
                    throw;
                }
            }
        /**
         *
         * 插入成功后，返回可以写入的新项。
         * 如果块链中的一个块被写满，该函数会申请一个新块。调用之后_p_list_head会指向新块。
         * 若发生内存不足，会抛出mempool指定的异常。
         * 该函数要求info_t不需要构造（C风格struct）
         *
        **/
        template<typename info_t> 
        info_t& _push_info( block_list_node_t<info_t> *& p_list_head ){
            if ( NULL!=p_list_head && p_list_head->current < _S_ITEMS_PER_NODE - 1 ){
                ++ p_list_head->current; //如果块链未满，只增加计数器
                return p_list_head->data[p_list_head->current];   
            }else{
                typedef block_list_node_t<info_t> node_t;
                node_t* p_tmp = static_cast<node_t *>(_mempool.malloc(sizeof(node_t))); //throw
                if ( !p_tmp ){
                    throw Lsc::BadAllocException()
                        <<BSL_EARG<<"_mempool["<<&_mempool<<"] size["<<sizeof(node_t)<<"]";
                }
                p_tmp->p_next = p_list_head;
                p_tmp->current= 0;
                p_list_head = p_tmp;
                return p_list_head->data[0];
            }
        }

        /**
         *
        **/
        template<typename info_t> 
        void _pop_info( block_list_node_t<info_t> *& p_list_head ){
            if ( p_list_head->current > 0 ){
                -- p_list_head->current; //如果块链不为空，只减少计数器
            }else{
                block_list_node_t<info_t>* p_tmp = p_list_head;
                p_list_head = p_list_head->p_next;
                _mempool.free(p_tmp, sizeof(block_list_node_t<info_t>));
            }
        }

        /**
         * 函数执行后，p_list_head会被置为NULL。
         * 该函数具有无抛掷保证。
         *
        **/
        template<typename info_t> 
        void _clear_info_list(block_list_node_t<info_t> *& p_list_head ){
            while( NULL != p_list_head ){
                block_list_node_t<info_t> *p_tmp = p_list_head;
                p_list_head = p_list_head->p_next;
                _mempool.free(p_tmp, sizeof(block_list_node_t<info_t>));
            }
        }
        
        /**
         *
         * 用户不应以任何方式创建资源池对象的副本。
         *
         **/
        ResourcePool( const ResourcePool& other); 

        /**
         *
         * 用户不应该以任何方式对ResourcePool赋值
         *
        **/
        ResourcePool& operator = ( const ResourcePool& );

        /**
         *
        **/
        const char * _vprintf( 
                alloc_array_info_t& info, 
                size_t hint_capacity, 
                const char *format, 
                va_list ap 
                );
        /**
         **/
        static void _s_deallocate( void * begin, void * end, mempool& pool ){
            pool.free( begin, static_cast<char *>(end) - static_cast<char *>(begin) );
        }

        /**
         *
         **/
        template<typename T>
            static void _s_destroy_and_deallocate( void * p_object, mempool& pool ){
                static_cast<T*>(p_object)->~T();
                pool.free(p_object, sizeof(T));
            }

        /**
         *
         **/
        template<typename T>
            static void _s_destroy_and_deallocate_array( void * begin, void * end, mempool& pool ){
                __BSL_DESTROY(static_cast<T*>(begin), static_cast<T*>(end)); // this method will be specially optimized to the type which has trivial destructor;
                pool.free(begin, static_cast<char*>(end) - static_cast<char*>(begin) );
            }

        /**
        *  
        * 默认构造的ResourcePool 使用该Pool
        */
        syspool     _syspool;

        /**
        *  
        */
        mempool&    _mempool;
        /**
        *  
        *  
        */
        block_list_node_t<attach_object_info_t>    *_p_attach_object_list;
        /**
        *  
        *  
        */
        block_list_node_t<attach_array_info_t>     *_p_attach_array_list;
        /**
        *  
        *  
        */
        block_list_node_t<alloc_object_info_t>     *_p_alloc_object_list;
        /**
        *  
        *  
        */
        block_list_node_t<alloc_array_info_t>      *_p_alloc_array_list;


    };

} // namespace Lsc
#endif  //__RESOURCEPOOL_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
