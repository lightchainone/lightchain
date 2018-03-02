#include "stdafx.h"
#include "DolcleList.h"
#include "UnitTest.h"

#define DECLARE_TEST_CLASS(testclassname) \
class testclassname { \
  plclic: \
    void SetUp() {} \
    void TearDown() {} \
}; 

class DLTest {
plclic:
    void SetUp() {}
    void TearDown() {}
};


struct DLNode{
    int i;
    DLINK link;
};

typedef TLinkedList<DLNode> LList;

static DLNode* NewDLNode(int i)
{
    DLNode* p = new DLNode;
    p->i = i;
    DLINK_INITIALIZE(&p->link);
    return p;
}

static void DestroyDList(LList* pl)
{
	DLNode* p = NULL;
	while(NULL != (p=pl->pop_front())){
		delete p;
	}
}



static bool Enum1(DLNode* pNode, ptr)
{
	printf("node %d\n", pNode->i);
	return true;
}

 


TEST_F(DLTest, Case1_test_initialization)
{

	TLinkedList<DLNode> l;

	ASSERT(l.size() == 0);		
	ASSERT(l.get_back() == NULL);		
	ASSERT(l.get_front() == NULL);		
	ASSERT(l.pop_front() == NULL);		
	ASSERT(l.pop_back() == NULL);		

	printf("test Print()...\n");
	l.Print();
	
	l.Assert();
	
	SHOULD_ABORT(l.contains(NULL));

	SHOULD_ABORT(l.remove(NULL););

	ASSERT(l.Enumerate(Enum1, NULL));

	ASSERT(l.Eat(Enum1, NULL));
}


TEST_F(DLTest, Case2_test_push_back)
{
	printf("test TLinkedList::push_back()...\n");

	TLinkedList<DLNode> l;

	
	SHOULD_ABORT(l.push_back(NULL));

	
	DLNode* p1 = NewDLNode(1);
	l.push_back(p1);

	ASSERT(1 == l.size());
	ASSERT(l.contains(p1));

	
	SHOULD_ABORT(l.push_back(p1));
	
	
	DLNode* p2 = NewDLNode(2);
	l.push_back(p2);

	ASSERT(2 == l.size());
	ASSERT(l.contains(p2));

	ASSERT(p1 == l.get_front());
	ASSERT(p2 == l.get_back());

	DestroyDList(&l);
}



TEST_F(DLTest, Case3_test_push_front)
{
	printf("test TLinkedList::push_front()...\n");

	TLinkedList<DLNode> l;

	
	SHOULD_ABORT(l.push_front(NULL));

	
	DLNode* p1 = NewDLNode(1);
	l.push_front(p1);

	ASSERT(1 == l.size());
	ASSERT(l.contains(p1));

	
	SHOULD_ABORT(l.push_front(p1));
	
	
	DLNode* p2 = NewDLNode(2);
	l.push_front(p2);

	ASSERT(2 == l.size());
	ASSERT(l.contains(p2));

	ASSERT(p2 == l.get_front());
	ASSERT(p1 == l.get_back());

	DestroyDList(&l);
}



TEST_F(DLTest, Case4_test_assert)
{
	printf("test TLinkedList::Assert()...\n");

	TLinkedList<DLNode> l;
	l.Assert();

	
	DLNode* p1 = NewDLNode(1);
	l.push_front(p1);
	ASSERT(l.contains(p1));

	l.Assert();
	l.Print();

	DestroyDList(&l);
}



TEST_F(DLTest, Case5_test_contains)
{
	printf("test TLinkedList::contains()...\n");

	TLinkedList<DLNode> l;
	SHOULD_ABORT(l.contains(NULL));	

	
	DLNode* p1 = NewDLNode(1);
	l.push_front(p1);

	ASSERT(1 == l.size());
	ASSERT(l.contains(p1));

	DestroyDList(&l);
}



TEST_F(DLTest, Case6_test_merge)
{
	printf("test TLinkedList::merge()...\n");

	TLinkedList<DLNode> l1, l2;

	DLNode* p1 = NewDLNode(1);
	DLNode* p2 = NewDLNode(2);

	l1.push_back(p1);

	
	l1.merge(&l2);
	ASSERT(1 == l1.size());
	ASSERT(0 == l2.size());

	
	l2.merge(&l1);
	ASSERT(0 == l1.size());
	ASSERT(1 == l2.size());
	ASSERT(l2.contains(p1));

	
	l1.push_front(p2);	
	l1.merge(&l2);
	ASSERT(2 == l1.size());
	ASSERT(0 == l2.size());
	ASSERT(l1.contains(p1));
	ASSERT(l1.contains(p2));
	printf("Nodes in l1:\n");
	l1.Enumerate(Enum1, NULL);
	printf("Nodes in l2:\n");
	l2.Enumerate(Enum1, NULL);

	DestroyDList(&l1);
	DestroyDList(&l2);
}


TEST_F(DLTest, Case7_test_pop_back)
{
	printf("test TLinkedList::pop_back()...\n");

	TLinkedList<DLNode> l;
	
	ASSERT(NULL == l.pop_back());

	DLNode* p1 = NewDLNode(1);
	DLNode* p2 = NewDLNode(2);
	
	l.push_back(p1);
	l.push_back(p2);
	
	
	ASSERT(p2 == l.pop_back());
	ASSERT(DLINK_IS_STANDALONE(&p2->link));
	ASSERT(p1 == l.pop_back());
	ASSERT(DLINK_IS_STANDALONE(&p1->link));
	ASSERT(NULL == l.pop_back());

	DestroyDList(&l);
}	



TEST_F(DLTest, Case8_test_get)
{
	printf("test TLinkedList::get_back()\\get_front()...\n");

	TLinkedList<DLNode> l;

	ASSERT(NULL == l.get_front());
	ASSERT(NULL == l.get_back());

	DLNode* p1 = NewDLNode(1);
	DLNode* p2 = NewDLNode(2);
	
	l.push_back(p1);
	ASSERT(p1 == l.get_front());
	ASSERT(p1 == l.get_back());

	l.push_back(p2);
	
	ASSERT(p1 == l.get_front());
	ASSERT(p2 == l.get_back());

	DestroyDList(&l);
}



TEST_F(DLTest, Case9_test_pop_front)
{
	printf("test TLinkedList::pop_front()...\n");

	TLinkedList<DLNode> l;

	ASSERT(NULL == l.pop_front());

	DLNode* p1 = NewDLNode(1);
	DLNode* p2 = NewDLNode(2);
	
	l.push_back(p1);
	l.push_back(p2);
	
	
	ASSERT(p1 == l.pop_front());
	ASSERT(DLINK_IS_STANDALONE(&p1->link));
	ASSERT(p2 == l.pop_front());
	ASSERT(DLINK_IS_STANDALONE(&p2->link));
	ASSERT(NULL == l.pop_front());

	DestroyDList(&l);
}
	

TEST_F(DLTest, Case10_test_get_next_prev)
{
	printf("test TLinkedList::get_next()\\get_prev()...\n");

	TLinkedList<DLNode> l;
	
	
	SHOULD_ABORT(l.get_next(NULL));
	SHOULD_ABORT(l.get_prev(NULL));

	DLNode* p1 = NewDLNode(1);
	DLNode* p2 = NewDLNode(2);

	
	SHOULD_ABORT(l.get_next(p1));
	SHOULD_ABORT(l.get_prev(p2));

	l.push_back(p1);
	
	ASSERT(NULL == l.get_next(p1));
	ASSERT(NULL == l.get_prev(p1));

	l.push_back(p2);
	
	ASSERT(p2 == l.get_next(p1));	
	ASSERT(NULL == l.get_prev(p1));	
	ASSERT(NULL == l.get_next(p2));	
	ASSERT(p1 == l.get_prev(p2));	

	DestroyDList(&l);
}



TEST_F(DLTest, Case11_test_remove)
{
	printf("test TLinkedList::remove()...\n");

	TLinkedList<DLNode> l;

	
	SHOULD_ABORT(l.remove(NULL));

	DLNode* p1 = NewDLNode(1);
	DLNode* p2 = NewDLNode(2);

	
	SHOULD_ABORT(l.remove(p1));

	l.push_back(p1);
	l.push_back(p2);
	
	
	ASSERT(true == l.contains(p1));
	ASSERT(true == l.contains(p2));

	l.remove(p1);
	ASSERT(DLINK_IS_STANDALONE(&p1->link));
	ASSERT(false == l.contains(p1));
	ASSERT(true == l.contains(p2));

	l.remove(p2);
	ASSERT(DLINK_IS_STANDALONE(&p2->link));
	ASSERT(false == l.contains(p2));

	delete p1;
	delete p2;
}

typedef TLinkedList<DLNode>* p_LL;


bool Enum2(DLNode* pNode, ptr pData)
{
	p_LL pl = (p_LL)pData;
	ASSERT(NULL != pNode);
	if(0 == pNode->i%2){
		pl->remove(pNode);
		delete pNode;
		return true;
	}
	else if(0 == pNode->i%3){
		pNode->i = pNode->i *2 +1;
		return false;
	}
	else
		return true;
}



TEST_F(DLTest, Case12_test_enumerate)
{
	printf("test TLinkedList::Enumerate()...\n");

	TLinkedList<DLNode> l;
	
	ASSERT(true == l.Enumerate(Enum2, (ptr)&l));
	
	DLNode* p[8];
	for(int i = 7; i > 0; --i){
		p[i] = NewDLNode(i);
		l.push_back(p[i]);
	}

	ASSERT(7 == l.size());
	
	
	ASSERT(false == l.Enumerate(Enum2, (ptr)&l));
	
	
	printf("Enumerate using Enum2:\n");
	l.Enumerate(Enum1, NULL);

	
	ASSERT(true == l.Enumerate(Enum2, (ptr)&l));
	
	
	printf("Enumerate using Enum2 again:\n");
	l.Enumerate(Enum1, NULL);

	DestroyDList(&l);
}


bool Enum3(DLNode* pNode, ptr pData)
{
	ASSERT(DLINK_IS_STANDALONE(&pNode->link));
	p_LL pl = (p_LL)pData;
	if(pNode->i%2 == 0){
		pNode->i++;
		pl->push_back(pNode);
		return true;
	}
	else if(pNode->i%3 == 0){
		pNode->i = pNode->i*2 + 1;
		return false;
	}
	else{
		delete pNode;
		return true;
	}
}



TEST_F(DLTest, Case13_test_eat)
{
	printf("test TLinkedList::Eat()...\n");

	TLinkedList<DLNode> l;
	
	ASSERT(true == l.Eat(Enum3, (ptr)&l));
	
	DLNode* p[8];
	for(int i = 7; i > 0; --i){
		p[i] = NewDLNode(i);
		l.push_back(p[i]);
	}
	

	
	ASSERT(false == l.Eat(Enum3, (ptr)&l));
	
	printf("after first eat:\n");
	l.Enumerate(Enum1, NULL);

	
	ASSERT(false == l.Eat(Enum3, (ptr)&l));
	
	
	printf("after eat again:\n");
	l.Enumerate(Enum1, NULL);
	
	
	ASSERT(true == l.Eat(Enum3, (ptr)&l));
	ASSERT(0 == l.size());

	DestroyDList(&l);
}


bool Enum4(DLNode* pNode, ptr pData)
{
	ASSERT(NULL != pNode);
	int N = *(int*)pData;
	if(pNode->i%N == 0)
		return true;
	return false;
}


TEST_F(DLTest, Case14_test_findif)
{
	printf("test TLinkedList::FindIf()...\n");

	int N = 10;

	TLinkedList<DLNode> l;
	
	ASSERT(NULL == l.FindIf(Enum4, (ptr)&N));
	
	
	DLNode* p[8];
	for(int i = 7; i > 0; --i){
		p[i] = NewDLNode(i);
		l.push_back(p[i]);
	}

	DLNode* p1 = NULL;
	N = 3;
	p1 = l.FindIf(Enum4, (ptr)&N);	
	ASSERT(6 == p1->i);

	N = 4;
	p1 = l.FindIf(Enum4, (ptr)&N);	
	ASSERT(4 == p1->i);

	N = 8;
	p1 = l.FindIf(Enum4, (ptr)&N);	
	ASSERT(NULL == p1);

	ASSERT(7 == l.size());

	DestroyDList(&l);
}


bool EnumCondRemove(DLNode* pNode, ptr pData)
{
    ASSERT(NULL != pNode);
    TLinkedList<DLNode>* pList = (TLinkedList<DLNode>*)pData;
    if (2 < pNode->i) {
        pList->remove(pNode);
        pList->Enumerate(EnumCondRemove, pList);
    }
    return true;
}

TEST_F(DLTest, Case15_test_Enumerate_remove)
{
    printf("test TLinkedList::Enumerate and remove...\n");

    int N = 10;

    TLinkedList<DLNode> l;

    DLNode* p[8];
    for(int i = 7; i > 0; --i){
        p[i] = NewDLNode(i);
        l.push_back(p[i]);
    }

    l.Enumerate(EnumCondRemove, &l);

    
    l.Enumerate(Enum1, &l);
}

TEST_F(DLTest, Case16_test_merge)
{
	printf("test merge...\n");
	TLinkedList<DLNode> l;
	DLNode* p[8];

    for(int i = 7; i > 0; --i){
        p[i] = NewDLNode(i);
        l.push_back(p[i]);
    }

	TLinkedList<DLNode> l2;
	l.merge(&l2);
	
	l.Print();
	l2.Print();
}

