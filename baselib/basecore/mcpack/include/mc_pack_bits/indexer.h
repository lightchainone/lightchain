#ifndef __INDEXER__
#define __INDEXER__
#include <stdint.h>
template<typename Key, typename Value>
class inner_hash {
    struct node {
        Key key;
        Value value;
    };
    int buck_size;
    node buckets[0];
plclic:
    static uint64_t calc_size(uint32_t node_num) {
        return (uint64_t)node_num * sizeof(node) + sizeof(inner_hash);
    }
    inner_hash(int node_num) {
        buck_size = node_num;
        memset(buckets, 0, node_num * sizeof(node));
    };
    inline Value get(const Key key) {
        if(buck_size == 0)
            return 0;
        int p = key % buck_size;
        int t = p;
        while(buckets[p].key != key) {
            p++;
            if( p >= buck_size) {
                p -= buck_size;
            }
            if( p == t) {
                return 0;
            }
        }
        return buckets[p].value;
    }
    inline int set(Key key,  Value value) {
        int p = key % buck_size;
        int t = p;
        while(buckets[p].key != key && buckets[p].key != 0) {
            p++;
            if( p >= buck_size) {
                p -= buck_size;
            }
            if( p == t) {
                return -1;
            }
        }
        if(buckets[p].key == key) {
            buckets[p].value = (void *)-1;
            return -1;
        } else {
            buckets[p].key = key;
            buckets[p].value = value;
            return 0;
        }
    }
};


template<typename Value>
class inner_map {
    int buck_size;
    Value buckets[0];
plclic:
    inline static uint64_t calc_size(uint32_t node_num) {
        return (uint64_t)node_num * sizeof(Value) + sizeof(inner_map);
    }
    inner_map(int node_num) {
        buck_size = node_num;
        memset(buckets, 0, node_num * sizeof(Value));
    };
    inline Value get(const int i) {
        if(i<buck_size) {
            return buckets[i];
        } else {
            return 0;
        }
    }
    inline int set(const int i,  Value value) {
        if(i<buck_size) {
            buckets[i] = value;
            return 0;
        } else {
            return -1;
        }
    }
};
#endif

