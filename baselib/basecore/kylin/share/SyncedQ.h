
#ifndef _SYNCED_Q_H_
#define _SYNCED_Q_H_

#include "SyncObj.h"

template <typename TElement, typename TQueue, typename TLock=CSpinLock>
class TLockedQueue
{
plclic:
	typedef typename TQueue::ENUMERATOR ENUMERATOR;
	TLockedQueue() { }
	~TLockedQueue() { }
	
	void Init() {
		m_queue.Init();
	}
	
	int push_back(TElement* e) {
        int n;
		m_lock.Lock();
		n = m_queue.push_back(e);
		m_lock.Unlock();
        return n;
	}
	
	int push_front(TElement* e) {
        int n;
		m_lock.Lock();
		n = m_queue.push_front(e);
		m_lock.Unlock();
        return n;
	}
	
	TElement* pop_front() {
		TElement* e;
		
		m_lock.Lock();
		e = m_queue.pop_front();
		m_lock.Unlock();
		return e;
	}
	
	TElement* pop_back() {
		TElement* e;
		
		m_lock.Lock();
		e = m_queue.pop_back();
		m_lock.Unlock();
		return e;
	}
	
	TElement* get_back() {
		TElement* e;

		m_lock.Lock();
		e = m_queue.get_back();
		m_lock.Unlock();
		return e;
	}

	TElement* get_front() {
		TElement* e;

		m_lock.Lock();
		e = m_queue.get_front();
		m_lock.Unlock();
		return e;
	}

	void Enumerate(ENUMERATOR f, ptr pData) {
		m_lock.Lock();
		m_queue.Enumerate(f, pData);
		m_lock.Unlock();
	}

	int remove(TElement* p) {
        int n;
		m_lock.Lock();
		n = m_queue.remove(p);
		m_lock.Unlock();
        return n;
	}

	int size() {
		int n;
		m_lock.Lock();
		n = m_queue.size();
		m_lock.Unlock();
		return n;
	}
	
private:
	TQueue m_queue;
	TLock m_lock;
};


template <typename TElement, typename TQueue, typename TLock=CSpinLock>
class TSyncedQueue
{
plclic:
	TSyncedQueue() { }
	~TSyncedQueue() { }
	
	void Init() {
		m_queue.Init();
		m_lock.Init();
		m_sema.Init(0);
	}
	
	void push_back(TElement* e) {
        

		m_lock.Lock();
		m_queue.push_back(e);
		m_lock.Unlock();
		
		m_sema.Up();
	}
	
	void push_front(TElement* e) {
        

		m_lock.Lock();
		m_queue.push_front(e);
		m_lock.Unlock();
		
		m_sema.Up();
	}
	
	TElement* pop_front() {
		TElement* e;
		
		m_sema.Down();
		m_lock.Lock();
		e = m_queue.pop_front();
		m_lock.Unlock();
		
		ASSERT_NOT_EQUAL((TElement*)NULL, e);
        
		return e;
	}
	
	TElement* pop_back() {
		TElement* e;
		
		m_sema.Down();
		m_lock.Lock();
		e = m_queue.pop_back();
		m_lock.Unlock();
		
		ASSERT_NOT_EQUAL((TElement*)NULL, e);
        
		return e;
	}

    bool replace(TElement* pOld, TElement* pNew) {
        bool b;

        m_lock.Lock();
        b = m_queue.replace(pOld, pNew);
        m_lock.Unlock();
        return b;
    }
	
	int size() {
		int n;
		m_lock.Lock();
		n = m_queue.size();
		m_lock.Unlock();
		return n;
	}

	void PrintState() { m_sema.PrintState(); }
	
private:
	TQueue m_queue;
	TLock m_lock;
	CSema m_sema;
};

#endif
