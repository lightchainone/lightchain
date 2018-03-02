

#ifndef _COMP_Q_H_
#define _COMP_Q_H_

#include <set>
#include <map>
#include "Trace.h"
#include "SingleList.h"
#include "DolcleList.h"
using namespace std;


template<typename TElement, typename TLesser=less<TElement*> >
class TIndexedQueue
{
	typedef TLinkedList<TElement> Queue;
	typedef set<TElement*, TLesser> Indexer;
	typedef typename Indexer::iterator Iterator;
plclic:
	typedef bool (*ENUMERATOR)(TElement* pE, ptr pData);	

	TIndexedQueue() {}
	~TIndexedQueue() {}
	
	bool push_back(TElement* pElement, TElement** ppOldElement=NULL) {
        ASSERT(NULL != pElement);
        ASSERT(DLINK_IS_STANDALONE(&pElement->link));

		pair<Iterator, bool> pr;
		pr = m_Indexer.insert(pElement);
		if (pr.second) {
			m_Queue.push_back(pElement);
			return true;
		}
        if (ppOldElement) {
			*ppOldElement = *(pr.first);
        }
		return false;
	}

	bool push_front(TElement* pElement, TElement** ppOldElement=NULL) {
        ASSERT(NULL != pElement);
        ASSERT(DLINK_IS_STANDALONE(&pElement->link));

		pair<Iterator, bool> pr;
		pr = m_Indexer.insert(pElement);
		if (pr.second) {
			m_Queue.push_front(pElement);
			return true;
		}
        if (ppOldElement) {
			*ppOldElement = *(pr.first);
        }
		return false;
	}

	TElement* front_to_back() {
		TElement* pE = m_Queue.pop_front();
		if (pE) {
			m_Queue.push_back(pE);
		}
		return pE;
	}
	
	TElement* pop_front() {
		TElement* pE = m_Queue.pop_front();
		if (pE) {
			Iterator it = m_Indexer.find(pE);
			ASSERT(it != m_Indexer.end());
			m_Indexer.erase(it);
		}
		return pE;
	}
	
	TElement* get_front() {
		return m_Queue.get_front();
	}
	
	bool remove(TElement* pElement) {
        ASSERT(NULL != pElement);
		Iterator it = m_Indexer.find(pElement);
		if (it != m_Indexer.end()) {
			ASSERT(pElement == *it);
			m_Queue.remove(pElement);
			m_Indexer.erase(it);
			return true;
		}
		return false;
	}
	
	bool contains(TElement* pElement) const {
		Iterator it = m_Indexer.find(pElement);
		return (it != m_Indexer.end());
	}

	int size() const { return m_Queue.size(); }

	bool Enumerate(ENUMERATOR f, ptr pData) {
		return m_Queue.Enumerate(f, pData);
	}

private:
	Indexer m_Indexer;
	Queue m_Queue;
};


template<typename Key, typename TElement, typename Traits=less<Key> >
class TKeyedQueue
{
protected:
	typedef map<Key, TElement*, Traits> Indexer;
	typedef TLinkedList<TElement> Queue;
    typedef typename Indexer::value_type value_type;
plclic:
	typedef typename Indexer::iterator Iterator;
	typedef bool (*ENUMERATOR)(TElement* pE, ptr pData);	

	TKeyedQueue() {}
	~TKeyedQueue() {}

    void Transfer(TKeyedQueue<Key, TElement, Traits>* pDst) {
        pDst->m_Indexer = m_Indexer;
        m_Queue.Transfer(&pDst->m_Queue);
        m_Indexer.clear();
    }
	
	bool push_back(TElement* pElement, TElement** ppOldElement=NULL) {
        ASSERT(NULL != pElement);
        ASSERT(DLINK_IS_STANDALONE(&pElement->link));

		pair<Iterator, bool> pr;
		pr = m_Indexer.insert(value_type(pElement->key, pElement));
		if (pr.second) {
			m_Queue.push_back(pElement);
			return true;
		}
        if (ppOldElement) {
			*ppOldElement = pr.first->second;
        }
		return false;
	}

    TElement* push_back_and_replace(TElement* pNew) {
        ASSERT(NULL != pNew);
        ASSERT(DLINK_IS_STANDALONE(&pNew->link));

        pair<Iterator, bool> pr;
        pr = m_Indexer.insert(value_type(pNew->key, pNew));
        if (pr.second) {
            m_Queue.push_back(pNew);
        }
        else {
            TElement* pOld = pr.first->second;
            if (pOld != pNew) {                     
                pr.first->second = pNew;
                DLINK_REPLACE(&pOld->link, &pNew->link);
                DLINK_INITIALIZE(&pOld->link);
                return pOld;
            }
        }
        return NULL;
    }
	
	TElement* pop_front() {
		TElement* pE = m_Queue.pop_front();
		if (pE) {
			Iterator it = m_Indexer.find(pE->key);
			ASSERT(it != m_Indexer.end());
			m_Indexer.erase(it);
		}
		return pE;
	}

    TElement* front_to_back() {
        TElement* pE = m_Queue.pop_front();
        if (pE) {
            m_Queue.push_back(pE);
        }
        return pE;
    }

    bool move_to_back(TElement* pElement) {
        ASSERT(NULL != pElement);
        Iterator it = m_Indexer.find(pElement->key);
        if (m_Indexer.end()!=it && pElement==it->second) {
            m_Queue.remove(pElement);
            m_Queue.push_back(pElement);
            return true;
        }
        return false;
    }
	
    bool move_to_front(TElement* pElement) {
        ASSERT(NULL != pElement);
        Iterator it = m_Indexer.find(pElement->key);
        if (m_Indexer.end()!=it && pElement==it->second) {
            m_Queue.remove(pElement);
            m_Queue.push_front(pElement);
            return true;
        }
        return false;
    }

	TElement* get_front() {
		return m_Queue.get_front();
	}
	
	TElement* find(Key k) {
		Iterator it = m_Indexer.find(k);
		if (it != m_Indexer.end()) {
			return it->second;
		}
		return NULL;
	}

	TElement* remove(Key k) {
		Iterator it = m_Indexer.find(k);
		if (it != m_Indexer.end()) {
			TElement* pE = it->second;
			m_Queue.remove(pE);
			m_Indexer.erase(it);
			return pE;
		}
		return NULL;
	}
	 
	bool remove(TElement* pElement) {
        ASSERT(NULL != pElement);
		Iterator it = m_Indexer.find(pElement->key);
		if (m_Indexer.end()!=it && pElement==it->second) {
			m_Indexer.erase(it);
			m_Queue.remove(pElement);
			return true;
		}
		return false;
	}

	bool Enumerate(ENUMERATOR f, ptr pData) {
		return m_Queue.Enumerate(f, pData);
	}

	int size() const { return m_Queue.size(); }

protected:
	Indexer m_Indexer;          
	Queue m_Queue;              
};


template <typename TKey, typename TElement>
class TKeyedMultiQueue : plclic TKeyedQueue<TKey, TElement>
{
	typedef typename TKeyedQueue<TKey, TElement>::Indexer Indexer;
    typedef typename Indexer::value_type value_type;
plclic:
	typedef typename TKeyedQueue<TKey, TElement>::Iterator Iterator;
	TKeyedMultiQueue() {}
	~TKeyedMultiQueue() {}
	
	void push_back(TElement* pElement) {
        ASSERT(NULL != pElement);
        ASSERT(DLINK_IS_STANDALONE(&pElement->link));
        ASSERT(PTR_IS_NULL(pElement->pNext));

        TElement* pOld;
        pElement->pNext = NULL;
		if (!TKeyedQueue<TKey, TElement>::push_back(pElement, &pOld)) {
            ASSERT(!slink_contains(pOld, pElement));
            slink_push(&pOld->pNext, pElement);
		}
	}
	
	void push_back_and_supercede(TElement* pElement) {
        ASSERT(NULL != pElement);
        ASSERT(DLINK_IS_STANDALONE(&pElement->link));
        ASSERT(PTR_IS_NULL(pElement->pNext));

        pElement->pNext = NULL;
        TElement* pOld = TKeyedQueue<TKey, TElement>::push_back_and_replace(pElement);
        if (pOld) {
            ASSERT(!slink_contains(pOld, pElement));
            slink_push(&pElement->pNext, pOld);
        }
	}

    
	bool remove(TElement* pElement) {
        ASSERT(NULL != pElement);

		Iterator it;
		it = TKeyedQueue<TKey, TElement>::m_Indexer.find(pElement->key);
		if (it != TKeyedQueue<TKey, TElement>::m_Indexer.end()) {
			if (pElement == it->second) {
                
                TElement* pNext = pElement->pNext;
                if (pNext) {
                    TElement* pOld = TKeyedQueue<TKey, TElement>::push_back_and_replace(pNext);
                    ASSERT(pOld == pElement);
                    pElement->pNext = NULL;
                    return true;
                }
                return TKeyedQueue<TKey, TElement>::remove(pElement);
			}
			return slink_remove(&it->second->pNext, pElement);
		}
		return false;
	}

    
    TElement* remove(TKey key) {
        return TKeyedQueue<TKey, TElement>::remove(key);
    }

	int size() const { return TKeyedQueue<TKey, TElement>::size(); }
};


template <typename TKey, typename TElement, typename TLesser=less<TKey> >
class TMultiMap
{
	std::map<TKey, TElement*, TLesser> m_Indexer;
plclic:
    typedef typename std::map<TKey, TElement*, TLesser>::iterator Iterator;
    typedef typename std::map<TKey, TElement*, TLesser>::reverse_iterator R_Iterator;
    
    TMultiMap() {}
	~TMultiMap() {}

	void insert(TKey key, TElement* pE) {
        ASSERT(NULL != pE);
        ASSERT(DLINK_IS_STANDALONE(&pE->link));

		std::pair<Iterator, bool> pr = m_Indexer.insert(std::pair<TKey, TElement*>(key, pE));
		if (!pr.second) {
			TElement* pOld = pr.first->second;
			DLINK_INSERT_PREV(&pOld->link, &pE->link);
		}
        else {
            DLINK_INITIALIZE(&pE->link);
        }
	}

	TElement* find(TKey key) {
		Iterator it = m_Indexer.find(key);
		if (it != m_Indexer.end()) {
			return it->second;
		}
		return NULL;
	}

	TElement* remove(TKey key) {
		Iterator it = m_Indexer.find(key);
		if (it != m_Indexer.end()) {
			TElement* pOld = it->second;
			m_Indexer.erase(it);
			return pOld;
		}
		return NULL;
	}

    Iterator begin() { return m_Indexer.begin(); }
    Iterator end() { return m_Indexer.end(); }
    R_Iterator rbegin() { return m_Indexer.rbegin(); }
    R_Iterator rend() { return m_Indexer.rend(); }

    void erase(Iterator it) { m_Indexer.erase(it); }

	bool remove(TKey key, TElement* pE) {
        ASSERT(NULL != pE);
		Iterator it = m_Indexer.find(key);
		if (it != m_Indexer.end()) {
			TElement* pOld = it->second;
			DLINK* p = &pE->link;
			
			if (pE == pOld) {
                if (p != p->_next) {
					it->second = CONTAINING_RECORD(p->_next, TElement, link);
                    DLINK_REMOVE(p);
					DLINK_INITIALIZE(p);
				}
				else {
					m_Indexer.erase(it);
				}
                return true;
			}
            ASSERT(dlink_contains(pOld, pE));
			
			if (p != p->_next) {
				DLINK_REMOVE(p);
				DLINK_INITIALIZE(p);
                return true;
			}
		}
		return false;
	}
};


template <typename TKey, typename TElement, typename TLesser=less<TKey> >
class TPriorityQueue : TMultiMap<TKey, TElement, TLesser>
{
    typedef typename TMultiMap<TKey, TElement, TLesser>::Iterator Iterator;
    typedef typename TMultiMap<TKey, TElement, TLesser>::R_Iterator R_Iterator;
    int m_count;

plclic:
    TPriorityQueue() { m_count=0; }
    ~TPriorityQueue() {}
    
    void push(TElement* pE) {
        ASSERT(NULL != pE);
        insert(pE->nPriority, pE);
        m_count ++;
    }

    TElement* pop_bottom() {
        Iterator it = TMultiMap<TKey, TElement, TLesser>::begin();
        if (TMultiMap<TKey, TElement, TLesser>::end() != it) {
            TElement* pE = it->second;
            VERIFY_OR_ABORT((TMultiMap<TKey, TElement, TLesser>::remove(pE->nPriority, pE)));
            m_count --;
            return pE;
        }
        return NULL;
    }

    TElement* get_bottom() {
        Iterator it = TMultiMap<TKey, TElement, TLesser>::begin();
        if (TMultiMap<TKey, TElement, TLesser>::end() != it) {
            return it->second;
        }
        return NULL;
    }

    TElement* pop_top() {
        R_Iterator it = TMultiMap<TKey, TElement, TLesser>::rbegin();
        if (TMultiMap<TKey, TElement, TLesser>::rend() != it) {
            TElement* pE = it->second;
            VERIFY_OR_ABORT((TMultiMap<TKey, TElement, TLesser>::remove(pE->nPriority, pE)));
            m_count --;
            return pE;
        }
        return NULL;
    }

    TElement* get_top() {
        R_Iterator it = TMultiMap<TKey, TElement, TLesser>::rbegin();
        if (TMultiMap<TKey, TElement, TLesser>::rend() != it) {
            return it->second;
        }
        return NULL;
    }

    TKey top(TKey kDefault) {
        R_Iterator it = TMultiMap<TKey, TElement, TLesser>::rbegin();
        if (TMultiMap<TKey, TElement, TLesser>::rend() != it) {
            return it->second->nPriority;
        }
        return kDefault;
    }

    TKey bottom(TKey kDefault) {
        Iterator it = TMultiMap<TKey, TElement, TLesser>::begin();
        if (TMultiMap<TKey, TElement, TLesser>::end() != it) {
            return it->second->nPriority;
        }
        return kDefault;
    }

    bool remove(TElement* pE) {
        if (TMultiMap<TKey, TElement, TLesser>::remove(pE->nPriority, pE)) {
            m_count --;
            return true;
        }
        return false;
    }

    int size() const {
        return m_count;
    }
};

#endif
