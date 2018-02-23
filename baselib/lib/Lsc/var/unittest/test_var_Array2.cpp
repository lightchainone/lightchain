
#include "test_var_invalid.h"
#include <Lsc/pool/Lsc_poolalloc.h>
#include <Lsc/pool/Lsc_xmempool.h>
#include <Lsc/pool/Lsc_xcompool.h>
#include <Lsc/pool/Lsc_pool.h>
#include <Lsc/exception.h>
#include <malloc.h>

#include <execinfo.h>
#include <typeinfo>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#if !(defined(__cplusplus) && (!defined(__GXX_ABI_VERSION) || __GXX_ABI_VERSION < 100))
    #include <cxxabi.h>
#endif

Lsc::xcompool g_xcompool;

class TestVarArray: plclic ITestVar{
    
plclic:
    typedef Lsc::var::IVar::string_type         string_type;
    typedef Lsc::var::IVar::field_type          field_type;
    typedef Lsc::var::ArrayIterator             array_iterator;
    typedef Lsc::var::ArrayConstIterator        array_const_iterator;
    typedef Lsc::var::Array::allocator_type     allocator_type;

    TestVarArray() {
        g_xcompool.create();
        allocator_type __alloc(&g_xcompool);
        _alloc = __alloc;
        Lsc::var::Array __array(_alloc);
        _arr = __array;
    }

    virtual ~TestVarArray(){}
    
    //special methods
    virtual void test_special(){
        //mempool
        {
            char buff[10000];
            Lsc::xmempool pool;
            pool.create(buff, sizeof(buff));
            Lsc::var::Array::allocator_type pool_alloc( &pool );
            Lsc::var::Array arr1( pool_alloc );
            Lsc::var::Array arr2;
            Lsc::var::Int32 i32(123);
            arr2[100] = i32;
            arr1 = arr2;
            assert( arr2.size() == arr2.size() );
            assert( arr2[100].to_int32() == 123 );

            Lsc::var::Array arr3(arr1);
            assert( arr3.size() == arr1.size() );
            assert( arr3[100].to_int32() == 123 );

            Lsc::var::Array arr4;
            arr4 = arr1;
            assert( arr4.size() == arr1.size() );
            assert( arr4[100].to_int32() == 123 );
            pool.clear();
        }
        
        //copy ctor
        {
            Lsc::var::Array arr1( _alloc );
            Lsc::var::Int32 i32(123);
            arr1[100] = i32;
            Lsc::var::Array arr2(arr1);
            assert( arr2.size() == arr1.size() );
            assert( arr2[100].to_int32() == 123 );
        }
        
        //assign
        {
            Lsc::var::Array arr1( _alloc );
            Lsc::var::Int32 i32(123);
            arr1[100] = i32;
            Lsc::var::Array arr2( _alloc );
            arr2 = arr1;
            assert( arr2.size() == arr1.size() );
            assert( arr2[100].to_int32() == 123 );
        }
                    
    }
    virtual void test_mask(){
        test_mask_consistency( Lsc::var::Array() );
    }
    //methods for all
    virtual void test_size() {
        {
            assert( Lsc::var::Array().size() == 0 );
        }
        {
            Lsc::var::Array arr( _alloc );
            arr[100] = Lsc::var::Null::null;
            assert( arr.size() == 101 );
        }

    }

    virtual void test_clear() {
        {
            Lsc::var::Array arr( _alloc );
            arr.clear();    //assert no-throw
            assert(arr.size() == 0);
        }
    }

    virtual void test_dump() {
        stlc();
    }

    virtual void test_to_string() {
        stlc();
    }

    virtual void test_get_type() {
        assert( Lsc::var::Array().get_type() == string_type("Lsc::var::BasicArray") );
    }

    virtual void test_clone(){
        Lsc::ResourcePool rp;
        Lsc::var::Int32 i0(0),i1(1),i2(2);
        Lsc::var::Array arr( allocator_type( &rp.get_mempool() ) );
        arr[0] = i0;
        arr[1] = i1;
        arr[2] = i2;
        Lsc::var::Array& rep = arr.clone(rp);
        assert( rep.size() == arr.size() );
        assert( rep[0].to_int32() == i0.to_int32() );
        assert( rep[1].to_int32() == i1.to_int32() );
        assert( rep[2].to_int32() == i2.to_int32() );
    }

    virtual void test_bool(){
        test_invalid_bool(_arr);
    }

    virtual void test_raw(){
        test_invalid_raw(_arr);
    }

    virtual void test_number(){
        test_invalid_number(_arr);
    }

    virtual void test_string(){
        test_invalid_string(_arr);
    }

    virtual void test_array(){
        test_array_get();
        test_array_set();
        test_array_del();
        test_array_iterator();
        test_array_const_iterator();
        test_array_operator_square();
    }

    virtual void test_dict(){
        test_invalid_dict(_arr);
    }

    virtual void test_callable(){
        test_invalid_callable(_arr);
    }

    //methods for array
    virtual void test_array_get(){
        //normal get
        {
            assert( Lsc::var::Array().get(0).is_null() );
        }
        {
            Lsc::var::Array arr( _alloc );
            arr[0] = null;
            assert( arr.get(0).is_null() );
        }
        {
            Lsc::var::Array arr( _alloc );
            Lsc::var::Int32 i32 = 1949;
            arr[123] = i32;
            assert( arr.get(123).to_int32() == 1949 );
            assert( arr.get(122).is_null() );
            assert( arr.get(456).is_null() );
        }
        //geek get
        {
            Lsc::var::Array arr( _alloc );
            Lsc::var::Int32 i32 = 1234;
            arr[0] = i32;
            arr.get(0) = 4321;
            assert( arr[0].to_int32() == 4321 );
        }
        //get with default
        {
            Lsc::var::Array arr( _alloc );
            Lsc::var::Int32 i32 = 9394;
            Lsc::var::Ref ref;
            Lsc::var::Ref ref_i32 = i32;

            arr[9] = i32;
            arr[0] = null;

            assert( arr.get(9, ref).to_int32() == i32.to_int32() );
            assert( &Lsc::var::Ref(arr.get(9, ref)).ref() == &i32 );
            assert( arr.get(0, ref).is_null() );
            assert( arr.get(9999, ref_i32).to_int32() == i32.to_int32() );
            assert( arr.get(99999, null).is_null() );
        }
    }

    virtual void test_array_set(){
        {
            Lsc::var::Array arr( _alloc );
            arr.set(127, null );
            assert( arr.size() == 128 );
            assert( arr[127].is_null() );
        }
    }

    virtual void test_array_del(){
        {
            assert( Lsc::var::Array().del(0) == false );
        }
    }

    virtual void test_array_iterator(){
        {
            Lsc::var::Array arr( _alloc );
            //empty array:
            assert( arr.array_begin() == arr.array_end() );
        }
        {
            //iterators from different array are not equal
            assert( Lsc::var::Array().array_begin() != Lsc::var::Array().array_begin() );
            assert( Lsc::var::Array().array_end() != Lsc::var::Array().array_end() );
        }

        {
            Lsc::ResourcePool rp;
            Lsc::var::Array arr( _alloc );
            arr[0] = rp.create<Lsc::var::Int32>(123);
            arr[2] = rp.create<Lsc::var::Int32>(456);
            Lsc::var::Array::array_iterator iter = arr.array_begin();

            //iter => arr[0]
            assert( iter == arr.array_begin() );
            assert( iter != arr.array_end() );
            assert( iter->key() == 0 );
            assert( iter->value().is_int32() );
            assert( iter->value().to_int32() == 123 );
            assert( (++ iter)->key() == 1 );

            //iter => arr[1]
            assert( iter != arr.array_end() );
            assert( iter->key() == 1 );
            assert( iter->value().is_null() );
            iter->value() = rp.create<Lsc::var::Int32>(789);
            assert( arr[1].is_int32() );
            assert( arr[1].to_int32() == 789 );
            assert( (++ iter)->key() == 2 );

            //iter => arr[2]
            assert( iter != arr.array_end() );
            assert( iter->key() == 2 );
            assert( iter->value().is_int32() );
            assert( iter->value().to_int32() == 456 );
            assert( (++ iter) == arr.array_end() );

            //iter == arr.array_end()
            assert( iter == arr.array_end() );
        }

        {
            Lsc::ResourcePool rp;
            Lsc::var::Array arr( _alloc );
            Lsc::var::Array::array_iterator iter = arr.array_begin();
            iter = arr.array_end();
            Lsc::var::Array::array_iterator iter2(iter);
            assert( iter2 == arr.array_end() );
        }
    }

    virtual void test_array_const_iterator(){
        {
            const Lsc::var::Array arr( _alloc );
            //empty array:
            assert( arr.array_begin() == arr.array_end() );
        }
        {
            //iterators from different array are not equal
            assert( Lsc::var::Array().array_begin() != Lsc::var::Array().array_begin() );
            assert( Lsc::var::Array().array_end() != Lsc::var::Array().array_end() );
        }

        {
            Lsc::ResourcePool rp;
            Lsc::var::Array arr1( _alloc );
            arr1[0] = rp.create<Lsc::var::Int32>(123);
            arr1[2] = rp.create<Lsc::var::Int32>(456);
            const Lsc::var::Array& arr = arr1;
            Lsc::var::Array::array_const_iterator iter = arr.array_begin();

            //iter => arr[0]
            assert( iter == arr.array_begin() );
            assert( iter != arr.array_end() );
            assert( iter->key() == 0 );
            assert( iter->value().is_int32() );
            assert( iter->value().to_int32() == 123 );
            assert( (++ iter)->key() == 1 );

            //iter => arr[1]
            assert( iter != arr.array_end() );
            assert( iter->key() == 1 );
            assert( iter->value().is_null() );
            assert( (++ iter)->key() == 2 );

            //iter => arr[2]
            assert( iter != arr.array_end() );
            assert( iter->key() == 2 );
            assert( iter->value().is_int32() );
            assert( iter->value().to_int32() == 456 );
            assert( (++ iter) == arr.array_end() );

            //iter == arr.array_end()
            assert( iter == arr.array_end() );
        }

    }

    virtual void test_array_operator_square(){
        Lsc::ResourcePool rp;

        //non-const
        {
            Lsc::var::Array arr( _alloc );
            arr[100] = rp.create<Lsc::var::Int32>(123);
            assert( arr.size() == 101 );
            assert( arr[100].is_int32() );
            assert( arr[100].to_int32() == 123);
        }
        {
            Lsc::var::Array arr( _alloc );
            assert( arr[456].is_ref() );
            assert( arr[789].is_null() );
        }

        //const
        {
            Lsc::var::Array arr1( _alloc );
            arr1[100] = rp.create<Lsc::var::Int32>(123);
            const Lsc::var::Array& arr = arr1;
            assert( arr.size() == 101 );
            assert( arr[100].is_int32() );
            assert( arr[100].to_int32() == 123);

            assert( arr[0].is_null() );
        }

    }

    virtual void test_operator_assign(){
        //valid assignments
        {
            //prepare
            Lsc::var::Array arr1( _alloc );
            Lsc::var::Array arr2( _alloc );
            Lsc::var::Int32 i32 = 123;
            arr1[0] = i32;

            //test
            arr2 = arr1;
            assert( arr2.size() == 1 );
            assert( arr2[0].to_int32() == 123 );

            arr2 = arr2;    //self assignment!
            assert( arr2.size() == 1 );
            assert( arr2[0].to_int32() == 123 );

        }
        //invalid assignments
        {
            Lsc::var::Array arr( _alloc );
            Lsc::var::Int32 i32 = 123;
            ASSERT_THROW( arr = 123, Lsc::InvalidOperationException );
            ASSERT_THROW( arr = 123LL, Lsc::InvalidOperationException );
            ASSERT_THROW( arr = 123.456, Lsc::InvalidOperationException );
            ASSERT_THROW( arr = "123", Lsc::InvalidOperationException );
            ASSERT_THROW( arr = i32, Lsc::InvalidOperationException );
        }
    }

    virtual void test_operator_paren(){
        {
            Lsc::var::Array args( _alloc );
            Lsc::ResourcePool rp;
            ASSERT_THROW( Lsc::var::Array()(args, rp), Lsc::InvalidOperationException );
        }
        {
            Lsc::var::Dict self;
            Lsc::var::Array args;
            Lsc::ResourcePool rp;
            ASSERT_THROW( Lsc::var::Array()(self, args, rp), Lsc::InvalidOperationException );
        }
    }
private:
    Lsc::var::Null   null;
    Lsc::var::Array  _arr;
    allocator_type   _alloc;
};


int main(){
    TestVarArray().test_all();
    return 0;
}
/* vim: set ts=4 sw=4 sts=4 tw=100 */
