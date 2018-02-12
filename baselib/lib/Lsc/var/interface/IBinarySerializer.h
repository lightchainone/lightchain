

#ifndef  __BSL_VAR_IBINARY_SERIALIZER_H__
#define  __BSL_VAR_IBINARY_SERIALIZER_H__

#include "Lsc/var/IVar.h"
namespace Lsc{
namespace var{
	class IBinarySerializer{
	plclic:
		virtual ~IBinarySerializer(){}

		virtual size_t serialize(const IVar& var, void* buf, size_t max_size) = 0;
	};
}}   //namespace Lsc::var

#endif  //__BSL_VAR_IBINARY_SERIALIZER_H__

/* vim: set ts=4 sw=4 sts=4 tw=100 noet: */
