#include "stdafx.h"
#include "CompQ.h"
#include "UnitTest.h"

#define DECLARE_TEST_CLASS(testclassname) \
class testclassname { \
  plclic: \
    void SetUp() {} \
    void TearDown() {} \
}; 



template<typename pElement, typename pQ>
static bool TEnum(pElement pE, ptr pData)
{
	char c = pE->data;
	pQ q = (pQ)pData;

	if((c>='a' && c<='z')||(c>='A' && c<='Z'))
		q->remove(pE);
	else if(IS_DIGIT(c)){
		q->remove(pE);
		q->push_back(pE);
	}
	else
		return false;

	return true;
}



template<typename pElement>
static bool TPrintEnum(pElement pE, ptr pData)
{
	ASSERT(NULL != pE);
	printf("node %c\n", pE->data);
	return true;
}



template<typename itor, typename pElement>
void f_merg(itor it, pElement pNewElement)
{
	ASSERT(it->first == pNewElement->key);
	pElement pOld = it->second;
	it->second = pNewElement;

		DLINK_INITIALIZE(&pOld->link);
}

 


struct IdxQElement
{
	char data;
	DLINK link;
};

typedef TIndexedQueue<IdxQElement> IdxQ;

static inline IdxQElement* NewIdxQEle(char data)
{
	IdxQElement* p = new IdxQElement;
	p->data = data;
	DLINK_INITIALIZE(&p->link);
	return p;
}

static inline void DestroyIdxQ(IdxQ* idxQ)
{
	IdxQElement* p = NULL;

	while(NULL!=(p=idxQ->pop_front()))
		delete p;
}



DECLARE_TEST_CLASS(IdxQTest) 


TEST_F(IdxQTest, Case1_test_initialization)
{
	printf("test TIndexedQueue initialization...\n");
	IdxQ idxQ;
	IdxQElement* p1 = NewIdxQEle('a');

	ASSERT(idxQ.size() == 0);
	ASSERT(idxQ.remove(p1) == false);
	ASSERT(idxQ.contains(p1) == false);
	ASSERT(idxQ.get_front() == NULL);
	ASSERT(idxQ.pop_front() == NULL);
	ASSERT(idxQ.front_to_back() == NULL);

	delete p1;
}



TEST_F(IdxQTest, Case2_test_push)
{
	printf("test TIndexedQueue::push_back()\\push_front()...\n");

	IdxQ idxQ;
	IdxQElement* p1 = NewIdxQEle('a');
	IdxQElement* p2 = NewIdxQEle('b');
	IdxQElement* p3 = NewIdxQEle('c');
	
	
	SHOULD_ABORT(idxQ.push_back(NULL));
	SHOULD_ABORT(idxQ.push_front(NULL));

	ASSERT(true == idxQ.push_back(p1));
	ASSERT(idxQ.size() == 1);
	ASSERT(idxQ.contains(p1) == true);
	ASSERT(p1 == idxQ.get_front());
	
	
    SHOULD_ABORT(idxQ.push_back(p1));

	
	ASSERT(idxQ.push_front(p2));

	ASSERT(idxQ.size() == 2);
	ASSERT(true == idxQ.contains(p1));
	ASSERT(true == idxQ.contains(p2));

	
	SHOULD_ABORT(idxQ.push_front(p2));

	ASSERT(p2 == idxQ.get_front());

	
	ASSERT(idxQ.push_front(p3));
	ASSERT(p3 == idxQ.get_front());

	DestroyIdxQ(&idxQ);
}




class f_IdxQComp
{
plclic:
	bool operator() (IdxQElement* x, IdxQElement* y){
		return x->data < y->data;
	}
};


TEST_F(IdxQTest, Case3_test_push)
{
	printf("test collisions when push_back()\\push_front()...\n");
	
	TIndexedQueue<IdxQElement, f_IdxQComp> idxQ;

	IdxQElement* p1 = NewIdxQEle('a');
	IdxQElement* p2 = NewIdxQEle('b');
	IdxQElement* p3 = NewIdxQEle('a');
	IdxQElement* p4 = NewIdxQEle('b');

	IdxQElement* p = NULL;
	
	
	ASSERT(idxQ.push_back(p1, &p));
	ASSERT(NULL == p);
	
	
	ASSERT(false == idxQ.push_back(p3, &p));
	ASSERT(p == p1);
	
	
	ASSERT(idxQ.push_front(p2, &p));

	
	ASSERT(false == idxQ.push_front(p4, &p));
	ASSERT(p == p2);

	ASSERT(2 == idxQ.size());

	ASSERT(p2 == idxQ.pop_front());
	ASSERT(p1 == idxQ.pop_front());
	ASSERT(0 == idxQ.size());
	
	delete p1;
	delete p2;
	delete p3;
	delete p4;
}


TEST_F(IdxQTest, Case4_test_front_to_back)
{
	printf("test TIndexedQueue::front_to_back()...\n");

	IdxQ idxQ;

	
	ASSERT(NULL == idxQ.front_to_back());	

	IdxQElement* p1 = NewIdxQEle('a');
	idxQ.push_back(p1);
	
	
	ASSERT(p1 == idxQ.front_to_back());

	
	IdxQElement* p2 = NewIdxQEle('b');
	idxQ.push_back(p2);

	ASSERT(p1 == idxQ.front_to_back());
	ASSERT(p2 == idxQ.front_to_back());
	ASSERT(2 == idxQ.size());

	DestroyIdxQ(&idxQ);
}



TEST_F(IdxQTest, Case5_test_pop_front)
{
	printf("test TIndexedQueue::pop_front()\\get_front()\\contains()...\n");

	IdxQ idxQ;
	
	
	ASSERT(NULL == idxQ.pop_front());	
	ASSERT(NULL == idxQ.get_front());	
	ASSERT(false == idxQ.contains(NULL));	

	
	IdxQElement* p1 = NewIdxQEle('a');
	idxQ.push_back(p1);

	ASSERT(p1 == idxQ.get_front());	
	ASSERT(true == idxQ.contains(p1));	
	
	IdxQElement* p2 = NewIdxQEle('b');
	ASSERT(false == idxQ.contains(p2));

	idxQ.push_front(p2);

	ASSERT(p2 == idxQ.get_front());	
	ASSERT(true == idxQ.contains(p1));	
	ASSERT(true == idxQ.contains(p2));	
	
	ASSERT(p2 == idxQ.pop_front());	
	ASSERT(p1 == idxQ.pop_front());	
	
	
	ASSERT(DLINK_IS_STANDALONE(&p1->link));
	ASSERT(DLINK_IS_STANDALONE(&p2->link));

	ASSERT(NULL == idxQ.pop_front());	
	ASSERT(false == idxQ.contains(p1));	
	ASSERT(false == idxQ.contains(p2));	

	delete p1;
	delete p2;
}


TEST_F(IdxQTest, Case6_test_remove)
{
	printf("test TIndexedQueue::remove()...\n");

	IdxQ idxQ;
	IdxQElement* p1 = NewIdxQEle('a');
	IdxQElement* p2 = NewIdxQEle('b');
	IdxQElement* p3 = NewIdxQEle('c');
	
	
    SHOULD_ABORT(idxQ.remove(NULL));
	ASSERT(false == idxQ.remove(p1));	

	idxQ.push_back(p1);
	idxQ.push_back(p2);
	idxQ.push_back(p3);
	
	
	ASSERT(3 == idxQ.size());

	
	ASSERT(true == idxQ.remove(p2));	
	ASSERT(false == idxQ.contains(p2));	
	ASSERT(2 == idxQ.size());
	

	
	ASSERT(true == idxQ.remove(p1));	
	ASSERT(false == idxQ.contains(p1));	

	ASSERT(true == idxQ.remove(p3));	
	ASSERT(0 == idxQ.size());

	
	ASSERT(DLINK_IS_STANDALONE(&p1->link));	
	ASSERT(DLINK_IS_STANDALONE(&p2->link));	
	ASSERT(DLINK_IS_STANDALONE(&p3->link));	

	delete p1;
	delete p2;
	delete p3;
}



TEST_F(IdxQTest, Case7_test_enumerate)
{
	printf("test TIndexedQueue::Enumerate()...\n");

	typedef TIndexedQueue<IdxQElement>* P_IdxQ;

	IdxQ idxQ;
	
	IdxQElement* p[9];
	char C[9] = {'a', 'b', 'c', 
		         '1', '2', '3',
	for(int i = 0; i < 9; ++i)
		p[i] = NewIdxQEle(C[i]);

	
	ASSERT(true == idxQ.Enumerate(TEnum<IdxQElement*, P_IdxQ>, (ptr)&idxQ));	
	
	for(int i = 0; i < 9; ++i)
		idxQ.push_back(p[i]);
	
	
	printf("before enumerate:\n");
	ASSERT(true == idxQ.Enumerate(TPrintEnum<IdxQElement*>, NULL));

	printf("after enumerate:\n");
	ASSERT(false == idxQ.Enumerate(TEnum<IdxQElement*, P_IdxQ>, (ptr)&idxQ));	
	ASSERT(true == idxQ.Enumerate(TPrintEnum<IdxQElement*>, NULL));

	ASSERT(DLINK_IS_STANDALONE(&p[0]->link));
	ASSERT(DLINK_IS_STANDALONE(&p[1]->link));
	ASSERT(DLINK_IS_STANDALONE(&p[2]->link));

	IdxQElement* pTmp = NULL;
	while(NULL != (pTmp=idxQ.pop_front()));

	for(int i = 0; i<9; ++i)
		delete p[i];
}

 


struct KeyedQElement
{
	char data;
	int key;
	DLINK link;
};

typedef TKeyedQueue<int, KeyedQElement> KeyedQ;

static inline KeyedQElement* NewKeyedQEle(char data, int key)
{
	KeyedQElement* p = new KeyedQElement;
	p->data = data;
	p->key = key;
	DLINK_INITIALIZE(&p->link);
	return p;
}

static inline void DestroyKeyedQ(KeyedQ* kdQ)
{
	KeyedQElement* p = NULL;

	while(NULL!=(p=kdQ->pop_front()))
		delete p;
}



DECLARE_TEST_CLASS(KeyedQTest) 


TEST_F(KeyedQTest, Case1_test_initialization)
{
	printf("test TKeyedQueue initialization...\n");

	KeyedQ kdQ;

	ASSERT(NULL == kdQ.pop_front());
	ASSERT(NULL == kdQ.front_to_back());
	ASSERT(NULL == kdQ.get_front());
	ASSERT(0 == kdQ.size());
}


TEST_F(KeyedQTest, Case2_test_push_back)
{
	printf("test TKeyedQueue push_back()...\n");

	KeyedQ kdQ;

	
	SHOULD_ABORT(kdQ.push_back(NULL));

	KeyedQElement* pa = NewKeyedQEle('a',(int)'a');
	KeyedQElement* pb = NewKeyedQEle('b',(int)'b');
	KeyedQElement* pA = NewKeyedQEle('A',(int)'a');
	KeyedQElement* pOld = NULL;


	
	ASSERT(true == kdQ.push_back(pa));	
	
	ASSERT(1 == kdQ.size());

	
	SHOULD_ABORT(kdQ.push_back(pa));

	
	ASSERT(true == kdQ.push_back(pb));	
	ASSERT(2 == kdQ.size());

	
	ASSERT(false == kdQ.push_back(pA, &pOld));	
	ASSERT(pOld == pa);	
	ASSERT(2 == kdQ.size());
	ASSERT(DLINK_IS_STANDALONE(&pA->link));

	DestroyKeyedQ(&kdQ);	
	delete pA;
}



TEST_F(KeyedQTest, Case3_test_push_back_and_replace)
{
	printf("test TKeyedQueue push_back_and_replace()...\n");

	KeyedQ kdQ;

	
	SHOULD_ABORT(kdQ.push_back_and_replace(NULL));

	KeyedQElement* pa = NewKeyedQEle('a',(int)'a');
	KeyedQElement* pb = NewKeyedQEle('b',(int)'b');
	KeyedQElement* pA = NewKeyedQEle('A',(int)'a');

	
	ASSERT(NULL == kdQ.push_back_and_replace(pa));
	ASSERT(NULL == kdQ.push_back_and_replace(pb));

	
	SHOULD_ABORT(kdQ.push_back_and_replace(pa));
		
	
	ASSERT(pa == kdQ.find(pa->key));
	ASSERT(pa == kdQ.push_back_and_replace(pA));
	ASSERT(DLINK_IS_STANDALONE(&pa->link));
	ASSERT(pA == kdQ.find(pa->key));

	DestroyKeyedQ(&kdQ);	
	delete pa;
}





TEST_F(KeyedQTest, Case4_test_push_back_and_replace)
{
	printf("test TKeyedQueue pop_front()\\front_to_back()\\get_front()\\find()...\n");

	KeyedQ kdQ;

	KeyedQElement* pa = NewKeyedQEle('a',(int)'a');
	KeyedQElement* pb = NewKeyedQEle('b',(int)'b');

	
	ASSERT(NULL == kdQ.pop_front());	
	ASSERT(NULL == kdQ.get_front());	
	ASSERT(NULL == kdQ.front_to_back());	
	ASSERT(NULL == kdQ.find(pa->key));	

	kdQ.push_back(pa);
	
	ASSERT(pa == kdQ.front_to_back());

	kdQ.push_back(pb);

	
	ASSERT(pa == kdQ.get_front());
	ASSERT(pa == kdQ.front_to_back());
	ASSERT(pb == kdQ.get_front());
	
	
	ASSERT(pa == kdQ.find(pa->key));
	ASSERT(pb == kdQ.find(pb->key));

	ASSERT(pb == kdQ.pop_front());
	ASSERT(DLINK_IS_STANDALONE(&pb->link));
	ASSERT(NULL == kdQ.find(pb->key));

	ASSERT(pa == kdQ.pop_front());
	ASSERT(0 == kdQ.size());

	delete pa;
	delete pb;
}




TEST_F(KeyedQTest, Case5_test_remove)
{
	printf("test TKeyedQueue remove()...\n");

	KeyedQ kdQ;

	KeyedQElement* pa = NewKeyedQEle('a',(int)'a');
	KeyedQElement* pb = NewKeyedQEle('b',(int)'b');

	
	ASSERT(NULL == kdQ.remove(pa->key));
	SHOULD_ABORT(kdQ.remove((KeyedQElement*)NULL));

	kdQ.push_back(pa);
	kdQ.push_back(pb);
	
	ASSERT(pa == kdQ.remove(pa->key));
	ASSERT(DLINK_IS_STANDALONE(&pa->link));
	
	ASSERT(true == kdQ.remove(pb));
	ASSERT(DLINK_IS_STANDALONE(&pb->link));

	ASSERT(false == kdQ.remove(pb));
	ASSERT(false == kdQ.remove(pa));

	ASSERT(0 == kdQ.size());

	delete pa;
	delete pb;
}



TEST_F(KeyedQTest, Case6_test_enumerate)
{
	printf("test TKeyedQueue::Enumerate()...\n");

	KeyedQ kdQ;

	KeyedQElement* p[9];
	char C[9] = {'a', 'b', 'c', 
		         '1', '2', '3',
	for(int i = 0; i < 9; ++i)
		p[i] = NewKeyedQEle(C[i],(int)C[i]);

	
	ASSERT(true == kdQ.Enumerate(TEnum<KeyedQElement*, KeyedQ*>, (ptr)&kdQ));	
	
	for(int i = 0; i < 9; ++i)
		kdQ.push_back(p[i]);
	
	
	printf("before enumerate:\n");
	kdQ.Enumerate(TPrintEnum<KeyedQElement*>, NULL);	

	ASSERT(false == kdQ.Enumerate(TEnum<KeyedQElement*, KeyedQ*>, (ptr)&kdQ));	

	printf("after enumerate:\n");
	kdQ.Enumerate(TPrintEnum<KeyedQElement*>, NULL);	

	KeyedQElement* pTmp = NULL;
	while(NULL != (pTmp = kdQ.pop_front()));

	for(int i = 0; i < 9; ++i)
		delete p[i];
}


 


struct KdMQElement
{
	char data;
	int key;
	DLINK link;
	KdMQElement* pNext;
};

typedef TKeyedMultiQueue<int, KdMQElement> KdMQ;

#if 0
static inline KdMQElement* NewKdMQEle(char data, int key)
{
	KdMQElement* p = new KdMQElement;
	p->data = data;
	p->key = key;
	p->pNext = NULL;
	DLINK_INITIALIZE(&p->link);
	return p;
}
#endif


static bool Enum1(KdMQElement* pE, ptr pData)
{
	printf("Key %d: %c ", pE->key, pE->data);

	KdMQElement* pTmp = pE->pNext;

	while(pTmp){
		printf("%c ", pTmp->data);
		pTmp = pTmp->pNext;
	}

	printf("\n");
		
	return true;
}


const static int KmqDataSize = 9;
static KdMQElement kmqdata[KmqDataSize] = 
{{'a', 1}, {'b', 1}, {'c', 1},
 {'1', 2}, {'2', 2}, {'3', 2},

static inline void init_dataset()
{
	for(int i = 0; i < KmqDataSize; ++i){
		kmqdata[i].pNext = NULL;
		DLINK_INITIALIZE(&kmqdata[i].link);
	}
}

#define PUSH_BACK_ALL \
	for(int i = 0; i < KmqDataSize; ++i){ \
		kmQ.push_back(&kmqdata[i]); \
	}

#define PRINT_KMQ \
	printf("----------------\n");\
	kmQ.Enumerate(Enum1, NULL);


DECLARE_TEST_CLASS(KeyedMQTest) 


TEST_F(KeyedMQTest, Case1_test_push_back)
{
	printf("test TKeyedMultiQueue::push_back()...\n");
	
	KdMQ kmQ;
	init_dataset();	
	KdMQElement* p = kmqdata;

	
	SHOULD_ABORT(kmQ.push_back(NULL));
	
	
	kmQ.push_back(&p[0]);
	kmQ.push_back(&p[3]);
	kmQ.push_back(&p[6]);

	
	SHOULD_ABORT(kmQ.push_back(&p[0]));
	
	ASSERT(3 == kmQ.size());
	kmQ.Enumerate(Enum1, NULL);
	
	
	kmQ.push_back(&p[1]);
	kmQ.push_back(&p[4]);
	kmQ.push_back(&p[7]);

	ASSERT(3 == kmQ.size());
	ASSERT(&p[0] == kmQ.find(p[0].key));
	ASSERT(&p[3] == kmQ.find(p[3].key));
	
	PRINT_KMQ

	kmQ.push_back(&p[2]);
	kmQ.push_back(&p[5]);
	kmQ.push_back(&p[8]);
	PRINT_KMQ

	
	SHOULD_ABORT(kmQ.push_back(&p[1]));



	
}



TEST_F(KeyedMQTest, Case2_test_push_back_and_supercede)
{
	printf("test TKeyedMultiQueue::push_back_and_supercede()...\n");
	
	KdMQ kmQ;
	init_dataset();	
	KdMQElement* p = kmqdata;
	
	
	SHOULD_ABORT(kmQ.push_back_and_supercede(NULL));
	
	
	kmQ.push_back_and_supercede(&p[0]);
	kmQ.push_back_and_supercede(&p[3]);
	kmQ.push_back_and_supercede(&p[6]);
	
	kmQ.Enumerate(Enum1, NULL);

	ASSERT(3 == kmQ.size());
	
	kmQ.push_back_and_supercede(&p[1]);
	kmQ.push_back_and_supercede(&p[4]);
	kmQ.push_back_and_supercede(&p[7]);
	
	PRINT_KMQ

	kmQ.push_back_and_supercede(&p[2]);
	kmQ.push_back_and_supercede(&p[5]);
	kmQ.push_back_and_supercede(&p[8]);
	PRINT_KMQ

	
	SHOULD_ABORT(kmQ.push_back_and_supercede(&p[0]));



}



TEST_F(KeyedMQTest, Case3_test_remove)
{
	printf("test TKeyedMultiQueue remove(pElement*)...\n");
	
	KdMQ kmQ;
	init_dataset();	
	KdMQElement* p = kmqdata;
	
	
	SHOULD_ABORT(kmQ.remove((KdMQElement*)NULL));
	ASSERT(false == kmQ.remove(&p[0]));	

	
	PUSH_BACK_ALL	
	printf("before remove:\n");
	kmQ.Enumerate(Enum1, NULL);

	
	ASSERT(true == kmQ.remove(&p[0]));
	ASSERT(DLINK_IS_STANDALONE(&p[0].link));
	ASSERT(NULL == p[0].pNext);

	printf("----------------\n");
	printf("after remove('%c'):\n", p[0].data);
	kmQ.Enumerate(Enum1, NULL);

	
	ASSERT(true == kmQ.remove(&p[5]));
	ASSERT(DLINK_IS_STANDALONE(&p[5].link));
	ASSERT(NULL == p[5].pNext);
	ASSERT(p[3].pNext == &p[4]);
	printf("----------------\n");
	printf("after remove('%c'):\n",p[5].data);
	kmQ.Enumerate(Enum1, NULL);

	
	ASSERT(true == kmQ.remove(&p[1]));
	ASSERT(DLINK_IS_STANDALONE(&p[1].link));
	ASSERT(NULL == p[1].pNext);
	printf("----------------\n");
	printf("after remove('%c'):\n",p[1].data);
	kmQ.Enumerate(Enum1, NULL);

	
	ASSERT(false == kmQ.remove(&p[1]));
}



TEST_F(KeyedMQTest, Case4_test_remove)
{
	printf("test TKeyedMultiQueue remove(key)...\n");
	
	KdMQ kmQ;
	init_dataset();	
	KdMQElement* p = kmqdata;

	
	ASSERT(NULL == kmQ.remove(1));	

	
	PUSH_BACK_ALL	
	printf("before remove:\n");
	kmQ.Enumerate(Enum1, NULL);

	
	ASSERT(&p[6] == kmQ.remove(3));
	printf("----------------\n");
	printf("after remove list(key=3):\n");
	kmQ.Enumerate(Enum1, NULL);

	
	ASSERT(NULL == kmQ.remove(3));

	
	ASSERT(NULL == kmQ.remove(4));
}



TEST_F(KeyedMQTest, Case4_test_inherited_funcs)
{
	printf("\x1b[37mTODO:test TKeyedMultiQueue functions inherited from TKeyedQueue\x1b[0m\n");
	
}

#undef PUSH_BACK_ALL 
#undef PRINT_KMQ


 


typedef struct MMElement
{
	char data;
	int key;
	DLINK link;
};



const static int MMDataSize = 9;
static MMElement mmdata[MMDataSize] = 
{{'a', 1, }, {'b', 1, }, {'c', 1, },
 {'1', 2, }, {'2', 2, }, {'3', 2, },
};

static inline void init_mmdata()
{
	for(int i = 0; i < MMDataSize; ++i)
		DLINK_INITIALIZE(&mmdata[i].link);
}



class TMultiMapTest
{
plclic:
	void SetUp() {}
	void TearDown() {}
};

typedef TMultiMap<int, MMElement> MMap;
typedef MMap::Iterator MMIterator;

static void PrintMap(MMap& mmap)
{
	MMIterator it = mmap.begin();
	while(it != mmap.end()){
		printf("Key %d: %c ", it->first, it->second->data);
		DLINK* head = &it->second->link;
		DLINK* tmp = head->_next;
		MMElement *p = NULL;
		
		while(tmp != head){
			p = CONTAINING_RECORD(tmp, MMElement, link);
			printf("%c ", p->data);
			tmp = tmp->_next;
		}

		printf("\n");
		++it;
	}
	printf("----------------\n");
}


TEST_F(TMultiMapTest, Case1_test_initialization)
{
	MMap mmap;
	init_mmdata();

	printf("test TMultiMap initialization...\n");
	ASSERT(mmap.begin() == mmap.end());
	ASSERT(mmap.find(1) == NULL);
	ASSERT(mmap.remove(1, &mmdata[0]) == false);
	SHOULD_ABORT(mmap.remove(1, NULL));
}


TEST_F(TMultiMapTest, Case2_test_insert)
{
	printf("test TMultiMap::insert()...\n");
	MMap mmap;
	init_mmdata();

	
	SHOULD_ABORT(mmap.insert(0, NULL));

	
#define INSERT_ALL \
	for(int i = 0; i < MMDataSize; ++i) \
		mmap.insert(mmdata[i].key, &mmdata[i]); 
	INSERT_ALL
	PrintMap(mmap);

	
	SHOULD_ABORT(mmap.insert(mmdata[0].key, &mmdata[0]));
}



TEST_F(TMultiMapTest, Case3_test_find)
{
	printf("test TMultiMap::find()...\n");
	MMap mmap;
	init_mmdata();

	
	ASSERT(NULL == mmap.find(1));

	INSERT_ALL

	ASSERT(&mmdata[0] == mmap.find(1));
	ASSERT(&mmdata[3] == mmap.find(2));
	ASSERT(&mmdata[6] == mmap.find(3));
	ASSERT(NULL == mmap.find(4));
}



TEST_F(TMultiMapTest, Case4_test_remove)
{
	printf("test TMultiMap::remove(key)...\n");
	MMap mmap;
	init_mmdata();

	
	ASSERT(NULL == mmap.remove(1));

	INSERT_ALL

	printf("before remove(key = 1):\n");
	PrintMap(mmap);
	ASSERT(&mmdata[0] == mmap.remove(1));
	printf("after remove(key = 1):\n");
	PrintMap(mmap);

	
	ASSERT(NULL == mmap.remove(1));

	
	ASSERT(&mmdata[3] == mmap.remove(2));
	ASSERT(&mmdata[6] == mmap.remove(3));

	ASSERT(mmap.begin() == mmap.end());
	
}



TEST_F(TMultiMapTest, Case5_test_remove)
{
	printf("test TMultiMap::remove(key, TElement*)...\n");
	MMap mmap;
	init_mmdata();

	
	SHOULD_ABORT(mmap.remove(1, NULL));

	
	ASSERT(false == mmap.remove(1, &mmdata[0]));

	INSERT_ALL
	
	printf("before remove(1, '%c'):\n", mmdata[0].data);
	PrintMap(mmap);
	ASSERT(true == mmap.remove(1, &mmdata[0]));
	printf("after remove(1, '%c'):\n", mmdata[0].data);
	PrintMap(mmap);
	ASSERT(true == DLINK_IS_STANDALONE(&mmdata[0].link));

	
	SHOULD_ABORT(mmap.remove(1, &mmdata[0]));

	ASSERT(true == mmap.remove(1, &mmdata[1]));
	ASSERT(true == mmap.remove(1, &mmdata[2]));
	printf("continue remove(1, '%c'), (1, '%c'):\n", mmdata[1].data, mmdata[2].data);
	PrintMap(mmap);
}

#undef INSERT_ALL


