#include "stdafx.h"
#include "ObjectPool.h"
#include "UnitTest.h"


class ObjPoolTest {
plclic:
	void SetUp() {}
	void TearDown() {}
};



template <typename TObj, class TLock=CNoLock>
class ObjPool : plclic TObjectPool<TObj, TLock>
{
private:
	struct AllocedObj{
		SLINK link;
		TObj* pObj;
	};

	TSingleList<AllocedObj> m_AllocedList;

plclic:
    ObjPool() : TObjectPool<TObj, TLock>("objpool", BUFPOOL_C1) {}
	~ObjPool() {
		FreeAll();
	}

	bool AllocOne() {
		TObj* p = TObjectPool<TObj, TLock>::Allocate();
		if(p){
			AllocedObj* pA = new AllocedObj;
			pA->pObj = p;
			m_AllocedList.push(pA);
			return true;
		}
		else
			return false;
	}

	void FreeOne() {
		AllocedObj* p = m_AllocedList.pop();
		if(p){
			TObjectPool<TObj, TLock>::Free(p->pObj);
			free(p);
		}
	}

	void FreeAll() {
		AllocedObj* p = m_AllocedList.pop();
		while(p) {
			TObjectPool<TObj, TLock>::Free(p->pObj);
			free(p);
			p = m_AllocedList.pop();
		}
	}

	void PrintObjs() {
		TObjectPool<TObj, TLock>::PrintObjs();

		TRACE0("Used  Objects: ");
		AllocedObj*	p = m_AllocedList.top();
		while(p) {
			printf("%p ", MAKE_PTR(ptr, p->pObj, -sizeof(SLINK)));
			p = (AllocedObj*)p->link._next;
		}
		printf("\n-------------------------\n");
	}
};


TEST_F(ObjPoolTest, Case1_test_create)
{
	printf("test allocate 0 object every time TObjectPool::Create() called...\n");

	ObjPool<int> pool;

	
	SHOULD_ABORT(pool.Create(0, 1));
}


TEST_F(ObjPoolTest, Case2_test_create)
{
	printf("test nStart>nMax in TObjectPool::Create()...\n");

	ObjPool<int> pool;
	 
	SHOULD_ABORT(pool.Create(1, 1, 0));


}



TEST_F(ObjPoolTest, Case3_test_create)
{
	printf("test NOT alloc when TObjectPool::Create(), nStart==0...\n");

	ObjPool<int> pool;
	 
	ASSERT(pool.Create(1, 0, 1));
	ASSERT(0 == pool.GetFreeCount());
	pool.PrintObjs();
}



TEST_F(ObjPoolTest, Case4_test_create)
{
	printf("test normal situation TObjectPool::Create()...\n");

	ObjPool<int, CSpinLock> pool;
	
	ASSERT(pool.Create(4, 1, 2));
	pool.PrintObjs();
}


TEST_F(ObjPoolTest, Case5_test_allocate_and_free)
{
	printf("test TObjectPool::Allocate()\\Free()...\n");

	ObjPool<int> pool;
	
	ASSERT(pool.Create(2, 1, 2));

#ifndef  _DEBUG
	printf("before allocate:\n");	
	pool.PrintObjs();
	ASSERT2(2, ==, pool.GetFreeCount());
#endif

	
	ASSERT(pool.AllocOne());
	printf("allocate 1 objs:\n");	
	pool.PrintObjs();
	ASSERT(1 == pool.GetFreeCount());

	
	ASSERT(pool.AllocOne());
	printf("allocate 2 objs:\n");	
	pool.PrintObjs();
	ASSERT(0 == pool.GetFreeCount());

	
	ASSERT(pool.AllocOne());
	printf("allocate 3 objs:\n");	
	pool.PrintObjs();
	ASSERT(1 == pool.GetFreeCount());
		
	
	ASSERT(pool.AllocOne());
	printf("allocate 4 objs:\n");	
	pool.PrintObjs();
	ASSERT(0 == pool.GetFreeCount());

	
	printf("allocate 5 objs, Exceed the MAX!!\n");	
	printf("-------------------------\n");
	ASSERT(false == pool.AllocOne());

	
	printf("free 1 allocated obj:\n");	
	pool.FreeOne();
	pool.PrintObjs();
	ASSERT(1 == pool.GetFreeCount());

	
	printf("free 2 allocated objs:\n");	
	pool.FreeOne();
	pool.PrintObjs();
	ASSERT(2 == pool.GetFreeCount());
		
	
	printf("free 4 allocated objs:\n");	
	pool.FreeAll();
	pool.PrintObjs();
	ASSERT(4 == pool.GetFreeCount());
}


TEST_F(ObjPoolTest, Case6_test_destroy)
{
	printf("test TObjectPool::Destroy()...\n");

	ObjPool<int> pool;
	
	ASSERT(pool.Create(2, 0, 2));
	
	printf("at the beginning:\n");
	pool.PrintObjs();

	ASSERT(pool.AllocOne());
	ASSERT(pool.AllocOne());
	ASSERT(pool.AllocOne());
	ASSERT(pool.AllocOne());

	printf("allocate all:\n");
	pool.PrintObjs();

	
	pool.FreeAll();
	ASSERT(pool.Destroy(false));	
	printf("after destroy:\n");
	pool.PrintObjs();
}





