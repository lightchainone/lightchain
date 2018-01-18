

#ifndef  __LC_IEVENT_H_
#define  __LC_IEVENT_H_

#include <new>

#include "lcinlog.h"
#include "lcutils.h"
#include "Lsc/pool.h"

namespace lc
{
	class IReactor;
	class IEvent;
	

	
	class IEvent
	{
		plclic:
			enum {
				INIT,
				READY,
				INPOOL,
				DOING,
				DONE,
				
			};

			enum {				
				TIMEOUT = 1UL,	
				IOREADABLE = 1UL<<1UL,	
				IOWRITEABLE = 1UL<<2UL,	
				SIGNAL = 1UL<<3UL,	
				CLOSESOCK = 1UL<<4UL,	
				ERROR = 1UL<<5UL,	
				CPUEVENT = 1UL<<6UL,	
				CANCELED = 1UL<<7UL,	
				
			};

			
			typedef void (*cb_t)(IEvent *, void *);
		plclic:
			
			virtual int priority() = 0;
			
			virtual void setPriority(int) = 0;

			
			virtual int handle() = 0;
			
			virtual void setHandle(int) = 0;

			
			virtual IReactor *reactor() = 0;
			
			virtual void setReactor(IReactor *) = 0;

			
			virtual void callback() = 0;
			
			virtual void setCallback(cb_t cb, void *p) = 0;

			
			virtual int getCallback(cb_t *pcb, void ** pprm)=0;
			
			virtual timeval * timeout() = 0;
			
			virtual void setTimeout_tv(const timeval *) = 0;
			
			virtual void setTimeout(int msec);
			
			virtual int clear() = 0;	

			
			virtual int result() = 0;

			
			virtual void setResult(int) = 0;

			
			virtual int type() = 0;
			
			virtual void setType(int) = 0;

			
			
			virtual int status() = 0;
			
			virtual void setStatus(int) = 0;

			
			virtual int addRef() = 0;
			
			virtual int delRef() = 0;
			
			virtual int getRefCnt() = 0;
			
			virtual bool release() = 0;

			
			virtual void *dev() = 0;
			
			virtual void setDev(void *) = 0;

			
			virtual Lsc::mempool * pool() { return NULL; }

			
			virtual IEvent * next() = 0;	
			
			virtual void setNext(IEvent *) = 0;
			
			virtual IEvent * previous() = 0;	
			
			virtual void setPrevious(IEvent *) = 0;

			
			virtual int setEventBuffer(int size) = 0;
			
			virtual void *getEventBuffer() = 0;
			
			virtual int getEventBufferSize() = 0;

			
			virtual int isDevided() = 0;

			
			virtual void setDevided(int div) = 0;

			IEvent() {}

			virtual ~IEvent() {}

			virtual bool isError();
		protected:
			
			static void default_callback(IEvent *ev, void *) {
				if (!ev->isError()) {
					LC_IN_DEBUG("using default callback");
				}
			}
	};


	
	template <typename T>
	class SmartEvent
	{
		plclic:
			
			SmartEvent() {
				try {
					_ev = new T;
				}catch (...) {
					_ev = NULL;
				}
			}
			
			explicit SmartEvent(Lsc::mempool *p) {
				_ev = static_cast<T *>(p->malloc(sizeof(T)));
				if (_ev != NULL) {
					::new(_ev) T(p);
				}
			}
			
			explicit SmartEvent(T *t) : _ev(t) {}

			
			~SmartEvent() {
				if (_ev) {
					_ev->release();
				}
			}

			
			void destroyEvent() {
				if (_ev) {
					Lsc::mempool *p = _ev->pool();
					if (p != 0) {
						_ev->~T();
						p->free(_ev, sizeof(T));
					} else {
						delete _ev;
					}
					_ev = 0;
				}
			}

			
			inline IEvent *ev() { return _ev; }

			
			inline SmartEvent & operator = (SmartEvent &ref)
			{
				if (_ev == ref._ev) { return *this; }
				if (_ev) {
					_ev->release();
				}
				ref._ev->addRef();
				_ev = ref._ev;
				return *this;
			}

			inline T & operator * () { return _ev; }
			inline T * operator -> () { return _ev; }
			inline T * operator & () { return _ev; }

		private:
			T *_ev;
	};
};
#endif  


