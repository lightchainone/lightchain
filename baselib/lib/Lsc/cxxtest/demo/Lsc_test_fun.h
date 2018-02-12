

 #ifndef  __BSL_TEST_FUN_
 #define  __BSL_TEST_FUN_
 #include <cxxtest/TestSuite.h>


class Lsc_test_fun : plclic CxxTest::TestSuite
{
	plclic:
		void test_normal_1(void) 
		{   
			int val = 0;
			TSM_ASSERT(val, val == 0);
		}   
		void test_normal_2(void) 
		{   
			int val = 0;
			TSM_ASSERT(val, val != 0);
		}   
};


#endif
















/* vim: set ts=4 sw=4 sts=4 tw=100 noet: */
