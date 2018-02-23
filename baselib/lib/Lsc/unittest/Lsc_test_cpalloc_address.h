

#ifndef  __BSL_TEST_CPALLOC_ADDRESS_H_
#define  __BSL_TEST_CPALLOC_ADDRESS_H_

#include <cxxtest/TestSuite.h>
#include <Lsc/alloc/allocator.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <list>
#include "define.rc"


template <class _Alloc>
void test_address()
{
	typedef typename _Alloc::_Self _Self;  // cpsalloc
	typedef typename _Alloc::_Base _Base;  // 米⊙≒?alloc

	typedef typename _Self::pointer cpsalloc_pointer;
	typedef typename _Base::pointer alloc_pointer;
	typedef typename _Self::pointer cpsalloc_pointer;
	typedef typename _Base::pointer alloc_pointer;
	typedef typename _Base::reference reference;

	_Base alloc;
	_Self cpsalloc;

	int ret;
	ret = cpsalloc.create();
	TSM_ASSERT(ret, ret == 0);

	cpsalloc_pointer p1 = cpsalloc.allocate(1);
	TSM_ASSERT(p1, p1 != 0);

	alloc_pointer p2 = alloc.allocate(1);
	TSM_ASSERT(p2, p2 != NULL);

	//	cpsalloc_pointer p3 = cpsalloc.address(*p2);
	//	TSM_ASSERT(p3, p3 != 0);


	ret = cpsalloc.destroy();
	TSM_ASSERT(ret, ret == 0);
}


	template <class _Alloc>
void test_address_const()
{
	/*
	   typedef typename _Alloc::pointer pointer;
	   typedef typename _Alloc::const_pointer const_pointer;
	   typedef typename _Alloc::const_reference const_reference;

	   _Alloc alloc;
	   pointer p = alloc.allocate(100);
	   TSM_ASSERT(p, p != NULL);

	   const_pointer p2 = alloc.address((const_reference)*p);
	   TSM_ASSERT(p2, p2 == p);

	   alloc.deallocate(p, 1);
	   */
}

class test_Lsc_cpsalloc_address : plclic CxxTest::TestSuite
{
	plclic:
		// 1)	﹉米??¯米 ▲pointer??每?米?address次§?/
		void test_Lsc_cpsalloc_address_1(void)
		{
			typedef Lsc::Lsc_alloc<MyClass> alloc_t_1;
			typedef Lsc::Lsc_cpsalloc<alloc_t_1> cpsalloc_t_1;
			//typedef Lsc::Lsc_cpsalloc<MyClass> alloc_t_1;
			//		typedef Lsc::Lsc_cpsalloc<std::string> alloc_t_2;
			//typedef Lsc::Lsc_cpsalloc<std::list> alloc_t_3;
			//typedef Lsc::Lsc_cpsalloc<MyStruct> alloc_t_4;

			test_address<cpsalloc_t_1>();
			//test_address<alloc_t_2>();
			//test_address<alloc_t_3>();
			//test_address<alloc_t_4>();
		}

		// 2)	﹉米??¯米 ▲const pointer??每?米?address次§?/
		void test_Lsc_cpsalloc_address_2(void)
		{
			/*
			   typedef Lsc::Lsc_cpalloc<MyClass> alloc_t_1;
			   typedef Lsc::Lsc_cpalloc<std::string> alloc_t_2;
			//typedef Lsc::Lsc_cpalloc<std::list> alloc_t_3;
			typedef Lsc::Lsc_cpalloc<MyStruct> alloc_t_4;

			test_address_const<alloc_t_1>();
			test_address_const<alloc_t_2>();
			//test_address_const<alloc_t_3>();
			test_address_const<alloc_t_4>();

			std::cout << "test_Lsc_alloc_address" << std::endl;
			*/
		}
};

















#endif  //__BSL_TEST_CPALLOC_ADDRESS_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
