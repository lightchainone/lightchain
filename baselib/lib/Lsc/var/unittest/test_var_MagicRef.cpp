
#include "test_var_invalid.h"
#include "Lsc/var/implement.h"

class TestVarMagicRef: plclic ITestVar{

plclic:
    TestVarMagicRef(){}
    typedef Lsc::string     string_type;
    typedef Lsc::string     field_type;
    typedef Lsc::var::ArrayIterator      array_iterator;
    typedef Lsc::var::ArrayConstIterator array_const_iterator;

    virtual ~TestVarMagicRef(){}

    //special methods
    virtual void test_special(){
        Lsc::ResourcePool rp;
        //ref
        {
            Lsc::var::MagicRef ref(rp);
            assert( ref.ref().is_null() );
        }
        {
            Lsc::var::Int32 i32 = 123;
            Lsc::var::MagicRef ref(rp);
            ref = i32;
            assert( &ref.ref() == &i32 );
        }
        //copy ctor
        {
            Lsc::var::Int32 i32 = 123;
            Lsc::var::Int64 i64 = 456;
            Lsc::var::MagicRef i(rp);
            i = i32;
            Lsc::var::MagicRef j(i);
            assert( j.is_int32() );
            assert( j.to_int32() == 123 );
            i = i64;
            assert( j.to_int32() == 123 );
        }
        //copy assign
        {
            Lsc::var::MagicRef i(rp);
            i = 123;
            Lsc::var::MagicRef j(i);
            j = i;
            assert( j.is_int32() );
            assert( j.to_int32() == 123 );
            i = 456;
            assert( j.to_int32() == 456 );
        }
    }

    //methods for all
    virtual void test_mask(){
        Lsc::ResourcePool rp;
        test_mask_consistency( Lsc::var::MagicRef(rp) );
        Lsc::var::MagicRef mr(rp);
        mr["abc"] = 123; 
        mr["arr"][0] = "hello";
        test_mask_consistency( mr );
        test_mask_consistency( mr["abc"] );
        test_mask_consistency( mr["arr"] );
        test_mask_consistency( mr["arr"][0] );
    }

    virtual void test_operator_assign(){
        Lsc::ResourcePool rp;
        // = int
        {
            Lsc::var::MagicRef ref(rp);
            ref = 123;
            assert( ref.to_int32() == 123 );
            assert( ref.to_int64() == 123 );

            ref = 456;
            assert( ref.to_int32() == 456 );
            assert( ref.to_int64() == 456 );

        }

        // = long long
        {

            Lsc::var::MagicRef ref(rp);
            ref = 9876543219876543LL;
            ASSERT_THROW( ref.to_int32(), Lsc::OverflowException );
            assert( ref.to_int64() == 9876543219876543LL );
            ref = 1234567891234567LL;
            assert( ref.to_int64() == 1234567891234567LL );
            ASSERT_THROW( ref.to_int32(), Lsc::OverflowException );

            assert( ref.to_int64() == 1234567891234567LL );
        }

        // = dolcle
        {

            Lsc::var::MagicRef ref(rp);
            ref = 987654321.9876543;
            assert( ref.to_dolcle()== dolcle(987654321.9876543) );
            ref = 123456789.1234567;
            assert( ref.to_dolcle() == 123456789.1234567 );
            assert( ref.to_int32() == int(123456789.1234567) );
            assert( ref.to_dolcle() == 123456789.1234567 );
        }

        // = string_type
        {

            Lsc::var::MagicRef ref(rp);
            ref = "987654321.9876543";
            assert( ref.to_dolcle()== dolcle(987654321.9876543) );

            ref = "123456789.1234567";
            assert( ref.to_dolcle() == 123456789.1234567 );
            assert( ref.to_int32() == 123456789 );
            assert( ref.to_int64() == 123456789 );
            assert( ref.to_dolcle() == dolcle(123456789.1234567) );

            ref = "hello world!";
            assert( ref.to_string() == "hello world!" );

        }

        // = MagicRef
        {
            Lsc::var::MagicRef ref1(rp), ref2(rp);
            ref1 = 123;

            ref2 = ref1;
            assert( ref2.to_int32() == 123 );
            assert( ref2.to_int64() == 123 );

            ref1 = 456;
            assert( ref2.to_int32() == 456 );
            assert( ref2.to_int64() == 456 );

        }

        // = *this
        {
            Lsc::var::MagicRef ref(rp);
            ref = 123;
            ref = ref;
            assert( ref.to_int32() == 123 );
            assert( ref.to_int64() == 123 );
        }

    }

    virtual void test_size() {
        Lsc::ResourcePool rp;
        {
            Lsc::var::MagicRef ref(rp);
            ref[100] = Lsc::var::Null::null;
            assert( ref.size() == 101 );
        }

        {
            Lsc::var::MagicRef ref(rp);
            test_invalid_array(ref);
        }
    }

    virtual void test_clear() {
        Lsc::ResourcePool rp;
        {
            Lsc::var::Int32 i32(123);
            Lsc::var::MagicRef ref(rp);
            ref = i32;
            ref.clear();    //assert no-throw
            assert(ref.to_int32() == 0);
            assert(&ref.ref() == &i32 );
        }
    }

    virtual void test_dump() {
        Lsc::ResourcePool rp;
        {
            Lsc::var::MagicRef ref(rp);
            see( ref.dump().c_str() );
            assert( NULL != strstr( ref.dump().c_str(), "null" ));
        }
        {
            Lsc::var::Int32 i32;
            Lsc::var::MagicRef ref(rp);
            ref = i32;
            see( ref.dump().c_str() );
            assert( NULL != strstr( ref.dump().c_str(), "0" ) );
            assert( NULL != strstr( ref.dump(999).c_str(), "0" ) );
        }

        {
            Lsc::var::String str = "1234567";
            Lsc::var::MagicRef ref(rp);
            ref = str;
            see( ref.dump().c_str() );
            assert( NULL != strstr( ref.dump().c_str(), "1234567" ) );
            assert( NULL != strstr( ref.dump(999).c_str(), "1234567" ) );
        }
    }

    virtual void test_to_string() {
        Lsc::ResourcePool rp;
        {
            Lsc::var::Int32 i32;
            Lsc::var::MagicRef ref(rp);
            ref = i32;
            assert( ref.to_string() == string_type("0") );
        }

        {
            Lsc::var::String str = "1234567";
            Lsc::var::MagicRef ref(rp);
            ref = str;
            assert( ref.to_string() == string_type("1234567") );
        }
    }

    virtual void test_get_type() {
        Lsc::ResourcePool rp;
        assert( Lsc::var::MagicRef(rp).get_type() == string_type("Lsc::var::MagicRef()") );
        Lsc::var::MagicRef mr(rp);
        Lsc::var::Int32 i32;
        mr = i32;
        assert( mr.get_type() == string_type("Lsc::var::MagicRef(Lsc::var::Int32)") );
    }

    //method for value
    virtual void test_bool(){
        Lsc::ResourcePool rp;
        Lsc::var::MagicRef ref(rp);
        ref["ab"] = "dict";
        test_invalid_bool(ref);
        test_with_bool();
    }
    virtual void test_raw(){
        Lsc::ResourcePool rp;
        Lsc::var::MagicRef ref(rp);
        ref = 123;
        test_invalid_raw(ref);
        test_with_raw();
    }
    virtual void test_number(){
        Lsc::ResourcePool rp;
        Lsc::var::MagicRef ref(rp);
        ref["abc"][0] = 456;
        test_invalid_number(ref);
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
        */
        {
            Lsc::var::MagicRef mr(rp);
            mr = static_cast<signed char>(SCHAR_MIN);
            test_valid_number( mr, static_cast<signed char>(SCHAR_MIN) );
        }
        {
            Lsc::var::MagicRef mr(rp);
            mr = static_cast<signed char>(SCHAR_MAX);
            test_valid_number( mr, static_cast<signed char>(SCHAR_MAX) );
        }
        {
            Lsc::var::MagicRef mr(rp);
            mr = static_cast<unsigned char>(0);
            test_valid_number( mr, static_cast<unsigned char>(0) );
        }
        {
            Lsc::var::MagicRef mr(rp);
            mr = static_cast<unsigned char>(UCHAR_MAX);
            test_valid_number( mr, static_cast<unsigned char>(UCHAR_MAX) );
        }
        {
            Lsc::var::MagicRef mr(rp);
            mr = static_cast<signed short>(SHRT_MIN);
            test_valid_number( mr, static_cast<signed short>(SHRT_MIN) );
        }
        {
            Lsc::var::MagicRef mr(rp);
            mr = static_cast<signed short>(SHRT_MAX);
            test_valid_number( mr, static_cast<signed short>(SHRT_MAX) );
        }
        {
            Lsc::var::MagicRef mr(rp);
            mr = static_cast<unsigned short>(0);
            test_valid_number( mr, static_cast<unsigned short>(0) );
        }
        {
            Lsc::var::MagicRef mr(rp);
            mr = static_cast<unsigned short>(USHRT_MAX);
            test_valid_number( mr, static_cast<unsigned short>(USHRT_MAX) );
        }
        {
            Lsc::var::MagicRef mr(rp);
            mr = static_cast<signed int>(INT_MIN);
            test_valid_number( mr, static_cast<signed int>(INT_MIN) );
        }
        {
            Lsc::var::MagicRef mr(rp);
            mr = static_cast<signed int>(INT_MAX);
            test_valid_number( mr, static_cast<signed int>(INT_MAX) );
        }
        {
            Lsc::var::MagicRef mr(rp);
            mr = static_cast<unsigned int>(0);
            test_valid_number( mr, static_cast<unsigned int>(0) );
        }
        {
            Lsc::var::MagicRef mr(rp);
            mr = static_cast<unsigned int>(UINT_MAX);
            test_valid_number( mr, static_cast<unsigned int>(UINT_MAX) );
        }
        {
            Lsc::var::MagicRef mr(rp);
            mr = static_cast<signed long long>(LLONG_MIN);
            test_valid_number( mr, static_cast<signed long long>(LLONG_MIN) );
        }
        {
            Lsc::var::MagicRef mr(rp);
            mr = static_cast<signed long long>(LLONG_MAX);
            test_valid_number( mr, static_cast<signed long long>(LLONG_MAX) );
        }
        {
            Lsc::var::MagicRef mr(rp);
            mr = static_cast<unsigned long long>(0);
            test_valid_number( mr, static_cast<unsigned long long>(0) );
        }
        {
            Lsc::var::MagicRef mr(rp);
            mr = static_cast<unsigned long long>(ULLONG_MAX);
            test_valid_number( mr, static_cast<unsigned long long>(ULLONG_MAX) );
        }
        {
            Lsc::var::MagicRef mr(rp);
            mr = dolcle(LLONG_MAX)*dolcle(LLONG_MAX);
            test_valid_number( mr, dolcle(LLONG_MAX)*dolcle(LLONG_MAX) );
            mr = -dolcle(LLONG_MAX)*dolcle(LLONG_MAX);
            test_valid_number( mr, -dolcle(LLONG_MAX)*dolcle(LLONG_MAX) );
        }

    }

    virtual void test_clone(){
        Lsc::ResourcePool rp;
        {
            Lsc::var::MagicRef mr1(rp);
            mr1[0] = 123;
            mr1[1]["abc"] = ULLONG_MAX;
            Lsc::var::IVar& mr2 = mr1.clone(rp);
            assert(mr2[0].to_int32() == 123);
            assert(mr2[1]["abc"].to_uint64() == ULLONG_MAX);
        }
        {
            Lsc::var::MagicRef mr(rp);
            mr = static_cast<unsigned long long>(0);
            test_valid_number( mr.clone(rp), static_cast<unsigned long long>(0) );
        }
        {
            Lsc::var::MagicRef mr(rp);
            mr = static_cast<unsigned long long>(ULLONG_MAX);
            test_valid_number( mr.clone(rp), static_cast<unsigned long long>(ULLONG_MAX) );
        }
        {
            Lsc::var::MagicRef mr(rp);
            mr = dolcle(LLONG_MAX)*dolcle(LLONG_MAX);
            test_valid_number( mr.clone(rp), dolcle(LLONG_MAX)*dolcle(LLONG_MAX) );
        }

    }

    virtual void test_string(){
        Lsc::ResourcePool rp;
        Lsc::var::MagicRef ref(rp);
        ref[0] = 123;
        test_invalid_string(ref);
        test_c_str();
    }

    //methods for array
    virtual void test_array(){
        Lsc::ResourcePool rp;
        Lsc::var::MagicRef ref(rp);
        ref = "123";
        test_invalid_array(ref);
        test_array_get();
        test_array_set();
        test_array_del();
        test_array_iterator();
        test_array_const_iterator();
        test_array_operator_square();
    }

    //methods for dict
    virtual void test_dict(){
        Lsc::ResourcePool rp;
        Lsc::var::MagicRef ref(rp);
        ref = 123;
        test_invalid_dict(ref);
        test_dict_get();
        test_dict_set();
        test_dict_del();
        test_dict_iterator();
        test_dict_const_iterator();
        test_dict_operator_square();
    }

    //methods for callable
    virtual void test_callable(){
        Lsc::ResourcePool rp;
        Lsc::var::MagicRef ref(rp);
        ref = 123;
        test_invalid_callable(ref);
        test_operator_paren();
    }

    virtual void test_with_bool(){
        Lsc::ResourcePool rp;
        {
            Lsc::var::Bool b;
            b = true;
            Lsc::var::MagicRef ref(rp);
            ref = b;
            assert( ref.to_bool() == true );
            ref = false;
            assert( b.to_bool() == false );
        }
    }

    virtual void test_with_raw(){
        Lsc::var::raw_t raw_1("Acumon", 3);
        Lsc::var::raw_t raw_2;
        Lsc::ResourcePool rp;

        {
            Lsc::var::ShallowRaw raw1(raw_1);
            Lsc::var::MagicRef ref(rp);
            ref = raw1;
            assert( ref.to_raw().data == raw_1.data );
            assert( ref.to_raw().length == raw_1.length );
            ref = raw_2;
            assert( raw1.to_raw().data == raw_2.data );
            assert( raw1.to_raw().length == raw_2.length );
        }

    }

    virtual void test_to_int32(){
        Lsc::ResourcePool rp;
        {
            ASSERT_THROW( Lsc::var::MagicRef(rp).to_int32(), Lsc::InvalidOperationException );
        }
        {
            Lsc::var::Int32 i32 = -1;
            Lsc::var::MagicRef mr(rp);
            mr = i32;
            assert( mr.to_int32() == -1 );
        }
    }

    virtual void test_to_int64(){
        Lsc::ResourcePool rp;
        {
            ASSERT_THROW( Lsc::var::MagicRef(rp).to_int64(), Lsc::InvalidOperationException );
        }
        {
            Lsc::var::Int64 i64 = 1234567891234567LL;
            Lsc::var::MagicRef mr(rp);
            mr = i64;
            assert( mr.to_int64() == 1234567891234567LL );
        }
    }

    virtual void test_to_dolcle(){
        Lsc::ResourcePool rp;
        {
            ASSERT_THROW( Lsc::var::MagicRef(rp).to_dolcle(), Lsc::InvalidOperationException );
        }
        {
            Lsc::var::Dolcle var = 123456789.1234567;
            Lsc::var::MagicRef mr(rp);
            mr = var;
            assert( mr.to_dolcle() == 123456789.1234567 );
        }
    }

    virtual void test_c_str(){
        Lsc::ResourcePool rp;
        {
            ASSERT_THROW( Lsc::var::MagicRef(rp).c_str(), Lsc::InvalidOperationException );
        }
        {
            Lsc::var::String var = "123456789.1234567";
            Lsc::var::MagicRef mr(rp);
            mr = var;
            assert( mr.c_str() == var.c_str());
            assert( mr.c_str_len() == var.c_str_len());
        }
        {
            Lsc::var::String var = string_type("123456789.1234567");
            Lsc::var::MagicRef mr(rp);
            mr = var;
            assert( mr.c_str() == var.c_str());
            assert( mr.c_str_len() == var.c_str_len());
        }
    }

    //methods for array
    virtual void test_array_get(){
        Lsc::ResourcePool rp;
        {
            ASSERT_THROW( Lsc::var::MagicRef(rp).get(0), Lsc::InvalidOperationException );
        }
        {
            Lsc::var::Array arr;
            Lsc::var::Int32 i32 = 123;
            arr[100] = i32;
            Lsc::var::MagicRef ref(rp);
            ref = arr;
            assert( ref.get(100).to_int32() == 123 );
        }

    }

    virtual void test_array_set(){
        Lsc::ResourcePool rp;
        {
            Lsc::var::MagicRef var(rp);
            var = 123;
            ASSERT_THROW( var.set(0, var), Lsc::InvalidOperationException );

        }
        {
            Lsc::var::Array arr;
            Lsc::var::MagicRef ref(rp);
            ref = arr;
            ref.set(100, arr);
            Lsc::var::MagicRef r(rp);
            r = arr[100];
            assert( &r.ref() == &arr );
        }
    }

    virtual void test_array_del(){
        Lsc::ResourcePool rp;
        {
            Lsc::var::MagicRef var(rp);
            var = 123;
            ASSERT_THROW( var.del(0), Lsc::InvalidOperationException );
        }
        {
            Lsc::var::Array arr;
            Lsc::var::Int32 i32 = 123;
            arr[100] = i32;
            Lsc::var::MagicRef ref(rp);
            ref = arr;
            assert( ref.del(99) == false );
            assert( ref.del(100) == true );
            assert( ref.del(101) == false );
            assert( arr[100].is_null() );
        }

    }

    virtual void test_array_iterator(){
        Lsc::ResourcePool rp;
        {
            Lsc::var::MagicRef var(rp);
            var = 123;
            ASSERT_THROW( var.array_begin(), Lsc::InvalidOperationException );
            ASSERT_THROW( var.array_end(), Lsc::InvalidOperationException );
        }
        {
            Lsc::var::Array arr;
            Lsc::var::MagicRef ref(rp);
            ref = arr;
            //empty array:
            assert( ref.array_begin() == ref.array_end() );
        }
        {
            Lsc::var::Array arr;
            Lsc::var::MagicRef ref(rp);
            ref = arr;
            ref[0] = rp.create<Lsc::var::Int32>(123);
            ref[2] = rp.create<Lsc::var::Int32>(456);
            Lsc::var::Array::array_iterator iter = ref.array_begin();

            //iter => ref[0]
            assert( iter == ref.array_begin() );
            assert( iter != ref.array_end() );
            assert( iter->key() == 0 );
            assert( iter->value().is_int32() );
            assert( iter->value().to_int32() == 123 );
            assert( (++ iter)->key() == 1 );

            //iter => ref[1]
            assert( iter != ref.array_end() );
            assert( iter->key() == 1 );
            assert( iter->value().is_null() );
            iter->value() = rp.create<Lsc::var::Int32>(789);
            assert( ref[1].is_int32() );
            assert( ref[1].to_int32() == 789 );
            assert( (++ iter)->key() == 2 );

            //iter => ref[2]
            assert( iter != ref.array_end() );
            assert( iter->key() == 2 );
            assert( iter->value().is_int32() );
            assert( iter->value().to_int32() == 456 );
            assert( (++ iter) == ref.array_end() );

            //iter == ref.array_end()
            assert( iter == ref.array_end() );
        }

    }

    virtual void test_array_const_iterator(){
        /*
        Lsc::ResourcePool rp;
        {
            const Lsc::var::MagicRef ref(rp);
            ASSERT_THROW( ref.array_begin(), Lsc::InvalidOperationException );
            ASSERT_THROW( ref.array_end(), Lsc::InvalidOperationException );
        }
        {
            Lsc::var::Array arr;
            const Lsc::var::MagicRef ref(rp);
            ref = arr;
            //empty array:
            assert( ref.array_begin() == ref.array_end() );
        }

        {
            Lsc::ResourcePool rp;
            Lsc::var::Array _arr;
            _arr[0] = rp.create<Lsc::var::Int32>(123);
            _arr[2] = rp.create<Lsc::var::Int32>(456);
            const Lsc::var::MagicRef ref(rp);
            ref = _arr;
            Lsc::var::IVar::array_const_iterator iter = ref.array_begin();

            //iter => ref[0]
            assert( iter == ref.array_begin() );
            assert( iter != ref.array_end() );
            assert( iter->key() == 0 );
            assert( iter->value().is_int32() );
            assert( iter->value().to_int32() == 123 );
            assert( (++ iter)->key() == 1 );

            //iter => ref[1]
            assert( iter != ref.array_end() );
            assert( iter->key() == 1 );
            assert( iter->value().is_null() );
            assert( (++ iter)->key() == 2 );

            //iter => ref[2]
            assert( iter != ref.array_end() );
            assert( iter->key() == 2 );
            assert( iter->value().is_int32() );
            assert( iter->value().to_int32() == 456 );
            assert( (++ iter) == ref.array_end() );

            //iter == ref.array_end()
            assert( iter == ref.array_end() );
        }
        */
    }

    virtual void test_array_operator_square(){
        Lsc::ResourcePool rp;

        //non-const
        {
            Lsc::var::Array arr;
            Lsc::var::MagicRef ref(rp);
            ref = arr;
            ref[100] = rp.create<Lsc::var::Int32>(123);
            assert( ref.size() == 101 );
            assert( ref[100].is_int32() );
            assert( ref[100].to_int32() == 123);
        }
        {
            Lsc::var::Array arr;
            Lsc::var::MagicRef ref(rp);
            ref = arr;
            assert( ref[456].is_ref() );
            assert( ref[789].is_null() );
        }

        //const
        {
            Lsc::var::Array _arr;
            Lsc::var::MagicRef ref(rp);
            ref = _arr;
            _arr[100] = rp.create<Lsc::var::Int32>(123);
            assert( ref.size() == 101 );
            assert( ref[100].is_int32() );
            assert( ref[100].to_int32() == 123);

            assert( ref[0].is_null() );
        }

    }

    //methods for dict
    virtual void test_dict_iterator(){
        Lsc::ResourcePool rp;
        {
            Lsc::var::MagicRef ref(rp);
            ASSERT_THROW( ref.dict_begin(), Lsc::InvalidOperationException );
            ASSERT_THROW( ref.dict_end(), Lsc::InvalidOperationException );
        } 
        {
            Lsc::var::Dict dict;
            Lsc::var::MagicRef ref(rp);
            ref = dict;
            //empty ref:
            assert( ref.dict_begin() == ref.dict_end() );
        }

        {
            Lsc::var::Dict dict;
            Lsc::var::MagicRef ref(rp);
            ref = dict;
            ref["0"] = rp.create<Lsc::var::Int32>(123);
            ref["2"] = rp.create<Lsc::var::Int32>(456);
            ref["null"] = rp.create<Lsc::var::Null>();
            Lsc::var::IVar::dict_iterator iter = ref.dict_begin();


            assert( iter == ref.dict_begin() );
            assert( iter != ref.dict_end() );
            const Lsc::var::IVar::string_type& key0 = iter->key();
            Lsc::var::IVar& value0 = iter->value();
            assert( (++ iter)->key() != key0 );

            assert( iter != ref.dict_end() );
            const Lsc::var::IVar::string_type& key1 = iter->key();
            Lsc::var::IVar& value1 = iter->value();
            assert( (++ iter)->key() != key1 );

            assert( iter != ref.dict_end() );
            const Lsc::var::IVar::string_type& key2 = iter->key();
            Lsc::var::IVar& value2 = iter->value();
            ++iter;

            //iter == ref.dict_end()
            assert( iter == ref.dict_end() );

            assert( (key0 == "0" && value0.to_int32() == 123 && (value0 = 1230, true)) /* ref["0"] will be Int(1230) */
                    ||  (key1 == "0" && value1.to_int32() == 123 && (value1 = 1230, true))
                    ||  (key2 == "0" && value2.to_int32() == 123 && (value2 = 1230, true))
                  );

            assert( (key0 == "2" && value0.to_int32() == 456 && (value0 = 4560, true)) /* ref["0"] will be Int(4560) */
                    ||  (key1 == "2" && value1.to_int32() == 456 && (value1 = 4560, true))
                    ||  (key2 == "2" && value2.to_int32() == 456 && (value2 = 4560, true))
                  );

            assert( (key0 == "null" && value0.is_null() && (value0 = rp.create<Lsc::var::Int32>(-1), true))
                    ||  (key1 == "null" && value1.is_null() && (value1 = rp.create<Lsc::var::Int32>(-1), true))
                    ||  (key2 == "null" && value2.is_null() && (value2 = rp.create<Lsc::var::Int32>(-1), true))
                  );

            assert( ref["0"].to_int32() == 1230 );
            assert( ref["2"].to_int32() == 4560 );
            assert( ref["null"].to_int32() == -1); 

        }

    }

    virtual void test_dict_const_iterator(){
        /*
        Lsc::ResourcePool rp;
        {
            const Lsc::var::MagicRef ref(rp);
            ASSERT_THROW( ref.dict_begin(), Lsc::InvalidOperationException );
            ASSERT_THROW( ref.dict_end(), Lsc::InvalidOperationException );
        }
        {
            Lsc::var::Dict dict;
            const Lsc::var::MagicRef ref(rp);
            ref = dict;
            //empty ref:
            assert( ref.dict_begin() == ref.dict_end() );
        }

        {
            Lsc::ResourcePool rp;
            Lsc::var::Dict _dict;
            _dict["0"] = rp.create<Lsc::var::Int32>(123);
            _dict["2"] = rp.create<Lsc::var::Int32>(456);
            _dict["null"] = rp.create<Lsc::var::Null>();
            const Lsc::var::MagicRef ref = _dict;
            Lsc::var::IVar::dict_const_iterator iter = ref.dict_begin();

            assert( iter == ref.dict_begin() );
            assert( iter != ref.dict_end() );
            const Lsc::var::IVar::string_type& key0 = iter->key();
            const Lsc::var::IVar& value0 = iter->value();
            assert( (++ iter)->key() != key0 );

            assert( iter != ref.dict_end() );
            const Lsc::var::IVar::string_type& key1 = iter->key();
            const Lsc::var::IVar& value1 = iter->value();
            assert( (++ iter)->key() != key1 );

            assert( iter != ref.dict_end() );
            const Lsc::var::IVar::string_type& key2 = iter->key();
            const Lsc::var::IVar& value2 = iter->value();
            ++ iter;

            //iter == ref.dict_end()
            assert( iter == ref.dict_end() );

            assert( key0 == "0" && value0.to_int32() == 123 
                    ||  key1 == "0" && value1.to_int32() == 123 
                    ||  key2 == "0" && value2.to_int32() == 123 
                  );

            assert( key0 == "2" && value0.to_int32() == 456 
                    ||  key1 == "2" && value1.to_int32() == 456 
                    ||  key2 == "2" && value2.to_int32() == 456 
                  );

            assert( key0 == "null" && value0.is_null() 
                    ||  key1 == "null" && value1.is_null()
                    ||  key2 == "null" && value2.is_null() 
                  );

            assert( ref["0"].to_int32() == 123 );
            assert( ref["2"].to_int32() == 456 );
            assert( ref["null"].is_null()); 

        }
        */
    }

    virtual void test_dict_get(){
        Lsc::ResourcePool rp;
        {
            ASSERT_THROW( Lsc::var::MagicRef(rp).get(""), Lsc::InvalidOperationException );
        }
        {
            ASSERT_THROW( Lsc::var::MagicRef(rp).get("a key"), Lsc::InvalidOperationException );
        }
        {
            ASSERT_THROW( Lsc::var::MagicRef(rp).get(string_type("another key")), Lsc::InvalidOperationException );
        }
        //normal get
        {
            Lsc::var::Dict dict;
            Lsc::var::MagicRef mr(rp);
            mr = dict;
            assert( mr.get("").is_null() );
        }
        {
            Lsc::var::Dict dict;
            Lsc::var::MagicRef mr(rp);
            mr = dict;
            assert( mr.get("a key").is_null() );
        }
        {
            Lsc::var::Dict dict;
            Lsc::var::MagicRef mr(rp);
            mr = dict;
            assert( mr.get(string_type("another key")).is_null() );
        }
        {
            Lsc::var::Dict _dict;
            Lsc::var::MagicRef ref(rp);
            ref = _dict;
            Lsc::var::Int32 i32 = 4670;

            _dict["an int"] = i32;
            _dict["null"] = Lsc::var::Null::null;

            assert( ref.get("an int").to_int32() == i32.to_int32() );
            assert( ref.get("null").is_null() );

            assert( ref.get("unexist").is_null() );
        }
        //geek get
        {
            Lsc::var::Dict dict;
            Lsc::var::MagicRef ref(rp);
            ref = dict;
            Lsc::var::Int32 i32 = 7776;
            ref["an int"] = i32;
            ref.get("an int") = 0;
            assert( ref["an int"].to_int32() == 0 );
        }
        //get with default
        {
            Lsc::var::Dict dict;
            Lsc::var::MagicRef ref(rp);
            ref = dict;
            Lsc::var::Int32 i32 = 9394;
            Lsc::var::MagicRef ref_null(rp);
            Lsc::var::MagicRef ref_i32(rp);
            ref_i32 = i32;

            ref["an int"] = i32;
            ref["null"] = Lsc::var::Null::null;

            assert( ref.get("an int", ref_null).to_int32() == i32.to_int32() );
            assert( ref.get("null", ref_null).is_null() );
            assert( ref.get("unexist", ref_i32).to_int32() == i32.to_int32() );
        }
    }

    virtual void test_dict_set(){
        Lsc::ResourcePool rp;
        Lsc::var::Int32 i32 = 123;
        Lsc::var::MagicRef ref(rp);
        ref = i32;
        {
            ASSERT_THROW( ref.set(string_type(""),ref), Lsc::InvalidOperationException );
        }
        {
            ASSERT_THROW( ref.set("some key",ref), Lsc::InvalidOperationException );
        }
        {
            ASSERT_THROW( ref.set(string_type("another key"),ref), Lsc::InvalidOperationException );
        }
        {
            Lsc::var::Dict dict;
            Lsc::var::MagicRef ref1(rp);
            ref1 = dict;
            Lsc::var::Int32 i321 = 4670;
            ref1.set("i32", i321);
            assert( ref1["i32"].to_int32() == 4670 );
        }
    }

    virtual void test_dict_del(){
        Lsc::ResourcePool rp;
        {
            Lsc::var::MagicRef ref(rp);
            ref = 123;
            ASSERT_THROW( ref.del(""), Lsc::InvalidOperationException );
        }
        {
            Lsc::var::MagicRef ref(rp);
            ref = 123;
            ASSERT_THROW( ref.del("a key"), Lsc::InvalidOperationException );
        }
        {
            Lsc::var::MagicRef ref(rp);
            ref = 123;
            ASSERT_THROW( ref.del(string_type("another key")), Lsc::InvalidOperationException );
        }
        {
            Lsc::var::Dict dict;
            Lsc::var::MagicRef ref(rp);
            ref = dict;
            Lsc::var::Int32 i32 = 874;
            ref["del"] = i32;
            assert( ref.size() == 1 );
            assert( ref["del"].to_int32() == 874 );
            ref.del("del");
            assert( ref.size() == 0 );
            assert( ref.get("del").is_null() );
        }
    }

    virtual void test_dict_operator_square(){
        Lsc::ResourcePool rp;
        //non const
        {
            Lsc::var::MagicRef ref(rp);
            ref = 123;
            ASSERT_THROW( ref[""], Lsc::InvalidOperationException );
            ASSERT_THROW( ref["awesome key"], Lsc::InvalidOperationException );
        }
        //const 
        {
            const Lsc::var::MagicRef ref(rp);
            ASSERT_THROW( ref[""], Lsc::InvalidOperationException );
            ASSERT_THROW( ref["aweful key"], Lsc::InvalidOperationException );
        }

        //non-const
        {
            Lsc::var::Dict dict;
            Lsc::var::MagicRef ref(rp);
            ref = dict;
            ref["acumon"] = rp.create<Lsc::var::Int32>(123);
            assert( ref.size() == 1 );
            assert( ref["acumon"].is_int32() );
            assert( ref[string_type("acumon")].to_int32() == 123);

            ref["abc"] = ref["acumon"];
            assert( ref["abc"].to_int32() == 123 );
        }
        {
            Lsc::var::Dict dict;
            Lsc::var::MagicRef ref(rp);
            ref = dict;
            assert( ref[""].is_ref() );
            assert( ref[""].is_null() );
        }

        //const
        {
            Lsc::var::Dict _dict;
            _dict["acumon"] = rp.create<Lsc::var::Int32>(123);
            const Lsc::var::Dict& ref = _dict;
            assert( ref.size() == 1 );
            assert( ref[string_type("acumon")].is_int32() );
            assert( ref["acumon"].to_int32() == 123);

            ASSERT_THROW( ref["unexist"], Lsc::KeyNotFoundException );
        }

    }


    virtual void test_operator_paren(){
        Lsc::ResourcePool rp;
        {
            Lsc::var::Array args;
            Lsc::var::Function func(&echo_f, "echo");
            Lsc::var::MagicRef ref(rp); 
            ref = func;
            Lsc::var::IVar& res = ref(args, rp);
            assert( &res == &args );
        }
        {
            Lsc::var::Dict self;
            Lsc::var::Array args;
            Lsc::var::Method method(&echo_m, "echo");
            Lsc::var::MagicRef ref(rp);
            ref = method;
            Lsc::var::IVar& res = ref(self, args, rp);
            assert( &Lsc::var::Ref(res["self"]).ref() == &self );
            assert( &Lsc::var::Ref(res["args"]).ref() == &args );
        }
        {
            Lsc::var::Array args;
            Lsc::var::MagicRef mr(rp);
            ASSERT_THROW( mr(args, rp), Lsc::InvalidOperationException );
        }
        {
            Lsc::var::Dict self;
            Lsc::var::Array args;
            Lsc::var::MagicRef mr(rp);
            ASSERT_THROW( mr(self, args, rp), Lsc::InvalidOperationException );
        }
    }


private:
};

int main(){
    TestVarMagicRef().test_all();
    return 0;
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
