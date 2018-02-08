

#ifndef __MYHASHTABLE_CWC22_H__
#define __MYHASHTABLE_CWC22_H__

#ifdef __cplusplus
extern "C" {
#endif

struct myhashtable;







struct myhashtable *
create_myhashtable(unsigned int minsize,
                 unsigned int (*hashfunction) (void*),
                 int (*key_eq_fn) (void*,void*));



int 
myhashtable_insert(struct myhashtable *h, void *k, void *v);

#define DEFINE_HASHTABLE_INSERT(fnname, keytype, valuetype) \
int fnname (struct myhashtable *h, keytype *k, valuetype *v) \
{ \
    return myhashtable_insert(h,k,v); \
}



void *
myhashtable_search(struct myhashtable *h, void *k);

#define DEFINE_HASHTABLE_SEARCH(fnname, keytype, valuetype) \
valuetype * fnname (struct myhashtable *h, keytype *k) \
{ \
    return (valuetype *) (myhashtable_search(h,k)); \
}



void * 
myhashtable_remove(struct myhashtable *h, void *k);

#define DEFINE_HASHTABLE_REMOVE(fnname, keytype, valuetype) \
valuetype * fnname (struct myhashtable *h, keytype *k) \
{ \
    return (valuetype *) (myhashtable_remove(h,k)); \
}




void * 
myhashtable_remove_prefix(struct myhashtable *h, void *k, int free_values);

#define DEFINE_HASHTABLE_REMOVE_PREFIX(fnname, keytype, valuetype) \
valuetype * fnname (struct myhashtable *h, keytype *k) \
{ \
    return (valuetype *) (myhashtable_remove_prefix(h,k)); \
}


unsigned int
myhashtable_count(struct myhashtable *h);




void
myhashtable_destroy(struct myhashtable *h, int free_values);

#ifdef __cplusplus
}
#endif

#endif 


