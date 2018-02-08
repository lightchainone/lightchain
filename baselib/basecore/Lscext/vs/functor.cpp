
#include <Lsc/var/IVar.h>
#include <Lsc/var/Array.h>
#include <Lsc/var/VScriptEvaluator.h>

namespace Lsc{ namespace var{
    class AppendFunctor: plclic IVar{
    plclic:
        typedef IVar::string_type          string_type;
        typedef IVar::field_type           field_type;
        typedef IVar& (* function_type )(IVar&, Lsc::ResourcePool& );

        AppendFunctor( IVar& func, IVar& args )
            :IVar(), _func(func), _args(args){ 
                if ( !func.is_callable() ){
                    throw BadArgumentException()<<BSL_EARG<<"func.is_callable() should be true";
                }
                if ( !args.is_array() ){
                    throw BadArgumentException()<<BSL_EARG<<"args.is_array() should be true";
                }
            }

        AppendFunctor( const AppendFunctor& other )
            :IVar(), _func(other._func), _args(other._args) { }

        virtual ~AppendFunctor() {}

        virtual IVar::mask_type get_mask() const{
            return IVar::IS_CALLABLE;
        }
        
        virtual AppendFunctor& clone( Lsc::ResourcePool& rp) const{
            return rp.clone<AppendFunctor>(*this);
        }
        
        virtual AppendFunctor& clone( Lsc::ResourcePool& rp, bool ) const{
            return rp.clone<AppendFunctor>(*this);
        }

        virtual IVar& operator = ( IVar& ){
            throw Lsc::InvalidOperationException()<<BSL_EARG<<"AppendFunctor is immutable";
        }

        virtual void clear() {}

        virtual string_type dump(size_t verbose_level = 0) const {
            if ( 0 == verbose_level ){
                return string_type("[Lsc::var::AppendFunctor]").appendf("%p", this);
            }else{
                string_type res("[Lsc::var::AppendFunctor]");
                res.appendf("%p{func:", this);
                res.append( _func.dump( verbose_level - 1 ) );
                res.append(", args:");
                res.append( _args.dump( verbose_level - 1 ) );
                res.append("}");
                return res;
            }
        }

        virtual string_type to_string() const {
            return string_type("[Lsc::var::AppendFunctor]").appendf("%p", this);
        }

        virtual string_type get_type() const {
            return "Lsc::var::AppendFunctor";
        }

        virtual bool is_callable() const {
            return true;
        }

        virtual IVar& operator()(IVar& args, Lsc::ResourcePool& rp ){
            return _func( merge(args, rp), rp );
        }

        virtual IVar& operator()(IVar& self, IVar& args, Lsc::ResourcePool& rp ){
            return _func( self, merge(args, rp), rp );
        }

        using IVar::operator =;

    private:
        AppendFunctor& operator =( const AppendFunctor& );

        IVar& merge( IVar& args, Lsc::ResourcePool& rp ){
            if ( args.size() == 0 ){
                return _args;
            }else if ( _args.size() == 0 ){
                return args;
            }else{
                IVar& merge_args = rp.create<Lsc::var::Array>();    
                size_t i = 0; 
                for( size_t j = 0, jn = _args.size(); j < jn; ++ i, ++j ){
                    merge_args[i] = _args[j];
                }
                for( size_t j = 0, jn = args.size(); j < jn; ++ i, ++j ){
                    merge_args[i] = args[j];
                }
                return merge_args;
            }
        }

        IVar&   _func;
        IVar&   _args;
    };

    class NullFunctor: plclic IVar{
    plclic:
        typedef IVar::string_type          string_type;
        typedef IVar::field_type           field_type;
        typedef IVar& (* function_type )(IVar&, Lsc::ResourcePool& );

        NullFunctor( IVar& func, IVar& args )
            :IVar(), _func(func), _args(args){ 
                if ( !func.is_callable() ){
                    throw BadArgumentException()<<BSL_EARG<<"func.is_callable() should be true";
                }
                if ( !args.is_array() ){
                    throw BadArgumentException()<<BSL_EARG<<"args.is_array() should be true";
                }
            }

        NullFunctor( const NullFunctor& other )
            :IVar(), _func(other._func), _args(other._args) {}

        virtual ~NullFunctor() {}

        virtual IVar::mask_type get_mask() const{
            return IVar::IS_CALLABLE;
        }
        
        virtual NullFunctor& clone( Lsc::ResourcePool& rp ) const{
            return rp.clone<NullFunctor>(*this);
        }
        
        virtual NullFunctor& clone( Lsc::ResourcePool& rp, bool ) const{
            return rp.clone<NullFunctor>(*this);
        }

        virtual IVar& operator = ( IVar& ){
            throw Lsc::InvalidOperationException()<<BSL_EARG<<"NullFunctor is immutable";
        }

        virtual void clear() {}

        virtual string_type dump(size_t verbose_level = 0) const {
            if ( 0 == verbose_level ){
                return string_type("[Lsc::var::NullFunctor]").appendf("%p", this);
            }else{
                string_type res("[Lsc::var::NullFunctor]");
                res.appendf("%p{func:", this);
                res.append( _func.dump( verbose_level - 1 ) );
                res.append(", args:");
                res.append( _args.dump( verbose_level - 1 ) );
                res.append("}");
                return res;
            }
        }

        virtual string_type to_string() const {
            return string_type("[Lsc::var::NullFunctor]").appendf("%p", this);
        }

        virtual string_type get_type() const {
            return "Lsc::var::NullFunctor";
        }

        virtual bool is_callable() const {
            return true;
        }

        virtual IVar& operator()(IVar& args, Lsc::ResourcePool& rp ){
            return _func( merge( args, rp ), rp );
        }

        virtual IVar& operator()(IVar& self, IVar& args, Lsc::ResourcePool& rp ){
            return _func( self, merge( args, rp ), rp );
        }

        using IVar::operator =;

    private:
        IVar& merge( IVar& args, Lsc::ResourcePool& rp ){
            if ( args.size() == 0 ){
                return _args;
            }else if ( _args.size() == 0 ){
                return args;
            }else{
                IVar& merge_args = rp.create<Lsc::var::Array>();    
                size_t i=0, in=_args.size();
                size_t j=0, jn=args.size();
                size_t k=0;
                for(; i<in; ++i, ++k ){
                    if ( j<jn && _args[j].is_null() ){
                        merge_args[k] = args[j++];  
                    }else{
                        merge_args[k] = _args[i];   
                    }
                }
                for(; j < jn; ++j, ++k){    
                    merge_args[k] = args[j];    
                }
                return merge_args;
            }
        }

        NullFunctor& operator =( const NullFunctor& );
        IVar&   _func;
        IVar&   _args;
    };

    class ScopelessLambda: plclic IVar{
    plclic:
        typedef IVar::string_type          string_type;
        typedef IVar::field_type           field_type;
        typedef IVar& (* function_type )(IVar&, Lsc::ResourcePool& );

        ScopelessLambda( IVar& lambda, IVar& scope )
            :IVar(), _lambda(lambda), _scope(scope){ 
                if ( !lambda.is_string() ){
                    throw BadArgumentException()<<BSL_EARG<<"expr.is_string() should be true";
                }
            }

        ScopelessLambda( const ScopelessLambda& other )
            :IVar(), _lambda(other._lambda), _scope(other._scope) {}

        virtual ~ScopelessLambda() {}

        virtual IVar::mask_type get_mask() const{
            return IVar::IS_CALLABLE;
        }

        virtual ScopelessLambda& clone( Lsc::ResourcePool& rp ) const{
            return rp.clone<ScopelessLambda>(*this);
        }

        virtual ScopelessLambda& clone( Lsc::ResourcePool& rp, bool  ) const{
            return rp.clone<ScopelessLambda>(*this);
        }

        virtual IVar& operator = ( IVar& ){
            throw Lsc::InvalidOperationException()<<BSL_EARG<<"ScopelessLambda is immutable";
        }

        virtual void clear() {}

        virtual string_type dump(size_t verbose_level = 0) const {
            if ( 0 == verbose_level ){
                return string_type("[Lsc::var::ScopelessLambda]").appendf("%p", this);
            }else{
                string_type res("[Lsc::var::ScopelessLambda]");
                res.appendf("%p{lambda:", this);
                res.append( _lambda.dump( verbose_level - 1 ) );
                res.append(", scope:");
                res.append( _scope.dump( verbose_level - 1 ) );
                res.append("}");
                return res;
            }
        }

        virtual string_type to_string() const {
            return string_type("[Lsc::var::ScopelessLambda]").appendf("%p", this);
        }

        virtual string_type get_type() const {
            return "Lsc::var::ScopelessLambda";
        }

        virtual bool is_callable() const {
            return true;
        }

        virtual IVar& operator()(IVar& args, Lsc::ResourcePool& rp ){
            
            IVar& __args__ = _scope.get(ARGS_LITERAL);

            
            _scope.set(ARGS_LITERAL, args);

            
            Lsc::var::VScriptEvaluator evaluator(rp);
            IVar& res = evaluator.eval( _lambda.c_str(), _scope );

            
            if ( __args__.is_null() ){
                _scope.del(ARGS_LITERAL);
            }else{
                _scope.set(ARGS_LITERAL, __args__);
            }

            return res;
        }

        virtual IVar& operator()(IVar& self, IVar& args, Lsc::ResourcePool& rp ){
            
            IVar& __self__ = _scope.get(SELF_LITERAL);
            IVar& __args__ = _scope.get(ARGS_LITERAL);

            
            _scope.set(SELF_LITERAL, self);
            _scope.set(ARGS_LITERAL, args);

            
            Lsc::var::VScriptEvaluator evaluator(rp);
            IVar& res = evaluator.eval( _lambda.c_str(), _scope );

            
            if ( __self__.is_null() ){
                _scope.set(SELF_LITERAL, __self__);
            }else{
                _scope.del(SELF_LITERAL);
            }
            if ( __args__.is_null() ){
                _scope.del(SELF_LITERAL);
            }else{
                _scope.set(ARGS_LITERAL, __args__);
            }

            return res;
        }

        using IVar::operator =;

    private:
        ScopelessLambda& operator =( const ScopelessLambda& );
        IVar& _lambda;
        IVar& _scope;
        static const IVar::field_type ARGS_LITERAL;
        static const IVar::field_type SELF_LITERAL;
    };

    const IVar::field_type ScopelessLambda::ARGS_LITERAL("__args__");
    const IVar::field_type ScopelessLambda::SELF_LITERAL("__self__");

} } 

namespace Lsc{ namespace vs{
    Lsc::var::IVar& append_functor( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::AppendFunctor>( Lsc::var::AppendFunctor(args[0], args[1]) );
    }

    Lsc::var::IVar& null_functor( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::NullFunctor>( Lsc::var::NullFunctor(args[0], args[1]) );
    }

    Lsc::var::IVar& scopeless_lambda( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
        return rp.create<Lsc::var::ScopelessLambda>( Lsc::var::ScopelessLambda(args[0], args[1]) );
    }
    
} } 



