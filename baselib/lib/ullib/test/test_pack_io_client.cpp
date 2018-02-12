
#include "ul_ullib.h"
#include <stdio.h>

int main (int argc, char **argv)
{
    /*
    int client_fd, ret;
    int msec = 1000;

    ul_package_t *ppack_send = NULL;
    ppack_send = ul_pack_init();
    if (ppack_send == NULL) {
        printf("new error");
        return -1;
    }
    char *reqbuf = new char[500];
    memset(reqbuf, 0, 500);
    snprintf(reqbuf, 500, "%s", "test client");
    ul_pack_putbuf(ppack_send, reqbuf, 500);
    printf("plc buf ok\n");
    int i=0;
    char cKey[64];
    char cValue[64];
    for (i=0; i<300; i++) {
        snprintf(cKey, 64, "%s%d",
"key11111111111111", i);
        snprintf(cValue, 64, "%s%d",
"value11111111111", i);
        ul_pack_putvalue(ppack_send, cKey, cValue);   
    }
    printf("put value ok\n");
    // client connect server
    client_fd = ul_tcpconnect("127.0.0.1", 6666);

    // clients send data to server          
    ret = ul_pack_write_mo_ms(client_fd, ppack_send, msec);
    if (ret < 0)  {
        printf("%s\n", "len error");
    }
    ret = ul_pack_write_mo_ms(client_fd, ppack_send, msec);
    if (ret < 0) {
        printf("%s\n", "len error");
    }
    ret = ul_pack_free(ppack_send);
    delete [] reqbuf;
    ul_sclose(client_fd);
    */
    
    char key[250];
    char value[250];
    char send_data[20] = "test pay load!";
    char buf[240];
    memset(buf,'k',240);
    buf[239]='\0';
    int ret;
    ul_package_t* pack = ul_pack_init();
    for(int i=0;i<MAX_PACK_ITEM_NUM;i++)
    {
        ret = 100;
        snprintf(key,sizeof(key),"%skey%d",buf,i);
        memset(value,'v',250);
        ret = ul_pack_putvalue(pack,key,value);
    }
    ul_pack_putbuf(pack, send_data, sizeof(send_data));



    int server_sk,client_sk;

    ret = 100;
    int msec;
    ret = ul_pack_writeo_ms(server_sk, pack, msec);


    ret = 100;
    ret = ul_pack_write_mo_ms(client_sk, pack, msec);

    ret = ul_pack_free(pack);
    ul_sclose(client_sk);
    ul_sclose(server_sk);
}

















/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
