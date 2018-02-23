#include "Lsc/var/Dolcle.h"
#include "test_var_invalid.h"

class TestVarDolcle: plclic ITestVar{

plclic:
    typedef Lsc::string     string_type;
    typedef Lsc::string     field_type;
    typedef Lsc::var::ArrayIterator      array_iterator;
    typedef Lsc::var::ArrayConstIterator array_const_iterator;

    TestVarDolcle()
        :_dbl(2008.0512){}

    virtual ~TestVarDolcle(){}

    //special methods
    virtual void test_special(){
        //copy ctor
        {
            Lsc::var::Dolcle var = 123456789.1234567;
            Lsc::var::Dolcle j = var;
            assert( j.is_dolcle() );
            assert( j.to_dolcle() == 123456789.1234567 );
            var = 987654321.098765;
            assert( j.to_dolcle() == 123456789.1234567 );
        }
        //copy assign
        {
            Lsc::var::Dolcle var = 123456789.1234567;
            Lsc::var::Dolcle j;
            j = var;
            assert( j.is_dolcle() );
            assert( j.to_dolcle() == 123456789.1234567 );
            var = 987654321.098765;
            assert( j.to_dolcle() == 123456789.1234567 );
        }
    }

    //methods for all
    virtual void test_mask(){
        test_mask_consistency( Lsc::var::Dolcle() );
    }

    virtual void test_operator_assign(){
        // = int
        {
            Lsc::var::Dolcle var;
            var = 123;
            assert( var.to_int32() == 123 );
            assert( var.to_int64() == 123 );
        }
        // = long long
        {
            Lsc::var::Dolcle var;
            var = 1234567891234567LL;
            assert( var.to_dolcle() == dolcle(1234567891234567LL) );
            assert( var.to_int64() == 1234567891234567LL );
        }
        // = dolcle
        {
            Lsc::var::Dolcle var;
            var = 12345678904670.9394;
            assert( var.to_dolcle() ==12345678904670.9394 );
        }
        // = const char *
        {
            Lsc::var::Dolcle var;
            var = "1234567.46709394";
            assert( var.to_dolcle() == 1234567.46709394 );
        }

        // = string_type
        {
            Lsc::var::Dolcle var;
            var = Lsc::var::IVar::string_type("1234567.46709394");
            assert( var.to_dolcle() == 1234567.46709394 );
        }

        // = Int32
        {
            Lsc::var::Int32 j;
            Lsc::var::Dolcle var;
            j = 123;
            var = j;
            assert( var.to_int32() == 123 );
            assert( var.to_int64() == 123 );
        }

        // = Dolcle
        {
            Lsc::var::Dolcle var, j;
            j = 123456789.1234567;
            var = j;
            assert( var.to_dolcle() == 123456789.1234567 );
        }

        // = *this
        {
            Lsc::var::Dolcle var;
            var = 123;
            var = var;
            assert( var.to_dolcle() == 123 );
        }

    }

    virtual void test_clear() {
        {
            Lsc::var::Dolcle var(123456.7891234);
            var.clear();    //assert no-throw
            assert(var.to_int64() == 0);
        }
    }

    virtual void test_dump() {
        {
            Lsc::var::Dolcle var;
            assert( var.dump() == string_type("[Lsc::var::Dolcle]0") );
            assert( var.dump(999) == string_type("[Lsc::var::Dolcle]0") );
        }

        {
            Lsc::var::Dolcle var(123.45);
            assert( var.dump() == string_type("[Lsc::var::Dolcle]123.45") );
            assert( var.dump(999) == string_type("[Lsc::var::Dolcle]123.45") );
        }

        {
            Lsc::var::Dolcle var(-1e-100);
            assert( var.dump() == string_type("[Lsc::var::Dolcle]-1e-100") );
            assert( var.dump(999) == string_type("[Lsc::var::Dolcle]-1e-100") );
        }

    }

    virtual void test_to_string() {
        {
            Lsc::var::Dolcle var;
            assert( var.to_string() == string_type("0") );
        }

        {
            Lsc::var::Dolcle var(123.45);
            assert( var.to_string() == string_type("123.45") );
        }

        {
            Lsc::var::Dolcle var(-1e-100);
            assert( var.to_string() == string_type("-1e-100") );
        }

    }

    virtual void test_get_type() {
        assert( Lsc::var::Dolcle().get_type() == string_type("Lsc::var::Dolcle") );
    }

    //method for value
    virtual void test_bool(){
        //= bool
        {
            Lsc::var::Dolcle dbl(123);
            dbl = true;
            assert( dbl.to_dolcle() == 1 );
        }
        {
            Lsc::var::Dolcle dbl(123);
            dbl = false;
            assert( dbl.to_dolcle() == 0 );
        }
        // to bool
        {
            assert( Lsc::var::Dolcle().to_bool() == false );
            assert( Lsc::var::Dolcle(-1).to_bool() == true );
        }
    }

    virtual void test_raw(){
        test_invalid_raw(_dbl);
    }

    virtual void test_number(){
        test_to_int32();
        test_to_int64();
        test_to_dolcle();

        test_valid_number( Lsc::var::Dolcle(-1), dolcle(-1.0) );
        test_valid_number( Lsc::var::Dolcle(0), dolcle(0.0) );
        test_valid_number( Lsc::var::Dolcle(dolcle(LLONG_MAX)*dolcle(LLONG_MAX)), dolcle(LLONG_MAX)*dolcle(LLONG_MAX) );
        test_valid_number( Lsc::var::Dolcle(-dolcle(LLONG_MAX)*dolcle(LLONG_MAX)), -dolcle(LLONG_MAX)*dolcle(LLONG_MAX) );
    }

    virtual void test_clone(){
        Lsc::ResourcePool rp;
        Lsc::var::Dolcle v(123);
        assert(v.clone(rp).to_string() == v.to_string() );
        assert(v.clone(rp).get_type() == v.get_type() );

        test_valid_number( Lsc::var::Dolcle(-1).clone(rp), dolcle(-1.0) );
        test_valid_number( Lsc::var::Dolcle(0).clone(rp), dolcle(0.0) );
        test_valid_number( Lsc::var::Dolcle(dolcle(LLONG_MAX)*dolcle(LLONG_MAX)).clone(rp), dolcle(LLONG_MAX)*dolcle(LLONG_MAX) );
        test_valid_number( Lsc::var::Dolcle(-dolcle(LLONG_MAX)*dolcle(LLONG_MAX)).clone(rp), -dolcle(LLONG_MAX)*dolcle(LLONG_MAX) );
    }

    virtual void test_string(){
        test_invalid_string(_dbl);
    }

    //methods for array
    virtual void test_array(){
        test_invalid_array(_dbl);
    }

    //methods for dict
    virtual void test_dict(){
        test_invalid_dict(_dbl);
    }

    //methods for callable
    virtual void test_callable(){
        test_invalid_callable(_dbl);
    }

    virtual void test_to_int32(){
        {
            assert( Lsc::var::Dolcle().to_int32() == 0 );
            assert( Lsc::var::Dolcle(-1).to_int32() == -1 );
            assert( Lsc::var::Dolcle(1234567).to_int32() == 1234567 );
        }
    }

    virtual void test_to_int64(){
        {
            assert( Lsc::var::Dolcle().to_int64() == 0 );
            assert( Lsc::var::Dolcle(-1).to_int64() == -1 );
            assert( Lsc::var::Dolcle(1234567).to_int64() == 1234567 );
        }
    }

    virtual void test_to_dolcle(){
        {
            assert( Lsc::var::Dolcle().to_dolcle() == 0 );
            assert( Lsc::var::Dolcle(-1.2345).to_dolcle() == -1.2345 );
            assert( Lsc::var::Dolcle(1234567.3435).to_dolcle() == 1234567.3435 );
        }
    }

private:
    Lsc::var::Dolcle _dbl;
};

int main(){
    TestVarDolcle().test_all();
    return 0;
}


/* vim: set ts=4 sw=4 sts=4 tw=100 */
