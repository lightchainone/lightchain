#include "stdafx.h"
#include "SingleList.h"
#include "UnitTest.h"

class SLTest {
  plclic:
    void SetUp() {
    }
    void TearDown() {
    }
};


struct SLNode {
    int i;
    SLNode* pNext;
};


static int g_i = 5;


static SLNode* NewNode(int i)
{
    SLNode* p = new SLNode;
    p->i = i;
    p->pNext = NULL;
    return p;
}


static bool Enum1(SLNode* pNode, ptr)
{
    printf("node %d\n", pNode->i);
    ASSERT(pNode->i == g_i);
    ASSERT(NULL == pNode->pNext);
    g_i = pNode->i-1;
	delete pNode;
    return true;
}



TEST_F(SLTest, Case1_test_slink_eat_list) 
{
    printf("test slink_push() and slink_eat_list()...\n");
    SLNode* pHead = NULL;
    
    g_i = 5;
    slink_push(&pHead, NewNode(1));
    slink_push(&pHead, NewNode(2));
    slink_push(&pHead, NewNode(3));
    slink_push(&pHead, NewNode(4));
    slink_push(&pHead, NewNode(5));

    slink_eat_list(&pHead, Enum1, NULL);
    ASSERT(NULL == pHead);
    ASSERT(0 == g_i);
}



static bool Enum2(SLNode* pNode, ptr)
{
    printf("node %d\n", pNode->i);
    ASSERT(pNode->i == (g_i + 1));
    ASSERT(NULL == pNode->pNext);
    g_i = pNode->i;
	delete pNode;
    return true;
}


TEST_F(SLTest, Case2_test_slink_eat_list_rev) 
{
    printf("test slink_eat_list_rev()...\n");

    SLNode* pHead = NULL;

    g_i = 0;

    slink_push(&pHead, NewNode(1));
    slink_push(&pHead, NewNode(2));
    slink_push(&pHead, NewNode(3));
    slink_push(&pHead, NewNode(4));
    slink_push(&pHead, NewNode(5));

    slink_eat_list_rev(&pHead, Enum2, NULL);
    ASSERT(NULL == pHead);
    ASSERT(5 == g_i);
}



TEST_F(SLTest, Case3_test_slink_push) 
{
    printf("test pushback a list of elements to an empty singlelist using slink_push()...\n");
	
    SLNode* pHead = NULL;

    g_i = 0;
    slink_push(&pHead, NewNode(1));
    slink_push(&pHead, NewNode(2));
    slink_push(&pHead, NewNode(3));
    slink_push(&pHead, NewNode(4));
    slink_push(&pHead, NewNode(5));


    SLNode* pHead2 = NULL;
    slink_push(&pHead2, pHead);

    slink_eat_list_rev(&pHead2, Enum2, NULL);
    ASSERT(NULL == pHead2);
    ASSERT(5 == g_i);
}


TEST_F(SLTest, Case4_test_slink_push) 
{
    printf("test pushback a list of elements to an non-empty singlelist using slink_push()...\n");

    SLNode* pHead = NULL;

    g_i = 0;
    slink_push(&pHead, NewNode(2));
    slink_push(&pHead, NewNode(3));
    slink_push(&pHead, NewNode(4));
    slink_push(&pHead, NewNode(5));

    SLNode* pHead2 = NewNode(1);
    slink_push(&pHead2, pHead);
	
	
    slink_eat_list_rev(&pHead2, Enum2, NULL);
    ASSERT(NULL == pHead2);
    ASSERT(5 == g_i);
}




static bool Enum3(SLNode* pNode, ptr)
{
    printf("node %d\n", pNode->i);
    ASSERT(pNode->i == (g_i + 1));
    ASSERT(NULL == pNode->pNext);
    if (pNode->i == 3)
        return false;
    g_i = pNode->i;
	delete pNode;
    return true;
}

TEST_F(SLTest, Case5_test_slink_eat_list_rev) 
{
    printf("test breakout when slink_eat_list_rev()...\n");
    SLNode* pHead = NULL;

    g_i = 0;
    slink_push(&pHead, NewNode(1));
    slink_push(&pHead, NewNode(2));
    slink_push(&pHead, NewNode(3));
    slink_push(&pHead, NewNode(4));
    slink_push(&pHead, NewNode(5));
	
	
    SLNode* pHead2 = NULL;
    slink_push(&pHead2, pHead);

    bool b = slink_eat_list_rev(&pHead2, Enum3, NULL);
    ASSERT(!b);
    ASSERT(2 == g_i);
    ASSERT(NULL != pHead2);
	

	printf("slink_eat_list_rev() break here.\n");
	printf("so go on eating...\n");
    b = slink_eat_list_rev(&pHead2, Enum2, NULL);
    ASSERT(b);
    ASSERT(NULL == pHead2);
    ASSERT(5 == g_i);
}



TEST_F(SLTest, Case6_test_slink_pop) 
{
    printf("test slink_pop()...\n");

    SLNode* pHead = NULL;
    slink_push(&pHead, NewNode(1));
    slink_push(&pHead, NewNode(2));
	
	
    SLNode* pTmp = NULL;
	pTmp = slink_pop(&pHead);
	ASSERT(pTmp->i == 2);
	delete pTmp;
	ASSERT(NULL != pHead);
	
	pTmp = slink_pop(&pHead);
	ASSERT(pTmp->i == 1);
	delete pTmp;
	ASSERT(NULL == pHead);

	pTmp = slink_pop(&pHead);
	ASSERT(NULL == pTmp);
}



TEST_F(SLTest, Case7_test_slink_remove) 
{
    printf("test slink_remove() the head of list...\n");

    SLNode* pHead = NULL;

	SHOULD_ABORT(slink_remove(&pHead, (SLNode*)NULL));

    SLNode* p1 = NewNode(1);
    slink_push(&pHead, p1);
	SHOULD_ABORT(slink_remove(&pHead, (SLNode*)NULL));
	
	ASSERT(slink_remove(&pHead, p1));
	ASSERT(NULL == pHead);

	SLNode* p2 = NewNode(2);
	slink_push(&pHead, p1);
	slink_push(&pHead, p2);
	
	
	ASSERT(NULL != pHead);
	ASSERT(slink_remove(&pHead, p1));
	ASSERT(pHead == p2);

	ASSERT(slink_remove(&pHead, p2));
	ASSERT(NULL == pHead);

	delete p1;
	delete p2;
}



static bool Enum4(SLNode* pNode, ptr)
{
	ASSERT(NULL != pNode);
    printf("node %d\n", pNode->i);
    return true;
}



TEST_F(SLTest, Case8_test_slink_remove) 
{
    printf("test slink_remove() a middle element of list...\n");
	
	SLNode* pHead = NULL;
	
	SLNode* p1 = NewNode(1);
	SLNode* p2 = NewNode(2);
	SLNode* p3 = NewNode(3);
	
    slink_push(&pHead, p1);
    slink_push(&pHead, p2);
    slink_push(&pHead, p3);
	
	
	printf("before delete 2:\n");
	ASSERT(slink_enum_list(&pHead, Enum4, NULL));
		
	ASSERT(slink_remove(&pHead, p2));
	
	
	printf("after delete 2:\n");
	ASSERT(slink_enum_list(&pHead, Enum4, NULL));

	delete p1;
	delete p2;
	delete p3;
}


TEST_F(SLTest, Case9_test_slink_remove) 
{
    printf("test slink_remove() the last element of list...\n");
	
	SLNode* pHead = NULL;
	
	SLNode* p1 = NewNode(1);
	SLNode* p2 = NewNode(2);
	SLNode* p3 = NewNode(3);

    slink_push(&pHead, p1);
    slink_push(&pHead, p2);
    slink_push(&pHead, p3);
	
	
	printf("before remove 1:\n");
	ASSERT(slink_enum_list(&pHead, Enum4, NULL));
		
	ASSERT(slink_remove(&pHead, p1));

	
	printf("after remove 1:\n");
	ASSERT(slink_enum_list(&pHead, Enum4, NULL));

	delete p1;
	delete p2;
	delete p3;
}



static bool Enum5(SLNode* pNode, ptr)
{
	ASSERT(NULL != pNode);
	if(pNode->i%3 == 0 && pNode->i%2 != 0)
		return false;

	SLNode*	pNext = pNode->pNext;
	if(pNext && pNext->i%2 == 0){
		pNode->pNext = pNext->pNext;
		delete pNext;
	}

    return true;
}



TEST_F(SLTest, Case10_test_slink_enum_list) 
{
    printf("test slink_enum_list()...\n");
	
	SLNode* pHead = NULL;
	
    slink_push(&pHead, NewNode(1));
    slink_push(&pHead, NewNode(2));
    slink_push(&pHead, NewNode(3));
    slink_push(&pHead, NewNode(4));
    slink_push(&pHead, NewNode(5));
    slink_push(&pHead, NewNode(6));
    slink_push(&pHead, NewNode(7));
	
	
	printf("before enumerate using Enum5:\n");
	ASSERT(slink_enum_list(&pHead, Enum4, NULL));
		
	ASSERT(!slink_enum_list(&pHead, Enum5, NULL));
	
	
	printf("after enumerate using Enum5:\n");
	ASSERT(slink_enum_list(&pHead, Enum4, NULL));

	SLNode* pTmp;
	while(NULL != (pTmp = slink_pop(&pHead)))
		delete pTmp;

	ASSERT(NULL == pHead);
}



static bool Enum6(SLNode* pNode, ptr)
{
	ASSERT(NULL != pNode);
	if(pNode->i%3 == 0 && pNode->i%2 == 0)
		return false;

	SLNode*	pNext = pNode->pNext;
	if(pNext && pNext->i%2 == 0){
		pNode->pNext = pNext->pNext;
		delete pNext;
	}

    return true;
}



TEST_F(SLTest, Case11_test_slink_enum_list_rev) 
{
    printf("test slink_enum_list_rev()...\n");
	
	SLNode* pHead = NULL;
	
    slink_push(&pHead, NewNode(1));
    slink_push(&pHead, NewNode(2));
    slink_push(&pHead, NewNode(3));
    slink_push(&pHead, NewNode(4));
    slink_push(&pHead, NewNode(5));
    slink_push(&pHead, NewNode(6));
    slink_push(&pHead, NewNode(7));
	
	
	printf("before reversely enumerate using Enum6:\n");
	ASSERT(slink_enum_list_rev(&pHead, Enum4, NULL));
		
	ASSERT(!slink_enum_list_rev(&pHead, Enum6, NULL));
	
	
	printf("after reversely enumerate using Enum6:\n");
	ASSERT(slink_enum_list_rev(&pHead, Enum4, NULL));

	SLNode* pTmp;
	while(NULL != (pTmp = slink_pop(&pHead)))
		delete pTmp;

	ASSERT(NULL == pHead);
}

struct Node {
    int i;
    SLINK link;
};


TEST_F(SLTest, Case12_test_pushn)
{
    printf("test TSingleList::pushn\n");

    Node nodes[10];
    memset(nodes, 0xCC, sizeof(nodes));
    TSingleList<Node> ll;

    ll.pushn(10, nodes);

    Node* p;
    for (int i=0; i<10; i++) {
        p = ll.pop();
        printf("pop: %p\n", p);
    }
}
