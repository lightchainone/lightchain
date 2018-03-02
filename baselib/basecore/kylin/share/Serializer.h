
#ifndef _SERIALIZER_H_
#define _SERIALIZER_H_

#include "DolcleList.h"


template <typename TItem>
class TSerializer
{
plclic:
    TSerializer() {}
    ~TSerializer() {}

    typedef void (*ON_ITEM_READY)(TItem* pItem, ptr pCtx);
    typedef bool (*ENUMERATOR)(TItem* pItem, ptr pData);
    void Create(ON_ITEM_READY f, ptr pCtx) {
        m_pCtx = pCtx;
        m_fReady = f;
        m_bEnable = true;
    }
    
    void SetEnable(bool bEnable) {
        m_bEnable = bEnable;
    }

    void Register(TItem* pItem) {
        ASSERT(NULL != pItem);
        pItem->bSignaled = false;
        m_Queue.push_back(pItem);
    }

    void Signal(TItem* pItem) {
        ASSERT(NULL != pItem);
        ASSERT(!pItem->bSignaled);
        pItem->bSignaled = true;
        if (m_bEnable) {
            int n = m_Queue.size();
            ASSERT(0 != n);
            for (int i=0; i<n; i++) {
                pItem = m_Queue.pop_front();
                if (pItem->bSignaled) {
                    m_fReady(pItem, m_pCtx);
                }
                else {
                    m_Queue.push_front(pItem);
                    break;
                }
            }
        }
        else {
            m_fReady(pItem, m_pCtx);
            m_Queue.remove(pItem);
        }
    }

    int GetNumberOfUnsignaled() {
        return m_Queue.size();
    }

    bool Enumerate(ENUMERATOR f, ptr pData) {
        return m_Queue.Enumerate(f, pData);
    }

private:
    ON_ITEM_READY m_fReady;
    ptr m_pCtx;
    TLinkedList<TItem> m_Queue;
    bool m_bEnable;
};

#endif
