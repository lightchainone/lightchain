
#include <socketpool.h>
#include <socketpoolext.h>
#include <connectpool.h>

int main(int argc, char **argv)
{
    SocketPool pool;
    SocketExtStrategy stry;

    pool.setStrategy(&stry);
    
    ConnectPool::server_conf_t server_conf[2];
    snprintf(server_conf[0].addr, sizeof(server_conf[0].addr), "%s", "127.0.0.1");
    server_conf[0].port = 55555;
    server_conf[0].ctimeout_ms = 1000;

    snprintf(server_conf[1].addr, sizeof(server_conf[1].addr), "%s", "127.0.0.1");
    server_conf[1].port = 66666;
    server_conf[1].ctimeout_ms = 1000;

    if (pool.init(server_conf, 2, 10, 5, true) != 0) {
        fprintf(stderr, "pool init error\n");
        return -1;
    }

    int fd = pool.FetchSocket(-1);
    if (fd < 0) {
        fprintf(stderr, "fetch socket error\n");
        return -1;
    }
    fprintf(stderr, "get fd:%d ok\n", fd);
    pool.FreeSocket(fd, false);

    fd = pool.FetchSocket();

    pool.FreeSocket(fd, false);


    while(1) {
        sleep(5);
        fd = pool.FetchSocket();
        sleep(3);
        pool.FreeSocket(fd, false);

    }

    return 0;
}




















