
#include "BmlSerializer.h"
#include "McPackSerializer.h"
#include <Lsc/exception/Lsc_exception.h>
#include <Lsc/check_cast.h>
#include <Lsc/var/Null.h>
#include <Lsc/var/utils.h>

namespace Lsc{ namespace var {

    inline void BmlSerializer::_serialize_bits(){
        if ( _bit_idx ){
            _bit_idx = (_bit_idx+7) & -8;   
            size_t bytes = _bit_idx >> 3;
            if (  bytes  > size_t( _buf_end - _buf_cur ) ){
                throw Lsc::OutOfBoundException()<<BSL_EARG<<"buffer too small!";
            }
            Lsc::xmemcpy( _buf_cur, &_bit_buf, bytes );
            _bit_idx = _bit_buf = 0;
            _buf_cur += bytes;
        }
    }

    inline void BmlSerializer::_serialize_vint(long long num){
        if (  10  > size_t( _buf_end - _buf_cur ) ){ 
            throw Lsc::OutOfBoundException()<<BSL_EARG<<"buffer too small!";
        }
        if ( num >= 0 ){
            while(true){
                *_buf_cur = num & 127; 
                num >>= 7;
                if ( num != 0){
                    *_buf_cur++ |= 128; 
                }else{
                    if ( (*_buf_cur) & 64 ){    
                        
                        *_buf_cur++ |= 128;       
                        *_buf_cur++ = 0;        
                    }else{
                        ++ _buf_cur;
                    }
                    return;
                }
            }
        }else{
            while(true){
                *_buf_cur = num & 127; 
                num >>= 7;
                if ( num != -1 ){
                    *_buf_cur++ |= 128; 
                }else{
                    if ( ((*_buf_cur) & 64) == 0 ){ 
                        
                        *_buf_cur++ |= 128;       
                        *_buf_cur++ = 127 ;       
                    }else{
                        ++ _buf_cur;
                    }
                    return;
                }
            }
        }
    }

    inline void BmlSerializer::_serialize_uvint(unsigned long long num){
        if (  10  > size_t( _buf_end - _buf_cur ) ){ 
            throw Lsc::OutOfBoundException()<<BSL_EARG<<"buffer too small!";
        }
        while(true){
            *_buf_cur = num & 127; 
            num >>= 7;
            if ( num != 0){
                *_buf_cur++ |= 128; 
            }else{
                ++ _buf_cur;
                return;
            }
        }
    }

    inline const Lsc::var::IVar* BmlSerializer::_get_pvar(){
        if ( !_var_frame.pvar ){
            
            return NULL;
        }
        if ( _frame.struct_is_dict ){
            
            if ( _modifier.mask & MODIFIER_NAME ){
                return &_var_frame.pvar->get(Lsc::string(_modifier.name, _modifier.name_len));
            }else{
                
                return NULL;
            }
        }else if ( _stack_size ){
            
            return &_var_frame.pvar->get(_frame.index);
        }else{
            
            return _var_frame.pvar;
        }

    }
    
    template<typename T, typename R>
        inline void BmlSerializer::_serialize_headword_common( R (Lsc::var::IVar::* pmethod)() const ){
            
            const IVar* pvar = _get_pvar();

            
            _serialize_bits();

            
            if ( _modifier.mask & MODIFIER_DEQUEUE ){
                size_t var_size = pvar ? pvar->size() : 0;
                if ( var_size < _modifier.dynamic_len ){
                    throw Lsc::OutOfBoundException()<<BSL_EARG<<"var_size["<<var_size<<"] < _modifier.dynamic_len["<<_modifier.dynamic_len<<"] from queue["<<_modifier.dequeue_id<<"]";
                }
                if ( _modifier.mask & MODIFIER_LEN ){
                    
                    if ( _modifier.static_len < _modifier.dynamic_len ){
                        throw Lsc::OutOfBoundException()<<BSL_EARG<<"_modifier.static_len["<<_modifier.static_len<<"] < _modifier.dynamic_len["<<_modifier.dynamic_len<<"] from queue["<<_modifier.dequeue_id<<"]";
                    }
                    if (  _modifier.static_len * sizeof(T)  > size_t( _buf_end - _buf_cur ) ){
                        throw Lsc::OutOfBoundException()<<BSL_EARG<<"buffer too small!";
                    }
                    
                    if ( _modifier.mask & MODIFIER_ENQUEUE ){
                        
                        for( size_t i = 0; i < _modifier.dynamic_len; ++ i ){   
                            T t = (pvar->get(i).*pmethod)();
                            ssize_t ll = static_cast<ssize_t>(t);
                            if ( ll < 0 ){
                                throw Lsc::UnderflowException()<<BSL_EARG<<"length["<<t<<"] < 0";
                            }
                            push_len( _modifier.enqueue_id, static_cast<size_t>(t) );
                            reinterpret_cast<T *>(_buf_cur)[i] = t;
                        }
                    }else{
                        
                        for( size_t i = 0; i < _modifier.dynamic_len; ++ i ){   
                            reinterpret_cast<T *>(_buf_cur)[i] = (pvar->get(i).*pmethod)();
                        }
                    }
                    _buf_cur += _modifier.static_len * sizeof(T);    
                }else{
                    
                    if (  _modifier.dynamic_len * sizeof(T)  > size_t( _buf_end - _buf_cur ) ){
                        throw Lsc::OutOfBoundException()<<BSL_EARG<<"buffer too small!";
                    }

                    if ( _modifier.mask & MODIFIER_ENQUEUE ){
                        for( size_t i = 0; i < _modifier.dynamic_len; ++ i ){   
                            T t = (pvar->get(i).*pmethod)();
                            ssize_t ll = static_cast<ssize_t>(t);
                            if ( ll < 0 ){
                                throw Lsc::UnderflowException()<<BSL_EARG<<"length["<<t<<"] < 0";
                            }
                            push_len( _modifier.enqueue_id, static_cast<size_t>(t) );
                            reinterpret_cast<T *>(_buf_cur)[i] = t;
                        }
                    }else{
                        for( size_t i = 0; i < _modifier.dynamic_len; ++ i ){   
                            reinterpret_cast<T *>(_buf_cur)[i] = (pvar->get(i).*pmethod)();
                        }
                    }
                    _buf_cur += _modifier.dynamic_len * sizeof(T);
                }

            }else if ( _modifier.mask & MODIFIER_LEN ){
                
                size_t var_size = pvar ? pvar->size() : 0;
                size_t n = std::min( var_size, _modifier.static_len );
                if (  _modifier.static_len * sizeof(T)  > size_t( _buf_end - _buf_cur ) ){
                    throw Lsc::OutOfBoundException()<<BSL_EARG<<"buffer too small!";
                }
                if ( _modifier.mask & MODIFIER_ENQUEUE ){
                    for( size_t i = 0; i < n; ++ i ){   
                        T t = (pvar->get(i).*pmethod)();
                        ssize_t ll = static_cast<ssize_t>(t);
                        if ( ll < 0 ){
                            throw Lsc::UnderflowException()<<BSL_EARG<<"length["<<t<<"] < 0";
                        }
                        push_len( _modifier.enqueue_id, static_cast<size_t>(t) );
                        reinterpret_cast<T *>(_buf_cur)[i] = t;
                    }
                    if ( var_size < _modifier.static_len ){
                        for( size_t i = var_size; i < _modifier.static_len; ++ i ){
                            push_len( _modifier.enqueue_id, static_cast<size_t>(0) );
                        }
                        memset( _buf_cur + var_size * sizeof(T), 0, (_modifier.static_len - var_size) * sizeof(T) ); 
                    }
                }else{
                    for( size_t i = 0; i < n; ++ i){
                        reinterpret_cast<T *>(_buf_cur)[i] = (pvar->get(i).*pmethod)();
                    }
                    if ( var_size < _modifier.static_len ){
                        memset( _buf_cur + var_size * sizeof(T), 0, (_modifier.static_len - var_size) * sizeof(T) ); 
                    }
                }
                _buf_cur += _modifier.static_len * sizeof(T);
            }else{
                
                if (  sizeof(T)  > size_t( _buf_end - _buf_cur ) ){
                    throw Lsc::OutOfBoundException()<<BSL_EARG<<"buffer too small!";
                }
                T t = pvar ? check_cast<T>((pvar->*pmethod)()) : 0;
                if ( _modifier.mask & MODIFIER_ENQUEUE ){
                    
                    ssize_t ll = static_cast<ssize_t>(t);
                    if ( ll < 0 ){
                        throw Lsc::UnderflowException()<<BSL_EARG<<"length["<<t<<"] < 0";
                    }
                    push_len( _modifier.enqueue_id, static_cast<size_t>(t) );
                }
                *reinterpret_cast<T *>(_buf_cur) = t;
                _buf_cur += sizeof(T);
            }

        }

    void BmlSerializer::_serialize_char( bool append_trailing_zero ){
        
        _serialize_bits();

        
        if ( _modifier.mask & MODIFIER_ENQUEUE ){
            push_bml( _modifier.enqueue_id, _buf_cur );
        }

        
        const IVar* pvar = _get_pvar();
        const char * cstr;
        size_t cstr_len;

        if ( pvar ){
            if ( pvar->is_string() ){
                cstr = pvar->c_str();
                cstr_len = pvar->c_str_len();
            }else{
                raw_t raw = pvar->to_raw();
                cstr = static_cast<const char*>(raw.data);
                cstr_len = raw.length;
            }
        }else{
            cstr = "";
            cstr_len = 0;
        }

        
        if ( _modifier.mask & MODIFIER_DEQUEUE ){
            if ( cstr_len < _modifier.dynamic_len ){
                throw Lsc::OutOfBoundException()<<BSL_EARG<<"cstr_len["<<cstr_len<<"] < _modifier.dynamic_len["<<_modifier.dynamic_len<<"] from queue["<<_modifier.dequeue_id<<"]";
            }
            if ( _modifier.mask & MODIFIER_LEN ){
                
                if ( _modifier.static_len < _modifier.dynamic_len || (_modifier.static_len == _modifier.dynamic_len && append_trailing_zero ) ){
                    throw Lsc::OutOfBoundException()<<BSL_EARG<<"_modifier.static_len["<<_modifier.static_len<<"] <= _modifier.dynamic_len["<<_modifier.dynamic_len<<"] from queue["<<_modifier.dequeue_id<<"]";
                }
                if (  _modifier.static_len  > size_t( _buf_end - _buf_cur ) ){
                    throw Lsc::OutOfBoundException()<<BSL_EARG<<"buffer too small!";
                }
                xmemcpy( _buf_cur, cstr, _modifier.dynamic_len );
                if ( append_trailing_zero ){
                    _buf_cur[_modifier.dynamic_len] = '\0';
                }
                _buf_cur += _modifier.static_len;
            }else{
                
                if ( append_trailing_zero ){
                    if ( _modifier.dynamic_len + 1 > size_t( _buf_end - _buf_cur ) ){
                        throw Lsc::OutOfBoundException()<<BSL_EARG<<"buffer too small!";
                    }
                    xmemcpy( _buf_cur, cstr, _modifier.dynamic_len );
                    _buf_cur[_modifier.dynamic_len] = '\0';
                    _buf_cur += _modifier.dynamic_len + 1;
                }else{
                    if ( _modifier.dynamic_len > size_t( _buf_end - _buf_cur ) ){
                        throw Lsc::OutOfBoundException()<<BSL_EARG<<"buffer too small!";
                    }
                    xmemcpy( _buf_cur, cstr, _modifier.dynamic_len );
                    _buf_cur += _modifier.dynamic_len;
                }
            }

        }else if ( _modifier.mask & MODIFIER_LEN ){
            
            if (  _modifier.static_len  > size_t( _buf_end - _buf_cur ) ){
                throw Lsc::OutOfBoundException()<<BSL_EARG<<"buffer too small!";
            }
            if ( cstr_len < _modifier.static_len ){
                xmemcpy( _buf_cur, cstr, cstr_len );
                _buf_cur[cstr_len] = '\0';  
            }else{
                
                if ( append_trailing_zero ){
                    xmemcpy( _buf_cur, cstr, _modifier.static_len - 1 );
                    _buf_cur[_modifier.static_len -1] = '\0';
                }else{
                    xmemcpy( _buf_cur, cstr, _modifier.static_len );
                }
            }
            _buf_cur += _modifier.static_len;
        }else{
            
            if ( _buf_cur == _buf_end ){
                throw Lsc::OutOfBoundException()<<BSL_EARG<<"buffer too small!";
            }
            *_buf_cur = cstr[0];
            ++ _buf_cur;
        }

    }

    void BmlSerializer::_serialize_mcpack(int version){
        
        McPackSerializer serializer;
        serializer.set_throw_on_mcpack_error(true);
        serializer.set_opt( MCPACK_VERSION, &version, sizeof(version) );

        
        const IVar* pvar = _get_pvar();

        
        _serialize_bits();

        
        if ( _modifier.mask & (MODIFIER_DEQUEUE|MODIFIER_LEN) ){
            
            if ( (_modifier.mask & (MODIFIER_DEQUEUE|MODIFIER_LEN) )==(MODIFIER_DEQUEUE|MODIFIER_LEN) ){
                
                if ( _modifier.dynamic_len != _modifier.static_len ){
                    throw Lsc::OutOfBoundException()<<BSL_EARG<<"_modifier.dynamic_len["<<_modifier.dynamic_len<<"] != _modifier.static_len["<<_modifier.static_len<<"]";
                }
            }
            size_t len = _modifier.mask & MODIFIER_DEQUEUE ? _modifier.dynamic_len : _modifier.static_len;
            for( size_t i = 0; i < len; ++ i ){   
                size_t res = serializer.serialize(pvar->get(i), _buf_cur, size_t( _buf_end - _buf_cur ) ); 
                _buf_cur += res;
            }
        }else{
            
            size_t res = serializer.serialize(*pvar, _buf_cur, size_t( _buf_end - _buf_cur ) ); 
            _buf_cur += res;
        }
    }

    size_t BmlSerializer::serialize(const IVar& var, void* buf, size_t max_size){
        if ( !buf ){
            throw Lsc::NullPointerException()<<BSL_EARG<<"buf is NULL!";
        }

        
        clear_len_queues();
        clear_bml_queues();
        _var_stack.clear();
        _var_frame.pvar = &var;
        _buf_begin = _buf_cur = static_cast<char *>(buf);
        _buf_end = _buf_cur + max_size;
        _bit_idx = 0;
        _bit_buf = 0;
        parse(_bml.c_str());
        _serialize_bits();
        return _buf_cur - _buf_begin;
    }

    void BmlSerializer::on_headword_lparen(){
        
        const IVar* pvar = _get_pvar();

        
        _serialize_bits();

        
        _var_stack.push_back(_var_frame);
        if ( _modifier.mask & MODIFIER_DEQUEUE ){
            size_t var_size = pvar ? pvar->size() : 0;
            if ( _modifier.dynamic_len > var_size ){
                throw Lsc::OutOfBoundException()<<BSL_EARG<<"var_size["<<var_size<<"] < _modifier.dynamic_len["<<_modifier.dynamic_len<<"] from queue["<<_modifier.dequeue_id<<"]";
            }
            if ( _modifier.mask & MODIFIER_LEN ){
                
                if ( _modifier.dynamic_len > _modifier.static_len ){
                    throw Lsc::OutOfBoundException()<<BSL_EARG<<"_modifier.static_len["<<_modifier.static_len<<"] < _modifier.dynamic_len["<<_modifier.dynamic_len<<"] from queue["<<_modifier.dequeue_id<<"]";
                }
                _var_frame.ploop_var = pvar;
                _var_frame.pvar = pvar ? &pvar->get(0) : NULL;
            }
        }else if ( _modifier.mask & MODIFIER_LEN ){
            if ( pvar ){
                _var_frame.ploop_var = pvar;
                _var_frame.pvar = &pvar->get(0);
            }else{
                
                _var_frame.ploop_var = NULL;
                _var_frame.pvar = NULL;
            }
        }else{
            
            _var_frame.ploop_var = NULL;
            _var_frame.pvar = pvar;
        }

        _var_frame.struct_begin = _buf_cur;
    }

    void BmlSerializer::on_headword_rparen(){
        
        _serialize_bits();

        
        if ( _frame.struct_index < _frame.struct_count - 1 ){
            
            _var_frame.pvar = _var_frame.ploop_var ? &_var_frame.ploop_var->get(_frame.struct_index + 1) : NULL;
            _var_frame.struct_begin  = _buf_cur;    

        }else{
            
            if ( _frame.struct_index < _frame.struct_static_count - 1 ){
                if ( _frame.vary_struct_size ){
                    throw Lsc::OutOfBoundException()<<BSL_EARG<<"static size of the struct varies when dynamic_len < static_len";
                }else{
                    
                    size_t remain_size = ( _buf_cur - _var_frame.struct_begin ) 
                        * ( _frame.struct_static_count - _frame.struct_index - 1 );
                    if (  remain_size  > size_t( _buf_end - _buf_cur ) ){
                        throw Lsc::OutOfBoundException()<<BSL_EARG<<"buffer too small!";
                    }
                    memset( _buf_cur, 0, remain_size );
                    _buf_cur += remain_size;
                }
            }

            
            _var_frame = _var_stack.back();
            _var_stack.pop_back();

        }

    }
    void BmlSerializer::on_headword_pad(){
        
        _serialize_bits();

        
        if ( _modifier.mask & MODIFIER_LEN ){
            _buf_cur += _modifier.static_len;
        }else if ( _modifier.mask & MODIFIER_DEQUEUE ){
            _buf_cur += _modifier.dynamic_len;
        }else{
            ++ _buf_cur;
        }

    }
    void BmlSerializer::on_headword_align(){
        
        _serialize_bits();
        size_t buf_size = _buf_cur - _buf_begin;
        if ( _modifier.mask & MODIFIER_LEN ){
            buf_size = (buf_size + _modifier.static_len -1) & (-_modifier.static_len); 
            _buf_cur = _buf_begin + buf_size;
        }else{
            
        }
    }
    void BmlSerializer::on_headword_u(){
        
    }
    void BmlSerializer::on_headword_b(){
        
        const IVar* pvar = _get_pvar();
        size_t len;
        long long num;

        
        if ( _modifier.mask & MODIFIER_LEN ){
            if ( _modifier.mask & MODIFIER_DEQUEUE ){
                
                if ( _modifier.dynamic_len > _modifier.static_len ){
                    throw Lsc::OutOfBoundException()<<BSL_EARG<<"dynamic_len["<<_modifier.dynamic_len<<"] > static_len["<<_modifier.static_len<<"]";
                }
                len = _modifier.dynamic_len;
            }else{
                
                len = _modifier.static_len;
            }
            num = pvar ? pvar->to_int64() : 0;
        } else if ( _modifier.mask & MODIFIER_DEQUEUE ){
            
            len = _modifier.dynamic_len;
            num = pvar ? pvar->to_int64() : 0;
        }else{
            
            
            len = 1;
            num = pvar ? pvar->to_bool() : 0;
        }
        if ( len >= 64 ){  
            throw Lsc::OverflowException()<<BSL_EARG<<len<<" bits is not supported";
        }

        
        unsigned long long bit_num= num & ((1ULL<<len) - 1); 
        if ( bit_num != static_cast<unsigned long long>(num) ){
            
            throw Lsc::OverflowException()<<BSL_EARG<<"try to fit "<<num<<" into "<<len<<" bits";
        }

        
        if ( _modifier.mask & MODIFIER_ENQUEUE ){
            push_len( _modifier.enqueue_id, static_cast<size_t>(num) );
        }

        
        if ( _bit_idx + len < 64 ){   
            
            _bit_buf |= bit_num << _bit_idx;
            _bit_idx += len;
        }else{
            
            _bit_buf |= bit_num << _bit_idx;
            if (  sizeof(unsigned long long)  > size_t( _buf_end - _buf_cur ) ){   
                throw Lsc::OutOfBoundException()<<BSL_EARG<<"buffer too small!"; 
            }
            *reinterpret_cast<unsigned long long*>(_buf_cur) = _bit_buf;
            _buf_cur += sizeof(unsigned long long);
            _bit_buf = bit_num >> (64 - _bit_idx);
            _bit_idx = len + _bit_idx - 64;
        }

    }
    void BmlSerializer::on_headword_B(){
        _serialize_headword_common<bool>(&Lsc::var::IVar::to_bool);
    }
    void BmlSerializer::on_headword_c(){
        _serialize_char( true );
    }
    void BmlSerializer::on_headword_C(){
        _serialize_char( false );
    }
    void BmlSerializer::on_headword_t(){
        _serialize_headword_common<signed char>(&Lsc::var::IVar::to_int8);
    }
    void BmlSerializer::on_headword_T(){
        _serialize_headword_common<unsigned char>(&Lsc::var::IVar::to_uint8);
    }
    void BmlSerializer::on_headword_h(){
        _serialize_headword_common<short>(&Lsc::var::IVar::to_int16);
    }
    void BmlSerializer::on_headword_H(){
        _serialize_headword_common<unsigned short>(&Lsc::var::IVar::to_uint16);
    }
    void BmlSerializer::on_headword_i(){
        _serialize_headword_common<int>(&Lsc::var::IVar::to_int32);
    }
    void BmlSerializer::on_headword_I(){
        _serialize_headword_common<unsigned int>(&Lsc::var::IVar::to_uint32);
    }
    void BmlSerializer::on_headword_l(){
        _serialize_headword_common<long long>(&Lsc::var::IVar::to_int64);
    }
    void BmlSerializer::on_headword_L(){
        _serialize_headword_common<unsigned long long>(&Lsc::var::IVar::to_uint64);
    }
    void BmlSerializer::on_headword_f(){
        _serialize_headword_common<float>(&Lsc::var::IVar::to_float);
    }
    void BmlSerializer::on_headword_d(){
        _serialize_headword_common<dolcle>(&Lsc::var::IVar::to_dolcle);
    }
    void BmlSerializer::on_headword_v(){
        
        const IVar* pvar = _get_pvar();

        
        _serialize_bits();
        if ( _modifier.mask & (MODIFIER_DEQUEUE|MODIFIER_LEN) ){
            if ( (_modifier.mask & (MODIFIER_DEQUEUE|MODIFIER_LEN)) == (MODIFIER_DEQUEUE|MODIFIER_LEN) ){
                
                if ( _modifier.dynamic_len != _modifier.static_len ){
                    throw Lsc::OutOfBoundException()<<BSL_EARG<<"_modifier.static_len["<<_modifier.static_len<<"] != _modifier.dynamic_len["<<_modifier.dynamic_len<<"] from queue["<<_modifier.dequeue_id<<"]";
                }
            }
            size_t n = _modifier.mask & MODIFIER_DEQUEUE ? _modifier.dynamic_len : _modifier.static_len;
            if ( _modifier.mask & MODIFIER_ENQUEUE ){
                
                for( size_t i = 0; i < n; ++ i ){
                    long long num = pvar->get(i).to_int64(); 
                    if ( ssize_t(num) < 0 ){
                        throw Lsc::UnderflowException()<<BSL_EARG<<"length["<<num<<"] < 0";
                    }
                    push_len( _modifier.enqueue_id, num );
                    _serialize_vint(num);
                }
            }else{
                for( size_t i = 0; i < n; ++ i ){
                    _serialize_vint( pvar->get(i).to_int64() );
                }
            }

        }else{
            
            long long num = pvar ? pvar->to_int64() : 0;
            if ( _modifier.mask & MODIFIER_ENQUEUE ){
                if ( ssize_t(num) < 0 ){
                    throw Lsc::UnderflowException()<<BSL_EARG<<"length["<<num<<"] < 0";
                }
                push_len( _modifier.enqueue_id, num );
            }
            _serialize_vint( num );
        }
    }

    void BmlSerializer::on_headword_V(){
        
        const IVar* pvar = _get_pvar();

        
        _serialize_bits();
        if ( _modifier.mask & (MODIFIER_DEQUEUE|MODIFIER_LEN) ){
            if ( (_modifier.mask & (MODIFIER_DEQUEUE|MODIFIER_LEN)) == (MODIFIER_DEQUEUE|MODIFIER_LEN) ){
                
                if ( _modifier.dynamic_len != _modifier.static_len ){
                    throw Lsc::OutOfBoundException()<<BSL_EARG<<"_modifier.static_len["<<_modifier.static_len<<"] != _modifier.dynamic_len["<<_modifier.dynamic_len<<"] from queue["<<_modifier.dequeue_id<<"]";
                }
            }
            size_t n = _modifier.mask & MODIFIER_DEQUEUE ? _modifier.dynamic_len : _modifier.static_len;
            if ( _modifier.mask & MODIFIER_ENQUEUE ){
                
                for( size_t i = 0; i < n; ++ i ){
                    long long num = pvar->get(i).to_int64(); 
                    if ( ssize_t(num) < 0 ){
                        throw Lsc::UnderflowException()<<BSL_EARG<<"length["<<num<<"] < 0";
                    }
                    push_len( _modifier.enqueue_id, num );
                    _serialize_uvint(num);
                }
            }else{
                for( size_t i = 0; i < n; ++ i ){
                    _serialize_uvint( pvar->get(i).to_int64() );
                }
            }

        }else{
            
            long long num = pvar ? pvar->to_int64() : 0;
            if ( _modifier.mask & MODIFIER_ENQUEUE ){
                if ( ssize_t(num) < 0 ){
                    throw Lsc::UnderflowException()<<BSL_EARG<<"length["<<num<<"] < 0";
                }
                push_len( _modifier.enqueue_id, num );
            }
            _serialize_uvint( num );
        }
    }

    void BmlSerializer::on_headword_s(){
        
        const IVar* pvar = _get_pvar();
        const char * cstr;
        size_t cstr_len;

        
        _serialize_bits();

        
        if ( _modifier.mask & (MODIFIER_DEQUEUE|MODIFIER_LEN) ){
            if ( (_modifier.mask & (MODIFIER_DEQUEUE|MODIFIER_LEN)) == (MODIFIER_DEQUEUE|MODIFIER_LEN) ){
                if ( _modifier.dynamic_len != _modifier.static_len ){
                    throw Lsc::OutOfBoundException()<<BSL_EARG<<"_modifier.dynamic_len["<<_modifier.dynamic_len<<"] != _modifier.static_len["<<_modifier.static_len<<"]";
                }
            }
            size_t n = _modifier.mask & MODIFIER_DEQUEUE ? _modifier.dynamic_len : _modifier.static_len; 

            for( size_t i = 0; i < n; ++ i ){
                if ( pvar ){
                    const IVar& s = pvar->get(i);
                    cstr    = s.c_str();
                    cstr_len= s.c_str_len();
                }else{
                    cstr    = "";
                    cstr_len = 0;
                }
                if ( _modifier.mask & MODIFIER_ENQUEUE ){
                    push_bml( _modifier.enqueue_id, _buf_cur );
                }
                size_t len = pop_len(0);
                if ( len != size_t(-1) ){
                    if ( cstr_len < len ){
                        throw OutOfBoundException()<<BSL_EARG<<"length["<<len<<"] from queue 0 > cstr_len["<<cstr_len<<"]";
                    }
                    if (  len + 1  > size_t( _buf_end - _buf_cur ) ){
                        throw OutOfBoundException()<<BSL_EARG<<"buffer too small";
                    }
                    xmemcpy( _buf_cur, cstr, len );
                    _buf_cur[len] = 0;
                    _buf_cur += len + 1;

                }else{
                    
                    if (  cstr_len + 1  > size_t( _buf_end - _buf_cur ) ){
                        throw OutOfBoundException()<<BSL_EARG<<"buffer too small";
                    }
                    xmemcpy( _buf_cur, cstr, cstr_len + 1 );
                    _buf_cur += cstr_len + 1;
                }
            }

        }else{
            
            if ( pvar ){
                cstr    = pvar->c_str();
                cstr_len= pvar->c_str_len();
            }else{
                cstr    = "";
                cstr_len = 0;
            }
            if ( _modifier.mask & MODIFIER_ENQUEUE ){
                push_bml( _modifier.enqueue_id, _buf_cur );
            }
            size_t len = pop_len(0);
            if ( len != size_t(-1) ){
                if ( cstr_len < len ){
                    throw OutOfBoundException()<<BSL_EARG<<"length["<<len<<"] from queue 0 > cstr_len["<<cstr_len<<"]";
                }
                if (  len + 1  > size_t( _buf_end - _buf_cur ) ){
                    throw OutOfBoundException()<<BSL_EARG<<"buffer too small";
                }
                xmemcpy( _buf_cur, cstr, len );
                _buf_cur[len] = 0;
                _buf_cur += len + 1;

            }else{
                
                if (  cstr_len + 1  > size_t( _buf_end - _buf_cur ) ){
                    throw OutOfBoundException()<<BSL_EARG<<"buffer too small";
                }
                xmemcpy( _buf_cur, cstr, cstr_len + 1 );
                _buf_cur += cstr_len + 1;
            }

        }
    }

    void BmlSerializer::on_headword_S(){
        on_headword_s();
    }

    void BmlSerializer::on_headword_r(){
        
        const IVar* pvar = _get_pvar();
        const char * raw_data;
        size_t raw_data_len;

        
        _serialize_bits();

        if ( _modifier.mask & MODIFIER_DEREFERENCE ){
            
            if ( _modifier.mask & (MODIFIER_DEQUEUE|MODIFIER_LEN) ){
                if ( (_modifier.mask & (MODIFIER_DEQUEUE|MODIFIER_LEN) )==(MODIFIER_DEQUEUE|MODIFIER_LEN) ){
                    if ( _modifier.dynamic_len != _modifier.static_len ){
                        throw Lsc::OutOfBoundException()<<BSL_EARG<<"_modifier.dynamic_len["<<_modifier.dynamic_len<<"] != _modifier.static_len["<<_modifier.static_len<<"]";
                    }
                }
                size_t len = _modifier.mask & MODIFIER_DEQUEUE ? _modifier.dynamic_len : _modifier.static_len;

                for( size_t i = 0; i < len; ++ i ){
                    const char *bml = pop_bml( _modifier.dereference_id );
                    if ( !bml ){
                        throw Lsc::OutOfBoundException()<<BSL_EARG<<"bml_queue["<<_modifier.dereference_id<<"] does not exist";
                    }
                    BmlSerializer bs(bml);
                    _buf_cur += bs.serialize( pvar->get(i), _buf_cur, _buf_end - _buf_cur );
                }

            }else{
                
                const char *bml = pop_bml( _modifier.dereference_id );
                if ( !bml ){
                    throw Lsc::OutOfBoundException()<<BSL_EARG<<"bml_queue["<<_modifier.dereference_id<<"] does not exist";
                }
                BmlSerializer bs(bml);
                _buf_cur += bs.serialize( *pvar, _buf_cur, _buf_end - _buf_cur );
            }
        }else if ( _modifier.mask & (MODIFIER_DEQUEUE|MODIFIER_LEN) ){
            if ( (_modifier.mask & (MODIFIER_DEQUEUE|MODIFIER_LEN) ) == (MODIFIER_DEQUEUE|MODIFIER_LEN) ){
                if ( _modifier.dynamic_len != _modifier.static_len ){
                    throw Lsc::OutOfBoundException()<<BSL_EARG<<"_modifier.dynamic_len["<<_modifier.dynamic_len<<"] != _modifier.static_len["<<_modifier.static_len<<"]";
                }
            }

            size_t n = _modifier.mask & MODIFIER_DEQUEUE ? _modifier.dynamic_len : _modifier.static_len;
            for( size_t i = 0; i < n; ++ i ){
                if ( pvar ){
                    Lsc::var::raw_t raw = pvar->get(i).to_raw();
                    raw_data    = static_cast<const char *>(raw.data);
                    raw_data_len= raw.length;
                }else{
                    raw_data    = "";
                    raw_data_len = 0;
                }
                size_t len = pop_len(0);
                if ( len != size_t(-1) ){
                    if ( raw_data_len < len ){
                        throw OutOfBoundException()<<BSL_EARG<<"length["<<len<<"] from queue 0 > raw_data_len["<<raw_data_len<<"]";
                    }
                    if (  len  > size_t( _buf_end - _buf_cur ) ){
                        throw OutOfBoundException()<<BSL_EARG<<"buffer too small";
                    }
                    xmemcpy( _buf_cur, raw_data, len );
                    _buf_cur += len;

                }else{
                    
                    if (  raw_data_len  > size_t( _buf_end - _buf_cur ) ){
                        throw OutOfBoundException()<<BSL_EARG<<"buffer too small";
                    }
                    xmemcpy( _buf_cur, raw_data, raw_data_len );
                    _buf_cur += raw_data_len;
                }
            }
        }else{
            
            if ( pvar ){
                Lsc::var::raw_t raw = pvar->to_raw();
                raw_data    = static_cast<const char *>(raw.data);
                raw_data_len= raw.length;
            }else{
                raw_data    = "";
                raw_data_len = 0;
            }

            size_t len = pop_len(0);
            if ( len != size_t(-1) ){
                if ( raw_data_len < len ){
                    throw OutOfBoundException()<<BSL_EARG<<"length["<<len<<"] from queue 0 > raw_data_len["<<raw_data_len<<"]";
                }
                if (  len  > size_t( _buf_end - _buf_cur ) ){
                    throw OutOfBoundException()<<BSL_EARG<<"buffer too small";
                }
                xmemcpy( _buf_cur, raw_data, len );
                _buf_cur += len;

            }else{
                
                if (  raw_data_len  > size_t( _buf_end - _buf_cur ) ){
                    throw OutOfBoundException()<<BSL_EARG<<"buffer too small";
                }
                xmemcpy( _buf_cur, raw_data, raw_data_len );
                _buf_cur += raw_data_len;
            }
        }

    }

    void BmlSerializer::on_headword_R(){
        on_headword_r();
    }

    void BmlSerializer::on_headword_m(){
        _serialize_mcpack(1);
    }

    void BmlSerializer::on_headword_M(){
        _serialize_mcpack(2);
    }

    void BmlSerializer::on_headword_n(){
        
    }
    void BmlSerializer::on_headword_N(){
        
    }
    size_t BmlSerializer::on_unknown_modifier(const char *bml){
        
        throw Lsc::ParseErrorException()<<BSL_EARG<<"unexpected modifier["<<*bml<<"]";
    }
    size_t BmlSerializer::on_unknown_headword(const char *bml){
        
        throw Lsc::ParseErrorException()<<BSL_EARG<<"unexpected headword["<<*bml<<"]";
    }
} }  


