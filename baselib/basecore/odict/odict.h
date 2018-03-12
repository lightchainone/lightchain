
#ifndef _ODICT_H_
#define _ODICT_H_

enum {
    ODB_ADD_OK=0, 
    ODB_ADD_ERROR=1,
    ODB_ADD_EXISTS=2,
    ODB_ADD_OVERWRITE=3,
    ODB_ADD_INVALID=4,
    
    ODB_DEL_OK=0,
    ODB_DEL_ERROR=1,
    ODB_DEL_NOT_EXISTS=2,
    
    ODB_SEEK_OK=0,
    ODB_SEEK_ERROR=1,
    ODB_SEEK_FAIL=2,
    
    ODB_MOD_OK=0,
    ODB_MOD_ERROR=1,
    ODB_MOD_FAIL=2,
    
    ODB_RENEW_OK=0,
    ODB_RENEW_ERROR=1,

    ODB_SAVE_OK=0,
    ODB_SAVE_ERROR=1,

    ODB_LOAD_NOT_EXISTS=1,
    
    ODB_DESTROY_OK=0,
};


const int ODB_MID_NODE_NUM=4*1024;

const int ODB_NODE_BLOCK_NUM=256*1024;

const unsigned int ODB_IO_BUFFSIZE=4*1024*1024;

const int ODB_DEFAULT_HASH_NUM=1000000;

typedef struct _sodict_snode_t{
    unsigned int sign1;
    unsigned int sign2;
    unsigned int cuint1;
    unsigned int cuint2;
}sodict_snode_t;

typedef struct _sodict128_snode_t{
    unsigned long long sign1;
    unsigned long long sign2;
    unsigned int cuint1;
    unsigned int cuint2;
}sodict128_snode_t;

typedef struct _sodict_build_t sodict_build_t;
typedef struct _sodict_search_t sodict_search_t;
typedef struct _sodict128_build_t sodict128_build_t;
typedef struct _sodict128_search_t sodict128_search_t;


sodict_build_t *odb_creat(int hash_num);


sodict_build_t *odb_load(char *path, char *filename, int hash_num);




sodict_search_t *odb_load_search(char *path, char *filename);



sodict_search_t *odb_load_search_mmap(char *path, char *filename);


int odb_destroy_search_mmap(sodict_search_t *sdb);

int odb_destroy(sodict_build_t *sdb);



int odb_destroy_search(sodict_search_t *sdb);



int odb_save(sodict_build_t *sdb, char *path, char *filename);




int odb_save_search(sodict_search_t *sdb, char *path, char *filename);





int odb_save_safely(sodict_build_t *sdb, char *path, char *filename);




int odb_save_search_safely(sodict_search_t *sdb, char *path, char *filename);

int odb_renew(sodict_build_t *sdb);




int odb_renew_ex(sodict_build_t *sdb);



int odb_add(sodict_build_t *sdb, sodict_snode_t *snode, int overwrite_if_exists);



int odb_del(sodict_build_t *sdb, sodict_snode_t *snode);



int odb_seek(sodict_build_t *sdb, sodict_snode_t *snode);



int odb_seek_search(sodict_search_t *sdb, sodict_snode_t *snode);




int odb_mod(sodict_build_t *sdb, sodict_snode_t *snode);



void odb_traverse(sodict_build_t *sdb, int include_deleted,
                  void (*traverse_handler)(sodict_snode_t *snode, int *stop_now));




void odb_traverse_ex(sodict_build_t *sdb, int include_deleted,
                     void (*traverse_handler)(sodict_snode_t *snode, int *stop_now, void *arg), void *ex_arg);




void odb_traverse_search(sodict_search_t *sdb, void (*traverse_handler)(sodict_snode_t *snode, int *stop_now));


void odb_traverse_search_ex(sodict_search_t *sdb, void (*traverse_handler)(sodict_snode_t *snode, int *stop_now,void *arg),
                            void *ex_arg);

void odb_build_report(sodict_build_t *sdb);



void odb_search_report(sodict_search_t *sdb);


int odb_get_nodenum(sodict_build_t *sdb);

int odb_search_get_nodenum(sodict_search_t *sdb);


int odb_get_hashnum(sodict_build_t *sdb);

int odb_search_get_hashnum(sodict_search_t *sdb);


int odb_adjust(sodict_build_t *sdb);



int odb_is_equal_search(sodict_search_t *sdb1, sodict_search_t *sdb2);



sodict128_build_t *odb128_creat(int hash_num);





sodict128_build_t *odb128_load(char *path, char *filename, int hash_num);




sodict128_search_t *odb128_load_search(char *path, char *filename);



sodict128_search_t *odb128_load_search_mmap(char *path, char *filename);


int odb_destroy_search_mmap(sodict128_search_t *sdb);

int odb_destroy(sodict128_build_t *sdb);



int odb_destroy_search(sodict128_search_t *sdb);



int odb_save(sodict128_build_t *sdb, char *path, char *filename);




int odb_save_search(sodict128_search_t *sdb, char *path, char *filename);





int odb_save_safely(sodict128_build_t *sdb, char *path, char *filename);




int odb_save_search_safely(sodict128_search_t *sdb, char *path, char *filename);

int odb_renew(sodict128_build_t *sdb);




int odb_renew_ex(sodict128_build_t *sdb);



int odb_add(sodict128_build_t *sdb, sodict128_snode_t *snode, int overwrite_if_exists);



int odb_del(sodict128_build_t *sdb, sodict128_snode_t *snode);



int odb_seek(sodict128_build_t *sdb, sodict128_snode_t *snode);



int odb_seek_search(sodict128_search_t *sdb, sodict128_snode_t *snode);




int odb_mod(sodict128_build_t *sdb, sodict128_snode_t *snode);



void odb_traverse(sodict128_build_t *sdb, int include_deleted,
                  void (*traverse_handler)(sodict128_snode_t *snode, int *stop_now));




void odb_traverse_ex(sodict128_build_t *sdb, int include_deleted,
                     void (*traverse_handler)(sodict128_snode_t *snode, int *stop_now, void *arg), void *ex_arg);




void odb_traverse_search(sodict128_search_t *sdb, void (*traverse_handler)(sodict128_snode_t *snode, int *stop_now));


void odb_traverse_search_ex(sodict128_search_t *sdb, void (*traverse_handler)(sodict128_snode_t *snode, int *stop_now,void *arg),
                            void *ex_arg);

void odb_build_report(sodict128_build_t *sdb);



void odb_search_report(sodict128_search_t *sdb);


int odb_get_nodenum(sodict128_build_t *sdb);

int odb_search_get_nodenum(sodict128_search_t *sdb);


int odb_get_hashnum(sodict128_build_t *sdb);

int odb_search_get_hashnum(sodict128_search_t *sdb);


int odb_adjust(sodict128_build_t *sdb);


int odb_is_equal_search(sodict128_search_t *sdb1, sodict128_search_t *sdb2);

#endif
