

#ifndef  __BSL_TEST_PHASHMAP_DUMP_H_
#define  __BSL_TEST_PHASHMAP_DUMP_H_

#include <header_string.hpp>

const int DATANUM = 1<<20;
//const int DATANUM = 10;
const char *g_dumpfile = "Lsc_test_phashmap_dump.dat";

template <typename HASH>
void * __query__(void *param)
{
	auto_timer t("query time at one thread");
	HASH *test = (HASH *)param;

	for (int i=0; i<DATANUM/10; ++i) {
		Lsc::string str = randstr();
		Lsc::string val;
		int ret = test->get(str, &val);
		if (ret == Lsc::HASH_EXIST) {
			__XASSERT2_R(str == val);
		}
	}

	fprintf(stderr, "byebye query\n");
	return NULL;
}

std::set<Lsc::string> g_set;

#include <pthread.h>
pthread_mutex_t g_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t g_cond = PTHREAD_COND_INITIALIZER;

static bool g_flagrun = true;
static bool g_cpend = false;

template <typename HASH>
void * __update__(void *param)
{
	auto_timer t("update time");
	HASH *test = (HASH *)param;

	for (int i=0; i<DATANUM; ++i) {
		Lsc::string str = randstr();
		g_set.insert(str);
		test->set(str, str);
	}

	fprintf(stderr, "start to checkpoint\n");
	pthread_mutex_lock (&g_lock);
	if (test->make_checkpoint() != 0) {
		g_flagrun = false;
	}
	g_cpend = true;
	fprintf(stderr, "singal post %d\n", (int)g_flagrun);
	pthread_cond_signal(&g_cond);
	pthread_mutex_unlock(&g_lock);

	__XASSERT2_R(g_flagrun);

	fprintf(stderr, "inserrt again\n");
	
	for (int i=0; i<DATANUM; ++i) {
		Lsc::string str = randstr();
		test->set(str, str);
	}

	fprintf(stderr, "byebye insert\n");

	return NULL;
}

template <typename HASH>
void *__dump__(void *param)
{
	auto_timer t("dump");
	HASH *test = (HASH *)param;

	fprintf(stderr, "start dump\n");
	pthread_mutex_lock(&g_lock);
	if (g_cpend) {
		pthread_mutex_unlock(&g_lock);
		return NULL;
	}
	pthread_cond_wait(&g_cond, &g_lock);
	pthread_mutex_unlock(&g_lock);
	fprintf(stderr, "sart to dump to file\n");

	__XASSERT2_R(g_flagrun);

	Lsc::filestream fs;
	__XASSERT2_R(fs.open(g_dumpfile, "w") == 0);
	Lsc::binarchive ar(fs);
	__XASSERT2_R(test->serialization(ar) == 0);
	fs.close();

	HASH *q = new HASH;
	__XASSERT2_R(q->create(1<<20) == 0);
	__XASSERT2_R(fs.open(g_dumpfile, "r") == 0);
	Lsc::binarchive inar(fs);
	__XASSERT2_R(q->deserialization(ar) == 0);
	fs.close();

	for (typename HASH::iterator iter = q->begin();
			iter != q->end(); ++iter) {
		__XASSERT2_R(g_set.find(iter->first) != g_set.end());
	}

	fprintf(stderr, "byebye dump");
	return NULL;
}

template <typename HASH>
bool test_phashmap_dump()
{
	//保存文件明 Lsc_test_phashmap_dump.dat
	//10个线程查询，1个线程更新，1个线程等待dump
	__XASSERT2(
			test_phashmap_temp<HASH>(
				10, __query__<HASH>, 
				1, __update__<HASH>, 
				1, __dump__<HASH>)
			);
	//检查load数据是否正确
	
	return true;
}

class Lsc_test_phashmap_dump : plclic CxxTest::TestSuite
{
plclic:
	typedef Lsc::phashmap<Lsc::string, Lsc::string, hash_key> type1_t;
	void test_phashmap_dump_ () {
		TSM_ASSERT("test_phashmap_dump", test_phashmap_dump<type1_t>());
	}
};


#endif  //__BSL_TEST_PHASHMAP_DUMP_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
