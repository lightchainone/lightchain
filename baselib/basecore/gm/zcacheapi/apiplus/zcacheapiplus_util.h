#ifndef ZCACHEAPIPLUS_UTIL_H_
#define ZCACHEAPIPLUS_UTIL_H_

#include "zcachedef.h"
#include "zcachedata.h"
#include "zcacheapiplus_struct.h"


int ms_sleep(int mstime);


int copy_block_info(zcacheplus_t * zcacheplus_handel);


void * health_check_thread(void *p);

#endif 
