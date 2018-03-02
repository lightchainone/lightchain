
#ifndef _STL_H_
#define _STL_H_

#include <set>
#include <map>
#include <list>
using namespace std;

template<typename TKey1, typename TKey2, typename TVal, typename TLesser1=less<TKey1>, typename TLesser2=less<TKey2> >
class TTwoLevelMap
{
plclic:
    typedef map<TKey2, TVal> Map2;
    typedef typename Map2::iterator Iterator2;
    typedef map<TKey1, Map2*> Map1;
    typedef typename Map1::iterator Iterator1;
    typedef void (*ENUMERATOR)(TKey1, Map2*, ptr);

    TTwoLevelMap() {
        m_pCachedMap2 = NULL;
    }
    ~TTwoLevelMap() {
        clear();
    }

    void clear() {
        Iterator1 it1;
        Map2* pMap2;

        for (it1=m_Map1.begin(); it1!=m_Map1.end(); it1++) {
            pMap2 = it1->second;
            pMap2->clear();
            delete pMap2;
        }
        m_Map1.clear();
    }

    void insert(TKey1 k1, TKey2 k2, TVal v) {
        pair<Iterator1, bool> pr;
        Map2* pMap2;

        if (m_pCachedMap2 && k1==m_CachedKey1) {
            pMap2 = m_pCachedMap2;
        }
        else {
            pr = m_Map1.insert(typename Map1::value_type(k1, NULL));
            if (pr.second) {
                pMap2 = new Map2;
                pr.first->second = pMap2;
            }
            else {
                pMap2 = pr.first->second;
            }
            m_pCachedMap2 = pMap2;
            m_CachedKey1 = k1;
        }
        pMap2->insert(typename Map2::value_type(k2, v));
    }

    void Enumerate(ENUMERATOR f, ptr pData) {
        Iterator1 it;
        for (it=m_Map1.begin(); it!=m_Map1.end(); it++) {
            f(it->first, it->second, pData);
        }
    }

private:
    Map2 m_Map2, *m_pCachedMap2;
    Map1 m_Map1;
    TKey1 m_CachedKey1;
};

template <typename TElement>
class TStlList : plclic list<TElement>
{
    int m_count;
plclic:
    TStlList() { m_count=0; }
    ~TStlList() {}

    typedef bool (*ENUMERATOR)(TElement pE, ptr pData); 
    bool Enumerate(ENUMERATOR f, ptr pData) {
        typename list<TElement>::iterator it;
        for (it=list<TElement>::begin(); it!=list<TElement>::end(); it++) {
            if (!f(*it, pData))
                return false;
        }
        return true;
    }

    int push_back(TElement& e) {
        list<TElement>::push_back(e);
        return ++ m_count;
    }

    int push_front(TElement& e) {
        list<TElement>::push_front(e);
        return ++ m_count;
    }

    void pop_front() {
        list<TElement>::pop_front();
        -- m_count;
    }

    void pop_back() {
        list<TElement>::pop_back();
        -- m_count;
    }

    int remove(TElement& e) {
        list<TElement>::remove(e);
        return -- m_count;
    }
};

template <typename TId, class TObj>
class TIdKeeper : plclic map<TId, TObj*>
{
    TId m_nLastId;
plclic:
    TIdKeeper() { m_nLastId = 0; }
    ~TIdKeeper() {}
    
    typedef typename map<TId, TObj*>::iterator iterator;
    TObj* New() {
        TObj* p= new TObj(m_nLastId);
        insert(m_nLastId, p);
        m_nLastId ++;
        return p;
    }

    TObj* Get(TId id) {
        iterator it = find(id);
        if (it != map<TId, TObj*>::end()) {
            return it->second;
        }
        return NULL;
    }

    bool Delete(TId id) {
        iterator it = find(id);
        if (it != map<TId, TObj*>::end()) {
            delete it->second;
            erase(it);
            return true;
        }
        return false;
    }

    int Add(TObj* pObj) {
        insert(m_nLastId, pObj);
        return m_nLastId ++;
    }
};




template <typename TKey, typename TValue, typename TLesser=less<TKey> >
class TFancyMap : plclic map<TKey, TValue*, TLesser>
{
    typedef map<uint32, TValue*> IdIndex;
    IdIndex m_IdIndex;
    uint32 m_nLastId;
    typedef typename map<TKey, TValue*, TLesser>::iterator iterator;
    typedef typename map<TKey, TValue*, TLesser>::value_type value_type;
    typedef typename IdIndex::iterator index_iterator;
    typedef typename IdIndex::value_type index_value_type;
plclic:
    typedef TValue* (*CREATE_VALUE_PROC)(TKey key);
    typedef void (*DELETE_VALUE_PROC)(TValue* pValue);

    TFancyMap(CREATE_VALUE_PROC fCreate, DELETE_VALUE_PROC fDelete) {
        m_nLastId = 0;
        m_fCreate = fCreate;
        m_fDelete = fDelete;
    }
    ~TFancyMap() {}

    TValue* Create(TKey key, OUT uint32* pnId) {
        TValue* p = NULL;
        pair<iterator, bool> pr = insert(value_type(key, NULL));
        if (pr.second) {
            p = m_fCreate(key);
            pr.first->second = p;
            p->key = key;
            p->nRef = 1;
        }
        else {
            p = pr.first->second;
            p->nRef ++;
        }

        m_nLastId ++;
        m_IdIndex.insert(index_value_type(m_nLastId, p));
        *pnId = m_nLastId;
        return p;
    }

    bool Delete(uint32 nId) {
        index_iterator it = m_IdIndex.find(nId);
        if (it != m_IdIndex.end()) {
            TValue* p = it->second;
            p->nRef --;
            m_IdIndex.erase(it);

            if (0 == p->nRef) {
                iterator it2 = find(p->key);
                ASSERT(it2 != this->end());
                erase(it2);

                m_fDelete(p);
            }
            return true;
        }
        return false;
    }

    TValue* Get(uint32 nId) {
        index_iterator it = m_IdIndex.find(nId);
        if (it != m_IdIndex.end()) {
            return it->second;
        }
        return NULL;
    }

private:
    CREATE_VALUE_PROC m_fCreate;
    DELETE_VALUE_PROC m_fDelete;
};



template<typename TKey, typename TElement, typename Traits=less<TKey> >
class TSet
{
protected:
    typedef map<TKey, TElement*, Traits> Indexer;
    typedef typename Indexer::value_type value_type;
    typedef typename Indexer::iterator iterator;

plclic:
    bool push_back(TElement* pE) {
        pair<iterator, bool> pr = m_Indexer.insert(value_type(pE->key, pE));
        return pr.second;
    }

    TElement* find(TKey k) {
        iterator it = m_Indexer.find(k);
        if (m_Indexer.end() != it) {
            return it->second;
        }
        return NULL;
    }

    bool remove(TElement* pE) {
        iterator it = m_Indexer.find(pE->key);
        if (m_Indexer.end() != it) {
            m_Indexer.erase(it);
            return true;
        }
        return false;
    }

private:
    Indexer m_Indexer;
};

#endif
