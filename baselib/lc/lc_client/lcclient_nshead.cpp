
#include "lcclient_nshead.h"
#include "lcclient_define.h"
#include "lcclient_connection.h"
#include "lcclient_manager.h"

namespace lc
{
    
    void set_lcnshead_atalk(lcclient_nshead_atalk_t *p, nshead_talkwith_t *talk,
                            lc::IEvent::cb_t cb, void *param)
    {
        
        p->talk = talk;

        
        p->slctalk.req = &talk->reqhead;
        p->slctalk.res = &talk->reshead;
        p->slctalk.len = talk->maxreslen;
        p->slctalk.alloccallback = talk->alloccallback;
        p->slctalk.freecallback = talk->freecallback;
        p->slctalk.allocarg = talk->allocarg;
        p->slctalk.reqbuf = talk->reqbuf;
        p->slctalk.resbuf = talk->resbuf;

        p->slctalk.readtimeout = p->readtimeout;
        p->slctalk.writetimeout = p->writetimeout;
        p->slctalk.cb = cb;
        p->slctalk.param = param;
    }

    LcClientNsheadEvent :: LcClientNsheadEvent()
    {
        _ftag = (void *)&_talk;
        _callback_backup = IEvent::default_callback;
        _callback_arg = NULL;
        _backtag = 0;
        _fclass_type = CLASS_NSHEAD;
    }

    int LcClientNsheadEvent :: activating_session()
    {
        getCallback(&_callback_backup, &_callback_arg);
        _backtag = 1;

        setResult(0);
        set_sock_status(0);
        backup_callback();
        setCallback(LcClientEvent::conn_callback, &_talk);
        return repost();
    }

    void LcClientNsheadEvent :: session_begin()
    {
        if (0 == _backtag) {
            getCallback(&_callback_backup, &_callback_arg);
        }
        struct timeval tm;
        gettimeofday(&tm, NULL);
        sock_data.timer.connect = tvTv2Ms(tvSelfSlc(tm, sock_data.timer.start));
        
        setResult(0);
        set_sock_status(0);
        sock_data.read_timeout = _talk.readtimeout;
        sock_data.write_timeout = _talk.writetimeout;

        if (0 != nshead_awrite()) {
            set_sock_status(STATUS_ERROR);
            LC_LOG_WARNING("awrite error, maybe fd error, logid:%d", get_logid());
            session_done();
        }
    }

    int LcClientNsheadEvent :: session_done()
    {
        LcEvent::setCallback(_callback_backup, _callback_arg);
        _callback_backup = IEvent::default_callback;
        _callback_arg = NULL;
        _backtag = 0;
        common_talkwith_t *currTalk = &_talk;
        setHandle(-1);
        timeval tv;
        gettimeofday(&tv, NULL);
        sock_data.timer.total = tvTv2Ms(tvSelfSlc(tv, sock_data.timer.start));
        LcClientEvent::freeConn(currTalk);
        return 0;
    }

    void LcClientNsheadEvent :: read_done_callback()
    {
        
        session_done();
    }

    void LcClientNsheadEvent :: write_done_callback()
    {
        if (isError()) {
            LC_LOG_WARNING("write error, logid:%d", get_logid());
            session_done();
            return;
        }
        setTimeout(_talk.readtimeout);
        
        if ( 0 > nshead_aread()) {
            LC_LOG_WARNING("%s event post error,maybe fd error, logid:%d", __func__, get_logid());
            session_done();
        }
    }

    void LcClientNsheadEvent :: event_error_callback() {
        int fd = this->handle();
        struct sockaddr_in addr;
        socklen_t addr_len = sizeof(addr);
        getpeername(fd, (struct sockaddr*)&addr, &addr_len);
        LC_IN_WARNING("[ServerIp:%s, Port:%d, clientevent:%p] talkwith server error[0x%X], logid:%d",
                inet_ntoa(addr.sin_addr), ntohs(addr.sin_port), this, sock_data.status, get_logid());

        if (get_sock_status() & 0x0A) {
            if (_talk.mgr) {
                _talk.mgr->add_monitor_info_ipport("READ_ERROR_NUMBER", fd, _talk.servicename.c_str());
            }
        } else if (get_sock_status() & 0x0B) {
            if (_talk.mgr) {
                _talk.mgr->add_monitor_info_ipport("WRITE_ERROR_NUMBER", fd, _talk.servicename.c_str());
            }
        }

        session_done();
    }

    bool LcClientNsheadEvent :: isError()
    {
        if (SockEventBase :: isError()) {
            return true;
        }

        if (get_sock_status() & 0xfeffff00) {
            return true;
        }

        return false;
    }

    bsl::string LcClientNsheadEvent :: status_to_string(unsigned int st)
    {
        bsl::string res = LcEvent::status_to_string(st);
        if (st & STATUS_CONNERROR) {
            res.append("|CONNECT_ERROR");
        }

        return res;
    }

    void LcClientDuSepNsheadEvent :: session_begin()
    {        
        if (0 == _backtag) {
            getCallback(&_callback_backup, &_callback_arg);
        }
        struct timeval tm;
        gettimeofday(&tm, NULL);
        sock_data.timer.connect = tvTv2Ms(tvSelfSlc(tm, sock_data.timer.start));
        
        setResult(0);
        set_sock_status(0);
        sock_data.read_timeout = _talk.readtimeout;
        sock_data.write_timeout = _talk.writetimeout;

        if (0 != nshead_awrite()) {
            set_sock_status(STATUS_ERROR);
            LC_LOG_WARNING("awrite error, maybe fd error");
            event_error_callback();
        }
    }

    void LcClientDuSepNsheadEvent :: event_error_callback()
    {
        
        _in_event->clear_event();
        
        if (result() & IEvent::IOWRITEABLE) {
            LcClientEvent :: freeConnWithoutCallback(&_talk, 1);
            release();
        } else if (get_sock_status() & STATUS_READ) {
            _in_event->decReadCnt();
        }

        session_done();
    }

    int LcClientDuSepNsheadEvent :: session_done()
    {
        LcEvent::setCallback(_callback_backup, _callback_arg);
        _callback_backup = IEvent::default_callback;
        _callback_arg = NULL;
        _backtag = 0;
        common_talkwith_t *currTalk = &_talk;
        setHandle(-1);
        timeval tv;
        gettimeofday(&tv, NULL);
        sock_data.timer.total = tvTv2Ms(tvSelfSlc(tv, sock_data.timer.start));
        currTalk->talkwithcallback(currTalk);
        return 0;
    }

    void LcClientDuSepNsheadEvent :: write_done_callback()
    {
        setTimeout(_talk.readtimeout);
        LcClientInDuSepEvent::unified_rw_callback(this, _in_event, 0);
    }

    void LcClientDuSepNsheadEvent :: read_done_callback()
    {
        LcClientInDuSepEvent :: unified_rw_callback(this, _in_event, 1);
    }

    void LcClientInDuSepEvent :: session_begin()
    {
        _write_event->setInEvent(this);
        _write_event->session_begin();
    }

    int LcClientInDuSepEvent :: read_done(LcClientDuSepNsheadEvent *out_event)
    {
        out_event->session_done();
        {
            _deque_lock.lock();
            if (!_oevent_deque.empty()) {
                LcClientDuSepNsheadEvent *new_event = _oevent_deque.front();
                _oevent_deque.pop_front();
                if (new_event->nshead_aread() != 0) {
                    _deque_lock.unlock();
                    new_event->set_sock_status(STATUS_ERROR);
                    new_event->event_error_callback();    
                    new_event->release();
                    return -1;
                }
                new_event->release();
            } else {
                _read_cnt.decrementAndGet();
            }
            _deque_lock.unlock();
        }
        return 0;
    }

    void LcClientInDuSepEvent :: unified_rw_callback(IEvent *e, void *p, int tag)
    {
        LcClientInDuSepEvent *in_event = (LcClientInDuSepEvent*)p;
        LcClientDuSepNsheadEvent *out_event = dynamic_cast<LcClientDuSepNsheadEvent*>(e);
        if (0 == tag) {
            in_event->write_done();
        } else if (1 == tag) {
            in_event->read_done(out_event);
        }
    }

    int LcClientInDuSepEvent :: write_done()
    {
        
        LcClientDuSepNsheadEvent *temp_event = _write_event;
        LcClientEvent::freeConnWithoutCallback((common_talkwith_t *)(_write_event->get_tag()), 1);
        
        temp_event->set_firstreactor(reactor());
        
        
        _deque_lock.lock();
        if (_read_cnt.get() != 0) {
            _oevent_deque.push_back(temp_event);
        } else {
            _read_cnt.incrementAndGet();
            if (temp_event->nshead_aread() != 0) {
                _read_cnt.decrementAndGet();
                _deque_lock.unlock();
                LC_LOG_WARNING("%s event post error,maybe fd error", __func__);
                
                temp_event->set_sock_status(STATUS_ERROR);
                temp_event->event_error_callback();
                temp_event->release();
                return -1;
            }
            temp_event->release();
        }
        _deque_lock.unlock();
        return 0;
    }

    void LcClientInDuSepEvent :: clear_event()
    {
        AutoMLock lock(_deque_lock);
        while(!_oevent_deque.empty()) {
            LcClientDuSepNsheadEvent *new_event = _oevent_deque.front();
            _oevent_deque.pop_front();
            new_event->set_sock_status(STATUS_ERROR);
            new_event->session_done();
            new_event->release();
        }
    }
}













