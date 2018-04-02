#include <lc/ievent.h>
#include <lc/lcevent.h>
#include "NsheadSvrEvent.h"
#include <lc_miscdef.h>
#include "event_log.h"

namespace lc {

void NsheadSvrEvent :: read_done_callback() {
	if (_freaddone_callback) {
		_freaddone_callback(this);
	}
}
void NsheadSvrEvent :: write_done_callback() {
	if (NULL != _fserver) {
		_fserver->session_done(this);
	}
}
int NsheadSvrEvent :: activating_session() {
	set_sock_status(LcEvent::STATUS_ACCEPTED);
	if (0 < sock_data.read_timeout) {
		set_first_rwto(sock_data.read_timeout);
	}
	session_begin();
	return 0;
}




int NsheadSvrEvent :: session_done() {
	timeval tv;
	gettimeofday(&tv, NULL);
	LCEVENT_DEBUG(this,"[time: %ld, %ld]" ,tv.tv_sec, tv.tv_usec);
	LCEVENT_DEBUG(this,"[time: %ld, %ld]" ,tv.tv_sec, tv.tv_usec);
	sock_data.timer.total = tvTv2Ms(tvSelfSlc(tv, sock_data.timer.start));
	Lsc::string n = get_notice_string();
	LC_IN_NOTICE("%s",n.c_str());

    if (_fserver) {
        _fserver->set_monitor_request_time(sock_data.timer.total);
        if (isError()) {
            _fserver->set_monitor_request_num_fail();
        } else {
            _fserver->set_monitor_request_num_success();
        }
    }

	return sock_data.connect_type;
}

LcAServer * NsheadSvrEvent :: get_server() {
	return _fserver;
}

void NsheadSvrEvent :: set_server(LcAServer * svr) {
	_fserver = svr;
}

}

