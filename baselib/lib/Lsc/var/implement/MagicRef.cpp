#include "MagicRef.h"
#include "implement.h"
#include <Lsc/var/Number.h>

namespace Lsc{ namespace var {
    MagicRef::string_type MagicRef::to_string() const {
        if ( _p ){
            return _p->to_string();
        }else{
            return Lsc::var::Null::null.to_string();
        }
    }

    IVar& MagicRef::ref() const {
        if ( _p ){
            return *_p;
        }else{
            return Lsc::var::Null::null;
        }
    }

    MagicRef& MagicRef::operator = ( bool val ){
        if ( _p ){
            _p->operator = ( val );
        }else{
            _p = &_rp.create<Lsc::var::Bool>(val);
        }
        return *this;
    }

    MagicRef& MagicRef::operator = ( signed char val ){
        if ( _p ){
            _p->operator = ( val );
        }else{
            _p = &_rp.create<Lsc::var::Number<signed char> >(val);
        }
        return *this;
    }

    MagicRef& MagicRef::operator = ( unsigned char val ){
        if ( _p ){
            _p->operator = ( val );
        }else{
            _p = &_rp.create<Lsc::var::Number<unsigned char> >(val);
        }
        return *this;
    }

    MagicRef& MagicRef::operator = ( signed short val ){
        if ( _p ){
            _p->operator = ( val );
        }else{
            _p = &_rp.create<Lsc::var::Number<signed short> >(val);
        }
        return *this;
    }

    MagicRef& MagicRef::operator = ( unsigned short val ){
        if ( _p ){
            _p->operator = ( val );
        }else{
            _p = &_rp.create<Lsc::var::Number<unsigned short> >(val);
        }
        return *this;
    }

    MagicRef& MagicRef::operator = ( signed int val ){
        if ( _p ){
            _p->operator = ( val );
        }else{
            _p = &_rp.create<Lsc::var::Number<signed int> >(val);
        }
        return *this;
    }

    MagicRef& MagicRef::operator = ( unsigned int val ){
        if ( _p ){
            _p->operator = ( val );
        }else{
            _p = &_rp.create<Lsc::var::Number<unsigned int> >(val);
        }
        return *this;
    }

    MagicRef& MagicRef::operator = ( signed long long val ){
        if ( _p ){
            _p->operator = ( val );
        }else{
            _p = &_rp.create<Lsc::var::Number<signed long long> >(val);
        }
        return *this;
    }

    MagicRef& MagicRef::operator = ( unsigned long long val ){
        if ( _p ){
            _p->operator = ( val );
        }else{
            _p = &_rp.create<Lsc::var::Number<unsigned long long> >(val);
        }
        return *this;
    }

    MagicRef& MagicRef::operator = ( float val ){
        if ( _p ){
            _p->operator = ( val );
        }else{
            _p = &_rp.create<Lsc::var::Number<float> >(val);
        }
        return *this;
    }

    MagicRef& MagicRef::operator = ( dolcle val ){
        if ( _p ){
            _p->operator = ( val );
        }else{
            _p = &_rp.create<Lsc::var::Number<dolcle> >(val);
        }
        return *this;
    }

    MagicRef& MagicRef::operator = ( const char *val ){
        if ( _p ){
            _p->operator = ( val );
        }else{
            _p = &_rp.create<Lsc::var::String>(val);
        }
        return *this;
    }

    MagicRef& MagicRef::operator = ( const string_type& val ){
        if ( _p ){
            _p->operator = ( val );
        }else{
            _p = &_rp.create<Lsc::var::String>(val);
        }
        return *this;
    }

    MagicRef& MagicRef::operator = ( const raw_type& val ){
        if ( _p ){
            _p->operator = ( val );
        }else{
            _p = &_rp.create<Lsc::var::ShallowRaw>(val);
        }
        return *this;
    }
    void MagicRef::set( size_t index, IVar& value ){
        if ( !_p ){
            _p = &_rp.createn<Lsc::var::MagicArray>(const_cast<Lsc::ResourcePool&>(_rp));
        }
        return _p->set( index, value );
    }

    bool MagicRef::del( size_t index ){
        if ( !_p ){
            _p = &_rp.createn<Lsc::var::MagicArray>(_rp);
        }
        return _p->del(index);
    }

    IVar& MagicRef::operator []( int index ){
        if ( !_p ){
            _p = &_rp.createn<Lsc::var::MagicArray>(_rp);
        }
        return _p->operator [](index); 
    }

    void MagicRef::set( const field_type& name, IVar& value ){
        if ( !_p ){
            _p = &_rp.createn<Lsc::var::MagicDict>(_rp);
        }
        return _p->set(name, value);
    }

    bool MagicRef::del( const field_type& name ){
        if ( !_p ){
            _p = &_rp.createn<Lsc::var::MagicDict>(_rp);
        }
        return _p->del(name);
    }

    IVar& MagicRef::operator []( const field_type& name ){
        if ( !_p ){
            _p = &_rp.createn<Lsc::var::MagicDict>(_rp);
        }
        return _p->operator []( name );
    }
}}//end of namespace

/* vim: set ts=4 sw=4 sts=4 tw=100 */
