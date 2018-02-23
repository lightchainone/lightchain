

#ifndef  __BSL_TEST_HASH_QA_H_
#define  __BSL_TEST_HASH_QA_H_

#include <cxxtest/TestSuite.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <Lsc/containers/hash/Lsc_hashmap.h>
#include <Lsc/containers/hash/Lsc_phashmap.h>
#include <Lsc/containers/hash/Lsc_rwhashset.h>
#include <Lsc/containers/hash/Lsc_readset.h>
#include <Lsc/archive/Lsc_filestream.h>
#include <Lsc/archive/Lsc_serialization.h>
#include <Lsc/archive/Lsc_binarchive.h>
#include <Lsc/alloc/allocator.h>
#include <vector>
#include <map>

#define __XASSERT(flag, fmt, arg...)  \
{\
	bool ___Lsc_flag = flag; \
	if (!(___Lsc_flag)) { \
		fprintf(stdout, "\n[error][%s:%d][%s]"fmt"\n", __FILE__, __LINE__, #flag, ##arg); \
		return false; \
	}\
}

#define __XASSERT2(flag) __XASSERT(flag, "")




bool test_hash_qa ()
{
	{
		Lsc::hashmap<int, int> h;
		__XASSERT2(h.create((1UL<<10UL) - 1UL) == 0);
	}
	if (0) {
		Lsc::hashmap<int, int> h;
		if (sizeof(void *) == sizeof(char)*4) {
			__XASSERT2(h.create(1<<30) != 0);
		} else {
			__XASSERT2(h.create(1<<30) == 0);
		}
	}
	return true;
}

bool test_hash_qa2 ()
{
	Lsc::phashmap<int, int> v;
	std::map<int, int> h;
	int num = 10;
	for (int i=0; i<num; ++i) {
		h[i] = i;
	}
	__XASSERT2(v.create(1000) == 0);
	__XASSERT2(v.assign(h.begin(), h.end()) == 0);
#if 0
	for (Lsc::phashmap<int, int>::iterator iter=v.begin();
			iter != v.end(); ++iter) {
		std::cout<<iter->first<<" "<<iter->second<<std::endl;
	}
#endif
	__XASSERT(v.size() == (size_t)num, "size(%zu) num(%d)", v.size(), num);
	return true;
}

#if 0
class cmpstr
{
plclic:
	inline size_t  operator () (const char * key) const {
		size_t ptr  = 0;
		for (size_t i=0; key[i] != 0; ++i) {
			ptr = ptr * 26 + (size_t)key[i] - 'a';
		}
		return ptr;
	}
};
#endif
typedef size_t (*cmpstr_t) (const char *key);
size_t cmpstr (const char * key) {
	size_t ptr  = 0;
	for (size_t i=0; key[i] != 0; ++i) {
		ptr = ptr * 26 + (size_t)key[i] - 'a';
	}
	return ptr;
}

bool test_hash_qa3 ()
{
	Lsc::Lsc_rwhashset<char *, cmpstr_t, std::equal_to<char *>,
		Lsc::Lsc_cpsalloc<Lsc::Lsc_alloc<char *> > > vs;
	__XASSERT2(vs.create(1000, 10, cmpstr) == 0);
	vs.set("hello");
	vs.set("hell3");
	vs.set("hell2");
	vs.set("hello");
	__XASSERT2(vs.size() == 3);
	vs.erase("hello");
	__XASSERT2(vs.size() == 2);
	vs.clear();

	return true;
}

void * _fun_ (void *param)
{
	Lsc::Lsc_rwhashset<char *> *vs = (Lsc::Lsc_rwhashset<char *> *)param;
	for (int i=0; i<100; ++i) vs->set(0);
	return NULL;
}

bool test_hash_qa4 ()
{
	Lsc::Lsc_rwhashset<char *> vs;
	__XASSERT2(vs.create(1000) == 0);
	pthread_t pid[10];
	for (size_t i=0; i<sizeof(pid)/sizeof(pid[0]); ++i) {
		pthread_create(pid+i, NULL, _fun_, &vs);
	}
	for (size_t i=0; i<sizeof(pid)/sizeof(pid[0]); ++i) {
		pthread_join(pid[i], NULL);
	}
	std::cout<<"----------------"<<vs.size()<<std::endl;
	__XASSERT2(vs.size() == 1);

	return true;
}
template <size_t size>
struct type_t
{
	char data[size];
};

// htt
template <size_t size>
void set_type(type_t<size> & __key, int n)
{
	assert (size >= sizeof(int));
	int * dat = (int *)__key.data;
	*dat = n;
}

// htt
template <size_t size>
struct hash_key
{
	size_t operator () (const type_t<size> &__key)const {
		assert (size >= sizeof(int));
		return *((int *)__key.data);
    }
};

template <size_t size>
struct hash_equal
{	
    bool operator () (const type_t<size> &__key1, const type_t<size> &__key2)const {
		for(int i = 0; i < (int)size; i++){
			if(__key1.data[i] != __key2.data[i]){
				return false;
			}
		}
		return true;
    }
};

template <size_t size>
bool test_hash_qa5()
{
		std::cout<<"Lsc_readset"<<size<<std::endl;
		typedef Lsc::readset<type_t<size>, hash_key<size> ,hash_equal<size> > rm_t;
		//./.bench --hash_bucket=2000000 --hash_datanum=1000000 --thread_num=1 --value_size=128 --query_loop=1
		int hash_bucket = 2000000;
		int hash_datanum = 1000000;

		rm_t hash;
		hash.create(hash_bucket);
		std::cout<<std::endl;
		std::vector<type_t<size> > vec;
		type_t<size> v;
		for (int i=0; i<(int)hash_datanum; ++i) {
			set_type(v, i);
			vec.push_back(v);
		}
		std::cout<<"Time stamp(ass): "<<clock() / (CLOCKS_PER_SEC/1000) <<std::endl;
		assert (hash.assign(vec.begin(), vec.end()) == 0);
		std::cout<<"Time stamp(test): "<<clock() / (CLOCKS_PER_SEC/1000) <<std::endl;

		int loop = 10000000;
		//test_query<rm_t, type_t<size> >(hash, p);
		for (int i=0; i<loop; ++i) {
			set_type(v, rand()%hash_datanum);
			hash.get(v);
		}

		std::cout<<"Time stamp(end): "<<clock() / (CLOCKS_PER_SEC/1000) <<std::endl;
		hash.clear();

		return true;
}

class Lsc_test_hash_qa : plclic CxxTest::TestSuite
{
plclic:
	void test_hash_qa_ () {
		TSM_ASSERT ("", test_hash_qa());
	}
	void test_hash_qa2_ () {
		TSM_ASSERT ("", test_hash_qa2());
	}
	void test_hash_qa3_ () {
		TSM_ASSERT ("", test_hash_qa3());
	}
	void test_hash_qa4_ () {
		TSM_ASSERT ("", test_hash_qa4());
	}
	void test_hash_qa5_ () {
		TSM_ASSERT ("", test_hash_qa5<128>());
	}
};








#endif  //__BSL_TEST_HASH_QA_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
