#ifndef _BSL_CONSTRUCT_H
#define _BSL_CONSTRUCT_H

#include <algorithm>
#include <new>

#if __GNUC__ >= 4
#include <Lsc/utils/Lsc_type_traits.h>
#endif

namespace Lsc
{
  /**
   * Constructs an object in existing memory by invoking an allocated
   * object's constructor with an initializer.
   */
  template<typename _T1, typename _T2>
    inline void
    Lsc_construct(_T1* __p, const _T2& __value)
    {
      // _GLIBCXX_RESOLVE_LIB_DEFECTS
      // 402. wrong new expression in [some_]allocator::construct
      ::new(static_cast<void*>(__p)) _T1(__value);
    }

  /**
   * Constructs an object in existing memory by invoking an allocated
   * object's default constructor (no initializers).
   */
  template<typename _T1>
    inline void
    Lsc_construct(_T1* __p)
    {
      // _GLIBCXX_RESOLVE_LIB_DEFECTS
      // 402. wrong new expression in [some_]allocator::construct
      ::new(static_cast<void*>(__p)) _T1();
    }

  /**
   * Destroy the object pointed to by a pointer type.
   */
  template<typename _Tp>
    inline void
    Lsc_destruct(_Tp* __pointer)
    { __pointer->~_Tp(); }

  /**
   * Destroy a range of objects with nontrivial destructors.
   *
   * This is a helper function used only by destruct().
   */
  template<typename _ForwardIterator>
    inline void
    __Lsc_destroy_aux(_ForwardIterator __first, _ForwardIterator __last,
		  __false_type)
    { for ( ; __first != __last; ++__first) Lsc::Lsc_destruct(&*__first); }

  /**
   * Destroy a range of objects with trivial destructors.  Since the destructors
   * are trivial, there's nothing to do and hopefully this function will be
   * entirely optimized away.
   *
   * This is a helper function used only by destruct().
   */
  template<typename _ForwardIterator>
    inline void
    __Lsc_destroy_aux(_ForwardIterator, _ForwardIterator, __true_type)
    { }

  /**
   * Destroy a range of objects.  If the value_type of the object has
   * a trivial destructor, the compiler should optimize all of this
   * away, otherwise the objects' destructors must be invoked.
   */
  template<typename _ForwardIterator>
    inline void
    Lsc_destruct(_ForwardIterator __first, _ForwardIterator __last)
    {
      typedef typename std::iterator_traits<_ForwardIterator>::value_type _Value_type;
      typedef typename __type_traits < _Value_type >::has_trivial_destructor
                       _Has_trivial_destructor;
      Lsc::__Lsc_destroy_aux(__first, __last, _Has_trivial_destructor());
    }
} // namespace Lsc

#endif /* _BSL_CONSTRUCT_H */

