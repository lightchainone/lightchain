
#include "lcclient_writevevent.h"

namespace lc
{

    void LcClientNsheadWritevEvent :: session_begin()
    {
        if (store_iov(_iov, _iov_count) != 0) {
            LC_LOG_WARNING("iov or iov_count is error");
            set_sock_status(STATUS_ERROR);
            session_done();
            return;
        }

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

        if (trigger_write() != 0) {
            LC_LOG_WARNING("trigger_awrite error, maybe fd error");
            process_error(STATUS_ERROR);
        }
    }

    void LcClientNsheadWritevEvent :: write_done_callback()
    {
        
        int ret = writev(handle(), _iov, _iov_count);
        if (ret < 0) {
            if (errno == EAGAIN) {
                if (reactor()->post(this) != 0) {
                    process_error(STATUS_ERROR);
                    return;
                }
            } else if (errno == EINVAL){
                LC_LOG_WARNING("the sum of iov_len overflow ssize_t or iov_count greater than IOV_MAX");
                process_error(STATUS_ERROR);
                return;
            } else {
                LC_LOG_WARNING("writev error:%d %m", ret);
                process_error(STATUS_ERROR);
                return;
            }
        } else {
            
            if (parse_iov(ret) != 0) {
                if (reactor()->post(this) != 0) {
                    process_error(STATUS_ERROR);
                    return;
                }
            } else {
                
                reset_iov();
                if (nshead_aread() != 0) {
                    set_sock_status(STATUS_ERROR);
                    session_done();
                    return;
                }
            }
        }
        return;
    }

    int LcClientNsheadWritevEvent :: parse_iov(int ret)
    {
        
        struct iovec *tmp_iov = NULL;
        size_t i;
        for (i=0; i<_iov_count; i++) {
            if ((int)_iov[i].iov_len <= (ret))     {
                ret -= _iov[i].iov_len;
                continue;
            } else {
                tmp_iov = &_iov[i];
                _iov[i].iov_base = (void *)(((char *)(_iov[i].iov_base)) + ret);
                _iov[i].iov_len -= ret;
                break;
            }
        }

        if (i == _iov_count) {
            return 0;
        } else if (i < _iov_count) {
            _iov_count -= i;
            _iov = tmp_iov;
            return -1;
        }
        
        return 0;
    }
}




















