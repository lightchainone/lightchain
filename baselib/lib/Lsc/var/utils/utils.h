#ifndef  __BSL_VAR_UTILS_H_
#define  __BSL_VAR_UTILS_H_

#include "Lsc/var/IVar.h"
#include "Lsc/var/assign.h"
#include "Lsc/var/var_traits.h"
#include "Lsc/var/IRef.h"

namespace Lsc{
    namespace var{
        /**
         *
        **/
        void dump_to_string(const IVar& var, IVar::string_type& res, size_t verbose_level, const char *line_delimiter, size_t per_indent = 4, size_t total_indent = 0 );

        /**
         *
        **/
        void print( const IVar& var, size_t verbose_level );      
    }
}
#endif  //__UTILS_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
