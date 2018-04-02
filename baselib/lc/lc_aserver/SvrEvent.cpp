#include <stdarg.h>
#include "SvrEvent.h"
#include <lc/lcevent.h>
#include <lc/lcutils.h>
#include "event_log.h"
namespace lc {






















void SvrEvent :: read_done_callback() {
	if (_freaddone_callback) {
		_freaddone_callback(this);
	}
}
void SvrEvent :: write_done_callback() {
	if (NULL != _fserver) {
		_fserver->session_done(this);
	}
}
int SvrEvent :: activating_session() {









	set_sock_status(LcEvent::STATUS_ACCEPTED);
	if (0 < sock_data.read_timeout) {
		set_first_rwto(sock_data.read_timeout);
	}
	session_begin();
	return 0;
}




int SvrEvent :: session_done() {
	timeval tv;
	gettimeofday(&tv, NULL);
	LCEVENT_DEBUG(this,"[time: %ld, %ld]" ,tv.tv_sec, tv.tv_usec);
	LCEVENT_DEBUG(this,"[time: %ld, %ld]" ,tv.tv_sec, tv.tv_usec);
	sock_data.timer.total = tvTv2Ms(tvSelfSlc(tv, sock_data.timer.start));
	Lsc::string n = get_notice_string();
	LC_IN_NOTICE("%s",n.c_str());
	return sock_data.connect_type;
}

LcAServer * SvrEvent :: get_server() {
	return _fserver;
}

void SvrEvent :: set_server(LcAServer * svr) {
	_fserver = svr;
}

int SvrEvent :: if_post() {
	if (NULL == _fserver) {
		return 0;
	}
	if (0 == _fserver->is_running()) {
		return 0;
	}
	return 1;
}

}


