
#include "BmlDeserializer.h"
#include "McPackDeserializer.h"
#include <Lsc/exception/Lsc_exception.h>
#include <Lsc/check_cast.h>
#include <Lsc/var/implement.h>
#include <Lsc/var/utils.h>

namespace Lsc{ namespace var {

    inline static size_t cstr_len( const char *cstr, size_t max_len ){
        size_t i = 0;
        for( i = 0; i < max_len; ++ i ){
            if ( cstr[i] == '\0' ){
                return i;
            }
        }
        return max_len;
    }

    inline unsigned long long BmlDeserializer::_deserialize_bits(size_t len){
        unsigned long long res = 0;
        size_t partial_len = 0;
        if ( _bit_idx + len > _bit_max ){
            
            partial_len = _bit_max - _bit_idx;
            len -= partial_len;
            res = ( _bit_buf >> _bit_idx ) & ( ( 1 << partial_len ) - 1 );   

            
            _bit_idx = 0;
            if ( _buf_cur + sizeof(unsigned long long) <= _buf_end ){
                
                _bit_buf = *reinterpret_cast<const unsigned long long*>(_buf_cur);
                _bit_max = 64; 
                _buf_cur += sizeof(unsigned long long);
            }else if ( _buf_cur + ((len + 7) >> 3) <= _buf_end ){
                xmemcpy( &_bit_buf, _buf_cur, _buf_end - _buf_cur );
                _bit_max = ( _buf_end - _buf_cur ) << 3;    
                _buf_cur = _buf_end;
            }else{
                
                _bit_buf = 0;
                _bit_max = 64;
                _buf_cur += sizeof(unsigned long long);
            }
        }

        
        
        res |= ( ( _bit_buf >> _bit_idx ) & ( ( 1 << len ) - 1 ) ) << partial_len;   
        _bit_idx += len;
        return res;

    }

    template<typename VarT>
    inline void BmlDeserializer::_deserialize_string(){

        
        _unrefill_bit_buf();

        
        if ( _modifier.mask & (MODIFIER_DEQUEUE|MODIFIER_LEN) ){
            
            if ( (_modifier.mask & (MODIFIER_LEN|MODIFIER_DEQUEUE)) == (MODIFIER_LEN|MODIFIER_DEQUEUE) ){
                if ( _modifier.dynamic_len != _modifier.static_len ){
                    throw Lsc::OutOfBoundException()<<BSL_EARG<<"_modifier.dynamic_len["<<_modifier.static_len<<"] != _modifier.static_len["<<_modifier.static_len<<"]";
                }
            }
            size_t n;
            if ( _modifier.mask & MODIFIER_DEQUEUE ){
                n = _modifier.dynamic_len;
            }else{
                n = _modifier.static_len;
            }
            Lsc::var::IVar& arr = _rp.template create<Lsc::var::Array>();

            if ( exist_len_queue(0) ){
                for( size_t i = 0; i < n; ++ i ){
                    if ( _modifier.mask & MODIFIER_ENQUEUE ){
                        if ( _buf_cur < _buf_end ){
                            
                            push_bml( _modifier.enqueue_id, _buf_cur );
                        }else{
                            
                            push_bml( _modifier.enqueue_id, "" );
                        }
                    }
                    size_t len = pop_len(0);
                    if ( ptrdiff_t( len + 1 ) < 0 ){
                        
                        throw Lsc::OverflowException()<<BSL_EARG<<"len["<<len<<"] too large";
                    }
                    if (  ptrdiff_t( len + 1 ) > _buf_end - _buf_cur  ){
                        
                        
                        if ( 0 == len_queue_size(0) ){
                            _buf_cur += size_t(n-i)*(len+1);
                            break;
                        }else{
                            _buf_cur += len + 1;
                        }

                    }else{
                        if ( _buf_cur[len] != '\0' ){
                            throw Lsc::ParseErrorException()<<BSL_EARG<<"expect '\\0' at pos["<<_buf_cur + len - _buf_begin<<"] find["<<_buf_cur[len]<<"]";
                        }
                        arr.set( i, _rp.template create<VarT>( _buf_cur, len ) );
                        _buf_cur += len + 1;
                    }
                }
            }else{
                
                for( size_t i = 0; i < n; ++ i ){
                    if ( _modifier.mask & MODIFIER_ENQUEUE ){
                        if ( _buf_cur < _buf_end ){
                            
                            push_bml( _modifier.enqueue_id, _buf_cur );
                        }else{
                            
                            push_bml( _modifier.enqueue_id, "" );
                        }
                    }
                    if ( _buf_cur < _buf_end ){
                        
                        size_t len = cstr_len( _buf_cur, _buf_end - _buf_cur );
                        if ( _buf_cur + len == _buf_end ){
                            
                            
                            
                            
                            _buf_cur = _buf_end + 1;    
                        }else{
                            arr.set( i, _rp.template create<VarT>( _buf_cur, len ) );
                            _buf_cur += len + 1;
                        }

                    }else{
                        
                        
                        _buf_cur += n - i;
                        break;
                    }
                }
            }
            _set_var( arr );

        }else{
            
            
            if ( _modifier.mask & MODIFIER_ENQUEUE ){
                if ( _buf_cur < _buf_end ){
                    
                    push_bml( _modifier.enqueue_id, _buf_cur );
                }else{
                    
                    push_bml( _modifier.enqueue_id, "" );
                }
            }

            
            size_t len = pop_len( 0 );
            if ( len != size_t(-1) ){
                if ( ptrdiff_t( len + 1 ) < 0 ){
                    
                    throw Lsc::OverflowException()<<BSL_EARG<<"len["<<len<<"] too large";
                }
                if (  ptrdiff_t(len + 1) > _buf_end - _buf_cur  ){
                    
                    
                    _buf_cur += len + 1;
                }else{
                    
                    if ( _buf_cur[len] != '\0' ){
                        throw Lsc::ParseErrorException()<<BSL_EARG<<"expect '\\0' at pos["<<_buf_cur + len - _buf_begin<<"] find["<<_buf_cur[len]<<"]";
                    }
                    _set_var( _rp.template create<VarT>( _buf_cur, len ) );
                    _buf_cur += len + 1;
                }
            }else{
                
                if ( _buf_cur < _buf_end ){
                    
                    len = cstr_len( _buf_cur, _buf_end - _buf_cur );
                }else{
                    
                    len = 0;
                }
                if ( _buf_cur + len >= _buf_end ){
                    
                    
                    
                    
                    _buf_cur += 1;
                }else{
                    
                    _set_var( _rp.template create<VarT>( _buf_cur, len ) );
                    _buf_cur += len + 1;
                }
            }
        }
    }

    inline void BmlDeserializer::_deserialize_raw(bool shallow_copy){
        
        _unrefill_bit_buf();
        if ( _modifier.mask & MODIFIER_DEREFERENCE ){
            
            if ( _modifier.mask & (MODIFIER_DEQUEUE|MODIFIER_LEN) ){
                if ( (_modifier.mask & (MODIFIER_DEQUEUE|MODIFIER_LEN) )==(MODIFIER_DEQUEUE|MODIFIER_LEN) ){
                    if ( _modifier.dynamic_len != _modifier.static_len ){
                        throw Lsc::OutOfBoundException()<<BSL_EARG<<"_modifier.dynamic_len["<<_modifier.dynamic_len<<"] != _modifier.static_len["<<_modifier.static_len<<"]";
                    }
                }
                size_t len = _modifier.mask & MODIFIER_DEQUEUE ? _modifier.dynamic_len : _modifier.static_len;
                Lsc::var::IVar& arr = _rp.create<Lsc::var::Array>();

                for( size_t i = 0; i < len; ++ i ){
                    const char *bml = pop_bml( _modifier.dereference_id );
                    if ( !bml ){
                        throw Lsc::OutOfBoundException()<<BSL_EARG<<"bml_queue["<<_modifier.dereference_id<<"] does not exist!";
                    }
                    BmlDeserializer deserializer(bml, _rp);
                    size_t size = _buf_cur < _buf_end ? _buf_end - _buf_cur : 0;
                    _buf_cur += deserializer.try_deserialize( _buf_cur, size );
                    IVar& res = deserializer.get_result();
                    arr.set( i, res ); 
                }

                _set_var( arr );

            }else{
                
                const char *bml = pop_bml( _modifier.dereference_id );
                if ( !bml ){
                    throw Lsc::OutOfBoundException()<<BSL_EARG<<"bml_queue["<<_modifier.dereference_id<<"] does not exist!";
                }
                BmlDeserializer deserializer(bml, _rp);
                _buf_cur += deserializer.try_deserialize( _buf_cur, _buf_end - _buf_cur );
                IVar& res = deserializer.get_result();
                _set_var( res ); 
            }
        }else if ( exist_len_queue(0) ){

            
            if ( _modifier.mask & (MODIFIER_DEQUEUE|MODIFIER_LEN) ){
                
                if ( (_modifier.mask & (MODIFIER_LEN|MODIFIER_DEQUEUE)) == (MODIFIER_LEN|MODIFIER_DEQUEUE) ){
                    if ( _modifier.dynamic_len != _modifier.static_len ){
                        throw Lsc::OutOfBoundException()<<BSL_EARG<<"_modifier.dynamic_len["<<_modifier.static_len<<"] != _modifier.static_len["<<_modifier.static_len<<"]";
                    }
                }
                Lsc::var::IVar& arr = _rp.create<Lsc::var::Array>();

                size_t n =  _modifier.mask & MODIFIER_DEQUEUE ? _modifier.dynamic_len : _modifier.static_len;
                for( size_t i = 0; i < n; ++ i ){
                    size_t len = pop_len(0);
                    if ( ptrdiff_t(len) < 0 ){
                        
                        throw Lsc::OverflowException()<<BSL_EARG<<"len["<<len<<"] too large";
                    }
                    if (  ptrdiff_t(len)  > _buf_end - _buf_cur ){
                        
                        
                        _buf_cur += len;
                    }else{
                        
                        const void * raw = shallow_copy ? _buf_cur : _rp.clone_raw( _buf_cur, len );
                        arr.set( i, _rp.create<Lsc::var::ShallowRaw>( raw, len ) );
                        _buf_cur += len;
                    }
                }
                _set_var( arr );

            }else{
                
                
                size_t len = pop_len(0);
                if ( ptrdiff_t(len) < 0 ){
                    
                    throw Lsc::OverflowException()<<BSL_EARG<<"len["<<len<<"] too large";
                }
                if ( ptrdiff_t(len) <= _buf_end - _buf_cur ){
                    
                    const void * raw = shallow_copy ? _buf_cur : _rp.clone_raw( _buf_cur, len );
                    _set_var( _rp.create<Lsc::var::ShallowRaw>( raw, len ) );
                    _buf_cur += len;
                }else{
                    
                    _buf_cur += len;
                }
            }
        }else{
            
            if ( _modifier.mask & (MODIFIER_LEN|MODIFIER_DEQUEUE) ){
                
                size_t len;
                if ( _modifier.mask & MODIFIER_DEQUEUE ){
                    if ( _modifier.mask & MODIFIER_LEN ){
                        if ( _modifier.dynamic_len != _modifier.static_len ){
                            throw Lsc::OutOfBoundException()<<BSL_EARG<<"_modifier.dynamic_len["<<_modifier.static_len<<"] != _modifier.static_len["<<_modifier.static_len<<"]";
                        }
                    }
                    len = _modifier.dynamic_len;
                }else{
                    len = _modifier.static_len;
                }
                Lsc::var::Array& arr = _rp.create<Lsc::var::Array>();
                if ( _buf_cur <= _buf_end ){
                    
                    if ( len != 0 ){
                        Lsc::var::ShallowRaw *var_arr = _rp.create_array<Lsc::var::ShallowRaw>(len);
                        Lsc::var::raw_t raw;
                        raw.data = shallow_copy ? _buf_cur : _rp.clone_raw( _buf_cur, _buf_end - _buf_cur );
                        raw.length = _buf_end - _buf_cur;
                        var_arr[0] = raw;
                        arr[0] = var_arr[0];
                        _buf_cur = _buf_end;
                        raw.data = NULL;
                        raw.length = 0;
                        for( size_t i = 1; i < len; ++ i ){
                            var_arr[i] = raw;
                            arr[i] = var_arr[i];
                        }
                    }
                }
                _set_var( arr );
            }else{
                
                if ( _buf_cur <= _buf_end ){
                    
                    const void * data = shallow_copy ? _buf_cur : _rp.clone_raw( _buf_cur, _buf_end - _buf_cur );
                    _set_var( _rp.create<Lsc::var::ShallowRaw>( data, _buf_end - _buf_cur ) );
                    _buf_cur = _buf_end;
                }
            }
        }
    }

    inline void BmlDeserializer::_deserialize_mcpack(){
        
        McPackDeserializer deserializer(_rp);
        if ( _copy_opt == DEEP ){
            deserializer.set_copy_option(McPackDeserializer::DEEP);
        }else{
            deserializer.set_copy_option(McPackDeserializer::SHALLOW);
        }

        
        _unrefill_bit_buf();
        if ( _modifier.mask & (MODIFIER_DEQUEUE|MODIFIER_LEN) ){
            if ( (_modifier.mask & (MODIFIER_DEQUEUE|MODIFIER_LEN) )==(MODIFIER_DEQUEUE|MODIFIER_LEN) ){
                if ( _modifier.dynamic_len != _modifier.static_len ){
                    throw Lsc::OutOfBoundException()<<BSL_EARG<<"_modifier.dynamic_len["<<_modifier.dynamic_len<<"] != _modifier.static_len["<<_modifier.static_len<<"]";
                }
            }
            size_t len = _modifier.mask & MODIFIER_DEQUEUE ? _modifier.dynamic_len : _modifier.static_len;
            Lsc::var::IVar& arr = _rp.create<Lsc::var::Array>();

            for( size_t i = 0; i < len; ++ i ){
                size_t size = _buf_cur < _buf_end ? _buf_end - _buf_cur : 0;
                size_t pack_len = deserializer.try_deserialize( _buf_cur, size );
                _buf_cur += pack_len;
                if ( _buf_cur <= _buf_end ){
                    IVar& res = deserializer.get_result();
                    arr.set( i, res );
                }
            }

            _set_var( arr );

        }else{
            
            size_t size = _buf_cur < _buf_end ? _buf_end - _buf_cur : 0;
            size_t pack_len = deserializer.try_deserialize( _buf_cur, size );
            _buf_cur += pack_len;
            if ( _buf_cur <= _buf_end ){
                IVar& res = deserializer.get_result();
                _set_var( res );
            }
        }
    }

    inline void BmlDeserializer::_unrefill_bit_buf(){
        _bit_idx = (_bit_idx+7) & -8;   
        _buf_cur -= (_bit_max - _bit_idx) >> 3; 
        _bit_max = _bit_idx;
    }

    inline long long BmlDeserializer::_deserialize_vint(){
        long long res = 0;
        size_t idx;
        for( idx = 0; idx < 63 && _buf_cur < _buf_end; idx += 7, ++ _buf_cur){ 
            res |= static_cast<long long>( *_buf_cur & 127 ) << idx; 
            if ( (*_buf_cur & 128) == 0 ){  
                ++ _buf_cur;
                res <<= (57 - idx); 
                res >>= (57 - idx); 
                return res;
            }
        }
        if ( _buf_cur < _buf_end ){
            
            if ( *_buf_cur == 0 ){          
                
                ++ _buf_cur;
                return res; 
            }else if ( *_buf_cur == 127 ){  
                
                ++ _buf_cur;
                res |= 1LL << 63;
                return res;
            }else{
                
                for( ; _buf_cur < _buf_end; ++ _buf_cur ){
                    if ( (*_buf_cur & 128) == 0 ){  
                        if ( (*_buf_cur & 64) == 0 ){   
                            throw Lsc::OverflowException()<<BSL_EARG<<"positive variable-length integer too large for Lsc::var::Int64, not support temporarily!";
                        }else{
                            throw Lsc::UnderflowException()<<BSL_EARG<<"negative variable-length integer too large for Lsc::var::Int64, not support temporarily!";
                        }
                    }
                }
                
                
                ++ _buf_cur;    
                return 0;
            }
        }else{
            
            
            ++ _buf_cur;    
            return 0;
        }
    }

    inline unsigned long long BmlDeserializer::_deserialize_uvint(){
        unsigned long long res = 0;
        size_t idx;
        for( idx = 0; idx < 63 && _buf_cur < _buf_end; idx += 7, ++ _buf_cur){ 
            res |= static_cast<unsigned long long>( *_buf_cur & 127 ) << idx; 
            if ( (*_buf_cur & 128) == 0 ){  
                ++ _buf_cur;
                return res;
            }
        }
        if ( _buf_cur < _buf_end ){
            if ( *_buf_cur == 1 ){
                
                ++ _buf_cur;
                return res | (1ULL << 63 );
            }else{  
                throw Lsc::OverflowException()<<BSL_EARG<<"variable-length integer too large for Lsc::var::Int64, not support temporarily!";
            }
        }else{
            
            
            ++ _buf_cur;
            return 0;
        }
    }

    inline bool BmlDeserializer::_need_deserialization(){
        if ( !_var_frame.pvar ){
            if ( _stack_size ){
                
                return false;
            }else{
                
                return true;
            }
        }else if ( _frame.struct_is_dict ){
            
            if ( _modifier.mask & MODIFIER_NAME ){
                return true;
            }else{
                
                return false;
            }
        }else{
            
            return true;
        }
    }
    
    inline void BmlDeserializer::_set_var(Lsc::var::IVar& var){
        if ( !_var_frame.pvar ){
            if ( _stack_size ){
                
                
            }else{
                
                _var_frame.pvar = &var;
            }
        }else if ( _frame.struct_is_dict ){
            
            if ( _modifier.mask & MODIFIER_NAME ){
                _var_frame.pvar->set(Lsc::string(_modifier.name, _modifier.name_len), var);
            }else{
                
                
            }
        }else{
            
            _var_frame.pvar->set(_frame.index, var);
        }
    }

    template<typename T>
    inline void BmlDeserializer::_enqueue( size_t len ){
        for( size_t i = 0; i < len; ++ i ){   
            T t = reinterpret_cast<const T *>(_buf_cur)[i];
            ssize_t ll = static_cast<ssize_t>(t);
            if ( ll < 0 ){
                throw Lsc::UnderflowException()<<BSL_EARG<<"length["<<t<<"] < 0";
            }
            push_len( _modifier.enqueue_id, static_cast<size_t>(t) );
        }
    }

    inline void BmlDeserializer::_dry_enqueue( size_t len ){
        for( size_t i = 0; i < len; ++ i ){   
            push_len( _modifier.enqueue_id, 0 );
        }
    }

    template<typename T, typename VarT>
        inline void BmlDeserializer::_deserialize_headword_number(){
            
            _unrefill_bit_buf();

            if ( _need_deserialization() ){
                
                if ( _modifier.mask & MODIFIER_DEQUEUE ){
                    Lsc::var::IVar& arr = _rp.template create<Lsc::var::Array>();
                    VarT * var_arr = _rp.template create_array<VarT>( _modifier.dynamic_len );
                    size_t add_len = _modifier.dynamic_len;

                    if ( _modifier.mask & MODIFIER_LEN ){
                        
                        if ( _modifier.static_len < _modifier.dynamic_len ){
                            throw Lsc::OutOfBoundException()<<BSL_EARG<<"_modifier.static_len["<<_modifier.static_len<<"] < _modifier.dynamic_len["<<_modifier.dynamic_len<<"] from queue["<<_modifier.dequeue_id<<"]";
                        }
                        add_len = _modifier.static_len;
                    }

                    
                    if (  ptrdiff_t(add_len * sizeof(T)) < 0){
                        throw Lsc::OverflowException()<<BSL_EARG<<"_modifier.static_len["<<_modifier.static_len<<"] too large";
                    }

                    if (  ptrdiff_t(add_len * sizeof(T))  > _buf_end - _buf_cur ){
                        
                        
                        _buf_cur += add_len * sizeof(T);
                        if ( _modifier.mask & MODIFIER_ENQUEUE ){
                            
                            _dry_enqueue( _modifier.dynamic_len );
                        }
                        return;
                    }

                    
                    if ( _modifier.mask & MODIFIER_ENQUEUE ){
                        _enqueue<T>( _modifier.dynamic_len );
                    }

                    
                    for( size_t i = 0; i < _modifier.dynamic_len; ++ i ){   
                        var_arr[i] = reinterpret_cast<const T *>(_buf_cur)[i];
                        arr.set( i, var_arr[i] );
                    }
                    _buf_cur += add_len * sizeof(T);    
                    _set_var( arr );

                }else if ( _modifier.mask & MODIFIER_LEN ){
                    
                    if ( ptrdiff_t(_modifier.static_len * sizeof(T)) < 0 ){
                        throw Lsc::OverflowException()<<BSL_EARG<<"_modifier.static_len["<<_modifier.static_len<<"] too large";
                    }
                    if (  ptrdiff_t(_modifier.static_len * sizeof(T))  > _buf_end - _buf_cur ){
                        
                        
                        _buf_cur += _modifier.static_len * sizeof(T);
                        if ( _modifier.mask & MODIFIER_ENQUEUE ){
                            
                            _dry_enqueue( _modifier.static_len );
                        }
                        return;
                    }
                    Lsc::var::IVar& arr = _rp.template create<Lsc::var::Array>();
                    VarT * var_arr = _rp.template create_array<VarT>( _modifier.static_len );

                    
                    if ( _modifier.mask & MODIFIER_ENQUEUE ){
                        _enqueue<T>( _modifier.static_len );
                    }

                    
                    for( size_t i = 0; i < _modifier.static_len; ++ i ){   
                        var_arr[i] = reinterpret_cast<const T *>(_buf_cur)[i];
                        arr.set( i, var_arr[i] );
                    }
                    _buf_cur += _modifier.static_len * sizeof(T);
                    _set_var( arr );

                }else{
                    
                    if (  ptrdiff_t(sizeof(T))  > _buf_end - _buf_cur ){
                        
                        
                        _buf_cur += sizeof(T);
                        if ( _modifier.mask & MODIFIER_ENQUEUE ){
                            
                            push_len( _modifier.enqueue_id, 0 );
                        }
                        return;
                    }
                    
                    if ( _modifier.mask & MODIFIER_ENQUEUE ){
                        _enqueue<T>(1);
                    }
                    
                    
                    T t = *reinterpret_cast<const T *>(_buf_cur);
                    _buf_cur += sizeof(T);
                    _set_var( _rp.template create<VarT>(t) );
                }
            }else{
                
                if ( _modifier.mask & MODIFIER_LEN ){
                    if ( _modifier.mask & MODIFIER_DEQUEUE ){
                        
                        if (  ptrdiff_t(_modifier.static_len * sizeof(T)) < 0){
                            throw Lsc::OverflowException()<<BSL_EARG<<"_modifier.static_len["<<_modifier.static_len<<"]";
                        }
                        if ( ptrdiff_t(_modifier.static_len * sizeof(T)) <= _buf_end - _buf_cur ){
                            
                            
                            if ( _modifier.mask & MODIFIER_ENQUEUE ){
                                _enqueue<T>( _modifier.dynamic_len );
                            }
                            _buf_cur += sizeof(T) * _modifier.static_len;

                        }else{
                            
                            if ( _modifier.mask & MODIFIER_ENQUEUE ){
                                
                                _dry_enqueue( _modifier.dynamic_len );
                            }
                            _buf_cur += sizeof(T) * _modifier.static_len;
                        }
                    }else{
                        
                        if (  ptrdiff_t(_modifier.static_len * sizeof(T)) < 0){
                            throw Lsc::OverflowException()<<BSL_EARG<<"_modifier.static_len["<<_modifier.static_len<<"]";
                        }
                        if ( ptrdiff_t(_modifier.static_len * sizeof(T)) <= _buf_end - _buf_cur ){
                            
                            
                            if ( _modifier.mask & MODIFIER_ENQUEUE ){
                                _enqueue<T>( _modifier.static_len );
                            }
                            _buf_cur += sizeof(T) * _modifier.static_len;
                        }else{
                            
                            if ( _modifier.mask & MODIFIER_ENQUEUE ){
                                
                                _dry_enqueue( _modifier.static_len );
                            }
                            _buf_cur += sizeof(T) * _modifier.static_len;
                        }
                    }
                }else if ( _modifier.mask & MODIFIER_DEQUEUE ){
                    
                    if (  ptrdiff_t(_modifier.dynamic_len * sizeof(T)) < 0){
                        throw Lsc::OverflowException()<<BSL_EARG<<"_modifier.dynamic_len["<<_modifier.dynamic_len<<"]";
                    }
                    if ( ptrdiff_t(_modifier.dynamic_len * sizeof(T)) <= _buf_end - _buf_cur ){
                        
                        
                        if ( _modifier.mask & MODIFIER_ENQUEUE ){
                            _enqueue<T>( _modifier.dynamic_len );
                        }
                        _buf_cur += sizeof(T) * _modifier.dynamic_len;
                    }else{
                        
                        if ( _modifier.mask & MODIFIER_ENQUEUE ){
                            
                            _dry_enqueue( _modifier.dynamic_len );
                        }
                        _buf_cur += sizeof(T) * _modifier.dynamic_len;
                    }
                }else{
                    
                    if ( ptrdiff_t(sizeof(T)) <= _buf_end - _buf_cur ){
                        
                        if ( _modifier.mask & MODIFIER_ENQUEUE ){
                            _enqueue<T>( 1 );
                        }
                        _buf_cur += sizeof(T);

                    }else{
                        
                        if ( _modifier.mask & MODIFIER_ENQUEUE ){
                            
                            _dry_enqueue( 1 );
                        }
                        _buf_cur += sizeof(T);
                    }
                }
            }
        }

    void BmlDeserializer::_deserialize_char( bool as_string ){
        
        _unrefill_bit_buf();

        
        if ( _modifier.mask & MODIFIER_ENQUEUE ){
            if ( _buf_cur <= _buf_end ){
                
                push_bml( _modifier.enqueue_id, _buf_cur );
            }else{
                
                push_bml( _modifier.enqueue_id, "" );
            }
        }

        if ( _modifier.mask & MODIFIER_DEQUEUE ){
            if ( _modifier.mask & MODIFIER_LEN ){
                
                if ( _modifier.dynamic_len > _modifier.static_len || _modifier.dynamic_len == _modifier.static_len && as_string ){
                    throw Lsc::OutOfBoundException()<<BSL_EARG<<"_modifier.static_len["<<_modifier.static_len<<"] <= _modifier.dynamic_len["<<_modifier.dynamic_len<<"] from queue["<<_modifier.dequeue_id<<"]";
                }
                if ( ptrdiff_t(_modifier.static_len) < 0 ){
                    throw Lsc::OverflowException()<<BSL_EARG<<"_modifier.static_len["<<_modifier.static_len<<"]";
                }
                if (  ptrdiff_t(_modifier.static_len)  > _buf_end - _buf_cur ){
                    
                    
                    _buf_cur += _modifier.static_len;
                    return;
                }

                
                if ( as_string ){
                    if( _buf_cur[_modifier.dynamic_len] != '\0' ){
                        throw Lsc::ParseErrorException()<<BSL_EARG<<"expect '\\0' at pos["<<_buf_cur + _modifier.dynamic_len - _buf_begin<<"] find["<<_buf_cur[_modifier.dynamic_len]<<"]";
                    }
                    if ( _copy_opt != DEEP ){
                        _set_var( _rp.create<Lsc::var::ShallowString>( _buf_cur, _modifier.dynamic_len ) );
                    }else{
                        _set_var( _rp.create<Lsc::var::String>( _buf_cur, _modifier.dynamic_len ) );
                    }
                }else{
                    if ( _copy_opt != DEEP ){
                        _set_var( _rp.create<Lsc::var::ShallowRaw>( _buf_cur, _modifier.dynamic_len ) );
                    }else{
                        const void * raw = _rp.clone_raw( _buf_cur, _modifier.dynamic_len );
                        _set_var( _rp.create<Lsc::var::ShallowRaw>( raw, _modifier.dynamic_len ) );
                    }
                }
                _buf_cur += _modifier.static_len;

            }else{
                
                if ( ptrdiff_t( _modifier.dynamic_len ) < 0 ){
                    throw Lsc::OverflowException()<<BSL_EARG<<"_modifier.dynamic_len["<<_modifier.dynamic_len<<"]";
                }
                if ( as_string ){
                    if (  ptrdiff_t(_modifier.dynamic_len)  >= _buf_end - _buf_cur ){
                        
                        
                        _buf_cur += _modifier.dynamic_len + 1;
                        return;
                    }

                    if( _buf_cur[_modifier.dynamic_len] != '\0' ){
                        throw Lsc::ParseErrorException()<<BSL_EARG<<"expect '\\0' at pos["<<_buf_cur + _modifier.dynamic_len - _buf_begin<<"] find["<<_buf_cur[_modifier.dynamic_len]<<"]";
                    }
                    if ( _copy_opt != DEEP ){
                        _set_var( _rp.create<Lsc::var::ShallowString>( _buf_cur, _modifier.dynamic_len ) );
                    }else{
                        _set_var( _rp.create<Lsc::var::String>( _buf_cur, _modifier.dynamic_len ) );
                    }
                    _buf_cur += _modifier.dynamic_len + 1;
                }else{
                    if (  ptrdiff_t(_modifier.dynamic_len)  > _buf_end - _buf_cur ){
                        
                        
                        _buf_cur += _modifier.dynamic_len;
                        return;
                    }
                    if ( _copy_opt != DEEP ){
                        _set_var( _rp.create<Lsc::var::ShallowRaw>( _buf_cur, _modifier.dynamic_len ) );
                    }else{
                        const void * raw = _rp.clone_raw( _buf_cur, _modifier.dynamic_len );
                        _set_var( _rp.create<Lsc::var::ShallowRaw>( raw, _modifier.dynamic_len ) );
                    }
                    _buf_cur += _modifier.dynamic_len;
                }
            }
        }else if ( _modifier.mask & MODIFIER_LEN ){
            
            if (  ptrdiff_t(_modifier.static_len) < 0){
                throw Lsc::OverflowException()<<BSL_EARG<<"_modifier.static_len["<<_modifier.static_len<<"]";
            }
            if (  ptrdiff_t(_modifier.static_len)  > _buf_end - _buf_cur ){
                
                
                _buf_cur += _modifier.static_len;
                return;
            }
            if ( as_string ){
                size_t len = cstr_len( _buf_cur, _modifier.static_len );
                if ( len == _modifier.static_len ){
                    
                    _set_var( _rp.create<Lsc::var::String>( _buf_cur, _modifier.static_len ) );
                }else{
                    
                    if ( _copy_opt != DEEP ){
                        _set_var( _rp.create<Lsc::var::ShallowString>( _buf_cur, len ) );
                    }else{
                        _set_var( _rp.create<Lsc::var::String>( _buf_cur, len ) );
                    }
                }
            }else{
                if ( _copy_opt != DEEP ){
                    _set_var( _rp.create<Lsc::var::ShallowRaw>( _buf_cur, _modifier.static_len ) );
                }else{
                    const void * raw = _rp.clone_raw( _buf_cur, _modifier.static_len );
                    _set_var( _rp.create<Lsc::var::ShallowRaw>( raw, _modifier.static_len ) );
                }
            }
            _buf_cur += _modifier.static_len;

        }else{
            
            
            if ( ptrdiff_t(sizeof(char)) <= _buf_end - _buf_cur  ){
                
                if ( as_string ){
                    _set_var( _rp.create<Lsc::var::String>( _buf_cur, 1 ) );
                }else{
                    if ( _copy_opt != DEEP ){
                        _set_var( _rp.create<Lsc::var::ShallowRaw>( _buf_cur, 1 ) );
                    }else{
                        char& ch = _rp.create<char>( *_buf_cur );
                        _set_var( _rp.create<Lsc::var::ShallowRaw>( &ch, 1 ) );
                    }
                }
                ++ _buf_cur;
            }else{
                
                ++ _buf_cur;
            }
        }
    }

    size_t BmlDeserializer::try_deserialize(const void* buf, size_t max_size){
        if ( !buf ){
            throw Lsc::NullPointerException()<<BSL_EARG<<"buf is NULL!";
        }

        
        clear_len_queues();
        clear_bml_queues();
        _var_stack.clear();
        _var_frame.pvar = NULL;
        _var_frame.struct_name = NULL;
        _var_frame.struct_name_len = 0;
        _buf_begin = _buf_cur = static_cast<const char *>(buf);
        _buf_end = _buf_cur + max_size;
        _bit_idx = 0;
        _bit_max = 0;
        _bit_buf = 0;
        parse(_bml.c_str());
        return _buf_cur - _buf_begin;
    }

    void BmlDeserializer::on_headword_lparen(){
        _unrefill_bit_buf();
        _var_stack.push_back(_var_frame);

        
        if ( _need_deserialization() ){
            if ( _frame.struct_is_dict ){
                
                _var_frame.struct_name = _modifier.name;
                _var_frame.struct_name_len = _modifier.name_len;
            }else{
                _var_frame.struct_name = NULL;
                _var_frame.struct_name_len = 0;
            }
            _var_frame.struct_begin = _buf_cur;
            if ( _modifier.mask & (MODIFIER_LEN|MODIFIER_DEQUEUE) ){
                
                if ( ( _modifier.mask & MODIFIER_LEN ) && (_modifier.mask & MODIFIER_DEQUEUE ) ){
                    
                    if ( _modifier.dynamic_len > _modifier.static_len ){
                        throw Lsc::OutOfBoundException()<<BSL_EARG<<"_modifier.static_len["<<_modifier.static_len<<"] < _modifier.dynamic_len["<<_modifier.dynamic_len<<"] from queue["<<_modifier.dequeue_id<<"]";
                    }
                }
                _var_frame.ploop_var = &_rp.create<Lsc::var::Array>();
            }else{
                _var_frame.ploop_var = NULL;
            }
            if ( _modifier.mask & MODIFIER_DICT ){
                _var_frame.pvar = &_rp.create<Lsc::var::Dict>();
            }else{
                _var_frame.pvar = &_rp.create<Lsc::var::Array>();
            }

        }else{
            
            _var_frame.ploop_var = NULL;
            _var_frame.pvar = NULL;
            _var_frame.struct_begin = _buf_cur;
            _var_frame.struct_name = NULL;
            _var_frame.struct_name_len = 0;
        }
    }

    void BmlDeserializer::on_headword_rparen(){
        
        _unrefill_bit_buf();

        
        if ( _frame.struct_index < _frame.struct_count - 1 ){
            
            if ( _var_frame.ploop_var ){
                
                _var_frame.ploop_var->set( _frame.struct_index, *_var_frame.pvar );
                
                
                if ( _frame.struct_is_dict ){
                    _var_frame.pvar = &_rp.create<Lsc::var::Dict>();
                }else{
                    _var_frame.pvar = &_rp.create<Lsc::var::Array>();
                }
            }else{
                _var_frame.pvar = NULL;
            }
            _var_frame.struct_begin  = _buf_cur;    

        }else{
            
            var_stack_node_t& parent_var_frame = _var_stack.back();
            stack_node_t& parent_frame = _stack.back();
            Lsc::var::IVar *pres = NULL;

            if ( _var_frame.ploop_var ){
                
                _var_frame.ploop_var->set( _frame.struct_index, *_var_frame.pvar );
                pres = _var_frame.ploop_var;
            }else{
                pres = _var_frame.pvar;
            }

            
            if ( pres ){
                
                if ( !parent_var_frame.pvar ){
                    if ( _stack_size - 1 ){ 
                        
                        
                    }else{
                        
                        parent_var_frame.pvar = pres;
                    }
                }else if ( parent_frame.struct_is_dict ){
                    
                    if ( _var_frame.struct_name ){
                        parent_var_frame.pvar->set(Lsc::string(_var_frame.struct_name, _var_frame.struct_name_len), *pres);
                    }else{
                        
                        
                    }
                }else{
                    
                    parent_var_frame.pvar->set(parent_frame.index, *pres);
                }
            }else{
                
                
            }

            
            _var_frame = parent_var_frame;
            _var_stack.pop_back();

            
            if ( _frame.struct_index < _frame.struct_static_count - 1 ){
                if ( _frame.vary_struct_size ){
                    throw Lsc::BadArgumentException()<<BSL_EARG<<"static size of the struct varies when dynamic_len < static_len";
                }else{
                    
                    size_t remain_size = ( _buf_cur - _var_frame.struct_begin ) 
                        * ( _frame.struct_static_count - _frame.struct_index - 1 );
                    _buf_cur += remain_size;
                }
            }

        }

    }
    void BmlDeserializer::on_headword_pad(){
        
        _unrefill_bit_buf();

        
        if ( _modifier.mask & MODIFIER_LEN ){
            _buf_cur += _modifier.static_len;
        }else if ( _modifier.mask & MODIFIER_DEQUEUE ){
            _buf_cur += _modifier.dynamic_len;
        }else{
            ++ _buf_cur;
        }

    }
    void BmlDeserializer::on_headword_align(){
        
        _unrefill_bit_buf();
        size_t buf_size = _buf_cur - _buf_begin;
        if ( _modifier.mask & MODIFIER_LEN ){
            buf_size = (buf_size + _modifier.static_len -1) & (-_modifier.static_len); 
            _buf_cur = _buf_begin + buf_size;
        }else{
            
        }
    }
    void BmlDeserializer::on_headword_u(){
        
        _unrefill_bit_buf();
        if ( _modifier.mask & MODIFIER_DEQUEUE ){
            
            Lsc::var::IVar& res = _rp.create<Lsc::var::Array>();
            res[ _modifier.dynamic_len - 1 ] = Lsc::var::Null::null; 
            _set_var( res );
        }else if ( _modifier.mask & MODIFIER_LEN ){
            
            Lsc::var::IVar& res = _rp.create<Lsc::var::Array>();
            res[ _modifier.static_len - 1 ] = Lsc::var::Null::null; 
            _set_var( res );
        }else{
            _set_var( Lsc::var::Null::null );
        }
    }

    void BmlDeserializer::on_headword_b(){
        unsigned long long num;
        if ( _modifier.mask & MODIFIER_LEN ){
            if ( _modifier.mask & MODIFIER_DEQUEUE ){
                
                if ( _modifier.dynamic_len > _modifier.static_len ){
                    throw Lsc::OutOfBoundException()<<BSL_EARG<<"dynamic_len["<<_modifier.dynamic_len<<"] > static_len["<<_modifier.static_len<<"]";
                }
            }
            num = _deserialize_bits( _modifier.static_len );

        }else if ( _modifier.mask & MODIFIER_DEQUEUE ){
            
            num = _deserialize_bits( _modifier.dynamic_len );
        }else{
            
            num = _deserialize_bits( 1 );
        }

        
        if ( _modifier.mask & MODIFIER_ENQUEUE ){
            if ( ssize_t(num) < 0 ){
                throw Lsc::UnderflowException()<<BSL_EARG<<"length["<<num<<"] < 0";
            }
            push_len( _modifier.enqueue_id, num );
        }

        
        if ( _buf_cur <= _buf_end ){
            if ( _modifier.mask & (MODIFIER_DEQUEUE|MODIFIER_LEN) ){
                _set_var( _rp.create<Lsc::var::Int64>(num) );
            }else{
                _set_var( _rp.create<Lsc::var::Bool>(num) );
            }
        }

    }

    void BmlDeserializer::on_headword_B(){
        _deserialize_headword_number<bool, Bool>();
    }
    void BmlDeserializer::on_headword_c(){
        _deserialize_char( true );
    }
    void BmlDeserializer::on_headword_C(){
        _deserialize_char( false );
    }
    void BmlDeserializer::on_headword_t(){
        _deserialize_headword_number<signed char,Number<signed char> >();
    }
    void BmlDeserializer::on_headword_T(){
        _deserialize_headword_number<unsigned char,Number<unsigned char> >();
    }
    void BmlDeserializer::on_headword_h(){
        _deserialize_headword_number<short,Number<short> >();
    }
    void BmlDeserializer::on_headword_H(){
        _deserialize_headword_number<unsigned short,Number<unsigned short> >();
    }
    void BmlDeserializer::on_headword_i(){
        _deserialize_headword_number<int,Number<int> >();
    }
    void BmlDeserializer::on_headword_I(){
        _deserialize_headword_number<unsigned int,Number<unsigned int> >();
    }
    void BmlDeserializer::on_headword_l(){
        _deserialize_headword_number<long long,Number<long long> >();
    }
    void BmlDeserializer::on_headword_L(){
        _deserialize_headword_number<unsigned long long,Number<unsigned long long> >();
    }
    void BmlDeserializer::on_headword_f(){
        _deserialize_headword_number<float,Number<float> >();
    }
    void BmlDeserializer::on_headword_d(){
        _deserialize_headword_number<dolcle,Number<dolcle> >();
    }
    void BmlDeserializer::on_headword_v(){

        
        _unrefill_bit_buf();
        if ( _modifier.mask & (MODIFIER_DEQUEUE|MODIFIER_LEN) ){
            if ( (_modifier.mask & (MODIFIER_LEN|MODIFIER_DEQUEUE)) == (MODIFIER_LEN|MODIFIER_DEQUEUE) ){
                
                if ( _modifier.dynamic_len != _modifier.static_len ){
                    throw Lsc::OutOfBoundException()<<BSL_EARG<<"_modifier.static_len["<<_modifier.static_len<<"] != _modifier.dynamic_len["<<_modifier.dynamic_len<<"] from queue["<<_modifier.dequeue_id<<"]";
                }
            }
            size_t len;
            if ( _modifier.mask & MODIFIER_DEQUEUE ){
                len = _modifier.dynamic_len;
            }else{
                len = _modifier.static_len;
            }
            if ( ptrdiff_t(len) < 0 ){
                
                throw Lsc::OverflowException()<<BSL_EARG<<"len["<<len<<"]";
            }
            if ( ptrdiff_t(len) > _buf_end - _buf_cur){
                
                
                _buf_cur += len;
                if ( _modifier.mask & MODIFIER_ENQUEUE ){
                    
                    _dry_enqueue( len );
                }
                return;
            }

            Lsc::var::IVar& arr = _rp.create<Lsc::var::Array>();
            Lsc::var::Int64 *var_arr = _rp.create_array<Lsc::var::Int64>( len );
            if ( _modifier.mask & MODIFIER_ENQUEUE ){
                
                for( size_t i = 0; i < len; ++ i ){
                    long long num = _deserialize_vint();
                    if ( _buf_cur <= _buf_end ){
                        
                        if ( ssize_t(num) < 0 ){
                            throw Lsc::UnderflowException()<<BSL_EARG<<"length["<<num<<"] < 0";
                        }
                        push_len( _modifier.enqueue_id, num );
                        var_arr[i] = num;
                        arr.set( i, var_arr[i] );
                    }else{
                        
                        _dry_enqueue( 1 );
                    }
                }
            }else{
                for( size_t i = 0; i < len; ++ i ){
                    long long num = _deserialize_vint();
                    if( _buf_cur <= _buf_end ){
                        var_arr[i] = num;
                        arr.set( i, var_arr[i] );
                    }else{
                        
                        
                    }
                }
            }
            _set_var( arr );

        }else{
            
            long long num = _deserialize_vint();
            if ( _buf_cur <= _buf_end ){
                
                if ( _modifier.mask & MODIFIER_ENQUEUE ){
                    if ( ssize_t(num) < 0 ){
                        throw Lsc::UnderflowException()<<BSL_EARG<<"length["<<num<<"] < 0";
                    }
                    push_len( _modifier.enqueue_id, num );
                }
                _set_var( _rp.create<Lsc::var::Int64>(num) );
            }else{
                
                if ( _modifier.mask & MODIFIER_ENQUEUE ){
                    
                    _dry_enqueue( 1 );
                }
            }
        }
    }

    void BmlDeserializer::on_headword_V(){

        
        _unrefill_bit_buf();
        if ( _modifier.mask & (MODIFIER_DEQUEUE|MODIFIER_LEN) ){
            if ( (_modifier.mask & (MODIFIER_LEN|MODIFIER_DEQUEUE)) == (MODIFIER_LEN|MODIFIER_DEQUEUE) ){
                
                if ( _modifier.dynamic_len != _modifier.static_len ){
                    throw Lsc::OutOfBoundException()<<BSL_EARG<<"_modifier.static_len["<<_modifier.static_len<<"] != _modifier.dynamic_len["<<_modifier.dynamic_len<<"] from queue["<<_modifier.dequeue_id<<"]";
                }
            }
            size_t len;
            if ( _modifier.mask & MODIFIER_DEQUEUE ){
                len = _modifier.dynamic_len;
            }else{
                len = _modifier.static_len;
            }
            if ( ptrdiff_t(len) < 0 ){
                throw Lsc::OverflowException()<<BSL_EARG<<"len["<<len<<"]";
            }
            if ( ptrdiff_t(len) > _buf_end - _buf_cur){
                
                
                _buf_cur += len;
                if ( _modifier.mask & MODIFIER_ENQUEUE ){
                    _dry_enqueue( len );
                }
                return;
            }

            Lsc::var::IVar& arr = _rp.create<Lsc::var::Array>();
            Lsc::var::Int64 *var_arr = _rp.create_array<Lsc::var::Int64>( len );
            if ( _modifier.mask & MODIFIER_ENQUEUE ){
                
                for( size_t i = 0; i < len; ++ i ){
                    unsigned long long num = _deserialize_uvint();
                    if ( _buf_cur <= _buf_end ){
                        
                        if ( ssize_t(num) < 0 ){
                            throw Lsc::UnderflowException()<<BSL_EARG<<"length["<<num<<"] < 0";
                        }
                        push_len( _modifier.enqueue_id, num );
                        var_arr[i] = check_cast<long long>(num);
                        arr.set( i, var_arr[i] );
                    }else{
                        
                        _dry_enqueue( 1 );
                    }
                }
            }else{
                for( size_t i = 0; i < len; ++ i ){
                    unsigned long long num = _deserialize_uvint();
                    if ( _buf_cur <= _buf_end ){
                        
                        var_arr[i] = check_cast<long long>(num);
                        arr.set( i, var_arr[i] );
                    }else{
                        
                        
                    }
                }
            }
            _set_var( arr );

        }else{
            
            unsigned long long num = _deserialize_uvint();
            if ( _buf_cur <= _buf_end ){
                
                if ( _modifier.mask & MODIFIER_ENQUEUE ){
                    if ( ssize_t(num) < 0 ){
                        throw Lsc::UnderflowException()<<BSL_EARG<<"length["<<num<<"] < 0";
                    }
                    push_len( _modifier.enqueue_id, num );
                }
                _set_var( _rp.create<Lsc::var::Int64>(check_cast<long long>(num)) );
            }else{
                
                if ( _modifier.mask & MODIFIER_ENQUEUE ){
                    
                    _dry_enqueue( 1 );
                }
            }
        }
    }

    void BmlDeserializer::on_headword_s(){
        if ( _copy_opt != SHALLOW ){
            _deserialize_string<Lsc::var::String>();
        }else{
            _deserialize_string<Lsc::var::ShallowString>();
        }
    }

    void BmlDeserializer::on_headword_S(){
        if ( _copy_opt == DEEP ){
            _deserialize_string<Lsc::var::String>();
        }else{
            _deserialize_string<Lsc::var::ShallowString>();
        }
    }

    void BmlDeserializer::on_headword_r(){
        if ( _copy_opt != SHALLOW ){
            _deserialize_raw(false);
        }else{
            _deserialize_raw(true);
        }
    }

    void BmlDeserializer::on_headword_R(){
        if ( _copy_opt == DEEP ){
            _deserialize_raw(false);
        }else{
            _deserialize_raw(true);
        }
    }

    void BmlDeserializer::on_headword_n(){
        
        size_t bits = ((_buf_cur - _buf_begin) << 3) - (_bit_max - _bit_idx);
        if ( _modifier.mask & MODIFIER_DEQUEUE ){
            
            Lsc::var::IVar& res = _rp.create<Lsc::var::Array>();
            Lsc::var::Int64 *arr= _rp.create_array<Lsc::var::Int64>( _modifier.dynamic_len, static_cast<long long>(bits) );
            for( size_t i = 0; i < _modifier.dynamic_len; ++ i ){
                res[i] = arr[i];
            }
            _set_var( res );
        }else if ( _modifier.mask & MODIFIER_LEN ){
            
            Lsc::var::IVar& res = _rp.create<Lsc::var::Array>();
            Lsc::var::Int64 *arr= _rp.create_array<Lsc::var::Int64>( _modifier.static_len, static_cast<long long>(bits) );
            for( size_t i = 0; i < _modifier.static_len; ++ i ){
                res[i] = arr[i];
            }
            _set_var( res );
        }else{
            _set_var( _rp.create<Lsc::var::Int64>(bits) );
        }
    }

    void BmlDeserializer::on_headword_N(){
        
        size_t bytes = (_buf_cur - _buf_begin) - ((_bit_max - _bit_idx) >> 3 );
        if ( _modifier.mask & MODIFIER_DEQUEUE ){
            
            Lsc::var::IVar& res = _rp.create<Lsc::var::Array>();
            Lsc::var::Int64 *arr= _rp.create_array<Lsc::var::Int64>( _modifier.dynamic_len, static_cast<long long>(bytes) );
            for( size_t i = 0; i < _modifier.dynamic_len; ++ i ){
                res[i] = arr[i];
            }
            _set_var(res);
        }else if ( _modifier.mask & MODIFIER_LEN ){
            
            Lsc::var::IVar& res = _rp.create<Lsc::var::Array>();
            Lsc::var::Int64 *arr= _rp.create_array<Lsc::var::Int64>( _modifier.static_len, static_cast<long long>(bytes) );
            for( size_t i = 0; i < _modifier.static_len; ++ i ){
                res[i] = arr[i];
            }
            _set_var(res);
        }else{
            _set_var( _rp.create<Lsc::var::Int64>(bytes) );
        }
    }

    size_t BmlDeserializer::on_unknown_modifier(const char *bml){
        
        throw Lsc::ParseErrorException()<<BSL_EARG<<"unexpected modifier["<<*bml<<"]";
    }
    size_t BmlDeserializer::on_unknown_headword(const char *bml){
        
        throw Lsc::ParseErrorException()<<BSL_EARG<<"unexpected headword["<<*bml<<"]";
    }
} }  


