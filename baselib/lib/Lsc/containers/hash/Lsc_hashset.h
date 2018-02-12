

#ifndef  __BSL_HASHSET_H_
#define  __BSL_HASHSET_H_


#include <Lsc/containers/hash/Lsc_hashtable.h>

namespace Lsc
{

/**
 */
template <class _Key, /*作为hashkey的类型*/
		 /**
		  * hash 函数的仿函数，比如
		  * struct xhash {
		  * 	inline size_t operator () (const _Key &_1);
		  * };
		  **/
		 class _HashFun = xhash<_Key>,
		 /**
		  * 判断两个key相等的仿函数
		  * 比如 struct equal {
		  * 	inline bool operator () (const _Tp &_1, const _Tp &_2);
		  * };
		  */
		 class _Equl = std::equal_to<_Key>,	
		 /**
		  * 空间分配器，默认的空间分配器能够高效率的管理小内存，防止内存碎片
		  * 但是在容器生命骑内不会释放申请的内存
		  *
		  * Lsc_alloc<_Key>做内存分配器，可以在容器生命期内释放内存，
		  * 但是不能有效防止内存碎片
		  */
		 class _InnerAlloc = Lsc_sample_alloc<Lsc_alloc<_Key>, 256>
		>
class hashset
{
    /**
     */
	typedef _Key key_type;
    /**
     */
	typedef _Key value_type;
    /**
     */
	typedef hashset<_Key, _HashFun, _Equl, _InnerAlloc> _Self;
	/**
     */
    typedef Lsc_hashtable<_Key, _Key, _HashFun, _Equl,  param_select<_Key>, _InnerAlloc> hash_type;
	/**
     */
    hash_type _ht;
plclic:
    /**
     */
	typedef typename hash_type::iterator iterator;
    /**
     */
	typedef typename hash_type::const_iterator const_iterator;

plclic:
    /**
     * 无异常
     * 需调create
     **/
    hashset() {}

    /**
     * 失败会抛出异常
     * 不需调create
     **/
    hashset(size_t bitems, const _HashFun& hf = _HashFun(), const _Equl& eq = _Equl())
        :_ht(bitems, hf, eq){}

    /**
     * 失败会抛出异常
     */
    hashset(const _Self& other): _ht(other._ht){}

    /**
     * 失败抛出异常
     **/
    _Self& operator = (const _Self& other) { 
        _ht = other._ht;
        return *this;
    }

	/**
	 *
	**/
	iterator begin() {
		return _ht.begin(); 
	}

    /**
     */
	const_iterator begin() const {
		return _ht.begin();
	}

	/**
	 *
	**/
	iterator end() { 
		return _ht.end(); 
	}
    /**
     */
	const_iterator end() const {
		return _ht.end();
	}

	/**
	 *
	**/
	size_t size() const { 
		return _ht.size(); 
	}

	/**
	 *
	 * 				返回	其他	表示创建失败
	**/
	int create(size_t bitems, const _HashFun &hf = _HashFun(), const _Equl & eq = _Equl()) {
		return _ht.create(bitems, hf, eq);
	}

    /* *
     * */
    bool is_created() const{
        return _ht.is_created();
    }

	/**
	 *
	**/
	int destroy() {
		return _ht.destroy();
	}

	/**
	 *
	 *				返回 HASH_EXIST		表示hash值存在
	 *				返回 HASH_NOEXIST	表示hash值不存在
	**/
	int get(const key_type &k) const {
		//返回的是指针
		if (_ht.find(_ht._hashfun(k), k) == NULL) {
            return HASH_NOEXIST;
        }
		return HASH_EXIST;
	}

    /**
     */
	int get(const key_type &k) {
		//返回的是指针
		if (_ht.find(_ht._hashfun(k), k) == NULL) {
            return HASH_NOEXIST;
        }
		return HASH_EXIST;
	}

	/**
	 *
	 * 			返回	-1表示set调用出错
	 * 			返回	HASH_EXIST	表示hash结点存在
	 * 			返回	HASH_INSERT_SEC	表示插入成功
	**/
	int set(const key_type &k) {
		return  _ht.set(_ht._hashfun(k), k, k);
	}

	/**
	 *
	 * 		返回	HASH_EXIST表示结点存在并删除成功
	 * 		返回	HASH_NOEXIST表示结点不存在不用删除
	**/
	int erase(const key_type &k) {
		return _ht.erase(_ht._hashfun(k), k);
	}

	/**
	 *
	**/
	template <class _Iterator>
	int assign(_Iterator __begin, _Iterator __end) {
		return _ht.assign(__begin, __end);
	}

    /**
     */
	template <class _Archive>
	int serialization(_Archive &ar) {
		return Lsc::serialization(ar, _ht);
	}

    /**
     */
	template <class _Archive>
	int deserialization(_Archive &ar) {
		return Lsc::deserialization(ar, _ht);
	}
    /**
     */
	int clear() {
		return _ht.clear();
	}
};
}
#endif  //__HASHSET_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
