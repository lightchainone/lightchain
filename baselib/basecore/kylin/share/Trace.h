
#ifndef _TRACE_H_
#define _TRACE_H_

#ifndef DISABLE_TRACE
#   ifndef __TRACE__
#       define __TRACE__        7
#   endif
#else
#   undef   __TRACE__
#   define  __TRACE__           0
#endif

#ifndef __TRACE__
#   define  TRACE0(...)
#   define  TRACE1(...)
#   define  TRACE2(...)
#   define  TRACE3(...)
#   define  TRACE4(...)
#   define  TRACE5(...)
#   define  TRACE6(...)
#   define  TRACE7(...)
#elif   __TRACE__ >= 7
#   define  TRACE0(...)      { int save=GetLastError(); g_fTrace(__VA_ARGS__); SetLastError(save); }
#   define  TRACE1(...)      if (g_nKLLogLevel>=1) { int save=GetLastError(); g_fTrace(__VA_ARGS__); SetLastError(save); }
#   define  TRACE2(...)      if (g_nKLLogLevel>=2) { int save=GetLastError(); g_fTrace(__VA_ARGS__); SetLastError(save); }
#   define  TRACE3(...)      if (g_nKLLogLevel>=3) { int save=GetLastError(); g_fTrace(__VA_ARGS__); SetLastError(save); }
#   define  TRACE4(...)      if (g_nKLLogLevel>=4) { int save=GetLastError(); g_fTrace(__VA_ARGS__); SetLastError(save); }
#   define  TRACE5(...)      if (g_nKLLogLevel>=5) { int save=GetLastError(); g_fTrace(__VA_ARGS__); SetLastError(save); }
#   define  TRACE6(...)      if (g_nKLLogLevel>=6) { int save=GetLastError(); g_fTrace(__VA_ARGS__); SetLastError(save); }
#   define  TRACE7(...)      if (g_nKLLogLevel>=7) { int save=GetLastError(); g_fTrace(__VA_ARGS__); SetLastError(save); }
#elif   __TRACE__ >= 6
#   define  TRACE0(...)      { int save=GetLastError(); g_fTrace(__VA_ARGS__); SetLastError(save); }
#   define  TRACE1(...)      if (g_nKLLogLevel>=1) { int save=GetLastError(); g_fTrace(__VA_ARGS__); SetLastError(save); }
#   define  TRACE2(...)      if (g_nKLLogLevel>=2) { int save=GetLastError(); g_fTrace(__VA_ARGS__); SetLastError(save); }
#   define  TRACE3(...)      if (g_nKLLogLevel>=3) { int save=GetLastError(); g_fTrace(__VA_ARGS__); SetLastError(save); }
#   define  TRACE4(...)      if (g_nKLLogLevel>=4) { int save=GetLastError(); g_fTrace(__VA_ARGS__); SetLastError(save); }
#   define  TRACE5(...)      if (g_nKLLogLevel>=5) { int save=GetLastError(); g_fTrace(__VA_ARGS__); SetLastError(save); }
#   define  TRACE6(...)      if (g_nKLLogLevel>=6) { int save=GetLastError(); g_fTrace(__VA_ARGS__); SetLastError(save); }
#   define  TRACE7(...)
#elif   __TRACE__ >= 5
#   define  TRACE0(...)      { int save=GetLastError(); g_fTrace(__VA_ARGS__); SetLastError(save); }
#   define  TRACE1(...)      if (g_nKLLogLevel>=1) { int save=GetLastError(); g_fTrace(__VA_ARGS__); SetLastError(save); }
#   define  TRACE2(...)      if (g_nKLLogLevel>=2) { int save=GetLastError(); g_fTrace(__VA_ARGS__); SetLastError(save); }
#   define  TRACE3(...)      if (g_nKLLogLevel>=3) { int save=GetLastError(); g_fTrace(__VA_ARGS__); SetLastError(save); }
#   define  TRACE4(...)      if (g_nKLLogLevel>=4) { int save=GetLastError(); g_fTrace(__VA_ARGS__); SetLastError(save); }
#   define  TRACE5(...)      if (g_nKLLogLevel>=5) { int save=GetLastError(); g_fTrace(__VA_ARGS__); SetLastError(save); }
#   define  TRACE6(...)
#   define  TRACE7(...)
#elif   __TRACE__ >= 4
#   define  TRACE0(...)      { int save=GetLastError(); g_fTrace(__VA_ARGS__); SetLastError(save); }
#   define  TRACE1(...)      if (g_nKLLogLevel>=1) { int save=GetLastError(); g_fTrace(__VA_ARGS__); SetLastError(save); }
#   define  TRACE2(...)      if (g_nKLLogLevel>=2) { int save=GetLastError(); g_fTrace(__VA_ARGS__); SetLastError(save); }
#   define  TRACE3(...)      if (g_nKLLogLevel>=3) { int save=GetLastError(); g_fTrace(__VA_ARGS__); SetLastError(save); }
#   define  TRACE4(...)      if (g_nKLLogLevel>=4) { int save=GetLastError(); g_fTrace(__VA_ARGS__); SetLastError(save); }
#   define  TRACE5(...)
#   define  TRACE6(...)
#   define  TRACE7(...)
#elif   __TRACE__ >= 3
#   define  TRACE0(...)      { int save=GetLastError(); g_fTrace(__VA_ARGS__); SetLastError(save); }
#   define  TRACE1(...)      if (g_nKLLogLevel>=1) { int save=GetLastError(); g_fTrace(__VA_ARGS__); SetLastError(save); }
#   define  TRACE2(...)      if (g_nKLLogLevel>=2) { int save=GetLastError(); g_fTrace(__VA_ARGS__); SetLastError(save); }
#   define  TRACE3(...)      if (g_nKLLogLevel>=3) { int save=GetLastError(); g_fTrace(__VA_ARGS__); SetLastError(save); }
#   define  TRACE4(...)
#   define  TRACE5(...)
#   define  TRACE6(...)
#   define  TRACE7(...)
#elif   __TRACE__ >= 2
#   define  TRACE0(...)      { int save=GetLastError(); g_fTrace(__VA_ARGS__); SetLastError(save); }
#   define  TRACE1(...)      if (g_nKLLogLevel>=1) { int save=GetLastError(); g_fTrace(__VA_ARGS__); SetLastError(save); }
#   define  TRACE2(...)      if (g_nKLLogLevel>=2) { int save=GetLastError(); g_fTrace(__VA_ARGS__); SetLastError(save); }
#   define  TRACE3(...)
#   define  TRACE4(...)
#   define  TRACE5(...)
#   define  TRACE6(...)
#   define  TRACE7(...)
#elif   __TRACE__ >= 1
#   define  TRACE0(...)     { int save=GetLastError(); g_fTrace(__VA_ARGS__); SetLastError(save); }
#   define  TRACE1(...)     if (g_nKLLogLevel>=1) { int save=GetLastError(); g_fTrace(__VA_ARGS__); SetLastError(save); }
#   define  TRACE2(...)
#   define  TRACE3(...)
#   define  TRACE4(...)
#   define  TRACE5(...)
#   define  TRACE6(...)
#   define  TRACE7(...)
#elif   __TRACE__ == 0
#   define  TRACE0(...)     { int save=GetLastError(); g_fTrace(__VA_ARGS__); SetLastError(save); }
#   define  TRACE1(...)
#   define  TRACE2(...)
#   define  TRACE3(...)
#   define  TRACE4(...)
#   define  TRACE5(...)
#   define  TRACE6(...)
#   define  TRACE7(...)
#endif

#ifdef ASSERT
#   undef ASSERT
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*TRACE_FUNC)(const char *fmt, ...);
extern int g_nKLLogLevel;
extern TRACE_FUNC g_fTrace;
extern void KLSetLog(FILE* fpLog, int nLogLevel, TRACE_FUNC f);

#ifdef __cplusplus
}
#endif

#if defined(_UNITTEST)

#include "UnitTest.h"
static inline void DebugBreak(const char* msg)
{
    using namespace unittest;
    Exception* e = new Exception(msg);
    Environment::Instance()->AddException(e);
    throw e;
}

#else

#if 1
#define DebugBreak(c)    abort()
#else
static inline void DebugBreak(const char*)
{
    for (int i=0; i<30; i++)
        sleep(1);
    TRACE0("Debug Break\n");
}
#endif

#endif

#if (__GNUC__ >= 3)
#   include <ostream>
#else
#   include <ostream.h>
#endif
#include <sstream>
#include <iomanip>

#if defined(_DEBUG)

#define ASSERT(c)   if (!(c)) {                                                         \
        std::ostringstream os;                                                          \
        TRACE0(os.str().c_str());                                                       \
        DebugBreak(os.str().c_str());                                                   \
    }

#define ASSERT2(x, op, y)   if (!((x) op (y))) {                                        \
        std::ostringstream os;                                                          \
        os<<"   "<<std::setw(40)<<#x" = "<<x<<std::endl;                                \
        os<<"   "<<std::setw(40)<<#y" = "<<y<<std::endl;                                \
        TRACE0(os.str().c_str());                                                       \
        DebugBreak(os.str().c_str());                                                   \
    }

#define ASSERT_AFTER_UNLOCK(c, l) if (!(c)) {                                           \
        l.Unlock();                                                                     \
        std::ostringstream os;                                                          \
        TRACE0(os.str().c_str());                                                       \
        DebugBreak(os.str().c_str());                                                   \
    }

#define VERIFY_OR_DO(expr, do_something)                                                \
    ASSERT(expr)

#else

#define ASSERT2(x, op, y)
#define ASSERT(c)
#define ASSERT_AFTER_UNLOCK(l, c)

#define VERIFY_OR_DO(expr, do_something)                                                \
    if (!(expr)) {                                                                      \
        do_something;                                                                   \
    }

#endif

#define ASSERT_EQUAL(x, y)          ASSERT2(x, ==, y)
#define ASSERT_NOT_EQUAL(x, y)      ASSERT2(x, !=, y)
#define ASSERT_GREATER(x, y)        ASSERT2(x, >, y)
#define ASSERT_LESS(x, y)           ASSERT2(x, <, y)
#define ASSERT_NOT_GREATER(x, y)    ASSERT2(x, <=, y)
#define ASSERT_NOT_LESS(x, y)       ASSERT2(x, >=, y)

#define BLANK

#define VERIFY_OR_RETURN(expr, ret_if_fail)                                             \
    VERIFY_OR_DO(expr, return ret_if_fail)

#define VERIFY_OR_ABORT(expr)                                                           \
    VERIFY_OR_DO(expr, DebugBreak(#expr))

#define FAIL(c) {                                                                       \
        std::ostringstream os;                                                          \
        TRACE0(os.str().c_str());                                                       \
        DebugBreak(os.str().c_str());                                                   \
    }

#define RaiseError(c)           FAIL(#c)

#define PERROR(str) {                                                                   \
        TRACE0("%s: %s\n", str, strerror(errno));                                       \
    }



template <typename T>
static inline void AssertEqual(T& t1, T& t2) {
    ASSERT_EQUAL(t1, t2);
}

template <typename T>
static inline void AssertEqualSet(const T& bs1, const T& bs2) {
    typename T::iterator it1, it2;
    ASSERT_EQUAL(bs1.size(), bs2.size());
    for (it1=bs1.begin(), it2=bs2.begin(); it1!=bs1.end(); it1++, it2++) {
        AssertEqual(*it1, *it2);
    }
}

template <typename T>
static inline void AssertEqualMap(const T& bs1, const T& bs2) {
    typename T::iterator it1, it2;
    ASSERT_EQUAL(bs1.size(), bs2.size());
    for (it1=bs1.begin(), it2=bs2.begin(); it1!=bs1.end(); it1++, it2++) {
        AssertEqual(it1->first, it2->first);
        AssertEqual(it1->second, it2->second);
    }
}

#endif
