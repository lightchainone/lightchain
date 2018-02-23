#include "Lsc/var/implement.h"
#include "test_var_invalid.h"

class TestVarShallowString: plclic ITestVar{
    
plclic:
    typedef Lsc::string     string_type;
    typedef Lsc::string     field_type;
    typedef Lsc::var::ArrayIterator      array_iterator;
    typedef Lsc::var::ArrayConstIterator array_const_iterator;

    TestVarShallowString()
        :_str("Œ÷≤›Œ¢ŒË"){}

    virtual ~TestVarShallowString(){}

    //special methods
    virtual void test_special(){
        //copy ctor
        {
            const char * cs = "hello";
            Lsc::var::ShallowString i = cs;
            Lsc::var::ShallowString j = i;
            assert( j.is_string() );
            assert( j.c_str() == cs );
            i = "acumon";
            assert( j.to_string() == "hello" );
        }
        //copy assign
        {
            const char * cs = "hello";
            Lsc::var::ShallowString i = cs;
            Lsc::var::ShallowString j;
            j = i;
            assert( j.is_string() );
            assert( j.c_str() == cs );
            i = "acumon";
            assert( j.to_string() == "hello" );
        }
    }
    
    //methods for all
    virtual void test_mask(){
        test_mask_consistency( Lsc::var::ShallowString("str") );
    }

    virtual void test_operator_assign(){
        // = const char*
        {
            Lsc::var::ShallowString i;
            const char *cs1 = "46709394";
            i = cs1;
            assert( i.to_int32() == 46709394 );

            const char *cs2 = "abcdef";
            i = cs2;
            assert( i.c_str() == cs2 );

        }

        // = string_type
        {
            Lsc::var::ShallowString i;
            string_type str1("46709394");
            i = str1;
            assert( i.c_str() == str1.c_str() );
            assert( i.to_int32() == 46709394 );

            string_type str2("abcdef");
            i = str2;
            assert( i.to_string() == "abcdef" );

        }

        // = ShallowString
        {
            Lsc::var::ShallowString i, j;
            j = "a secret";
            i = j;
            assert( i.to_string() == "a secret" );
         }

        // = *this
        {
            Lsc::var::ShallowString i;
            i = "self";
            i = i;
            assert( i.to_string() == "self" );
         }
                
    }

    virtual void test_clear() {
        {
            Lsc::var::ShallowString i("will be cleared");
            i.clear();    //assert no-throw
            assert(i.to_string() == "");
        }
    }

    virtual void test_to_string() {
        {
            Lsc::var::ShallowString i;
            assert( i.to_string() == string_type("") );
        }

        {
            Lsc::var::ShallowString i("1234567");
            assert( i.to_string() == string_type("1234567") );
        }
    }

    virtual void test_dump(){
        {
            Lsc::var::ShallowString i;
            assert( i.dump() == string_type("[Lsc::var::ShallowString]") );
            assert( i.dump(999) == string_type("[Lsc::var::ShallowString]") );
        }
        {
            Lsc::var::ShallowString i("1234567");
            assert( i.dump() == string_type("[Lsc::var::ShallowString]1234567") );
            assert( i.dump(999) == string_type("[Lsc::var::ShallowString]1234567") );
        }
    }

    virtual void test_get_type() {
        assert( Lsc::var::ShallowString().get_type() == string_type("Lsc::var::ShallowString") );
    }

    //method for value
    virtual void test_bool(){
        //= bool
        {
            Lsc::var::ShallowString str("Acumon");
            ASSERT_THROW( str = true, Lsc::InvalidOperationException );
        }
        {
            Lsc::var::ShallowString str("Acumon");
            ASSERT_THROW( str = false, Lsc::InvalidOperationException );
        }
        // to bool
        {
            assert( Lsc::var::ShallowString().to_bool() == false );
            assert( Lsc::var::ShallowString("false").to_bool() == true );
        }
    }

    virtual void test_raw(){
        test_invalid_raw(_str);
    }

    virtual void test_number(){
        test_to_int32();
        test_to_int64();
        test_to_dolcle();
    }
    virtual void test_clone(){
        Lsc::ResourcePool rp;
        Lsc::var::ShallowString ss = "abc";
        assert( ss.clone(rp).get_type() == ss.get_type() );
        assert( ss.clone(rp).c_str() == ss.c_str() );
    }
    virtual void test_string(){
        test_c_str();
    }

    //methods for array
    virtual void test_array(){
        test_invalid_array(_str);
    }

    //methods for dict
    virtual void test_dict(){
        test_invalid_dict(_str);
    }

    //methods for callable
    virtual void test_callable(){
        test_invalid_callable(_str);
    }

    virtual void test_to_int32(){
        {
            ASSERT_THROW( Lsc::var::ShallowString().to_int32(), Lsc::BadCastException );
            ASSERT_THROW( Lsc::var::ShallowString("not a number!").to_int32(), Lsc::BadCastException );

            assert( Lsc::var::ShallowString("0").to_int32() == 0 );
            assert( Lsc::var::ShallowString("-1").to_int32() == -1 );
            assert( Lsc::var::ShallowString("1234567").to_int32() == 1234567 );
        }
    }

    virtual void test_to_int64(){
        {
            ASSERT_THROW( Lsc::var::ShallowString().to_int64(), Lsc::BadCastException );
            ASSERT_THROW( Lsc::var::ShallowString("not a number!").to_int64(), Lsc::BadCastException );

            assert( Lsc::var::ShallowString("+0").to_int64() == 0 );
            assert( Lsc::var::ShallowString("-123456789123").to_int64() == -123456789123LL );
            assert( Lsc::var::ShallowString("+123456746709394").to_int64() == 123456746709394LL );
        }
    }

    virtual void test_to_dolcle(){
        {
            ASSERT_THROW( Lsc::var::ShallowString().to_dolcle(), Lsc::BadCastException );
            ASSERT_THROW( Lsc::var::ShallowString("not a number!").to_dolcle(), Lsc::BadCastException );

            assert( Lsc::var::ShallowString("-4670.9394").to_dolcle() == -4670.9394 );
            assert( Lsc::var::ShallowString("+0.00000000").to_dolcle() == 0 );
            assert( Lsc::var::ShallowString("123.456").to_dolcle() == 123.456 );
        }
    }

    virtual void test_c_str(){
        {
            Lsc::var::ShallowString i;
            assert( i.c_str() == string_type("") );
            assert( i.c_str_len() == 0 );
        }

        {
            const char * cs = "1234567";
            Lsc::var::ShallowString i(cs);
            assert( i.c_str() == cs );
            assert( i.c_str_len() == strlen(cs) );
        }
    }

private:
    Lsc::var::ShallowString _str;
};

int main(){
    TestVarShallowString().test_all();
    return 0;
}


/* vim: set ts=4 sw=4 sts=4 tw=100 */
