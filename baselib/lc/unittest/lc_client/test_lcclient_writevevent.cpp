
#include <lcclient_writevevent.h>
#include <gtest/gtest.h>
	
#include "lcclient_manager.h"

#include "bsl/var/IVar.h"
#include "bsl/var/Number.h"
#include "bsl/ResourcePool.h"
#include "bsl/var/String.h"



int main(int argc, char **argv)
{
 	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

class test_LcClientNsheadWritevEvent_LcClientNsheadWritevEvent_suite : plclic ::testing::Test{
    protected:
        test_LcClientNsheadWritevEvent_LcClientNsheadWritevEvent_suite(){};
        virtual ~test_LcClientNsheadWritevEvent_LcClientNsheadWritevEvent_suite(){};
        virtual void SetUp() {
            
        };
        virtual void TearDown() {
            
        };
};
 

TEST_F(test_LcClientNsheadWritevEvent_LcClientNsheadWritevEvent_suite, case_name1)
{
	
}
 

class test_LcClientNsheadWritevEvent_set_iov_suite : plclic ::testing::Test{
    protected:
        test_LcClientNsheadWritevEvent_set_iov_suite(){};
        virtual ~test_LcClientNsheadWritevEvent_set_iov_suite(){};
        virtual void SetUp() {
            
        };
        virtual void TearDown() {
            
        };
};
 

TEST_F(test_LcClientNsheadWritevEvent_set_iov_suite, case_name1)
{
	
}
 

class test_LcClientNsheadWritevEvent_session_begin_suite : plclic ::testing::Test{
    protected:
        test_LcClientNsheadWritevEvent_session_begin_suite(){};
        virtual ~test_LcClientNsheadWritevEvent_session_begin_suite(){};
        virtual void SetUp() {
            
        };
        virtual void TearDown() {
            
        };
};
 

TEST_F(test_LcClientNsheadWritevEvent_session_begin_suite, case_name1)
{
	
}
 

class test_LcClientNsheadWritevEvent_write_done_callback_suite : plclic ::testing::Test{
    protected:
        test_LcClientNsheadWritevEvent_write_done_callback_suite(){};
        virtual ~test_LcClientNsheadWritevEvent_write_done_callback_suite(){};
        virtual void SetUp() {
            
        };
        virtual void TearDown() {
            
        };
};
 

TEST_F(test_LcClientNsheadWritevEvent_write_done_callback_suite, case_name1)
{
	
}
 


class test_LcClientNsheadWritevEvent_function_suite : plclic ::testing::Test{
    protected:
        test_LcClientNsheadWritevEvent_function_suite(){};
        virtual ~test_LcClientNsheadWritevEvent_function_suite(){};
        virtual void SetUp() {
            
        };
        virtual void TearDown() {
            
        };
};
 
static void foo1(
		lc::LcClientManager *lcmgr,
		lc::LcClientNsheadWritevEvent *event, 
		const char *service_name, bool ok)
{
	int ret;
	struct iovec iov[2];

	int bufsize = 16;
	char buffer[bufsize];
	strcpy(buffer, "hello");

	iov[1].iov_base = buffer;
	iov[1].iov_len =  bufsize;

	nshead_t req_head;
	req_head.magic_num = NSHEAD_MAGICNUM;
	req_head.body_len = bufsize;

	iov[0].iov_base = (void*)&req_head;
	iov[0].iov_len  = sizeof(nshead_t);

	event->set_iov(iov, 2);

	ret = lcmgr->common_singletalk(service_name, event);
	if(ok)
		ASSERT_EQ(ret, 0);
	else
		ASSERT_NE(ret, 0);
}

static void test1(lc::LcClientManager *lcmgr)
{
	lc::LcClientNsheadWritevEvent *event = new lc::LcClientNsheadWritevEvent;
	foo1(lcmgr, event, "echo", true);
	foo1(lcmgr, event, "none", false);
	foo1(lcmgr, event, "echo", true);
	event->release();
}

static void foo2(
		lc::LcClientManager *lcmgr,
		lc::LcClientNsheadWritevEvent *event, 
		const char *service_name, bool ok)
{
	int ret;
	ret = lcmgr->common_singletalk(service_name, event);
	if(ok)
		ASSERT_EQ(ret, 0);
	else
		ASSERT_NE(ret, 0);
}


static void test2(lc::LcClientManager *lcmgr)
{
	lc::LcClientNsheadWritevEvent *event = new lc::LcClientNsheadWritevEvent;
	
	struct iovec iov[2];

	int bufsize = 5*1024*1024;
	char *buffer = (char*)malloc(bufsize);
	strcpy(buffer, "hello");

	iov[1].iov_base = buffer;
	iov[1].iov_len =  bufsize;

	nshead_t req_head;
	req_head.magic_num = NSHEAD_MAGICNUM;
	req_head.body_len = bufsize;

	iov[0].iov_base = (void*)&req_head;
	iov[0].iov_len  = sizeof(nshead_t);

	event->set_iov(iov, 2);

	foo2(lcmgr, event, "echo", true);
	foo2(lcmgr, event, "none", false);
	foo2(lcmgr, event, "echo", true);
	event->release();
	free(buffer);
}



TEST_F(test_LcClientNsheadWritevEvent_function_suite, case_name1)
{
	comcfg::Configure conf;
	ASSERT_EQ(conf.load("./conf", "client.conf"), 0);

	lc_init_comlog(conf["Log"]);

	lc::LcClientManager lcmgr;
	ASSERT_EQ(lcmgr.init("./conf", "client.conf"), 0);
	test1(&lcmgr);
	test2(&lcmgr);

	lcmgr.close();

}
 
