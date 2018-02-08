#ifndef __BSL_VAR_COMPACK_SERIALIZER_H__
#define __BSL_VAR_COMPACK_SERIALIZER_H__
#include "Lsc/var/IBinarySerializer.h"
#include "compack/compack.h"

namespace Lsc{ namespace var{

	
    class CompackSerializer: plclic IBinarySerializer{
    plclic:
		
		
		size_t serialize(const IVar& var, void* buf, size_t max_size);	
		
		
		
		int set_opt(int optname, const void *optval, size_t optlen);
		
		

	private: 
		size_t _serialize_unknown_to_buf(const IVar& value, compack::Buffer &buf);
		size_t _serialize_null_to_buf(const IVar& value, compack::Buffer &buf);
		size_t _serialize_bool_to_buf(const IVar& value, compack::Buffer &buf);
		size_t _serialize_string_to_buf(const IVar& value, compack::Buffer &buf);
		size_t _serialize_raw_to_buf(const IVar& value, compack::Buffer &buf);
		size_t _serialize_array_to_buf(const IVar& value, compack::Buffer &buf);
		size_t _serialize_dict_to_buf(const IVar& value, compack::Buffer &buf);
		template<typename T>
			size_t _serialize_number_to_buf(const IVar& value, compack::Buffer &buf);

		void _serialize_unknown_to_array(const IVar& value, compack::buffer::Array &arr);
		void _serialize_null_to_array(const IVar& value, compack::buffer::Array &arr);
		void _serialize_bool_to_array(const IVar& value, compack::buffer::Array &arr);
		void _serialize_string_to_array(const IVar& value, compack::buffer::Array &arr);
		void _serialize_raw_to_array(const IVar& value, compack::buffer::Array &arr);
		void _serialize_array_to_array(const IVar& value, compack::buffer::Array &arr);
		void _serialize_dict_to_array(const IVar& value, compack::buffer::Array &arr);
		template<typename T>
			void _serialize_number_to_array(const IVar& value, compack::buffer::Array &arr);

		void _serialize_unknown_to_dict(const IVar::field_type& key, const IVar& value, compack::buffer::Object &dict);
		void _serialize_null_to_dict(const IVar::field_type& key, const IVar& value, compack::buffer::Object &dict);
		void _serialize_bool_to_dict(const IVar::field_type& key, const IVar& value, compack::buffer::Object &dict);
		void _serialize_string_to_dict(const IVar::field_type& key, const IVar& value, compack::buffer::Object &dict);
		void _serialize_raw_to_dict(const IVar::field_type& key, const IVar& value, compack::buffer::Object &dict);
		void _serialize_array_to_dict(const IVar::field_type& key, const IVar& value, compack::buffer::Object &dict);
		void _serialize_dict_to_dict(const IVar::field_type& key, const IVar& value, compack::buffer::Object &dict);
		template<typename T>
			void _serialize_number_to_dict(const IVar::field_type& key, const IVar& value, compack::buffer::Object &dict);

        typedef size_t ( CompackSerializer::* buf_handler_t )(const IVar& value, compack::Buffer &buf);

        typedef void ( CompackSerializer::* array_handler_t )(const IVar& value, compack::buffer::Array &arr);

        typedef void ( CompackSerializer::* dict_handler_t )(const IVar::field_type& key, const IVar& , compack::buffer::Object &dict);

        static void _s_set_buf_type_handler( buf_handler_t handler, IVar::mask_type set_mask, IVar::mask_type unset_mask = IVar::NONE_MASK );

        static void _s_set_buf_slctype_handler( buf_handler_t handler, IVar::mask_type set_mask, IVar::mask_type unset_mask = IVar::NONE_MASK );

        static void _s_set_array_type_handler( array_handler_t handler, IVar::mask_type set_mask, IVar::mask_type unset_mask = IVar::NONE_MASK );

        static void _s_set_array_slctype_handler( array_handler_t handler, IVar::mask_type set_mask, IVar::mask_type unset_mask = IVar::NONE_MASK );

        static void _s_set_dict_type_handler( dict_handler_t handler, IVar::mask_type set_mask, IVar::mask_type unset_mask = IVar::NONE_MASK );

        static void _s_set_dict_slctype_handler( dict_handler_t handler, IVar::mask_type set_mask, IVar::mask_type unset_mask = IVar::NONE_MASK );

        static bool _s_init_type_map();

        static bool _s_init_slctype_map();
		
		static bool _s_initer;
	private:
        
        static buf_handler_t _s_buf_type_map[256];
        
        static buf_handler_t _s_buf_slctype_map[256];
        
        static array_handler_t _s_array_type_map[256];
        
        static array_handler_t _s_array_slctype_map[256];
        
        static dict_handler_t _s_dict_type_map[256];
        
        static dict_handler_t _s_dict_slctype_map[256];
	};

}} 


#endif 


