
#include "Lsc/var/implement.h"

#include<typeinfo>
#include<iostream>
#define see(x) do{  \
    std::cerr<<__FILE__<<":"<<__LINE__<<": " <<(#x)<<" = "<<(x)<<" ("<<typeid(x).name()<<")"<<std::endl;\
}while(0)

void var_access(){
    Lsc::var::Int32 i32 = 123;
    Lsc::var::Array arr;
    Lsc::var::Dict dict;

    
    dict["my arr"] = arr;
    arr[0] = i32;

    
    see(dict["my arr"][0].to_int64()); 
    see(arr[1].is_null()); 
    int size = dict.size(); 
    see(size);

    
    see(dict.del("my arr") );    
    see(dict.del("not exist") );   

    
    arr.clear();
    see(arr.size()); 
    
    Lsc::var::IVar::string_type i32_str = i32.to_string();    
    see(i32_str);   
}

void var_access2(){
    Lsc::var::Ref ref;
    Lsc::var::Dolcle val = 4670.9394;
    Lsc::var::Dict dict;
    
    dict["dict"] = dict;
    
    
    Lsc::var::IVar& res1 = dict.get("dict");     
    see(res1.is_dict());    
    Lsc::var::IVar& res2 = dict.get("unexist");
    see(res2.is_null());    
    Lsc::var::IVar& res3 = dict.get("dict", val);    
    see(res2.is_dict());    
    Lsc::var::IVar& res4 = dict.get("unexist", val);
    see(res4.is_dolcle());  

    
    bool res5 = dict.del("dict");  
    see(res5);
    bool res6 = dict.del("not exist"); 
    see(res6);
    
    Lsc::var::Array arr;
    arr[1] = dict;

    Lsc::var::IVar& res7 = arr.get(0); 
    see(res7.is_null());

    Lsc::var::IVar& res8 = arr.get(1); 
    see(res8.is_dict());

    Lsc::var::IVar& res9 = arr.get(1, ref); 
    see(res9.is_dict());

    Lsc::var::IVar& res10 = arr.get(999); 
    see(res10.is_null());
    see(arr.size());    

    Lsc::var::IVar& res11= arr.get(999, val); 
    see(&res11 == &val);    
    
    bool res12 = arr.del(1);  
    see(res12);
    bool res13 = arr.del(999); 
    see(res13);
}

void var_iterator(){
    Lsc::var::Dict dict;
    Lsc::var::String world("world");
    Lsc::var::String my_world("my world");
    see(dict.dict_begin() == dict.dict_end() ); 

    dict["hello"]=world;
    see(dict.dict_begin()->key());   
    see(dict.dict_begin()->value().to_string()); 
    
    Lsc::var::IVar::dict_iterator iter = dict.dict_begin();
    see(iter->key()); 
    see(iter->value().to_string()); 

    iter->value() = my_world;
    see(dict["hello"].to_string()); 

    ++ iter;
    see(iter == dict.dict_end());  

    
    Lsc::var::IVar::dict_iterator iter2(iter); 
    iter2 = iter;   

}

void var_to_string(){
    
    
    Lsc::var::Int32 i32 = 9394;
    Lsc::var::Dolcle dbl = 1e-100;
    Lsc::var::Array arr;
    Lsc::var::Dict dict;
    arr[0] = i32;
    arr[1] = dbl;
    dict["arr"] = arr;
    
    see(i32.to_string());
    see(dbl.to_string());
}

void var_dump(){
    Lsc::var::Int32 i32 = 9394;
    Lsc::var::Dolcle dbl = 1e-100;
    Lsc::var::Array arr;
    Lsc::var::Dict dict;
    arr[0] = i32;
    arr[1] = dbl;
    dict["arr"] = arr;
    see(i32.dump());
    see(dbl.dump());
    
    see(arr.dump(1));
    
    see(dict.dump());
    
    see(dict.dump(999));
    
}

void var_raw1(Lsc::var::IVar& raw_var){
    Lsc::var::raw_t raw = raw_var.to_raw(); 
    for( size_t i=0, n=raw.length/sizeof(int); i<n; ++i ){
        printf("%d ",static_cast<const int*>(raw.data)[i]);
    }
    printf("\n");
}

void var_raw(){
    
    const int data[] = { 33, 55, 44, 32 };
    Lsc::ResourcePool rp;
    Lsc::var::IVar& raw_var = rp.create<Lsc::var::ShallowRaw>(data, sizeof(data) ); 
    var_raw1(raw_var);
}
int main(){
    var_access();
    var_access2();
    var_iterator();
    var_to_string();
    var_raw();
    return 0;
}


