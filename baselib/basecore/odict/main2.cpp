#include <sys/time.h>
#include "ul_sign.h"
#include "ul_dict.h"

#include "odict.h"

int main(int argc, char *argv[])
{
#define ADD_COUNT    100
#define SEEK_COUNT   100

    sodict_build_t *sdb;
    sodict_snode_t snode;
 
    Sdict_build    *udb;
    Sdict_snode    unode;

    struct timeval ss, se, us, ue;

    printf("Comparison between odict and uldict!\n");

    printf("Create a odict dictionary object ... \t");
    sdb = odb_creat(10000000);
    if (sdb != NULL) {
        printf("OK\n");
    }
    else {
        printf("FAIL\n");
        return 1;
    }

    printf("Create a uldict dictionary object ... \t");
    udb = db_creat(10000000, 0);
    if (sdb != NULL) {
        printf("OK\n");
    }
    else {
        printf("FAIL\n");
        return 1;
    }

    
    printf("Insert %d nodes into odict dictionary object ... \t", ADD_COUNT);
    snode.sign1 = 25;
    snode.sign2 = 30;
    snode.cuint1 = 28;
    snode.cuint2 = 0xffffffff;
    gettimeofday(&ss, NULL);
    for (int i = 0 ; i < ADD_COUNT ; i++) {
        snode.sign1 = 25 + i;
        snode.sign2 = 30 + i;
        if (ODB_ADD_OK != odb_add(sdb, &snode, 1)) {
            printf("FAIL\n");
        }
    }
    gettimeofday(&se, NULL);
    printf("Finished!\n");

    printf("Insert %d nodes into uldict dictionary object ... \t", ADD_COUNT);
    unode.sign1 = 25;
    unode.sign2 = 30;
    unode.code = 28;
    unode.other = 0x00ffffff;
    gettimeofday(&us, NULL);
    for (int i = 0 ; i < ADD_COUNT ; i++) {
        unode.sign1 = 25 + i;
        unode.sign2 = 30 + i;
        db_op1(udb, &unode, ADD);
    }
    gettimeofday(&ue, NULL);
    printf("Finished!\n");
    
    printf("odict add operation: %f ms, %f us per req\n", 
        (se.tv_sec - ss.tv_sec) * 1000 + (se.tv_usec - ss.tv_usec) / 1000.0,
        ((se.tv_sec - ss.tv_sec) * 1000000.0 + (se.tv_usec - ss.tv_usec)) / ADD_COUNT);
    printf("uldict add operation: %f ms, %f us per req\n", 
        (ue.tv_sec - us.tv_sec) * 1000 + (ue.tv_usec - us.tv_usec) / 1000.0,
        ((ue.tv_sec - us.tv_sec) * 1000000.0 + (ue.tv_usec - us.tv_usec)) / ADD_COUNT);

    
    printf("Seek %d nodes from odict dictionary object ... \t", SEEK_COUNT);
    snode.sign1 = 25;
    snode.sign2 = 30;
    snode.cuint1 = 28;
    snode.cuint2 = 0xffffffff;
    gettimeofday(&ss, NULL);
    for (int i = 0 ; i < SEEK_COUNT ; i++) {
        snode.sign1 = 25 + i;
        snode.sign2 = 30 + i;
        if (ODB_ADD_OK != odb_seek(sdb, &snode)) {
            printf("FAIL\n");
        }
    }
    gettimeofday(&se, NULL);
    printf("Finished!\n");

    odb_adjust(sdb);
    snode.sign1 = 25;
    snode.sign2 = 30;
    snode.cuint1 = 28;
    snode.cuint2 = 0xffffffff;
    for (int i = 0 ; i < SEEK_COUNT ; i++) {
	snode.sign1 = 25 + i;
	snode.sign2 = 30 + i;
	if (ODB_ADD_OK != odb_seek(sdb, &snode)) {
	     printf("FAIL\n");
	}
    }
    
    odb_del(sdb,&snode);
    odb_add(sdb,&snode,1);
    if (ODB_ADD_OK != odb_seek(sdb, &snode)){
    	printf ("fail\n");
    }
    printf("Seek %d nodes from uldict dictionary object ... \t", SEEK_COUNT);
    unode.sign1 = 25;
    unode.sign2 = 30;
    unode.code = 28;
    unode.other = 0x00ffffff;
    gettimeofday(&us, NULL);
    for (int i = 0 ; i < SEEK_COUNT ; i++) {
        unode.sign1 = 25 + i;
        unode.sign2 = 30 + i;
        db_op1(udb, &unode, SEEK);
    }
    gettimeofday(&ue, NULL);
    printf("Finished!\n");

    printf("odict seek operation: %f ms, %f us per req\n", 
        (se.tv_sec - ss.tv_sec) * 1000 + (se.tv_usec - ss.tv_usec) / 1000.0,
        ((se.tv_sec - ss.tv_sec) * 1000000.0 + (se.tv_usec - ss.tv_usec)) / SEEK_COUNT);
    printf("uldict seek operation: %f ms, %f us per req\n", 
        (ue.tv_sec - us.tv_sec) * 1000 + (ue.tv_usec - us.tv_usec) / 1000.0,
        ((ue.tv_sec - us.tv_sec) * 1000000.0 + (ue.tv_usec - us.tv_usec)) / SEEK_COUNT);


    
    printf("Save odict dictionary to file ... \t");
    gettimeofday(&ss, NULL);
    odb_save(sdb, "./", "com.test.dict.odict");
    gettimeofday(&se, NULL);
    printf("OK\n");
    printf("Save uldict dictionary to file ... \t");
    gettimeofday(&us, NULL);
    db_save(udb, "./", "com.test.dict.uldict");
    gettimeofday(&ue, NULL);
    printf("OK\n");

    printf("odict save operation: %f ms, %f us per req\n", 
        (se.tv_sec - ss.tv_sec) * 1000 + (se.tv_usec - ss.tv_usec) / 1000.0,
        ((se.tv_sec - ss.tv_sec) * 1000000.0 + (se.tv_usec - ss.tv_usec)) / SEEK_COUNT);
    printf("uldict save operation: %f ms, %f us per req\n", 
        (ue.tv_sec - us.tv_sec) * 1000 + (ue.tv_usec - us.tv_usec) / 1000.0,
        ((ue.tv_sec - us.tv_sec) * 1000000.0 + (ue.tv_usec - us.tv_usec)) / SEEK_COUNT);
    odb_adjust(sdb);
    printf ("hash %d\n",odb_get_hashnum(sdb));
 
    
    return 0;
}
