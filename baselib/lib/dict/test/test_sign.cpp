#include <stdio.h>
#include <ul_sign.h>
#include "ul_url_sign.h"
#include <assert.h>
int main(int argc,char** argv)
{
    char buf[1024];
    int len;
    unsigned int sign[2];
    unsigned int mode;
    int output;
    unsigned long long v1,v2;    
    if(argc!= 3){
        fprintf(stdout,"Usage: %s mode output\n",argv[0]);
        return -1;
    }
    output=atoi(argv[2]);
    mode=atoi(argv[1]);
    while(fgets(buf,sizeof(buf),stdin)!=NULL){
        len=strlen(buf);
        while((buf[len-1]=='\n')||(buf[len-1]=='\r')){
            buf[len-1]=0;
            len--;
        }
        switch(mode){
            case 1:
                creat_sign_f64(buf,len,sign,sign+1);
                break;
            case 2:
                creat_sign_fs64(buf,len,sign,sign+1);
                break;
            case 3:
                creat_sign_7_host(buf,len,sign,sign+1);
                break;
            case 4:
                creat_sign_md64(buf,len,sign,sign+1);
                break;
            case 5:
                creat_sign_mds64(buf,len,sign,sign+1);
                break;
            case 6:
                creat_sign_murmur64(buf,len,sign,sign+1);
                break;
            case 7:
                creat_sign_murmurs64(buf,len,sign,sign+1);
                break;
            case 8:{
                unsigned int site_sign[2];
                int ret1=0,ret2=0;
                ret1=create_url_sign(buf,site_sign,sign);
                ret2=create_url_sign(buf,v1,v2);
                if(output){
                    fprintf(stdout,"ret1:%d ret2:%d\n",ret1,ret2);
                    fprintf(stdout,"v1:%llu,v2:%llu\n",v1,v2);
                    fprintf(stdout,"site:[0x%x:0x%x]\n",site_sign[0],site_sign[1]);
                }
            }
                break;
            case 9:{
                unsigned int site_sign[2];
                int ret1=0,ret2=0;
                ret1=create_url_sign2(buf,site_sign,sign);
                ret2=create_url_sign2(buf,v1,v2);
                if(output){
                    fprintf(stdout,"ret1:%d ret2:%d\n",ret1,ret2);
                    fprintf(stdout,"v1:%llu,v2:%llu\n",v1,v2);
                    fprintf(stdout,"site:[0x%x:0x%x]\n",site_sign[0],site_sign[1]);
                }
            }
                break;
            default:
                sign[0]=0;
                sign[1]=0;
                break;
        }
        if(output){
            //fprintf(stdout,"%s [%u:%u]\n", buf,sign[0],sign[1]);
            fprintf(stdout,"[0x%x:0x%x]\n",sign[0],sign[1]);
        }
    }
}
    
