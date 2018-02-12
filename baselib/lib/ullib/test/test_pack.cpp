#include "ul_ullib.h"

#include <stdio.h>

int main(int argc, char **argv)
{
    ul_package_t *oldPack = ul_pack_init();
    if (NULL == oldPack) {
        printf("%s\n", "init old pack error");
        return -1;
    }

    char *tempName1 = "name";
    char *value1 = "sunxiao";
    char *tempName2 = "sex";
    char *value2 = "male";
    char *tempName3 = "school : ";
    char *value3 = "bupt";
    char *tempName4 = "school";
    char *value4 = "bupt\r\n";
    char *tempName5 = "";
    char *value5 = "bupt\r\n";

    ul_pack_putvalue(oldPack, tempName1, value1);
    ul_pack_putvalue(oldPack, tempName1, value1);
    int ret = ul_pack_delvalue(oldPack, tempName2);
    printf("del %s twice is : %d\n", tempName2, ret);
    ul_pack_delvalue(oldPack, tempName1);
    ul_pack_delvalue(oldPack, tempName1);

    ul_pack_putvalue(oldPack, tempName2, value2);

    ret = ul_pack_putvalue(oldPack, tempName3, value3);
    if (ret < 0) {
        printf("put value=%s to key=%s error!\n", value3, tempName3);
    }

    ret = ul_pack_putvalue(oldPack, tempName4, value4);
    if (ret < 0) {
        printf("put value=%s to key=%s error!\n", value4, tempName4);
    }

    ret = ul_pack_putvalue(oldPack, tempName5, value5);
    if (ret < 0) {
        printf("put value=%s to key=%s error!\n", value5, tempName5);
    }

    for (int i=0; i<oldPack->pairnum; i++) {
        printf("size: %d,  %s : %s\n", oldPack->pairnum, oldPack->name[i], oldPack->value[i]);
    }
    ul_pack_free(oldPack);
    
}





















/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
