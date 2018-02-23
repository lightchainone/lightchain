
#include<new>
#include<iostream>
#include<cassert>
#include "test_var_invalid.h"
#include "Lsc/pool.h"
#include "Lsc/pool/Lsc_pool.h"
#include "Lsc/var/Dict.h"
#include "Lsc/var/assign.h"
#include "Lsc/ResourcePool.h"
#include "Lsc/pool/Lsc_xmempool.h"

#if !(defined(__cplusplus) && (!defined(__GXX_ABI_VERSION) || __GXX_ABI_VERSION < 100)) 
#include <cxxabi.h>
#endif

Lsc::xmempool g_mempool;
Lsc::mempool& g_pool = g_mempool;

template<class dict_t, template<class>class creater_t>
class TestVarDict: plclic ITestVar{
plclic:
    typedef Lsc::var::IVar::string_type     string_type;
    typedef Lsc::var::IVar::field_type      field_type;
    typedef Lsc::var::DictIterator          dict_iterator;
    typedef Lsc::var::DictConstIterator     dict_const_iterator;
    typedef Lsc::var::Dict::allocator_type  allocator_type;

    virtual ~TestVarDict(){}
    
    //special methods
    virtual void test_special(){
        //pool
        {
            char buff[10000];
            Lsc::xmempool pool;
            pool.create(buff, sizeof(buff));

            allocator_type pool_alloc( &pool );
            Lsc::var::Dict dict1( pool_alloc );
            Lsc::var::Int32 i32 = 123;
            dict1["null"] = null;
            dict1["i32"] = i32;
            
            Lsc::var::Dict dict2 = dict1;
            assert( dict2.get("null").is_null() );
            assert( dict2.get("i32").to_int32() == 123 );

            pool.clear();
        }
        
        //copy ctor
        {
            //prepare
            dict_t& dict1 = _creater.create();
            Lsc::var::Int32 i32 = 123;
            dict1["null"] = null;
            dict1["i32"]  = i32;
            
            //test
            dict_t dict2 = dict1;
            assert( dict2.get("null").is_null() );
            assert( dict2.get("i32").to_int32() == 123 );
        }
        //copy assign
        {
            //prepare
            dict_t& dict1 = _creater.create();
            Lsc::var::Int32 i32 = 123;
            dict1["null"] = null;
            dict1["i32"]  = i32;
            
            //test
            dict_t& dict2 = _creater.create();
            dict2 = dict1;
            assert( dict2.get("null").is_null() );
            assert( dict2.get("i32").to_int32() == 123 );

            //不同实现类型之间的赋值
            Lsc::var::StdMapDict smd;
            smd = dict1;
            assert( smd["null"].is_null() );
            assert( smd["i32"].to_int32() == 123 );

            Lsc::var::GnuHashDict ghd;
            ghd = dict1;
            assert( ghd["null"].is_null() );
            assert( ghd["i32"].to_int32() == 123 );
        }
    }
    //methods for all
    virtual void test_mask(){
        test_mask_consistency( Lsc::var::Dict() );
    }

    virtual void test_size() {
        {
            assert( _creater.create().size() == 0 );
        }
        {
            dict_t& dict = _creater.create();
            dict["abc"] = Lsc::var::Null::null;
            assert( dict.size() == 1 );
        }

    }

    virtual void test_clear() {
        {
            dict_t& dict = _creater.create();
            dict.clear();    //assert no-throw
            assert(dict.size() == 0);
        }
    }

    virtual void test_dump() {
        {
            stlc();
        }
    }

    virtual void test_to_string() {
        {
            stlc();
        }
    }

    virtual void test_get_type() {
        assert( _creater.create().get_type() == string_type("Lsc::var::BasicDict") );
    }

    virtual void test_bool(){
        Lsc::var::IVar& dict = _creater.create();
        test_invalid_bool(dict);
    }

    virtual void test_raw(){
        Lsc::var::IVar& dict = _creater.create();
        test_invalid_raw(dict);
    }

    virtual void test_number(){
        Lsc::var::IVar& dict = _creater.create();
        test_invalid_number(dict);
    }
    virtual void test_clone(){
        Lsc::ResourcePool rp;
        Lsc::var::Dict dict;
        Lsc::var::Int32 i32(123);
        Lsc::var::Int64 i64(456);
        Lsc::var::assign( dict, "hello", i32, "world", i64 ); // dict = {"hello":i32, "world":i64}
        {
            //Shallow copy
            Lsc::var::Dict& cl = dict.clone(rp);
            assert( cl.get_type() == dict.get_type() );
            assert( cl.size() == dict.size() );
            assert( cl["hello"].to_int32() == 123 );
            assert( cl["world"].to_int64() == 456 );
            assert( &Lsc::var::Ref(cl["hello"]).ref() == &i32 );
            assert( &Lsc::var::Ref(cl["world"]).ref() == &i64 );
        }
        {
            //deep copy
            Lsc::var::Dict& cl = dict.clone(rp, true);
            assert( cl.get_type() == dict.get_type() );
            assert( cl.size() == dict.size() );
            assert( cl["hello"].to_int32() == 123 );
            assert( cl["world"].to_int64() == 456 );
            assert( &Lsc::var::Ref(cl["hello"]).ref() != &i32 );
            assert( &Lsc::var::Ref(cl["world"]).ref() != &i64 );
        }
    }
    virtual void test_string(){
        Lsc::var::IVar& dict = _creater.create();
        test_invalid_string(dict);
    }

    virtual void test_array(){
        Lsc::var::IVar& dict = _creater.create();
        test_invalid_array(dict);
    }

    virtual void test_dict(){
        test_dict_get();
        test_dict_set();
        test_dict_del();
        test_dict_iterator();
        test_dict_const_iterator();
        test_dict_operator_square();
    }

    virtual void test_callable(){
        Lsc::var::IVar& dict = _creater.create();
        test_invalid_callable(dict);
    }

    virtual void test_dict_iterator(){
        {
            dict_t dict = _creater.create();
            //empty dict:
            assert( dict.dict_begin() == dict.dict_end() );
        }
        {
            //iterators from different dict are not equal
            //true to std::map
            //not true to __gnu_cxx::hash_map, since end() yields iterators that considered equal
            //disable such test case
            /*
            dict_t dict1 = _creater.create();
            dict_t dict2 = _creater.create();
            dict1["null"]= Lsc::var::Null::null;
            dict2["null"]= Lsc::var::Null::null;
            assert( dict1.dict_begin() != dict2.dict_begin() );
            assert( dict1.dict_end() != dict2.dict_end() );
            */
        }

        {
            Lsc::ResourcePool rp;
            dict_t& dict = _creater.create();
            dict["0"] = rp.template create<Lsc::var::Int32>(123);
            dict["2"] = rp.template create<Lsc::var::Int32>(456);
            dict["null"] = rp.template create<Lsc::var::Null>();
            Lsc::var::Dict::dict_iterator iter = dict.dict_begin();


            assert( iter == dict.dict_begin() );
            assert( iter != dict.dict_end() );
            const Lsc::var::IVar::string_type& key0 = iter->key();
            Lsc::var::IVar& value0 = iter->value();
            assert( (++ iter)->key() != key0 );

            assert( iter != dict.dict_end() );
            const Lsc::var::IVar::string_type& key1 = iter->key();
            Lsc::var::IVar& value1 = iter->value();
            assert( (++ iter)->key() != key1 );

            assert( iter != dict.dict_end() );
            const Lsc::var::IVar::string_type& key2 = iter->key();
            Lsc::var::IVar& value2 = iter->value();

            iter == dict.dict_end();
            //assert( (++ iter)->key() != key2 );

            assert( ++ iter == dict.dict_end() );

            //无序的东西真难测
            assert( (key0 == "0" && value0.to_int32() == 123 && (value0 = 1230, true)) // dict["0"] will be Int(1230) 
                ||  (key1 == "0" && value1.to_int32() == 123 && (value1 = 1230, true))
                ||  (key2 == "0" && value2.to_int32() == 123 && (value2 = 1230, true))
                );


            assert( (key0 == "2" && value0.to_int32() == 456 && (value0 = 4560, true)) // dict["0"] will be Int(4560) 
                ||  (key1 == "2" && value1.to_int32() == 456 && (value1 = 4560, true))
                ||  (key2 == "2" && value2.to_int32() == 456 && (value2 = 4560, true))
                );
            assert( (key0 == "null" && value0.is_null() && (value0 = rp.template create<Lsc::var::Int32>(-1), true))
                ||  (key1 == "null" && value1.is_null() && (value1 = rp.template create<Lsc::var::Int32>(-1), true))
                ||  (key2 == "null" && value2.is_null() && (value2 = rp.template create<Lsc::var::Int32>(-1), true))
                );

            assert( dict["0"].to_int32() == 1230 );
            assert( dict["2"].to_int32() == 4560 );
            assert( dict["null"].to_int32() == -1); 

        }

    }

    virtual void test_dict_const_iterator(){
        {
            const dict_t& dict = _creater.create();
            //empty dict:
            assert( dict.dict_begin() == dict.dict_end() );
        }

        {
            Lsc::ResourcePool rp;
            dict_t& _dict = _creater.create();
            _dict["0"] = rp.template create<Lsc::var::Int32>(123);
            _dict["2"] = rp.template create<Lsc::var::Int32>(456);
            _dict["null"] = rp.template create<Lsc::var::Null>();
            const dict_t& dict = _dict;
            Lsc::var::Dict::dict_const_iterator iter = dict.dict_begin();

            assert( iter == dict.dict_begin() );
            assert( iter != dict.dict_end() );
            const Lsc::var::IVar::string_type& key0 = iter->key();
            const Lsc::var::IVar& value0 = iter->value();
            assert( (++ iter)->key() != key0 );

            assert( iter != dict.dict_end() );
            const Lsc::var::IVar::string_type& key1 = iter->key();
            const Lsc::var::IVar& value1 = iter->value();
            assert( (++ iter)->key() != key1 );

            assert( iter != dict.dict_end() );
            const Lsc::var::IVar::string_type& key2 = iter->key();
            const Lsc::var::IVar& value2 = iter->value();
            //assert( (++ iter)->key() != key2 );

            //iter == dict.dict_end()
            assert( (++iter) == dict.dict_end() );

            //无序的东西真难测
            assert( (key0 == "0" && value0.to_int32() == 123) 
                ||  (key1 == "0" && value1.to_int32() == 123) 
                ||  (key2 == "0" && value2.to_int32() == 123) 
                );

            assert( (key0 == "2" && value0.to_int32() == 456) 
                ||  (key1 == "2" && value1.to_int32() == 456) 
                ||  (key2 == "2" && value2.to_int32() == 456) 
                );

            assert( (key0 == "null" && value0.is_null()) 
                ||  (key1 == "null" && value1.is_null())
                ||  (key2 == "null" && value2.is_null()) 
                );

            assert( dict["0"].to_int32() == 123 );
            assert( dict["2"].to_int32() == 456 );
            assert( dict["null"].is_null()); 

        }

    }

    virtual void test_dict_get(){
        //normal get
        {
            assert( _creater.create().get("").is_null() );
        }
        {
            assert( _creater.create().get("a key").is_null() );
        }
        {
            assert( _creater.create().get(string_type("another key")).is_null() );
        }
        {
            dict_t _dict = _creater.create();
            const dict_t& dict = _dict;
            Lsc::var::Int32 i32 = 4670;

            _dict["an int"] = i32;
            _dict["null"] = null;

            assert( dict.get("an int").to_int32() == i32.to_int32() );
            assert( dict.get(string_type("an int")).to_int32() == i32.to_int32() );
            assert( dict.get("null").is_null() );
           
            assert( dict.get("unexist").is_null() );
        }
        //geek get
        {
            dict_t& dict = _creater.create();
            Lsc::var::Int32 i32 = 7776;
            dict["an int"] = i32;
            dict.get("an int") = 0;
            assert( dict["an int"].to_int32() == 0 );
        }
        //get with default
        {
            dict_t& dict = _creater.create();
            Lsc::var::Int32 i32 = 9394;
            Lsc::var::Ref ref;
            Lsc::var::Ref ref_i32 = i32;

            dict["an int"] = i32;
            dict["null"] = null;

            assert( dict.get("an int", ref).to_int32() == i32.to_int32() );
            assert( dict.get(string_type("an int"), ref).to_int32() == i32.to_int32() );
            assert( dict.get("null", ref).is_null() );
            assert( dict.get("unexist", ref_i32).to_int32() == i32.to_int32() );
        }
    }

    virtual void test_dict_set(){
        {
            dict_t& dict = _creater.create();
            Lsc::var::Int32 i32 = 4670;
            dict.set("i32", i32);
            assert( dict["i32"].to_int32() == 4670 );
        }
    }

    virtual void test_dict_del(){
        {
            dict_t& dict = _creater.create();
            Lsc::var::Int32 i32 = 874;
            dict["del"] = i32;
            assert( dict.size() == 1 );
            assert( dict["del"].to_int32() == 874 );
            dict.del("del");
            assert( dict.size() == 0 );
            assert( dict.get("del").is_null() );
        }
    }

    virtual void test_dict_operator_square(){

        //non-const
        {
            Lsc::ResourcePool rp;
            dict_t& dict = _creater.create();
            dict["acumon"] = rp.template create<Lsc::var::Int32>(123);
            assert( dict.size() == 1 );
            assert( dict["acumon"].is_int32() );
            assert( dict[string_type("acumon")].to_int32() == 123);

            dict["abc"] = dict["acumon"];
            assert( dict["abc"].to_int32() == 123 );
        }
        {
            dict_t& dict = _creater.create();
            assert( dict[""].is_ref() );
            assert( dict[""].is_null() );
        }

        //const
        {
            Lsc::ResourcePool rp;
            dict_t& _dict = _creater.create();
            _dict["acumon"] = rp.template create<Lsc::var::Int32>(123);
            const dict_t& dict = _dict;
            assert( dict.size() == 1 );
            assert( dict[string_type("acumon")].is_int32() );
            assert( dict["acumon"].to_int32() == 123);

            ASSERT_THROW( dict["unexist"], Lsc::KeyNotFoundException );
        }

    }

    virtual void test_operator_assign(){
        //valid assignments
        {
            //prepare
            dict_t& dict1 = _creater.create();
            dict_t& dict2 = _creater.create();
            Lsc::var::Int32 i32 = 123;
            dict1["hi"] = i32;

            //test
            dict2 = dict1;
            assert( dict2.size() == 1 );
            assert( dict2["hi"].to_int32() == 123 );

            dict2 = dict2;    //self assignment!
            assert( dict2.size() == 1 );
            assert( dict2["hi"].to_int32() == 123 );

        }

    }

private:
    Lsc::var::Null          null;
    creater_t<dict_t>  _creater;
};


template<class dict_t>
class DefaultCreater{
plclic:
    dict_t& create(){
        return _rp.template create<dict_t>();
    }
private:
    Lsc::ResourcePool _rp;
};

template<class dict_t>
class CapacityCreater{
plclic:
    static size_t capacity;
    dict_t& create(){
        return _rp.template create<dict_t>(capacity);
    }
private:
    Lsc::ResourcePool _rp;
};

template<class dict_t>
class AllocatorCreater{
plclic:
    static typename dict_t::allocator_type alloc;
    dict_t& create(){
        return _rp.template create<dict_t>(alloc);
    }
private:
    Lsc::ResourcePool _rp;
};

template<class dict_t>
class PoolAllocatorCreater{
plclic:
    typedef typename dict_t::allocator_type allocator_type;
    dict_t& create(){
        return _rp.template create<dict_t>(allocator_type(&g_pool));
    }
private:
    Lsc::ResourcePool _rp;
};

template<class dict_t>
class CapacityAllocatorCreater{
plclic:
    static size_t capacity;
    static typename dict_t::allocator_type alloc;
    dict_t& create(){
        return _rp.template create<dict_t>(capacity, alloc);
    }
private:
    Lsc::ResourcePool _rp;
};

template<class dict_t>
class CapacityPoolAllocatorCreater{
plclic:
    static size_t capacity;
    typedef typename dict_t::allocator_type allocator_type;
    dict_t& create(){
        return _rp.template create<dict_t>(capacity, allocator_type(&g_pool));
    } 
private:
    Lsc::ResourcePool _rp;
};


//configuration
template<>
size_t CapacityCreater<Lsc::var::StdMapDict>::capacity                  = 1000;
template<>
size_t CapacityAllocatorCreater<Lsc::var::StdMapDict>::capacity         = 1000;
template<>
size_t CapacityPoolAllocatorCreater<Lsc::var::StdMapDict>::capacity     = 1000;
template<>
size_t CapacityCreater<Lsc::var::GnuHashDict>::capacity                 = 1000;
template<>
size_t CapacityAllocatorCreater<Lsc::var::GnuHashDict>::capacity        = 1000;
template<>
size_t CapacityPoolAllocatorCreater<Lsc::var::GnuHashDict>::capacity    = 1000;


template<>
Lsc::var::StdMapDict::allocator_type 
AllocatorCreater<Lsc::var::StdMapDict>::alloc = Lsc::var::StdMapDict::allocator_type();

template<>
Lsc::var::StdMapDict::allocator_type 
CapacityAllocatorCreater<Lsc::var::StdMapDict>::alloc = Lsc::var::StdMapDict::allocator_type();

template<>
Lsc::var::GnuHashDict::allocator_type 
AllocatorCreater<Lsc::var::GnuHashDict>::alloc = Lsc::var::GnuHashDict::allocator_type();

template<>
Lsc::var::GnuHashDict::allocator_type 
CapacityAllocatorCreater<Lsc::var::GnuHashDict>::alloc = Lsc::var::GnuHashDict::allocator_type();



/**
 *
 * 测试组合共有12个，分别是
 *
 * std::map             std::allocator      DefaultCreater
 * std::map             std::allocator      CapacityCreater
 * std::map             std::allocator      AllocatorCreater
 * std::map             std::allocator      CapacityAllocatorCreater
 *
 * __gnu_cxx::hash_map  std::allocator      DefaultCreater
 * __gnu_cxx::hash_map  std::allocator      CapacityCreater
 * __gnu_cxx::hash_map  std::allocator      AllocatorCreater
 * __gnu_cxx::hash_map  std::allocator      CapacityAllocatorCreater
 *
 * std::map             Lsc::pool_allocator AllocatorCreater
 * std::map             Lsc::pool_allocator CapacityAllocatorCreater
 * __gnu_cxx::hash_map  Lsc::pool_allocator AllocatorCreater
 * __gnu_cxx::hash_map  Lsc::pool_allocator CapacityAllocatorCreater
 *
**/
char g_membuf[104857600];
int main(){

    //init
    g_mempool.create(g_membuf, sizeof(g_membuf));

    //test
    TestVarDict<Lsc::var::StdMapDict, DefaultCreater>().test_all();
    g_mempool.clear();
    TestVarDict<Lsc::var::StdMapDict, CapacityCreater>().test_all();
    g_mempool.clear();
    TestVarDict<Lsc::var::StdMapDict, AllocatorCreater>().test_all();
    g_mempool.clear();
    TestVarDict<Lsc::var::StdMapDict, PoolAllocatorCreater>().test_all();
    g_mempool.clear();
    TestVarDict<Lsc::var::StdMapDict, CapacityAllocatorCreater>().test_all();
    g_mempool.clear();
    TestVarDict<Lsc::var::StdMapDict, CapacityPoolAllocatorCreater>().test_all();
    g_mempool.clear();

    TestVarDict<Lsc::var::GnuHashDict, DefaultCreater>().test_all();
    g_mempool.clear();
    TestVarDict<Lsc::var::GnuHashDict, CapacityCreater>().test_all();
    g_mempool.clear();
    TestVarDict<Lsc::var::GnuHashDict, AllocatorCreater>().test_all();
    g_mempool.clear();
    TestVarDict<Lsc::var::GnuHashDict, PoolAllocatorCreater>().test_all();
    g_mempool.clear();
    TestVarDict<Lsc::var::GnuHashDict, CapacityAllocatorCreater>().test_all();
    g_mempool.clear();
    TestVarDict<Lsc::var::GnuHashDict, CapacityPoolAllocatorCreater>().test_all();
    g_mempool.clear();
    
    return 0;
}
/* vim: set ts=4 sw=4 sts=4 tw=100 */
