#ifndef _SINGLE_LINK_LIST_H_
#define _SINGLE_LINK_LIST_H_



template <typename TElement>
static inline
int slink_push(INOUT TElement** ppHead, TElement* pNode)
{
    ASSERT(NULL != ppHead);
    ASSERT(NULL != pNode);
    int n = 1;

    TElement* pLast;
    for (pLast=pNode; pLast->pNext; pLast=pLast->pNext) n++;
    pLast->pNext = *ppHead;
    *ppHead = pNode;
    return n;
}


template <typename TElement>
static inline
TElement* slink_pop(INOUT TElement** ppHead)
{
    ASSERT(NULL != ppHead);

    TElement* pHead = *ppHead;
    if (pHead) {
        *ppHead = pHead->pNext;
        pHead->pNext = NULL;
        return pHead;
    }
    return NULL;
}


template <typename TElement>
static inline
bool slink_remove(INOUT TElement** ppHead, TElement* pNode)
{
    ASSERT(NULL != ppHead);
    ASSERT(NULL != pNode);

    TElement* pThis = *ppHead;
    if (pThis == pNode) {
        *ppHead = pThis->pNext;
        pThis->pNext = NULL;
        return true;
    }
    while (pThis) {
        if (pThis->pNext == pNode) {
            pThis->pNext = pNode->pNext;
            pNode->pNext = NULL;
            return true;
        }
        pThis = pThis->pNext;
    } 
    return false;
}


template <typename TElement>
static inline
bool slink_contains(TElement* pHead, TElement* pNode)
{
    ASSERT(NULL != pHead);
    ASSERT(NULL != pNode);

    for ( ; pHead; pHead=pHead->pNext) {
        if (pHead == pNode)
            return true;
    }
    return false;
}



template <typename TElement, typename TCallback>
static inline
bool slink_eat_list(INOUT TElement** ppHead, TCallback f, ptr pCtx)
{
    ASSERT(NULL != ppHead);
    TElement *pHead, *pNext;
    
    for (pHead=*ppHead; pHead; pHead=pNext) {
        pNext = pHead->pNext;
        pHead->pNext = NULL;
        if (!f(pHead, pCtx)) {
            *ppHead = pHead;
            return false;
        }
    }
    *ppHead = NULL;
    return true;
}

template <typename TElement, typename TCallback>
static inline
bool slink_enum_list(INOUT TElement** ppHead, TCallback f, ptr pCtx)
{
    ASSERT(NULL != ppHead);
    TElement *pHead, *pNext;

    for (pHead=*ppHead; pHead; pHead=pNext) {
        pNext = pHead->pNext;
        if (!f(pHead, pCtx)) {
            return false;
        }
    }
    return true;
}


template <typename TElement, typename TCallback>
static inline
bool slink_eat_list_rev(TElement** ppHead, TCallback f, ptr pCtx)
{
    ASSERT(NULL != ppHead);
    TElement* pNode = *ppHead;

    if (NULL == pNode)
        return true;
    if (pNode->pNext && !slink_eat_list_rev(&pNode->pNext, f, pCtx))
        return false;
    pNode->pNext = NULL;
    if (!f(pNode, pCtx))
        return false;
    *ppHead = NULL;
    return true;
}

template <typename TElement, typename TCallback>
static inline
bool slink_enum_list_rev(TElement** ppHead, TCallback f, ptr pCtx)
{
    ASSERT(NULL != ppHead);
    TElement* pNode = *ppHead;

    if (NULL == pNode)
        return true;
    if (pNode->pNext && !slink_enum_list_rev(&pNode->pNext, f, pCtx))
        return false;
    if (!f(pNode, pCtx))
        return false;
    return true;
}


#include "link.h"

template <typename TElement>
class TSingleList
{
protected:
    int m_count;
    SLINK m_head;

plclic:
    typedef bool (*ENUMERATOR)(TElement* pE, ptr pData);    

    TSingleList() { Init(); }
    ~TSingleList() {}

    void Init() {
        m_head._next = NULL;
        m_count = 0;
    }

    void push(TElement* e) {
        e->link._next = m_head._next;
        m_head._next = &e->link;
        m_count ++;
    }

    void pushn(int n, TElement* e, int size=sizeof(TElement)) {
        TElement* ee;

        SLINK* pHead = m_head._next;
        m_head._next = &e->link;
        for (int i=0; i<n-1; i++) {
            ee = (TElement*)(((char*)e) + size);
            e->link._next = &ee->link;
            e = ee;
        }
        e->link._next = pHead;
        m_count += n;
    }

    TElement* pop() {
        SLINK* p = m_head._next;
        if (p != NULL) {
            m_head._next = p->_next;
            m_count --;
            return CONTAINING_RECORD(p, TElement, link);
        }
        return NULL;
    }

    TElement* top() {
        SLINK* p = m_head._next;
        return (p != NULL) ? CONTAINING_RECORD(p, TElement, link) : NULL;
    }
    
    int size() const {
        return m_count;
    }

    bool Enumerate(ENUMERATOR f, ptr pData) {
        return slink_enum_list(&m_head, f, pData);
    }
};

#endif
