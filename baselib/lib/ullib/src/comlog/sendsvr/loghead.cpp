#include "loghead.h"
#include "xdebug.h"
#include "ul_net.h"
#include "ul_log.h"
namespace comspace{

int
loghead_read(int sock, loghead_t *head, void *req, size_t req_size,
	    void *buf, size_t buf_size, int timeout,
	    unsigned flags);

int
loghead_write(int sock, loghead_t *head, void *req, size_t req_size,
	     void *buf, int timeout, unsigned flags);

int
loghead_read(int sock, void *buf, size_t buf_size, int timeout, unsigned flags)
{
    if (buf_size < sizeof(loghead_t))
	return LOGHEAD_RET_EPARAM;

    return loghead_read(sock, (loghead_t*)buf,
	    	       (void*)((loghead_t*)buf + 1), 0,
	               (void*)((loghead_t*)buf + 1), buf_size - sizeof(loghead_t),
		       timeout, flags);
}


int
loghead_read(int sock, loghead_t *head, void *req, size_t req_size,
	    void *buf, size_t buf_size, int timeout,
	    unsigned flags)
{
    int rlen;

    if (flags & LOGHEAD_CHECK_PARAM) {
	/* 奇怪的检查们 */
	if (sock < 0 || !head)
	    return LOGHEAD_RET_EPARAM;
    }

    /* 读入 LOGHEAD 头 */
    rlen = ul_sreado_ms_ex(sock, head, sizeof(loghead_t), timeout);
    if (rlen <= 0)
	goto loghead_read_fail;
    else if (rlen != sizeof(loghead_t)) {
	_com_error(
		"<%u> loghead_read head fail: ret %d want %u ERR[%m]",
		head->log_id, rlen, (unsigned int)req_size);
	return LOGHEAD_RET_READHEAD;
    }

    /* 检查 Magic Number */
    if (flags & LOGHEAD_CHECK_MAGICNUM) {
	if (head->magic_num != LOGHEAD_MAGICNUM) {
	    _com_error(
		    "<%u> loghead_read magic num mismatch: ret %x want %x",
		    head->log_id, head->magic_num, LOGHEAD_MAGICNUM);
	    return LOGHEAD_RET_EMAGICNUM;
	}
    }

    /* 对 body 的长度进行检查, 过长和过短都不符合要求 */
    if (head->body_len < req_size || head->body_len - req_size > buf_size) {
	_com_error(
		"<%u> loghead_read body_len error: req_size=%u buf_size=%u body_len=%u",
		head->log_id, (unsigned int)req_size, (unsigned int)buf_size, head->body_len);
	return LOGHEAD_RET_EBODYLEN;
    }

    /* Read Request */
    if (req_size > 0) {
	rlen = ul_sreado_ms_ex(sock, req, req_size, timeout);
	if (rlen <= 0)
	    goto loghead_read_fail;
	else if (rlen != (int)req_size) {
	    _com_error(
		    "<%u> loghead_read fail: ret %d want %u ERR[%m]",
		    head->log_id, rlen, (unsigned int)req_size);
	    return LOGHEAD_RET_READ;
	}
    }

    /* Read Detail */
    if (head->body_len - req_size > 0) {
	rlen = ul_sreado_ms_ex(sock, buf, head->body_len - req_size, timeout);
	if (rlen <= 0)
	    goto loghead_read_fail;
	else if (rlen != (int)(head->body_len - req_size)) {
	    _com_error(
		    "<%u> loghead_read fail: ret %d want %d ERR[%m]",
		    head->log_id, rlen, int(head->body_len - req_size));
	    return LOGHEAD_RET_READ;
	}
    }

    return LOGHEAD_RET_SUCCESS;

loghead_read_fail:
    if (rlen == 0)
	return LOGHEAD_RET_PEARCLOSE;

    _com_error("<%u> loghead_read fail: ret=%d ERR[%m]",head->log_id, rlen);
    if (rlen == -1 && errno == ETIMEDOUT)
	return LOGHEAD_RET_ETIMEDOUT;
    else
	return LOGHEAD_RET_READ;
}


int
loghead_write(int sock, loghead_t *head, int timeout,
	     unsigned flags)
{
    return loghead_write(sock, head,
	    		(void*)(head + 1), 0,
	                (void*)(head + 1), timeout, flags);
}


int
loghead_write(int sock, loghead_t *head, void *req, size_t req_size,
	     void *buf, int timeout, unsigned flags)
{
    int wlen;

    if (flags & LOGHEAD_CHECK_PARAM) {
	/* 奇怪的检查们 */
	if (sock < 0 || !head)
	    return LOGHEAD_RET_EPARAM;
    }

    /* 校验 size */
    if (head->body_len < req_size) {
	_com_error(
		"<%u> loghead_write body_len error: req_size=%u body_len=%u",
		head->log_id, (unsigned int)req_size, head->body_len);
	return LOGHEAD_RET_EBODYLEN;
    }

    if (flags & LOGHEAD_CHECK_MAGICNUM)
	head->magic_num = LOGHEAD_MAGICNUM;

    wlen = ul_swriteo_ms_ex(sock, head, sizeof(loghead_t), timeout);
    if (wlen <= 0)
	goto loghead_write_fail;
    else if (wlen != sizeof(loghead_t)) {
	_com_error(
		"<%u> loghead_write head fail: ret %d want %u ERR[%m]",
		head->log_id, wlen, (unsigned int)sizeof(loghead_t));
	return LOGHEAD_RET_WRITEHEAD;
    }

    if (req_size > 0) {
	wlen = ul_swriteo_ms_ex(sock, req, req_size, timeout);
	if (wlen <= 0)
	    goto loghead_write_fail;
	else if (wlen != (int)req_size) {
	    _com_error(
		    "<%u> loghead_write fail: ret %d want %u ERR[%m]",
		    head->log_id, wlen, (unsigned int)req_size);
	    return LOGHEAD_RET_WRITE;
	}
    }

    if (head->body_len - req_size > 0) {
	wlen = ul_swriteo_ms_ex(sock, buf, head->body_len - req_size, timeout);
	if (wlen <= 0)
	    goto loghead_write_fail;
	else if (wlen != (int)(head->body_len - req_size)) {
	    _com_error(
		    "<%u> loghead_write fail: ret %d want %u ERR[%m]",
		    head->log_id, wlen, (unsigned int)req_size);
	    return LOGHEAD_RET_WRITE;
	}
    }

    return LOGHEAD_RET_SUCCESS;

loghead_write_fail:
    _com_error(
	    "<%u> loghead_write fail: ret=%d ERR[%m]",
	    head->log_id, wlen);
    if (wlen == 0)
	return LOGHEAD_RET_PEARCLOSE;
    else if (wlen == -1 && errno == ETIMEDOUT)
	return LOGHEAD_RET_ETIMEDOUT;
    else
	return LOGHEAD_RET_WRITE;
}

}













/* vim: set ts=4 sw=4 sts=4 tw=100 */
