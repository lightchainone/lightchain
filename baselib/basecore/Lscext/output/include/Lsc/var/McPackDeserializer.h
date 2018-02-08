

#ifndef  __MCPACKDESERIALIZER_H_
#define  __MCPACKDESERIALIZER_H_

#include "Lsc/var/Null.h"
#include "Lsc/var/IBinaryDeserializer.h"
#include "Lsc/ResourcePool.h"
#include "mc_pack.h"

#include "McPackSerializer.h"


namespace Lsc{
namespace var{
	
	
	class McPackDeserializerException : plclic Lsc::BasicException<McPackDeserializerException, 
			Lsc::ParseErrorException> {};

    
    class McPackDeserializer: plclic IBinaryDeserializer{
    plclic:
		
		
		enum copy_option_t{
			SHALLOW,
			DEEP,
		};

		
        McPackDeserializer(ResourcePool& rp)
        :_rp(rp),
		_pres(&Null::null),
		_tmp_buffer_size(DEFAULT_TMP_BUFFER_SIZE),
		_copy_opt(DEEP)
		{ }
		
		
		IVar& deserialize(const void* buf, size_t max_size){
			size_t res = try_deserialize( buf, max_size );
			if ( res > max_size ){
				throw Lsc::OutOfBoundException()<<BSL_EARG<<"buffer too small! max_size["<<max_size<<"] expect[>="<<res<<"]";
			}
			return *_pres;
		}

		size_t try_deserialize( const void * buf, size_t max_size);
		
		virtual IVar& get_result() const{
			return *_pres;
		}
#ifdef __MC_PACK_DEF_H_
		
		IVar& deserialize(const mc_pack_t* pack);
#endif
		
		
		IVar& deserialize(const mc_pack_item_t* item);
		
		
		int set_opt(int optname, const void *optval, size_t optlen);

		void set_copy_option( copy_option_t opt ){
			_copy_opt = opt;
		}

		copy_option_t get_copy_option() const {
			return _copy_opt;
		}
	private:
	
		
		void _deserialize_dict(IVar& var, const mc_pack_t* pack);
		
		
		void _deserialize_array(IVar& var, const mc_pack_t* pack);
	
	private:
        ResourcePool&  _rp;    
		IVar*			_pres;
		int _tmp_buffer_size;  
		copy_option_t _copy_opt;		  
	};

}}   


#endif  


