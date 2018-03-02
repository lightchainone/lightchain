#ifndef __LINK_H__
#define __LINK_H__


typedef struct _SLINK{
   _SLINK* _next;
} SLINK, *PSLINK;


#define SLINK_INITIALIZE(_head)           ((_head)->_next = NULL)


#define SLINK_IS_EMPTY(_head)             ((_head)->_next == NULL)


#define SLINK_POP(_head)                  (_head)->_next;\
                                          (_head)->_next =  (_head)->_next->_next;

#define SLINK_PUSH(_head,_link)           (_link)->_next =  (_head)->_next;\
                                          (_head)->_next =  (_link)


typedef struct _DLINK{
   _DLINK* _prev;
   _DLINK* _next;
} DLINK, *PDLINK;


#define DLINK_INITIALIZE(_head)            ((_head)->_next = (_head)->_prev = (_head))


#define DLINK_IS_EMPTY(_head)              ((_head)->_next == (_head))


#define DLINK_INSERT_NEXT(_head,_dlink)    (_dlink)->_next = (_head)->_next;		\
                                           (_dlink)->_prev = (_head);				\
                                           (_head)->_next->_prev = (_dlink);		\
                                           (_head)->_next = (_dlink)


#define DLINK_INSERT_PREV(_head,_dlink)    (_dlink)->_prev = (_head)->_prev;		\
                                           (_dlink)->_next = (_head);				\
                                           (_head)->_prev->_next = (_dlink);		\
                                           (_head)->_prev = (_dlink)

#define DLINK_REMOVE(_dlink)               (_dlink)->_prev->_next = (_dlink)->_next;\
                                           (_dlink)->_next->_prev = (_dlink)->_prev

#define DLINK_EXTRUCT_PREV(_head)          (_head)->_prev;							\
                                           DLINK_REMOVE((_head)->_prev)

#define DLINK_EXTRUCT_NEXT(_head)           (_head)->_next;							\
                                            DLINK_REMOVE((_head)->_next)


#define DLINK_REPLACE(_old, _new)           (_old)->_next->_prev = (_new);	\
                                            (_old)->_prev->_next = (_new);	\
                                            (_new)->_next = (_old)->_next;	\
                                            (_new)->_prev = (_old)->_prev

#ifdef _DEBUG
#if defined(__x86_64__)
#define INVALID_POINTER                     (DLINK*)0xCCCCCCCCCCCCCCCCULL
#else
#define INVALID_POINTER                     (DLINK*)0xCCCCCCCCUL
#endif

#define DLINK_IS_STANDALONE(_dlink)         (((_dlink)==(_dlink)->_next                 \
                                                && (_dlink)==(_dlink)->_prev)           \
                                            || (NULL==(_dlink)->_next                   \
                                                && NULL==(_dlink)->_prev)               \
                                            || (INVALID_POINTER==(_dlink)->_next        \
                                                && INVALID_POINTER==(_dlink)->_prev)    \
                                            )
#define PTR_IS_NULL(_ptr)                   (NULL==(_ptr) || INVALID_POINTER==(DLINK*)(_ptr))

#else
#define DLINK_IS_STANDALONE(_dlink)         (((_dlink)==(_dlink)->_next                 \
                                                && (_dlink)==(_dlink)->_prev)           \
                                            || (NULL==(_dlink)->_next                   \
                                                && NULL==(_dlink)->_prev)               \
                                            )
#define PTR_IS_NULL(_ptr)                   (NULL==(_ptr))

#endif


#ifndef CONTAINING_RECORD
#define CONTAINING_RECORD(address, type, field) ((type *)( 				            \
                                                 (char*)(address) - 			    \
                                                 (long)(&((type *)0)->field)))
#endif

#endif
