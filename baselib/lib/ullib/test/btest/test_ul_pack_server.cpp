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
	ul_package_t *recv = NULL;

	// init
	recv = ul_pack_init();
	if (recv == NULL) {
		EXPECT_EQ(0, -1);
	}

	// listen and accept
	int listen_fd = ul_tcplisten(6666, 5);
	ASSERT_LT(0, listen_fd);

	struct sockaddr_in saddr;
	socklen_t addrsize;    
	int server_fd = ul_accept(listen_fd, (struct sockaddr *)&saddr, &addrsize);
	if (server_fd < 0) {
		ul_sclose(listen_fd);
		ASSERT_LT(0, server_fd);
	}

	// read
	char buf[BUF_SIZE];
	ul_pack_putbuf(recv, buf, BUF_SIZE);

	ret = ul_pack_reado_ms(server_fd, recv, 1000);
	ASSERT_LT(0, ret);

	char key[ITEM_SIZE], value[ITEM_SIZE];
	char key1[ITEM_SIZE], value1[ITEM_SIZE];
	for (int i = 0; i < ITEM_SIZE - 4; i++) {
		key1[i] = 'k';
		value1[i] = 'v';
	}
	key1[ITEM_SIZE - 4] = 0;
	value1[ITEM_SIZE - 4] = 0;
	for (int i = 0; i < recv->pairnum; i++) {
		snprintf(key, ITEM_SIZE, "%s%d", key1, i);
		EXPECT_STREQ(key, recv->name[i]);
		snprintf(value, ITEM_SIZE, "%s%d", value1, i);
		EXPECT_STREQ(value, recv->value[i]);
	}
	char buf1[BUF_SIZE] = "Du lang";
	for (int i = 7; i < BUF_SIZE; i++) {
		buf1[i] = 'i';
	}
	buf1[BUF_SIZE - 1] = 0;
	EXPECT_STREQ(buf1, recv->buf);

	// free
	ul_pack_free(recv);
}
