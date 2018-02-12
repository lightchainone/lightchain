#include <ul_pack.h>
#include <gtest/gtest.h>

int main(int argc, char **argv)
{
 	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS_IN_PROC();
}

/**
**/
class test_ul_pack_suite : plclic ::testing::Test{
    protected:
        test_ul_pack_suite(){};
        virtual ~test_ul_pack_suite(){};
        virtual void SetUp() {
        };
        virtual void TearDown() {
        };
};
 
#define BUF_SIZE	(1024*1024)
#define ITEM_SIZE	(2048)
TEST_F(test_ul_pack_suite, client)
{
	int ret = 0;
	ul_package_t *send = NULL;

	// init
	send = ul_pack_init();
	if (send == NULL) {
		ASSERT_EQ(0, -1);
	}

	// buf 1M
	char buf[BUF_SIZE] = "Du lang";
	for (int i = 7; i < BUF_SIZE; i++) {
		buf[i] = 'i';
	}
	buf[BUF_SIZE - 1] = 0;
	ret = ul_pack_putbuf(send, buf, BUF_SIZE);
	EXPECT_EQ(0, ret);

	// head
	char key[ITEM_SIZE], value[ITEM_SIZE];
	char key1[ITEM_SIZE], value1[ITEM_SIZE];
	for (int i = 0; i < ITEM_SIZE - 4; i++) {
		key1[i] = 'k';
		value1[i] = 'v';
	}
	key1[ITEM_SIZE - 4] = 0;
	value1[ITEM_SIZE - 4] = 0;
	for (int i = 0; i < 6; i++) {
		snprintf(key, ITEM_SIZE, "%s%d", key1, i);
		snprintf(value, ITEM_SIZE, "%s%d", value1, i);
		ret = ul_pack_putvalue(send, key, value);
		EXPECT_EQ(0, ret);
	}

	// write
	int client_fd = ul_tcpconnect("127.0.0.1", 6666);
//	int client_fd = ul_tcpconnect("10.81.43.28", 6666);
	ASSERT_LT(0, client_fd);

	ret = ul_pack_writeo_ms(client_fd, send, 1000);
	ASSERT_LE(0, ret);

	// free
	ul_pack_free(send);
}
