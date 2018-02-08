#ifndef __HASH_H__
#define __HASH_H__

#include<stdio.h>

unsigned int hash(
                 const void *key,                                       
                 size_t      length,                                    
                 const unsigned int    initval = 0);

#endif
