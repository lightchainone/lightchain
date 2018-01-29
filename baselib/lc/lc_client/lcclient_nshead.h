
#ifndef  __LCCLIENT_NSHEAD_H_
#define  __LCCLIENT_NSHEAD_H_

#include <lc/lc2.h>
#include <deque>

#include "lcclient_struct.h"
#include "lcclient_http.h"
#include "NsheadEvent.h"

namespace lc {
    
    typedef int (*lc_client_rpc_preprocess)(void *talk);
      
    
    typedef struct _server_talk_returninfo_t {
        int realreadtime;        
        int realwritetime;       
        int reserve;             
    }server_talk_returninfo_t;
    
    
    class nshead_talkwith_t : plclic lcclient_talkwith_t
    {
        plclic:
            nshead_t reqhead;         
            char *reqbuf;             
            u_int reqlen;             
            nshead_t reshead;         
            char *resbuf;             
            u_int maxreslen;          
            u_int reslen;             


            
            lc_client_callbackalloc alloccallback;        
            lc_client_callbackfree freecallback;          
            void *allocarg;                               
            
            void *serverArg;          

            
            lc_client_rpc_preprocess rpccallback1;

            
            server_talk_returninfo_t returninfo; 

            
            lcclient_nshead_atalk_t innerTalk;        
            
            
            nshead_talkwith_t(){
                type = LCCLIENT_NSHEAD;
                reqbuf = NULL;
                reqlen = 0;
                resbuf = NULL;
                maxreslen = 0;
                reslen = 0;
                talkwithcallback = NULL;
                callarg = NULL;
                alloccallback = NULL;
                freecallback = NULL;
                allocarg = NULL;
                rpccallback1 = NULL;
                success = 0;
                serverArg = NULL;

                returninfo.realreadtime = 0;
                returninfo.realwritetime = 0;

                innerTalk.talk = NULL;
                innerTalk.readtimeout = 0;
                innerTalk.writetimeout = 0;
                innerTalk.conn = NULL;
                innerTalk.server = NULL;
                innerTalk.pool = NULL;
                innerTalk.mgr = NULL;
                innerTalk.eventpool = NULL;
                innerTalk.ipnum = 0;
            }

            
            void clean() {
                talkwithcallback = NULL;
                callarg = NULL;
                alloccallback = NULL;
                freecallback = NULL;
                allocarg = NULL;
                freecallback = NULL;        
                rpccallback1 = NULL;
                success = 0;
                
                memset(defaultserverarg.range, 0, LCCLIENT_FILE_LEN);
                memset(defaultserverarg.version, 0, LCCLIENT_FILE_LEN);
                defaultserverarg.master = 1;
                defaultserverarg.key = -1;

                innerTalk.talk = NULL;
                innerTalk.readtimeout = 0;
                innerTalk.writetimeout = 0;
                innerTalk.conn = NULL;
                innerTalk.server = NULL;
                innerTalk.pool = NULL;
                innerTalk.mgr = NULL;
                innerTalk.eventpool = NULL;
                innerTalk.ipnum = 0;
            }
    };

    
    void set_lcnshead_atalk(lcclient_nshead_atalk_t *p, nshead_talkwith_t *talk,
                            lc::IEvent::cb_t cb, void *param);


    
    class LcClientNsheadEvent : plclic NsheadEvent
    {
    plclic:
        
        LcClientNsheadEvent();

        
        virtual ~LcClientNsheadEvent(){}

        
        virtual int activating_session();

        
        virtual void read_done_callback();

        
        virtual void write_done_callback();

        
        virtual void session_begin();

        
        virtual int session_done();

        
        virtual common_talkwith_t* getTalk(){return &_talk;}

        
        virtual void event_error_callback();

        
        virtual bool isError();

        
        virtual bsl::string status_to_string(unsigned int st);

    protected:
        common_talkwith_t _talk;          
        cb_t _callback_backup;        
        void *_callback_arg;          
        int _backtag;         
    };

    class LcClientInDuSepEvent;

    enum {
        COMMON_TALK = 0,          
        DUSEP_TALK_NOWAIT = 1,        
        DUSEP_TALK_WAIT = 2,          
    };

    
    class LcClientDuSepNsheadEvent : plclic LcClientNsheadEvent
    {
        plclic:
            
            LcClientDuSepNsheadEvent() : LcClientNsheadEvent(){}

            
            virtual ~LcClientDuSepNsheadEvent(){}
            
            
            virtual void session_begin();
 
            
            virtual int session_done();
    
            
            virtual void read_done_callback();

            
            virtual void write_done_callback();             

            
            virtual void event_error_callback();

            
            
            
            void setInEvent(LcClientInDuSepEvent *event) {_in_event = event;}
        protected:
            LcClientInDuSepEvent *_in_event;          
    };

    
    
    class LcClientInDuSepEvent : plclic LcClientNsheadEvent
    {
        plclic:
            
            LcClientInDuSepEvent():_read_cnt(0), _write_event(NULL){}

            
            virtual ~LcClientInDuSepEvent(){}

            
            
            virtual void session_begin();

            
            static void unified_rw_callback(IEvent *e, void *p, int tag);

            
            virtual int write_done();

            
            virtual void setWriteEvent(LcClientDuSepNsheadEvent * event) {
                event->addRef();
                _write_event = event;
            }

            
            virtual int read_done(LcClientDuSepNsheadEvent *out_event);

            
            virtual void clear_event();

            
            void decReadCnt() {_read_cnt.decrementAndGet();}
        protected:
            AtomicInt _read_cnt;          
            LcClientDuSepNsheadEvent * _write_event;          
            MLock _deque_lock;        
            std::deque<LcClientDuSepNsheadEvent*> _oevent_deque;          
    };

}










#endif  


