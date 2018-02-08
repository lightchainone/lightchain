
#include <algorithm>
#include <sys/socket.h>
#include <netinet/tcp.h>

#include <comlog/sendsvr/loghead.h>

#include <ul_net.h>
#include <comlog/xutils.h>
#include <comlog/namemg.h>
#include <comlog/event.h>
#include <comlog/xdebug.h>

#include "comlogproxy_if.h"
#include "dfsappender_utils.h"

#include "dfsappender.h"
#include "dfsappendersvr.h"
namespace comspace
{

using namespace comspace::comlogplugin::dfsappender;

static void * _sendsvr_dfs_callback(void *param)
{
    while (((SendSvr *)(param))->isRun() ){
        ((SendSvr *)(param))->run();
    }
    if(com_logstatus() == USING_COMLOG){
        
        com_closelog_r();
    }
    return NULL;
}


class _netapender_sort_cmp_
{
plclic:
    bool operator() (const SendSvr::vptr_t & _1, const  SendSvr::vptr_t & _2 )
    {
        DfsAppender::netdata_t *v1 = (DfsAppender::netdata_t *)_1;
        DfsAppender::netdata_t *v2 = (DfsAppender::netdata_t *)_2;
        if (v1->item.handle.high < v2->item.handle.high) {
            return true;
              
        } else if (v1->item.handle.high == v2->item.handle.high 
                && v1->item.handle.low < v2->item.handle.low) {
            return true;
        } else if (v1->item.handle.high == v2->item.handle.high 
                && v1->item.handle.low == v2->item.handle.low) {
            return v1->item.time < v2->item.time; 
        } 
        return false;
    }
};

bool DfsAppenderSvr::reopens(server_t *svr, int)
{
	
	for (int j=0; j<_inappsize; ++j) {
		
        _com_debug("_inappender [%d] is [%p]", j, _inappender[j]);
		if (_inappender[j]->open(svr) != 0) {
			_com_error("can't open %s server", _name);
			return false;
		}
	}
	return true;
}

int DfsAppenderSvr::synchandle(int items)
{
	int i=0;
	int loop = 0;
again:
	for (i=0; i<_svrsiz; ++i) {
		_com_debug("ser valid[%d]", (int)_svrs[i].isvalid);
		if (_svrs[i].isvalid) {
			
			if (i != _now) {
				
				if (ul_swriteable_ms(_svrs[i].sock, CHECKTIMEOUT) > 0 &&
						
						reopens(_svrs+i, items)) 
				{
					if (_now >= 0) {
						close_safe(_now);
					}
					_now = i;
					break;
				} else {
					
					_com_error("invalid sock[%d] or reopen err %m", _svrs[i].sock);
					close_safe(i);
				}
			} else {
				
				if (ul_swriteable_ms(_svrs[i].sock, CHECKTIMEOUT) > 0) {
					break;
				} else {
					_com_error("invalid sock[%d] %m", _svrs[i].sock);
					clearsock(_now);
				}
			}
		}
	}
	++ loop;

	if (_now < 0 || i >= _svrsiz) {
		_now = -1;
		if (loop == 1) {
			if (tryalive(1000) == 0) {	
				goto again;
			}
		}
		_com_error("can't find valid sd _now[%d] i[%d]", _now, i);
		return -1;
	}

	return 0;
}

int DfsAppenderSvr::next_iovecs()
{
    if (_readbufnum <= 0 || _cur_item >= _readbufnum) {
        return 0;
    }
    _iosiz = 0;
    _iovecs[_iosiz].iov_base = _sendbuf;
    _iovecs[_iosiz].iov_len = sizeof(loghead_t);
    loghead_t *snd = (loghead_t*)_sendbuf;
    uint32_t *logsize = NULL; 
    uint32_t *lognum = NULL;
    ++_iosiz;
    int cur_item = _cur_item;
    int body_len = 0;
    comlogproxy_handle_t cur_handle = {0ULL, 0ULL}; 
    bool found = true;
    time_t first_time = 0;
    while (cur_item < _readbufnum) {
        DfsAppender::netdata_t * ndt = (DfsAppender::netdata_t *)(_readbuf[cur_item]);
        if (cur_handle.high != ndt->item.handle.high 
                || cur_handle.low != ndt->item.handle.low) {
            if (!found) {
                break;
            }
            _iovecs[_iosiz].iov_base = &(ndt->item);
            _iovecs[_iosiz].iov_len = sizeof(ndt->item) + ndt->item.size;
            ndt->item.cmd = COMLOG_PROXY_PRINT;
            ndt->item.magic_num = COMLOG_PROXY_PRINT_MAGIC_NUM;
            ndt->item.reserved = 0;
            first_time = ndt->item.time;
            logsize = &(ndt->item.size);
            lognum = &(ndt->item.num);
            *lognum = 1;
            cur_handle = ndt->item.handle;
            found = false;
        } else { 
            if (*logsize + ndt->item.size > sizeof(_sendbuf) - sizeof(*snd) - 
                    sizeof(comlog_proxy_print_req_t)) {
                
                break;
            }
            
            
            if (first_time - first_time%10 != 
                    (time_t)(ndt->item.time) - (time_t)(ndt->item.time)%10) {
                break;
            }
            
            _iovecs[_iosiz].iov_base = ndt->item.logdata;
            _iovecs[_iosiz].iov_len = ndt->item.size;
            *logsize += _iovecs[_iosiz].iov_len;
            (*lognum)++;
        }
        body_len += _iovecs[_iosiz].iov_len;
        ++cur_item;
        ++_iosiz;
    }
    _cur_item = cur_item;
    snd->version = COMLOG_PROXY_VERSION;
    snd->log_id = _log_id++;
    snd->magic_num = LOGHEAD_MAGICNUM;
    snd->body_len = body_len;
    snd->reserved = 0;
    return _iosiz - 1;
}




int DfsAppenderSvr::senddata(struct iovec *vec, int count)
{
	
    if (synchandle(1) < 0) {
        clearsock(_now);
        safe_msleep(100);
        return -1;
    }
    int sendsock = _svrs[_now].sock;
    loghead_t *snd = (loghead_t*)vec[0].iov_base;
    
    int ret = ul_swritev_ms(sendsock, vec, count, 1000);

    char resbuf[1024];
    comlog_proxy_res_t * res = NULL;
    if (ret != (int)(snd->body_len + sizeof(loghead_t))) {
        _com_error("send err %m sock[%d] now[%d] svr{[%d][%d]}, ret[%d], snd->bodylen[%d]",
                sendsock, _now, _svrs[_now].sock, _svrs[_now].isvalid, ret, snd->body_len);
        goto fail;
    }
    _com_debug("writev bytes[%d] bodylen[%d]", ret, snd->bodylen);
    ret = loghead_read(sendsock, (loghead_t *)resbuf, sizeof(resbuf), 1000);
    if (ret != 0) {
        _com_error("print read fail fd[%d] ret[%d] err[%m]", sendsock, ret);
        goto fail;
    }

    res = (comlog_proxy_res_t*)(resbuf + sizeof(loghead_t));
    if (res->err_no != 0) {
        _com_error("logproxy print fail err[%.*s]", res->err_len, res->err_info);
        goto fail;
    }
	return 0;

fail:
    
    clearsock(_now);
    safe_msleep(100);
    return -1;
}

int DfsAppenderSvr::run()
{
	_debug_time_def;
	_debug_getstart;
    if (0 == _iosiz) {
        _cur_item = 0;
        _readbufnum = 0;
        
        
        if (_disk_least_buff_size <= 0)  {
            if (_diskbaklog != NULL) {
                _disk_least_buff_size = _diskbaklog->read(_sendbuf + sizeof(loghead_t),
                        sizeof(_sendbuf) - sizeof(loghead_t));
                if (0 == _disk_least_buff_size) {
                    _diskbaklog->commit_read(0);
                }
            }
            _disk_cur_pos = sizeof(loghead_t);
        }
        if (_disk_least_buff_size > 0) {
            comlog_proxy_print_req_t *item = NULL;
            char *cur = _sendbuf + _disk_cur_pos; 
            char *end = cur + _disk_least_buff_size;
            for (uint32_t i = 0; i < sizeof(_readbuf)/sizeof(_readbuf[0]) && 
                    cur < end; ++i) {
                item = (comlog_proxy_print_req_t*)cur;
                if (cur + sizeof(comlog_proxy_print_req_t) < end) {
                    if (item->magic_num != COMLOG_PROXY_PRINT_MAGIC_NUM) {
                        
                        char *p = cur;
                        while (p < end) {
                            if ((*(uint32_t*)p) == COMLOG_PROXY_PRINT_MAGIC_NUM) {
                                break;
                            }
                            ++p;
                        }
                        uint64_t idx = 0;
                        int64_t offset = 0;
                        
                        if (p < end) {
                            p -= UL_OFFSETOF(comlog_proxy_print_req_t, magic_num);
                            _diskbaklog->getpos(p - _sendbuf - _disk_cur_pos, &idx, &offset);
                        } else {
                            _diskbaklog->getpos(0, &idx, &offset);
                            idx++;
                            offset = 0;
                        }
                        _com_error("error bak log in disk is error magic_num[%u] guess next item[%lld:%llu]",
                                item->magic_num, (unsigned long long)idx, (long long)offset);
                        return -1;
                    }
                } else {
                    _disk_least_buff_size = 0;
                    break;
                }
                if (cur + sizeof(comlog_proxy_print_req_t) + item->size > end) {
                    _disk_least_buff_size = 0;
                    break;
                }
                _readbuf[_readbufnum++] = (DfsAppender::netdata_t *)(item); 
                cur += sizeof(comlog_proxy_print_req_t) + item->size;
                _disk_least_buff_size -= sizeof(comlog_proxy_print_req_t) + item->size;
            }
            if (cur >= end) {
                _disk_least_buff_size = 0; 
            }
            _sendbuf_size = cur - (_sendbuf + _disk_cur_pos);
            
            if (0 == _sendbuf_size) {
                
                _diskbaklog->commit_read(0);
                _disk_least_buff_size = 0;
            }
            _disk_cur_pos = cur - _sendbuf;
            _com_debug("pop back disk %d", _readbufnum);
        } else {
            
            _readbufnum = _queue.pop_backs(_readbuf, sizeof(_readbuf)/sizeof(_readbuf[0]), 5);
        }
        _com_debug("pop_backs %d\n", _readbufnum);
        
        if (_readbufnum > 0) {
            std::sort(_readbuf, _readbuf + _readbufnum, _netapender_sort_cmp_());
        }
        this->next_iovecs();
    }
    if (0 == _iosiz) {
        return 0;
    }
    
    
    
    do {
        if (senddata(_iovecs, _iosiz) < 0) {
            goto fail;
        }
    } while (this->next_iovecs());

    if (0 == _sendbuf_size) {
        
        for (int i = 0; i < _readbufnum; ++i) {
            
            free(_readbuf[i]);
        }
        _iosiz = 0;
    } else {
        
        _diskbaklog->commit_read(_sendbuf_size);
        _iosiz = 0;
        _sendbuf_size = 0;
    }
	_debug_getend;
	_com_debug("send data used time[%d]", _debug_time);
	
	return 0;
fail:
	return -1;
}

DfsAppenderSvr::DfsAppenderSvr()
{
	_now = -1;
    _sendbuf_size = 0;
    _iosiz = 0;
    _cur_item = 0;
    _readbufnum = 0;
    _sendbuf_size = 0;
    _disk_buff_size = 0;
    _disk_least_buff_size = 0;
    _disk_cur_pos = 0;
    _log_id = 1;
    _diskbaklog = NULL;
    
    memset(_sendbuf, 0, sizeof(loghead_t));
    queue_size = 0;
}

DfsAppenderSvr::~DfsAppenderSvr()
{
    
    if (0 == _sendbuf_size && _iosiz != 0) {
        
        do {
            if (NULL == _diskbaklog) {
                if (check_slcdir(this->_bkpath) < 0) {
                    _com_error("check slcdir error [%m]");
                    goto end;
                }
                if (_disklog.open(this->_bkpath, this->_bkfile, this->_bklimit, 
                            this->_bkremove) < 0) {
                    _com_error("diskbaklog open error [%m]");
                    goto end;
                }
                _diskbaklog = &_disklog;
            }
            _diskbaklog->writev(_iovecs + 1, _iosiz - 1);
            
        } while (this->next_iovecs());
        for (int i = 0; i < _readbufnum; ++i) {
            free(_readbuf[i]);
        }
    }
    
    
    if  (_queue.size() != 0) {
        safe_msleep(100);
        int items;
        do {
            items = _queue.pop_backs(_readbuf, sizeof(_readbuf)/sizeof(_readbuf[0]), 0);
            for (int i = 0; i < items; ++i) {
                DfsAppender::netdata_t *write_item =
                    (DfsAppender::netdata_t *)(_readbuf[i]);
                
                if (NULL == _diskbaklog) {
                    if (check_slcdir(this->_bkpath) < 0) {
                        _com_error("check slcdir error [%m]");
                        goto end;
                    }
                    if (_disklog.open(this->_bkpath, this->_bkfile, this->_bklimit, 
                                this->_bkremove) < 0) {
                        _com_error("diskbaklog open error [%m]");
                        goto end;
                    }
                    _diskbaklog = &_disklog;
                }
                _diskbaklog->write(write_item, sizeof(comlog_proxy_print_req_t)+
                        write_item->item.size);
                
                free(write_item);
            }
        } while (items > 0);
    }
    return;
end:
    _com_error("diskbaklog write error [%m]");
}

int DfsAppenderSvr::initServer(const char *iplist)
{
	int items = xparserSvrConf(iplist, _svrs, sizeof(_svrs)/sizeof(_svrs[0]));
	if (items <= 0) {
		_com_error("parser %s err", iplist);
		return -1;
	}
	_svrsiz = items;
	snprintf(_name, sizeof(_name), "%s", iplist);
	_com_debug("initserver svrsiz[%d]", items);
	return 0;
}


int DfsAppenderSvr::push(vptr_t dat)
{
	_debug_time_def;

	_debug_getstart;

	pthread_mutex_lock (&_queue._mutex);
	Event *evt = (Event *)dat;
	int ret = 0;
    DfsAppender::netdata_t *val = NULL;
    _com_debug("_render_msgbuf_len is %d", evt->_render_msgbuf_len);
    val = (DfsAppender::netdata_t *)malloc(sizeof(DfsAppender::netdata_t) + evt->_render_msgbuf_len + 1);
    DfsAppender *ap = NULL;
	if (val == NULL) {
		ret = -1;
		goto end;
	}
	
    ap = dynamic_cast<DfsAppender*>(evt->_nowapp);
    val->item.cmd = COMLOG_PROXY_PRINT;
	val->item.handle = ap->_handle;
    val->item.magic_num = COMLOG_PROXY_PRINT_MAGIC_NUM;
    val->item.num  = 1;
	val->item.time = (u_int)evt->_print_time.tv_sec;
	val->item.size = (u_int)evt->_render_msgbuf_len;
    val->item.reserved = 0;
	memcpy(val->item.logdata, evt->_render_msgbuf, val->item.size);
	_com_debug("buflen=%d", val->item.size);
	


    if (queue_size < 5 
            || (queue_size >= 5 && _queue._queue.push_front(val) < 0)) {
        
        if (NULL == _diskbaklog) {
            if (check_slcdir(this->_bkpath) < 0) {
                ap->logerror("build dir [%s] fail [%m]", this->_bkpath);
                goto end;
            }
            if (_disklog.open(this->_bkpath, this->_bkfile, this->_bklimit, 
                        this->_bkremove) < 0) {
                ap->logerror("no open bklog[%s][%s] [%m]", this->_bkpath,
                        this->_bkfile);
                goto end;
            }
            _diskbaklog = &_disklog;
        }
        _diskbaklog->write(&(val->item), sizeof(val->item)+val->item.size);
        ap->logerror("[queue full] write disk log [size=%d] [queue_size=%d]",
                val->item.size, _queue._queue.size());
        free(val);
        ret = -1;
    }

    pthread_cond_signal(&_queue._cond);
end:

	pthread_mutex_unlock(&_queue._mutex);
	_debug_getend;

	

	return ret;
}

int DfsAppenderSvr::createQueue(uint32_t queue_size)
{
    if (queue_size < 5) {
        queue_size = 5;
    }
    if (_queue.create(queue_size) != 0) { 
        _com_error("create queue error");
        return -1;
    }
    return 0;
}

int DfsAppenderSvr::createServer(const char *ip)
{
    if (ip == NULL) { 
        return -1;
    }
       if (initServer(ip) != 0) { 
        return -1;
    }
    int ret = checkalive(200);  
    if (ret != 0) { 
        _com_error("can't find valid server");
    }       

    ret = comlog_get_gcheckSvr()->registerServer(this);
    if (ret != 0) { 
        return -1;
    }       
   
    _run = 1;
    if ( pthread_create(&_tid, NULL, _sendsvr_dfs_callback, this) != 0 ){ 
        _run = 0;
        _com_error("Failed to create new thread for SendSvr [%s]", ip);
        return -1;
    }

    _com_debug("start sendserver[%s]", ip);
    return 0;
}

SendSvr *DfsAppenderSvr::getServer(const char *iplist,
        disk_log_conf_t *disklog)
{
	xAutoLock autolock(get_sendsvr_lock());

	char name[MAXSVRNAMESIZE];
	server_t svrs[COM_MAXDUPSERVER];
	int items = xparserSvrConf(iplist, svrs, sizeof(svrs)/sizeof(svrs[0]));
	if (items <= 0) {
		_com_error("parser %s err", iplist);
		return NULL;
	}
	int len = 0;
	for (int i=0; i<items; ++i) {
		snprintf(name+len, sizeof(name)-len, "%s:%d", svrs[i].host, svrs[i].port);
		len = strlen(name);
		_com_debug("parser svr: %s:%d", svrs[i].host, svrs[i].port);
	}
	SendSvr *svr = (SendSvr *)comlog_get_nameMg()->get(type_sendsvr, name);
	if (svr == NULL) {
		_com_debug("start to create netappender svr");
		DfsAppenderSvr *dfssvr = new DfsAppenderSvr();
        if (NULL == dfssvr) {
            goto fail;
        }
        svr = dfssvr;
        
        if (dfssvr->createQueue(disklog->queue_size) < 0) {
            _com_error("create queue fail");
            goto fail;
        }
       
        if (disklog->path[0] == '\0') {
            snprintf(disklog->path, sizeof(disklog->path), "%s/log/%s",
                    get_proc_path(),
                    get_proc_name());
        }
        if (disklog->file[0] == '\0') {
            snprintf(disklog->file, sizeof(disklog->file), "%s", 
                    get_proc_name());
        }
        snprintf(dfssvr->_bkpath, sizeof(dfssvr->_bkpath), "%s",
                disklog->path);
        snprintf(dfssvr->_bkfile, sizeof(dfssvr->_bkfile), "%s",
                disklog->file);
        dfssvr->_bklimit = disklog->max_size;
        dfssvr->_bkremove = disklog->remove;
        dfssvr->_diskbaklog = NULL;
        
        struct stat stbuf;
        if (stat(dfssvr->_bkpath, &stbuf) == 0) {
           if (!S_ISDIR(stbuf.st_mode)) {
                _com_error("cound no bklog in queue full [%m]");
           } else {
               
               if (dfssvr->_disklog.open(dfssvr->_bkpath, dfssvr->_bkfile, dfssvr->_bklimit, 
                           dfssvr->_bkremove) < 0) {
                   _com_error("no open bklog[%s][%s] [%m]", dfssvr->_bkpath,
                           dfssvr->_bkfile);
               } else {
                    dfssvr->_diskbaklog = &dfssvr->_disklog;
               }
           }
        }
        dfssvr->queue_size = disklog->queue_size;        
        if (dfssvr->createServer(iplist) != 0) {
            _com_error("create Server fail");
            goto fail;
        } 
	    if (comlog_get_nameMg()->set(name, svr) != 0) {
			_com_error("set server err[%s]", name);
            goto fail;
		}
       
       
	}
	return svr;

fail:
    if (svr != NULL) {
        delete svr;
    }
    return NULL;
}

void DfsAppenderSvr::clearsock(int i)
{
	close_safe(i);
	_now = -1;
}
}



