
#include "CompackDeserializer.h"
#include "Lsc/var/implement.h"

namespace Lsc{
namespace var {
	IVar& CompackDeserializer::_deserialize_value(const compack::buffer::Reader & reader,
			const compack::Iterator & it)
	{
		switch ( it.getType() ) {
			case compack::protocol::INT_8:
				return _rp.create<var::Number<signed char> >(*(const signed char*)(it.value()));
			case compack::protocol::INT_16:
				return _rp.create<var::Number<signed short> >(*(const short *)(it.value()));
			case compack::protocol::INT_32:
				return _rp.create<var::Number<signed int> >(*(const int *)(it.value()));
			case compack::protocol::INT_64:
				return _rp.create<var::Number<signed long long> >(*(const long long*)(it.value()));
			case compack::protocol::UINT_8:
				return _rp.create<var::Number<unsigned char> >(*(const unsigned char*)(it.value()));
			case compack::protocol::UINT_16:
				return _rp.create<var::Number<unsigned short> >(*(const unsigned short*)(it.value()));
			case compack::protocol::UINT_32:
				return _rp.create<var::Number<unsigned int> >(*(const unsigned int*)(it.value()));
			case compack::protocol::UINT_64:
				return _rp.create<var::Number<unsigned long long> >(*(const unsigned long long*)(it.value()));
			case compack::protocol::FLOAT:
				return _rp.create<var::Number<float> >(*(const float*)(it.value()));
			case compack::protocol::DOLCLE:
				return _rp.create<var::Number<dolcle> >(*(const dolcle*)(it.value()));
			case compack::protocol::BOOL:
				return _rp.create<var::Bool>(*(const bool *)it.value());
			case compack::protocol::MCPACK_NULL:
				return var::Null::null;
			case compack::protocol::BINARY:
			case compack::protocol::BINARY | compack::protocol::SHORT_ITEM:
				{
					if ( _copy_opt == DEEP ){
						const void * raw = _rp.clone_raw(it.value(), it.length());
						return _rp.create<Lsc::var::ShallowRaw> (raw, it.length());
					}else{
						return _rp.create<Lsc::var::ShallowRaw> (it.value(), it.length());
					}
					
				}
			case compack::protocol::STRING:
			case compack::protocol::STRING | compack::protocol::SHORT_ITEM:
				{
					size_t str_buf_len = it.length();
					if ( !str_buf_len ) {
						return _rp.create<var::String>(var::String::allocator_type(&_rp.get_mempool()));
					}
					else if ( _copy_opt == DEEP ){
						return _rp.create<var::String>((const char*)it.value(), str_buf_len - 1,
								var::String::allocator_type(&_rp.get_mempool()));
					}
					else{
						return _rp.create<var::ShallowString>((const char*)it.value(), str_buf_len - 1);
					}
				}
			case compack::protocol::OBJECT:
				{
					compack::buffer::Reader obj(reader, it);
					return _deserialize_dict(obj);
				}
			case compack::protocol::ARRAY:
			case compack::protocol::ISOARRAY:
				{
					compack::buffer::Reader obj(reader, it);
					return _deserialize_array(obj);
				}
			case compack::protocol::OBJECTISOARRAY:
				{
					compack::buffer::Reader obj(reader, it);
					return _deserialize_objectisoarray(obj);
				}
			default :
				throw Lsc::BadArgumentException()<<BSL_EARG<<"invalid mcpack item type "<<it.getType();
		}
	}
    
	IVar & CompackDeserializer::_deserialize_dict(compack::buffer::Reader &reader)
	{
		compack::ObjectIterator it;
		IVar& object = _rp.create<Dict>(Dict::allocator_type(&_rp.get_mempool()));
		while(reader.next(it)){
			object[Lsc::string(it.getName(), Lsc::string::allocator_type(&_rp.get_mempool()))] 
				= _deserialize_value(reader, it);
		}
		return object;
	}
	
	IVar & CompackDeserializer::_deserialize_array(compack::buffer::Reader &reader)
	{
		compack::Iterator it;
		IVar& var = _rp.create<Array>(Array::allocator_type(&_rp.get_mempool()));
		while(reader.next(it)){
			var[var.size()] = _deserialize_value(reader, it);
		}
		return var;
	}
	
	IVar & CompackDeserializer::_deserialize_objectisoarray(compack::buffer::Reader &reader)
	{
		IVar& arr = _rp.create<Array>(Array::allocator_type(&_rp.get_mempool()));
		unsigned int field = reader.count();
		if(field > 128){
			throw Lsc::OutOfBoundException()<<BSL_EARG<<"too many isoarray fields! found "<<field;
		}
		else if(field == 0){
			
			
			return arr;
		}
		
		
		compack::ObjectIterator field_iter;
		if ( !reader.next(field_iter) ){
			throw Lsc::OutOfBoundException()<<BSL_EARG<<"cannot fetch field 0/"<<field;
		}
		reader.reset();
		compack::buffer::Reader first_field( reader, field_iter );
		size_t size = first_field.count();
		Lsc::var::Dict* children = _rp.create_array<Lsc::var::Dict>(size, Lsc::var::Dict::allocator_type(&_rp.get_mempool()));
		for(size_t i = 0; i < size; ++i){
			arr.set( i, children[i] );
		}
		for(size_t f = 0; f < field; ++f){
			if(!reader.next(field_iter)){
				throw Lsc::OutOfBoundException()<<BSL_EARG<<"cannot fetch field  "<<f<<"/"<<field;
			}
			compack::buffer::Reader field_reader( reader, field_iter );
			Lsc::string key( field_iter.getName(), field_iter.getNameLength(), Lsc::string::allocator_type(&_rp.get_mempool()) );
			compack::Iterator child_iter;
			for( size_t i = 0; i < size; ++i ){
				if ( !field_reader.next(child_iter) ){
					throw Lsc::OutOfBoundException()<<BSL_EARG<<"cannot fetch item from field  "<<f<<"/"<<field<<" named "<<field_iter.getName();
				}
				Lsc::var::IVar& value = _deserialize_value( field_reader, child_iter );
				children[i].set( key, value );
			}
		}
		return arr;
	}
	
	size_t getCompackSize(const compack::buffer::buffer& buffer){
		size_t pack_size = 0;
		size_t buf_size = buffer.capacity();
        char type = buffer.read<unsigned char>(0);
        if(type & compack::protocol::DELETED_ITEM) {
            if(type & compack::protocol::FIXED_ITEM) {
				if(buf_size < sizeof(compack::protocol::short_item)){
					pack_size = sizeof(compack::protocol::short_item);
				}else{
					const compack::protocol::short_item & item = buffer.read<compack::protocol::short_item>(0);
					pack_size = item.size();
			    }
			}else if(type & compack::protocol::SHORT_ITEM) {
                if(buf_size < sizeof(compack::protocol::short_vitem)){
					pack_size = sizeof(compack::protocol::short_vitem);
				}else{
					const compack::protocol::short_vitem & item = buffer.read<compack::protocol::short_vitem>(0);
					pack_size = item.size();
				}				
			}else{
				if(buf_size < sizeof(compack::protocol::long_vitem)){
					pack_size = sizeof(compack::protocol::long_vitem);
				}else{	
					const compack::protocol::long_vitem & item = buffer.read<compack::protocol::long_vitem>(0);
					pack_size = item.size();
				}
			}
        }
		return pack_size;	
	}

	size_t CompackDeserializer::try_deserialize( const void* buf, size_t max_size)
	{
		if(buf == NULL){
			throw Lsc::NullPointerException() << BSL_EARG << "buf is NULL";
		}

		compack::buffer::buffer buffer(const_cast<void *>(buf), max_size);
		size_t pack_size = getCompackSize(buffer);
		if(pack_size == 0){
			
			throw Lsc::ParseErrorException() << BSL_EARG 
				<< "bad compack data, unknown compack item type, buffer size:"<<max_size;
		}
		if(max_size < pack_size){
			
			return pack_size;
		}

		compack::Iterator it = compack::buffer::Reader::getIterator(buffer);
		switch( it.getType() ){
			case compack::protocol::OBJECT:
				{
					compack::buffer::Reader reader(buffer);
					_pres = & _deserialize_dict(reader);
					break;
				}
			case compack::protocol::OBJECTISOARRAY: 
				{
					compack::buffer::Reader reader(buffer);
					_pres = & _deserialize_objectisoarray(reader);
					break;
				}
			case compack::protocol::ARRAY:
			case compack::protocol::ISOARRAY:
				{
					compack::buffer::Reader reader(buffer);
					_pres = & _deserialize_array(reader);
					break;
				}
			default:
				{
					_pres = & _deserialize_value(*(compack::buffer::Reader *)0, it);
					break;
				}
		}
		return pack_size;
	}

	int CompackDeserializer::set_opt(int optname, const void *, size_t)
	{
		switch ( optname ) {
			default:
				break;
		}
		return -1;
	}
}
}	




