#include "ul_ullib.h"
#include <stdio.h>

int main(int argc, char **argv)
{

    ul_openlog("./log/", "test_server", NULL, 1024, NULL);
    int listen_fd, client_fd;

    listen_fd = ul_tcplisten(6666, 5);
    if (listen < 0) {
        printf("socket listen error\n");
        return -1;
    }

    struct sockaddr_in saddr;
    socklen_t addrsize;    

    client_fd = ul_accept(listen_fd, (struct sockaddr*)&saddr, &addrsize);
    if (client_fd < 0){
        printf("%s\n", "accept error");
        ul_sclose(listen_fd);
        return -1;
    }

    ul_package_t *ppack_recv = NULL;
    ppack_recv = ul_pack_init();

    char *resbuf = new char[500];
    if (NULL == resbuf) {
        return -1;
    }
    ul_pack_putbuf(ppack_recv, resbuf, 500);
    int ret = ul_pack_reado_ms(client_fd, ppack_recv, 1000);

    if (ret < 0) {
        printf("%s\n", "recv error");
    }

    int i = 0;
    for (i=0; i<ppack_recv->pairnum; i++) {
        printf("%s : %s\n", ppack_recv->name[i], ppack_recv->value[i]);
    }

    printf("recv body:  %s\n", ul_pack_getbuf(ppack_recv));
    delete []resbuf;
    ul_pack_free(ppack_recv); 
    printf("%s\n", "recv OK");
    return 0;
}


















/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
