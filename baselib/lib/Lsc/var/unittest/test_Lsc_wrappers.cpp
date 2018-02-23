
#include"Lsc/ResourcePool.h"
#include<cmath>
#include<typeinfo>
#include<iostream>
using namespace std;
using namespace Lsc;
#define see(x) do{  \
    cerr<<__FILE__<<":"<<__LINE__<<": " <<(#x)<<" = "<<(x)<<" ("<<typeid(x).name()<<")"<<endl;\
}while(0)

#include<cassert>


// #define NG_TEST_DEBUG 
/*
 *  __construct_aux [Lsc]
    __construct_aux [Lsc]
    Lsc::construct [Lsc]
    Lsc::construct [Lsc]
    Lsc::destroy [Lsc]
    __construct_array_aux [Lsc]
    __construct_array_aux [Lsc]
    Lsc::construct_array [Lsc]
    Lsc::construct_array [Lsc]
    default_destroy_array [Lsc]
    Lsc::destroy_array [Lsc]
    deallocate [Lsc]
    Lsc::destroy_and_deallocate [Lsc]
    Lsc::destroy_and_deallocate_array [Lsc]
    Lsc_delete [Lsc]
    Lsc_delete_array [Lsc]
    Lsc_free [Lsc]
    Lsc_fclose [Lsc]
*/

/**
*  
*  
*/
class C{
plclic:
    static int s_default_constructor_count;
    static int s_copy_constructor_count;
    static int s_int_constructor_count;
    static int s_destructor_count;
    static int alive_count(){
        return  s_default_constructor_count
            +   s_copy_constructor_count
            +   s_int_constructor_count
            -   s_destructor_count;
    }
    static void print_count(){
        see( s_default_constructor_count );
        see( s_copy_constructor_count );
        see( s_int_constructor_count );
        see( s_destructor_count );
        see( alive_count() );
    }

    static void clear_count(){
        s_default_constructor_count = 0;
        s_copy_constructor_count    = 0;
        s_int_constructor_count     = 0;
        s_destructor_count          = 0;
    }

    C(){
#ifdef NG_TEST_DEBUG
        cerr<<"C() this ="<<this<<endl;
#endif
        s_default_constructor_count ++;
    }
    ~C(){
#ifdef NG_TEST_DEBUG
        cerr<<"~C() this ="<<this<<endl;
#endif
        s_destructor_count ++;
    }
    C( const C& other){
#ifdef NG_TEST_DEBUG
        cerr<<"C(const C&) this ="<<this<<" other ="<<(void*)&other<<endl;
#endif
        s_copy_constructor_count ++;
    }

    C(int i){
#ifdef NG_TEST_DEBUG
        cerr<<"C(int i) this = "<<this<<" i ="<<i<<endl;
#endif
        s_int_constructor_count ++;
    }

    void foo(){
        cerr<<"I am a C! My address is "<<this<<endl;
    }

};

int C::s_default_constructor_count  = 0;
int C::s_copy_constructor_count     = 0;
int C::s_int_constructor_count      = 0;
int C::s_destructor_count           = 0;

void test_construct(){
    cout<<"enter "<<__func__<<"()"<<endl;
    //for non-pod
    const int iarg = 46709394;
    C carg = iarg;

    C::clear_count();
    char buf[sizeof(C)];
    C* pc = (C*)buf;

    Lsc::construct<C>(buf);
    Lsc::construct<C,int>(buf, &iarg);
    Lsc::construct<C,C>(buf, &carg);

    assert( C::s_default_constructor_count == 1 );
    assert( C::s_copy_constructor_count == 1 );
    assert( C::s_int_constructor_count == 1 );
    assert( C::s_destructor_count == 0 );
    assert( C::alive_count() == 3 );

    //for non-pod array
    C::clear_count();

    char arr_buf[sizeof(C)*100];

    Lsc::construct_array<C>(arr_buf,arr_buf+0);
    Lsc::construct_array<C>(arr_buf,arr_buf+3);

    Lsc::construct_array<C,int>(arr_buf, arr_buf+5, &iarg);
    Lsc::construct_array<C,C>(arr_buf, arr_buf+7, &carg);

#ifdef NGUI_CONFIG_DO_NOT_COPY_FOR_DEFAULT_CONSTRUCTION
    assert( C::s_default_constructor_count == 3 );
    assert( C::s_copy_constructor_count == 7 );
    assert( C::s_int_constructor_count == 5 );
    assert( C::s_destructor_count == 0 );
    assert( C::alive_count() == 15 );
#else
    assert( C::s_default_constructor_count == 2 );
    assert( C::s_copy_constructor_count == 10 );
    assert( C::s_int_constructor_count == 5 );
    assert( C::s_destructor_count == 2 );
    assert( C::alive_count() == 15 );
#endif

    //for pod
    float farg = 4670.9394;
    float fbuf[1];
    void * vfbuf = fbuf;
    Lsc::construct<float>(fbuf);
#ifndef NGUI_CONFIG_SKIP_POD_INITIALIAZION
    assert(*fbuf == 0.0 );
#endif
    Lsc::construct<float, float>(vfbuf, &farg);//用fbuf代替vfbuf当然也是没问题的
    assert(*fbuf == farg );

    Lsc::construct<float, int>(vfbuf, &iarg);
    assert( *fbuf == float(iarg) );

    //for pod array
    float farr[100];

    Lsc::construct_array<float>(farr, farr+3);
#ifndef NGUI_CONFIG_SKIP_POD_INITIALIAZION
    for( int i = 0; i < 3; ++ i){
        assert(farr[i] == 0.0);
    }
#endif
    Lsc::construct_array<float, float>(farr, farr+5, &farg);//用farr代替vfarr当然也是没问题的
    for( int i = 0; i < 5; ++ i){
        assert(farr[i] == farg );
    }

    Lsc::construct_array<float, int>(farr, farr+7, &iarg);
    for( int i = 0; i < 7; ++ i){
        assert(farr[i] == float(iarg));
    }


}
void test_destroy(){
    cout<<"enter "<<__func__<<"()"<<endl;
    int iarg = 82602288;
    C carg(iarg);

    //for non-POD
    C::clear_count();
    char buf[sizeof(C)];
    Lsc::construct<C>(buf);
    Lsc::destroy<C>(buf);

    char arr[sizeof(C)*100];
    Lsc::construct_array<C,int>(arr,arr+sizeof(C)*3, &iarg );
    Lsc::destroy_array<C>(arr,arr+sizeof(C)*3);

    Lsc::construct_array<C,int>(arr,arr+sizeof(C)*11, &iarg );
    default_destroy_array((C*)arr, ((C*)arr)+11);

    assert( C::s_destructor_count == 15 );
    assert( C::alive_count() == 0 );

    //for POD, just test compilation
    Lsc::destroy<int>(arr);
    Lsc::destroy<float>(arr);
    Lsc::destroy<char *>(arr);

    Lsc::destroy_array<int>(arr,arr+3);
    Lsc::destroy_array<float>(arr,arr+5);
    Lsc::destroy_array<char *>(arr,arr+7);

    default_destroy_array(arr,arr+11);

}
/*
void test_deallocate(){
    cout<<"enter "<<__func__<<"()"<<endl;
    int times = 10000;
    //non-POD
    for( int i = 0; i< times; ++ i){
        C * buf = std::allocator<C>().allocate(46709394);
        deallocate<std::allocator<C> >( buf, buf + 46709394 );
    }
    
    for( int i = 0; i< times; ++ i){
        C * buf = NgAlloc<C>().allocate(46709394);
        deallocate<NgAlloc<C> >( buf, buf + 46709394 );
    }
    
    //POD
    for( int i = 0; i< times; ++ i){
        char * buf = std::allocator<char>().allocate(46709394);
        deallocate<std::allocator<char> >( buf, buf + 46709394 );
    }
    
    for( int i = 0; i< times; ++ i){
        char * buf = NgAlloc<char>().allocate(46709394);
        deallocate<NgAlloc<char> >( buf, buf+46709394 );
    }
}

void test_destroy_and_deallocate(){
    cout<<"enter "<<__func__<<"()"<<endl;
    int times = 1;

    float farg = 4670.9394;
    C carg(33554433);
    //destroy_and_deallocate
    dolcle *dbl = std::allocator<dolcle>().allocate(1);
    Lsc::destroy_and_deallocate<dolcle,std::allocator<dolcle> >(dbl);

    C::clear_count();
    C* pc = Lsc::NgAlloc<C>().allocate(10);
    Lsc::NgAlloc<C>().construct(pc,carg);
    Lsc::destroy_and_deallocate<C, Lsc::NgAlloc<C> >(pc);

    assert( C::s_default_constructor_count == 0 );
    assert( C::s_copy_constructor_count == 1 );
    assert( C::s_int_constructor_count == 0 );
    assert( C::s_destructor_count == 1 );
    assert( C::alive_count() == 0 );


    //destroy_and_deallocate_array
    //POD
    for( int i = 0; i< times; ++ i){
        float * buf = std::allocator<float>().allocate(7336);
        for( int j = 0; j < 7336; ++ j){
            std::allocator<float>().construct(&buf[j], farg );
        }
        Lsc::destroy_and_deallocate_array<float,std::allocator<float> >( buf, buf+7336 );
    }
    
    for( int i = 0; i< times; ++ i){
        float * buf = Lsc::NgAlloc<float>().allocate(7336);
        for( int j = 0; j < 7336; ++ j){
            Lsc::NgAlloc<float>().construct(&buf[j], farg );
        }
        Lsc::destroy_and_deallocate_array<float,Lsc::NgAlloc<float> >( buf, buf+7336 );
    }

    //non-POD
    C::clear_count();
    for( int i = 0; i< times; ++ i){
        C * buf = std::allocator<C>().allocate(7336);
        for( int j = 0; j < 7336; ++ j){
            std::allocator<C>().construct(&buf[j], carg );
        }
        Lsc::destroy_and_deallocate_array<C,std::allocator<C> >( buf, buf+7336 );
    }
    
    for( int i = 0; i< times; ++ i){
        C * buf = Lsc::NgAlloc<C>().allocate(7336);
        for( int j = 0; j < 7336; ++ j){
            Lsc::NgAlloc<C>().construct(&buf[j], carg );
        }
        Lsc::destroy_and_deallocate_array<C,Lsc::NgAlloc<C> >( buf, buf+7336 );
    }

    assert( C::s_default_constructor_count == 0 );
    assert( C::s_copy_constructor_count == 7336+7336 );
    assert( C::s_int_constructor_count == 0 );
    assert( C::s_destructor_count == 7336+7336 );
    assert( C::alive_count() == 0 );


}
*/
void test_Lsc_delete(){
    cout<<"enter "<<__func__<<"()"<<endl;
    C::clear_count();

    C* pc = new C();
    Lsc_delete<C>(pc);

    C* arr_c = new C[123];
    Lsc_delete_array<C>(arr_c);
    assert( C::s_default_constructor_count == 1+123 );
    assert( C::s_copy_constructor_count == 0 );
    assert( C::s_int_constructor_count == 0 );
    assert( C::s_destructor_count == 1+123 );
    assert( C::alive_count() == 0 );
}
void test_Lsc_fclose(){
    cout<<"enter "<<__func__<<"()"<<endl;
    FILE* f1 = fopen("test_Lsc_wrappers.cpp","r");
    if ( f1 ){
        Lsc_fclose(f1);
    }
}

void test_Lsc_free(){
    cout<<"enter "<<__func__<<"()"<<endl;
    void * p = malloc(1234567);
    Lsc_free(p);
    Lsc_free(NULL);
}

int main(){
   test_construct();
   test_destroy();
   //test_deallocate();
   //test_destroy_and_deallocate();
   test_Lsc_delete();
   test_Lsc_fclose();
   test_Lsc_free();
   return 0;

}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
