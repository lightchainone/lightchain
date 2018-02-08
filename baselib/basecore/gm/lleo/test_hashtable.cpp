#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "myhashtable.h"

unsigned int string_hash_djb2(void *str)
{
    unsigned int hash = 5381;
    const char *pch = (const char*)str;
    int c;

    while ((c = *(pch++)))
        hash = ((hash << 5) + hash) + c; 

    return hash;
}

int string_equal(void *key1,void *key2)
{
    return strcmp((const char*)key1,(const char*)key2)==0;
}

int main(){
    int rc = 0;
    void *pv = NULL;
    printf("Hello world!\n");

    myhashtable *ht = create_myhashtable(64, string_hash_djb2, string_equal);
    char key[] = "head";
    char value[] = "head";
    rc = myhashtable_insert(ht, strdup(key), value);
    printf("insert 1, rc:%d\n", rc);
    pv = myhashtable_search(ht, (void*)key);
    printf("search 1, pv:%s\n", (char*)pv);

    char key1[] = "head1";
    char value1[] = "head1";
    rc = myhashtable_insert(ht, strdup(key1), value1);
    printf("insert 2, rc:%d\n", rc);
    pv = myhashtable_search(ht, (void*)key1);
    printf("search 2, pv:%s\n", (char*)pv);

    char key3[] = "hea";
    char value3[] = "hea";
    rc = myhashtable_insert(ht, strdup(key3), value3);
    printf("insert 3, rc:%d\n", rc);
    pv = myhashtable_search(ht, (void*)key3);
    printf("search 3, pv:%s\n", (char*)pv);

    char key2[] = "head";
    myhashtable_remove_prefix(ht, (void*)key2, 0);
    pv = myhashtable_search(ht, (void*)key);
    if (NULL != pv)
        printf("search 4, pv:%s\n", (char*)pv);
    pv = myhashtable_search(ht, (void*)key1);
    if (NULL != pv)
       printf("search 5, pv:%s\n", (char*)pv);
    pv = myhashtable_search(ht, (void*)key3);
    if (NULL != pv)
       printf("search 6, pv:%s\n", (char*)pv);

    printf("Done\n");
    return 0;
}
