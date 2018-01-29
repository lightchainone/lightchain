
#include "lcclient_http.h"
#include "lcclient_connectserver.h"
#include "lcclient_connectmanager.h"
#include "lcclient_struct.h"
#include "LcSockEvent.h"
#include "lcclient_manager.h"
#include "lcclient_strategy.h"

#include <LcSockEvent.h>

namespace lc {
	
	void LcClientEvent :: postConn(common_talkwith_t *currTalk, LcClientServer *server,
					   			   lc::LcEvent *lcEvent)
	{
        LcClientManager *t_mgr = currTalk->mgr;
        if (t_mgr) {
            t_mgr->add_monitor_info_ipport("CONNECT_FAIL_NUMBER", server->getIP(currTalk->ipnum), server->getPort(), server->getManager()->getServiceName());
        }

		if (currTalk->ipnum+1 >= server->getIPCount()) {
			LC_LOG_WARNING("connect server[%s:%d] all ip error, logid:%d", server->getIP(), server->getPort(), lcEvent->get_logid());
			currTalk->conn->setErrno(-1);
			server->setHealthy(false);  
			server->freeConnectionAsync(currTalk->conn);
			currTalk->success = NSHEAD_TCP_CONNECT_ERROR;
			lcEvent->set_sock_status(STATUS_CONNERROR);
			lcEvent->setHandle(-1);
			LcClientConnectManager *mgr = server->getManager();
            int ret = mgr->fetchConnectionRetry(currTalk, *(currTalk->conf), lcEvent,
                                    dynamic_cast<NetReactor*>(lcEvent->reactor()));
			if (0 != ret) {
				struct timeval tm;
				gettimeofday(&tm, NULL);
                lc_socket_t *sock_data = lcEvent->get_sock_data();
				sock_data->timer.connect = tvTv2Ms(tvSelfSlc(tm, sock_data->timer.start));
				sock_data->timer.total = tvTv2Ms(tvSelfSlc(tm, sock_data->timer.start));
				currTalk->talkwithcallback(currTalk);
			}
			return;
		} else {
			currTalk->ipnum ++;
			while (currTalk->ipnum < server->getIPCount()) {
				if (lcEvent->tcpConnect(server->getIP(currTalk->ipnum), 
										server->getPort()) != 0) {
					LC_LOG_WARNING("tcpConnect error ip:%s, port:%d, logid:%d [%m]", 
								server->getIP(currTalk->ipnum), server->getPort(), lcEvent->get_logid());
					currTalk->ipnum ++;
					continue;
				} else {
					break;
				}
			}
			if (currTalk->ipnum >= server->getIPCount()) {
				LC_LOG_WARNING("all tcpConnect error ip:%s, port%d, logid:%d", server->getIP(0),
								server->getPort(), lcEvent->get_logid()); 
				currTalk->conn->setErrno(-1);
				server->setHealthy(false);  
				server->freeConnectionAsync(currTalk->conn);
				currTalk->success = NSHEAD_TCP_CONNECT_ERROR;
				lcEvent->set_sock_status(STATUS_CONNERROR);
				lcEvent->setHandle(-1);
				LcClientConnectManager *mgr = server->getManager();
				int ret = mgr->fetchConnectionRetry(currTalk, *(currTalk->conf), lcEvent,
								dynamic_cast<NetReactor*>(lcEvent->reactor()));
				if (0 != ret) {
					struct timeval tm;
					gettimeofday(&tm, NULL);
					lc_socket_t *sock_data = lcEvent->get_sock_data();
					sock_data->timer.connect = tvTv2Ms(tvSelfSlc(tm, sock_data->timer.start));
					sock_data->timer.total = tvTv2Ms(tvSelfSlc(tm, sock_data->timer.start));
					currTalk->talkwithcallback(currTalk);
				}
				return;
			}
			LC_LOG_DEBUG("tcpConnect success ip:%s, port:%d", 
						 server->getIP(currTalk->ipnum), server->getPort());
			lcEvent->setTimeout(server->getTimeout());
			if (lcEvent->activating_session() < 0) {
				currTalk->conn->setErrno(-1);
				server->freeConnectionAsync(currTalk->conn);
				currTalk->success = NSHEAD_TCP_CONNECT_ERROR;
				lcEvent->set_sock_status(STATUS_CONNERROR);
				lcEvent->setHandle(-1);
				LcClientConnectManager *mgr = server->getManager();
            	int ret = mgr->fetchConnectionRetry(currTalk, *currTalk->conf, lcEvent,
                                    dynamic_cast<NetReactor*>(lcEvent->reactor()));
				if (0 != ret) {
					struct timeval tm;
					gettimeofday(&tm, NULL);
					lc_socket_t *sock_data = lcEvent->get_sock_data();
					sock_data->timer.connect = tvTv2Ms(tvSelfSlc(tm, sock_data->timer.start));
					sock_data->timer.total = tvTv2Ms(tvSelfSlc(tm, sock_data->timer.start));
					currTalk->talkwithcallback(currTalk);
				}
				return;
			}
			return;
		}
		return;
	}
	
	void LcClientEvent :: conn_callback(IEvent *e, void *p)
	{
		if (NULL == e || NULL == p) {
			return ;
		}

		common_talkwith_t *talkmsg = (common_talkwith_t *)(p);
		if (NULL == talkmsg) {
			return;
		}
		LcEvent *ev = dynamic_cast<LcEvent*>(e);
		if (NULL == ev) {
			return;
		}
		ev->restore_callback();
		
		LcClientServer *server = talkmsg->server;
		if (NULL == server) {
			LC_LOG_WARNING("server is NULL");
			talkmsg->success = lc::NSHEAD_UNKNOW_ERROR;
			ev->set_sock_status(LcEvent::STATUS_ERROR);
			talkmsg->talkwithcallback(talkmsg);
			return;
		}

		if (ev->isError()) {
			LC_LOG_WARNING("connect error happened ip:%d, err:%d, logid:%d %m", talkmsg->ipnum, 
							ev->result(), ev->get_logid());
            LcClientConnectManager *mgr = server->getManager();
            LcClientStrategy *stt = mgr->getStrategy();
            stt->setServerArgAfterConn(server, -2);
			postConn(talkmsg, server, ev);
			return;
		}

        LcClientConnectManager *mgr = server->getManager();
        LcClientStrategy *stt = mgr->getStrategy();
        stt->setServerArgAfterConn(server, 0);
		int currFD = ev->handle();

        
        LcClientManager *clt_mgr = talkmsg->mgr;
        if (clt_mgr) {
            clt_mgr->add_monitor_info_ipport("QUERY_CONNECT_NUMBER", currFD, mgr->getServiceName());
        }
		struct timeval tm;
		gettimeofday(&tm, NULL);
		unsigned int connect = tvTv2Ms(tvSelfSlc(tm, ev->get_sock_data()->timer.start));
        if (clt_mgr) {
            clt_mgr->add_monitor_info_ipport_ex("CONNECT_TIME", currFD, mgr->getServiceName(), connect);
        }

		if (currFD >= 0) {
			int on = 1;
			if (setsockopt(currFD, IPPROTO_TCP,TCP_NODELAY, &on, sizeof(on)) < 0) {
				LC_LOG_WARNING("setsockopt nodelay error, logid:%d [%m]", ev->get_logid());
				talkmsg->conn->setHandle(currFD);
				postConn(talkmsg, server, ev);
				return;
			}
			if (server->isLinger()) {
				struct linger li;
				memset(&li, 0, sizeof(li));
				li.l_onoff = 1;
				li.l_linger = 0;
				if (setsockopt(currFD, SOL_SOCKET, SO_LINGER, (const char*)&li, sizeof(li)) < 0 ) {
					LC_LOG_WARNING("setsockopt so_linger error, logid:%d [%m]", ev->get_logid());
					talkmsg->conn->setHandle(currFD);
					postConn(talkmsg, server, ev);
					return;
				}
			}	
        } else {
			LC_LOG_WARNING("get fd error %d, logid:%d", currFD, ev->get_logid());
			postConn(talkmsg, server, ev);
			return;
		}
		talkmsg->conn->setHandle(currFD);
		talkmsg->conn->setCurrIP(talkmsg->ipnum);
		ev->setHandle(currFD);
		ev->setTimeout(server->getWriteTimeout());
		ev->set_sock_status(LcEvent::STATUS_READ);
        
        if (talkmsg->wait == 1 || talkmsg->wait == 2) {
            LcClientInDuSepEvent *in_event = talkmsg->conn->getInEvent();
            in_event->setWriteEvent(dynamic_cast<LcClientDuSepNsheadEvent*>(ev));
            in_event->session_begin();
        } else {
		    ev->session_begin();
        }
		return;
	}

    int LcClientEvent :: freeConnWithoutCallback(common_talkwith_t *currTalk, int tag)
    {
        if (NULL == currTalk) {
            return -1;
        }
        LcEvent *lcevent = dynamic_cast<LcEvent*>(currTalk->event);
		if (NULL == currTalk->conn) {
			currTalk->success = lc::NSHEAD_UNKNOW_ERROR;
			lcevent->set_sock_status(LcEvent::STATUS_ERROR);
			currTalk->talkwithcallback(currTalk);
			return -1;
		}

		if (0 != lcevent->isError()) {
			LC_LOG_DEBUG("talk error[%d]", lcevent->result());
			currTalk->success = lcevent->result();
			currTalk->conn->setErrno(currTalk->success);
		} else {
			currTalk->success = 0;
		}

		if (NULL == currTalk->conn) {
			LC_LOG_WARNING("conn is NULL, logid:%d", lcevent->get_logid());
		} else {
			int handleError = 0;
			socklen_t len = 0;
			len = sizeof(handleError);
			int *curr_handle = (int *)currTalk->conn->getHandle();
			int curr_fd = *curr_handle;
			int val = getsockopt(curr_fd, SOL_SOCKET, SO_ERROR, &handleError, &len);
			if (-1 == val) {
				LC_LOG_DEBUG("nshead_talk : get sockFD[%d] error[%d]", handleError,
							handleError);
				currTalk->conn->setErrno(lc::LC_CONN_FD_ERROR);
			}
		}

        nshead_talkwith_t temp_talk;
        lc_socket_t * sock_data = lcevent->get_sock_data();
		
        LcClientServer *server = currTalk->server;
		if (NULL == server) {
			LC_LOG_WARNING("something wrong of talk for Server or manager, logid:%d", lcevent->get_logid());
			goto End;
		}
		if (NULL != currTalk->conn) {
			server->freeConnectionAsync(currTalk->conn, tag);
            if (tag == 0) {
			    currTalk->conn = NULL;
            }
		}

        temp_talk.returninfo.realreadtime = sock_data->timer.read;
        temp_talk.returninfo.realwritetime = sock_data->timer.write;
        
        if (NULL != currTalk->pool) {
            currTalk->pool->setServerArg(server, &temp_talk);
        }

		return 0;

	End:
		
		
		currTalk->success = lc::NSHEAD_UNKNOW_ERROR;
		lcevent->set_sock_status(LcEvent::STATUS_ERROR);
		return 0;
    }

	int LcClientEvent :: freeConn(common_talkwith_t *currTalk, int tag)
	{
		
		
		
		
        if (freeConnWithoutCallback(currTalk, tag) == 0) {
		    currTalk->talkwithcallback(currTalk);
            return 0;
        }
		return -1;
    }

	LcClientHttpEvent :: LcClientHttpEvent()
	{
		_ftag = (void *)&_talk;
        _callback_backup = IEvent::default_callback;
        _callback_arg = NULL;
        _backtag = 0;
        _fclass_type = CLASS_HTTP;
	}

	int LcClientHttpEvent :: activating_session()
	{
        getCallback(&_callback_backup, &_callback_arg);
        _backtag = 1;
		setResult(0);
        set_sock_status(0);
        backup_callback();
        setCallback(LcClientEvent::conn_callback, &_talk);
		
        return repost();
	}

	void LcClientHttpEvent :: session_begin()
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
		
		
		if (0 != awrite()) {
			set_sock_status(STATUS_ERROR);
			LC_LOG_WARNING("awrite error, maybe fd error");
			session_done();
		}
	}

	int LcClientHttpEvent :: session_done()
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

	void LcClientHttpEvent :: read_done_callback()
	{
		
		session_done();
	}

	void LcClientHttpEvent :: write_done_callback()
	{
		if (isError()) {
			LC_LOG_WARNING("write error");
			session_done();
			return;
		}
		setTimeout(_talk.readtimeout);
		
		if ( 0 > http_package_read()) {
			LC_LOG_WARNING("event post error,maybe fd error");
			session_done();
		}
	}

	void LcClientHttpEvent :: http_read_header_done()
	{
		if (0 > http_section_read(get_http_bodylen())) {
			LC_LOG_WARNING("%s http_section_read error, maybe fd error", __func__);
			session_done();
			return;
		}
    	if (http_package_finished()) {
        	http_read_section_done();
    	}
	}

	void LcClientHttpEvent :: http_read_section_done()
	{
		if (isError()) {
			return;
		}
		session_done();
		return;
	}

	void LcClientHttpEvent :: event_error_callback() {
        int fd = this->handle();
        struct sockaddr_in addr;
        socklen_t addr_len = sizeof(addr);
        getpeername(fd, (struct sockaddr*)&addr, &addr_len);
        LC_IN_WARNING("[ServerIp:%s, Port:%d] talkwith server error[0x%X]",
                inet_ntoa(addr.sin_addr), ntohs(addr.sin_port), sock_data.status);

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

	bool LcClientHttpEvent :: isError()
	{
		if (SockEventBase :: isError()) {
			return true;
		}
		
		if (get_sock_status() & 0xfeffff00) {
			return true;
		}

		return false;
	}

	bsl::string LcClientHttpEvent :: status_to_string(unsigned int st)
	{
		bsl::string res = LcEvent::status_to_string(st);
		if (st & STATUS_CONNERROR) {
			res.append("|CONNECT_ERROR");
		}

		return res;
	}
}







