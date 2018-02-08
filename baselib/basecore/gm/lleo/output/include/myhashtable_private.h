

#ifndef __MYHASHTABLE_PRIVATE_CWC22_H__
#define __MYHASHTABLE_PRIVATE_CWC22_H__

#include "myhashtable.h"


struct entry
{
    void *k, *v;
    unsigned int h;
    struct entry *next;
};

struct myhashtable {
    unsigned int tablelength;
    struct entry **table;
    unsigned int entrycount;
    unsigned int loadlimit;
    unsigned int primeindex;
    unsigned int (*hashfn) (void *k);
    int (*eqfn) (void *k1, void *k2);
};


unsigned int
hash(struct myhashtable *h, void *k);



static inline unsigned int
indexFor(unsigned int tablelength, unsigned int hashvalue) {
    return (hashvalue % tablelength);
};





#define freekey(X) free(X)





#endif 


