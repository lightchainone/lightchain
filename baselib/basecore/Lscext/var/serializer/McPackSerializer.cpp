
#include "McPackSerializer.h"
#include "Lsc/var/var_traits.h"

#include<typeinfo>
#include<iostream>
using namespace std;
#define see(x) do{  \
	cerr<<__FILE__<<":"<<__LINE__<<": " <<(#x)<<" = "<<(x)<<" ("<<typeid(x).name()<<")"<<endl;\
}while(0)

namespace Lsc{ namespace var{

	size_t McPackSerializer::serialize(const IVar& var, void* buf, size_t max_size){
		if ( !buf ) {
			throw Lsc::NullPointerException()<<BSL_EARG<<"buf";
		}

		if ( max_size == 0 ){
			throw Lsc::BadArgumentException()<<BSL_EARG<<"max_size can't be 0";
		}

		if ( !var.is_dict() ) {
			throw Lsc::BadArgumentException()<<BSL_EARG<<"Only Dict is accepted";
		}

		mc_pack_t* pack = NULL;
		int ret = 0; 

#ifdef __MC_PACK_DEF_H_
		char* tmpbuf = static_cast<char*>(alloca(_tmp_buffer_size));		
		if ( _version == 2 || _version == 1 ) {
			pack = mc_pack_open_w(_version, static_cast<char*>(buf), max_size, 
					tmpbuf, _tmp_buffer_size);
			ret = MC_PACK_PTR_ERR(pack); 
			if ( ret ) {
				throw McPackSerializerException()<< BSL_EARG <<"pack create failed: " << ret;
			}
		}else {
			throw McPackSerializerException()<<BSL_EARG<<"version "<<_version<<"is not supported"; 
		}
#else
		pack = mc_pack_create((char *)buf, max_size);
		ret = MC_PACK_PTR_ERR(pack); 
		if ( ret ) {
			throw McPackSerializerException()<< BSL_EARG <<"pack create failed: " << ret;
		}
#endif
		_serialize_dict( var, pack );

		mc_pack_close(pack);

#ifdef __MC_PACK_DEF_H_
		if ( buf != mc_pack_get_buffer(pack) ) {
			
			throw McPackSerializerException()<<BSL_EARG<<"buf not equal to mc_pack_get_buffer";
		}
#endif

		return mc_pack_get_size(pack);
	}

	size_t McPackSerializer::serialize(const IVar& var, mc_pack_t * pack) {
		if ( pack == NULL ){
			throw Lsc::NullPointerException()<<BSL_EARG<<"pack cannot be NULL!";
		}
		int ret = MC_PACK_PTR_ERR(pack); 
		if ( ret ) {
			throw McPackSerializerException()<< BSL_EARG <<"not a valid pack" << ret;
		}

		if ( var.is_dict() ) {
			_serialize_dict(var, pack );
		}else if ( var.is_array() ) {
			_serialize_array(var, pack );
		}else {
			throw Lsc::BadArgumentException()<<BSL_EARG<<"Only Dict/Array is accepted";
		}	

		return mc_pack_get_size(pack);
	}

	int McPackSerializer::set_opt(int optname, const void *optval, size_t optlen) {
		if ( optval == NULL ){
			throw Lsc::NullPointerException()<<BSL_EARG<<"optval cannot be NULL!";
		}
		switch ( optname ) 
		{
			case MCPACK_VERSION:
				{
					if ( optval && optlen == sizeof(int) ) 
					{
						_version = *(const int*)optval;	
						return 0;
					}
				}
				break;
			case TMP_BUFFER_SIZE:
				{
					if ( optval && optlen == sizeof(int) ) 
					{
						_tmp_buffer_size = *(const int*)optval;	
						return 0;
					}
				}
				break;
			default:
				break;
		}
		return -1;
	}

	void McPackSerializer::serialize_unknown(mc_pack_t* , const char* , const IVar& ){
		
	}

	void McPackSerializer::serialize_string(mc_pack_t* pack, const char* key, const IVar& value){
		int err = mc_pack_put_str(pack,key,value.c_str() );
		if ( _throw_on_mcpack_error && err) {
			throw McPackSerializerException()<<BSL_EARG<<"mc_pack_put_str() failed: " <<mc_pack_perror(err);
		}
	}

	void McPackSerializer::serialize_int32(mc_pack_t* pack, const char* key, const IVar& value){
		int err = mc_pack_put_int32(pack,key,value.to_int32() );
		if ( _throw_on_mcpack_error && err) {
			throw McPackSerializerException()<<BSL_EARG<<"mc_pack_put_int32() failed: " <<mc_pack_perror(err);
		}
	}
	void McPackSerializer::serialize_int64(mc_pack_t* pack, const char* key, const IVar& value){
		int err = mc_pack_put_int64(pack,key,value.to_int64() );
		if ( _throw_on_mcpack_error && err) {
			throw McPackSerializerException()<<BSL_EARG<<"mc_pack_put_int64() failed: " <<mc_pack_perror(err);
		}
	}
	void McPackSerializer::serialize_uint32(mc_pack_t* pack, const char* key, const IVar& value){
		int err = mc_pack_put_uint32(pack,key,value.to_uint32() );
		if ( _throw_on_mcpack_error && err) {
			throw McPackSerializerException()<<BSL_EARG<<"mc_pack_put_uint32() failed: " <<mc_pack_perror(err);
		}
	}
	void McPackSerializer::serialize_uint64(mc_pack_t* pack, const char* key, const IVar& value){
		int err = mc_pack_put_uint64(pack,key,value.to_uint64() );
		if ( _throw_on_mcpack_error && err) {
			throw McPackSerializerException()<<BSL_EARG<<"mc_pack_put_uint64() failed: " <<mc_pack_perror(err);
		}
	}
	void McPackSerializer::serialize_dolcle(mc_pack_t* pack, const char* key, const IVar& value){
		int err = mc_pack_put_dolcle(pack,key,value.to_dolcle() );
		if ( _throw_on_mcpack_error && err) {
			throw McPackSerializerException()<<BSL_EARG<<"mc_pack_put_dolcle() failed: " <<mc_pack_perror(err); }
	}
	void McPackSerializer::serialize_bool(mc_pack_t* pack, const char* key, const IVar& value){
		int err = mc_pack_put_bool(pack,key,value.to_bool() );
		if ( _throw_on_mcpack_error && err) {
			throw McPackSerializerException()<<BSL_EARG<<"mc_pack_put_bool() failed: " <<mc_pack_perror(err);
		}
	}
	void McPackSerializer::serialize_raw(mc_pack_t* pack, const char* key, const IVar& value){
		int err = mc_pack_put_raw(pack,key,value.to_raw().data,value.to_raw().length );
		if ( _throw_on_mcpack_error && err) {
			throw McPackSerializerException()<<BSL_EARG<<"mc_pack_put_raw() failed: " <<mc_pack_perror(err);
		}
	}
	void McPackSerializer::serialize_array(mc_pack_t* pack, const char* key, const IVar& value){
		
		mc_pack_t* ppack = mc_pack_put_array(pack,key);
		if (MC_PACK_PTR_ERR(ppack)) {
			throw McPackSerializerException()<<BSL_EARG<<"mc_pack_put_array() failed: " <<mc_pack_perror(MC_PACK_PTR_ERR(ppack)); 
		}
		_serialize_array( value, ppack );
#ifdef __MC_PACK_DEF_H_
		mc_pack_finish(ppack);
#endif
	}
	void McPackSerializer::serialize_dict(mc_pack_t* pack, const char* key, const IVar& value){
		
		mc_pack_t* ppack = mc_pack_put_object(pack,key);
		if (MC_PACK_PTR_ERR(ppack)) {
			throw McPackSerializerException()<<BSL_EARG<<"mc_pack_put_object() failed: " <<mc_pack_perror(MC_PACK_PTR_ERR(ppack));
		}
		_serialize_dict( value, ppack );
#ifdef __MC_PACK_DEF_H_
		mc_pack_finish(ppack);
#endif
	}

	void McPackSerializer::serialize_null(mc_pack_t* pack, const char* key, const IVar& ){
		int err = mc_pack_put_null(pack,key);
		if ( _throw_on_mcpack_error && err) {
			throw McPackSerializerException()<<BSL_EARG<<"pack put object failed: " <<mc_pack_perror(err);
		}
	}

	void McPackSerializer::_serialize_dict(const IVar& var, mc_pack_t* pack ){
		IVar::dict_const_iterator iter = var.dict_begin();
		IVar::dict_const_iterator end  = var.dict_end();
		for( ; iter != end; ++iter ){
			const char* key = iter->key().c_str();
			if ( key && key[0] ) { 
				const IVar& child = iter->value();
				IVar::mask_type mask = child.get_mask();
				if ( mask & IVar::IS_NUMBER ){
					(this->*_s_slctype_map[(mask>>8)&0xFF])( pack, key, child );
				}else{
					(this->*_s_type_map[mask&0xFF])( pack, key, child );
				}
			}else {
				throw McPackSerializerException()<<BSL_EARG<<"mc pack key cann't be empty";
			}
		}
	}

	void McPackSerializer::_serialize_array(const IVar& var, mc_pack_t* pack ){
		size_t size = var.size();
		size_t i=0;
		for (; i<size; ++i) {
			const IVar& child = var.get(i);
			IVar::mask_type mask = child.get_mask();
			if ( mask & IVar::IS_NUMBER ){
				(this->*_s_slctype_map[(mask>>8)&0xFF])( pack, NULL, child );
			}else{
				(this->*_s_type_map[mask&0xFF])( pack, NULL, child );
			}
		}
	}

	void McPackSerializer::_s_set_type_handler( handler_t handler, IVar::mask_type set_mask, IVar::mask_type unset_mask ){
		for( IVar::mask_type i = 0; i < 256; ++ i ){
			if ( IVar::check_mask( i, set_mask, unset_mask ) ){
				_s_type_map[i] = handler;
			}
		}
	}

	void McPackSerializer::_s_set_slctype_handler( handler_t handler, IVar::mask_type set_mask, IVar::mask_type unset_mask ){
		for( IVar::mask_type i = 0; i < 256; ++ i ){
			if ( IVar::check_mask( (i << 8)|IVar::IS_NUMBER, set_mask, unset_mask ) ){
				_s_slctype_map[i] = handler;
			}
		}
	}

	bool McPackSerializer::_s_init_type_map(){
		
		_s_set_type_handler( &McPackSerializer::serialize_unknown,    IVar::NONE_MASK, IVar::NONE_MASK );
		_s_set_type_handler( &McPackSerializer::serialize_null,       IVar::NONE_MASK, IVar::ALL_MASK );
		_s_set_type_handler( &McPackSerializer::serialize_bool,       IVar::IS_BOOL );
		
		_s_set_type_handler( &McPackSerializer::serialize_string,     IVar::IS_STRING );
		_s_set_type_handler( &McPackSerializer::serialize_raw,        IVar::IS_RAW );
		_s_set_type_handler( &McPackSerializer::serialize_array,      IVar::IS_ARRAY );
		_s_set_type_handler( &McPackSerializer::serialize_dict,       IVar::IS_DICT );
		
		
		return true;
	}

	bool McPackSerializer::_s_init_slctype_map(){
		
		_s_set_slctype_handler( &McPackSerializer::serialize_unknown, IVar::NONE_MASK, IVar::NONE_MASK );
		
		_s_set_slctype_handler( &McPackSerializer::serialize_int32, var_traits<signed char>::MASK, var_traits<signed char>::ANTI_MASK );
		_s_set_slctype_handler( &McPackSerializer::serialize_uint32, var_traits<unsigned char>::MASK, var_traits<unsigned char>::ANTI_MASK );
		_s_set_slctype_handler( &McPackSerializer::serialize_int32, var_traits<signed short>::MASK, var_traits<signed short>::ANTI_MASK );
		_s_set_slctype_handler( &McPackSerializer::serialize_uint32, var_traits<unsigned short>::MASK, var_traits<unsigned short>::ANTI_MASK );
		_s_set_slctype_handler( &McPackSerializer::serialize_int32, var_traits<signed int>::MASK, var_traits<signed int>::ANTI_MASK );
		_s_set_slctype_handler( &McPackSerializer::serialize_uint32, var_traits<unsigned int>::MASK, var_traits<unsigned int>::ANTI_MASK );
		_s_set_slctype_handler( &McPackSerializer::serialize_int64, var_traits<signed long long>::MASK, var_traits<signed long long>::ANTI_MASK );
		_s_set_slctype_handler( &McPackSerializer::serialize_uint64, var_traits<unsigned long long>::MASK, var_traits<unsigned long long>::ANTI_MASK );
		_s_set_slctype_handler( &McPackSerializer::serialize_dolcle, var_traits<float>::MASK, var_traits<float>::ANTI_MASK );
		_s_set_slctype_handler( &McPackSerializer::serialize_dolcle, var_traits<dolcle>::MASK, var_traits<dolcle>::ANTI_MASK );
		return true;
	}

	
	McPackSerializer::handler_t McPackSerializer::_s_type_map[];
	
	McPackSerializer::handler_t McPackSerializer::_s_slctype_map[];
	bool McPackSerializer::_s_initer = McPackSerializer::_s_init_type_map() && McPackSerializer::_s_init_slctype_map();
}}	


