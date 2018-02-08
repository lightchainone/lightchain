



#ifndef  __COMMWITHCS_H_
#define  __COMMWITHCS_H_

#define CS_IP_LEN 32

#define CS_REQ_DEF(req_cmd_t, res_cmd_t)                                \
    char csvr_ip0[CS_IP_LEN];                                                     \
    char csvr_ip1[CS_IP_LEN];                                                     \
    inet_ntop(AF_INET, &ip[0], csvr_ip0, CS_IP_LEN);                                                     \
    inet_ntop(AF_INET, &ip[1], csvr_ip1, CS_IP_LEN);                                                     \
    int ret = 0;                                                                    \
    char        *mreq_buf = NULL;                                                   \
    char        *mres_buf = NULL;                                                   \
    async_remote_invoker_arg_t *arg = NULL;                                         \
    BufHandle   *req_handle = NULL;                                                 \
    BufHandle   *res_handle = NULL;                                                 \
    unsigned int    *req_num = NULL;                                                \
    unsigned int    *res_num = NULL;                                                \
    lnhead_t    *req_lnhead  = NULL;                                                \
    lnhead_t    *res_lnhead  = NULL;                                                \
    cmd_head_t  *req_head = NULL;                                                   \
    cmd_head_t  *res_head = NULL;                                                   \
    req_cmd_t   *req = NULL;                                                        \
    res_cmd_t   *res = NULL;                                                        \
    unsigned int    timeout_ms = zcache->ms_read_timeout_ms;                            \
    arg = (async_remote_invoker_arg_t*)calloc(1,                                    \
        sizeof(async_remote_invoker_arg_t)+sizeof(unsigned long long)               \
        + sizeof(cmd_head_t) + sizeof(unsigned int) + sizeof(req_cmd_t)             \
        + sizeof(cmd_head_t) + sizeof(unsigned int) + sizeof(res_cmd_t)             \
        + 2*sizeof(lnhead_t));                                                      \
    if(arg == NULL)                                                                 \
    {                                                                               \
       WARNING_LOG("fail to calloc memory for async_remote_invocker_arg");          \
       ret = -1;                                                                    \
       goto err_exit;                                                               \
    }                                                                               \
                                                                                    \
    req_handle  =&(arg->req_handle);                                                \
    res_handle  = &(arg->res_handle);                                               \
    arg->arg = zcache;                                                              \
    mreq_buf = ((char*)(&arg[1]))+sizeof(unsigned long long);                       \
    req_lnhead = (lnhead_t*)mreq_buf;                                               \
    req_num  = (unsigned int*)(mreq_buf+sizeof(lnhead_t));                          \
    *req_num = 1;                                                                   \
    req_head = (cmd_head_t*)(mreq_buf +sizeof(lnhead_t)+ sizeof(unsigned int));     \
    req = (req_cmd_t*)(mreq_buf +sizeof(lnhead_t) + sizeof(unsigned int)+ sizeof(cmd_head_t));             \
                                                                                                           \
    mres_buf = mreq_buf + sizeof(lnhead_t) + sizeof(cmd_head_t) + sizeof(unsigned int) + sizeof(req_cmd_t);\
    res_lnhead = (lnhead_t*)mres_buf;                                                                      \
    res_num  = (unsigned int*)(mres_buf + sizeof(lnhead_t));                                               \
    res_head = (cmd_head_t*)(mres_buf + sizeof(unsigned int) + sizeof(lnhead_t));                          \
    res = (res_cmd_t*)(mres_buf + sizeof(unsigned int)+ sizeof(cmd_head_t) + sizeof(lnhead_t));            \
                                                                                                           \
    req_handle->pBuf = mreq_buf;                                                                           \
    req_handle->nBufLen = sizeof(cmd_head_t)+sizeof(unsigned int)+sizeof(req_cmd_t) + sizeof(lnhead_t);    \
    req_handle->nDataLen = sizeof(cmd_head_t)+sizeof(unsigned int)+sizeof(req_cmd_t)+ sizeof(lnhead_t);    \
                                                                                                           \
    res_handle->pBuf = mres_buf;                                                                           \
    res_handle->nBufLen = sizeof(cmd_head_t) + sizeof(unsigned int)+sizeof(res_cmd_t) + sizeof(lnhead_t);  \
    res_handle->nDataLen =0;                                                                               \
                                                                                         \
    req_lnhead->log_id = gen_logid();                                                                      \
    req_lnhead->magic_num = NSHEAD_MAGICNUM;                                                               \
    req_lnhead->body_len = sizeof(unsigned int) + sizeof(cmd_head_t) + sizeof(req_cmd_t);                  \
    req_head->body_len = sizeof(req_cmd_t);                                                                \
                                                                                            \
    arg->ip[0] = ip[0];                                                                                    \
    arg->ip[1] = ip[1];                                                                                    \
    arg->port = port;                                                                                      \
    arg->timeout_ms = zcache->ms_read_timeout_ms;                                                              \
    arg->logid = req_lnhead->log_id;                                                                       \
    zlog_set_thlogid(arg->logid);
 
#define CS_REQ_DESTROY()                                               \
{       \
    return ret;                                                        \
err_exit:                                                              \
    do                                                                  \
    {                                                                   \
        if(arg->res_handle._next != NULL)                               \
            bufHandle_free(arg->res_handle._next);                      \
        if(arg->req_handle._next != NULL)                               \
            bufHandle_free(arg->req_handle._next);                      \
        UL_SAFEFREE(arg);                                               \
        return ret;                                              \
    }while(0);                                                          \
}

#define CS_REQ_CB_DEF(arg,req_cmd_t, res_cmd_t)                        \
    char csvr_ip0[CS_IP_LEN];                                          \
    char csvr_ip1[CS_IP_LEN];                                          \
    inet_ntop(AF_INET, &arg->ip[0], csvr_ip0, CS_IP_LEN);                                     \
    inet_ntop(AF_INET, &arg->ip[1], csvr_ip1, CS_IP_LEN);                                     \
    bool        is_data_correct = true;                                  \
    unsigned int    *req_num = NULL;                                         \
    unsigned int    *res_num = NULL;                                         \
    unsigned int    bufhdl_buf_len = 0;                                      \
    BufHandle   *bufhdl_trav = NULL;                                     \
    lnhead_t    *req_lnhead = NULL;                                      \
    lnhead_t    *res_lnhead = NULL;                                      \
    cmd_head_t  *req_head = NULL;                                        \
    cmd_head_t  *res_head = NULL;                                        \
    req_cmd_t   *req = NULL;                                             \
    res_cmd_t   *res = NULL;                                             \
    char        *mreq_buf = NULL;                                        \
    char        *mres_buf = NULL;                                        \
    single_zcache_t *zcache = (single_zcache_t *)arg->arg;                                  \
    BufHandle   *req_handle = &(arg->req_handle);                        \
    BufHandle   *res_handle = &(arg->res_handle);                        \
    mreq_buf = req_handle->pBuf;                                         \
    mres_buf = res_handle->pBuf;                                         \
    req_lnhead = (lnhead_t*)mreq_buf;                                                                   \
    req_num  = (unsigned int*)(mreq_buf+sizeof(lnhead_t));                                              \
    req_head = (cmd_head_t*)(mreq_buf +sizeof(lnhead_t)+ sizeof(unsigned int));                         \
    req = (req_cmd_t*)(mreq_buf +sizeof(lnhead_t) + sizeof(unsigned int)+ sizeof(cmd_head_t));          \
    res_lnhead = (lnhead_t*)mres_buf;                                                                   \
    res_num  = (unsigned int*)(mres_buf + sizeof(lnhead_t));                                            \
    res_head = (cmd_head_t*)(mres_buf + sizeof(unsigned int) + sizeof(lnhead_t));                       \
    res = (res_cmd_t*)(mres_buf + sizeof(unsigned int)+ sizeof(cmd_head_t) + sizeof(lnhead_t));         \
    arg->timeout_ms = zcache->ms_write_timeout_ms;                                                           \
                                                                                       \
    zlog_set_thlogid(arg->logid);                                                                       \
    bufhdl_trav = res_handle;                                                                           \
    while(bufhdl_trav != NULL)                                                                          \
    {                                                                                                   \
        bufhdl_buf_len += bufhdl_trav->nDataLen;                                                        \
        bufhdl_trav = bufhdl_trav->_next;                                                               \
    }                                                                                                   \
                                                                                    \
    zlog_set_thlogid(arg->logid);                                                                       \
    if(arg->status == ZCACHE_OK                                                                         \
       &&(                                                                                              \
         (unsigned int)arg->res_handle.nDataLen < sizeof(lnhead_t) + sizeof(cmd_head_t) + sizeof(res_cmd_t)+  \
                                              sizeof(unsigned int)                                            \
         ||res_lnhead->body_len < sizeof(cmd_head_t) + sizeof(res_cmd_t) + sizeof(unsigned int)               \
         ||res_lnhead->body_len != res_head->body_len + sizeof(unsigned int) +  sizeof(cmd_head_t)            \
         ||bufhdl_buf_len != res_lnhead->body_len + sizeof(lnhead_t)                                          \
         )                                                                                                    \
      )                                                                                                       \
    {                                                                                                         \
        WARNING_LOG("csvr response with wrong format data [ip0:%s, ip1:%s, port:%u]",                         \
                    csvr_ip0, csvr_ip1, arg->port);                                                           \
        is_data_correct = false;                                                                              \
    }

#define CS_REQ_CB_DESTROY()                                                 \
{                                                                           \
    do                                                                      \
    {                                                                       \
        if(arg->res_handle._next != NULL)                                   \
            bufHandle_free(arg->res_handle._next);                          \
        if(arg->req_handle._next != NULL)                                   \
            bufHandle_free(arg->req_handle._next);                          \
        UL_SAFEFREE(arg);                                                   \
    } while (0);                                                            \
}

void *cs_check(void *arg);
















#endif  


