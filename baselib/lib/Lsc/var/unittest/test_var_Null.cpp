#include "test_var_invalid.h"

class TestVarNull: plclic ITestVar{

plclic:
    typedef Lsc::var::IVar::string_type   string_type;
    typedef Lsc::var::IVar::field_type    field_type;
    typedef Lsc::var::ArrayIterator      array_iterator;
    typedef Lsc::var::ArrayConstIterator array_const_iterator;

    virtual ~TestVarNull(){}

    //speical methods
    virtual void test_special(){
        // copy ctor
        {
            Lsc::var::Null null1;
            Lsc::var::Null null2(null1);
            assert( null2.is_null() );
        }
        // copy assign
        {
            Lsc::var::Null null1, null2;
            null2 = null1;
            assert( null2.is_null() );
        }
    }
    //methods for all
    virtual void test_mask(){
        test_mask_consistency( Lsc::var::Null() );
    }

    virtual void test_clear() {
        {
            Lsc::var::Null null;
            null.clear();    //assert no-throw
        }
    }

    virtual void test_dump() {
        {
            Lsc::var::Null null;
            assert( null.dump() == string_type("[Lsc::var::Null]null") );
            assert( null.dump(999) == string_type("[Lsc::var::Null]null") );
        }

    }

    virtual void test_to_string() {
        {
            Lsc::var::Null null;
            assert( null.to_string() == string_type("null") );
        }

    }

    virtual void test_get_type() {
        assert( Lsc::var::Null().get_type() == string_type("Lsc::var::Null") );
    }

    //method for value
    virtual void test_bool(){
        test_invalid_bool(Lsc::var::Null::null);
    }
    virtual void test_raw(){
        test_invalid_raw(Lsc::var::Null::null);
    }
    virtual void test_number(){
        test_invalid_number(Lsc::var::Null::null);
    }
    virtual void test_clone(){
        Lsc::ResourcePool rp;
        assert( Lsc::var::Null::null.clone(rp).is_null() );
    }

    virtual void test_string(){
        test_invalid_string(Lsc::var::Null::null);
    }

    //methods for array
    virtual void test_array(){
        test_invalid_array(Lsc::var::Null::null);
    }

    //methods for dict
    virtual void test_dict(){
        test_invalid_dict(Lsc::var::Null::null);
    }

    //methods for callable
    virtual void test_callable(){
        test_invalid_callable(Lsc::var::Null::null);
    }

    virtual void test_operator_assign(){
        //valid assign
        {
            Lsc::var::Null null1, null2;
            Lsc::var::Ref ref_to_null;
            null1 = null1;
            assert( null1.is_null() );

            null1 = null2;
            assert( null1.is_null() );

            null1 = (Lsc::var::IVar&)(null2);
            assert( null1.is_null() );

            null1 = ref_to_null;
            assert( null1.is_null() );

        }
        //invalid assign
        {
            Lsc::var::Null null;
            Lsc::var::Int32 i32;
            ASSERT_THROW( null = 123, Lsc::InvalidOperationException );
            ASSERT_THROW( null = 123LL, Lsc::InvalidOperationException );
            ASSERT_THROW( null = 123.456, Lsc::InvalidOperationException );
            ASSERT_THROW( null = "123", Lsc::InvalidOperationException );
            ASSERT_THROW( null = i32, Lsc::InvalidOperationException );
        }
    }

private:
};

int main(){
    TestVarNull test;
    test.test_all();
    return 0;
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
