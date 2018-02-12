#include <com_log_if.h>
#include <gtest/gtest.h>
#include <signal.h>
#include <xutils.h>
#include <ul_log.h>

#include <com_log.h>
ul_logstat_t logstat;
static void * _run_ (void *)
{
	ul_openlog_r("ultest", &logstat);
	for (int i=0; i<5; ++i) {
		ul_writelog(UL_LOG_NOTICE, "open-r that.s[%ld] - %d111111111111111111111111111111111111111111111111111111111111111111111111111", (long)pthread_self(), i);
	}
	return NULL;
}
int main(char **argv,int argc)
{
 	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

class test_com_ul_log_suite : plclic ::testing::Test{
    protected:
        test_com_ul_log_suite(){};
        virtual ~test_com_ul_log_suite(){};
        virtual void SetUp() {
        };
        virtual void TearDown() {
        };
};
 
TEST_F(test_com_ul_log_suite, comul)
{
	int PNUM = 3;
	memset(&logstat, 0, sizeof(logstat));
	logstat.events = 16;
	int ret = ul_openlog("./log/", "ultest.", &logstat, 2000);
	ASSERT_EQ(ret,0);
	pthread_t pid[PNUM];
	timeval s, e;
	gettimeofday(&s, NULL);
	for (int i=0; i<PNUM; ++i) {
		pthread_create(pid+i, NULL, _run_, NULL);
	}
	for (int i=0; i<PNUM; ++i) {
		pthread_join(pid[i], NULL);
	}
	gettimeofday(&e, NULL);
	int a = (e.tv_sec-s.tv_sec)*1000 + (e.tv_usec-s.tv_usec)/1000;
	float b = (float)(PNUM * 5)/(float)(a);
	fprintf(stderr, "const time[%d]ms, speed[%f]/ms\n", a, b);
	com_closelog();
}

class test_comlog_suite : plclic ::testing::Test{
    protected:
        test_comlog_suite(){};
        virtual ~test_comlog_suite(){};
        virtual void SetUp() {
        };
        virtual void TearDown() {
        };
};
 
TEST_F(test_comlog_suite, comlog)
{
	signal(SIGPIPE, SIG_IGN);
	int ret = com_loadlog("./", "test.conf");
	ASSERT_EQ(ret,0);
	int num = 10;
	timeval s, e;
	com_logstat_t logstat;
	logstat.sysevents = 1;

	com_reglogstat("./", "test.conf");

	gettimeofday(&s, NULL);
	for (int i=0; i<num; ++i) {
		ASSERT_EQ(0,com_writelog(COMLOG_NOTICE, "hello comlog1v1"));
		com_pushlog("a", "%d", 1)("b", "%d", 2)("name", "xiaowei")(COMLOG_NOTICE, "hello comlog1v1");
		com_writelog(COMLOG_DEBUG, "hello debughello comlog1v1");
		com_writelog(COMLOG_WARNING, "this is warning notice");
		com_writelog("xiaowei", "this is xiaowei speakinghello comlog1v1");
		com_pushlog("now_cnt", "%d", (i+1)*5)(COMLOG_WARNING, "thathello comlog1v1");
		com_changelogstat(&logstat);
	}
	com_writelog(COMLOG_NOTICE, "END");
	gettimeofday(&e, NULL);
	int a = (e.tv_sec-s.tv_sec)*1000 + (e.tv_usec-s.tv_usec)/1000;
	float b = (float)(num*5)/(float)(a);
	
	gettimeofday(&s, NULL);
	com_closelog(60000);
	gettimeofday(&e, NULL);
	int c = (e.tv_sec-s.tv_sec)*1000 + (e.tv_usec-s.tv_usec)/1000;

	fprintf(stderr, "%s: const time[%d]ms, speed[%f]/ms sleep[%d]ms \n", "", a, b, c);

	char xmfttest[3];
	ret = comspace::xfmtime(xmfttest, sizeof(xmfttest), e);
	com_writelog("xiaowei", "%s ret=%d", xmfttest, ret);

	com_writelog("xiaowei", "%s ret=%d", xmfttest, ret);

	com_closelog();
}

TEST_F(test_comlog_suite, comlog1) {
	//设置接受的日志等级
	com_logstat_t logstat;
	logstat.sysevents = 16;
	snprintf(logstat.userevents, sizeof(logstat.userevents), "xiaowei,faint");
	//创建设备
	com_device_t dev[3];
	//本地日志
	snprintf(dev[0].host, sizeof(dev[0].host), "log");
	snprintf(dev[0].file, sizeof(dev[0].file), "test2.log");
	//dev[0].type = COM_ULLOGDEVICE;
	strcpy(dev[0].type, "ULLOG");
	dev[0].log_size = 2048;
	COMLOG_SETSYSLOG(dev[0]);
	//设置自定义日志支持xiaow和faint日志
	set_comdevice_selflog(dev, "xiaowei,faint");

	//网络日志
	snprintf(dev[1].host, sizeof(dev[1].host), "127.0.0.1:9898/log");
	snprintf(dev[1].file, sizeof(dev[1].file), "test2.log");
	//dev[1].type = COM_ULNETDEVICE;
	strcpy(dev[1].type, "ULNET");
	COMLOG_SETSYSLOG(dev[1]);
	set_comdevice_selflog(dev+1, "xiaowei,faint");

	//tty日志
	//dev[2].type = COM_TTYDEVICE;
	strcpy(dev[2].type, "TTY");
#if 1
	//打开设备
	int ret = com_openlog("test2", dev, 2, &logstat);
	ASSERT_EQ(ret,0);
#endif
	ASSERT_EQ(0, com_writelog(COMLOG_NOTICE, "hello comlog"));

	com_closelog();

}

TEST_F(test_comlog_suite, device_num) {

	int ret = com_loadlog("./", "test_1023_device.conf");
        if (ret != 0) {
                fprintf(stderr, "load err\n");
                
        }

        int num = 1024;
//        char selflevel[64] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
        for (int i = 0; i < num; i++) {
                com_writelog(COMLOG_NOTICE, "hello max %d", i);
//		com_writelog(selflevel, "hello max %d", i);
        }

        sleep(2);
        com_closelog(600000);


}



TEST_F(test_comlog_suite, loglevel_namelength) {

        int ret = com_loadlog("./", "test_loglevel.conf");
        if (ret != 0) {
                fprintf(stderr, "load err\n");

        }

        int num = 1024;
char selflevel[1030] = "****12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789";
        for (int i = 0; i < num; i++) {
                com_writelog(COMLOG_NOTICE, "hello max %d", i);
                com_writelog(selflevel, "hello max %d", i);
        }

        sleep(2);
        com_closelog(600000);


}

TEST_F(test_comlog_suite, loadlog) {
	int ret = com_loadlog("./", "test.conf");
	ASSERT_EQ(ret,0);
	
	ret = com_closelog();
	ASSERT_EQ(ret,0);

	ret = com_loadlog_ex("./", "test.conf", 1);
	ASSERT_EQ(ret,0);
	
	ret = com_closelog();
	ASSERT_EQ(ret,0);

	ret = com_loadlog_ex("./", "test.conf", -1);
	ASSERT_EQ(ret,-1);
}


TEST_F(test_comlog_suite, resetlog) {
	int ret = com_loadlog("./", "test.conf");
	ASSERT_EQ(ret,0);

	ret = com_resetlog();
	ASSERT_EQ(ret,0);

	ret = com_loadlog("./", "test.conf");
	ASSERT_EQ(ret,0);

	ret = com_closelog();
	ASSERT_EQ(ret,0);
}

TEST_F(test_comlog_suite, getLogVersion) {
	int ret = com_loadlog("./", "test.conf");
	ASSERT_EQ(ret,0);

	ret = com_writelog_ex(COMLOG_FATAL, 1, "type1", "fang gouzi");
	ASSERT_EQ(ret,0);
	ret = com_writelog_ex(COMLOG_FATAL, -1, "type1", "fang gouzi");
	ASSERT_EQ(ret,-1);
	ret = com_writelog_ex(COMLOG_FATAL, 10, "", "fang gouzi");
	ASSERT_EQ(ret,-1);
	ret = com_writelog_ex(COMLOG_FATAL, -1, "", "fang gouzi");
	ASSERT_EQ(ret,-1);

	ret = com_writelog_ex("hello", 10, "type2", "fang gouzi");
	ASSERT_EQ(ret,0);
	ret = com_writelog_ex("hello", -1, "type2", "fang gouzi");
	ASSERT_EQ(ret,-1);
	ret = com_writelog_ex("hello", 10, "", "fang gouzi");
	ASSERT_EQ(ret,-1);
	ret = com_writelog_ex("hello", -1, "", "fang gouzi");
	ASSERT_EQ(ret,-1);

	ret = com_closelog();
	ASSERT_EQ(ret,0);
}
	





/* vim: set ts=4 sw=4 sts=4 tw=100 */
