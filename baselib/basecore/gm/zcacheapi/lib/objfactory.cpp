#include "objfactory.h"
#include "log.h"

ObjFactory::ObjFactory()
{
    memset(&this->memconfig,0,sizeof(obj_memconfig_t));
    this->blocks = NULL;
    this->block_size = 0;
    this->cur_used = 0;
    this->total_count = 0;
    memset(&this->free_head,0,sizeof(obj_ptr_t));
    pthread_mutex_init(&mutex,NULL);
}

ObjFactory::~ObjFactory()
{
    if (this->blocks != NULL)
    {
        for (unsigned int i = 0; i < this->block_size; i++)
        {
            if (this->blocks[i])
            {
                free(this->blocks[i]);
            }
        }
        free(this->blocks);
    }
    pthread_mutex_destroy(&mutex);
}

int ObjFactory::Init( unsigned int obj_size, unsigned int peralloc_num, unsigned int max_num)
{
    if (obj_size < sizeof (obj_ptr_t))
    {
        WARNING_LOG("objsize %u <  sizeof (obj_ptr_t) %u",obj_size,(unsigned int)sizeof(obj_ptr_t));
        return -1;
    }
    else if (peralloc_num==0)
    {
        WARNING_LOG("error [peralloc_num:%u]",peralloc_num);
        return -1;
    }
    int block_num = 16;
    if (max_num != 0)
    {
        
        block_num = max_num/peralloc_num+1;
    }
    memset(&this->memconfig,0,sizeof(obj_memconfig_t));
    this->block_size = 0;
    this->cur_used = 0;
    this->total_count = 0;
    memset(&this->free_head,0,sizeof(obj_ptr_t));
    if (this->blocks != NULL)
    {
        for (unsigned int i = 0; i < this->block_size; i++)
        {
            if (this->blocks[i])
            {
                free(this->blocks[i]);
            }
        }
        free(this->blocks);
    }

    this->blocks = (char **)calloc(block_num,sizeof(char *));
    if (this->blocks == NULL)
    {
        WARNING_LOG("calloc error,[block_num:%d]",block_num);
        return -1;
    }
    this->blocks[0] = (char *)calloc(peralloc_num,obj_size);
    if (this->blocks[0] == NULL)
    {
        free(this->blocks);
        this->blocks = NULL;
        WARNING_LOG("calloc error,[peralloc_num:%d]",peralloc_num);
        return -1;
    }
    this->memconfig.max_num = max_num;
    this->memconfig.obj_size = obj_size;
    this->memconfig.peralloc_num = peralloc_num;
    this->total_count = peralloc_num;
    this->block_size = block_num;

    
    for (unsigned int i = 0; i < peralloc_num; i++)
    {
        ((obj_ptr_t *)(this->blocks[0]+i*obj_size))->next=free_head.next;
        free_head.next = (obj_ptr_t *)(this->blocks[0]+i*obj_size);
    }
    return 0;
}

int ObjFactory::Init(const obj_memconfig_t *memconfig)
{
    if (memconfig == NULL)
    {
        WARNING_LOG("error, memconfig == NULL");
        return -1;
    }
    return Init(memconfig->obj_size,memconfig->peralloc_num,memconfig->max_num);
}

void *ObjFactory::Malloc()
{
    if (this->memconfig.max_num != 0 && this->cur_used >= this->memconfig.max_num)
    {
        WARNING_LOG("cur_used %u >= max_num %u",this->cur_used,this->memconfig.max_num);
        return NULL;
    }
    obj_ptr_t *ptr = NULL;
    if (this->free_head.next == NULL)
    {
        
        unsigned int block_index = this->total_count/this->memconfig.peralloc_num;
        if (block_index == this->block_size)
        {
            
            char **blocks = (char **)realloc(this->blocks,this->block_size*2*sizeof(char*));
            if (blocks == NULL)
            {
                WARNING_LOG("realloc blocks error,[block_size:%u,cur_used:%u]",this->block_size,this->cur_used);
                return NULL;
            }
            this->blocks = blocks;
            memset(&this->blocks[block_index],0,this->block_size*sizeof(char *));
            this->block_size *= 2;
        }
        this->blocks[block_index] = (char *)calloc(this->memconfig.peralloc_num,this->memconfig.obj_size);
        if (this->blocks[block_index] == NULL)
        {
            WARNING_LOG("realloc blocks error, [block_size:%u,cur_used:%u]",this->block_size,this->cur_used);
            return NULL;
        }
        this->total_count += this->memconfig.peralloc_num;
        for (unsigned int i = 0; i < this->memconfig.peralloc_num; i++)
        {
            ((obj_ptr_t *)(this->blocks[block_index]+i*this->memconfig.obj_size))->next = free_head.next;
            free_head.next = (obj_ptr_t *)(this->blocks[block_index]+i*this->memconfig.obj_size);
        }
    }
    this->cur_used ++;
    ptr = this->free_head.next;
    this->free_head.next = ptr->next;
    return(void *)ptr;
}

void ObjFactory::Free(void *ptr)
{
    if (ptr == NULL)
    {
        return;
    }
    obj_ptr_t *obj = (obj_ptr_t *)ptr;
    obj->next = this->free_head.next;
    this->free_head.next = obj;
    this->cur_used --;
}

void *ObjFactory::Malloc_r ()
{
    void *obj=NULL;
    pthread_mutex_lock(&mutex);
    obj=Malloc();
    pthread_mutex_unlock(&mutex);
    return obj;
}

void ObjFactory::Free_r (void *ptr)
{
    pthread_mutex_lock(&mutex);
    Free(ptr);
    pthread_mutex_unlock(&mutex);
}
