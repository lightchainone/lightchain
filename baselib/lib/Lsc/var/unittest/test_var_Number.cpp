#include "Lsc/var/implement.h"
#include "test_var_invalid.h"
#include <climits>

class TestVarNumber: plclic ITestVar{
    
plclic:
    typedef Lsc::string     string_type;
    typedef Lsc::string     field_type;
    typedef Lsc::var::ArrayIterator      array_iterator;
    typedef Lsc::var::ArrayConstIterator array_const_iterator;

    TestVarNumber()
        :_i32(123){}

    virtual ~TestVarNumber(){}

    //special methods
    virtual void test_special(){
        //copy ctor
        {
            Lsc::var::Number<int> i = 123;
            Lsc::var::Number<int> j = i;
            assert( j.is_int32() );
            assert( j.to_int32() == 123 );
            i = 456;
            assert( j.to_int32() == 123 );
        }
        //copy assign
        {
            Lsc::var::Number<int> i = 123;
            Lsc::var::Number<int> j;
            j = i;
            assert( j.is_int32() );
            assert( j.to_int32() == 123 );
            i = 456;
            assert( j.to_int32() == 123 );
        }
    }
    
    //methods for all
    virtual void test_mask(){
/*
signed char	int8
unsigned char	uint8
signed short	int16
unsigned short	uint16
signed int	int32
unsigned int	uint32
signed long long	int64
unsigned long long	uint64
*/
        test_mask_consistency( Lsc::var::Number<signed char>() );
        test_mask_consistency( Lsc::var::Number<unsigned char>() );
        test_mask_consistency( Lsc::var::Number<signed short>() );
        test_mask_consistency( Lsc::var::Number<unsigned short>() );
        test_mask_consistency( Lsc::var::Number<signed int>() );
        test_mask_consistency( Lsc::var::Number<unsigned int>() );
        test_mask_consistency( Lsc::var::Number<signed long long>() );
        test_mask_consistency( Lsc::var::Number<unsigned long long>() );
        test_mask_consistency( Lsc::var::Number<float>() );
        test_mask_consistency( Lsc::var::Number<dolcle>() );
    }

    virtual void test_operator_assign(){
        // = int
        {
            Lsc::var::Number<int> i;
            i = 123;
            assert( i.to_int32() == 123 );
            assert( i.to_int64() == 123 );
        }

        // = long long
        {

            Lsc::var::Number<int> i;
            ASSERT_THROW( i = 1234567891234567LL, Lsc::OverflowException );
        }

        // = dolcle
        {
            Lsc::var::Number<int> i;
            i = 4670.9394;
            assert( i.to_int32() == 4670 );
            assert( i.to_int64() == 4670 );
            assert( i.to_dolcle() == 4670 );
        }

        // = const char *
        {
            Lsc::var::Number<int> i;
            i = "46709394";
            assert( i.to_int32() == 46709394 );
        }

        // = string_type
        {
            Lsc::var::Number<int> i;
            i = Lsc::var::IVar::string_type("46709394");
            assert( i.to_int32() == 46709394 );
        }

        // = Number<int>
        {
            Lsc::var::Number<int> i, j;
            j = 123;
            i = j;
            assert( i.to_int32() == 123 );
            assert( i.to_int64() == 123 );
         }

        // = *this
        {
            Lsc::var::Number<int> i;
            i = 123;
            i = i;
            assert( i.to_int32() == 123 );
            assert( i.to_int64() == 123 );
         }
                
    }

    virtual void test_clear() {
        {
            Lsc::var::Number<int> i(123);
            i.clear();    //assert no-throw
            assert(i.to_int32() == 0);
        }
    }

    virtual void test_dump() {
        {
            Lsc::var::Number<int> i;
            assert( i.dump() == string_type("[Lsc::var::Number<i>]0") );
            assert( i.dump(999) == string_type("[Lsc::var::Number<i>]0") );
        }

        {
            Lsc::var::Number<int> i(1234567);
            assert( i.dump() == string_type("[Lsc::var::Number<i>]1234567") );
            assert( i.dump(999) == string_type("[Lsc::var::Number<i>]1234567") );
        }
    }

    virtual void test_to_string() {
        {
            Lsc::var::Number<int> i;
            assert( i.to_string() == string_type("0") );
        }

        {
            Lsc::var::Number<int> i(1234567);
            assert( i.to_string() == string_type("1234567") );
        }
    }

    virtual void test_get_type() {
        assert( Lsc::var::Number<int>().get_type() == string_type("Lsc::var::Number") );
    }

    //method for value
    virtual void test_bool(){
        //= bool
        {
            Lsc::var::Number<int> i32(123);
            i32 = true;
            assert( i32.to_int32() == 1 );
        }
        {
            Lsc::var::Number<int> i32(123);
            i32 = false;
            assert( i32.to_int32() == 0 );
        }
        // to bool
        {
            assert( Lsc::var::Number<int>().to_bool() == false );
            assert( Lsc::var::Number<int>(-1).to_bool() == true );
        }
    }
    virtual void test_raw(){
        test_invalid_raw(_i32);
    }
    virtual void test_number(){
        test_to_int32();
        test_to_int64();
        test_to_dolcle();

/*
signed char	int8	SCHAR_MIN
signed char	int8	SCHAR_MAX
unsigned char	uint8	0
unsigned char	uint8	UCHAR_MAX
signed short	int16	SHRT_MIN
signed short	int16	SHRT_MAX
unsigned short	uint16	0
unsigned short	uint16	USHRT_MAX
signed int	int32	INT_MIN
signed int	int32	INT_MAX
unsigned int	uint32	0
unsigned int	uint32	UINT_MAX
signed long long	int64	LLONG_MIN
signed long long	int64	LLONG_MAX
unsigned long long	uint64	0
unsigned long long	uint64	ULLONG_MAX

        test_valid_number( Lsc::var::Number<\1>(\3), static_cast<\1>(\3) );

*/
        
        test_valid_number( Lsc::var::Number<signed char>(SCHAR_MIN), static_cast<signed char>(SCHAR_MIN) );
        test_valid_number( Lsc::var::Number<signed char>(SCHAR_MAX), static_cast<signed char>(SCHAR_MAX) );
        test_valid_number( Lsc::var::Number<unsigned char>(0), static_cast<unsigned char>(0) );
        test_valid_number( Lsc::var::Number<unsigned char>(UCHAR_MAX), static_cast<unsigned char>(UCHAR_MAX) );
        test_valid_number( Lsc::var::Number<signed short>(SHRT_MIN), static_cast<signed short>(SHRT_MIN) );
        test_valid_number( Lsc::var::Number<signed short>(SHRT_MAX), static_cast<signed short>(SHRT_MAX) );
        test_valid_number( Lsc::var::Number<unsigned short>(0), static_cast<unsigned short>(0) );
        test_valid_number( Lsc::var::Number<unsigned short>(USHRT_MAX), static_cast<unsigned short>(USHRT_MAX) );
        test_valid_number( Lsc::var::Number<signed int>(INT_MIN), static_cast<signed int>(INT_MIN) );
        test_valid_number( Lsc::var::Number<signed int>(INT_MAX), static_cast<signed int>(INT_MAX) );
        test_valid_number( Lsc::var::Number<unsigned int>(0), static_cast<unsigned int>(0) );
        test_valid_number( Lsc::var::Number<unsigned int>(UINT_MAX), static_cast<unsigned int>(UINT_MAX) );
        test_valid_number( Lsc::var::Number<signed long long>(LLONG_MIN), static_cast<signed long long>(LLONG_MIN) );
        test_valid_number( Lsc::var::Number<signed long long>(LLONG_MAX), static_cast<signed long long>(LLONG_MAX) );
        test_valid_number( Lsc::var::Number<unsigned long long>(0), static_cast<unsigned long long>(0) );
        test_valid_number( Lsc::var::Number<unsigned long long>(ULLONG_MAX), static_cast<unsigned long long>(ULLONG_MAX) );
    }

    virtual void test_clone(){
        Lsc::ResourcePool rp;
        Lsc::var::Number<int> v(123);
        assert(v.clone(rp).to_string() == v.to_string() );
        assert(v.clone(rp).get_type() == v.get_type() );

        test_valid_number( Lsc::var::Number<int>(-1).clone(rp), -1 );
        test_valid_number( Lsc::var::Number<int>(0).clone(rp), 0 );

        test_valid_number( Lsc::var::Number<signed char>(SCHAR_MIN).clone(rp), static_cast<signed char>(SCHAR_MIN) );
        test_valid_number( Lsc::var::Number<signed char>(SCHAR_MAX).clone(rp), static_cast<signed char>(SCHAR_MAX) );
        test_valid_number( Lsc::var::Number<unsigned char>(0).clone(rp), static_cast<unsigned char>(0) );
        test_valid_number( Lsc::var::Number<unsigned char>(UCHAR_MAX).clone(rp), static_cast<unsigned char>(UCHAR_MAX) );
        test_valid_number( Lsc::var::Number<signed short>(SHRT_MIN).clone(rp), static_cast<signed short>(SHRT_MIN) );
        test_valid_number( Lsc::var::Number<signed short>(SHRT_MAX).clone(rp), static_cast<signed short>(SHRT_MAX) );
        test_valid_number( Lsc::var::Number<unsigned short>(0).clone(rp), static_cast<unsigned short>(0) );
        test_valid_number( Lsc::var::Number<unsigned short>(USHRT_MAX).clone(rp), static_cast<unsigned short>(USHRT_MAX) );
        test_valid_number( Lsc::var::Number<signed int>(INT_MIN).clone(rp), static_cast<signed int>(INT_MIN) );
        test_valid_number( Lsc::var::Number<signed int>(INT_MAX).clone(rp), static_cast<signed int>(INT_MAX) );
        test_valid_number( Lsc::var::Number<unsigned int>(0).clone(rp), static_cast<unsigned int>(0) );
        test_valid_number( Lsc::var::Number<unsigned int>(UINT_MAX).clone(rp), static_cast<unsigned int>(UINT_MAX) );
        test_valid_number( Lsc::var::Number<signed long long>(LLONG_MIN).clone(rp), static_cast<signed long long>(LLONG_MIN) );
        test_valid_number( Lsc::var::Number<signed long long>(LLONG_MAX).clone(rp), static_cast<signed long long>(LLONG_MAX) );
        test_valid_number( Lsc::var::Number<unsigned long long>(0).clone(rp), static_cast<unsigned long long>(0) );
        test_valid_number( Lsc::var::Number<unsigned long long>(ULLONG_MAX).clone(rp), static_cast<unsigned long long>(ULLONG_MAX) );
    }

    virtual void test_string(){
        test_invalid_string(_i32);
    }

    //methods for array
    virtual void test_array(){
        test_invalid_array(_i32);
    }

    //methods for dict
    virtual void test_dict(){
        test_invalid_dict(_i32);
    }

    //methods for callable
    virtual void test_callable(){
        test_invalid_callable(_i32);
    }

    virtual void test_to_int32(){
        {
            assert( Lsc::var::Number<int>().to_int32() == 0 );
            assert( Lsc::var::Number<int>(-1).to_int32() == -1 );
            assert( Lsc::var::Number<int>(1234567).to_int32() == 1234567 );
        }
    }

    virtual void test_to_int64(){
        {
            assert( Lsc::var::Number<int>().to_int64() == 0 );
            assert( Lsc::var::Number<int>(-1).to_int64() == -1 );
            assert( Lsc::var::Number<int>(1234567).to_int64() == 1234567 );
        }
    }

    virtual void test_to_dolcle(){
        {
            assert( Lsc::var::Number<int>().to_dolcle() == 0 );
            assert( Lsc::var::Number<int>(-1).to_dolcle() == -1 );
            assert( Lsc::var::Number<int>(1234567).to_dolcle() == 1234567 );
        }
    }

private:
    Lsc::var::Number<int> _i32;
};

int main(){
    TestVarNumber().test_all();
    return 0;
}


