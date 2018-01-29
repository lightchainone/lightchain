
#include "lcclient_ofdevent.h"

namespace lc
{
    int LcClientOFDNsheadEvent :: activating_session()
    {
        LC_LOG_WARNING("LcClientOFDNsheadEvent not support %s\n", __func__);
        return -1;
    }

    void LcClientOFDNsheadEvent :: session_begin()
    {
        if (0 == _backtag) {
            getCallback(&_callback_backup, &_callback_arg);
        }
        timeval tv;
        gettimeofday(&tv, NULL);
        sock_data.timer.start = tv;
        setResult(0);
        set_sock_status(0);
        if (0 != nshead_awrite()) {
            set_sock_status(STATUS_ERROR);
            LC_LOG_WARNING("awrite error, maybe fd error");
            session_done();
        }
    }

    void LcClientOFDNsheadEvent :: write_done_callback()
    {
        if (isError()) {
            LC_LOG_WARNING("write error");
            session_done();
            return;
        }

        if ( 0 > nshead_aread()) {
            LC_LOG_WARNING("%s event post error,maybe fd error", __func__);
            session_done();
        }
    }

    int LcClientOFDNsheadEvent :: session_done()
    {
        LcEvent::setCallback(_callback_backup, _callback_arg);
        _callback_backup = IEvent::default_callback;
        _callback_arg = NULL;
        _backtag = 0;
        if (NULL == _talk.talkwithcallback) {
            LC_LOG_WARNING("talk's callback is NULL");
            return -1;
        }
        timeval tv;
        gettimeofday(&tv, NULL);
        sock_data.timer.total = tvTv2Ms(tvSelfSlc(tv, sock_data.timer.start));
        _talk.talkwithcallback(&_talk);
        return 0;
    }
}


















