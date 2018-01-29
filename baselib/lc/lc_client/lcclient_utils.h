



#ifndef  __LCCLIENT_UTILS_H_
#define  __LCCLIENT_UTILS_H_

#include <vector>
#include <list>
#include <map>

#include <pthread.h>
#include <arpa/inet.h>
#include <connectmgr_utils.h>
#include <Configure.h>

#include "lcclient_define.h"

typedef struct _galileo_t galileo_t;

namespace lc {

    
    int register_server_bygalileo(const comcfg::ConfigUnit &conf);

    
    int convert_conf_domain2ip(comcfg::ConfigUnit &conf);

    
    const char *get_talk_errorinfo(int errNo);  

    
    void print_talk_errorinfo(int errNo);

    
    class IntVector {
        plclic:
            
            IntVector() : _maxSize(LCCLIENT_SELECTEDSERVER_SIZE), _currSize(0){}
            
            ~IntVector(){}

            
            int push_back(int id);

            
            int size() const{return _currSize;}

            
            int operator[](int pos) const{
                if (pos >= _currSize || pos < 0) {
                    return -1;
                }
                return _id[pos];
            }

            
            bool isFull(){return _currSize >= _maxSize;}

            
            void clear(){_currSize = 0;}
        protected:
            int _id[LCCLIENT_SELECTEDSERVER_SIZE];        
            int _maxSize;                                 
            int _currSize;                                
    };

    
    class ResourceCleaner
    {
    plclic:
        
        ResourceCleaner();
        
        
        ~ResourceCleaner();    
    };
}

#endif  


