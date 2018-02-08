#ifndef __ASYNCCOND_H__
#define __ASYNCCOND_H__

#include <pthread.h>

class AsyncCond
{
plclic:
    AsyncCond();
    ~AsyncCond();
    void AddWait(int num);
    void Wait();
    void Finish();
private:
    pthread_cond_t _cond;
    pthread_mutex_t _mutex;
    volatile unsigned int _wait_count;                                                    
    volatile unsigned int _finished_count;                                                
    volatile unsigned int _test_flag;                                                
};

#endif
