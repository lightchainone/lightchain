#ifndef __TYPELIST__
#define __TYPELIST__

struct NullType;
template <typename T0 = NullType, typename T1 = NullType, typename T2 = NullType,
         typename T3 = NullType, typename T4 = NullType, typename T5 = NullType>
struct typelist {
    typedef T0 Head;
    typedef typelist<T1, T2, T3, T4, T5> Tail;
    enum { length = 1 + Tail::length };
};
template <>
struct typelist<NullType, NullType, NullType, NullType, NullType, NullType> {
    enum { length = 0 };
};
template <typename TL, int n, bool OutOfRange = (n >= TL::length)>
struct type_at {
    typedef typename type_at<typename TL::Tail, n-1>::type type;
};
template <typename TL>
struct type_at<TL, 0, false> {
    typedef typename TL::Head type;
};
template <typename TL, int n>
struct type_at<TL, n, true> {
};
template <typename T, typename U>
struct is_same_type {
    enum { value = false };
};
template <typename T>
struct is_same_type<T, T> {
    enum { value = true };
};
template <typename TL, typename T, int count = -1, bool equalsHead =
is_same_type<typename TL::Head, T>::value>
struct index_of {
    enum { value = 1 + index_of<typename TL::Tail, T, count-1>::value };
};
template <typename TL, typename T, int count>
struct index_of<TL, T, count, true> {
    enum { value = 0 };
};
template <typename T, typename U, int count>
struct index_of<typelist<T>, U, count, false> {
    enum { value = count };
};
template <typename T, typename U, bool greaterThan = (sizeof(T) >= sizeof(U))>
struct type_selector {
    typedef T type;
};
template <typename T, typename U>
struct type_selector<T, U, false> {
    typedef U type;
};
template <typename TL>
struct largest_type {
    typedef typename type_selector<typename TL::Head, typename largest_type<typename
    TL::Tail>::type>::type type;
};
template <typename T>
struct largest_type<typelist<T> > {
    typedef T type;
};

#endif

