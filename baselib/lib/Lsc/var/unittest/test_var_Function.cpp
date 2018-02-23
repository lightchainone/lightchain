#include "test_var_invalid.h"

Lsc::var::IVar& echo( Lsc::var::IVar& args, Lsc::ResourcePool& ){
    return args;
}

Lsc::var::IVar& join( Lsc::var::IVar& args, Lsc::ResourcePool& rp ){
    Lsc::var::IVar::string_type res;
    for( size_t i = 0; i < args.size(); ++ i ){
        res.append(args[int(i)].to_string());
    }
    return rp.create<Lsc::var::String>(res);
}

class TestVarFunction: plclic ITestVar{
plclic:
    typedef Lsc::var::IVar::string_type   string_type;
    typedef Lsc::var::IVar::field_type    field_type;
    typedef Lsc::var::ArrayIterator      array_iterator;
    typedef Lsc::var::ArrayConstIterator array_const_iterator;

    TestVarFunction()
        :_func(echo, "echo"){}

    virtual ~TestVarFunction(){}
    
    //speical methods
    virtual void test_special(){
        // ctors
        {
            Lsc::var::Function func1(&echo, "echo");
            assert( func1.to_string() == "echo" );
            Lsc::var::Function func2(func1);
            assert( func2.to_string() == "echo" );
        }
        // copy assign
        {
            Lsc::var::Function func1(&echo, "echo");
            Lsc::var::Function func2(&join, "join");
            func1 = func2;
            assert( func1.to_string() == "join" );
        }
    }
    //methods for all
    virtual void test_mask(){
        test_mask_consistency( Lsc::var::Function(&echo, "echo") );
    }


    virtual void test_clear() {
        {
            Lsc::var::Function func1(&echo, "echo");
            func1.clear(); //no effect
            assert( func1.to_string() == "echo" );
        }
    }

    virtual void test_dump() {
        {
            Lsc::var::Function func(echo, "echo");
            assert( func.dump() == "[Lsc::var::Function]echo" );
            assert( func.dump(999) == "[Lsc::var::Function]echo" );
        }

    }

    virtual void test_to_string() {
        {
            Lsc::var::Function func(echo, "echo");
            assert( func.to_string() == "echo" );
        }

    }

    virtual void test_get_type() {
        assert( Lsc::var::Function(&echo, "echo").get_type() == string_type("Lsc::var::Function") );
    }

    //method for value
    virtual void test_bool(){
        test_invalid_bool(_func);
    }
    virtual void test_raw(){
        test_invalid_raw(_func);
    }
    virtual void test_number(){
        test_invalid_number(_func);
    }
    virtual void test_clone(){
        Lsc::ResourcePool rp;
        Lsc::var::Function f(&echo, "echo");
        Lsc::var::Function& cl = f.clone(rp);
        Lsc::var::MagicRef args(rp);
        args[0] = 123;
        args[1] = "hello";
        assert( cl.get_type() == string_type("Lsc::var::Function") );
        assert( cl.to_string() == string_type("echo") );
        Lsc::var::IVar& res = cl(args, rp);
        assert( res[0].to_int32() == 123 );
        assert( res[1].c_str() == args[1].c_str() );
    }
        
    virtual void test_string(){
        test_invalid_string(_func);
    }

    //methods for array
    virtual void test_array(){
        test_invalid_array(_func);
    }

    //methods for dict
    virtual void test_dict(){
        test_invalid_dict(_func);
    }

    //methods for callable
    virtual void test_callable(){
        test_operator_paren();
    }

    virtual void test_operator_assign(){
        //valid assign
        {
            Lsc::var::Function func1(&echo, "echo"), func2(&join, "join");
            Lsc::var::IVar& ivar = func1;
            func2 = ivar;
            assert( func2.to_string() == func1.to_string() );

        }
        //invalid assign
        {
            Lsc::var::Function func(&echo, "echo");
            Lsc::var::Int32 i32;
            ASSERT_THROW( func = 123, Lsc::InvalidOperationException );
            ASSERT_THROW( func = 123LL, Lsc::InvalidOperationException );
            ASSERT_THROW( func = 123.456, Lsc::InvalidOperationException );
            ASSERT_THROW( func = "123", Lsc::InvalidOperationException );
            ASSERT_THROW( func = i32, Lsc::InvalidOperationException );
        }
    }

    virtual void test_operator_paren(){
        {
            Lsc::var::Array args;
            Lsc::ResourcePool rp;
            Lsc::var::IVar& res = Lsc::var::Function(&echo, "echo")(args, rp);
            assert( &res == &args );
        }
        {
            Lsc::ResourcePool rp;
            Lsc::var::Array args;
            Lsc::var::Int32 i32(123);
            Lsc::var::String str("hao");
            args[0] = str;
            args[1] = i32;
            Lsc::var::IVar& res = Lsc::var::Function(&join, "join")(args, rp);
            assert( res.to_string() == "hao123" );
        }
        {
            Lsc::ResourcePool rp;
            Lsc::var::Dict self;
            Lsc::var::Array args;
            Lsc::var::Int32 i32(123);
            Lsc::var::String str("hao");
            args[0] = str;
            args[1] = i32;
            Lsc::var::IVar& res = Lsc::var::Function(&join, "join")(self, args, rp);
            assert( res.to_string() == "hao123" );
        }
    }
private:
    Lsc::var::Function _func;
};

int main(){
    TestVarFunction test;
    test.test_all();
    return 0;
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
