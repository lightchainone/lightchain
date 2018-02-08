
#include "CompackSerializer.h"
#include "compack/compack.h"
#include <Lsc/var/var_traits.h>

namespace Lsc{
namespace var{
	
	size_t CompackSerializer::serialize(const IVar& var, void* buf, size_t max_size)	
	{
		if ( !buf ) {
			throw Lsc::NullPointerException()<<BSL_EARG<<"buf";
		}
		
		if ( max_size == 0 ){
			throw Lsc::BadArgumentException()<<BSL_EARG<<"max_size can't be 0";
		}

		compack::Buffer buffer(buf, max_size);
		IVar::mask_type mask = var.get_mask();
		if ( mask & IVar::IS_NUMBER ){
			return (this->*_s_buf_slctype_map[(mask>>8)&0xFF])( var, buffer );
		}else{
			return (this->*_s_buf_type_map[mask&0xFF])( var, buffer );
		}
	}
		
	
	int CompackSerializer::set_opt(int optname, const void *, size_t)
	{
		switch ( optname ) 
		{
			default:
				break;
		}
		return -1;
	}

	size_t CompackSerializer::_serialize_string_to_buf(const IVar& value, compack::Buffer &buf){
		return compack::Writer<compack::Buffer>::putString(&buf, value.c_str(), static_cast<unsigned int>(value.c_str_len()) + 1);
	}

	size_t CompackSerializer::_serialize_bool_to_buf(const IVar& value, compack::Buffer &buf){
		return compack::Writer<compack::Buffer>::putNum(&buf, value.to_bool() );
	}

	template<typename T>
		size_t CompackSerializer::_serialize_number_to_buf(const IVar& value, compack::Buffer &buf){
			return compack::Writer<compack::Buffer>::putNum(&buf, value.template to<T>() );
		}

	size_t CompackSerializer::_serialize_raw_to_buf(const IVar& value, compack::Buffer &buf){
		return compack::Writer<compack::Buffer>::putBinary(&buf, value.to_raw().data, static_cast<unsigned int>(value.to_raw().length) );
	}

	size_t CompackSerializer::_serialize_array_to_buf(const IVar& value, compack::Buffer &buf){
		compack::buffer::Array array(&buf);
		size_t n = value.size();
		size_t i=0;
		for (; i<n; ++i) {
			const IVar & child = value.get(i);
			IVar::mask_type mask = child.get_mask();
			if ( mask & IVar::IS_NUMBER ){
				(this->*_s_array_slctype_map[(mask>>8)&0xFF])( child, array );
			}else{
				(this->*_s_array_type_map[mask&0xFF])( child, array );
			}
		}
		return array.size();
	}

	size_t CompackSerializer::_serialize_dict_to_buf(const IVar& value, compack::Buffer &buf){
		compack::buffer::Object object(&buf);
        IVar::dict_const_iterator iter = value.dict_begin();
        IVar::dict_const_iterator end  = value.dict_end();
		for( ; iter != end; ++iter ){
			const IVar & child = iter->value();
			IVar::mask_type mask = child.get_mask();
			if ( mask & IVar::IS_NUMBER ){
				(this->*_s_dict_slctype_map[(mask>>8)&0xFF])( iter->key(), child, object );
			}else{
				(this->*_s_dict_type_map[mask&0xFF])( iter->key(), child, object );
			}
		}
		return object.size();
	}

	size_t CompackSerializer::_serialize_null_to_buf(const IVar& , compack::Buffer &buf){
		return compack::Writer<compack::Buffer>::putNull(&buf);
	}

	size_t CompackSerializer::_serialize_unknown_to_buf(const IVar& , compack::Buffer &){
		return 0;
	}

	void CompackSerializer::_serialize_string_to_array(const IVar& value, compack::buffer::Array &arr){
		arr.putString(value.c_str(), value.c_str_len() + 1);
	}

	void CompackSerializer::_serialize_bool_to_array(const IVar& value, compack::buffer::Array &arr){
		arr.putNum(value.to_bool() );
	}

	template<typename T>
		void CompackSerializer::_serialize_number_to_array(const IVar& value, compack::buffer::Array &arr){
			arr.putNum(value.template to<T>() );
		}

	void CompackSerializer::_serialize_raw_to_array(const IVar& value, compack::buffer::Array &arr){
		arr.putBinary(value.to_raw().data, value.to_raw().length );
	}

	void CompackSerializer::_serialize_array_to_array(const IVar& value, compack::buffer::Array &arr){
		compack::buffer::Array array(&arr);
		size_t n = value.size();
		size_t i=0;
		for (; i<n; ++i) {
			const IVar & child = value.get(i);
			IVar::mask_type mask = child.get_mask();
			if ( mask & IVar::IS_NUMBER ){
				(this->*_s_array_slctype_map[(mask>>8)&0xFF])( child, array );
			}else{
				(this->*_s_array_type_map[mask&0xFF])( child, array );
			}
		}
	}

	void CompackSerializer::_serialize_dict_to_array(const IVar& value, compack::buffer::Array &arr){
		compack::buffer::Object object(&arr);
        IVar::dict_const_iterator iter = value.dict_begin();
        IVar::dict_const_iterator end  = value.dict_end();
		for( ; iter != end; ++iter ){
			const IVar & child = iter->value();
			IVar::mask_type mask = child.get_mask();
			if ( mask & IVar::IS_NUMBER ){
				(this->*_s_dict_slctype_map[(mask>>8)&0xFF])( iter->key(), child, object );
			}else{
				(this->*_s_dict_type_map[mask&0xFF])( iter->key(), child, object );
			}
		}
	}

	void CompackSerializer::_serialize_null_to_array(const IVar& , compack::buffer::Array &arr){
		arr.putNull();
	}
	void CompackSerializer::_serialize_unknown_to_array(const IVar& , compack::buffer::Array &){
		
	}

	void CompackSerializer::_serialize_string_to_dict(const IVar::field_type& key, const IVar& value, compack::buffer::Object &dict){
		dict.putString(key.c_str(), key.length() + 1, value.c_str(), value.c_str_len() + 1);
	}

	void CompackSerializer::_serialize_bool_to_dict(const IVar::field_type& key, const IVar& value, compack::buffer::Object &dict){
		dict.putNum(key.c_str(), key.length() + 1, value.to_bool() );
	}

	template<typename T>
		void CompackSerializer::_serialize_number_to_dict(const IVar::field_type& key, const IVar& value, compack::buffer::Object &dict){
			dict.putNum(key.c_str(), key.length() + 1, value.template to<T>() );
		}

	void CompackSerializer::_serialize_raw_to_dict(const IVar::field_type& key, const IVar& value, compack::buffer::Object &dict){
		dict.putBinary(key.c_str(), key.length() + 1, value.to_raw().data, value.to_raw().length );
	}

	void CompackSerializer::_serialize_array_to_dict(const IVar::field_type& key, const IVar& value, compack::buffer::Object &dict){
		compack::buffer::Array array(&dict, key.c_str(), key.length() + 1);
		size_t n = value.size();
		size_t i=0;
		for (; i<n; ++i) {
			const IVar & child = value.get(i);
			IVar::mask_type mask = child.get_mask();
			if ( mask & IVar::IS_NUMBER ){
				(this->*_s_array_slctype_map[(mask>>8)&0xFF])( child, array );
			}else{
				(this->*_s_array_type_map[mask&0xFF])( child, array );
			}
		}
	}

	void CompackSerializer::_serialize_dict_to_dict(const IVar::field_type& key, const IVar& value, compack::buffer::Object &dict){
		compack::buffer::Object object(&dict, key.c_str(), key.length() + 1);
        IVar::dict_const_iterator iter = value.dict_begin();
        IVar::dict_const_iterator end  = value.dict_end();
		for( ; iter != end; ++iter ){
			const IVar & child = iter->value();
			IVar::mask_type mask = child.get_mask();
			if ( mask & IVar::IS_NUMBER ){
				(this->*_s_dict_slctype_map[(mask>>8)&0xFF])( iter->key(), child, object );
			}else{
				(this->*_s_dict_type_map[mask&0xFF])( iter->key(), child, object );
			}
		}
	}

	void CompackSerializer::_serialize_null_to_dict(const IVar::field_type& key, const IVar& , compack::buffer::Object &dict){
		dict.putNull(key.c_str(), key.length() + 1);
	}
	void CompackSerializer::_serialize_unknown_to_dict(const IVar::field_type& , const IVar& , compack::buffer::Object &){
		
	}

	void CompackSerializer::_s_set_buf_type_handler( buf_handler_t handler, IVar::mask_type set_mask, IVar::mask_type unset_mask ){
		for( IVar::mask_type i = 0; i < 256; ++ i ){
			if ( IVar::check_mask( i, set_mask, unset_mask ) ){
				_s_buf_type_map[i] = handler;
			}
		}
	}

	void CompackSerializer::_s_set_buf_slctype_handler( buf_handler_t handler, IVar::mask_type set_mask, IVar::mask_type unset_mask ){
		for( IVar::mask_type i = 0; i < 256; ++ i ){
			if ( IVar::check_mask( (i << 8)|IVar::IS_NUMBER, set_mask, unset_mask ) ){
				_s_buf_slctype_map[i] = handler;
			}
		}
	}

	void CompackSerializer::_s_set_array_type_handler( array_handler_t handler, IVar::mask_type set_mask, IVar::mask_type unset_mask ){
		for( IVar::mask_type i = 0; i < 256; ++ i ){
			if ( IVar::check_mask( i, set_mask, unset_mask ) ){
				_s_array_type_map[i] = handler;
			}
		}
	}

	void CompackSerializer::_s_set_array_slctype_handler( array_handler_t handler, IVar::mask_type set_mask, IVar::mask_type unset_mask ){
		for( IVar::mask_type i = 0; i < 256; ++ i ){
			if ( IVar::check_mask( (i << 8)|IVar::IS_NUMBER, set_mask, unset_mask ) ){
				_s_array_slctype_map[i] = handler;
			}
		}
	}

	void CompackSerializer::_s_set_dict_type_handler( dict_handler_t handler, IVar::mask_type set_mask, IVar::mask_type unset_mask ){
		for( IVar::mask_type i = 0; i < 256; ++ i ){
			if ( IVar::check_mask( i, set_mask, unset_mask ) ){
				_s_dict_type_map[i] = handler;
			}
		}
	}

	void CompackSerializer::_s_set_dict_slctype_handler( dict_handler_t handler, IVar::mask_type set_mask, IVar::mask_type unset_mask ){
		for( IVar::mask_type i = 0; i < 256; ++ i ){
			if ( IVar::check_mask( (i << 8)|IVar::IS_NUMBER, set_mask, unset_mask ) ){
				_s_dict_slctype_map[i] = handler;
			}
		}
	}

	bool CompackSerializer::_s_init_type_map(){
		
		
		_s_set_buf_type_handler( &CompackSerializer::_serialize_unknown_to_buf,    IVar::NONE_MASK, IVar::NONE_MASK );
		_s_set_buf_type_handler( &CompackSerializer::_serialize_null_to_buf,       IVar::NONE_MASK, IVar::ALL_MASK );
		_s_set_buf_type_handler( &CompackSerializer::_serialize_bool_to_buf,       IVar::IS_BOOL );
		_s_set_buf_type_handler( &CompackSerializer::_serialize_string_to_buf,     IVar::IS_STRING );
		_s_set_buf_type_handler( &CompackSerializer::_serialize_raw_to_buf,        IVar::IS_RAW );
		_s_set_buf_type_handler( &CompackSerializer::_serialize_array_to_buf,      IVar::IS_ARRAY );
		_s_set_buf_type_handler( &CompackSerializer::_serialize_dict_to_buf,       IVar::IS_DICT );

		_s_set_array_type_handler( &CompackSerializer::_serialize_unknown_to_array,    IVar::NONE_MASK, IVar::NONE_MASK );
		_s_set_array_type_handler( &CompackSerializer::_serialize_null_to_array,       IVar::NONE_MASK, IVar::ALL_MASK );
		_s_set_array_type_handler( &CompackSerializer::_serialize_bool_to_array,       IVar::IS_BOOL );
		_s_set_array_type_handler( &CompackSerializer::_serialize_string_to_array,     IVar::IS_STRING );
		_s_set_array_type_handler( &CompackSerializer::_serialize_raw_to_array,        IVar::IS_RAW );
		_s_set_array_type_handler( &CompackSerializer::_serialize_array_to_array,      IVar::IS_ARRAY );
		_s_set_array_type_handler( &CompackSerializer::_serialize_dict_to_array,       IVar::IS_DICT );

		_s_set_dict_type_handler( &CompackSerializer::_serialize_unknown_to_dict,    IVar::NONE_MASK, IVar::NONE_MASK );
		_s_set_dict_type_handler( &CompackSerializer::_serialize_null_to_dict,       IVar::NONE_MASK, IVar::ALL_MASK );
		_s_set_dict_type_handler( &CompackSerializer::_serialize_bool_to_dict,       IVar::IS_BOOL );
		_s_set_dict_type_handler( &CompackSerializer::_serialize_string_to_dict,     IVar::IS_STRING );
		_s_set_dict_type_handler( &CompackSerializer::_serialize_raw_to_dict,        IVar::IS_RAW );
		_s_set_dict_type_handler( &CompackSerializer::_serialize_array_to_dict,      IVar::IS_ARRAY );
		_s_set_dict_type_handler( &CompackSerializer::_serialize_dict_to_dict,       IVar::IS_DICT );
		return true;
	}

	bool CompackSerializer::_s_init_slctype_map(){
		
		_s_set_buf_slctype_handler( &CompackSerializer::_serialize_unknown_to_buf, IVar::NONE_MASK, IVar::NONE_MASK );
		_s_set_buf_slctype_handler( &CompackSerializer::_serialize_number_to_buf<signed char>, var_traits<signed char>::MASK, var_traits<signed char>::ANTI_MASK );
		_s_set_buf_slctype_handler( &CompackSerializer::_serialize_number_to_buf<unsigned char>, var_traits<unsigned char>::MASK, var_traits<unsigned char>::ANTI_MASK );
		_s_set_buf_slctype_handler( &CompackSerializer::_serialize_number_to_buf<signed short>, var_traits<signed short>::MASK, var_traits<signed short>::ANTI_MASK );
		_s_set_buf_slctype_handler( &CompackSerializer::_serialize_number_to_buf<unsigned short>, var_traits<unsigned short>::MASK, var_traits<unsigned short>::ANTI_MASK );
		_s_set_buf_slctype_handler( &CompackSerializer::_serialize_number_to_buf<signed int>, var_traits<signed int>::MASK, var_traits<signed int>::ANTI_MASK );
		_s_set_buf_slctype_handler( &CompackSerializer::_serialize_number_to_buf<unsigned int>, var_traits<unsigned int>::MASK, var_traits<unsigned int>::ANTI_MASK );
#if __WORDSIZE == 64
		_s_set_buf_slctype_handler( &CompackSerializer::_serialize_number_to_buf<signed long>, var_traits<signed long long>::MASK, var_traits<signed long long>::ANTI_MASK );
		_s_set_buf_slctype_handler( &CompackSerializer::_serialize_number_to_buf<unsigned long>, var_traits<unsigned long long>::MASK, var_traits<unsigned long long>::ANTI_MASK );
#else
		_s_set_buf_slctype_handler( &CompackSerializer::_serialize_number_to_buf<signed long long>, var_traits<signed long long>::MASK, var_traits<signed long long>::ANTI_MASK );
		_s_set_buf_slctype_handler( &CompackSerializer::_serialize_number_to_buf<unsigned long long>, var_traits<unsigned long long>::MASK, var_traits<unsigned long long>::ANTI_MASK );
#endif
		_s_set_buf_slctype_handler( &CompackSerializer::_serialize_number_to_buf<float>, var_traits<float>::MASK, var_traits<float>::ANTI_MASK );
		_s_set_buf_slctype_handler( &CompackSerializer::_serialize_number_to_buf<dolcle>, var_traits<dolcle>::MASK, var_traits<dolcle>::ANTI_MASK );

		_s_set_array_slctype_handler( &CompackSerializer::_serialize_unknown_to_array, IVar::NONE_MASK, IVar::NONE_MASK );
		_s_set_array_slctype_handler( &CompackSerializer::_serialize_number_to_array<signed char>, var_traits<signed char>::MASK, var_traits<signed char>::ANTI_MASK );
		_s_set_array_slctype_handler( &CompackSerializer::_serialize_number_to_array<unsigned char>, var_traits<unsigned char>::MASK, var_traits<unsigned char>::ANTI_MASK );
		_s_set_array_slctype_handler( &CompackSerializer::_serialize_number_to_array<signed short>, var_traits<signed short>::MASK, var_traits<signed short>::ANTI_MASK );
		_s_set_array_slctype_handler( &CompackSerializer::_serialize_number_to_array<unsigned short>, var_traits<unsigned short>::MASK, var_traits<unsigned short>::ANTI_MASK );
		_s_set_array_slctype_handler( &CompackSerializer::_serialize_number_to_array<signed int>, var_traits<signed int>::MASK, var_traits<signed int>::ANTI_MASK );
		_s_set_array_slctype_handler( &CompackSerializer::_serialize_number_to_array<unsigned int>, var_traits<unsigned int>::MASK, var_traits<unsigned int>::ANTI_MASK );
#if __WORDSIZE == 64
		_s_set_array_slctype_handler( &CompackSerializer::_serialize_number_to_array<signed long>, var_traits<signed long long>::MASK, var_traits<signed long long>::ANTI_MASK );
		_s_set_array_slctype_handler( &CompackSerializer::_serialize_number_to_array<unsigned long>, var_traits<unsigned long long>::MASK, var_traits<unsigned long long>::ANTI_MASK );
#else
		_s_set_array_slctype_handler( &CompackSerializer::_serialize_number_to_array<signed long long>, var_traits<signed long long>::MASK, var_traits<signed long long>::ANTI_MASK );
		_s_set_array_slctype_handler( &CompackSerializer::_serialize_number_to_array<unsigned long long>, var_traits<unsigned long long>::MASK, var_traits<unsigned long long>::ANTI_MASK );
#endif
		_s_set_array_slctype_handler( &CompackSerializer::_serialize_number_to_array<float>, var_traits<float>::MASK, var_traits<float>::ANTI_MASK );
		_s_set_array_slctype_handler( &CompackSerializer::_serialize_number_to_array<dolcle>, var_traits<dolcle>::MASK, var_traits<dolcle>::ANTI_MASK );

		_s_set_dict_slctype_handler( &CompackSerializer::_serialize_unknown_to_dict, IVar::NONE_MASK, IVar::NONE_MASK );
		_s_set_dict_slctype_handler( &CompackSerializer::_serialize_number_to_dict<signed char>, var_traits<signed char>::MASK, var_traits<signed char>::ANTI_MASK );
		_s_set_dict_slctype_handler( &CompackSerializer::_serialize_number_to_dict<unsigned char>, var_traits<unsigned char>::MASK, var_traits<unsigned char>::ANTI_MASK );
		_s_set_dict_slctype_handler( &CompackSerializer::_serialize_number_to_dict<signed short>, var_traits<signed short>::MASK, var_traits<signed short>::ANTI_MASK );
		_s_set_dict_slctype_handler( &CompackSerializer::_serialize_number_to_dict<unsigned short>, var_traits<unsigned short>::MASK, var_traits<unsigned short>::ANTI_MASK );
		_s_set_dict_slctype_handler( &CompackSerializer::_serialize_number_to_dict<signed int>, var_traits<signed int>::MASK, var_traits<signed int>::ANTI_MASK );
		_s_set_dict_slctype_handler( &CompackSerializer::_serialize_number_to_dict<unsigned int>, var_traits<unsigned int>::MASK, var_traits<unsigned int>::ANTI_MASK );
#if __WORDSIZE == 64
		_s_set_dict_slctype_handler( &CompackSerializer::_serialize_number_to_dict<signed long>, var_traits<signed long long>::MASK, var_traits<signed long long>::ANTI_MASK );
		_s_set_dict_slctype_handler( &CompackSerializer::_serialize_number_to_dict<unsigned long>, var_traits<unsigned long long>::MASK, var_traits<unsigned long long>::ANTI_MASK );
#else
		_s_set_dict_slctype_handler( &CompackSerializer::_serialize_number_to_dict<signed long long>, var_traits<signed long long>::MASK, var_traits<signed long long>::ANTI_MASK );
		_s_set_dict_slctype_handler( &CompackSerializer::_serialize_number_to_dict<unsigned long long>, var_traits<unsigned long long>::MASK, var_traits<unsigned long long>::ANTI_MASK );
#endif
		_s_set_dict_slctype_handler( &CompackSerializer::_serialize_number_to_dict<float>, var_traits<float>::MASK, var_traits<float>::ANTI_MASK );
		_s_set_dict_slctype_handler( &CompackSerializer::_serialize_number_to_dict<dolcle>, var_traits<dolcle>::MASK, var_traits<dolcle>::ANTI_MASK );
		return true;
	}

	
	CompackSerializer::buf_handler_t CompackSerializer::_s_buf_type_map[256];
	
	CompackSerializer::buf_handler_t CompackSerializer::_s_buf_slctype_map[256];
	
	CompackSerializer::array_handler_t CompackSerializer::_s_array_type_map[256];
	
	CompackSerializer::array_handler_t CompackSerializer::_s_array_slctype_map[256];
	
	CompackSerializer::dict_handler_t CompackSerializer::_s_dict_type_map[256];
	
	CompackSerializer::dict_handler_t CompackSerializer::_s_dict_slctype_map[256];
	bool CompackSerializer::_s_initer = CompackSerializer::_s_init_type_map() && CompackSerializer::_s_init_slctype_map();
}}	


