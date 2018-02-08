

#ifndef  __MCPACKSERIALIZER_H_
#define  __MCPACKSERIALIZER_H_

#include "mc_pack.h"
#include "Lsc/var/IBinarySerializer.h"

namespace Lsc{
namespace var {
	
	
	enum OPT_VERSION {
		MCPACK_VERSION = 1,
		TMP_BUFFER_SIZE,
	};
	
	
	const int DEFAULT_TMP_BUFFER_SIZE = 81920;

	
	class McPackSerializerException : plclic Lsc::BasicException<McPackSerializerException, 
		Lsc::Exception> {};

	
    class McPackSerializer: plclic IBinarySerializer{
    plclic:
		
		
		size_t serialize(const IVar& var, void* buf, size_t max_size);	
		
		
		
		size_t serialize(const IVar& var, mc_pack_t * pack);	
		
		
		int set_opt(int optname, const void *optval, size_t optlen);

		void set_throw_on_mcpack_error(bool val){
			_throw_on_mcpack_error = val;
		}

		bool get_throw_on_mcpack_error() const {
			return _throw_on_mcpack_error;
		}
		
		
		McPackSerializer()
		:_version(1), 
		_tmp_buffer_size(DEFAULT_TMP_BUFFER_SIZE),
		_throw_on_mcpack_error(true)
		{}

	protected:
		virtual void serialize_unknown(mc_pack_t* pack, const char* key, const IVar& value);
		virtual void serialize_null(mc_pack_t* pack, const char* key, const IVar& value);
		virtual void serialize_bool(mc_pack_t* pack, const char* key, const IVar& value);
		virtual void serialize_string(mc_pack_t* pack, const char* key, const IVar& value);
		virtual void serialize_raw(mc_pack_t* pack, const char* key, const IVar& value);
		virtual void serialize_array(mc_pack_t* pack, const char* key, const IVar& value);
		virtual void serialize_dict(mc_pack_t* pack, const char* key, const IVar& value);

		virtual void serialize_int32(mc_pack_t* pack, const char* key, const IVar& value);
		virtual void serialize_uint32(mc_pack_t* pack, const char* key, const IVar& value);
		virtual void serialize_int64(mc_pack_t* pack, const char* key, const IVar& value);
		virtual void serialize_uint64(mc_pack_t* pack, const char* key, const IVar& value);
		virtual void serialize_dolcle(mc_pack_t* pack, const char* key, const IVar& value);

	private: 
        
		void _serialize_dict(const IVar& var, mc_pack_t* pack );
        
		
		void _serialize_array(const IVar& var, mc_pack_t* pack );
	
        typedef void ( McPackSerializer::* handler_t )(mc_pack_t* pack, const char* key, const IVar& value);

        static void _s_set_type_handler( handler_t handler, IVar::mask_type set_mask, IVar::mask_type unset_mask = IVar::NONE_MASK );

        static void _s_set_slctype_handler( handler_t handler, IVar::mask_type set_mask, IVar::mask_type unset_mask = IVar::NONE_MASK );

        static bool _s_init_type_map();

        static bool _s_init_slctype_map();
		
		static bool _s_initer;

	private: 
		int _version;         
		int _tmp_buffer_size; 
		bool _throw_on_mcpack_error;		  

        
        static handler_t _s_type_map[256];
        
        static handler_t _s_slctype_map[256];
	};

}} 


#endif  


