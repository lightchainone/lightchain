
#include <lcclient_healthystrategy.h>
#include <gtest/gtest.h>
	
	
int main(int argc, char **argv)
{
 	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

class test_filterServer_suite : plclic ::testing::Test{
    protected:
        test_filterServer_suite(){};
        virtual ~test_filterServer_suite(){};
        virtual void SetUp() {
            
        };
        virtual void TearDown() {
            
        };
};
 

TEST_F(test_filterServer_suite, case_name1)
{
	
}
 
