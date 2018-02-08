

#include "resourcepool.h"

int main(){
    RsrcPool *respool_hdlr = RsrcPool::get_instance();
    int ret = respool_hdlr->init("./conf/", "lleo.conf_core", 1);
    if (0 != ret){
        fprintf(stderr, "initalization failed, ret:%d\n", ret);
        exit(-1);
    }   
    fprintf(stderr, "initalized!\n");
    
    
    
    ret = respool_hdlr->add_request_resource("dbsvr","/admin",100);
    
    
    
    
    

    
    
    fprintf(stdout, ">>>>>>>>>\nret:%d\n>>>>>>>>>>>\n", ret);
    
    return 0;
}
