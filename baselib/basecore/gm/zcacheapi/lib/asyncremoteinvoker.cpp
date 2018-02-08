#include <sys/types.h>
#include <linux/unistd.h>
#include "asyncremoteinvoker.h"
#include "link.h"
#include "log.h"
#include "zcachedef.h"
#include "atomic.h"
#include "ul_def.h"
#include "util.h"
#include "ul_net.h"
#include <time.h>
#include <stdio.h>
#include "util.h"

pid_t gettid(void) { return syscall(__NR_gettid); }

void async_remote_mgr_thread_init_func(int type, int iid)
{
	
	

    char thread_name[128] = "";
    snprintf(thread_name, 127, "Kylin thread [type:%d, id%d]", type,iid);
    zlog_open_r(thread_name);
}


struct TimerRequest
{
    union
    {
        AsyncContext async;
        DLINK link;
    };
    async_remote_invoker_arg_t *arg;
    async_remote_cb_t cb;
    bool re_alloc;
    unsigned int delay_time;
};

AsyncRemoteClient::AsyncRemoteClient(AsyncRemoteMgr *mgr) : _mgr(mgr)
{
	
	

    _cb=NULL;
    _arg=NULL;
    _is_time_put=false;
    _process_state = CLOSED;
    DLINK_INITIALIZE(&_hashlink);
    DLINK_INITIALIZE(&_listlink);
    DLINK_INITIALIZE(&_freelink);
	pthread_mutex_init(&_mutex, NULL);
    _mgr->_RefNumInc();
}

AsyncRemoteClient::~AsyncRemoteClient()
{
	
	

	DEBUG_LOG("~AsyncRemoteClient");
    _mgr->_RefNumDec();
    _mgr=NULL;
    Close();
    DLINK_REMOVE(&_freelink);
}


int AsyncRemoteClient::BeginJob(async_remote_invoker_arg_t *arg,
                                async_remote_cb_t cb,
                                unsigned int connect_timeout_ms,
                                bool re_alloc
                               )
{
	
	

    int ret = 0;
    if (arg == NULL || cb == NULL)
    {
        WARNING_LOG("param null");
        return -1;
    }

    unsigned int timeout=0;

    DEBUG_LOG("[AsyncId:%d,ref:%d,fd:%d,nDataLen:%d,bodylen:%u,timeout_ms:%u]",
              GetAsyncId(),GetRef(),m_s,arg->req_handle.nDataLen,((lnhead_t*)arg->req_handle.pBuf)->body_len,
              arg->timeout_ms);
    _req_handle = &(arg->req_handle);
    _res_handle = &(arg->res_handle);
    _cur_res_handle = _res_handle;
    _cb = cb;
    _arg = arg;
    _re_alloc = re_alloc;
    _begin_time = time_get_cur();
    memset(_process_time,0,sizeof(_process_time));
    memset(_process_byte,0,sizeof(_process_byte));
    _arg->status=ZCACHE_ERR_LIB;

    if (ST_CONNECTED!=AtomicGetValue(m_type))
    {
        if (!IsCreated())
        {
            if (Create(true) != APFE_OK)
            {
                WARNING_LOG("Create error");
                _arg->status=ZCACHE_ERR_NET;
                goto err_exit2;
            }
        }
        _ip_port[0] = arg->ip[0];
        _ip_port[1] = arg->ip[1];
        _ip_port[2] = arg->port;
        if (_ip_port[0] == 0)
        {
            _tried_idx = 1;
        }
        else if (_ip_port[1] == 0)
        {
            _tried_idx = 0;
        }
        else
        {
            _tried_idx = _begin_time%2;
        }
        _tried_ip[0]='0';
        inet_ntop(AF_INET,&_ip_port[_tried_idx],_tried_ip,sizeof(_tried_ip));
        DEBUG_LOG("connect to [AsyncId:%d,ip:%s,port:%d]",GetAsyncId(),_tried_ip, (unsigned short)_ip_port[2]);
        Connect(_ip_port[_tried_idx], (unsigned short)_ip_port[2]);
        _con_ip_port[0] = _ip_port[_tried_idx];
        _con_ip_port[1] = _ip_port[2];
        timeout = connect_timeout_ms;
        _process_state = CONNECTED;
    }
    else
    {
        _process_state = SENDED;
        NetRequest *nreq=NEW NetRequest;
        if (nreq==NULL)
        {
            WARNING_LOG("new error");
            _arg->status=ZCACHE_ERR_LIB;
            goto err_exit1;
        }
        memset(nreq,0,sizeof(NetRequest));
        Write(_req_handle, nreq);
        timeout=arg->timeout_ms;
    }


    ret =_delay(AA_TIMEOUT,timeout,NULL);
    if (ret == -1)
    {
        
    }
    else if (ret == -2)
    {
        goto err_exit1;;
    }
    return 0;

err_exit1:
    
    Shutdown();
    return 0;

err_exit2:
    
    _cb=NULL;
    _arg=NULL;
    Close();
    return -1;
}

const char * AsyncRemoteClient::GetProcessState()
{
	
	

    switch (_process_state)
    {
    case CONNECTED:
        return "CONNECTED";
        break;
    case SENDED:
        return "SENDED";
        break;
    case RECVED_NSHEAD:
        return "RECVED_NSHEAD";
        break;
    case RECVED_BUF:
        return "RECVED_BUF";
        break;
    case IDLE:
        return "IDLE";
        break;
    case CLOSED:
        return "CLOSED";
        break;
    default:
        return "Unknown";
        break;
    }
}

bool AsyncRemoteClient::OnConnected(APF_ERROR nErrCode)
{
	
	

    NetRequest *nreq=NULL;

    if (_cb==NULL)
    {
        goto err_exit;
    }
    DEBUG_LOG("[AsyncId:%d,ref:%d,ip:%s,fd:%d,nErrCode:%d,time:%llu]",
              GetAsyncId(),GetRef(),_tried_ip,m_s,nErrCode,time_get_cur()-_begin_time);
    if (nErrCode == APFE_OK)
    {
        int ret = 0;
        _process_state = SENDED;
        nreq=NEW NetRequest;
        if (!nreq)
        {
            WARNING_LOG("new error");
            _arg->status=ZCACHE_ERR_LIB;
            goto err_exit;
        }
        memset(nreq,0,sizeof(NetRequest));
        SetNoDelay(m_s);
        Write(_req_handle, nreq);                                    
        ret =_delay(AA_TIMEOUT,_arg->timeout_ms,NULL);
        if (ret == -1)
        {
            _arg->status=ZCACHE_ERR_LIB;
            return false;
        }
        else if (ret == -2)
        {
            FATAL_LOG("error");
            _arg->status=ZCACHE_ERR_LIB;
            goto err_exit;
        }
    }
    else
    {
        int  err_no = 0;
        socklen_t err_size = sizeof(err_no);
        _arg->status=ZCACHE_ERR_NET;
        if (m_s >= 0)
        {
            getsockopt(m_s,  SOL_SOCKET, SO_ERROR,  &err_no, &err_size);
        }
        WARNING_LOG("Connect error [ip:%s, port:%u, err:%s, process_state:%s]", _tried_ip, _con_ip_port[1],strerror(err_no),
                    GetProcessState());
        goto err_exit;
    }

    return true;

err_exit:
    Shutdown();
    return false;
}

void AsyncRemoteClient::_finish(NetRequest *nreq)
{
	
	

    async_remote_cb_t cb=NULL;
    async_remote_invoker_arg_t *arg=NULL;
    TRACE_LOG("[AsyncId:%d,ref:%d,ip:%s,fd:%d,writelen:%d,readlen:%d,writetime:%llu,readtime:%llu, ttid:%d]",
              GetAsyncId(),GetRef(),_tried_ip,m_s, _process_byte[0],_process_byte[1],_process_time[0],_process_time[1], gettid());
    _cancel();
    if (_cb)
    {
        cb=_cb;
        arg=_arg;
        _cb=NULL;
        _arg=NULL;
    }
    if (cb)
    {
        cb(arg);
    }
    UL_SAFEDELETE(nreq);
}

void AsyncRemoteClient::OnWritten(APF_ERROR nErrCode, NetRequest *pReq)
{
	
	

    DEBUG_LOG("[ip:%s,fd:%d,nErrCode:%d,AsyncId:%d,ref:%d,xfered:%d,time:%llu]",
              _tried_ip,m_s,nErrCode,GetAsyncId(),GetRef(),pReq->xfered,time_get_cur()-_begin_time);
    if (_cb==NULL)
    {
        goto label_exit;
    }
    else if (nErrCode == APFE_OK)
    {
        unsigned long long curtime=time_get_cur();
        _process_time[0]=curtime-_begin_time;
        _process_byte[0]=pReq->xfered;

        if (_process_state == SENDED)
        {
            _process_state = RECVED_NSHEAD;
            DEBUG_LOG("write finish,begin read head,[AsyncId:%d,ref:%d,fd:%d, tid:%ld, ttid:%d]",GetAsyncId(),GetRef(),m_s, pthread_self(), gettid());
            Read(_res_handle->pBuf, sizeof(lnhead_t),sizeof(lnhead_t),pReq);
        }
        else
        {
            FATAL_LOG("process_state error,[process_state:%s]",GetProcessState());
            _arg->status=ZCACHE_ERR_LIB;
            goto label_exit;
        }
    }
    else
    {
        int err_no;
        socklen_t err_size = sizeof(err_no);
        if (_arg->status == ZCACHE_OK) {
            _arg->status=ZCACHE_ERR_NET;
        }
        if (m_s >= 0)
        {
            getsockopt(m_s,  SOL_SOCKET, SO_ERROR,  &err_no, &err_size);
        }
        WARNING_LOG("write error [ip:%s, port:%u, err:%s, process_state:%s]", _tried_ip, _con_ip_port[1],strerror(err_no),
                    GetProcessState());
        goto label_exit;
    }
    return;

label_exit:
    delete pReq;
    Shutdown();
}

void AsyncRemoteClient::Shutdown()
{
	
	

    
    
    int ret = 0;
    DEBUG_LOG("[ip:%s,fd:%d,AsyncId:%d,ref:%d]",_tried_ip,m_s,GetAsyncId(),GetRef());
    ret = _cancel();
    
    
    
    if (ret == 0)
    {
        CSocketV::Shutdown();
    }
    
}

void AsyncRemoteClient::OnRead(APF_ERROR nErrCode, NetRequest *pReq)
{
	
	

    int err = ZCACHE_ERR_TIMEOUT;
    if (_cb==NULL)
    {
        WARNING_LOG("_cb null");
        goto label_exit;
    }

    DEBUG_LOG("[AsyncId:%d,ref:%d,ip:%s,fd:%d,nErrCode:%d,xfered:%d,request:%d,len:%d,nBufLen:%d,nDataLen:%d,timeus:%llu, ttid:%d]",
              GetAsyncId(),GetRef(),_tried_ip,m_s,nErrCode,pReq->xfered,pReq->request,pReq->len,
              _cur_res_handle->nBufLen,_cur_res_handle->nDataLen,time_get_cur()-_begin_time, gettid());
    if (nErrCode == APFE_OK)
    {
        _process_byte[1]+=pReq->xfered;
        if (_process_state == RECVED_NSHEAD)
        {
            if (pReq->xfered != (int)sizeof(lnhead_t))
            {
                WARNING_LOG("read head error,[AsyncId:%d,ref:%d,ip:%s,fd:%d,readlen:%d,request:%d, process_state:%s]",
                            GetAsyncId(),GetRef(),_tried_ip,m_s,pReq->xfered,pReq->request, GetProcessState());
                err=ZCACHE_ERR_NET;
                goto label_exit;
            }
            lnhead_t *reshead=(lnhead_t*)_res_handle->pBuf;
            int32_t read_size = 0;
            _process_state = RECVED_BUF;
            int32_t res_buf_size = 0;
            BufHandle *last_handle = NULL;

            if (reshead->magic_num != NSHEAD_MAGICNUM)
            {
                WARNING_LOG("recv wrong magic number,[AsyncId:%d,ref:%d,ip:%s,fd:%d,magic_num:%u, process_state:%s]",
                            GetAsyncId(),GetRef(),_tried_ip,m_s,reshead->magic_num, GetProcessState());
                err=ZCACHE_ERR_NET;
                goto label_exit;
            }
            else if (reshead->body_len > ZCACHE_MAX_MSG_BODY_LEN)
            {
                WARNING_LOG("too large bodylen,[AsyncId:%d,ref:%d,ip:%s,fd:%d,bodylen:%u, process_state:%s]",
                            GetAsyncId(),GetRef(),_tried_ip,m_s,reshead->body_len, GetProcessState());
                err=ZCACHE_ERR_NET;
                goto label_exit;
            }

            _cur_res_handle->nDataLen=pReq->xfered;
            for (BufHandle *hand = _res_handle; hand != NULL; hand = hand->_next)
            {
                res_buf_size += hand->nBufLen;
                last_handle = hand;
            }
            if (res_buf_size < int(reshead->body_len+sizeof(lnhead_t)))
            {
                if (!_re_alloc)
                {
                    WARNING_LOG("recv buf is not enough[process_state:%s]", GetProcessState());
                    err = ZCACHE_ERR_BUF_NOT_ENOUGH;
                    goto label_exit;
                }
                else
                {
                    DEBUG_LOG("need realloc,[bufsize:%d,msglen:%u]",
                              res_buf_size,reshead->body_len+(unsigned int)sizeof(lnhead_t));
                    last_handle->_next = bufHandle_new(reshead->body_len+sizeof(lnhead_t) - res_buf_size);
                    if (last_handle->_next == NULL)
                    {
                        WARNING_LOG("cann't alloc memory for response [process_state:%s]", GetProcessState());
                        err = ZCACHE_ERR_LIB;
                        goto label_exit;
                    }
                }
            }
            _unreaded_len = reshead->body_len;
            _process_state = RECVED_BUF;
            if (_unreaded_len==0)
            {
                
                _process_time[1]=time_get_cur()-_begin_time-_process_time[0];
                _arg->status = ZCACHE_OK;
                _finish(pReq);
                return;
            }
            else if (_cur_res_handle->nBufLen-sizeof(lnhead_t)>0)
            {
                read_size=(_unreaded_len<_cur_res_handle->nBufLen-(int)sizeof(lnhead_t))?
                          _unreaded_len:_cur_res_handle->nBufLen-(int)sizeof(lnhead_t);
                DEBUG_LOG("read more,[AsyncId:%d,ref:%d,fd:%d,read_size:%d,unreadsize:%u]",
                          GetAsyncId(),GetRef(),m_s,read_size,_unreaded_len);
                Read(_cur_res_handle->pBuf+sizeof(lnhead_t), read_size, read_size,pReq);
            }
            else
            {
                _cur_res_handle=_cur_res_handle->_next;
                while (_cur_res_handle->nBufLen<=0&&_cur_res_handle->_next)
                {
                    _cur_res_handle=_cur_res_handle->_next;
                }
                if (_cur_res_handle->nBufLen<=0)
                {
                    WARNING_LOG("err buf [process_state:%s]", GetProcessState());
                    err=ZCACHE_ERR_PARAM;
                    goto label_exit;
                }
                else
                {
                    read_size = (_unreaded_len<_cur_res_handle->nBufLen)?
                                _unreaded_len:_cur_res_handle->nBufLen;
                    DEBUG_LOG("read more,[AsyncId:%d,ref:%d,fd:%d,readsize:%d,unreadsize:%u]",
                              GetAsyncId(),GetRef(),m_s,read_size,_unreaded_len);
                    Read(_cur_res_handle->pBuf, read_size, read_size,pReq);
                }
            }
        }
        else if (_process_state == RECVED_BUF)
        {
            _unreaded_len-=pReq->xfered;
            _cur_res_handle->nDataLen+=pReq->xfered;
            int32_t read_size = 0;
            if (_unreaded_len > 0)
            {

                if (_cur_res_handle->nDataLen==_cur_res_handle->nBufLen)
                {
                    _cur_res_handle=_cur_res_handle->_next;
                }
                while (_cur_res_handle!=NULL&&_cur_res_handle->nBufLen<=0)
                {
                    _cur_res_handle=_cur_res_handle->_next;
                }
                if (_cur_res_handle==NULL||_cur_res_handle->nBufLen<=0)
                {
                    WARNING_LOG("err buf");
                    err=ZCACHE_ERR_PARAM;
                    goto label_exit;
                }
                read_size = (_unreaded_len<_cur_res_handle->nBufLen)?
                            _unreaded_len:_cur_res_handle->nBufLen;
                DEBUG_LOG("read more,[AsyncId:%d,ref:%d,fd:%d,readsize:%d,unreadsize:%u]",
                          GetAsyncId(),GetRef(),m_s,read_size,_unreaded_len);
                Read(_cur_res_handle->pBuf, read_size, read_size,pReq);
            }
            else
            {
                
                _process_time[1]=time_get_cur()-_begin_time-_process_time[0];
                _arg->status = ZCACHE_OK;
                _finish(pReq);
                return;
            }
        }
        else
        {
            err=ZCACHE_ERR_LIB;
            FATAL_LOG("_process_state error [process_state:%s]", GetProcessState());
            goto label_exit;
        }
    }
    else
    {
        int err_no = 0;
        socklen_t err_size = sizeof(err_no);
        if (m_s >= 0)
        {
            getsockopt(m_s,  SOL_SOCKET, SO_ERROR,  &err_no, &err_size);
        }
        WARNING_LOG("read data error [fd:%d,nErrCode:%d, process_state:%s,err:%s]",m_s,nErrCode, GetProcessState(),
                    strerror(err_no));
        err=ZCACHE_ERR_NET;
        goto label_exit;
    }
    return;

label_exit:
    if (_arg && _arg->status == ZCACHE_OK)
    {
        _arg->status = err;
    }
    delete pReq;
    Shutdown();
}

void AsyncRemoteClient::OnBroken(APF_ERROR nErrCode)
{
	
	

    async_remote_cb_t cb=NULL;
    async_remote_invoker_arg_t *arg=NULL;
    WARNING_LOG("socket brocken,[ip:%s,fd:%d,nErrcode:%d,AsyncId:%d,ref:%d, process_state:%s, is_time_put:%d]",
                _tried_ip,m_s,nErrCode,GetAsyncId(),GetRef(), GetProcessState(),  _is_time_put);
    if (_cb&&_arg)
    {
        if (ZCACHE_ERR_TIMEOUT != _arg->status) {
            _arg->status = ZCACHE_ERR_NET;
        }
        cb = _cb;
        arg = _arg;
        _cb=NULL;
        _arg=NULL;
    }
    
    _cancel();
    Close();
    if (cb)
    {
        cb(arg);
    }
}

void AsyncRemoteClient::OnTimeout()
{
	
	

    if (_arg && _cb)
    {
        _arg->status = ZCACHE_ERR_TIMEOUT;
        unsigned long long curtime=time_get_cur();
        curtime=(curtime>_begin_time)?curtime-_begin_time:0;
        WARNING_LOG("[AsyncId:%d,ref:%d,timeoutus:%llu, process_state:%s]",GetAsyncId(),GetRef(),curtime,
                    GetProcessState());
    }
    _is_time_put=false;
    Shutdown();
}

int AsyncRemoteClient::Release()
{
	
	

    int n = 0;
    n = CAsyncClient::Release();
    DEBUG_LOG("release client !!! [addr:%p, ref:%d, m_s:%d, m_type:%d, Ref:%d, tid:%ld, ttid:%d]", this,n, m_s,  m_type, GetRef(), pthread_self(), gettid());
    if (n == 0)
    {
        delete this;
    }
    else if (GetRef()==1)
    {
        _mgr->ReleaseAsyncClient(this,false);
    }
    return n;
}

void AsyncRemoteClient::OnClear()
{
	
	

    this->_mgr->Clear();
}

void AsyncRemoteClient::OnCompletion(AsyncContext *pContext)
{
	
	

	
    int nAction = pContext->nAction;
    int nErrCode=pContext->nErrCode;
    if (nAction == AA_TIMEOUT)
    {
        if (_arg)
        {
            zlog_set_thlogid(_arg->logid);
        }
        else
        {
            zlog_set_thlogid(0);
        }
        OnTimeout();
    }
    else if (nAction == AA_RELEASE)
    {
		
        OnClear();
        delete pContext;
    }
    else if (nAction == AA_DELAY)
    {
        
        TimerRequest *treq=(TimerRequest*)pContext;
        if (treq->cb)
        {
            zlog_set_thlogid(treq->arg->logid);
        }
        else
        {
            zlog_set_thlogid(0);
        }
        if (treq->cb)
        {
            treq->cb(treq->arg);
        }
        DEBUG_LOG("timeout trigged,[AsyncId:%d,ref:%d]",GetAsyncId(),GetRef());
        delete treq;
    }
    else if (nAction == AA_NEW_TASK)
    {
        
        TimerRequest *treq=(TimerRequest*)pContext;
        if (treq->arg)
        {
            zlog_set_thlogid(treq->arg->logid);
            if (_mgr->AsyncExec(treq->arg,treq->cb,treq->re_alloc)<0)
            {
                treq->arg->status=ZCACHE_ERR_LIB;
                if (treq->cb)
                {
                    treq->cb(treq->arg);
                }
                
            }
            else
            {
                
            }
        }
        else
        {
            
        }
        delete treq;
    }
    else
    {
		
		

        int nAction = pContext->nAction;

		
		
        if (_arg)
        {
		
		
            zlog_set_thlogid(_arg->logid);

            DEBUG_LOG("[AsyncId:%d,ref:%d,action:%d,err:%d,fd:%d,state:%d, ttid:%d]",
                      GetAsyncId(),GetRef(),pContext->nAction,pContext->nErrCode,m_s,_process_state, gettid());
        }
        else
        {
		
		
            zlog_set_thlogid(0);            
            
            
        }
        if (nErrCode != 0 && m_s != -1)
        {
		
		
            int err_no;
            socklen_t err_size = sizeof(err_no);
		
		
            if (getsockopt(m_s, SOL_SOCKET, SO_ERROR, &err_no, &err_size) != 0)
            {
		
		
                WARNING_LOG("sockfd error [action:%d,AsyncId:%d,ref:%d, nErrCode:%d,state:%d, err:%s]",
                            nAction, GetAsyncId(),GetRef(), nErrCode,m_Ctx.state, strerror(err_no));
            }
            else
            {
		
		
                WARNING_LOG("getsockopt sockfd error [action:%d,AsyncId:%d,ref:%d, "
                            "nErrCode:%d,state:%d, err:%s]", nAction, GetAsyncId(),GetRef(),
                            nErrCode,m_Ctx.state, strerror(errno));
            }
        }
		
		
        CSocketV::OnCompletion(pContext);
		
		
        DEBUG_LOG("[after action:%d,AsyncId:%d,ref:%d, nErrCode:%d,state:%d, tid:%ld, ttid:%d]",nAction,
                  GetAsyncId(),GetRef(), nErrCode,m_Ctx.state, pthread_self(), gettid());

        if (_cb==NULL)
        {
            if ((nErrCode==APFE_OK)&&(GetRef()==3&&AtomicGetValue(m_Ctx.state)==SS_CONNECTED))
            {
                _mgr->ReleaseAsyncClient(this,true);
            }
        }
    }
	
}

int AsyncRemoteClient::Delay(int action,unsigned int nMilliseconds, AsyncContext* pCtx)
{
	
	

    int ret=0;
    ret=_delay(action,nMilliseconds,pCtx);
    return ret;
}

int AsyncRemoteClient::_cancel()
{
	
	
    if (!_is_time_put)
    {
        DEBUG_LOG("no delaytime,[AsyncId:%d,ref:%d]",GetAsyncId(),GetRef());
        return 0;
    }
    else if (CExecMan::Instance()->CancelExec((AsyncContext*)&_delay_req)!=APFE_OK)
    {
        DEBUG_LOG("can't cancel,[AsyncId:%d,ref:%d]",GetAsyncId(),GetRef());
        return -1;
    }
    else
    {
        _is_time_put=false;
        DEBUG_LOG("canceled,[AsyncId:%d,ref:%d,fd:%d, ttid:%d]",GetAsyncId(),GetRef(),m_s, gettid());
        return 0;
    }
}

int AsyncRemoteClient::_delay(int action,unsigned int nMilliseconds, AsyncContext* pCtx)
{
	
	

    int ret=0;
    if (pCtx==NULL)
    {
        pCtx=(AsyncContext*)&_delay_req;

        DEBUG_LOG("befor core,[AsyncId:%d,ref:%d, nErrCode:%d]",GetAsyncId(),GetRef(), pCtx->nErrCode);

        if (_is_time_put)
        {
            ret = CExecMan::Instance()->CancelExec((AsyncContext*)&_delay_req);
            if (ret != APFE_OK)
            {
                WARNING_LOG("can't cancel,[AsyncId:%d,ref:%d], key:%d, error:%d",
                            GetAsyncId(), GetRef(), pCtx->nErrCode, ret);
                ret=-1;
                goto label_exit;
            }
            else
            {
                DEBUG_LOG("canceled,[AsyncId:%d,ref:%d, ttid:%d]",GetAsyncId(),GetRef(), gettid());
                _is_time_put=false;
            }
        }

        DEBUG_LOG("end core,[AsyncId:%d,ref:%d, nErrCode:%d]",GetAsyncId(),GetRef(), pCtx->nErrCode);
    }
    DLINK_INITIALIZE(&(pCtx->link));

	

    ret = CExecMan::Instance()->DelayExec(action, (CAsyncClient*)this,
                                          nMilliseconds, pCtx);
    if (ret != APFE_OK)
    {
        WARNING_LOG("delay error,[action:%d,nMSec:%u,AsyncId:%d,ref:%d], error:%d",
                    action,nMilliseconds,GetAsyncId(),GetRef(), ret);
        ret=-2;
        goto label_exit;
    }
    else
    {
        if (pCtx==(AsyncContext*)&_delay_req)
        {
            _is_time_put=true;
            _begin_time=time_get_cur();
        }
        DEBUG_LOG("[action:%d,nMSec:%u,AsyncId:%d,ref:%d, ttid:%d]",
                  action,nMilliseconds,GetAsyncId(),GetRef(), gettid());
        ret=0;
    }
label_exit:


    return ret;
}

DLINK* AsyncRemoteClient::GetHashLink()
{
	
	

    return &_hashlink;
}

DLINK* AsyncRemoteClient::GetListLink()
{
	
	

    return &_listlink;
}

DLINK* AsyncRemoteClient::GetFreeLink()
{
	
	

    return &_freelink;
}

AsyncRemoteClient *AsyncRemoteClient::GetThisByList(DLINK* link)
{
	
	

    return((AsyncRemoteClient *)((char*)(link) - (long)(((AsyncRemoteClient *)0)->GetListLink())));
}


AsyncRemoteClient *AsyncRemoteClient::GetThisByHash(DLINK* link)
{
	
	

    return((AsyncRemoteClient *)((char*)(link) - (long)(((AsyncRemoteClient *)0)->GetHashLink())));
}

AsyncRemoteClient *AsyncRemoteClient::GetThisByFree(DLINK* link)
{
	
	

    return((AsyncRemoteClient *)((char*)(link) - (long)(((AsyncRemoteClient *)0)->GetFreeLink())));
}


AsyncRemoteMgr::AsyncRemoteMgr()
{
	
	

    DLINK_INITIALIZE(&_free_svr_list);
    DLINK_INITIALIZE(&_free_client_list);
    DLINK_INITIALIZE(&_used_client_list);
    DLINK_INITIALIZE(&_all_client_list);
    _delay_client = NULL;
    _free_svr_array = NULL;

	pthread_mutex_init(&_mutex, NULL);
    _time_to_close = 0;
    _max_fdnum = 0;
    _connect_timeout_ms = 0;
    _used_async_client_count = 0;
    _max_used_async_client_count = 0;
    _asyncclient_ref_num = 0;
    _stopped = false;
    _released_count = 0;
}

AsyncRemoteMgr::~AsyncRemoteMgr()
{
	
	
	

	DEBUG_LOG("~AsyncRemoteMgr");
    unsigned long long stop_used_time_usec = time_get_cur();
    Stop();
    stop_used_time_usec = time_get_cur() - stop_used_time_usec;
    NOTICE_LOG("AsyncRemoteMgr destroied [stop_us:%llu]", stop_used_time_usec);
}

void AsyncRemoteMgr::SetConnTimeout(unsigned int connect_timeout_ms)
{
	
	

    _connect_timeout_ms = connect_timeout_ms;
}

int AsyncRemoteMgr::Init(int time_to_close_ms, int max_fdnum,unsigned int connect_timeout_ms)
{
	
	

    if (max_fdnum<=0 || connect_timeout_ms<=0)
    {
        WARNING_LOG("param error");
        return -1;
    }
    Stop();
    _released_count = 0;

    TRACE_LOG("[time_to_close_ms:%d,max_fdnum%d]",time_to_close_ms,max_fdnum);
    AsyncRemoteClient *client=NULL;
    _free_svr_array = (async_server_t*) calloc(max_fdnum, sizeof(async_server_t));
    if (_free_svr_array == NULL)
    {
        WARNING_LOG("fail to calloc memory for _free_svr_array");
        goto label_exit;
    }
    _delay_client = NEW AsyncRemoteClient(this);
    if (_delay_client==NULL)
    {
        WARNING_LOG("new AsyncRemoteClient error");
        goto label_exit;
    }
    _delay_client->AddRef();
    
    if (_svr_hashtable.Init(max_fdnum) < 0 )
    {
        WARNING_LOG("fail to init hashtable");
        goto label_exit;
    }

    if (!IsKylinRunning())
    {
        KLSetLog(NULL, 0, NULL);
        if (InitKylin(1,KYLIN_NETWORK_THREAD_NUM,0,async_remote_mgr_thread_init_func, 1)!=APFE_OK)
        {
            FATAL_LOG("fail to InitKylin");
            goto label_exit;
        }
    }
    for (int i = 0; i < max_fdnum; i++)
    {
        DLINK_INSERT_NEXT(&_free_svr_list, &(_free_svr_array[i].server_list));
        client=NEW AsyncRemoteClient(this);
        if (client==NULL)
        {
            WARNING_LOG("new AsyncRemoteClient error");
            goto label_exit;
        }
        else
        {
            client->AddRef();
            DLINK_INSERT_NEXT(&_free_client_list,client->GetListLink());
            DLINK_INSERT_NEXT(&_all_client_list,  client->GetFreeLink());
            DLINK_INITIALIZE(client->GetHashLink());
        }
    }

    _time_to_close = time_to_close_ms;
    _max_fdnum = max_fdnum;
    _connect_timeout_ms = connect_timeout_ms;
    _stopped = false;
    return 0;

label_exit:
    Stop();
    return -1;
}

void AsyncRemoteMgr::ReleaseAsyncClient(AsyncRemoteClient *client,bool isok)
{

	
	

    int ret = 0;
    if (client==NULL)
    {
        WARNING_LOG("try to release a null client");
        return;
    }
    else if (client == _delay_client)
    {
        
        DEBUG_LOG("release delay client [addr:%p, ref:%d, isok:%d]",client, client->GetRef(),  isok);
        return;
    }
    else
    {
        if (DLINK_IS_EMPTY(client->GetHashLink())
            && DLINK_IS_EMPTY(client->GetListLink()))
        {
            AtomicDec(_used_async_client_count);
        }
        DEBUG_LOG("release asyncclient [addr:%p, AsyncId:%d, client_ref:%d, is_ok:%d, _used_async_client_count:%d, ttid:%d]",
                  client, client->GetAsyncId(),client->GetRef(), isok, AtomicGetValue(_used_async_client_count), gettid());
    }
    if (_max_used_async_client_count < _used_async_client_count)
    {
        _max_used_async_client_count = _used_async_client_count;
        MONITOR_LOG("_max_used_async_client_count:%d", _max_used_async_client_count);
    }
    async_server_t *svr = NULL;
    if (_max_fdnum==0||client->GetRef()<1)
    {
        
        
        FATAL_LOG("[_max_fdnum:%u,ref:%d]",_max_fdnum,client->GetRef());
        goto err_exit;
    }
    else if (!isok)
    {
        
        DLINK_REMOVE(client->GetListLink());
        DLINK_REMOVE(client->GetHashLink());
        DLINK_INITIALIZE(client->GetListLink());
        DLINK_INITIALIZE(client->GetHashLink());

        DLINK_INITIALIZE(client->GetHashLink());
        DLINK_INSERT_NEXT(&_free_client_list, client->GetListLink());
    }
    else if (_time_to_close<=0)
    {
        DEBUG_LOG("_time_to_close<=0");
        goto shutdown_exit;
    }
    else if ((ret = client->Delay(AA_TIMEOUT,_time_to_close,NULL))<0)
    {
        WARNING_LOG("delay error,[AsyncId:%d,ref:%d]",client->GetAsyncId(),client->GetRef());
        if (ret == -1)
        {
            AtomicInc(_used_async_client_count);
        }
        else if (ret == -2)
        {
            goto shutdown_exit;
        }
    }
    else
    {
        
        svr = _svr_hashtable.Find((char*)client->GetIpPort());
        if (svr == NULL)
        {
            if (DLINK_IS_EMPTY(&_free_svr_list))
            {
                FATAL_LOG("logic error, can't find async_server in free list");
                goto shutdown_exit;
            }

            unsigned int *ip_port = client->GetIpPort();
            svr =CONTAINING_RECORD(_free_svr_list._next, async_server_t,server_list);
            DLINK_REMOVE(&svr->server_list);
            DLINK_INITIALIZE(&svr->client_list);
            svr->ip = ip_port[0];
            svr->port = ip_port[1];
            _svr_hashtable.Put(svr);
        }
        DLINK_REMOVE(client->GetListLink());
        DLINK_REMOVE(client->GetHashLink());
        DLINK_INITIALIZE(client->GetListLink());
        DLINK_INITIALIZE(client->GetHashLink());
        DLINK_INSERT_NEXT(&(svr->client_list), client->GetHashLink());
        DLINK_INSERT_NEXT(&_used_client_list,client->GetListLink());
    }
err_exit:
    return;

shutdown_exit:
    AtomicInc(_used_async_client_count);
    client->Shutdown();
}

int AsyncRemoteMgr::AsyncTimerInvoker(async_remote_invoker_arg_t *arg,
                                      async_remote_cb_t cb,
                                      unsigned int timeoutms)
{
	
	

    if(_stopped)
    {
        WARNING_LOG("Mgr has already stopped");
        return -1;
    }
    if (arg==NULL||cb==NULL)
    {
        WARNING_LOG("param null");
        return -1;
    }
    int ret=0;
    if (_delay_client)
    {
        TimerRequest *cont=NEW TimerRequest;
        if (cont)
        {
            memset(cont,0,sizeof(TimerRequest));
            cont->arg=arg;
            cont->cb=cb;
            ret=_delay_client->Delay(AA_DELAY,timeoutms,(AsyncContext*)cont);
            if (ret!=0)
            {
                delete cont;
            }
            else
            {
                
            }
        }
        else
        {
            WARNING_LOG("new AsyncContext error");
            ret=-1;
        }
    }
    else
    {
        ret=-1;
    }
    return ret;
}


int   AsyncRemoteMgr::_GetRefNum()
{
	
	

    return AtomicGetValue(_asyncclient_ref_num);
}
void  AsyncRemoteMgr::_RefNumDec()
{
	
	

    AtomicDec(_asyncclient_ref_num);
}

void AsyncRemoteMgr::_RefNumInc()
{
	
	

    AtomicInc(_asyncclient_ref_num);
}

AsyncRemoteClient* AsyncRemoteMgr::_getClient(async_remote_invoker_arg_t *arg)
{
	
	

    async_server_t *svr = NULL;
    AsyncRemoteClient *client = NULL;
    DLINK *indx=NULL;
    unsigned int ip_port[2];
    ip_port[0] = arg->ip[0];
    ip_port[1] = arg->port;
    if (_stopped)
    {
        WARNING_LOG("already stopped");
        return NULL;
    }

    if (ip_port[0]!=0)
    {
        svr = _svr_hashtable.Find((char*)ip_port);
    }
    if (svr == NULL&&arg->ip[1]!=0)
    {
        ip_port[0] = arg->ip[1];
        svr = _svr_hashtable.Find((char*)ip_port);
    }
    if (svr != NULL && (!DLINK_IS_EMPTY(&svr->client_list)))
    {
        client=AsyncRemoteClient::GetThisByHash(svr->client_list._next);
        DLINK_REMOVE(client->GetHashLink());
        DLINK_REMOVE(client->GetListLink());
        DLINK_INITIALIZE(client->GetHashLink());
        DLINK_INITIALIZE(client->GetListLink());
        AtomicInc(_used_async_client_count);
        if (DLINK_IS_EMPTY(&(svr->client_list)))
        {
            _svr_hashtable.Remove(svr);                              
            DLINK_INSERT_NEXT(&_free_svr_list, &svr->server_list);
        }
        DEBUG_LOG("get client,[AsyncId:%d,ref:%d,ip0:%u,ip1:%u,port:%u]",
                  client->GetAsyncId(),client->GetRef(),arg->ip[0],arg->ip[1],arg->port);
    }
    else if (DLINK_IS_EMPTY(&_free_client_list))
    {
        WARNING_LOG("there is not enough resource in the AsyncRemoteMgr,[_used_async_client_count:%d]",
                    _used_async_client_count);
        
        indx=_used_client_list._next;
        int count=0;
        while (indx!=&_used_client_list && count<10)
        {
            client = AsyncRemoteClient::GetThisByList(indx);
            svr=NULL;
            indx=client->GetHashLink()->_next;
            if (indx==client->GetHashLink()->_prev&&indx!=client->GetHashLink())
            {
                svr=CONTAINING_RECORD(indx,async_server_t,client_list);
            }
            DLINK_REMOVE(client->GetHashLink());
            DLINK_REMOVE(client->GetListLink());
            DLINK_INITIALIZE(client->GetHashLink());
            DLINK_INITIALIZE(client->GetListLink());
            AtomicInc(_used_async_client_count);
            client->Shutdown();
            count++;
            if (svr)
            {
                _svr_hashtable.Remove(svr);                          
                DLINK_INSERT_NEXT(&_free_svr_list, &svr->server_list);
            }
            indx=_used_client_list._next;
        }
        client=NULL;
    }
    else
    {
        
        client=AsyncRemoteClient::GetThisByList(_free_client_list._next);
        DLINK_REMOVE(client->GetListLink());
        DLINK_REMOVE(client->GetHashLink());
        DLINK_INITIALIZE(client->GetHashLink());
        DLINK_INITIALIZE(client->GetListLink());
        AtomicInc(_used_async_client_count);
        DEBUG_LOG("get free client,[AsyncId:%d,ref:%d]",client->GetAsyncId(),client->GetRef());
    }
    return client;
}

int AsyncRemoteMgr::AsyncRemoteInvoker(async_remote_invoker_arg_t *arg,
                                       async_remote_cb_t cb,
                                       bool re_alloc)
{
	
	

    
    const dolcle most_used_socket_percent = 0.85;
    int fd_num_limit = 0;
    int tmp;
    int ret = 0;
    if(_stopped)
    {
        WARNING_LOG("Mgr has already stopped");
        return -1;
    }
    tmp =(int)( _max_fdnum*most_used_socket_percent);
    fd_num_limit = tmp>0?tmp:1;

	
	

    if (arg == NULL || cb == NULL || (arg->ip[0]==0&&arg->ip[1]==0))
    {
	
	
        WARNING_LOG("param error");
        return -1;
    }
    else if (arg->req_handle.pBuf==NULL||arg->req_handle.nDataLen<(int)sizeof(lnhead_t))
    {
	
	
        WARNING_LOG("req_handle error,[reqDataLen:%d]",arg->req_handle.nDataLen);
        return -1;
    }
    else if (arg->res_handle.pBuf!=NULL
             &&(arg->res_handle.nBufLen-arg->res_handle.nDataLen<(int)sizeof(lnhead_t)))
    {
	
	
        WARNING_LOG("res_handle error,[resBufLen:%d,resDataLen:%d]",
                    arg->res_handle.nBufLen,arg->res_handle.nDataLen);
        return -1;
    }
    else if (_max_fdnum==0)
    {
	
	
        WARNING_LOG("not init");
        return -1;
    }
    else if (AtomicGetValue(_used_async_client_count)>= fd_num_limit)
    {
	
	
        WARNING_LOG("too many asyncinvoker,[_used_async_client_count:%d, fd_num_limit:%d]",
                    AtomicGetValue(_used_async_client_count), fd_num_limit);
        return -1;
    }
    else if (_delay_client)
    {
	
	
        TimerRequest *cont=NEW TimerRequest;
        if (cont)
        {
	
	
            memset(cont,0,sizeof(TimerRequest));
            cont->arg=arg;
            cont->cb=cb;
            cont->re_alloc=re_alloc;
            if (QueueExec(APFE_OK,AA_NEW_TASK,_delay_client,(AsyncContext*)cont,false)!=APFE_OK)
            {
	
	
                ret=-1;
                WARNING_LOG("QueueExec error");
                delete cont;
            }
	
	
        }
        else
        {
	
	
            WARNING_LOG("new AsyncContext error");
            ret=-1;
        }
    }
    else
    {
	
	
        WARNING_LOG("not init");
        ret=-1;
    }
	
	
    return ret;
}


int AsyncRemoteMgr::AsyncExec(async_remote_invoker_arg_t *arg,
                              async_remote_cb_t cb,
                              bool re_alloc)
{

	
	

    int ret=0;
    if(_stopped)
    {
        WARNING_LOG("Mgr has already stopped");
        return -1;
    }

    AsyncRemoteClient *client=_getClient(arg);
    if (client)
    {
        ret = client->BeginJob(arg,cb,_connect_timeout_ms,re_alloc);
        if (ret==-1)
        {
            WARNING_LOG("BeginJob error");
            ReleaseAsyncClient(client,false);
        }
    }
    else
    {
        ret=-1;
    }

    return ret;
}

int AsyncRemoteMgr::Stop()
{
	
	

    const unsigned int stop_check_interval_usec = 300000;

    struct timeval v;


    _stopped = true;
    if (_delay_client == NULL)
    {
        return 0;
    }

    
    if (!IsKylinRunning())
    {
        _delay_client->_mgr->Clear();
        goto out; 
    }

    while (_GetRefNum() > 1)
    {
        AsyncContext *ctxt = NULL;
        ctxt = NEW AsyncContext;
        if (ctxt != NULL)
        {
            ctxt->pClient = _delay_client;
            _delay_client->AddRef();
            if (QueueExec(ZCACHE_OK, AA_RELEASE ,_delay_client, ctxt, true) != APFE_OK)
            {
                WARNING_LOG("QueueExec error");
                UL_SAFEDELETE(ctxt);
            }
        }
        else
        {
            WARNING_LOG("NEW AsyncContext error");
        }

        

    	time_to_val(stop_check_interval_usec, v);
	select(0, NULL, NULL, NULL, &v);

        DEBUG_LOG("[undeleted client num:%d, _delay_client ref:%d]",  _GetRefNum() - 1, _delay_client->GetRef());
    }

    while (_delay_client->GetRef() > 1)
    {
        
	
    	time_to_val(stop_check_interval_usec, v);
	select(0, NULL, NULL, NULL, &v);

        DEBUG_LOG("_delay client [ref:%d]",  _GetRefNum());
    }

out:

    StopKylin(true);
    _delay_client->Release();
    _delay_client = NULL;
    if (_free_svr_array != NULL)
    {
        free(_free_svr_array);
        _free_svr_array = NULL;
    }
    
    DLINK_INITIALIZE(&_free_svr_list);
    DLINK_INITIALIZE(&_all_client_list);
    DLINK_INITIALIZE(&_free_client_list);
    DLINK_INITIALIZE(&_used_client_list);
    _time_to_close=0;
    _max_fdnum=0;
    NOTICE_LOG("stop [mgr_addr:%p,max_fdnum:%d, _max_used_async_count:%d, _used_async_client_count:%d, _released_count:%d]",
               this,_max_fdnum, _max_used_async_client_count, _used_async_client_count, _released_count);
    _max_used_async_client_count = 0;
    return 0;
}

void AsyncRemoteMgr::Clear()
{
	
	

    AsyncRemoteClient* client=NULL;
    DLINK *link=NULL;
    link=_used_client_list._next;
    while (link!=&_used_client_list)
    {
        client=AsyncRemoteClient::GetThisByList(link);
        DLINK_REMOVE(client->GetListLink());
        DLINK_REMOVE(client->GetHashLink());
        DLINK_INITIALIZE(client->GetListLink());
        DLINK_INITIALIZE(client->GetHashLink());
        AtomicInc(_used_async_client_count);
        client->Shutdown();
        link=_used_client_list._next;
    }

    link=_free_client_list._next;
    while (link!=&_free_client_list)
    {
        client=AsyncRemoteClient::GetThisByList(link);
        if (client->GetRef() == 1)
        {
            DLINK_REMOVE(client->GetListLink());
            DLINK_REMOVE(client->GetHashLink());
            DLINK_INITIALIZE(client->GetListLink());
            DLINK_INITIALIZE(client->GetHashLink());
            client->Release();
            _released_count ++;
            DEBUG_LOG("[_released_count:%d]",  _released_count);
            link=_free_client_list._next;
        }
        else
        {
            link = link->_next;
        }
    }
}
