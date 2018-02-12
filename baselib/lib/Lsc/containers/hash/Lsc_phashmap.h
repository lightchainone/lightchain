

#ifndef  __BSL_PHASHMAP_H_
#define  __BSL_PHASHMAP_H_

#include <Lsc/containers/hash/Lsc_phashtable.h>

namespace Lsc
{

template <class _Key, /*作为hashkey的类型*/
		 class _Value,	/*作为hash value的类型*/
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
class phashmap : plclic Lsc_phashtable<_Key, std::pair<_Key, _Value>, _HashFun, _Equl, 
	Lsc::pair_first<std::pair<_Key, _Value> >, _InnerAlloc>
{
plclic:
	typedef std::pair<_Key, _Value> _Pair;
	typedef phashmap<_Key, _Value, _HashFun, _Equl, _InnerAlloc> _Self;
	typedef Lsc_phashtable<_Key, _Pair, _HashFun, _Equl, Lsc::pair_first<_Pair>, _InnerAlloc> _Base;
	typedef typename _Base::iterator iterator;
	typedef typename _Base::const_iterator const_iterator;

	typedef _Value value_type;
	typedef _Key key_type;

private:
    /* * 
     **/
    phashmap(const _Self &);            //禁用
    /* *
     * */
	_Self & operator = (const _Self &); //禁用

plclic:

    /**
     * 无异常
     * 需调用create才能用
     **/
    phashmap(){}

    /**
     *  如果失败抛异常
     *  不需调用create
     *
     **/
    phashmap(size_t bitems, size_t rwlsize = 0,
            const _HashFun &hf = _HashFun(),
            const _Equl &eq = _Equl()):
        _Base(bitems, rwlsize, hf, eq){
    }

	/**
	 * create方法参考phashtable的方法,直接继承
	 * destroy方法也直接继承
	 */


	/**
	 *
	 * 				如果value不为空，将*val附值查找的值
	 *				返回 HASH_EXIST		表示hash值存在
	 *				返回 HASH_NOEXIST	表示hash值不存在
	**/
	int get(const key_type &k, value_type *val = 0) const {
		return _Base::get_map(k, val);
	}
	int get(const key_type &k, value_type *val = 0) {
		return _Base::get_map(k, val);
	}
    
    /**
     * 若不需要原值，传入NULL指针
     * _ValOpFunc可以是函数指针，也可以是functor，参数类型为(_Second*, void*)
	 **/
    template <typename _ValOpFunc> 
    int get(const key_type &k, value_type *old_val, _ValOpFunc val_op_func, void *args = 0) {
		return _Base::get_map(k, old_val, val_op_func, args);
	}

	/**
	 *
	 * 				flag ＝＝0，如果值存在直接返回，
	 * 				非0表示，如果值存在，替换旧值
	 *  			返回	-1	表示set调用出错, (无法分配新结点)
	 * 			其他均表示插入成功：插入成功分下面三个状态
	 * 				返回	HASH_OVERWRITE	表示覆盖旧结点成功(在flag非0的时候返回)
	 * 				返回	HASH_INSERT_SEC	表示插入新结点成功
	 * 				返回	HASH_EXIST	表示hash表结点存在(在flag为0的时候返回)
	**/
	int set(const key_type &k, const value_type &val, int flag = 0) {
		return _Base::set(k, _Pair(k, val), flag);
	}

	/**
	 *
	 * 		返回	HASH_EXIST表示结点存在并删除成功
	 * 		返回	HASH_NOEXIST表示结点不存在不用删除
	**/
	int erase(const key_type &k) {
		return _Base::erase(k);
	}
};

};


#endif  //__PHASHMAP_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
