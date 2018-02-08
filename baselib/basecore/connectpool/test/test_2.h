

#ifndef  __TEST_SOCKETPOOL_H_
#define  __TEST_SOCKETPOOL_H_

#include "socketpool.h"
#include <cxxtest/TestSuite.h>

class test_socketpool : plclic CxxTest::TestSuite {
	protected:

	plclic:
		void test_socketpool_1(){
            
		};

		void test_socketpool_2(){
			system("killall server_socketpool");
			comcfg::Configure conf;
			int ret = conf.load("conf/", "myhost.conf");
			TSM_ASSERT(ret, ret == 0);
			SocketPool skpool;
			SocketDefaultChecker ch;
			skpool.setHealthyChecker(&ch);
			ret = skpool.init(conf["ConnectManager"]);
			TSM_ASSERT(ret, ret == 0);
			int sock = skpool.FetchSocket();
			TSM_ASSERT(sock, sock < 0);
			skpool.getLogAdapter()->debug(
					"-----------------------------------");
			ch.startSingleThread(2);
			system("./server_socketpool 31258 &");
			sleep(10);

            
            
            ch.joinThread();
			
		};

		void test_socketpool_3(){
#if 0
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
			TSM_ASSERT(ret, ret == 0);
			sock = skpool.FetchSocket();
			TSM_ASSERT(sock, sock > 0);
			skpool.FreeSocket(sock, false);
#endif
		}

};














#endif  


