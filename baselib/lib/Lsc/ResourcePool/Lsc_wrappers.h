
#ifndef  __BSL_WRAPPERS_H_
#define  __BSL_WRAPPERS_H_

#include <memory>
#include <iostream>
#include <cstdio>
#include "Lsc/ResourcePool/Lsc_wrappers_config.h"

/**
 *  
 *  定义这个宏可以模仿malloc()和placement operator new的行为,并提高分配POD数组的速度
 *  不定义这个宏可以模仿calloc(), 普通operator new，STL容器的行为。
 */
//#define BSL_CONFIG_SKIP_POD_INITIALIAZION

/**
 *  
 *  定义这个宏可以模仿operator new的行为
 *  不定义这个宏可以模仿STL容器的行为
 */
//#define BSL_CONFIG_DO_NOT_COPY_FOR_DEFAULT_CONSTRUCTION

namespace Lsc{
    /**
     *
     * 注：内部使用，用户不应直接调用该函数。
     *
     **/
    template<typename T>
        inline void __construct_aux(void * p_object, __BSL_TRUE_TYPE__){
#ifndef BSL_CONFIG_SKIP_POD_INITIALIAZION
            *static_cast<T*>(p_object) = 0;
#endif
        }

    /**
     *
     * 注：内部使用，用户不应直接调用该函数。
     *
     **/
    template<typename T>
        inline void __construct_aux(void * p_object, __BSL_FALSE_TYPE__){
            ::new(p_object) T();    
        }

    /**
     *  
     *  
     */
    template<typename T>
        inline void construct(void * p_object){
            typedef __BSL_HAS_TRIVIAL_DEFAULT_CONSTRUCTOR(T) __is_POD;
            __construct_aux<T>(p_object, __is_POD() );
        }
    /**
     *  
     *  
     */
    template<typename T, typename T_arg>
        inline void construct(void * p_object, const void * p_arg ){
            new(p_object) T(*static_cast<const T_arg*>(p_arg));
        }

    /**
     *
     **/
    template<typename T>
        inline void destroy(void * p_object){
            static_cast<T*>(p_object)->~T();
        }

    /**
     *
     * 注：内部使用，用户不应直接调用该函数。
     **/
    template<typename T>
        inline void __construct_array_aux( void * begin, void * end, __BSL_FALSE_TYPE__){
#ifndef BSL_CONFIG_DO_NOT_COPY_FOR_DEFAULT_CONSTRUCTION
            std::uninitialized_fill( static_cast<T*>(begin), static_cast<T*>(end), T() ); 
#else
            ::new(begin) T[static_cast<T*>(end) - static_cast<T*>(begin)];
#endif 
        }

    /**
     *
     * 注：内部使用，用户不应直接调用该函数。
     **/
    template<typename T>
        inline void __construct_array_aux( void * begin, void * end, __BSL_TRUE_TYPE__){
#ifndef BSL_CONFIG_SKIP_POD_INITIALIAZION
            memset(begin, 0, static_cast<char*>(end) - static_cast<char*>(begin) );
#endif
        }

    /**
     *
     **/
    template<typename T>
        inline void construct_array(void * begin, void * end ){
            typedef __BSL_HAS_TRIVIAL_DEFAULT_CONSTRUCTOR(T) __is_POD;
            __construct_array_aux<T>( begin, end, __is_POD() );
        }

    /**
     *
     **/
    template<typename T, typename T_arg >
        inline void construct_array(void * begin, void * end, const void * src ){
            std::uninitialized_fill( static_cast<T *>(begin), static_cast<T *>(end), *static_cast<const T_arg *>(src) ); // this method will be specially optimized to POD;
        }

    /**
     *
     **/
    template<typename T>
        inline void default_destroy_array(T* begin, T* end){
            while( begin != end ){
                (begin ++) -> ~T();
            }
        }

    /**
     *
     **/
    template<typename T>
        inline void destroy_array( void * begin, void * end ){
            __BSL_DESTROY(static_cast<T*>(begin), static_cast<T*>(end)); // this method will be specially optimized to the type which has trivial destructor;
        }

    /**
     * 若该段内存不可由Alloc类对象释放或（ end - begin ) ％ sizeof(Alloc::value_type) != 0，则结果未定义
     **/
    template<typename Alloc >
        inline void deallocate( void * begin, void * end ){
            typedef typename Alloc::value_type value_t;
            value_t * vt_begin = static_cast<value_t *>(begin);
            value_t * vt_end   = static_cast<value_t *>(end);
            Alloc().deallocate( vt_begin, vt_end - vt_begin );
        }

    /**
     *
     **/
    template<typename T, typename Alloc>
        inline void destroy_and_deallocate( void * p_object ){
            static_cast<T*>(p_object)->~T();
            typedef typename Alloc::template rebind<T>::other T_Alloc;
            T_Alloc().deallocate(static_cast<T *>(p_object), 1);
        }

    /**
     *
     **/
    template<typename T, typename Alloc>
        inline void destroy_and_deallocate_array( void * begin, void * end ){
            __BSL_DESTROY(static_cast<T*>(begin), static_cast<T*>(end)); // this method will be specially optimized to the type which has trivial destructor;
            typedef typename Alloc::template rebind<T>::other T_Alloc;
            T_Alloc().deallocate(static_cast<T *>(begin), static_cast<T*>(end) - static_cast<T*>(begin) );
        }

    /**
     *
     **/
    template<typename T>
        inline void Lsc_delete(void * object ){
            delete static_cast<T*>(object);
        }

    /**
     *
     **/
    template<typename T>
        inline void Lsc_delete_array( void * array ){
            delete[] static_cast<T*>(array);
        }

    /**
     *
     * 在BSL中，建议使用该函数，而非系统函数free(void*)
     *
     **/
    inline void Lsc_free( void *data){
#ifdef BSL_DEBUG_FLAG
        // 输出调试信息
        std::cerr<<"memory "<<data<<" freeed"<<std::endl;
#endif
        free(data);
    }

    /**
     *
     * 在BSL中，建议使用该函数，而非系统函数fclose(FILE*)
     **/
    inline void Lsc_fclose( void *data){
#ifdef BSL_DEBUG_FLAG
        // 输出调试信息
        std::cerr<<"file "<<data<<" closed"<<std::endl;
#endif
        fclose(static_cast<FILE*>(data));
    }

}   //namespace Lsc
#endif  //__BSL_WRAPPERS_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
