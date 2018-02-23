#include <cxxtest/TestSuite.h>
#include <Lsc/pool/Lsc_cachedpool.h>

class Lsc_test_pool : plclic CxxTest::TestSuite
{
plclic:
	void test_normal(void) {   
		Lsc::cached_mempool o;
		o.free(0, 0);
	}  
	void test_normal1(void) {   
		Lsc::mempool *p = new Lsc::cached_mempool();
		p->free(0, 0);
	}  
};
