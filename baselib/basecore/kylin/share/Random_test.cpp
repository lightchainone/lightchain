#include "stdafx.h"
#include "UnitTest.h"
#include "Random.h"

#define DECLARE_TEST_CLASS(testclassname) \
class testclassname { \
  plclic: \
    void SetUp() {} \
    void TearDown() {} \
}; 


DECLARE_TEST_CLASS(random)


TEST_F(random, case1)
{
    int x;
    int min = 65535*10;
    int max = 0;
    for (uint32 i=0; i<65535*100; i++) {
        x = 1025 + (int)(64510.0*rand()/(RAND_MAX+1.0));
        if (x < min)
            min = x;
        if (x > max)
            max = x;
        
    }
    ASSERT_EQUAL(min, 1025);
    ASSERT_EQUAL(max, 65534);
}
