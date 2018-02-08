 
#include "Lsc/ResourcePool.h"
#include "Lsc/var/Null.h"
#include "Lsc/var/JsonDeserializer.h"
 
void var_invalid_operation(){
    
    
    try{
        Lsc::var::Null null;
        null["unexist"] = null; 
    }catch(Lsc::Exception& e){
        printf( "%s thrown from %s:%d:%s is caught!<CR>  message:%s<CR>  stack:<CR>%s\n",
                e.name(), e.file(), e.line(), e.function(), e.what(), e.stack()
              );
    }
    
}

void var_parse_error(){
    
    
    const char * JSON = 
" {                                     \n\
    'user' : {                          \n\
        'name' : 'acumon', age : 24     \n\
    },                                  \n\
    'email' : 'dongyunbiao@ixintui.com'  \n\
  }                                     \n\
";
    try{
        Lsc::ResourcePool rp;
        Lsc::var::JsonDeserializer jd(rp);
        Lsc::var::IVar& res = jd.deserialize( JSON );
    }catch(Lsc::Exception& e){
        printf( "%s thrown from %s:%d:%s is caught!<CR>  message:%s<CR>  stack:<CR>%s\n",
                e.name(), e.file(), e.line(), e.function(), e.what(), e.stack()
              );
    }

    
}
int main(){
	
    var_invalid_operation();
    var_parse_error();
    return 0;
}



