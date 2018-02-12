#include <com_log.h>
#include <gtest/gtest.h>

int main(int argc, char **argv)
{
 	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS_IN_PROC();
}

/**
**/
class test_comlog_hostname_suite : plclic ::testing::Test{
    protected:
        test_comlog_hostname_suite(){};
        virtual ~test_comlog_hostname_suite(){};
        virtual void SetUp() {
        };
        virtual void TearDown() {
        };
};
 
TEST_F(test_comlog_hostname_suite, hostname)
{
	int ret;

	ret = com_loadlog("conf", "hostname.conf");
	ASSERT_EQ(ret, 0);

	ret = com_writelog(COMLOG_FATAL, "Ying chao begin");
	ASSERT_EQ(ret, 0);

	sleep(1);

	ret = com_closelog();
	ASSERT_EQ(ret, 0);
}
