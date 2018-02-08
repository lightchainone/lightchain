

#ifndef  __TEST_SOCKETPOOL_H_
#define  __TEST_SOCKETPOOL_H_

#include "socketpool.h"
#include <cxxtest/TestSuite.h>

class test_socketpool : plclic CxxTest::TestSuite {
	protected:

	plclic:
		void test_socketpool_1(){
			comcfg::Configure conf;
			int ret = conf.load("conf/", "cpool.conf");
			TSM_ASSERT(ret, ret == 0);
			SocketPool skpool;
			ret = skpool.init(conf["ConnectManager"]);
			TSM_ASSERT(ret, ret == 0);
		};

		void test_socketpool_2(){
#if 1
			comcfg::Configure conf;
			system("killall server_socketpool");
			system("./server_socketpool 31258 &");
			sleep(5);
			int ret = conf.load("conf/", "myhost.conf");
			TSM_ASSERT(ret, ret == 0);
			SocketPool skpool;
			ret = skpool.init(conf["ConnectManager"]);
			TSM_ASSERT(ret, ret == 0);
			int sock = skpool.FetchSocket();
			TSM_ASSERT(sock, sock > 0);
			char buf[16] = "yingxiang";
			char buf2[16];
            memset(buf2, 0, sizeof(buf2));
			int len = strlen(buf);
			ret = write(sock, buf, len);
			TSM_ASSERT(ret, ret == len);
			ret = read(sock, buf2, len);
			TSM_ASSERT(ret, ret == len);
			buf2[len] = 0;
			ret = strcmp(buf2, "YINGXIANG");
			printf("I read : %s\n", buf2);
			TSM_ASSERT(ret, ret == 0);
#endif
		};
#if 1
		void test_socketpool_3(){
			system("killall server_socketpool");
			system("./server_socketpool 31258 &");
            sleep(5);
			comcfg::Configure conf, conf2;
			int ret = conf.load("conf/", "myhost.conf");
			TSM_ASSERT(ret, ret == 0);
			SocketPool skpool;
			ret = skpool.init(conf["ConnectManager"]);
			TSM_ASSERT(ret, ret == 0);
			int sock = skpool.FetchSocket();
			TSM_ASSERT(sock, sock > 0);
			skpool.FreeSocket(sock, false);
			ret = conf2.load("conf/", "multi_server.conf");
            
			TSM_ASSERT(ret, ret == 0);

			ret = skpool.reload(conf2["ConnectManager"]);
			system("sh ./run_multi_server.sh");
			TSM_ASSERT(ret, ret == 0);
            

			sock = skpool.FetchSocket();
			TSM_ASSERT(sock, sock > 0);
			char buf[16] = "yingxiang";
			char buf2[16];
			int len = strlen(buf);
			ret = write(sock, buf, len);
			TSM_ASSERT(ret, ret == len);
			ret = read(sock, buf2, len);
			TSM_ASSERT(ret, ret == len);
			buf2[len] = 0;
			ret = strcmp(buf2, "YINGXIANG");
			printf("I read : %s\n", buf2);
			skpool.FreeSocket(sock, false);
		}
#endif
		void test_socketpool_4(){
			system("killall server_socketpool");
			system("./server_socketpool 31258 &");
            sleep(5);
			comcfg::Configure conf, conf2;
			int ret = conf.load("conf/", "myhost.conf");
			TSM_ASSERT(ret, ret == 0);
			SocketPool skpool;
			ret = skpool.init(conf["ConnectManager"]);
			TSM_ASSERT(ret, ret == 0);
			int sock = skpool.FetchSocket();
			TSM_ASSERT(sock, sock > 0);
			skpool.FreeSocket(sock, false);
			ret = conf2.load("conf/", "multi_server.conf");
            
			TSM_ASSERT(ret, ret == 0);

			ret = skpool.reload(NULL, 0);
			system("sh ./run_multi_server.sh");
			TSM_ASSERT(ret, ret == 0);
            

			sock = skpool.FetchSocket();
			TSM_ASSERT(sock, sock < 0);
            int err;
            comcm::Connection *con = skpool.getManager()->fetchConnection(-1, 0, &err); 
            
            TSM_ASSERT(con, con == NULL);
            TSM_ASSERT(sock, sock < 0);
            TSM_ASSERT(sock, err == comcm::REQ_SVR_SELECT_FAIL);
		}
		void test_socketpool_5(){
			system("killall server_socketpool");
			system("./server_socketpool 31258 &");
            sleep(5);
			comcfg::Configure conf, conf2;
			int ret = conf.load("conf/", "myhost.conf");
			TSM_ASSERT(ret, ret == 0);
			SocketPool skpool;
			ret = skpool.init(NULL, 0, false);
			TSM_ASSERT(ret, ret == 0);
            comcm::Server *svr = new comcm::Server;
            svr->setIP("127.0.0.1");
            svr->setPort(31258);
            svr->setCapacity(10);
            svr->setTimeout(1000);
            svr->setTag("aaa");
            skpool.getManager()->addServer(svr);
			int sock = skpool.FetchSocket();
			TSM_ASSERT(sock, sock > 0);
			skpool.FreeSocket(sock, false);

            
            int x;
            const int *u = skpool.getManager()->getServerByTag("aaa", &x);
            TSM_ASSERT(x, x == 1);
            
		}


};














#endif  


