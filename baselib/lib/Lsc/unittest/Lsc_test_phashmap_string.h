

#ifndef  __BSL_TEST_PHASHMAP_H_
#define  __BSL_TEST_PHASHMAP_H_

#include <header_string.hpp>

static const int DATANUM = 100000;

/////////////
/**
 * 测试多线程的读写安全情况
 * 这个case测试两个hash表，
 * 一个是phash，一个是rwhash
 */
template <class HASH>
void * __test_phashmap_fun_query(void *param)
{
	auto_timer t("query time at one thread");
	HASH * test = (HASH *)param;
	__XASSERT2(test!=NULL);
	int qloop = DATANUM / 20;
	for (int i=0; i<qloop; ++i) {
		//if (i % (1<<15) == 0) fprintf(stderr, "query success[%d]\n", i);
		Lsc::string str = randstr();
		Lsc::string val;
		int ret = test->get(str, &val);
		if (ret == Lsc::HASH_EXIST) {
			__XASSERT2_R(str == val);
		}
	}

	return NULL;
}
template <class HASH>
void * __test_phashmap_fun_update(void *param)
{
	//auto_timer t("update time at one thread");
	HASH *test = (HASH *)param;
	__XASSERT2(test!=NULL);
	int uloop = DATANUM;
	for (int i=0; i<uloop; ++i) {
		//if (i % (1<<15) == 0) fprintf(stderr, "update success[%d]\n", i);
		Lsc::string s1 = randstr();
		__XASSERT2_R(test->set(s1, s1) != -1);
		test->erase(randstr());
	}
	return NULL;
}


template <class HASH>
bool test_phashmap()
{
	return test_phashmap_temp<HASH>(20, __test_phashmap_fun_query<HASH>, 
			1, __test_phashmap_fun_update<HASH>);
}

//测试在写情况下dump数据问题
template <class HASH>
void * __test_cp_fun1(void *param)
{
	HASH *test = (HASH *)param;
	__XASSERT2(test != NULL);
	int loop = 1000;
	for (int i=0; i<loop; ++i) {
		test->make_checkpoint();
	}
	return NULL;
}
template <class HASH>
void * __test_cp_fun2(void *param)
{
	HASH *test = (HASH *)param;
	__XASSERT2(test != NULL);
	int loop = 1000;
	for (int i=0; i<loop; ++i) {
		test->end_checkpoint();
	}

	return NULL;
}

template <class HASH>
bool test_phashmap_cp1()
{
	return test_phashmap_temp<HASH>(10, __test_cp_fun1<HASH>,
			10, __test_cp_fun2<HASH>);
}



//测试在写情况下dump数据问题
template <class HASH>
void * __test_cp_fun3(void *param)
{
	HASH *test = (HASH *)param;
	__XASSERT2_R(test != NULL);
	int num = DATANUM;
	for (int i=0; i<num; ++i) {
		__XASSERT2_R(test->set(int2str(i), int2str(i)) == Lsc::HASH_INSERT_SEC);
	}
	{
		auto_timer t("make checkpoint");
		__XASSERT2_R(test->make_checkpoint() == 0);
	}
	for (int i=0; i<num; ++i) {
		auto_timer t("1 set", i);
		__XASSERT2_R(test->set(int2str(i), int2str(num/2 - i), 1) == Lsc::HASH_OVERWRITE);
	}
#if 0
	//check
	for (int i=0; i<num; ++i) {
		Lsc::string val;
		__XASSERT2_R(test->get(int2str(i), &val) == Lsc::HASH_EXIST);
		__XASSERT2_R(val == int2str(num/2 - i));
	}
	__BSL_ERROR("start to erase");
#endif
	for (int i=0; i<num; ++i) {
		auto_timer t("2 erase", i);
		__XASSERT_R(test->erase(int2str(i)) == Lsc::HASH_EXIST, "erase %d %d", i, 
				test->dumpstatus());
	}
	for (int i=0; i<num; ++i) {
		auto_timer t("3 set", i);
		__XASSERT2_R(test->set(int2str(i), int2str(num - i)) == Lsc::HASH_INSERT_SEC);
	}
	return NULL;
}
template <class HASH>
void * __test_cp_fun4(void *param)
{
	HASH *test = (HASH *)param;
	__XASSERT2_R(test != NULL);
	while (test->dumpstatus() == Lsc::PHASH_NORMAL) {
		struct timespec tv;
		tv.tv_sec = 0;
		tv.tv_nsec = 10 * 1000;
	
		nanosleep(&tv, NULL);
	}
	Lsc::filestream fd;
	__XASSERT2_R(fd.open("phash.dat.1", "w") == 0);
	Lsc::binarchive ar(fd);
	__XASSERT2_R(ar.write(*test) == 0);
	fd.close();

	__XASSERT2_R(fd.open("phash.dat.1", "r") == 0);
	Lsc::binarchive ar2(fd);
	HASH test2;
	__XASSERT2_R(ar.read(&test2) == 0);
	fd.close();

	int num = DATANUM;
	__BSL_DEBUG("---------start to check size %d", (int)test2.size());
	for (int i=0; i<num; ++i) {
		Lsc::string val;
		__XASSERT2_R(test2.get(int2str(i), &val) == Lsc::HASH_EXIST);
		__XASSERT2_R(val == int2str(i));
	}

	return NULL;
}

template <class HASH>
void * __test_cp_fun5(void *param)
{
	HASH *test = (HASH *)param;
	__XASSERT2_R(test != NULL);
	int loop = DATANUM / 20;
	for (int i=0; i<loop; ++i) {
		test->get(int2str(i));
	}
	return NULL;
}

template <class HASH>
bool test_phashmap_cp2()
{
	return test_phashmap_temp<HASH>(1, __test_cp_fun3<HASH>,
			1, __test_cp_fun4<HASH>, 20, __test_cp_fun5<HASH>);
}

class Lsc_test_phashmap : plclic CxxTest::TestSuite
{
plclic:
	typedef Lsc::phashmap<Lsc::string, Lsc::string, hash_key> type1_t;
	void test_phashmap_(void) {
		TSM_ASSERT ("", test_phashmap<type1_t>());
	}
	void test_phashmap_cp1_() {
		TSM_ASSERT ("", test_phashmap_cp1<type1_t>());
	}
	void test_phashmap_cp2_() {
		TSM_ASSERT ("", test_phashmap_cp2<type1_t>());
	}

	typedef Lsc::phashmap<Lsc::string, Lsc::string, hash_key, Lsc::equal<Lsc::string>,
			Lsc::Lsc_alloc<Lsc::string> > type2_t;
	void test_phashmap_2(void) {
		TSM_ASSERT ("", test_phashmap<type2_t>());
	}
	void test_phashmap_cp1_2() {
		TSM_ASSERT ("", test_phashmap_cp1<type2_t>());
	}
	void test_phashmap_cp2_2() {
		TSM_ASSERT ("", test_phashmap_cp2<type2_t>());
	}

	///
	typedef Lsc::phashmap<Lsc::string, Lsc::string, hash_key, Lsc::equal<Lsc::string>,
			Lsc::Lsc_cpsalloc<Lsc::Lsc_alloc<Lsc::string> > > type3_t;
	void test_phashmap_3(void) {
		TSM_ASSERT ("", test_phashmap<type3_t>());
	}
	void test_phashmap_cp1_3() {
		TSM_ASSERT ("", test_phashmap_cp1<type3_t>());
	}
	void test_phashmap_cp2_3() {
		TSM_ASSERT ("", test_phashmap_cp2<type3_t>());
	}

};

#endif  //__BSL_TEST_PHASHMAP_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 noet: */
