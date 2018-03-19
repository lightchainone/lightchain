

#ifndef EVENT_LOG_H_
#define EVENT_LOG_H_

#include <lc/lcevent.h>
#include "LcSockEvent.h"
#define LCEVENT_FATAL(event, fmt, arg...) \
do { \
	::lc::lc_socket_t * skd = event->get_sock_data();\
	unsigned char *ip = (unsigned char *)&skd->client_addr.sin_addr.s_addr;\
	LC_IN_FATAL("[logid:%d, event:%p, fd:%d, status:0x%X, maxsize:%u, srdbuf:%p,"\
		 "rdbuf:%p,rdlen:%u,rdused:%u, swtbuf:%p,wtbuf:%p,wtlen:%u,wtused:%u, "\
		 "rdto:%d,wtto:%d,cnto:%d,proto:%d, "\
		 "ip:%u.%u.%u.%u,port:%d, proctime:total:%u(ms),activate:%u+read:%u+proc:%u+write:%u]"fmt, \
		 event->get_logid(), event,event->handle(), skd->status, skd->max_buffer_size, \
		 skd->small_readbuf,skd->read_buf, skd->read_buf_len, skd->read_buf_used,\
		 skd->small_writebuf,skd->write_buf,skd->write_buf_len,skd->write_buf_used,\
		 skd->read_timeout,skd->write_timeout,skd->connect_timeout,skd->process_timeout,\
		 ip[0], ip[1], ip[2], ip[3], ntohs(skd->client_addr.sin_port),\
		 skd->timer.total, skd->timer.connect, skd->timer.read, skd->timer.proc, skd->timer.write,\
		 ##arg);\
} while (0)

#define LCEVENT_WARNING(event, fmt, arg...) \
do { \
	::lc::lc_socket_t * skd = event->get_sock_data();\
	unsigned char *ip = (unsigned char *)&skd->client_addr.sin_addr.s_addr;\
	LC_IN_WARNING("[logid:%d, event:%p, fd:%d, status:0x%X, maxsize:%u, srdbuf:%p,"\
		 "rdbuf:%p,rdlen:%u,rdused:%u, swtbuf:%p,wtbuf:%p,wtlen:%u,wtused:%u, "\
		 "rdto:%d,wtto:%d,cnto:%d,proto:%d, "\
		 "ip:%u.%u.%u.%u, port:%d, proctime:total:%u(ms),activate:%u+read:%u+proc:%u+write:%u]"fmt, \
		 event->get_logid(), event,event->handle(), skd->status, skd->max_buffer_size, \
		 skd->small_readbuf,skd->read_buf, skd->read_buf_len, skd->read_buf_used,\
		 skd->small_writebuf,skd->write_buf,skd->write_buf_len,skd->write_buf_used,\
		 skd->read_timeout,skd->write_timeout,skd->connect_timeout,skd->process_timeout,\
		 ip[0], ip[1], ip[2], ip[3], ntohs(skd->client_addr.sin_port),\
		 skd->timer.total, skd->timer.connect, skd->timer.read, skd->timer.proc, skd->timer.write,\
		 ##arg);\
} while (0)


#define LCEVENT_TRACE(event, fmt, arg...) \
do { \
	::lc::lc_socket_t * skd = event->get_sock_data();\
	unsigned char *ip = (unsigned char *)&skd->client_addr.sin_addr.s_addr;\
	LC_IN_TRACE("[logid:%d, event:%p, fd:%d, status:0x%X, maxsize:%u, srdbuf:%p,"\
		 "rdbuf:%p,rdlen:%u,rdused:%u, swtbuf:%p,wtbuf:%p,wtlen:%u,wtused:%u, "\
		 "rdto:%d,wtto:%d,cnto:%d,proto:%d, "\
		 "ip:%u.%u.%u.%u, port:%d, proctime:total:%u(ms),activate:%u+read:%u+proc:%u+write:%u]"fmt, \
		 event->get_logid(), event,event->handle(), skd->status, skd->max_buffer_size, \
		 skd->small_readbuf,skd->read_buf, skd->read_buf_len, skd->read_buf_used,\
		 skd->small_writebuf,skd->write_buf,skd->write_buf_len,skd->write_buf_used,\
		 skd->read_timeout,skd->write_timeout,skd->connect_timeout,skd->process_timeout,\
		 ip[0], ip[1], ip[2], ip[3], ntohs(skd->client_addr.sin_port),\
		 skd->timer.total, skd->timer.connect, skd->timer.read, skd->timer.proc, skd->timer.write,\
		 ##arg);\
} while (0)

#define LCEVENT_DEBUG(event, fmt, arg...) \
do { \
	::lc::lc_socket_t * skd = event->get_sock_data();\
	unsigned char *ip = (unsigned char *)&skd->client_addr.sin_addr.s_addr;\
	LC_IN_DEBUG("[logid:%d, event:%p, fd:%d, status:0x%X, maxsize:%u, srdbuf:%p,"\
		 "rdbuf:%p,rdlen:%u,rdused:%u, swtbuf:%p,wtbuf:%p,wtlen:%u,wtused:%u, "\
		 "rdto:%d,wtto:%d,cnto:%d,proto:%d, "\
		 "ip:%u.%u.%u.%u, port:%d, proctime:total:%u(ms),activate:%u+read:%u+proc:%u+write:%u]"fmt, \
		 event->get_logid(), event,event->handle(), skd->status, skd->max_buffer_size, \
		 skd->small_readbuf,skd->read_buf, skd->read_buf_len, skd->read_buf_used,\
		 skd->small_writebuf,skd->write_buf,skd->write_buf_len,skd->write_buf_used,\
		 skd->read_timeout,skd->write_timeout,skd->connect_timeout,skd->process_timeout,\
		 ip[0], ip[1], ip[2], ip[3], ntohs(skd->client_addr.sin_port),\
		 skd->timer.total, skd->timer.connect, skd->timer.read, skd->timer.proc, skd->timer.write,\
		 ##arg);\
} while (0)


#endif 
