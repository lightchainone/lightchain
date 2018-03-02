#include "stdafx.h"
#include "Stl.h"
#include "UnitTest.h"

class StlTest {
  plclic:
    void SetUp() {
    }
    void TearDown() {
    }
};


struct Struct {
    int key;
    uint32 nRef;
};

static int s_nStructs = 0;
static Struct* CreateStruct(int key)
{
    Struct* p = new Struct;
    s_nStructs ++;
    return p;
}

static void DeleteStruct(Struct* p)
{
    s_nStructs --;
    ASSERT(NULL != p);
    delete p;
}

TEST_F(StlTest, FancyMapCase1) 
{
    TFancyMap<int, Struct> fm(CreateStruct, DeleteStruct);
    uint32 nId1, nId2, nId3;
    Struct *p1, *p2, *p3;
    
    s_nStructs = 0;
    p1 = fm.Create(1, &nId1);
    ASSERT_EQUAL(s_nStructs, 1);
    p2 = fm.Create(1, &nId2);
    ASSERT_EQUAL(s_nStructs, 1);
    ASSERT_EQUAL(p1, p2);
    ASSERT(nId1 != nId2);

    ASSERT_EQUAL(p1, fm.Get(nId1));
    ASSERT_EQUAL(p2, fm.Get(nId2));

    p3 = fm.Create(2, &nId3);
    ASSERT_EQUAL(s_nStructs, 2);
    ASSERT(nId3 != nId1);
    ASSERT(nId3 != nId2);
    ASSERT(p3 != p1);
    ASSERT_EQUAL(2, (int)fm.size());
    ASSERT_EQUAL(p3, fm.Get(nId3));

    bool b;
    
    ASSERT_EQUAL((Struct*)NULL, fm.Get(9999));
    b = fm.Delete(9999);
    ASSERT(!b);
    

    b = fm.Delete(nId3);
    ASSERT(b);
    ASSERT_EQUAL(s_nStructs, 1);

    b = fm.Delete(nId2);
    ASSERT(b);
    ASSERT_EQUAL(s_nStructs, 1);

    b = fm.Delete(nId1);
    ASSERT(b);
    ASSERT_EQUAL(s_nStructs, 0);

    ASSERT_EQUAL(0, (int)fm.size());
}
