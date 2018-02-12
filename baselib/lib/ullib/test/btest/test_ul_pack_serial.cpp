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
 
#define BUF_SIZE	(1024)
#define ITEM_SIZE	(2048)
#define ITEM_NUM	6
TEST_F(test_ul_pack_suite, serial)
{
	int ret = 0;
	ul_package_t *send = NULL, *recv = NULL;

	// init
	send = ul_pack_init();
	if (send == NULL) {
		ASSERT_EQ(0, -1);
	}

	// buf 1M
	char buf[BUF_SIZE] = "Du lang de na xie shi!";
	ret = ul_pack_putbuf(send, buf, BUF_SIZE);
	EXPECT_EQ(0, ret);

	// head
	char key[ITEM_SIZE], value[ITEM_SIZE];
	char key1[ITEM_SIZE], value1[ITEM_SIZE];
	for (int i = 0; i < ITEM_SIZE - 4; i++) {
		key1[i] = 'k';
		value1[i] = 'v';
	}
	for (int i = 0; i < ITEM_NUM; i++) {
		snprintf(key, ITEM_SIZE, "%s%d", key1, i);
		snprintf(value, ITEM_SIZE, "%s%d", value1, i);
		ret = ul_pack_putvalue(send, key, value);
		EXPECT_EQ(0, ret);
	}

	// serial
	char membuf[102400];
	int sendsize = ul_pack_serialization(membuf, 102400, send);
	EXPECT_LT(0, sendsize);
	/* print pack info
	for(int i = 0; i < ret; i++) {
		printf("%c", *((char *)membuf+i));
	}
	printf("\nsize=%d", ret);
	*/

	// deserial
	recv = ul_pack_init();
	if (recv == NULL) {
		ASSERT_EQ(0, -1);
	}
	char buf1[BUF_SIZE];
	ret = ul_pack_putbuf(recv, buf1, BUF_SIZE);
	EXPECT_EQ(0, ret);

	ret = ul_pack_deserialization(NULL, 1, recv);
	ASSERT_EQ(-1, ret);
	ret = ul_pack_deserialization(membuf, 0, recv);
	ASSERT_EQ(-1, ret);
	ret = ul_pack_deserialization(membuf, 1, NULL);
	ASSERT_EQ(-1, ret);

	int recvsize = ul_pack_deserialization(membuf, 102400, recv);
	ASSERT_EQ(recvsize, sendsize);
	EXPECT_EQ(send->pairnum, recv->pairnum);
	for (int i = 0; i < recv->pairnum; i++) {
		EXPECT_STREQ(send->name[i], recv->name[i]);
		EXPECT_STREQ(send->value[i], recv->value[i]);
	}
	EXPECT_EQ(recv->bufsize, send->bufsize);
	EXPECT_STREQ(recv->buf, send->buf);

	// free
	ul_pack_free(send);
	ul_pack_free(recv);
}
