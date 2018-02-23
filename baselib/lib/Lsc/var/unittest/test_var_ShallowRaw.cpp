#include "test_var_invalid.h"

class TestVarShallowRaw: plclic ITestVar{

plclic:
    typedef Lsc::var::IVar::string_type   string_type;
    typedef Lsc::var::IVar::field_type    field_type;
    typedef Lsc::var::ArrayIterator      array_iterator;
    typedef Lsc::var::ArrayConstIterator array_const_iterator;

    virtual ~TestVarShallowRaw(){}

    //speical methods
    virtual void test_special(){
        //ctor
        {
            Lsc::var::ShallowRaw raw;
            assert( raw.to_raw().data == NULL );
            assert( raw.to_raw().length == 0 );
        }
        {
            Lsc::var::raw_t raw_(static_cast<const void *>("hello"), strlen("hello")+1);
            Lsc::var::ShallowRaw raw(raw_);
            assert( raw.to_raw().data == raw_.data );
            assert( raw.to_raw().length == raw_.length );
        }
        {
            Lsc::var::raw_t raw_("hello", strlen("hello")+1);
            Lsc::var::ShallowRaw raw( raw_.data, raw_.length );
            assert( raw.to_raw().data == raw_.data );
            assert( raw.to_raw().length == raw_.length );
        }
        // copy ctor
        {
            Lsc::var::raw_t raw_("hello", strlen("hello")+1);
            Lsc::var::ShallowRaw raw1(raw_);
            Lsc::var::ShallowRaw raw2(raw1);
            assert( raw2.to_raw().data == raw_.data );
            assert( raw2.to_raw().length == raw_.length );
           
        }
        // copy assign
        {
            Lsc::var::raw_t raw_("hello", strlen("hello")+1);
            Lsc::var::ShallowRaw raw1(raw_);
            Lsc::var::ShallowRaw raw2;
            raw2 = raw1;
            assert( raw2.to_raw().data == raw_.data );
            assert( raw2.to_raw().length == raw_.length );
            raw2 = raw2;
            assert( raw2.to_raw().data == raw_.data );
            assert( raw2.to_raw().length == raw_.length );
        }
    }
    //methods for all
    virtual void test_mask(){
        test_mask_consistency( Lsc::var::ShallowRaw("hello",1) );
    }

    virtual void test_clear() {
        {
            Lsc::var::ShallowRaw raw;
            raw.clear();
            assert( raw.to_raw().data == NULL );
            assert( raw.to_raw().length == 0 );
        }
        {
            Lsc::var::ShallowRaw raw("hi", 1);
            raw.clear();
            assert( raw.to_raw().data == NULL );
            assert( raw.to_raw().length == 0 );
        }
    }

    virtual void test_dump(){
        stlc();
    }

    virtual void test_to_string() {
        stlc();
    }

    virtual void test_get_type() {
        assert( Lsc::var::ShallowRaw().get_type() == string_type("Lsc::var::ShallowRaw") );
    }

    //method for value
    virtual void test_bool(){
        Lsc::var::ShallowRaw raw;
        test_invalid_bool(raw);
    }
    virtual void test_raw(){
        Lsc::var::raw_t raw_("Acumon", 3);
        Lsc::var::ShallowRaw raw;
        raw = raw_;
        assert( raw.to_raw().data == raw_.data );
        assert( raw.to_raw().length == raw_.length );

        const Lsc::var::ShallowRaw const_raw(raw_);
        assert( const_raw.to_raw().data == raw_.data );
        assert( const_raw.to_raw().length == raw_.length );
        
    }
    virtual void test_number(){
        Lsc::var::ShallowRaw raw;
        test_invalid_number(raw);
    }
    virtual void test_clone(){
        Lsc::var::raw_t raw_("hello", 5);
        Lsc::var::ShallowRaw raw(raw_);
        Lsc::ResourcePool rp;
        assert( raw.clone(rp).get_type() == raw.get_type() );
        assert( raw.clone(rp).to_raw().data == raw_.data );
        assert( raw.clone(rp).to_raw().length == raw_.length );
    }

    virtual void test_string(){
        Lsc::var::ShallowRaw raw;
        test_invalid_string(raw);
    }

    //methods for array and dict
    virtual void test_array(){
        Lsc::var::ShallowRaw raw;
        test_invalid_array(raw);
    }

    //methods for dict
    virtual void test_dict(){
        Lsc::var::ShallowRaw raw;
        test_invalid_dict(raw);
    }

    //methods for callable
    virtual void test_callable(){
        Lsc::var::ShallowRaw raw;
        test_invalid_callable(raw);
    }

    virtual void test_operator_assign(){
        //valid assign
        {
            Lsc::var::raw_t raw_("", 1);
            Lsc::var::ShallowRaw raw1, raw2(raw_);
            raw1 = (Lsc::var::IVar&)(raw2);
            assert( raw1.to_raw().data == raw_.data );
            assert( raw1.to_raw().length == raw_.length );
        }
        {
            Lsc::var::raw_t raw_("", 1);
            Lsc::var::ShallowRaw raw1, raw2(raw_);
            Lsc::var::Ref ref = raw2;
            raw1 = raw2;
            assert( raw1.to_raw().data == raw_.data );
            assert( raw1.to_raw().length == raw_.length );
        }

    }

private:
};

int main(){
    TestVarShallowRaw test;
    test.test_all();
    return 0;
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
