#ifndef  __BSL_DICT_LOADER_H_
#define  __BSL_DICT_LOADER_H_

#include <Lsc/map.h>
#include <odict.h>

namespace Lsc{  
  
    
    template<typename _T>
    struct _odict_key_t{
        _T sign1;
        _T sign2;
        _odict_key_t(_T _sign1 = 0, _T _sign2 = 0): sign1(_sign1), sign2(_sign2){}
        bool operator == (const _odict_key_t& rhs) const{
            return (sign1 == rhs.sign1 && sign2 == rhs.sign2);
        }
    };
    typedef _odict_key_t<unsigned int> odict_key_t;
    typedef _odict_key_t<unsigned long long> odict128_key_t;
    
    
    struct _odict_value_t{
        unsigned int cuint1;
        unsigned int cuint2;
        _odict_value_t(unsigned int _cuint1 = 0, unsigned int _cuint2 = 0): cuint1(_cuint1), cuint2(_cuint2){}
        bool operator == (const _odict_value_t& rhs) const{
            return (cuint1 == rhs.cuint1 && cuint2 == rhs.cuint2);
        }
    };
    typedef _odict_value_t odict_value_t;
    typedef _odict_value_t odict128_value_t;
    
    
    template<typename _Key>
    struct OdictKeyHashFunc{
        inline size_t operator()(const _Key& key) const{
            return (size_t)(key.sign1 + key.sign2);
        }
    };

    
    typedef Lsc::hashmap<odict_key_t, odict_value_t, OdictKeyHashFunc<odict_key_t> > odict_hashmap_t;
    
    typedef Lsc::hashmap<odict128_key_t, odict_value_t, OdictKeyHashFunc<odict128_key_t> > odict128_hashmap_t;
    

    
    
    
    bool load_odict(odict_hashmap_t* map, sodict_build_t* sdb);
    
    
    bool load_odict(odict_hashmap_t* map, sodict_search_t* sdb);
      
    
    bool load_odict(odict_hashmap_t* map, const char* path, const char* filename, int hash_num = 0);
 
    
    
    
    bool load_odict128(odict128_hashmap_t* map, sodict128_build_t* sdb);
 
    
    bool load_odict128(odict128_hashmap_t* map, sodict128_search_t* sdb);

    
    bool load_odict128(odict128_hashmap_t* map, const char* path, const char* filename, int hash_num = 0);
    
  } 
#endif 


