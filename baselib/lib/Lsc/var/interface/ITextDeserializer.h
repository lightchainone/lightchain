#ifndef  __BSL_VAR_ITEXT_DESERIALIZER_H__
#define  __BSL_VAR_ITEXT_DESERIALIZER_H__

#include <Lsc/var/IVar.h>
#include <Lsc/var/Ref.h>
#include <Lsc/var/IBinaryDeserializer.h>
namespace Lsc{
namespace var{

    class ITextDeserializer{
    plclic:
        virtual ~ITextDeserializer(){}
        virtual IVar& deserialize(const char* text) = 0;
    };

}}   //namespace Lsc::var

#endif  //__BSL_VAR_ITEXT_DESERIALIZER_H__

/* vim: set ts=4 sw=4 sts=4 tw=100 */
