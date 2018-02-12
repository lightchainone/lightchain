#ifndef  __BSL_VAR__AUTOREF_H_
#define  __BSL_VAR__AUTOREF_H_

#include <Lsc/var/IRef.h>
#include <Lsc/ResourcePool.h>

namespace Lsc{ namespace var {

    /**
    *  
    *  根据第一次使用来变成相应的类型
    */
    class MagicRef: plclic Lsc::var::IRef{
    plclic:
        MagicRef(ResourcePool& rp)
            :IRef(), _p(NULL), _rp(rp){}
    
        MagicRef( const MagicRef& other)
            :IRef(other), _p(other._p), _rp(other._rp){}

        MagicRef& operator = ( const MagicRef& other ){
            _p = other._p;
            return *this;
        }

        virtual ~MagicRef(){}
        
        /**
         *
         * 该函数只克隆本身结点，不克隆子结点，对引用类型，克隆指向的结点
         *
        **/
        virtual IVar& clone( Lsc::ResourcePool& rp ) const {
            if ( !_p ){
                return rp.clone(*this);
            }else{
                return _p->clone(rp);
            }
        }

        /**
         * 
         * 由is_deep_copy参数控制是否深复制
         * 若为false, 只克隆本身结点，不克隆子结点
         * 若为true, 克隆本身结点，并且递归克隆子结点
         * 对引用类型，克隆指向的结点
         * 
        **/
        virtual IVar& clone( Lsc::ResourcePool& rp, bool is_deep_copy ) const {
            if ( !_p ){
                return rp.clone(*this);
            }else{
                return _p->clone(rp, is_deep_copy);
            }
        }

        //methods for ref
        virtual IVar& ref() const ;

        //methods for all
        /**
         *
         **/
        virtual void clear(){
            if ( _p ){
                _p->clear();
            }
        }

        /**
         *
        **/
        virtual string_type dump(size_t verbose_level = 0) const {
            string_type res;
            if ( _p ){
                res.append(_p->dump(verbose_level));
            }else{
                res.append("<magic null>");
            }
            return res;
        }

        /**
         *
        **/
        virtual string_type to_string() const ;

        /**
         *
        **/
        virtual string_type get_type() const {
            string_type res("Lsc::var::MagicRef(");
            if ( _p ){
                res.append(_p->get_type());
            }
            res.append(")");
            return res;
        }

        /**
         *
        **/
        virtual IVar::mask_type get_mask() const {
            return IVar::mask_type( IVar::IS_REF | ( _p ? _p->get_mask() : 0 ) );
        }

        virtual MagicRef& operator = ( IVar& var ){
            if ( var.is_ref() ){
                IRef * __p = dynamic_cast<IRef*>(&var);
                if ( NULL != __p ){
                    //make a shortcut
                    _p = &__p->ref();
                }else{
                    //unknown ref type
                    _p = &var;
                }
            }else{
                //non-ref value;
                _p = &var;
            }
            return *this;
        }


        //methods for all, test methods
        virtual bool is_null() const {
            if ( _p ){
                return _p->is_null();
            }else{
                return true;
            }
        }

        virtual bool is_ref() const {
            return true;
        }
        
        virtual bool is_bool() const {
            if ( _p ){
                return _p->is_bool();
            }else{
                return false;
            }
        }

        virtual bool is_number() const {
            if ( _p ){
                return _p->is_number();
            }else{
                return false;
            }
        }

        virtual bool is_int8() const {
            if ( _p ){
                return _p->is_int8();
            }else{
                return false;
            }
        }

        virtual bool is_uint8() const {
            if ( _p ){
                return _p->is_uint8();
            }else{
                return false;
            }
        }

        virtual bool is_int16() const {
            if ( _p ){
                return _p->is_int16();
            }else{
                return false;
            }
        }

        virtual bool is_uint16() const {
            if ( _p ){
                return _p->is_uint16();
            }else{
                return false;
            }
        }

        virtual bool is_int32() const {
            if ( _p ){
                return _p->is_int32();
            }else{
                return false;
            }
        }

        virtual bool is_uint32() const {
            if ( _p ){
                return _p->is_uint32();
            }else{
                return false;
            }
        }

        virtual bool is_int64() const {
            if ( _p ){
                return _p->is_int64();
            }else{
                return false;
            }
        }

        virtual bool is_uint64() const {
            if ( _p ){
                return _p->is_uint64();
            }else{
                return false;
            }
        }

        virtual bool is_float() const {
            if ( _p ){
                return _p->is_float();
            }else{
                return false;
            }
        }

        virtual bool is_dolcle() const {
            if ( _p ){
                return _p->is_dolcle();
            }else{
                return false;
            }
        }

        /**
         *
        **/
        virtual bool is_string() const {
            if ( _p ){
                return _p->is_string();
            }else{
                return false;
            }
        }

        virtual bool is_array() const {
            if ( _p ){
                return _p->is_array();
            }else{
                return false;
            }
        }

        virtual bool is_dict() const {
            if ( _p ){
                return _p->is_dict();
            }else{
                return false;
            };
        }

        virtual bool is_callable() const {
            if ( _p ){
                return _p->is_callable();
            }else{
                return false;
            }
        }

        virtual bool is_raw() const {
            if ( _p ){
                return _p->is_raw();
            }else{
                return false;
            }
        }

        //methods for value
        /**
         *
        **/
        virtual bool to_bool() const {
            if ( _p ){
                return _p->to_bool();  
            }else{
                return IRef::to_bool();
            }
        }

        /**
         *
        **/
        virtual signed char to_int8() const {
            if ( _p ){
                return _p->to_int8();  
            }else{
                return IRef::to_int8();
            }
        }

        /**
         *
        **/
        virtual unsigned char to_uint8() const {
            if ( _p ){
                return _p->to_uint8();  
            }else{
                return IRef::to_uint8();
            }
        }

        /**
         *
        **/
        virtual signed short to_int16() const {
            if ( _p ){
                return _p->to_int16();  
            }else{
                return IRef::to_int16();
            }
        }

        /**
         *
        **/
        virtual unsigned short to_uint16() const {
            if ( _p ){
                return _p->to_uint16();  
            }else{
                return IRef::to_uint16();
            }
        }

        /**
         *
        **/
        virtual signed int to_int32() const {
            if ( _p ){
                return _p->to_int32();  
            }else{
                return IRef::to_int32();
            }
        }

        /**
         *
        **/
        virtual unsigned int to_uint32() const {
            if ( _p ){
                return _p->to_uint32();  
            }else{
                return IRef::to_uint32();
            }
        }

        /**
         *
        **/
        virtual signed long long to_int64() const {
            if ( _p ){
                return _p->to_int64();  
            }else{
                return IRef::to_int64();
            }
        }

        /**
         *
        **/
        virtual unsigned long long to_uint64() const {
            if ( _p ){
                return _p->to_uint64();  
            }else{
                return IRef::to_uint64();
            }
        }

        /**
         *
        **/
        virtual float to_float() const {
            if ( _p ){
                return _p->to_float();  
            }else{
                return IRef::to_float();
            }
        }

        /**
         *
        **/
        virtual dolcle to_dolcle() const {
            if ( _p ){
                return _p->to_dolcle();  
            }else{
                return IRef::to_dolcle();
            }
        }

        virtual raw_type to_raw() const {
            if ( _p ){
                return _p->to_raw();  
            }else{
                return IRef::to_raw();
            }
        }

        virtual MagicRef& operator = ( bool val );

        /**
         *
        **/
        virtual MagicRef& operator = ( signed char val );

        /**
         *
        **/
        virtual MagicRef& operator = ( unsigned char val );

        /**
         *
        **/
        virtual MagicRef& operator = ( signed short val );

        /**
         *
        **/
        virtual MagicRef& operator = ( unsigned short val );

        /**
         *
        **/
        virtual MagicRef& operator = ( signed int val );

        /**
         *
        **/
        virtual MagicRef& operator = ( unsigned int val );

        /**
         *
        **/
        virtual MagicRef& operator = ( signed long long val );

        /**
         *
        **/
        virtual MagicRef& operator = ( unsigned long long val );

        /**
         *
        **/
        virtual MagicRef& operator = ( float val );

        /**
         *
        **/
        virtual MagicRef& operator = ( dolcle val );

        /**
         *
        **/
        virtual MagicRef& operator = ( const char *val );

        /**
         *
        **/
        virtual MagicRef& operator = ( const string_type& val );

        virtual MagicRef& operator = ( const raw_type& val );
        
        //methods for string
        virtual const char *c_str() const {
            if ( _p ){
                return _p->c_str();
            }else{
                return IRef::c_str();
            }
        }

        virtual size_t c_str_len() const {
            if ( _p ){
                return _p->c_str_len();
            }else{
                return IRef::c_str_len();
            }
        }

        //methods for array or dict
            /**
             *
            **/
        virtual size_t size() const {
            if ( _p ){
                return _p->size();
            }else{
                return IRef::size();
            }
        }

        //methods for array
        virtual IVar& get( size_t index ) {
            if ( _p ){
                return _p->get(index);
            }else{
                return IRef::get(index);
            }
        }

        virtual IVar& get( size_t index, IVar& default_value ) {
            if ( _p ){
                return _p->get(index, default_value);
            }else{
                return IRef::get(index, default_value);
            }
            
        }

        virtual const IVar& get( size_t index ) const {
            if ( _p ){
                return _p->get(index);
            }else{
                return IRef::get(index);
            }
        }

        virtual const IVar& get( size_t index, const IVar& default_value ) const {
            if ( _p ){
                return _p->get(index,default_value);
            }else{
                return IRef::get(index, default_value);
            }
        }

        virtual void set( size_t index, IVar& value );

        virtual bool del( size_t index );

        virtual array_const_iterator array_begin() const {
            if ( _p ){
                return const_cast<const IVar *>(_p)->array_begin();
            }else{
                return IRef::array_begin();
            }
        }

        virtual array_iterator array_begin() {
            if ( _p ){
                return _p->array_begin();
            }else{
                return IRef::array_begin();
            }
        }

        virtual array_const_iterator array_end() const {
            if ( _p ){
                return const_cast<const IVar *>(_p)->array_end();
            }else{
                return IRef::array_end();
            }
        }

        virtual array_iterator array_end() {
            if ( _p ){
                return _p->array_end();
            }else{
                return IRef::array_end();
            }
        }

        virtual const IVar& operator []( int index ) const {
            if ( _p ){
                return _p->operator [](index);
            }else{
                return IRef::operator[](index);
            }
        }

        virtual IVar& operator []( int index );

        //methods for dict
        virtual IVar& get( const field_type& name ) {
            if ( _p ){
                return _p->get(name);
            }else{
                return IRef::get(name);
            }
        }

        virtual IVar& get( const field_type& name, IVar& default_value ) {
            if ( _p ){
                return _p->get(name, default_value);
            }else{
                return IRef::get(name, default_value);
            }
        }

        virtual const IVar& get( const field_type& name ) const {
            if ( _p ){
                return _p->get(name);
            }else{
                return IRef::get(name);
            }
        }

        virtual const IVar& get( const field_type& name, const IVar& default_value ) const {
            if ( _p ){
                return _p->get(name, default_value);
            }else{
                return IRef::get(name, default_value);
            }
        }

        virtual void set( const field_type& name, IVar& value );

        virtual bool del( const field_type& name );

        virtual const IVar& operator []( const field_type& name ) const {
            if ( _p ){
                return _p->operator [](name);
            }else{
                return IRef::operator [](name);
            }
        }

        virtual IVar& operator []( const field_type& name );

        virtual dict_const_iterator dict_begin() const {
            if ( _p ){
                return const_cast<const IVar *>(_p)->dict_begin();
            }else{
                return IRef::dict_begin();
            }
        }

        virtual dict_iterator dict_begin() {
            if ( _p ){
                return _p->dict_begin();
            }else{
                return IRef::dict_begin();
            }
        }

        virtual dict_const_iterator dict_end() const {
            if ( _p ){
                return const_cast<const IVar *>(_p)->dict_end();
            }else{
                return IRef::dict_end();
            }
        }

        virtual dict_iterator dict_end() {
            if ( _p ){
                return _p->dict_end();
            }else{
                return IRef::dict_end();
            }
        }

        virtual IVar& operator()(IVar& param, Lsc::ResourcePool& rp ){
            if ( _p ){
                return (*_p)(param, rp);
            }else{
                return IRef::operator()(param, rp);
            }
        }

        virtual IVar& operator()(IVar& self, IVar& param, Lsc::ResourcePool& rp ){
            if ( _p ){
                return (*_p)(self, param, rp);
            }else{
                return IRef::operator()(self, param, rp);
            }
        }
    private:
        Lsc::var::IVar* _p;
        Lsc::ResourcePool& _rp;
        
    };  //end of class 
}}//end of namespace
#endif  //__BSL_VAR__AUTOREF_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
