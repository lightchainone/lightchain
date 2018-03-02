
#ifndef _SLIDING_WINDOW_H_
#define _SLIDING_WINDOW_H_

#include "extypes.h"
#include "Trace.h"
#include "thread.h"


template <typename TItem>
class TSlidingWindow
{
plclic:
	typedef void (*ON_ITEM_READY)(TItem* pItem, ptr pCtx);
	typedef void (*ON_SLOT_EMPTY)(TItem* pItem, ptr pCtx);
	typedef void (*INIT_ITEM)(TItem* pItem, int nWhichSlot, ptr pCtx);
	
private:
	TItem *m_pItems;
	uint8 *m_pBitmap;
	int m_nItems, m_nReady, m_nEmpty;
	ptr m_pCtx;
    enum State {
        SWS_VOID, SWS_CREATED, SWS_RUN, SWS_PAUSED
    } m_state;

	ON_ITEM_READY m_fNotifyReady;
	ON_SLOT_EMPTY m_fNotifyEmpty;
    INIT_ITEM m_fNotifyInit;
	int m_nThread;

	void _Notify() {
        while (1 == m_pBitmap[m_nReady]) {
            m_pBitmap[m_nReady] = 3;
            m_fNotifyReady(&m_pItems[m_nReady], m_pCtx);

            m_nReady ++;
            if (m_nReady == m_nItems) {
                m_nReady = 0;
            }
        }
        while (0 == m_pBitmap[m_nEmpty]) {
            m_pBitmap[m_nEmpty] = 2;
            m_fNotifyEmpty(&m_pItems[m_nEmpty], m_pCtx);

            m_nEmpty ++;
            if (m_nEmpty == m_nItems) {
                m_nEmpty = 0;
            }
        }
	}

plclic:
	TSlidingWindow() {
		m_nReady = 0;
		m_nEmpty = 0;
		m_nItems = 0;
		m_pItems = NULL;
		m_pBitmap = NULL;
        m_state = SWS_VOID;
	}
	~TSlidingWindow() {
        if (m_pItems) {
			delete[] m_pItems;
        }
        if (m_pBitmap) {
			free(m_pBitmap);
        }
	}

	bool Create(int nItems, ON_ITEM_READY f1, ON_SLOT_EMPTY f2, INIT_ITEM f3) {
		ASSERT_EQUAL(0, m_nItems);
        ASSERT_EQUAL(SWS_VOID, m_state);
		m_nItems = nItems;
		m_pItems = new TItem[nItems];
		m_pBitmap = (uint8*)ZeroAlloc(nItems);
		if (m_pItems && m_pBitmap) {
			m_fNotifyReady = f1;
			m_fNotifyEmpty = f2;
            m_fNotifyInit = f3;
			m_nReady = 0;
			m_nEmpty = 0;
            m_state = SWS_CREATED;
			return true;
		}
        TRACE0("Fatal: no enough memory to create sliding window\n");
		return false;
	}

	void Start(ptr pCtx) {
        ASSERT(NULL != m_pBitmap);
        ASSERT(SWS_VOID != m_state);
        if (SWS_PAUSED != m_state) {
            m_nThread = thread_getid();
            ZeroMemory(m_pBitmap, m_nItems);
            m_nReady = 0;
            m_nEmpty = 0;
            m_pCtx = pCtx;
            if (SWS_CREATED == m_state) {
                for (int i=0; i<m_nItems; i++) {
                    m_fNotifyInit(&m_pItems[i], i, pCtx);
                    m_pItems[i].nSlot = i;
                }
            }
        }
#ifdef _DEBUG
        else {
            ASSERT_EQUAL(m_nThread, thread_getid());
        }
#endif
        m_state = SWS_RUN;
        _Notify();
	}

    void Pause() {
        ASSERT_EQUAL(m_nThread, thread_getid());
        m_state = SWS_PAUSED;
    }

	void AddItem(TItem* pItem) {
		ASSERT_EQUAL(m_nThread, thread_getid());
		ASSERT_EQUAL((uint8)2, m_pBitmap[pItem->nSlot]);
		m_pBitmap[pItem->nSlot] = 1;

        if (SWS_RUN == m_state) {
		    _Notify();
        }
	}

	void FreeItem(TItem* pItem) {
		ASSERT_EQUAL(m_nThread, thread_getid());
		ASSERT_EQUAL((uint8)3, m_pBitmap[pItem->nSlot]);
		m_pBitmap[pItem->nSlot] = 0;

        if (SWS_RUN == m_state) {
		    _Notify();
        }
	}
};

#endif
