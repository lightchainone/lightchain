
#include "Lsc/var/McPackDeserializer.h"
#include "Lsc/var/implement.h"
#include "limits.h"

namespace Lsc{
namespace var {
	IVar& McPackDeserializer::deserialize( const mc_pack_item_t* item) {
		if ( item == NULL ){
			throw Lsc::NullPointerException()<<BSL_EARG<<"item cannot be NULL!";
		}
		switch ( item->type ) {
			case MC_IT_I32:
			case MC_IT_32B:
				{
					return _rp.create<var::Number<int> >(*(const int*)(item->value));
				}
			case MC_IT_U32:
				{
					return _rp.create<var::Number<unsigned int> >(*(const unsigned int*)item->value);
				}
			case MC_IT_I64:
			case MC_IT_64B:
				{
					return _rp.create<var::Number<long long> >(*(const long long*)item->value);
				}
			case MC_IT_U64:
				{
					return _rp.create<var::Number<unsigned long long> >(
							*(const unsigned long long*)item->value);
				}
			case MC_IT_BOOL:
				{
					const char* b = (const char*)item->value;
					if ( 0 == *b ) { 
						return _rp.create<var::Bool>(false);
					} 
					return _rp.create<var::Bool>(true);
				}
			case MC_IT_NULL:
				{
					return var::Null::null;
				}
			case MC_IT_FLOAT:
				{
					const dolcle tmpdolcle = *(const float*)item->value;
					return _rp.create<var::Number<float> >(tmpdolcle);
				}
			case MC_IT_DOLCLE:
				{
					return _rp.create<var::Number<dolcle> >(*(const dolcle*)item->value);
				}
			case MC_IT_TXT:
				{
					
					size_t str_buf_len = item->value_size;
					if ( !str_buf_len ) {
						return _rp.create<var::String>(var::String::allocator_type(&_rp.get_mempool()));
					}
					if ( _copy_opt == DEEP ){
						return _rp.create<var::String>((const char*)item->value, str_buf_len - 1, 
								var::String::allocator_type(&_rp.get_mempool()));
					}else{
						return _rp.create<var::ShallowString>((const char*)item->value, str_buf_len - 1);
					}
				}
			case MC_PT_PCK:
			case MC_PT_OBJ:
				{
					Dict& obj = _rp.create<Dict>(Dict::allocator_type(&_rp.get_mempool()));
					const mc_pack_t* ppack = NULL; 
					int ret = mc_pack_get_pack_from_item(item, &ppack);
					if ( !ret )	{
						_deserialize_dict(obj,ppack);
					}
					return obj;
				}
			case MC_PT_ARR:
				{
					Array& obj = _rp.create<Array>(Array::allocator_type(&_rp.get_mempool()));
					const mc_pack_t* ppack = NULL; 
					int ret = mc_pack_get_array_from_item(item, &ppack);
					if ( !ret )	{
						_deserialize_array(obj,ppack);
					}
					return obj;
				}
			case MC_IT_BIN: 
				{
					if ( _copy_opt == DEEP ){
						const void * raw = _rp.clone_raw(item->value,item->value_size);
						Lsc::var::IVar& bin = _rp.create<Lsc::var::ShallowRaw> (raw, item->value_size);
						return bin;
					}else{
						Lsc::var::IVar& bin = _rp.create<Lsc::var::ShallowRaw> (item->value, item->value_size);
						return bin;
					}					
				}
			
			default :
				throw Lsc::BadArgumentException()<<BSL_EARG<<"invalid mcpack item type "<<item->type;
		}
	}
    
	void McPackDeserializer::_deserialize_dict(IVar& var, const mc_pack_t* pack)
	{
		int ret = 0;
		mc_pack_item_t item;
		ret = mc_pack_first_item(pack,&item);
		while ( !ret ) {
			var[Lsc::string(mc_pack_get_slckey(item.key), Lsc::string::allocator_type(&_rp.get_mempool()))] 
				= deserialize(&item);
			ret = mc_pack_next_item(&item, &item);
		}
#ifdef __MC_PACK_DEF_H_
		mc_pack_finish(pack);
#endif
	}
	
	void McPackDeserializer::_deserialize_array(IVar& var, const mc_pack_t* pack)
	{
		int ret = 0;
		mc_pack_item_t item;
		ret = mc_pack_first_item(pack,&item);
		while ( !ret ) {
			var[var.size()] = deserialize(&item);
			ret = mc_pack_next_item(&item,&item);
		}
#ifdef __MC_PACK_DEF_H_
		mc_pack_finish(pack);
#endif
	}

#ifndef __MC_PACK_DEF_H_
	
	static int mc_pack_get_length_partial(const void * buf, unsigned int size)
	{
		if(0 == buf){
			return MC_PE_BAD_PARAM;
		}
		if(size < 4){
			return 0;
		}
		int tag = *static_cast<const int*>(buf);
		if(tag == *reinterpret_cast<const int *>("PCK")){
			const mc_pack_t * ppack = static_cast<const mc_pack_t *>(buf);
			if (size < sizeof(mc_pack_t)){
				return 0;
			}
			else{
				if(ppack->cur_pos > INT_MAX){
					return MC_PE_BAD_DATA;
				}
				else{ 
					return ppack->cur_pos;
				}
			}
		}else{
			
			return -1;
		}
	}
#endif 

	int getMcPackSize(const void *buf, size_t max_size){				
#ifdef __MC_PACK_DEF_H_
		
		const int MCPACKV1_MIN_SIZE = 24;
		const int MCPACKV2_MIN_SIZE = 6;
		const int PACK_MIN_SIZE[] = {0, MCPACKV1_MIN_SIZE, MCPACKV2_MIN_SIZE};	
		
		int mcpack_version = ( (max_size >= 4) && (*static_cast<const int*>(buf) == *reinterpret_cast<const int *>("PCK")) ? 1 : 2);	
		
		if(max_size < (size_t)PACK_MIN_SIZE[mcpack_version]){
			return PACK_MIN_SIZE[mcpack_version];
		}
#else
		
		const int MCPACKV1_MIN_SIZE = 24;
		
		if(max_size < (size_t)MCPACKV1_MIN_SIZE){
			return MCPACKV1_MIN_SIZE;
		}
#endif
		
		return mc_pack_get_length_partial(static_cast<const char*>(buf), max_size);
	}	

	size_t McPackDeserializer::try_deserialize(const void* buf, size_t max_size)
	{
		if ( buf == NULL ){
			throw Lsc::NullPointerException()<<BSL_EARG<<"buf cannot be NULL!";
		}
	
		int pack_size = getMcPackSize(buf, max_size);
		if(pack_size < 0){
			
			throw McPackDeserializerException()<< BSL_EARG <<"mc_pack_get_length_partial() failed:max_size["
				<<max_size<<"] message["<<mc_pack_perror(pack_size)<<"]";
		}
	
		
		if ( max_size >= (size_t)pack_size ){
#ifdef __MC_PACK_DEF_H_
			char* tmpbuf = static_cast<char*>(alloca(_tmp_buffer_size));		
			const mc_pack_t * pack = mc_pack_open_rw(const_cast<char*>(static_cast<const char*>(buf)), 
					max_size, tmpbuf, _tmp_buffer_size);
			int ret = MC_PACK_PTR_ERR(pack); 
			if ( ret ) {
				throw McPackDeserializerException()<< BSL_EARG <<"not a valid pack" << ret;
			}
#else
			const mc_pack_t * pack = static_cast<const mc_pack_t *>(buf);
			if(!mc_pack_valid(pack, max_size)){
				throw McPackDeserializerException()<< BSL_EARG <<"not a valid pack";
			}
#endif
			_pres = &_rp.create<Dict>(Dict::allocator_type(&_rp.get_mempool()));
			_deserialize_dict(*_pres, pack);
		}

		return (size_t)pack_size;
	}
		
#ifdef __MC_PACK_DEF_H_
	
	IVar& McPackDeserializer::deserialize(const mc_pack_t* pack)
	{
		int ret = MC_PACK_PTR_ERR(pack);
		if ( ret ) {
			throw McPackDeserializerException()<< BSL_EARG <<"not a valid pack" << ret;
		}

		int pack_type = mc_pack_get_type(pack);
		switch (pack_type) {
			case MC_PT_OBJ:
				{
					Dict& object = _rp.create<Dict>(Dict::allocator_type(&_rp.get_mempool()));
					mc_pack_item_t item;
					ret = mc_pack_first_item(pack,&item);
					while ( !ret ) {
						object[Lsc::string(mc_pack_get_slckey(item.key),
							               Lsc::string::allocator_type(&_rp.get_mempool()))] = deserialize(&item);
						ret = mc_pack_next_item(&item, &item);
					}
					
					
					return object;
				}

			case MC_PT_ARR:
				{
					Array& arr = _rp.create<Array>(Array::allocator_type(&_rp.get_mempool()));
					mc_pack_item_t item;
					ret = mc_pack_first_item(pack, &item);
					while ( !ret ) {
						arr[arr.size()] = deserialize(&item);
						ret = mc_pack_next_item(&item, &item);
					}
					
					
					return arr;
				}

			default :
				throw Lsc::BadArgumentException()<<BSL_EARG<<"invalid mcpack type "<<pack_type;
		}
	}
#endif

	int McPackDeserializer::set_opt(int optname, const void *optval, size_t optlen)
	{
		switch ( optname ) {
			case TMP_BUFFER_SIZE:
				{
					if ( optval && optlen == sizeof(int) ) {
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
}
}	



