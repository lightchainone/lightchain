
#include <util.hpp>
#include <gtest/gtest.h>
	
	
int main(int argc, char **argv)
{
 	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

class test_create_function_object_suite : plclic ::testing::Test{
    protected:
        test_create_function_object_suite(){};
        virtual ~test_create_function_object_suite(){};
        virtual void SetUp() {
            
        };
        virtual void TearDown() {
            
        };
};
 

TEST_F(test_create_function_object_suite, case_name1)
{
	
}
 

class test_call_function_object_suite : plclic ::testing::Test{
    protected:
        test_call_function_object_suite(){};
        virtual ~test_call_function_object_suite(){};
        virtual void SetUp() {
            
        };
        virtual void TearDown() {
            
        };
};
 

TEST_F(test_call_function_object_suite, case_name1)
{
	
}
 
