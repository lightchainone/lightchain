#include "asynccond.h"


AsyncCond::AsyncCond()
{
    pthread_mutex_init(&_mutex,NULL);
    pthread_cond_init(&_cond,NULL);
    _wait_count=0;
    _finished_count=0;
    
    _test_flag = 0;
}

AsyncCond::~AsyncCond()
{
    pthread_mutex_destroy(&_mutex);
    pthread_cond_destroy(&_cond);
}

void AsyncCond::AddWait(int num)
{
    _wait_count+=num;
    _test_flag++;
}

void AsyncCond::Wait()
{
    pthread_mutex_lock(&_mutex);
    while (_finished_count<_wait_count)
    {
        pthread_cond_wait(&_cond,&_mutex);
    	_test_flag++;
    }
    _finished_count=0;
    _wait_count=0;
    pthread_mutex_unlock(&_mutex);
}

void AsyncCond::Finish()
{
    pthread_mutex_lock(&_mutex);
    _finished_count++;
    if (_finished_count>=_wait_count)
    {
        pthread_cond_signal(&_cond);
    	_test_flag++;
    }
    pthread_mutex_unlock(&_mutex);
}
