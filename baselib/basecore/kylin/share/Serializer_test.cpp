#include "stdafx.h"
#include "Serializer.h"
#include "UnitTest.h"

class TSerializerTest {
plclic: 
    void SetUp() {} 
    void TearDown() {} 
};


struct SItem {
	int data;
	DLINK link;
	bool bSignaled;
};


static void inline PrintSItem(SItem* pItem, ptr pCtx)
{
	ASSERT(pItem->bSignaled);
	printf("Item %d be signaled.\n", pItem->data);
}


static const int SItemNum = 8;
static SItem SIData[SItemNum];


static inline void init_sidata()
{
	for(int i = 0; i < SItemNum; ++i){
		SIData[i].data = i;
		SIData[i].bSignaled = false;
		DLINK_INITIALIZE(&SIData[i].link);
	}
}


TEST_F(TSerializerTest, Case1_test_Register)
{
	printf("test Register()...\n");
	TSerializer<SItem> s;
	init_sidata();

	s.Create(PrintSItem, NULL);

	
	SHOULD_ABORT(s.Register(NULL));

	
#define REGISTER_ALL \
	for(int i = 0; i < SItemNum; ++i) \
		s.Register(&SIData[i]);

	REGISTER_ALL

	ASSERT(8 == s.GetNumberOfUnsignaled());

	
	SHOULD_ABORT(s.Register(&SIData[0]));
}


TEST_F(TSerializerTest, Case2_test_Signal)
{
	printf("test Signal()...\n");
	TSerializer<SItem> s;
	init_sidata();
	
	s.Create(PrintSItem, NULL);
	REGISTER_ALL
	ASSERT(8 == s.GetNumberOfUnsignaled());
	
	
	SHOULD_ABORT(s.Signal(NULL));

#define SIGNAL_ONE(i) \
	printf("signal Item %d\n", i); \
	s.Signal(&SIData[i]); 

	
	SIGNAL_ONE(0)
	ASSERT(7 == s.GetNumberOfUnsignaled());
	ASSERT(DLINK_IS_STANDALONE(&SIData[0].link));

	
	SIGNAL_ONE(2)
	SIGNAL_ONE(3)
	printf("sigal Item 2, 3 while 1 still unsignaled.\n");
	ASSERT(7 == s.GetNumberOfUnsignaled());
	
	
	SIGNAL_ONE(1)
	ASSERT(4 == s.GetNumberOfUnsignaled());
	ASSERT(DLINK_IS_STANDALONE(&SIData[1].link));
	ASSERT(DLINK_IS_STANDALONE(&SIData[2].link));

	
	SHOULD_ABORT(s.Signal(&SIData[0]));
}



TEST_F(TSerializerTest, Case3_test_SetEnable)
{
	printf("test SetEnable()...\n");
	TSerializer<SItem> s;
	init_sidata();
	
	s.Create(PrintSItem, NULL);
	REGISTER_ALL

	ASSERT(8 == s.GetNumberOfUnsignaled());

	
	SIGNAL_ONE(1)	
	SIGNAL_ONE(0)	

	
	printf("Disable Serializer...\n");
	s.SetEnable(false);
	ASSERT(6 == s.GetNumberOfUnsignaled());

	SIGNAL_ONE(5)
	ASSERT(5 == s.GetNumberOfUnsignaled());
	ASSERT(DLINK_IS_STANDALONE(&SIData[5].link));

	SIGNAL_ONE(7)

	
	printf("Enable Serializer...\n");
	s.SetEnable(true);
	ASSERT(4 == s.GetNumberOfUnsignaled());
	
	SIGNAL_ONE(6)
	SIGNAL_ONE(3)
	SIGNAL_ONE(4)
	SIGNAL_ONE(2)
	ASSERT(0 == s.GetNumberOfUnsignaled());
}

#undef SIGNAL_ONE
#undef REGISTER_ALL
