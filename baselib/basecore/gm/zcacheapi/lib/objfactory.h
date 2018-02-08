


#ifndef  __OBJFACTORY_H__
#define  __OBJFACTORY_H__

#include<pthread.h>





typedef struct _obj_memconfig_t
{
    unsigned int obj_size;                                              
    unsigned int peralloc_num;                                          
    unsigned int max_num;                                               
}obj_memconfig_t;                                                       

class ObjFactory
{
    typedef struct _obj_ptr_t
    {
        struct _obj_ptr_t *next;
    }obj_ptr_t;                                                         

plclic:
    ObjFactory();
    ~ObjFactory();

    
    int Init (unsigned int obj_size, unsigned int peralloc_num, unsigned int max_num = 0);

    
    int Init (const obj_memconfig_t *memconfig);

    
    void *Malloc ();

    
    void Free (void *ptr);

    void *Malloc_r ();
    void Free_r (void *ptr);

private:
    obj_memconfig_t memconfig;
    char **blocks;                                                      
    unsigned int block_size;                                            
    unsigned int cur_used;                                              
    unsigned int total_count;                                           
    obj_ptr_t free_head;                                                
    pthread_mutex_t mutex;
};                                                                      

#endif  


