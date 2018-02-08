
#include <cstdio>

#include "McpackError.h"
#include "McpackGenerator.h"
#include "McpackSampleGenerator.h"
#include "McpackArrayShortString.h"
#include "McpackArrayShortInt32.h"
#include "McpackArrayLargeInt32.h"
#include "McpackArrayLongString.h"
#include "McpackObjectShortString.h"
#include "McpackObjectShortInt32.h"
#include "McpackObjectLargeInt32.h"
#include "McpackObjectLongString.h"
#include "McpackEmpty.h"
#include "JsonGenerator.h"
#include "TextGenerator.h"


int json_vs_mcpack_size(){
    McpackGenerator *mgs[9]={
        new McpackArrayShortString(),
        new McpackArrayShortInt32(),
        new McpackArrayLargeInt32(),
        new McpackArrayLongString(),
        new McpackObjectShortString(),
        new McpackObjectShortInt32(),
        new McpackObjectLargeInt32(),
        new McpackObjectLongString(),
        new McpackEmpty(),
    };
    for(int i=0;i<9;i++){
        McpackGenerator *mg=mgs[i];
        mc_pack_t *pack=mg->Generate();
        JsonGenerator jg(4*1024*1024);
        jg.Pack2Json(pack);
        printf("[Tag:%s][Mcpack:%d][Json:%d]\n",
               mg->Tag(),mg->McpackLength(),jg.JsonLength());
        delete mg;
    }
    return 0;
}


int sample(){
    
    McpackSampleGenerator msg;
    mc_pack_t *pack=msg.Generate();
    assert(!MC_PACK_PTR_ERR(pack));

    JsonGenerator jg;
    jg.Pack2Json(pack);
    printf("==========Json==========\n");
    printf("%s\n",jg.JsonData());
    

    TextGenerator tg;
    tg.Pack2Text(pack);
    printf("==========Text==========\n");
    printf("%s\n",tg.TextData());
    
    return 0;
}

int debug(){
    
    char data[1024];
    char temp[2048];
    mc_pack_t* pack=mc_pack_open_w(2,data,sizeof(data),temp,sizeof(temp));
    assert(mc_pack_put_uint64(pack,"hello",1)==0);
    mc_pack_t* slcpack=mc_pack_put_object(pack,"hello2");
    assert(MC_PACK_PTR_ERR(slcpack)==0);
    assert(mc_pack_put_str(slcpack,"x","string")==0);
    mc_pack_finish(slcpack);
    slcpack=mc_pack_put_array(pack,"hello3");
    assert(MC_PACK_PTR_ERR(slcpack)==0);
    assert(mc_pack_put_str(slcpack,NULL,"string")==0);
    mc_pack_finish(slcpack);
    mc_pack_close(pack);

    
    char text[1024];
    pack=mc_pack_open_r(data,sizeof(data),temp,sizeof(temp));
    mc_pack_pack2text(pack,text,sizeof(text),1);
    
    
    mc_pack_close(pack);

    printf("%s\n",text);

    
    pack=mc_pack_open_w(2,data,sizeof(data),temp,sizeof(temp));
    mc_pack_text2pack(text,pack);
    
    
    mc_pack_close(pack);
    
    
    pack=mc_pack_open_r(data,sizeof(data),temp,sizeof(temp));
    mc_uint64_t value;
    assert(mc_pack_get_uint64(pack,"hello",&value)==0);
    assert(value==1);
    mc_pack_close(pack);
    return 0;
}

int debug1(){
    char info_buf[100000] = { 0 };
    char mcpack_buf[100000];
    char temp_buf[100000];
    mc_pack_t * info_pack = NULL;
    info_pack = mc_pack_open_w(2, mcpack_buf, sizeof(mcpack_buf), temp_buf, sizeof(temp_buf));
    int ret = mc_pack_json2pack(info_buf, info_pack);
    mc_pack_close(info_pack);
    printf("info_pack=%p\n", info_pack);
    printf("ret=%d\n", ret);

    info_pack = mc_pack_open_w(2, mcpack_buf, sizeof(mcpack_buf), temp_buf, sizeof(temp_buf));
    ret = mc_pack_json2pack_gbk(info_buf, info_pack);
    mc_pack_close(info_pack);
    printf("info_pack=%p\n", info_pack);
    printf("ret=%d\n", ret);
    
    info_pack = mc_pack_open_w(2, mcpack_buf, sizeof(mcpack_buf), temp_buf, sizeof(temp_buf));
    ret = mc_pack_text2pack(info_buf, info_pack);
    mc_pack_close(info_pack);
    printf("info_pack=%p\n", info_pack);
    printf("ret=%d\n", ret);

    return 0;
}

int main(){    
    
    
    debug();
    debug1();
    return 0;    
}
