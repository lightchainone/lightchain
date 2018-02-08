#include "Lsc/var/JsonSerializer.h"
#include "Lsc/var/JsonDeserializer.h"
#include "Lsc/var/McPackSerializer.h"
#include "Lsc/var/McPackDeserializer.h"
#include "Lsc/var/BmlSerializer.h"
#include "Lsc/var/BmlDeserializer.h"
#include "Lsc/var/utils.h"
 
#include<typeinfo>
#include<iostream>
#define see(x) do{  \
    std::cerr<<__FILE__<<":"<<__LINE__<<": " <<(#x)<<" = "<<(x)<<" ("<<typeid(x).name()<<")"<<std::endl;\
}while(0)

void var_json_deserialize(){

    {
        
        const char * JSON_STR= "[ 12.3, { 'hello': '世界' }, \"abc\n\\\"quote\\\"123\" ]";
        Lsc::ResourcePool rp;
        Lsc::var::JsonDeserializer jd(rp);
        Lsc::var::IVar& json = jd.deserialize(JSON_STR);
        see(json.dump(999));

    }

    {
        
        const char * JSON_STR= "'\\u4e2d\\u6587'";  
        Lsc::ResourcePool rp;
        Lsc::var::JsonDeserializer jd(rp);
        Lsc::var::IVar& str = jd.deserialize(JSON_STR);
        see(str.c_str());    

        jd.set_unescape_chinese(true);
        see(jd.get_unescape_chinese()); 
        
        jd.set_output_encoding(Lsc::var::JsonDeserializer::GBK);
        see(jd.get_output_encoding() == Lsc::var::JsonDeserializer::GBK);   
        Lsc::var::IVar& encode_str = jd.deserialize(JSON_STR);
        see(encode_str.to_string());    
    }

    {
        
        const char *JSON_STR = "[ true, false, null ]";
        Lsc::ResourcePool rp;
        Lsc::var::JsonDeserializer jd(rp);
        Lsc::var::IVar& arr = jd.deserialize(JSON_STR);
        see(arr[0].to_string());    
        see(arr[1].to_string());    
        see(arr[2].to_string());    
    }

}

void var_json_serialize(){
    {
        
        const char * JSON = "[ { 'abc': '一二三' }, 1.23, null,\"\t\r\n/\b\f\\\"\\\"\" ]";
        Lsc::ResourcePool rp;
        Lsc::var::JsonDeserializer jd(rp);
        Lsc::var::IVar& var = jd.deserialize(JSON);
        Lsc::var::JsonSerializer js;
        Lsc::AutoBuffer buf;
        js.serialize(var, buf);
        see(buf.c_str());   

        js.set_escape_chinese(true);
        see(js.get_escape_chinese());   
        js.set_input_encoding(Lsc::var::JsonSerializer::GBK);
        Lsc::AutoBuffer escape_buf;
        js.serialize(var, escape_buf);
        see(escape_buf.c_str());    
    }

}
void var_mcpack(){

    
    
    
    
    
    const char *JSON = "{ 'arr': [ 9394, 2.71828, null], 'str': \"abc123\t\r\n/\b\f\\\"\\\"\" }";
    Lsc::ResourcePool rp;
    Lsc::var::IVar& var = Lsc::var::JsonDeserializer(rp).deserialize(JSON);
    
    
    const size_t SIZE = 1024;
    char buf[SIZE];
    Lsc::var::McPackSerializer mps;
    int version = 2;
    mps.set_opt(Lsc::var::MCPACK_VERSION, &version, sizeof(version));
    size_t length = mps.serialize( var, buf, SIZE );
    see(length);    
    
    
    char text[SIZE];
    mc_pack_pack2text(reinterpret_cast<mc_pack_t*>(buf), text, SIZE, 0 );
    see(text);  

    
    Lsc::var::McPackDeserializer mpd(rp);
    Lsc::var::IVar& res = mpd.deserialize(buf, SIZE);
    Lsc::var::print( res, 3 );
    
    
    Lsc::AutoBuffer res_json;
    Lsc::var::JsonSerializer js;
    js.serialize(res, res_json);
    see(res_json.c_str());  
    
}
struct S{
    unsigned int cmdno;
    short args[2]; 
    char sender[10];
};
const char* S_BML = "#([cmdno]I[args]2h[sender]10c)";
void var_bml(){
    
    S s; s.cmdno = 123; s.args[0] = 456; s.args[1] = 789;
    strcpy(s.sender, "Acumon");
    
    Lsc::ResourcePool rp;
    Lsc::var::BmlDeserializer bd(S_BML, rp);
    Lsc::var::IVar& res = bd.deserialize(&s, sizeof(s));
    
    Lsc::AutoBuffer buf;
    Lsc::var::JsonSerializer js;
    js.serialize(res, buf);
    see(buf.c_str());   
    
    Lsc::var::JsonDeserializer jd(rp);
    Lsc::var::IVar& req = jd.deserialize("{'args':[-4670,+9394],'cmdno':42,'sender':'Obama'}");
    
    Lsc::var::BmlSerializer bs(S_BML);
    bs.serialize(req, &s, sizeof(s));
    see(s.cmdno);   
    see(s.args[0]); 
    see(s.args[1]); 
    see(s.sender);  
}

int main(){
    var_json_deserialize();
    var_json_serialize();
    var_mcpack();
    var_bml();
    return 0;
}


