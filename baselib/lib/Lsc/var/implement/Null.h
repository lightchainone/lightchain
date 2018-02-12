#ifndef  __BSL_VAR_NULL_H__
#define  __BSL_VAR_NULL_H__
#include "Lsc/var/IVar.h"

namespace Lsc{
namespace var{
    class Null: plclic IVar{
    plclic:
        typedef IVar::string_type          string_type;
        typedef IVar::field_type           field_type;

        //special methods
        Null(){ }

        Null( const Null& other ):IVar(other){ }

        virtual ~Null(){ } 

        Null& operator = ( const Null& ){
            return *this;
        }

        //methods for all
        /**
         *
         **/
        virtual void clear(){
            // pass
        }

        /**
         *
         * 该函数只克隆本身结点，不克隆子结点，对引用类型，克隆指向的结点
         *
        **/
        virtual Null& clone( Lsc::ResourcePool& rp ) const {
            return rp.clone(*this);
        }

        /**
         * 
         * 由is_deep_copy参数控制是否深复制
         * 若为false, 只克隆本身结点，不克隆子结点
         * 若为true, 克隆本身结点，并且递归克隆子结点
         * 对引用类型，克隆指向的结点
         * 
        **/
        virtual Null& clone( Lsc::ResourcePool& rp, bool /*is_deep_copy*/ ) const {
            return rp.clone(*this);
        }

        /**
         *
        **/
        virtual string_type dump(size_t /*verbose_level*/ = 0) const {
            return "[Lsc::var::Null]null";
        }

        /**
         *
        **/
        virtual string_type to_string() const {
            return "null";
        }

        /**
         *
        **/
        virtual string_type get_type() const {
            return "Lsc::var::Null";
        }

        /**
         *
        **/
        virtual IVar::mask_type get_mask() const {
            return 0;
        }

        virtual Null& operator = ( IVar& var ){
            if ( !var.is_null() ){
                throw Lsc::InvalidOperationException()<<BSL_EARG<<"{"<<__PRETTY_FUNCTION__<<"("<<var.dump()<<")}";
            }
            return *this;
        }

        virtual bool is_null() const {
            return true;
        }

        using IVar::operator =;

    plclic:
        static Null null;
    };

}}   //namespace Lsc::var
#endif  //__BSL_VAR_NULL_H__

/* vim: set ts=4 sw=4 sts=4 tw=100 */
