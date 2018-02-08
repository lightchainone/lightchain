#include "DictLoader.h"

namespace Lsc{  
 
    template<typename _Key, typename _Value, typename _Hash, typename _Node>
    static void process_node(_Node *snode, int* , void *arg){
        _Hash* map = static_cast<_Hash*>(arg);
        _Key key(snode->sign1, snode->sign2);
        _Value val(snode->cuint1, snode->cuint2);
        if(map->set(key, val) == -1){
            fprintf(stderr, "insert key(%llu,%llu) value (%u,%u) to hashmap failed.\n", 
                (unsigned long long)key.sign1, (unsigned long long)key.sign2, val.cuint1, val.cuint2);
        }
        return;
    }
 
    template<typename _Key, typename _Value, typename _Hash, typename _Dict, typename _Node>
    bool _load_odict(_Hash* map, _Dict* sdb){
        if(NULL == sdb){
            fprintf(stderr, "_load_odict: odict pointer can not be null.\n");
            return false;
        }
        if(NULL == map){         
            fprintf(stderr, "_load_odict: hashmap pointer can not be null.\n");
            return false;
        }
        if(map->create(odb_get_hashnum(sdb)) != 0){
            fprintf(stderr, "_load_odict: create hashmap failed.\n");
            return false;
        }
        odb_traverse_ex(sdb, 0, process_node<_Key, _Value, _Hash, _Node>, map);
        return true;
    }
    
    template<typename _Key, typename _Value, typename _Hash, typename _Dict, typename _Node>
    bool _load_odict_search(_Hash* map, _Dict* sdb){
        if(NULL == sdb){
            fprintf(stderr, "_load_odict_search: odict pointer can not be null.\n");
            return false;
        }
        if(NULL == map){         
            fprintf(stderr, "_load_odict_search: odict pointer can not be null.\n");
            return false;
        }
        if(map->create(odb_search_get_hashnum(sdb)) != 0){
            fprintf(stderr, "_load_odict: create hashmap failed.\n");
            return false;
        }
        odb_traverse_search_ex(sdb, process_node<_Key, _Value, _Hash, _Node>, map);  
        return true;
    }
    
    
    

    
    bool load_odict(odict_hashmap_t* map, sodict_build_t* sdb){
        return _load_odict<odict_key_t, odict_value_t, 
               odict_hashmap_t, sodict_build_t, sodict_snode_t>(map, sdb);
    }
     
    
    bool load_odict(odict_hashmap_t* map, sodict_search_t* sdb){
       return _load_odict_search<odict_key_t, odict_value_t, 
               odict_hashmap_t, sodict_search_t, sodict_snode_t>(map, sdb);
    }

    
    bool load_odict(odict_hashmap_t* map, const char* path, const char* filename, int hash_num ){
        if(NULL == path || NULL == filename){
            fprintf(stderr, "load_odict: path and filename should not be NULL.\n");
            return false;
        }
        sodict_build_t* sdb = odb_load(const_cast<char*>(path), const_cast<char*>(filename), hash_num);
        if(NULL == sdb || (unsigned long)ODB_LOAD_NOT_EXISTS == (unsigned long)sdb){
            fprintf(stderr, "load_odict: load odict file failed.\n");
            return false;
        }
        bool res = _load_odict<odict_key_t, odict_value_t, odict_hashmap_t, 
             sodict_build_t, sodict_snode_t>(map, sdb);
        res = res && (odb_destroy(sdb) == ODB_DESTROY_OK);
        return res;
    }
       
    
    

    
    bool load_odict128(odict128_hashmap_t* map, sodict128_build_t *sdb){
        return _load_odict<odict128_key_t, odict128_value_t, 
               odict128_hashmap_t, sodict128_build_t, sodict128_snode_t>(map, sdb);
    }
 
    
    bool load_odict128(odict128_hashmap_t* map, sodict128_search_t *sdb){
        return _load_odict_search<odict128_key_t, odict128_value_t, 
               odict128_hashmap_t, sodict128_search_t, sodict128_snode_t>(map, sdb);
    }

    
    bool load_odict128(odict128_hashmap_t* map, const char* path, const char* filename, int hash_num ){
        if(NULL == path || NULL == filename){
            fprintf(stderr, "load_odict128: path and filename pointer should not be NULL.\n");
            return false;
        }       
        sodict128_build_t* sdb = odb128_load(const_cast<char*>(path), const_cast<char*>(filename), hash_num);
        if(NULL == sdb || (unsigned long)ODB_LOAD_NOT_EXISTS == (unsigned long)sdb){
            fprintf(stderr, "load_odict128: load odict file failed.\n");
            return false;
        }
        bool res = _load_odict<odict128_key_t, odict128_value_t, odict128_hashmap_t, 
             sodict128_build_t, sodict128_snode_t>(map, sdb);
        res = res && (odb_destroy(sdb) == ODB_DESTROY_OK);
        return res;
    }
    
} 
