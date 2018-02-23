

#ifndef  __BSL_TEST_UTILS_H_
#define  __BSL_TEST_UTILS_H_

#include <cxxtest/TestSuite.h>
#include <Lsc/utils/Lsc_utils.h>
#include <iostream>
#include <stdio.h>

#define __XASSERT(flag, fmt, arg...)  \
{\
	bool ___Lsc_flag = flag; \
	if (!(___Lsc_flag)) { \
		fprintf(stdout, "\n[error][%s:%d][%s]"fmt"\n", __FILE__, __LINE__, #flag, ##arg); \
		return false; \
	}\
}

#define __XASSERT2(flag) __XASSERT(flag, "")

bool test_max () {
	int a = 10, b = 5, c = 10;
	int min = Lsc::min<int>(a, b);
	int max = Lsc::max<int>(a, b);
	if (min > max) { 
		max = min;
	}
	Lsc::equal<int>()(a, c);
	Lsc::more<int>()(a, b);
	Lsc::less<int>()(b, a);

	printf("\nhello world\n");
	//std::cout<<Lsc::high_bitpos<0x1234>::value<<std::endl;
	//std::cout<<Lsc::moreq2<0x1234>::value<<std::endl;
	//-------13
	//-------0x2000
	//-------0x8000
	//-------32768
	printf("-------%d\n", Lsc::high_bitpos<0x1234>::value);
	__XASSERT2(Lsc::high_bitpos<0x1234>::value == 13);
	printf("-------0x%lx\n", Lsc::moreq2<0x1234>::value);
	__XASSERT2(Lsc::moreq2<0x1234>::value == 0x2000UL);
	printf("-------0x%lx\n", Lsc::moreq2<0x5678>::value);
	__XASSERT2(Lsc::moreq2<0x5678>::value == 0x8000UL);
	printf("-------%ld\n", Lsc::moreq2<0x5678>::value);
	__XASSERT2(Lsc::moreq2<0x5678>::value == 32768UL);

	return true;
}

class Lsc_test_utils : plclic CxxTest::TestSuite
{
plclic:
	void test_max_() {
		TSM_ASSERT("", test_max());
	}
};

















#endif  //__BSL_TEST_UTILS_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 noet: */
