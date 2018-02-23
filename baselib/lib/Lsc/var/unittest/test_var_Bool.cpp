#include "test_var_invalid.h"

class TestVarBool: plclic ITestVar{

plclic:
    typedef Lsc::var::IVar::string_type   string_type;
    typedef Lsc::var::IVar::field_type    field_type;
    typedef Lsc::var::ArrayIterator      array_iterator;
    typedef Lsc::var::ArrayConstIterator array_const_iterator;

    virtual ~TestVarBool(){}

    //speical methods
    virtual void test_special(){
        //ctor
        {
            Lsc::var::Bool b;
            assert( b.to_bool() == false );
        }
        {
            Lsc::var::Bool b(true);
            assert( b.to_bool() == true );
        }
        {
            Lsc::var::Bool b(false);
            assert( b.to_bool() == false );
        }
        // copy ctor
        {
            Lsc::var::Bool b1(true);
            Lsc::var::Bool b2(b1);
            assert( b2.to_bool() == true );
           
        }
        // copy assign
        {
            Lsc::var::Bool b1(true), b2(false);
            b1 = b2;
            assert( b1.to_bool() == false );
        }
    }
    virtual void test_mask(){
        test_mask_consistency( Lsc::var::Bool() );
    }
    //methods for all
    virtual void test_clear() {
        {
            Lsc::var::Bool b;
            b.clear();    //assert no-throw
            assert( b.to_bool() == false );
        }
        {
            Lsc::var::Bool b(true);
            b.clear();
            assert( b.to_bool() == false );
        }
    }

    virtual void test_dump() {
        {
            Lsc::var::Bool b;
            assert( b.dump() == "[Lsc::var::Bool]false" );
            assert( b.dump(999) == "[Lsc::var::Bool]false" );
        }
        {
            Lsc::var::Bool b(true);
            assert( b.dump() == "[Lsc::var::Bool]true" );
            assert( b.dump(999) == "[Lsc::var::Bool]true" );
        }

    }

    virtual void test_to_string() {
        {
            Lsc::var::Bool b;
            assert( b.to_string() == "false" );
        }
        {
            Lsc::var::Bool b(true);
            assert( b.to_string() == "true" );
        }

    }

    virtual void test_get_type() {
        assert( Lsc::var::Bool().get_type() == string_type("Lsc::var::Bool") );
    }

    //method for value
    virtual void test_bool(){
        {
            Lsc::var::Bool b;
            b = true;
            assert( b.to_bool() == true );
        }
        {
            const Lsc::var::Bool b(true);
            assert( b.to_bool() == true );
        }
    }
    virtual void test_raw(){
        Lsc::var::Bool b;
        test_invalid_raw(b);
    }

    virtual void test_number(){
        test_with_int32();
        test_with_int64();
        test_with_dolcle();
        
        test_valid_number( Lsc::var::Bool(true), true );
        test_valid_number( Lsc::var::Bool(false), false );
    }

    virtual void test_clone(){
        Lsc::ResourcePool rp;
        Lsc::var::Bool b;
        assert(b.clone(rp).to_string() == b.to_string() );
        assert(b.clone(rp).get_type() == b.get_type() );
        test_valid_number( Lsc::var::Bool(true).clone(rp), true );
        test_valid_number( Lsc::var::Bool(false).clone(rp), false );
    }

    virtual void test_string(){
        Lsc::var::Bool b;
        test_valid_string();
        test_invalid_string(b);
    }

    //methods for array and dict
    virtual void test_array(){
        Lsc::var::Bool b;
        test_invalid_array(b);
    }

    //methods for dict
    virtual void test_dict(){
        Lsc::var::Bool b;
        test_invalid_dict(b);
    }

    //methods for callable
    virtual void test_callable(){
        Lsc::var::Bool b;
        test_invalid_callable(b);
    }

    virtual void test_operator_assign(){
        //valid assign
        {
            Lsc::var::Bool b1, b2(true);
            b1 = (Lsc::var::IVar&)(b2);
            assert( b1.to_bool() == true );
        }
        {
            Lsc::var::Bool b1, b2(true);
            Lsc::var::Ref ref = b2;
            b1 = ref;
            assert( b1.to_bool() == true );
        }

    }

    virtual void test_valid_string(){
        //= const char *
        {
            Lsc::var::Bool b(true);
            b = static_cast<const char *>(NULL);
            assert( b.to_bool() == false );
        }
        {
            Lsc::var::Bool b(true);
            b = "";
            assert( b.to_bool() == false );
        }
        {
            Lsc::var::Bool b(true);
            b = "abc";
            assert( b.to_bool() == true );
        }

        //= string_type
        {
            Lsc::var::Bool b(true);
            b = string_type("");
            assert( b.to_bool() == false );
        }
        {
            Lsc::var::Bool b(true);
            b = string_type("false");   //wierd but correct
            assert( b.to_bool() == true );
        }

    }
    virtual void test_with_int32(){
        //operator =
        {
            Lsc::var::Bool b;
            b = 123;
            assert( b.to_bool() == true );
            b = 0;
            assert( b.to_bool() == false );
        }
        //to_int32()
        {
            assert( Lsc::var::Bool().to_int32() == 0 );
            assert( Lsc::var::Bool(true).to_int32() == 1 );
        }
        //from Int32
        {
            Lsc::var::Int32 i32(9394);
            Lsc::var::Bool b;
            b = i32;
            assert( b.to_bool() == true );
        }
        {
            Lsc::var::Int32 i32;
            Lsc::var::Bool b;
            b = i32;
            assert( b.to_bool() == false );
        }
        //to Int32
        {
            Lsc::var::Bool b(true);
            Lsc::var::Int32 i32;
            i32 = b;
            assert( i32.to_int32() == 1 );
        }
        {
            Lsc::var::Bool b;
            Lsc::var::Int32 i32;
            i32 = b;
            assert( i32.to_int32() == 0 );
        }
    }

    virtual void test_with_int64(){
        //operator =
        {
            Lsc::var::Bool b;
            b = 123LL;
            assert( b.to_bool() == true );
            b = 0LL;
            assert( b.to_bool() == false );
        }
        //to_int64()
        {
            assert( Lsc::var::Bool().to_int64() == 0 );
            assert( Lsc::var::Bool(true).to_int64() == 1 );
        }
        //from Int64
        {
            Lsc::var::Int64 i64(9394);
            Lsc::var::Bool b;
            b = i64;
            assert( b.to_bool() == true );
        }
        {
            Lsc::var::Int64 i64;
            Lsc::var::Bool b;
            b = i64;
            assert( b.to_bool() == false );
        }
        //to Int64
        {
            Lsc::var::Bool b(true);
            Lsc::var::Int64 i64;
            i64 = b;
            assert( i64.to_int64() == 1 );
        }
        {
            Lsc::var::Bool b;
            Lsc::var::Int64 i64;
            i64 = b;
            assert( i64.to_int64() == 0 );
        }
    }

    virtual void test_with_dolcle(){
        //operator =
        {
            Lsc::var::Bool b;
            b = 123.456;
            assert( b.to_bool() == true );
            b = 0.0;
            assert( b.to_bool() == false );
        }
        //to_dolcle()
        {
            assert( Lsc::var::Bool().to_dolcle() == 0 );
            assert( Lsc::var::Bool(true).to_dolcle() == 1 );
        }
        //from Dolcle
        {
            Lsc::var::Dolcle dbl(4670.9394);
            Lsc::var::Bool b;
            b = dbl;
            assert( b.to_bool() == true );
        }
        {
            Lsc::var::Dolcle dbl;
            Lsc::var::Bool b;
            b = dbl;
            assert( b.to_bool() == false );
        }
        //to Dolcle
        {
            Lsc::var::Bool b(true);
            Lsc::var::Dolcle dbl;
            dbl = b;
            assert( dbl.to_dolcle() == 1 );
        }
        {
            Lsc::var::Bool b;
            Lsc::var::Dolcle dbl;
            dbl = b;
            assert( dbl.to_dolcle() == 0 );
        }
    }

private:
};

int main(){
    TestVarBool test;
    test.test_all();
    return 0;
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
