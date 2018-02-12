#ifndef _BSL_SLIST__
#define _BSL_SLIST__

#include <Lsc/alloc/Lsc_alloc.h>
#include <Lsc/alloc/Lsc_sample_alloc.h>
#include <Lsc/archive/Lsc_serialization.h>
#include <Lsc/exception/Lsc_exception.h>

#if 0
template <typename _Tp>
class alloc
{
plclic:
	typedef _Tp* pointer;	//空间地址指针
	typedef _Tp* const const_pointer;	//常空间地址指针
	typedef _Tp& reference;	//常空间地址指针
	_Tp * allocate(int n)
	{
		return new _Tp[n];
	}
	void deallocate(_Tp * _x )
	{
		delete [] _x;
	}	

};
#endif
namespace Lsc
{

/**
 */
template <typename _Tp, typename _Alloc>
struct slist_node_t
{
	typedef _Tp value_type;
	typedef slist_node_t<_Tp, _Alloc> node_t;
	typedef typename _Alloc::template rebind<node_t>::other node_alloc_t;
	typedef typename node_alloc_t::pointer node_pointer;
	node_pointer next;
	value_type val;
};

template <class _Tp, class _Alloc > class slist;

/**
 */
template<class _Tp, class _Ptr, class _Ref, class _Alloc>
class slist_const_iterator
{
private:
    typedef slist<_Tp, _Alloc> slist_t;
plclic:
	//typedef std::forward_iterator_tag iterator_category;
	typedef typename slist_t::value_type value_type;
	typedef typename slist_t::iterator iterator;
	typedef _Ptr pointer;
	typedef _Ref reference;
private:
	typedef slist_const_iterator<_Tp, _Ptr, _Ref, _Alloc> self;
	typedef slist_node_t<value_type, _Alloc> node_t;
    typedef typename _Alloc::template rebind<node_t>::other node_alloc_t;
	typedef typename node_alloc_t::pointer node_pointer;
    friend class slist<value_type, _Alloc>;
plclic:
	slist_const_iterator()
	{
		_node = 0;
	}
	slist_const_iterator(const self& __x)
	{
		_node = __x._node;
	}
	slist_const_iterator(const iterator & __x)
	{
		_node = __x._node;
	}
	reference operator *() const
	{
		return _node->val;
	}
	bool operator == (const self & __iter) const
	{
		return (_node == __iter._node);
	}
	bool operator != (const self & __iter) const
	{
		return (_node != __iter._node);
	}
	pointer operator ->() const
	{
		return &_node->val;
	}
	self & operator ++ ()
	{
		_node = _node->next;
		return *this;
	}
	self operator ++ (int)
	{
		self iter = *this;
		++ *this;
		return iter;
	}
private:
	slist_const_iterator(node_pointer __x)
	{
		_node = __x;
	}
plclic:
	node_pointer _node;
};

/**
 */
template<class _Tp, class _Ptr, class _Ref, class _Alloc>
class slist_iterator
{
private:
    typedef slist<_Tp, _Alloc> slist_t;
plclic:
    //typedef std::forward_iterator_tag iterator_category;
	typedef typename slist_t::value_type value_type;
	typedef typename slist_t::iterator iterator;
	typedef typename slist_t::const_iterator const_iterator;
	typedef _Ptr pointer;
	typedef _Ref reference;
private:
    typedef slist_iterator<_Tp, _Ptr, _Ref, _Alloc> self;
	typedef slist_node_t<value_type, _Alloc> node_t;
   	typedef typename _Alloc::template rebind<node_t>::other node_alloc_t;
	typedef typename node_alloc_t::pointer node_pointer;
    friend class slist<value_type, _Alloc>;
plclic:
    slist_iterator()
	{    
        _slist = 0;
		_node = 0;
	}
	slist_iterator(const self& __x)
	{
        _slist = __x._slist;
		_node = __x._node;
	}
	reference operator *() const
	{
		return _node->val;
	}
	bool operator == (const self & __iter) const
	{
		return (_node == __iter._node);
	}
	bool operator != (const self & __iter) const
	{
		return (_node != __iter._node);
	}
	pointer operator ->() const
	{
		return &_node->val;
	}
	self & operator ++ ()
	{
		_node = _node->next;
		return *this;
	}
	self operator ++ (int)
	{
		self iter = *this;
		++ *this;
		return iter;
	}
private:
    slist_iterator(slist_t* __l, node_pointer __x)
	{
        _slist = __l;
		_node = __x;
	}   
plclic:
    node_pointer _node;
private:
    slist_t *_slist; //归属的slist
};

/**
 */
template <class _Tp, class _Alloc = Lsc_sample_alloc<Lsc_alloc<_Tp>, 256> >
class slist
{
private:
	typedef slist<_Tp,_Alloc> self;
	typedef slist_node_t<_Tp, _Alloc> node_t;
	typedef typename _Alloc::template rebind<node_t>::other node_alloc_t;
plclic:
	typedef _Tp value_type;
	typedef _Tp *  pointer;
	typedef const _Tp * const_pointer;
	typedef _Tp & reference; 
	typedef const _Tp & const_reference; 
	typedef size_t size_type;
	typedef long difference_type;
	typedef slist_iterator<value_type, pointer, reference, _Alloc> iterator;
	typedef slist_const_iterator<value_type, const_pointer, 
                                    const_reference, _Alloc> const_iterator;
private:
	typedef typename node_alloc_t::pointer node_pointer;	
	//typedef typename node_alloc_t::const_pointer node_const_pointer;	
	//typedef typename node_alloc_t::reference node_reference; 
	//typedef typename node_alloc_t::const_reference node_const_reference; 
	//friend class slist_iterator<_Tp, _Alloc>;
	node_pointer _header;
	node_alloc_t _node_allocator;
    size_type _size;

plclic:
	/**
     * 无异常
     * 不需调create
	**/
	slist()
	{
        _reset();
        create();
	}

    /**
	 * 拷贝构造函数，抛异常
     * 如果拷贝失败，将恢复成原来的空slist
     * 不需调create
	**/
	slist( const self& other )
	{
		_reset();
        create();
        if (assign(other.begin(),other.end()) != 0) {
            throw BadAllocException()<<BSL_EARG<<"assign slist error";
        }
	}

	/**
	 * 赋值函数，抛异常
     * 如果插入失败，将恢复成原slist，强异常安全保证
     * 如果删除失败，从末尾删除结点，不会造成资源泄漏，基本异常安全保证
	**/
	self& operator = (const self& other)
	{
        if (this != &other) {
            iterator first1         = begin();
            iterator last1          = end();
            const_iterator first2   = other.begin();
            const_iterator last2    = other.end();
            size_t size1            = size();
            size_t size2            = other.size();
            if (size2 > size1) {
                for ( size_t i = 1; i < size1; ++ i ) {
                    ++ first1;
                    ++ first2;
                }
                // this不为空，则first2为需要插入的第一个结点
                if (size1) {
                    ++ first2;
                }
                iterator tmp = first1;
                while ( first2 != last2 ) {
                    if (insert_after( first1, *first2) != 0) {
                        erase_after( tmp, end() );
                        throw BadAllocException()<<BSL_EARG<<"insert_after slist error";
                    }
                    if ( 0 == first1._node ) { // 需要判断头结点是否为空
                        first1 = begin();
                    } else {
                        ++ first1;
                    }
                    ++ first2;
                }
                first1 = begin();
                first2 = other.begin();
            } else { // size1 >= size2
                for ( size_t i = size1; i > size2; -- i ) {
                    if ( pop_front() != 0 ) {
                        throw BadAllocException()<<BSL_EARG
                            <<"pop_front slist error size["<<size1<<"~"<<size2<<"]";
                    }
                }
            }
            while ( first1 != last1 && first2 != last2 ) {
                *first1 ++ = *first2 ++;
            }
        }
		return *this; 
	}
	/**
	 * 为容器类分配内存
	**/
	int create()
	{
		_node_allocator.create();
        return 0;
	}
	/**
	 * 释放容器类占用的内存
	**/
	int destroy()
    {
		this->clear();
		_node_allocator.destroy();
		return 0;

	}
	/**
	 * 析构函数
	**/
	~slist()
	{
		destroy();
	}
	/**
	 * 从另一个容器复制副本
	**/
	int assign (const self & __x)
	{
		if (&__x != this) {
			return assign(__x.begin(), __x.end());
        } else {
			return 0;
		}
	}

	/**
	 * 从迭代器复制副本
	**/
	template <typename __iterator>	
	int assign(__iterator __begin, __iterator __end)
	{
		node_pointer tmp_header;
		node_pointer cur = reinterpret_cast<node_pointer>(&tmp_header);
		__iterator tp = __begin;
        size_type n = 0;
		while(tp != __end)
		{
			node_pointer tmp = _node_allocator.allocate(1);
			if (!tmp) {
				goto err1;
            }
			Lsc::Lsc_construct(&tmp->val, *tp);
			cur->next = tmp;
			cur = tmp;
			++tp;
            ++n;
		}
		cur->next = 0;
		clear();
		_header = tmp_header;
        _size = n;
		return 0;
	err1:
		if(0 == tmp_header->next) {
			return -1;
        } else {
			tmp_header = tmp_header->next;
        }
		do {
			node_pointer tmp = tmp_header->next;
			Lsc::Lsc_destruct(&(tmp_header->val));
			_node_allocator.deallocate(tmp_header, 1);
			tmp_header = tmp;
		} while(cur != tmp_header);
		return -1;
	}

	/**
	 * 清除所有元素
	**/
	int clear()
	{
		while(_header)
		{
			pop_front();
		}
        _size = 0;
		return 0;
	}

	/**
	 * 检查容器是否为空
	**/
	inline bool empty() const
	{
		return _header == 0;
	}

	/**
	 * 将新元素加入到链表头
	**/
	int push_front(const value_type &__x)
	{
      	node_pointer tmp = _node_allocator.allocate(1);
		if (!tmp) {
			goto err;
        }
		Lsc::Lsc_construct(&tmp->val, __x);
		tmp->next = _header;
		_header = tmp;
        ++_size;
		return 0;
	err:
		return -1;
	}

	/**
	 * 删除链表的头元素
	**/
	int pop_front()
	{
		if ( 0 == _header) {
			return -1;
        }

		node_pointer tmp = _header->next;
		Lsc::Lsc_destruct(&(_header->val));
		_node_allocator.deallocate(_header, 1);
		_header = tmp;
        --_size;
		return 0;
	}

	/**
	 * 将元素插入到__i指向的位置之后，如果迭代器指向空节点，则插入到头节点。
	**/
	int insert_after( iterator __i,  const value_type &__x)
	{
		if( 0 == __i._node && _header != 0) {
			return -1;
        }

		node_pointer tmp = _node_allocator.allocate(1);
		if (!tmp) {
			goto err;
        }
		Lsc::Lsc_construct(&tmp->val, __x);
		if(__i._node)
		{
			tmp->next = __i._node->next;
			__i._node->next = tmp;
		}
		else
		{
			tmp->next = _header;
			_header = tmp;
		}
        __i._slist->_size += 1;
		return 0;
	err:
		return -1;
	}
	/**
	 * 删除__i指向的节点的下一个节点
	**/
	int erase_after ( iterator __i)
	{
		if( 0 == __i._node) {
			return -1;
        }
		node_pointer tmp = __i._node->next;
		__i._node->next = tmp->next;
		Lsc::Lsc_destruct(&(tmp->val));
		_node_allocator.deallocate(tmp, 1);
        __i._slist->_size -= 1;
		return 0;
	}
	/**
	 * 删除__s和__e之间的节点，区间为(__s,__e)
	**/
	int erase_after ( iterator __s, iterator __e)
	{
		if(__s._node == 0) {
			return -1;
        }
		node_pointer s = __s._node->next;
		node_pointer e = __e._node;
        size_t n = 0;
		while(s != e)
		{
			node_pointer tmp = s;
			s = s->next;
			Lsc::Lsc_destruct(&(tmp->val));
			_node_allocator.deallocate(tmp, 1);
            ++n;
		}
		__s._node->next = __e._node;
        __s._slist->_size -= n;
		return 0;
	}
	/**
	 * 返回头元素的常引用
	**/
	inline const_reference front() const
	{
		if( 0 == _header ) {
			throw Lsc::InvalidOperationException()<<BSL_EARG
                <<"no front item in empty list.";
        }
		return _header->val;
	}
	
	/**
	 * 返回头元素的引用
	**/
	inline reference front()
	{
		if( 0 == _header ) {
			throw Lsc::InvalidOperationException()<<BSL_EARG
                <<"no front item in empty list.";
        }
		return _header->val;
	}

	/**
	 * 返回指向头节点的常迭代器
	**/
	inline const_iterator begin() const
	{
		return const_iterator(_header);
	}

	/**
	 * 返回指向头节点的迭代器
	**/
	inline iterator begin()
	{
		return iterator(this, _header);
	}

	/**
	 * 返回指向尾节点的常迭代器
	**/
	inline const_iterator end() const
	{
		return const_iterator(0);
	}

	/**
	 * 返回指向尾节点的迭代器
	**/
	inline iterator end()
	{
		return iterator(this, 0);
	}
	/**
	 * 将链表反序排列
	**/
	int reverse()
	{
		node_pointer prev = 0;
		node_pointer cur = _header;
		while(cur != 0)
		{
			node_pointer tmp = cur->next;
			cur->next = prev;
			prev = cur;
			cur = tmp;
		}
		_header = prev;
		return 0;
	}
	
	/**
	 * 返回指向上一个节点的迭代器，复杂度O(n)
	**/
	iterator previous(iterator pos)
	{
		if(0 == pos._node) {
			return iterator(this, 0);
        }
		node_pointer tp = _header;
		if(tp == pos._node) {
			return iterator(this, 0);
        }
		while(tp != 0)
		{
			if(tp->next == pos._node) {
				return iterator(this, tp);
            } else {
				tp = tp->next;
            }
		}
		return iterator(this, 0);
	}
	
	/**
	 * 返回指向上一个节点的迭代器，复杂度O(n)
	**/
	const_iterator previous(const_iterator pos) const
	{
		if(0 == pos._node) {
			return const_iterator(0);
        }
		node_pointer tp = _header;
		while(tp->next != pos._node)
		{
			tp = tp->next;
		}
		return const_iterator(tp);
	}
	/**
	 * 返回容器的节点数量
	**/
	size_type size() const
	{
        return _size;
	}
	/**
	 * 返回容器所能容纳的最大节点数量
	**/
	inline size_type max_size() const
	{
		return (size_type)(-1);
	}
	/**
	 * 和__x容器交换内容
	**/
	int swap( self & __x)
	{
		//if(__x
		_node_allocator.swap(__x._node_allocator);
		std::swap(_header, __x._header);
        std::swap(_size, __x._size);
		//check();
		//__x.check();
		return 0;
	}
	/**
	 * 序列化
	**/
	template <typename _Archive>
	int serialization(_Archive &ar)
	{
		size_type l = size();
		if(Lsc::serialization(ar, l)) {
			goto err;
        }
		for(iterator i=begin(); i!=end(); i++)
		{
			if(Lsc::serialization(ar, *i)) {
				goto err;
            }
		}
		return 0;
	err:
		return -1;
	}

	/**
	 * 反序列化
	**/
	template <typename _Archive>
	int deserialization(_Archive &ar)
	{
		node_pointer cur;
		size_type l;
		clear();
		if(Lsc::deserialization(ar, l)) {
			goto err;
        }
		cur = reinterpret_cast<node_pointer>(&_header);
		for(size_type i=0; i<l; i++)
		{
			value_type tmp;
			if(Lsc::deserialization(ar, tmp)) {
				goto err;
            }
			if(insert_after(iterator(this, cur), tmp)) {
				goto err2;
            }
			cur = cur->next;
		}
		return 0;
	err:
	err2:
		clear();
		return -1;

	}
#define __BSL_LIST_STACK_DEPTH 64
	/**
	 * 按递增顺序排序
		tmp->val = __x;
	**/
	inline int sort()
	{
		return sort(std::less<_Tp>());
	}
	/**
	 * 按Predicate指定的顺序排序
	**/
	template <typename Predicate>	
	int sort(Predicate _p)
	{
		if( 0 == _header) {
			return 0;
        }
		node_pointer cur = _header;
		node_pointer counter[__BSL_LIST_STACK_DEPTH];
		for(int i=0; i<__BSL_LIST_STACK_DEPTH ; i++)
		{
			counter[i] = 0;
		}
	//	int fill=0;
		while( cur != 0)
		{
			node_pointer tmp = cur;
			cur = cur->next;
			tmp->next = 0;
			int i = 0;
			while(counter[i] != 0)
			{
				tmp = _merge(tmp,counter[i], _p);
				counter[i]=0;
				i++;
			}
			counter[i] = tmp;
	//		if(i > fill)
	//			fill = i;
		}
		for(int i = 1; i < __BSL_LIST_STACK_DEPTH ; i++)
		{
			counter[i] = _merge(counter[i-1], counter[i], _p);
		}
		_header = counter[__BSL_LIST_STACK_DEPTH - 1];
		return 0;
	}
	
	/**
	 * 将两个已排序的链表合并，合并后，__other将为空 
	**/
	int merge(self & __other)
	{
		return merge(__other, std::less<_Tp>());
	}

	/**
	 * 将两个已排序的链表合并，合并后，__other将为空
	**/
	template <typename _Predicator>
	int merge(self & __other, _Predicator __p)
	{
		if(_header != 0 && __other._header != 0)
		{
            size_type other_size = __other.size();
			_node_allocator.merge(__other._node_allocator);
			_header = _merge(_header, __other._header, __p);
			__other._header = 0;
            _size += other_size;
            __other._size = 0;
			return 0;
		}
		else if(__other._header != 0)
		{
			return swap(__other);
		}
		else
		{
			return 0;
		}
	}
	/**
	 * 从链表中删除与__x值相同的实例
	**/
	int remove(const value_type & __x)
	{
		return remove_if(std::bind2nd(std::equal_to<_Tp>(), __x));
	}
	/**
	 * 根据条件函数删除元素
	**/
	template<class Predicate> 
	int remove_if(Predicate __p) 
	{
		//为了避免对头节点特殊处理，这里强制将_header看作特殊的节点。
		node_pointer cur = reinterpret_cast<node_pointer>(&_header);
		while(cur->next != 0)
		{
			if(__p(cur->next->val))
			{
				node_pointer tmp = cur->next;
				cur->next = tmp->next;
				Lsc::Lsc_destruct(&(tmp->val));
				_node_allocator.deallocate(tmp, 1);
                --_size;
			}
			else
			{
				cur = cur->next;
			}
		}
		return 0;
	}
private:

    void _reset(){
        _header = 0;
        _size = 0;
    }

	template <typename _Predicate>
	inline node_pointer _merge(node_pointer list1, node_pointer list2, const _Predicate &_p)
	{
		if(0 == list2) {
			return list1;
        }
		if(0 == list1) {
			return list2;
        }
		volatile node_pointer tmp_root;
		//为了避免对头节点特殊处理，这里强制将_header看作特殊的节点。
		node_pointer tp = (node_pointer)(&tmp_root);
		node_pointer a = list1;
		node_pointer b = list2;
		while(true)
		{
			if(_p(a->val,b->val))
			{
				tp->next = a;
				if(a->next != 0)
				{
					tp = a;
					a = a->next;
				}
				else
				{
					a->next = b;
					return tmp_root;
				}

			}
			else
			{
				tp->next = b;
				if(b->next != 0)
				{
					tp = b;
					b = b->next;
				}
				else
				{
					b->next = a;
					return tmp_root;
				}
			}
		}
	}
};

}

#endif
