#include "test_var_invalid.h"

Lsc::var::IVar& echo( Lsc::var::IVar& self, Lsc::var::IVar& args, Lsc::ResourcePool& rp){
    Lsc::var::IVar& res = rp.create<Lsc::var::Dict>();
    res["self"] = self;
    res["args"] = args;
    return res;
}

Lsc::var::IVar& size( Lsc::var::IVar& self, Lsc::var::IVar& , Lsc::ResourcePool& rp ){
    return rp.create<Lsc::var::Int32>(int(self.size()));
}

class TestVarMethod: plclic ITestVar{
    
plclic:
    typedef Lsc::var::IVar::string_type   string_type;
    typedef Lsc::var::IVar::field_type    field_type;
    typedef Lsc::var::ArrayIterator      array_iterator;
    typedef Lsc::var::ArrayConstIterator array_const_iterator;

    TestVarMethod()
        :_method(echo, "method"){}

    virtual ~TestVarMethod(){}
    
    //speical methods
    virtual void test_special(){
        // ctors
        {
            Lsc::var::Method func1(&echo, "echo");
            assert( func1.to_string() == "echo" );
            Lsc::var::Method func2(func1);
            assert( func2.to_string() == "echo" );
        }
        // copy assign
        {
            Lsc::var::Method func1(&echo, "echo");
            Lsc::var::Method func2(&size, "size");
            func1 = func2;
            assert( func1.to_string() == "size" );
        }
    }
    //methods for all
    virtual void test_mask(){
        test_mask_consistency( Lsc::var::Method(&echo, "echo") );
    }

    virtual void test_clear() {
        {
            Lsc::var::Method func1(&echo, "echo");
            func1.clear(); //no effect
            assert( func1.to_string() == "echo" );
        }
    }

    virtual void test_dump() {
        {
            Lsc::var::Method func(echo, "echo");
            assert( func.dump() == "[Lsc::var::Method]echo" );
            assert( func.dump(999) == "[Lsc::var::Method]echo" );
        }

    }

    virtual void test_to_string() {
        {
            Lsc::var::Method func(echo, "echo");
            assert( func.to_string() == "echo" );
        }

    }

    virtual void test_get_type() {
        assert( Lsc::var::Method(&echo, "echo").get_type() == string_type("Lsc::var::Method") );
    }

    //method for value
    virtual void test_bool(){
        test_invalid_bool(_method);
    }
    virtual void test_raw(){
        test_invalid_raw(_method);
    }
    virtual void test_number(){
        test_invalid_number(_method);
    }
    virtual void test_clone(){
        Lsc::ResourcePool rp;
        Lsc::var::Method f(&echo, "echo");
        Lsc::var::Method& cl = f.clone(rp);
        Lsc::var::MagicRef args(rp);
        Lsc::var::Dict self;
        args[0] = 123;
        args[1] = "hello";
        assert( cl.get_type() == string_type("Lsc::var::Method") );
        assert( cl.to_string() == string_type("echo") );
        Lsc::var::IVar& res = cl(self, args, rp);
        assert( res["args"][0].to_int32() == 123 );
        assert( res["args"][1].c_str() == args[1].c_str() );
        assert( res["self"].is_dict() );
    }
    virtual void test_string(){
        test_invalid_string(_method);
    }

    //methods for array
    virtual void test_array(){
        test_invalid_array(_method);
    }

    //methods for dict
    virtual void test_dict(){
        test_invalid_dict(_method);
    }

    //methods for callable
    virtual void test_callable(){
        test_operator_paren();
    }

    virtual void test_operator_assign(){
        //valid assign
        {
            Lsc::var::Method func1(&echo, "echo"), func2(&size, "size");
            Lsc::var::IVar& ivar = func1;
            func2 = ivar;
            assert( func2.to_string() == func1.to_string() );

        }
        //invalid assign
        {
            Lsc::var::Method func(&echo, "echo");
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
            Lsc::var::Dict self;
            Lsc::var::Array args;
            Lsc::ResourcePool rp;
            Lsc::var::IVar& res = Lsc::var::Method(&echo, "echo")(self, args, rp);
            assert( &Lsc::var::Ref(res["self"]).ref() == &self );
            assert( &Lsc::var::Ref(res["args"]).ref() == &args );
        }
        {
            Lsc::var::Array args;
            Lsc::ResourcePool rp;
            Lsc::var::IVar& res = Lsc::var::Method(&echo, "echo")(args, rp);
            assert( res["self"].is_null() );
            assert( &Lsc::var::Ref(res["args"]).ref() == &args );
        }
        {
            Lsc::ResourcePool rp;
            Lsc::var::Dict self;
            Lsc::var::Array args;
            Lsc::var::Int32 i32(123);
            Lsc::var::String str("hao");
            self["str"] = str;
            self["i32"] = i32;
            Lsc::var::IVar& res = Lsc::var::Method(&size, "size")(self, args, rp);
            assert( res.to_int32() == 2 );
        }
    }
private:
    Lsc::var::Method _method;
};

int main(){
    TestVarMethod test;
    test.test_all();
    return 0;
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
