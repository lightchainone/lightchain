#include "Lsc/var/Int64.h"
#include "test_var_invalid.h"

class TestVarInt64: plclic ITestVar{

plclic:
    typedef Lsc::string     string_type;
    typedef Lsc::string     field_type;
    typedef Lsc::var::ArrayIterator      array_iterator;
    typedef Lsc::var::ArrayConstIterator array_const_iterator;
    TestVarInt64()
        :_i64(1234567891234567LL){}

    virtual ~TestVarInt64(){}

    //special methods
    virtual void test_special(){
        //copy ctor
        {
            Lsc::var::Int64 i = 1234567891234567LL;
            Lsc::var::Int64 j = i;
            assert( j.is_int64() );
            assert( j.to_int64() == 1234567891234567LL );
            i = 987654321098765LL;
            assert( j.to_int64() == 1234567891234567LL );
        }
        //copy assign
        {
            Lsc::var::Int64 i = 1234567891234567LL;
            Lsc::var::Int64 j;
            j = i;
            assert( j.is_int64() );
            assert( j.to_int64() == 1234567891234567LL );
            i = 987654321098765LL;
            assert( j.to_int64() == 1234567891234567LL );
        }
    }

    //methods for all
    virtual void test_mask(){
        test_mask_consistency( Lsc::var::Int64() );
    }

    virtual void test_operator_assign(){
        // = int
        {
            Lsc::var::Int64 i;
            i = 123;
            assert( i.to_int32() == 123 );
            assert( i.to_int64() == 123 );
        }
        // = long long
        {
            Lsc::var::Int64 i;
            i = 1234567891234567LL;
            ASSERT_THROW( i.to_int32(), Lsc::OverflowException );
            assert( i.to_int64() == 1234567891234567LL );
            assert( i.to_dolcle() == dolcle(1234567891234567LL) );
        }
        // = dolcle
        {
            Lsc::var::Int64 i;
            i = 12345678904670.9394;
            ASSERT_THROW( i.to_int32(), Lsc::OverflowException );
            assert( i.to_int64() == (long long)(12345678904670.9394) );
            assert( i.to_dolcle() ==(long long)(12345678904670.9394) );
        }

        // = const char *
        {
            Lsc::var::Int64 i;
            i = "123456746709394";
            assert( i.to_int64() == 123456746709394LL );
        }

        // = string_type
        {
            Lsc::var::Int64 i;
            i = Lsc::var::IVar::string_type("123456746709394");
            assert( i.to_int64() == 123456746709394LL );
        }

        // = Int32
        {
            Lsc::var::Int32 j;
            Lsc::var::Int64 i;
            j = 123;
            i = j;
            assert( i.to_int32() == 123 );
            assert( i.to_int64() == 123 );
        }

        // = Int64
        {
            Lsc::var::Int64 i, j;
            j = 1234567891234567LL;
            i = j;
            ASSERT_THROW( i.to_int32(), Lsc::OverflowException );
            assert( i.to_int64() == 1234567891234567LL );
        }

        // = *this
        {
            Lsc::var::Int64 i;
            i = 123;
            i = i;
            assert( i.to_int32() == 123 );
            assert( i.to_int64() == 123 );
        }

    }

    virtual void test_clear() {
        {
            Lsc::var::Int64 i(1234567891234LL);
            i.clear();    //assert no-throw
            assert(i.to_int64() == 0);
        }
    }

    virtual void test_dump() {
        {
            Lsc::var::Int64 i;
            assert( i.dump() == string_type("[Lsc::var::Int64]0") );
            assert( i.dump(999) == string_type("[Lsc::var::Int64]0") );
        }

        {
            Lsc::var::Int64 i(1234567891234567LL);
            assert( i.dump() == string_type("[Lsc::var::Int64]1234567891234567") );
            assert( i.dump(999) == string_type("[Lsc::var::Int64]1234567891234567") );
        }

    }

    virtual void test_to_string() {
        {
            Lsc::var::Int64 i;
            assert( i.to_string() == string_type("0") );
        }

        {
            Lsc::var::Int64 i(1234567891234567LL);
            assert( i.to_string() == string_type("1234567891234567") );
        }

    }

    virtual void test_get_type() {
        assert( Lsc::var::Int64().get_type() == string_type("Lsc::var::Int64") );
    }

    //method for value
    virtual void test_bool(){
        //= bool
        {
            Lsc::var::Int64 i64(123);
            i64 = true;
            assert( i64.to_int64() == 1 );
        }
        {
            Lsc::var::Int64 i64(123);
            i64 = false;
            assert( i64.to_int64() == 0 );
        }
        // to bool
        {
            assert( Lsc::var::Int64().to_bool() == false );
            assert( Lsc::var::Int64(-1).to_bool() == true );
        }
    }

    virtual void test_raw(){
        test_invalid_raw(_i64);
    }

    virtual void test_number(){
        test_to_int32();
        test_to_int64();
        test_to_dolcle();

        test_valid_number( Lsc::var::Int64(-1), -1LL );
        test_valid_number( Lsc::var::Int64(0), 0LL );
        test_valid_number( Lsc::var::Int64(LLONG_MIN), LLONG_MIN );
        test_valid_number( Lsc::var::Int64(LLONG_MAX), LLONG_MAX );
    }

    virtual void test_clone(){
        Lsc::ResourcePool rp;
        Lsc::var::Int64 v(123);
        assert(v.clone(rp).to_string() == v.to_string() );
        assert(v.clone(rp).get_type() == v.get_type() );

        test_valid_number( Lsc::var::Int64(-1).clone(rp), -1LL );
        test_valid_number( Lsc::var::Int64(0).clone(rp), 0LL );
        test_valid_number( Lsc::var::Int64(LLONG_MIN).clone(rp), LLONG_MIN );
        test_valid_number( Lsc::var::Int64(LLONG_MAX).clone(rp), LLONG_MAX );
    }

    virtual void test_string(){
        test_invalid_string(_i64);
    }

    //methods for array
    virtual void test_array(){
        test_invalid_array(_i64);
    }

    //methods for dict
    virtual void test_dict(){
        test_invalid_dict(_i64);
    }

    //methods for callable
    virtual void test_callable(){
        test_invalid_callable(_i64);
    }


    virtual void test_to_int32(){
        {
            assert( Lsc::var::Int64().to_int32() == 0 );
            assert( Lsc::var::Int64(-1).to_int32() == -1 );
            assert( Lsc::var::Int64(1234567).to_int32() == 1234567 );
        }
    }

    virtual void test_to_int64(){
        {
            assert( Lsc::var::Int64().to_int64() == 0 );
            assert( Lsc::var::Int64(-1).to_int64() == -1 );
            assert( Lsc::var::Int64(1234567).to_int64() == 1234567 );
        }
    }

    virtual void test_to_dolcle(){
        {
            assert( Lsc::var::Int64().to_dolcle() == 0 );
            assert( Lsc::var::Int64(-1).to_dolcle() == -1 );
            assert( Lsc::var::Int64(1234567).to_dolcle() == 1234567 );
        }
    }

private:
    Lsc::var::Int64 _i64;
};

int main(){
    TestVarInt64().test_all();
    return 0;
}


