


#ifndef  __BSL_ADJUSTCACHEDPOOL_H_
#define  __BSL_ADJUSTCACHEDPOOL_H_

#include "Lsc_pool.h"

namespace Lsc
{
class cached_mempool_append : plclic mempool
{
private:
    unsigned int _seg_size;
    unsigned int _seg_num;
    unsigned int _seg_usingnum;
    void * _head;
    unsigned int _head_len;
    void * _curr_seg;
    void * _pre_seg;
    char * _free_space;
    unsigned int _buff_freesize;

    void * _bigbuf_head;

    int _outer_tag;
plclic:
    inline cached_mempool_append()
    {
        _seg_size = 0;
        _seg_num = 0;
	    _seg_usingnum = 0;
        _head = 0;
        _head_len = 0;
        _free_space = 0;
	    _curr_seg = 0;
        _pre_seg = 0;
	    _buff_freesize = 0;
        
	    _bigbuf_head = 0;
	    _outer_tag = 0;

    }

    /**
     *
    **/
    inline int create (int init_size, int seg_size)
    {
        destroy();
        if (init_size <= 0 || seg_size <= 0) {
	        return -1;
	    }

		if (NULL != _head) {
		    return -2;
		}
		_head = ::malloc(init_size + sizeof(void*));
		if (NULL == _head) {
		    return -3;
		}
		*(void **)_head = NULL;
        _curr_seg = _head;
        _pre_seg = _head;
		_head_len = init_size + sizeof(void*);
		_seg_size = seg_size + sizeof(void*);

		_free_space = (char *)_head + sizeof(void*);
		_buff_freesize = _head_len - sizeof(void *);
        _seg_num = 1;
        _seg_usingnum = 0;
		return 0;
	}

	inline int create(void *head, int head_size, int seg_size)
	{
        destroy();
		if (NULL == head || head_size <= (int)sizeof(void *) || seg_size <= 0) {
		    return -1;
		}
		_head = head;
		*(void **)_head = NULL;
        _curr_seg = _head;
        _pre_seg = _head;
		_head_len = head_size;
		_seg_size = seg_size + sizeof(void *);

		_free_space = (char *)_head + sizeof(void *);
		_buff_freesize = _head_len - sizeof(void *);
        _seg_num = 1;
        _seg_usingnum = 0;

		_outer_tag = 1;
		return 0;
    }

    virtual void * malloc(size_t size);

    virtual inline void free(void *, size_t)
    {
    }

    inline void clear()
    {
        while(_bigbuf_head)
        {
            void * tmp = *(void **)_bigbuf_head;
            ::free(_bigbuf_head);
            _bigbuf_head = tmp;
        }
	    _seg_usingnum = 0;
	    _free_space = (char *)_head + sizeof(void*);
	    _buff_freesize = _head_len - sizeof(void*);
	    _curr_seg = _head;
        _pre_seg = _head;
    }

    inline void destroy()
    {    
        while(_bigbuf_head)
        {
            void * tmp = *(void **)_bigbuf_head;
            ::free(_bigbuf_head);
            _bigbuf_head = tmp;
        }

	    void *tmp = 0;
	    if (0 == _outer_tag) {
	        tmp = _head;
	    } else {
	        tmp = *(void **)_head;
	    }
	    while (tmp) {
	        void *tmp1 = *(void **)tmp;
	        ::free(tmp);
	        tmp = tmp1;
	    }
	    _head = 0;
        _outer_tag = 0;

    }

    inline virtual ~cached_mempool_append()
    {
        destroy();
    }
};
}















#endif  //__BSL_ADJUSTCACHEDPOOL_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
