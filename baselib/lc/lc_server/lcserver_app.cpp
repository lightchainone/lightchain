#include <nshead.h>
#include <lc_log.h>

#include "pool.h"
#include "lc_util.h"
#include "lc_misc.h"
#include "lcserver_app.h"
#include "LcMonitor.h"

int writev_default_native()
{
	nshead_t *req = (nshead_t *)lc_server_get_read_buf();
	int reqsiz = lc_server_get_read_size();
	int roto = lc_server_get_read_timeout();
	int wtto = lc_server_get_write_timeout();
	lc_server_t *sev = lc_server_get_server();
	int fd = lc_server_get_socket();
	lc_timer_t timer;
	int ret = 0;
    struct iovec **iov = lc_server_get_write_iovec();
    size_t *iov_count = lc_server_get_write_iovec_count();

	if (lc_log_clearnotice() != 0) {
		LC_LOG_WARNING("clear notice err");
	}

	if (req == NULL || sev==NULL) {
		LC_LOG_WARNING("req, sev all NULL");
		return -1;
	}
	if (fd < 0) {
		LC_LOG_WARNING("can't get valid socket[%d][%m]", fd);
		return -1;
	}

	lc_timer_init(&timer);
	lc_timer_settask(&timer, "rev");

	char reqip[32];
	in_addr addr;
	addr.s_addr = lc_server_get_ip();
	lc_log_setbasic(LC_LOG_REQIP, "%s", lc_inet_ntoa_r(addr, reqip, sizeof(reqip)));

	ret = nshead_read_ex(fd, req, reqsiz, roto);
	if (ret != 0) {
		LC_LOG_WARNING(EMSG_NET"read err from fd[%d] err[%d][%s]", fd, ret, nshead_error(ret));
		return -1;
	}

	lc_timer_settask(&timer, "proc");

	lc_log_setbasic(LC_LOG_LOGID, "%u", req->log_id);

	ret = 0;
	if (strcmp(LC_MONITOR_PROVIDER, req->provider) != 0) {
		if (sev->cb_check != NULL) {
			ret = sev->cb_check(req, reqsiz);
			if (ret != 0) {
				LC_LOG_WARNING("check the req err");
				goto out;
			}
		}
		ret = sev->cb_drastic();
        if (sev->monitor) {
            sev->monitor->add_request_number_success(sev->server_name);
        }
		if (ret != 0) {
            if (sev->monitor) {
                sev->monitor->add_request_number_failed(sev->server_name);
            }
			ret = -1;
			goto out;
		}
	}

	lc_timer_settask(&timer, "write", 1);
    
    if (NULL == iov || NULL == *iov || *iov_count <= 0) {
        LC_LOG_WARNING("iovec for write is error");
        ret = -1;
        goto out;
    }
    ret = ul_swritev_ms(fd, *iov, *iov_count, wtto);
    if (sev->cb_writedone != NULL) {
        sev->cb_writedone(ret);
    }
	if (ret <= 0) {
		LC_LOG_WARNING(EMSG_NET"write to fd[%d] err[%d][%s]", fd, ret, nshead_error(ret));
		ret = -1;
		goto out;
	}

    ret = 0;
out:
	

	lc_timer_endtask(&timer);
    if (sev->monitor) {
        sev->monitor->add_compute_time(sev->server_name, lc_timer_gettask(&timer, "lc total"));
    }
	lc_log_setbasic(LC_LOG_PROCTIME, "total:%u(ms) rev:%u+proc:%u+write:%u",
			lc_timer_gettask(&timer, "lc_total"), lc_timer_gettask(&timer, "rev"),
			lc_timer_gettask(&timer, "proc"), lc_timer_gettask(&timer, "write"));
	
	if (sev->log_print) {
		LC_LOG_NOTICE("");
	}

	return ret;

}

int default_native()
{
	nshead_t *req = (nshead_t *)lc_server_get_read_buf();
	int reqsiz = lc_server_get_read_size();
	nshead_t *res = (nshead_t *)lc_server_get_write_buf();
	int roto = lc_server_get_read_timeout();
	int wtto = lc_server_get_write_timeout();
	lc_server_t *sev = lc_server_get_server();
	int fd = lc_server_get_socket();
	lc_timer_t timer;
	int ret = 0;

	if (lc_log_clearnotice() != 0) {
		LC_LOG_WARNING("clear notice err");
	}

	if (req == NULL || res==NULL || sev==NULL) {
		LC_LOG_WARNING("req, res, sev all NULL");
		return -1;
	}
	if (fd < 0) {
		LC_LOG_WARNING("can't get valid socket[%d][%m]", fd);
		return -1;
	}

	lc_timer_init(&timer);
	lc_timer_settask(&timer, "rev");

	char reqip[32];
	in_addr addr;
	addr.s_addr = lc_server_get_ip();
	lc_log_setbasic(LC_LOG_REQIP, "%s", lc_inet_ntoa_r(addr, reqip, sizeof(reqip)));

	ret = nshead_read_ex(fd, req, reqsiz, roto);
	if (ret != 0) {
		LC_LOG_WARNING(EMSG_NET"read err from fd[%d] err[%d][%s]", fd, ret, nshead_error(ret));
        if (sev->monitor) {
            sev->monitor->add_read_error_number(sev->server_name);
        }
		return -1;
	}

	lc_timer_settask(&timer, "proc");

	lc_log_setbasic(LC_LOG_LOGID, "%u", req->log_id);
	res->body_len = 0;
	res->log_id = req->log_id;
	memcpy(res->provider, req->provider, sizeof(req->provider));

	ret = 0;
	if (strcmp(LC_MONITOR_PROVIDER, req->provider) != 0) {
		if (sev->cb_check != NULL) {
			ret = sev->cb_check(req, reqsiz);
			if (ret != 0) {
				LC_LOG_WARNING("check the req err");
				goto out;
			}
		}
		ret = sev->cb_drastic();
        if (0 == ret) {
			if (sev->monitor) {
				sev->monitor->add_request_number_success(sev->server_name);
			}
        } else {
            if (sev->monitor) {
                sev->monitor->add_request_number_failed(sev->server_name);
            }
			ret = -1;
			goto out;
		}
	}

	lc_timer_settask(&timer, "write", 1);
	ret = nshead_write_ex(fd, res, wtto);
	if (ret != 0) {
		LC_LOG_WARNING(EMSG_NET"write to fd[%d] err[%d][%s]", fd, ret, nshead_error(ret));
        if (sev->monitor) {
            sev->monitor->add_write_error_number(sev->server_name);
        }
		ret = -1;
		goto out;
	}

out:
	

	lc_timer_endtask(&timer);
    if (sev->monitor) {
        sev->monitor->add_compute_time(sev->server_name, lc_timer_gettask(&timer, "lc total"));
    }
	lc_log_setbasic(LC_LOG_PROCTIME, "total:%u(ms) rev:%u+proc:%u+write:%u",
			lc_timer_gettask(&timer, "lc_total"), lc_timer_gettask(&timer, "rev"),
			lc_timer_gettask(&timer, "proc"), lc_timer_gettask(&timer, "write"));
	
	if (sev->log_print) {
		LC_LOG_NOTICE("");
	}

	return ret;
}


