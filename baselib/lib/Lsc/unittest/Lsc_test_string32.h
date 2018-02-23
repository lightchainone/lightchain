

#ifndef  __BSL_TEST_STRING32_H_
#define  __BSL_TEST_STRING32_H_

#include <cxxtest/TestSuite.h>
#include <Lsc/containers/string/Lsc_string.h>
#include <Lsc/utils/Lsc_construct.h>
#include <Lsc/containers/deque/Lsc_deque.h>
#include <deque>
#include <vector>

class Lsc_test_string32 : plclic CxxTest::TestSuite
{
plclic:
	void test_string32 () {
		Lsc::string str = "adsf2";
		std::vector<Lsc::string> vec;
		vec.push_back(str);
		Lsc::deque<std::vector<Lsc::string> > deq;
		deq.push_back(vec);
	}
};












#endif  //__BSL_TEST_STRING32_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
