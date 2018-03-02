
#ifndef _DOLCLE_LINK_LIST_H_
#define _DOLCLE_LINK_LIST_H_

#include "link.h"
#include "Trace.h"
#include "utils.h"

template<typename TElement>
static inline
bool dlink_contains(TElement* pHead, TElement* pNode) 
{
    ASSERT(NULL != pHead);
    ASSERT(NULL != pNode);
    TElement* pTmp = pHead;
    do {
        if (pTmp == pNode)
            return true;
        if (pTmp && pTmp->link._next) {
            pTmp = CONTAINING_RECORD(pTmp->link._next, TElement, link);
        }
        else {
            break;
        }
    } while (NULL!=pTmp && pHead!=pTmp);
    return false;
}


template<typename TElement>
static inline
TElement* dlink_pop_self(TElement* pE)
{
    ASSERT(NULL != pE);
    DLINK* p = &pE->link;
    if (p!=p->_next && NULL!=p->_next) {
        pE = CONTAINING_RECORD(p->_next, TElement, link);
        DLINK_REMOVE(p);
        DLINK_INITIALIZE(p);
        return pE;
    }
    return NULL;
}

template<typename TElement>
static inline
TElement* dlink_get_next(TElement* pE)
{
    ASSERT(NULL != pE);
    DLINK* p = pE->link._next;
    if (NULL != p) {
        pE = CONTAINING_RECORD(p, TElement, link);
        return pE;
    }
    return NULL;
}

template<typename TElement>
static inline
TElement* dlink_get_prev(TElement* pE)
{
    ASSERT(NULL != pE);
    DLINK* p = pE->link._prev;
    if (NULL != p) {
        pE = CONTAINING_RECORD(p, TElement, link);
        return pE;
    }
    return NULL;
}

template<typename TElement>
static inline
TElement* dlink_get_container(DLINK* pLink)
{
    ASSERT(NULL != pLink);
    return CONTAINING_RECORD(pLink, TElement, link);
}

template <typename TElement>
class TLinkedList
{
plclic:
    typedef bool (*ENUMERATOR)(TElement* pE, ptr pData);    

    TLinkedList() { Init(); }
    ~TLinkedList() {}
    
    void Init() {
        DLINK_INITIALIZE(&m_head);
        m_count = 0;
        m_nEnuming = 0;
    }

    void Transfer(TLinkedList<TElement>* pDst) {
        pDst->m_count = m_count;
        pDst->m_head._next = m_head._next;
        m_head._next->_prev = &pDst->m_head;
        pDst->m_head._prev = m_head._prev;
        m_head._prev->_next = &pDst->m_head;
        pDst->m_nEnuming = m_nEnuming;
        
        Init();
    }
    
    void Assert() const {
        int n = 0;
        DLINK* p = m_head._next;

        ASSERT_EQUAL(p->_prev->_next, p);
        while (p != &m_head) {
            ASSERT_EQUAL(p->_prev->_next, p);
            p = p->_next;
            n ++;
        }
        ASSERT_EQUAL(m_count, n);
    }

    void Print() const {
        const DLINK *p = &m_head;

        TRACE0("-- DLINK head: %p --\n", p);
        do {
            TRACE0("%p <- %p -> %p\n", p->_prev, p, p->_next);
            p = p->_next;
        } while (p != &m_head);
        TRACE0("--           END          --\n");
    }

    
    bool contains(TElement* pNode) const {
        ASSERT(NULL != pNode);
        if (DLINK_IS_STANDALONE(&pNode->link)) {
            return false;
        }
#ifndef _UNITTEST
        return true;
#endif
        for (DLINK* p=m_head._next; p && p!=&m_head; p=p->_next) {
            if (p == &pNode->link)
                return true;
        }
        return false;
    }

    int size() const {
        return m_count;
    }

    void merge(TLinkedList<TElement>* pList) {
        m_head._prev->_next = pList->m_head._next;
        pList->m_head._next->_prev = m_head._prev;
        m_head._prev = pList->m_head._prev;
        pList->m_head._prev->_next = &m_head;
        DLINK_INITIALIZE(&pList->m_head);
        m_count += pList->m_count;
        pList->m_count = 0;
    }
    
    int push_back(TElement* e) {
        ASSERT(NULL != e);
        ASSERT(DLINK_IS_STANDALONE(&e->link));
        DLINK_INSERT_PREV(&m_head, &e->link);
        return ++ m_count;
    }

    TElement* pop_back() {
        DLINK* p = m_head._prev;
        if (p != &m_head) {
            DLINK_REMOVE(p);
            DLINK_INITIALIZE(p);
            m_count --;
            return CONTAINING_RECORD(p, TElement, link);
        }
        ASSERT(0 == m_count);
        return NULL;
    }

    TElement* get_back() {
        DLINK* p = m_head._prev;
        if (p != &m_head) {
            return CONTAINING_RECORD(p, TElement, link);
        }
        ASSERT(0 == m_count);
        return NULL;
    }

    int push_front(TElement* e) {
        ASSERT(NULL != e);

        ASSERT(DLINK_IS_STANDALONE(&e->link));
        DLINK_INSERT_NEXT(&m_head, &e->link);
        return ++ m_count;
    }

    TElement* pop_front() {
        DLINK* p = m_head._next;
        if (p != &m_head) {
            DLINK_REMOVE(p);
            DLINK_INITIALIZE(p);
            m_count --;
            return CONTAINING_RECORD(p, TElement, link);
        }
        ASSERT_EQUAL(0, m_count);
        return NULL;
    }

    TElement* get_front() {
        DLINK* p = m_head._next;
        if (p != &m_head) {
            ASSERT(0 < m_count);
            return CONTAINING_RECORD(p, TElement, link);
        }
        ASSERT_EQUAL(0, m_count);
        return NULL;
    }
  
    TElement* get_next(TElement* pElement) {
        ASSERT(contains(pElement));
        DLINK* p = pElement->link._next;
        if (p != &m_head) {
            return CONTAINING_RECORD(p, TElement, link);
        }
        return NULL;
    }

    TElement* get_prev(TElement* pElement) {
        ASSERT(contains(pElement));
        DLINK* p = pElement->link._prev;
        if (p != &m_head) {
            return CONTAINING_RECORD(p, TElement, link);
        }
        return NULL;
    }

    int remove(TElement* pElement) {
        VERIFY_OR_ABORT(contains(pElement));
        DLINK_REMOVE(&pElement->link);
        DLINK_INITIALIZE(&pElement->link);
        return -- m_count;
    }

    bool replace(TElement* pOld, TElement* pNew) {
        VERIFY_OR_RETURN(DLINK_IS_STANDALONE(&pNew->link), false);
        VERIFY_OR_RETURN(contains(pOld), false);
        
        DLINK_REPLACE(&pOld->link, &pNew->link);
        DLINK_INITIALIZE(&pOld->link);
        return true;
    }

    bool Enumerate(ENUMERATOR f, ptr pData) {
        UNLIKELY_IF (0 != m_nEnuming ++) {
            return false;
        }

        TElement* pE;
        DLINK* p = m_head._next;
        while (p != &m_head) {
            pE = CONTAINING_RECORD(p, TElement, link);
            p = p->_next;
            if (!f(pE, pData)) {
                if (1 != m_nEnuming) {
                    m_nEnuming = 0;
                    return Enumerate(f, pData);
                }
                m_nEnuming = 0;
                return false;
            }
        }
        m_nEnuming = 0;
        return true;
    }

    bool Eat(ENUMERATOR f, ptr pData) {
        TElement* pE;
        
        while (NULL != (pE=pop_front())) {
            if (!f(pE, pData)) {
                push_front(pE);
                return false;
            }
        }
        return true;
    }

    TElement* FindIf(ENUMERATOR f, ptr pData) {
        TElement* pE;
        DLINK* p = m_head._next;
        while (p != &m_head) {
            pE = CONTAINING_RECORD(p, TElement, link);
            p = p->_next;
            if (f(pE, pData))
                return pE;
        }
        return NULL;
    }

protected:
    int m_count;
    DLINK m_head;
    int m_nEnuming;
};

#endif
