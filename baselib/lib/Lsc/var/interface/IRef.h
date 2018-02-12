#ifndef  __BSL_VAR__IREF_H_
#define  __BSL_VAR__IREF_H_
#include <Lsc/var/IVar.h>
namespace Lsc{ namespace var {
    class IRef: plclic IVar{
    plclic:
        typedef IVar::string_type   string_type;
        typedef IVar::field_type    field_type;
        virtual IVar& ref() const = 0;
        using IVar::operator=;
    };  //end of class 
}}//end of namespace


#endif  //__BSL_VAR__IREF_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
