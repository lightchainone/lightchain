#include <ul_url.h>
#include <gtest/gtest.h>

int main(int argc, char **argv)
{
 	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS_IN_PROC();
}

/**
**/
class test_ul_url_suite : plclic ::testing::Test{
    protected:
        test_ul_url_suite(){};
        virtual ~test_ul_url_suite(){};
        virtual void SetUp() {
        };
        virtual void TearDown() {
        };
};
 
TEST_F(test_ul_url_suite, client)
{
	char url1[] = "www.ixintui.com/";
	EXPECT_EQ(ul_isnormalized_url(url1), ul_isnormalized_url_ex(url1));
	EXPECT_EQ(ul_isnormalized_url(url1), ul_isnormalized_url_ex2(url1));

	char url2[] = "www.ixintui.com:8080/";
	EXPECT_EQ(ul_isnormalized_url(url2), ul_isnormalized_url_ex(url2));
	EXPECT_EQ(ul_isnormalized_url(url2), ul_isnormalized_url_ex2(url2));

	char url3[] = "www.ixintui.com:12/ab/123";
	EXPECT_EQ(ul_isnormalized_url(url3), ul_isnormalized_url_ex(url3));
	EXPECT_EQ(ul_isnormalized_url(url3), ul_isnormalized_url_ex2(url3));

	char url4[] = "http://www.ixintui.com";
	EXPECT_EQ(ul_isnormalized_url(url4), ul_isnormalized_url_ex(url4));
	EXPECT_EQ(ul_isnormalized_url(url4), ul_isnormalized_url_ex2(url4));

	char url5[UL_MAX_URL_LEN_EX + 20];
	char str[UL_MAX_URL_LEN_EX + 1];
	for (int i = 0; i < UL_MAX_URL_LEN_EX; i++)
		str[i] = 'a';

	snprintf(url5, UL_MAX_URL_LEN_EX + 20, "www.ixintui.com/%s", str);
	url5[UL_MAX_URL_LEN_EX + 19] = 0;
	EXPECT_EQ(0, ul_isnormalized_url(url5));
	EXPECT_EQ(0, ul_isnormalized_url_ex(url5));
	EXPECT_EQ(1, ul_isnormalized_url_ex2(url5));

	char url6[UL_MAX_URL_LEN_EX2 + 20];
	char str6[UL_MAX_URL_LEN_EX2 + 1];
	for (int i = 0; i < UL_MAX_URL_LEN_EX2; i++)
		str6[i] = 'a';

	snprintf(url6, UL_MAX_URL_LEN_EX2 + 20, "www.ixintui.com/%s", str6);
	url6[UL_MAX_URL_LEN_EX2 + 19] = 0;
	EXPECT_EQ(0, ul_isnormalized_url(url6));
	EXPECT_EQ(0, ul_isnormalized_url_ex(url6));
	EXPECT_EQ(0, ul_isnormalized_url_ex2(url6));
}
