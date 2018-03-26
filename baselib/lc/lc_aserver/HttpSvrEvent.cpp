
#include "HttpSvrEvent.h"
#include <lc/lcutils.h>
#include "event_log.h"

namespace lc {



















void HttpSvrEvent :: read_done_callback() {
	if (_freaddone_callback) {
		_freaddone_callback(this);
	}
}
void HttpSvrEvent :: write_done_callback() {
	if (NULL != _fserver) {
		_fserver->session_done(this);
	}
}
int HttpSvrEvent :: activating_session() {









	set_sock_status(LcEvent::STATUS_ACCEPTED);
	if (0 < sock_data.read_timeout) {
		set_first_rwto(sock_data.read_timeout);
	}
	session_begin();
	return 0;
}

int HttpSvrEvent :: session_done() {
	timeval tv;
	gettimeofday(&tv, NULL);
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

LcAServer * HttpSvrEvent :: get_server() {
	return _fserver;
}

void HttpSvrEvent :: set_server(LcAServer * svr) {
	_fserver = svr;
}

}

