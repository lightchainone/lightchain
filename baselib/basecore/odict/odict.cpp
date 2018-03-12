#include <cassert>
#include <cstring>
#include <iostream>
#include "ul_conf.h"
#include "ul_log.h"
#include "ul_func.h"
#include "ul_file.h"
#include "odict.h"


template<typename T1,typename T2>struct CustomType
{
    typedef T1 RAW;
    typedef T2 ATOMIC; 
    
};
typedef CustomType<unsigned int,unsigned long long>Custom_unsigned_int;

template<typename SIGN_TYPE,typename CUSTOM_TYPE>struct OdictSNode{
    SIGN_TYPE sign1; 
    SIGN_TYPE sign2; 
    typename CUSTOM_TYPE::RAW cuint1; 
    typename CUSTOM_TYPE::RAW cuint2; 
};




template<typename SIGN_TYPE,typename CUSTOM_TYPE>struct OdictSNodeAtomic{
    SIGN_TYPE sign1;
    SIGN_TYPE sign2;
    typename CUSTOM_TYPE::ATOMIC cuint;
};


template<typename SIGN_TYPE,typename CUSTOM_TYPE>struct OdictBNode{
    SIGN_TYPE sign1;
    SIGN_TYPE sign2;
    typename CUSTOM_TYPE::RAW cuint1;
    typename CUSTOM_TYPE::RAW cuint2;
    OdictBNode<SIGN_TYPE,CUSTOM_TYPE> *next;
};


template<typename SIGN_TYPE,typename CUSTOM_TYPE>struct OdictBNodeAtomic{
    SIGN_TYPE sign1;
    SIGN_TYPE sign2;
    typename CUSTOM_TYPE::ATOMIC cuint;
    OdictBNode<SIGN_TYPE,CUSTOM_TYPE> *next;
};


template<typename SIGN_TYPE,typename CUSTOM_TYPE>struct OdictBHash{
    OdictBNode<SIGN_TYPE,CUSTOM_TYPE> *pnode; 
};


template<typename SIGN_TYPE,typename CUSTOM_TYPE>struct OdictBNodeBlock{
    OdictBNode<SIGN_TYPE,CUSTOM_TYPE> nodes[ODB_NODE_BLOCK_NUM]; 
    OdictBNodeBlock<SIGN_TYPE,CUSTOM_TYPE>*next; 
};


template<typename SIGN_TYPE,typename CUSTOM_TYPE>struct OdictBuild{
    int hash_num; 
    int node_num; 
    int cur_code; 
    OdictBHash<SIGN_TYPE,CUSTOM_TYPE> *hash; 
    OdictBNodeBlock<SIGN_TYPE,CUSTOM_TYPE> *nblock; 
    OdictBNodeBlock<SIGN_TYPE,CUSTOM_TYPE> *cur_nblock; 
    int node_avail; 
    OdictBNode<SIGN_TYPE,CUSTOM_TYPE> * cur_node; 
    typedef SIGN_TYPE SIGN;
    typedef CUSTOM_TYPE CUSTOM;
};


template<typename SIGN_TYPE,typename CUSTOM_TYPE>struct OdictSearch{
    int hash_num; 
    int node_num; 
    int cur_code; 
    unsigned int *hash; 
    unsigned int *num; 
    OdictSNode<SIGN_TYPE,CUSTOM_TYPE> *node; 
    typedef SIGN_TYPE SIGN;
    typedef CUSTOM_TYPE CUSTOM;
};

struct _sodict_build_t:plclic OdictBuild<unsigned int,Custom_unsigned_int>{
};
struct _sodict_search_t:plclic OdictSearch<unsigned int,Custom_unsigned_int>{
};
struct _sodict128_build_t:plclic OdictBuild<unsigned long long,Custom_unsigned_int>{
};
struct _sodict128_search_t:plclic OdictSearch<unsigned long long,Custom_unsigned_int>{
};

typedef unsigned int odict_offset_t;


static int odbi_cmps_path(const char *path, const char *filename, char *buffer, int bufsize)
{
    assert(path);
    assert(filename);
    assert(buffer);
    
    buffer[0] = '\0';
    int len1 = strlen(path);
    int len2 = strlen(filename);
    if (len1 + len2 + 2 > bufsize)
        return -1;
    
    strcpy(buffer, path);
    if (*(buffer + len1 - 1) != '/')
        strcat(buffer, "/");
    strcat(buffer, filename);
    return 0;
}


static int is_file_exist(const char *path,const char *filename)
{
    char full_path[PATH_SIZE];
    struct stat statbuf;
    if (filename == NULL) {
        return 0;
    }
    if (path == NULL || strlen(path) == 0) {
        snprintf (full_path,sizeof(full_path),"%s",filename);
    } else if (path[strlen(path)-1] == '/') {
        snprintf (full_path,sizeof(full_path),"%s%s",path,filename);
    } else {
        snprintf(full_path, sizeof(full_path),"%s/%s", path, filename);
    }
    if(stat(full_path,&statbuf)==-1) {
        return 0;
    }
    if(!S_ISREG(statbuf.st_mode)) {
        return 0;
    }
    return 1;
}


static int odbi_is_dictionary_valid(const char *path, const char *filename)
{
    char tname[PATH_SIZE];
    
    snprintf(tname, sizeof(tname), "%s.n", filename);
    if (is_file_exist(path,tname) != 1) {
        return 0;
    }
    snprintf(tname, sizeof(tname),"%s.ind1", filename);
    if (is_file_exist(path,tname) != 1) {
        return 0;
    }
    snprintf(tname, sizeof(tname),"%s.ind2", filename);
    if (is_file_exist(path,tname) != 1) {
        return 0;
    }
    return 1;
}


static int odbi_is_dictionary_exist(const char *path, const char *filename)
{
    char tname[PATH_SIZE];

    snprintf(tname, sizeof(tname), "%s.n", filename);
    if (is_file_exist(path,tname) == 1) {
        return 1;
    }
    snprintf(tname, sizeof(tname),"%s.ind1", filename);
    if (is_file_exist(path,tname) == 1) {
        return 1;
    }
    snprintf(tname, sizeof(tname),"%s.ind2", filename);
    if (is_file_exist(path,tname) == 1) {
        return 1;
    }
    return 0;
}

static int odbi_read(int fno, void *data, size_t len)
{
    ssize_t rsize = 0;
    ssize_t lsize = 0;
    if (data == NULL || fno < 0) {
        return -1;
    }
    char *ptr = (char *)data;

    while(len > 0) {
        rsize = (len<ODB_IO_BUFFSIZE)?len:ODB_IO_BUFFSIZE;
        lsize = ul_read(fno, ptr, rsize);
        if (lsize < 0 || lsize != rsize) {
            return -1;
        }
        ptr += lsize;
        len -= lsize;
        
    }
    return 0;
}


template<typename SIGN_TYPE,typename CUSTOM_TYPE>
    static int odbi_is_node_deleted(OdictBNode<SIGN_TYPE,CUSTOM_TYPE> *node)
{
    if (node->sign1 == 0 && node->sign2 == 0)
        return 1;
    else
        return 0;
}


template<typename SIGN_TYPE,typename CUSTOM_TYPE>
    static OdictBuild<SIGN_TYPE,CUSTOM_TYPE> *_odb_creat(int hash_num)
{
    typedef OdictBuild<SIGN_TYPE,CUSTOM_TYPE> _inl_sodict_build_t;
    typedef OdictBHash<SIGN_TYPE,CUSTOM_TYPE> _inl_sodict_bhash_t;
    typedef OdictBNodeBlock<SIGN_TYPE,CUSTOM_TYPE> _inl_sodict_bnode_block_t;
    
    _inl_sodict_build_t       *pdb = NULL;
    _inl_sodict_bhash_t       *phash = NULL;
    _inl_sodict_bnode_block_t *pblock = NULL;
    
    if (hash_num <= 0)
        hash_num = ODB_DEFAULT_HASH_NUM;
    
    
    if((pdb = (_inl_sodict_build_t*)ul_malloc(sizeof(_inl_sodict_build_t))) == NULL) {
        ul_writelog(UL_LOG_WARNING, "[ODICT ERROR] cannot allocate memory for dictionary object[%zu].",
                    sizeof(_inl_sodict_build_t));
        return NULL;
    }

    if((phash = (_inl_sodict_bhash_t*)ul_calloc(1, hash_num*sizeof(_inl_sodict_bhash_t))) == NULL) {
        ul_writelog(UL_LOG_WARNING, "[ODICT ERROR] cannot mallocate memory for hash table[%zu].",
                    (hash_num*sizeof(_inl_sodict_bhash_t)));
        ul_free(pdb);
        return NULL;
    }
    if((pblock = (_inl_sodict_bnode_block_t*)ul_calloc(1, sizeof(_inl_sodict_bnode_block_t)))==NULL) {
        ul_writelog(UL_LOG_WARNING, "[ODICT ERROR] cannot mallocate memory for node buffer[%zu].",
                    sizeof(_inl_sodict_bnode_block_t));
        ul_free(pdb);
        ul_free(phash);
        return NULL;
    }

    
    pdb->hash_num = hash_num;
    pdb->cur_code = 0;    
    pdb->node_num = 0;
    pdb->hash = phash;
    pdb->nblock = pblock;
    pdb->cur_nblock = pdb->nblock;
    pdb->cur_node = pdb->nblock->nodes;
    pdb->node_avail = ODB_NODE_BLOCK_NUM;

    
    return pdb;
}


template<typename SIGN_TYPE,typename CUSTOM_TYPE>
    static OdictSearch<SIGN_TYPE,CUSTOM_TYPE> *_odb_creat_search(int hash_num,int node_num)
{
    typedef OdictSearch<SIGN_TYPE,CUSTOM_TYPE> sodict_search_t;
    typedef OdictSNode<SIGN_TYPE,CUSTOM_TYPE> _inl_sodict_snode_t;
    
    sodict_search_t  *pds = NULL;
    
    
    if((pds = (sodict_search_t* )ul_calloc(1, sizeof(sodict_search_t))) == NULL) {
        ul_writelog(UL_LOG_WARNING,"[ODICT ERROR] cannot create the search dictionary object.");
        return NULL;
    }
    
    pds->hash_num = hash_num;
    pds->node_num = node_num;
    pds->cur_code = 0;
    if((pds->hash = (odict_offset_t* )ul_malloc(hash_num*sizeof(odict_offset_t))) == NULL) {
        ul_free(pds);
        return NULL;
    }
    if((pds->num = (odict_offset_t* )ul_malloc(hash_num*sizeof(odict_offset_t))) == NULL) {
        ul_free(pds->hash);
        ul_free(pds);
        return NULL;
    }
    if((pds->node = (_inl_sodict_snode_t*)ul_malloc(node_num*sizeof(_inl_sodict_snode_t))) == NULL) {
        ul_free(pds->hash);
        ul_free(pds->num);
        ul_free(pds);
        return NULL;
    }

    
    return pds;
}


template<typename SIGN_TYPE,typename CUSTOM_TYPE>
    static OdictBuild<SIGN_TYPE,CUSTOM_TYPE> *_odb_load(char *path, char *filename, int hash_num)
{
    typedef OdictSNode<SIGN_TYPE,CUSTOM_TYPE> _inl_sodict_snode_t;
    typedef OdictBuild<SIGN_TYPE,CUSTOM_TYPE> _inl_sodict_build_t;

    int on_num=0, n_num = 0;
    int snode = sizeof(_inl_sodict_snode_t);
    int full_size = ODB_MID_NODE_NUM * snode;
    int fno=-1;
    size_t ind2_size = 0;

    _inl_sodict_build_t *pdb=NULL;
    _inl_sodict_snode_t sn[ODB_MID_NODE_NUM];
    
    char    tname[PATH_SIZE];
    char    fullpath[PATH_SIZE];
    
    assert(path != NULL);
    assert(filename != NULL);

    if (strlen(path) >= sizeof(tname)-2) {
        ul_writelog(UL_LOG_WARNING, "[ODICT ERROR] odb_load path:%s too long",path);
        return NULL;
    }
    strcpy(tname,path);
    if (tname[strlen(tname)-1] != '/') {
        strcat(tname,"/");
    }
    
    if (!odbi_is_dictionary_valid(tname, filename)) {
        ul_writelog(UL_LOG_WARNING, "[ODICT ERROR] dictionary %s/%s does not exist.", tname, filename);
        return (_inl_sodict_build_t *)ODB_LOAD_NOT_EXISTS;
    }

    
    if (hash_num <= 0) {
        if(ul_readnum_oneint(tname, filename, "hash_num", &hash_num) != 1) {
            ul_writelog(UL_LOG_WARNING, "[ODICT ERROR] read hash number from %s/%s.n failed.", tname, filename);
            return NULL;
        }
    }
    if(ul_readnum_oneint(tname, filename, "node_num", &on_num) != 1) {
        ul_writelog(UL_LOG_WARNING,"[ODICT ERROR] read node number from %s/%s.n failed.", tname, filename);
        return NULL;
    }

    
    if ((pdb = _odb_creat<SIGN_TYPE,CUSTOM_TYPE>(hash_num)) == NULL) {
        ul_writelog(UL_LOG_WARNING,"[ODICT ERROR] cannot create the dictionary object by odb_creat.");
        return NULL;
    }

    
    sprintf(tname, "%s.ind2", filename);
    ind2_size = ul_fsize(path,tname);
    if(ind2_size != on_num*sizeof(_inl_sodict_snode_t)) {
        ul_writelog(UL_LOG_WARNING, "[ODICT ERROR] check ind2 size error!want[%zu] real[%zu]",
                    on_num*sizeof(_inl_sodict_snode_t),ind2_size);
        _odb_destroy(pdb);
        return NULL;
    }
    odbi_cmps_path(path, tname, fullpath, PATH_SIZE);
    if((fno = ul_open(fullpath, O_RDONLY, 0644)) == -1) {
        ul_writelog(UL_LOG_WARNING, "[ODICT ERROR] cannot open file %s.", fullpath);
        _odb_destroy(pdb);
        return NULL;
    }
    
    int rsize;
    while((rsize = ul_read(fno, sn, full_size)) > 0) {
        int rnum = rsize/snode;
        for (int i = 0; i < rnum; i++) {
            if (ODB_ADD_ERROR == _odb_add(pdb, &(sn[i]), 1)) {
                ul_writelog(UL_LOG_WARNING, "[ODICT ERROR] cannot insert node into dictionary.");
                _odb_destroy(pdb);
                ul_close(fno);
                return NULL;
            }
        }
        n_num += rnum;
    }

    if (n_num != on_num) {
        ul_writelog(UL_LOG_WARNING, "[ODICT WARNING] node number error[%d != %d].", n_num, on_num);
    }

    
    ul_close(fno);

    
    return pdb;
}


template<typename SIGN_TYPE,typename CUSTOM_TYPE>
    static OdictSearch<SIGN_TYPE,CUSTOM_TYPE> *_odb_load_search(char *path, char *filename)
{
    typedef OdictSearch<SIGN_TYPE,CUSTOM_TYPE> sodict_search_t;
    typedef OdictSNode<SIGN_TYPE,CUSTOM_TYPE> _inl_sodict_snode_t;
    
    int fno=-1;
    int hash_num=0, node_num=0;

    char    tname[PATH_SIZE];
    char    fullpath[PATH_SIZE];
    size_t ind1_size = 0;
    size_t ind2_size = 0;
    sodict_search_t    *pds = NULL;
    
    assert(path != NULL);
    assert(filename != NULL);
    if (strlen(path) >= sizeof(tname)-2) {
        ul_writelog(UL_LOG_WARNING, "[ODICT ERROR] odb_load path:%s too long",path);
        return NULL;
    }
    strcpy(tname,path);
    if (tname[strlen(tname)-1] != '/') {
        strcat(tname,"/");
    }
    
    if (!odbi_is_dictionary_valid(tname, filename)) {
        ul_writelog(UL_LOG_WARNING, "[ODICT ERROR] dictionary %s/%s does not exist.", tname, filename);
        return (sodict_search_t *)ODB_LOAD_NOT_EXISTS;
    }
    
    
    if(ul_readnum_oneint(tname, filename, "hash_num", &hash_num) != 1) {
        ul_writelog(UL_LOG_WARNING, "[ODICT ERROR] read hash number from %s/%s.n failed.", tname, filename);
        return NULL;
    }
    if(ul_readnum_oneint(tname, filename, "node_num", &node_num) != 1) {
        ul_writelog(UL_LOG_WARNING, "[ODICT ERROR] read node number from %s/%s.n failed.", tname, filename);
        return NULL;
    }
    
    
    if ((pds = _odb_creat_search<SIGN_TYPE,CUSTOM_TYPE>(hash_num, node_num)) == NULL) {
        ul_writelog(UL_LOG_WARNING,"[ODICT ERROR] cannot create the dictionary object by odb_creat_search.");
        return NULL;
    }

    
    snprintf(tname,sizeof(tname), "%s.ind1", filename);
    ind1_size = ul_fsize(path,tname);
    if(ind1_size != 2*hash_num*sizeof(odict_offset_t)) {
        ul_writelog(UL_LOG_WARNING, "[ODICT ERROR] check ind1 size error!want[%zu] real[%zu]",
                    2*hash_num*sizeof(odict_offset_t),ind1_size);
        _odb_destroy_search(pds);
        return NULL;
    }
    odbi_cmps_path(path, tname, fullpath, PATH_SIZE);
    if( (fno = ul_open(fullpath, O_RDONLY, 0644)) == -1 ) {
        _odb_destroy_search(pds);
        return NULL;
    }
    odbi_read(fno, pds->hash, (odict_offset_t)hash_num*sizeof(odict_offset_t));
    odbi_read(fno, pds->num, (odict_offset_t)hash_num*sizeof(odict_offset_t));  
    ul_close(fno);
    
    snprintf(tname,sizeof(tname), "%s.ind2", filename);

    ind2_size = ul_fsize(path,tname);
    if(ind2_size != node_num*sizeof(_inl_sodict_snode_t)) {
        ul_writelog(UL_LOG_WARNING, "[ODICT ERROR] check ind2 size error!want[%zu] real[%zu]",
                    node_num*sizeof(_inl_sodict_snode_t),ind2_size);
        _odb_destroy_search(pds);
        return NULL;
    }
    odbi_cmps_path(path, tname, fullpath, PATH_SIZE);
    if( (fno= ul_open(fullpath, O_RDONLY, 0644)) == -1 ) {
        _odb_destroy_search(pds);
        return NULL;
    }
    odbi_read(fno, pds->node, (size_t)node_num*sizeof(_inl_sodict_snode_t));
    ul_close(fno);

    
    return pds;
}

template<typename SIGN_TYPE,typename CUSTOM_TYPE>
    static OdictSearch<SIGN_TYPE,CUSTOM_TYPE> *_odb_load_search_mmap(char *path, char *filename)
{
    typedef OdictSearch<SIGN_TYPE,CUSTOM_TYPE> sodict_search_t;
    typedef OdictSNode<SIGN_TYPE,CUSTOM_TYPE> _inl_sodict_snode_t;

    int fno=-1;
    int hash_num=0, node_num=0;
    
    char    tname[PATH_SIZE];
    char    fullpath[PATH_SIZE];
    size_t ind1_size = 0;
    size_t ind2_size = 0;
    sodict_search_t    *pds = NULL;
    
    assert(path != NULL);
    assert(filename != NULL);
    if (strlen(path) >= sizeof(tname)-2) {
        ul_writelog(UL_LOG_WARNING, "[ODICT ERROR] odb_load path:%s too long",path);
        return NULL;
    }
    snprintf( tname, sizeof(tname), "%s",path);
    if (tname[strlen(tname)-1] != '/') {
        strcat(tname,"/");
    }
    
    if (!odbi_is_dictionary_valid(tname, filename)) {
        ul_writelog(UL_LOG_WARNING, "[ODICT ERROR] dictionary %s/%s does not exist.", tname, filename);
        return (sodict_search_t *)ODB_LOAD_NOT_EXISTS;
    }

    
    if(ul_readnum_oneint(tname, filename, "hash_num", &hash_num) != 1) {
        ul_writelog(UL_LOG_WARNING,
                    "[ODICT ERROR] read hash number from %s/%s.n failed.", tname, filename);
        return NULL;
    }
    if(ul_readnum_oneint(tname, filename, "node_num", &node_num) != 1) {
        ul_writelog(UL_LOG_WARNING,
                    "[ODICT ERROR] read node number from %s/%s.n failed.", tname, filename);
        return NULL;
    }
    if((pds = (sodict_search_t* )ul_calloc(1, sizeof(sodict_search_t))) == NULL) {
        ul_writelog(UL_LOG_WARNING,
                    "[ODICT ERROR] cannot create the search dictionary object.");
        return NULL;
    }
    pds->hash_num = hash_num;
    pds->node_num = node_num;
    pds->cur_code = 0;

    
    snprintf(tname,sizeof(tname), "%s.ind1", filename);
    ind1_size = ul_fsize(path,tname);
    if(ind1_size != 2*hash_num*sizeof(odict_offset_t)) {
        ul_writelog(UL_LOG_WARNING, "[ODICT ERROR] check ind1 size error!want[%zu] real[%zu]",
                    2*hash_num*sizeof(int),ind1_size);
        _odb_destroy_search(pds);
        return NULL;
    }
    odbi_cmps_path(path, tname, fullpath, PATH_SIZE);
    if( (fno = ul_open(fullpath, O_RDONLY, 0644)) == -1 ) {
        _odb_destroy_search(pds);
        return NULL;
    }
    pds->hash = (odict_offset_t*)mmap(NULL, 2 * (size_t)hash_num*sizeof(odict_offset_t),
                                    PROT_READ, MAP_PRIVATE, fno, 0);
    ul_close(fno);
    if(pds->hash == MAP_FAILED) {
        ul_writelog(UL_LOG_WARNING, "[ODICT ERROR]mmap hash failed, %m, size %d",
                    (int)( hash_num * sizeof(int)));
        _odb_destroy_search(pds);
        return NULL;
    }
    pds->num = pds->hash + hash_num;
    snprintf(tname,sizeof(tname), "%s.ind2", filename);
    ind2_size = ul_fsize(path,tname);
    if(ind2_size != node_num*sizeof(_inl_sodict_snode_t)) {
        ul_writelog(UL_LOG_WARNING, "[ODICT ERROR] check ind2 size error!want[%zu] real[%zu]",
                    node_num*sizeof(_inl_sodict_snode_t),ind2_size);
        _odb_destroy_search(pds);
        return NULL;
    }
    odbi_cmps_path(path, tname, fullpath, PATH_SIZE);
    if( (fno= ul_open(fullpath, O_RDONLY, 0644)) == -1 ) {
        _odb_destroy_search(pds);
        return NULL;
    }
    if(node_num > 0) {
        pds->node = (_inl_sodict_snode_t *) mmap(NULL, (size_t)node_num*sizeof(_inl_sodict_snode_t),
                                            PROT_READ, MAP_PRIVATE, fno, 0);
        ul_close(fno);
        if(pds->node == MAP_FAILED) {
            ul_writelog(UL_LOG_WARNING, "[ODICT ERROR]mmap node failed, %m, size %d",
                        (int)(node_num*sizeof(_inl_sodict_snode_t)));
            _odb_destroy_search(pds);
            return NULL;
        }
    }
    
    return pds;
}


template<typename SIGN_TYPE,typename CUSTOM_TYPE>
    static int _odb_destroy(OdictBuild<SIGN_TYPE,CUSTOM_TYPE>*sdb)
{
    typedef OdictBNodeBlock<SIGN_TYPE,CUSTOM_TYPE> _inl_sodict_bnode_block_t;
    
    assert(sdb != NULL);

    free(sdb->hash);
    _inl_sodict_bnode_block_t *block = sdb->nblock;
    while (block != NULL) {
        _inl_sodict_bnode_block_t *pblock = block;
        block = pblock->next;
        free(pblock);
    }
    free(sdb);

    return ODB_DESTROY_OK;
}

template<typename SIGN_TYPE,typename CUSTOM_TYPE>
    int _odb_destroy_search_mmap(OdictSearch<SIGN_TYPE,CUSTOM_TYPE> *sdb)
{
    
    typedef OdictSNode<SIGN_TYPE,CUSTOM_TYPE> _inl_sodict_snode_t;
    
    assert(sdb != NULL);
    if(sdb->node != MAP_FAILED && sdb->node != NULL)
        munmap(sdb->node, (size_t)sdb->node_num*sizeof(_inl_sodict_snode_t));
    if(sdb->hash != MAP_FAILED && sdb->hash != NULL)
        munmap(sdb->hash, 2*(size_t)sdb->hash_num*sizeof(odict_offset_t));
    ul_free(sdb);
    return ODB_DESTROY_OK;
}

template<typename SIGN_TYPE,typename CUSTOM_TYPE>
    static int _odb_destroy_search(OdictSearch<SIGN_TYPE,CUSTOM_TYPE> *sdb)
{

    assert(sdb != NULL);
    ul_free(sdb->node);
    ul_free(sdb->hash);
    ul_free(sdb->num);
    ul_free(sdb);
    return ODB_DESTROY_OK;
}


template<typename SIGN_TYPE,typename CUSTOM_TYPE>
    static int _odb_save(OdictBuild<SIGN_TYPE,CUSTOM_TYPE>* sdb, char *path, char *filename)
{
    typedef OdictSNode<SIGN_TYPE,CUSTOM_TYPE> _inl_sodict_snode_t;
    typedef OdictBNode<SIGN_TYPE,CUSTOM_TYPE> _inl_sodict_bnode_t;
    typedef OdictBHash<SIGN_TYPE,CUSTOM_TYPE> _inl_sodict_bhash_t;
    typedef OdictSNodeAtomic<SIGN_TYPE,CUSTOM_TYPE> sodict_snode_atomic_t;
    typedef OdictBNodeAtomic<SIGN_TYPE,CUSTOM_TYPE> sodict_bnode_atomic_t;

    int fno=-1;
    int snode = sizeof(_inl_sodict_snode_t);
    int wt_len = 0;
    _inl_sodict_snode_t ds[ODB_MID_NODE_NUM];

    odict_offset_t   *phi;           
    odict_offset_t   *pnum;          
    
    char    tname[PATH_SIZE];
    char    fullpath[PATH_SIZE];
    
    assert(sdb != NULL);
    assert(path != NULL);
    assert(filename != NULL);

    int h_num = sdb->hash_num;
    _inl_sodict_bhash_t *pbh = sdb->hash;
    if (strlen(path) >= sizeof(tname)-2) {
        ul_writelog(UL_LOG_WARNING,"[ODICT ERROR] path too long %s",path);
        return ODB_SAVE_ERROR;
    }
    strcpy(tname,path);
    if (tname[strlen(tname)-1] != '/') {
        strcat(tname,"/");
    }
    
    if((phi = (odict_offset_t *)ul_calloc(1,h_num * sizeof(odict_offset_t))) == NULL) {
        ul_writelog(UL_LOG_WARNING,"[ODICT ERROR] cannot allocate memory for phi[%zu].",
                    (h_num * sizeof(odict_offset_t)));
        return ODB_SAVE_ERROR;
    }
    if((pnum = (odict_offset_t *) ul_calloc(1, h_num * sizeof(odict_offset_t))) == NULL) {
        ul_writelog(UL_LOG_WARNING,"[ODICT ERROR] cannot allocate memory for pnum[%zu].",
                    (h_num * sizeof(odict_offset_t)));
        ul_free(phi);
        return ODB_SAVE_ERROR;
    }

    
    if (ul_writenum_init(tname, filename) == 1) {
        ul_writenum_oneint(tname, filename, "hash_num", sdb->hash_num);
        ul_writenum_oneint(tname, filename, "node_num", sdb->node_num);
        ul_writenum_oneint(tname, filename, "cur_code", sdb->cur_code);
    } else {
        ul_free(phi);
        ul_free(pnum);
        return ODB_SAVE_ERROR;
    }

    
    sprintf(tname, "%s.ind2", filename);
    odbi_cmps_path(path, tname, fullpath, PATH_SIZE);
    if((fno = ul_open(fullpath, O_WRONLY|O_CREAT|O_TRUNC, 0644)) == -1) {
        ul_writelog(UL_LOG_WARNING,"[ODICT ERROR] cannot open file[%s] for writing.", fullpath);
        ul_free(phi);
        ul_free(pnum);
        return ODB_SAVE_ERROR;
    }
    fchmod(fno,0644);

    int times = 0;
    int i=0;
    int j = 0;
    int ioff=0;
    for (i = 0; i < h_num; i++) {
        phi[i] = ioff;
        _inl_sodict_bnode_t *pnode = pbh[i].pnode;
        while (pnode != NULL) {
            if (odbi_is_node_deleted(pnode)) {
                pnode = pnode->next;
                continue;
            }
            
            ds[j].sign1 = pnode->sign1;
            ds[j].sign2 = pnode->sign2;
            
            ((sodict_snode_atomic_t*)&ds[j])->cuint=
                ((sodict_bnode_atomic_t*)pnode)->cuint;
            
            j++;
            pnum[i]++;
            ioff++;

            if (j >= ODB_MID_NODE_NUM) {
                wt_len = ODB_MID_NODE_NUM * snode;
                if (ul_write(fno, ds, wt_len) != wt_len) {
                    ul_writelog(UL_LOG_WARNING,"[ODICT ERROR] write data to file[%s] failed.", fullpath);
                    ul_free(phi);
                    ul_free(pnum);
                    ul_close(fno);
                    return ODB_SAVE_ERROR;
                }
                j = 0;
                times++;
            }
            
            pnode = pnode->next;
        }
    }

    if (j != 0) {
        wt_len = j * snode;
        if (ul_write(fno, ds, wt_len) != wt_len) {
            ul_writelog(UL_LOG_WARNING,"[ODICT ERROR] write data to file[%s] failed.", fullpath);
            ul_free(phi);
            ul_free(pnum);
            ul_close(fno);
            return ODB_SAVE_ERROR;
        }
    }

    ul_close(fno);

    
    sprintf(tname, "%s.ind1", filename);
    odbi_cmps_path(path, tname, fullpath, PATH_SIZE);
    if((fno = ul_open(fullpath, O_WRONLY|O_CREAT|O_TRUNC, 0644)) == -1) {
        ul_free(phi);
        ul_free(pnum);
        return ODB_SAVE_ERROR;
    }
    fchmod(fno, 0644);
    wt_len = h_num * sizeof(odict_offset_t);
    if (ul_write(fno, phi, wt_len) != wt_len) {
        ul_writelog(UL_LOG_WARNING,"[ODICT ERROR] write data1 to file[%s] failed.", fullpath);
        ul_free(phi);
        ul_free(pnum);
        ul_close(fno);
        return ODB_SAVE_ERROR;
    }
    wt_len = h_num * sizeof(odict_offset_t);
    if (ul_write(fno, pnum, wt_len) != wt_len) {
        ul_writelog(UL_LOG_WARNING,"[ODICT ERROR] write data2 to file[%s] failed.", fullpath);
        ul_free(phi);
        ul_free(pnum);
        ul_close(fno);
        return ODB_SAVE_ERROR;
    }

    ul_close(fno);

    
    if ( (times*ODB_MID_NODE_NUM+j != sdb->node_num) || (ioff != sdb->node_num) ) {
        ul_writelog(UL_LOG_WARNING, "[ODICT ERROR] node number error[%d=%d].", sdb->node_num, ioff);
        ul_free(phi);
        ul_free(pnum);
        return ODB_SAVE_ERROR;
    }

    
    ul_free(phi);
    ul_free(pnum);

    sdb->cur_code++;

    
    return ODB_SAVE_OK;
}

template<typename SIGN_TYPE,typename CUSTOM_TYPE>
    static int _odb_save_search(OdictSearch<SIGN_TYPE,CUSTOM_TYPE> *sdb, char *path, char *filename)
{
    typedef OdictSNode<SIGN_TYPE,CUSTOM_TYPE> _inl_sodict_snode_t;

    char fullpath[PATH_SIZE];
    char tname[PATH_SIZE];

    if (sdb == NULL || path == NULL || filename == NULL)
        return ODB_SAVE_ERROR;
    
    if (strlen(path) >= sizeof(tname)-2) {
        ul_writelog(UL_LOG_WARNING,"[ODICT ERROR] path too long %s",path);
        return ODB_SAVE_ERROR;
    }

    if (path[strlen(path) - 1] != '/') {
        snprintf(tname, sizeof(tname), "%s/", path);
    } else {
        snprintf(tname, sizeof(tname), "%s", path);
    }
    
    if (ul_writenum_init(tname, filename) == 1) {
        ul_writenum_oneint(tname, filename, "hash_num", sdb->hash_num);
        ul_writenum_oneint(tname, filename, "node_num", sdb->node_num);
        ul_writenum_oneint(tname, filename, "cur_code", sdb->cur_code);
    } else {
        return ODB_SAVE_ERROR;
    }

    snprintf(tname, sizeof(tname), "%s.ind1", filename);
    odbi_cmps_path(path, tname, fullpath, PATH_SIZE);
    int fno=-1;
    if ((fno = ul_open(fullpath, O_WRONLY | O_TRUNC | O_CREAT, 0644)) == -1) {
        return ODB_SAVE_ERROR;
    }
    ssize_t wt_len = sdb->hash_num*sizeof(odict_offset_t);
    if (ul_write(fno, sdb->hash, wt_len) != wt_len) {
        ul_writelog(UL_LOG_WARNING, "[ODICT ERROR] odb_save_search write hash to file[%s] failed.", fullpath);
        ul_close(fno);
        return ODB_SAVE_ERROR;
    }

    wt_len = sdb->hash_num*sizeof(odict_offset_t);

    if (ul_write(fno, sdb->num, wt_len) != wt_len) {
        ul_writelog(UL_LOG_WARNING, "[ODICT ERROR] odb_save_search write num to file[%s] failed.", fullpath);
        ul_close(fno);
        return ODB_SAVE_ERROR;
    }
    ul_close(fno);

    snprintf(tname, sizeof(tname), "%s.ind2", filename);
    odbi_cmps_path(path, tname, fullpath, PATH_SIZE);
    if ((fno = ul_open(fullpath, O_WRONLY | O_TRUNC | O_CREAT, 0644)) == -1) {
        return ODB_SAVE_ERROR;;
    }
    wt_len = sdb->node_num*sizeof(_inl_sodict_snode_t);
    if (ul_write(fno, sdb->node, wt_len) != wt_len) {
        ul_writelog(UL_LOG_WARNING, "[ODICT ERROR] odb_save_search write node to file[%s] failed.", fullpath);
        ul_close(fno);
        return ODB_SAVE_ERROR;
    }
    ul_close(fno);

    return ODB_SAVE_OK;
}

template<typename SIGN_TYPE,typename CUSTOM_TYPE>
    static int _odb_save_search_safely(OdictSearch<SIGN_TYPE,CUSTOM_TYPE> *sdb, char *path, char *filename)
{
    char sname[PATH_SIZE];
    char dname[PATH_SIZE];

    if (sdb == NULL || path == NULL || filename == NULL) {
        return ODB_SAVE_ERROR;
    }

    int need_protect = odbi_is_dictionary_exist(path, filename);
    if (need_protect) {
        snprintf(sname, sizeof(sname), "%s.n", filename);
        snprintf(dname, sizeof(dname), ".odb.ss.%s.n", filename);
        if (is_file_exist(path, sname)) {
            if (ul_mvfile(path, sname, dname)) {
                ul_writelog(UL_LOG_WARNING, "[ODICT ERROR] cannot move file from [%s] to [%s].", sname, dname);
                return ODB_SAVE_ERROR;
            }
        } else {
            ul_writelog(UL_LOG_WARNING,"[ODICT ERROR] file [%s/%s] not exist.", path,sname);
        }

        snprintf(sname, sizeof(sname), "%s.ind1", filename);
        snprintf(dname, sizeof(dname), ".odb.ss.%s.ind1", filename);
        if (is_file_exist(path, sname)) {
            if (ul_mvfile(path, sname, dname)) {
                ul_writelog(UL_LOG_WARNING,"[ODICT ERROR] cannot move file from [%s] to [%s].", sname, dname);
                return ODB_SAVE_ERROR;
            }
        } else {
            ul_writelog(UL_LOG_WARNING,"[ODICT ERROR] file [%s/%s] not exist.", path,sname);
        }

        snprintf(sname, sizeof(sname), "%s.ind2", filename);
        snprintf(dname, sizeof(dname), ".odb.ss.%s.ind2", filename);
        if (is_file_exist(path, sname)) {
            if (ul_mvfile(path, sname, dname)) {
                ul_writelog(UL_LOG_WARNING,"[ODICT ERROR] cannot move file from [%s] to [%s].", sname, dname);
                return ODB_SAVE_ERROR;
            }
        } else {
            ul_writelog(UL_LOG_WARNING,"[ODICT ERROR] file [%s/%s] not exist.", path,sname);
        }
    }
    
    int save_ret = _odb_save_search(sdb, path, filename);

    if (need_protect) {

        if (save_ret == ODB_SAVE_OK) {
            snprintf(dname, sizeof(dname), ".odb.ss.%s.n", filename);
            remove(dname);
            snprintf(dname, sizeof(dname), ".odb.ss.%s.ind1", filename);
            remove(dname);
            snprintf(dname, sizeof(dname), ".odb.ss.%s.ind2", filename);
            remove(dname);
        } else {
            snprintf(dname, sizeof(dname), "%s.n", filename);
            snprintf(sname, sizeof(sname), ".odb.ss.%s.n", filename);
            if (ul_mvfile(path, sname, dname) != 0) {
                ul_writelog(UL_LOG_WARNING,"[ODICT ERROR] cannot move file from [%s] to [%s], lost possibly.",
                            sname, dname);
            }
            snprintf(dname, sizeof(dname), "%s.ind1", filename);
            snprintf(sname, sizeof(sname), ".odb.ss.%s.ind1", filename);
            if (ul_mvfile(path, sname, dname) != 0) {
                ul_writelog(UL_LOG_WARNING,"[ODICT ERROR] cannot move file from [%s] to [%s], lost possibly.",
                            sname, dname);
            }
            snprintf(dname, sizeof(dname), "%s.ind2", filename);
            snprintf(sname, sizeof(sname), ".odb.ss.%s.ind2", filename);
            if (ul_mvfile(path, sname, dname) != 0) {
                ul_writelog(UL_LOG_WARNING,"[ODICT ERROR] cannot move file from [%s] to [%s], lost possibly.",
                            sname, dname);
            }
        }
    }

    
    return save_ret;

}



template<typename SIGN_TYPE,typename CUSTOM_TYPE>
    static int _odb_save_safely(OdictBuild<SIGN_TYPE,CUSTOM_TYPE> *sdb, char *path, char *filename)
{
    char sname[PATH_SIZE], dname[PATH_SIZE];
    
    assert(sdb != NULL);
    assert(path != NULL);
    assert(filename != NULL);

    int need_protect = odbi_is_dictionary_exist(path, filename);
    if (need_protect) {
        snprintf(sname, sizeof(sname), "%s.n", filename);
        snprintf(dname, sizeof(dname), ".odb.ss.%s.n", filename);
        if (is_file_exist(path,sname)) {
            if (0 != ul_mvfile(path, sname, dname)) {
                ul_writelog(UL_LOG_WARNING,"[ODICT ERROR] cannot move file from [%s] to [%s].", sname, dname);
                return ODB_SAVE_ERROR;
            }
        } else {
            ul_writelog(UL_LOG_WARNING,"[ODICT ERROR] file [%s/%s] not exist.", path,sname);
        }
        snprintf(sname, sizeof(sname), "%s.ind1", filename);
        snprintf(dname, sizeof(dname), ".odb.ss.%s.ind1", filename);
        if (is_file_exist(path,sname)) {
            if (0 != ul_mvfile(path, sname, dname)) {
                ul_writelog(UL_LOG_WARNING,"[ODICT ERROR] cannot move file from [%s] to [%s].", sname, dname);
                return ODB_SAVE_ERROR;
            }
        } else {
            ul_writelog(UL_LOG_WARNING,"[ODICT ERROR] file [%s/%s] not exist.", path,sname);
        }
        snprintf(sname, sizeof(sname), "%s.ind2", filename);
        snprintf(dname, sizeof(dname), ".odb.ss.%s.ind2", filename);
        if (is_file_exist(path,sname)) {
            if (0 != ul_mvfile(path, sname, dname)) {
                ul_writelog(UL_LOG_WARNING,"[ODICT ERROR] cannot move file from [%s] to [%s].", sname, dname);
                return ODB_SAVE_ERROR;
            }
        } else {
            ul_writelog(UL_LOG_WARNING,"[ODICT ERROR] file [%s/%s] not exist.", path,sname);
        }
    }

    int save_ret = _odb_save(sdb, path, filename);

    if (need_protect) {
        if (save_ret == ODB_SAVE_OK) {
            snprintf(dname, sizeof(dname), ".odb.ss.%s.n", filename);
            remove(dname);
            snprintf(dname, sizeof(dname), ".odb.ss.%s.ind1", filename);
            remove(dname);
            snprintf(dname, sizeof(dname), ".odb.ss.%s.ind2", filename);
            remove(dname);
        } else {
            snprintf(dname, sizeof(dname), "%s.n", filename);
            snprintf(sname, sizeof(sname), ".odb.ss.%s.n", filename);
            if (0 != ul_mvfile(path, sname, dname)) {
                ul_writelog(UL_LOG_WARNING,"[ODICT ERROR] cannot move file from [%s] to [%s], lost possibly.",
                            sname, dname);
            }
            snprintf(dname, sizeof(dname), "%s.ind1", filename);
            snprintf(sname, sizeof(sname), ".odb.ss.%s.ind1", filename);
            if (0 != ul_mvfile(path, sname, dname)) {
                ul_writelog(UL_LOG_WARNING,"[ODICT ERROR] cannot move file from [%s] to [%s], lost possibly.",
                            sname, dname);
            }
            snprintf(dname, sizeof(dname), "%s.ind2", filename);
            snprintf(sname, sizeof(sname), ".odb.ss.%s.ind2", filename);
            if (0 != ul_mvfile(path, sname, dname)) {
                ul_writelog(UL_LOG_WARNING,"[ODICT ERROR] cannot move file from [%s] to [%s], lost possibly.",
                            sname, dname);
            }
        }
    }

    
    return save_ret;
}


template<typename SIGN_TYPE,typename CUSTOM_TYPE>
    static int _odb_renew(OdictBuild<SIGN_TYPE,CUSTOM_TYPE> *sdb)
{
    typedef OdictBNodeBlock<SIGN_TYPE,CUSTOM_TYPE> _inl_sodict_bnode_block_t;
    
    assert(sdb != NULL);
    
    for(int i = 0 ; i < sdb->hash_num ; i++) {
        sdb->hash[i].pnode = NULL;
    }

    _inl_sodict_bnode_block_t *block = sdb->nblock;
    if(block != NULL) {
        _inl_sodict_bnode_block_t *pblock = block;
        block = block->next;
        pblock->next = NULL;
        while (block != NULL) {
            pblock = block;
            block = pblock->next;
            free(pblock);
        }
    }

    sdb->cur_code = 0;
    sdb->node_num = 0;
    sdb->cur_nblock = sdb->nblock;
    sdb->cur_node = sdb->nblock->nodes;
    sdb->node_avail = ODB_NODE_BLOCK_NUM;

    

    return ODB_RENEW_OK;
}

template<typename SIGN_TYPE,typename CUSTOM_TYPE>
    static int _odb_renew_ex(OdictBuild<SIGN_TYPE,CUSTOM_TYPE> *sdb)
{
    return _odb_renew(sdb);
}


template<typename SIGN_TYPE,typename CUSTOM_TYPE>
    static int _odb_add(OdictBuild<SIGN_TYPE,CUSTOM_TYPE>* sdb, OdictSNode<SIGN_TYPE,CUSTOM_TYPE> *snode, int overwrite_if_exists)
{
    typedef OdictBNode<SIGN_TYPE,CUSTOM_TYPE> _inl_sodict_bnode_t;
    typedef OdictBNodeBlock<SIGN_TYPE,CUSTOM_TYPE> _inl_sodict_bnode_block_t;   
    typedef OdictSNodeAtomic<SIGN_TYPE,CUSTOM_TYPE> sodict_snode_atomic_t;
    typedef OdictBNodeAtomic<SIGN_TYPE,CUSTOM_TYPE> sodict_bnode_atomic_t;
    
    assert(sdb != NULL);
    assert(snode != NULL);

    
    if(snode->sign1 == 0 && snode->sign2 == 0) {
        ul_writelog(UL_LOG_WARNING, "[ODICT ERROR] Invalid node: both the signatures are zero");
        return ODB_ADD_INVALID;
    }

    int hkey = (snode->sign1 + snode->sign2)%sdb->hash_num;
    
    _inl_sodict_bnode_t *pt_node = sdb->hash[hkey].pnode;
    _inl_sodict_bnode_t *pnode = NULL, *pfree = NULL;
    
    while(pt_node != NULL) {
        if ((pt_node->sign1 == snode->sign1) && (pt_node->sign2 == snode->sign2)) {
            pnode = pt_node;
            break;
        }
        if ((pfree == NULL) && (pt_node->sign1 == 0) && (pt_node->sign2 == 0)) {
            pfree = pt_node;
        }
        pt_node = pt_node->next;
    }

    if (pnode != NULL) {
        if (overwrite_if_exists == 1) {
            ((sodict_bnode_atomic_t*)pnode)->cuint=
                ((sodict_snode_atomic_t*)snode)->cuint;
            sdb->cur_code++;
            return ODB_ADD_OVERWRITE;
        } else {
            return ODB_ADD_EXISTS;
        }
    } else {
        if (pfree != NULL) {
            pnode = pfree;
            sdb->node_num++;
            pnode->sign1 = snode->sign1;
            pnode->sign2 = snode->sign2;
            ((sodict_bnode_atomic_t*)pnode)->cuint=
                ((sodict_snode_atomic_t*)snode)->cuint;
        } else {
            
            if (sdb->node_avail == 0) {
                sdb->cur_nblock->next = (_inl_sodict_bnode_block_t*)ul_calloc(1, sizeof(_inl_sodict_bnode_block_t));
                if (NULL == sdb->cur_nblock->next) {
                    ul_writelog(UL_LOG_WARNING,"[ODICT ERROR] cannot allocate memory for node block[%zu].",
                                sizeof(_inl_sodict_bnode_block_t));
                    return ODB_ADD_ERROR;
                }

                sdb->cur_nblock = sdb->cur_nblock->next;
                sdb->cur_node = sdb->cur_nblock->nodes;
                sdb->node_avail = ODB_NODE_BLOCK_NUM;

            }

            
            pnode = sdb->cur_node;
            pnode->next = NULL;
            pnode->sign1 = snode->sign1;
            pnode->sign2 = snode->sign2;
            ((sodict_bnode_atomic_t*)pnode)->cuint=
                ((sodict_snode_atomic_t*)snode)->cuint;

            
            if (sdb->hash[hkey].pnode == NULL) {
                sdb->hash[hkey].pnode = pnode;
            } else {
                pnode->next = sdb->hash[hkey].pnode;
                sdb->hash[hkey].pnode = pnode;
            }

            sdb->node_avail--;
            sdb->node_num++;
            
            if (sdb->node_avail == 0) {
                sdb->cur_nblock->next = (_inl_sodict_bnode_block_t*)ul_calloc(1, sizeof(_inl_sodict_bnode_block_t));
                if (NULL == sdb->cur_nblock->next) {
                    ul_writelog(UL_LOG_WARNING,"[ODICT ERROR] cannot prepare memory for node block[%zu].",
                                sizeof(_inl_sodict_bnode_block_t));
                } else {
                    sdb->cur_nblock = sdb->cur_nblock->next;
                    sdb->cur_node = sdb->cur_nblock->nodes;
                    sdb->node_avail = ODB_NODE_BLOCK_NUM;
                }
            } else {
                
                sdb->cur_node++;
            }
        }
    }
    sdb->cur_code++;

    
    return ODB_ADD_OK;
}


template<typename SIGN_TYPE,typename CUSTOM_TYPE>
    static int _odb_del(OdictBuild<SIGN_TYPE,CUSTOM_TYPE> *sdb, OdictSNode<SIGN_TYPE,CUSTOM_TYPE> *snode)
{
    typedef OdictBNode<SIGN_TYPE,CUSTOM_TYPE> _inl_sodict_bnode_t;
    
    assert(sdb != NULL);
    assert(snode != NULL);

    
    if (0 == snode->sign1 && 0 == snode->sign2) {
        ul_writelog(UL_LOG_WARNING, "[ODICT ERROR] [%s] Invalid node: both the signatures are zero",
                    __func__);
        return ODB_DEL_ERROR;
    }

    int hkey = (snode->sign1 + snode->sign2)%sdb->hash_num;

    _inl_sodict_bnode_t *pt_node = sdb->hash[hkey].pnode;
    _inl_sodict_bnode_t *pnode=NULL;
    while(pt_node != NULL) {
        if ((pt_node->sign1 == snode->sign1) && (pt_node->sign2 == snode->sign2)) {
            pnode = pt_node;
            break;
        }
        pt_node = pt_node->next;
    }

    if (NULL != pnode) {
        pnode->sign1 = 0;
        pnode->sign2 = 0;
        sdb->node_num--;
        sdb->cur_code++;
        return ODB_DEL_OK;
    } else {
        return ODB_DEL_NOT_EXISTS;
    }
}


template<typename SIGN_TYPE,typename CUSTOM_TYPE>
    static int _odb_seek(OdictBuild<SIGN_TYPE,CUSTOM_TYPE> *sdb, OdictSNode<SIGN_TYPE,CUSTOM_TYPE> *snode)
{
    typedef OdictBNode<SIGN_TYPE,CUSTOM_TYPE> _inl_sodict_bnode_t;
    typedef OdictSNodeAtomic<SIGN_TYPE,CUSTOM_TYPE> sodict_snode_atomic_t;
    typedef OdictBNodeAtomic<SIGN_TYPE,CUSTOM_TYPE> sodict_bnode_atomic_t;
    
    assert(sdb != NULL);
    assert(snode != NULL);

    sdb->cur_code++;

    int hkey = (snode->sign1 + snode->sign2)%sdb->hash_num;
    
    _inl_sodict_bnode_t *pt_node = sdb->hash[hkey].pnode;
    _inl_sodict_bnode_t *pnode = NULL;
    while(pt_node != NULL) {
        if ((pt_node->sign1 == snode->sign1) && (pt_node->sign2 == snode->sign2)) {
            pnode = pt_node;
            break;
        }
        pt_node = pt_node->next;
    }

    if (NULL != pnode) {
        ((sodict_snode_atomic_t*)snode)->cuint=
            ((sodict_bnode_atomic_t*)pnode)->cuint; 
        return ODB_SEEK_OK;
    } else {
        return ODB_SEEK_FAIL;
    }
}


template<typename SIGN_TYPE,typename CUSTOM_TYPE>
    static int _odb_seek_search(OdictSearch<SIGN_TYPE,CUSTOM_TYPE> *sdb, OdictSNode<SIGN_TYPE,CUSTOM_TYPE> *snode)
{
    typedef OdictSNode<SIGN_TYPE,CUSTOM_TYPE> _inl_sodict_snode_t;
    typedef OdictSNodeAtomic<SIGN_TYPE,CUSTOM_TYPE> sodict_snode_atomic_t;
    typedef OdictBNodeAtomic<SIGN_TYPE,CUSTOM_TYPE> sodict_bnode_atomic_t;
    
    assert(sdb != NULL);
    assert(snode != NULL);

    int hkey = (snode->sign1 + snode->sign2)%sdb->hash_num;
    _inl_sodict_snode_t *pnode = NULL;
    for (int i = 0 ; (odict_offset_t)i < sdb->num[hkey] ; i++) {
        int index = sdb->hash[hkey] + i;
        _inl_sodict_snode_t *pt_node = &(sdb->node[index]);
        if ((pt_node->sign1 == snode->sign1) && (pt_node->sign2 == snode->sign2)) {
            pnode = pt_node;
            break;
        }
    }
    if (NULL != pnode) {
        ((sodict_snode_atomic_t*)snode)->cuint=
            ((sodict_bnode_atomic_t*)pnode)->cuint; 
        return ODB_SEEK_OK;
    } else {
        return ODB_SEEK_FAIL;
    }
}



template<typename SIGN_TYPE,typename CUSTOM_TYPE>
    static int _odb_mod(OdictBuild<SIGN_TYPE,CUSTOM_TYPE> *sdb, OdictSNode<SIGN_TYPE,CUSTOM_TYPE> *snode)
{
    typedef OdictBNode<SIGN_TYPE,CUSTOM_TYPE> _inl_sodict_bnode_t;
    typedef OdictSNodeAtomic<SIGN_TYPE,CUSTOM_TYPE> sodict_snode_atomic_t;
    typedef OdictBNodeAtomic<SIGN_TYPE,CUSTOM_TYPE> sodict_bnode_atomic_t;
    
    assert(sdb != NULL);
    assert(snode != NULL);

    int hkey = (snode->sign1 + snode->sign2)%sdb->hash_num;
    
    _inl_sodict_bnode_t *pt_node = sdb->hash[hkey].pnode;
    _inl_sodict_bnode_t *pnode = NULL;
    while(pt_node != NULL) {
        if ((pt_node->sign1 == snode->sign1) && (pt_node->sign2 == snode->sign2)) {
            pnode = pt_node;
            break;
        }
        pt_node = pt_node->next;
    }

    if (NULL != pnode) {
        ((sodict_bnode_atomic_t*)pnode)->cuint=
            ((sodict_snode_atomic_t*)snode)->cuint;
        sdb->cur_code++;
        return ODB_MOD_OK;
    } else {
        return ODB_MOD_FAIL;
    }
}


template<typename SIGN_TYPE,typename CUSTOM_TYPE>
    static void _odb_traverse(OdictBuild<SIGN_TYPE,CUSTOM_TYPE> *sdb, int include_deleted,
                              void (*traverse_handler)(OdictSNode<SIGN_TYPE,CUSTOM_TYPE> *snode, int *stop_now))
{
    typedef OdictBNode<SIGN_TYPE,CUSTOM_TYPE> _inl_sodict_bnode_t;
    typedef OdictSNode<SIGN_TYPE,CUSTOM_TYPE> _inl_sodict_snode_t;
    typedef OdictSNodeAtomic<SIGN_TYPE,CUSTOM_TYPE> sodict_snode_atomic_t;
    typedef OdictBNodeAtomic<SIGN_TYPE,CUSTOM_TYPE> sodict_bnode_atomic_t;
    
    assert(sdb);
    
    for (int i = 0; i < sdb->hash_num; i++) {
        _inl_sodict_bnode_t *pnode = sdb->hash[i].pnode;
        if (NULL != pnode) {
            while (pnode != NULL) {
                if (include_deleted != 1) {
                    if (odbi_is_node_deleted(pnode)) {
                        pnode = pnode->next;
                        continue;
                    }
                }
                _inl_sodict_snode_t snode;
                snode.sign1 = pnode->sign1;
                snode.sign2 = pnode->sign2;
                ((sodict_snode_atomic_t*)&snode)->cuint=
                    ((sodict_bnode_atomic_t*)pnode)->cuint; 
                
                int stop_now = 0;
                traverse_handler(&snode, &stop_now);
                if (stop_now == 1)
                    return;

                pnode = pnode->next;
            }
        }
    }
}

template<typename SIGN_TYPE,typename CUSTOM_TYPE>
    static void _odb_traverse_ex(OdictBuild<SIGN_TYPE,CUSTOM_TYPE> *sdb, int include_deleted,
                                 void (*traverse_handler)(OdictSNode<SIGN_TYPE,CUSTOM_TYPE> *snode, int *stop_now, void *arg), 
                                 void *ex_arg)
{
    typedef OdictBNode<SIGN_TYPE,CUSTOM_TYPE> _inl_sodict_bnode_t;
    typedef OdictSNode<SIGN_TYPE,CUSTOM_TYPE> _inl_sodict_snode_t;   
    typedef OdictSNodeAtomic<SIGN_TYPE,CUSTOM_TYPE> sodict_snode_atomic_t;
    typedef OdictBNodeAtomic<SIGN_TYPE,CUSTOM_TYPE> sodict_bnode_atomic_t;
    
    assert(sdb);

    for (int i = 0; i < sdb->hash_num; i++) {
        _inl_sodict_bnode_t *pnode = sdb->hash[i].pnode;
        if (NULL != pnode) {
            while (pnode != NULL) {
                if (include_deleted != 1) {
                    if (odbi_is_node_deleted(pnode)) {
                        pnode = pnode->next;
                        continue;
                    }
                }
                _inl_sodict_snode_t snode;
                snode.sign1 = pnode->sign1;
                snode.sign2 = pnode->sign2;
                ((sodict_snode_atomic_t*)&snode)->cuint=
                    ((sodict_bnode_atomic_t*)pnode)->cuint; 

                int stop_now = 0;
                traverse_handler(&snode, &stop_now, ex_arg);
                if (stop_now == 1)
                    return;

                pnode = pnode->next;
            }
        }
    }
}

template<typename SIGN_TYPE,typename CUSTOM_TYPE>
    static void _odb_traverse_search(OdictSearch<SIGN_TYPE,CUSTOM_TYPE> *sdb, 
                                     void (*traverse_handler)(OdictSNode<SIGN_TYPE,CUSTOM_TYPE> *snode, int *stop_now))
{
    assert(sdb);
    
    for (int i = 0 ; i < sdb->hash_num ; i++) {
        if (sdb->num[i] <= 0)
            continue;
        for (int j = 0 ; (odict_offset_t)j < sdb->num[i] ; j++) {
            int stop_now = 0;
            traverse_handler(&(sdb->node[sdb->hash[i] + j]), &stop_now);
            if (stop_now == 1)
                return;
        }
    }
}

template<typename SIGN_TYPE,typename CUSTOM_TYPE>
    static void _odb_traverse_search_ex(OdictSearch<SIGN_TYPE,CUSTOM_TYPE> *sdb, 
                                        void (*traverse_handler)(OdictSNode<SIGN_TYPE,CUSTOM_TYPE> *snode, int *stop_now,void *arg),
                                        void *ex_arg)
{
    assert(sdb);

    for (int i = 0 ; i < sdb->hash_num ; i++) {
        if (sdb->num[i] <= 0)
            continue;
        for (int j = 0 ; (odict_offset_t)j < sdb->num[i] ; j++) {
            int stop_now = 0;
            traverse_handler(&(sdb->node[sdb->hash[i] + j]), &stop_now,ex_arg);
            if (stop_now == 1)
                return;
        }
    }
}


template<typename SIGN_TYPE,typename CUSTOM_TYPE>
    static void report_node(OdictSNode<SIGN_TYPE,CUSTOM_TYPE> *node, int * )
{
    
    std::cerr<<"sign1: "<<node->sign1<<std::endl;
    std::cerr<<"sign2: "<<node->sign2<<std::endl;
    std::cerr<<"custom data1: "<<node->cuint1<<std::endl;
    std::cerr<<"custom data2: "<<node->cuint2<<std::endl;
}

template<typename SIGN_TYPE,typename CUSTOM_TYPE>
    static void _odb_build_report(OdictBuild<SIGN_TYPE,CUSTOM_TYPE> *sdb)
{
    _odb_traverse(sdb, 0, report_node);
}

template<typename SIGN_TYPE,typename CUSTOM_TYPE>
    static void _odb_search_report(OdictSearch<SIGN_TYPE,CUSTOM_TYPE> *sdb)
{
    _odb_traverse_search(sdb, report_node);
}

template<typename SIGN_TYPE,typename CUSTOM_TYPE>
    static int _odb_get_nodenum(OdictBuild<SIGN_TYPE,CUSTOM_TYPE> *sdb)
{
    if (sdb) {
        return sdb->node_num;
    }
    return 0;
}

template<typename SIGN_TYPE,typename CUSTOM_TYPE>
    static int _odb_get_hashnum(OdictBuild<SIGN_TYPE,CUSTOM_TYPE> *sdb)
{
    if (sdb) {
        return sdb->hash_num;
    }
    return 0;
}

template<typename SIGN_TYPE,typename CUSTOM_TYPE>
    static int _odb_search_get_nodenum(OdictSearch<SIGN_TYPE,CUSTOM_TYPE> *sdb)
{
    if (sdb) {
        return sdb->node_num;
    }
    return 0;
}

template<typename SIGN_TYPE,typename CUSTOM_TYPE>
    static int _odb_search_get_hashnum(OdictSearch<SIGN_TYPE,CUSTOM_TYPE> *sdb)
{
    if (sdb) {
        return sdb->hash_num;
    }
    return 0;
}

template<typename SIGN_TYPE,typename CUSTOM_TYPE>
    static int _odb_adjust(OdictBuild<SIGN_TYPE,CUSTOM_TYPE> *sdb)
{
    typedef OdictBHash<SIGN_TYPE,CUSTOM_TYPE> _inl_sodict_bhash_t;
    typedef OdictBNodeBlock<SIGN_TYPE,CUSTOM_TYPE> _inl_sodict_bnode_block_t;
    typedef OdictBNode<SIGN_TYPE,CUSTOM_TYPE> _inl_sodict_bnode_t;

    if (!sdb) {
        return -1;
    }
    
    int n_num = sdb->node_num;
    int h_num = n_num/2;
    if ( (h_num % 2) == 0 ) {
        h_num++;
    }
    
    _inl_sodict_bhash_t *hash = (_inl_sodict_bhash_t *)ul_calloc(1,h_num*sizeof(_inl_sodict_bhash_t));
    if (!hash) {
        return -1;
    }

    if (sdb->hash) {
        ul_free(sdb->hash);
        sdb->hash = NULL;
    }
    sdb->hash = hash;
    
    sdb->hash_num = h_num;

    int hkey = 0;
    _inl_sodict_bnode_block_t *block = sdb->nblock;
    while(block != NULL) {
        int times = ODB_NODE_BLOCK_NUM;
        if (block->next==NULL) {
            times -= sdb->node_avail;
        }
        _inl_sodict_bnode_t *node = block->nodes;
        for (int i = 0; i < times; i ++) {
            if (node[i].sign1 == 0 && node[i].sign2 == 0) {
                
                hkey = (hkey+1) % sdb->hash_num;
            } else {
                hkey = (node[i].sign1 + node[i].sign2) % sdb->hash_num;
            }
            if (sdb->hash[hkey].pnode == NULL) {
                node[i].next = NULL;
                sdb->hash[hkey].pnode = &node[i];
            } else {
                node[i].next = sdb->hash[hkey].pnode;
                sdb->hash[hkey].pnode = &node[i];
            }
        }
        block = block->next;
    }
    return 0;
}

template<typename SIGN_TYPE,typename CUSTOM_TYPE>
    static int _odb_is_equal_search(OdictSearch<SIGN_TYPE,CUSTOM_TYPE> *sdb1, OdictSearch<SIGN_TYPE,CUSTOM_TYPE> *sdb2)
{
    typedef OdictSNode<SIGN_TYPE,CUSTOM_TYPE> _inl_sodict_snode_t;

    if (NULL == sdb1 || NULL == sdb2) {
        return 0;
    }
    
    if (NULL == sdb1->hash || NULL == sdb2->hash ||
        NULL == sdb1->num || NULL == sdb2->num ||
        NULL == sdb1->node || NULL == sdb2->node) {
        return 0;
    }
    
    if (sdb1->hash_num != sdb2->hash_num ||
        sdb1->node_num != sdb2->node_num) {
        return 0;
    }
    
    if (memcmp(sdb1->hash, sdb2->hash, sdb1->hash_num*sizeof(odict_offset_t)) != 0 ||
        memcmp(sdb1->num, sdb2->num, sdb1->hash_num*sizeof(odict_offset_t)) != 0  ||
        memcmp(sdb1->node, sdb2->node, sdb1->node_num*sizeof(_inl_sodict_snode_t)) != 0) {
        return 0;
    }
    
    return 1;
}




typedef OdictSNode<sodict_build_t::SIGN,sodict_build_t::CUSTOM> _inl_snode_t;
typedef void (*_inl_traverse_handler_t)(OdictSNode<sodict_build_t::SIGN,sodict_build_t::CUSTOM>*snode,int *stop_now);
typedef void (*_inl_traverse_handler_ex_t)(OdictSNode<sodict_build_t::SIGN,sodict_build_t::CUSTOM>*snode,int *stop_now,void *arg);

sodict_build_t *odb_creat(int hash_num){
    return (sodict_build_t*)_odb_creat<sodict_build_t::SIGN,sodict_build_t::CUSTOM>(hash_num);
}
sodict_build_t *odb_load(char *path, char *filename, int hash_num){
    return (sodict_build_t*)_odb_load<sodict_build_t::SIGN,sodict_build_t::CUSTOM>(path,filename,hash_num);
}
sodict_search_t *odb_load_search(char *path, char *filename){
    return (sodict_search_t*)_odb_load_search<sodict_build_t::SIGN,sodict_build_t::CUSTOM>(path,filename);
}
sodict_search_t *odb_creat_search(int hash_num,int node_num){
    return (sodict_search_t*)_odb_creat_search<sodict_build_t::SIGN,sodict_build_t::CUSTOM>(hash_num,node_num);
}
sodict_search_t *odb_load_search_mmap(char *path, char *filename){
    return (sodict_search_t*)_odb_load_search_mmap<sodict_build_t::SIGN,sodict_build_t::CUSTOM>(path,filename);
}
int odb_destroy_search_mmap(sodict_search_t *sdb){
    return _odb_destroy_search_mmap<sodict_build_t::SIGN,sodict_build_t::CUSTOM>(sdb);
}
int odb_destroy(sodict_build_t *sdb){
    return _odb_destroy<sodict_build_t::SIGN,sodict_build_t::CUSTOM>(sdb);
}
int odb_destroy_search(sodict_search_t *sdb){
    return _odb_destroy_search<sodict_build_t::SIGN,sodict_build_t::CUSTOM>(sdb);
}
int odb_save(sodict_build_t *sdb, char *path, char *filename){
    return _odb_save<sodict_build_t::SIGN,sodict_build_t::CUSTOM>(sdb,path,filename);
}
int odb_save_search(sodict_search_t *sdb, char *path, char *filename){
    return _odb_save_search<sodict_build_t::SIGN,sodict_build_t::CUSTOM>(sdb,path,filename);
}
int odb_save_safely(sodict_build_t *sdb, char *path, char *filename){
    return _odb_save_safely<sodict_build_t::SIGN,sodict_build_t::CUSTOM>(sdb,path,filename);
}
int odb_save_search_safely(sodict_search_t *sdb, char *path, char *filename){
    return _odb_save_search_safely<sodict_build_t::SIGN,sodict_build_t::CUSTOM>(sdb,path,filename);
}
int odb_renew(sodict_build_t *sdb){
    return _odb_renew<sodict_build_t::SIGN,sodict_build_t::CUSTOM>(sdb);
}
int odb_renew_ex(sodict_build_t *sdb){
    return _odb_renew_ex<sodict_build_t::SIGN,sodict_build_t::CUSTOM>(sdb);
}
int odb_add(sodict_build_t *sdb, sodict_snode_t *snode, int overwrite_if_exists){
    _inl_snode_t *_snode=(_inl_snode_t*)snode;
    return _odb_add<sodict_build_t::SIGN,sodict_build_t::CUSTOM>(sdb,_snode,overwrite_if_exists);
}
int odb_del(sodict_build_t *sdb, sodict_snode_t *snode){
    _inl_snode_t *_snode=(_inl_snode_t*)snode;
    return _odb_del<sodict_build_t::SIGN,sodict_build_t::CUSTOM>(sdb,_snode);
}
int odb_seek(sodict_build_t *sdb, sodict_snode_t *snode){
    _inl_snode_t *_snode=(_inl_snode_t*)snode;
    return _odb_seek<sodict_build_t::SIGN,sodict_build_t::CUSTOM>(sdb,_snode);
}
int odb_seek_search(sodict_search_t *sdb, sodict_snode_t *snode){
    _inl_snode_t *_snode=(_inl_snode_t*)snode;   
    return _odb_seek_search<sodict_build_t::SIGN,sodict_build_t::CUSTOM>(sdb,_snode);
}
int odb_mod(sodict_build_t *sdb, sodict_snode_t *snode){
    _inl_snode_t *_snode=(_inl_snode_t*)snode;
    return _odb_mod<sodict_build_t::SIGN,sodict_build_t::CUSTOM>(sdb,_snode);
}
void odb_traverse(sodict_build_t *sdb, int include_deleted,
                  void (*traverse_handler)(sodict_snode_t *snode, int *stop_now)){
    _inl_traverse_handler_t _traverse_handler=(_inl_traverse_handler_t)traverse_handler;
    return _odb_traverse<sodict_build_t::SIGN,sodict_build_t::CUSTOM>(sdb,include_deleted,_traverse_handler);
}
void odb_traverse_ex(sodict_build_t *sdb, int include_deleted,
                     void (*traverse_handler)(sodict_snode_t *snode, int *stop_now, void *arg), void *ex_arg){
    _inl_traverse_handler_ex_t _traverse_handler=(_inl_traverse_handler_ex_t)traverse_handler;
    return _odb_traverse_ex<sodict_build_t::SIGN,sodict_build_t::CUSTOM>(sdb,include_deleted,_traverse_handler,ex_arg);
}
void odb_traverse_search(sodict_search_t *sdb, void (*traverse_handler)(sodict_snode_t *snode, int *stop_now)){
    _inl_traverse_handler_t _traverse_handler=(_inl_traverse_handler_t)traverse_handler;
    return _odb_traverse_search<sodict_build_t::SIGN,sodict_build_t::CUSTOM>(sdb,_traverse_handler);
}
void odb_traverse_search_ex(sodict_search_t *sdb, void (*traverse_handler)(sodict_snode_t *snode, int *stop_now,void *arg),
                            void *ex_arg){
    _inl_traverse_handler_ex_t _traverse_handler=(_inl_traverse_handler_ex_t)traverse_handler;
    return _odb_traverse_search_ex<sodict_build_t::SIGN,sodict_build_t::CUSTOM>(sdb,_traverse_handler,ex_arg);
}
void odb_build_report(sodict_build_t *sdb){
    return _odb_build_report<sodict_build_t::SIGN,sodict_build_t::CUSTOM>(sdb);
}
void odb_search_report(sodict_search_t *sdb){
    return _odb_search_report<sodict_search_t::SIGN,sodict_search_t::CUSTOM>(sdb);
}
int odb_get_nodenum(sodict_build_t *sdb){
    return _odb_get_nodenum<sodict_search_t::SIGN,sodict_search_t::CUSTOM>(sdb);
}
int odb_search_get_nodenum(sodict_search_t *sdb){
    return _odb_search_get_nodenum<sodict_search_t::SIGN,sodict_search_t::CUSTOM>(sdb);
}
int odb_get_hashnum(sodict_build_t *sdb){
    return _odb_get_hashnum<sodict_build_t::SIGN,sodict_build_t::CUSTOM>(sdb);
}
int odb_search_get_hashnum(sodict_search_t *sdb){
    return _odb_search_get_hashnum<sodict_build_t::SIGN,sodict_build_t::CUSTOM>(sdb);
}
int odb_adjust(sodict_build_t *sdb){
    return _odb_adjust<sodict_build_t::SIGN,sodict_build_t::CUSTOM>(sdb);
}
int odb_is_equal_search(sodict_search_t *sdb1, sodict_search_t *sdb2){
    return _odb_is_equal_search<sodict_build_t::SIGN,sodict_build_t::CUSTOM>(sdb1,sdb2);
}






typedef OdictSNode<sodict128_build_t::SIGN,sodict128_build_t::CUSTOM> _inl128_snode_t;
typedef void (*_inl128_traverse_handler_t)(OdictSNode<sodict128_build_t::SIGN,sodict128_build_t::CUSTOM>*snode,int *stop_now);
typedef void (*_inl128_traverse_handler_ex_t)(OdictSNode<sodict128_build_t::SIGN,sodict128_build_t::CUSTOM>*snode,int *stop_now,void *arg);

sodict128_build_t *odb128_creat(int hash_num){
    return (sodict128_build_t*)_odb_creat<sodict128_build_t::SIGN,sodict128_build_t::CUSTOM>(hash_num);
}
sodict128_build_t *odb128_load(char *path, char *filename, int hash_num){
    return (sodict128_build_t*)_odb_load<sodict128_build_t::SIGN,sodict128_build_t::CUSTOM>(path,filename,hash_num);
}
sodict128_search_t *odb128_load_search(char *path, char *filename){
    return (sodict128_search_t*)_odb_load_search<sodict128_build_t::SIGN,sodict128_build_t::CUSTOM>(path,filename);
}
sodict128_search_t *odb128_creat_search(int hash_num,int node_num){
    return (sodict128_search_t*)_odb_creat_search<sodict128_build_t::SIGN,sodict128_build_t::CUSTOM>(hash_num,node_num);
}
sodict128_search_t *odb128_load_search_mmap(char *path, char *filename){
    return (sodict128_search_t*)_odb_load_search_mmap<sodict128_build_t::SIGN,sodict128_build_t::CUSTOM>(path,filename);
}
int odb_destroy_search_mmap(sodict128_search_t *sdb){
    return _odb_destroy_search_mmap<sodict128_build_t::SIGN,sodict128_build_t::CUSTOM>(sdb);
}
int odb_destroy(sodict128_build_t *sdb){
    return _odb_destroy<sodict128_build_t::SIGN,sodict128_build_t::CUSTOM>(sdb);
}
int odb_destroy_search(sodict128_search_t *sdb){
    return _odb_destroy_search<sodict128_build_t::SIGN,sodict128_build_t::CUSTOM>(sdb);
}
int odb_save(sodict128_build_t *sdb, char *path, char *filename){
    return _odb_save<sodict128_build_t::SIGN,sodict128_build_t::CUSTOM>(sdb,path,filename);
}
int odb_save_search(sodict128_search_t *sdb, char *path, char *filename){
    return _odb_save_search<sodict128_build_t::SIGN,sodict128_build_t::CUSTOM>(sdb,path,filename);
}
int odb_save_safely(sodict128_build_t *sdb, char *path, char *filename){
    return _odb_save_safely<sodict128_build_t::SIGN,sodict128_build_t::CUSTOM>(sdb,path,filename);
}
int odb_save_search_safely(sodict128_search_t *sdb, char *path, char *filename){
    return _odb_save_search_safely<sodict128_build_t::SIGN,sodict128_build_t::CUSTOM>(sdb,path,filename);
}
int odb_renew(sodict128_build_t *sdb){
    return _odb_renew<sodict128_build_t::SIGN,sodict128_build_t::CUSTOM>(sdb);
}
int odb_renew_ex(sodict128_build_t *sdb){
    return _odb_renew_ex<sodict128_build_t::SIGN,sodict128_build_t::CUSTOM>(sdb);
}
int odb_add(sodict128_build_t *sdb, sodict128_snode_t *snode, int overwrite_if_exists){
    _inl128_snode_t *_snode=(_inl128_snode_t*)snode;
    return _odb_add<sodict128_build_t::SIGN,sodict128_build_t::CUSTOM>(sdb,_snode,overwrite_if_exists);
}
int odb_del(sodict128_build_t *sdb, sodict128_snode_t *snode){
    _inl128_snode_t *_snode=(_inl128_snode_t*)snode;
    return _odb_del<sodict128_build_t::SIGN,sodict128_build_t::CUSTOM>(sdb,_snode);
}
int odb_seek(sodict128_build_t *sdb, sodict128_snode_t *snode){
    _inl128_snode_t *_snode=(_inl128_snode_t*)snode;
    return _odb_seek<sodict128_build_t::SIGN,sodict128_build_t::CUSTOM>(sdb,_snode);
}
int odb_seek_search(sodict128_search_t *sdb, sodict128_snode_t *snode){   
    _inl128_snode_t *_snode=(_inl128_snode_t*)snode;
    return _odb_seek_search<sodict128_build_t::SIGN,sodict128_build_t::CUSTOM>(sdb,_snode);
}
int odb_mod(sodict128_build_t *sdb, sodict128_snode_t *snode){
    _inl128_snode_t *_snode=(_inl128_snode_t*)snode;
    return _odb_mod<sodict128_build_t::SIGN,sodict128_build_t::CUSTOM>(sdb,_snode);
}
void odb_traverse(sodict128_build_t *sdb, int include_deleted,
                  void (*traverse_handler)(sodict128_snode_t *snode, int *stop_now)){
    _inl128_traverse_handler_t _traverse_handler=(_inl128_traverse_handler_t)traverse_handler;
    return _odb_traverse<sodict128_build_t::SIGN,sodict128_build_t::CUSTOM>(sdb,include_deleted,_traverse_handler);
}
void odb_traverse_ex(sodict128_build_t *sdb, int include_deleted,
                     void (*traverse_handler)(sodict128_snode_t *snode, int *stop_now, void *arg), void *ex_arg){
    _inl128_traverse_handler_ex_t _traverse_handler=(_inl128_traverse_handler_ex_t)traverse_handler;
    return _odb_traverse_ex<sodict128_build_t::SIGN,sodict128_build_t::CUSTOM>(sdb,include_deleted,_traverse_handler,ex_arg);
}
void odb_traverse_search(sodict128_search_t *sdb, void (*traverse_handler)(sodict128_snode_t *snode, int *stop_now)){
    _inl128_traverse_handler_t _traverse_handler=(_inl128_traverse_handler_t)traverse_handler;
    return _odb_traverse_search<sodict128_build_t::SIGN,sodict128_build_t::CUSTOM>(sdb,_traverse_handler);
}
void odb_traverse_search_ex(sodict128_search_t *sdb, void (*traverse_handler)(sodict128_snode_t *snode, int *stop_now,void *arg),
                            void *ex_arg){
    _inl128_traverse_handler_ex_t _traverse_handler=(_inl128_traverse_handler_ex_t)traverse_handler;
    return _odb_traverse_search_ex<sodict128_build_t::SIGN,sodict128_build_t::CUSTOM>(sdb,_traverse_handler,ex_arg);
}
void odb_build_report(sodict128_build_t *sdb){
    return _odb_build_report<sodict128_build_t::SIGN,sodict128_build_t::CUSTOM>(sdb);
}
void odb_search_report(sodict128_search_t *sdb){
    return _odb_search_report<sodict128_search_t::SIGN,sodict128_search_t::CUSTOM>(sdb);
}
int odb_get_nodenum(sodict128_build_t *sdb){
    return _odb_get_nodenum<sodict128_search_t::SIGN,sodict128_search_t::CUSTOM>(sdb);
}
int odb_search_get_nodenum(sodict128_search_t *sdb){
    return _odb_search_get_nodenum<sodict128_search_t::SIGN,sodict128_search_t::CUSTOM>(sdb);
}
int odb_get_hashnum(sodict128_build_t *sdb){
    return _odb_get_hashnum<sodict128_build_t::SIGN,sodict128_build_t::CUSTOM>(sdb);
}
int odb_search_get_hashnum(sodict128_search_t *sdb){
    return _odb_search_get_hashnum<sodict128_build_t::SIGN,sodict128_build_t::CUSTOM>(sdb);
}
int odb_adjust(sodict128_build_t *sdb){
    return _odb_adjust<sodict128_build_t::SIGN,sodict128_build_t::CUSTOM>(sdb);
}
int odb_is_equal_search(sodict128_search_t *sdb1, sodict128_search_t *sdb2){
    return _odb_is_equal_search<sodict128_build_t::SIGN,sodict128_build_t::CUSTOM>(sdb1,sdb2);
}
