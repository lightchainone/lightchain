


#ifndef  __LCCLIENT_OFDEVENT_H_
#define  __LCCLIENT_OFDEVENT_H_

#include "LcSockEvent.h"
#include "lcclient_nshead.h"

namespace lc
{
    
    class LcClientOFDNsheadEvent : plclic LcClientNsheadEvent
    {
    plclic:
        
        LcClientOFDNsheadEvent():LcClientNsheadEvent() {_ftag = (void *)&_talk;}

        
        virtual ~LcClientOFDNsheadEvent(){}

        
        virtual void write_done_callback();

        
        virtual void session_begin();

        
        virtual int session_done();

        
        virtual int activating_session();

    };

}















#endif  


