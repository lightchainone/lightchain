#ifndef  __BSL_CHECK_CAST_H_
#define  __BSL_CHECK_CAST_H_

#include "Lsc/exception/Lsc_exception.h"

namespace Lsc{
    /**
     *
     * 提供对C/C++各原生类型提供带上下溢出检查功能模板函数。使用方式与static_cast<>完全相同。
     *
     * 目前支持所有有符号数值类型、无符号数值类型、浮点数类型中任意两类型的双向转换以及C风格字符串（const char *）到以上各类型的单向转换。不考虑浮点数转换成浮点数类型的数值溢出问题。
     *
    **/
    template<typename DestType, typename SrcType>
        DestType check_cast( SrcType value );


    /**
     *
    **/
#if __GNUC__ < 4
    template<typename DestType>
        inline DestType check_cast( DestType value ){
            return value;
        }
#endif

}
#include "Lsc/check_cast/check_cast_generated.h"   //generated code
#include "Lsc/check_cast/check_cast_cstring.h"
#include "Lsc/check_cast/check_cast_Lsc_string.h"

#endif  //__CHECK_CAST_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
