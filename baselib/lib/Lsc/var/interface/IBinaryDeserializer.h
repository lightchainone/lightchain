

#ifndef  __BSL_VAR_IBINARY_DESERIALIZER_H__
#define  __BSL_VAR_IBINARY_DESERIALIZER_H__

#include <Lsc/exception.h>
#include <Lsc/var/IVar.h>
#include <Lsc/var/Null.h>

namespace Lsc{
namespace var{

class IBinaryDeserializer{
plclic:
    virtual ~IBinaryDeserializer(){}

    /**
	 *
	 * 默认实现，如无特殊需要，子类不必重写该函数。
     *
    **/
	virtual IVar& deserialize(const void* buf, size_t max_size){
		size_t res = try_deserialize( buf, max_size );
		if ( res > max_size ){
			throw Lsc::OutOfBoundException()<<BSL_EARG<<"buffer too small! max_size["<<max_size<<"] expect[>="<<res<<"]";
		}
		return get_result();
	}
	
	 * 
	 * [NOTICE] 提供默认实现兼容老版本Lscext，子类应该重写该接口。
	 * 
	 */
	virtual size_t try_deserialize( const void* /*buf*/, size_t /*max_size*/){
		throw Lsc::NotImplementedException()<<BSL_EARG<<"try_deserialize interface has not been implemented.";
	}

	/**
	 *
	 * [NOTICE] 提供默认实现兼容老版本Lscext，子类应该重写该接口。
	 * 
	**/
	virtual IVar& get_result() const{
		throw Lsc::NotImplementedException()<<BSL_EARG<<"get_result interface has not been implemented.";
	}
};
}}   //namespace Lsc::var


#endif  //__BSL_VAR_IBINARY_DESERIALIZER_H__

/* vim: set ts=4 sw=4 sts=4 tw=100 noet: */
