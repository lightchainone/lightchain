
#ifndef  __BSL_VAR_COMPACK_DESERIALIZER_H__
#define  __BSL_VAR_COMPACK_DESERIALIZER_H__

#include "Lsc/var/IBinaryDeserializer.h"
#include "Lsc/ResourcePool.h"
#include "Lsc/var/Null.h"
#include "compack/compack.h"

namespace Lsc{
namespace var{
	

    
    class CompackDeserializer: plclic IBinaryDeserializer{
    plclic:
		
		
		enum copy_option_t{
			SHALLOW,
			DEEP,
		};

		
        CompackDeserializer(ResourcePool& rp)
        :_rp(rp),
		_pres(&Null::null),
		_copy_opt(DEEP)
		{ }
		
		
		virtual size_t try_deserialize( const void* buf, size_t max_size);

		virtual IVar& get_result() const{
			return *_pres;
		}
		
		int set_opt(int optname, const void *optval, size_t optlen);

		void set_copy_option( copy_option_t opt ){
			_copy_opt = opt;
		}

		copy_option_t get_copy_option() const {
			return _copy_opt;
		}
	private:
		
		IVar& _deserialize_value(const compack::buffer::Reader & reader, const compack::Iterator &it);
	
		
		IVar& _deserialize_dict(compack::buffer::Reader & reader);
		
		
		IVar& _deserialize_array(compack::buffer::Reader & reader);

		
		IVar& _deserialize_objectisoarray(compack::buffer::Reader &pack);
	
	private:
        ResourcePool&  _rp;    
		IVar*		   _pres;
		copy_option_t _copy_opt;		  
	};

}}   


#endif  



