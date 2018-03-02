    

#include "stdafx.h"
#include "SlidingWindow.h"
#include "UnitTest.h"

enum ItemState {
	IS_SLOT_EMPTY = 0,
	IS_ITEM_GOT = 1,
	IS_TO_GET_ITEM = 2,
	IS_TO_FREE_ITEM = 3
};


typedef struct __SDTestItem {
	ItemState state;
	int nSlot;
}SDTestItem, *p_SDTestItem;

static void OnItemCreated(SDTestItem* pItem, int nSlot, ptr);
static void OnSlotEmpty(SDTestItem* pItem, ptr pCtx);
static void OnItemReady(SDTestItem* pItem, ptr pCtx);


class PseudoAgent {
private:
	int m_nWindowSize;
	int m_nEmptySlots;
	int m_nReadyItems;
	SDTestItem **m_pItemPointerArray;
	TSlidingWindow<SDTestItem> m_SlidingWindow;
plclic:
	PseudoAgent(void) {
		m_nWindowSize = 0;
		m_nEmptySlots = 0;
	    m_nReadyItems = 0;
		m_pItemPointerArray = NULL;
	}

	~PseudoAgent(void) {
		if(m_pItemPointerArray)
			free(m_pItemPointerArray);	
	}

	void StartSlidingWindow() {
		printf("start SlidingWindow...\n");
		m_SlidingWindow.Start(this);
	}

	void PauseSlidingWindow() {
		printf("pause SlidingWindow...\n");
		m_SlidingWindow.Pause();
	}

	bool Create(int nWindowSize) {
		m_nWindowSize = nWindowSize;
		m_pItemPointerArray = (SDTestItem**)ZeroAlloc(m_nWindowSize * sizeof(p_SDTestItem));
		if(m_pItemPointerArray == NULL)
			return false;
		return m_SlidingWindow.Create(m_nWindowSize, OnItemReady, OnSlotEmpty, OnItemCreated);
	}

	bool CheckSlotorItem(int nSlot){
		return m_pItemPointerArray[nSlot] != NULL;
	}

	SDTestItem* GetItem(int nSlot){
		return m_pItemPointerArray[nSlot];
	}

	bool IsAcked(int nSlot){
		return m_pItemPointerArray[nSlot] != NULL;
	}

	void AddItem(int nSlot) {
		printf("\x1b[36m         User:\x1b[0m add item to slot#%d.\n", nSlot);
		ASSERT(m_nEmptySlots > 0);
		ASSERT(m_pItemPointerArray[nSlot] != NULL);
		m_nEmptySlots--;
		p_SDTestItem pTmp = m_pItemPointerArray[nSlot];
		m_pItemPointerArray[nSlot] = NULL;
		m_SlidingWindow.AddItem(pTmp);
	}

	void FreeItem(int nSlot) {
		printf("\x1b[36m         User:\x1b[0m free item in slot#%d.\n", nSlot);
		ASSERT(m_nReadyItems > 0);
		ASSERT(m_pItemPointerArray[nSlot] != NULL);
		m_nReadyItems--;
		p_SDTestItem pTmp = m_pItemPointerArray[nSlot];
		m_pItemPointerArray[nSlot] = NULL;
		m_SlidingWindow.FreeItem(pTmp);
	}
	
	void _ToGetItem(SDTestItem* pItem){
		printf("\x1b[35mSlidingWindow:\x1b[0m notify slot#%d is empty.\n", pItem->nSlot);
		ASSERT(m_pItemPointerArray[pItem->nSlot] == NULL);
		m_nEmptySlots++;
		pItem->state = IS_TO_GET_ITEM;
		m_pItemPointerArray[pItem->nSlot] = pItem;
	}
	
	void _ToUseItem(SDTestItem* pItem){
		printf("\x1b[35mSlidingWindow:\x1b[0m notify item in slot#%d is ready.\n", pItem->nSlot);
		ASSERT(m_pItemPointerArray[pItem->nSlot] == NULL);
		m_nReadyItems++;
		pItem->state = IS_TO_FREE_ITEM;
		m_pItemPointerArray[pItem->nSlot] = pItem;
	}

	int GetEmptySlots() const {
		return m_nEmptySlots;
	}

	int GetReadyItems() const {
		return m_nReadyItems;
	}
};


static void OnItemCreated(SDTestItem* pItem, int nSlot, ptr) {
	ZeroMemory(pItem, sizeof(SDTestItem));
}

static void OnSlotEmpty(SDTestItem* pItem, ptr pCtx){
	((PseudoAgent*)pCtx)->_ToGetItem(pItem);
}

static void OnItemReady(SDTestItem* pItem, ptr pCtx){
	((PseudoAgent*)pCtx)->_ToUseItem(pItem);
}

class TSlidingWindowTest {
plclic: 
    void SetUp() {} 
    void TearDown() {} 
protected:
	PseudoAgent m_Agent;
};

TEST_F(TSlidingWindowTest, Case1)
{


	
	const int nWindowSize = 16;
	ASSERT(m_Agent.Create(nWindowSize) == true);
	m_Agent.StartSlidingWindow();

	
	ASSERT(m_Agent.GetEmptySlots() == 16);
	ASSERT(m_Agent.GetReadyItems() == 0);
	for(int i = 0; i < nWindowSize; ++i)
		ASSERT(m_Agent.CheckSlotorItem(i));

	
	m_Agent.AddItem(0);
	ASSERT(m_Agent.GetEmptySlots() == 15);
	ASSERT(m_Agent.GetReadyItems() == 1);
	ASSERT(m_Agent.CheckSlotorItem(0));

	
	m_Agent.FreeItem(0);
	ASSERT(m_Agent.GetEmptySlots() == 16);
	ASSERT(m_Agent.GetReadyItems() == 0);
	ASSERT(m_Agent.CheckSlotorItem(0));

}


TEST_F(TSlidingWindowTest, Case2)
{


	
	const int nWindowSize = 16;
	ASSERT(m_Agent.Create(nWindowSize) == true);
	m_Agent.StartSlidingWindow();

	
	ASSERT(m_Agent.GetEmptySlots() == 16);
	ASSERT(m_Agent.GetReadyItems() == 0);
	for(int i = 0; i < nWindowSize; ++i)
		ASSERT(m_Agent.CheckSlotorItem(i));

	
	m_Agent.AddItem(1);
	ASSERT(m_Agent.GetItem(1) == NULL);
	ASSERT(m_Agent.IsAcked(1) == false);
	
	
	
	
	ASSERT(m_Agent.GetEmptySlots() == 15);
	ASSERT(m_Agent.GetReadyItems() == 0);
	
	
	m_Agent.AddItem(3);
	ASSERT(m_Agent.GetItem(3) == NULL);
	ASSERT(m_Agent.IsAcked(3) == false);
	
	
	
	
	ASSERT(m_Agent.GetEmptySlots() == 14);
	ASSERT(m_Agent.GetReadyItems() == 0);

	
	m_Agent.AddItem(0);
	ASSERT(m_Agent.IsAcked(0) == true);
	ASSERT(m_Agent.GetItem(0)->state == IS_TO_FREE_ITEM);
	ASSERT(m_Agent.GetItem(1)->state == IS_TO_FREE_ITEM);

	
	ASSERT(m_Agent.GetEmptySlots() == 13);
	ASSERT(m_Agent.GetReadyItems() == 2);

	
	m_Agent.FreeItem(1);
	ASSERT(m_Agent.IsAcked(1) == false);
	ASSERT(m_Agent.GetItem(0)->state == IS_TO_FREE_ITEM);
	ASSERT(m_Agent.GetItem(1) == NULL);

	
	ASSERT(m_Agent.GetEmptySlots() == 13);
	ASSERT(m_Agent.GetReadyItems() == 1);

	
	m_Agent.AddItem(2);
	ASSERT(m_Agent.IsAcked(2) == true);
	ASSERT(m_Agent.IsAcked(3) == true);	
	
	
	ASSERT(m_Agent.GetEmptySlots() == 12);
	ASSERT(m_Agent.GetReadyItems() == 3);

	
	m_Agent.FreeItem(0);
	ASSERT(m_Agent.IsAcked(0) == true);
	ASSERT(m_Agent.IsAcked(1) == true);

	ASSERT(m_Agent.GetEmptySlots() == 14);
	ASSERT(m_Agent.GetReadyItems() == 2);

	
	m_Agent.PauseSlidingWindow();	

	m_Agent.FreeItem(3);
	ASSERT(m_Agent.IsAcked(3) == false);

	m_Agent.FreeItem(2);
	ASSERT(m_Agent.IsAcked(2) == false);

	m_Agent.AddItem(4);
	ASSERT(m_Agent.IsAcked(4) == false);

	ASSERT(m_Agent.GetEmptySlots() == 13);
	ASSERT(m_Agent.GetReadyItems() == 0);

	
	m_Agent.StartSlidingWindow();	
	ASSERT(m_Agent.IsAcked(2) == true);
	ASSERT(m_Agent.IsAcked(3) == true);
	ASSERT(m_Agent.IsAcked(4) == true);

	ASSERT(m_Agent.GetEmptySlots() == 15);
	ASSERT(m_Agent.GetReadyItems() == 1);

	
	

}
