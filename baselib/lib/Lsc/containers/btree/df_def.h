
#ifndef __DF_DEF_H__
#define __DF_DEF_H__

// Enable macros definition in stdint.h
#ifndef __STDC_LIMIT_MACROS
# define __STDC_LIMIT_MACROS 1
# undef _STDINT_H
#endif

#include <stdint.h>
//#include "ul_def.h"

// Definition of common types
typedef signed char char8;
typedef unsigned char u_char8;

const uint64_t  MAX_U_INT64 = UINT64_MAX;
const int64_t   MAX_INT64   = INT64_MAX;
const int64_t   MIN_INT64   = INT64_MIN;
const uint32_t  MAX_U_INT32 = UINT32_MAX;
const int32_t   MAX_INT32   = INT32_MAX;
const int32_t   MIN_INT32   = INT32_MIN;
const uint16_t  MAX_U_INT16 = UINT16_MAX;
const int16_t   MAX_INT16   = INT16_MAX;
const int16_t   MIN_INT16   = INT16_MIN;
const uint8_t   MAX_U_INT8  = UINT8_MAX;
const int8_t    MAX_INT8    = INT8_MAX;
const int8_t    MIN_INT8    = INT8_MIN;

#define IXINTUI_INLINE __attribute__((__always_inline__))
#define IXINTUI_UNLIKELY(exp) (exp)
#define IXINTUI_LIKELY(exp) (exp)
#endif // #ifndef __DF_DEF_H__

