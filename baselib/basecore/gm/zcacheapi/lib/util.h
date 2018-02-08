#include<new>
#include "lnhead.h"
#include "BufHandle.h"
#include "msgdef.h"

#define BUFHANDLE_LEN (1024*16-(int)sizeof(BufHandle))      

const char *get_err_code(zcache_err_t err);


unsigned int gen_logid();

#define NEW new(std::nothrow)


unsigned long long time_get_cur();


void time_to_val(unsigned long long t,struct timeval& v);


unsigned long long time_to_uint64(struct timeval& val);


BufHandle *bufHandle_new(int nBufLen);


void bufHandle_free(BufHandle *handle);


void bufHandle_set(BufHandle *handle,
                   BufHandle *next,char* pBuf,
                   int nBufLen,int nDataLen);


int bufHandle_append(BufHandle *handle,char *buf,int len);



#define zcachemsg_minlen(num) (sizeof(zcache_head_t)-sizeof(lnhead_t)+sizeof(cmd_head_t)*num)



void lnhead_set (lnhead_t *head,
                 unsigned int log_id,const char *provider,
                 unsigned int body_len);


void cmd_head_set (cmd_head_t *cmdhead,unsigned short type,unsigned short ret,unsigned int len);
