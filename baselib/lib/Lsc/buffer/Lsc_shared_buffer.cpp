#include "Lsc_shared_buffer.h"
#include "Lsc_exception.h"

namespace Lsc{
    const char * const SharedBuffer::TRUE_LITERAL = "true";
    const char * const SharedBuffer::FALSE_LITERAL= "false";

    SharedBuffer::SharedBuffer( const SharedBuffer& other )
        : _pbuf( other._pbuf ) {
            _pbuf->_ref_count += 1;
            if ( _pbuf->_ref_count < 0 ){
                //reference count overflow
                _pbuf->_ref_count -= 1;
                throw OverflowException()<<BSL_EARG<<"_ref_count";
            }
        }

    SharedBuffer& SharedBuffer::operator = ( const SharedBuffer& other ){
        if ( &other != this ){
            _pbuf->_ref_count -=1;
            
            if ( !_pbuf->_ref_count ){
                free( _pbuf );
            }
            
            _pbuf = other._pbuf;
            _pbuf->_ref_count += 1;
            
            if ( _pbuf->_ref_count < 0 ){
                throw OverflowException()<<BSL_EARG<<"_ref_count";
            }
        }
        return *this;
    }

    SharedBuffer& SharedBuffer::push(const char* cstr, size_t slc_str_len ){
        if ( cstr != NULL ){
            size_t len = 0;
            for(; cstr[len] && len < slc_str_len; ++ len){
                //pass
            }

            if ( int(_pbuf->_size + len) <= _pbuf->_capacity ){
                memcpy( _pbuf->_begin, cstr, len );
                _pbuf->_size += len;
                _pbuf->_begin[ _pbuf->_size ] = 0;
                _pbuf->_truncated = false;

            }else{  //not enough space;
                if ( reserve( _pbuf->_size + len)  ){
                    //reserve succeed, copy succeed
                    memcpy( _pbuf->_begin, cstr, len );
                    _pbuf->_size += len;
                    _pbuf->_begin[ _pbuf->_size ] = 0;
                    _pbuf->_truncated = false;

                }else{
                    //reserve failed
                    //content is truncated
                    memcpy( _pbuf->_begin, cstr, _pbuf->_capacity - _pbuf->_size );
                    _pbuf->_size = _pbuf->_capacity;
                    _pbuf->_begin[ _pbuf->_size ] = 0;
                    _pbuf->_truncated = true;
                }

            }
        }
        return *this;
    }

    SharedBuffer& SharedBuffer::vpushf( const char *format, va_list ap ){
        va_list aq;
        va_copy( aq, ap );
        int len = vsnprintf( _pbuf->_begin + _pbuf->_size, _pbuf->_capacity - _pbuf->_size + 1, format, aq );
        va_end( aq );
        if ( len >= 0 ){

            if ( _pbuf->_size + len <= _pbuf->_capacity ){
                _pbuf->_size += len;
                _pbuf->_truncated = false;

            }else{  //truncated
                if ( reserve( _pbuf->_size + len)  ){
                    //reserve succeed, vsnprintf succeed
                    va_copy( aq, ap );
                    vsnprintf( _pbuf->_begin + _pbuf->_size, _pbuf->_capacity - _pbuf->_size + 1, format, aq );
                    va_end( aq );

                    _pbuf->_size += len;
                    _pbuf->_truncated = false;

                }else{
                    //reserve failed
                    //content is truncated
                    _pbuf->_size = _pbuf->_capacity;
                    _pbuf->_truncated = true;
                }

            }
        }
        return *this;
    }

    void SharedBuffer::_init( int __capacity ){
        if ( __capacity < 0 ){
            throw BadArgumentException()<<BSL_EARG<<"__capacity < 0";
        }
        _pbuf = static_cast<_buffer_t*>(malloc(sizeof(_buffer_t)));
        if ( !_pbuf ){
            throw BadAllocException()<<BSL_EARG;
        }
        _pbuf->_begin = static_cast<char *>(malloc(__capacity + 1));
        if ( !_pbuf->_begin ){
            free(_pbuf);
            throw BadAllocException()<<BSL_EARG;
        }

        _pbuf->_size        = 0;
        _pbuf->_capacity    = __capacity;
        _pbuf->_ref_count   = 1;
        _pbuf->_truncated   = false;
    }
}
/* vim: set ts=4 sw=4 sts=4 tw=100 */
