

#ifndef  __BSL_TEST_ALLOC_H_
#define  __BSL_TEST_ALLOC_H_
#include <cxxtest/TestSuite.h>
#include <Lsc/alloc/allocator.h>
#include <vector>


#define __XASSERT(flag, fmt, arg...)  \
{\
	bool ___Lsc_flag = flag; \
	if (!(___Lsc_flag)) { \
		fprintf(stdout, "\n[error][%s:%d][%s]"fmt"\n", __FILE__, __LINE__, #flag, ##arg); \
		return false; \
	}\
}

#define __XASSERT2(flag) __XASSERT(flag, "")



template <class _Alloc>
bool test_normal()
{
	typedef typename _Alloc::value_type value_type;
	typedef typename _Alloc::pointer pointer;
	typedef std::vector<value_type> vec_t;
	typedef std::vector<pointer> vecp_t;

	_Alloc alloc;
	__XASSERT2(alloc.create() == 0);
	vec_t vecs;
	vecp_t vecp;
	int loop = 100000;
	for (int i=0; i<loop; ++i) {
		pointer p = alloc.allocate(1);
		__XASSERT2(p != 0);
		value_type v = rand();
		(*alloc.getp(p)) = v;
		vecs.push_back(v);
		vecp.push_back(p);
	}
	for (int i=0; i<loop; ++i) {
		__XASSERT2(*alloc.getp(vecp[i]) == vecs[i]);
	}
	for (int i=0; i<loop; ++i) {
		alloc.deallocate(vecp[i], 1);
	}

	for (int i=0; i<loop; ++i) {
		pointer p = alloc.allocate(1);
		__XASSERT2(p != 0);
		alloc.deallocate(p, 1);
	}

	vecs.clear();
	vecp.clear();

	for (int i=0; i<loop; ++i) {
		pointer p = alloc.allocate(1);
		__XASSERT2(p != 0);
		value_type v = rand();
		(*alloc.getp(p)) = v;
		vecs.push_back(v);
		vecp.push_back(p);
	}
	for (int i=0; i<loop; ++i) {
		__XASSERT2(*alloc.getp(vecp[i]) == vecs[i]);
		alloc.deallocate(vecp[i], 1);
	}
	return true;
}

class Lsc_test_fun : plclic CxxTest::TestSuite
{
plclic:
	typedef Lsc::Lsc_alloc<int> intalloc_t;
	void test_normal_int(void) {   
		TSM_ASSERT (0, test_normal<intalloc_t>());
	}   

	typedef Lsc::Lsc_alloc<dolcle> dballoc_t;
	void test_normal_dolcle() {
		TSM_ASSERT (0, test_normal<dballoc_t>());
	}

	typedef Lsc::Lsc_sample_alloc<intalloc_t, 256 > intsalloc_t;
	void test_salloc_int(void) {
		TSM_ASSERT (0, test_normal<intsalloc_t>());
	}

	typedef Lsc::Lsc_sample_alloc<dballoc_t, 256> dbsalloc_t;
	void test_salloc_dolcle() {
		TSM_ASSERT (0, test_normal<dbsalloc_t>());
	}

	typedef Lsc::Lsc_cpsalloc<intalloc_t> intcpalloc_t;
	void test_cpalloc_int() {
		TSM_ASSERT (0, test_normal<intcpalloc_t>());
	}

	typedef Lsc::Lsc_cpsalloc<dballoc_t> dbcpalloc_t;
	void test_cpalloc_dolcle() {
		TSM_ASSERT (0, test_normal<dbcpalloc_t>());
	}
};




#endif  //__BSL_TEST_ALLOC_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 noet: */
