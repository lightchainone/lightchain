#ifndef __HASH_TABLE_H__
#define __HASH_TABLE_H__

#include <iterator>
#include "link.h"
#include "log.h"




template <typename Hash, typename T = typename Hash::item_type>
class hash_const_iterator
{
plclic:
    typedef ptrdiff_t                     difference_type;
    typedef std::output_iterator_tag                        iterator_category;
    typedef T                                   value_type;
    typedef const T*                        pointer;
    typedef const T&                        reference;

    hash_const_iterator(const Hash &ref, DLINK *head, bool oldslot):
    _hash(ref), _head(head), _curr(head)
    {
        _pos = 0;
        _in_oldslot = oldslot;
        if (_head)                                                      
        {
            operator++();
        }
    }

    hash_const_iterator(const hash_const_iterator &lfs):
    _hash(lfs._hash), _head(lfs._head), _curr(lfs._curr)
    {
        _pos = lfs._pos;
        _in_oldslot = lfs._in_oldslot;
    }

    virtual  ~hash_const_iterator()
    {
    }


    const T &operator*()
    {
        return *_hash._get_item(_curr);
    }

    const T *operator->()
    {
        return _hash._get_item(_curr);
    }

    hash_const_iterator &operator++()
    {
        if (_curr->_next != _head)
        {
            _curr = _curr->_next;
            return *this;
        }
        if (_in_oldslot)                                                
        {
            if (_pos >= _hash._hashnum)
            {
                _head = NULL;                                           
                _curr = NULL;
                _pos = 0;
                return *this;
            }
        }
        retry:      unsigned int hashnum;
        DLINK *hashslots;
        if (!_in_oldslot)                                               
        {
            hashnum = _hash._new_hashnum;
            hashslots = _hash._new_hashslots;
        }
        else
        {
            hashnum = _hash._hashnum;
            hashslots = _hash._hashslots;
        }
        _curr = _curr->_next;
        while (_curr == _head)
        {
            ++_pos;
            if (_pos < hashnum)
            {
                _head = &hashslots[_pos];
                _curr = _head->_next;
            }
            else
            {
                if (!_in_oldslot)
                {
                    
                    _pos = _hash._expand_slots - 1;
                    _in_oldslot = !_in_oldslot;
                    goto retry;
                }
                else
                {
                    _head = NULL;
                    _curr = NULL;
                    _pos = 0;
                    return *this;
                }
            }
        }
        return *this;
    }

    hash_const_iterator operator++(int)
    {
        hash_const_iterator temp(*this);
        operator++();
        return temp;
    }

    bool operator==(const hash_const_iterator &rhs)
    {
        return(&rhs._hash == &_hash &&
               rhs._pos == _pos &&
               rhs._curr == _curr &&
               rhs._head == _head);
    }

    bool operator!=(const hash_const_iterator &rhs)
    {
        return !operator==(rhs);
    }

private:
    const Hash &_hash;
    DLINK *_head;
    DLINK *_curr;
    unsigned int _pos;
    bool _in_oldslot;
};



template <typename Hash, typename T = typename Hash::item_type>
class hash_iterator : plclic hash_const_iterator<Hash, T>
{
plclic:
    hash_iterator(const Hash &ref, DLINK *head, bool oldslot):
    hash_const_iterator<Hash, T>(ref, head, oldslot)
    {
    }

    hash_iterator(const hash_iterator &lfs):
    hash_const_iterator<Hash, T>(lfs)
    {
    }

    ~hash_iterator()
    {
    }


    T &operator*()
    {
        return const_cast<T &>(hash_const_iterator<Hash, T>::operator *());
    }

    T *operator->()
    {
        return const_cast<T *>(hash_const_iterator<Hash, T>::operator->());

    }

    hash_iterator &operator++()
    {
        hash_const_iterator<Hash, T>::operator++();
        return *this;
    }

    hash_iterator operator++(int)
    {
        hash_iterator temp(*this);
        operator++();
        return temp;
    }

    bool operator==(const hash_iterator &rhs)
    {
        return hash_const_iterator<Hash, T>::operator==(rhs);
    }

    bool operator!=(const hash_iterator &rhs)
    {
        return !operator==(rhs);
    }
};



template <typename Item, typename Key>
class HashItemOperation
{
plclic:
    typedef Item item_type;
    typedef Key key_type;

plclic:
    HashItemOperation()
    {
    }
    virtual ~HashItemOperation()
    {
    }
    virtual unsigned int GetHash(const Item *item) const = 0;
    virtual unsigned int GetHashOfKey(const Key *key) const = 0;
    virtual bool IsEqual(const Item *item, const Key *key) const = 0;
    virtual DLINK *GetList(Item *item) = 0;
    virtual Item *GetItem(DLINK *list) = 0;
};

template <typename THashItemOP,
    typename Item = typename THashItemOP::item_type,
    typename Key = typename THashItemOP::key_type>
class HashTable
{
plclic:
    typedef Item  item_type;
    typedef Key key_type;

    template <typename Hash, typename T>
    friend class hash_const_iterator;
    typedef hash_const_iterator<HashTable<THashItemOP, Item, Key> > const_iterator;


    template <typename Hash, typename T>
    friend class hash_iterator;
    typedef hash_iterator<HashTable<THashItemOP, Item, Key> > iterator;

plclic:
    HashTable()
    {
        memset(this,0,sizeof(*this));
    }

    
    ~HashTable()
    {
        free(_hashslots);
        free(_new_hashslots);
    }

    int Init(unsigned int num,float factor=1.5)
    {
        if ( factor < 1.0)
        {
            WARNING_LOG("param error,[factor:%f]",factor);
            return -1;
        }
        if (num <= 0)
        {
            WARNING_LOG("num error,[num:%u]", num);
            return -1;
        }
        if (_hashslots!=NULL)
        {
            free(_hashslots);
        }
        if (_new_hashslots!=NULL)
        {
            free(_new_hashslots);
        }

        memset(this,0,sizeof(*this));
        if (num>0)
        {
            _hashslots = (DLINK *)calloc(num,sizeof(DLINK));
            if (_hashslots == NULL)
            {
                WARNING_LOG("new _hashslots error,[num:%u]",num);
                return -1;
            }
            for (unsigned int i = 0; i < num; i++)
            {
                DLINK_INITIALIZE(&_hashslots[i]);
            }
            _hashnum = num;
            _factor = factor;
            _expand_itemnum = (unsigned int)(num*factor)+1;
        }
        return 0;
    }

    int Put(Item *item)                                                 
    {
        if (item == NULL)
        {
            WARNING_LOG("item null");
            return -1;
        }
        if (_hashnum==0)
        {
            WARNING_LOG("_hashnum==0");
            return -1;
        }
        THashItemOP itemop;
        unsigned int hv = itemop.GetHash(item);
        return Put(item, hv);
    }

    
    int Put(Item *item, unsigned int hv)                                
    {
        THashItemOP itemop;
        unsigned int slot = hv%_hashnum;
        DLINK *slot_head = NULL;
        DLINK *item_indx = itemop.GetList(item);
        if (_new_hashslots != NULL)
        {
            if (slot < _expand_slots)                                   
            {
                slot_head = &_new_hashslots[hv%_new_hashnum];
            }
            else
            {
                slot_head = &_hashslots[slot];
            }
        }
        else
        {
            slot_head = &_hashslots[slot];
        }
        if (slot_head->_next == NULL)
        {
            slot_head->_next = slot_head;
        }
        if (slot_head->_prev == NULL)
        {
            slot_head->_prev = slot_head;
        }
        
        DLINK_INSERT_NEXT(slot_head,item_indx);
        _itemnum++;
        _move_to_new();
        return 0;
    }

    Item *Find(const Key *key) const
    {
        if (key == NULL)
        {
            WARNING_LOG("item null");
            return NULL;
        }
        if (_hashnum==0)
        {
            WARNING_LOG("_hashnum==0");
            return NULL;
        }
        THashItemOP itemop;
        unsigned int hv = itemop.GetHashOfKey(key);
        return Find(key,hv);
    }

    
    Item *Find(const Key *key,unsigned int hv) const
    {
        if (key == NULL)
        {
            WARNING_LOG("item null");
            return NULL;
        }
        if (_hashnum==0)
        {
            WARNING_LOG("_hashnum==0");
            return NULL;
        }
        THashItemOP itemop;
        unsigned int slot = hv%_hashnum;
        DLINK *slot_head = NULL;
        DLINK *item_indx = NULL;
        Item *item = NULL;
        if (_new_hashslots != NULL)
        {
            if (slot < _expand_slots)                                   
            {
                slot_head = &_new_hashslots[hv%_new_hashnum];
            }
            else
            {
                slot_head = &_hashslots[slot];
            }
        }
        else
        {
            slot_head = &_hashslots[slot];
        }
        if (slot_head->_next == NULL)
        {
            slot_head->_next = slot_head;
        }
        if (slot_head->_prev == NULL)
        {
            slot_head->_prev = slot_head;
        }
        item_indx = slot_head->_next;
        while (item_indx != slot_head)
        {
            item = itemop.GetItem(item_indx);
            if (itemop.IsEqual(item,key))
            {
                return item;
            }
            item_indx = item_indx->_next;
        }
        return NULL;
    }

    int Remove(Item *item)
    {
        if (item == NULL)
        {
            WARNING_LOG("item null");
            return -1;
        }
        if (_hashnum==0)
        {
            WARNING_LOG("_hashnum==0");
            return -1;
        }
        THashItemOP itemop;
        DLINK *item_indx = itemop.GetList(item);
        DLINK_REMOVE(item_indx);
        _itemnum--;
        return 0;
    }

    unsigned int ItemNum() const
    {
        return _itemnum;
    }

    const_iterator Begin() const
    {
        return const_cast<HashTable *>(this)->Begin();
    }

    iterator Begin()
    {
        if (_hashnum==0)
        {
            return iterator(*this, NULL, true);
        }
        DLINK *head;
        bool in_oldslot = true;
        if (_new_hashslots == NULL)                                     
        {
            head = &_hashslots[0];
        }
        else                                                            
        {
            
            head = &_new_hashslots[0];
            in_oldslot = false;
        }
        return iterator(*this, head, in_oldslot);
    }

    const_iterator End() const
    {
        return const_cast<HashTable *>(this)->End();
    }

    iterator End()
    {
        return iterator(*this, NULL, true);
    }

private:
    float _factor;

    DLINK *_hashslots;
    DLINK *_new_hashslots;
    unsigned int _hashnum;
    unsigned int _new_hashnum;
    unsigned int _itemnum;
    unsigned int _expand_itemnum;
    unsigned int _expand_slots;                                         

private:
    void _move_to_new()
    {
        
        if (_new_hashslots == NULL && _itemnum >= _expand_itemnum)
        {
            
            _new_hashslots = (DLINK *)calloc(_hashnum*2,sizeof(DLINK));
            if (_new_hashslots == NULL)
            {
                WARNING_LOG("new _new_hashslots error,[newhashnum:%u]",_hashnum*2);
                
                return;
            }
            _new_hashnum = _hashnum*2;
            _expand_slots = 0;
            for (unsigned int i = 0; i < _new_hashnum; i++)
            {
                DLINK_INITIALIZE(&_new_hashslots[i]);
            }
        }
        if (_new_hashslots != NULL)
        {
            THashItemOP itemop;
            DLINK *slot_head = &_hashslots[_expand_slots];
            if (slot_head->_next == NULL)
            {
                slot_head->_next = slot_head;
            }
            if (slot_head->_prev == NULL)
            {
                slot_head->_prev = slot_head;
            }
            DLINK *new_slot_head = NULL;
            DLINK *item_indx = NULL;
            Item *item = NULL;
            unsigned int hv = 0;
            while (!DLINK_IS_EMPTY(slot_head))
            {
                
                item_indx = slot_head->_next;
                item = itemop.GetItem(item_indx);
                DLINK_REMOVE(item_indx);
                hv = itemop.GetHash(item);
                new_slot_head = &_new_hashslots[hv%_new_hashnum];
                if (new_slot_head->_next == NULL)
                {
                    new_slot_head->_next = new_slot_head;
                }
                if (new_slot_head->_prev == NULL)
                {
                    new_slot_head->_prev = new_slot_head;
                }
                DLINK_INSERT_NEXT(new_slot_head,item_indx);
            }
            _expand_slots++;
            if (_expand_slots == _hashnum)
            {
                free(_hashslots);
                _hashslots = _new_hashslots;
                _hashnum = _new_hashnum;
                _new_hashnum = 0;
                _new_hashslots = NULL;
                _expand_slots = 0;
                _expand_itemnum = (unsigned int)(_hashnum*_factor)+1;
            }
        }
    }

    Item *_get_item(DLINK *list) const
    {
        THashItemOP itemop;
        return itemop.GetItem(list);
    }
};

#endif
