


#ifndef  __LCEVENT_H_
#define  __LCEVENT_H_

#include "ievent.h"
#include "ireactor.h"
#include "lcatomic.h"
#include "ul_net.h"

namespace lc
{
	class EventBase : plclic IEvent
	{
		plclic:
			
			virtual void set(int hd, int evs, cb_t cb, void *p);

			
			virtual int priority() { return _pri; }


			virtual void setPriority(int ptype) { _pri = ptype; }

			
			virtual int handle() { return _handle; }

			
			virtual void setHandle(int hd) { _handle = hd; }

			
			virtual IReactor *reactor() { return _reactor; }

			
			virtual void setReactor(IReactor *r) { _reactor = r; }

			
			virtual void callback() { _cb(this, _cbp); }

			
			virtual void setCallback(cb_t cb, void *p);

			virtual int getCallback(cb_t *pcb, void ** pprm);

			
			virtual timeval * timeout();

			
			virtual void setTimeout_tv(const timeval *tv);

			
			virtual int result() { return _events; }
			
			virtual void setResult(int ev) { _events = ev; }
			
			virtual int type() { return _type; }
			
			virtual void setType(int t) { _type = t; }
			
			virtual int getRefCnt() { return _cnt.get(); }

			
			virtual int addRef() {  return _cnt.incrementAndGet(); }
			
			virtual int delRef() { return _cnt.decrementAndGet(); }
			
			virtual bool release();

			
			virtual int status () { return _status;}
			
			virtual void setStatus(int s) { _status = s; }
			
			virtual void * dev() { return _dev; }
			
			virtual void setDev(void *d) { _dev = d; }
			
			virtual Lsc::mempool *pool() { return _pool; }
			
			virtual int clear() { return 0; }
			
			virtual IEvent * next() { return _next; }
			
			virtual void setNext(IEvent *ev) { _next = ev; }
			
			virtual IEvent *previous() { return _pre; }
			
			virtual void setPrevious(IEvent *ev) { _pre = ev; }

			
			virtual int setEventBuffer(int size);
			
			virtual void *getEventBuffer();
			
			virtual int getEventBufferSize();

			
			virtual int isDevided();

			
			virtual void setDevided(int div);

			EventBase(Lsc::mempool *p = NULL);
			virtual ~EventBase();

		private:
			int _pri;
			int _handle;
			int _type;
			int _events;
			int _status;
			AtomicInt _cnt;

			bool _btv;
			IReactor *_reactor;
			timeval _tv;

			cb_t _cb;
			void * _cbp;
			void * _dev;
			Lsc::mempool *_pool;

			IEvent *_pre;
			IEvent *_next;

			void *_evbuf;
			int _evbufsiz;

			int _devided;
	};

	class SockEventBase : plclic EventBase
	{
		enum {
			BASE,
			ACCEPT,
			READ,
			WRITE,
			TCPCONNECT,
		};

		plclic:
			SockEventBase();
			~SockEventBase();
			
			void setType(int t);
			
			int accept(int fd);
			
			int read(int fd, void *buf, size_t count);
			
			int write(int fd, const void *buf, size_t count);
			
			int tcpConnect(const char *host, int port);
			int tcpConnectBind(const char *host, int port, const char *client_host, int client_port);

		plclic:
			
			virtual void *buffer() { return _buf; }
			
			virtual size_t needCount() { return _cnt; }
			
			virtual ssize_t retCount() { return _readcnt; }

			
			virtual int retVal() { return _ret; }

			
			virtual int sockSetTypes() { return _types; }
			
			
			virtual int clear();

			virtual void callback();

			
			void set_first_rwto(unsigned int rwto) { _first_rwto = rwto; }

			const struct sockaddr_in * get_sockaddr() {
				return &_addr;
			}

		protected:
			unsigned int _first_rwto; 	  

		private:

			virtual void accept_callback();
			virtual void rw_callback();
			virtual void tcpconnect_callback();
		
        protected:
			int _types;
			int _ioflags;
			int _ret;
			bool _needrcy;

			union {
				struct {	
					struct sockaddr_in _addr;
					socklen_t _len;
				};
				struct {
					void *_buf;
					size_t _cnt;
					ssize_t _readcnt;
				};
			};


	};

	typedef EventBase Event;
	typedef SockEventBase SockEvent;
	typedef SmartEvent<EventBase> EventPtr;
	typedef SmartEvent<SockEventBase> SockEventPtr;
};

#endif  


