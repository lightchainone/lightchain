

#ifndef  __BSL_TEST_POOL_H_
#define  __BSL_TEST_POOL_H_


#include <Lsc/pool/Lsc_pool.h>
#include <Lsc/pool/Lsc_poolalloc.h>
#include <Lsc/pool/Lsc_xmempool.h>
#include <Lsc/pool/Lsc_xcompool.h>
#include <Lsc/pool/Lsc_debugpool.h>
#include <vector>
#include <cxxtest/TestSuite.h>
#include <vector>
#include <set>

#define __XASSERT(flag, fmt, arg...)  \
{\
	bool ___Lsc_flag = flag; \
	if (!(___Lsc_flag)) { \
		fprintf(stdout, "\n[error][%s:%d][%s]"fmt"\n", __FILE__, __LINE__, #flag, ##arg); \
		return false; \
	}\
}

#define __XASSERT2(flag) __XASSERT(flag, "")


typedef bool (*fun_t)(Lsc::mempool *);

bool test_normal_pool (Lsc::mempool *pool)
{
	std::vector<void *> vec;
	std::vector<size_t> vec2;
	//size_t sizv[] = {0, 4, 16, 1<<21, 1<<19};
	size_t vsize = 1000;
	size_t cnt = 0;
	for (size_t i=0; i<vsize; ++i) {
		size_t size = rand()%(1<<20)*2;//sizv[i];
		void * ptr = pool->malloc(size);
		if (ptr) {
			memset(ptr, 0, size);
			cnt += size;
			vec.push_back(ptr);
			vec2.push_back(size);
			void *ptr2 = vec.back();
			size_t ptr2siz = vec2.back();
		    pool->free (ptr2, ptr2siz);
		}
	}
	std::cout<<(cnt>>20)<<std::endl;
	vec.clear();
	vec2.clear();

	for (size_t i=0; i<vsize; ++i) {
		size_t size = rand()%(1<<20) * 2;
		void *ptr = pool->malloc (size);
		if (ptr) {
			memset (ptr, 0, size);
			cnt += size;
			vec.push_back(ptr);
			vec2.push_back(size);
		}
	}
	for (size_t i=0; i<vec.size(); ++i) {
		pool->free (vec[i], vec2[i]);
	}
	return true;
}

bool test_xmempool (fun_t op)
{
	Lsc::xmempool *pool = new Lsc::xmempool;
	size_t size = 1<<24;
	void *dat = malloc (size);
	pool->create(dat, size, 2, 1<<16, 1.8);
	__XASSERT2(op((Lsc::mempool *)pool));

	pool->clear();
	for (int i=0; i<1<<16; i = int(float(i)*1.5) + 1) 
		std::cout<<i<<" "<< pool->goodsize(i) <<" "<<pool->goodsize(pool->goodsize(i))<<" "
			<<pool->goodsize(pool->goodsize(i)+1)<<std::endl;

	//pool->destroy();
	delete pool;
	free (dat);
	{
		Lsc::xmempool pool2;
	}
	return true;
}

bool test_xcompool (fun_t op)
{
	Lsc::xcompool *pool = new Lsc::xcompool;
	pool->create(1<<20, 2,  1<<16, 2.0f);
	__XASSERT2(op((Lsc::mempool *)pool));
	pool->destroy();
	delete pool;
	{
		Lsc::xcompool pool2;
	}
	return true;
};

bool test_debugpool (fun_t op)
{
	Lsc::debugpool *pool = new Lsc::debugpool;
	__XASSERT2(op((Lsc::mempool *)pool));
	pool->free(pool->malloc(10), 10);
	delete pool;
	return true;
}

bool g_exp = true;
template <typename Tp>
bool test_stl (Lsc::mempool *pool)
{
#ifndef __i386
	typedef Lsc::pool_allocator<Tp> alloc;
	alloc a(pool);
	typedef std::vector<Tp, alloc> vector;
	vector v(a);
	typedef std::set<Tp, std::less<Tp>, alloc> set;
	set s(std::less<Tp>(), a);
	size_t num = 10000;
	try {
		for (size_t i=0; i<num; ++i) {
			v.push_back((Tp)(i));
			s.insert((Tp)(i));
		}
	} catch (...) {
		__XASSERT(g_exp, "exp");
	}
	std::sort(v.begin(), v.end(), std::greater<Tp>());
	for (size_t i=0; i<v.size(); ++i) {
		if (s.find((Tp)i) == s.end()) {
			__XASSERT(false, "not find");
		}
	}
#endif
	return true;
}

bool test_comp (Lsc::mempool *pool)
{
#ifndef __i386
	typedef Lsc::pool_allocator<char> alloc_c;
	typedef std::basic_string<char, std::char_traits<char>, alloc_c> string;
	typedef Lsc::pool_allocator<string> alloc_s;
	typedef std::vector<string, alloc_s> vector;
	typedef std::set<string, std::less<string>, alloc_s> set;

	alloc_c ca(pool);
	alloc_s cs(pool);

	string a(ca);
	a = "helloa";
	string b(ca);
	b = "hellob";
	string c(ca);
	c = "helloc";

	size_t slen = 1000;
	vector v(cs);
	for (size_t i=0; i<slen; ++i) {
		v.push_back(a);
		v.push_back(b);
		v.push_back(c);
	}
	std::sort(v.begin(), v.end(), std::greater<string>());
	set s(std::less<string>(), cs);
	for (size_t i=0; i<v.size(); ++i) {
		s.insert(v[i]);
	}
#endif

	return true;
}

bool test_compool (Lsc::mempool *pool)
{
	for (int i=0; i<1<<24; ++i) {
		__XASSERT2(pool->malloc(rand()%16+1) != NULL);
	}
	return true;
}

class Lsc_test_pool : plclic CxxTest::TestSuite
{
plclic:
	void test_normal_pool_ () {
		TSM_ASSERT(0, test_xmempool(test_normal_pool));
		TSM_ASSERT(0, test_xcompool(test_normal_pool));
		TSM_ASSERT(0, test_xcompool(test_compool));
	}
	void test_stl_pool_ () {
		g_exp = true;
		TSM_ASSERT(1, test_xmempool(test_stl<int>));
		TSM_ASSERT(1, test_xmempool(test_stl<char>));
		TSM_ASSERT(1, test_xmempool(test_stl<float>));
		TSM_ASSERT(1, test_xmempool(test_stl<dolcle>));

		g_exp = false;
		TSM_ASSERT(1, test_xcompool(test_stl<int>));
		TSM_ASSERT(1, test_xcompool(test_stl<char>));
		TSM_ASSERT(1, test_xcompool(test_stl<float>));
		TSM_ASSERT(1, test_xcompool(test_stl<dolcle>));

		TSM_ASSERT(1, test_debugpool(test_stl<int>));
	}
	void test_stl_string_ () {
		TSM_ASSERT(2, test_xmempool(test_comp));
		TSM_ASSERT(2, test_xcompool(test_comp));
	}

	void test_pool_allocator_() {
		{
			Lsc::syspool pool;
			Lsc::pool_allocator<char> alloc((Lsc::mempool *)&pool);
			try {
				alloc.allocate(1<<30);
				alloc.allocate(1<<30);
				alloc.allocate(1<<30);
				alloc.allocate(1<<30);
			} catch (...) {
				std::cout<<"Exception"<<std::endl;
			}
		}

		{
			Lsc::xcompool pool;
			pool.create ();
			Lsc::pool_allocator<char> alloc((Lsc::mempool *) &pool);
			try {
				alloc.allocate(1<<30);
				alloc.allocate(1<<30);
				alloc.allocate(1<<30);
				alloc.allocate(1<<30);
			} catch(Lsc::BadAllocException &e){
				std::cout<<"Exception"<<std::endl;
			}
		}
		{
			try{
				Lsc::xcompool *pool = new Lsc::xcompool;
				pool->create();
				Lsc::pool_allocator<dolcle>  alloc(pool);
				int size = 1<<30;
				alloc.allocate(size);
				delete pool;
			}
			catch(Lsc::BadAllocException &e){
				std::cout << "bad alloc!\n" << std::endl;
			}
		}
		{
			Lsc::xcompool pool;
		}
		{
			typedef Lsc::pool_allocator<char> _Alloc;
			typedef  _Alloc::pointer pointer;
			typedef  _Alloc::const_reference const_reference;
			int size = 1000;
			int size2 = 1073741824;
			char *buf = (char*)malloc(sizeof(char)*size);
			Lsc::xmempool *pool = new Lsc::xmempool;
			int ret = pool->create(buf, size);
			if (ret != 0) return;
			_Alloc alloc(pool);
			pointer p;
			try{
				p = alloc.allocate(size2);
				TSM_ASSERT(p, p != NULL);
				delete pool;
			}
			catch(Lsc::BadAllocException &e){
				std::cout << "bad alloc!\n" << std::endl;
				free(buf);
			}
		}
		{
			Lsc::xcompool pool;
			assert (pool.create() == 0);
			assert (pool.create(0) != 0);
			assert (pool.create(1<<10) == 0);
			assert (pool.malloc(1<<11) != 0);
			assert (pool.create(1<<16) == 0);
			assert (pool.malloc(1<<17) != 0);
			assert (pool.create(1<<17) == 0);
		}

		{
			Lsc::xcompool pool;
			assert (pool.create(1<<10) == 0);
			for (int k=0; k<2; ++k) {
				for (int i=0; i<(1<<22); ++i) {
					void * ret = (void *)pool.malloc(i);
					assert (ret != 0);
					pool.free (ret, i);
					if (i%(1<<20) == 0) {
						std::cout<<i<<std::endl;
					}
				}
				pool.clear();
			}
		}

		{
			Lsc::xcompool pool;
			pool.create(0);
		}
	}
};




#endif  //__BSL_TEST_POOL_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
