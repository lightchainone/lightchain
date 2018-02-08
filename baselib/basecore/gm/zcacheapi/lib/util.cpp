#include<sys/time.h>
#include<stdlib.h>
#include<stdio.h>
#include <string.h>
#include "log.h"
#include "zcachedef.h"
#include "util.h"

static const char*err_codes[]={
    
    "ZCACHE_OK",
    "ZCACHE_ERR_PARAM",                                                 
    "ZCACHE_ERR_NOT_AUTH",                                              
    "ZCACHE_ERR_BUF_NOT_ENOUGH",                                        
    "ZCACHE_ERR_EXIST",                                                 
    
    "ZCACHE_ERR_NOT_EXIST",                                             
    "ZCACHE_ERR_BLOCK_NOT_EXIST",                                       
    "ZCACHE_ERR_PRODUCT_NOT_EXIST",
    "ZCACHE_ERR_BUSY",                                                  
    "ZCACHE_ERR_FROZEN_DELETE",                                         
    
    "ZCACHE_ERR_BLOCK_UPDATED",                                         
    "ZCACHE_ERR_TIMEOUT",                                               
    "ZCACHE_ERR_NET",                                                   
    "ZCACHE_ERR_MEM",                                                   
    "ZCACHE_ERR_DISK",                                                  
    
    "ZCACHE_ERR_METASERVER",
    "ZCACHE_ERR_CACHESERVER",
    "ZCACHE_ERR_LIB",
    "ZCACHE_ERR_PART_SUC",
    "ZCACHE_ERR_BLOCK_WRONG_STATE"
};

const char *get_err_code(zcache_err_t err)
{
    if (err>=ZCACHE_OK&&err<ZCACHE_ERR_END)
    {
        return err_codes[err];
    }
    return "";
}

unsigned int gen_logid()
{
    static unsigned int logid = time(NULL);
    logid = (logid<<16) + (logid>>16) + random();
    return logid;
}


unsigned long long time_get_cur()
{
    struct timeval val;
    gettimeofday(&val,NULL);
    return time_to_uint64(val);
}


void time_to_val(unsigned long long t,struct timeval& v)
{
    v.tv_sec = time_t(t/1000000);
    v.tv_usec = suseconds_t(t%1000000);
}


unsigned long long time_to_uint64(struct timeval& val)
{
    return(unsigned long long)val.tv_sec*1000000+val.tv_usec;
}


void bufHandle_set(BufHandle *handle,
                   BufHandle *next,char* pBuf,
                   int nBufLen,int nDataLen)
{
    handle->_next = next;
    handle->pBuf = pBuf;
    handle->nBufLen = nBufLen;
    handle->nDataLen = nDataLen;
}

BufHandle *bufHandle_new(int nBufLen)
{
    BufHandle *h=NULL;
    char *buf=NEW char[sizeof(BufHandle)+nBufLen];
    if (buf!=NULL)
    {
        h=(BufHandle*)buf;
        bufHandle_set(h,NULL,buf+sizeof(BufHandle),nBufLen,0);
    }
    return h;
}

void bufHandle_free(BufHandle *handle)
{
    while (handle)
    {
        char *b=(char*)handle;
        handle=handle->_next;
        delete[] b;
    }
}

int bufHandle_append(BufHandle *handle,char *buf,int len)
{
    if (handle->nBufLen<handle->nDataLen+len)
    {
        return -1;
    }
    else
    {
        memcpy(handle->pBuf+handle->nDataLen,buf,len);
        handle->nDataLen+=len;
        return len;
    }
}

void lnhead_set (lnhead_t *head,
                 unsigned int log_id,const char *provider,
                 unsigned int body_len)
{
    memset(head,0,sizeof(lnhead_t));
    head->id = 0;
    head->version = 0;
    head->log_id = log_id;
    head->magic_num = NSHEAD_MAGICNUM;
    snprintf(head->provider,sizeof(head->provider),"%s",provider);
    head->body_len = body_len;
}

void cmd_head_set (cmd_head_t *cmdhead,unsigned short type,unsigned short ret,unsigned int len)
{
    cmdhead->cmd_type = type;
    cmdhead->cmd_ret = ret;
    cmdhead->body_len = len;
}
