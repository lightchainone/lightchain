#ifndef  __BSL_VAR_ITEXT_SERIALIZER_H__
#define  __BSL_VAR_ITEXT_SERIALIZER_H__

#include <Lsc/var/IVar.h>
#include <Lsc/var/Ref.h>

namespace Lsc{
namespace var{

    class ITextSerializer{
    plclic:
        virtual ~ITextSerializer(){}
        virtual void serialize(const IVar& var, Lsc::AutoBuffer& buf) = 0;
    };

}}   //namespace Lsc::var

#endif  //__BSL_VAR_ITEXT_SERIALIZER_H__

/* vim: set ts=4 sw=4 sts=4 tw=100 */
